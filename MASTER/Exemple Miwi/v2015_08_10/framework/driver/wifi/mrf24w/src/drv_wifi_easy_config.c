/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_wifi_easy_config.c

  Summary:
    Module for Microchip TCP/IP Stack
    -Provides access to MRF24W WiFi controller
    -Reference: MRF24W Data sheet, IEEE 802.11 Standard

  Description:
    MRF24W Driver

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

#include "tcpip/tcpip.h"

#if defined(WF_CS_TRIS)

#include "driver/wifi/mrf24w/drv_wifi_api.h"

#include "drv_wifi_easy_config.h"
#include "drv_wifi_console.h"

#if defined ( EZ_CONFIG_SCAN )
volatile tWFScanCtx  g_ScanCtx;
#endif /* EZ_CONFIG_SCAN */

#if defined(STACK_USE_EZ_CONFIG)
/* Easy Config Globals */
extern uint8_t ConnectionProfileID;

tWFEasyConfigCtx g_easyConfigCtx;

#if (MY_DEFAULT_NETWORK_TYPE == WF_SOFT_AP) || defined(WF_PRE_SCAN_IN_ADHOC)
tWFScanResult preScanResult[50];      //WF_PRESCAN  May change struct later for memory optimization
#endif

#if (MY_DEFAULT_NETWORK_TYPE == WF_SOFT_AP)
extern tWFHibernate WF_hibernate;
#endif

/* Easy Config Private Functions */
static int WFEasyConfigProcess(void);

void WFEasyConfigInit()
{
    CFGCXT.ssid[0] = 0;
    CFGCXT.security = WF_SECURITY_OPEN;
    CFGCXT.key[0] = 0;
    CFGCXT.defaultWepKey = WF_WEP_KEY_INVALID;
    CFGCXT.type = WF_INFRASTRUCTURE;
    CFGCXT.cfg_state = cfg_stopped;
    CFGCXT.isWifiNeedToConfigure = 0;

    #if defined (EZ_CONFIG_STORE)
    CFGCXT.isWifiDoneConfigure = AppConfig.dataValid;
    #endif
    return;
}

void WFEasyConfigMgr()
{
    if (CFGCXT.isWifiNeedToConfigure) {
        if (WFEasyConfigProcess()) {
            //Has been configured, clear flag
            CFGCXT.isWifiNeedToConfigure = 0;
            CFGCXT.isWifiDoneConfigure = 1;
        }
    }
    return;
}

static int WFEasyConfigProcess(void)
{
    uint8_t ConnectionProfileID;
    uint8_t ConnectionState;

    #if defined (EZ_CONFIG_STALL)
    if (CFGCXT.cfg_state == cfg_stopped)
    {
        /* State machine just started get current time stamp */
        CFGCXT.cfg_state = cfg_stalled;
        CFGCXT.timeStart = TickGet();
        return 0;
    }

    /* Wait for stall time to expire */
    if (CFGCXT.cfg_state == cfg_stalled)
    {
        uint32_t time = TickGet();
        if ((time - CFGCXT.timeStart) < WF_STALL_TIME_MS)
            return 0;
    }
    #endif // EZ_CONFIG_STALL

    /* We will re-use the current profile */
    WF_CMGetConnectionState(&ConnectionState, &ConnectionProfileID);

    /* Need to disconnect */
    WF_CMDisconnect();

    /* Delete profile */
    WF_CPDelete(ConnectionProfileID);

    /* Create and prepare new profile */
    WF_CPCreate(&ConnectionProfileID);
    AppConfig.passPhraseToKeyFlag = 0;

    /* Now set the various connection profile parameters */

    /* Set SSID... */
    if (CFGCXT.ssid)
#if defined(__XC8)
        WF_CPSetSsid(ConnectionProfileID,
            //(ROM char *)CFGCXT.ssid,   Note (VMH): fixed compile warning - not sure why this is necessary.
            CFGCXT.ssid,
            strlen(CFGCXT.ssid));
#else
        WF_CPSetSsid(ConnectionProfileID,
            CFGCXT.ssid,
            strlen((char*)CFGCXT.ssid));
#endif

#if WF_HOST_DERIVE_KEY_FROM_PASSPHRASE == WF_ENABLED
        if ((uint8_t)CFGCXT.security == WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE)
        {
            WF_ConvPassphraseToKey(strlen((char *)CFGCXT.key), CFGCXT.key, strlen((char*)CFGCXT.ssid), CFGCXT.ssid);
            CFGCXT.security--;
            #if MY_DEFAULT_NETWORK_TYPE == WF_ADHOC
                if (AppConfig.SecurityMode == WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE) {
                    WF_ConvPassphraseToKey(AppConfig.SecurityKeyLength, AppConfig.SecurityKey,
                    AppConfig.SsidLength, AppConfig.MySSID);
                    AppConfig.SecurityMode--;
                    AppConfig.SecurityKeyLength = 32;
                    AppConfig.passPhraseToKeyFlag = 1;
                }
            #endif /* MY_DEFAULT_NETWORK_TYPE == WF_ADHOC */
        }
#endif /* #if WF_HOST_DERIVE_KEY_FROM_PASSPHRASE == WF_ENABLED */

    /* Now deal with security... */
    switch ((uint8_t)CFGCXT.security) {
    case WF_SECURITY_OPEN: /* No security */
        WF_CPSetSecurity(ConnectionProfileID, WF_SECURITY_OPEN, 0, 0, 0);
        break;

    case WF_SECURITY_WEP_40:
        {
            uint8_t  keys[20];
            int   i;

            if (CFGCXT.key) {
                /* Clear key */
                for (i = 0; i < 20; i++)
                    keys[i] = 0;
                memcpy(keys, (void*)CFGCXT.key, 20);
                WF_CPSetSecurity(ConnectionProfileID, WF_SECURITY_WEP_40, CFGCXT.defaultWepKey, keys, 20);
            }
        }
        break;

    case WF_SECURITY_WEP_104:
        {
            uint8_t  keys[52];
            int   i;

            if (CFGCXT.key) {
                /* Clear key */
                for (i = 0; i < 52; i++)
                    keys[i] = 0;
                memcpy(keys, (void*)CFGCXT.key, 52);
                WF_CPSetSecurity(ConnectionProfileID, WF_SECURITY_WEP_104, CFGCXT.defaultWepKey, keys, 52);
            }
        }
        break;

    case WF_SECURITY_WPA_AUTO_WITH_KEY:
        if (CFGCXT.key) {
            WF_CPSetSecurity(ConnectionProfileID, WF_SECURITY_WPA_AUTO_WITH_KEY, 0, CFGCXT.key, 32);
        }
        break;

    case WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE:
        if (CFGCXT.key) {
            WF_CPSetSecurity(ConnectionProfileID, WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE, 0, CFGCXT.key, strlen((char *)CFGCXT.key));
        }
        break;
    }

    #if defined (EZ_CONFIG_STORE)
    SaveAppConfig(&AppConfig);
    #endif

    /* Set wlan mode */
    WF_CPSetNetworkType(ConnectionProfileID, CFGCXT.type);

    if (AppConfig.networkType == WF_INFRASTRUCTURE)
        WF_CASetListRetryCount(MY_DEFAULT_LIST_RETRY_COUNT_INFRASTRUCTURE);

#if WF_MODULE_CONNECTION_MANAGER == WF_DISABLED 
        WF_DisableModuleConnectionManager();
#endif

#if MY_DEFAULT_NETWORK_TYPE == WF_SOFT_AP
    // SoftAP: To allow redirection, need to hibernate before changing network type. Module FW has SoftAP flag and therefore hibernate mode
    //             is needed to clear this indication and allow proper network change.
    //             This should work for non-SoftAP. But these have not been tested yet.

    // Operation of hibernate mode
    //     Turned off LDO of the MRF24W module, which is turning off the power completely. Has same effect of resetting the module.
    //

    // FW flow of hibernate mode:
    // Ensure WF_USE_POWER_SAVE_FUNCTIONS is enabled.
    // In main() loop, StackTask() -> MACProcess() will be called. It will invoke CheckHibernate(), which executes/handles
    // hibernate mode based on WF_hibernate.state and  WF_hibernate.wakeup_notice.
    WF_hibernate.state = WF_HB_ENTER_SLEEP;
    WF_hibernate.wakeup_notice = false;
    //WFConsolePrintRomStr("SoftAP redirection: Put Wi-Fi module into hibernate mode.", true);

    DelayMs(50); // SOFTAP_ZEROCONF_SUPPORT. Timing reduced from 200 to 50.

    WF_hibernate.wakeup_notice = true;
    //WFConsolePrintRomStr("Wakeup Wi-Fi module.", true);
#else
    /* Kick off connection now... */
    WF_CMConnect(ConnectionProfileID);
#endif

    /* Change state and return true to show we are done! */
    CFGCXT.cfg_state = cfg_stopped;

    return 1;
}
#endif /* STACK_USE_EZ_CONFIG */

#if defined ( EZ_CONFIG_SCAN )
void WFInitScan(void)
{
    SCANCXT.scanState = 0;
    SCANCXT.numScanResults = 0;
    SCANCXT.displayIdx = 0;

    return;
}

//uint8_t state_SavedBeforeScan = 0, ID_SavedBeforeScan = 0;

uint16_t WFStartScan(void)
{
    // save the state and ID
    //WF_CMCheckConnectionState(&state_SavedBeforeScan, &ID_SavedBeforeScan);

    /* If scan already in progress bail out */
    if (IS_SCAN_IN_PROGRESS(SCANCXT.scanState))
        return WF_ERROR_OPERATION_CANCELLED;

    if (WF_Scan(WF_SCAN_ALL) != WF_SUCCESS)
        return WF_ERROR_OPERATION_CANCELLED;

    SCAN_SET_IN_PROGRESS(SCANCXT.scanState);

    return WF_SUCCESS;
}

uint16_t WFRetrieveScanResult(uint8_t Idx, tWFScanResult *p_ScanResult)
{
    if (Idx >= SCANCXT.numScanResults)
        return WF_ERROR_INVALID_PARAM;

    WF_ScanGetResult(Idx, p_ScanResult);
    if (p_ScanResult->ssidLen < WF_MAX_SSID_LENGTH)
        p_ScanResult->ssid[p_ScanResult->ssidLen] = 0; /* Terminate */
    //if (p_ScanResult->ssidLen == WF_MAX_SSID_LENGTH)
    //    p_ScanResult->ssidLen -= 1;
    return WF_SUCCESS;
}

void WFScanEventHandler(uint16_t scanResults)
{
    /* Cache number APs found in scan. Max 60 */
    SCANCXT.numScanResults = scanResults;

    /* Clear the scan in progress */
    SCAN_CLEAR_IN_PROGRESS(SCANCXT.scanState);
    SCAN_SET_VALID(SCANCXT.scanState);

    return;
}
#endif /* EZ_CONFIG_SCAN */

//#if defined ( WF_CONSOLE ) && defined ( EZ_CONFIG_SCAN ) && !defined(__XC8)
#if defined ( EZ_CONFIG_SCAN ) && !defined(__XC8)
#if defined(WF_PRE_SCAN_IN_ADHOC)
void WFGetScanResults(void)
{
    int id;

    if (SCANCXT.numScanResults == 0) {
       SCAN_CLEAR_DISPLAY(SCANCXT.scanState);
       return;
    }

    if (IS_SCAN_IN_PROGRESS(SCANCXT.scanState))
       return;

    if (!IS_SCAN_STATE_VALID(SCANCXT.scanState))
       return;

    for (id = 0; id < SCANCXT.numScanResults; id++)
    {
        WFRetrieveScanResult(id, &preScanResult[id]);
    }
    
    SCAN_CLEAR_DISPLAY(SCANCXT.scanState);
    return;
}
#endif // #if defined(WF_PRE_SCAN_IN_ADHOC)

void WFDisplayScanMgr()
{
    tWFScanResult bssDesc;
    char ssid[WF_MAX_SSID_LENGTH + 1];
    char rssiChan[48];
    int  i;
    char st[80];

    if (SCANCXT.numScanResults == 0) {
       SCAN_CLEAR_DISPLAY(SCANCXT.scanState);
       return;
    }
    if (!IS_SCAN_STATE_DISPLAY(SCANCXT.scanState))
       return;

    if (IS_SCAN_IN_PROGRESS(SCANCXT.scanState))
       return;

    if (!IS_SCAN_STATE_VALID(SCANCXT.scanState))
       return;

    WFRetrieveScanResult(SCANCXT.displayIdx, &bssDesc);
    sprintf(st,"%3d ",SCANCXT.displayIdx);
    putrsUART(st);

    if (bssDesc.bssType == 1)
        sprintf(st,"NetType: Infra.");
    else if (bssDesc.bssType == 2)
        sprintf(st,"NetType: Ad-hoc");

    putrsUART(st);

    sprintf(st,", ESSID:");
    putrsUART(st);

    /* Display SSID */
    for (i = 0; i < bssDesc.ssidLen; i++)
        ssid[i] = bssDesc.ssid[i];

    ssid[bssDesc.ssidLen] = 0;
    putsUART(ssid);

    putrsUART("\r\n");

    /* Display SSID  & Channel */
    sprintf(rssiChan, "\tRSSI: %3u, Channel: %2u", bssDesc.rssi, bssDesc.channel);
    putsUART(rssiChan);

    /* Display BSSID */
#if defined(DUMP_DETAILED_SCAN_RESULTS)
    sprintf(rssiChan, ", BSSID: %02x:%02x:%02x:%02x:%02x:%02x",
            bssDesc.bssid[0],bssDesc.bssid[1],bssDesc.bssid[2],
            bssDesc.bssid[3],bssDesc.bssid[4],bssDesc.bssid[5]);
    putsUART(rssiChan);

    /* Display Security Mode */
    if ((bssDesc.apConfig & 0x10) == 0) // bit4 == 0: open (no security)
    {
        sprintf(rssiChan, ", SecMode: %s\r\n", "Open");
    }
    else // bit4 == 1: security
    {
        if ((bssDesc.apConfig & 0xc0) == 0xc0) // bit7 == 1: WPA2, bit6 == 1: WPA
        {
            sprintf(rssiChan, ", SecMode: %s\r\n", "WPA-PSK/WPA2-PSK");
        }
        else if ((bssDesc.apConfig & 0x80) == 0x80) // bit7 == 1: WPA2
        {
            sprintf(rssiChan, ", SecMode: %s\r\n", "WPA2-PSK");
        }
        else if ((bssDesc.apConfig & 0x40) == 0x40) // bit6 == 1: WPA
        {
            sprintf(rssiChan, ", SecMode: %s\r\n", "WPA-PSK");
        }
        else // bit7 == 0, bit6 == 0, WEP
        {
            sprintf(rssiChan, ", SecMode: %s\r\n", "WEP");
        }
    }
    putsUART(rssiChan);
#endif // DUMP_DETAILED_SCAN_RESULTS

    putrsUART("\r\n");

#if (MY_DEFAULT_NETWORK_TYPE == WF_SOFT_AP)
    preScanResult[SCANCXT.displayIdx]= bssDesc; // WF_PRESCAN
    if (SCANCXT.displayIdx == sizeof(preScanResult) / sizeof(preScanResult[0]) - 1) {
        SCAN_CLEAR_DISPLAY(SCANCXT.scanState);
        SCANCXT.displayIdx = 0;
        #if defined(WF_CONSOLE) & defined(STACK_USE_UART)
        WFConsoleReleaseConsoleMsg();
        #endif
    }
#endif

    if (++SCANCXT.displayIdx == SCANCXT.numScanResults)  {
        SCAN_CLEAR_DISPLAY(SCANCXT.scanState);
        SCANCXT.displayIdx = 0;
#if defined(WF_CONSOLE) & defined(STACK_USE_UART)
        WFConsoleReleaseConsoleMsg();
#endif
    }

    return;
}
#endif /* WF_CONSOLE */

#endif /* WF_CS_TRIS */
