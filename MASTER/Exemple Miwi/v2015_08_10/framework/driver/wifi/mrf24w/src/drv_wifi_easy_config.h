/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_wifi_easy_config.h

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

#ifndef __DRV_WF_EASY_CONFIG_H_
#define __DRV_WF_EASY_CONFIG_H_

#if defined(EZ_CONFIG_STORE)
    #include "main.h"
#endif

typedef struct
{
    uint8_t configBits;                    /* used to dictate MAC behavior following the calculation */
    uint8_t phraseLen;                     /* number of valid bytes in passphrase */
    uint8_t ssidLen;                       /* number of valid bytes in ssid */
    uint8_t reserved;                      /* alignment byte */
    int8_t ssid[32];           /* the string of characters representing the ssid */
    int8_t passPhrase[64 + 1]; /* the string of characters representing the pass phrase */
} tWFPskCalcReq;

typedef enum {
    cfg_stopped = 0,
    cfg_stalled,
    cfg_wait_idle_complete,
    cfg_max
} tWFDynamicConfigStates;

typedef struct {
    uint8_t            isWifiNeedToConfigure;
    uint8_t            isWifiDoneConfigure;
    uint8_t            ssid[33];               // 32 for ssid and one for term character
    uint8_t            type;                   // Net type infrastructure vs adhoc
    uint8_t            prevSSID[33];           // previous SSID we were connected to
    uint8_t            prevWLAN;               // previous WLAN type

    uint8_t            security;               // security type
    uint8_t            defaultWepKey;          // WEP index value
    uint8_t            key[65];                // 64-byte key plus term character

    tWFDynamicConfigStates cfg_state;
    #if defined (EZ_CONFIG_STALL)
    uint32_t timeStart;
    #endif
} tWFEasyConfigCtx;

extern tWFEasyConfigCtx g_easyConfigCtx;
#define CFGCXT    (g_easyConfigCtx)

typedef struct
{
    int8_t       scanState;
    uint16_t  numScanResults;
    uint16_t  displayIdx;
} tWFScanCtx;

extern volatile tWFScanCtx  g_ScanCtx;
#define SCANCXT       (g_ScanCtx)

/* Easy Config Public Functions */
void WFEasyConfigInit(void);
void WFEasyConfigMgr(void);
void WFInitScan(void);

void WFScanEventHandler(uint16_t scanResults);

void WFDisplayScanMgr(void);

uint16_t WFStartScan(void);

uint16_t WFRetrieveScanResult(uint8_t Idx, tWFScanResult *p_ScanResult);

/* Macros */
#define WF_WEP_KEY_INVALID     0xff

#define WF_START_EASY_CONFIG()   CFGCXT.isWifiNeedToConfigure = 1;

/* Definitions for stall feature of state machine */
#define WF_STALL_TIME       1  /* In seconds */
#define WF_STALL_TIME_MS    (WF_STALL_TIME * TICKS_PER_SECOND)

/* Scan status/control bits */
#define SCAN_STATE_IN_PROGRESS    0x0001
#define SCAN_STATE_VALID_RESULTS  0x0002
#define SCAN_STATE_DISPLAY_RESULTS  0x0004

#define IS_SCAN_IN_PROGRESS(x)        ((x) & SCAN_STATE_IN_PROGRESS)
#define IS_SCAN_STATE_VALID(x)        ((x) & SCAN_STATE_VALID_RESULTS)
#define IS_SCAN_STATE_DISPLAY(x)      ((x) & SCAN_STATE_DISPLAY_RESULTS)
#define SCAN_SET_IN_PROGRESS(x)       ((x) |= SCAN_STATE_IN_PROGRESS)
#define SCAN_SET_VALID(x)             ((x) |= SCAN_STATE_VALID_RESULTS)
#define SCAN_SET_DISPLAY(x)           ((x) |= SCAN_STATE_DISPLAY_RESULTS)
#define SCAN_CLEAR_IN_PROGRESS(x)     ((x) &= ~SCAN_STATE_IN_PROGRESS)
#define SCAN_CLEAR_VALID(x)           ((x) &= ~SCAN_STATE_VALID_RESULTS)
#define SCAN_CLEAR_DISPLAY(x)         ((x) &= ~SCAN_STATE_DISPLAY_RESULTS)
#endif /* __DRV_WF_EASY_CONFIG_H_ */
