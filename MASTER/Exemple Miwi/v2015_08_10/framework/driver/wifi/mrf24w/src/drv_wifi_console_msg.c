/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_wifi_console_msg.c

  Summary:
    Module for Microchip TCP/IP Stack
    -Provides access to MRF24W WiFi controller
    -Reference: MRF24W Data sheet, IEEE 802.11 Standard

  Description:
    MRF24W Driver Console Messages

 *******************************************************************************/

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

#include <ctype.h>

#include "tcpip/tcpip.h"

#include "drv_wifi_console.h"

//---------------------
// token parsing states
//---------------------
enum
{
    kWFWaitingForStartOfToken,
    kWFWaitingForEndOfToken
};

//----------------
// Command strings
//----------------
ROM int8_t helpCmd[]      = "help";
ROM int8_t helpHelp[]     = "Lists all commands";

ROM int8_t getwfverCmd[]  = "getwfver";
ROM int8_t getwfverHelp[] = "Gets WiFi device version";

ROM int8_t resetCmd[]     = "reset";
ROM int8_t resetHelp[]    = "Reset host MCU";

ROM int8_t clsCmd[]       = "cls";
ROM int8_t clsHelp[]      = "Clears screen";

#if defined(MRF24WG)
ROM int8_t wpsPinCmd[]    = "wpspin";
ROM int8_t wpsPinHelp[]   = "8-digit WPS Pin. Eg wpspin 12390212. Next connection will use the WPS-Pin method";

ROM int8_t wpsPushButtonCmd[]  = "wpspbc";
ROM int8_t wpsPushButtonHelp[] = "Next connection will use the WPS-Push-Button method";

ROM int8_t wpsCredCmd[]        = "wpscred";
ROM int8_t wpsCredHelp[]       = "Dump WPS credentials";
#endif /* MRF24WG */

#if defined(WF_CONSOLE_IFCFGUTIL)
ROM int8_t ifConfigCmd[]  = "ifconfig";
ROM int8_t iwConfigCmd[]  = "iwconfig";
ROM int8_t iwPrivCmd[]    = "iwpriv";
#endif // WF_CONSOLE_IFCFGUTIL

ROM int8_t seeDocHelp[]     = "see documentation";
ROM int8_t pingCmd[]        = "ping";
ROM int8_t pingHelp[]       = "ping 192.168.1.1";

ROM int8_t KillPingCmd[]        = "killping";
ROM int8_t KillPingHelp[]       = "stop the running ping session";

ROM int8_t iw_update_tcp_Cmd[]      = "iw_update_tcp";
ROM int8_t iw_update_tcp_Help[]     = "iw_update_tcp";

ROM int8_t iw_update_uart_Cmd[]     = "iw_update_uart";
ROM int8_t iw_update_uart_Help[]    = "iw_update_uart";

//----------------------
// Console Command Table
//-----------------------
const tWFCmd g_consoleCmd[] = {

    {helpCmd,                      // cmd name
     helpHelp,                     // cmd description
     2},                           // max tokens

    {getwfverCmd,                  // [1]
     getwfverHelp,
     1},

    {resetCmd,                     // [2]
     resetHelp,
     1},

    {clsCmd,                       // [3]
     clsHelp,
     1},

#if defined(MRF24WG)
    {wpsPinCmd,
     wpsPinHelp,
     2},

     {wpsPushButtonCmd,
      wpsPushButtonHelp,
      1},

     {wpsCredCmd,
      wpsCredHelp,
      1},
#endif /* MRF24WG */

#if defined(WF_CONSOLE_IFCFGUTIL)
    {ifConfigCmd,                  // [4]
     seeDocHelp,
     12},

    {iwConfigCmd,                  // [5]
     seeDocHelp,
     12},

    {iwPrivCmd,                    // [6]
     seeDocHelp,
     12},
#endif // WF_CONSOLE_IFCFGUTIL

    {pingCmd,
    pingHelp,
    1},

    {KillPingCmd,
    KillPingHelp,
    1},

    {iw_update_tcp_Cmd,
    iw_update_tcp_Help,
    1},

    {iw_update_uart_Cmd,
    iw_update_uart_Help,
    1},
};

const uint8_t g_numCmds   = sizeof(g_consoleCmd) / sizeof(tWFCmd);

/*****************************************************************************
 * FUNCTION: ConvertASCIIHexToBinary
 *
 * RETURNS: true if conversion successful, else false
 *
 * PARAMS:  p_ascii   -- ascii string to be converted
 *          p_binary  -- binary value if conversion successful
 *
 * NOTES:   Converts an input ascii hex string to binary value (up to 32-bit value)
 *****************************************************************************/
bool ConvertASCIIHexToBinary(int8_t *p_ascii, uint16_t *p_binary)
{
    int8_t  i;
    uint32_t multiplier = 1;

    *p_binary = 0;

    // not allowed to have a string of more than 4 nibbles
    if (strlen((char*)p_ascii) > 8u)
    {
        return false;
    }

    // first, ensure all characters are a hex digit
    for (i = (uint8_t)strlen((char *)p_ascii) - 1; i >= 0 ; --i)
    {
        if (!isxdigit(p_ascii[i]))
        {
            return false;
        }
        *p_binary += multiplier * HexToBin(p_ascii[i]);
        multiplier *= 16;
    }

    return true;
}

/*****************************************************************************
 * FUNCTION: HexToBin
 *
 * RETURNS: binary value associated with ASCII hex input value
 *
 * PARAMS:  hexChar -- ASCII hex character
 *
 * NOTES:   Converts an input ascii hex character to its binary value.  Function
 *          does not error check; it assumes only hex characters are passed in.
 *****************************************************************************/
uint8_t HexToBin(uint8_t hexChar)
{
    if ((hexChar >= 'a') && (hexChar <= 'f'))
    {
        return (0x0a + (hexChar - 'a'));
    }
    else if ((hexChar >= 'A') && (hexChar <= 'F'))
    {
        return (0x0a + (hexChar - 'A'));
    }
    else //  ((hexChar >= '0') && (hexChar <= '9'))
    {
        return (0x00 + (hexChar - '0'));
    }

}

#if defined ( WF_CONSOLE )
/*****************************************************************************
 * FUNCTION: TokenizeCmdLine
 *
 * RETURNS: None
 *
 * PARAMS:  p_line -- pointer to the null terminated command line
 *
 * NOTES: Converts the input string into tokens separated by '\0'.
  *****************************************************************************/
void TokenizeCmdLine(int8_t *p_line)
{
    uint8_t state = kWFWaitingForStartOfToken;
    uint8_t index = 0;

    ARGC = 0;

    //---------------------------
    // while not at end of string
    //---------------------------
    while (p_line[index] != (int8_t)'\0')
    {

        //----------------------------------------
        if (state == (uint8_t)kWFWaitingForStartOfToken)
        //----------------------------------------
        {
            // if hit non whitespace
            if (!isspace((int)p_line[index]))
            {
               // argument string starts here
               ARGV[ARGC++] = (int8_t *)(&(p_line[index]));
               if (ARGC >= (uint8_t)kWFMaxTokensPerCmd)
               {
                   return;  // truncate because too many tokens
               }
               state = kWFWaitingForEndOfToken;
            }
            ++index;

        }
        //----------------------------------------
        else if (state == (uint8_t)kWFWaitingForEndOfToken)
        //----------------------------------------
        {
            // if white space, then end of token
            if (isspace((int)p_line[index]))
            {
                // string terminate the token
                p_line[index] = '\0';
                state = kWFWaitingForStartOfToken;
            }
            ++index;
        }
    }
}

/*****************************************************************************
 * FUNCTION: GetCmdId
 *
 * RETURNS: None
 *
 * PARAMS:  void
 *
 * NOTES: Determines index of cmd in CMD struct
  *****************************************************************************/
uint8_t GetCmdId(void)
{
    uint8_t i;
    const tWFCmd  *p_msgList;
    uint16_t  msgCount;

    p_msgList = g_consoleCmd;
    msgCount  = g_numCmds;

    for (i = 0; i < msgCount; ++i)
    {
        if ( strcmppgm2ram( (FAR char *)ARGV[0], (FAR ROM char *) p_msgList[i].p_cmdName) == 0)
        {
            return i;
        }
    }

    return INVALID_CMD;
}

/*****************************************************************************
 * FUNCTION: ConvertASCIIUnsignedDecimalToBinary
 *
 * RETURNS: true if conversion successful, else false
 *
 * PARAMS:  p_ascii   -- ascii string to be converted
 *          p_binary  -- binary value if conversion successful
 *
 * NOTES:   Converts an input ascii decimal string to binary value
 *****************************************************************************/
bool ConvertASCIIUnsignedDecimalToBinary(int8_t *p_ascii, uint16_t *p_binary)
{
    int8_t  i;
    uint32_t multiplier = 1;
    int8_t len;

    *p_binary = 0;
    len = (int8_t)strlen((char *)p_ascii);

    // should not be any numbers greater than 6 digits
    if ((len > 5) || (len == 0))
    {
        return false;
    }

    // first, ensure all characters are a decimal digit
    for (i = len - 1; i >= 0 ; --i)
    {
        if (!isdigit(p_ascii[i]))
        {
            return false;
        }
        *p_binary += multiplier * (p_ascii[i] - '0');
        multiplier *= 10;
    }

    return true;
}

/*****************************************************************************
 * FUNCTION: ConvertASCIISignedDecimalToBinary
 *
 * RETURNS: true if conversion successful, else false
 *
 * PARAMS:  p_ascii   -- ascii string to be converted
 *          p_binary  -- binary value if conversion successful
 *
 * NOTES:   Converts an input ascii signed decimal string to binary value
 *****************************************************************************/
bool ConvertASCIISignedDecimalToBinary(int8_t *p_ascii, int16_t *p_binary)
{
    int8_t   i;
    uint32_t  multiplier = 1;
    bool negFlag = false;
    int8_t   endIndex = 0;
    int8_t  len;

    *p_binary = 0;
    len = (int8_t)strlen((char *)p_ascii);

    // should not be any numbers greater than 5 digits (with -)
    if (len > 6)
    {
        return false;
    }

    if (p_ascii[0] == (int8_t)'-')
    {
        negFlag = true;
        endIndex = 1;
    }

    // first, ensure all characters are a decimal digit

    for (i = len - 1; i >= endIndex ; --i)
    {
        if (!isdigit(p_ascii[i]))
        {
            return false;
        }
        *p_binary += multiplier * (p_ascii[i] - '0');
        multiplier *= 10;
    }

    if (negFlag == true)
    {
        *p_binary *=  - 1;
    }

    return true;
}

bool ExtractandValidateU16Range(int8_t *p_string, uint16_t *pValue, uint16_t minValue, uint16_t maxValue)
{
    /* extract next parameter as an unsigned short integer */
    if (!ConvertASCIIUnsignedDecimalToBinary(p_string, pValue))
    {
        /* WFConsolePrintf("   Unable to parse paramter value"); */
        return false;
    }

    if ((*pValue < minValue) || (*pValue > maxValue))
    {
        /* WFConsolePrintf("   parameter value out of range"); */
        return false;
    }

    return true;
}

#endif /* WF_CONSOLE */
