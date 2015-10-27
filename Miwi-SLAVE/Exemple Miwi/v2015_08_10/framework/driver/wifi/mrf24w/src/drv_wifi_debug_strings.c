/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_wifi_debug_strings.c

  Summary:
    

  Description:
    

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

#if defined(STACK_USE_UART)

/* used for assertions */
#if defined(WF_DEBUG)
    #define WF_MODULE_NUMBER   WF_MODULE_WF_DEBUG_STRINGS
#endif

ROM char *connectionFailureStrings[] = {
                                        "NULL",                                  /* 0 - not used */
                                        "NULL",                                  /* 1 - not used */
                                        "WF_JOIN_FAILURE",                       /* 2            */
                                        "WF_AUTHENTICATION_FAILURE",             /* 3            */
                                        "WF_ASSOCIATION_FAILURE",                /* 4            */
                                        "WF_WEP_HANDSHAKE_FAILURE",              /* 5            */
                                        "WF_PSK_CALCULATION_FAILURE",            /* 6            */
                                        "WF_PSK_HANDSHAKE_FAILURE",              /* 7            */
                                        "WF_ADHOC_JOIN_FAILURE",                 /* 8            */
                                        "WF_SECURITY_MISMATCH_FAILURE",          /* 9            */
                                        "WF_NO_SUITABLE_AP_FOUND_FAILURE",       /* 10           */
                                        "WF_RETRY_FOREVER_NOT_SUPPORTED_FAILURE",/* 11           */
                                        "WF_LINK_LOST",                          /* 12           */
                                        "",                   /* 13           */
                                        "WF_RSN_MIXED_MODE_NOT_SUPPORTED",       /* 14           */
                                        "WF_RECV_DEAUTH",                        /* 15           */
                                        "WF_RECV_DISASSOC",                      /* 16           */
                                        "WF_WPS_FAILURE",                         /* 17           */
                                        "WF_P2P_FAILURE",                         /* 18           */
                                        "WF_LINK_DOWN"                         /* 19           */
                                        };

ROM char *connectionLostStrings[] = {
                                        "Association Failure",      /* 0 */
                                        "WF_BEACON_TIMEOUT",        /* 1 */
                                        "WF_DEAUTH_RECEIVED",       /* 2 */
                                        "WF_DISASSOCIATE_RECEIVED", /* 3 */
                                        "WF_TKIP_MIC_FAILURE",      /* 4 */
                                        "WF_LINK_DOWN"              /* 5 */
                                    };

#if defined(MRF24WG) && !defined(__XC8)
    ROM char *deauthDisssocReasonCodes[] =
                                    {
                                        "",                                     /* 0 */
                                        "WF_UNSPECIFIED",                       /* 1 */
                                        "WF_PREV_AUTH_NOT_VALID",               /* 2 */
                                        "WF_DEAUTH_LEAVING",                    /* 3 */
                                        "WF_DISASSOC_DUE_TO_INACTIVITY",        /* 4 */
                                        "WF_DISASSOC_AP_BUSY",                  /* 5 */
                                        "WF_CLASS2_FRAME_FROM_NONAUTH_STA",     /* 6 */
                                        "WF_CLASS3_FRAME_FROM_NONASSOC_STA",    /* 7 */
                                        "WF_DISASSOC_STA_HAS_LEFT",             /* 8 */
                                        "WF_STA_REQ_ASSOC_WITHOUT_AUTH",        /* 9 */
                                        "",                                     /* 10 */
                                        "",                                     /* 11 */
                                        "",                                     /* 12 */
                                        "WF_INVALID_IE",                        /* 13 */
                                        "WF_MIC_FAILURE",                       /* 14 */
                                        "WF_4WAY_HANDSHAKE_TIMEOUT",            /* 15 */
                                        "WF_GROUP_KEY_HANDSHAKE_TIMEOUT",       /* 16 */
                                        "WF_IE_DIFFERENT",                      /* 17 */
                                        "WF_INVALID_GROUP_CIPHER",              /* 18 */
                                        "WF_INVALID_PAIRWISE_CIPHER",           /* 19 */
                                        "WF_INVALID_AKMP",                      /* 20 */
                                        "WF_UNSUPP_RSN_VERSION",                /* 21 */
                                        "WF_INVALID_RSN_IE_CAP",                /* 22 */
                                        "WF_IEEE8021X_FAILED",                  /* 23 */
                                        "WF_CIPHER_SUITE_REJECTED"              /* 24 */
                                    };

    ROM char *statusCodes[] = {
                                        "",                                     /* 0 */
                                        "WF_UNSPECIFIED_FAILURE",               /* 1 */
                                        "",                                     /* 2 */
                                        "",                                     /* 3 */
                                        "",                                     /* 4 */
                                        "",                                     /* 5 */
                                        "",                                     /* 6 */
                                        "",                                     /* 7 */
                                        "",                                     /* 8 */
                                        "",                                     /* 9 */
                                        "WF_CAPS_UNSUPPORTED",                  /* 10 */
                                        "WF_REASSOC_NO_ASSOC",                  /* 11 */
                                        "WF_ASSOC_DENIED_UNSPEC",               /* 12 */
                                        "WF_NOT_SUPPORTED_AUTH_ALG",            /* 13 */
                                        "WF_UNKNOWN_AUTH_TRANSACTION",          /* 14 */
                                        "WF_CHALLENGE_FAIL",                    /* 15 */
                                        "WF_AUTH_TIMEOUT",                      /* 16 */
                                        "WF_AP_UNABLE_TO_HANDLE_NEW_STA",       /* 17 */
                                        "WF_ASSOC_DENIED_RATES",                /* 18 */
                                        "WF_ASSOC_DENIED_NOSHORTPREAMBLE",      /* 19 */
                                        "WF_ASSOC_DENIED_NOPBCC",               /* 20 */
                                        "WF_ASSOC_DENIED_NOAGILITY",            /* 21 */
                                        "",                                     /* 22 */
                                        "",                                     /* 23 */
                                        "",                                     /* 24 */
                                        "WF_ASSOC_DENIED_NOSHORTTIME",          /* 25 */
                                        "WF_ASSOC_DENIED_NODSSSOFDM",           /* 26 */
                                        "",                                     /* 27 */
                                        "",                                     /* 28 */
                                        "",                                     /* 29 */
                                        "",                                     /* 30 */
                                        "",                                     /* 31 */
                                        "",                                     /* 32 */
                                        "",                                     /* 33 */
                                        "",                                     /* 34 */
                                        "",                                     /* 35 */
                                        "",                                     /* 36 */
                                        "",                                     /* 37 */
                                        "",                                     /* 38 */
                                        "",                                     /* 39 */
                                        "WF_NOT_VALID_IE",                      /* 40 */
                                        "WF_NOT_VALID_GROUPCIPHER",             /* 41 */
                                        "WF_NOT_VALID_PAIRWISE_CIPHER",         /* 42 */
                                        "WF_NOT_VALID_AKMP",                    /* 43 */
                                        "WF_UNSUPPORTED_RSN_VERSION",           /* 44 */
                                        "WF_INVALID_RSN_IE_CAP",                /* 45 */
                                        "WF_CIPHER_SUITE_REJECTED",             /* 46 */
                                        "WF_TIMEOUT"                            /* 47 */
                              };

    ROM char *wpsState[] = {
                                        "NONE",
                                        "EAPOL_START",
                                        "EAP_REQ_IDENTITY",
                                        "EAP_RSP_IDENTITY",
                                        "EAP_WPS_START",
                                        "EAP_RSP_M1",
                                        "EAP_REQ_M2",
                                        "EAP_RSP_M3",
                                        "EAP_REQ_M4",
                                        "EAP_RSP_M5",
                                        "EAP_REQ_M6",
                                        "EAP_RSP_M7",
                                        "EAP_REQ_M8",
                                        "EAP_RSP_DONE",
                                        "EAP_FAILURE"
                           };

    ROM char *wpsConfigErr[] = {
                                        "NOERR",
                                        "SESSION_OVERLAPPED",
                                        "DECRYPT_CRC_FAILURE",
                                        "24G_NOT_SUPPORTED",
                                        "RETRY_FAILURE",
                                        "INVALID_MSG",
                                        "AUTH_FAILURE",
                                        "ASSOC_FAILURE",
                                        "MSG_TIMEOUT",
                                        "SESSION_TIMEOUT",
                                        "DEVPASSWD_AUTH_FAILURE", /* 10 */
                                        "NO_CONN_TOREG",
                                        "MULTI_PBC_DETECTED",
                                        "EAP_FAILURE",
                                        "DEV_BUSY",
                                        "SETUP_LOCKED"
                               };

    static ROM char *P2PErr[] = {
                           "WFD_SUCCESS",
                           "WFD_INFO_CURRENTLY_UNAVAILABLE",
                           "WFD_INCOMPATIBLE_PARAMS",
                           "WFD_LIMIT_REACHED",
                           "WFD_INVALID_PARAMS",
                           "WFD_UNABLE_TO_ACCOMMODATE",
                           "WFD_PREV_PROTOCOL_ERROR",
                           "WFD_NO_COMMON_CHANNELS",
                           "WFD_UNKNOWN_GROUP",
                           "",
                           "WFD_INCOMPATIBLE_PROV_METHOD", /* 10 */
                           "WFD_REJECTED_BY_USER",
                           "WFD_NO_MEM",
                           "WFD_INVALID_ACTION",
                           "WFD_TX_FAILURE",
                           "WFD_TIME_OUT"
                };

    static ROM char *P2PState[] = {
                        "P2PSTIdle",
                        "P2PSTScan",
                        "P2PSTListen",
                        "P2PSTFind",
                        "P2PSTStartFormation",
                        "P2PSTGONegoReqDone",
                        "P2PSTGOWaitNegoReqDone",
                        "P2PSTWaitFormationDone",
                        "P2PSTInvite",
                        "P2PSTProvision",
                        "P2PSTClient"
                };
#endif /* MRF24WG */

#if defined(DISPLAY_FILENAME) && !defined(__XC8)
    #if defined(MRF24WG)
        ROM char *moduleName[] = {
            "main.c",
            "drv_wifi_config.c",
            "drv_wifi_eint.c",
            "drv_wifi_spi.c",
            "drv_wifi_mac_24g.c",
            "drv_wifi_param_msg_24g.c",
            "drv_wifi_connection_profile.c",
            "drv_wifi_connection_algorithm.c",
            "drv_wifi_connection_manager.c",
            "drv_wifi_com_24g.c",
            "drv_wifi_init.c",
            "drv_wifi_raw_24g.c",
            "drv_wifi_mgmt_msg_24g.c",
            "drv_wifi_tx_power.c",
            "drv_wifi_power_save.c",
            "drv_wifi_event_handler.c",
            "drv_wifi_scan.c",
            "drv_wifi_debug_strings.c",
            "drv_wifi_auto_update_tcp_client_24g.c",
            "drv_wifi_auto_update_uart_24g.c",
            "iperf_app.c",
            "ftp_client_demo.c",
            "ping_demo.c",
    };
    #else
        ROM char *moduleName[] = {
            "main.c",
            "drv_wifi_config.c",
            "drv_wifi_eint.c",
            "drv_wifi_spi.c",
            "drv_wifi_mac.c",
            "drv_wifi_param_msg.c",
            "drv_wifi_connection_profile.c",
            "drv_wifi_connection_algorithm.c",
            "drv_wifi_connection_manager.c",
            "drv_wifi_com.c",
            "drv_wifi_init.c",
            "drv_wifi_raw.c",
            "drv_wifi_mgmt_msg.c",
            "drv_wifi_tx_power.c",
            "drv_wifi_power_save.c",
            "drv_wifi_event_handler.c",
            "drv_wifi_scan.c",
            "drv_wifi_debug_strings.c",
            "", // used in MRF24WG only
            "", // used in MRF24WG only
            "iperf_app.c",
            "ftp_client_demo.c",
            "ping_demo.c",
        };
    #endif
#endif /* DISPLAY_FILENAME */

static void OutputConnectionTempLostMsg(uint16_t eventInfo);
static void OutputConnectionFailedMsg(uint16_t eventInfo);
static void OutputConnectionPermLostMsg(uint16_t eventInfo);

#if !defined(__XC8)
static void OutputDomainString(void);
static void OutputMacAddress(void);
static void WF_OutputSecurityDebugMessage(uint8_t wepKeyIndex);
#endif

#if (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPS_PIN)
   static bool ValidatePinChecksum(unsigned long int pin);
#endif

#if defined(MRF24WG)
void WF_OutputConnectionContext(void)
{
    tWFConnectContext context;
    int i;
    char buf[6];

#if defined(EZ_CONFIG_STORE)
    if (AppConfig.networkType == WF_INFRASTRUCTURE)  {
#else
    if (true) {
#endif
        WF_CMGetConnectContext(&context);

        putrsUART("\tbssid:    ");
        for (i = 0; i < 6; ++i) {
            if (i < 5) {
                sprintf(buf, "%02X:", context.bssid[i]);
                putsUART(buf);
            } else {
                sprintf(buf, "%02X\r\n", context.bssid[i]);
                putsUART(buf);
            }
        }

        putrsUART("\tchannel:  ");
        sprintf(buf, "%d\r\n", context.channel);
        putsUART(buf);
    }
}
#endif /* MRF24WG */

void WF_OutputConnectionDebugMsg(uint8_t event, uint16_t eventInfo)
{
        if (event == WF_EVENT_CONNECTION_TEMPORARILY_LOST)
        {
            OutputConnectionTempLostMsg(eventInfo);
        }
        else if (event == WF_EVENT_CONNECTION_FAILED)
        {
            OutputConnectionFailedMsg(eventInfo);
        }
        else if (event == WF_EVENT_CONNECTION_PERMANENTLY_LOST)
        {
            OutputConnectionPermLostMsg(eventInfo);
        }
}

static void OutputConnectionTempLostMsg(uint16_t eventInfo)
{
    char buf[8];

#if defined(MRF24WG)
    if (eventInfo & WF_MASK_DEAUTH_REASONCODE)
    {
        putrsUART("Event: Connection Temporarily Lost  -- eventInfo = Received deauth : ");
        #if !defined(__XC8)
            putrsUART(deauthDisssocReasonCodes[eventInfo & ~WF_MASK_DEAUTH_REASONCODE]);
        #endif
    }
    else if (eventInfo & WF_MASK_DISASSOC_REASONCODE)
    {
        putrsUART("Event: Connection Temporarily Lost  -- eventInfo = Received disassoc : ");
        #if !defined(__XC8)
            putrsUART(deauthDisssocReasonCodes[eventInfo & ~WF_MASK_DISASSOC_REASONCODE]);
        #endif
    }
    else
    {
        putrsUART("Event: Connection Temporarily Lost  -- eventInfo = ");
        sprintf(buf, "%d, ", eventInfo);
        putsUART(buf);
        putrsUART(connectionLostStrings[eventInfo]);
    }
#else /* !defined(MRF24WG) */

    putrsUART("Event: Connection Temporarily Lost -- eventInfo = ");
    sprintf(buf, "%d, ", eventInfo);
    putsUART(buf);
    putrsUART(connectionLostStrings[eventInfo]);
#endif  /* defined(MRF24WG) */

    putrsUART("\r\n");
}

static void OutputConnectionFailedMsg(uint16_t eventInfo)
{
#if defined(MRF24WG)
    uint8_t status;
    uint8_t reason;

    status = (uint8_t)(eventInfo >> 8);
    reason = (uint8_t)(eventInfo & 0xff);
    if (status == WF_RECV_DEAUTH || status == WF_RECV_DISASSOC)
    {
        putrsUART("Event: Connection Failed : ");
        putrsUART(connectionFailureStrings[status]);
        putrsUART(" : ");
        #if !defined(__XC8)
            putrsUART(deauthDisssocReasonCodes[reason]);
        #endif

    }
    else if (status == WF_AUTHENTICATION_FAILURE || status == WF_ASSOCIATION_FAILURE)
    {
        putrsUART("Event: Connection Failed : ");
        putrsUART(connectionFailureStrings[status]);
        putrsUART(" : ");
        #if !defined(__XC8)
            putrsUART(statusCodes[reason]);
        #endif
    }
    else if (status == WF_WPS_FAILURE)
    {
        putrsUART("Event: Connection Failed : ");
        putrsUART(connectionFailureStrings[status]);
        putrsUART(" : ");
        #if !defined(__XC8)
            putrsUART(wpsState[reason >> 4]);
            putrsUART(" : ");
            putrsUART(wpsConfigErr[reason & 0x0f]);
        #endif
    }
    else if (status == WF_P2P_FAILURE)
    {
        putrsUART("Event: Connection Failed : ");
        putrsUART(connectionFailureStrings[status]);
        putrsUART(" : ");
        #if !defined(__XC8)
            putrsUART(P2PState[reason >> 4]);
            putrsUART(" : ");
            putrsUART(P2PErr[reason & 0x0f]);
        #endif
    }
    else
    {
        putrsUART("Event: Connection Failed : ");
        putrsUART(connectionFailureStrings[status]);
    }
#else /* !MRF24WG */
    char buf[8];

    putrsUART("Event: Connection Failed  -- eventInfo = ");
    sprintf(buf, "%d, ", eventInfo);
    putsUART(buf);
    putrsUART(connectionFailureStrings[eventInfo]);
#endif    /* defined(MRF24WG) */

    putrsUART("\r\n");
}

static void OutputConnectionPermLostMsg(uint16_t eventInfo)
{
    char buf[8];

#if defined(MRF24WG)
    if (eventInfo & WF_MASK_DEAUTH_REASONCODE)
    {
        putrsUART("Event: Connection Permanently Lost  -- eventInfo = Received deauth : ");
        #if !defined(__XC8)
            putrsUART(deauthDisssocReasonCodes[eventInfo & ~WF_MASK_DEAUTH_REASONCODE]);
        #endif
    }
    else if (eventInfo & WF_MASK_DISASSOC_REASONCODE)
    {
        putrsUART("Event: Connection Permanently Lost  -- eventInfo = Received disassoc : ");
        #if !defined(__XC8)
            putrsUART(deauthDisssocReasonCodes[eventInfo & ~WF_MASK_DISASSOC_REASONCODE]);
        #endif
    }
    else
    {
        putrsUART("Event: Connection Permanently Lost  -- eventInfo = ");
        sprintf(buf, "%d, ", eventInfo);
        putsUART(buf);
        putrsUART(connectionLostStrings[eventInfo]);
    }
#else /* !defined(MRF24WG) */
    putrsUART("Event: Connection Permanently Lost -- eventInfo = ");
    sprintf(buf, "%d, ", eventInfo);
    putsUART(buf);
    putrsUART(connectionLostStrings[eventInfo]);
#endif /* defined(MRF24WG) */

    putrsUART("\r\n");
}

void WF_OutputConnectionInfo(const APP_CONFIG *p_AppConfig)
{
    #if !defined(__XC8)
    uint8_t channelList[] = MY_DEFAULT_CHANNEL_LIST;

#if (MY_DEFAULT_NETWORK_TYPE == WF_SOFT_AP)
    uint8_t channelList_softAP_redirect[] = MY_DEFAULT_CHANNEL_LIST_POSTSCAN;
#endif

    int i;
    char buf[64];

#if 1
    #if defined(WF_CONSOLE_DEMO)
        putrsUART("\r\n*** WiFi Console Demo ***\r\n");
        putrsUART("Start WiFi Connect\r\n");
    #elif defined(WF_TCPIP_DEMO)
        putrsUART("\r\n*** WiFi TCP/IP Demo ***\r\n");
        putrsUART("Start WiFi Connect\r\n");
    #elif defined(WF_EASY_CONFIG_DEMO)
        putrsUART("\r\n*** WiFi EZConfig Demo ***\r\n");
        putrsUART("Start WiFi Connect\r\n");
    #endif
#endif

    putrsUART("Domain:          ");

    OutputDomainString();

    OutputMacAddress();

    putrsUART("SSID:            ");
    if (p_AppConfig->MySSID[0] == '\0')
    {
        putrsUART("(none)");
    }
    else
    {
        putsUART((char *)p_AppConfig->MySSID);
    }
    putrsUART("\r\n");

    putrsUART("Network Type:    ");
    #if defined(EZ_CONFIG_STORE) && !defined(WF_CONSOLE_DEMO)   /* if EZConfig demo */
        if (AppConfig.networkType == WF_ADHOC)
        {
            putrsUART("AdHoc\r\n");
        }
        else if (AppConfig.networkType == WF_SOFT_AP)
        {
            putrsUART("SoftAP\r\n");
        }
        else if (AppConfig.networkType == WF_INFRASTRUCTURE)
        {
            putrsUART("Infrastructure\r\n");
        }
        else
        {   // Default to unknown
            putrsUART("Unknown\r\n");
        }
    #else
        #if (MY_DEFAULT_NETWORK_TYPE == WF_ADHOC)
            putrsUART("AdHoc\r\n");
        #elif (MY_DEFAULT_NETWORK_TYPE == WF_P2P)
            putrsUART("WiFi Direct\r\n");
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

    #if MY_DEFAULT_NETWORK_TYPE != WF_SOFT_AP
        putrsUART("Scan Type:       ");
        #if (MY_DEFAULT_SCAN_TYPE == WF_PASSIVE_SCAN)
            putrsUART("Passive Scan\r\n");
        #else
           putrsUART("Active Scan\r\n");
        #endif
    #endif /* MY_DEFAULT_NETWORK_TYPE != WF_SOFT_AP */

    putrsUART("Channel List:    ");

#if (MY_DEFAULT_NETWORK_TYPE == WF_SOFT_AP) // Clean up later......
    if (AppConfig.networkType==WF_SOFT_AP)
    {
        for (i = 0; i < sizeof(channelList); ++i)
        {
            sprintf(buf, "%d", channelList[i]);
            putsUART(buf);

            if (i != sizeof(channelList) - 1)
            {
                putrsUART(", ");
            }
            else
            {
                putrsUART("\r\n");
            }
        }

        if (sizeof(channelList) == 0)
        {
            putrsUART("All channels in domain\r\n");
        }

    }
    else
    {
        if (sizeof(channelList_softAP_redirect) == 0)
        {
            putrsUART("All channels in domain\r\n");
        }
    }
#else
    for (i = 0; i < sizeof(channelList); ++i)
    {
        sprintf(buf, "%d", channelList[i]);
        putsUART(buf);

        if (i != sizeof(channelList) - 1)
        {
            putrsUART(", ");
        }
        else
        {
            putrsUART("\r\n");
        }
    }

    if (sizeof(channelList) == 0)
    {
        putrsUART("All channels in domain\r\n");
    }
#endif

    #if MY_DEFAULT_NETWORK_TYPE != WF_SOFT_AP
        putrsUART("Retry Count:     ");
        #if (MY_DEFAULT_LIST_RETRY_COUNT == WF_RETRY_FOREVER)
            putrsUART("Retry Forever\r\n");
        #else
            sprintf(buf, "%d\r\n", MY_DEFAULT_LIST_RETRY_COUNT);
            putsUART(buf);
        #endif

        putrsUART("Beacon Timeout:  ");
        #if (MY_DEFAULT_BEACON_TIMEOUT == 0)
            putrsUART("Ignored\r\n");
        #else
            sprintf(buf, "%d\r\n", MY_DEFAULT_BEACON_TIMEOUT);
            putsUART(buf);
        #endif
    #endif /* MY_DEFAULT_NETWORK_TYPE != WF_SOFT_AP */

    WF_OutputSecurityDebugMessage(AppConfig.WepKeyIndex);

    #if MY_DEFAULT_NETWORK_TYPE != WF_SOFT_AP
        putrsUART("Power Save:      ");
        #if (MY_DEFAULT_PS_POLL == WF_ENABLED)
            putrsUART("Enabled\r\n");
        #else
            putrsUART("Disabled\r\n");
        #endif
    #endif /* MY_DEFAULT_NETWORK_TYPE != WF_SOFT_AP */

    #if defined(WF_CONSOLE_DEMO)
        putrsUART("IP Address:      ");
        #if defined(ENABLE_STATIC_IP)
            for (i = 0; i < 4; ++i)
            {
                sprintf(buf, "%d", (uint8_t)(AppConfig.MyIPAddr.Val >> (8 * i)));
                putsUART(buf);
                if (i != 3)
                {
                    putrsUART(".");
                }
            }
            putrsUART(" (static)\r\n");
        #else
            putrsUART("via DHCP\r\n");
        #endif
    #endif

    #endif /* __XC8 */
}

#if !defined(__XC8)
static void OutputDomainString(void)
{
    #if (MY_DEFAULT_DOMAIN == WF_DOMAIN_FCC)
        putrsUART("FCC");
    #elif (MY_DEFAULT_DOMAIN == WF_DOMAIN_JAPAN)
        putrsUART("Japan");
    #elif (MY_DEFAULT_DOMAIN == WF_DOMAIN_ETSI)
        putrsUART("ETSI");
    #elif (MY_DEFAULT_DOMAIN == WF_DOMAIN_OTHER)
        putrsUART("Other");
    #else
        putrsUART("Unknown Regional Domain!!");
    #endif

    putrsUART("\r\n");
}
#endif /* #if !defined(__XC8) */

#if !defined(__XC8)
static void OutputMacAddress(void)
{
    uint8_t mac[6];
    int i;
    char buf[16];

    WF_GetMacAddress(mac);

    putrsUART("MAC:             ");
    for (i = 0; i < 6; ++i)
    {
        sprintf(buf, "%02X ", mac[i]);
        putsUART(buf);
    }
    putrsUART("\r\n");
}
#endif /* !defined(__XC8) */

#if !defined(__XC8)
static void WF_OutputSecurityDebugMessage(uint8_t wepKeyIndex)
{
    putrsUART("Security:        ");
    switch (AppConfig.SecurityMode) {
    case WF_SECURITY_OPEN:
        putrsUART("Open\r\n");
        break;
    case WF_SECURITY_WPA_AUTO_WITH_KEY:
        if (AppConfig.passPhraseToKeyFlag == 0)
            putrsUART("WPA-PSK/WPA2-PSK Auto with key\r\n");
        else
            putrsUART("WPA-PSK/WPA2-PSK Auto with passphrase\r\n");
        break;
    case WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE:
        putrsUART("WPA-PSK/WPA2-PSK Auto with passphrase\r\n");
        break;
    case WF_SECURITY_WPS_PUSH_BUTTON:
        putrsUART("WPS PBC method\r\n");
        break;
    case WF_SECURITY_WPS_PIN:
        putrsUART("WPS PIN method\r\n");
        break;
    case WF_SECURITY_WEP_40:
    case WF_SECURITY_WEP_104:
        {
            if (AppConfig.SecurityMode == WF_SECURITY_WEP_40)
                putrsUART("WEP40, ");
            else
                putrsUART("WEP104, ");

            #if (MY_DEFAULT_WIFI_SECURITY_WEP_KEYTYPE == WF_SECURITY_WEP_OPENKEY)
                putrsUART("Open Key\r\n");
            #elif (MY_DEFAULT_WIFI_SECURITY_WEP_KEYTYPE == WF_SECURITY_WEP_SHAREDKEY)
                putrsUART("Shared Key\r\n");
            #endif
            {
                char buf[4];

                putrsUART("WEP Key Index:   ");
                sprintf(buf, "%d\r\n", wepKeyIndex);
                putsUART(buf);
            }
        }
        break;
    default:
        WF_ASSERT(false);
        break;
    }
}
#endif /* !defined(__XC8) */

void ValidateConfig(void)
{
#if (MY_DEFAULT_NETWORK_TYPE == WF_INFRASTRUCTURE) ||  (MY_DEFAULT_NETWORK_TYPE == WF_P2P) ||  \
    (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPS_PIN) ||                                  \
    ((MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPS_PUSH_BUTTON) && (MY_DEFAULT_NETWORK_TYPE != WF_P2P))
    char buf[64];
#endif

    #if (   (MY_DEFAULT_NETWORK_TYPE == WF_INFRASTRUCTURE) \
                                 &&                        \
            ((MY_DEFAULT_WIFI_SECURITY_MODE != WF_SECURITY_WPS_PIN) && (MY_DEFAULT_WIFI_SECURITY_MODE != WF_SECURITY_WPS_PUSH_BUTTON)))
        strcpypgm2ram(buf, MY_DEFAULT_SSID_NAME);
        if (strlen(buf) == 0)
        {
            putrsUART("Fatal Error:  Must define an SSID for this configuration\r\n");
            WF_ASSERT(false);
        }
    #endif

    #if (MY_DEFAULT_NETWORK_TYPE == WF_P2P)
        uint8_t channelList[] = MY_DEFAULT_CHANNEL_LIST;
        strcpypgm2ram(buf, MY_DEFAULT_SSID_NAME);
        if (strcmp(buf, "DIRECT-") != 0)
        {
            putrsUART("Fatal Error:  When in P2P the SSID must be 'DIRECT-'\r\n");
            WF_ASSERT(false);
        }

        if (sizeof(channelList) != 3)
        {
            putrsUART("Fatal Error: For P2P, channel list must be 1, 6, 11\r\n");
            WF_ASSERT(false);
        }

        if ((channelList[0] != 1) || (channelList[1] != 6) || (channelList[2] != 11))
        {
            putrsUART("Fatal Error: For P2P, channel list must be 1, 6, 11\r\n");
            WF_ASSERT(false);
        }
    #endif

    #if (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPS_PIN)
        strcpypgm2ram(buf, MY_DEFAULT_SSID_NAME);
        if (strlen(buf) == 0)
        {
            putrsUART("Fatal Error: For WF_SECURITY_WPS_PIN, an SSID must be defined\r\n");
            WF_ASSERT(false);
        }

        #if !defined(MY_DEFAULT_WPS_PIN)
            putrsUART("Fatal Error: If security is WF_SECURITY_WPS_PIN then MY_DEFAULT_WPS_PIN must be defined\r\n");
            WF_ASSERT(false);
        #endif

        {
            strcpypgm2ram(buf, MY_DEFAULT_WPS_PIN);
            unsigned long int numericPin;

            if (strlen(buf) != 8)
            {
                putrsUART("Fatal Error: MY_DEFAULT_WPS_PIN must be exactly 8 digits\r\n");
                WF_ASSERT(false);
            }

        #if !defined(__XC8) // XC8 compiler does not support scanf function
            // validate the pin
            sscanf(buf, "%ld", &numericPin);
            if (!ValidatePinChecksum(numericPin))
            {
                putrsUART("Fatal Error: MY_DEFAULT_WPS_PIN has an invalid checksum\r\n");
                WF_ASSERT(false);
            }
        #endif /* !(__XC8) */
        }
    #endif

    #if (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPS_PUSH_BUTTON) && (MY_DEFAULT_NETWORK_TYPE != WF_P2P)
        strcpypgm2ram(buf, MY_DEFAULT_SSID_NAME);
        if (strlen(buf) != 0)
        {
            putrsUART("Fatal Error: For WF_SECURITY_WPS_PUSH_BUTTON, MY_DEFAULT_SSID_NAME must be defined as \"\" (empty string)\r\n");
            WF_ASSERT(false);
        }
    #endif

    #if (MY_DEFAULT_NETWORK_TYPE != WF_P2P) && \
        ((MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPS_PIN) || (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPS_PUSH_BUTTON))
    {
        uint8_t channelList[] = MY_DEFAULT_CHANNEL_LIST;
        uint8_t i;

        if (sizeof(channelList) != 0)
        {
        #if (MY_DEFAULT_DOMAIN == WF_DOMAIN_FCC)
            if (sizeof(channelList) != 11)
            {
                putrsUART("Fatal Error: If using WPS under FCC must have all 11 channels in list\r\n");
                WF_ASSERT(false);
            }

            for (i = 1; i <= 11; ++i)
            {
                if (channelList[i - 1] != i)
                {
                    putrsUART("Fatal Error: If using WPS under FCC must have channels 1 - 11 in list\r\n");
                    WF_ASSERT(false);
                }
            }
        #elif (MY_DEFAULT_DOMAIN == WF_DOMAIN_ETSI)
            if (sizeof(channelList) != 13)
            {
                putrsUART("Fatal Error: If using WPS under ETSI domain must have all 13 channels in list\r\n");
                WF_ASSERT(false);
            }

            for (i = 1; i <= 13; ++i)
            {
                if (channelList[i - 1] != i)
                {
                    putrsUART("Fatal Error: If using WPS under ETSI domain must have channels 1 - 13 in list\r\n");
                    WF_ASSERT(false);
                }
            }
        #elif (MY_DEFAULT_DOMAIN == WF_DOMAIN_JAPAN)
            if (sizeof(channelList) != 14)
            {
                putrsUART("Fatal Error: If using WPS under ETSI domain must have all 14 channels in list\r\n");
                WF_ASSERT(false);
            }

            for (i = 1; i <= 14; ++i)
            {
                if (channelList[i - 1] != i)
                {
                    putrsUART("Fatal Error: If using WPS under JAPAN domain must have channels 1 - 14 in list\r\n");
                    WF_ASSERT(false);
                }
            }
        #endif
        } // Not using default domain channel list
    }
    #endif

}

 #if (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPS_PIN)
    static bool ValidatePinChecksum(unsigned long int pin)
    {
        unsigned long int accum = 0;
        accum += 3 * ((pin / 10000000) % 10);
        accum += 1 * ((pin / 1000000) % 10);
        accum += 3 * ((pin / 100000) % 10);
        accum += 1 * ((pin / 10000) % 10);
        accum += 3 * ((pin / 1000) % 10);
        accum += 1 * ((pin / 100) % 10);
        accum += 3 * ((pin / 10) % 10);
        accum += 1 * ((pin / 1) % 10);
        return (0 == (accum % 10));
    }
#endif

#endif /* STACK_USE_UART */

/*
*********************************************************************************************************
*                                   WF_AssertionFailed()
*
* Description : Called by a WiFi library function when an assert occurs.
*
* Argument(s) : moduleNumber - module number (located in drv_wifi_api.h)
*
*               lineNumber   - line number within module where assert occurred.
*
* Return(s)   : None
*
* Caller(s)   : WF Driver
*
* Notes:      : (1) If the WF_ASSERT macro is enabled (via the WF_DEBUG define in drv_wifi_config.h) this is the
*                   function that gets called when WF_ASSERT() fails.
*
*               (2) Customize this function as desired to handle assertion errors
*
*********************************************************************************************************
*/
#if defined(WF_DEBUG)
#define WIFI_ASSERT_STRING "WiFi Assert    M:"

void WF_AssertionFailed(uint8_t moduleNumber, uint16_t lineNumber)
{
#if defined(STACK_USE_UART)
    char buf[64];
    #if defined(DISPLAY_FILENAME)
    uint16_t moduleNameIdx;
    #endif

    #if defined(DISPLAY_FILENAME)
        putrsUART("WF ASSERTION at ");
        if (moduleNumber < 100)
            moduleNameIdx = moduleNumber;
        else
            moduleNameIdx = moduleNumber - 81; /* to make index 19 */

        #if defined(__XC8)
            strcpypgm2ram((char*)buf,  moduleName[moduleNameIdx]);
        #else
        sprintf(buf, "%s  ", moduleName[moduleNameIdx]);
        #endif /* __XC8 */

    #else /* not displaying file name, just module number */
        putrsUART("WF ASSERTION: Module Number = ");
        sprintf(buf, "%d  ", moduleNumber);
    #endif /* DISPLAY_FILENAME */

    putsUART(buf);

    putrsUART("Line Number = ");

    sprintf(buf, "%d\r\n", lineNumber);
    putsUART(buf);
#endif /* STACK_USE_UART */

    #if defined(USE_LCD)
    {
        char buf[] = {WIFI_ASSERT_STRING};
        memset(LCDText, ' ', sizeof(LCDText));
        memcpy((void *)LCDText, (void *)buf, strlen(buf));
        uitoa(moduleNumber, (uint8_t*)buf);
        memcpy((void *)&LCDText[18], (void *)buf, strlen(buf));
        LCDText[23] = 'L';
        LCDText[24] = ':';
        uitoa(lineNumber, &LCDText[25]);
        LCDUpdate();
    }
    #endif

    while (1)
        ;
}

#if defined(MRF24WG)
void WF_DisplayModuleAssertInfo()
{
    #if defined(STACK_USE_UART)
    char buf[16];
    uint32_t assertInfo;

    assertInfo = (((uint32_t)Read16BitWFRegister(WF_HOST_MAIL_BOX_0_MSW_REG)) << 16) | Read16BitWFRegister(WF_HOST_MAIL_BOX_0_LSW_REG);
    putrsUART("MRF24G module ASSERTED -- assertInfo = ");
    sprintf(buf, "M:%d, I:%d", (int)(assertInfo >> 24), (int)(assertInfo & 0xffffff));
    putsUART(buf);
    putrsUART("\r\n");
    #endif
}
#endif /* MRF24WG */

#endif /* WF_DEBUG */

#endif /* WF_CS_TRIS */
