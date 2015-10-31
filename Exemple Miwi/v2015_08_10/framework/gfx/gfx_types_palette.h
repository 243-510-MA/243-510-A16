/*******************************************************************************
Graphics Resource Types Header for Microchip Graphics Library

  Company:
    Microchip Technology Inc.
 
  File Name:
    gfx_types_palette.h

  Summary:
    This header file defines the types used in palettes for the Microchip Graphics
    Library.

  Description:
    This header defines the different structures used for palette resources in the
    Microchip Graphics Library.

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

#ifndef GFX_TYPES_PALETTE_H
// DOM-IGNORE-BEGIN
    #define GFX_TYPES_PALETTE_H
// DOM-IGNORE-END

/*****************************************************************************
 * SECTION: Includes
 *****************************************************************************/
#include <stdint.h>


// *****************************************************************************
/*
    <GROUP primitive_types>

    Typedef:
        GFX_PALETTE_ENTRY

    Summary:
        Defines the union used for each entry in the palette table.

    Description:
        Defines the union used for each entry in the palette table
        specified by GFX_RESOURCE_PALETTE structure.

        There are two types of palette:
        - For TFT: color is defined as 5-6-5 RGB format
                   where 5-bits for RED, 6-bits for GREEN
                   and 5-bits for BLUE.
        - For Monochrome: 4 bits are used to represent the luma.

    Remarks:
        None.

*/
// *****************************************************************************
typedef union __attribute__ ((packed))
{
    uint16_t    value;                // a 16-bit value representing a color or palette entry
    struct __attribute__ ((packed))
    {
        uint16_t    r : 5;            // represents the RED component 
        uint16_t    g : 6;            // represents the GREEN component 
        uint16_t    b : 5;            // represents the BLUE component 
    } color;                          // color value in 5-6-5 RGB format

    struct __attribute__ ((packed))
    {
        uint16_t    reserved : 12;    // reserved, used as a filler 
        uint16_t    luma : 4;         // monochrome LUMA value
    } monochrome;                     // monochrome LUMA value

} GFX_PALETTE_ENTRY;

#endif  // GFX_TYPES_PALETTE_H
