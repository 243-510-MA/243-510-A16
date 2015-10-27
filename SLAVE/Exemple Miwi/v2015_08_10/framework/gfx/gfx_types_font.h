/*******************************************************************************
 Graphics Resource Types Header for Microchip Graphics Library 

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_types_font.h

  Summary:
    This header file defines the types used in fonts for the Microchip Graphics 
    Library.

  Description:
    This header defines the different structures used for fonts resources in the 
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

#ifndef GFX_TYPES_FONT_H
// DOM-IGNORE-BEGIN
    #define GFX_TYPES_FONT_H
// DOM-IGNORE-END

/*****************************************************************************
 * SECTION: Includes
 *****************************************************************************/
#include <stdint.h>
#include <stddef.h>
#include "system_config.h"


// *****************************************************************************
/*  GFX_CONFIG_FONT_CHAR_SIZE

    Summary
        Configuration for the text character size.

    Description
        This configuration sets the text character size used in the library.

        To enable support for unicode fonts, GFX_CONFIG_FONT_CHAR_SIZE
        must be defined as 16 (size is 16 bits). For standard ascii
        fonts, this can be defined as 8. This changes the
        GFX_XCHAR definition. See GFX_XCHAR for details.

	<TABLE>
    	Set in configuration                    GFX_XCHAR	                        Description
     	####################################    #############################	##########################
     	#define GFX_CONFIG_FONT_CHAR_SIZE 16    #define GFX_XCHAR uint16_t	Use multi-byte characters (0-2^16 range).
     	#define GFX_CONFIG_FONT_CHAR_SIZE 8     #define GFX_XCHAR uint8_t	Use byte characters (0-255 range).
	</TABLE>

    Remarks:
        None.

*/
// *****************************************************************************
#if (GFX_CONFIG_FONT_CHAR_SIZE == 16)
    #define GFX_XCHAR   uint16_t
    #define GFX_UXCHAR  uint16_t
#elif (GFX_CONFIG_FONT_CHAR_SIZE == 8)
    #define GFX_XCHAR   char
    #define GFX_UXCHAR  uint8_t
#else
    #define GFX_XCHAR   char
    #define GFX_UXCHAR  uint8_t
#endif

// *****************************************************************************
/*  GFX_CONFIG_FONT_PROG_SPACE_ENABLE

    Summary
        Configuration for font to be located in the psv (constant) or
        program space of the PIC MCU.

    Description
        This configuration sets the location of the font to be placed
        in the psv (constant) or program space of the PIC MCU. This
        configuration macro, if used, must be defined in gfx_config.h.

        If this is not defined, the font will be placed in the
        const section instead.

        This configuration helps in the conversion of the font
        to files to be in the application by assigning the right
        type when the arrays are declared in C. The type used should
        match the type used in the applicaiton when referencing the
        fonts.
 
    Remarks:
        None.

*/
// *****************************************************************************
#ifdef GFX_CONFIG_FONT_PROG_SPACE_ENABLE
#ifndef __PIC32MX__
#ifndef GFX_CONFIG_FONT_PACKED_ENABLE
#define GFX_FONT_SPACE __prog__
#else
#define GFX_FONT_SPACE __pack_upper_byte
#endif
#else
#define GFX_FONT_SPACE const
#endif
#else
#define GFX_FONT_SPACE const
#endif

// *****************************************************************************
/*
    <GROUP primitive_types>

    Typedef:
        GFX_FONT_HEADER

    Summary:
        Specifies the header structure used for fonts in the library.

    Description:
        Specifies font header structure used in the library.

    Remarks:
        None.

*/
// *****************************************************************************
typedef struct
{
    uint8_t     fontID;                     // User assigned value
    uint8_t     extendedGlyphEntry : 1;     // Extended Glyph entry flag. When set font has extended glyph feature enabled.
    uint8_t     res1               : 1;     // Reserved for future use  (must be set to 0)
    uint8_t     bpp                : 2;     // Actual BPP = 2<sup>bpp</sup>  
                                            //   - 0 - 1 BPP
                                            //   - 1 - 2 BPP
                                            //   - 2 - 4 BPP
                                            //   - 3 - 8 BPP
    uint8_t     orientation        : 2;     // Orientation of the character glyphs (0,90,180,270 degrees)
                                            //   - 00 - Normal
                                            //   - 01 - Characters rotated 270 degrees clockwise
                                            //   - 10 - Characters rotated 180 degrees
                                            //   - 11 - Characters rotated 90 degrees clockwise
                                            // Note: Rendering DO NOT rotate the characters. The table contains rotated characters
                                            //       and will be rendered as is.
    uint8_t     res2               : 2;     // Reserved for future use (must be set to 0).
    uint16_t    firstChar;                  // Character code of first character (e.g. 32).
    uint16_t    lastChar;                   // Character code of last character in font (e.g. 3006).
    uint16_t    height;                     // Font characters height in pixels.
} GFX_FONT_HEADER;

// *****************************************************************************
/*
    <GROUP primitive_types>

    Typedef:
        GFX_FONT_GLYPH_ENTRY

    Summary:
        Specifies the structure of each glyph entry in the font
        of the library.

    Description:
        Specifies the structure of each glyph entry in the font
        of the library. This glyph entry structures defines the
        location of the glyph bitmap in the font table.

    Remarks:
        None.

*/
// *****************************************************************************
typedef struct
{
    uint8_t     width;                      // The width of the glyph.
    uint8_t     offsetLSB;                  // The least Significant byte of
                                            // the glyph location offset.
    uint16_t    offsetMSB;                  // The most Significant (2) bytes
                                            // of the glyph location offset.
} GFX_FONT_GLYPH_ENTRY;

// *****************************************************************************
/*
    <GROUP primitive_types>

    Typedef:
        GFX_FONT_GLYPH_ENTRY_EXTENDED

    Summary:
        Specifies the structure of each glyph entry in the font
        of the library when using the extended glyph feature.

    Description:
        Similar to the GFX_FONT_GLYPH_ENTRY, each character's
        glyph in a font that supports entended characters is
        also described by its glyph entry structure. The
        difference is that the extended glyph entry will
        contain additional information on how the characters
        are overlapped.

    Remarks:
        None.

*/
// *****************************************************************************
typedef struct
{
    uint32_t    offset;                     // The offset of the glyph. The
                                            // offset order is:
                                            // LSW_LSB LSW_MSB MSW_MSB MSW_MSB.
    uint16_t    cursorAdvance;              // The x-value by which cursor has
                                            // to advance after rendering
                                            // the glyph.
    uint16_t    glyphWidth;                 // The width of the glyph.
    int16_t     xAdjust;                    // The value that adjusts the 
                                            // x-position.
    int16_t     yAdjust;                    // The value that adjusts the
                                            // y-position.
} GFX_FONT_GLYPH_ENTRY_EXTENDED;

// DOM-IGNORE-BEGIN

// *****************************************************************************
/*
    <GROUP primitive_types>

    Typedef:
        GFX_FONT_OUTCHAR

    Summary:
        Specifies the structure of the character that will be
        rendered.

    Description:
        Specifies the structure of the character that will be
        rendered. This is used by the functions that renders
        characters unto the display buffer.

    Remarks:
        None.

*/
// *****************************************************************************
typedef struct 
{ 
#ifndef GFX_CONFIG_FONT_FLASH_DISABLE
    GFX_FONT_SPACE GFX_FONT_GLYPH_ENTRY             *pChTable;              // pointer to the glyph entry
    GFX_FONT_SPACE GFX_FONT_GLYPH_ENTRY_EXTENDED    *pChTableExtended;      // pointer to the extended glyph entry
#endif
#ifndef GFX_CONFIG_FONT_EXTERNAL_DISABLE
    uint8_t                 chImage[GFX_EXTERNAL_FONT_RASTER_BUFFER_SIZE];  // buffer of the character bitmap when sourced from external memory
    int16_t                 chEffectiveGlyphWidth;                          // the width of the glyph
#endif
    uint8_t                 bpp;                                            // color depth of the character bitmap
    int16_t                 chGlyphWidth;                                   // width of the character
    GFX_FONT_SPACE uint8_t  *pChImage;                                      // pointer to the bitmap of the character
    int16_t                 xAdjust;                                        // horizontal adjustment when rendering the character
    int16_t                 yAdjust;                                        // vertical adjustment when rendering the character
    int16_t                 xWidthAdjust;                                   // width adjustment when rendering the character
    int16_t                 heightOvershoot;                                // overshoot of the glyph when rendering the character
} GFX_FONT_OUTCHAR;

// DOM-IGNORE-END

#endif // GFX_TYPES_FONT_H

