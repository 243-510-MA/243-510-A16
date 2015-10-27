/*******************************************************************************
 Module for Microchip Graphics Library - Primitive Layer

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_palette.c

  Summary:
    This implements the palette routines of the Primitive Layer.
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
#include <stdbool.h>
#include "gfx/gfx_palette.h"

#ifndef GFX_CONFIG_PALETTE_DISABLE

// *****************************************************************************
/*
    Function:
        GFX_FEATURE_STATUS GFX_PaletteStatusGet()

    Summary:
        This function returns the status of the palette mode feature
        of the library.

    Description:
        This function returns the current status of the palette mode
        feature if enabled or disabled.

*/
// *****************************************************************************
GFX_FEATURE_STATUS __attribute__ ((weak)) GFX_PaletteStatusGet(void)
{
    return (GFX_FEATURE_DISABLED);
}

// *****************************************************************************
/*
    Function:
        GFX_STATUS GFX_PaletteSet()

    Summary:
        This function sets the palette to be used to render pixels
        to the display.

    Description:
        This function sets the palette to be used to render pixels.
        Any pixels rendered to the display buffer will be using the
        index of the color in the palette that is specified by this
        function. It is possible to modify a section of the palette
        by specifying the start and count parameters.

        The start and count must be within bounds of the supported
        palette size. The function will return GFX_STATUS_FAILURE
        if the start value and/or start + count exceeds the size
        of the palette. Size of the palette is dependent on the
        color depth set by GFX_CONFIG_COLOR_DEPTH configuration.

*/
// *****************************************************************************
GFX_STATUS __attribute__ ((weak)) GFX_PaletteSet(
                                GFX_RESOURCE_HDR *pPalette,
                                uint16_t start,
                                uint16_t count)

{
#ifndef GFX_CONFIG_PALETTE_EXTERNAL_DISABLE
    GFX_PALETTE_ENTRY   paletteEntry[256];
#endif
    GFX_STATUS          status = GFX_STATUS_SUCCESS;


    switch(pPalette->type)
    {
    
        case GFX_RESOURCE_PALETTE_FLASH_NONE:

            if (pPalette->resource.palette.numberOfEntries < count)
            {
                count = pPalette->resource.palette.numberOfEntries;
            }

            status = DRV_GFX_PaletteSet(
                            pPalette->resource.palette.location.constByteAddress,
                            start,
                            count);
            break;

        case GFX_RESOURCE_PALETTE_RAM_NONE :

            if (pPalette->resource.palette.numberOfEntries < count)
            {
                count = pPalette->resource.palette.numberOfEntries;
            }

            status = DRV_GFX_PaletteSet(
                            pPalette->resource.palette.location.ramAddress,
                            start,
                            count);
            break;

#ifndef GFX_CONFIG_PALETTE_EXTERNAL_DISABLE
        case GFX_RESOURCE_PALETTE_EXTERNAL_NONE:
            /* NOTE:
               When GFX_PaletteSet() is called in an interrupt sub-routine
               (ISR) and the palette is sourced from external memory, make
               sure that the resource sharing is managed properly. Problems
               can occur when the main routine is performing access to the
               same memory and the interrupt occurs.
               The GFX_ExternalResourceCallback() may use that same driver
               to the external memory and may break currently ongoing accesses.
               Therefore, for cases where the GFX_PaletteSet() is launched
               in an ISR and the palette is from external memory. It will be
               safer to copy the palette to RAM first and call GFX_PaletteSet()
               in the ISR with the palette coming from the copy in RAM.
            */
            GFX_ExternalResourceCallback(
                    pPalette,
                    (startEntry * sizeof(GFX_PALETTE_ENTRY)),
                    sizeof(GFX_PALETTE_ENTRY)*length,
                    (void*)paletteEntry);

            status = DRV_GFX_PaletteSet(paletteEntry, startEntry, length);
            break;
#endif
        default:
            status = GFX_STATUS_FAILURE;
            break;

    }

    return (status);
}


#endif // #ifndef GFX_CONFIG_PALETTE_DISABLE
