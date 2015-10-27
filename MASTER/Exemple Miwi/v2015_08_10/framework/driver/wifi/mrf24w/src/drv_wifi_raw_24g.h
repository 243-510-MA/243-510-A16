/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_wifi_raw_24g.h

  Summary:
    Module for Microchip TCP/IP Stack
    -Provides access to MRF24W WiFi controller
    -Reference: MRF24W Data sheet, IEEE 802.11 Standard

  Description:
    MRF24W Driver Raw driver

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
*                                       DEFINES
********************************************************************************/

/* Supported RAW Windows */
#define RAW_ID_0                        (0)
#define RAW_ID_1                        (1)
#define RAW_ID_2                        (2)
#define RAW_ID_3                        (3)
#define RAW_ID_4                        (4)
#define RAW_ID_5                        (5)

/* Usage of RAW Windows */
#define RAW_DATA_RX_ID                  RAW_ID_0
#define RAW_DATA_TX_ID                  RAW_ID_1
#define RAW_MGMT_RX_ID                  RAW_ID_2
#define RAW_MGMT_TX_ID                  RAW_ID_3
#define RAW_SCRATCH_ID                  RAW_ID_4
#define RAW_UNUSED_ID                   RAW_ID_5

// Source/Destination objects on the MRF24W
#define RAW_MAC                         (0x00)   /* Cmd processor (aka MRF24W MAC)              */
#define RAW_MGMT_POOL                   (0x10)   /* For 802.11 Management packets                  */
#define RAW_DATA_POOL                   (0x20)   /* Data Memory pool used for tx and rx operations */
#define RAW_SCRATCH_POOL                (0x30)   /* Scratch object                                 */
#define RAW_STACK_MEM                   (0x40)   /* single level stack to save state of RAW        */
#define RAW_COPY                        (0x70)   /* RAW to RAW copy                                */

/*---------------------*/
/* 8-bit RAW registers */
/*---------------------*/
#define RAW_0_DATA_REG                  (0x20)   /* Data Rx       */
#define RAW_1_DATA_REG                  (0x21)   /* Data Tx       */
#define RAW_2_DATA_REG                  (0x06)   /* Mgmt Rx       */
#define RAW_3_DATA_REG                  (0x07)   /* Mgmt Tx       */
#define RAW_4_DATA_REG                  (0x08)   /* Scratch Tx/Rx */
#define RAW_5_DATA_REG                  (0x09)   /* not used      */

/*----------------------*/
/* 16 bit RAW registers */
/*----------------------*/
#define RAW_0_CTRL_0_REG                (0x25)      /* RAW 0 -- Data Rx       */
#define RAW_0_CTRL_1_REG                (0x26)
#define RAW_0_INDEX_REG                 (0x27)
#define RAW_0_STATUS_REG                (0x28)

#define RAW_1_CTRL_0_REG                (0x29)      /* RAW 1 -- Data Tx       */
#define RAW_1_CTRL_1_REG                (0x2a)
#define RAW_1_INDEX_REG                 (0x2b)
#define RAW_1_STATUS_REG                (0x2c)

#define RAW_2_CTRL_0_REG                (0x18)      /* RAW 2 -- Mgmt Rx       */
#define RAW_2_CTRL_1_REG                (0x19)
#define RAW_2_INDEX_REG                 (0x1a)
#define RAW_2_STATUS_REG                (0x1b)

#define RAW_3_CTRL_0_REG                (0x1c)      /* RAW 3 -- Mgmt Tx       */
#define RAW_3_CTRL_1_REG                (0x1d)
#define RAW_3_INDEX_REG                 (0x1e)
#define RAW_3_STATUS_REG                (0x1f)

#define RAW_4_CTRL_0_REG                (0x0a)      /* RAW 4 -- Scratch Tx/Rx */
#define RAW_4_CTRL_1_REG                (0x0b)
#define RAW_4_INDEX_REG                 (0x0c)
#define RAW_4_STATUS_REG                (0x0d)

#define RAW_5_CTRL_0_REG                (0x0e)      /* RAW 5 -- Not used       */
#define RAW_5_CTRL_1_REG                (0x0f)
#define RAW_5_INDEX_REG                 (0x22)
#define RAW_5_STATUS_REG                (0x23)

/* RAW Window states */
#define WF_RAW_UNMOUNTED            (0)
#define WF_SCRATCH_MOUNTED          (1)
#define WF_RAW_DATA_MOUNTED         (2)
#define WF_RAW_MGMT_MOUNTED         (3)

/* sets and gets the state of RAW data tx/rx windows */
#define SetRawWindowState(rawId, state)    RawWindowState[rawId] = state
#define GetRawWindowState(rawId)           RawWindowState[rawId]

/* these macros set a flag bit if the raw index is set past the end of the raw window, or clear the */
/* flag bit if the raw index is set within the raw window.                                              */
#define SetIndexOutOfBoundsFlag(rawId)      g_RawIndexPastEnd |= g_RawAccessOutOfBoundsMask[rawId]
#define ClearIndexOutOfBoundsFlag(rawId)    g_RawIndexPastEnd &= ~g_RawAccessOutOfBoundsMask[rawId]
#define isIndexOutOfBounds(rawId)           ((g_RawIndexPastEnd & g_RawAccessOutOfBoundsMask[rawId]) > 0)

/* macros to get and set the Rx data buffer size (size of the currently mounted Rx data packet */
#define SetRxDataPacketLength(length)      g_rxBufferLength = length
#define GetRxDataPacketLength()            g_rxBufferLength

/*******************************************************************************
*                                       GLOBALS
********************************************************************************/
extern bool   g_HostRAWDataPacketReceived;
extern uint8_t  RawWindowState[2];     /* see RAW Window states above */
extern uint8_t  g_RawIndexPastEnd;
extern uint16_t g_rxBufferLength;
extern ROM uint8_t g_RawAccessOutOfBoundsMask[];

/*******************************************************************************
*                                 FUNCTION PROTOTYPES
********************************************************************************/
void   RawInit(void);

bool    RawSetIndex(uint16_t rawId, uint16_t index);
uint16_t  RawGetIndex(uint16_t rawId);
void    RawGetByte(uint16_t rawId, uint8_t *pBuffer, uint16_t length);
void    RawSetByte(uint16_t rawId, uint8_t *pBuffer, uint16_t length);
void    RawSetByteROM(uint16_t rawId, ROM uint8_t *pBuffer, uint16_t length);
void    RawGetMgmtRxBuffer(uint16_t *p_numBytes);
void    RawFreeRxMgmtBuffer(void);
void    SendRAWDataFrame(uint16_t bufLen);
void    PushRawWindow(uint8_t rawId);
uint16_t  PopRawWindow(uint8_t rawId);
void    ScratchUnmount(uint8_t rawId);
uint16_t  ScratchMount(uint8_t rawId);
void    RawRead(uint8_t rawId, uint16_t startIndex, uint16_t length, uint8_t *p_dest);
void    RawWrite(uint8_t rawId, uint16_t startIndex, uint16_t length, uint8_t *p_src);
bool    AllocateMgmtTxBuffer(uint16_t bytesNeeded);
void    DeallocateMgmtRxBuffer(void);
bool    AllocateDataTxBuffer(uint16_t bytesNeeded);

void    DeallocateDataRxBuffer(void);

uint16_t  RawMountRxBuffer(uint8_t rawId);
void    RawToRawCopy(uint8_t rawDestId, uint8_t rawSourceId, uint16_t length);

