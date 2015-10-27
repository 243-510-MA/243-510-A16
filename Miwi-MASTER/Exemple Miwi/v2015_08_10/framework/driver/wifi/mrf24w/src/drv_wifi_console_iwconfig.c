/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_wifi_console_iwconfig.c

  Summary:
    Module for Microchip TCP/IP Stack
    -Provides access to MRF24W WiFi controller
    -Reference: MRF24W Data sheet, IEEE 802.11 Standard

  Description:
    MRF24W Driver Iwconfig

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
 *                                 INCLUDES
 *******************************************************************************/
#include <ctype.h>

#include "tcpip/tcpip.h"

#include "drv_wifi_console.h"

#if defined( WF_CONSOLE_IFCFGUTIL )

#include "drv_wifi_console_iwconfig.h"
#include "drv_wifi_console_msg.h"
#include "drv_wifi_console_msg_handler.h"

#if defined ( EZ_CONFIG_SCAN )
    #include "drv_wifi_easy_config.h"
#endif /* EZ_CONFIG_SCAN */

/*******************************************************************************
 *                             CONSTANTS
 *******************************************************************************/

/*******************************************************************************
 *                             GLOBALS
 *******************************************************************************/

static bool   iwconfigCbInitialized = false;
tWFIwconfigCb iwconfigCb;

/*******************************************************************************
 *                       LOCAL FUNCTION PROTOTYPES
 *******************************************************************************/
static void iwconfigDisplayStatus(void);
static bool iwconfigSetSsid(void);
static bool iwconfigSetMode(void);
static bool iwconfigSetChannel(void);
static bool iwconfigSetPower(void);
static bool iwconfigSetDomain(void);
static bool iwconfigSetRTS(void);
static bool iwconfigSetTxRate(void);
static bool iwconfigSetConnect(void);
tWFHibernate WF_hibernate;

bool     test_flag;
uint32_t test_sec;
int      test_count;
char     test_buf[80];

/*******************************************************************************
  Function:
    void do_iwconfig_cmd(void)

  Summary:
    Responds to the user invoking iwconfig command

  Description:
    Responds to the user invoking iwconfig command

  Precondition:
    MACInit must be called first.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
 *****************************************************************************/
void do_iwconfig_cmd(void)
{
    if (!WF_hibernate.state && !iwconfigSetCb() )
        return;

    // if user only typed in iwconfig with no other parameters
    if (ARGC == 1u)
    {
        if (!WF_hibernate.state)
            iwconfigDisplayStatus();
        else
            #if defined(STACK_USE_UART)
                WFConsolePrintRomStr("The Wi-Fi module is in hibernate mode - command failed.", true);
            #endif
        return;
    }

    if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "wakeup") == 0) )
    {
        if (!WF_hibernate.wakeup_notice)
        {
            WF_hibernate.wakeup_notice = true;
        }

        #if defined(STACK_USE_UART)
            WFConsolePrintRomStr("The Wi-Fi module is awake.", true);
        #endif

        return;
    }

    if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "hibernate") == 0) )
    {
        if (!WF_hibernate.state)
        {
            WF_hibernate.state = WF_HB_ENTER_SLEEP;
            WF_hibernate.wakeup_notice = false;
            WFConsolePrintRomStr("The Wi-Fi module is in hibernate mode.", true);
        }
        else
            WFConsolePrintRomStr("The Wi-Fi module is in hibernate mode.", true);
        return;
    }

    if (WF_hibernate.state)
    {
        WFConsolePrintRomStr("The Wi-Fi module is in hibernate mode - command failed.", true);
        return;
    }

    if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "ssid") == 0) )
    {
        if (!WF_hibernate.state && !iwconfigSetSsid())
            return;
    }
    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "mode") == 0) )
    {
        if (!WF_hibernate.state && !iwconfigSetMode())
            return;
    }
    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "channel") == 0) )
    {
        if (!WF_hibernate.state && !iwconfigSetChannel())
            return;
    }
    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "power") == 0) )
    {
        if (!WF_hibernate.state && !iwconfigSetPower())
            return;
    }
    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "domain") == 0) )
    {
        if (!WF_hibernate.state && !iwconfigSetDomain())
            return;
    }
    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "rts") == 0) )
    {
        if (!WF_hibernate.state && !iwconfigSetRTS())
            return;
    }
    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "txrate") == 0) )
    {
        // txrate is NOT available. Will always return false
        if (!WF_hibernate.state && !iwconfigSetTxRate())
            return;
    }

    #if defined ( EZ_CONFIG_SCAN ) && !defined(__XC8)
    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "scan") == 0))
    {
        if (!WF_hibernate.state)
        {
            // WFInitScan();
            WFConsolePrintRomStr("Scanning...", true);
            if (WFStartScan() == WF_SUCCESS)
            {
                WFConsolePrintRomStr("Scan completed.", true);
            }
            else
            {
                WFConsolePrintRomStr("Scan failed. Already in progress or not allowed", true);
            }
        }
        else
        {
            WFConsolePrintRomStr("In hibernate mode - scan is not allowed.", true);
        }
        return;
    }
    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "scanresults") == 0) )
    {
        if (IS_SCAN_IN_PROGRESS(SCANCXT.scanState))
            WFConsolePrintRomStr("Scann in process...please wait or try again later", true);
        else if (SCANCXT.numScanResults > 0)
        {
            SCAN_SET_DISPLAY(SCANCXT.scanState);
            SCANCXT.displayIdx = 0;
            while (IS_SCAN_STATE_DISPLAY(SCANCXT.scanState))
            {
                WFDisplayScanMgr();
            }
        }
        else
            WFConsolePrintRomStr("No scan results to display.", true);

        return;
    }
    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "starttest") == 0))
    {
        test_flag = true;
        test_count = 0;
        return;
    }
    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "stoptest") == 0))
    {
        test_flag = false;
        return;
    }

    #endif /* EZ_CONFIG_SCAN */

    else if ( (2u <= ARGC) && (strcmppgm2ram((char*)ARGV[1], "connect") == 0))
    {
        iwconfigSetConnect();
        return;
    }
    else
    {
        WFConsolePrintRomStr("Unknown parameter", true);
        return;
    }
}

/*******************************************************************************
  Function:
      bool iwconfigSetCb(void)

  Summary:
    Set the iwconfigCb structure

  Description:
    Set the iwconfigCb structure

  Parameters:
      None.

  Returns:
      true or false

  Remarks:
       None.
 *****************************************************************************/
bool iwconfigSetCb(void)
{
    uint8_t cpId;

    if ( !iwconfigCbInitialized ) // first time call of iwconfigSetCb
    {
        memset(&iwconfigCb, 0, sizeof(iwconfigCb));
        iwconfigCbInitialized = true;
    }

    #if defined(WF_USE_POWER_SAVE_FUNCTIONS)
        WF_GetPowerSaveState(&iwconfigCb.powerSaveState);
    #endif
    if (iwconfigCb.powerSaveState == WF_PS_HIBERNATE)
    {
        WFConsolePrintRomStr("WF device hibernated", true);
        return false;
    }

    WF_CMGetConnectionState(&iwconfigCb.connState, &cpId);

    if ( iwconfigCb.cpId == WF_CURRENT_CPID_NONE )
    {
        if ( cpId == WF_CURRENT_CPID_NONE )
        {
            iwconfigCb.cpId = 1; // console demo only supports 1 CPID; don't create a new one here
        }
        else if ( cpId == WF_CURRENT_CPID_LIST )
        {
            WFConsolePrintRomStr("Connection profile list not supported", true);
            return false;
        }
        else
        {
            iwconfigCb.cpId = cpId; // use the application-created profile
        }
    }
    else // WF_MIN_NUM_CPID <= iwconfigCb.cpId && iwconfigCb.cpId <= WF_MAX_NUM_CPID
    {
        if ( cpId == WF_CURRENT_CPID_NONE )
        {
            // continue to use iwconfigCb.cpId
        }
        else if ( cpId == WF_CURRENT_CPID_LIST )
        {
            WFConsolePrintRomStr("Conection profile list not supported", true);

            WF_CPDelete(iwconfigCb.cpId);
            iwconfigCb.cpId = WF_CURRENT_CPID_NONE;

            return false;
        }
        else if ( cpId != iwconfigCb.cpId )
        {
            WF_CPDelete(iwconfigCb.cpId);
            iwconfigCb.cpId = cpId; // use the application-created profile
        }
        else // cpId == iwconfigCb.cpId
        {
            // contine to use iwconfigCb.cpId
        }
    }

    if ((iwconfigCb.connState == WF_CSTATE_NOT_CONNECTED) || (iwconfigCb.connState == WF_CSTATE_CONNECTION_PERMANENTLY_LOST))
    {
        iwconfigCb.isIdle = true;
    }
    else
    {
        iwconfigCb.isIdle = false;
    }

    return true;
}

#if defined(MRF24WG)
static void OutputMacAddress(void)
{
    uint8_t mac[6];
    int i;
    char buf[4];

    WF_GetMacAddress(mac);
    for (i = 0; i < 6; ++i)
    {
        sprintf(buf, "%02X ", mac[i]);
        putsUART(buf);
    }
    putrsUART("\r\n");
}
#endif

extern void WF_OutputConnectionContext(void);

/*******************************************************************************
  Function:
      void iwconfigDisplayStatus(void)

  Summary:
    Responds to the user invoking iwconfig with no parameters

  Description:
    Responds to the user invoking iwconfig with no parameters

  Parameters:
      None.

  Returns:
    None

  Remarks:
       None.
 *****************************************************************************/
static void iwconfigDisplayStatus(void)
{
    uint8_t *p;
    uint8_t tmp;
    uint8_t connectionState;
    uint8_t cpId;
    #if defined(MRF24WG)
    #if (MY_DEFAULT_NETWORK_TYPE == WF_ADHOC || MY_DEFAULT_NETWORK_TYPE == WF_INFRASTRUCTURE)
    char buf[6];
    #endif
    #endif

    union
    {
        struct
        {
            uint8_t List[WF_CHANNEL_LIST_LENGTH];
            uint8_t Num;
        } Channel;

        uint8_t Domain;

        struct
        {
            uint8_t String[WF_MAX_SSID_LENGTH + 1];
            uint8_t Len;
        } Ssid;

        struct
        {
            uint8_t NetworkType;
        } Mode;

        struct
        {
            uint16_t Threshold;
        } Rts;
    } ws; // workspace

    // cpId
    {
        WFConsolePrintRomStr("\tcpid:     ", false);
        WFConsolePrintInteger(iwconfigCb.cpId, 'd');
        WFConsolePrintRomStr("", true);
    }

    // channel
    {
        WF_CAGetChannelList(ws.Channel.List, &ws.Channel.Num);
        WFConsolePrintRomStr("\tchannel:  ", false);

        p = ws.Channel.List;
        tmp = ws.Channel.Num;

        while ( --tmp > 0u )
        {
            WFConsolePrintInteger(*p, 'd');
            WFConsolePrintRomStr(",", false);
            p++;
        }

        WFConsolePrintInteger(*p, 'd');
        WFConsolePrintRomStr("", true);
    }

#if defined(MRF24WG)
    // domain
    {
        WF_GetRegionalDomain(&ws.Domain);

        WFConsolePrintRomStr("\tdomain:   ", false);

        if ( ws.Domain == WF_DOMAIN_FCC )
        {
            WFConsolePrintRomStr("fcc", true);
        }
        else if ( ws.Domain == WF_DOMAIN_ETSI )
        {
            WFConsolePrintRomStr("etsi", true);
        }
        else if ( ws.Domain == WF_DOMAIN_JAPAN )
        {
            WFConsolePrintRomStr("japan", true);
        }
        else if ( ws.Domain == WF_DOMAIN_OTHER )
        {
            WFConsolePrintRomStr("other", true);
        }
        else
        {
            WFConsolePrintRomStr("unknown", true);
        }
    }
#else
    // domain
    {
        WF_GetRegionalDomain(&ws.Domain);

        WFConsolePrintRomStr("\tdomain:   ", false);

        if ( ws.Domain == WF_DOMAIN_FCC )
        {
            WFConsolePrintRomStr("fcc", true);
        }
        else if ( ws.Domain == WF_DOMAIN_IC )
        {
            WFConsolePrintRomStr("ic", true);
        }
        else if ( ws.Domain == WF_DOMAIN_ETSI )
        {
            WFConsolePrintRomStr("etsi", true);
        }
        else if ( ws.Domain == WF_DOMAIN_SPAIN )
        {
            WFConsolePrintRomStr("spain", true);
        }
        else if ( ws.Domain == WF_DOMAIN_FRANCE )
        {
            WFConsolePrintRomStr("france", true);
        }
        else if ( ws.Domain == WF_DOMAIN_JAPAN_A )
        {
            WFConsolePrintRomStr("japana", true);
        }
        else if ( ws.Domain == WF_DOMAIN_JAPAN_B )
        {
            WFConsolePrintRomStr("japanb", true);
        }
        else
        {
            WFConsolePrintRomStr("unknown", true);
        }
    }
#endif

    // rts
    {
        WF_GetRtsThreshold(&ws.Rts.Threshold);

        WFConsolePrintRomStr("\trts:      ", false);
        WFConsolePrintInteger(ws.Rts.Threshold, 'd');
        WFConsolePrintRomStr("", true);
    }

    // mode
    {

        WF_CMGetConnectionState(&connectionState, &cpId);
        WF_CPGetNetworkType(iwconfigCb.cpId, &ws.Mode.NetworkType);

        WFConsolePrintRomStr("\tmode:     ", false);

        if (iwconfigCb.isIdle)
        {
            if (iwconfigCb.connState == WF_CSTATE_NOT_CONNECTED)
            {
                WFConsolePrintRomStr("idle", true);
            }
            else if (iwconfigCb.connState == WF_CSTATE_CONNECTION_PERMANENTLY_LOST)
            {
                WFConsolePrintRomStr("idle (connection permanently lost)", true);
            }
            else
            {
                WFConsolePrintRomStr("idle (?)", true);
            }
        }
        else
        {
            WF_CPGetNetworkType(iwconfigCb.cpId, &ws.Mode.NetworkType);
            if (ws.Mode.NetworkType == WF_INFRASTRUCTURE)
            {
                if (iwconfigCb.connState == WF_CSTATE_CONNECTION_IN_PROGRESS)
                {
                    WFConsolePrintRomStr("managed (connection in progress)", true);
                }
                else if (iwconfigCb.connState == WF_CSTATE_CONNECTED_INFRASTRUCTURE)
                {
                    WFConsolePrintRomStr("managed", true);
                }
                else if (iwconfigCb.connState == WF_CSTATE_RECONNECTION_IN_PROGRESS)
                {
                    WFConsolePrintRomStr("managed (reconnection in progress)", true);
                }
                else
                {
                    WFConsolePrintRomStr("managed (?)", true);
                }
            }
            else if (ws.Mode.NetworkType == WF_ADHOC)
            {
                if (iwconfigCb.connState == WF_CSTATE_CONNECTION_IN_PROGRESS)
                {
                    WFConsolePrintRomStr("adhoc (connection in progress)", true);
                }
                else if (iwconfigCb.connState == WF_CSTATE_CONNECTED_ADHOC)
                {
                    WFConsolePrintRomStr("adhoc", true);
                }
                else if (iwconfigCb.connState == WF_CSTATE_RECONNECTION_IN_PROGRESS)
                {
                    WFConsolePrintRomStr("adhoc (reconnection in progress)", true);
                }
                else
                {
                    WFConsolePrintRomStr("adhoc (?)", true);
                }
            }
            else
            {
                WFConsolePrintRomStr("unknown", true);
            }
        }
    }

    // ssid
    {
        WF_CPGetSsid(iwconfigCb.cpId, ws.Ssid.String, &ws.Ssid.Len);
        ws.Ssid.String[ws.Ssid.Len] = '\0';

        WFConsolePrintRomStr("\tssid:     ", false);
        WFConsolePrintRamStr(ws.Ssid.String, true);
    }

    // power
    {
        switch (iwconfigCb.powerSaveState)
        {
        case WF_PS_PS_POLL_DTIM_ENABLED:
            WFConsolePrintRomStr("\tpwrsave:  enabled", true);
            WFConsolePrintRomStr("\tdtim rx:  enabled", true);
            break;
        case WF_PS_PS_POLL_DTIM_DISABLED:
            WFConsolePrintRomStr("\tpwrsave:  enabled", true);
            WFConsolePrintRomStr("\tdtim rx:  disabled", true);
            break;
        case WF_PS_OFF:
            WFConsolePrintRomStr("\tpwrsave:  disabled", true);
            break;
        default:
            WFConsolePrintRomStr("\tpwrsave:  unknown(", false);
            WFConsolePrintInteger(iwconfigCb.powerSaveState, 'd');
            WFConsolePrintRomStr(")", true);
            break;
        }
    }

    #if defined(MRF24WG)
    // context
    WF_OutputConnectionContext();

    // Network Type
    putrsUART("\tNetwork:  ");
    #if defined(EZ_CONFIG_STORE) && !defined(WF_CONSOLE_DEMO) /* if EZConfig demo */
        if (AppConfig.networkType == WF_ADHOC)
        {
            putrsUART("AdHoc\r\n");
        }
        else
        {
            putrsUART("Infrastructure\r\n");
        }
    #else
        #if (MY_DEFAULT_NETWORK_TYPE == WF_ADHOC)
            putrsUART("AdHoc\r\n");
        #elif (MY_DEFAULT_NETWORK_TYPE == WF_P2P)
            putrsUART("P2P\r\n");
        #elif (MY_DEFAULT_NETWORK_TYPE == WF_INFRASTRUCTURE)
            #if (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPS_PUSH_BUTTON)
                putrsUART("Infrastructure (using WPS Push Button)\r\n");
            #elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPS_PIN)
                putrsUART("Infrastructure (using WPS Pin)\r\n");
            #else
                putrsUART("Infrastructure\r\n");
            #endif
        #endif
    #endif /* EZ_CONFIG_STORE  */

    // Retry Count
    putrsUART("\tRetries   ");
    #if (MY_DEFAULT_NETWORK_TYPE == WF_ADHOC)
        sprintf(buf, "%d\r\n", ADHOC_RETRY_COUNT);
        putsUART(buf);
    #elif (MY_DEFAULT_NETWORK_TYPE == WF_INFRASTRUCTURE)
        #if (INFRASTRUCTURE_RETRY_COUNT == WF_RETRY_FOREVER)
            sprintf(buf, "Retry Forever\r\n");
            putsUART(buf);
        #else
            sprintf(buf, "%d\r\n", INFRASTRUCTURE_RETRY_COUNT);
            putsUART(buf);
        #endif
    #endif /* (MY_DEFAULT_NETWORK_TYPE == WF_ADHOC) */

    // Security
    putrsUART("\tSecurity: ");
    #if (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_OPEN)
        putrsUART("WF_SECURITY_OPEN");
    #elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WEP_40)
        putrsUART("WF_SECURITY_WEP_40");
    #elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WEP_104)
        putrsUART("WF_SECURITY_WEP_104");
    #elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_AUTO_WITH_KEY)
        putrsUART("WF_SECURITY_WPA_AUTO_WITH_KEY");
    #elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE)
        putrsUART("WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE");
    #elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPS_PUSH_BUTTON)
        putrsUART("WF_SECURITY_WPS_PUSH_BUTTON");
    #elif (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPS_PIN)
        putrsUART("WF_SECURITY_WPS_PIN");
    #else
        putrsUART("Unknown");
    #endif
    putrsUART("\r\n");

    // scan type
    putrsUART("\tScan:     ");
    #if (MY_DEFAULT_SCAN_TYPE == WF_ACTIVE_SCAN)
        putrsUART("Active Scan\r\n");
    #else
        putrsUART("Passive Scan\r\n");
    #endif

    // MAC address
    putrsUART("\tMAC:      ");
    OutputMacAddress();

    #endif /* MRF24WG */
}

static bool iwconfigSetSsid(void)
{
    if (ARGC < 3u)
    {
        WFConsolePrintRomStr("Missing value for last parameter", true);
        return false;
    }

    if (ARGC > 3u)
    {
        WFConsolePrintRomStr("SSID may not contain space for this demo", true);
        return false;
    }

    WF_CPSetSsid(iwconfigCb.cpId, (uint8_t *)ARGV[2], strlen((char*)ARGV[2]));

    return true;
}

/*******************************************************************************
  Function:
      bool iwconfigSetMode(void)

  Summary:
     Set the mode to idle, managed or adhoc.

  Description:
     Idle mode        - Force MRF24W module to disconnect from any currently connected network
        Managed mode - MRF24W module will connect to SSID in infrastructure mode. Ensure all network
                                  parameters are correct before this command is invoked.
        Adhoc mode     - MRF24W module will connect to SSID in adhoc mode. Ensure all network
                                  parameters are correct before this command is invoked.

  Parameters:
      Mode - idle / managed /adhoc

  Returns:
    true or false

  Remarks:
       None.
 *****************************************************************************/
static bool iwconfigSetMode(void)
{
    uint8_t networkType;

    WF_CPGetNetworkType(iwconfigCb.cpId, &networkType);

    if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "idle") == 0) )
    {
        if ( iwconfigCb.isIdle )
        {
            WFConsolePrintRomStr("Already in the idle mode", true);
        }
        else
        {
            if (WF_CMDisconnect() != WF_SUCCESS)
            {
                #if defined(STACK_USE_UART)
                    putsUART("Disconnect failed. Disconnect is allowed only when module is in connected state\r\n");
                #endif
            }
            WF_PsPollDisable();
        }
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "managed") == 0) )
    {
        if ( iwconfigCb.isIdle )
        {
            WF_CPSetNetworkType(iwconfigCb.cpId, WF_INFRASTRUCTURE);
            WF_CMConnect(iwconfigCb.cpId);
        }
        else
        {
            WF_CPGetNetworkType(iwconfigCb.cpId, &networkType);
            if (networkType == WF_INFRASTRUCTURE)
            {
                WFConsolePrintRomStr("Already in the managed mode", true);
            }
            else
            {
                if (WF_CMDisconnect() != WF_SUCCESS)
                {
                    #if defined(STACK_USE_UART)
                        putsUART("Disconnect failed. Disconnect is allowed only when module is in connected state\r\n");
                    #endif
                }

                WF_CPSetNetworkType(iwconfigCb.cpId, WF_INFRASTRUCTURE);
                WF_CMConnect(iwconfigCb.cpId);
            }
        }
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "adhoc") == 0) )
    {
        if ( iwconfigCb.isIdle )
        {
            WF_CASetListRetryCount(ADHOC_RETRY_COUNT);
            WF_CPSetNetworkType(iwconfigCb.cpId, WF_ADHOC);
            WF_CPSetAdHocBehavior(iwconfigCb.cpId, WF_ADHOC_CONNECT_THEN_START);
            WF_CMConnect(iwconfigCb.cpId);
        }
        else
        {
            WF_CPGetNetworkType(iwconfigCb.cpId, &networkType);
            if (networkType == WF_ADHOC)
            {
                WFConsolePrintRomStr("Already in the adhoc mode", true);
            }
            else
            {
                if (WF_CMDisconnect() != WF_SUCCESS)
                {
                    #if defined(STACK_USE_UART)
                        putsUART("Disconnect failed. Disconnect is allowed only when module is in connected state\r\n");
                    #endif
                }

                WF_CPSetNetworkType(iwconfigCb.cpId, WF_ADHOC);
                WF_CMConnect(iwconfigCb.cpId);
            }
        }
    }
    else
    {
        WFConsolePrintRomStr("Unknown parameter", true);
        return false;
    }

    return true;
}

static bool iwconfigSetChannel(void)
{
    uint8_t *p1, *p2;
    uint8_t *p_channelList;
    uint8_t index = 0;
    uint16_t temp;

    if (ARGC < 3u)
    {
        WFConsolePrintRomStr("Missing value for last parameter", true);
        return false;
    }

    if ( !iwconfigCb.isIdle )
    {
        WFConsolePrintRomStr("Channel can only be set in idle mode", true);
        return false;
    }

    p_channelList = (uint8_t*) ARGV[2];
    p1 = p2 = p_channelList;

    if ( strlen( (char*) p_channelList) == 0u )
        return false;

    if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "all") == 0) )
    {
        WF_CASetChannelList(p_channelList, 0); // reset to domain default channel list
        return true;
    }

    do
    {
       if ( (p2 = (uint8_t*) strchr( (const char *) p1, (int) ',')) != NULL )
       {
          *p2='\0';
          p2++;
       }

       if ( !ConvertASCIIUnsignedDecimalToBinary((int8_t *)p1, &temp) )
          return  false;

       p1 = p2;
       p_channelList[index] = (uint8_t) temp;
       index++;

    } while (  p2 != NULL );

    WF_CASetChannelList(p_channelList, index);

    return true;
}

/*******************************************************************************
  Function:
      bool iwconfigSetPower(void)

  Summary:
     Enables or disables PS Poll mode.

  Description:
     Enables or disables PS Poll mode.
     reenable / all - Enables all power saving features (PS_POLL) of the MRF24W. MRF24W
                            will wake up to check for all types of traffic (unicast, multicast, and broadcast)
        disable          - Disables any power savings features.
        unicast          - MRF24W will be in its deepest sleep state, only waking up at periodic intervals
                               to check for unicast data. MRF24W will not wake up on the DTIM period for
                               broadcast or multicast traffic.

  Parameters:
       reenable / disable / unicast /all

  Returns:
    true or false

  Remarks:
       WF_USE_POWER_SAVE_FUNCTIONS must be defined to use PS Poll mode.
 *****************************************************************************/
static bool iwconfigSetPower(void)
{
    if (ARGC < 3u)
    {
        WFConsolePrintRomStr("Missing value for last parameter", true);
        return false;
    }

    if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "reenable") == 0) )
    {    // reenable power saving
        #if defined(WF_USE_POWER_SAVE_FUNCTIONS)
        WF_PsPollEnable(true);
        #endif
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "disable") == 0) )
    {    // disable power saving
        #if defined(WF_USE_POWER_SAVE_FUNCTIONS)
        WF_PsPollDisable();
        #endif
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "unicast") == 0) )
    {   // enable power saving but don't poll for DTIM
        #if defined(WF_USE_POWER_SAVE_FUNCTIONS)
        WF_PsPollEnable(false);
        #endif
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "all") == 0) )
    {    // enable power saving and poll for DTIM
        #if defined(WF_USE_POWER_SAVE_FUNCTIONS)
        WF_PsPollEnable(true);
        #endif
    }
    else
    {
        WFConsolePrintRomStr("Unknown parameter", true);
        return false;
    }

    return true;
}

/*******************************************************************************
  Function:
      bool iwconfigSetDomain(void)

  Summary:
     Set the domain.

  Description:
     Set the MRF24W Regional Domain.
     For MRF24WG with RF module FW version 0x3107 and future releases, this function
     is NOT supported due to changes in FCC requirements, which does not allow programming
     of the regional domain.

  Parameters:
       Domain - fcc / etsi /japan / other

  Returns:
    true or false

  Remarks:
       None.
 *****************************************************************************/
static bool iwconfigSetDomain(void)
{
    uint8_t domain;

    if (ARGC < 3u)
    {
        WFConsolePrintRomStr("Missing value for last parameter", true);
        return false;
    }

    if ( !iwconfigCb.isIdle )
    {
        WFConsolePrintRomStr("Domain can only be set in idle mode", true);
        return false;
    }

#if defined(MRF24WG)
    if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "fcc") == 0) )
    {
        domain = WF_DOMAIN_FCC;
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "etsi") == 0) )
    {
        domain = WF_DOMAIN_ETSI;
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "japan") == 0) )
    {
        domain = WF_DOMAIN_JAPAN;
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "other") == 0) )
    {
        domain = WF_DOMAIN_OTHER;
    }
    else
    {
        WFConsolePrintRomStr("Unknown domain", true);
        return false;
    }
#else
    if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "fcc") == 0) )
    {
        domain = WF_DOMAIN_FCC;
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "ic") == 0) )
    {
        domain = WF_DOMAIN_IC;
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "etsi") == 0) )
    {
        domain = WF_DOMAIN_ETSI;
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "spain") == 0) )
    {
        domain = WF_DOMAIN_SPAIN;
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "france") == 0) )
    {
        domain = WF_DOMAIN_FRANCE;
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "japana") == 0) )
    {
        domain = WF_DOMAIN_JAPAN_A;
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char*)ARGV[2], "japanb") == 0) )
    {
        domain = WF_DOMAIN_JAPAN_B;
    }
    else
    {
        WFConsolePrintRomStr("Unknown domain", true);
        return false;
    }
#endif

    WF_SetRegionalDomain(domain);
    WF_CASetChannelList(NULL, 0); // reset to domain default channel list

    return true;
}

static bool iwconfigSetRTS(void)
{
    uint16_t rtsThreshold;

    if (ARGC < 3u)
    {
        WFConsolePrintRomStr("Missing value for last parameter", true);
        return false;
    }

    if ( !ConvertASCIIUnsignedDecimalToBinary(ARGV[2], &rtsThreshold) )
        return  false;

    WF_SetRtsThreshold(rtsThreshold);

    return true;
}

static bool iwconfigSetTxRate(void)
{
    return false;
}
static uint8_t SetMode_idle(void)
{
    uint8_t networkType;

    WF_CPGetNetworkType(iwconfigCb.cpId, &networkType);

    if (false == iwconfigCb.isIdle )
    {
        if (WF_CMDisconnect() != WF_SUCCESS)
        {
            putsUART("Disconnect failed. Disconnect is allowed only when module is in connected state\r\n");
        }
        WF_PsPollDisable();
    }
    return networkType;
}
static void SetMode_NotIdle(uint8_t networkType)
{
    if (WF_INFRASTRUCTURE == networkType)
    {
        WF_CPSetNetworkType(iwconfigCb.cpId, WF_INFRASTRUCTURE);
        WF_CMConnect(iwconfigCb.cpId);
    }
    else if (WF_ADHOC == networkType)
    {
        WF_CASetListRetryCount(ADHOC_RETRY_COUNT);
        WF_CPSetNetworkType(iwconfigCb.cpId, WF_ADHOC);
        WF_CPSetAdHocBehavior(iwconfigCb.cpId, WF_ADHOC_CONNECT_THEN_START);
        WF_CMConnect(iwconfigCb.cpId);
    }
    else
    {
        //To be done
    }

}

static bool iwconfigSetConnect(void)
{ // IWCONFIG CONNECT [ssid] [channel] [power-mode]   //[security-mode] [WEP-key/passphrase] [retry-attempt]
    uint8_t networkType;
    if (ARGC < 3u)
    {
        putsUART("Wrong command, correct command is:IWCONFIG CONNECT [ssid] [bssid] [channel] [power-mode]\r\n");
        return false;
    }
    
    networkType = SetMode_idle();
    if (ARGC >= 3u) // ssid
    {
        WF_CPSetSsid(iwconfigCb.cpId, (uint8_t *)ARGV[2], strlen((char*)ARGV[2]));
    }
    
    if (ARGC >= 4u) //channel
    {
        int int_channel;
        sscanf((const char *)ARGV[3], (const char *)"%d",&int_channel);
        if ((int_channel >= 1) && (int_channel <= 14))
        {
            WF_CASetChannelList((uint8_t *)&int_channel, 1);
        }
        else
        {
            WFConsolePrintRomStr("channel err (1~14): Unknown parameter", true);
            return false;
        }
    }
    
    if (ARGC >= 5u) //channel
    {
        int int_channel;
        uint8_t channel;
        
        sscanf((const char *)ARGV[4], (const char *)"%d",&int_channel);
        if ((int_channel >= 1) && (int_channel <= 14))
        {
            channel = int_channel;
                //{char buf_t[20];sprintf(buf_t,"channel=%d\r\n",int_channel);putsUART(buf_t);}
            WF_CASetChannelList(&channel, 1);
            DelayMs(100);
        }
        else
        {
            WFConsolePrintRomStr("channel err (1~14): Unknown parameter", true);
            return false;
        }
    }
    
    if (ARGC >= 6u) // power-mode
    {
        if (strcmppgm2ram((char*)ARGV[5], "reenable") == 0)
        {    // reenable power saving
            #if defined(WF_USE_POWER_SAVE_FUNCTIONS)
            WF_PsPollEnable(true);
            #endif
        }
        else if  (strcmppgm2ram((char*)ARGV[5], "disable") == 0)
        {    // disable power saving
            #if defined(WF_USE_POWER_SAVE_FUNCTIONS)
            WF_PsPollDisable();
            #endif
        }
        else if  (strcmppgm2ram((char*)ARGV[5], "unicast") == 0)
        {   // enable power saving but don't poll for DTIM
            #if defined(WF_USE_POWER_SAVE_FUNCTIONS)
            WF_PsPollEnable(false);
            #endif
        }
        else if  (strcmppgm2ram((char*)ARGV[5], "all") == 0)
        {    // enable power saving and poll for DTIM
            #if defined(WF_USE_POWER_SAVE_FUNCTIONS)
            WF_PsPollEnable(true);
            #endif
        }
        else
        {
            WFConsolePrintRomStr("Unknown parameter", true);
            return false;
        }
    }
    
    if (ARGC >= 7u) // [security-mode]
    {

    }
    SetMode_NotIdle(networkType);
    return true;
}

#endif  /* WF_CONSOLE_IFCFGUTIL */
