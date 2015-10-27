/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_wifi_priv.h

  Summary:
    Module for Microchip TCP/IP Stack
    -Provides access to MRF24W WiFi controller
    -Reference: MRF24W Data sheet, IEEE 802.11 Standard

  Description:
    MRF24W Driver Internal use

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

#ifndef __DRV_WF_DRIVERPRV_H_
#define __DRV_WF_DRIVERPRV_H_

/*******************************************************************************
*                                           DEFINES
********************************************************************************/

/*--------------------*/
/* Endianness defines */
/*--------------------*/
#define WF_BIG_ENDIAN                   (0)
#define WF_LITTLE_ENDIAN                (1)

/* Indicate whether the Host CPU is big-endian or little-endian */
#define HOST_CPU_ENDIANNESS                 WF_LITTLE_ENDIAN    /* WF_BIG_ENDIAN or WF_LITTLE_ENDIAN */

/*-------------------*/
/* Endianness Macros */
/*-------------------*/
/* if the Host CPU is Little Endian, which does not match the MRF24W */
#if (HOST_CPU_ENDIANNESS == WF_LITTLE_ENDIAN)

    /* 32-bit data type conversion */
    #define HTOWFL(a) (((a & 0x000000ff) << 24) | ((a & 0x0000ff00) << 8) | ((a & 0x00ff0000) >> 8) | ((a & 0xff000000) >> 24))
    #define WFTOHL(a) HTOWFL(a)

    /* 16-bit data type conversion */
    #define HSTOWFS(a) (((a) << 8) | ((a) >> 8))
    #define WFSTOHS(a) HSTOWFS(a)

/* else Host CPU is Big-Endian, which matches the MRF24W */
#else
    #define HTOWFL(a)   (a)
    #define WFTOHL(a)   (a)
    #define HSTOWFS(a)  (a)
    #define WFSTOHS(a)  (a)
#endif

/*------------*/
/* Endianness */
/*------------*/
/* ensure that endianness has been defined */
#if (HOST_CPU_ENDIANNESS != WF_LITTLE_ENDIAN) && (HOST_CPU_ENDIANNESS != WF_BIG_ENDIAN)
#error Must define HOST_CPU_ENDIANNESS to either WF_LITTLE_ENDIAN or WF_BIG_ENDIAN
#endif

// Must set level first, than configure as output
// This avoids a potential glitch on the WiFi reset line that was occurring after the PLL work-around,
// causing the module to fail during chip reset.
#define WF_SetCE_N(level)                                           \
    /* set pin to desired level       */                            \
    WF_HIBERNATE_IO   = level;                                      \
                                                                    \
    /* configure I/O as ouput         */                            \
    WF_HIBERNATE_TRIS = 0

// must set level first, than configure as output
#define WF_SetRST_N(level)                                          \
    /* set pin to desired level         */                          \
    WF_RESET_IO   = level;                                          \
                                                                    \
    /* configure the I/O as an output   */                          \
    WF_RESET_TRIS = 0

/* SPI Tx Message Types */
#define WF_DATA_REQUEST_TYPE            ((uint8_t)1)
#define WF_MGMT_REQUEST_TYPE            ((uint8_t)2)

/* SPI Rx Message Types */
#define WF_DATA_TX_CONFIRM_TYPE         ((uint8_t)1)
#define WF_MGMT_CONFIRM_TYPE            ((uint8_t)2)
#define WF_DATA_RX_INDICATE_TYPE        ((uint8_t)3)
#define WF_MGMT_INDICATE_TYPE           ((uint8_t)4)

/* SPI Tx/Rx Data Message Subtypes */
#define WF_STD_DATA_MSG_SUBTYPE         ((uint8_t)1)
#define WF_NULL_DATA_MSG_SUBTYPE        ((uint8_t)2)
/* reserved value                       ((uint8_t)3) */
#define WF_UNTAMPERED_DATA_MSG_SUBTYPE  ((uint8_t)4)

#define WF_TX_DATA_MSG_PREAMBLE_LENGTH  ((uint8_t)3)

#define WF_READ_REGISTER_MASK           ((uint8_t)(0x40))
#define WF_WRITE_REGISTER_MASK          ((uint8_t)(0x00))

/*--------------------------------*/
/* MRF24W 8-bit Host Registers */
/*--------------------------------*/
#define WF_HOST_INTR_REG            ((uint8_t)(0x01))  /* 8-bit register containing 1st level interrupt bits. */
#define WF_HOST_MASK_REG            ((uint8_t)(0x02))  /* 8-bit register containing 1st level interrupt mask. */

/*---------------------------------*/
/* MRF24W 16-bit Host Registers */
/*---------------------------------*/
#define WF_HOST_RAW0_CTRL1_REG      ((uint16_t)(0x26))
#define WF_HOST_RAW0_STATUS_REG     ((uint16_t)(0x28))
#define WF_HOST_RAW1_CTRL1_REG      ((uint16_t)(0x2a))
#define WF_HOST_INTR2_REG           ((uint16_t)(0x2d)) /* 16-bit register containing 2nd level interrupt bits */
#define WF_HOST_INTR2_MASK_REG      ((uint16_t)(0x2e))
#define WF_HOST_WFIFO_BCNT0_REG     ((uint16_t)(0x2f)) /* 16-bit register containing available write size for fifo 0 (data)   */
                                                       /* (LS 12 bits contain the length)                                     */

#define WF_HOST_WFIFO_BCNT1_REG     ((uint16_t)(0x31)) /* 16-bit register containing available write size for fifo 1 (mgmt)   */
                                                       /* (LS 12 bits contain the length)                                     */

#define WF_HOST_RFIFO_BCNT0_REG     ((uint16_t)(0x33)) /* 16-bit register containing number of bytes in read fifo 0 (data rx) */
                                                       /* (LS 12 bits contain the length)                                     */
#define WF_HOST_RESET_REG           ((uint16_t)(0x3c))
#define WF_HOST_RESET_MASK          ((uint16_t)(0x0001))

#define WF_PSPOLL_H_REG             ((uint16_t)(0x3d)) /* 16-bit register used to control low power mode                      */
#define WF_INDEX_ADDR_REG           ((uint16_t)(0x3e)) /* 16-bit register to move the data window                             */
#define WF_INDEX_DATA_REG           ((uint16_t)(0x3f)) /* 16-bit register to read or write address-indexed register           */

/*----------------------------------------------------------------------------------------*/
/* MRF24W registers accessed via the WF_INDEX_ADDR_REG and WF_INDEX_DATA_REG registers */
/*----------------------------------------------------------------------------------------*/
#define WF_HW_STATUS_REG            ((uint16_t)(0x2a)) /* 16-bit read only register providing hardware status bits */
#define WF_CONFIG_CTRL0_REG         ((uint16_t)(0x2e)) /* 16-bit register used to initiate Hard reset              */
#define WF_LOW_POWER_STATUS_REG     ((uint16_t)(0x3e)) /* 16-bit register read to determine when low power is done */

/* This bit mask is used in the HW_STATUS_REG to determine */
/* when the MRF24W has completed its hardware reset.       */
/*  0 : MRF24W is in reset                                 */
/*  1 : MRF24W is not in reset                             */
#define WF_HW_STATUS_NOT_IN_RESET_MASK ((uint16_t)(0x1000))

/* Definitions represent individual interrupt bits for the 8-bit host interrupt registers */
/*  WF_HOST_INTR_REG and WF_HOST_MASK_REG                                                 */
#define WF_HOST_INT_MASK_INT2               ((uint8_t)(0x01))
#define WF_HOST_INT_MASK_FIFO_1_THRESHOLD   ((uint8_t)(0x80))
#define WF_HOST_INT_MASK_FIFO_0_THRESHOLD   ((uint8_t)(0x40))
#define WF_HOST_INT_MASK_RAW_1_INT_0        ((uint8_t)(0x04))
#define WF_HOST_INT_MASK_RAW_0_INT_0        ((uint8_t)(0x02))
#define WF_HOST_INT_MASK_ALL_INT            ((uint8_t)(0xff))

/* Bit mask for all interrupts in the level 2 16-bit interrupt register */
#define WF_HOST_2_INT_MASK_ALL_INT          ((uint16_t)(0xffff))

/* these definitions are used in calls to enable and
 * disable interrupt bits. */
#define WF_INT_DISABLE ((uint8_t)0)
#define WF_INT_ENABLE  ((uint8_t)1)

#define WF_LOW_POWER_MODE_ON            (1)
#define WF_LOW_POWER_MODE_OFF           (0)

#if defined(WF_USE_POWER_SAVE_FUNCTIONS)
    void EnsureWFisAwake(void);
    void WFConfigureLowPowerMode(uint8_t action);
    bool WFisPsPollEnabled(void);
    bool WFIsPsPollActive(void);
#else
    #define EnsureWFisAwake()
    #define WFConfigureLowPowerMode(action)
#endif

#define WF_MAC_ADDRESS_LENGTH            (6)

/*******************************************************************************
*                                           FUNCTION PROTOTYPES
********************************************************************************/

/* tComContext - Used by the COM layer to manage State information */
typedef struct
{
    volatile uint8_t rawInterrupt;
    bool           waitingForRawMoveCompleteInterrupt;
} tRawMoveState;

extern tRawMoveState RawMoveState;

/*******************************************************************************
*                             FUNCTION PROTOTYPES
********************************************************************************/

void    WFHardwareInit(void);
uint16_t  Read16BitWFRegister(uint8_t regId);
void    Write16BitWFRegister(uint8_t regId, uint16_t value);
uint8_t   Read8BitWFRegister(uint8_t regId);
void    Write8BitWFRegister(uint8_t regId, uint8_t value);
void    WriteWFArray(uint8_t regId, uint8_t *pBuf, uint16_t length);
void    WriteWFROMArray(uint8_t regId, ROM uint8_t *pBuf, uint16_t length);
void    ReadWFArray(uint8_t  regId, uint8_t *pBuf, uint16_t length);

bool WFisConnected(void);
void SetLogicalConnectionState(bool state);
uint8_t GetEventNotificationMask(void);

#endif /* __DRV_WF_DRIVERPRV_H_ */
