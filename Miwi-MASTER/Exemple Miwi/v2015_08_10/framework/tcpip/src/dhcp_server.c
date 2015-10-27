/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    dhcp_server.c

  Summary:
    Module for Microchip TCP/IP Stack
    -Provides automatic IP address, subnet mask, gateway address,
     DNS server address, and other configuration parameters on DHCP
     enabled networks.
    -Reference: RFC 2131, 2132

  Description:
    Dynamic Host Configuration Protocol (DHCP) Server

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

#define __DHCP_SERVER_C_

#include "tcpip/tcpip.h"

#if defined(STACK_USE_DHCP_SERVER)

// Duration of our DHCP Lease in seconds.  This is extremely short so
// the client won't use our IP for long if we inadvertently
// provide a lease on a network that has a more authoritative DHCP server.
#define DHCP_LEASE_DURATION  60*60ul // 1Hr
// Ignore: #define DHCP_MAX_LEASES  2 // Not implemented
#define MAX_DHCP_CLIENTS_NUMBER  6

typedef struct {
    MAC_ADDR ClientMAC;
    IP_ADDR Client_Addr;
    bool isUsed;
    uint32_t Client_Lease_Time;
} DHCP_IP_POOL;
DHCP_IP_POOL DhcpIpPool[MAX_DHCP_CLIENTS_NUMBER];

// DHCP Control Block.  Lease IP address is derived from index into DCB array.
typedef struct {
    uint32_t LeaseExpires; // Expiration time for this lease
    MAC_ADDR ClientMAC; // Client's MAC address.  Multicast bit is used to determine if a lease is given out or not

    enum {
        LEASE_UNUSED = 0,
        LEASE_REQUESTED,
        LEASE_GRANTED
    } smLease; // Status of this lease
} DHCP_CONTROL_BLOCK;

static UDP_SOCKET MySocket = INVALID_UDP_SOCKET; // Socket used by DHCP Server
static IP_ADDR DHCPNextLease; // IP Address to provide for next lease
bool bDHCPServerEnabled = true; // Whether or not the DHCP server is enabled

static void DHCPReplyToDiscovery(BOOTP_HEADER *Header);
static void DHCPReplyToRequest(BOOTP_HEADER *Header, bool bAccept, bool bRenew);

static void DHCPServerInit(void);

static bool isIpAddrInPool(IP_ADDR ipaddr);
static IP_ADDR GetIPAddrFromIndex_DhcpPool(uint8_t index);
static uint8_t preAssign_ToDHCPClient_FromPool(BOOTP_HEADER *Header);
static uint8_t postAssign_ToDHCPClient_FromPool(MAC_ADDR *macAddr, IP_ADDR *ipv4Addr);
static void renew_dhcps_Pool(void);
static bool Compare_MAC_addr(const MAC_ADDR *macAddr1, const MAC_ADDR *macAddr2);
static uint8_t getIndexByMacaddr_DhcpPool(const MAC_ADDR *MacAddr);
static bool isMacAddr_Effective(const MAC_ADDR *macAddr);

static enum {
    DHCPS_DISABLE,
    DHCPS_OPEN_SOCKET,
    DHCPS_LISTEN
} smDHCPServer = DHCPS_OPEN_SOCKET;

/*****************************************************************************
  Function:
    void DHCPServerTask(void)

  Summary:
    Performs periodic DHCP server tasks.

  Description:
    This function performs any periodic tasks requied by the DHCP server
    module, such as processing DHCP requests and distributing IP addresses.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None
 ***************************************************************************/
void DHCPServerTask(void)
{
    uint8_t i;
    uint8_t Option, Len;
    BOOTP_HEADER BOOTPHeader;
    uint32_t dw;
    bool bAccept, bRenew;

    // Init IP pool
    static bool flag_init = false;
    if (flag_init == false) {
        flag_init = true;
        DHCPServerInit();
    }

#if defined(STACK_USE_DHCP_CLIENT)
    // Make sure we don't clobber anyone else's DHCP server
    if (DHCPIsServerDetected(0))
        return;
#endif

    if (!bDHCPServerEnabled)
        return;

    renew_dhcps_Pool();
    switch (smDHCPServer) {
    case DHCPS_DISABLE:
        break;
    case DHCPS_OPEN_SOCKET:
        // Obtain a UDP socket to listen/transmit on
        //MySocket = UDPOpen(DHCP_SERVER_PORT, NULL, DHCP_CLIENT_PORT);
        MySocket = UDPOpenEx(0, UDP_OPEN_SERVER, DHCP_SERVER_PORT, DHCP_CLIENT_PORT);
        if (MySocket == INVALID_UDP_SOCKET)
            break;

        // Decide which address to lease out
        // Note that this needs to be changed if we are to
        // support more than one lease
        DHCPNextLease.Val = (AppConfig.MyIPAddr.Val & AppConfig.MyMask.Val) + 0x02000000;
        if (DHCPNextLease.v[3] == 255u)
            DHCPNextLease.v[3] += 0x03;
        if (DHCPNextLease.v[3] == 0u)
            DHCPNextLease.v[3] += 0x02;

        smDHCPServer++;

    case DHCPS_LISTEN:
        // Check to see if a valid DHCP packet has arrived
        if (UDPIsGetReady(MySocket) < 241u)
            break;

        // Retrieve the BOOTP header
        UDPGetArray((uint8_t *) & BOOTPHeader, sizeof (BOOTPHeader));

        if (true == isIpAddrInPool(BOOTPHeader.ClientIP)) {
            bRenew = true;
            bAccept = true;
        } else if (BOOTPHeader.ClientIP.Val == 0x00000000u) {
            bRenew = false;
            bAccept = true;
        } else {
            bRenew = false;
            bAccept = false;
        }
        //bAccept = (BOOTPHeader.ClientIP.Val == DHCPNextLease.Val) || (BOOTPHeader.ClientIP.Val == 0x00000000u);

        // Validate first three fields
        if (BOOTPHeader.MessageType != 1u)
            break;
        if (BOOTPHeader.HardwareType != 1u)
            break;
        if (BOOTPHeader.HardwareLen != 6u)
            break;

        // Throw away 10 unused bytes of hardware address,
        // server host name, and boot file name -- unsupported/not needed.
        for (i = 0; i < 64 + 128 + (16 - sizeof (MAC_ADDR)); i++)
            UDPGet(&Option);

        // Obtain Magic Cookie and verify
        UDPGetArray((uint8_t *) & dw, sizeof (uint32_t));
        if (dw != 0x63538263ul)
            break;

        // Obtain options
        while (1) {
            // Get option type
            if (!UDPGet(&Option))
                break;
            if (Option == DHCP_END_OPTION)
                break;

            // Get option length
            UDPGet(&Len);

            // Process option
            switch (Option) {
            case DHCP_MESSAGE_TYPE:
                UDPGet(&i);
                switch (i) {
                case DHCP_DISCOVER_MESSAGE:
                    DHCPReplyToDiscovery(&BOOTPHeader);
                    break;

                case DHCP_REQUEST_MESSAGE:
                    DHCPReplyToRequest(&BOOTPHeader, bAccept, bRenew);
                    break;

                // Need to handle these if supporting more than one DHCP lease
                case DHCP_RELEASE_MESSAGE:
                case DHCP_DECLINE_MESSAGE:
                    break;
                }
                break;

            case DHCP_PARAM_REQUEST_IP_ADDRESS:
                if (Len == 4u) {
                    IP_ADDR tmp_ip;
                    // Get the requested IP address and see if it is the one we have on offer.
                    UDPGetArray((uint8_t *) & dw, 4);
                    Len -= 4;
                    tmp_ip.Val = dw;
                    //bAccept = (dw == DHCPNextLease.Val);
                    if (true == isIpAddrInPool(tmp_ip)) {
                        bRenew = true;
                        bAccept = true;
                    } else if (tmp_ip.Val == 0x00000000u) {
                        bRenew = false;
                        bAccept = true;
                    } else {
                        bRenew = false;
                        bAccept = false;
                    }

                }
                break;

            case DHCP_END_OPTION:
                UDPDiscard();
                return;
            }

            // Remove any unprocessed bytes that we don't care about
            while (Len--) {
                UDPGet(&i);
            }
        }

        UDPDiscard();
        break;
    }
}

/*****************************************************************************
  Function:
    static void DHCPReplyToDiscovery(BOOTP_HEADER *Header)

  Summary:
    Replies to a DHCP Discover message.

  Description:
    This function replies to a DHCP Discover message by sending out a
    DHCP Offer message.

  Precondition:
    None

  Parameters:
    Header - the BootP header this is in response to.

  Returns:
    None
 ***************************************************************************/
static void DHCPReplyToDiscovery(BOOTP_HEADER *Header)
{
    uint8_t i;
    int8_t IndexOfPool;
    IP_ADDR ipAddr;
    // Set the correct socket to active and ensure that
    // enough space is available to generate the DHCP response
    if (UDPIsPutReady(MySocket) < 300u)
        return;

    // find in pool
    IndexOfPool = preAssign_ToDHCPClient_FromPool(Header);
    if (-1 == IndexOfPool) return;

    // Begin putting the BOOTP Header and DHCP options
    UDPPut(BOOT_REPLY); // Message Type: 2 (BOOTP Reply)
    // Reply with the same Hardware Type, Hardware Address Length, Hops, and Transaction ID fields
    UDPPutArray((uint8_t *)&(Header->HardwareType), 7);
    UDPPut(0x00); // Seconds Elapsed: 0 (Not used)
    UDPPut(0x00); // Seconds Elapsed: 0 (Not used)
    UDPPutArray((uint8_t *)&(Header->BootpFlags), sizeof (Header->BootpFlags));
    UDPPut(0x00); // Your (client) IP Address: 0.0.0.0 (none yet assigned)
    UDPPut(0x00); // Your (client) IP Address: 0.0.0.0 (none yet assigned)
    UDPPut(0x00); // Your (client) IP Address: 0.0.0.0 (none yet assigned)
    UDPPut(0x00); // Your (client) IP Address: 0.0.0.0 (none yet assigned)
    //UDPPutArray((uint8_t *)&DHCPNextLease, sizeof(IP_ADDR)); // Lease IP address to give out
    ipAddr = GetIPAddrFromIndex_DhcpPool(IndexOfPool);
    UDPPutArray((uint8_t *) & ipAddr, sizeof (IP_ADDR)); // Lease IP address to give out
    UDPPut(0x00); // Next Server IP Address: 0.0.0.0 (not used)
    UDPPut(0x00); // Next Server IP Address: 0.0.0.0 (not used)
    UDPPut(0x00); // Next Server IP Address: 0.0.0.0 (not used)
    UDPPut(0x00); // Next Server IP Address: 0.0.0.0 (not used)
    UDPPut(0x00); // Relay Agent IP Address: 0.0.0.0 (not used)
    UDPPut(0x00); // Relay Agent IP Address: 0.0.0.0 (not used)
    UDPPut(0x00); // Relay Agent IP Address: 0.0.0.0 (not used)
    UDPPut(0x00); // Relay Agent IP Address: 0.0.0.0 (not used)
    UDPPutArray((uint8_t *)&(Header->ClientMAC), sizeof (MAC_ADDR)); // Client MAC address: Same as given by client
    for (i = 0; i < 64 + 128 + (16 - sizeof (MAC_ADDR)); i++) // Remaining 10 bytes of client hardware address, server host name: Null string (not used)
        UDPPut(0x00); // Boot filename: Null string (not used)
    UDPPut(0x63); // Magic Cookie: 0x63538263
    UDPPut(0x82); // Magic Cookie: 0x63538263
    UDPPut(0x53); // Magic Cookie: 0x63538263
    UDPPut(0x63); // Magic Cookie: 0x63538263

    // Options: DHCP Offer
    UDPPut(DHCP_MESSAGE_TYPE);
    UDPPut(1);
    UDPPut(DHCP_OFFER_MESSAGE);

    // Option: Subnet Mask
    UDPPut(DHCP_SUBNET_MASK);
    UDPPut(sizeof (IP_ADDR));
    UDPPutArray((uint8_t *) & AppConfig.MyMask, sizeof (IP_ADDR));

    // Option: Lease duration
    UDPPut(DHCP_IP_LEASE_TIME);
    UDPPut(4);
    UDPPut((DHCP_LEASE_DURATION >> 24) & 0xFF);
    UDPPut((DHCP_LEASE_DURATION >> 16) & 0xFF);
    UDPPut((DHCP_LEASE_DURATION >> 8) & 0xFF);
    UDPPut((DHCP_LEASE_DURATION) & 0xFF);

    // Option: Server identifier
    UDPPut(DHCP_SERVER_IDENTIFIER);
    UDPPut(sizeof (IP_ADDR));
    UDPPutArray((uint8_t *) & AppConfig.MyIPAddr, sizeof (IP_ADDR));

    // Option: Router/Gateway address
    UDPPut(DHCP_ROUTER);
    UDPPut(sizeof (IP_ADDR));
    UDPPutArray((uint8_t *) & AppConfig.MyIPAddr, sizeof (IP_ADDR));

    // Option: DNS server address
    UDPPut(DHCP_DNS);
    UDPPut(sizeof (IP_ADDR));
    UDPPutArray((uint8_t *) & AppConfig.MyIPAddr, sizeof (IP_ADDR));

    // No more options, mark ending
    UDPPut(DHCP_END_OPTION);

    // Add zero padding to ensure compatibility with old BOOTP relays that discard small packets (<300 UDP octets)
    while (UDPTxCount < 300u)
        UDPPut(0);

    // Force remote destination address to be the broadcast address, regardless
    // of what the node's source IP address was (to ensure we don't try to
    // unicast to 0.0.0.0).
    memset((void *) &UDPSocketInfo[MySocket].remote.remoteNode, 0xFF, sizeof (NODE_INFO));

    // Transmit the packet
    UDPFlush();
}

/*****************************************************************************
  Function:
    static void DHCPReplyToRequest(BOOTP_HEADER *Header, bool bAccept)

  Summary:
    Replies to a DHCP Request message.

  Description:
    This function replies to a DHCP Request message by sending out a
    DHCP Acknowledge message.

  Precondition:
    None

  Parameters:
    Header - the BootP header this is in response to.
    bAccept - whether or not we've accepted this request

  Returns:
    None

  Internal:
    Needs to support more than one simultaneous lease in the future.
 ***************************************************************************/
static void DHCPReplyToRequest(BOOTP_HEADER *Header, bool bAccept, bool bRenew)
{
    uint8_t i;
    int8_t indexOfPool = 255;
    IP_ADDR ipAddr;
    // Set the correct socket to active and ensure that
    // enough space is available to generate the DHCP response
    if (UDPIsPutReady(MySocket) < 300u)
        return;

    // Search through all remaining options and look for the Requested IP address field
    // Obtain options

    while (UDPIsGetReady(MySocket)) {
        uint8_t Option, Len;
        uint32_t dw;
        MAC_ADDR tmp_MacAddr;

        // Get option type
        if (!UDPGet(&Option))
            break;
        if (Option == DHCP_END_OPTION)
            break;

        // Get option length
        UDPGet(&Len);
        if (bRenew) {
            if ((Option == DHCP_PARAM_REQUEST_CLIENT_ID) && (Len == 7u)) {
                // Get the requested IP address and see if it is the one we have on offer.  If not, we should send back a NAK, but since there could be some other DHCP server offering this address, we'll just silently ignore this request.
                UDPGet(&i);
                UDPGetArray((uint8_t *) & tmp_MacAddr, 6);
                Len -= 7;
                indexOfPool = getIndexByMacaddr_DhcpPool(&tmp_MacAddr); //(&tmp_MacAddr,(IPV4_ADDR*)&Header->);
                if (-1 != indexOfPool) {
                    if (GetIPAddrFromIndex_DhcpPool(indexOfPool).Val == Header->ClientIP.Val)
                        postAssign_ToDHCPClient_FromPool(&tmp_MacAddr, &(Header->ClientIP));
                    else
                        bAccept = false;
                } else {
                    bAccept = false;
                }

                break;
            }
        } else {
            if ((Option == DHCP_PARAM_REQUEST_IP_ADDRESS) && (Len == 4u)) {
                // Get the requested IP address and see if it is the one we have on offer.  If not, we should send back a NAK, but since there could be some other DHCP server offering this address, we'll just silently ignore this request.
                UDPGetArray((uint8_t *) & dw, 4);
                Len -= 4;
                indexOfPool = postAssign_ToDHCPClient_FromPool(&(Header->ClientMAC), (IP_ADDR*) & dw);
                if (-1 == indexOfPool) {
                    bAccept = false;
                }
                break;
            }
        }

        // Remove the unprocessed bytes that we don't care about
        while (Len--) {
            UDPGet(&i);
        }
    }

    // Begin putting the BOOTP Header and DHCP options
    UDPPut(BOOT_REPLY); // Message Type: 2 (BOOTP Reply)
    // Reply with the same Hardware Type, Hardware Address Length, Hops, and Transaction ID fields
    UDPPutArray((uint8_t *)&(Header->HardwareType), 7);
    UDPPut(0x00); // Seconds Elapsed: 0 (Not used)
    UDPPut(0x00); // Seconds Elapsed: 0 (Not used)
    UDPPutArray((uint8_t *)&(Header->BootpFlags), sizeof (Header->BootpFlags));
    UDPPutArray((uint8_t *)&(Header->ClientIP), sizeof (IP_ADDR)); // Your (client) IP Address:
    //UDPPutArray((uint8_t *)&DHCPNextLease, sizeof(IP_ADDR)); // Lease IP address to give out
    if (bAccept) ipAddr = GetIPAddrFromIndex_DhcpPool(indexOfPool);
    else ipAddr.Val = 0u;
    UDPPutArray((uint8_t *) & ipAddr, sizeof (IP_ADDR)); // Lease IP address to give out
    UDPPut(0x00); // Next Server IP Address: 0.0.0.0 (not used)
    UDPPut(0x00); // Next Server IP Address: 0.0.0.0 (not used)
    UDPPut(0x00); // Next Server IP Address: 0.0.0.0 (not used)
    UDPPut(0x00); // Next Server IP Address: 0.0.0.0 (not used)
    UDPPut(0x00); // Relay Agent IP Address: 0.0.0.0 (not used)
    UDPPut(0x00); // Relay Agent IP Address: 0.0.0.0 (not used)
    UDPPut(0x00); // Relay Agent IP Address: 0.0.0.0 (not used)
    UDPPut(0x00); // Relay Agent IP Address: 0.0.0.0 (not used)
    UDPPutArray((uint8_t *)&(Header->ClientMAC), sizeof (MAC_ADDR)); // Client MAC address: Same as given by client
    for (i = 0; i < 64 + 128 + (16 - sizeof (MAC_ADDR)); i++) // Remaining 10 bytes of client hardware address, server host name: Null string (not used)
        UDPPut(0x00); // Boot filename: Null string (not used)
    UDPPut(0x63); // Magic Cookie: 0x63538263
    UDPPut(0x82); // Magic Cookie: 0x63538263
    UDPPut(0x53); // Magic Cookie: 0x63538263
    UDPPut(0x63); // Magic Cookie: 0x63538263

    // Options: DHCP lease ACKnowledge
    if (bAccept) {
        UDPPut(DHCP_OPTION_ACK_MESSAGE);
        UDPPut(1);
        UDPPut(DHCP_ACK_MESSAGE);
    } else // Send a NACK
    {
        UDPPut(DHCP_OPTION_ACK_MESSAGE);
        UDPPut(1);
        UDPPut(DHCP_NAK_MESSAGE);
    }

    // Option: Lease duration
    UDPPut(DHCP_IP_LEASE_TIME);
    UDPPut(4);
    UDPPut((DHCP_LEASE_DURATION >> 24) & 0xFF);
    UDPPut((DHCP_LEASE_DURATION >> 16) & 0xFF);
    UDPPut((DHCP_LEASE_DURATION >> 8) & 0xFF);
    UDPPut((DHCP_LEASE_DURATION) & 0xFF);

    // Option: Server identifier
    UDPPut(DHCP_SERVER_IDENTIFIER);
    UDPPut(sizeof (IP_ADDR));
    UDPPutArray((uint8_t *) & AppConfig.MyIPAddr, sizeof (IP_ADDR));

    // Option: Subnet Mask
    UDPPut(DHCP_SUBNET_MASK);
    UDPPut(sizeof (IP_ADDR));
    UDPPutArray((uint8_t *) & AppConfig.MyMask, sizeof (IP_ADDR));

    // Option: Router/Gateway address
    UDPPut(DHCP_ROUTER);
    UDPPut(sizeof (IP_ADDR));
    UDPPutArray((uint8_t *) & AppConfig.MyIPAddr, sizeof (IP_ADDR));

    // Option: DNS server address
    UDPPut(DHCP_DNS);
    UDPPut(sizeof (IP_ADDR));
    UDPPutArray((uint8_t *) & AppConfig.MyIPAddr, sizeof (IP_ADDR));

    // No more options, mark ending
    UDPPut(DHCP_END_OPTION);

    // Add zero padding to ensure compatibility with old BOOTP relays that discard small packets (<300 UDP octets)
    while (UDPTxCount < 300u)
        UDPPut(0);

    // Force remote destination address to be the broadcast address, regardless
    // of what the node's source IP address was (to ensure we don't try to
    // unicast to 0.0.0.0).
    memset((void *) &UDPSocketInfo[MySocket].remote.remoteNode, 0xFF, sizeof (NODE_INFO));

    // Transmit the packet
    UDPFlush();
}

static void DHCPServerInit(void)
{
    int i;
    //init ip pool
    for (i = 0; i < MAX_DHCP_CLIENTS_NUMBER; i++) {
        DhcpIpPool[i].isUsed = false;
        DhcpIpPool[i].Client_Lease_Time = 0; // 1 hour

#if defined(WF_CS_TRIS)
#if  (MY_DEFAULT_NETWORK_TYPE == WF_SOFT_AP  )
        {
            DhcpIpPool[i].Client_Addr.v[0] = 192;
            DhcpIpPool[i].Client_Addr.v[1] = 168;
        }
#elif (MY_DEFAULT_NETWORK_TYPE == WF_ADHOC)
        {
            DhcpIpPool[i].Client_Addr.v[0] = MY_DEFAULT_IP_ADDR_BYTE1; // default - 169
            DhcpIpPool[i].Client_Addr.v[1] = MY_DEFAULT_IP_ADDR_BYTE2; // default - 254

        }
#elif (MY_DEFAULT_NETWORK_TYPE == WF_INFRASTRUCTURE)
        {
            DhcpIpPool[i].Client_Addr.v[0] = MY_DEFAULT_IP_ADDR_BYTE1; // default - 169
            DhcpIpPool[i].Client_Addr.v[1] = MY_DEFAULT_IP_ADDR_BYTE2; // default - 254
        }
#else
        {
            DhcpIpPool[i].Client_Addr.v[0] = MY_DEFAULT_IP_ADDR_BYTE1; // default - 169
            DhcpIpPool[i].Client_Addr.v[1] = MY_DEFAULT_IP_ADDR_BYTE2; // default - 254
        }
#endif
#else
        DhcpIpPool[i].Client_Addr.v[0] = MY_DEFAULT_IP_ADDR_BYTE1; // default - 169
        DhcpIpPool[i].Client_Addr.v[1] = MY_DEFAULT_IP_ADDR_BYTE2; // default - 254
#endif

        DhcpIpPool[i].Client_Addr.v[2] = 0;
        DhcpIpPool[i].Client_Addr.v[3] = 100 + i;
        DhcpIpPool[i].ClientMAC.v[0] = 0;
        DhcpIpPool[i].ClientMAC.v[1] = 0;
        DhcpIpPool[i].ClientMAC.v[2] = 0;
        DhcpIpPool[i].ClientMAC.v[3] = 0;
        DhcpIpPool[i].ClientMAC.v[4] = 0;
        DhcpIpPool[i].ClientMAC.v[5] = 0;
    }

}

static void renew_dhcps_Pool(void)
{
    static uint32_t dhcp_timer = 0;
    uint32_t current_timer = TickGet();
    int i;
    if ((current_timer - dhcp_timer) < 1 * TICK_SECOND) {
        return;
    }
    dhcp_timer = current_timer;
    for (i = 0; i < MAX_DHCP_CLIENTS_NUMBER; i++) {
        if (DhcpIpPool[i].isUsed == false) continue;

        if (DhcpIpPool[i].Client_Lease_Time != 0) DhcpIpPool[i].Client_Lease_Time--;
        if (DhcpIpPool[i].Client_Lease_Time == 0) {
            DhcpIpPool[i].isUsed = false;
            DhcpIpPool[i].ClientMAC.v[0] = 00;
            DhcpIpPool[i].ClientMAC.v[1] = 00;
            DhcpIpPool[i].ClientMAC.v[2] = 00;
            DhcpIpPool[i].ClientMAC.v[3] = 00;
            DhcpIpPool[i].ClientMAC.v[4] = 00;
            DhcpIpPool[i].ClientMAC.v[5] = 00;
        }
    }
}

static uint8_t getIndexByMacaddr_DhcpPool(const MAC_ADDR *MacAddr)
{
    int i;
    if (false == isMacAddr_Effective(MacAddr)) return -1;
    for (i = 0; i < MAX_DHCP_CLIENTS_NUMBER; i++) {
        if (true == Compare_MAC_addr(&DhcpIpPool[i].ClientMAC, MacAddr)) return i;
    }
    return -1;
}

static bool isIpAddrInPool(IP_ADDR ipaddr)
{
    int i;
    for (i = 0; i < MAX_DHCP_CLIENTS_NUMBER; i++) {
        if (DhcpIpPool[i].Client_Addr.Val == ipaddr.Val) {
            return true;
        }
    }
    return false;
}

static IP_ADDR GetIPAddrFromIndex_DhcpPool(uint8_t index)
{
    IP_ADDR tmpIpAddr;
    tmpIpAddr.Val = 0u;
    if (index > MAX_DHCP_CLIENTS_NUMBER) return tmpIpAddr;
    return DhcpIpPool[index].Client_Addr;
}

static bool Compare_MAC_addr(const MAC_ADDR *macAddr1, const MAC_ADDR *macAddr2)
{
    int i;
    for (i = 0; i < 6; i++) {
        if (macAddr1->v[i] != macAddr2->v[i]) return false;
    }
    return true;
}

static bool isMacAddr_Effective(const MAC_ADDR *macAddr)
{
    int i;
    for (i = 0; i < 6; i++) {
        if (macAddr->v[i] != 0) return true;
    }
    return false;
}

static uint8_t preAssign_ToDHCPClient_FromPool(BOOTP_HEADER *Header)
{
    int i;
    // if MAC==00:00:00:00:00:00, then return -1
    if (false == isMacAddr_Effective(&(Header->ClientMAC))) return -1;
    // Find in Pool, look for the same MAC addr
    for (i = 0; i < MAX_DHCP_CLIENTS_NUMBER; i++) {
        if (true == Compare_MAC_addr(&DhcpIpPool[i].ClientMAC, &Header->ClientMAC)) {
            //if(true == DhcpIpPool[i].isUsed) return -1;
            //DhcpIpPool[i].isUsed = true;
            return i;
        }
    }
    // Find in pool, look for a empty MAC addr
    for (i = 0; i < MAX_DHCP_CLIENTS_NUMBER; i++) {
        if (false == isMacAddr_Effective(&DhcpIpPool[i].ClientMAC)) { // this is empty MAC in pool
            int j;
            for (j = 0; j < 6; j++) DhcpIpPool[i].ClientMAC.v[j] = Header->ClientMAC.v[j];
            //DhcpIpPool[i].isUsed = true;
            return i;
        }
    }
#if 1
    // Find in pool, look for a unsued item
    for (i = 0; i < MAX_DHCP_CLIENTS_NUMBER; i++) {
        if (false == DhcpIpPool[i].isUsed) { // this is unused MAC in pool
            int j;
            for (j = 0; j < 6; j++) DhcpIpPool[i].ClientMAC.v[j] = Header->ClientMAC.v[j];
            DhcpIpPool[i].isUsed = true;
            return i;
        }
    }
#endif
    return -1;
}

static uint8_t postAssign_ToDHCPClient_FromPool(MAC_ADDR *macAddr, IP_ADDR *ipv4Addr)
{
    int i;
    for (i = 0; i < MAX_DHCP_CLIENTS_NUMBER; i++) {
        if (ipv4Addr->Val == DhcpIpPool[i].Client_Addr.Val) {
            if (true == Compare_MAC_addr(macAddr, &DhcpIpPool[i].ClientMAC)) {
                DhcpIpPool[i].isUsed = true;
                DhcpIpPool[i].Client_Lease_Time = DHCP_LEASE_DURATION;
                return i;
            } else
                return -1;
        }
    }
    return -1;
}

void DHCPServer_Disable(void)
{
    smDHCPServer = DHCPS_DISABLE;

    if (MySocket != INVALID_UDP_SOCKET) {
        UDPClose(MySocket);
        MySocket = INVALID_UDP_SOCKET;
    }
}

void DHCPServer_Enable(void)
{
    if (smDHCPServer == DHCPS_DISABLE) {
        smDHCPServer = DHCPS_OPEN_SOCKET;
    }
}

void DHCPServer_ClientRemove(uint8_t *Address)
{
    int i;
    MAC_ADDR macAddr;

    macAddr.v[0] = Address[0];
    macAddr.v[1] = Address[1];
    macAddr.v[2] = Address[2];
    macAddr.v[3] = Address[3];
    macAddr.v[4] = Address[4];
    macAddr.v[5] = Address[5];

    for (i = 0; i < MAX_DHCP_CLIENTS_NUMBER; i++)
    {
        if (Compare_MAC_addr(&macAddr, &DhcpIpPool[i].ClientMAC))
        {
            DhcpIpPool[i].ClientMAC.v[0] = 00;
            DhcpIpPool[i].ClientMAC.v[1] = 00;
            DhcpIpPool[i].ClientMAC.v[2] = 00;
            DhcpIpPool[i].ClientMAC.v[3] = 00;
            DhcpIpPool[i].ClientMAC.v[4] = 00;
            DhcpIpPool[i].ClientMAC.v[5] = 00;
            DhcpIpPool[i].Client_Addr.v[0] = 0;
            DhcpIpPool[i].Client_Addr.v[1] = 0;
            DhcpIpPool[i].Client_Addr.v[2] = 0;
            DhcpIpPool[i].Client_Addr.v[3] = 0;
            DhcpIpPool[i].isUsed = false;
            DhcpIpPool[i].Client_Lease_Time = 0;
        }
    }
}

#endif /* #if defined(STACK_USE_DHCP_SERVER) */
