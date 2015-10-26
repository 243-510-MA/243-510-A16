/*******************************************************************************
 Display Driver for Microchip Graphics Library - Display Driver Layer

  Company:
    Microchip Technology Inc.

  File Name:
    drv_gfx_sh1101a_ssd1303.h

  Summary:
    Display Driver header file for use with the Microchip Graphics Library.

  Description:
    This module implements the display driver for the following controllers:
    * Sino Wealth Microelectronic SH1101A OLED controller driver
    * Solomon Systech SSD1303 LCD controller driver
    This module implements the basic Display Driver Layer API required by the
    Microchip Graphics Library to enable, initialize and render pixels
    to the display controller.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2014 released Microchip Technology Inc.  All rights reserved.

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


#ifndef _SH1101A_SSD1303_OLED_H
    #define _SH1101A_SSD1303_OLED_H

#include <stdint.h>
#include "system_config.h"

    #ifdef USE_16BIT_PMP
        #error "This driver do not support 16-bit PMP (remove USE_16BIT_PMP option from system_config.h)"
    #endif
    #ifndef DISP_HOR_RESOLUTION
        #error "DISP_HOR_RESOLUTION must be defined in system_config.h"
    #endif
    #ifndef DISP_VER_RESOLUTION
        #error "DISP_VER_RESOLUTION must be defined in system_config.h"
    #endif
    #if (GFX_CONFIG_COLOR_DEPTH != 1)
        #error "GFX_CONFIG_COLOR_DEPTH must be defined to 1 in gfx_config.h"
    #endif
    #ifndef DISP_ORIENTATION
        #error "DISP_ORIENTATION must be defined in system_config.h"
    #endif

    #ifndef GFX_CONFIG_DOUBLE_BUFFERING_DISABLE
        #error GFX_CONFIG_DOUBLE_BUFFERING_DISABLE must be defined in system_config.h. This driver do not support double buffering feature.
    #endif

/*********************************************************************
* Overview: Horizontal and vertical screen size.
*********************************************************************/
    #if (DISP_HOR_RESOLUTION != 128)
        #error "This driver do not support this resolution. Set horisontal resolution to 128 pixels."
    #endif
    #if (DISP_VER_RESOLUTION != 64)
        #error "This driver do not support this resolution. Set vertical resolution to 64 pixels."
    #endif

/*********************************************************************
* Overview: Display orientation.
*********************************************************************/
    #if (DISP_ORIENTATION != 0)
        #error "This driver do not support this orientation. Set the DISP_ORIENTATION to 0."
    #endif

/*********************************************************************
* Overview: Defines the display offset in x direction. Dependent on the display 
* used and how it is connected.
*********************************************************************/
    #define DRV_GFX_OFFSET  2


/*********************************************************************
* Overview: Screen Saver parameters. 
*   - SSON - Means that screen saver will be enabled when
*            ScreenSaver(SSON) function is called with SSON
*            as parameter.
*   - SSOFF - Means that screen saver will be disbled when
*            ScreenSaver(SSOFF) function is called with SSOFF
*            as parameter.
*           
*********************************************************************/
    #define SSON    1           // screen saver is turned on
    #define SSOFF   0           // screen saver is turned off

// Memory pitch for line
    #define LINE_MEM_PITCH  0x100

#endif // _SH1101A_SSD1303_OLED_H
