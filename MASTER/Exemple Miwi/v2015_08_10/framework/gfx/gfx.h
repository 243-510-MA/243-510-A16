/*******************************************************************************
 Module for Microchip Graphics Library

  Company:
    Microchip Technology Inc.

  File Name:
    gfx.h

  Summary:
    The header file joins all header files used in the graphics library
    and contains compile options and defaults.

  Description:
    This header file includes all the header files required to use the
    Microchip Graphics Library. Library features and options defined
    in the Graphics Library configurations will be included in each build.
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

#ifndef _GFX_H
// DOM-IGNORE-BEGIN
    #define _GFX_H
// DOM-IGNORE-END

/////////////////////// GRAPHICS_LIBRARY_VERSION /////////////////////
// MSB is version, LSB is subversion
#define GRAPHICS_LIBRARY_VERSION    0x0460

////////////// GRAPHICS_LIBRARY_RESOURCE_FORMAT_VERSION //////////////
#define GFX_RESOURCE_FORMAT_VERSION 0x0101

////////////////////////////// INCLUDES //////////////////////////////
#include "system_config.h"
#include "gfx/gfx_types_macros.h"           
#include "driver/gfx/drv_gfx_display.h"
#include "gfx/gfx_primitive.h"
#include "gfx/gfx_colors.h"

#ifndef GFX_CONFIG_GOL_DISABLE
#include "gfx/gfx_gol.h"
#include "gfx/gfx_gol_scan_codes.h"
#include "gfx/gfx_gol_button.h"
#include "gfx/gfx_gol_check_box.h"
#include "gfx/gfx_gol_digital_meter.h"
#include "gfx/gfx_gol_edit_box.h"
#include "gfx/gfx_gol_group_box.h"
#include "gfx/gfx_gol_list_box.h"
#include "gfx/gfx_gol_meter.h"
#include "gfx/gfx_gol_picture.h"
#include "gfx/gfx_gol_progress_bar.h"
#include "gfx/gfx_gol_radio_button.h"
#include "gfx/gfx_gol_scroll_bar.h"
#include "gfx/gfx_gol_static_text.h"
#include "gfx/gfx_gol_text_entry.h"
#include "gfx/gfx_gol_window.h"
#endif

#endif // _GFX_H
