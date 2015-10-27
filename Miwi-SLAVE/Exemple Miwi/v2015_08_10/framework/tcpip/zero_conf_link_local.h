/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    zero_conf_link_local.h

  Summary:
    

  Description:
    Zero Confiruation (Zeroconf) IPV4 Link Local Addressing
    Module for Microchip TCP/IP Stack

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

#ifndef __ZEROCONF_LINK_LOCAL_H_
#define __ZEROCONF_LINK_LOCAL_H_

#define ZEROCONF_LINK_LOCAL_DHCP_TIMEOUT 5 // Time for which ZCLL allows DHCP to get IP

// define the debugging levels
#include "tcpip/src/zero_conf_helper.h"

// For verbose debugging
#ifdef DEBUG_ZCLL
#define DEBUG_ZCLL_PRINT(x) debug_zcll_print(x)
#define DEBUG_ZCLL_MESG sprintf
#else
#define DEBUG_ZCLL_PRINT(x)
#if defined(__XC8)
#define DEBUG_ZCLL_MESG(x)
#else
#define DEBUG_ZCLL_MESG(x,y,...)
#endif
#endif

// For less verbose debugging
#ifdef INFO_ZCLL
#define INFO_ZCLL_PRINT(x) info_zcll_print(x)
#define INFO_ZCLL_MESG sprintf
#else
#define INFO_ZCLL_PRINT(x)
#if defined(__XC8)
#define INFO_ZCLL_MESG(x)
#else
#define INFO_ZCLL_MESG(x,y,...)
#endif
#endif

// For warnings, errors, and other exceptions
#ifdef WARN_ZCLL
#define WARN_ZCLL_PRINT(x) putsUART(x)
#define WARN_ZCLL_MESG sprintf
#else
#define WARN_ZCLL_PRINT(x)
#if defined(__XC8)
#define WARN_ZCLL_MESG(x)
#else
#define WARN_ZCLL_MESG(x,y,...)
#endif
#endif

// Always print
#define ZCLL_PRINT(x) putsUART(x)
#define ZCLL_MESG sprintf

#define DEBUG0_ZCLL_PRINT DEBUG_ZCLL_PRINT
#define DEBUG0_ZCLL_MESG DEBUG_ZCLL_MESG

void ZeroconfLLInitialize(void);
void ZeroconfLLProcess(void);

#endif // #ifndef __ZEROCONF_LINK_LOCAL_H_
