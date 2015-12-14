/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_wifi_console_msg_handler.h

  Summary:
    Module for Microchip TCP/IP Stack
    -Provides access to MRF24W WiFi controller
    -Reference: MRF24W Data sheet, IEEE 802.11 Standard

  Description:
    MRF24W Driver Console messages

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

#ifndef __DRV_WF_CONSOLE_MSG_HANDLER_H_
#define __DRV_WF_CONSOLE_MSG_HANDLER_H_

#include "drv_wifi_console_msg.h"

#if defined (WF_CONSOLE)
typedef struct
{
    int8_t    rxBuf[kConsoleMaxMsgSize];  // buf for received characters
    int8_t    txBuf[kConsoleMaxMsgSize];  // buf for transmitted characters
    uint8_t   rxState;                    // current state of Rx state machine
    uint8_t   cursorIndex;                // cursor index
    bool    firstChar;                  // false if waiting for very first character from user or PC
    bool    echoOn;                     // true if human input at console, false if PC communicating

    int8_t    p_cmdStrings[kConsoleCmdMaxNum][kConsoleCmdMaxLen];  // cmd string array
    uint8_t   numCmdStrings;              // number of cmd strings in p_cmdStrings

    uint8_t   appConsoleMsgRx;            // true if app received a console msg, else false

#if defined( __XC8 )
    FAR int8_t*   argv[kWFMaxTokensPerCmd];   // pointer to each token in the rxBuf
#else
    int8_t*       argv[kWFMaxTokensPerCmd];   // pointer to each token in the rxBuf
#endif

    uint8_t   argc;                       // number of tokens in rxBuf
    uint8_t   subState;
    bool    bStateMachineLoop;
    uint8_t   req;
} tConsoleContext;
#endif

#define SET_ECHO_ON()       g_ConsoleContext.echoOn = true
#define SET_ECHO_OFF()      g_ConsoleContext.echoOn = false
#define IS_ECHO_ON()        g_ConsoleContext.echoOn

/*******************************************************************************
 *                          FUNCTION PROTOTYPES
 *******************************************************************************/
bool convertAsciiToHexInPlace(int8_t *p_string, uint8_t expectedHexBinSize);

void process_cmd(void);

#endif /* __DRV_WF_CONSOLE_MSG_HANDLER_H_ */
