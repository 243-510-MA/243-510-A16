/*******************************************************************************
 Graphics Resource Types Header for Microchip Graphics Library 

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_types_image.h

  Summary:
    This header file defines the types used in images for the Microchip Graphics 
    Library.

  Description:
    This header defines the different structures used for images resources in the 
    Microchip Graphics Library. 
    
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

#ifndef GFX_TYPES_IMAGE_H
// DOM-IGNORE-BEGIN
    #define GFX_TYPES_IMAGE_H
// DOM-IGNORE-END
    
/*****************************************************************************
 * SECTION: Includes
 *****************************************************************************/
#include <stdint.h>
#include "system_config.h"

#if defined (__PIC32MX__) || defined (__PIC32MZ__)
typedef const uint8_t                   uint8_gfx_image_prog;
typedef const int8_t                    int8_gfx_image_prog;
typedef const uint16_t                  uint16_gfx_image_prog;
typedef const int16_t                   int16_gfx_image_prog;
typedef const uint32_t                  uint32_gfx_image_prog;
typedef const int32_t                   int32_gfx_image_prog;
#else
// General program space data types
typedef __prog__ uint8_t                uint8_prog;
typedef __prog__ int8_t                 int8_prog;
typedef __prog__ uint16_t               uint16_prog;
typedef __prog__ int16_t                int16_prog;
typedef __prog__ uint32_t               uint32_prog;
typedef __prog__ int32_t                int32_prog;

#ifdef GFX_CONFIG_IMAGE_PACKED_ENABLE
typedef __pack_upper_byte uint8_t       uint8_prog_pack;
typedef __pack_upper_byte int8_t        int8_prog_pack;
typedef __pack_upper_byte uint16_t      uint16_prog_pack;
typedef __pack_upper_byte int16_t       int16_prog_pack;
typedef __pack_upper_byte uint32_t      uint32_prog_pack;
typedef __pack_upper_byte int32_t       int32_prog_pack;
#endif

// GFX Image data Types
#ifndef GFX_CONFIG_IMAGE_PACKED_ENABLE
typedef uint8_prog                      uint8_gfx_image_prog;
typedef int8_prog                       int8_gfx_image_prog;
typedef uint16_prog                     uint16_gfx_image_prog;
typedef int16_prog                      int16_gfx_image_prog;
typedef uint32_prog                     uint32_gfx_image_prog;
typedef int32_prog                      int32_gfx_image_prog;
#else
typedef uint8_prog_pack                 uint8_gfx_image_prog;
typedef int8_prog_pack                  int8_gfx_image_prog;
typedef uint16_prog_pack                uint16_gfx_image_prog;
typedef int16_prog_pack                 int16_gfx_image_prog;
typedef uint32_prog_pack                uint32_gfx_image_prog;
typedef int32_prog_pack                 int32_gfx_image_prog;
#endif

#endif

// *****************************************************************************
/* 
    <GROUP primitive_types>

    Typedef:
        GFX_MCHP_BITMAP_HEADER

    Summary:
        The structure used to define the Microchip bitmap header.
        
    Description:
        The structure used to define the Microchip bitmap header.

    Remarks:
        None.
        
*/
// *****************************************************************************
typedef struct
{
    uint8_t     bitmapType;                 // type of image, information on how to render the image  
                                            // 0 - no compression, palette is present 
                                            //     for color depth = 8, 4 and 1 BPP
                                            // 1 - palette is provided as a separate 
                                            //     object (see PALETTE_HEADER) for 
                                            //     color depth = 8, 4, and 1 BPP, 
                                            //     ID to the palette is embedded in the bitmap. 

    uint8_t     colorDepth;                 // Color depth used
    int16_t     height;                     // Image height
    int16_t     width;                      // Image width
} GFX_MCHP_BITMAP_HEADER;

#endif // GFX_TYPES_IMAGE_H

