/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_wifi_raw_24g.c

  Summary:
    Module for Microchip TCP/IP Stack
    -Provides access to MRF24W WiFi controller
    -Reference: MRF24W Data sheet, IEEE 802.11 Standard

  Description:
    MRF24W Driver RAW Driver

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
*                               INCLUDES
********************************************************************************/
#include "tcpip/tcpip.h"

#if defined(WF_CS_TRIS)

#include "drv_wifi_mac.h"

/* used for assertions */
#if defined(WF_DEBUG)
    #define WF_MODULE_NUMBER   WF_MODULE_WF_DRIVER_RAW
#endif

/*******************************************************************************
*                               DEFINES
********************************************************************************/

// RAW register masks
#define WF_RAW_STATUS_REG_ERROR_MASK    ((uint16_t)(0x0002))
#define WF_RAW_STATUS_REG_BUSY_MASK     ((uint16_t)(0x0001))

#define NUM_RAW_WINDOWS                 (6) /* only using raw windows 0 thru 4 */

/*******************************************************************************
*                               LOCAL DATA TYPES
********************************************************************************/

/*******************************************************************************
*                              LOCAL GLOBAL VARIABLES
********************************************************************************/

bool g_HostRAWDataPacketReceived = false;  // set true by state machine in drv_wifi_com.c
bool gIntDisabled = false;

/* keeps track of whether raw tx/rx data windows mounted or not */
uint8_t  RawWindowState[2];

/* raw registers for each raw window being used */
static ROM uint8_t  g_RawIndexReg[NUM_RAW_WINDOWS]  = {RAW_0_INDEX_REG,  RAW_1_INDEX_REG,  RAW_2_INDEX_REG,  RAW_3_INDEX_REG,  RAW_4_INDEX_REG, RAW_5_INDEX_REG};
static ROM uint8_t  g_RawStatusReg[NUM_RAW_WINDOWS] = {RAW_0_STATUS_REG, RAW_1_STATUS_REG, RAW_2_STATUS_REG, RAW_3_STATUS_REG, RAW_4_STATUS_REG, RAW_5_STATUS_REG};
static ROM uint16_t g_RawCtrl0Reg[NUM_RAW_WINDOWS]  = {RAW_0_CTRL_0_REG, RAW_1_CTRL_0_REG, RAW_2_CTRL_0_REG, RAW_3_CTRL_0_REG, RAW_4_CTRL_0_REG, RAW_5_CTRL_0_REG};
static ROM uint16_t g_RawCtrl1Reg[NUM_RAW_WINDOWS]  = {RAW_0_CTRL_1_REG, RAW_1_CTRL_1_REG, RAW_2_CTRL_1_REG, RAW_3_CTRL_1_REG, RAW_4_CTRL_1_REG, RAW_5_CTRL_1_REG};
static ROM uint16_t g_RawDataReg[NUM_RAW_WINDOWS]   = {RAW_0_DATA_REG,   RAW_1_DATA_REG,   RAW_2_DATA_REG,   RAW_3_DATA_REG,   RAW_4_DATA_REG, RAW_5_DATA_REG};

/* interrupt mask for each raw window; note that raw0 and raw1 are really 8 bit values and will be cast when used  */
static ROM uint16_t g_RawIntMask[NUM_RAW_WINDOWS]   = {WF_HOST_INT_MASK_RAW_0_INT_0,   /* used in HOST_INTR reg (8-bit register)   */
                                                     WF_HOST_INT_MASK_RAW_1_INT_0,   /* used in HOST_INTR reg (8-bit register)   */
                                                     WF_HOST_INT_MASK_RAW_2_INT_0,   /* used in HOST_INTR2 reg (16-bit register) */
                                                     WF_HOST_INT_MASK_RAW_3_INT_0,   /* used in HOST_INTR2 reg (16-bit register) */
                                                     WF_HOST_INT_MASK_RAW_4_INT_0,   /* used in HOST_INTR2 reg (16-bit register) */
                                                     WF_HOST_INT_MASK_RAW_5_INT_0};  /* used in HOST_INTR2 reg (16-bit register) */

/* bit mask where each bit corresponds to a raw window id.  If set, the raw index has been set past */
/* the end of the raw window.                                                                       */
ROM uint8_t g_RawAccessOutOfBoundsMask[NUM_RAW_WINDOWS] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20};
uint8_t g_RawIndexPastEnd = 0x00;  /* no indexes are past end of window */

/*******************************************************************************
*                                LOCAL FUNCTION PROTOTYPES
********************************************************************************/
static uint16_t RawMove(uint16_t rawId, uint16_t srcDest, bool rawIsDestination, uint16_t size);
static uint16_t WaitForRawMoveComplete(uint8_t rawId);

#define ENC_RD_PTR_ID (0)  /* same as define in drv_wifi_mac.c */
extern uint8_t GetReadPtrRawWindow(void);

uint16_t RawCalculateChecksum(uint16_t length)
{
    uint16_t checksum;
    uint8_t rawId;

    rawId = GetReadPtrRawWindow(); /* get raw window id for window being pointed to by read pointer */

    checksum = RawMove(rawId, RAW_COPY, false, length); /* this raw move instructs the firmware to do a checksum   */
                                                        /* for 'length' bytes starting at current raw window index */

    return checksum;

}

/*****************************************************************************
  Function:
    bool AllocateMgmtTxBuffer(uint16_t bytesNeeded)

  Summary:
    Allocates a Mgmt Tx buffer

  Description:
    Determines if WiFi chip has enough memory to allocate a tx mgmt buffer, and,
    if so, allocates it.

  Precondition:
    None

  Parameters:
    bytesNeeded -- number of bytes needed for the mgmt tx message

  Returns:
    True if mgmt tx buffer successfully allocated, else False

  Remarks:
    None
*****************************************************************************/
bool AllocateMgmtTxBuffer(uint16_t bytesNeeded)
{
    uint16_t bufAvail;
    uint16_t byteCount;
    //char st[80];

    /* get total bytes available for MGMT tx memory pool */
    bufAvail = Read16BitWFRegister(WF_HOST_WFIFO_BCNT1_REG) & 0x0fff; /* LS 12 bits contain length */

    /* if enough bytes available to allocate */
    if ( bufAvail >= bytesNeeded )
    {
        /* allocate and create the new Mgmt Tx buffer */
        byteCount = RawMove(RAW_MGMT_TX_ID, RAW_MGMT_POOL, true, bytesNeeded);
        if (byteCount == 0)     // something wrong - we failed to complete the move
        {
            //putrsUART("AllocateMgmtTxBuffer: something wrong -- RawMove failed to complete mgmt Tx. deallocate RAW_MGMT_TX_ID, RAW_MGMT_POOL\r\n");
            return false;
        }
        else
            return true;
    }
    /* else not enough bytes available at this time to satisfy request */
    else
    {
        /* if we allocated some bytes, but not enough, then dealloacate what was allocated */
        //if (bufAvail > 0)
        //{
        //    RawMove(RAW_MGMT_RX_ID, RAW_MGMT_POOL, false, 0);
        //}

        //sprintf(st,"AllocateMgmtTxBuffer: failed to allocate memory, bufAvail=%d, bytesNeeded=%d\r\n",bufAvail,bytesNeeded);
        //putrsUART(st);
        return false;
    }
}

/*****************************************************************************
  Function:
    void DeallocateMgmtRxBuffer(void)

  Summary:
    Deallocates a mgmt Rx buffer

  Description:
    Called by WiFi driver when its finished processing a Rx mgmt message.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    None
*****************************************************************************/
void DeallocateMgmtRxBuffer(void)
{
     /* Unmount (release) mgmt packet now that we are done with it */
    RawMove(RAW_MGMT_RX_ID, RAW_MGMT_POOL, false, 0);
}

/*****************************************************************************
  Function:
    bool AllocateDataTxBuffer(uint16_t bytesNeeded)

  Summary:
    Allocates a Data Tx buffer for use by the TCP/IP stack.

  Description:
    Determines if WiFi chip has enough memory to allocate a tx data buffer, and,
    if so, allocates it.

  Precondition:
    None

  Parameters:
    bytesNeeded -- number of bytes needed for the data tx message

  Returns:
    True if data tx buffer successfully allocated, else False

  Remarks:
    None
*****************************************************************************/
bool AllocateDataTxBuffer(uint16_t bytesNeeded)
{
    uint16_t bufAvail;
    uint16_t byteCount;
    //char st[80];

    WF_ASSERT(GetRawWindowState(RAW_DATA_TX_ID) != WF_RAW_DATA_MOUNTED);

    /* Ensure the MRF24W is awake (only applies if PS-Poll was enabled) */
    EnsureWFisAwake();

    /* get total bytes available for DATA tx memory pool */
    bufAvail = Read16BitWFRegister(WF_HOST_WFIFO_BCNT0_REG) & 0x0fff; /* LS 12 bits contain length */

    /* if enough bytes available to allocate */
    if ( bufAvail >= bytesNeeded )
    {
        /* allocate and create the new Tx buffer (mgmt or data) */
        byteCount = RawMove(RAW_DATA_TX_ID, RAW_DATA_POOL, true, bytesNeeded);
        if (byteCount == 0)     // something wrong - we can't complete data Tx
        {
            //putrsUART("AllocateDataTxBuffer: something wrong -- RawMove failed to complete data Tx. Deallocate RAW_DATA_TX_ID, RAW_DATA_POOL\r\n");
            return false;       // just return and let host retry again.
        }
        else
        {
            /* flag this raw window as mounted (in use) */
            SetRawWindowState(RAW_DATA_TX_ID, WF_RAW_DATA_MOUNTED);
            return true;
        }

    }
    /* else not enough bytes available at this time to satisfy request */
    else
    {
        //sprintf(st,"AllocateDataTxBuffer: failed to allocate memory, bufAvail=%d, bytesNeeded=%d\r\n",bufAvail,bytesNeeded);
        //putrsUART(st);
        return false;
    }

}

/*****************************************************************************
  Function:
    void SendRAWDataFrame(uint16_t bufLen)

  Summary:
    Sends a data frame to the WiFi chip for transmission to the 802.11 network.

  Description:
    After the TCP/IP stack has created a data frame and called MACFlush() this
    function is called to notify the WiFi chip to transmit the data frame to the
    802.11 network.  The actual data frame starts at index 4 in the allocated buffer.
    The first 4 bytes (indexes 0 - 3) were reserved, and this function fills them
    in with an internal preamble which notifies the WiFi chip that this is a data
    frame to send (as opposed to a management frame).

  Precondition:
    None

  Parameters:
    bufLen -- number of data bytes that comprise the data frame.

  Returns:
    None

  Remarks:
    None
*****************************************************************************/
void SendRAWDataFrame(uint16_t bufLen)
{
    /* create internal preamble */
    uint8_t txDataPreamble[4] = { WF_DATA_REQUEST_TYPE, WF_STD_DATA_MSG_SUBTYPE, 1, 0};

    /* write out internal preamble, starting at index 0 in the raw window */
    RawWrite(RAW_DATA_TX_ID, 0, sizeof(txDataPreamble), txDataPreamble);

    /* Notify WiFi device that there is a transmit frame to send .  The frame will */
    /* be automatically deallocated after RF transmission is completed.            */
    RawMove(RAW_DATA_TX_ID, RAW_MAC, false, bufLen);

    /* this raw window is logically no longer mounted.  The WiFi chip will  */
    /* automatically deallocate after RF transmission.                      */
    SetRawWindowState(RAW_DATA_TX_ID, WF_RAW_UNMOUNTED);
}

/*****************************************************************************
  Function:
    void SendRAWManagementFrame(uint16_t bufLen)

  Summary:
    Sends a management frame to the WiFi chip.

  Description:
    The management header, which consists of a type and subtype, have already
    been written to the frame before this function is called.

  Precondition:
    None

  Parameters:
    bufLen -- number of bytes that comprise the management frame.

  Returns:
    None

  Remarks:
    None
*****************************************************************************/
void SendRAWManagementFrame(uint16_t bufLen)
{
    /* Notify WiFi device that management message is ready to be processed */
    RawMove(RAW_MGMT_TX_ID, RAW_MAC, false, bufLen);
}

/*****************************************************************************
  Function:
    void DeallocateDataRxBuffer(void)

  Summary:
    Deallocates a Data Rx buffer

  Description:
    Typically called by MACGetHeader(), the assumption being that when the stack
    is checking for a newly received data message it is finished with the previously
    received data message.  Also called by MACGetHeader() if the SNAP header is invalid
    and the packet is thrown away.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    None
*****************************************************************************/
void DeallocateDataRxBuffer(void)
{
    WF_ASSERT(GetRawWindowState(RAW_DATA_RX_ID) == WF_RAW_DATA_MOUNTED);

    /* perform deallocation of raw rx buffer */
    RawMove(RAW_DATA_RX_ID, RAW_DATA_POOL, false, 0);

    /* set state flag */
    SetRawWindowState(RAW_DATA_RX_ID, WF_RAW_UNMOUNTED);

    /* the current length of the Rx data packet is now 0 (used by MAC) */
    SetRxDataPacketLength(0);
}

#if 0
void DeallocateDataTxBuffer(void)
{
    RawMove(RAW_DATA_TX_ID, RAW_DATA_POOL, false, 0);
    SetRawWindowState(RAW_DATA_TX_ID, WF_RAW_UNMOUNTED);
}
#endif

#if 0
/* transmits raw Tx data frame to 802.11 network */
void RawSendTxBuffer(uint16_t len)
{
    RawMove(RAW_DATA_TX_ID, RAW_MAC, false, len);
    SetRawWindowState(RAW_DATA_TX_ID, WF_RAW_UNMOUNTED);
}
#endif

/*****************************************************************************
  Function:
    uint16_t RawMountRxBuffer(uint8_t rawId)

  Summary:
    Mounts most recent Rx message.

  Description:
    This function mounts the most recent Rx message from the WiFi chip, which
    could be either a management or a data message.

  Precondition:
    None

  Parameters:
    rawId -- RAW ID specifying which raw window to mount the rx packet in.

  Returns:
    length -- number of bytes in the received message.

  Remarks:
    None
*****************************************************************************/
uint16_t RawMountRxBuffer(uint8_t rawId)
{
    uint16_t length;

    length = RawMove(rawId, RAW_MAC, true, 0);
    WF_ASSERT(length > 0);

    /* if mounting a Raw Rx data frame */
    if (rawId == RAW_DATA_RX_ID)
    {
        /* notify WiFi driver that an Rx data frame is mounted */
        SetRawWindowState(RAW_DATA_RX_ID, WF_RAW_DATA_MOUNTED);
    }

    return length;
}

/*****************************************************************************
  Function:
    void RawToRawCopy(uint8_t rawDestId, uint8_t rawSourceId, uint16_t length)

  Summary:
    Performs a data copy from one raw window to another.

  Description:
    There is an additional step needed before invoking RawMove(); need to
    write the source and destination values to ScratchPad0 register so that
    the information is there before the WiFi chip starts the copy operation.

  Precondition:
    None

  Parameters:
    rawDestId   -- RAW ID that is the destination of copy
    rawSourceId -- RAW ID that is the source of copy
    length      -- number of bytes to copy from source to destination

  Returns:
    None

  Remarks:
    None
*****************************************************************************/
void RawToRawCopy(uint8_t rawDestId, uint8_t rawSourceId, uint16_t length)
{
    /* write source and destination address to scratchpad 0 register. The WiFi firmware */
    /* will get these values when it starts processing the raw move operation and       */
    /* determines a raw copy operation is taking place.                                 */
    Write16BitWFRegister(WF_INDEX_ADDR_REG, WF_SCRATCHPAD_0_REG);
    Write16BitWFRegister(WF_INDEX_DATA_REG, (((uint16_t)rawSourceId << 8) | (uint16_t)rawDestId));

    /* always perform the actual move on RAW 0, the firmware will check the scratchpad register */
    /* for the actual source and destination                                                    */
    RawMove(RAW_ID_0, RAW_COPY, true, length);
}

/*****************************************************************************
  Function:
    void PushRawWindow(uint8_t rawId)

  Summary:
    Pushes a Raw window

  Description:
    Each RAW window can have its context saved in a 'stack' that is one level
    deep.  Each time this function is called any raw state that had been saved
    is destroyed.

  Precondition:
    None

  Parameters:
    rawId -- RAW window that is being pushed

  Returns:
    None

  Remarks:
    None
*****************************************************************************/
void PushRawWindow(uint8_t rawId)
{
    RawMove(rawId, RAW_STACK_MEM, false, 0);
}

/*****************************************************************************
  Function:
    uint16_t PopRawWindow(uint8_t rawId)

  Summary:
    Pops a Raw window

  Description:
    Each RAW window can have its context saved in a 'stack' that is one level
    deep.  When this function is called the saved context is restored and reads
    and writes to the window are operational.  The currently mounted window state
    is lost.

  Precondition:
    None

  Parameters:
    rawId -- RAW window that is being popped

  Returns:
    Byte count of the RAW window buffer being restored.  If no context has
    been saved this function returns a 0.

  Remarks:
    None
*****************************************************************************/
uint16_t PopRawWindow(uint8_t rawId)
{
    uint16_t byteCount;

    byteCount = RawMove(rawId, RAW_STACK_MEM, true, 0);

    return byteCount;
}

/*****************************************************************************
  Function:
    uint16_t ScratchMount(uint8_t rawId)

  Summary:
    Mounts RAW scratch window

  Description:
    The scratch window is not dynamically allocated, but references a static
    portion of the WiFi device RAM. Thus, the Scratch data is not lost when
    the scratch window is unmounted.

  Precondition:
    None

  Parameters:
    rawId -- RAW window ID being used to mount the scratch data

  Returns:
    Size, in bytes, of Scratch buffer

  Remarks:
    None
*****************************************************************************/
uint16_t ScratchMount(uint8_t rawId)
{
    uint16_t byteCount;

    byteCount = RawMove(rawId, RAW_SCRATCH_POOL, true, 0);
    WF_ASSERT(byteCount > 0);  /* scratch mount should always return value > 0 */

    return byteCount;
}

/*****************************************************************************
  Function:
    void ScratchUnmount(uint8_t rawId)

  Summary:
    Unmounts RAW scratch window

  Description:
    The scratch window is not dynamically allocated, but references a static
    portion of the WiFi device RAM. Thus, the Scratch data is not lost when
    the scratch window is unmounted.

  Precondition:
    None

  Parameters:
    rawId -- RAW window ID that was used to mount the scratch window

  Returns:
    Size, in bytes, of Scratch buffer

  Remarks:
    None
*****************************************************************************/
void ScratchUnmount(uint8_t rawId)
{
    RawMove(rawId, RAW_SCRATCH_POOL, false, 0);
}

/*
*********************************************************************************************************
*                                   RawRead()
*
* Description : Reads the specified number of bytes from a mounted RAW window from the specified starting
*               index;
*
* Argument(s) : rawId      -- RAW window ID being read from
*               startIndex -- start index within RAW window to read from
*               length     -- number of bytes to read from the RAW window
*               p_dest     -- pointer to Host buffer where read data is copied
*
* Return(s)   : error code
*
* Caller(s)   : WF Driver
*
* Notes:      : None
*
*********************************************************************************************************
*/
void RawRead(uint8_t rawId, uint16_t startIndex, uint16_t length, uint8_t *p_dest)
{
    RawSetIndex(rawId, startIndex);
    RawGetByte(rawId, p_dest, length);
}

/*
*********************************************************************************************************
*                                   RawWrite()
*
* Description : Writes the specified number of bytes to a mounted RAW window at the specified starting
*               index
*
* Argument(s) : rawId      -- RAW window ID being written to
*               startIndex -- start index within RAW window to write to
*               length     -- number of bytes to write to RAW window
*               p_src      -- pointer to Host buffer write data
*
* Return(s)   : None
*
* Caller(s)   : WF Driver
*
* Notes:      : None
*
*********************************************************************************************************
*/
void RawWrite(uint8_t rawId, uint16_t startIndex, uint16_t length, uint8_t *p_src)
{
    /*set raw index in dest memory */
    RawSetIndex(rawId, startIndex);

    /* write data to RAW window */
    RawSetByte(rawId, p_src, length);
}

/*****************************************************************************
  Function: uint16_t RawMove(uint16_t rawId,
                           uint16_t srcDest,
                           bool rawIsDestination,
                           uint16_t size)

  Summary:
    Performs RAW Move operation

  Description:
    Raw Moves perform a variety of operations (e.g. allocating tx buffers,
    mounting rx buffers, copying from one raw window to another, etc.)

  Precondition:
    None

  Parameters:
    rawId -- Raw ID 0 thru 5, except is srcDest is RAW_COPY, in which case rawId
             contains the source address in the upper 4 bits and destination
             address in lower 4 bits.

    srcDest -- object that will either be the source or destination of the move:
                  RAW_MAC
                  RAW_MGMT_POOL
                  RAW_DATA_POOL
                  RAW_SCRATCH_POOL
                  RAW_STACK_MEM
                  RAW_COPY (this object not allowed, handled in RawToRawCopy() )

    rawIsDestination -- true is srcDest is the destination, false if srcDest is
                        the source of the move

    size -- number of bytes to overlay (not always applicable)

  Returns:
     Not always applicable, depending on the type of the raw move.  When applicable,
     this function returns the number of bytes overlayed by the raw move.

  Remarks:
    None
*****************************************************************************/
static uint16_t RawMove(uint16_t    rawId,
                       uint16_t   srcDest,
                       bool     rawIsDestination,
                       uint16_t   size)
{
    uint16_t byteCount;
    uint8_t  regId;
    uint8_t  regValue;
    uint16_t ctrlVal = 0;

    /* create control value that will be written to raw control register, which initiates the raw move */
    if (rawIsDestination)
    {
        ctrlVal |= 0x8000;
    }
    /* fix later, simply need to ensure that size is 12 bits are less */
    ctrlVal |= (srcDest << 8);              /* defines are already shifted by 4 bits */
    ctrlVal |= ((size >> 8) & 0x0f) << 8;   /* MS 4 bits of size (bits 11:8)         */
    ctrlVal |= (size & 0x00ff);             /* LS 8 bits of size (bits 7:0)          */

    /*---------------------------------------------------------------------------------------*/
    /* this next 'if' block is used to ensure the expected raw interrupt signifying raw move */
    /* complete is cleared                                                                   */
    /*---------------------------------------------------------------------------------------*/

    /* if doing a raw move on Raw 0 or 1 (data rx or data tx) */
    if (rawId <= RAW_ID_1)
    {
        /* Clear the interrupt bit in the host interrupt register (Raw 0 and 1 are in 8-bit host intr reg) */
        regValue = (uint8_t)g_RawIntMask[rawId];
        Write8BitWFRegister(WF_HOST_INTR_REG, regValue);
    }
    /* else doing raw move on mgmt rx, mgmt tx, or scratch */
    else
    {
        /* Clear the interrupt bit in the host interrupt 2 register (Raw 2,3, and 4 are in 16-bit host intr2 reg */
        regValue = g_RawIntMask[rawId];
        Write16BitWFRegister(WF_HOST_INTR2_REG, regValue);
    }

    /*------------------------------------------------------------------------------------------------*/
    /* now that the expected raw move complete interrupt has been cleared and we are ready to receive */
    /* it, initiate the raw move operation by writing to the appropriate RawCtrl0.                    */
    /*------------------------------------------------------------------------------------------------*/
    regId = g_RawCtrl0Reg[rawId];                   /* get RawCtrl0 register address for desired raw ID */
    gIntDisabled = WF_EintIsDisabled();
    WF_EintDisable();
    Write16BitWFRegister(regId, ctrlVal);           /* write ctrl value to register                     */
    byteCount = WaitForRawMoveComplete(rawId);      /* wait for raw move to complete                    */

    /* byte count is not valid for all raw move operations */
    return byteCount;
}

/*****************************************************************************
  Function:
    bool RawSetIndex(uint16_t rawId, uint16_t index)

  Summary:
    Sets the index within the specified RAW window.

  Description:
    Sets the index within the specified RAW window.    If attempt to set RAW index
    outside boundaries of RAW window (past the end) this function will time out.
    It's legal to set the index past the end of the raw window so long as there
    is no attempt to read or write at that index.

  Precondition:
    None

  Parameters:
    rawId -- RAW window ID
    index -- desired index within RAW window

  Returns:
     True if successful, False if caller tried to set raw index past end of
     window

  Remarks:
    None
*****************************************************************************/
bool RawSetIndex(uint16_t rawId, uint16_t index)
{
    uint8_t regId;
    uint16_t regValue;
    uint32_t startTickCount;
    uint32_t maxAllowedTicks;

    /* get the index register associated with the raw ID and write to it */
    regId = g_RawIndexReg[rawId];
    Write16BitWFRegister(regId, index);

    /* Get the raw status register address associated with the raw ID.  This will be polled to         */
    /* determine that:                                                                                 */
    /*  1) raw set index completed successfully  OR                                                    */
    /*  2) raw set index failed, implying that the raw index was set past the end of the raw window    */
    regId = g_RawStatusReg[rawId];

    maxAllowedTicks = TICKS_PER_SECOND / 200;   /* 5ms */
    startTickCount = (uint32_t)TickGet();

    /* read the status register until set index operation completes or times out */
    while (1)
    {
        regValue = Read16BitWFRegister(regId);
        if ((regValue & WF_RAW_STATUS_REG_BUSY_MASK) == 0)
        {
            ClearIndexOutOfBoundsFlag(rawId);
            return true;
        }

        /* if timed out then trying to set index past end of raw window, which is OK so long as the app */
        /* doesn't try to access it                                                                     */
        if (TickGet() - startTickCount >= maxAllowedTicks)
        {
            SetIndexOutOfBoundsFlag(rawId);
            return false;  /* timed out waiting for Raw set index to complete */
        }
    }
}

/*****************************************************************************
 * FUNCTION: RawGetIndex
 *
 * RETURNS: Returns the current RAW index for the specified RAW engine.
 *
 * PARAMS:
 *      rawId - RAW ID
 *
 *  NOTES: None
 *****************************************************************************/
uint16_t RawGetIndex(uint16_t rawId)
{
    uint8_t  regId;
    uint16_t index;

    regId = g_RawIndexReg[rawId];

    index = Read16BitWFRegister(regId);

    return index;
}

/*****************************************************************************
 * FUNCTION: RawGetByte
 *
 * RETURNS: error code
 *
 * PARAMS:
 *      rawId   - RAW ID
 *      pBuffer - Buffer to read bytes into
 *      length  - number of bytes to read
 *
 *  NOTES: Reads bytes from the RAW engine
 *****************************************************************************/
void RawGetByte(uint16_t rawId, uint8_t *pBuffer, uint16_t length)
{
    uint8_t regId;

    //WF_ASSERT(!isIndexOutOfBounds(rawId)); /* attempting to read past end of RAW buffer */
    if (isIndexOutOfBounds(rawId))
    {
        //putrsUART("====================> RawGetByte: IndexOutOfBounds\r\n");
        return;
    }
    regId = g_RawDataReg[rawId];
    ReadWFArray(regId, pBuffer, length);
}

/*****************************************************************************
 * FUNCTION: RawSetByte
 *
 * RETURNS: None
 *
 * PARAMS:
 *      rawId   - RAW ID
 *      pBuffer - Buffer containing bytes to write
 *      length  - number of bytes to read
 *
 *  NOTES: Writes bytes to RAW window
 *****************************************************************************/
void RawSetByte(uint16_t rawId, uint8_t *pBuffer, uint16_t length)
{
    uint8_t regId;

    //WF_ASSERT(!isIndexOutOfBounds(rawId)); /* attempting to write past end of RAW buffer */

    if (isIndexOutOfBounds(rawId))
    {
        //putrsUART("====================> RawSetByte: IndexOutOfBounds\r\n");
        return;
    }
    /* write data to raw window */
    regId = g_RawDataReg[rawId];
    WriteWFArray(regId, pBuffer, length);

}

#if defined (__XC8)
/*****************************************************************************
 * FUNCTION: RawSetByteROM
 *
 * RETURNS: True if successful, else false
 *
 * PARAMS:
 *      rawId   - RAW ID
 *      pBuffer - Buffer containing bytes to write
 *      length  - number of bytes to read
 *
 *  NOTES: Reads bytes from the RAW engine.  Same as RawSetByte except
 *         using a ROM pointer instead of RAM pointer
 *****************************************************************************/
void RawSetByteROM(uint16_t rawId, ROM uint8_t *pBuffer, uint16_t length)
{
    uint8_t regId;

    regId = g_RawDataReg[rawId];
    WriteWFROMArray(regId, pBuffer, length);
}
#endif

/*****************************************************************************
 * FUNCTION: WaitForRawMoveComplete
 *
 * RETURNS: Number of bytes that were overlayed (not always applicable)
 *
 * PARAMS:
 *   rawId - RAW ID
 *
 * NOTES: Waits for a RAW move to complete.
 *****************************************************************************/
static uint16_t WaitForRawMoveComplete(uint8_t rawId)
{
    uint8_t  rawIntMask;
    uint16_t byteCount;
    uint8_t  regId;
    #if defined(WF_DEBUG)
    uint32_t startTickCount;
    uint32_t maxAllowedTicks;
    #endif

    /* create mask to check against for Raw Move complete interrupt for either RAW0 or RAW1 */
    if (rawId <= RAW_ID_1)
    {
        /* will be either raw 0 or raw 1 */
        rawIntMask = (rawId == RAW_ID_0) ? WF_HOST_INT_MASK_RAW_0_INT_0 : WF_HOST_INT_MASK_RAW_1_INT_0;
    }
    else
    {
        /* will be INTR2 bit in host register, signifying RAW2, RAW3, or RAW4 */
        rawIntMask = WF_HOST_INT_MASK_INT2;
    }

    /*
    These variables are shared with the ISR so need to be careful when setting them.
    the WFEintHandler() is the ISR that will touch these variables but will only touch
    them if RawMoveState.waitingForRawMoveCompleteInterrupt is set to true.
    RawMoveState.waitingForRawMoveCompleteInterrupt is only set true here and only here.
    so as long as we set RawMoveState.rawInterrupt first and then set RawMoveState.waitingForRawMoveCompleteInterrupt
    to true, we are guranteed that the ISR won't touch RawMoveState.rawInterrupt and
    RawMoveState.waitingForRawMoveCompleteInterrupt.
    */
    RawMoveState.rawInterrupt  = 0;
    RawMoveState.waitingForRawMoveCompleteInterrupt = true;
    WF_EintEnable();

    #if defined(WF_DEBUG)
    // Before we enter the while loop, get the tick timer count and save it
    maxAllowedTicks = TICKS_PER_SECOND / 2; /* 500 ms timeout */
    startTickCount = (uint32_t)TickGet();
    #endif
    while (1)
    {
        /* if received an external interrupt that signaled the RAW Move */
        /* completed then break out of this loop                        */
        if (RawMoveState.rawInterrupt & rawIntMask)
        {
            break;
        }

        #if defined(WF_DEBUG)
        /* If timed out waiting for RAW Move complete then lock up */
        if (TickGet() - startTickCount >= maxAllowedTicks)
        {
            WF_ASSERT(false);
        }
        #endif
    } /* end while */

    /* if interrupt was enabled by us here, we should disable it now that we're finished */
    if (gIntDisabled)
    {
        WF_EintDisable();
    }

    /* read the byte count and return it */
    regId = g_RawCtrl1Reg[rawId];
    byteCount = Read16BitWFRegister(regId);

    return ( byteCount );
}
#endif /* WF_CS_TRIS */

/* EOF */
