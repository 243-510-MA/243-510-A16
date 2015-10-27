/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    dns.h

  Summary:


  Description:
    DNS Client/Server Module Header

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

#ifndef __DNS_H_
#define __DNS_H_

#define DNS_TYPE_A (1u) // Constant for record type in DNSResolve. Indicates an A (standard address) record.
#define DNS_TYPE_MX (15u) // Constant for record type in DNSResolve. Indicates an MX (mail exchanger) record.

// dns_client.c function prototypes
void DNSInit(void);
bool DNSBeginUsage(void);
void DNSResolve(uint8_t *HostName, uint8_t Type);
bool DNSIsResolved(IP_ADDR *HostIP);
bool DNSEndUsage(void);

#if defined(__XC8)
void DNSResolveROM(ROM uint8_t *Hostname, uint8_t Type);
#else
// Non-ROM variant for C30/C32
#define DNSResolveROM(a,b)  DNSResolve((uint8_t *)a,b)
#endif

// dns_server.c function prototype
void DNSServerTask(void);

#endif
