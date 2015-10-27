/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    ddns.h

  Summary:
    

  Description:
    DynDNS Headers for Microchip TCP/IP Stack

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

#ifndef __DYNDNSCLIENT_H_
#define __DYNDNSCLIENT_H_

/***************************************************************************
  Section:
    Dynamic DNS Client Configuration Parameters
 ***************************************************************************/
#define DDNS_CHECKIP_SERVER  (ROM uint8_t *)"checkip.dyndns.com" // Default CheckIP server for determining current IP address
#define DDNS_DEFAULT_PORT  (80u) // Default port for CheckIP server

/***************************************************************************
  Section:
    Dynamic DNS Enumerations
 ***************************************************************************/

// Dynamic DNS Services.
// Must support the DynDNS API (Auxlang) and correspond
// to ddnsServiceHosts and ddnsServicePorts in ddns.c.
typedef enum {
    DYNDNS_ORG = 0u, // www.dyndns.org
    NO_IP_COM, // www.no-ip.com
    DNSOMATIC_COM // www.dnsomatic.com
} DDNS_SERVICES;

// Status message for DynDNS client.  GOOD and NOCHG are OK, but
// ABUSE through 911 are fatal.  UNCHANGED through INVALID are locally
// defined.
typedef enum {
    DDNS_STATUS_GOOD = 0u, // Update successful, hostname is now updated
    DDNS_STATUS_NOCHG, // Update changed no setting and is considered abusive.  Additional 'nochg' updates will cause hostname to be blocked.

    DDNS_STATUS_ABUSE, // The hostname specified is blocked for update abuse.
    DDNS_STATUS_BADSYS, // System parameter not valid. Should be dyndns, statdns or custom.
    DDNS_STATUS_BADAGENT, // The user agent was blocked or not sent.
    DDNS_STATUS_BADAUTH, // The username and password pair do not match a real user.
    DDNS_STATUS_NOT_DONATOR, // An option available only to credited users (such as offline URL) was specified, but the user is not a credited user. If multiple hosts were specified, only a single !donator will be returned.
    DDNS_STATUS_NOT_FQDN, // The hostname specified is not a fully-qualified domain name (not in the form hostname.dyndns.org or domain.com).
    DDNS_STATUS_NOHOST, // The hostname specified does not exist in this user account (or is not in the service specified in the system parameter).
    DDNS_STATUS_NOT_YOURS, // The hostname specified does not belong to this user account.
    DDNS_STATUS_NUMHOST, // Too many hosts specified in an update.
    DDNS_STATUS_DNSERR, // Unspecified DNS error encountered by the DDNS service.
    DDNS_STATUS_911, // There is a problem or scheduled maintenance with the DDNS service.

    DDNS_STATUS_UPDATE_ERROR, // Error communicating with Update service.
    DDNS_STATUS_UNCHANGED, // The IP Check indicated that no update was necessary.
    DDNS_STATUS_CHECKIP_ERROR, // Error communicating with CheckIP service.
    DDNS_STATUS_INVALID, // DDNS Client data is not valid.
    DDNS_STATUS_UNKNOWN // DDNS client has not yet been executed with this configuration.
} DDNS_STATUS;

/***************************************************************************
  Summary:
    Configuration parameters for the Dynamic DNS Client

  Description:
    This structure of pointers configures the Dynamic DNS Client.  Initially,
    all pointers will be null and the client will be disabled.  <c>Set
    DDNSClient.[field name].szRAM</c> to use a string stored in RAM, or
    <c>DDNSClient.[field name].szROM</c> to use a string stored in ROM.
    (Where <c>[field name]</c> is one of the parameters below.)

    If a ROM string is specified, <c>DDNSClient.ROMPointers.[field name]</c>
    must also be set to 1 to indicate that this field should be retrieved
    from ROM instead of RAM.

  Parameters:
    CheckIPServer - The server used to determine the external IP address
    CheckIPPort -   Port on the above server to connect to
    UpdateServer -  The server where updates should be posted
    UpdatePort -    Port on the above server to connect to
    Username -      The user name for the dynamic DNS server
    Password -      The password to supply when making updates
    Host -          The host name you wish to update
    ROMPointers -   Indicates which parameters to read from ROM instead of RAM.
 ***************************************************************************/
typedef struct {
    union {
        uint8_t *szRAM;
        ROM uint8_t *szROM;
    } CheckIPServer;

    uint16_t CheckIPPort;

    union {
        uint8_t *szRAM;
        ROM uint8_t *szROM;
    } UpdateServer;

    uint16_t UpdatePort;

    union {
        uint8_t *szRAM;
        ROM uint8_t *szROM;
    } Username;

    union {
        uint8_t *szRAM;
        ROM uint8_t *szROM;
    } Password;

    union {
        uint8_t *szRAM;
        ROM uint8_t *szROM;
    } Host;

    struct {
        unsigned char CheckIPServer : 1;
        unsigned char UpdateServer : 1;
        unsigned char Username : 1;
        unsigned char Password : 1;
        unsigned char Host : 1;

    } ROMPointers;
} DDNS_POINTERS;

// Global DDNS Configuration parameters
extern DDNS_POINTERS DDNSClient;

/***************************************************************************
  Section:
    Function Prototypes
 ***************************************************************************/
void DDNSTask(void);
void DDNSInit(void);
void DDNSForceUpdate(void);
void DDNSSetService(DDNS_SERVICES svc);
DDNS_STATUS DDNSGetLastStatus(void);
IP_ADDR DDNSGetLastIP(void);

#endif
