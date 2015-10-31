/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_wifi_console_ifconfig.c

  Summary:
    Module for Microchip TCP/IP Stack
    -Provides access to MRF24W WiFi controller
    -Reference: MRF24W Data sheet, IEEE 802.11 Standard

  Description:
    MRF24W Driver Ifconfig

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

/*******************************************************************************
 *                                  INCLUDES
 *******************************************************************************/
#include <ctype.h>

#include "tcpip/tcpip.h"

#include "drv_wifi_console.h"

#if defined( WF_CONSOLE_IFCFGUTIL )

#include "driver/wifi/mrf24w/drv_wifi_api.h"

#include "drv_wifi_console_ifconfig.h"
#include "drv_wifi_console_msg.h"
#include "drv_wifi_console_msg_handler.h"

/*******************************************************************************
 *                             CONSTANTS
 *******************************************************************************/

/*******************************************************************************
 *                             GLOBALS
 *******************************************************************************/
extern tWFHibernate WF_hibernate;

/*******************************************************************************
 *                      LOCAL FUNCTION PROTOTYPES
 *******************************************************************************/
static void IfconfigDisplayStatus(void);
static bool isMacAddress(int8_t *p_string, uint8_t *p_Address);
#if defined(STACK_USE_DHCP_CLIENT)
static void setDHCPState(bool enable);
#endif
static void missingValue(void);
static void notHandledParam(uint8_t index);

void LCDDisplayIPValue(IP_ADDR IPVal)
{
#ifdef USE_LCD

    uint8_t IPDigit[4];
    uint8_t i;
    uint8_t j;
    uint8_t LCDPos=16;

    for (i = 0; i < sizeof(IP_ADDR); i++)
    {
        uitoa((uint16_t)IPVal.v[i], IPDigit);

        for (j = 0; j < strlen((char*)IPDigit); j++)
        {
            LCDText[LCDPos++] = IPDigit[j];
        }
        
        if (i == sizeof(IP_ADDR) - 1)
            break;
            
        LCDText[LCDPos++] = '.';
    }

    if (LCDPos < 32u)
        LCDText[LCDPos] = 0;
    
    LCDUpdate();

#endif
}

/*****************************************************************************
 * FUNCTION: do_ifconfig_cmd
 *
 * RETURNS: None
 *
 * PARAMS:  None
 *
 * NOTES:   Responds to the user invoking ifconfig
 *****************************************************************************/
void do_ifconfig_cmd(void)
{
     uint8_t   macAddress[6];
     uint8_t conState, cpId;
     IP_ADDR ipAddress;

    // if user only typed in ifconfig with no other parameters
    if (ARGC == 1u)
    {
        IfconfigDisplayStatus();
        return;
    }

    if (WF_hibernate.state)
    {
        WFConsolePrintRomStr("The Wi-Fi module is in hibernate mode - command failed.", true);
        return;
    }

#if defined(WF_CM_DEBUG)
    else if ( (ARGC == 2u) && !strcmp((char *) ARGV[1], "info") )
    {
        uint8_t i;
        tWFCMInfoFSMStats cm_stats;

        WF_CMInfoGetFSMStats(&cm_stats);
        for (i = 0; i < 12; i++)
        {
            sprintf( (char *) g_ConsoleContext.txBuf,
                    "[%02X]: %02X%02X %02X%02X",
                    i,
                    cm_stats.byte[i*4 + 0],
                    cm_stats.byte[i*4 + 1],
                    cm_stats.byte[i*4 + 2],
                    cm_stats.byte[i*4 + 3]
                    );
            WFConsolePrintRamStr( (char *) g_ConsoleContext.txBuf , true);
        }
    }
    else if ( (ARGC == 2u) && !strcmp((char *) ARGV[1], "scan") )
    {
        if (WF_Scan(1) != WF_SUCCESS) // scan, using CP 1
            WFConsolePrintRomStr("Scan failed", true);
    }
    else if ( (ARGC == 2u) && !strcmp((char *) ARGV[1], "scanget") ) //"scangetresult"
    {
        tWFScanResult pScanResult[1];

        WF_ScanGetResult(0, pScanResult);
    }
    else if ( (ARGC == 2u) && !strcmp((char *) ARGV[1], "cpgete") ) //"cpgetelements"
    {
        tWFCPElements pCPElements[1];

        WF_CPGetElements(1, pCPElements);
    }
#endif
    // else if 2 arguments and the second arg is IP address
    else if ( (ARGC == 2u) && (StringToIPAddress((uint8_t*)ARGV[1], &ipAddress)) )
    {
        #if defined(STACK_USE_DHCP_CLIENT)
        if (DHCPIsEnabled(0))
        {
          WFConsolePrintRomStr("Static IP address should not be set with DHCP enabled", true);
          return;
        }
        #endif

        AppConfig.MyIPAddr.v[0] = ipAddress.v[0];
        AppConfig.MyIPAddr.v[1] = ipAddress.v[1];
        AppConfig.MyIPAddr.v[2] = ipAddress.v[2];
        AppConfig.MyIPAddr.v[3] = ipAddress.v[3];

        /* Microchip DHCP client clobbers static ip on every iteration of loop, even if dhcp is turned off*/
        AppConfig.DefaultIPAddr.v[0] = ipAddress.v[0];
        AppConfig.DefaultIPAddr.v[1] = ipAddress.v[1];
        AppConfig.DefaultIPAddr.v[2] = ipAddress.v[2];
        AppConfig.DefaultIPAddr.v[3] = ipAddress.v[3];

        LCDDisplayIPValue(AppConfig.MyIPAddr);
    }
    // else if 2 args and second arg is MAC address
    else if ( (ARGC == 2u) && isMacAddress(ARGV[1], macAddress))
    {
        /* Can only set MAC address in idle state */
        WF_CMGetConnectionState(&conState, &cpId);
        if ( conState != WF_CSTATE_NOT_CONNECTED )
        {
            WFConsolePrintRomStr("HW MAC address can only be set in idle mode", true);
            return;
        }

        WF_SetMacAddress( macAddress );
        AppConfig.MyMACAddr.v[0] = macAddress[0];
        AppConfig.MyMACAddr.v[1] = macAddress[1];
        AppConfig.MyMACAddr.v[2] = macAddress[2];
        AppConfig.MyMACAddr.v[3] = macAddress[3];
        AppConfig.MyMACAddr.v[4] = macAddress[4];
        AppConfig.MyMACAddr.v[5] = macAddress[5];
    }
    else if ( (2u <= ARGC) && (strcmppgm2ram((char *)ARGV[1], (ROM FAR char *)"netmask") == 0) )
    {
        if (ARGC != 3u)
        {
            missingValue();
            return;
        }

        #if defined(STACK_USE_DHCP_CLIENT)
        if ( DHCPIsEnabled(0) )
        {
            WFConsolePrintRomStr(
                "The Netmask should not be set with DHCP enabled", true);
            return;
        }
        #endif

        if ( !StringToIPAddress((uint8_t*)ARGV[2], &ipAddress) )
        {
            WFConsolePrintRomStr("Invalid netmask value", true);
            return;
        }

        AppConfig.MyMask.v[0] = ipAddress.v[0];
        AppConfig.MyMask.v[1] = ipAddress.v[1];
        AppConfig.MyMask.v[2] = ipAddress.v[2];
        AppConfig.MyMask.v[3] = ipAddress.v[3];

        /* Microchip DHCP client clobbers static netmask on every iteration of loop, even if dhcp is turned off*/
        AppConfig.DefaultMask.v[0] = ipAddress.v[0];
        AppConfig.DefaultMask.v[1] = ipAddress.v[1];
        AppConfig.DefaultMask.v[2] = ipAddress.v[2];
        AppConfig.DefaultMask.v[3] = ipAddress.v[3];
    }
    else if ( (2u <= ARGC) && (strcmppgm2ram((char *)ARGV[1], (ROM FAR char *)"gateway") == 0) )
    {
        if (ARGC != 3u)
        {
            missingValue();
            return;
        }

        if ( !StringToIPAddress((uint8_t*)ARGV[2], &ipAddress) )
        {
            WFConsolePrintRomStr("Invalid gateway value", true);
            return;
        }

        AppConfig.MyGateway.v[0] = ipAddress.v[0];
        AppConfig.MyGateway.v[1] = ipAddress.v[1];
        AppConfig.MyGateway.v[2] = ipAddress.v[2];
        AppConfig.MyGateway.v[3] = ipAddress.v[3];
    }
    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "auto-dhcp") == 0) )
    {
        if (ARGC != 3u)
        {
            missingValue();
            return;
        }

        #if defined(STACK_USE_DHCP_CLIENT)
        if (strcmppgm2ram((char*)ARGV[2], "start") == 0)
        {
            setDHCPState(true);
        }
        else if (strcmppgm2ram((char*)ARGV[2], "stop") == 0)
        {
            setDHCPState(false);
        }
        else
        #endif
        {
            WFConsolePrintRomStr("   Invalid dhcp param", true);
            return;
        }
    }
    else
    {
        notHandledParam(1);
    }
}

static void missingValue(void)
{
    WFConsolePrintRomStr(
        "Missing value after last parameter", true);
}

static void notHandledParam(uint8_t index)
{
    WFConsolePrintRomStr("Param ", false);
    WFConsolePrintInteger(index, 'd');
    WFConsolePrintRomStr(" not handled", true);
}

/*****************************************************************************
 * FUNCTION: isMacAddress
 *
 * RETURNS: True if valid MAC address, else False
 *
 * PARAMS:    p_string  -- string to check
 *          p_Address -- Array where MAC values will be written
 *
 * NOTES:   Determines if the input string is a valid MAC address.
 *          If it is, then returns an array of 6 bytes for each of the values.
 *          MAC address must be in hex in the format xx:xx:xx:xx:xx:xx
 *****************************************************************************/
static bool isMacAddress(int8_t *p_string, uint8_t *p_Address)
{
    uint8_t i;
    uint16_t tmp;

    if (strlen((char *)p_string) != 17u)
    {
        return false;
    }

    // ensure the ':' is in the right place, and if so, set them to 0
    for (i = 2; i < 17u; i += 3)
    {
        if (p_string[i] == (int8_t)':')
        {
            p_string[i] = '\0';
        }
        else
        {
            return false;
        }
    }

    // now extract each hex number string
    for (i = 0; i < 6u;  ++i)
    {
        if (!ConvertASCIIHexToBinary(&p_string[i * 3], &tmp))
        {
            return false;
        }

        p_Address[i] = (uint8_t) (tmp & 0xFF);

    }

    return true;
}

/*****************************************************************************
 * FUNCTION: IfconfigDisplayStatus
 *
 * RETURNS: None
 *
 * PARAMS:    None
 *
 * NOTES:   Responds to the user invoking ifconfig with no parameters
 *****************************************************************************/
static void IfconfigDisplayStatus(void)
{
    sprintf( (char *) g_ConsoleContext.txBuf,
              "\tIP addr:  %d.%d.%d.%d",   AppConfig.MyIPAddr.v[0],
                                           AppConfig.MyIPAddr.v[1],
                                           AppConfig.MyIPAddr.v[2],
                                           AppConfig.MyIPAddr.v[3] );
    WFConsolePrintRamStr( (char *) g_ConsoleContext.txBuf , true);

    sprintf( (char *) g_ConsoleContext.txBuf,
             "\tMAC addr: %02X:%02X:%02X:%02X:%02X:%02X", AppConfig.MyMACAddr.v[0],
                                                          AppConfig.MyMACAddr.v[1],
                                                          AppConfig.MyMACAddr.v[2],
                                                          AppConfig.MyMACAddr.v[3],
                                                          AppConfig.MyMACAddr.v[4],
                                                          AppConfig.MyMACAddr.v[5]);
    WFConsolePrintRamStr( (char *) g_ConsoleContext.txBuf , true);

    sprintf( (char *) g_ConsoleContext.txBuf,
              "\tNetmask:  %d.%d.%d.%d", AppConfig.MyMask.v[0],
                                         AppConfig.MyMask.v[1],
                                         AppConfig.MyMask.v[2],
                                         AppConfig.MyMask.v[3] );
    WFConsolePrintRamStr( (char *) g_ConsoleContext.txBuf , true);

    sprintf( (char *) g_ConsoleContext.txBuf,
              "\tGateway:  %d.%d.%d.%d", AppConfig.MyGateway.v[0],
                                         AppConfig.MyGateway.v[1],
                                         AppConfig.MyGateway.v[2],
                                         AppConfig.MyGateway.v[3] );
    WFConsolePrintRamStr( (char *) g_ConsoleContext.txBuf , true);

    #if defined(STACK_USE_DHCP_CLIENT)
    if ( DHCPIsEnabled(0) )
       WFConsolePrintRomStr("\tDHCP:     Started", true);
    else
       WFConsolePrintRomStr("\tDHCP:     Stopped", true);
    #endif
}

#if defined(STACK_USE_DHCP_CLIENT)
/*****************************************************************************
 * FUNCTION: setDHCPState
 *
 * RETURNS: None
 *
 * PARAMS:  enable -- a boolean indicating whether to enable DHCP or not
 *
 * NOTES:   Enable or disable DHCP operation
 *****************************************************************************/
static void setDHCPState(bool enable)
{
    if ( enable )
    {
        AppConfig.Flags.bIsDHCPEnabled = true;
        DHCPEnable(0);
    }
    else
    {
        AppConfig.Flags.bIsDHCPEnabled = false;
        DHCPDisable(0);
    }
}
#endif

#endif /* WF_CONSOLE_IFCFGUTIL */

