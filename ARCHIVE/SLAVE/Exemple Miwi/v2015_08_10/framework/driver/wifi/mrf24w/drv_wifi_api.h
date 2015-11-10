/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_wifi_api.h

  Summary:
    Module for Microchip TCP/IP Stack
    -Provides access to MRF24W WiFi Controller (MRF24WB0MA/B, MRF24WG0MA/B)
    -Reference: MRF24W Data sheet, IEEE 802.11 Standard

  Description:
    MRF24W Driver API Interface

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

//DOM-IGNORE-BEGIN
#ifndef __DRV_WF_API_H_
#define __DRV_WF_API_H_
//DOM-IGNORE-END

#include <stdbool.h>
#include <stdint.h>

/*= WF_ASSERT MACRO ==================================================================*/
/* Customize how the WiFi driver assert macro (WF_ASSERT) should operate.             */
/*  To DISABLE the WF_ASSERT macro: Comment out '#define WF_DEBUG'                    */
/*  To ENABLE the WF_ASSERT macro:  Unoomment out '#define WF_DEBUG'                   */
/*====================================================================================*/
#define WF_DEBUG
#if !defined(__XC8)
    #define DISPLAY_FILENAME  /* will diplay file names instead of module numbers on an assert */
#endif

/*----------------------------------------------------------------------------*/
/* This block of defines allows for code and data reduction by removing       */
/* WiFi driver code and or data that is not needed by the application.        */
/* Comment out those function blocks that are not needed.                     */
/*----------------------------------------------------------------------------*/
#define WF_USE_SCAN_FUNCTIONS
//#define WF_USE_TX_POWER_CONTROL_FUNCTIONS
#define WF_USE_POWER_SAVE_FUNCTIONS
#define WF_USE_MULTICAST_FUNCTIONS
#define WF_USE_INDIVIDUAL_SET_GETS
#define WF_USE_GROUP_SET_GETS
//#define USE_GRATUITOUS_ARP

/*******************************************************************************
*                              DEFINES
********************************************************************************/
/*-----------------------------*/
/* WiFi Driver Version Number  */
/*-----------------------------*/
#define WF_HOST_DRIVER_VERSION_NUMBER    "2.8.0"

/* API defines */
#define WF_MAX_NUM_CHANNELS             (14)
#define WF_MAX_NUM_CONNECTION_PROFILES  (8)
#define WF_CP_LIST_LENGTH               (8)
#define WF_MAX_SSID_LENGTH              (32)
#define WF_BSSID_LENGTH                 (6)
#define WF_RETRY_FOREVER                (255)
#define WF_CHANNEL_LIST_LENGTH          (14)
#define WF_MAX_SECURITY_KEY_LENGTH      (64)

#define WF_MIN_NUM_CPID                 (1)
#define WF_MAX_NUM_CPID                 (8)

#define WF_NO_CPID_ACTIVE               (0)

#define WF_RTS_THRESHOLD_MAX            (2347) /* maximum RTS threshold size in bytes */

#define WF_MAX_NUM_RATES                (8)

/* Key size defines */
#define WF_MIN_WPA_PASS_PHRASE_LENGTH         (8)
#define WF_MIN_WPA_PASS_PHRASE_LENGTH         (8)
#define WF_MAX_WPA_PASS_PHRASE_LENGTH        (64)
#define WF_MAX_WPA2_PASS_PHRASE_LENGTH       (64)
#define WF_WPA_KEY_LENGTH                    (32)
#define WF_WPA2_KEY_LENGTH                   (32)

/*------------------------------------------------------------------------------*/
/* These are error codes returned in the result field of a management response. */
/*------------------------------------------------------------------------------*/
#define WF_SUCCESS                                              ((uint16_t)1)
#define WF_ERROR_INVALID_SUBTYPE                                ((uint16_t)2)
#define WF_ERROR_OPERATION_CANCELLED                            ((uint16_t)3)
#define WF_ERROR_FRAME_END_OF_LINE_OCCURRED                     ((uint16_t)4)
#define WF_ERROR_FRAME_RETRY_LIMIT_EXCEEDED                     ((uint16_t)5)
#define WF_ERROR_EXPECTED_BSS_VALUE_NOT_IN_FRAME                ((uint16_t)6)
#define WF_ERROR_FRAME_SIZE_EXCEEDS_BUFFER_SIZE                 ((uint16_t)7)
#define WF_ERROR_FRAME_ENCRYPT_FAILED                           ((uint16_t)8)
#define WF_ERROR_INVALID_PARAM                                  ((uint16_t)9)
#define WF_ERROR_AUTH_REQ_ISSUED_WHILE_IN_AUTH_STATE            ((uint16_t)10)
#define WF_ERROR_ASSOC_REQ_ISSUED_WHILE_IN_ASSOC_STATE          ((uint16_t)11)
#define WF_ERROR_INSUFFICIENT_RESOURCES                         ((uint16_t)12)
#define WF_ERROR_TIMEOUT_OCCURRED                               ((uint16_t)13)
#define WF_ERROR_BAD_EXCHANGE_ENCOUNTERED_IN_FRAME_RECEPTION    ((uint16_t)14)
#define WF_ERROR_AUTH_REQUEST_REFUSED                           ((uint16_t)15)
#define WF_ERROR_ASSOCIATION_REQUEST_REFUSED                    ((uint16_t)16)
#define WF_ERROR_PRIOR_MGMT_REQUEST_IN_PROGRESS                 ((uint16_t)17)
#define WF_ERROR_NOT_IN_JOINED_STATE                            ((uint16_t)18)
#define WF_ERROR_NOT_IN_ASSOCIATED_STATE                        ((uint16_t)19)
#define WF_ERROR_NOT_IN_AUTHENTICATED_STATE                     ((uint16_t)20)
#define WF_ERROR_SUPPLICANT_FAILED                              ((uint16_t)21)
#define WF_ERROR_UNSUPPORTED_FEATURE                            ((uint16_t)22)
#define WF_ERROR_REQUEST_OUT_OF_SYNC                            ((uint16_t)23)
#define WF_ERROR_CP_INVALID_ELEMENT_TYPE                        ((uint16_t)24)
#define WF_ERROR_CP_INVALID_PROFILE_ID                          ((uint16_t)25)
#define WF_ERROR_CP_INVALID_DATA_LENGTH                         ((uint16_t)26)
#define WF_ERROR_CP_INVALID_SSID_LENGTH                         ((uint16_t)27)
#define WF_ERROR_CP_INVALID_SECURITY_TYPE                       ((uint16_t)28)
#define WF_ERROR_CP_INVALID_SECURITY_KEY_LENGTH                 ((uint16_t)29)
#define WF_ERROR_CP_INVALID_WEP_KEY_ID                          ((uint16_t)30)
#define WF_ERROR_CP_INVALID_NETWORK_TYPE                        ((uint16_t)31)
#define WF_ERROR_CP_INVALID_ADHOC_MODE                          ((uint16_t)32)
#define WF_ERROR_CP_INVALID_SCAN_TYPE                           ((uint16_t)33)
#define WF_ERROR_CP_INVALID_CP_LIST                             ((uint16_t)34)
#define WF_ERROR_CP_INVALID_CHANNEL_LIST_LENGTH                 ((uint16_t)35)
#define WF_ERROR_NOT_CONNECTED                                  ((uint16_t)36)
#define WF_ERROR_ALREADY_CONNECTING                             ((uint16_t)37)
#define WF_ERROR_DISCONNECT_FAILED                              ((uint16_t)38)
#define WF_ERROR_NO_STORED_BSS_DESCRIPTOR                       ((uint16_t)39)
#define WF_ERROR_INVALID_MAX_POWER                              ((uint16_t)40)
#define WF_ERROR_CONNECTION_TERMINATED                          ((uint16_t)41)
#define WF_ERROR_HOST_SCAN_NOT_ALLOWED                          ((uint16_t)42)
#define WF_ERROR_INVALID_WPS_PIN                                ((uint16_t)44)

/*---------------------------------------------------------------------*/
/* Used for eventNotificationField bit mask in tWFCAElements structure */
/*---------------------------------------------------------------------*/
#define WF_NOTIFY_CONNECTION_ATTEMPT_SUCCESSFUL     ((uint8_t)(0x01))
#define WF_NOTIFY_CONNECTION_ATTEMPT_FAILED         ((uint8_t)(0x02))
#define WF_NOTIFY_CONNECTION_TEMPORARILY_LOST       ((uint8_t)(0x04))
#define WF_NOTIFY_CONNECTION_PERMANENTLY_LOST       ((uint8_t)(0x08))
#define WF_NOTIFY_CONNECTION_REESTABLISHED          ((uint8_t)(0x10))
#define WF_NOTIFY_ALL_EVENTS                        ((uint8_t)(0x1f))

/*---------------------------------------------------------------------*/
/* Used for Tx mode selection */
/*---------------------------------------------------------------------*/
#define WF_TXMODE_G_RATES        0
#define WF_TXMODE_B_RATES        1
#define WF_TXMODE_LEGACY_RATES   2

/*------------------------------------------------------------------------------------------*/
/* Multicast Filter ID's                                                                    */
/* Infrastructure can use 2,3,4,5 and AdHoc can only use 4,5.  Use 4,5 which works for both */
/*------------------------------------------------------------------------------------------*/
#define WF_MULTICAST_FILTER_1       (4)
#define WF_MULTICAST_FILTER_2       (5)
#if defined(MRF24WG)
    #define WF_MULTICAST_FILTER_3       (6)
    #define WF_MULTICAST_FILTER_4       (7)
    #define WF_MULTICAST_FILTER_5       (8)
    #define WF_MULTICAST_FILTER_6       (9)
    #define WF_MULTICAST_FILTER_7       (10)
    #define WF_MULTICAST_FILTER_8       (11)
    #define WF_MULTICAST_FILTER_9       (12)
    #define WF_MULTICAST_FILTER_10      (13)
    #define WF_MULTICAST_FILTER_11      (14)
    #define WF_MULTICAST_FILTER_12      (15)
    #define WF_MULTICAST_FILTER_13      (16)
    #define WF_MULTICAST_FILTER_14      (17)
    #define WF_MULTICAST_FILTER_15      (18)
    #define WF_MULTICAST_FILTER_16      (19)

    #define WF_MULTICAST_DISABLE_ALL    (0)
    #define WF_MULTICAST_ENABLE_ALL     (1)
    #define WF_MULTICAST_USE_FILTERS    (2)
#endif /* MRF24WG */

#define WF_MASK_DEAUTH_REASONCODE       ((uint8_t)0x80)
#define WF_MASK_DISASSOC_REASONCODE     ((uint8_t)0x40)

#define WF_SCAN_ALL ((uint8_t)(0xff))

/* hw RSSI reference value to be used to derive real RSSI.  For MRF24WB sets to 200, and MRF24WG sets to 128. */
#ifdef MRF24WG
    #define WF_HWRSSIVAL_MAX    128    
#else
    #define WF_HWRSSIVAL_MAX    200    
#endif

/*
*********************************************************************************************************
*                                           DATA TYPES
*********************************************************************************************************
*/

/*----------------------------------------------------------------------------*/
/* Events that can be invoked in WF_ProcessEvent().  Note that the            */
/* connection events are optional, all other events the app must be notified. */
/*----------------------------------------------------------------------------*/

#define WF_EVENT_CONNECTION_SUCCESSFUL           (1)   /* Connection attempt to network successful            */
#define WF_EVENT_CONNECTION_FAILED               (2)   /* Connection attempt failed                           */

#define WF_EVENT_CONNECTION_TEMPORARILY_LOST     (3)   /* Connection lost; MRF24W attempting to reconnect     */
#define WF_EVENT_CONNECTION_PERMANENTLY_LOST     (4)   /* Connection lost; MRF24W no longer trying to connect */
#define WF_EVENT_CONNECTION_REESTABLISHED        (5)

#define WF_EVENT_FLASH_UPDATE_SUCCESSFUL         (6)   /* Update to FLASH successful                          */
#define WF_EVENT_FLASH_UPDATE_FAILED             (7)   /* Update to FLASH failed                              */

#define WF_EVENT_KEY_CALCULATION_REQUEST        (8)   /* Key calculation is required                       */

#define WF_EVENT_SCAN_RESULTS_READY              (9)   /* scan results are ready                              */
#define WF_EVENT_IE_RESULTS_READY                (10)  /* IE data ready                                       */

#define WF_EVENT_RX_PACKET_RECEIVED              (11)   /* Rx data packet has been received by MRF24W          */
#define WF_EVENT_INVALID_WPS_PIN                 (12)   /* Invalid WPS pin was entered                            */
#define WF_EVENT_SOFT_AP_EVENT                      13   /* Client connection events                     */

typedef struct WFMacStatsStruct
{
    /**
      Number of frames received with the Protected Frame subfield of the Frame
      Control field set to zero and the value of dot11ExcludeUnencrypted causes
      that frame to be discarded.
      */
    uint32_t MibWEPExcludeCtr;
    uint32_t MibTxBytesCtr; // Total number of Tx bytes that have been transmitted

    /**
      Number of frames successfully transmitted that had the multicast bit set
      in the destination MAC address.
      */
    uint32_t MibTxMulticastCtr;
    /**
      Number of Tx frames that failed due to the number of transmits exceeding
      the retry count.
      */
    uint32_t MibTxFailedCtr;
    uint32_t MibTxRtryCtr;        // Number of times a transmitted frame needed to be retried
    uint32_t MibTxMultRtryCtr;    // Number of times a frame was successfully transmitted after more than one retransmission.
    uint32_t MibTxSuccessCtr;     // Number of Tx frames successfully transmitted.
    uint32_t MibRxDupCtr;         // Number of frames received where the Sequence Control field indicates a duplicate.
    uint32_t MibRxCtsSuccCtr;     // Number of CTS frames received in response to an RTS frame.
    uint32_t MibRxCtsFailCtr;     // Number of times an RTS frame was not received in response to a CTS frame.
    uint32_t MibRxAckFailCtr;     // Number of times an Ack was not received in response to a Tx frame.
    uint32_t MibRxBytesCtr;       // Total number of Rx bytes received.
    uint32_t MibRxFragCtr;        // Number of successful received frames (management or data)
    uint32_t MibRxMultCtr;        // Number of frames received with the multicast bit set in the destination MAC address.
    uint32_t MibRxFCSErrCtr;      // Number of frames received with an invalid Frame Checksum (FCS).

    /**
      Number of frames received where the Protected Frame subfield of the Frame Control Field is set to
      one and the WEPOn value for the key mapped to the transmitter’s MAC address indicates the frame
      should not have been encrypted.
      */
    uint32_t MibRxWEPUndecryptCtr;
    uint32_t MibRxFragAgedCtr; // Number of times that fragments ‘aged out’, or were not received in the allowable time.
    uint32_t MibRxMICFailureCtr; // Number of MIC failures that have occurred.
} tWFMacStats;

/*-------------------------------------------------------*/
/* Security Type defines                                 */
/* Used in WF_CPSet/GetSecurityType WF_CPSet/GetElements */
/*-------------------------------------------------------*/
#define WF_SECURITY_OPEN                         (0)
#define WF_SECURITY_WEP_40                       (1)
#define WF_SECURITY_WEP_104                      (2)
#define WF_SECURITY_WPA_AUTO_WITH_KEY            (7)
#define WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE    (8)
#define WF_SECURITY_WPS_PUSH_BUTTON              (9)
#define WF_SECURITY_WPS_PIN                      (10)

// *****************************************************************************
/*  WEP Key Types

  Summary:
    Selections for WEP key type when using WEP security.

  Description
    Select one of two choices available for the WEP key type when using
    WEP security.  The recommended key type (and default) is Open key.
*/

#define WF_SECURITY_WEP_SHAREDKEY  (0)
#define WF_SECURITY_WEP_OPENKEY    (1)

/*---------------------------------------------------------------------*/
/* Network Type defines                                                */
/* Used in WF_CPSet/GetNetworkType, WF_CPSetElements, WF_CPGetElements */
/*---------------------------------------------------------------------*/
#define WF_INFRASTRUCTURE 1
#define WF_ADHOC          2
#define WF_P2P            3
#define WF_SOFT_AP        4

/*--------------------------------------------------------*/
/* Ad Hoc behavior defines                                */
/* Used in WF_CPSet/GetAdhocBehavor, WF_CPSet/GetElements */
/*--------------------------------------------------------*/
#define WF_ADHOC_CONNECT_THEN_START  (0)
#define WF_ADHOC_CONNECT_ONLY        (1)
#define WF_ADHOC_START_ONLY          (2)

/*----------------------------------------------------*/
/* Scan type defines                                  */
/* Used in WF_CASet/GetScanType, WF_CASet/GetElements */
/*----------------------------------------------------*/
#define WF_ACTIVE_SCAN   (1)
#define WF_PASSIVE_SCAN  (2)

/*-----------------------------------------------------------------------------------------*/
/* Beacon Timeout and Deauth defines                                                       */
/* Used in WF_CASet/GetBeaconTimeoutAction, WF_CASet/GetDeauthAction, WF_CASet/GetElements */
/*-----------------------------------------------------------------------------------------*/
#define WF_DO_NOT_ATTEMPT_TO_RECONNECT  (0)
#define WF_ATTEMPT_TO_RECONNECT         (1)

#define WF_DISABLED  (0)
#define WF_ENABLED   (1)

/* eventInfo defines for WF_ProcessEvent(), case WF_EVENT_CONNECTION_FAILED */
/* Also value for index 3 of WF_CONNECTION_FAILED_EVENT_SUBTYPE */
#define WF_JOIN_FAILURE                         (2)
#define WF_AUTHENTICATION_FAILURE               (3)
#define WF_ASSOCIATION_FAILURE                  (4)
#define WF_WEP_HANDSHAKE_FAILURE                (5)
#define WF_PSK_CALCULATION_FAILURE              (6)
#define WF_PSK_HANDSHAKE_FAILURE                (7)
#define WF_ADHOC_JOIN_FAILURE                   (8)
#define WF_SECURITY_MISMATCH_FAILURE            (9)
#define WF_NO_SUITABLE_AP_FOUND_FAILURE         (10)
#define WF_RETRY_FOREVER_NOT_SUPPORTED_FAILURE  (11)
#define WF_LINK_LOST                            (12)
#define WF_TKIP_MIC_FAILURE                     (13)
#define WF_RSN_MIXED_MODE_NOT_SUPPORTED         (14)
#define WF_RECV_DEAUTH                          (15)
#define WF_RECV_DISASSOC                        (16)
#define WF_WPS_FAILURE                          (17)
#define WF_P2P_FAILURE                          (18)

/* Reason Codes */
#define WF_UNSPECIFIED                          (1)
#define WF_REASON_PREV_AUTH_NOT_VALID           (2)
#define WF_DEAUTH_LEAVING                       (3)
#define WF_DISASSOC_DUE_TO_INACTIVITY           (4)
#define WF_DISASSOC_AP_BUSY                     (5)
#define WF_CLASS2_FRAME_FROM_NONAUTH_STA        (6)
#define WF_CLASS3_FRAME_FROM_NONASSOC_STA       (7)
#define WF_DISASSOC_STA_HAS_LEFT                (8)
#define WF_STA_REQ_ASSOC_WITHOUT_AUTH           (9)
#define WF_INVALID_IE                           (13)
#define WF_MIC_FAILURE                          (14)
#define WF_4WAY_HANDSHAKE_TIMEOUT               (15)
#define WF_GROUP_KEY_HANDSHAKE_TIMEOUT          (16)
#define WF_IE_DIFFERENT                         (17)
#define WF_INVALID_GROUP_CIPHER                 (18)
#define WF_INVALID_PAIRWISE_CIPHER              (19)
#define WF_INVALID_AKMP                         (20)
#define WF_UNSUPP_RSN_VERSION                   (21)
#define WF_INVALID_RSN_IE_CAP                   (22)
#define WF_IEEE8021X_FAILED                     (23)
#define WF_CIPHER_SUITE_REJECTED                (24)

/* eventInfo defines for WF_ProcessEvent(), case WF_EVENT_CONNECTION_TEMPORARILY_LOST */
#define WF_BEACON_TIMEOUT        (1)
#define WF_DEAUTH_RECEIVED       (2)
#define WF_DISASSOCIATE_RECEIVED (3)

/* Status Codes */
#define WF_UNSPECIFIED_FAILURE          (1)
#define WF_CAPS_UNSUPPORTED             (10)
#define WF_REASSOC_NO_ASSOC             (11)
#define WF_ASSOC_DENIED_UNSPEC          (12)
#define WF_NOT_SUPPORTED_AUTH_ALG       (13)
#define WF_UNKNOWN_AUTH_TRANSACTION     (14)
#define WF_CHALLENGE_FAIL               (15)
#define WF_AUTH_TIMEOUT                 (16)
#define WF_AP_UNABLE_TO_HANDLE_NEW_STA  (17)
#define WF_ASSOC_DENIED_RATES           (18)
#define WF_ASSOC_DENIED_NOSHORTPREAMBLE (19)
#define WF_ASSOC_DENIED_NOPBCC          (20)
#define WF_ASSOC_DENIED_NOAGILITY       (21)
#define WF_ASSOC_DENIED_NOSHORTTIME     (25)
#define WF_ASSOC_DENIED_NODSSSOFDM      (26)
#define WF_S_INVALID_IE                 (40)
#define WF_S_INVALID_GROUPCIPHER        (41)
#define WF_S_INVALID_PAIRWISE_CIPHER    (42)
#define WF_S_INVALID_AKMP               (43)
#define WF_UNSUPPORTED_RSN_VERSION      (44)
#define WF_S_INVALID_RSN_IE_CAP         (45)
#define WF_S_CIPHER_SUITE_REJECTED      (46)
#define WF_TIMEOUT                      (47)

#define MRF24WB0M_DEVICE  (1)
#define MRF24WG0M_DEVICE  (2)

#if defined(MRF24WG)
    /* Domain Codes */
    #define WF_DOMAIN_FCC      (0)           /* MRF24WG - Available Channels: 1 - 11 */
    #define WF_DOMAIN_ETSI     (2)           /* MRF24WG - Available Channels: 1 - 13 */
    #define WF_DOMAIN_JAPAN    (7)           /* MRF24WG - Available Channels: 1 - 14 */
    #define WF_DOMAIN_OTHER    (7)           /* MRF24WG - Available Channels: 1 - 14 */
#else
    /* Domain Codes */
    #define WF_DOMAIN_FCC      (0)           /* MRF24WB - Available Channels: 1  - 11 */
    #define WF_DOMAIN_IC       (1)           /* MRF24WB - Available Channels: 1  - 11 */
    #define WF_DOMAIN_ETSI     (2)           /* MRF24WB - Available Channels: 1  - 13 */
    #define WF_DOMAIN_SPAIN    (3)           /* MRF24WB - Available Channels: 10 - 11 */
    #define WF_DOMAIN_FRANCE   (4)           /* MRF24WB - Available Channels: 10 - 13 */
    #define WF_DOMAIN_JAPAN_A  (5)           /* MRF24WB - Available Channels: 14      */
    #define WF_DOMAIN_JAPAN_B  (6)           /* MRF24WB - Available Channels: 1  - 13 */
#endif

/* Power save states */
#define WF_PS_HIBERNATE              (1)
#define WF_PS_PS_POLL_DTIM_ENABLED   (2)
#define WF_PS_PS_POLL_DTIM_DISABLED  (3)
#define WF_PS_OFF                    (4)

/* Hibernate states */
#define WF_HB_NO_SLEEP          (0)
#define WF_HB_ENTER_SLEEP       (1)
#define WF_HB_WAIT_WAKEUP       (2)

/* Pin Level */
#define WF_LOW   (0)
#define WF_HIGH  (1)

/*-----------------------------------------------------------------------*/
/* defines used for the p_currentCpID value in WF_CMGetConnectionState() */
/*-----------------------------------------------------------------------*/
#define WF_CURRENT_CPID_NONE    (0)
#define WF_CURRENT_CPID_LIST    (0xff)

/* Connection States */
#define WF_CSTATE_NOT_CONNECTED                 (1)
#define WF_CSTATE_CONNECTION_IN_PROGRESS        (2)
#define WF_CSTATE_CONNECTED_INFRASTRUCTURE      (3)
#define WF_CSTATE_CONNECTED_ADHOC               (4)
#define WF_CSTATE_RECONNECTION_IN_PROGRESS      (5)
#define WF_CSTATE_CONNECTION_PERMANENTLY_LOST   (6)

/* eventInfo define for WF_ProcessEvent() when no additional info is supplied */
#define WF_NO_ADDITIONAL_INFO       ((uint16_t)0xffff)

#define ENABLE_WPS_PRINTS    1 //(1 << 0)
#define ENABLE_P2P_PRINTS    (1 << 1)

#define SHA1_MAC_LEN 20

enum SoftAPEvent { SOFTAP_EVENT_CONNECTED, SOFTAP_EVENT_DISCONNECTED };
enum SoftAPEventReason {SOFTAP_EVENT_LINK_LOST, SOFTAP_EVENT_RECEIVED_DEAUTH };

/*-----------------------------*/
/* Connection Profile Elements */
/*-----------------------------*/

// Connection profile elements structure
typedef struct WFCPElementsStruct
{
    /**
      SSID, which must be less than or equal to 32 characters.  Set to all 0’s
      if not being used.  If ssidLength is 0 this field is ignored.  If SSID is
      not defined then the MRF24W, when using this profile to connect, will
      scan all channels within its regional domain.

      Default: SSID not used.
      */
    uint8_t  ssid[WF_MAX_SSID_LENGTH];
    /**
      Basic Service Set Identifier, always 6 bytes.  This is the 48-bit MAC of
      the SSID.  It is an optional field that can be used to specify a specific
      SSID if more than one AP exists with the same SSID.  This field can also
      be used in lieu of the SSID.

      Set each byte to 0xFF if BSSID is not going to be used.
      Default: BSSID not used (all FF’s)
      */
    uint8_t  bssid[WF_BSSID_LENGTH];
    /**
      Number of ASCII bytes in ssid.  Set to 0 is SSID is not going to be used.

      Default: 0
      */
    uint8_t  ssidLength;
    /**
      Designates the desired security level for the connection.  Choices are:
      <table>
        WF_SECURITY_OPEN                        No security encryption used.
        WF_SECURITY_WEP_40                      Use WEP security.
                                                WEP40 key, using four 5-byte keys will be provided in securityKey.
                                                 Note that only open authentication is supported for WEP.
        WF_SECURITY_WEP_104                     Use WEP security.
                                                 WEP104 key, using four 13-byte keys will be provided in securityKey.
                                                 Note that only open authentication is supported for WEP.
        WF_SECURITY_WPA_AUTO_WITH_KEY           Use WPA-PSK or WPA2-PSK security.
                                                 32-byte PSK (Pre-shared Key) will be provided in securityKey.
                                                 It is often displayed as 64 hexadecimal characters.
        WF_SECURITY_WPA_AUTO_WITH_PASSPHRASE    Use WPA-PSK or WPA2-PSK security.
                                                 ASCII passphrase (8 to 63 characters long) will be provided in securityKey.
                                                 MRF24W will convert this to PSK (which can take up to 30 seconds).
      </table>
      Default: WF_SECURITY_OPEN
      */
    uint8_t  securityType;
    /**
      Set to NULL if securityType is WF_SECURITY_OPEN.  If securityKeyLength is 0
      this field is ignored.
      <table>
        WEP Keys            If using WEP this field must contain 4 WEP Keys.  For WF_SECURITY_WEP_40
                             each key must be 5 bytes in length.  For WF_SECURITY_WEP_104 each key
                             must be 13 bytes in length.  The keys must be contiguous within this field.
                             For example, if using 5 bytes keys the first key starts at index 0, the second
                             key at index 5, the third key at index 10, and the last key at index 15.
                             Unused keys should be set to all 0’s.
        WPA-PSK/WPA2-PSK    If using WPA-PSK or WPA2-PSK this field contains either the 32-byte PSK (displayed
                             as 64 hex characters) or the ASCII passphrase (8 to 63 characters long).
                             If ASCII passphrase is given MRF24W will convert it to PSK. 
      </table>
      Default: No security key defined
      */
    uint8_t  securityKey[WF_MAX_SECURITY_KEY_LENGTH];
    /**
      Number of bytes used in the securityKey.  Set to 0 if securityType is WF_SECURITY_OPEN.
      <table>
        WEP Keys-Length If securityType is WF_SECURITY_WEP_40 or WF_SECURITY_WEP_104
                         then this field is the length of the four WEP keys.

                         Range is:
                         20 if securityType is WF_SECURITY_WEP_40 (four 5-byte keys),
                         52 if securityType is WF_SECURITY_WEP_104 (four 13-byte keys)
        PSK-Length      If securityType is WPA-PSK or WPA2-PSK this field is the number of bytes in PSK or
                         the number of characters in ASCII passphrase.
      </table>
      Default: 0
      */
    uint8_t  securityKeyLength;
    /**
      This field is only used if securityType is  WF_SECURITY_WEP_40 or
      WF_SECURITY_WEP_104.  This field designates which of the four WEP
      keys defined in securityKey to use when connecting to a WiFi network.
      Only WEP key index (wepDefaultKeyId)  0 is used in RF module FW.
      */
    uint8_t  wepDefaultKeyId;
    /**
      WF_INFRASTRUCTURE / WF_ADHOC / WF_P2P /  WF_SOFT_AP

      Default: WF_INFRASTRUCTURE
      */
    uint8_t  networkType;
    /**
      Only applicable if networkType is WF_ADHOC.  Configures Adhoc behavior.  Choices are:
      <table>
        WF_ADHOC_CONNECT_THEN_START     Attempt to connect to existing network.
                                         If that fails, then start a network.
        WF_ADHOC_CONNECT_ONLY           Connect only to an existing network.
                                         Do not start a network.
        WF_ADHOC_START_ONLY             Only start a network.
      </table>
      Default: WF_ADHOC_CONNECT_THEN_START
      */
    uint8_t  adHocBehavior;
    /**
    1 - enable hidden ssid in adhoc mode
    */
    uint8_t hiddenSSID;
    /**
    0- shared key, 1 - open key
    */
    uint8_t wepKeyType;
} tWFCPElements;

/*-------------------------------*/
/* Connection Algorithm Elements */
/*-------------------------------*/
typedef struct WFCAElementsStruct
{
    /**
      This parameter is only used when PS Poll mode is enabled.  See
      WF_PsPollEnable().  Number of 100ms intervals between instances when the
      MRF24W wakes up to received buffered messages from the network.  Range
      is from 1 (100ms) to 6553.5 sec (~109 min).

      Note that the 802.11 standard defines the listen interval in terms of
      Beacon Periods, which are typically 100ms.  If the MRF24W is communicating
      to a network with a network that has Beacon Periods that is not 100ms it
      will round up (or down) as needed to match the actual Beacon Period as
      closely as possible.

      Important Note: If the listenInterval is modified while connected to a
      network the MRF24W will automatically reconnect to the network with the
      new Beacon Period value.  This may cause a temporary loss of data packets.
      */
    uint16_t  listenInterval;
    /**
      WF_ACTIVE_SCAN (Probe Requests transmitted out) or WF_PASSIVE_SCAN (listen only for beacons received)

      Default: WF_ACTIVE_SCAN
      */
    uint8_t   scanType;
    /**
      Specifies RSSI restrictions when connecting.  This field is only used if:
      1.  The Connection Profile has not defined a SSID or BSSID, or
      2.  An SSID is defined in the Connection Profile and multiple AP’s are discovered with the same SSID.

      <table>
        0       Connect to the first network found
        1-254   Only connect to a network if the RSSI is greater than or equal to the specified value.
        255     Connect to the highest RSSI found (default)
      </table>
      */
    uint8_t   rssi;
    /**
      <b>Note: Connection Profile lists are not yet supported.  This array should be set to all FF’s.</b>
      */
    uint8_t   connectionProfileList[WF_CP_LIST_LENGTH];
    /**
      This field is used to specify the number of retries for the single
      connection profile before taking the connection lost action.

      Range 1 to 254 or WF_RETRY_FOREVER (255)

      Default is 3
      */
    uint8_t   listRetryCount;
    /**
      There are several connection-related events that can occur.  The Host has
      the option to be notified (or not) when some of these events occur.  This
      field controls event notification for connection-related events.
      <table>
        Bit 7   Bit 6   Bit 5   Bit 4   Bit 3   Bit 2   Bit 1   Bit 0
        -----   -----   -----   -----   -----   -----   -----   -----
        Not     Not     Not     Event   Event   Event   Event   Event
         used    used    used    E       D       C       B       A
      </table>
      The defines for each bit are shown below.
      <table>
        Event Code  #define
        ----------  -------
        A           WF_NOTIFY_CONNECTION_ATTEMPT_SUCCESSFUL
        B           WF_NOTIFY_CONNECTION_ATTEMPT_FAILED
        C           WF_NOTIFY_CONNECTION_TEMPORARILY_LOST
        D           WF_NOTIFY_CONNECTION_PERMANENTLY_LOST
        E           WF_NOTIFY_CONNECTION_REESTABLISHED
      </table>
      If a bit is set, then the host will be notified if the associated event
      occurs.  If the bit is not set then the host will not be notified if the
      associated event occurs.   A #define, WF_NOTIFY_ALL_EVENTS, exists as a
      shortcut to allow notification for all events.

      Note that if an event is not in the above bit mask the application will
      always be notified of the event via the call to WF_ProcessEvent().

      Default: WF_NOTIFY_ALL_EVENTS
      */
    uint8_t   eventNotificationAction;
    /**
      Specifies the action the Connection Manager should take if a Connection is
      lost due to a Beacon Timeout.
      If this field is set to WF_ATTEMPT_TO_RECONNECT then the number of attempts
      is limited to the value in listRetryCount.

      Choices are:
      WF_ATTEMPT_TO_RECONNECT or WF_DO_NOT_ATTEMPT_TO_RECONNECT

      Default: WF_ATTEMPT_TO_RECONNECT
      */
    uint8_t   beaconTimeoutAction;
    /**
      Designates what action the Connection Manager should take if it receives a
      Deauthentication message from the AP.

      If this field is set to WF_ATTEMPT_TO_RECONNECT then the number of attempts
      is limited to the value in listRetryCount.

      Choices are:
      WF_ATTEMPT_TO_RECONNECT or WF_DO_NOT_ATTEMPT_TO_RECONNECT

      Default: WF_ATTEMPT_TO_RECONNECT
      */
    uint8_t   deauthAction;
    /**
      List of one or more channels that the MRF24W should utilize when
      connecting or scanning.  If numChannelsInList is set to 0 then this
      parameter should be set to NULL.

      Default: All valid channels for the regional domain of the MRF24W (set
      at manufacturing).
    */
    uint8_t   channelList[WF_CHANNEL_LIST_LENGTH];
    /**
      Number of channels in channelList.  If set to 0 then the MRF24W will
      populate the list with all valid channels for the regional domain.

      Default: The number of valid channels for the regional domain of the
      MRF24W (set at manufacturing).
      */
    uint8_t   numChannelsInList;
    /**
      Specifies the number of beacons that can be missed before the action
      described in beaconTimeoutAction is taken.

      <table>
        0       * Not monitor the beacon timeout condition
                 * Will not indicate this condition to Host
        1-255   Beacons missed before disconnect event occurs and beaconTimeoutAction
                 occurs.  If enabled, host will receive an event message indicating
                 connection temporarily or permanently lost, and if retrying, a
                 connection successful event.
      </table>
      Default: 0 (no monitoring or notification of beacon timeout)
      */
    uint8_t   beaconTimeout;
    /**
      The number of times to scan a channel while attempting to find a particular
      access point.

      Default: 1
      */
    uint8_t   scanCount;
    uint8_t   pad1;
    /**
      The minimum time (in milliseconds) the connection manager will wait for a
      probe response after sending a probe request.  If no probe responses are
      received in minChannelTime then the connection manager will go on to the
      next channel, if any are left to scan, or quit.

      Default: 200ms
      */
    uint16_t  minChannelTime;
    /**
      If a probe response is received within minChannelTime then the connection
      manager will continue to collect any additional probe responses up to
      maxChannelTime before going to the next channel in the channelList.  Units
      are in milliseconds.

      Default: 400ms
      */
    uint16_t  maxChannelTime;
    /**
      The number of microseconds to delay before transmitting a probe request
      following the channel change event.

      Default: 20us
    */
    uint16_t  probeDelay;

#if defined (MRF24WG)
    /**
    Default : 4
    */
    uint16_t dtimInterval;

    /**
    Default : 100 (ms)
    */
    uint16_t beaconPrd;
#endif
} tWFCAElements;

/*--------------------------*/
/* used in WF_GetDeviceInfo */
/*--------------------------*/
typedef struct tWFDeviceInfoStruct
{
    uint8_t  deviceType;    /* MRF24WB0M_DEVICE  or MRF24WG0M_DEVICE */
    uint8_t  romVersion;    /* ROM version number     */
    uint8_t  patchVersion;  /* Patch version number   */
} tWFDeviceInfo;

#if defined (MRF24WG)
/*--------------------------*/
/* used in WF_CMGetConnectContext */
/*--------------------------*/
typedef struct tWFConnectContextStruct
{
    uint8_t  channel;    /* channel number of current connection */
    uint8_t  bssid[6];    /* bssid of connected AP    */
} tWFConnectContext;
#endif

/*--------------*/
/* Scan Results */
/*--------------*/
typedef struct
{
    uint8_t      bssid[WF_BSSID_LENGTH]; // Network BSSID value
    uint8_t      ssid[WF_MAX_SSID_LENGTH]; // Network SSID value

    /**
      Access point configuration
      <table>
        Bit 7        Bit 6        Bit 5        Bit 4        Bit 3        Bit 2        Bit 1        Bit 0
        -----        -----        -----        -----        -----        -----        -----        -----
        WPA2         WPA          Preamble     Privacy      Reserved     Reserved     Reserved     IE
      </table>

      <table>
      IE        1 if AP broadcasting one or more Information Elements, else 0
      Privacy   0: AP is open (no security)
                 1: AP using security,  if neither WPA and WPA2 set then security is WEP.
      Preamble  0: AP transmitting with short preamble
                 1: AP transmitting with long preamble
      WPA       Only valid if Privacy is 1.
                 0: AP does not support WPA
                 1: AP supports WPA
      WPA2      Only valid if Privacy is 1.
                 0: AP does not support WPA2
                 1: AP supports WPA2
      </table>
      */
    uint8_t      apConfig;
    uint8_t      reserved;
    uint16_t     beaconPeriod; // Network beacon interval
    uint16_t     atimWindow; // Only valid if bssType = WF_INFRASTRUCTURE

    /**
      List of Network basic rates.  Each rate has the following format:

      Bit 7
      * 0 – rate is not part of the basic rates set
      * 1 – rate is part of the basic rates set

      Bits 6:0
      Multiple of 500kbps giving the supported rate.  For example, a value of 2
      (2 * 500kbps) indicates that 1mbps is a supported rate.  A value of 4 in
      this field indicates a 2mbps rate (4 * 500kbps).
      */
    uint8_t      basicRateSet[WF_MAX_NUM_RATES];
    /**
       Signal Strength RSSI

       MRF24WB : RSSI_MAX (200) , RSSI_MIN (106)
       MRF24WG : RSSI_MAX (128) , RSSI_MIN (43)
      */
    uint8_t      rssi; // Signal strength of received frame beacon or probe response
    uint8_t      numRates; // Number of valid rates in basicRates
    uint8_t      DtimPeriod; // Part of TIM element
    uint8_t      bssType; // WF_INFRASTRUCTURE or WF_ADHOC
    uint8_t      channel; // Channel number
    uint8_t      ssidLen; // Number of valid characters in ssid

} tWFScanResult;

typedef struct WFHibernate {
    uint8_t state;
    uint8_t wakeup_notice;
} tWFHibernate;

#if defined(WF_CM_DEBUG)
typedef struct
{
    uint8_t     byte[12 * 4];  // Currently, CM has 12 states; 4-byte for each state info entry.
} tWFCMInfoFSMStats;
#endif

#if defined (MRF24WG)
    #if defined(WF_USE_MULTICAST_FUNCTIONS)
    typedef struct WFMulticastConfigStruct
    {
        uint8_t filterId;
        uint8_t action;
        uint8_t macBytes[6];
        uint8_t macBitMask;

    } tWFMultiCastConfig;
    #endif

    typedef struct
    {
        uint8_t ssid[32];   /* SSID */
        uint8_t netKey[64]; /* Net Key PSK */
        uint16_t authType;  /* Authentication Type: AUTH_OPEN / AUTH_WPA_PSK / AUTH_SHARED / AUTH_WPA / AUTH_WPA2 / AUTH_WPA2_PSK */
        uint16_t encType;   /* Encoding Type: ENC_NONE / ENC_WEP / ENC_TKIP / ENC_AES */
        uint8_t netIdx;     /* Net ID */
        uint8_t ssidLen;    /* SSID length */
        uint8_t keyIdx;     /* Key ID */
        uint8_t keyLen;     /* WPA-PSK/WPA2-PSK key length */
        uint8_t bssid[6];   /* BSSID */
    } tWFWpsCred;
#endif /* MRF24WG */

/*------------------------*/
/* if asserts are enabled */
/*------------------------*/
#if defined(WF_DEBUG)
    /*---------------------------------------------------------*/
    /* Module numbers that will be used in the WF_ASSERT macro */
    /*---------------------------------------------------------*/
    typedef enum
    {
        WF_MODULE_MAIN_DEMO                  = 0,   /* main.c                                */
        WF_MODULE_WF_CONFIG                  = 1,   /* drv_wifi_config.c                     */
        WF_MODULE_WF_EINT                    = 2,   /* drv_wifi_eint.c                       */
        WF_MODULE_WF_SPI                     = 3,   /* drv_wifi_spi.c                        */
        WF_MODULE_WF_MAC                     = 4,   /* drv_wifi_mac.c                        */
        WF_MODULE_WF_PARAM_MSG               = 5,   /* drv_wifi_param_msg.c                  */
        WF_MODULE_WF_CONNECTION_PROFILE      = 6,   /* drv_wifi_connection_profile.c         */
        WF_MODULE_WF_CONNECTION_ALGORITHM    = 7,   /* drv_wifi_connection_algorithm.c       */
        WF_MODULE_WF_CONNECTION_MANAGER      = 8,   /* drv_wifi_connection_manager.c         */
        WF_MODULE_WF_DRIVER_COM              = 9,   /* drv_wifi_com.c                        */
        WF_MODULE_WF_INIT                    = 10,  /* drv_wifi_init.c                       */
        WF_MODULE_WF_DRIVER_RAW              = 11,  /* drv_wifi_raw.c                        */
        WF_MODULE_WF_MGMT_MSG                = 12,  /* drv_wifi_mgmt_msg.c                   */
        WF_MODULE_WF_TX_POWER                = 13,  /* drv_wifi_tx_power.c                   */
        WF_MODULE_WF_POWER_SAVE              = 14,  /* drv_wifi_power_save.c                 */
        WF_MODULE_WF_EVENT_HANDLER           = 15,  /* drv_wifi_event_handler.c              */
        WF_MODULE_WF_SCAN                    = 16,  /* drv_wifi_scan.c                       */
        WF_MODULE_WF_DEBUG_STRINGS           = 17,  /* drv_wifi_debug_strings.c              */
        WF_MODULE_WF_AUTO_UPDATE_TCP         = 18,  /* drv_wifi_auto_update_tcp_client_24g.c */
        WF_MODULE_WF_AUTO_UPDATE_UART        = 19,  /* drv_wifi_auto_update_uart_24g.c       */
        WF_MODULE_IPERF_APP                  = 20,  /* iperf_app.c                           */
        WF_MODULE_FTP_CLIENT_DEMO            = 21,  /* ftp_client_demo.c                     */
        WF_MODULE_PING_DEMO                  = 22,  /* ping_demo.c                           */
    } tWFModuleNumber;

    void WF_AssertionFailed(uint8_t moduleNumber, uint16_t lineNumber);

    #define WF_ASSERT(expr)                                         \
       do {                                                         \
           if (!(expr))                                             \
              {                                                     \
                   WF_AssertionFailed(WF_MODULE_NUMBER, __LINE__);  \
              }                                                     \
       } while (0)
/*---------------------------*/
/* else asserts are disabled */
/*---------------------------*/
#else
    #define WF_ASSERT(expr) do {} while (0)
#endif  /* WF_DEBUG */

/*
 *********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 *********************************************************************************************************
 */

/*****************************************************************************
 * FUNCTION: WF_Init
 *
 * RETURNS: None
 *
 * PARAMS:
 *      N/A.
 *
 *
 *  NOTES: This function must be called once prior to calling any other WF...
 *          functions.  This function initializes the Wifi Driver internal State.
 *          It also verifies functionality of the lower level SPI driver and
 *          connected hardware.
 *****************************************************************************/
void WF_Init(void);

/*******************************************************************************
  Function:
    void WF_GetDeviceInfo(tWFDeviceInfo *p_deviceInfo)

  Summary:
    Retrieves WF device information (MRF24WB0M_DEVICE/MRF24WG0M_DEVICE, romVersion
    and patchVersion).

  Description:
   Retrieves RF module information.
   * MRF24WB will have romVersion = 0x12.
   * MRF24WG will have romVersion = 0x30 or 0x31.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_deviceInfo - Pointer where device info will be written

  Returns:
    None.

  Remarks:
    None.
 *****************************************************************************/
void WF_GetDeviceInfo(tWFDeviceInfo *p_deviceInfo);

#if defined(MRF24WG)
/*******************************************************************************
  Function:
    void WF_CMGetConnectContext(tWFConnectContext *p_ctx)

  Summary:
    Retrieves WF connection context for MRF24WG0MA/B

  Description:
    Retrieves WF connection context for MRF24WG0MA/B

  Precondition:
    MACInit must be called first.

  Parameters:
    p_ctx -- pointer where connect context will be written

  Returns:
    None.

  Remarks:
      None.
 *****************************************************************************/
void WF_CMGetConnectContext(tWFConnectContext *p_ctx);
#endif

/*****************************************************************************
 * FUNCTION: WFProcess
 *
 * RETURNS:  None
 *
 * PARAMS:   None
 *
 *  NOTES:   This function is called from WFProcess.  It does the following:
 *             1) checks for and processes MRF24W external interrupt events
 *             2) checks for and processes received management messages from the MRF24W
 *             3) maintains the PS-Poll state (if applicable)
 *
 *****************************************************************************/
void WFProcess(void);

/*---------------------------------------*/
/* WF Driver External Interrupt function */
/* Must be called when:                  */
/*  1) External interrupt is enabled AND */
/*  2) EXINT line is asserted (low)      */
/*---------------------------------------*/
void WFEintISR(void);

/*******************************************************************************
  Function:
    void WF_SetMacAddress(uint8_t *p_mac)

  Summary:
    Uses a different MAC address for the MRF24W

  Description:
    Directs the MRF24W to use the input MAC address instead of its
    factory-default MAC address.  This function does not overwrite the factory
    default, which is in FLASH memory – it simply tells the MRF24W to use a
    different MAC.

  Precondition:
    MACInit must be called first.  Cannot be called when the MRF24W is in a
    connected state.

  Parameters:
    p_mac  - Pointer to 6-byte MAC that will be sent to MRF24W

  Returns:
    None.

  Remarks:
    None.
  *****************************************************************************/
void WF_SetMacAddress(uint8_t *p_mac);

/*******************************************************************************
  Function:
    void WF_GetMacAddress(uint8_t *p_mac)

  Summary:
    Retrieves the MRF24W MAC address

  Description:

  Precondition:
    MACInit must be called first.

  Parameters:
    p_mac  - Pointer where mac will be written (must point to a 6-byte buffer)

  Returns:
    None.

  Remarks:
    None.
  *****************************************************************************/
void WF_GetMacAddress(uint8_t *p_mac);

/*******************************************************************************
  Function:
    void WF_CPCreate(uint8_t *p_CpId)

  Summary:
    Creates a Connection Profile on the MRF24W.

  Description:
    Requests the MRF24W to create a Connection Profile (CP), assign it an ID,
    and set all the elements to default values.  The ID returned by this function
    is used in other connection profile functions.  A maximum of 2 Connection
    Profiles can exist on the MRF24W.
    Users are encouraged to use 1 profile ID for MRF24W based on v5 stack SW.
    In v6 stack SW, plan is to have 1 profile ID for MRF24W but stack will be designed to
    have capability to handle multiple profiles.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_CpId - Pointer to where Connection Profile ID will be written.  If
              function fails, the CP ID will be set to 0xff.

  Returns:
    None.

  Remarks:
    None.
  *****************************************************************************/
void WF_CPCreate(uint8_t *p_CpId);

/*******************************************************************************
  Function:
    void WF_CPDelete(uint8_t CpId)

  Summary:
    Deletes a Connection Profile on the MRF24W.

  Description:
    Deletes the specified Connection Profile.  If the Connection Profile was in
    FLASH it will be erased from FLASH.

  Precondition:
    MACInit must be called first.

  Parameters:
    CpId - Connection Profile to delete.

  Returns:
    None.

  Remarks:
    None.
  *****************************************************************************/
void WF_CPDelete(uint8_t CpId);

/*******************************************************************************
  Function:
    void WF_CPGetIds(uint8_t *p_cpIdList)

  Summary:
    Retrieves the CP ID bit mask.

  Description:
    Returns a list of all Connection Profile IDs that have been created on the
    MRF24W.  This is not to be confused with the Connection Algorithm's
    connectionProfileList.  This function returns a bit mask corresponding to a
    list of all Connection Profiles that have been created (whether they are in
    the connectionProfileList or not).  Any Connection Profiles that have been
    saved to FLASH will be included.
    Users are encouraged to use 1 profile ID for MRF24W based on v5 stack SW.
    In v6 stack SW, plan is to have 1 profile ID for MRF24W but stack will be designed to
    have capability to handle multiple profiles.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_cpIdList - Pointer to value representing the bit mask where each bit
                  index (plus 1) corresponds to a Connection Profile ID that has
                  been created.  For example, if this value is 0x03, then
                  Connection Profile IDïs 1 and and 2 have been created.

  Returns:
    None.

  Remarks:
    None.
  *****************************************************************************/
void WF_CPGetIds(uint8_t *cpIdList);

#if defined(WF_USE_GROUP_SET_GETS)
/*******************************************************************************
  Function:
    void WF_CPSetElements(uint8_t CpId, tWFCPElements *p_elements)

  Summary:
    Writes out data for a specific connection profile element.

  Description:
    Sets all Connection Profile elements.  If the Host CPU does not have enough
    memory to create a structure of this size then call the individual set
    functions.

  Precondition:
    MACInit must be called.

  Parameters:
    CpId - Connectino Profile ID.
    p_elements - Pointer to Connection Profile elements structure.

  Returns:
    None.

  Remarks:
    None.
  *****************************************************************************/
void WF_CPSetElements(uint8_t CpId, tWFCPElements *p_elements);

/*****************************************************************************
  Function:
    void WF_CASetElementsN(const tWFCAElements *p_elements)

  Summary:
    Writes all Connection Algorithm elements.

  Description:
    Sends a message to the MRF24W which sets all the Connection Algorithm
    elements.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_elements - Pointer to the input structure (tWFCAElements) containing the
                 connection algorithm elements.

  Returns:
    None

  Remarks:
    None
*****************************************************************************/
void WF_CASetElementsN(const tWFCAElements *p_elements);

void WF_CPGetElements(uint8_t CpId, tWFCPElements *p_elements);
#endif

#if defined(WF_USE_INDIVIDUAL_SET_GETS)
    void WF_CPSetSsid(uint8_t CpId, uint8_t *p_ssid,  uint8_t ssidLength);
    void WF_CPSetSsidType(uint8_t CpId, uint8_t hidden);
    void WF_CPGetSsidType(uint8_t CpId, uint8_t *hidden);
    void WF_CPGetSsid(uint8_t CpId, uint8_t *p_ssid, uint8_t *p_ssidLength);
    void WF_CPSetBssid(uint8_t CpId, uint8_t *p_bssid);
    void WF_CPGetBssid(uint8_t CpId, uint8_t *p_bssid);
    void WF_CPSetSecurity(uint8_t CpId,
                          uint8_t securityType,
                          uint8_t wepKeyIndex,
                          uint8_t *p_securityKey,
                          uint8_t securityKeyLength);
    void WF_CPGetSecurity(uint8_t CpId,
                          uint8_t *p_securityType,
                          uint8_t *p_wepKeyIndex,
                          uint8_t *p_securityKey,
                          uint8_t *p_securityKeyLength);
    void WF_CPSetDefaultWepKeyIndex(uint8_t CpId, uint8_t defaultWepKeyIndex);
    void WF_CPGetDefaultWepKeyIndex(uint8_t CpId, uint8_t *p_defaultWepKeyIndex);
    void WF_CPSetNetworkType(uint8_t CpId, uint8_t networkType);
    void WF_CPGetNetworkType(uint8_t CpId, uint8_t *p_networkType);
    void WF_CPSetWepKeyType(uint8_t CpId, uint8_t wepKeyType);
    void WF_CPGetWepKeyType(uint8_t CpId, uint8_t *p_wepKeyType);
#if defined (MRF24WG)
    void WF_CPGetWPSCredentials(uint8_t CpId, tWFWpsCred *p_cred);
#endif
    void WF_CPSetAdHocBehavior(uint8_t CpId, uint8_t adHocBehavior);
    void WF_CPGetAdHocBehavior(uint8_t CpId, uint8_t *p_adHocBehavior);
#endif /* WF_USE_INDIVIDUAL_SET_GETS */

#if defined(WF_USE_GROUP_SET_GETS)
    /*****************************************************************************
  Function:
    void WF_CASetElements(tWFCAElements *p_elements)

  Summary:
    Writes all Connection Algorithm elements.

  Description:
    Sends a message to the MRF24W which sets all the Connection Algorithm
    elements.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_elements - Pointer to the input structure (tWFCAElements) containing the
                 connection algorithm elements.

  Returns:
    None

  Remarks:
    None
*****************************************************************************/
void WF_CASetElements(tWFCAElements *p_elements);

/*****************************************************************************
  Function:
    void WF_CAGetElements(tWFCAElements *p_elements)

  Summary:
    Reads all Connection Algorithm elements.

  Description:
    Sends a message to the MRF24W which requests all the Connection Algorithm
    elements.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_elements - Pointer to the output structure (tWFCAElements) where the
                 connection algorithm elements are written.

  Returns:
    None

  Remarks:
    None
*****************************************************************************/
void WF_CAGetElements(tWFCAElements *p_elements);
#endif

#if defined(WF_USE_INDIVIDUAL_SET_GETS)
/*****************************************************************************
  Function:
    void WF_CASetScanType(uint8_t scanType)

  Summary:
    Sets the Connection Algorith scan type

  Description:
    Configures the Connection Algorithm for the desired scan type.
    In active scan, STA will transmit probe request frames and AP/routers will respond by
    transmitting probe response frames. For AP/Router with hidden SSID, active scan is used.
    In passive scan, AP/router will continuously transmit beacon frames for every beacon
    interval (BI), any STAs may/will receive these beacons and know of existence of this
    AP/router.

  Precondition:
    MACInit must be called first.

  Parameters:
    scanType - Desired scan type.  Either WF_ACTIVE_SCAN or WF_PASSIVE_SCAN.

  Returns:
    None

  Remarks:
    Active scanning causes the MRF24W to send probe requests.  Passive
    scanning implies the MRF24W only listens for beacons.
    Default is WF_ACTIVE_SCAN.
*****************************************************************************/
    void WF_CASetScanType(uint8_t scanType);

/*****************************************************************************
  Function:
    void WF_CAGetScanType(uint8_t *p_scanType)

  Summary:
    Gets the Connection Algorithm scan type

  Description:
    Reads the current Connection Algorithm scan type.
    In active scan, STA will transmit probe request frames and AP/routers will respond by
    transmitting probe response frames. For AP/Router with hidden SSID, active scan is used.
    In passive scan, AP/router will continuously transmit beacon frames for every beacon
    interval (BI), any STAs may/will receive these beacons and know of existence of this
    AP/router.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_scanType - Pointer where Connection Algorithm scan type is written.
                         Either WF_ACTIVE_SCAN or WF_PASSIVE_SCAN.

  Returns:
    None

  Remarks:
    Active scanning causes the MRF24W to send probe requests.  Passive
    scanning implies the MRF24W only listens for beacons.
    Default is WF_ACTIVE_SCAN.
*****************************************************************************/
    void WF_CAGetScanType(uint8_t *p_scanType);

/*****************************************************************************
  Function:
    void WF_CASetRssi(uint8_t rssi)

  Summary:
    Sets the RSSI threshold

  Description:
    Specifies the RSSI behavior when connecting.  This value is only used if
      1) The current Connection Profile has not defined an SSID or BSSID
      2) An SSID is defined in the current Connection Profile and multiple
         access points are discovered with the same SSID.

     Values:
       0      : Connect to the first network found
       1 - 200 (MRF24WB), 1 - 128 (MRF24WG): Only connect to a network if the RSSI
           is greater than or equal to the specified value
       201 - 255:     Connect to the highest RSSI found

       Note that RSSI is a relative value with no units -- it is not correlated to dBm.

  Precondition:
    MACInit must be called first.

  Parameters:
    scanCount - Desired scan count

  Returns:
    None

  Remarks:
    Default is 255
*****************************************************************************/
    void WF_CASetRssi(uint8_t rssi);

/*****************************************************************************
  Function:
    void WF_CAGetRssi(uint8_t p_rssi)

  Summary:
    Gets the RSSI threshold

  Description:
    See WF_CASetRssi.  Note that this function only retrieves the RSSI threshold
    used during the connection -- this is not the current RSSI of an existing connection.
    If it is desired to retrieve the current RSSI state then a scan must be performed and
    the scan result will contain the current RSSI state.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_rssi - Pointer to where RSSI value is written

  Returns:
    None

  Remarks:
    Default is 255
*****************************************************************************/
    void WF_CAGetRssi(uint8_t *p_rssi);

/*****************************************************************************
  Function:
    void WF_CASetConnectionProfileList(uint8_t cpList[WF_CP_LIST_LENGTH])

  Summary:
    Not currently supported

  Description:
    Not currently supported

  Precondition:
    MACInit must be called first.

  Parameters:
    cpList - Array of connection profile ID's used to create CP list

  Returns:
    None

  Remarks:
    Not currently supported.  The list size is always WF_CP_LIST_SIZE.
    The list should start at index 0.  Unused entries in the list must be set
    to 0xff.
*****************************************************************************/
    void WF_CASetConnectionProfileList(uint8_t cpList[WF_CP_LIST_LENGTH]);

/*****************************************************************************
  Function:
    void WF_CAGetConnectionProfileList(uint8_t cpList[WF_CP_LIST_LENGTH])

  Summary:
    Not currently supported

  Description:
    Not currently supported

  Precondition:
    MACInit must be called first.

  Parameters:
    cpList - Array of connection profile ID's used to create CP list

  Returns:
    None

  Remarks:
    Not currently supported.  The list size is always WF_CP_LIST_SIZE.
*****************************************************************************/
    void WF_CAGetConnectionProfileList(uint8_t cpList[WF_CP_LIST_LENGTH]);

/*****************************************************************************
  Function:
    void WF_CASetListRetryCount(uint8_t listRetryCount)

  Summary:
    Sets the list retry count

  Description:
    Number of times to cycle through Connection Profile List before giving up on
    the connection attempt.  Since lists are not yet supported, this function
    actually sets the number of times the Connection Manager will try to connect
    with the current Connection Profile before giving up.

  Precondition:
    MACInit must be called first.

  Parameters:
    listRetryCount - 0 to 254 or WF_RETRY_FOREVER (255)

  Returns:
    None

  Remarks:
    None
*****************************************************************************/
    void WF_CASetListRetryCount(uint8_t listRetryCount);

/*****************************************************************************
  Function:
    void WF_CAGetListRetryCount(uint8_t *p_listRetryCount)

  Summary:
    Gets the list retry count

  Description:
    See description in WF_CASetListRetryCount()

  Precondition:
    MACInit must be called first.

  Parameters:
    p_listRetryCount - Pointer to where list retry count is written.

  Returns:
    None

  Remarks:
    None
*****************************************************************************/
    void WF_CAGetListRetryCount(uint8_t *p_listRetryCount);

/*******************************************************************************
  Function:
    void WF_CASetEventNotificationAction(uint8_t eventNotificationAction)

  Summary:
    Sets the WiFi events that the host wishes to be notified of.

  Description:
    Sets the Event Notification Action used by the Connection Algorithm.  The
    bit mask for the allowable entries is as follows:

    <table>
        Bit     Event
        ---     -----
        0       WF_NOTIFY_CONNECTION_ATTEMPT_SUCCESSFUL
        1       WF_NOTIFY_CONNECTION_ATTEMPT_FAILED
        2       WF_NOTIFY_CONNECTION_TEMPORARILY_LOST
        3       WF_NOTIFY_CONNECTION_PERMANENTLY_LOST
        4       WF_NOTIFY_CONNECTION_REESTABLISHED
    </table>

  Precondition:
    MACInit must be called first.

  Parameters:
    eventNotificationAction - Bit mask indicating which events the host wants
                               to be notifed of.

  Returns:
    None.

  Remarks:
    None.
 *****************************************************************************/
    void WF_CASetEventNotificationAction(uint8_t eventNotificationAction);

/*******************************************************************************
  Function:
    void WF_CAGetEventNotificationAction(uint8_t *p_eventNotificationAction)

  Summary:
    Reads the Connection Algorithm event notification action.

  Description:
    Gets the Event Notification Action used by the Connection Algorithm.  The
    value read back will be a bit mask that corresponds to the following table:

    <table>
        Bit     Event
        ---     -----
        0       WF_NOTIFY_CONNECTION_ATTEMPT_SUCCESSFUL
        1       WF_NOTIFY_CONNECTION_ATTEMPT_FAILED
        2       WF_NOTIFY_CONNECTION_TEMPORARILY_LOST
        3       WF_NOTIFY_CONNECTION_PERMANENTLY_LOST
        4       WF_NOTIFY_CONNECTION_REESTABLISHED
    </table>

  Precondition:
    MACInit must be called first.

  Parameters:
    p_eventNotificationAction - Pointer to where returned value is written.

  Returns:
    None.

  Remarks:
    None.
 *****************************************************************************/
    void WF_CAGetEventNotificationAction(uint8_t *p_eventNotificationAction);

/*******************************************************************************
  Function:
    void WF_CASetBeaconTimeoutAction(uint8_t beaconTimeoutAction)

  Summary:
    Action to take if a connection is lost due to a beacon timeout.

  Description:
    Sets the Beacon Timeout Action used by the Connection Algorithm.

  Precondition:
    MACInit must be called first.

  Parameters:
    beaconTimeoutAction - Action to take if a connection is lost due
                           to a beacon timeout.  Choices are either:
                           * WF_ATTEMPT_TO_RECONNECT
                           * WF_DO_NOT_ATTEMPT_TO_RECONNECT

  Returns:
    None.

  Remarks:
    None.
 *****************************************************************************/
    void WF_CASetBeaconTimeoutAction(uint8_t beaconTimeoutAction);

/*******************************************************************************
  Function:
    void WF_CAGetBeaconTimeoutAction(uint8_t *p_beaconTimeoutAction)

  Summary:
    Reads the Connection Algorithm beacon timeout action.

  Description:
    Gets the Beacon Timeout Action used by the Connection Algorithm.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_beaconTimeoutAction - Pointer where returned value is written.  The value
                             will be either:
                             * WF_ATTEMPT_TO_RECONNECT
                             * WF_DO_NOT_ATTEMPT_TO_RECONNECT

  Returns:
    None.

  Remarks:
    None.
 *****************************************************************************/
    void WF_CAGetBeaconTimeoutAction(uint8_t *p_beaconTimeoutAction);

/*******************************************************************************
  Function:
    void WF_CASetDeauthAction(uint8_t deauthAction)

  Summary:
    Sets the DeauthAction used by the Connection Algorithm.

  Description:
    Action to take if a connection is lost due to receiving a deauthentification
    message from an AP.

  Precondition:
    MACInit must be called first.

  Parameters:
    deauthAction - Action to take in the event of a deauthentication.
                    Allowable values are:
                    * WF_ATTEMPT_TO_RECONNECT
                    * WF_DO_NOT_ATTEMPT_TO_RECONNECT

  Returns:
    None.

  Remarks:
    None.
 *****************************************************************************/
    void WF_CASetDeauthAction(uint8_t deauthAction);

/*******************************************************************************
  Function:
    void WF_CASetDeauthAction(uint8_t deauthAction)

  Summary:
    Sets the DeauthAction used by the Connection Algorithm.

  Description:
    Action to take if a connection is lost due to receiving a deauthentification
    message from an AP.

  Precondition:
    MACInit must be called first.

  Parameters:
    deauthAction - Action to take in the event of a deauthentication.
                    Allowable values are:
                    * WF_ATTEMPT_TO_RECONNECT
                    * WF_DO_NOT_ATTEMPT_TO_RECONNECT

  Returns:
    None.

  Remarks:
    None.
 *****************************************************************************/
    void WF_CAGetDeauthAction(uint8_t *p_deauthAction);

/*******************************************************************************
  Function:
    void WF_CASetChannelList(uint8_t *p_channelList, uint8_t numChannels)

  Summary:
    Sets the channel list.

  Description:
    Sets the Channel List used by the Connection Algorithm.
    MRF24W is programmed with channel 1 to 11 as default.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_channelList - Pointer to channel list.
    numChannels   - Number of channels in p_channelList.  If set to 0, the
                     MRF24W will use all valid channels for the current
                     regional domain.

  Returns:
    None.

  Remarks:
    None.
 *****************************************************************************/
    void WF_CASetChannelList(uint8_t *p_channelList, uint8_t numChannels);

/*******************************************************************************
  Function:
    void WF_CAGetChannelList(uint8_t *p_channelList, uint8_t *p_numChannels)

  Summary:
    Gets the channel list.

  Description:
    Gets the Channel List used by the Connection Algorithm.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_channelList - Pointer to where channel list will be returned
    p_numChannels - Pointer to where number of channels in list will be
                     returned

  Returns:
    None.

  Remarks:
    None.
 *****************************************************************************/
    void WF_CAGetChannelList(uint8_t *p_channelList, uint8_t *p_numChannels);

/*******************************************************************************
  Function:
    void WF_CASetListenInterval(uint16_t listenInterval)

  Summary:
    Sets the listen interval.

  Description:
    Sets the listen interval used by the Connection Algorithm.  This value is
    measured in 100ms intervals, the default beacon period of APs.

    <table>
        Value   Description
        -----   -----------
        1       MRF24W wakes up every 100ms to receive buffered messages.
        2       MRF24W wakes up every 200ms to receive buffered messages.
        ...     ...
        65535   MRF24W wakes up every 6535.5 seconds (~109 minutes) to
                 receive buffered messages.
    </table>

  Precondition:
    MACInit must be called first.  Only used when PS Poll mode is enabled.

  Parameters:
    listenInterval - Number of 100ms intervals between instances when
                      the MRF24W wakes up to receive buffered messages
                      from the network.

  Returns:
    None.

  Remarks:
    None.
 *****************************************************************************/
    void WF_CASetListenInterval(uint16_t listenInterval);

/*******************************************************************************
  Function:
    void WF_CAGetListenInterval(uint16_t *p_listenInterval)

  Summary:
    Gets the listen interval.

  Description:
    Gets the Listen Interval used by the Connection Algorithm.  This value is
    measured in 100ms intervals, the default beacon period of APs.

    <table>
        Value   Description
        -----   -----------
        1       MRF24W wakes up every 100ms to receive buffered messages.
        2       MRF24W wakes up every 200ms to receive buffered messages.
        ...     ...
        65535   MRF24W wakes up every 6535.5 seconds (~109 minutes) to
                 receive buffered messages.
    </table>

  Precondition:
    MACInit must be called first.  Only used when PS Poll mode is enabled.

  Parameters:
    p_listenInterval - Pointer to where listen interval is returned

  Returns:
    None.

  Remarks:
    None.
 *****************************************************************************/
    void WF_CAGetListenInterval(uint16_t *p_listenInterval);

    #if defined(MRF24WG)
/*******************************************************************************
  Function:
    void WF_CASetDtimInterval(uint16_t dtimInterval)

  Summary:
    Sets the dtim interval for MRF24WG0MA/B.

  Description:
    Sets the dtim interval used by the Connection Algorithm for MRF24WG0MA/B.

  Precondition:
    MACInit must be called first.  Only used when PS Poll mode is enabled.

  Parameters:
    dtimInterval -- Number of DTIM intervals between instances when
                      the MRF24W wakes up to receive buffered messages
                      from the network.

  Returns:
    None.

  Remarks:
    None.
 *****************************************************************************/
        void WF_CASetDtimInterval(uint16_t dtimInterval);

/*******************************************************************************
  Function:
    void WF_CAGetDtimInterval(uint16_t *p_dtimInterval)

  Summary:
    Gets the dtim interval for MRF24WG0MA/B.

  Description:
    Gets the DTIM Interval used by the Connection Algorithm for MRF24WG0MA/B.

  Precondition:
    MACInit must be called first.  Only used when PS Poll mode is enabled.

  Parameters:
    p_dtimInterval -- pointer to where listen interval is returned

  Returns:
    None.

  Remarks:
    None.
 *****************************************************************************/
        void WF_CAGetDtimInterval(uint16_t *p_dtimInterval);
    #endif

/*******************************************************************************
  Function:
    void WF_CASetBeaconTimeout(uint8_t beaconTimeout)

  Summary:
    Sets the beacon timeout value.

  Description:
    Sets the Beacon Timeout used by the Connection Algorithm.

    <table>
        Value   Description
        -----   -----------
        0       No monitoring of the beacon timeout condition.  The host will
                 not be notified of this event.
        1-255   Number of beacons missed before disconnect event occurs and
                 beaconTimeoutAction occurs.  If enabled, host will receive
                 an event message indicating connection temporarily or
                 permanently lost, and if retrying, a connection successful
                 event.
    </table>

  Precondition:
    MACInit must be called first.

  Parameters:
    beaconTimeout - Number of beacons that can be missed before the action in
                    beaconTimeoutAction is taken.

  Returns:
    None.

  Remarks:
    None.
 *****************************************************************************/
    void WF_CASetBeaconTimeout(uint8_t beaconTimeout);

/*******************************************************************************
  Function:
    void WF_CAGetBeaconTimeout(uint8_t *p_beaconTimeout)

  Summary:
    Reads the beacon timeout value.

  Description:
    Gets the Beacon Timeout used by the Connection Algorithm.

    <table>
        Value   Description
        -----   -----------
        0       No monitoring of the beacon timeout condition.  The host will
                 not be notified of this event.
        1-255   Number of beacons missed before disconnect event occurs and
                 beaconTimeoutAction occurs.  If enabled, host will receive
                 an event message indicating connection temporarily or
                 permanently lost, and if retrying, a connection successful
                 event.
    </table>

  Precondition:
    MACInit must be called first.

  Parameters:
    p_beaconTimeout - Pointer where beacon timeout value is written

  Returns:
    None.

  Remarks:
    None.
 *****************************************************************************/
    void WF_CAGetBeaconTimeout(uint8_t *p_beaconTimeout);

/*****************************************************************************
  Function:
    void WF_CASetScanCount(uint8_t scanCount)

  Summary:
    Sets the scan count

  Description:
    The number of times the Connection Manager will scan a channel while attempting
    to find a particular WiFi network.

  Precondition:
    MACInit must be called first.

  Parameters:
    scanCount - Desired scan count

  Returns:
    None

  Remarks:
    Default is 1
*****************************************************************************/
    void WF_CASetScanCount(uint8_t scanCount);

/*****************************************************************************
  Function:
    void WF_CAGetScanCount(uint8_t *p_scanCount)

  Summary:
    Gets the scan count

  Description:
    The number of times the Connection Manager will scan a channel while attempting
    to find a particular WiFi network.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_scanCount - Pointer to where scan count is written

  Returns:
    None

  Remarks:
    Default is 1
*****************************************************************************/
    void WF_CAGetScanCount(uint8_t *p_scanCount);

/*****************************************************************************
  Function:
    void WF_CASetMinChannelTime(uint16_t minChannelTime)

  Summary:
    Sets the minimum channel time (in milliseconds)

  Description:
    The minimum time (in milliseconds) the connection manager will wait for a
    probe response after sending a probe request.  If no probe responses are
    received in minChannelTime then the connection manager will go on to the
    next channel, if any are left to scan, or quit.

  Precondition:
    MACInit must be called first.

  Parameters:
    minChannelTime - Minimum time to wait for a probe response (in milliseconds)

  Returns:
    None

  Remarks:
    Default is 200ms
*****************************************************************************/
    void WF_CASetMinChannelTime(uint16_t minChannelTime);

/*****************************************************************************
  Function:
    void WF_CAGetMinChannelTime(uint16_t *p_minChannelTime)

  Summary:
    Gets the current Connection Algorithm minimum channel time.

  Description:
    Gets the minimum time the connection manager waits for a probe response after
    sending a probe request.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_minChannelTime - Pointer where minimum time to wait for a probe response
                       (in milliseconds) will be written.

  Returns:
    None

  Remarks:
    None
*****************************************************************************/
    void WF_CAGetMinChannelTime(uint16_t *p_minChannelTime);

/*****************************************************************************
  Function:
    void WF_CASetMaxChannelTime(uint16_t maxChannelTime)

  Summary:
    Sets the maximum channel time (in milliseconds)

  Description:
    The maximum time (in milliseconds) the connection manager will wait for a
    probe response after sending a probe request.  If no probe responses are
    received in maxChannelTime then the connection manager will go on to the
    next channel, if any are left to scan, or quit.

  Precondition:
    MACInit must be called first.

  Parameters:
    maxChannelTime - Maximum time to wait for a probe response (in milliseconds)

  Returns:
    None

  Remarks:
    Default is 400ms
*****************************************************************************/
    void WF_CASetMaxChannelTime(uint16_t minChannelTime);

/*****************************************************************************
  Function:
    void WF_CAGetMaxChannelTime(uint16_t *p_maxChannelTime)

  Summary:
    Gets the Max Channel Time (in milliseconds)

  Description:
    Gets the maximum time the connection manager waits for a probe response after
    sending a probe request.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_maxChannelTime - Pointer where maximum channel time is written

  Returns:
    None

  Remarks:
    Default is 400ms
*****************************************************************************/
    void WF_CAGetMaxChannelTime(uint16_t *p_minChannelTime);

/*****************************************************************************
  Function:
    void WF_CASetProbeDelay(uint16_t probeDelay)

  Summary:
    Sets the Probe Delay (in microseconds)

  Description:
    The number of microseconds to delay before transmitting a probe request
    following the channel change event.

  Precondition:
    MACInit must be called first.

  Parameters:
    probeDelay - Desired probe delay

  Returns:
    None

  Remarks:
    Default is 20uS
*****************************************************************************/
    void WF_CASetProbeDelay(uint16_t probeDelay);

/*****************************************************************************
  Function:
    void WF_CAGetProbeDelay(uint16_t probeDelay)

  Summary:
    Gets the Probe Delay (in microseconds)

  Description:
    The number of microseconds to delay before transmitting a probe request
    following the channel change event.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_probeDelay - Pointer to where probe delay is written

  Returns:
    None

  Remarks:
    Default is 20uS
*****************************************************************************/
    void WF_CAGetProbeDelay(uint16_t *p_probeDelay);

/*******************************************************************************
  Function:
    void WF_CASetBeaconPeriod(uint16_t beaconPeriod)

  Summary:
    Sets the beacon period in Adhoc start mode for MRF24WG0MA/B

  Description:
    Sets the beacon period used by the Connection Algorithm for MRF24WG0MA/B.

  Precondition:
    MACInit must be called first.

  Parameters:
    beaconPeriod - beacon period in adhoc start mode by ms resoluation

  Returns:
    None.

  Remarks:
    None.
 *****************************************************************************/
    void WF_CASetBeaconPeriod(uint16_t beaconPeriod);

/*******************************************************************************
  Function:
    void WF_CAGetBeaconPeriod(uint16_t *beaconPeriod)

  Summary:
    Retrieves beacon period in Adhoc start mode for MRF24WG0MA/B.

  Description:
    Gets Beacon period in Adhoc start mode for MRF24WG0MA/B.

  Precondition:
    MACInit must be called first.

  Parameters:
    listenInterval -- pointer to where beacon period is returned

  Returns:
    None.

  Remarks:
    None.
 *****************************************************************************/
    void WF_CAGetBeaconPeriod(uint16_t *beaconPeriod);
#endif /* WF_USE_INDIVIDUAL_SET_GETS */

/*******************************************************************************
  Function:
    void WF_CMConnect(uint8_t CpId)

  Summary:
    Commands the MRF24W to start a connection.

  Description:
    Directs the Connection Manager to scan for and connect to a WiFi network.
    This function does not wait until the connection attempt is successful, but
    returns immediately.  See WF_ProcessEvent for events that can occur as a
    result of a connection attempt being successful or not.

    Note that if the Connection Profile being used has WPA-PSK or WPA2-PSK security
    enabled and is using a passphrase, the connection manager will first
    calculate the PSK, and then start the connection process.  The key
    calculation can take up to 30 seconds.

  Precondition:
    MACInit must be called first.

  Parameters:
    CpId - If this value is equal to an existing Connection Profile ID then
            only that Connection Profile will be used to attempt a connection to
            a WiFi network.
            If this value is set to WF_CM_CONNECT_USING_LIST then the
            connectionProfileList will be used to connect, starting with the
            first Connection Profile in the list.

  Returns:
    None.

  Remarks:
    None.
  *****************************************************************************/
void WF_CMConnect(uint8_t CpId);

/*******************************************************************************
  Function:
    uint16_t WF_CMDisconnect(void)

  Summary:
    Commands the MRF24W to close any open connections and/or to cease
    attempting to connect.

  Description:
    Directs the Connection Manager to close any open connection or connection
    attempt in progress.  No further attempts to connect are taken until
    WF_CMConnect() is called.

  Precondition:
    MACInit must be called.

  Parameters:
    None.

  Returns:
    Operation results. Success or Failure

  Remarks:
    Disconnection is allowed only in connected state.
    If MRF24W FW is in the midst of connection (or reconnection) process, then
    disconnect can hammer connection process, and furthermore it may cause
    fatal failure in MRF24W FW operation. To be safe to use disconnect, we strongly
    recommend the user to disable module FW connection manager by setting
    WF_MODULE_CONNECTION_MANAGER == WF_DISABLED in drv_wifi_config.h
  *****************************************************************************/
uint16_t WF_CMDisconnect(void);

/*******************************************************************************
  Function:
    void WF_CMGetConnectionState(uint8_t *p_state, uint8_t *p_currentCpId)

  Summary:
    Returns the current connection state.
    Caution when using WF_CMGetConnectionState, as it may cause redirection issues
    when using iPhone iOS6.1, even though redirection on laptop is functional.
    Users are encouraged to use 1 profile ID for MRF24W based on v5 stack SW. This function is
    retained for backward compatibility.
    In v6 stack SW, we are keeping to 1 profile ID for MRF24W and changing stack SW to have
    capability to handle multiple profile IDs.

  Description:
     Returns the current connection state.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_state - Pointer to location where connection state will be written
    p_currentCpId - Pointer to location of current connection profile ID that
                     is being queried.

  Returns:
    None.

  Remarks:
    None.
  *****************************************************************************/
void WF_CMGetConnectionState(uint8_t *p_state, uint8_t *p_currentCpId);

/*******************************************************************************
  Function:
    void WF_CMCheckConnectionState(uint8_t *p_state, uint8_t *p_currentCpId)

  Summary:
    Returns the current connection state. This API is not valid in SoftAP mode.

  Description:
    Returns the current connection state. This API is not valid in SoftAP mode.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_state - Pointer to location where connection state will be written
    p_currentCpId - Pointer to location of current connection profile ID that
                     is being queried.

  Returns:
    None.

  Remarks:
    None.
  *****************************************************************************/
void WF_CMCheckConnectionState(uint8_t *p_state, uint8_t *p_currentCpId);

#if defined(MRF24WG)
/*******************************************************************************
  Function:
  WF_SetTxMode(uint8_t mode)

  Summary:
    Configures 802.11 transmission (Tx) rates

  Description:
    Configures 802.11 transmission (Tx) rates  for 802.11b or 802.11g
    or 802.11 legacy rates (1-2Mbps)

  Precondition:
    MACInit must be called first.

  Parameters:
    mode - Tx rate
             * WF_TXMODE_G_RATES (default) -- will use all 802.11g rates
             * WF_TXMODE_B_RATES           -- will only use 802.11b rates
             * WF_TXMODE_LEGACY_RATES      -- will only use 1 and 2 mbps rates

  Returns:
    None.

  Remarks:
    None.
 *****************************************************************************/
    void WF_SetTxMode(uint8_t mode);

/*******************************************************************************
  Function:
    void WF_GetTxMode(uint8_t *mode)

  Summary:
    Retrieves transmission (tx) mode that indicates transmission rates

  Description:
    Retrieves transmission (tx) mode that indicates transmission rates (802.11b or
    802.11g or 802.11 legacy rates)

  Precondition:
    MACInit must be called first.

  Parameters:
    p_mode -- pointer to location to store the mode value

  Returns:
    None.

  Remarks:
    None.
 *****************************************************************************/
    void WF_GetTxMode(uint8_t *mode);

/*******************************************************************************
  Function:
    void WFEnableDebugPrint(uint8_t option)

  Summary:
    Can be called to enable printfs for WPS & P2P

  Description:
    Can be called to enable printfs for WPS & P2P

  Precondition:
    MACInit must be called first.

  Parameters:
    option -- has option value to enable the printfs

  Returns:
    None.

  Remarks:
    None.
 *******************************************************************************/
    void WFEnableDebugPrint(uint8_t option);
#endif

/*******************************************************************************
  Function:
    void WF_SetLinkDownThreshold(uint8_t threshold)

  Summary:
    Sets number of consecutive WiFi Tx failures before link is considered down.

  Description:
    This function allows the application to set the number of MRF24W consecutive Tx failures
    before the connection failure event (DRV_WIFI_LINK_LOST) is reported to the host application.

  Precondition:
    MACInit must be called first. 
    In SoftAP mode, WF_SOFTAP_SEND_KEEP_ALIVE needs to be enabled. 
    In other modes, WF_CHECK_LINK_STATUS needs to be enabled. 

  Parameters:
    threshold -- 0:     disabled (default)
                 1-255: number of consecutive Tx failures before connection failure event is reported

  Returns:
    None.

  Remarks:
    None.
 *******************************************************************************/
void WF_SetLinkDownThreshold(uint8_t threshold);

/*----------------------------*/
/* Tx Power Control Functions */
/*----------------------------*/
#if defined(WF_USE_TX_POWER_CONTROL_FUNCTIONS)
    #if defined (MRF24WG)
/*******************************************************************************
  Function:
    void WF_TxPowerSetMax(int8_t maxTxPower)

  Summary:
    Sets the Tx max power on the MRF24WG0MA/B.

  Description:
    After initialization the  MRF24WG0MA/B max Tx power is determined by a
    factory-set value.  This function can set a different maximum
    Tx power levels.  However, this function can never set a maximum Tx power
    greater than the factory-set value, which can be read via
    WF_TxPowerGetFactoryMax().

  Precondition:
    MACInit must be called first.

  Parameters:
    maxTxPower - valid range (0 to 18 dBm)

  Returns:
    None.

  Remarks:
    No conversion of units needed, input to  MRF24WG0MA/B is in dBm.
 *******************************************************************************/
        void WF_TxPowerSetMax(int8_t maxTxPower);

/*******************************************************************************
  Function:
    void WF_TxPowerGetMax(int8_t *p_maxTxPower)

  Summary:
    Gets the Tx max power on the MRF24WG0MA/B.

  Description:
    After initialization the  MRF24WG0MA/B max Tx power is determined by a
    factory-set value.  This function can set a different maximum
    Tx power levels.  However, this function can never set a maximum Tx power
    greater than the factory-set value, which can be read via
    WF_TxPowerGetFactoryMax().

  Precondition:
    MACInit must be called first.

  Parameters:
    p_maxTxPower - Pointer to location to write the maxTxPower

  Returns:
     None.

  Remarks:
      No conversion of units needed, input to MRF24WG0MA/B is in dBm.
  *****************************************************************************/
        void WF_TxPowerGetMax(int8_t *p_maxTxPower);

    //DOM-IGNORE-BEGIN
    #else /* !defined (MRF24WG) */
    //DOM-IGNORE-END
/*******************************************************************************
  Function:
    void WF_TxPowerSetMinMax(int8_t minTxPower, int8_t maxTxPower)

  Summary:
    Sets the Tx min and max power on the MRF24WB0MA/B.

  Description:
    After initialization the MRF24WB0MA/B max Tx power is determined by a
    factory-set value.  This function can set a different minimum and maximum
    Tx power levels.  However, this function can never set a maximum Tx power
    greater than the factory-set value, which can be read via
    WF_TxPowerGetFactoryMax().

  Precondition:
    MACInit must be called first.

  Parameters:
    minTxPower - Desired minTxPower (-10 to 10dB)
    maxTxPower - Desired maxTxPower (-10 to 10dB)

  Returns:
     None.

  Remarks:
      No conversion of units needed, input to MRF24WB0MA/B is in dB.
  *****************************************************************************/
        void WF_TxPowerSetMinMax(int8_t minTxPower, int8_t maxTxPower);

/*******************************************************************************
  Function:
    void WF_TxPowerGetMinMax(int8_t *p_minTxPower, int8_t *p_maxTxPower)

  Summary:
    Gets the Tx min and max power on the MRF24WB0MA/B.

  Description:
    After initialization the MRF24WB0MA/B max Tx power is determined by a
    factory-set value.  This function can set a different minimum and maximum
    Tx power levels.  However, this function can never set a maximum Tx power
    greater than the factory-set value, which can be read via
    WF_TxPowerGetFactoryMax().

  Precondition:
    MACInit must be called first.

  Parameters:
    p_minTxPower - Pointer to location to write the minTxPower
    p_maxTxPower - Pointer to location to write the maxTxPower

  Returns:
     None.

  Remarks:
      No conversion of units needed, input to MRF24WB0MA/B is in dB.
  *****************************************************************************/
        void WF_TxPowerGetMinMax(int8_t *p_minTxPower, int8_t *p_maxTxPower);

/*******************************************************************************
  Function:
    void WF_FixTxRateWithMaxPower(bool oneMegaBps)

  Summary:
    Fix transmission rate with maximum power for MRF24WB0MA/B.

  Description:
    Fix transmission rate with maximum power for MRF24WB0MA/B.

  Precondition:
    MACInit must be called first.

  Parameters:
    oneMegaBps - When true, that is 1 mbps. Otherwise 2 mbps

  Returns:
     None.

  Remarks:
     None.
  *****************************************************************************/
        void WF_FixTxRateWithMaxPower(bool oneMegaBps);

    //DOM-IGNORE-BEGIN
    #endif /* defined (MRF24WG) */
    //DOM-IGNORE-END

/*******************************************************************************
  Function:
    void WF_TxPowerGetFactoryMax(int8_t *p_factoryMaxTxPower)

  Summary:
    Retrieves the factory-set max Tx power from the MRF24WB0MA/B and MRF24WG0MA/B.

  Description:
     For MRF24WB, desired maxTxPower (-10 to 10 dBm), in 1dB steps
     For MRF24WG, desired maxTxPower ( 0 to 18 dBm), in 1dB steps

  Precondition:
    MACInit must be called first.

  Parameters:
    p_factoryMaxTxPower -
    for MRF24WB, desired maxTxPower (-10 to 10 dBm), in 1dB steps
    for MRF24WG, desired maxTxPower ( 0 to 18 dBm), in 1dB steps

  Returns:
     None.

  Remarks:
     None.
  *****************************************************************************/
    void WF_TxPowerGetFactoryMax(int8_t *p_factoryMaxTxPower);
#endif

void WiFiTask(void);

/*----------------------------*/
/* Power Management Functions */
/*----------------------------*/
#if defined(WF_USE_POWER_SAVE_FUNCTIONS)
/*******************************************************************************
  Function:
    void WF_PsPollDisable(void)

  Summary:
    Disables PS-Poll mode.

  Description:
    Disables PS Poll mode.  The MRF24W will stay active and not go sleep.

  Precondition:
    MACInit must be called first.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
 *******************************************************************************/
    void WF_PsPollDisable(void);

/*******************************************************************************
  Function:
    void WF_PsPollEnable(bool rxDtim,  bool aggressive)

  Summary:
    Enables PS Poll mode.

  Description:
    Enables PS Poll mode.  PS-Poll (Power-Save Poll) is a mode allowing for
    longer battery life.  The MRF24W coordinates with the Access Point to go
    to sleep and wake up at periodic intervals to check for data messages, which
    the Access Point will buffer.  The listenInterval in the Connection
    Algorithm defines the sleep interval.  By default, PS-Poll mode is disabled.

    When PS Poll is enabled, the WF Host Driver will automatically force the
    MRF24W to wake up each time the Host sends Tx data or a control message
    to the MRF24W.  When the Host message transaction is complete the
    MRF24W driver will automatically re-enable PS Poll mode.

    When the application is likely to experience a high volume of data traffic
    then PS-Poll mode should be disabled for two reasons:
    1. No power savings will be realized in the presence of heavy data traffic.
    2. Performance will be impacted adversely as the WiFi Host Driver
        continually activates and deactivates PS-Poll mode via SPI messages.

  Precondition:
    MACInit must be called first.

  Parameters:
    rxDtim - true listens at the DTIM interval and false listens at the CASetListenInterval

  Returns:
    None.

  Remarks:
    None.
 *******************************************************************************/
    void WF_PsPollEnable(bool rxDtim);

/*******************************************************************************
  Function:
    void WF_GetPowerSaveState(uint8_t *p_powerSaveState)

  Summary:
    Returns current power-save state.

  Description:
    Returns the current MRF24W power save state.

    <table>
    Value                        Definition
    -----                        ----------
    WF_PS_HIBERNATE              MRF24W in hibernate state
    WF_PS_PS_POLL_DTIM_ENABLED   MRF24W in PS-Poll mode with DTIM enabled
    WF_PS_PS_POLL_DTIM_DISABLED  MRF24W in PS-Poll mode with DTIM disabled
    WF_PS_POLL_OFF               MRF24W is not in any power-save state
    </table>

  Precondition:
    MACInit must be called first.

  Parameters:
    p_powerSaveState - Pointer to where power state is written

  Returns:
    None.

  Remarks:
    None.
 *******************************************************************************/
    void WF_GetPowerSaveState(uint8_t *p_powerSaveState);

/*******************************************************************************
  Function:
    void WF_HibernateEnable()

  Summary:
    Puts the MRF24W into hibernate mode by setting HIBERNATE pin to HIGH.

  Description:
    Enables Hibernate mode on the MRF24W, which effectively turns off the
    device for maximum power savings. HIBERNATE pin on MRF24W is set
    to HIGH.

    MRF24W state is not maintained when it transitions to hibernate mode.
    To remove the MRF24W from hibernate mode call WF_Init().

  Precondition:
    MACInit must be called first.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    Note that because the MRF24W does not save state, there will be a
    disconnect between the TCP/IP stack and the MRF24B0M state.  If it is
    desired by the application to use hibernate, additional measures must be
    taken to save application state.  Then the host should be reset.  This will
    ensure a clean connection between MRF24W and TCP/IP stack

    Future versions of the stack might have the ability to save stack context
    as well, ensuring a clean wake up for the MRF24W without needing a host
    reset.
 *******************************************************************************/
    void WF_HibernateEnable(void);
#endif

/*******************************************************************************
  Function:
    void WF_SetRtsThreshold(uint16_t rtsThreshold)

  Summary:
    Sets the RTS Threshold.

  Description:
    Sets the RTS/CTS packet size threshold for when RTS/CTS frame will be sent.
    The default is 2347 bytes – the maximum for 802.11.  It is recommended that
    the user leave the default at 2347 until they understand the performance and
    power ramifications of setting it smaller.  Valid values are from 0 to
    WF_RTS_THRESHOLD_MAX (2347).

  Precondition:
    MACInit must be called first.

  Parameters:
    rtsThreshold - Value of the packet size threshold

  Returns:
    None.

  Remarks:
    None.
  *****************************************************************************/
void WF_SetRtsThreshold(uint16_t rtsThreshold);

/*******************************************************************************
  Function:
    void WF_GetRtsThreshold(uint16_t *p_rtsThreshold)

  Summary:
    Gets the RTS Threshold

  Description:
    Gets the RTS/CTS packet size threshold.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_rtsThreshold - Pointer to where RTS threshold is written

  Returns:
    None.

  Remarks:
    None.
  *****************************************************************************/
void WF_GetRtsThreshold(uint16_t *p_rtsThreshold);

/*******************************************************************************
  Function:
    void WF_YieldPassphraseToHost(void)

  Summary:
    Allows host to handle WPS WPA/WPA2-PSK passphrase

  Description:
     During WPS the MRF24WG will receive a passphrase.  That passphrase must be
     converted to a binary key.  The MRF24WG can do this, but it will take 20-30
     seconds.  The host MCU will be able to convert the passphrase to a
     binary key much faster.  If that is desired, then this function should be called
     to request the MRF24WG to signal the DRV_WIFI_EVENT_KEY_CALCULATION_REQUEST
     event when the passphrase is ready.  Then the host can retrieve the passphrase
     (see DRV_WIFI_WPSCredentialsGet()),  convert to a binary key (see
     WF_ConvPassphraseToKey()), and send it back to the MRF24WG via the
     DRV_WIFI_SetPSK() function.

  Precondition:
    MACInit must be called first.

  Parameters:
       None

  Returns:
    None.

  Remarks:
    None.
 *****************************************************************************/
void WF_YieldPassphraseToHost(void);

/*******************************************************************************
  Function:
    void WF_SetPSK(uint8_t *psk)

  Summary:
    Sends PSK to module FW in WPS mode.

  Description:
    This function is used in conjunction with DRV_WIFI_YieldPassphraseToHost().
    It sends the binary key to the MRF24WG after the host has converted an ASCII
    passphrase to a binary key.

  Precondition:
    MACInit must be called first.

  Parameters:
       None

  Returns:
    None.

  Remarks:
    None.

 *****************************************************************************/
void WF_SetPSK(uint8_t *psk);

/*******************************************************************************
  Function:
    void WF_SetRegionalDomain(uint8_t regionalDomain)

  Summary:
    Enables or disables the MRF24W Regional Domain. This function is NOT supported
    due to FCC requirements, which does not allow programming of the regional domain.

  Description:
    MRF24W is programmed with FCC regional domain as default.
    To cater for other regional domains, use WF_CASetChannelList()
    to set up specific channels.

  Precondition:
    MACInit must be called first.  This function must not be called while in a
    connected state.

  Parameters:
    regionalDomain - Value to set the regional domain to

  Returns:
    None.

  Remarks:
    None.
  *****************************************************************************/
void WF_SetRegionalDomain(uint8_t regionalDomain);     /* see tWFRegDomain enumerated types */

/*******************************************************************************
  Function:
    void WF_GetRegionalDomain(uint8_t *p_regionalDomain)

  Summary:
    Retrieves the MRF24W Regional domain

  Description:
    Gets the regional domain on the MRF24W.
    MRF24W is programmed with FCC regional domain as default.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_regionalDomain - Pointer where the regional domain value will be written

  Returns:
    None.

  Remarks:
    None.
  *****************************************************************************/
void WF_GetRegionalDomain(uint8_t *p_regionalDomain);  /* see tWFRegDomain enumerated types */

#if defined(WF_USE_MULTICAST_FUNCTIONS)
/*******************************************************************************
  Function:
    void WF_SetMultiCastFilter(uint8_t multicastFilterId,
                               uint8_t multicastAddress[6])

  Summary:
    Sets a multicast address filter using one of the two multicast filters.

  Description:
    This function allows the application to configure up to two Multicast
    Address Filters on the MRF24WB0M.  If two active multicast filters are set
    up they are OR’d together – the MRF24WB0M will receive and pass to the Host
    CPU received packets from either multicast address.
    The allowable values for the multicast filter are:
    * WF_MULTICAST_FILTER_1
    * WF_MULTICAST_FILTER_2

    By default, both Multicast Filters are inactive.

  Precondition:
    MACInit must be called first.

  Parameters:
    multicastFilterId - WF_MULTICAST_FILTER_1 or WF_MULTICAST_FILTER_2
    multicastAddress  - 6-byte address (all 0xFF will inactivate the filter)

  Returns:
    None.

  Remarks:
    Definition WF_USE_MULTICAST_FUNCTIONS needs to be enabled.
  *****************************************************************************/
    void WF_SetMultiCastFilter(uint8_t multicastFilterId, uint8_t multicastAddress[6]);

/*******************************************************************************
  Function:
    void WF_GetMultiCastFilter(uint8_t multicastFilterId,
                               uint8_t multicastAddress[6])

  Summary:
    Gets a multicast address filter from one of the two multicast filters.

  Description:
    Gets the current state of the specified Multicast Filter.

    Normally would call SendGetParamMsg, but this GetParam returns all 6 address
    filters + 2 more bytes for a total of 48 bytes plus header. So, doing this
    msg manually to not require a large stack allocation to hold all the data.

    Exact format of returned message is:
    <table>
    [0]     -- always mgmt response (2)
    [1]     -- always WF_GET_PARAM_SUBTYPE (16)
    [2]     -- result (1 if successful)
    [3]     -- mac state (not used)
    [4]     -- data length (length of response data starting at index 6)
    [5]     -- not used
    [6-11]  -- Compare Address 0 address
    [12]    -- Compare Address 0 group
    [13]    -- Compare Address 0 type
    [14-19] -- Compare Address 1 address
    [20]    -- Compare Address 1 group
    [21]    -- Compare Address 1 type
    [22-27] -- Compare Address 2 address
    [28]    -- Compare Address 2 group
    [29]    -- Compare Address 2 type
    [30-35] -- Compare Address 3 address
    [36]    -- Compare Address 3 group
    [37]    -- Compare Address 3 type
    [38-43] -- Compare Address 4 address
    [44]    -- Compare Address 4 group
    [45]    -- Compare Address 4 type
    [46-51] -- Compare Address 5 address
    [52]    -- Compare Address 5 group
    [53]    -- Compare Address 5 type
   </table>

  Precondition:
    MACInit must be called first.

  Parameters:
    multicastFilterId - WF_MULTICAST_FILTER_1 or WF_MULTICAST_FILTER_2
    multicastAddress - 6-byte address

  Returns:
    None.

  Remarks:
    None.
  *****************************************************************************/
    void WF_GetMultiCastFilter(uint8_t multicastFilterId, uint8_t multicastAddress[6]);
    
    #if defined(MRF24WG)
/*******************************************************************************
  Function:
    void WF_MulticastSetConfig(tWFMultiCastConfig *p_config);

  Summary:
    Sets a multicast address filter using one of the two multicast filters.

  Description:
    This function allows the application to configure up to two Multicast
    Address Filters on the MRF24W.  If two active multicast filters are set
    up they are OR’d together – the MRF24W will receive and pass to the Host
    CPU received packets from either multicast address.
    The allowable values in p_config (tWFMultiCastConfig / WFMulticastConfigStruct)
    are:

    * filterId -- WF_MULTICAST_FILTER_1 or WF_MULTICAST_FILTER_2

    * action   -- WF_MULTICAST_DISABLE_ALL (default)
                   The Multicast Filter discards all received
                   multicast messages – they will not be forwarded
                   to the Host PIC.  The remaining fields in this
                   structure are ignored.

                WF_MULTICAST_ENABLE_ALL
                   The Multicast Filter forwards all received multicast messages
                   to the Host PIC. The remaining fields in this structure are
                   ignored.

                WF_MULTICAST_USE_FILTERS
                   The MAC filter will be used and the remaining fields in this
                   structure configure which Multicast messages are forwarded to
                   the Host PIC.

    * macBytes -- Array containing the MAC address to filter on (using the destination
                address of each incoming 802.11 frame).  Specific bytes with the
                MAC address can be designated as ‘don’t care’ bytes.  See macBitMask.
                This field in only used if action = WF_MULTICAST_USE_FILTERS.

    * macBitMask -- A byte where bits 5:0 correspond to macBytes[5:0].  If the bit is
                  zero then the corresponding MAC byte must be an exact match for the
                  frame to be forwarded to the Host PIC.  If the bit is one then the
                  corresponding MAC byte is a ‘don’t care’ and not used in the
                  Multicast filtering process.  This field in only used if
                  action = WF_MULTICAST_USE_FILTERS.

    By default, both Multicast Filters are inactive.

    Example -- Filter on Multicast Address of 01:00:5e:xx:xx:xx where xx are don't care bytes.
                  p_config->filterId = WF_MULTICAST_FILTER_1

                                         [0] [1] [2] [3] [4] [5]
                  p_config->macBytes[] = 01, 00, 5e, ff, ff, ff  (0xff are the don't care bytes)

                  p_config->macBitMask = 0x38 --> bits 5:3 = 1 (don't care on bytes 3,4,5)
                                              --> bits 2:0 = 0 (exact match required on bytes 0,1,2)

  Precondition:
    MACInit must be called first.

  Parameters:
    p_config -- pointer to the multicast config structure.  See documentation.

  Returns:
    None.

  Remarks:
    Definition WF_USE_MULTICAST_FUNCTIONS needs to be enabled.
 *****************************************************************************/
        void WF_MulticastSetConfig(tWFMultiCastConfig *p_config);

/*******************************************************************************
  Function:
    void WF_EnableSWMultiCastFilter(void)

  Summary:
    Forces the module FW to use software filter instead of hardware filter

  Description:
    This function allows the application to configure up to 16 software-based Multicast
    Address Filters on the MRF24WG0MA/B.

  Precondition:
    MACInit must be called first.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    Definition WF_SOFTWARE_MULTICAST_FILTER needs to be set to WF_ENABLED
  *****************************************************************************/
        void WF_EnableSWMultiCastFilter(void);
    #endif
#endif /* WF_USE_MULTICAST_FUNCTIONS */

/*******************************************************************************
  Function:
    void WF_GetMacStats(tWFMacStats *p_macStats)

  Summary:
    Gets MAC statistics.

  Description:
    Returns MAC statistics on number of frames received or transmitted for defined situations
    such as number of frames transmitted with multicast bit set in destination MAC address.
    Refer to drv_wifi_api.h for data struct WFMacStatsStruct / tWFMacStats.

  Precondition:
    MACInit must be called first.

  Parameters:
    p_macStats - Pointer to where MAC statistics are written

  Returns:
    None.

  Remarks:
    None.
  *****************************************************************************/
void WF_GetMacStats(tWFMacStats *p_macStats);

/*----------------*/
/* Scan Functions */
/*----------------*/
#if defined(WF_USE_SCAN_FUNCTIONS)
/*******************************************************************************
  Function:
    uint16_t WF_Scan(uint8_t CpId)

  Summary:
    Commands the MRF24W to start a scan operation.  This will generate the
    WF_EVENT_SCAN_RESULTS_READY event.

  Description:
    Directs the MRF24W to initiate a scan operation utilizing the input
    Connection Profile ID.  The Host Application will be notified that the scan
    results are ready when it receives the WF_EVENT_SCAN_RESULTS_READY event.
    The eventInfo field for this event will contain the number of scan results.
    Once the scan results are ready they can be retrieved with
    WF_ScanGetResult().

    Scan results are retained on the MRF24W until:
    1.  Calling WF_Scan() again (after scan results returned from previous
        call).
    2.  MRF24W reset.

    MRF24WB0M & MRF24WG0M support up to max of 60 scan results (SSIDs).

  Precondition:
    MACInit must be called first.

  Parameters:
    CpId - Connection Profile to use.
            If the CpId is valid then the values from that Connection Profile
            will be used for filtering scan results.  If the CpId is set to
            WF_SCAN_ALL (0xFF) then a default filter will be used.

            Valid CpId
            * If CP has a defined SSID only scan results with that SSID are
               retained.
            * If CP does not have a defined SSID then all scanned SSID’s will be
               retained
            * Only scan results from Infrastructure or AdHoc networks are
               retained, depending on the value of networkType in the Connection Profile
            * The channel list that is scanned will be determined from
               channelList in the Connection Algorithm (which must be defined
               before calling this function).

            CpId is equal to WF_SCAN_ALL
            * All scan results are retained (both Infrastructure and Ad Hoc
               networks).
            * All channels within the MRF24W’s regional domain will be
               scanned.
            * No Connection Profiles need to be defined before calling this
               function.
            * The Connection Algorithm does not need to be defined before
               calling this function.

  Returns:
    Operation results. Success or Failure

  Remarks:
    Host scan is allowed only in idle or connected state.
    If MRF24W FW is in the midst of connection ( or reconnection) process, then
    host scan can hammer connection process, and furthermore it may cause
    fatal failure in MRF24W FW operation. To use host scan, we strongly
    recommend the user to disable MRF24W FW connection manager by setting
    WF_MODULE_CONNECTION_MANAGER == WF_DISABLED in drv_wifi_config.h
  *****************************************************************************/
    uint16_t WF_Scan(uint8_t CpId);

/*******************************************************************************
  Function:
    void WF_ScanGetResult(uint8_t listIndex, tWFScanResult  *p_scanResult)

  Summary:
    Read scan results back from MRF24W.

  Description:
    After a scan has completed this function is used to read one or more of the
    scan results from the MRF24W.  The scan results will be written
    contiguously starting at p_scanResult (see tWFScanResult structure for
    format of scan result).
    MRF24WB0M & MRF24WG0M support up to max of 60 scan results (SSIDs).

  Precondition:
    MACInit must be called first.  WF_EVENT_SCAN_RESULTS_READY event must have
    already occurrerd.

  Parameters:
    listIndex - Index (0-based list) of the scan entry to retrieve.
    p_scanResult - Pointer to location to store the scan result structure

  Returns:
    None.

  Remarks:
      RSSI can only be obtained from the scan results p_scanResult->rssi.
      MRF24W checks out the signal strength from the preamble of the incoming packets.
      The higher the values, the stronger is the received signal strength.

      p_scanResult->rssi contains received signal strength indicator (RSSI).
      * MRF24WB : RSSI_MAX (200) , RSSI_MIN (106).
      * MRF24WG : RSSI_MAX (128) , RSSI_MIN (43).

       The RSSI value is not directly translated to dbm because this is not calibrated number.
       However, as a guideline, for 802.11b, MAX(200) corresponds to 0 dbm, MIN (106) corresponds to -94 dbm.

  *****************************************************************************/
    void WF_ScanGetResult(uint8_t         listIndex,
                          tWFScanResult *p_scanResult);
#endif /* WF_SCAN_FUNCTIONS */

/*****************************************************************************
 * FUNCTION:WF_EintInit (Specific to PIC18)
 *
 * RETURNS: None
 *
 * PARAMS:  None
 *
 * NOTES:   Called by WiFi Driver to initialize the MRF24W External Interrupt.
 *****************************************************************************/
void WF_EintInit(void);

/*****************************************************************************
 * FUNCTION:WF_EintEnable (Specific to PIC18)
 *
 * RETURNS: None
 *
 * PARAMS:  None
 *
 * NOTES:   Called by WiFi Driver to enable the MRF24W External Interrupt.
 *****************************************************************************/
void WF_EintEnable(void);

/*****************************************************************************
 * FUNCTION:WF_EintDisable (Specific to PIC18)
 *
 * RETURNS: None
 *
 * PARAMS:  None
 *
 * NOTES:   Called by WiFi Driver to disable the MRF24W External Interrupt.
 *****************************************************************************/
void WF_EintDisable(void);

/*****************************************************************************
 * FUNCTION:WF_EintIsDisabled
 *
 * RETURNS: true if MRF24W External Interrupt is disabled, else returns false
 *
 * PARAMS:  None
 *
 * NOTES:   Called by WiFi Driver to determine if the MRF24W External Interrupt
 *          is disabled.
 *****************************************************************************/
bool WF_EintIsDisabled(void);

/*****************************************************************************
 * FUNCTION: WFEintHandler
 *
 * RETURNS: N/A
 *
 * PARAMS:
 *      N/A
 *
 *
 *  NOTES: This function must be called once, each time an external interrupt
 *      is received from the WiFi device.   The WiFi Driver will schedule any
 *      subsequent SPI communication to process the interrupt.
 *
 * IMPORTANT NOTE: This function, and functions that are called by this function
 *                 must NOT use local variables.  The PIC18, or any other processor
 *                 that uses overlay memory will corrupt the logical stack within
 *                 overlay memory if the interrupt uses local variables.
 *                 If local variables are used within an interrupt routine the toolchain
 *                 cannot properly determine how not to overwrite local variables in
 *                 non-interrupt releated functions, specifically the function that was
 *                 interrupted.
 *****************************************************************************/
void WFEintHandler(void);

/*****************************************************************************
 * FUNCTION: WF_EintIsPending
 *
 * RETURNS: true if MRF24W External Interrupt is disabled, else returns false
 *
 * PARAMS:  None
 *
 * NOTES:
 * Used by the WF Driver to test for whether external interrupts are pending.
 * The pending case is not something that should normally happen.  It says
 * we have the interrupt line asserted but the WF_EINT_IF bit is not set,
 * thus, no interrupt generated.
 *****************************************************************************/
bool WF_EintIsPending(void);

/*****************************************************************************
  Function:
    void WF_SpiInit(void)

  Summary:
    Initializes the SPI interface to the MRF24W device.

  Description:
    Configures the SPI interface for communications with the MRF24W.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    This function is called by WFHardwareInit.
*****************************************************************************/
void WF_SpiInit(void);

/*****************************************************************************
  Function:
    void WF_SpiEnableChipSelect(void)

  Summary:
    Enables the MRF24W SPI chip select.

  Description:
    Enables the MRF24W SPI chip select as part of the sequence of SPI
    communications.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    If the SPI bus is shared with other peripherals then the current SPI context
    is saved.
*****************************************************************************/
void WF_SpiEnableChipSelect(void);

/*****************************************************************************
  Function:
    void WF_SpiDisableChipSelect(void)

  Summary:
    Disables the MRF24W SPI chip select.

  Description:
    Disables the MRF24W SPI chip select as part of the sequence of SPI
    communications.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    If the SPI bus is shared with other peripherals then the current SPI context
    is restored.
*****************************************************************************/
void WF_SpiDisableChipSelect(void);

/*****************************************************************************
  Function:
    void WFSpiTxRx(void)

  Summary:
    Transmits and receives SPI bytes

  Description:
    Transmits and receives N bytes of SPI data.

  Precondition:
    None

  Parameters:
    p_txBuf - pointer to SPI tx data
    txLen   - number of bytes to Tx
    p_rxBuf - pointer to where SPI rx data will be stored
    rxLen   - number of SPI rx bytes caller wants copied to p_rxBuf

  Returns:
    None

  Remarks:
    Will clock out the larger of txLen or rxLen, and pad if necessary.
*****************************************************************************/
void WFSpiTxRx(uint8_t   *p_txBuf,
               uint16_t  txLen,
               uint8_t   *p_rxBuf,
               uint16_t  rxLen);

#if defined (__XC8)
/*****************************************************************************
  Function:
    void WFSpiTxRx_Rom(void)

  Summary:
    Transmits and receives SPI bytes

  Description:
    Specific to the PIC18, transmits bytes from ROM storage and receives SPI data
    bytes.

  Precondition:
    None

  Parameters:
    p_txBuf - pointer to SPI ROM tx data
    txLen   - number of bytes to Tx
    p_rxBuf - pointer to where SPI rx data will be stored
    rxLen   - number of SPI rx bytes caller wants copied to p_rxBuf

  Returns:
    None

  Remarks:
    Will clock out the larger of txLen or rxLen, and pad if necessary.
*****************************************************************************/
    void WFSpiTxRx_Rom(ROM uint8_t *p_txBuf,
                       uint16_t    txLen,
                       uint8_t     *p_rxBuf,
                       uint16_t    rxLen);
#endif /* __XC8 */

/*****************************************************************************
 * FUNCTION: WF_ProcessEvent
 *
 * RETURNS:  None
 *
 * PARAMS:   event      -- event that occurred
 *           eventInfo  -- additional information about the event.  Not all events
 *           have associated info, in which case this value will be
 *           set to WF_NO_ADDITIONAL_INFO (0xff)
 *           extraInfo - more additional information about the event
 *
 *  NOTES:   The Host application must NOT directly call this function.  This
 *           function is called by the WiFi Driver code when an event occurs
 *           that may need processing by the Host CPU.
 *
 *           No other WiFi Driver function should be called from this function, with the
 *           exception of WF_ASSERT.  It is recommended that if the application wishes to be
 *           notified of an event that it simply set a flag and let application code in the
 *           main loop handle the event.
 *
 *           WFSetFuncState must be called when entering and leaving this function.
 *           When WF_DEBUG is enabled this allows a runtime check if any illegal WF functions
 *           are called from within this function.
 *
 *           For events that the application is not interested in simply leave the
 *           case statement empty.
  *
 *           Customize this function as needed for your application.
 *****************************************************************************/
void WF_ProcessEvent(uint8_t event, uint16_t eventInfo, uint8_t *extraInfo);

#if defined(MRF24WG)
    void WF_DisplayModuleAssertInfo(void);
#endif

#if defined(WF_CM_DEBUG)
    void WF_CMInfoGetFSMStats(tWFCMInfoFSMStats *p_info);
#endif

/*******************************************************************************
  Function:
    void WF_DisableModuleConnectionManager(void)

  Summary:
    When compilation flag WF_MODULE_CONNECTION_MANAGER is set to WF_DISABLED,
    this will disable MRF24W connection manager.

  Description:
    Disable MRF24W connection manager.

  Precondition:
    MACInit must be called first.

  Parameters:
    None.

  Returns:
    None.

  Remarks:
    None.
 *****************************************************************************/
extern void WF_DisableModuleConnectionManager(void);

extern void pbkdf2_sha1(const char *passphrase, const char *ssid, uint16_t ssid_len,
                        uint16_t iterations, uint8_t *buf, uint16_t buflen);

extern void WF_ConvPassphraseToKey(uint8_t key_len, uint8_t *key, uint8_t ssid_len, uint8_t *ssid);

#endif /* __DRV_WF_API_H_ */
