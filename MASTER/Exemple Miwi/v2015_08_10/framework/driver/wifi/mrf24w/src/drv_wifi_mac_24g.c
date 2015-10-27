/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_wifi_mac_24g.c

  Summary:
    Module for Microchip TCP/IP Stack
    -Provides access to MRF24W WiFi controller
    -Reference: MRF24W Data sheet, IEEE 802.11 Standard

  Description:
    MRF24W Driver Medium Access Control (MAC) Layer

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
 *                                INCLUDES
 *******************************************************************************/
#include "tcpip/tcpip.h"

#if defined(WF_CS_TRIS)

#include "drv_wifi_mac.h"

#if defined( WF_CONSOLE )
#include "drv_wifi_console.h"
#include "iperf_app.h"
#endif

#if defined(STACK_USE_ZEROCONF_LINK_LOCAL)
#include "tcpip/zero_conf_link_local.h"
#endif

#if defined(STACK_USE_ZEROCONF_MDNS_SD)
#include "tcpip/zero_conf_link_multicast_dns.h"
#endif

/*******************************************************************************
*                             DEFINES
********************************************************************************/
/* used for assertions */
#if defined(WF_DEBUG)
    #define WF_MODULE_NUMBER WF_MODULE_WF_MAC
#endif

#define SNAP_VAL      (0xaa)
#define SNAP_CTRL_VAL (0x03)
#define SNAP_TYPE_VAL (0x00)

#define ETHER_IP  (0x00)
#define ETHER_ARP (0x06)

#if defined( __XC32 )
    #define IPL_MASK ((uint32_t)0x3f << 10)
#endif

#define SNAP_SIZE (6)

#define ENC_PREAMBLE_SIZE   (sizeof(ENC_PREAMBLE))
#define ENC_PREAMBLE_OFFSET (10)

#define WF_RX_PREAMBLE_SIZE   (sizeof(tWFRxPreamble))
#define WF_TX_PREAMBLE_OFFSET (0)

#define WF_TX_PREAMBLE_SIZE (sizeof(tWFTxPreamble))

#define MCHP_DATA_PACKET_SIZE (4 + MAX_PACKET_SIZE + 4)

#define RETRIEVE_BINARY_KEY (defined(WF_CONSOLE_DEMO) && (MY_DEFAULT_WIFI_SECURITY_MODE == WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE))

/*******************************************************************************
 * Rx/Tx Buffer Constants
 * Used to correlate former Ethernet packets to MRF24W packets.
 *******************************************************************************/
#define ENC_RX_BUF_TO_RAW_RX_BUF_ADJUSTMENT       ((RXSTART + ENC_PREAMBLE_SIZE) - (ENC_PREAMBLE_OFFSET + WF_RX_PREAMBLE_SIZE))
#define ENC_TX_BUF_TO_RAW_TX_BUF_ADJUSTMENT       ((TXSTART + WF_TX_PREAMBLE_SIZE) - (WF_TX_PREAMBLE_OFFSET + WF_TX_PREAMBLE_SIZE))
#define ENC_TCB_BUF_TO_RAW_SCRATCH_BUF_ADJUSTMENT (BASE_SCRATCH_ADDR)

/*******************************************************************************
 *                              RAW Constants
 *******************************************************************************/
#define ENC_RD_PTR_ID        (0)
#define ENC_WT_PTR_ID        (1)

/*******************************************************************************
 *                              LOCAL MACROS
 *******************************************************************************/
#if WF_SAVE_WPS_CREDENTIALS == WF_ENABLED
    #define SHORTSWAP(s) ((((s) >> 8) & 0xff) | (((s) << 8) & 0xff00))
    #define LONGSWAP(l) ((((l) >> 24) & 0xff) | (((l) >> 8) & 0xff00) | (((l) << 8) & 0xff0000) | (((l) << 24) & 0xff000000))
#endif

/*******************************************************************************
 *                               LOCAL DATA TYPES
 *******************************************************************************/
#if WF_SAVE_WPS_CREDENTIALS == WF_ENABLED
enum {
    AUTH_OPEN       = 0x01,
    AUTH_WPA_PSK    = 0x02,
    AUTH_SHARED     = 0x04,
    AUTH_WPA        = 0x08,
    AUTH_WPA2       = 0x10,
    AUTH_WPA2_PSK   = 0x20
};

enum {
    ENC_NONE        = 0x01,
    ENC_WEP         = 0x02,
    ENC_TKIP        = 0x04,
    ENC_AES         = 0x08
};

enum {
    WEP_SHORT_KEY_SIZE  = 5,
    WEP_LONG_KEY_SIZE   = 13
};

enum {
    WEP_KEYIDX_MAX = 4,
    MSK_MAX = 64,
    PIN_MAX = 8,
};

typedef struct
{
    uint8_t key_idx;
    uint8_t key[WEP_KEYIDX_MAX][5];
}sec_wep40;

typedef struct
{
    uint8_t key_idx;
    uint8_t key[WEP_KEYIDX_MAX][13];
}sec_wep104;

typedef struct
{
    uint8_t key_len;
    uint8_t key[MSK_MAX];
}sec_wpa_psk;

typedef struct
{
    uint8_t pin[PIN_MAX];
}sec_wps;

typedef union
{
    sec_wep40 wep40;
    sec_wep104 wep104;
    sec_wpa_psk wpa_psk;
    sec_wps wps;
}sec_key;
#endif /* WF_SAVE_WPS_CREDENTIALS == WF_ENABLED */

#if defined(WF_CONSOLE_IFCFGUTIL)
    extern tWFHibernate WF_hibernate;
#else
    tWFHibernate WF_hibernate;
#endif

typedef struct
{
    uint8_t type;
    uint8_t subType;
} tRxPreamble;

/* A header appended at the start of all RX frames by the hardware */
typedef struct
{
    uint16_t         NextPacketPointer;
    RXSTATUS         StatusVector;
    MAC_ADDR         DestMACAddr;
    MAC_ADDR         SourceMACAddr;
    TCPIP_UINT16_VAL Type;
} ENC_PREAMBLE;

typedef struct
{
    uint8_t          snap[SNAP_SIZE];
    MAC_ADDR         DestMACAddr;
    MAC_ADDR         SourceMACAddr;
    TCPIP_UINT16_VAL Type;
} tWFRxPreamble;

typedef struct
{
    uint8_t reserved[4];
} tWFTxPreamble;

/*******************************************************************************
*                           LOCAL GLOBAL VARIABLES
********************************************************************************/

static uint8_t  g_encPtrRAWId[2]; /* indexed by ENC_RD_PTR_ID (0) and ENC_WT_PTR_ID (1).  Values can be: */
                                  /*   RAW_DATA_RX_ID, RAW_DATA_TX_ID, RAW_SCRATCH_ID                    */

/* keeps track of ENC read and write indexes */
static uint16_t g_encIndex[2]; /* index 0 stores current ENC read index, index 1 stores current ENC write index */

uint16_t g_rxBufferLength;
static uint16_t g_txPacketLength;
static uint16_t g_sizeofScratchMemory = 0;
#if defined(WF_DEBUG)
static bool g_txBufferFlushed;
#endif

/*******************************************************************************
*                           LOCAL FUNCTION PROTOTYPES
********************************************************************************/
#if defined(STACK_USE_UART)
extern void WF_OutputConnectionContext(void);
extern void ValidateConfig(void);
#endif

#if WF_SAVE_WPS_CREDENTIALS == WF_ENABLED
static void ConfigWep(tWFWpsCred *cred, uint8_t *secType, sec_key *key);
static void WF_SaveWPSCredentials(uint8_t CpId);
#endif

#if defined(WF_USE_POWER_SAVE_FUNCTIONS)
static void CheckHibernate(void);
#endif

static uint16_t MACIFService(void);
static void SyncENCPtrRAWState(uint8_t encPtrId, uint16_t encIndex);

extern void WF_Connect(void);

//#if defined ( WF_CONSOLE ) && defined ( EZ_CONFIG_SCAN ) && !defined(__XC8)
#if defined ( EZ_CONFIG_SCAN ) && !defined(__XC8)
extern void WFDisplayScanMgr();
#endif

/*******************************************************************************
 *                               Functions
 *******************************************************************************/
uint8_t GetReadPtrRawWindow()
{
    return g_encPtrRAWId[ENC_RD_PTR_ID];
}

/*****************************************************************************
  Function:
    void SyncENCPtrRAWState(uint8_t encPtrId)

  Summary:
    Synchronizes the Ethernet RAM index to the WiFi RAW window index.

  Description:
    Any time stack code changes the index within the 'logical' Ethernet RAM
    this function must be called to assure the RAW driver is synced up with
    where the stack code thinks it is within the Ethernet RAM.  This applies
    to reading/writing tx data, rx data, or tcb data.

    This function is typically called right after g_encIndex[] is updated.

  Precondition:
    None

  Parameters:
    encPtrId -- ENC_RD_PTR_ID or ENC_WT_PTR_ID

  Returns:
      None

  Remarks:
    None
*****************************************************************************/
static void SyncENCPtrRAWState(uint8_t encPtrId, uint16_t encIndex)
{
    uint8_t     rawId;
    uint16_t    rawIndex;
    uint32_t    startTickCount;
    uint32_t    maxAllowedTicks;

    EnsureWFisAwake();

    /* Save the the read or write index value.  Code below will convert it to a */
    /* Raw index                                                                */
    g_encIndex[encPtrId] = encIndex;

    /*-----------------------------------------------------------------------------*/
    /* if the input read or write pointer is in the Rx section of the Ethernet RAM */
    /*-----------------------------------------------------------------------------*/
    if (g_encIndex[encPtrId] < TXSTART)
    {
        /* set the rawId */
        rawId = RAW_DATA_RX_ID;

        /* Convert encPtr index to Raw Index */
        rawIndex = g_encIndex[encPtrId] - ENC_RX_BUF_TO_RAW_RX_BUF_ADJUSTMENT;

        // encPtr[encPtrId] < (RXSTART + ENC_PREAMBLE_SIZE) is an error since we don't have
        // the same preamble as the ENC chip
        WF_ASSERT( g_encIndex[encPtrId] >= (RXSTART + ENC_PREAMBLE_SIZE) );

    }
    /*----------------------------------------------------------------------------------*/
    /* else if the input read or write pointer is in the Tx section of the Ethernet RAM */
    /*----------------------------------------------------------------------------------*/
    else if (g_encIndex[encPtrId] < BASE_SCRATCH_ADDR/*BASE_TCB_ADDR*/)
    {
        /* if the Tx data raw window has not yet been allocated (the stack is creating a new Tx data packet) */
        if (GetRawWindowState(RAW_DATA_TX_ID) != WF_RAW_DATA_MOUNTED)
        {
            /* WiFi chip may be in processs of transmitting a prior tx packet, so it is very possible that  */
            /* we won't be able to allocate a tx data packet instantly.  Set up timer and wait a reasonable */
            /* amount of time.                                                                              */
            maxAllowedTicks = TICKS_PER_SECOND * 6;  /* 6 second timeout, needed if scan in progress */
            startTickCount = (uint32_t)TickGet();
            while ( !AllocateDataTxBuffer(MCHP_DATA_PACKET_SIZE) )
            {
                /* If timed out than lock up -- something bad happened */
                if (TickGet() - startTickCount >= maxAllowedTicks)
                {
                    WF_ASSERT(false);  /* timeout occurred */
                }
            }
        }

        /* set the rawId */
        rawId = RAW_DATA_TX_ID;

        /* convert enc Ptr index to raw index */
        rawIndex = g_encIndex[encPtrId] - ENC_TX_BUF_TO_RAW_TX_BUF_ADJUSTMENT;

        /* encPtr[encPtrId] < BASE_TX_ADDR is an error since we don't have the same  */
        /* pre-BASE_TX_ADDR or post tx buffer as the ENC chip                        */
        WF_ASSERT((g_encIndex[encPtrId] >= BASE_TX_ADDR) && (g_encIndex[encPtrId] <= (BASE_TX_ADDR + MAX_PACKET_SIZE)));
    }
    /*----------------------------------------------------------------------------------*/
    /* else input read or write pointer is in the TCB (Scratch) section of Ethernet RAM */
    /*----------------------------------------------------------------------------------*/
    else
    {
        /* set the raw Id, scratch is always mounted during intialization */
        rawId = RAW_SCRATCH_ID;

        /* convert Enc ptr index to raw index */
        rawIndex = g_encIndex[encPtrId] - ENC_TCB_BUF_TO_RAW_SCRATCH_BUF_ADJUSTMENT;
    }

    /* set the raw index in the specified raw window */
    RawSetIndex(rawId, rawIndex);

    /* Update state variable so we know which raw window is being indexed by the read or write index.  */
    /* Will be either RAW_DATA_RX_ID, RAW_DATA_TX_ID, or RAW_SCRATCH_ID.                               */
    g_encPtrRAWId[encPtrId] = rawId;
}

#if defined(STACK_CLIENT_MODE) && defined(USE_GRATUITOUS_ARP)
//following is the workaround algorithm for the 11Mbps broadcast bugfix
extern bool ARPSendPkt(uint32_t SrcIPAddr, uint32_t DestIPAddr, uint8_t op_req );
int WFArpBroadcastIntervalSec = 5; //interval in seconds, default to 5, can be changed
/*****************************************************************************
 * FUNCTION: WFPeriodicGratuitousArp
 *
 * RETURNS: None
 *
 * PARAMS:
 *          None
 *
 *  NOTES: this is a workaround algorithm for a bug appearing on some APs: they broadcasts
           ARP Request over basic rate at 11Mpbs, that leaves our devices in dark. Here
           we do ARP Request in the beginning for all the memebers in the subnet, then
           periodically do Gratuitous ARP to keep ourselves alive with the AP
 *****************************************************************************/
void WFPeriodicGratuitousArp(void)
{
    static uint32_t oldTime = 0, currTime;
    static uint8_t op_req = ARP_OPERATION_REQ;

    if (!MACIsLinked())
    {
        return;
    }

    currTime = TickGet();

    if ( (currTime < oldTime) //wrap-around case
            ||
         ((currTime - oldTime) > WFArpBroadcastIntervalSec*TICK_SECOND)
        )
    {
        op_req = op_req == ARP_OPERATION_REQ ? ARP_OPERATION_RESP : ARP_OPERATION_REQ;
        ARPSendPkt(*(uint32_t *)&AppConfig.MyIPAddr, *(uint32_t *)&AppConfig.MyIPAddr, op_req );
        oldTime = currTime;
    }
}
#endif // defined(STACK_CLIENT_MODE) && defined(USE_GRATUITOUS_ARP)

#if WF_SAVE_WPS_CREDENTIALS == WF_ENABLED
/*******************************************************************************
  Function:
    static uint8_t ConvAscii2Hex(uint8_t a)

  Summary:
    ConvAsciiKey2Hex helper function.

  Description:
    Converts from ASCII to HEX values.  Used mainly for WEP security modes.
    Called by ConvAsciiKey2Hex function.

  Precondition:
    MACInit must be called first.

  Parameters:
    a - value to convert

  Returns:
    Converted value.

  Remarks:
    None.
  *****************************************************************************/
static uint8_t ConvAscii2Hex(uint8_t a)
{
    if (a >= '0' && a <= '9')
        return (uint8_t)(a - 48);
    if (a >= 'a' && a <= 'f')
        return (uint8_t)(a - 97 + 10);
    if (a >= 'A' && a <= 'F')
        return (uint8_t)(a - 65 + 10);

    return '?';
}

/*******************************************************************************
  Function:
    static void ConvAsciiKey2Hex(uint8_t *key, uint8_t keyLen, uint8_t *hexKey)

  Summary:
    Converts from ASCII to HEX values.

  Description:
    Converts from ASCII to HEX values.  Used mainly for WEP security modes.

  Precondition:
    MACInit must be called first.

  Parameters:
    *key - pointer to security key in ASCII
    keyLen - length of key
    *hexKey - pointer to security key in HEX

  Returns:
    None.

  Remarks:
    None.
  *****************************************************************************/
static void ConvAsciiKey2Hex(uint8_t *key, uint8_t keyLen, uint8_t *hexKey)
{
    uint8_t i;

    for (i = 0; i < keyLen; i += 2) {
        hexKey[i / 2] = ConvAscii2Hex(key[i]) << 4;
        hexKey[i / 2] |= ConvAscii2Hex(key[i + 1]);
    }
}

/*******************************************************************************
  Function:
    static void ConfigWep(tWFWpsCred *cred, uint8_t *secType, sec_key *key)

  Summary:
    Configures WEP security mode from WPS credentials data

  Description:
    Configures WEP security mode from WPS credentials data. Based on key length,
    determine whether it is WF_SECURITY_WEP_40 or WF_SECURITY_WEP_104. Perform
    a key conversion to hex key values.

  Precondition:
    MACInit must be called first.

  Parameters:
    *cred - pointer to WPS credentials
    *secType - pointer to security mode type
    *key - pointer to security key

  Returns:
    None.

  Remarks:
    None.
  *****************************************************************************/
static void ConfigWep(tWFWpsCred *cred, uint8_t *secType, sec_key *key)
{
    uint8_t i;
    uint8_t wep_key[WEP_LONG_KEY_SIZE];
    sec_wep40 *wep_ctx = (sec_wep40 *)key;
    uint8_t *keys = (uint8_t *)wep_ctx + 1;
    uint8_t key_len;

    if (cred->keyLen == WEP_SHORT_KEY_SIZE * 2) {
        *secType = WF_SECURITY_WEP_40;
        ConvAsciiKey2Hex(cred->netKey, cred->keyLen, wep_key);
        key_len = cred->keyLen / 2;
    } else if (cred->keyLen == WEP_SHORT_KEY_SIZE) {
        *secType = WF_SECURITY_WEP_40;
        memcpy(wep_key, cred->netKey, cred->keyLen);
        key_len = cred->keyLen;
    } else if (cred->keyLen == WEP_LONG_KEY_SIZE * 2) {
        *secType = WF_SECURITY_WEP_104;
        ConvAsciiKey2Hex(cred->netKey, cred->keyLen, wep_key);
        key_len = cred->keyLen / 2;
    } else if (cred->keyLen == WEP_LONG_KEY_SIZE) {
        *secType = WF_SECURITY_WEP_104;
        memcpy(wep_key, cred->netKey, cred->keyLen);
        key_len = cred->keyLen;
    } else {
        WF_ASSERT(false);
    }

    for (i = 0; i < 4; i++)
        memcpy(keys + i * key_len, wep_key, key_len);

    wep_ctx->key_idx = cred->keyIdx - 1;
}

/*******************************************************************************
  Function:
    static void WF_SaveWPSCredentials(uint8_t CpId)

  Summary:
    Stores WPS credentials into global variable AppConfig

  Description:
    Reads back WPS credentials from MRF24WG and stores them in global variable AppConfig for future use.
  
  Precondition:
    MACInit must be called first.

  Parameters:
    CpId - Connection Profile ID

  Returns:
    None.

  Remarks:
    None.
  *****************************************************************************/
static void WF_SaveWPSCredentials(uint8_t CpId)
{
    tWFWpsCred cred;
    sec_key key;
    uint8_t *psk;
    static bool once = false;

    if (!once) {
        WF_CPGetWPSCredentials(CpId, &cred);
        cred.authType = SHORTSWAP(cred.authType);
        cred.encType = SHORTSWAP(cred.encType);
        memcpy((void *)AppConfig.MySSID, (void *)cred.ssid, cred.ssidLen);
        AppConfig.SsidLength = cred.ssidLen;
        switch (cred.authType) {
        case AUTH_OPEN:
            if (cred.encType == ENC_NONE) {
                AppConfig.SecurityMode = WF_SECURITY_OPEN;
            } else if (cred.encType == ENC_WEP) {
                ConfigWep(&cred, &AppConfig.SecurityMode, &key);
                if (AppConfig.SecurityMode == WF_SECURITY_WEP_40) {
                    memcpy((void *)AppConfig.SecurityKey, (void *)key.wep40.key, WEP_SHORT_KEY_SIZE * 4);
                    AppConfig.SecurityKeyLength = WEP_SHORT_KEY_SIZE * 4;
                    AppConfig.WepKeyIndex = key.wep40.key_idx;
                } else if (AppConfig.SecurityMode == WF_SECURITY_WEP_104) {
                    memcpy((void *)AppConfig.SecurityKey, (void *)key.wep104.key, WEP_LONG_KEY_SIZE * 4);
                    AppConfig.SecurityKeyLength = WEP_LONG_KEY_SIZE * 4;
                    AppConfig.WepKeyIndex = key.wep104.key_idx;
                } else {
                    WF_ASSERT(false);
                }
            }
            break;
        case AUTH_SHARED:
            ConfigWep(&cred, &AppConfig.SecurityMode, &key);
            if (AppConfig.SecurityMode == WF_SECURITY_WEP_40) {
                memcpy((void *)AppConfig.SecurityKey, (void *)key.wep40.key, WEP_SHORT_KEY_SIZE * 4);
                AppConfig.SecurityKeyLength = WEP_SHORT_KEY_SIZE * 4;
                AppConfig.WepKeyIndex = key.wep40.key_idx;
            } else if (AppConfig.SecurityMode == WF_SECURITY_WEP_104) {
                memcpy((void *)AppConfig.SecurityKey, (void *)key.wep104.key, WEP_LONG_KEY_SIZE * 4);
                AppConfig.SecurityKeyLength = WEP_LONG_KEY_SIZE * 4;
                AppConfig.WepKeyIndex = key.wep104.key_idx;
            } else {
                WF_ASSERT(false);
            }
            break;
        case AUTH_WPA_PSK:
        case AUTH_WPA2_PSK:
            psk = (uint8_t *)AppConfig.SecurityKey;
            memset((void *)psk, 0x00, 64);
            if (cred.keyLen == 64) {
                AppConfig.SecurityMode = WF_SECURITY_WPA_AUTO_WITH_KEY;
                AppConfig.SecurityKeyLength = 32;
                ConvAsciiKey2Hex(cred.netKey, cred.keyLen, psk);
            } else if (cred.keyLen >= 8 && cred.keyLen < 64) {
                AppConfig.SecurityMode = WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE;
                AppConfig.SecurityKeyLength = cred.keyLen;
                if (AppConfig.SecurityKeyLength > 8
                    && cred.netKey[AppConfig.SecurityKeyLength - 1] == '\0')
                    --AppConfig.SecurityKeyLength;
                memcpy(psk, cred.netKey, AppConfig.SecurityKeyLength);
            }
            break;
        default:
            WF_ASSERT(false);
            break;
        }
        once = true;
    }
}
#endif /* #if WF_SAVE_WPS_CREDENTIALS == WF_ENABLED */

// if WPA Passphrase being used
#if RETRIEVE_BINARY_KEY == true
/* 
 * Used when connected using a security pass-phrase.  After the
 * connection is successful, this code will retrieve the binary pass phrase and
 * update AppConfig with it.  It will also change AppConfig.SecurityMode to use
 * the binary key for future connections, specifically after a hibernate and wakeup.
 */
void RetrieveBinaryKey(uint8_t cpid)
{
    uint8_t securityType;
    uint8_t wepKeyIndex;

    /* copy binary key into AppConfig */
    WF_CPGetSecurity(cpid, &securityType, &wepKeyIndex, AppConfig.SecurityKey, &AppConfig.SecurityKeyLength);
    if (AppConfig.SecurityMode == WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE)
        AppConfig.SecurityMode -= 1;
}
#endif /* RETRIEVE_BINARY_KEY == true */

#if defined(WF_USE_POWER_SAVE_FUNCTIONS)
/*******************************************************************************
  Function:
    void CheckHibernate()

  Summary:
    Enters or exits from MRF24W hibernate mode.

  Description:
    Achieves maximum power savings. WF_USE_POWER_SAVE_FUNCTIONS must be enabled.
    In hibernate mode, it will turn off LDO of the MRF24W module, which is turning off the
    power completely. It has the same effect of resetting the module.

    MRF24W state is not maintained when it transitions to hibernate mode.
    To remove the MRF24W from hibernate mode call WF_Init().

  Precondition:
    MACInit must be called first.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    Refer to WFEasyConfigProcess() for a working example of softAP using hibernate mode.
  *****************************************************************************/
static void CheckHibernate(void)
{
    //#if defined( WF_CONSOLE_IFCFGUTIL ) // Resolve compilation errors if WF_CONSOLE_IFCFGUTIL is disable for SoftAP.
    uint8_t state, cpid;
    //#endif

    //#if defined( WF_CONSOLE_IFCFGUTIL )
         if ((WF_hibernate.wakeup_notice) && (WF_hibernate.state == WF_HB_WAIT_WAKEUP))
         {
            DelayMs(100); // SOFTAP_ZEROCONF_SUPPORT  Timing reduced from 200 to 100.

            AppConfig.hibernateFlag = 1;

            WF_hibernate.state = WF_HB_NO_SLEEP;
            StackInit();
            #if defined(WF_CONSOLE_DEMO) || (defined(WF_EASY_CONFIG_DEMO) && defined(__XC32))
            IperfAppInit();
            #endif

           // SOFTAP_ZEROCONF_SUPPORT.
           // SoftAP uses hibernate mode for redirection to another network. This is needed to clear SoftAP flag in RF module FW.
           // Therefore need to perform the proper reset sequences.
           #if defined(STACK_USE_ZEROCONF_LINK_LOCAL)
           ZeroconfLLInitialize();
           #endif

           #if defined(STACK_USE_ZEROCONF_MDNS_SD)
           mDNSServiceDeRegister();
           mDNSInitialize(MY_DEFAULT_HOST_NAME);
           mDNSServiceRegister(
               (const char *) "DemoWebServer",        // base name of the service
                "_http._tcp.local",                   // type of the service
                80,                                   // TCP or UDP port, at which this service is available
                ((const uint8_t *)"path=/index.htm"), // TXT info
                1,                                    // auto rename the service when if needed
                NULL,                                 // no callback function
                NULL                                  // no application context
               );
           mDNSMulticastFilterRegister();
           #endif

           WF_Connect();
        }
    //#endif

    //#if defined( WF_CONSOLE_IFCFGUTIL )
       wait_console_input:
    //#endif

    #if defined(WF_CONSOLE)
        WFConsoleProcess();
        #if defined( WF_CONSOLE_DEMO ) || (defined(WF_EASY_CONFIG_DEMO) && defined(__XC32))
            if (WF_hibernate.state == WF_HB_NO_SLEEP)
            {
                IperfAppCall();
            }
            WFConsoleProcessEpilogue();
        #endif
    #endif

    //#if defined( WF_CONSOLE_IFCFGUTIL )
        if (WF_hibernate.state != WF_HB_NO_SLEEP)
        {
            if (WF_hibernate.state == WF_HB_ENTER_SLEEP)
            {
                // if we are connected then disconnect before going into hibernate state
                WF_CMGetConnectionState(&state, &cpid);
                if ((state == WF_CSTATE_CONNECTED_INFRASTRUCTURE) || (state == WF_CSTATE_CONNECTED_ADHOC))
                {
                    WF_CMDisconnect();
                }

                WF_HibernateEnable();    // Set HIBERNATE pin on MRF24W to HIGH
                WF_hibernate.state = WF_HB_WAIT_WAKEUP;
            }
            if (WF_hibernate.wakeup_notice)
            {
                //continue;
            }
            else
            {
                goto wait_console_input;
            }
        }
    //#endif
}
#endif /* WF_USE_POWER_SAVE_FUNCTIONS */

/*******************************************************************************
  Function:
    void MACProcess()

  Summary:
    Task to execute 802.11 functions and operations

  Description:
    Called from main loop main(), within StackTask() , to support 802.11 operations

  Precondition:
    MACInit must be called first.

  Parameters:
    None.

  Returns:
    None.

  *****************************************************************************/
void MACProcess(void)
{
    static bool oneTimeJobDone = false;
    /* Let 802.11 processes have a chance to run */
    WFProcess();

    if (MACIsLinked())
    {
        if (!oneTimeJobDone)
        {
            #if defined(STACK_USE_UART) && defined(MRF24WG)
                WF_OutputConnectionContext();
            #endif

            // Retrieves binary key if we connected with WPA passphrase and changes AppConfig so we will
            // use the binary key to reconnect
            #if RETRIEVE_BINARY_KEY == true
                RetrieveBinaryKey(1);
            #endif

            #if WF_SAVE_WPS_CREDENTIALS == WF_ENABLED
                WF_SaveWPSCredentials(1);
            #endif    /* #if WF_SAVE_WPS_CREDENTIALS == WF_ENABLED */

            oneTimeJobDone = true;
        }
    }
    else
    {
        oneTimeJobDone = false;
    }

    // checks if going into or coming out of hibernate mode
    #if defined(WF_USE_POWER_SAVE_FUNCTIONS)
    CheckHibernate();
    #endif

    #if defined(STACK_CLIENT_MODE) && defined(USE_GRATUITOUS_ARP)
        //following is the workaround algorithm for the 11Mbps broadcast bugfix
        WFPeriodicGratuitousArp();
    #endif
}

  /******************************************************************************
   * Function:        uint16_t WFGetTCBSize(void)
   *
   * PreCondition:    None
   *
   * Input:           None
   *
   * Output:          Number of bytes in the TCB
   *
   * Side Effects:    None
   *
   * Overview:        Returns number of bytes available in TCP Control Block (TCB) so
   *                  higher-layer code can determine if the number of bytes available
   *                  can support the structures designated to be stored in the TCB.
   *
   * Note:            When running with WiFi the TCB is contained in the Scratch Memory
   *                  on the MRF24W.
   *****************************************************************************/
  uint16_t WFGetTCBSize(void)
  {
      return g_sizeofScratchMemory;
  }

 /******************************************************************************
 * Function:        void MACInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        MACInit sets up the PIC's SPI module and all the
 *                  registers in the MRF24W so that normal operation can
 *                  begin.
 *
 * Note:            None
 *****************************************************************************/
void MACInit(void)
{
    #if defined(STACK_USE_UART)
    ValidateConfig();
    #endif

    WF_Init();
}

/******************************************************************************
 * Function:        void RawInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Initializes the RAW window states.
 *
 * Note:            None
 *****************************************************************************/
void RawInit(void)
{
    /* By default the firmware mounts Scratch to RAW 1 after reset.  If desired,    */
    /* we can read the SysInfo data block from the Scratch.  We are not using this  */
    /* data, so unmount the scratch from this RAW window.                           */
    ScratchUnmount(RAW_ID_1);

    /* Mount scratch memory, index defaults to 0.  This will stay permanently mounted.   */
    /* If one needs to know, this function returns the number of bytes in scratch memory */
    g_sizeofScratchMemory = ScratchMount(RAW_SCRATCH_ID);

    g_encPtrRAWId[ENC_RD_PTR_ID] = RAW_DATA_RX_ID;
    g_encIndex[ENC_RD_PTR_ID]    = BASE_SCRATCH_ADDR;//BASE_TCB_ADDR;

    SetRawWindowState(RAW_DATA_TX_ID, WF_RAW_UNMOUNTED);

    g_encPtrRAWId[ENC_WT_PTR_ID]  = RAW_DATA_TX_ID;
    g_encIndex[ENC_WT_PTR_ID]     = BASE_TX_ADDR;                 // set tx encode ptr (index) to start of tx buf + 4 bytes

    SetRxDataPacketLength(0);                                     // current rx buffer length (none) is 0 bytes
    g_txPacketLength  = 0;                                        // current tx packet length (none) is 0 bytes
    #if defined(WF_DEBUG)
    g_txBufferFlushed = true;                                     // tx buffer is flushed
    #endif

    // from ENC MAC init
    // encWrPtr is left pointing to BASE_TX_ADDR
    // encRdPtr is not initialized... we leave it pointing to BASE_TCB_ADDR

    g_RawIndexPastEnd = 0x00; /* no raw indexes have been set past end of raw window */
}

/******************************************************************************
 * Function:        bool MACIsLinked(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          true: If the PHY reports that a link partner is present
 *                        and the link has been up continuously since the last
 *                        call to MACIsLinked()
 *                  false: If the PHY reports no link partner, or the link went
 *                         down momentarily since the last call to MACIsLinked()
 *
 * Side Effects:    None
 *
 * Overview:        Returns the PHSTAT1.LLSTAT bit.
 *
 * Note:            None
 *****************************************************************************/
bool MACIsLinked(void)
{
    return ( WFisConnected() );
}

/*****************************************************************************
  Function:
    bool MACIsTxReady(void))

  Summary:
    Checks if a tx data buffer is available on the WiFi chip.

  Description:
    Checks if a raw window is currently mounted.  If not, the tx data raw window
    is mounted.

  Precondition:
    MACInit must be called first.

  Parameters:
    None

  Returns:
    True if tx data buffer available, else false

  Remarks:
    None
*****************************************************************************/
bool MACIsTxReady(void)
{
    bool result;

    /* if raw tx data window is in use */
    if (GetRawWindowState(RAW_DATA_TX_ID) == WF_RAW_DATA_MOUNTED)
    {
        result = true;
    }
    /* else raw tx data window is available */
    else
    {

        /* create the new tx buffer */
        result = AllocateDataTxBuffer(MCHP_DATA_PACKET_SIZE);
    }

    return result;
}

void RawGetMgmtRxBuffer(uint16_t *p_numBytes)
{
    /* Mount the mgmt pool rx data, returns number of bytes in mgmt msg.  Read */
    /* index defaults to 0.                                                    */
    *p_numBytes = RawMountRxBuffer(RAW_MGMT_RX_ID);
}

/******************************************************************************
 * Function:        void MACDiscardRx(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Marks the last received packet (obtained using
 *                  MACGetHeader())as being processed and frees the buffer
 *                  memory associated with it
 *
 * Note:            It is safe to call this function multiple times between
 *                  MACGetHeader() calls.  Extra packets won't be thrown away
 *                  until MACGetHeader() makes it available.
 *****************************************************************************/
void MACDiscardRx(void)
{
    /* if we currently have a rx buffer mounted then deallocate it as stack no */
    /* longer needs it.                                                        */
    if ( GetRawWindowState(RAW_DATA_RX_ID) == WF_RAW_DATA_MOUNTED )
    {
        DeallocateDataRxBuffer();
    }
}

/******************************************************************************
 * Function:        uint16_t MACGetFreeRxSize(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          A uint16_t estimate of how much RX buffer space is free at
 *                  the present time.
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 *****************************************************************************/
uint16_t MACGetFreeRxSize(void)
{
    uint16_t size;

    /* if no Rx packet is currently mounted */
    if ( GetRxDataPacketLength() == 0)
    {
        /* TCP/IP stack has entire Rx block available */
        size = RXSIZE - 1;
    }
    /* else a Rx data packet is currently mounted */
    else
    {
        if ( (RXSTOP - RXSTART) > GetRxDataPacketLength() )
        {
            size = (RXSTOP - RXSTART) - GetRxDataPacketLength();
        }
        else
        {
            size = 0;
        }
    }

    return size;
}

/*****************************************************************************
 * FUNCTION: MACIFService
 *
 *
 * RETURNS: Number of bytes in the Data Rx packet if one is received, else 0.
 *
 * PARAMS:  None
 *
 *  NOTES: Called by MACGetHeader() to see if any data packets have been received.
 *         If the MRF24W has received a data packet and the data packet is not
 *         a management data packet, then this function returns the number of
 *         bytes in the data packet. Otherwise it returns 0.
 *****************************************************************************/
static uint16_t MACIFService(void)
{
    uint16_t byteCount;
    tRxPreamble wfPreamble;

    /* check if external interrupt has signalled receipt of a data frame */
    if (!g_HostRAWDataPacketReceived)
    {
        byteCount = 0;  /* return 0 indicating no data rx frame received */
    }
    /* else external interrupt received indicating a data rx frame ready for TCP/IP stack */
    else
    {
        g_HostRAWDataPacketReceived = false; /* clear flag for next data packet */

        /* Mount Read FIFO to RAW Rx window.  Allows use of RAW engine to read rx data packet. */
        /* Function call returns number of bytes in the data packet.                           */
        byteCount = RawMountRxBuffer(RAW_DATA_RX_ID);
        WF_ASSERT(byteCount > 0);
        SetRxDataPacketLength(byteCount);

        /* now that buffer mounted it is safe to reenable interrupts, which were left disabled */
        /* in the WiFi interrupt handler.                                                      */
        WF_EintEnable();

        /* read the data frame internal preamble (type and subtype) to verify that we did, in   */
        /* fact, mount an Rx data packet.  This read auto-increments the raw index to the first */
        /* actual data byte in the frame.                                                       */
        RawGetByte(RAW_DATA_RX_ID, (uint8_t*)&wfPreamble, sizeof(tRxPreamble));
        WF_ASSERT(wfPreamble.type == WF_DATA_RX_INDICATE_TYPE);
    }

    return byteCount;
}

/******************************************************************************
 * Function:        bool MACGetHeader(MAC_ADDR *remote, uint8_t* type)
 *
 * PreCondition:    None
 *
 * Input:           *remote: Location to store the Source MAC address of the
 *                           received frame.
 *                  *type: Location of a uint8_t to store the constant
 *                         MAC_UNKNOWN, ETHER_IP, or ETHER_ARP, representing
 *                         the contents of the Ethernet type field.
 *
 * Output:          true: If a packet was waiting in the RX buffer.  The
 *                        remote, and type values are updated.
 *                  false: If a packet was not pending.  remote and type are
 *                         not changed.
 *
 * Side Effects:    Last packet is discarded if MACDiscardRx() hasn't already
 *                  been called.
 *
 * Overview:        None
 *
 * Note:            None
 *****************************************************************************/
bool MACGetHeader(MAC_ADDR *remote, uint8_t* type)
{
    uint16_t len;
    tWFRxPreamble header;

    /* stack no longer cares about currently mounted rx packet, so clear flag */
    ClearIndexOutOfBoundsFlag(RAW_DATA_RX_ID);

    /* if we currently have a rx buffer mounted then deallocate it as stack no */
    /* longer needs it.                                                        */
    if ( GetRawWindowState(RAW_DATA_RX_ID) == WF_RAW_DATA_MOUNTED )
    {
        DeallocateDataRxBuffer();
    }

    /* check WiFi chip to see if a data frame has been received.  If so, the length of the */
    /* data frame is returned. If no data packet received then 0 will be returned.         */
    len = MACIFService();
    if ( len == 0 )
    {
        return false;
    }

    /* read preamble header.  The RAW index was left at index 2 in the call to MACIFService, which */
    /* is the logical start of the received data frame LLC Header, also known as the SNAP header.  */
    RawRead(RAW_DATA_RX_ID, ENC_PREAMBLE_OFFSET, WF_RX_PREAMBLE_SIZE, (uint8_t *)&header);

    /* as a sanity check verify that the expected bytes contain the SNAP header */
    if (!(header.snap[0] == SNAP_VAL        &&
          header.snap[1] == SNAP_VAL        &&
          header.snap[2] == SNAP_CTRL_VAL   &&
          header.snap[3] == SNAP_TYPE_VAL   &&
          header.snap[4] == SNAP_TYPE_VAL   &&
          header.snap[5] == SNAP_TYPE_VAL) )
    {
        /* if a vendor proprietary packet, throw away */
        DeallocateDataRxBuffer();
        return false;
    }

#if 0
/* this should no longer be needed */
    /* we can flush any saved RAW state now by saving and restoring the current rx buffer.  */
    PushRawWindow(RAW_DATA_RX_ID);
    PopRawWindow(RAW_DATA_RX_ID);
#endif

    // set RAW pointer to 802.11 payload
    RawSetIndex(RAW_DATA_RX_ID, (ENC_PREAMBLE_OFFSET + WF_RX_PREAMBLE_SIZE));

    g_encPtrRAWId[ENC_RD_PTR_ID] = RAW_DATA_RX_ID;
    g_encIndex[ENC_RD_PTR_ID]    = RXSTART + sizeof(ENC_PREAMBLE);

    // The EtherType field, like most items transmitted on the Ethernet medium
    // are in big endian.
    header.Type.Val = swaps(header.Type.Val);

    // Return the Ethernet frame's Source MAC address field to the caller
    // This parameter is useful for replying to requests without requiring an
    // ARP cycle.
    memcpy((void*)remote->v, (void*)header.SourceMACAddr.v, sizeof(*remote));

    // Return a simplified version of the EtherType field to the caller
    *type = MAC_UNKNOWN;
    if ( (header.Type.v[1] == 0x08u) &&
        ((header.Type.v[0] == ETHER_IP) || (header.Type.v[0] == ETHER_ARP)) )
    {
        *type = header.Type.v[0];
    }

    return true;
}

/******************************************************************************
 * Function:        void MACPutHeader(MAC_ADDR *remote, uint8_t type, uint16_t dataLen)
 *
 * PreCondition:    MACIsTxReady() must return true.
 *
 * Input:           *remote: Pointer to memory which contains the destination
 *                           MAC address (6 bytes)
 *                  type: The constant ETHER_ARP or ETHER_IP, defining which
 *                        value to write into the Ethernet header's type field.
 *                  dataLen: Length of the Ethernet data payload
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            Because of the dataLen parameter, it is probably
 *                  advantagous to call this function immediately before
 *                  transmitting a packet rather than initially when the
 *                  packet is first created.  The order in which the packet
 *                  is constructed (header first or data first) is not
 *                  important.
 *****************************************************************************/
void MACPutHeader(MAC_ADDR *remote, uint8_t type, uint16_t dataLen)
{
    uint8_t buf[14];

    #if defined(WF_DEBUG)
    g_txBufferFlushed = false;
    #endif

    g_txPacketLength = dataLen + (uint16_t)sizeof(ETHER_HEADER) + WF_TX_PREAMBLE_SIZE;

    // Set the SPI write pointer to the beginning of the transmit buffer (post WF_TX_PREAMBLE_SIZE)
    SyncENCPtrRAWState(ENC_WT_PTR_ID, TXSTART + WF_TX_PREAMBLE_SIZE);

    /*  write the Ethernet destination address to buffer (6 bytes) */
    memcpy(&buf[0], (void *)remote, sizeof(*remote));
    /* write snap header to buffer (6 bytes) */
    buf[6] =  SNAP_VAL;
    buf[7] =  SNAP_VAL;
    buf[8] =  SNAP_CTRL_VAL;
    buf[9] =  SNAP_TYPE_VAL;
    buf[10] = SNAP_TYPE_VAL;
    buf[11] = SNAP_TYPE_VAL;
    /* Write the appropriate Ethernet Type uint16_t for the protocol being used */
    buf[12] = 0x08;
    buf[13] = (type == MAC_IP) ? ETHER_IP : ETHER_ARP;

    /* write buffer to RAW window */
    MACPutArray((uint8_t *)buf, sizeof(buf));
}

/******************************************************************************
 * Function:        void MACPutGeneralHeader(MAC_ADDR *remote, uint16_t type, uint16_t dataLen)
 *
 * PreCondition:    MACIsTxReady() must return true.
 *
 * Input:           *remote: Pointer to memory which contains the destination
 *                           MAC address (6 bytes)
 *                  type:  defining which  value to write into the Ethernet header's type field.
 *                  dataLen: Length of the Ethernet data payload
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            Because of the dataLen parameter, it is probably
 *                  advantagous to call this function immediately before
 *                  transmitting a packet rather than initially when the
 *                  packet is first created.  The order in which the packet
 *                  is constructed (header first or data first) is not
 *                  important.
 *****************************************************************************/
void MACPutGeneralHeader(MAC_ADDR *remote, uint16_t type, uint16_t dataLen)
{
    uint8_t buf[14];

    #if defined(WF_DEBUG)
    g_txBufferFlushed = false;
    #endif

    g_txPacketLength = dataLen + (uint16_t)sizeof(ETHER_HEADER) + WF_TX_PREAMBLE_SIZE;

    // Set the SPI write pointer to the beginning of the transmit buffer (post WF_TX_PREAMBLE_SIZE)
    SyncENCPtrRAWState(ENC_WT_PTR_ID, TXSTART + WF_TX_PREAMBLE_SIZE);

    /*  write the Ethernet destination address to buffer (6 bytes) */
    memcpy(&buf[0], (void *)remote, sizeof(*remote));
    /* write snap header to buffer (6 bytes) */
    buf[6] =  SNAP_VAL;
    buf[7] =  SNAP_VAL;
    buf[8] =  SNAP_CTRL_VAL;
    buf[9] =  SNAP_TYPE_VAL;
    buf[10] = SNAP_TYPE_VAL;
    buf[11] = SNAP_TYPE_VAL;
    /* Write the appropriate Ethernet Type uint16_t for the protocol being used */
    buf[12] = type >> 8;
    buf[13] = type;

    /* write buffer to RAW window */
    MACPutArray((uint8_t *)buf, sizeof(buf));
}

/******************************************************************************
 * Function:        void MACFlush(void)
 *
 * PreCondition:    A packet has been created by calling MACPut() and
 *                  MACPutHeader().
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        MACFlush causes the current TX packet to be sent out on
 *                  the Ethernet medium.  The hardware MAC will take control
 *                  and handle CRC generation, collision retransmission and
 *                  other details.
 *
 * Note:            After transmission completes (MACIsTxReady() returns true),
 *                  the packet can be modified and transmitted again by calling
 *                  MACFlush() again.  Until MACPutHeader() or MACPut() is
 *                  called (in the TX data area), the data in the TX buffer
 *                  will not be corrupted.
 *****************************************************************************/
void MACFlush(void)
{

    /* this function should not be called after the current tx buffer has been transmitted */
    WF_ASSERT(!g_txBufferFlushed);

    #if defined(WF_DEBUG)
    g_txBufferFlushed = true;
    #endif

    EnsureWFisAwake();
    SendRAWDataFrame(g_txPacketLength);
}

/******************************************************************************
 * Function:        void MACSetReadPtrInRx(uint16_t offset)
 *
 * PreCondition:    A packet has been obtained by calling MACGetHeader() and
 *                  getting a true result.
 *
 * Input:           offset: uint16_t specifying how many bytes beyond the Ethernet
 *                          header's type field to relocate the SPI read
 *                          pointer.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        SPI read pointer are updated.  All calls to
 *                  MACGet() and MACGetArray() will use these new values.
 *
 * Note:            RXSTOP must be statically defined as being > RXSTART for
 *                  this function to work correctly.  In other words, do not
 *                  define an RX buffer which spans the 0x1FFF->0x0000 memory
 *                  boundary.
 *****************************************************************************/
void MACSetReadPtrInRx(uint16_t offset)
{
    SyncENCPtrRAWState(ENC_RD_PTR_ID, RXSTART + sizeof(ENC_PREAMBLE) + offset);
}

/******************************************************************************
 * Function:        uint16_t MACSetWritePtr(uint16_t Address)
 *
 * PreCondition:    None
 *
 * Input:           Address: Address to seek to
 *
 * Output:          uint16_t: Old EWRPT location
 *
 * Side Effects:    None
 *
 * Overview:        SPI write pointer is updated.  All calls to
 *                  MACPut() and MACPutArray() will use this new value.
 *
 * Note:            None
 *****************************************************************************/
PTR_BASE MACSetWritePtr(PTR_BASE address)
{
    PTR_BASE oldVal;

    oldVal = g_encIndex[ENC_WT_PTR_ID];

    SyncENCPtrRAWState(ENC_WT_PTR_ID, address);

    return oldVal;
}

/******************************************************************************
 * Function:        uint16_t MACSetReadPtr(uint16_t Address)
 *
 * PreCondition:    None
 *
 * Input:           Address: Address to seek to
 *
 * Output:          uint16_t: Old ERDPT value
 *
 * Side Effects:    None
 *
 * Overview:        SPI write pointer is updated.  All calls to
 *                  MACPut() and MACPutArray() will use this new value.
 *
 * Note:            None
 *****************************************************************************/
PTR_BASE MACSetReadPtr(PTR_BASE address)
{
    PTR_BASE oldVal;

    oldVal = g_encIndex[ENC_RD_PTR_ID];

    SyncENCPtrRAWState(ENC_RD_PTR_ID, address);

    return oldVal;
}

/******************************************************************************
 * Function:        uint16_t MACCalcRxChecksum(uint16_t offset, uint16_t len)
 *
 * PreCondition:    None
 *
 * Input:           offset  - Number of bytes beyond the beginning of the
 *                          Ethernet data (first byte after the type field)
 *                          where the checksum should begin
 *                  len     - Total number of bytes to include in the checksum
 *
 * Output:          16-bit checksum as defined by RFC 793.
 *
 * Side Effects:    None
 *
 * Overview:        This function performs a checksum calculation in the MAC
 *                  buffer itself
 *
 * Note:            None
 *****************************************************************************/
uint16_t MACCalcRxChecksum(uint16_t offset, uint16_t len)
{
    uint16_t temp;
    uint16_t rdSave;

    // Add the offset requested by firmware plus the Ethernet header
    temp = RXSTART + sizeof(ENC_PREAMBLE) + offset;

    rdSave = g_encIndex[ENC_RD_PTR_ID];

    SyncENCPtrRAWState(ENC_RD_PTR_ID, temp);

    temp = CalcIPBufferChecksum(len);

    SyncENCPtrRAWState(ENC_RD_PTR_ID, rdSave);

    return temp;
}

/******************************************************************************
 * Function:        void MACMemCopyAsync(uint16_t destAddr, uint16_t sourceAddr, uint16_t len)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *
 * Input:           destAddr:   Destination address in the Ethernet memory to
 *                              copy to.  If the MSb is set, the current EWRPT
 *                              value will be used instead.
 *                  sourceAddr: Source address to read from.  If the MSb is
 *                              set, the current ERDPT value will be used
 *                              instead.
 *                  len:        Number of bytes to copy
 *
 * Output:          Byte read from the MRF24W's RAM
 *
 * Side Effects:    None
 *
 * Overview:        Bytes are asynchronously transfered within the buffer.  Call
 *                  MACIsMemCopyDone() to see when the transfer is complete.
 *
 * Note:            If a prior transfer is already in progress prior to
 *                  calling this function, this function will block until it
 *                  can start this transfer.
 *
 *                  If a negative value (bit 15 set) is used for the sourceAddr or destAddr
 *                  parameters, then that pointer will get updated with the
 *                  next address after the read or write.
 *****************************************************************************/
void MACMemCopyAsync(PTR_BASE destAddr, PTR_BASE sourceAddr, uint16_t len)
{
    uint16_t readSave = 0, writeSave = 0;
    bool updateWritePointer;
    bool updateReadPointer;
    uint8_t rawScratchId = 0xff;  /* garbage value to avoid compiler warning */
    uint8_t copyBuf[8];
    uint16_t writeIndex, readIndex;
    uint16_t bytesLeft;
    uint16_t origRawIndex;

    if (len == 0)
    {
        return; /* nothing to do */
    }

    EnsureWFisAwake();

    /* if using current location of write pointer */
    if ( ((TCPIP_UINT16_VAL*)&destAddr)->bits.b15 )
    {
        updateWritePointer = true;
        destAddr = g_encIndex[ENC_WT_PTR_ID]; /* don't need to sync write pointer, should have already been done */
                                              /*  when encIndex was set previously                               */
    }
    /* else use destAddr as write pointer */
    else
    {
        updateWritePointer = false;
        writeSave = g_encIndex[ENC_WT_PTR_ID];
        SyncENCPtrRAWState(ENC_WT_PTR_ID, destAddr);
    }

    /* if using current location of read pointer */
    if ( ((TCPIP_UINT16_VAL*)&sourceAddr)->bits.b15 )
    {
        updateReadPointer = true;
        sourceAddr = g_encIndex[ENC_RD_PTR_ID]; /* don't need to sync read pointer, should have already been done */
                                                /*  when encIndex was set previously                              */
    }
    /* else use srcAddr as read pointer */
    else
    {
        updateReadPointer = false;
        readSave = g_encIndex[ENC_RD_PTR_ID];
        SyncENCPtrRAWState(ENC_RD_PTR_ID, sourceAddr);
    }

    /*-------------------------------------------------*/
    /* If copying from Scratch to Scratch (TCB to TCB) */
    /*-------------------------------------------------*/
    /* This is a special case. We cannot perform a RAW Copy operation within the same RAW window,              */
    /* but we can copy manually data from one section of the scratch to another section of the scratch window. */
    if ((destAddr >= BASE_SCRATCH_ADDR/*BASE_TCB_ADDR*/) && (sourceAddr >= BASE_SCRATCH_ADDR/*BASE_TCB_ADDR*/) )
    {
        bytesLeft = len;
        rawScratchId = RAW_SCRATCH_ID;
        /* save the current RAW index in this scratch window */
        origRawIndex = RawGetIndex(RAW_SCRATCH_ID);

        /*-------------------------------------------------------------------*/
        /* If we can safely do a forward copy (from first byte to last byte) */
        /*-------------------------------------------------------------------*/
        if ( ((sourceAddr + len) <= destAddr)   ||  // end of source before dest  (no overlap)
             ((destAddr   + len) <= sourceAddr) ||  // end of dest before source  (no overlap)
              (destAddr < sourceAddr)               // dest before source (overlap, but forward copy safe)
           )
        {
            /* map read and write indexs from TCB address to Scratch Index */
            readIndex  = sourceAddr - ENC_TCB_BUF_TO_RAW_SCRATCH_BUF_ADJUSTMENT;
            writeIndex = destAddr   - ENC_TCB_BUF_TO_RAW_SCRATCH_BUF_ADJUSTMENT;

            while (bytesLeft > 0)
            {
                /* if a full copyBuf worth of bytes to copy */
                if (bytesLeft >= sizeof(copyBuf))
                {
                    /* read a block of bytes from the source and write to destination */
                    RawRead( rawScratchId, readIndex,  sizeof(copyBuf), copyBuf);
                    RawWrite(rawScratchId, writeIndex, sizeof(copyBuf), copyBuf);

                    // index to next block in source and dest
                    readIndex  += sizeof(copyBuf);
                    writeIndex += sizeof(copyBuf);
                    bytesLeft  -= sizeof(copyBuf);
                }
                /* else less than a full copyBuf left to copy */
                else
                {
                    if (bytesLeft > 0)
                    {
                        RawRead( rawScratchId, readIndex,  bytesLeft, copyBuf);
                        RawWrite(rawScratchId, writeIndex, bytesLeft, copyBuf);
                        bytesLeft = 0;
                    }
                }
            }
        } /* end while for foward copy */
        /*---------------------------------------------------------------------------*/
        /* else we need to do a backward copy (from last byte to first byte) because */
        /* start of TCB dest block within TCB src block                              */
        /*---------------------------------------------------------------------------*/
        else if (destAddr > sourceAddr)
        {
            // map read index from TCB address to Scratch Index
            readIndex  = sourceAddr - ENC_TCB_BUF_TO_RAW_SCRATCH_BUF_ADJUSTMENT + len - 1;
            writeIndex = destAddr   - ENC_TCB_BUF_TO_RAW_SCRATCH_BUF_ADJUSTMENT + len - 1;

            while (bytesLeft > 0)
            {
                // if a full copyBuf worth of bytes to copy
                if (bytesLeft >= sizeof(copyBuf))
                {
                    RawRead(rawScratchId,  readIndex  - sizeof(copyBuf) + 1, sizeof(copyBuf), copyBuf);
                    RawWrite(rawScratchId, writeIndex - sizeof(copyBuf) + 1, sizeof(copyBuf), copyBuf);

                    // index to next block in source and dest
                    readIndex  -= sizeof(copyBuf);
                    writeIndex -= sizeof(copyBuf);
                    bytesLeft  -= sizeof(copyBuf);
                }
                // else less than a full copyBuf left to copy
                else
                {
                    if (bytesLeft > 0)
                    {
                        RawRead(rawScratchId, readIndex - bytesLeft + 1, bytesLeft - 1, copyBuf);
                        RawWrite(rawScratchId, writeIndex - bytesLeft + 1, bytesLeft - 1, copyBuf);
                        bytesLeft = 0;
                    }
                }
            } // end while
        }
        // restore raw index to where it was when this function was called
        RawSetIndex(RAW_SCRATCH_ID, origRawIndex);

    }
    /*-------------------------------------------------------------------------------------------*/
    /* else copying between RAW Data Rx and RAW Data Tx or between Raw Data Tx/Rx and RAW Scratch */
    /*-------------------------------------------------------------------------------------------*/
    else
    {
        #if defined(WF_DEBUG) /* if asserts enabled */
        /* if stack trying to copy from RAW Tx Data Window to same RAW Tx Data Window */
        if (((g_encIndex[ENC_WT_PTR_ID] >= TXSTART) && (g_encIndex[ENC_RD_PTR_ID] >= TXSTART))
                                                    &&
            ((g_encIndex[ENC_WT_PTR_ID] < BASE_SCRATCH_ADDR) && (g_encIndex[ENC_RD_PTR_ID] < BASE_SCRATCH_ADDR)))
        {
            WF_ASSERT(false);  /* can't copy from within same raw window */
        }

        /* if stack trying to copy from RAW Rx Data Window to RAW Rx Data Window */
        if ((g_encIndex[ENC_WT_PTR_ID] < TXSTART) && (g_encIndex[ENC_RD_PTR_ID] < TXSTART))
        {
            WF_ASSERT(false);  /* can't copy from within same raw window */
        }
        #endif /* WF_DEBUG */

        /* Copy from source RAW window to destination RAW window.  The indexes in the respective */
        /* RAW windows have been set either in this function or prior to this fucntion call.     */
        RawToRawCopy(g_encPtrRAWId[ENC_WT_PTR_ID], g_encPtrRAWId[ENC_RD_PTR_ID], len);
    }

    if ( !updateReadPointer )
    {
        SyncENCPtrRAWState(ENC_RD_PTR_ID, readSave);
    }

    if ( !updateWritePointer )
    {
        SyncENCPtrRAWState(ENC_WT_PTR_ID, writeSave);
    }

} /* end MACMemCopyAsync */

bool MACIsMemCopyDone(void)
{
    return true;
}

/******************************************************************************
 * Function:        uint8_t MACGet()
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *                  ERDPT must point to the place to read from.
 *
 * Input:           None
 *
 * Output:          Byte read from the MRF24W's RAM
 *
 * Side Effects:    None
 *
 * Overview:        MACGet returns the byte pointed to by ERDPT and
 *                  increments ERDPT so MACGet() can be called again.  The
 *                  increment will follow the receive buffer wrapping boundary.
 *
 * Note:            None
 *****************************************************************************/
uint8_t MACGet()
{
    uint8_t result;

    RawGetByte(g_encPtrRAWId[ENC_RD_PTR_ID], &result, 1);

    g_encIndex[ENC_RD_PTR_ID] += 1;

    return result;
}

/******************************************************************************
 * Function:        uint16_t MACGetArray(uint8_t *val, uint16_t len)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *                  ERDPT must point to the place to read from.
 *
 * Input:           *val: Pointer to storage location
 *                  len:  Number of bytes to read from the data buffer.
 *
 * Output:          Byte(s) of data read from the data buffer.
 *
 * Side Effects:    None
 *
 * Overview:        Burst reads several sequential bytes from the data buffer
 *                  and places them into local memory.  With SPI burst support,
 *                  it performs much faster than multiple MACGet() calls.
 *                  ERDPT is incremented after each byte, following the same
 *                  rules as MACGet().
 *
 * Note:            None
 *****************************************************************************/
uint16_t MACGetArray(uint8_t *val, uint16_t len)
{
    uint16_t i = 0;
    uint8_t byte;

    if ( len == 0 ) return 0;

    if ( val )
    {
        RawGetByte(g_encPtrRAWId[ENC_RD_PTR_ID], val, len);
    }
    else
    {
        // Read the data
        while (i < len)
        {
            RawGetByte(g_encPtrRAWId[ENC_RD_PTR_ID], &byte, 1);
            i++;
        }
    }
    
    g_encIndex[ENC_RD_PTR_ID] += len;

    return len;
}//end MACGetArray

/******************************************************************************
 * Function:        void MACPut(uint8_t val)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *                  EWRPT must point to the location to begin writing.
 *
 * Input:           Byte to write into the MRF24W buffer memory
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        MACPut outputs the Write Buffer Memory opcode/constant
 *                  (8 bits) and data to write (8 bits) over the SPI.
 *                  EWRPT is incremented after the write.
 *
 * Note:            None
 *****************************************************************************/
void MACPut(uint8_t val)
{
    uint8_t byte = val;

    RawSetByte(g_encPtrRAWId[ENC_WT_PTR_ID], &byte, 1);

    g_encIndex[ENC_WT_PTR_ID] += 1;
}//end MACPut

/******************************************************************************
 * Function:        void MACPutArray(uint8_t *val, uint16_t len)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *                  EWRPT must point to the location to begin writing.
 *
 * Input:           *val: Pointer to source of bytes to copy.
 *                  len:  Number of bytes to write to the data buffer.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        MACPutArray writes several sequential bytes to the
 *                  MRF24W RAM.  It performs faster than multiple MACPut()
 *                  calls.  EWRPT is incremented by len.
 *
 * Note:            None
 *****************************************************************************/
void MACPutArray(uint8_t *val, uint16_t len)
{
    RawSetByte(g_encPtrRAWId[ENC_WT_PTR_ID], val, len);

    g_encIndex[ENC_WT_PTR_ID] += len;
}//end MACPutArray

/******************************************************************************
 * Function:        void MACPutROMArray(ROM uint8_t *val, uint16_t len)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *                  EWRPT must point to the location to begin writing.
 *
 * Input:           *val: Pointer to source of bytes to copy.
 *                  len:  Number of bytes to write to the data buffer.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        MACPutArray writes several sequential bytes to the
 *                  MRF24W RAM.  It performs faster than multiple MACPut()
 *                  calls.  EWRPT is incremented by len.
 *
 * Note:            None
 *****************************************************************************/
#if defined(__XC8)
void MACPutROMArray(ROM uint8_t *val, uint16_t len)
{
    RawSetByteROM(g_encPtrRAWId[ENC_WT_PTR_ID], val, len);

    g_encIndex[ENC_WT_PTR_ID] += len;
}//end MACPutROMArray
#endif

/******************************************************************************
 * Function:        void MACPowerDown(void)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        MACPowerDown puts the MRF24W in low power sleep mode. In
 *                  sleep mode, no packets can be transmitted or received.
 *                  All MAC and PHY registers should not be accessed.
 *
 * Note:            If a packet is being transmitted while this function is
 *                  called, this function will block until it is it complete.
 *                  If anything is being received, it will be completed.
 *****************************************************************************/
void MACPowerDown(void)
{
}//end MACPowerDown

/******************************************************************************
 * Function:        void MACPowerUp(void)
 *
 * PreCondition:    SPI bus must be initialized (done in MACInit()).
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        MACPowerUp returns the MRF24W back to normal operation
 *                  after a previous call to MACPowerDown().  Calling this
 *                  function when already powered up will have no effect.
 *
 * Note:            If a link partner is present, it will take 10s of
 *                  milliseconds before a new link will be established after
 *                  waking up.  While not linked, packets which are
 *                  transmitted will most likely be lost.  MACIsLinked() can
 *                  be called to determine if a link is established.
 *****************************************************************************/
void MACPowerUp(void)
{
}//end MACPowerUp

/*****************************************************************************
  Function:
    uint16_t CalcIPBufferChecksum(uint16_t len)

  Summary:
    Calculates an IP checksum in the MAC buffer itself.

  Description:
    This function calculates an IP checksum over an array of input data
    existing in the MAC buffer.  The checksum is the 16-bit one's complement
    of one's complement sum of all words in the data (with zero-padding if
    an odd number of bytes are summed).  This checksum is defined in RFC 793.

  Precondition:
    TCP is initialized and the MAC buffer pointer is set to the start of
    the buffer.

  Parameters:
    len - number of bytes to be checksummed

  Returns:
    The calculated checksum.

  Remarks:
    All Microchip MACs should perform this function in hardware.
  ***************************************************************************/
extern uint16_t RawCalculateChecksum(uint16_t length);
uint16_t CalcIPBufferChecksum(uint16_t len)
{
    TCPIP_UINT32_VAL Checksum;

    // do checksum calculation via the checksum hardware engine
    Checksum.w[0] = HSTOWFS(RawCalculateChecksum(len));

    return Checksum.w[0];
}
#endif /* WF_CS_TRIS*/
