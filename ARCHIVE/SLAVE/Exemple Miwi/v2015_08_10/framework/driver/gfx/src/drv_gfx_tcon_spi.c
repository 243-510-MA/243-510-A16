/*******************************************************************************
 Module for Microchip Graphics Library

  Company:
    Microchip Technology Inc.

  File Name:
    drv_gfx_tcon_spi.c

  Summary:
    This module initializes the SSD1289 Timing Controller (TCON) Module.
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
#include <stdint.h>
#include <string.h>
#include "system.h"
#include "driver/gfx/drv_gfx_tcon.h"
#include "driver/spi/drv_spi.h"

/*
 * This is a generic Timing Controller (TCON) module driver using SPI as
 * the communication protocol. 
*/

static DRV_SPI_INIT_DATA spiInitData;

/******************************************************************************
  Function:
    void DRV_TCON_SPI_Initialize( const SYS_MODULE_INIT * const init )

  Summary:
    Initializes hardware and data for the given module

  Description:
    This routine initializes hardware for the instance of the NVM module,
    using the hardware initialization given data.  It also initializes all
    necessary internal data.

  Parameters:
    pInitData - Pointer to the data structure containing all data
                necessary to initialize the hardware. This pointer may
                be null if no data is required and static initialization
                values are to be used.

  Returns:
    None
******************************************************************************/
void DRV_TCON_SPI_Initialize(DRV_SPI_INIT_DATA *pInitData)
{
    // initialize the SPI channel to be used
    DRV_SPI_Initialize(pInitData);
    memcpy(&spiInitData, pInitData, sizeof(DRV_SPI_INIT_DATA));

}

/************************************************************************
* Function: void DRV_TCON_SPI_CommandWrite(uint8_t index, uint16_t value)
*                                                                       
* Overview: This writes a word to SPI module.
*                                                                       
* Input: index - The index (or address) of the register to be written.
*        value - The value that will be written to the register.
*                                                                       
* Output: none                                 
*                                                                       
************************************************************************/
void DRV_TCON_SPI_CommandWrite(uint16_t index, uint16_t value)
{
    typedef union
    {
        uint8_t  indexByte[2];
        uint16_t indexValue;
    }  GFX_TCON_INDEX;

    TCON_CS_LAT = 0;

    // Command
    TCON_DC_LAT = 0;
    DRV_SPI_Put(spiInitData.channel, ((GFX_TCON_INDEX)index).indexByte[1]);
    DRV_SPI_Put(spiInitData.channel, ((GFX_TCON_INDEX)index).indexByte[0]);

    TCON_CS_LAT = 1;
    __delay_us(10);
    TCON_CS_LAT = 0;

    // Data
    TCON_DC_LAT = 1;
    DRV_SPI_Put(spiInitData.channel, ((GFX_TCON_INDEX)value).indexByte[1]);
    DRV_SPI_Put(spiInitData.channel, ((GFX_TCON_INDEX)value).indexByte[0]);

    TCON_CS_LAT = 1;
    __delay_us(10);
}



