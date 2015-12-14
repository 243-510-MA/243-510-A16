/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    zero_conf_link_multicast_dns.h

  Summary:

  Description:
    Zero Configuration (Zeroconf) Multicast DNS and
    Service Discovery Module for Microchip TCP/IP Stack

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

#ifndef __ZEROCONF_MULTICAST_DNS_H_
#define __ZEROCONF_MULTICAST_DNS_H_

// --------------
// Configurations
//

#define HTTPUpdateRecord_INTERVAL (300u) // Seconds between HTTPUpdateRecord() calls.

#define MDNS_DEFAULT_SERVICE_NAME "Microchip Demo"

// define the debugging levels
#include "tcpip/src/zero_conf_helper.h"

#if defined(MRF24WG)
#include "driver/wifi/mrf24w/drv_wifi_api.h"
#endif

// For verbose debugging
#ifdef DEBUG_MDNS
#define DEBUG_MDNS_PRINT(x) debug_mdns_print(x)
#define DEBUG_MDNS_MESG sprintf
#else
#define DEBUG_MDNS_PRINT(x)
#if defined(__XC8)
#define DEBUG_MDNS_MESG(x)
#else
#define DEBUG_MDNS_MESG(x,y,...)
#endif
#endif

// For less verbose debugging
#ifdef INFO_MDNS
#define INFO_MDNS_PRINT(x) info_mdns_print(x)
#define INFO_MDNS_MESG sprintf
#else
#define INFO_MDNS_PRINT(x)
#if defined(__XC8)
#define INFO_MDNS_MESG(x)
#else
#define INFO_MDNS_MESG(x,y,...)
#endif
#endif

// For warnings, errors, and other exceptions
#ifdef WARN_MDNS
#define WARN_MDNS_PRINT(x) putsUART(x)
#define WARN_MDNS_MESG sprintf
#else
#define WARN_MDNS_PRINT(x)
#if defined(__XC8)
#define WARN_MDNS_MESG(x)
#else
#define WARN_MDNS_MESG(x,y,...)
#endif
#endif

// Always print
#define MDNS_PRINT(x) putsUART(x)
#define MDNS_MESG     sprintf

#define DEBUG0_MDNS_PRINT  INFO_MDNS_PRINT
#define DEBUG0_MDNS_MESG   INFO_MDNS_MESG

// --------------------------------------------------------------
// Function prototypes and data structures
//

void mDNSInitialize(const char *szHostName);
void mDNSProcess(void);
void mDNSFillHostRecord(void);

//void DisplayHostName(uint8_t *HostName);

typedef enum {
    MDNSD_SUCCESS = 0,
    MDNSD_ERR_BUSY = 1, /* Already Being used for another Service */
    MDNSD_ERR_CONFLICT = 2, /* Name Conflict */
    MDNSD_ERR_INVAL = 3, /* Invalid Parameter */
} MDNSD_ERR_CODE;

/* DNS-Service Discovery APIs */
MDNSD_ERR_CODE mDNSServiceRegister(
        const char *name,
        const char *regtype,
        uint16_t port,
        const uint8_t *txtRecord,
        uint8_t auto_rename,
        void (*call_back)(char *name, MDNSD_ERR_CODE err, void *context),
        void *context);
MDNSD_ERR_CODE mDNSServiceDeRegister();

MDNSD_ERR_CODE mDNSServiceUpdate(
        uint16_t port,
        const uint8_t *txt_record);

MDNSD_ERR_CODE mDNSHostRegister(const char *host_name);

MDNSD_ERR_CODE mDNSMulticastFilterRegister(void);

// #if defined(DEBUG_MDNS) || defined(INFO_MDNS)
void mDNSDumpInfo(void);
//#endif

#endif
