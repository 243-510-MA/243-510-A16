/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_wifi_console.h

  Summary:
    Module for Microchip TCP/IP Stack
    -Provides access to MRF24W WiFi controller
    -Reference: MRF24W Data sheet, IEEE 802.11 Standard

  Description:
    MRF24W Driver Console

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

#ifndef __DRV_WF_CONSOLE_H_
#define __DRV_WF_CONSOLE_H_

#include "drv_wifi_console_msg_handler.h"

#if defined (WF_CONSOLE)
  #if defined(__XC8)
    #pragma varlocate 4 g_ConsoleContext
  #endif

  extern tConsoleContext g_ConsoleContext;
#endif

#define ARGC           g_ConsoleContext.argc
#define ARGV           g_ConsoleContext.argv

#if !defined(WF_CS_TRIS) && defined(WF_CONSOLE)
  #undef WF_CONSOLE
#endif

#if !defined(WF_CONSOLE) && defined(WF_CONSOLE_IFCFGUTIL)
  #undef WF_CONSOLE_IFCFGUTIL
#endif

extern void WFConsoleInit(void);
extern void WFConsoleProcess(void);
extern void WFConsoleProcessEpilogue(void);
extern void WFConsoleReqClear(void);
extern bool WFConsoleIsConsoleMsgReceived(void);
extern void WFConsoleReleaseConsoleMsg(void);
extern int8_t ** WFConsoleGetCmdLineArgv(void);
extern uint8_t WFConsoleGetCmdLineArgc(void);
extern void WFConsoleSetMsgFlag(void);
extern bool WFConsoleIsIperfAppKillRequested(void);

extern void WFConsolePrintInteger(uint32_t val, char mode);
extern void WFConsolePrintHex(uint32_t val, uint8_t width);

#define WFConsolePrintRamStr(str, newline) do { \
        putsUART((char*)(str)); \
        if (newline) putrsUART("\n\r"); \
    } while (false)

#define WFConsolePrintRomStr(str, newline) do { \
        putrsUART(str); \
        if (newline) putrsUART("\n\r"); \
    } while (false)

#endif /* __DRV_WF_CONSOLE_H_ */
