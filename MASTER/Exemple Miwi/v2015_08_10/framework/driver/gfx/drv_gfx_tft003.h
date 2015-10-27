/*******************************************************************************
 Display Driver for Microchip Graphics Library - Display Driver Layer

  Company:
    Microchip Technology Inc.

  File Name:
    drv_gfx_tft003.h

  Summary:
    Display Driver header file for use with the Microchip Graphics Library.

  Description:
    This module implements the display driver for the following controllers:
    *  Ilitek ILI9341
    This module implements the basic Display Driver Layer API required by the 
    Microchip Graphics Library to enable, initialize and render pixels 
    to the display controller. 
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _DRV_GFX_TFT003_H
    #define _DRV_GFX_TFT003_H

#include <stdint.h>
#include "system_config.h"

/*********************************************************************
* Error Checking
*********************************************************************/
    #ifndef DISP_HOR_RESOLUTION
        #error DISP_HOR_RESOLUTION must be defined in system.h
    #endif
    #ifndef DISP_VER_RESOLUTION
        #error DISP_VER_RESOLUTION must be defined in system.h
    #endif
    #ifndef DISP_ORIENTATION
        #error DISP_ORIENTATION must be defined in system.h
    #endif
    #ifndef GFX_CONFIG_COLOR_DEPTH
        #error GFX_CONFIG_COLOR_DEPTH must be defined in gfx_config.h
    #endif

    #ifndef GFX_CONFIG_DOUBLE_BUFFERING_DISABLE
        #error GFX_CONFIG_DOUBLE_BUFFERING_DISABLE must be defined in system_config.h. This driver do not support double buffering feature.
    #endif


/*********************************************************************
* Overview: Horizontal and vertical screen size.
*********************************************************************/
    #if (DISP_HOR_RESOLUTION != 240)
        #error "This driver doesn't supports this resolution. Horizontal resolution must be 240 pixels."
    #endif
    #if (DISP_VER_RESOLUTION != 320)
        #error "This driver doesn't supports this resolution. Vertical resolution must be 320 pixels."
    #endif

/*********************************************************************
* Overview: Color depth.
*********************************************************************/
    #if (GFX_CONFIG_COLOR_DEPTH != 16)
        #error This driver support 16 BPP color depth only.
    #endif

// *****************************************************************************
/*  Function:
    void DRV_GFX_DisplayRegisterRead(uint8_t cmd, uint8_t length, uint8_t *pData)

    Summary:
        This function reads the specified register contents
        of the display controller.

    Description:
        This function reads the the contents of the register specified
        by cmd. Depending on the register being read, the number of
        data to read is specified by length and the contents placed in
        the address specified by pData.

*/
// *****************************************************************************
void DRV_GFX_DisplayRegisterRead(uint8_t cmd, uint8_t length, uint8_t *pData);

#endif // _DRV_GFX_TFT003_H
