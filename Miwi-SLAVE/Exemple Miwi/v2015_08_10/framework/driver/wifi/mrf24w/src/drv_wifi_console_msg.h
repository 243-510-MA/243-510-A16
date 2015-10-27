/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_wifi_console_msg.h

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

#ifndef __DRV_WF_CONSOLE_MSGS_H_
#define __DRV_WF_CONSOLE_MSGS_H_

#ifdef __cplusplus
 extern "C" {
#endif

/*******************************************************************************
 *                              DEFINES
 *******************************************************************************/

// !!! These defines MUST match the g_consoleCmd structure  !!!
enum validConsoleCmds
{
    HELP_MSG = 0,                   // only used by humans
    GET_WF_VERSION_MSG,
    RESET_HOST,
    CLEAR_SCREEN_MSG,
#if defined(MRF24WG)
    WPS_PIN_MSG,
    WPS_PUSHBUTTON_MSG,
    WPS_GET_CREDENTIALS_MSG,
#endif /* MRF24WG */

    IFCONFIG_MSG,
    IWCONFIG_MSG,
    IWPRIV_MSG,
    PING_MSG,
    KILLPING_MSG,
    IW_UPDATE_TCP_MSG,
    IW_UPDATE_UART_MSG,
    INVALID_CMD = 0xFF
};

#define kWFMaxTokensPerCmd  (16)  /* max tokens, including cmd and parameters */
#define kConsoleMaxMsgSize  (80)
#define kConsoleCmdMaxLen   (16)  /* max string length of console commands (w/o arguments) */
#define kConsoleCmdMaxNum   (8)   /* max number of registered console commands */

/*******************************************************************************
 *                                  TYPEDEFS
 *******************************************************************************/

// structure definition to define all ASCII messages
typedef struct msg_struct
{
    ROM int8_t   *p_cmdName;       // pointer to cmd name
    ROM int8_t   *p_cmdHelp;       // pointer to cmd help string
    uint8_t   maxTokens;            // max tokens for this command
} tWFCmd;

// parsed version numbers put in this structure
typedef struct version_struct
{
    uint8_t major;
    uint8_t minor;
    uint8_t revision;
} tWFVersion;

/*******************************************************************************
 *                          EXTERNAL GLOBALS
 *******************************************************************************/
extern  const tWFCmd    g_consoleCmd[];
extern  const uint8_t     g_numCmds;

/*******************************************************************************
 *                         FUNCTION PROTOTYPES
 *******************************************************************************/
void   TokenizeCmdLine(int8_t *p_line);
uint8_t  GetCmdId(void);
void   Output_Monitor_Hdr(void);
bool   ConvertASCIIHexToBinary(int8_t *p_ascii, uint16_t *p_binary);
bool   ConvertASCIIUnsignedDecimalToBinary(int8_t *p_ascii, uint16_t *p_binary);
bool   ConvertASCIISignedDecimalToBinary(int8_t *p_ascii, int16_t *p_binary);
uint8_t  HexToBin(uint8_t hexChar);
void   WFConsoleSetMsgFlag(void);

#ifdef __cplusplus
 }
#endif

#endif /* __DRV_WF_CONSOLE_MSGS_H_ */
