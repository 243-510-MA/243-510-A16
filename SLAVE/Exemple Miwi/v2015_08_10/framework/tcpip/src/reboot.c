/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    reboot.c

  Summary:
    Module for Microchip TCP/IP Stack
     -Remotely resets the PIC
     -Reference: Internet Bootloader documentation

  Description:
    Reboot Module

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

#define __REBOOT_C_

#include "tcpip/tcpip.h"

#if defined(STACK_USE_REBOOT_SERVER)

#define REBOOT_PORT  69 // UDP TFTP port

// For improved security, you might want to limit reboot capabilities
// to only users on the same IP subnet.  Define REBOOT_SAME_SUBNET_ONLY
// to enable this access restriction.
#define REBOOT_SAME_SUBNET_ONLY

extern NODE_INFO remoteNode;

/*********************************************************************
 * Function:        void RebootTask(void)
 *
 * PreCondition:    Stack is initialized()
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Checks for incomming traffic on port 69.
 *                  Resets the PIC if a 'R' is received.
 *
 * Note:            This module is primarily for use with the
 *                  Ethernet bootloader.  By resetting, the Ethernet
 *                  bootloader can take control for a second and let
 *                  a firmware upgrade take place.
 ********************************************************************/
void RebootTask(void)
{
    static UDP_SOCKET MySocket = INVALID_UDP_SOCKET;

    struct {
        uint8_t vMACAddress[6];
        uint32_t dwIPAddress;
        uint16_t wChecksum;
    } BootloaderAddress;

    if (MySocket == INVALID_UDP_SOCKET)
        MySocket = UDPOpenEx(0, UDP_OPEN_SERVER, REBOOT_PORT, INVALID_UDP_PORT);
    //MySocket = UDPOpen(REBOOT_PORT, NULL, INVALID_UDP_PORT);

    if (MySocket == INVALID_UDP_SOCKET)
        return;

    // Do nothing if no data is waiting
    if (!UDPIsGetReady(MySocket))
        return;

#if defined(REBOOT_SAME_SUBNET_ONLY)
    // Respond only to name requests sent to us from nodes on the same subnet
    if ((remoteNode.IPAddr.Val & AppConfig.MyMask.Val) != (AppConfig.MyIPAddr.Val & AppConfig.MyMask.Val)) {
        UDPDiscard();
        return;
    }
#endif

    // Get our MAC address, IP address, and compute a checksum of them
    memcpy((void *) &BootloaderAddress.vMACAddress[0], (void *) &AppConfig.MyMACAddr.v[0], sizeof (AppConfig.MyMACAddr));
    BootloaderAddress.dwIPAddress = AppConfig.MyIPAddr.Val;
    BootloaderAddress.wChecksum = CalcIPChecksum((uint8_t *) & BootloaderAddress, sizeof (BootloaderAddress) - sizeof (BootloaderAddress.wChecksum));

    // To enter the bootloader, we need to clear the /POR bit in RCON.
    // Otherwise, the bootloader will immediately hand off execution
    // to us.
#if defined(USE_LCD)
    strcpypgm2ram((char *) LCDText, "Bootloader Reset");
    LCDUpdate();
#endif
    RCONbits.POR = 0;
#if defined(__XC8)
    {
        TCPIP_UINT16_VAL wvPROD;

        wvPROD.Val = ((uint16_t) & BootloaderAddress);
        PRODH = wvPROD.v[1];
        PRODL = wvPROD.v[0];
    }
#endif
    Reset();
}

#endif // #if defined(STACK_USE_REBOOT_SERVER)
