/*******************************************************************************
 Module for Microchip Graphics Library

  Company:
    Microchip Technology Inc.

  File Name:
    drv_gfx_tcon.h

  Summary:
    This header declares common API used in TCON drivers.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

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
// DOM-IGNORE-END

#ifndef _GFXTCON_H
    #define _GFXTCON_H

#include <stdint.h>
#include "driver/spi/drv_spi.h"

// typedef for write function pointer
typedef void (*TCON_WRITE_FUNC)(uint16_t index, uint16_t value);

/*********************************************************************
* Function:  DRV_TCON_Initialize(TCON_WRITE_FUNC pWriteFunc)
*
* PreCondition:  IO ports used by the driver should be initialized.
*
* Input: pWriteFunc - non-volatile memory write function pointer
*
* Output: None
*
* Side Effects: None
*
* Overview: initializes the timing controller used.
*
* Note: none
*
********************************************************************/
void DRV_TCON_Initialize(TCON_WRITE_FUNC pWriteFunc);

/*********************************************************************
* Function:  DRV_TCON_Initialize(TCON_WRITE_FUNC pWriteFunc)
*
* PreCondition:  IO ports used by the driver should be initialized.
*
* Input: pWriteFunc - non-volatile memory write function pointer
*
* Output: None
*
* Side Effects: None
*
* Overview: initializes the timing controller used.
*
* Note: none
*
********************************************************************/
void DRV_TCON_SPI_Initialize(DRV_SPI_INIT_DATA *pInitData);

/*********************************************************************
* Function:  DRV_TCON_BB_CommandWrite(uint16_t index, uint16_t value)
*
* PreCondition:  IO ports used by the driver should be initialized.
*
* Input: index - The index (or address) of the register to be written.
*        value - The value that will be written to the register.
*
* Output: None
*
* Side Effects: None
*
* Overview: Writes commands to the Timing Controller (TCON) using
*           bit-banged SPI.
*
* Note: none
*
********************************************************************/
void DRV_TCON_BB_CommandWrite(uint16_t index, uint16_t value);

/*********************************************************************
* Function:  DRV_TCON_SPI_CommandWrite(uint16_t index, uint16_t value)
*
* PreCondition:  SPI module used by the driver should be initialized.
*
* Input: index - The index (or address) of the register to be written.
*        value - The value that will be written to the register.
*
* Output: None
*
* Side Effects: None
*
* Overview: Writes commands to the Timing Controller (TCON) using
*           the pre-configured SPI channel.
*
* Note: none
*
********************************************************************/
void DRV_TCON_SPI_CommandWrite(uint16_t index, uint16_t value);

#endif // #ifndef _GFXTCON_H

