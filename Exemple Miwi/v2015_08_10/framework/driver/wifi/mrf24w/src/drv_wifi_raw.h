/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_wifi_raw.h

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
#ifndef __DRV_WF_RAW_H_
#define __DRV_WF_RAW_H_

#define RAW_ID_0                        (0)
#define RAW_ID_1                        (1)

// RAW0 used for Rx, RAW1 used for Tx
#define RAW_RX_ID                       RAW_ID_0
#define RAW_TX_ID                       RAW_ID_1
#define RAW_INVALID_ID                  (0xff)

// Source/Destination objects on the MRF24W
#define RAW_MAC                         (0x00)   /* Cmd processor (aka MRF24W MAC)                 */
#define RAW_MGMT_POOL                   (0x10)   /* For 802.11 Management packets                  */
#define RAW_DATA_POOL                   (0x20)   /* Data Memory pool used for tx and rx operations */
#define RAW_SCRATCH_POOL                (0x30)   /* Scratch object                                 */
#define RAW_STACK_MEM                   (0x40)   /* single level stack to save state of RAW        */
#define RAW_COPY                        (0x70)   /* RAW to RAW copy                                */

/* 8-bit RAW registers */
#define RAW_0_DATA_REG                  (0x20)
#define RAW_1_DATA_REG                  (0x21)

/* 16 bit RAW registers */
#define RAW_0_CTRL_0_REG                (0x25)
#define RAW_0_CTRL_1_REG                (0x26)
#define RAW_0_INDEX_REG                 (0x27)
#define RAW_0_STATUS_REG                (0x28)
#define RAW_1_CTRL_0_REG                (0x29)
#define RAW_1_CTRL_1_REG                (0x2a)
#define RAW_1_INDEX_REG                 (0x2b)
#define RAW_1_STATUS_REG                (0x2c)

/* RAW Window states */
#define WF_RAW_UNMOUNTED            (0)
#define WF_SCRATCH_MOUNTED          (1)
#define WF_RAW_DATA_MOUNTED         (2)
#define WF_RAW_MGMT_MOUNTED         (3)

#define SetRawWindowState(rawId, state)    RawWindowState[rawId] = state
#define GetRawWindowState(rawId)           RawWindowState[rawId]

/*******************************************************************************
*                                       GLOBALS
********************************************************************************/
extern bool   g_HostRAWDataPacketReceived;
extern bool   RawWindowReady[2];     /* for Tx and Rx, true = ready for use, false = not ready for use */
extern uint8_t  RawWindowState[2];     /* see RAW Window states above                                    */

/*******************************************************************************
*                                       FUNCTION PROTOTYPES
********************************************************************************/
void   RawInit(void);

bool    RawSetIndex(uint16_t rawId, uint16_t index);
uint16_t  RawGetIndex(uint16_t rawId);
void    RawGetByte(uint16_t rawId, uint8_t *pBuffer, uint16_t length);
void    RawSetByte(uint16_t rawId, uint8_t *pBuffer, uint16_t length);
void    RawSetByteROM(uint16_t rawId, ROM uint8_t *pBuffer, uint16_t length);
bool    RawGetMgmtRxBuffer(uint16_t *p_numBytes);
void    RawFreeRxMgmtBuffer(void);
void    SetRawRxMgmtInProgress(bool action);
bool    isRawRxMgmtInProgress(void);
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
void    DeallocateDataTxBuffer(void);
void    DeallocateDataRxBuffer(void);
void    RawSendTxBuffer(uint16_t len);
uint16_t  RawMountRxBuffer(void);
void    RawToRawCopy(uint8_t rawdestId, uint16_t length);
#endif /* __DRV_WF_RAW_H_ */