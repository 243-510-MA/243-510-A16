/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_wifi_event_handler.c

  Summary:
    Module for Microchip TCP/IP Stack
    -Provides access to MRF24W WiFi controller
    -Reference: MRF24W Data sheet, IEEE 802.11 Standard

  Description:
    MRF24W Driver Event Handler
   

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

/*******************************************************************************
 *                                   INCLUDES
 *******************************************************************************/
#include "tcpip/tcpip.h"

#if defined(WF_CS_TRIS)

#include "drv_wifi_mac.h"

/*******************************************************************************
 *                                  DEFINES
 *******************************************************************************/
/* used for assertions */
#if defined(WF_DEBUG)
    #define WF_MODULE_NUMBER  WF_MODULE_WF_EVENT_HANDLER
#endif

/*-------------------------------------------*/
/* Connection Manager Event Message Subtypes */
/* (Used in Mgmt Indicate messages)          */
/*-------------------------------------------*/
#define WF_EVENT_CONNECTION_ATTEMPT_STATUS_SUBTYPE (6)
#define WF_EVENT_CONNECTION_LOST_SUBTYPE           (7)
#define WF_EVENT_CONNECTION_REESTABLISHED_SUBTYPE  (8)
#define WF_EVENT_KEY_CALCULATION_REQUEST_SUBTYPE   (9)
#define WF_EVENT_SCAN_RESULTS_READY_SUBTYPE        (11)
#define WF_EVENT_SCAN_IE_RESULTS_READY_SUBTYPE     (12)
#define WF_EVENT_SOFT_AP_EVENT_SUBTYPE             (13)

/* event values for index 2 of WF_CONNECTION_ATTEMPT_STATUS_EVENT_SUBTYPE */
#define CONNECTION_ATTEMPT_SUCCESSFUL    ((uint8_t)1) /* if not 1 then failed to connect and info field is error code */
#define CONNECTION_ATTEMPT_FAILED        ((uint8_t)2)

/* event values for index 2 of WF_EVENT_CONNECTION_LOST_SUBTYPE */
#define CONNECTION_TEMPORARILY_LOST      ((uint8_t)1)
#define CONNECTION_PERMANENTLY_LOST      ((uint8_t)2)
#define CONNECTION_REESTABLISHED         ((uint8_t)3)

/*******************************************************************************
 *                                  LOCAL FUNCTIONS
 *******************************************************************************/
static bool isNotifyApp(uint8_t event);
static bool isEventNotifyBitSet(uint8_t notifyMask, uint8_t notifyBit);

/* if a MRF24WB device */
#if !defined(MRF24WG)
    #define RAW_MGMT_RX_ID  RAW_RX_ID
#endif

extern void SignalWiFiConnectionChanged(bool state);
extern void RenewDhcp(void);

/*****************************************************************************
 * FUNCTION: WFProcessMgmtIndicateMsg
 *
 * RETURNS:  error code
 *
 * PARAMS:   None
 *
 *  NOTES:   Processes a management indicate message
 *****************************************************************************/
void WFProcessMgmtIndicateMsg()
{
    tMgmtIndicateHdr  hdr;
    uint8_t buf[6];
    uint8_t event = 0xff;
    uint16_t eventInfo;

#if defined(MRF24WG)
    tMgmtIndicatePassphraseReady passphraseReady;
    tMgmtIndicateSoftAPEvent softAPEvent;
#endif
    uint8_t *extra = NULL;

    /* read 2-byte header of management message */
    RawRead(RAW_MGMT_RX_ID, 0, sizeof(tMgmtIndicateHdr), (uint8_t *)&hdr);

    /* Determine which event occurred and handle it */
    switch (hdr.subType)
    {
    /*-----------------------------------------------------------------*/
    case WF_EVENT_CONNECTION_ATTEMPT_STATUS_SUBTYPE:
    /*-----------------------------------------------------------------*/
#if defined(MRF24WG)
        /* There is one data byte with this message */
        RawRead(RAW_MGMT_RX_ID, sizeof(tMgmtIndicateHdr),2, buf); /* read first 2 bytes after header */
        /* if connection attempt successful */
        if (buf[0] == CONNECTION_ATTEMPT_SUCCESSFUL)
        {
            event = WF_EVENT_CONNECTION_SUCCESSFUL;
            eventInfo = WF_NO_ADDITIONAL_INFO;
            SignalWiFiConnectionChanged(true);
            #if defined (STACK_USE_DHCP_CLIENT)
                RenewDhcp();
            #endif
            SetLogicalConnectionState(true);
        }
        /* else connection attempt failed */
        else
        {
            event = WF_EVENT_CONNECTION_FAILED;
            eventInfo = (uint16_t)(buf[0] << 8 | buf[1]); /* contains connection failure code */
            SetLogicalConnectionState(false);
            #ifdef LED_STOP_BLINKING_IF_CONNECTION_FAILED // WiFi G Demo Board stops blinking the LED to
                                                          // indicate a failed connection
                if ( (uint8_t)(eventInfo >> 8) != WF_RSN_MIXED_MODE_NOT_SUPPORTED )
                    AppConfig.connectionFailedFlag = 1;
            #endif
        }

#else /* !defined(MRF24WG) */
    /* There is one data byte with this message */
    RawRead(RAW_MGMT_RX_ID, sizeof(tMgmtIndicateHdr), 1, buf); /* read first byte after header */
    /* if connection attempt successful */
    if (buf[0] == CONNECTION_ATTEMPT_SUCCESSFUL)
    {
        event = WF_EVENT_CONNECTION_SUCCESSFUL;
        eventInfo = WF_NO_ADDITIONAL_INFO;
        SignalWiFiConnectionChanged(true);
        #if defined (STACK_USE_DHCP_CLIENT)
            RenewDhcp();
        #endif
        SetLogicalConnectionState(true);
    }
    /* else connection attempt failed */
    else
    {
        event = WF_EVENT_CONNECTION_FAILED;
        eventInfo = (uint16_t)buf[0]; /* contains connection failure code */
        SetLogicalConnectionState(false);
    }
#endif /* defined(MRF24WG) */
    break;

    /*-----------------------------------------------------------------*/
    case WF_EVENT_CONNECTION_LOST_SUBTYPE:
    /*-----------------------------------------------------------------*/
        /* read index 2 and 3 from message and store in buf[0] and buf[1]
           buf[0] -- 1: Connection temporarily lost  2: Connection permanently lost 3: Connection Reestablished
           buf[1] -- 0: Beacon Timeout  1: Deauth from AP  */
        RawRead(RAW_MGMT_RX_ID, sizeof(tMgmtIndicateHdr), 2, buf);

        if (buf[0] == CONNECTION_TEMPORARILY_LOST)
        {
            event     = WF_EVENT_CONNECTION_TEMPORARILY_LOST;
            eventInfo = (uint16_t)buf[1];    /* lost due to beacon timeout or deauth */
            SignalWiFiConnectionChanged(false);
        }
        else if (buf[0] == CONNECTION_PERMANENTLY_LOST)
        {
            event     = WF_EVENT_CONNECTION_PERMANENTLY_LOST;
            eventInfo = (uint16_t)buf[1];   /* lost due to beacon timeout or deauth */
            SetLogicalConnectionState(false);
            SignalWiFiConnectionChanged(false);
        }
        else if (buf[0] == CONNECTION_REESTABLISHED)
        {
            event     = WF_EVENT_CONNECTION_REESTABLISHED;
            eventInfo = (uint16_t)buf[1];    /* originally lost due to beacon timeout or deauth */
            #if defined(STACK_USE_DHCP_CLIENT)
                RenewDhcp();
            #endif
            SignalWiFiConnectionChanged(true);
            SetLogicalConnectionState(true);
        }
        else
        {
            /* invalid parameter in message */
            WF_ASSERT(false);
        }
        break;

    /*-----------------------------------------------------------------*/
    case WF_EVENT_SCAN_RESULTS_READY_SUBTYPE:
    /*-----------------------------------------------------------------*/
        /* read index 2 of mgmt indicate to get the number of scan results */
        RawRead(RAW_MGMT_RX_ID, sizeof(tMgmtIndicateHdr), 1, buf);
        event = WF_EVENT_SCAN_RESULTS_READY;
        eventInfo = (uint16_t)buf[0]; /* number of scan results */
        break;

    /*-----------------------------------------------------------------*/
    case WF_EVENT_SCAN_IE_RESULTS_READY_SUBTYPE:
    /*-----------------------------------------------------------------*/
        event = WF_EVENT_IE_RESULTS_READY;
        /* read indexes 2 and 3 containing the 16-bit value of IE bytes */
        RawRead(RAW_MGMT_RX_ID, sizeof(tMgmtIndicateHdr), 2, (uint8_t *)&eventInfo);
        eventInfo = WFSTOHS(eventInfo);     /* fix endianess of 16-bit value */
        break;

#if defined(MRF24WG)
    case WF_EVENT_KEY_CALCULATION_REQUEST_SUBTYPE:
        event = WF_EVENT_KEY_CALCULATION_REQUEST;
        RawRead(RAW_MGMT_RX_ID, sizeof(tMgmtIndicateHdr),
        sizeof(tMgmtIndicatePassphraseReady), (uint8_t *)&passphraseReady);
        extra = (uint8_t *)&passphraseReady;
        break;
    case WF_EVENT_SOFT_AP_EVENT_SUBTYPE:    /* Valid only with 3108 or the later module FW version */
        event = WF_EVENT_SOFT_AP_EVENT;
        RawRead(RAW_MGMT_RX_ID, sizeof(tMgmtIndicateHdr),
        sizeof(tMgmtIndicateSoftAPEvent), (uint8_t *)&softAPEvent);
        extra = (uint8_t *)&softAPEvent;
        break;
#endif

    /*-----------------------------------------------------------------*/
    default:
    /*-----------------------------------------------------------------*/
        WF_ASSERT(false);
        break;
    }

    /* free mgmt buffer */
    DeallocateMgmtRxBuffer();

    /* if the application wants to be notified of the event */
    if (isNotifyApp(event))
    {
        WF_ProcessEvent(event, eventInfo, extra);
    }
}

/*****************************************************************************
 * FUNCTION: isEventNotifyBitSet
 *
 * RETURNS:  true if the notify bit is set in the notify mask.
 *
 * PARAMS:   notifyMask -- the bit mask of events the application wishes to be
 *                         notified of
 *           notifyBit  -- the specific event that occurred
 *
 *  NOTES:   Determines if the input event it enabled in the notify mask
 *****************************************************************************/
static bool isEventNotifyBitSet(uint8_t notifyMask, uint8_t notifyBit)
{
    /* check if the event notify bit is set */
    return ((notifyMask & notifyBit) > 0);
}

/*****************************************************************************
 * FUNCTION: isNotifyApp
 *
 * RETURNS:  true if application wants to be notified of event, else false
 *
 * PARAMS:   event -- the event that occurred
 *
 *  NOTES:   Determines if the input event is one which the application should be
 *           notified of.
 *****************************************************************************/
static bool isNotifyApp(uint8_t event)
{
    bool notify = false;
    uint8_t notifyMask;

    notifyMask = GetEventNotificationMask();

    /* determine if user wants to be notified of event */
    switch (event) {
    case WF_EVENT_CONNECTION_SUCCESSFUL:
        if (isEventNotifyBitSet(notifyMask, WF_NOTIFY_CONNECTION_ATTEMPT_SUCCESSFUL))
        {
            notify = true;
        }
        break;

    case WF_EVENT_CONNECTION_FAILED:
        if (isEventNotifyBitSet(notifyMask, WF_NOTIFY_CONNECTION_ATTEMPT_FAILED))
        {
            notify = true;
        }
        break;

    case WF_EVENT_CONNECTION_TEMPORARILY_LOST:
        if (isEventNotifyBitSet(notifyMask, WF_NOTIFY_CONNECTION_TEMPORARILY_LOST))
        {
            notify = true;
        }
        break;

    case WF_EVENT_CONNECTION_PERMANENTLY_LOST:
        if (isEventNotifyBitSet(notifyMask, WF_NOTIFY_CONNECTION_PERMANENTLY_LOST))
        {
            notify = true;
        }
        break;

    case WF_EVENT_CONNECTION_REESTABLISHED:
        if (isEventNotifyBitSet(notifyMask, WF_NOTIFY_CONNECTION_REESTABLISHED))
        {
            notify = true;
        }
        break;

    default:
        notify = true;  /* the app gets notified of all other events */
        break;
    }

    return notify;

}

#endif /* WF_CS_TRIS */
