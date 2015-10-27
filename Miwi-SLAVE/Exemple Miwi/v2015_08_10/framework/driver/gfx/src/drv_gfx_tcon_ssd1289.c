/*******************************************************************************
 Module for Microchip Graphics Library

  Company:
    Microchip Technology Inc.

  File Name:
    drv_gfx_tcon_ssd1289.c

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
#include "system_config.h"
#include "system.h"
#include "driver/gfx/drv_gfx_tcon.h"

/************************************************************************
* Function: void DRV_TCON_Initialize(void)
*                                                                       
* Overview: Initialize the IOs to implement Bitbanged SPI interface
*           to connect to the Timing Controller through SPI.
*                                                                       
* Input: none
*                                                                       
* Output: none                                 
*                                                                       
************************************************************************/
void DRV_TCON_Initialize(TCON_WRITE_FUNC pWriteFunc)
{

    pWriteFunc(0x0028, 0x0006);
    pWriteFunc(0x0000, 0x0001);
    __delay_ms(15);

    pWriteFunc(0x002B, 0x9532);
    pWriteFunc(0x0003, 0xAAAC);
    pWriteFunc(0x000C, 0x0002);
    pWriteFunc(0x000D, 0x000A);
    pWriteFunc(0x000E, 0x2C00);
    pWriteFunc(0x001E, 0x00AA);
    pWriteFunc(0x0025, 0x8000);
    __delay_ms(15);

    pWriteFunc(0x0001, 0x2B3F);
    pWriteFunc(0x0002, 0x0600);
    pWriteFunc(0x0010, 0x0000);
    __delay_ms(20);

    pWriteFunc(0x0005, 0x0000);
    pWriteFunc(0x0006, 0x0000);


    pWriteFunc(0x0016, 0xEF1C);
    pWriteFunc(0x0017, 0x0003);
    pWriteFunc(0x0007, 0x0233);
    pWriteFunc(0x000B, 0x5312);
    pWriteFunc(0x000F, 0x0000);
    __delay_ms(20);

    pWriteFunc(0x0041, 0x0000);
    pWriteFunc(0x0042, 0x0000);
    pWriteFunc(0x0048, 0x0000);
    pWriteFunc(0x0049, 0x013F);
    pWriteFunc(0x0044, 0xEF00);
    pWriteFunc(0x0045, 0x0000);
    pWriteFunc(0x0046, 0x013F);
    pWriteFunc(0x004A, 0x0000);
    pWriteFunc(0x004B, 0x0000);
    __delay_ms(20);

    pWriteFunc(0x0030, 0x0707);
    pWriteFunc(0x0031, 0x0704);
    pWriteFunc(0x0032, 0x0204);
    pWriteFunc(0x0033, 0x0201);
    pWriteFunc(0x0034, 0x0203);
    pWriteFunc(0x0035, 0x0204);
    pWriteFunc(0x0036, 0x0204);
    pWriteFunc(0x0037, 0x0502);
    pWriteFunc(0x003A, 0x0302);
    pWriteFunc(0x003B, 0x0500);
    __delay_ms(20);

}


