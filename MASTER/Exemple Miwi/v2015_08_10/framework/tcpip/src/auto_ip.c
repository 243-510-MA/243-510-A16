/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    auto_ip.c

  Summary:
    Module for Microchip TCP/IP Stack
    -Provides AutoIP Code for automatically allocating a link-layer
     address
    -Reference: RFC 3927

  Description:
    AutoIP Client

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

#define __AUTO_IP_C_

#include "tcpip/tcpip.h"

#if defined (STACK_USE_AUTO_IP)

// Set of variables for each network interface
typedef struct {
    // Global state-tracker variable
    SM_AUTOIP smAUTOIPState;
    // Global backoff-time counter
    uint32_t gAutoIPConflictTimer;
    // Records the last time at which an AutoIP event occured
    uint32_t eventTime;
    // An ARP packet
    ARP_PACKET packet;
    // Holds the number of ticks needed for a random delay
    uint32_t randomDelay;
    // The seed value for the RNG
    uint32_t wRandSeed;

    union {
        struct {
            unsigned char gDisableAutoIP : 1; // Prevents AutoIP from initializing if the user wants a static address
            unsigned char bConfigureAutoIP : 1;
            unsigned char bLastLinkState : 1;
            unsigned char checkAddress : 1;
            unsigned char bCurrentLinkState : 1;
#if defined (STACK_USE_DHCP_CLIENT)
            unsigned char bLastDHCPState : 1;
#endif
        } bits;
        uint8_t val;
    } flags;

    uint8_t conflicts;
} AUTOIP_CLIENT_VARS;

// Prototypes
void AutoIPRandSeed(uint32_t seed, uint8_t vInterface);
uint32_t AutoIPRand(uint8_t vInterface);

/*****************************************************************************
  Function:
    static void LoadState(uint8_t vInterface)

  Summary:
    Saves the AutoIP client state information structure to the appropriate
    location and loads AutoIP client with the state information for the specified
    interface.

  Description:
    Saves the AutoIP client state information structure to the appropriate
    location and loads AutoIP client with the state information for the specified
    interface.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    This function does nothing when you only have one physical interface.
 ***************************************************************************/
#if NETWORK_INTERFACES > 1

static AUTOIP_CLIENT_VARS AutoIPClients[NETWORK_INTERFACES];
static AUTOIP_CLIENT_VARS *SelectedAutoIPClient;
#define AutoIPClient  (*SelectedAutoIPClient)
#define LoadState(v)  do(SelectedAutoIPClient = &AutoIPClients[v])while(0)

#else

static AUTOIP_CLIENT_VARS AutoIPClient;
#define LoadState(v)

#endif

/*****************************************************************************
  Function:
    void AutoIPInit(uint8_t vInterface)

  Summary:
    Resets the AutoIP client module for the specified interface.

  Description:
    Resets the AutoIP client module

  Precondition:
    None

  Parameters:
    vInterface - Interface number to initialize AutoIP client state variables
        for.   If you only have one interface, specify 0x00.

  Returns:
    None

  Remarks:
    This function may be called multiple times throughout the life of the
    application, if desired.
 ***************************************************************************/
void AutoIPInit(uint8_t vInterface)
{
    LoadState(vInterface);

    AutoIPClient.flags.bits.bConfigureAutoIP = false;
    AutoIPClient.smAUTOIPState = SM_AUTOIP_DISABLED;
    AutoIPClient.gAutoIPConflictTimer = 0;
    AutoIPClient.flags.bits.bLastLinkState = false;
    AutoIPClient.flags.bits.checkAddress = false;
    AutoIPClient.conflicts = 0;
#if defined (STACK_USE_DHCP_CLIENT)
    AutoIPClient.flags.bits.bLastDHCPState = false;
#endif
}

/*****************************************************************************
  Function:
    bool AutoIPIsConfigured (uint8_t vInterface)

  Summary:
    Determines if the AutoIP interface has successfully claimed an IP address

  Description:
    Determines if the AutoIP interface has successfully claimed an IP address

  Precondition:
    None

  Parameters:
    vInterface - Interface number to check AutoIP configuration for. If you
        only have one interface, specify 0x00.

  Returns:
    true - The AutoIP client has successfully claimed an IP address
    false - The AutoIP client has not claimed an IP address

  Remarks:
    None
 ***************************************************************************/
bool AutoIPIsConfigured(uint8_t vInterface)
{
    LoadState(vInterface);
    return (AutoIPClient.smAUTOIPState == SM_AUTOIP_CONFIGURED) ? true : false;
}

/*****************************************************************************
  Function:
    bool AutoIPConfigIsInProgress (uint8_t vInterface)

  Summary:
    Determines if the AutoIP address is being configured

  Description:
    Determines if the AutoIP address is being configured

  Precondition:
    None

  Parameters:
    vInterface - Interface number to check AutoIP configuration for. If you
        only have one interface, specify 0x00.

  Returns:
    true - The AutoIP client is being configured
    false - The AutoIP client is not in configuration mode

  Remarks:
    None
 ***************************************************************************/
bool AutoIPConfigIsInProgress(uint8_t vInterface)
{
    LoadState(vInterface);
    return AutoIPClient.flags.bits.bConfigureAutoIP;
}

/*****************************************************************************
  Function:
    void AutoIPTasks(void)

  Summary:
    Performs periodic AutoIP tasks.

  Description:
    This function performs any periodic tasks requied by the AutoIP module,
    such as re/transmitting gratuitous ARP packets and defending addresses

  Precondition:
    None

  Parameters:
    None

  Returns:
    None
 ***************************************************************************/
void AutoIPTasks(void)
{
    uint8_t i;

    for (i = 0; i < NETWORK_INTERFACES; i++) {
        LoadState(i);
        AutoIPClient.flags.bits.bCurrentLinkState = MACIsLinked();
        if (AutoIPClient.flags.bits.bCurrentLinkState != AutoIPClient.flags.bits.bLastLinkState) {
            AutoIPClient.flags.bits.bLastLinkState = AutoIPClient.flags.bits.bCurrentLinkState;
            if (!AutoIPClient.flags.bits.bCurrentLinkState) {
                AutoIPClient.flags.bits.bConfigureAutoIP = false;
                AutoIPClient.smAUTOIPState = SM_AUTOIP_DISABLED;
                AppConfig.MyIPAddr.Val = AppConfig.DefaultIPAddr.Val;
                AppConfig.MyMask.Val = AppConfig.DefaultMask.Val;
            } else {
                AutoIPClient.smAUTOIPState = SM_AUTOIP_INIT_RNG;
            }
        }

#if defined (STACK_USE_DHCP_CLIENT)
        if (DHCPIsBound(i)) {
            AutoIPClient.flags.bits.bConfigureAutoIP = false;
            AutoIPClient.smAUTOIPState = SM_AUTOIP_DISABLED;
            AutoIPClient.flags.bits.bLastDHCPState = true;
        } else {
            if (AutoIPClient.flags.bits.bLastDHCPState == true) {
                if (AutoIPClient.flags.bits.bCurrentLinkState)
                    AutoIPClient.smAUTOIPState = SM_AUTOIP_INIT_RNG;
            }
            AutoIPClient.flags.bits.bLastDHCPState = false;
        }
#endif

        if (AutoIPClient.flags.bits.gDisableAutoIP == true) {
            AutoIPClient.flags.bits.bConfigureAutoIP = false;
            AutoIPClient.smAUTOIPState = SM_AUTOIP_DISABLED;
        }

        switch (AutoIPClient.smAUTOIPState) {
        // Default no-AutoIP case
        case SM_AUTOIP_DISABLED:

            break;

        // Initializes the random number generator with a seed based on the MAC address
        case SM_AUTOIP_INIT_RNG:
            AutoIPRandSeed(((uint32_t) AppConfig.MyMACAddr.v[0] + ((uint32_t) AppConfig.MyMACAddr.v[1] << 8) + \
                        ((uint32_t) AppConfig.MyMACAddr.v[2] << 16) + ((uint32_t) AppConfig.MyMACAddr.v[3] << 24) + \
                        ((uint32_t) AppConfig.MyMACAddr.v[4]) + ((uint32_t) AppConfig.MyMACAddr.v[5] << 8)), i);

            AutoIPClient.smAUTOIPState = SM_AUTOIP_CHECK_ADDRESS;

        // Check the address to see if it's in use before we write it into AppConfig
        case SM_AUTOIP_CHECK_ADDRESS:

            if (AutoIPClient.flags.bits.checkAddress == false) {
                AutoIPClient.flags.bits.checkAddress = true;

                AppConfig.MyMask.Val = 0x00000000;

                // Generate a random IP address (based on the MAC address) to try and claim.
                // Dynamic link-local addresses can fall within the range:
                // 169.254.1.0 - 169.254.254.255
                AutoIPClient.packet.TargetIPAddr.byte.MB = AutoIPRand(i) % 256;
                AutoIPClient.packet.TargetIPAddr.byte.UB = (AutoIPRand(i) % 254) + 1;
                AutoIPClient.packet.TargetIPAddr.word.LW = 0xFEA9;

                ARPResolve(&AutoIPClient.packet.TargetIPAddr);

                AutoIPClient.eventTime = TickGet();
            }

            if (!ARPIsResolved(&AutoIPClient.packet.TargetIPAddr, &AutoIPClient.packet.TargetMACAddr)) {
                if (TickGet() - AutoIPClient.eventTime > TICK_SECOND) {
                    AutoIPClient.smAUTOIPState = SM_AUTOIP_SETUP_MESSAGE;
                }
            } else {
                AutoIPClient.flags.bits.checkAddress = false;
            }

            break;

        // Set up an ARP packet
        case SM_AUTOIP_SETUP_MESSAGE:

            AutoIPClient.flags.bits.checkAddress = false;

            // Set the bConfigureAutoIP flag- This flag will cause an AutoIP conflict
            // if a response packet is received from the address we're trying to claim.
            AutoIPClient.flags.bits.bConfigureAutoIP = true;

            // Configure the fields for a gratuitous ARP packet
            AutoIPClient.packet.Operation = ARP_OPERATION_REQ;

            AutoIPClient.packet.TargetMACAddr.v[0] = 0xff;
            AutoIPClient.packet.TargetMACAddr.v[1] = 0xff;
            AutoIPClient.packet.TargetMACAddr.v[2] = 0xff;
            AutoIPClient.packet.TargetMACAddr.v[3] = 0xff;
            AutoIPClient.packet.TargetMACAddr.v[4] = 0xff;
            AutoIPClient.packet.TargetMACAddr.v[5] = 0xff;

            AppConfig.MyIPAddr = AutoIPClient.packet.TargetIPAddr;
            AppConfig.MyMask.Val = 0x0000FFFF;
            memcpy(&AutoIPClient.packet.SenderMACAddr, (void *) &AppConfig.MyMACAddr, sizeof (AutoIPClient.packet.SenderMACAddr));
            AutoIPClient.packet.HardwareType = HW_ETHERNET;
            AutoIPClient.packet.Protocol = ARP_IP;
            AutoIPClient.packet.MACAddrLen = sizeof (MAC_ADDR);
            AutoIPClient.packet.ProtocolLen = sizeof (IP_ADDR);
            AutoIPClient.packet.SenderIPAddr.Val = AutoIPClient.packet.TargetIPAddr.Val;

            SwapARPPacket(&AutoIPClient.packet);

            // Generate a random delay between 0 and 1 second
            AutoIPClient.randomDelay = ((LFSRRand() % 20) * TICK_SECOND) / 20;
            // Store the current time
            AutoIPClient.eventTime = TickGet();

            // Set the state to send the ARP packet
            AutoIPClient.smAUTOIPState = SM_AUTOIP_GRATUITOUS_ARP1;

            break;

        // Send a gratuitous ARP packet to try and claim our address
        case SM_AUTOIP_GRATUITOUS_ARP1:
        case SM_AUTOIP_GRATUITOUS_ARP2:
        case SM_AUTOIP_GRATUITOUS_ARP3:
            // Check to ensure we've passed the delay time
            if (TickGet() - AutoIPClient.eventTime > AutoIPClient.randomDelay) {
                // Store the new event time
                AutoIPClient.eventTime = TickGet();
                // Generate a new random delay between 1 and 2 seconds
                AutoIPClient.randomDelay = TICK_SECOND + (((LFSRRand() % 20) * TICK_SECOND) / 20);

                // Transmit the packet
                while (!MACIsTxReady());
                MACSetWritePtr(BASE_TX_ADDR);

                MACPutHeader(&AutoIPClient.packet.TargetMACAddr, MAC_ARP, sizeof (AutoIPClient.packet));
                MACPutArray((uint8_t *) & AutoIPClient.packet, sizeof (AutoIPClient.packet));
                MACFlush();

                // Increment the probe iteration or increment to the delay state
                AutoIPClient.smAUTOIPState++;
            }
            break;

        // Delay for 1-2 seconds after sending the third ARP request before
        // entering the configured state
        case SM_AUTOIP_DELAY:
            if (TickGet() - AutoIPClient.eventTime > AutoIPClient.randomDelay)
                AutoIPClient.smAUTOIPState = SM_AUTOIP_CONFIGURED;
            break;

        // Configure the module to limit the rate at which packets are sent
        case SM_AUTOIP_RATE_LIMIT_SET:
            AutoIPClient.eventTime = TickGet();
            AppConfig.MyIPAddr.v[0] = MY_DEFAULT_IP_ADDR_BYTE1;
            AppConfig.MyIPAddr.v[1] = MY_DEFAULT_IP_ADDR_BYTE2;
            AppConfig.MyIPAddr.v[2] = MY_DEFAULT_IP_ADDR_BYTE3;
            AppConfig.MyIPAddr.v[3] = MY_DEFAULT_IP_ADDR_BYTE4;
            AutoIPClient.smAUTOIPState = SM_AUTOIP_RATE_LIMIT_WAIT;
            break;

        // Ensure that we don't try more than one address every 60 seconds
        case SM_AUTOIP_RATE_LIMIT_WAIT:
            if (TickGet() - AutoIPClient.eventTime > TICK_SECOND * 60)
                AutoIPClient.smAUTOIPState = SM_AUTOIP_CHECK_ADDRESS;
            break;

        // Configured state
        case SM_AUTOIP_CONFIGURED:
            AutoIPClient.flags.bits.bConfigureAutoIP = false;
            break;

        // Address defense state
        case SM_AUTOIP_DEFEND:
            // Prepare and send an ARP response
            AutoIPClient.packet.Operation = ARP_OPERATION_RESP;
            AutoIPClient.packet.HardwareType = HW_ETHERNET;
            AutoIPClient.packet.Protocol = ARP_IP;

            SwapARPPacket(&AutoIPClient.packet);

            while (!MACIsTxReady());
            MACSetWritePtr(BASE_TX_ADDR);

            MACPutHeader(&AutoIPClient.packet.TargetMACAddr, MAC_ARP, sizeof (AutoIPClient.packet));
            MACPutArray((uint8_t *) & AutoIPClient.packet, sizeof (AutoIPClient.packet));
            MACFlush();

            AutoIPClient.smAUTOIPState = SM_AUTOIP_CONFIGURED;
            break;
        }
    }
}

/*****************************************************************************
  Function:
    void AutoIPConflict(uint8_t vInterface)

  Summary:
    Handles AutoIP address conflicts.

  Description:
    This function will change the state machine to handle AutoIP address
    conflicts.

  Precondition:
    None

  Parameters:
    vInterface - Interface number to cause an AutoIP conflict for.   If
        you only have one interface, specify 0x00.

  Returns:
    None
 ***************************************************************************/
void AutoIPConflict(uint8_t vInterface)
{
    LoadState(vInterface);

    AutoIPClient.conflicts++;

    // State handler
    switch (AutoIPClient.smAUTOIPState) {
        // During configuration, if there is a conflict, immediately give
        // up the current address and select a new one.
        // If more than 10 conflicts have occured, limit the rate of
        // address retrys to 1 every 60 seconds.
    case SM_AUTOIP_INIT_RNG:
    case SM_AUTOIP_CHECK_ADDRESS:
    case SM_AUTOIP_SETUP_MESSAGE:
    case SM_AUTOIP_GRATUITOUS_ARP1:
    case SM_AUTOIP_GRATUITOUS_ARP2:
    case SM_AUTOIP_GRATUITOUS_ARP3:
    case SM_AUTOIP_DELAY:
        if (AutoIPClient.conflicts >= 10u)
            AutoIPClient.smAUTOIPState = SM_AUTOIP_RATE_LIMIT_SET;
        else
            AutoIPClient.smAUTOIPState = SM_AUTOIP_CHECK_ADDRESS;
        break;
    case SM_AUTOIP_RATE_LIMIT_SET:
    case SM_AUTOIP_RATE_LIMIT_WAIT:
    case SM_AUTOIP_DISABLED:
        AutoIPClient.conflicts--;
        break;
        // If there is a conflict while we have an address configured,
        // send a defense packet.  If more than one conflict occurs within
        // 10 seconds, claim a new address.
    case SM_AUTOIP_CONFIGURED:
    case SM_AUTOIP_DEFEND:
        if (AutoIPClient.gAutoIPConflictTimer != 0u) {
            if (TickGet() - AutoIPClient.gAutoIPConflictTimer < TICK_SECOND * 10) {
                AutoIPClient.smAUTOIPState = SM_AUTOIP_CHECK_ADDRESS;
                return;
                ;
            }
        }
        AutoIPClient.gAutoIPConflictTimer = TickGet();
        AutoIPClient.smAUTOIPState = SM_AUTOIP_DEFEND;
        break;
    }
}

/*****************************************************************************
  Function:
    void AutoIPDisable (uint8_t vInterface)

  Summary:
    Disables the AutoIP module.

  Description:
    Disables the AutoIP module.  If DHCP is enabled, this function will
    reset the IP address to the default address if no DHCP address has been
    bound.  If DHCP is disabled, this function will reset the IP address to
    the default address.

  Precondition:
    None

  Parameters:
    vInterface - Interface number to disable AutoIP for.   If you only
        have one interface, specify 0x00.

  Returns:
    None

  Remarks:
    None
 ***************************************************************************/
void AutoIPDisable(uint8_t vInterface)
{
    LoadState(vInterface);

    AutoIPClient.flags.bits.gDisableAutoIP = true;

#if defined (STACK_USE_DHCP_CLIENT)
    if (!DHCPIsBound(vInterface)) {
        AppConfig.MyIPAddr.Val = AppConfig.DefaultIPAddr.Val;
        AppConfig.MyMask.Val = AppConfig.DefaultMask.Val;
    }
#else
    AppConfig.MyIPAddr.Val = AppConfig.DefaultIPAddr.Val;
    AppConfig.MyMask.Val = AppConfig.DefaultMask.Val;
#endif
}

/*****************************************************************************
  Function:
    void AutoIPEnable (uint8_t vInterface)

  Summary:
    Enables the AutoIP module.

  Description:
    Enables the AutoIP module.  This function will end the manual-disable
    condition for AutoIP, and reset the state machine to the beginning.
    If a DHCP address is bound or the link is broken, the state will be
    automatically set back to disabled.

  Precondition:
    None

  Parameters:
    vInterface - Interface number to enable AutoIP for. If you only have
        one interface, specify 0x00.

  Returns:
    None

  Remarks:
    None
 ***************************************************************************/
void AutoIPEnable(uint8_t vInterface)
{
    LoadState(vInterface);

    AutoIPClient.flags.bits.gDisableAutoIP = false;
    AutoIPClient.smAUTOIPState = SM_AUTOIP_INIT_RNG;
}

/*****************************************************************************
  Function:
    bool AutoIPIsDisabled (uint8_t vInterface)

  Summary:
    Determines if the AutoIP state machine is in a disabled state.

  Description:
    Determines if the AutoIP state machine is in a disabled state.  This
    could occur because a DHCP address is bound, the link is broken, or
    the user has manually disabled the AutoIP module.

  Precondition:
    None

  Parameters:
    vInterface - Interface number to check the AutoIP disable status for.
        If you only have one interface, specify 0x00.

  Returns:
    true - The AutoIP client is disabled
    false - The AutoIP client in active

  Remarks:
    None
 ***************************************************************************/
bool AutoIPIsDisabled(uint8_t vInterface)
{
    LoadState(vInterface);

    return (AutoIPClient.smAUTOIPState == SM_AUTOIP_DISABLED) ? true : false;
}

/*****************************************************************************
  Function:
    void AutoIPRandSeed (uint32_t seed, uint8_t vInterface)

  Summary:
    Seeds a random number generator used to generate a MAC

  Description:
    Seeds a random number generator used to generate a MAC

  Precondition:
    None

  Parameters:
    seed - The seed value for the RNG
    vInterface - The interface to seed the generator for

  Returns:
    None

  Remarks:
    None
 ***************************************************************************/
void AutoIPRandSeed(uint32_t seed, uint8_t vInterface)
{
    LoadState(vInterface);
    AutoIPClient.wRandSeed = seed;
}

/*****************************************************************************
  Function:
    uint32_t AutoIPRand (uint8_t vInterface)

  Summary:
    Generates a random number

  Description:
    Generates a random number using a LFSR

  Precondition:
    None

  Parameters:
    vInterface - The interface to generate the RNG for

  Returns:
    A random number from 0 to 2^32-1

  Remarks:
    None
 ***************************************************************************/
uint32_t AutoIPRand(uint8_t vInterface)
{
    LoadState(vInterface);
    LFSRSeedRand(AutoIPClient.wRandSeed);
    AutoIPClient.wRandSeed = LFSRRand();
    return AutoIPClient.wRandSeed;
}

#endif
