/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_wifi_console.c

  Summary:
    Module for Microchip TCP/IP Stack
    -Provides access to MRF24W WiFi controller
    -Reference: MRF24W Data sheet, IEEE 802.11 Standard

  Description:
    MRF24W Driver WiFi Console

  ******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) <2014> released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
//DOM-IGNORE-END

/*******************************************************************************
 *                                INCLUDES
 *******************************************************************************/
#include <stdarg.h>
#include <ctype.h>

#include "tcpip/tcpip.h"

#include "drv_wifi_console.h"

#if defined ( WF_CONSOLE )

/*******************************************************************************
 *                                  CONSTANTS
 *******************************************************************************/

// This define determines the number of command lines that are stored in the history
// buffer.  Setting this define to 0 will remove the command line history feature by
// eliminating history code and buffers, thereby saving memory.
#define  kWFNumHistoryEntries   (1)

// states for WFConsoleProcess() state machine
enum
{
    kSTWaitForChar,
    kSTWaitForEscSeqSecondChar,
    kSTWaitForEscSeqThirdChar
};

// ASCII control keys

#define kWFTab             ('\t')
#define kWFNewLine         ('\n')
#define kWFBackspace       ('\b')
#define kWFCR              ('\r')
#define kWFSpace           (' ')
#define kWFEscape          (0x1b)
#define kWFDelete          (0x7f)   /* not supported on hyperterminal unless in VT100 mode */
#define kWFEnter           (0x0d)
#define kWFCtrl_B          (0x02)
#define kWFCtrl_C          (0x03)
#define kWFCtrl_D          (0x04)
#define kWFCtrl_E          (0x05)   /* used to turns off console echo */
#define kWFCtrl_H          (0x08)
#define kWFCtrl_I          (0x09)
#define kWFCtrl_O          (0x0f)
#define kWFCtrl_X          (0x18)

// total number of commands to and from Host Bridge
#define kWFNumCmdsToHostBridge    (sizeof(MsgsToHB) / sizeof(MSGS))
#define kWFNumCmdsFromHostBridge  (sizeof(MsgsFromHB) / sizeof(MSGS))

#define  kWFNextHistory          (0)
#define  kWFPrevHistory          (1)

#define kWFMaxInputEscapeSequence  (5)

/*******************************************************************************
 *                               MACROS
 *******************************************************************************/
#define SET_RX_STATE(state)     g_ConsoleContext.rxState = state
#define GET_RX_STATE()          g_ConsoleContext.rxState
#define SET_CURSOR(index)       g_ConsoleContext.cursorIndex = index
#define GET_CURSOR()            g_ConsoleContext.cursorIndex

#define GET_LEN_RX_CMD_STRING()        ( strlen( (char *) g_ConsoleContext.rxBuf))

/*******************************************************************************
 *                          TYPEDEFS
 *******************************************************************************/

#if (kWFNumHistoryEntries > 0)
typedef struct history_struct
{
    int8_t   buf[kWFNumHistoryEntries][kConsoleMaxMsgSize + 1];  // N lines of history
    uint8_t   index;
    bool seeded;
    int8_t   recallIndex;
} tWFHistory;
#endif

/*******************************************************************************
 *                              LOCAL GLOBALS
 *******************************************************************************/
#if (kWFNumHistoryEntries > 0)
static tWFHistory history;
#endif

static ROM int8_t gCmdLinePrompt[] = "> ";
static  uint8_t gCmdLinePromptLength = 2;
static  int8_t gTmpCmdLine[kConsoleMaxMsgSize];

/*******************************************************************************
 *                  COSNTANT LOCAL GLOBALS
 *******************************************************************************/

// VT100 escapse sequences that are output to the terminal
static ROM int8_t cursorLeftEscapeSequence[]           = {kWFEscape, '[', '1', 'D', 0x00};
static ROM int8_t cursorRightEscapeSequence[]          = {kWFEscape, '[', '1', 'C', 0x00};
static ROM int8_t cursorHomeEscapeSequence[]           = {kWFEscape, '[', 'f', 0x00};
static ROM int8_t eraseToEndOfLineEscapeSequence[]     = {kWFEscape, '[', 'K', 0x00};
static ROM int8_t saveCursorPositionEscapeSequence[]   = {kWFEscape, '7', 0x00};
static ROM int8_t restoreCursorPositionSequence[]      = {kWFEscape, '8', 0x00};
static ROM int8_t eraseEntireLineEscapeSequence[]      = {kWFEscape, '[', '2', 'K', 0x00};
static ROM int8_t eraseEntireScreenEscapeSequence[]    = {kWFEscape, '[', '2', 'J', 0x00};
static ROM int8_t underlineModeEscapeSequence[]        = {kWFEscape, '[', '4', 'm', 0x00};
static ROM int8_t normalModeEscapeSequence[]           = {kWFEscape, '[', 'm', 0x00};
static ROM int8_t highlightModeEscapeSequence[]        = {kWFEscape, '[', '1', 'm', 0x00};
static ROM int8_t inverseVideoEscapeSequence[]         = {kWFEscape, '[', '7', 'm', 0x00};

// VT100 escape sequences that are input from the terminal
// (note, if we ever use a longer sequence, update kWFMaxInputEscapeSequence)
static ROM int8_t upArrowEscapeSequence[]     = {kWFEscape, 0x5b, 'A', 0x00};
static ROM int8_t downArrowEscapeSequence[]   = {kWFEscape, 0x5b, 'B', 0x00};
static ROM int8_t rightArrowEscapeSequence[]  = {kWFEscape, 0x5b, 'C', 0x00};
static ROM int8_t leftArrowEscapeSequence[]   = {kWFEscape, 0x5b, 'D', 0x00};
static ROM int8_t homeKeyEscapeSequence[]     = {kWFEscape, 0x5b, 'H', 0x00};
static ROM int8_t endKeyEscapeSequence[]      = {kWFEscape, 0x5b, 'K', 0x00};

/*******************************************************************************
 *                                 GLOBALS
 *******************************************************************************/
#ifdef __XC8
#pragma udata wfcli_section
#endif

    tConsoleContext g_ConsoleContext;

#ifdef __XC8
#pragma udata
#endif

/*******************************************************************************
 *                     LOCAL FUNCTION PROTOTYPES
 *******************************************************************************/
#if 0   /* add back if needed */
static bool          isCtrlCharacter(int8_t c);
static void    NormalMode(void);
static void    UnderlineMode(void);
#endif

static bool          isPrintableCharacter(int8_t c);
static bool          isCmdLineOnlyWhitespace(void);
static void    InsertCharacter(int8_t c);
static void    Delete(void);
static void    Backspace(void);
static void    EchoCharacter(int8_t new_char);

#if 0 /* add back if you need this feature */
static void    EraseEntireScreen(void);
#endif

static void    EraseEntireLine(void);
static void    CursorRight(void);
static void    CursorRight_N(uint8_t n);
static void    CursorLeft(void);
static void    CursorLeft_N(uint8_t n);
static void    CursorHome(void);
static void    CursorEnd(void);

static void    OutputLine(int8_t lineChar, uint8_t count);
static void    Enter(void);
static void    ProcessEscapeSequence(int8_t *p_escape_sequence);
static void    OutputCommandPrompt(void);

#if (kWFNumHistoryEntries > 0)
static void    InitHistory(void);
static void    AddCmdToHistory(void);
static void    DisplayHistoryEntry(uint8_t action);
#endif

/*****************************************************************************
 * FUNCTION: WFConsoleInit
 *
 * RETURNS: None
 *
 * PARAMS:
 *
 * NOTES:   Initialization for console thread
 *
 *****************************************************************************/
void WFConsoleInit(void)
{
    SET_RX_STATE(kSTWaitForChar);
    SET_CURSOR(0);

    // zero out command line buffer
    memset(g_ConsoleContext.rxBuf, 0x00, sizeof(g_ConsoleContext.rxBuf));

    g_ConsoleContext.bStateMachineLoop = true;
    g_ConsoleContext.firstChar         = false;
    g_ConsoleContext.appConsoleMsgRx   = false;
    SET_ECHO_ON();
#if (kWFNumHistoryEntries > 0)
    InitHistory();
#endif

    g_ConsoleContext.echoOn = true;
}

/*****************************************************************************
 * FUNCTION: WFConsoleProcess
 *
 * RETURNS: None
 *
 * PARAMS:  None
 *
 * NOTES:   State machine called from main loop of app.  Handles serial input.
 *
 *****************************************************************************/
void WFConsoleProcess(void)
{
    //uint8_t *pStart = &(cmdline[0]);
    uint16_t rc;
    int8_t c;
    static int8_t escape_sequence[kWFMaxInputEscapeSequence];

    // if this state machine has been disabled
    if (g_ConsoleContext.bStateMachineLoop == false)
    {
        return;
    }

    // if a command was entered that is application=specific
    if (g_ConsoleContext.appConsoleMsgRx == true)
    {
        return; // wait until app done before processing further characters
    }

    // if no character(s) received
    if ( (rc = DataRdyUART() ) == 0u )
    {
        return;
    }

    // get the character
    c = (int8_t) ReadUART();

    // if this is the very first character received by this state machine
    if (g_ConsoleContext.firstChar == false)
    {
        Output_Monitor_Hdr();
        g_ConsoleContext.firstChar = true;
    }

    switch (GET_RX_STATE()) {
    //------------------------------------------
    case kSTWaitForChar:
    //------------------------------------------
        // if a 'normal' printable character
        if (isPrintableCharacter(c))
        {
            InsertCharacter(c);
        }
        // else if Delete key
        else if (c == kWFDelete)
        {
            Delete();
        }
        // else if Backspace key
        else if (c == (int8_t)kWFBackspace)
        {
            Backspace();
        }
        // else if Enter key
        else if (c == kWFEnter)
        {
            Enter();
        }
        // else if Escape key
        else if (c == kWFEscape)
        {
            /* zero out escape buffer, init with ESC */
            memset(escape_sequence, 0x00, sizeof(escape_sequence));
            escape_sequence[0] = kWFEscape;
            SET_RX_STATE(kSTWaitForEscSeqSecondChar);
        }
        // else if Ctrl C
        else if (c == kWFCtrl_C)
        {
           OutputCommandPrompt();
        }
        else {
            // Enter();
        }
        break;

    //------------------------------------------
    case kSTWaitForEscSeqSecondChar:
    //------------------------------------------
        /* if an arrow key, home, or end key (which is all that this state machine handles) */
        if (c == 0x5b)
        {
           escape_sequence[1] = c;
           SET_RX_STATE(kSTWaitForEscSeqThirdChar);
        }
        // else if user pressed escape followed by any printable character
        else if (isPrintableCharacter(c))
        {
            InsertCharacter(c);
            SET_RX_STATE(kSTWaitForChar);
        }
        // start this command line over
        else // anything else
        {
            OutputCommandPrompt();
            SET_RX_STATE(kSTWaitForChar);
        }
        break;

    //------------------------------------------
    case kSTWaitForEscSeqThirdChar:
    //------------------------------------------
        escape_sequence[2] = c;
        ProcessEscapeSequence(escape_sequence);
        SET_RX_STATE(kSTWaitForChar);
        break;

    } // end switch
}

/*****************************************************************************
 * FUNCTION: WFConsoleProcessEpilogue
 *
 * RETURNS: None
 *
 * PARAMS:  None
 *
 * NOTES:   Check if there is a left console msg, and release it if found.
 *
 *****************************************************************************/
void WFConsoleProcessEpilogue(void)
{
    if (WFConsoleIsConsoleMsgReceived())
    {
#if defined(WF_EASY_CONFIG_DEMO) && !defined(__XC32)
        putrsUART("Iperf supports only PIC32 for EasyConfig\n\r");
#else
        if (( memcmppgm2ram(ARGV[0], "iperf", 5) == 0 ) || ( memcmppgm2ram(ARGV[0], "kill", 4) == 0 ))
        {
            return;
        }
#endif
        if ( memcmppgm2ram(ARGV[0], "help", 4) != 0 )
        {
            WFConsolePrintRomStr("Unknown cmd: ", false);
            WFConsolePrintRamStr(ARGV[0], true);
        }

        WFConsoleReleaseConsoleMsg();
    }
}

int8_t ** WFConsoleGetCmdLineArgv(void)
{
    return g_ConsoleContext.argv;
}

uint8_t WFConsoleGetCmdLineArgc(void)
{
    return g_ConsoleContext.argc;
}

/*****************************************************************************
 * FUNCTION: ProcessEscapeSequence
 *
 * RETURNS: None
 *
 * PARAMS:  pEscapeSequence -- escape sequence string to be processed.
 *
 * NOTES:   Processes an escape sequence received by the state machine
 *
 *****************************************************************************/
static void ProcessEscapeSequence(int8_t *pEscapeSequence)
{

   /* if a Left Arrow Key */
   if (strcmppgm2ram( (const char *) pEscapeSequence, (ROM FAR char*) leftArrowEscapeSequence) == 0)
   {
      CursorLeft();
   }
   /* else if Right Arrow Key */
   else if (strcmppgm2ram( (const char *) pEscapeSequence, (ROM FAR char*) rightArrowEscapeSequence) == 0)
   {
      CursorRight();
   }
#if (kWFNumHistoryEntries > 0)
   /* else if Up Arrow Key */
   else if (strcmppgm2ram( (const char *) pEscapeSequence, (ROM FAR char*) upArrowEscapeSequence) == 0)
   {

      DisplayHistoryEntry(kWFPrevHistory);
   }
   /* else if Down Arrow Key */
   else if (strcmppgm2ram( (const char *) pEscapeSequence, (ROM FAR char*) downArrowEscapeSequence) == 0)
   {
      DisplayHistoryEntry(kWFNextHistory);
   }
#endif
   /* else if Home Key */
   else if (strcmppgm2ram( (const char *) pEscapeSequence, (ROM FAR char*) homeKeyEscapeSequence) == 0)
   {
      CursorHome();
   }
   /* else if End Key */
   else if (strcmppgm2ram( (const char *) pEscapeSequence, (ROM FAR char*) endKeyEscapeSequence) == 0)
   {
      CursorEnd();
   }
}

/*= Output_Monitor_Hdr =======================================================
Purpose: After clearing screen, outputs to terminal the header block of text.

Inputs:  None

Returns: None
============================================================================*/
void Output_Monitor_Hdr(void)
{
    putrsUART("\n\r");
    OutputLine('=', 79);
    putrsUART("* WiFi Host Interface Monitor\n\r");
    putrsUART("* (c) 2008, 2009, 2010, 2011 -- Microchip Technology, Inc.\n\r");
    putrsUART("*\n\r* Type 'help' to get a list of commands.\n\r");
    OutputLine('=', 79);
    OutputCommandPrompt();

}

/*= OutputLine ===============================================================
Purpose: Outputs a line of the specified character

Inputs:  lineChar -- character the comprises the line
         count    -- number of characters in the line

Returns: None
============================================================================*/
static void OutputLine(int8_t lineChar, uint8_t count)
{
    uint8_t i;

    for (i = 0; i < count; ++i)
    {
        while (BusyUART())
            ;

        putcUART(lineChar);
    }
    putrsUART("\n\r");
}

/*= is_Printable_Character ===================================================
Purpose: Determines if the input character can be output to the screen

Inputs:  c  -- char to test

Returns: True if printable, else False
============================================================================*/
static bool isPrintableCharacter(int8_t c)
{
   if ( ((isalpha(c))   ||
        (isdigit(c))    ||
        (isspace(c))    ||
        (ispunct(c)))

            &&

        (c != (int8_t)kWFEnter) && (c != (int8_t)kWFTab)

      )
   {
      return true;
   }
   else
   {
      return false;
   }
}

/*= InsertCharacter =========================================================
Purpose: Inserts and echoes an printable character into the command line at the
         cursor location.

Inputs:  c  -- char to insert

Returns: none
============================================================================*/
static void InsertCharacter(int8_t c)
{
   uint8_t len;

   uint8_t i;
   uint8_t orig_cursor_index = GET_CURSOR();
   uint8_t count;

   /* throw away characters if exceeded cmd line length */
   if (GET_LEN_RX_CMD_STRING() >= sizeof(g_ConsoleContext.rxBuf) - 1)
   {
      return;
   }

   len = GET_LEN_RX_CMD_STRING() + gCmdLinePromptLength;

   /* if inserting a character at end of cmd line */
   if (GET_CURSOR() == len)
   {
      g_ConsoleContext.rxBuf[GET_CURSOR() - gCmdLinePromptLength] = c;
      SET_CURSOR(GET_CURSOR() + 1);
      EchoCharacter(c);
   }
   /* inserting a character somewhere before the end of command line */
   else
   {
      /* Null out tmp cmd line */
      memset(gTmpCmdLine, 0x00, sizeof(gTmpCmdLine));

      /* copy up to the point of insertion */
      strncpy( (char *) gTmpCmdLine, (const char *) g_ConsoleContext.rxBuf, GET_CURSOR() - gCmdLinePromptLength);

      /* insert the new character */
      gTmpCmdLine[GET_CURSOR() - gCmdLinePromptLength] = c;

      /* copy the chars after the new character */
      strncpy( (char *) &gTmpCmdLine[GET_CURSOR() - gCmdLinePromptLength + 1],
               (const char *) &g_ConsoleContext.rxBuf[GET_CURSOR() - gCmdLinePromptLength],
               len - GET_CURSOR());

      /* put the first part of new string in the cmd line buffer */
      strcpy( (char *) g_ConsoleContext.rxBuf, (const char *) gTmpCmdLine);

      /* erase entire line, put the cursor at index 0 */
      EraseEntireLine();

      /* output the prompt */
      putrsUART( (ROM FAR char *) gCmdLinePrompt);

      /* Output the updated command line */
      putsUART( (char *) &g_ConsoleContext.rxBuf[0]);

      /* move the cursor to the next insert location */
      count = (len + 1) - orig_cursor_index - 1;
      for (i = 0; i < count; ++i)
      {
         putrsUART( (ROM FAR char *) cursorLeftEscapeSequence);
      }

      SET_CURSOR(orig_cursor_index + 1);
   }
}

/*= Delete ==================================================================
Purpose: Deletes the character at the cursor index

Inputs:  none

Returns: none
============================================================================*/

static void Delete(void)
{
   unsigned int num_chars;
   unsigned int orig_index = GET_CURSOR();

   /* if cursor is not at the end of the line */
   if (GET_CURSOR() != GET_LEN_RX_CMD_STRING() + gCmdLinePromptLength)
   {
      /* Null out tmp cmd line */
      memset(gTmpCmdLine, 0x00, sizeof(gTmpCmdLine));

      /* get characters before the deleted key */
      num_chars = GET_CURSOR() - gCmdLinePromptLength;
      strncpy( (char *) gTmpCmdLine, (const char *) g_ConsoleContext.rxBuf, num_chars);

      /* append characters after the deleted char (if there are any) */
      if (strlen( (char *) g_ConsoleContext.rxBuf) - 1 > 0u)
      {
         strcpy( (char *) &gTmpCmdLine[num_chars], (const char *) &g_ConsoleContext.rxBuf[num_chars + 1]);
      }

      EraseEntireLine();               /* leaves g_ConsoleContext.cursorIndex at 0 */
      putrsUART( (ROM FAR char *) gCmdLinePrompt);

      strcpy( (char *) g_ConsoleContext.rxBuf, (const char *) gTmpCmdLine);

      putsUART( (char *) g_ConsoleContext.rxBuf );
      SET_CURSOR(gCmdLinePromptLength + GET_LEN_RX_CMD_STRING());
      CursorHome(); /* to first character after prompt */

      /* move cursor to point of delete */
      CursorRight_N(orig_index - gCmdLinePromptLength);
   }
}

/*= EchoCharacter ===========================================================
Purpose: Echoes a character to the terminal.

Inputs:  new_char -- character to echo

Returns: none
============================================================================*/
static void EchoCharacter(int8_t c)
{
    if (IS_ECHO_ON())
    {
       /* output cr then lf for lf */
       if (c == (int8_t)'\n')
       {
          putcUART('\r');
       }

       putcUART(c);
    }
}

/*= Enter ====================================================================
Purpose: Enter key processing

Inputs:  None

Returns: none
============================================================================*/
static void Enter(void)
{
   bool cmd_flag = false;

   if ( IS_ECHO_ON() )
   {
       /* if the command line has any characters in it and it is not just white space */
       if ( (GET_LEN_RX_CMD_STRING() > 0u)  &&  (!isCmdLineOnlyWhitespace() ) )
       {
#if (kWFNumHistoryEntries > 0)
          AddCmdToHistory();
#endif
          cmd_flag = true;
       }
   }
   // else talking to PC app, presume it only sends valid commands
   else
   {
       cmd_flag = true;
   }

   // Process command
   if (cmd_flag == true)
   {
       process_cmd();
   }

   // if we got an app-specific command,
   if (g_ConsoleContext.appConsoleMsgRx == false)
   {
       /* linefeed and output prompt */
       OutputCommandPrompt();
   }

   // don't output command prompt, which also clears rx buf, until app processes it's command

}

#if (kWFNumHistoryEntries > 0)

/*****************************************************************************
 * FUNCTION: InitHistory
 *
 * RETURNS: None
 *
 * PARAMS:  None
 *
 * NOTES:   Initialize command line history states.
 *
 *****************************************************************************/
static void InitHistory(void)
{
    history.index       = 0;
    history.seeded      = false;
    history.recallIndex = 0;
}

/*****************************************************************************
 * FUNCTION: AddCmdToHistory
 *
 * RETURNS: None
 *
 * PARAMS:  None
 *
 * NOTES:   Adds latest command to history buffer
 *
 *****************************************************************************/
static void AddCmdToHistory(void)
{
    // zero out current command at this location
    memset((void *)&history.buf[history.index], 0x00, sizeof(history.buf[history.index]));

    // copy new command to buffer
    memcpy( (void *) &history.buf[history.index], (void *) g_ConsoleContext.rxBuf, strlen( (char *) g_ConsoleContext.rxBuf));

    // bump index to next line in history buffer
    history.index = (history.index + 1) % kWFNumHistoryEntries;

    // put the recall index one command in advance of the command we just added
    history.recallIndex = history.index;

    // at least one entry in history buffer
    history.seeded = true;
}

/*****************************************************************************
 * FUNCTION: DisplayHistoryEntry
 *
 * RETURNS: None
 *
 * PARAMS:  action -- PREV_HISTORY or NEXT_HISTORY
 *
 * NOTES:   In response to the user pressing up or down arrow key, display
 *          corresponding entry in history buffer.
 *
 *****************************************************************************/
static void DisplayHistoryEntry(uint8_t action)
{

   bool foundEntry = false;

   // if nothing in history buffer
   if (history.seeded == false)
   {
      return;
   }

   if (action == (uint8_t)kWFPrevHistory)
   {
      --history.recallIndex;
      if (history.recallIndex < 0)
      {
         history.recallIndex = kWFNumHistoryEntries - 1;
      }

      /* search until found a history entry or searched all entries */
      while (foundEntry == false)
      {
         /* if found a history entry */
         if (history.buf[history.recallIndex][0] != 0)
         {
            foundEntry = true;
         }
         else
         {
            --history.recallIndex;
            if (history.recallIndex < 0)
            {
               history.recallIndex = kWFNumHistoryEntries  - 1;
            }
         }
      }
   }
   else /* action == kWFNextHistory */
   {
      history.recallIndex = (history.recallIndex + 1) % kWFNumHistoryEntries;

      /* search until found a history entry or searched all entries */
      while (foundEntry == false)
      {
         /* if found a history entry */
         if (history.buf[history.recallIndex][0] != 0)
         {
            foundEntry = true;
         }
         else
         {
            history.recallIndex = (history.recallIndex + 1) % kWFNumHistoryEntries;
         }
      }
   }

   if (foundEntry)
   {
      // erase line on screen and output command from history
      EraseEntireLine();          /* leaves Cursor_Index at 0 */
      putrsUART( (ROM FAR char *) gCmdLinePrompt );
      putsUART( (char *) history.buf[history.recallIndex]);

      // copy history command to console buffer (so they match) and put cursor
      // at end of line
      memset(g_ConsoleContext.rxBuf, 0x00, GET_LEN_RX_CMD_STRING() );
      strcpy( (char *) g_ConsoleContext.rxBuf, (const char *) history.buf[history.recallIndex]);
      SET_CURSOR(gCmdLinePromptLength + strlen( (char *) history.buf[history.recallIndex]));
   }

}
#endif  /* (kWFNumHistoryEntries > 0) */

/*= Backspace ================================================================
Purpose: Performs a backspace operation on the command line

Inputs:  none

Returns: none
============================================================================*/
static void Backspace(void)
{
   uint8_t num_chars;
   uint8_t orig_index = GET_CURSOR();

   /* if cursor is not at the left-most position */
   if (GET_CURSOR() != gCmdLinePromptLength)
   {
      /* Null out tmp cmd line */
      memset(gTmpCmdLine, 0x00, sizeof(gTmpCmdLine));

      /* get characters before the backspace */
      num_chars = GET_CURSOR() - gCmdLinePromptLength - 1;
      strncpy( (char *) gTmpCmdLine, (const char *) g_ConsoleContext.rxBuf, num_chars);

      /* append characters after the deleted char (if there are any) */
      if ( (GET_LEN_RX_CMD_STRING() - 1) > 0u)
      {
         strcpy( (char *) &gTmpCmdLine[num_chars], (const char *) &g_ConsoleContext.rxBuf[num_chars + 1]);
      }

      EraseEntireLine();  /* leaves g_ConsoleContext.cursorIndex at 0 */

      strcpy( (char *) g_ConsoleContext.rxBuf, (const char *) gTmpCmdLine);

      putrsUART( (ROM FAR char *) gCmdLinePrompt);
      putsUART( (char *) g_ConsoleContext.rxBuf);
      SET_CURSOR(gCmdLinePromptLength + GET_LEN_RX_CMD_STRING());

      CursorHome(); /* to first character after prompt */

      /* move cursor to point of backspace */
      CursorRight_N(orig_index - 1 - gCmdLinePromptLength);
   }
}

static void EraseEntireLine()
{
   // int i;
   putrsUART( (ROM FAR char*) eraseEntireLineEscapeSequence);
   CursorLeft_N(GET_CURSOR());
   SET_CURSOR(0);
}

#if 0  /* add back if you want this feature */
static void EraseEntireScreen()
{
   putrsUART( (ROM FAR char*) eraseEntireScreenEscapeSequence);
}
#endif

static void OutputCommandPrompt(void)
{
    if ( IS_ECHO_ON() )
    {
     putrsUART("\n\r");
     putrsUART((ROM FAR char*) gCmdLinePrompt);
    }
    SET_CURSOR(gCmdLinePromptLength);
    memset(g_ConsoleContext.rxBuf, 0x00, sizeof(g_ConsoleContext.rxBuf));

}

/*= CursorRight =============================================================
Purpose: Moves the cursor right by one character

Inputs:  none

Returns: none
============================================================================*/
void CursorRight(void)
{
   /* if cursor is not already at the right-most position */
   if (GET_CURSOR() < GET_LEN_RX_CMD_STRING() + gCmdLinePromptLength)
   {
      SET_CURSOR( GET_CURSOR() + 1);
      putrsUART( (ROM FAR char*) cursorRightEscapeSequence);
   }
}

/*= CursorRight_N ==============================================================
Purpose: Moves the cursor left N characters to the right

Inputs:  n -- number of characters to move the cursor to the left

         Note: This sequence only takes a single digit of length, so may need to
               do the move in steps

Returns: none
============================================================================*/
void CursorRight_N(uint8_t n)
{
   int8_t sequence_string[sizeof(cursorRightEscapeSequence) + 2];  /* null and extra digit */

//   ASSERT(n <= (strlen(g_ConsoleContext.buf) + CMD_LINE_PROMPT_LENGTH));

   if (n > 0u)
   {
      SET_CURSOR( GET_CURSOR() + n );
      sequence_string[0] = cursorRightEscapeSequence[0]; /* ESC */
      sequence_string[1] = cursorRightEscapeSequence[1];  /* '[' */

      if (n < 10u)
      {
         sequence_string[2] = n + '0';  /* ascii digit */
         sequence_string[3] = cursorRightEscapeSequence[3];    /* 'C' */
         sequence_string[4] = '\0';
      }
      else
      {
         sequence_string[2] = (n / 10) + '0';  /* first ascii digit  */
         sequence_string[3] = (n % 10) + '0';  /* second ascii digit */
         sequence_string[4] = cursorRightEscapeSequence[3];    /* 'C' */
         sequence_string[5] = '\0';

      }

      putsUART( (char *) sequence_string);
   }
}

/*= CursorLeft ==============================================================
Purpose: Moves the cursor left by one character

Inputs:  none

Returns: none
============================================================================*/
void CursorLeft(void)
{
   /* if cursor is not already at the left-most position */
   if (GET_CURSOR() > strlenpgm( (ROM FAR char *) gCmdLinePrompt))
   {
      SET_CURSOR( GET_CURSOR() - 1);
      putrsUART( (ROM FAR char *) cursorLeftEscapeSequence);
   }
}

/*= CursorLeft_N ==============================================================
Purpose: Moves the cursor left N characters to the left

Inputs:  n -- number of characters to move the cursor to the left

         Note: This sequence only takes a single digit of length, so may need to
               do the move in steps

Returns: none
============================================================================*/
void CursorLeft_N(uint8_t n)
{
   int8_t sequence_string[sizeof(cursorLeftEscapeSequence) + 2];  /* null and extra digit */

//   ASSERT(n <= g_ConsoleContext.cursorIndex + CMD_LINE_PROMPT_LENGTH);

   if (n > 0u)
   {
      SET_CURSOR( GET_CURSOR() - n );

      sequence_string[0] = cursorLeftEscapeSequence[0];  /* ESC */
      sequence_string[1] = cursorLeftEscapeSequence[1];  /* '[' */

      if (n < 10u)
      {
         sequence_string[2] = n + '0';  /* ascii digit */
         sequence_string[3] = cursorLeftEscapeSequence[3];    /* 'D' */
         sequence_string[4] = '\0';
      }
      else
      {
         sequence_string[2] = (n / 10) + '0';  /* first ascii digit  */
         sequence_string[3] = (n % 10) + '0';  /* second ascii digit */
         sequence_string[4] = cursorLeftEscapeSequence[3];    /* 'D' */
         sequence_string[5] = '\0';

      }

      putsUART( (char *) sequence_string);
   }
}

/*= CursorHome ==============================================================
Purpose: Moves the cursor to the left-most position of the command line (just
         in front of the prompt).

Inputs:  none

Returns: none
============================================================================*/
static void CursorHome(void)
{
   /* if cursor not at start of command line */
   if (GET_CURSOR() != gCmdLinePromptLength)
   {
      /* move it to start of command line */
      CursorLeft_N(GET_CURSOR() - gCmdLinePromptLength);
   }
}

static void CursorEnd(void)
{
   uint8_t len;

   if ( (GET_LEN_RX_CMD_STRING() + gCmdLinePromptLength) != GET_CURSOR())
   {
      len = GET_LEN_RX_CMD_STRING() - GET_CURSOR() + gCmdLinePromptLength;
      CursorRight_N(len);
   }
}

static bool isCmdLineOnlyWhitespace(void)
{
   uint8_t i;
   uint8_t len = GET_LEN_RX_CMD_STRING();

   for (i = 0; i < len; ++i)
   {
      if ( !isspace(g_ConsoleContext.rxBuf[i]) )
      {
         return false;
      }
   }

   return true;
}

#if 0   /* Add back if you need this func */

static void UnderlineMode(void)
{
    putrsUART(inverseVideoEscapeSequence);
}

static void NormalMode(void)
{
    putrsUART(normalModeEscapeSequence);
}

/*****************************************************************************
 * FUNCTION: isCtrlCharacter
 *
 * RETURNS: true if input is a ctrl character, else false
 *          REG_OK_16_BIT_REG -- valid 16-bit register
 *          REG_UNKNOWN       -- unknown register ID
 *          REG_VAL_TOO_LARGE -- reg value to large for an 8-bit register
 *
 * PARAMS:  None
 *
 * NOTES:   Called by do_writereg_cmd and do_readreg_cmd to verify if accessing
 *          a legal register.  In the case of write, function verifies that
 *          write value doesn't overflow an 8-bit register.
 *
 *****************************************************************************/
static bool isCtrlCharacter(int8_t c)
{
    if (isprint(c))
    {
        return false;
    }
    else
    {
        return true;
    }
}
#endif

void WFConsoleSetMsgFlag(void)
{
    g_ConsoleContext.appConsoleMsgRx = true;
}

void WFConsoleReleaseConsoleMsg(void)
{
    /* linefeed and output prompt */
    OutputCommandPrompt();

    g_ConsoleContext.appConsoleMsgRx = false;
}

bool WFConsoleIsConsoleMsgReceived(void)
{
    return g_ConsoleContext.appConsoleMsgRx;
}

void WFConsolePrintInteger(uint32_t val, char mode)
{
    switch (mode)
    {
    case 'c':
        sprintf( (char *) g_ConsoleContext.txBuf, "%c", (int)val);
        break;
    case 'x':
        sprintf( (char *) g_ConsoleContext.txBuf, "%x", (unsigned int)val);
        break;
    case 'u':
        sprintf( (char *) g_ConsoleContext.txBuf, "%u", (unsigned int)val);
        break;
    case 'd':
    default:
        sprintf( (char *) g_ConsoleContext.txBuf, "%d", (int)val);
    }

    putsUART( (char*) g_ConsoleContext.txBuf);
}

void WFConsolePrintHex(uint32_t val, uint8_t width)
{
    switch (width)
    {
    case 2:
        sprintf( (char *) g_ConsoleContext.txBuf, "%02x", (unsigned int)val);
        break;
    case 4:
        sprintf( (char *) g_ConsoleContext.txBuf, "%04x", (unsigned int)val);
        break;
    case 8:
        sprintf( (char *) g_ConsoleContext.txBuf, "%08lx", (unsigned long)val);
        break;
    default:
        sprintf( (char *) g_ConsoleContext.txBuf, "%x", (unsigned int)val);
    }

    putsUART( (char*) g_ConsoleContext.txBuf);
}

#endif /* WF_CONSOLE */
