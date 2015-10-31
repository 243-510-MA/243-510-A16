/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    nbns.c

  Summary:
    Module for Microchip TCP/IP Stack
     -Responds to NBNS name requests to allow human name assignment
      to the board.  i.e. allows nodes on the same IP subnet to use a
      hostname to access the board instead of an IP address.
     -Reference: RFC 1002

  Description:
    NetBIOS Name Service (NBNS) Server

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

#define __NBNS_C_

#include "tcpip/tcpip.h"

#if defined(STACK_USE_NBNS)

// NetBIOS Name Service port
#define NBNS_PORT  (137u)

// NBNS Header structure
typedef struct _NBNS_HEADER {
    TCPIP_UINT16_VAL TransactionID;
    TCPIP_UINT16_VAL Flags;
    TCPIP_UINT16_VAL Questions;
    TCPIP_UINT16_VAL Answers;
    TCPIP_UINT16_VAL AuthoritativeRecords;
    TCPIP_UINT16_VAL AdditionalRecords;
} NBNS_HEADER;

static enum {
    NBNS_HOME = 0,
    NBNS_OPEN_SOCKET,
    NBNS_LISTEN
} smNBNS;

static void NBNSPutName(uint8_t *String);
static void NBNSGetName(uint8_t *String);

extern NODE_INFO remoteNode;

/*********************************************************************
 * Function:        void NBNSInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Initialize the NBNS state machine
 *
 * Note:            None
 ********************************************************************/
void NBNSInit(void)
{
    smNBNS = NBNS_HOME;
}

/*********************************************************************
 * Function:        void NBNSTask(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Sends responses to NetBIOS name requests
 *
 * Note:            None
 ********************************************************************/
void NBNSTask(void)
{
    static UDP_SOCKET MySocket;
    uint8_t i;
    TCPIP_UINT16_VAL Type, Class;
    NBNS_HEADER NBNSHeader;
    uint8_t NameString[16];

    switch (smNBNS) {
    case NBNS_HOME:
        smNBNS++;
        break;

    case NBNS_OPEN_SOCKET:
        //MySocket = UDPOpen(NBNS_PORT, NULL, NBNS_PORT);

        MySocket = UDPOpenEx(0, UDP_OPEN_SERVER, NBNS_PORT, NBNS_PORT);
        if (MySocket == INVALID_UDP_SOCKET)
            break;

        smNBNS++;

    case NBNS_LISTEN:
        if (!UDPIsGetReady(MySocket))
            break;

        // Respond only to name requests sent to us from nodes on the same subnet
        // This prevents us from sending out the wrong IP address information if
        // we haven't gotten a DHCP lease yet.
        if ((remoteNode.IPAddr.Val & AppConfig.MyMask.Val) != (AppConfig.MyIPAddr.Val & AppConfig.MyMask.Val)) {
            UDPDiscard();
            break;
        }

        // Retrieve the NBNS header and de-big-endian it
        UDPGet(&NBNSHeader.TransactionID.v[1]);
        UDPGet(&NBNSHeader.TransactionID.v[0]);
        UDPGet(&NBNSHeader.Flags.v[1]);
        UDPGet(&NBNSHeader.Flags.v[0]);
        UDPGet(&NBNSHeader.Questions.v[1]);
        UDPGet(&NBNSHeader.Questions.v[0]);
        UDPGet(&NBNSHeader.Answers.v[1]);
        UDPGet(&NBNSHeader.Answers.v[0]);
        UDPGet(&NBNSHeader.AuthoritativeRecords.v[1]);
        UDPGet(&NBNSHeader.AuthoritativeRecords.v[0]);
        UDPGet(&NBNSHeader.AdditionalRecords.v[1]);
        UDPGet(&NBNSHeader.AdditionalRecords.v[0]);

        // Remove all questions
        while (NBNSHeader.Questions.Val--) {
            NBNSGetName(NameString);
            UDPGet(&i); // <??> Trailing character on string
            UDPGet(&Type.v[1]); // Question type
            UDPGet(&Type.v[0]);
            UDPGet(&Class.v[1]); // Question class
            UDPGet(&Class.v[0]);

            if (Type.Val == 0x0020u && Class.Val == 0x0001u && memcmp((void *) NameString, (void *) AppConfig.NetBIOSName, sizeof (AppConfig.NetBIOSName)) == 0) {
                while (!UDPIsPutReady(MySocket));

                NBNSHeader.Flags.Val = 0x8400;

                UDPPut(NBNSHeader.TransactionID.v[1]);
                UDPPut(NBNSHeader.TransactionID.v[0]);
                UDPPut(NBNSHeader.Flags.v[1]);
                UDPPut(NBNSHeader.Flags.v[0]);
                UDPPut(0x00); // 0x0000 Questions
                UDPPut(0x00);
                UDPPut(0x00); // 0x0001 Answers
                UDPPut(0x01);
                UDPPut(0x00); // 0x0000 Athoritative records
                UDPPut(0x00);
                UDPPut(0x00); // 0x0000 Additional records
                UDPPut(0x00);

                NBNSPutName(AppConfig.NetBIOSName);
                UDPPut(0x00); // 0x0020 Type: NetBIOS
                UDPPut(0x20);
                UDPPut(0x00); // 0x0001 Class: Internet
                UDPPut(0x01);
                UDPPut(0x00); // 0x00000000 Time To Live
                UDPPut(0x00);
                UDPPut(0x00);
                UDPPut(0x00);

                UDPPut(0x00); // 0x0006 Data length
                UDPPut(0x06);
                UDPPut(0x60); // 0x6000 Flags: H-node, Unique
                UDPPut(0x00);
                UDPPut(AppConfig.MyIPAddr.v[0]); // Put out IP address
                UDPPut(AppConfig.MyIPAddr.v[1]);
                UDPPut(AppConfig.MyIPAddr.v[2]);
                UDPPut(AppConfig.MyIPAddr.v[3]);

                // Change the destination address to the unicast address of the last received packet
                memcpy((void *) &UDPSocketInfo[MySocket].remote.remoteNode, (const void *) &remoteNode, sizeof (remoteNode));
                UDPFlush();
            }
        }

        UDPDiscard();

        break;
    }
}

/*********************************************************************
 * Function:        static void NBNSPutName (uint8_t *String)
 *
 * PreCondition:    None
 *
 * Input:           String: The name to transmit
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Transmits the NetBIOS name across an open UDP
 *                  socket.
 *
 * Note:            None
 ********************************************************************/
static void NBNSPutName(uint8_t *String)
{
    uint8_t i, j;

    UDPPut(32); // NetBIOS names are always 32 bytes long (16 decoded bytes)
    for (i = 0; i < 16u; i++) {
        j = *String++;
        UDPPut((j >> 4) + 'A');
        UDPPut((j & 0x0F) + 'A');
    }

    UDPPut(0x00);
}

/*********************************************************************
 * Function:        static void NBNSGetName (uint8_t *String)
 *
 * PreCondition:    None
 *
 * Input:           String: Pointer to an array into which
 *                  a received NetBIOS name should be copied.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Reads the NetBIOS name from a UDP socket and
 *                  copies it into a user-specified buffer.
 *
 * Note:            None
 ********************************************************************/
static void NBNSGetName(uint8_t *String)
{
    uint8_t i, j, k;

    if (String == NULL) {
        UDPGet(&i);
        while (i--) {
            UDPGet(&j);
        }
    } else {
        UDPGet(&i);
        if (i != 32u) {
            *String = 0;
            return;
        }
        while (i--) {
            UDPGet(&j);
            j -= 'A';
            k = j << 4;
            i--;
            UDPGet(&j);
            j -= 'A';
            *String++ = k | j;
        }
    }
}

#endif // #if defined(STACK_USE_NBNS)
