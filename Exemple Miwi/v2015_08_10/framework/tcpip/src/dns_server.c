/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    dns_server.c

  Summary:
    Module for Microchip TCP/IP Stack
    -Acts as a DNS server, but gives out the local IP address for all
     queries to force web browsers to access the board.
    -Reference: RFC 1034 and RFC 1035

  Description:
    Domain Name System (DNS) Server dummy

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

#define __DNSS_C_

#include "tcpip/tcpip.h"

#if defined(STACK_USE_DNS_SERVER)

// Default port for the DNS server to listen on
#define DNS_PORT 53u

#define BUF_SIZE_DNS_NAME 64
static uint8_t DnsName_buf[BUF_SIZE_DNS_NAME];
static uint8_t Size_HostName = 0;

static void DNSCopyRXNameToTX(void);
static void DNSGetRXName(void);
static bool DNSName_valified(char *name, int name_size);

/*********************************************************************
Function:
    void DNSServerTask(void)

  Summary:
    None

  Description:
    Sends dummy responses that point to ourself for DNS requests

  Precondition:
    None

  Parameters:
    None

  Returns:
    None
 ********************************************************************/
void DNSServerTask(void)
{
    static UDP_SOCKET MySocket = INVALID_UDP_SOCKET;

    struct {
        uint16_t wTransactionID;
        uint16_t wFlags;
        uint16_t wQuestions;
        uint16_t wAnswerRRs;
        uint16_t wAuthorityRRs;
        uint16_t wAdditionalRRs;
    } DNSHeader;

    // Create a socket to listen on if this is the first time calling this function
    if (MySocket == INVALID_UDP_SOCKET) {
        //MySocket = UDPOpen(DNS_PORT, NULL, 0);

        MySocket = UDPOpenEx(0, UDP_OPEN_SERVER, DNS_PORT, 0);
        return;
    }

    // See if a DNS query packet has arrived
    if (UDPIsGetReady(MySocket) < sizeof (DNSHeader))
        return;

    // Read DNS header
    UDPGetArray((uint8_t *) & DNSHeader, sizeof (DNSHeader));

    // Ignore this packet if it isn't a query
    if ((DNSHeader.wFlags & 0x8000) == 0x8000u)
        return;

    // Ignore this packet if there are no questions in it
    if (DNSHeader.wQuestions == 0u)
        return;

    DNSGetRXName();

    // Ignore this packet if the last works is not ".local"
    if (false == DNSName_valified(".local", 6))
        return;

    // Block until we can transmit a DNS response packet
    while (!UDPIsPutReady(MySocket));

    // Write DNS response packet
    UDPPutArray((uint8_t *) & DNSHeader.wTransactionID, 2); // 2 byte Transaction ID
    if (DNSHeader.wFlags & 0x0100)
        UDPPut(0x81); // Message is a response with recursion desired
    else
        UDPPut(0x80); // Message is a response without recursion desired flag set
    UDPPut(0x80); // Recursion available
    UDPPut(0x00); // 0x0000 Questions
    UDPPut(0x00);
    UDPPut(0x00); // 0x0001 Answers RRs
    UDPPut(0x01);
    UDPPut(0x00); // 0x0000 Authority RRs
    UDPPut(0x00);
    UDPPut(0x00); // 0x0000 Additional RRs
    UDPPut(0x00);
    DNSCopyRXNameToTX(); // Copy hostname of first question over to TX packet
    UDPPut(0x00); // Type A Host address
    UDPPut(0x01);
    UDPPut(0x00); // Class INternet
    UDPPut(0x01);
    UDPPut(0x00); // Time to Live 10 seconds
    UDPPut(0x00);
    UDPPut(0x00);
    UDPPut(0x0A);
    UDPPut(0x00); // Data Length 4 bytes
    UDPPut(0x04);
    UDPPutArray((uint8_t *) & AppConfig.MyIPAddr.Val, 4); // Our IP address

    UDPFlush();
}

/*****************************************************************************
  Function:
    static void DNSGetRXName(void)

  Summary:
    Copies a DNS hostname, possibly including name compression, to the buffer DnsName_buf[64]

  Description:
    None

  Precondition:

  Parameters:
    None

  Returns:
    None
 ***************************************************************************/
static void DNSGetRXName(void)
{
    uint16_t w;
    uint8_t i;
    uint8_t len;

    for (Size_HostName = 0; Size_HostName < BUF_SIZE_DNS_NAME; Size_HostName++) DnsName_buf[Size_HostName] = 0x00;
    Size_HostName = 0;

    while (1) {
        // Get first byte which will tell us if this is a 16-bit pointer or the
        // length of the first of a series of labels
        if (!UDPGet(&i))
            return;

        // Check if this is a pointer, if so, get the reminaing 8 bits and seek to the pointer value
        if ((i & 0xC0u) == 0xC0u) {
            ((uint8_t *) & w)[1] = i & 0x3F;
            UDPGet((uint8_t *) & w);
            //IPSetRxBuffer(sizeof(UDP_HEADER) + w);
            continue;
        }

        // Write the length byte
        len = i;
        UDPPut(len);

        // Exit if we've reached a zero length label
        if (len == 0u)
            return;

        // Copy all of the bytes in this label
        while (len--) {
            UDPGet(&i);
            if (Size_HostName < BUF_SIZE_DNS_NAME) DnsName_buf[Size_HostName++] = i;
        }
        if (Size_HostName < BUF_SIZE_DNS_NAME) DnsName_buf[Size_HostName++] = '.';
    }
}

/*****************************************************************************
  Function:
    static int DNSName_FindPointInBuffer(int start, int end)

  Summary:
    Look for "." in buffer, if find it ,return the position;

  Description:
    None

  Precondition:

  Parameters:
    int start,
    int end

  Returns:
    int
 ***************************************************************************/

static int DNSName_FindPointInBuffer(int start, int end)
{
    int i;
    for (i = start; i < end; i++) {
        if ('.' == DnsName_buf[i]) return i;
    }
    return end;
}

/*****************************************************************************
  Function:
    static void DNSName_valified(char *name,int name_size)

  Summary:
    Look for name in buffer, if find it ,return true;
    if cannot find, rreturn false

  Description:
    None

  Precondition:
    RX pointer is set to currently point to the DNS name to copy

  Parameters:
    char *name:     For example: ".local"
    int name_size:   name size

  Returns:
    bool
 ***************************************************************************/
static bool DNSName_valified(char *name, int name_size)
{
    int buf_size = Size_HostName - 1; //ignore the last character "."
    int pos_Start = buf_size - name_size;
    int i;
    for (i = 0; i < name_size; i++) {
        if (DnsName_buf[pos_Start + i] != name[i])
            return false;
    }
    return true;
}

/*****************************************************************************
  Function:
    static void DNSCopyRXNameToTX(void)

  Summary:
    Copies a DNS hostname, possibly including name compression, from the buffer
    to the TX packet (without name compression in TX case).

  Description:
    None

  Precondition:
    RX pointer is set to currently point to the DNS name to copy

  Parameters:
    None

  Returns:
    None
 ***************************************************************************/
static void DNSCopyRXNameToTX(void)
{
    int i;

    int i_begin = 0;
    int len;
    int i_pos = 0;
    while (i_begin < Size_HostName) {
        i_pos = DNSName_FindPointInBuffer(i_begin, Size_HostName);

        len = i_pos - i_begin;
        UDPPut(len);
        for (i = 0; i < len; i++) {
            UDPPut(DnsName_buf[i_begin]);
            i_begin++;
        }
        i_begin++; // ignore  '.'
    }
    UDPPut(0x00);
}

#endif // #if defined(STACK_USE_DNS_SERVER)
