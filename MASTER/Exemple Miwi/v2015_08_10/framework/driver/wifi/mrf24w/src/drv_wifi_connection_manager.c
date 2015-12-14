/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_wifi_connection_manager.c

  Summary:
    Module for Microchip TCP/IP Stack
    -Provides access to MRF24W WiFi controller (MRF24WB0MA/B, MRF24WG0MA/B)
    -Reference: MRF24W Data sheet, IEEE 802.11 Standard

  Description:
    MRF24W Driver Connection Manager

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

/* used for assertions */
#if defined(WF_DEBUG)
    #define WF_MODULE_NUMBER    WF_MODULE_WF_CONNECTION_MANAGER
#endif

/*******************************************************************************
 *                          LOCAL GLOBAL VARIABLES
 *******************************************************************************/

static bool g_LogicalConnection = false;

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

    Note that if the Connection Profile being used has WPA-PSK/WPA2-PSK security
    and is using a passphrase, the connection manager will first calculate the PSK,
	and then start the connection process. Convert of passphrase to PSK can take up
    to 32 seconds in MRF24WB and 25 seconds in MRF24WG.

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
void WF_CMConnect(uint8_t CpId)
{
    uint8_t  hdrBuf[4];

    /* Write out header portion of msg (which is whole msg, there is no data) */
    hdrBuf[0] = WF_MGMT_REQUEST_TYPE;    /* indicate this is a mgmt msg     */
    hdrBuf[1] = WF_CM_CONNECT_SUBYTPE;   /* mgmt request subtype            */
    hdrBuf[2] = CpId;
    hdrBuf[3] = 0;

    SendMgmtMsg(hdrBuf,
                sizeof(hdrBuf),
                NULL,
                0);

    /* wait for mgmt response, free after it comes in, don't need data bytes */
    WaitForMgmtResponse(WF_CM_CONNECT_SUBYTPE, FREE_MGMT_BUFFER);
}

/*******************************************************************************
  Function:
    static bool WF_CMIsDisconnectAllowed(void)

  Summary:
    Checks whether disconnection is now allowed.

  Description:

  Precondition:
    MACInit must be called first.

  Parameters:
    None.

  Returns:
    true - Currently connected. Disconnection is allowed.
    false - Not connected to any network. So either no need for disconnection or disallowed.

  Remarks:
    None.
  *****************************************************************************/
static bool
WF_CMIsDisconnectAllowed(void)
{
    uint8_t   profileID;
    uint8_t   profileIDState;

    WF_CMCheckConnectionState(&profileIDState, &profileID);
    if (profileIDState == WF_CSTATE_CONNECTED_INFRASTRUCTURE || profileIDState == WF_CSTATE_CONNECTED_ADHOC)
    {
        return true;
    }

    return false;
}

/*******************************************************************************
  Function:
    uint16_t WF_CMDisconnect(void)

  Summary:
    Commands the MRF24W to close any open connections and/or to cease
    attempting to connect.

  Description:
    Directs the Connection Manager to close the open connection.
    No further attempts to connect are taken until WF_CMConnect() is called.

  Precondition:
    MACInit must be called.

  Parameters:
    None.

  Returns:
    Operation results. Success or Failure

  Remarks:
    Disconnection can work only in the connected state.
    To use this API safely, we recommend to disable module FW connection
    manager by setting WF_MODULE_CONNECTION_MANAGER == WF_DISABLED
    in drv_wifi_config.h
  *****************************************************************************/
uint16_t WF_CMDisconnect(void)
{
    uint8_t  hdrBuf[2];

    /* Check if we can call disconnect. Disconnect can work only in the connected state */
    if (!WF_CMIsDisconnectAllowed())
    {
        return WF_ERROR_DISCONNECT_FAILED;
    }

    hdrBuf[0] = WF_MGMT_REQUEST_TYPE;
    hdrBuf[1] = WF_CM_DISCONNECT_SUBYTPE;

    SendMgmtMsg(hdrBuf,
                sizeof(hdrBuf),
                NULL,
                0);

    /* wait for mgmt response, free after it comes in, don't need data bytes */
    WaitForMgmtResponse(WF_CM_DISCONNECT_SUBYTPE, FREE_MGMT_BUFFER);

    /* set state to no connection */
    SetLogicalConnectionState(false);

    return WF_SUCCESS;
}

/*******************************************************************************
  Function:
    void WF_CMGetConnectionState(uint8_t *p_state, uint8_t *p_currentCpId)

  Summary:
    Returns the current connection state.
    Users are encouraged to use 1 profile ID for MRF24W based on v5 stack SW. 
   
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
//extern uint8_t state_SavedBeforeScan, ID_SavedBeforeScan;
void WF_CMGetConnectionState(uint8_t *p_state, uint8_t *p_currentCpId)
{
    uint8_t  hdrBuf[2];
    uint8_t  msgData[2];
#if 0   //defined ( EZ_CONFIG_SCAN )
    if (IS_SCAN_IN_PROGRESS(SCANCXT.scanState))
    {
        *p_state = state_SavedBeforeScan;
        *p_currentCpId = ID_SavedBeforeScan;
        return;
    }
#endif
    hdrBuf[0] = WF_MGMT_REQUEST_TYPE;
    hdrBuf[1] = WF_CM_GET_CONNECTION_STATUS_SUBYTPE;

    SendMgmtMsg(hdrBuf,
                sizeof(hdrBuf),
                NULL,
                0);

    /* wait for mgmt response, read data, free after read */
    WaitForMgmtResponseAndReadData(WF_CM_GET_CONNECTION_STATUS_SUBYTPE,
                                   sizeof(msgData),                  /* num data bytes to read          */
                                   MGMT_RESP_1ST_DATA_BYTE_INDEX,    /* only used if num data bytes > 0 */
                                   msgData);                         /* only used if num data bytes > 0 */

    *p_state       = msgData[0];        /* connection state */
    *p_currentCpId = msgData[1];        /* current CpId     */

    if ((*p_state == WF_CSTATE_CONNECTED_INFRASTRUCTURE) || (*p_state == WF_CSTATE_CONNECTED_ADHOC))
    {
        SetLogicalConnectionState(true);
    }
    else
    {
        SetLogicalConnectionState(false);
    }
}

/*******************************************************************************
  Function:
    bool WFisConnected()

  Summary:
    Query the connection status of the MRF24W.

  Description:
    Determine the fine granularity status of the connection state of the MRF24W.

  Precondition:
    MACInit must be called first.

  Parameters:
    None.

  Returns:
    true if the MRF24W is either connected or attempting to connect.
    false for all other conditions.

  Remarks:
    None.
  *****************************************************************************/
bool WFisConnected()
{
    return g_LogicalConnection;
}

/*******************************************************************************
  Function:
    void SetLogicalConnectionState(bool state)

  Summary:
    Sets the logical connection state.

  Description:
    Logically, if the MRF24W is either connected or trying to connect, then
    it is "connected".  For all other scenarios, the MRF24W is "not
    connected".

  Precondition:
    MACInit must be called first.

  Parameters:
    state - Current logical connection state of the MRF24W.

  Returns:
    None.

  Remarks:
    None.
  *****************************************************************************/
void SetLogicalConnectionState(bool state)
{
    g_LogicalConnection = state;
}

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
void WF_CMCheckConnectionState(uint8_t *p_state, uint8_t *p_currentCpId)
{
    uint8_t  hdrBuf[2];
    uint8_t  msgData[2];

    hdrBuf[0] = WF_MGMT_REQUEST_TYPE;
    hdrBuf[1] = WF_CM_GET_CONNECTION_STATUS_SUBYTPE;

    SendMgmtMsg(hdrBuf,
                sizeof(hdrBuf),
                NULL,
                0);

    /* wait for mgmt response, read data, free after read */
    WaitForMgmtResponseAndReadData(WF_CM_GET_CONNECTION_STATUS_SUBYTPE,
                                   sizeof(msgData),                  /* num data bytes to read          */
                                   MGMT_RESP_1ST_DATA_BYTE_INDEX,    /* only used if num data bytes > 0 */
                                   msgData);                         /* only used if num data bytes > 0 */

    *p_state       = msgData[0];        /* connection state */
    *p_currentCpId = msgData[1];        /* current CpId     */
}

#if defined (MRF24WG)
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
void WF_CMGetConnectContext(tWFConnectContext *p_ctx)
{
    SendGetParamMsg(PARAM_CONNECT_CONTEXT, (uint8_t *)p_ctx, sizeof(*p_ctx));
}
#endif /* MRF24WG */

#if defined(__XC32)
/*******************************************************************************
  Function:
    void WF_ConvPassphraseToKey(uint8_t key_len, uint8_t *key, uint8_t ssid_len, uint8_t *ssid)

  Summary:
    Convert passphrase to key

  Description:
   
  Precondition:
    MACInit must be called first.

  Parameters:
    key_len : key length
    key : passphrase as an input. key as an output
    ssid_len : ssid length
    ssid : ssid

  Returns:
    None.

  Remarks:
    None.
 *****************************************************************************/
void
WF_ConvPassphraseToKey(uint8_t key_len, uint8_t *key, uint8_t ssid_len, uint8_t *ssid)
{
    uint8_t psk[32];

    key[key_len] = '\0';
    pbkdf2_sha1((const char *)key, (const char *)ssid, ssid_len, 4096, (uint8_t *)psk, 32);
    memcpy(key, psk, 32);
}
#endif /* __XC32 */

#endif /* WF_CS_TRIS */
