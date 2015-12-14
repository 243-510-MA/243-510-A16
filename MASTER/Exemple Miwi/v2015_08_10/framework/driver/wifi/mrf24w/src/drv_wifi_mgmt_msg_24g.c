/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_wifi_mgmt_msg_24g.c

  Summary:
    Module for Microchip TCP/IP Stack
    -Provides access to MRF24W WiFi controller
    -Reference: MRF24W Data sheet, IEEE 802.11 Standard

  Description:
    MRF24W Driver Management Messages

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
*                                 INCLUDES
********************************************************************************/

#include "tcpip/tcpip.h"

#if defined(WF_CS_TRIS)

/*******************************************************************************
*                                DEFINES
********************************************************************************/

/* used for assertions */
#if defined(WF_DEBUG)
    #define WF_MODULE_NUMBER   WF_MODULE_WF_MGMT_MSG
#endif

/* Macro used to determine is application is trying to send a management when in the driver function  */
/* WF_ProcessEvent().                                                                                 */
#define isInWFProcessEvent()    ((g_FuncFlags & WF_PROCESS_EVENT_FUNC) > 0)

/*******************************************************************************
*                                 LOCAL GLOBAL VARIABLES
********************************************************************************/

static volatile bool gMgmtConfirmMsgReceived  = false;
bool g_WaitingForMgmtResponse = false;

#if defined(WF_DEBUG)
   static uint8_t g_FuncFlags = 0x00;
#endif

/*******************************************************************************
  Function:
    void SendMgmtMsg(uint8_t *p_header, uint8_t headerLength, uint8_t *p_data, uint8_t dataLength)

  Summary:
    Sends a management message to MRF24W.

  Description:
    Allocates management buffer on MRF24W. Then sends the management message
    (header + data) to MRF24W.

  Precondition:
    MACInit must be called.

  Parameters:
    p_header        -- pointer to mgmt message header data
    headerLength  -- number of bytes in the header will be written
    p_data            -- pointer to mgmt message data
    dataLength     -- number of bytes of data

  Returns:
    None

  Remarks:
      *****************************************************************************/
void SendMgmtMsg(uint8_t *p_header,
                 uint8_t headerLength,
                 uint8_t *p_data,
                 uint8_t dataLength)
{
    uint32_t timeoutPeriod;
    uint32_t startTickCount;

    /* cannot send management messages while in WF_ProcessEvent() */
    WF_ASSERT(!isInWFProcessEvent());

    EnsureWFisAwake();

    /* Allocate a management buffer on the WiFi device.  May have to wait a bit while the previous Data Tx packet is being sent */
    /* which will free up the memory for this mgmt packet.                                                                      */
    timeoutPeriod = TICKS_PER_SECOND / 2;  /* 500 ms */
    startTickCount = (uint32_t)TickGet();
    while (AllocateMgmtTxBuffer(WF_MAX_TX_MGMT_MSG_SIZE) == false)
    {
        if (TickGet() - startTickCount >= timeoutPeriod)
        {
            WF_ASSERT(false);
        }
    }

    /* write out management header */
    RawSetByte(RAW_MGMT_TX_ID, p_header, headerLength);

    /* write out data (if any) */
    if (dataLength > 0)
    {
        RawSetByte(RAW_MGMT_TX_ID, p_data, dataLength);
    }

    /* send mgmt msg to MRF24W */
    SendRAWManagementFrame(headerLength + dataLength);

}

/*******************************************************************************
  Function:
    void SignalMgmtConfirmReceivedEvent(void)

  Summary:
    Signals that a confirmation message has been received.

  Description:
    Called by ProcessMgmtRxMsg() when a mgmt confirm type (WF_MGMT_CONFIRM_TYPE)
    has been received. This function then sets a local flag (gMgmtConfirmMsgReceived) for
    this module indicating the event.

  Precondition:
    MACInit must be called.

  Parameters:
    None

  Returns:
    None

  Remarks:
      *****************************************************************************/
void SignalMgmtConfirmReceivedEvent(void)
{
    gMgmtConfirmMsgReceived = true;
}

/*******************************************************************************
  Function:
    void WaitForMgmtResponse(uint8_t expectedSubtype, uint8_t freeAction)

  Summary:
    Waits for a management response

  Description:
    Called after sending a mgmt request.  This function waits for a mgmt
    response.  The caller can optionally request the the management
    response be freed immediately (by this function) or not freed.  If not
    freed, the caller is responsible to free the response buffer.

  Precondition:
    MACInit must be called.

  Parameters:
    expectedSubtype -- Expected subtype of the mgmt response
    freeAction           -- FREE_MGMT_BUFFER or DO_NOT_FREE_MGMT_BUFFER

  Returns:
    None

  Remarks:
      *****************************************************************************/
void WaitForMgmtResponse(uint8_t expectedSubtype, uint8_t freeAction)
{
    #if defined(__XC8)
        static tMgmtMsgRxHdr  hdr;
    #else
        tMgmtMsgRxHdr  hdr;
    #endif

    g_WaitingForMgmtResponse = true;

    /* Wait until mgmt response is received */
    while (gMgmtConfirmMsgReceived == false)
    {
        WFProcess();

        /* if received a data packet while waiting for mgmt packet */
        if (g_HostRAWDataPacketReceived)
        {
            // We can't let the StackTask processs data messages that come in while waiting for mgmt
            // response because the application might send another mgmt message, which is illegal until the response
            // is received for the first mgmt msg.  And, we can't prevent the race condition where a data message
            // comes in before a mgmt response is received.  Thus, the only solution is to throw away a data message
            // that comes in while waiting for a mgmt response.  This should happen very infrequently.  If using TCP then the
            // stack takes care of retries.  If using UDP, the application has to deal with occasional data messages not being
            // received.  Also, applications typically do not send a lot of management messages after connected.

            // throw away the data rx
            RawMountRxBuffer(RAW_DATA_RX_ID);
            DeallocateDataRxBuffer();
            g_HostRAWDataPacketReceived = false;

            /* ensure interrupts enabled */
            WF_EintEnable();
        }
    }
    g_WaitingForMgmtResponse = false;

    /* set this back to false so the next mgmt send won't think he has a response before one is received */
    gMgmtConfirmMsgReceived = false;

    /* if the caller wants to delete the response immediately (doesn't need any data from it */
    if (freeAction == FREE_MGMT_BUFFER)
    {
        /* read and verify result before freeing up buffer to ensure our message send was successful */
        RawRead(RAW_MGMT_RX_ID, 0, (uint16_t)(sizeof(tMgmtMsgRxHdr)), (uint8_t *)&hdr);

        /* mgmt response subtype had better match subtype we were expecting */
        WF_ASSERT(hdr.subtype == expectedSubtype);

        /* Mgmt response 'result' field should always indicate success.  If this assert is hit the error codes are located */
        /* drv_wifi_api.h.  Search for WF_SUCCESS for the list of error codes.                                                    */
        if (hdr.result == WF_ERROR_HOST_SCAN_NOT_ALLOWED)
        {
            #if defined(STACK_USE_UART)
                putrsUART("Host Scan Failed. Host scan is allowed only in idle or connected state\r\n");
            #endif
        } else if (hdr.result == WF_ERROR_INVALID_WPS_PIN)
        {
            #if defined(STACK_USE_UART)
                putrsUART("WPS failed : Invalid WPS PIN data\r\n");
            #endif
        } else if (hdr.result == WF_ERROR_DISCONNECT_FAILED)
        {
            #if defined(STACK_USE_UART)
                putrsUART("Disconnect failed. Disconnect is allowed only when module is in connected state\r\n");
            #endif
        }
        else if (hdr.result == WF_ERROR_NO_STORED_BSS_DESCRIPTOR)
        {
            #if defined(STACK_USE_UART)
                putrsUART("No stored scan results\r\n");
            #endif
        }
        else if (hdr.result != WF_SUCCESS)
        {
            #if defined(STACK_USE_UART)
                uint8_t buf[8];
                putrsUART("WaitForMgmtResponse result error: ");
                sprintf((char *)buf, "%d",hdr.result);
                putrsUART((char *)buf);
                putrsUART("\r\n");
            #endif
            //WF_ASSERT(false);
        }

        /* free mgmt buffer */
        DeallocateMgmtRxBuffer();
    }

}

/*******************************************************************************
  Function:
    void WaitForMgmtResponseAndReadData(uint8_t expectedSubtype, uint8_t numDataBytes,
                                        uint8_t startIndex, uint8_t *p_data)

  Summary:
    Waits for a management response and read data.

  Description:
    Waits for the mgmt response message and validates it by:
    1) checking the result field
    2) verifying that the received subtype matches the expected subtype
    In addition, this function reads the desired number of data bytes from
    the mgmt response, copies them to p_data, and then frees the mgmt buffer.

  Precondition:
    MACInit must be called.

  Parameters:
    expectedSubtype -- management message subtype that we are expecting
    p_data          -- pointer where any desired management data bytes will be written
    numDataBytes    -- Number of data bytes from mgmt response to write to
                                 p_data.  Data always starts at index 4 of mgmt response.
     skipDataRead    -- if true, then no data will be read and the mgmt buffer will not
                                   be freed.  If false, the data will be read and the mgmt buffer
                                  will be freed.

  Returns:
    None

  Remarks:
      *****************************************************************************/
void WaitForMgmtResponseAndReadData(uint8_t expectedSubtype,
                                    uint8_t numDataBytes,
                                    uint8_t startIndex,
                                    uint8_t *p_data)

{
    tMgmtMsgRxHdr  hdr;  /* management msg header struct */

    WaitForMgmtResponse(expectedSubtype, DO_NOT_FREE_MGMT_BUFFER);

    /* if made it here then received a management message */
    RawRead(RAW_MGMT_RX_ID, 0, (uint16_t)(sizeof(tMgmtMsgRxHdr)), (uint8_t *)&hdr);

    /* check header result and subtype fields */
    WF_ASSERT(hdr.result  == WF_SUCCESS || hdr.result == WF_ERROR_NO_STORED_BSS_DESCRIPTOR);
    WF_ASSERT(hdr.subtype == expectedSubtype);

    /* if caller wants to read data from this mgmt response */
    if (numDataBytes > 0)
    {
        RawRead(RAW_MGMT_RX_ID, startIndex, numDataBytes, p_data);
    }

    /* free the mgmt buffer */
    DeallocateMgmtRxBuffer();
}

#if defined(WF_DEBUG)
/*******************************************************************************
  Function:
    void WFSetFuncState(uint8_t funcMask, uint8_t state)

  Summary:
    For WF_DEBUG only. Sets up the func state.

  Description:
    Called by WF_ProcessEvent() to be able to detect if there is an attempt
    to send a management message while processing the event (not allowed).

  Precondition:
    MACInit must be called.

  Parameters:
    funcMask -- bit mask indicating the calling function
    state    -- WF_ENTERING_FUNCTION or WF_LEAVING_FUNCTION

  Returns:
    None

  Remarks:
      *****************************************************************************/
void WFSetFuncState(uint8_t funcMask, uint8_t state)
{
    if (state == WF_ENTERING_FUNCTION)
    {
        g_FuncFlags |= funcMask;
    }
    else
    {
        g_FuncFlags &= ~funcMask;
    }

}
#endif /* WF_DEBUG */

#endif /* WF_CS_TRIS */
