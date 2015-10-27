/*******************************************************************************
 Module for Microchip Graphics Library

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_colors_x11.h

  Summary:
    The header file contains X11 color definitions.

  Description:
    This header file provides X11 color definitions as an option of
    color definitions to use.

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

#ifndef _GFX_COLORS_X11_H
    #define _GFX_COLORS_X11_H

/*********************************************************************
* Section: Includes
*********************************************************************/
#include <stdint.h>
#include "gfx/gfx_types_macros.h"

/*********************************************************************
* These are color defines from the X11 color from the CSS3 specification
*********************************************************************/

/*********************************************************************
 * X11: Pink Colors
 *********************************************************************/
#define GFX_X11_PINK                GFX_RGBConvert(0xFF, 0xC0, 0xCB)
#define GFX_X11_LIGHT_PINK          GFX_RGBConvert(0xFF, 0xB6, 0xC1)
#define GFX_X11_HOT_PINK            GFX_RGBConvert(0xFF, 0x69, 0xB4)
#define GFX_X11_DEEP_PINK           GFX_RGBConvert(0xFF, 0x14, 0x93)
#define GFX_X11_PALE_VIOLET_RED     GFX_RGBConvert(0xDB, 0x70, 0x93)
#define GFX_X11_MEDIUM_VIOLET_RED   GFX_RGBConvert(0xC7, 0x15, 0x85)
/*********************************************************************
 * X11: Red Colors
 *********************************************************************/
#define GFX_X11_LIGHT_SALMON        GFX_RGBConvert(0xFF, 0xA0, 0x7A)
#define GFX_X11_SALMON              GFX_RGBConvert(0xFA, 0x80, 0x72)
#define GFX_X11_DARK_SALMON         GFX_RGBConvert(0xEA, 0x96, 0x7A)
#define GFX_X11_LIGHT_CAROL         GFX_RGBConvert(0xF0, 0x80, 0x80)
#define GFX_X11_INDIAN_RED          GFX_RGBConvert(0xCD, 0x5C, 0x5C)
#define GFX_X11_CRIMSON             GFX_RGBConvert(0xDC, 0x14, 0x3C)
#define GFX_X11_FIRE_BRICK          GFX_RGBConvert(0xB2, 0x22, 0x22)
#define GFX_X11_DARK_RED            GFX_RGBConvert(0x8B, 0x00, 0x00)
#define GFX_X11_RED                 GFX_RGBConvert(0xFF, 0x00, 0x00)
/*********************************************************************
 * X11: Orange Colors
 *********************************************************************/
#define GFX_X11_ORANGE_RED          GFX_RGBConvert(0xFF, 0x45, 0x00)
#define GFX_X11_TOMATO              GFX_RGBConvert(0xFF, 0x63, 0x47)
#define GFX_X11_CORAL               GFX_RGBConvert(0xFF, 0x7F, 0x50)
#define GFX_X11_DARK_ORANGE         GFX_RGBConvert(0xFF, 0x8C, 0x00)
#define GFX_X11_ORANGE              GFX_RGBConvert(0xFF, 0xA5, 0x00)
#define GFX_X11_GOLD                GFX_RGBConvert(0xFF, 0xD7, 0x00)
/*********************************************************************
 * X11: Yellow Colors
 *********************************************************************/
#define GFX_X11_YELLOW                  GFX_RGBConvert(0xFF, 0xFF, 0x00)
#define GFX_X11_LIGHT_YELLOW            GFX_RGBConvert(0xFF, 0xFF, 0xE0)
#define GFX_X11_LEMON_CHIFFON           GFX_RGBConvert(0xFF, 0xFA, 0xCD)
#define GFX_X11_LIGHT_GOLDENROD_YELLOW  GFX_RGBConvert(0xFA, 0xFA, 0xD2)
#define GFX_X11_PAPAYA_WHIP             GFX_RGBConvert(0xFF, 0xEF, 0xD5)
#define GFX_X11_MOCCASIN                GFX_RGBConvert(0xFF, 0xE4, 0xB5)
#define GFX_X11_PEACH_PUFF              GFX_RGBConvert(0xFF, 0xDA, 0xB9)
#define GFX_X11_PALE_GOLDENROD          GFX_RGBConvert(0xEE, 0xE8, 0xAA)
#define GFX_X11_KHAKI                   GFX_RGBConvert(0xF0, 0xE6, 0x8C)
#define GFX_X11_DARK_KHAKI              GFX_RGBConvert(0xBD, 0xB7, 0x6B)
/*********************************************************************
 * X11: Brown Colors
 *********************************************************************/
#define GFX_X11_CORNSILK            GFX_RGBConvert(0xFF, 0xF8, 0xDC)
#define GFX_X11_BLANCHED_ALMOND     GFX_RGBConvert(0xFF, 0xEB, 0xCD)
#define GFX_X11_BISQUE              GFX_RGBConvert(0xFF, 0xE4, 0xC4)
#define GFX_X11_NAVAJO_WHITE        GFX_RGBConvert(0xFF, 0xDE, 0xAD)
#define GFX_X11_WHEAT               GFX_RGBConvert(0xF5, 0xDE, 0xB3)
#define GFX_X11_BURLY_WOOD          GFX_RGBConvert(0xDE, 0xB8, 0x87)
#define GFX_X11_TAN                 GFX_RGBConvert(0xD2, 0xB4, 0x8C)
#define GFX_X11_ROSY_BROWN          GFX_RGBConvert(0xBc, 0x8F, 0x8F)
#define GFX_X11_SANDY_BROWN         GFX_RGBConvert(0xF4, 0xA4, 0x60)
#define GFX_X11_GOLDENROD           GFX_RGBConvert(0xDA, 0xA5, 0x20)
#define GFX_X11_DARK_GOLDENROD      GFX_RGBConvert(0xB8, 0x86, 0x0B)
#define GFX_X11_PERU                GFX_RGBConvert(0xCD, 0x85, 0x3F)
#define GFX_X11_CHOCOLATE           GFX_RGBConvert(0xD2, 0x69, 0x1E)
#define GFX_X11_SADDLE_BROWN        GFX_RGBConvert(0x8B, 0x45, 0x13)
#define GFX_X11_SIENNA              GFX_RGBConvert(0xA0, 0x52, 0x2D)
#define GFX_X11_BROWN               GFX_RGBConvert(0xA5, 0x2A, 0x2A)
#define GFX_X11_MARRON              GFX_RGBConvert(0x80, 0x00, 0x00)
/*********************************************************************
 * X11: Green Colors
 *********************************************************************/
#define GFX_X11_DARK_OLIVE_GREEN    GFX_RGBConvert(0x55, 0x6B, 0x2F)
#define GFX_X11_OLIVE               GFX_RGBConvert(0x80, 0x80, 0x00)
#define GFX_X11_OLIVE_DRAB          GFX_RGBConvert(0x6B, 0x8E, 0x23)
#define GFX_X11_YELLOW_GREEN        GFX_RGBConvert(0x9A, 0xCD, 0x32)
#define GFX_X11_LIME_GREEN          GFX_RGBConvert(0x32, 0xCD, 0x32)
#define GFX_X11_LIME                GFX_RGBConvert(0x00, 0xFF, 0x00)
#define GFX_X11_LAWN_GREEN          GFX_RGBConvert(0x7C, 0xFC, 0x00)
#define GFX_X11_CHARTEUSE           GFX_RGBConvert(0x7F, 0xFF, 0x00)
#define GFX_X11_GREEN_YELLOW        GFX_RGBConvert(0xAD, 0xFF, 0x2F)
#define GFX_X11_SPRING_GREEN        GFX_RGBConvert(0x00, 0xFF, 0x7F)
#define GFX_X11_MEDIUM_SPRING_GREEN GFX_RGBConvert(0x00, 0xFA, 0x9A)
#define GFX_X11_LIGHT_GREEN         GFX_RGBConvert(0x90, 0xEE, 0x90)
#define GFX_X11_PALE_GREEN          GFX_RGBConvert(0x98, 0xFB, 0x98)
#define GFX_X11_DARK_SEA_GREEN      GFX_RGBConvert(0x8F, 0xBC, 0x8F)
#define GFX_X11_MEDIUM_SEA_GREEN    GFX_RGBConvert(0x3C, 0xB3, 0x71)
#define GFX_X11_SEA_GREEN           GFX_RGBConvert(0x2E, 0x8B, 0x57)
#define GFX_X11_FOREST_GREEN        GFX_RGBConvert(0x22, 0x8B, 0x22)
#define GFX_X11_GREEN               GFX_RGBConvert(0x00, 0x80, 0x00)
#define GFX_X11_DARK_GREEN          GFX_RGBConvert(0x00, 0x64, 0x00)
/*********************************************************************
 * X11: CYAN COLORS
 *********************************************************************/
#define GFX_X11_MEDIUM_AQUAMARINE   GFX_RGBConvert(0x66, 0xCD, 0xAA)
#define GFX_X11_AQUA                GFX_RGBConvert(0x00, 0xFF, 0xFF)
#define GFX_X11_CYAN                GFX_RGBConvert(0x00, 0xFF, 0xFF)
#define GFX_X11_LIGHT_CYAN          GFX_RGBConvert(0xE0, 0xFF, 0xFF)
#define GFX_X11_PALE_TURQUOISE      GFX_RGBConvert(0xAF, 0xEE, 0xEE)
#define GFX_X11_AQUAMARINE          GFX_RGBConvert(0x7F, 0xFF, 0xD4)
#define GFX_X11_TURQUOISE           GFX_RGBConvert(0x40, 0xE0, 0xD0)
#define GFX_X11_MEDIUM_TURQUOISE    GFX_RGBConvert(0x48, 0xD1, 0xCC)
#define GFX_X11_DARK_TURQUOISE      GFX_RGBConvert(0x00, 0xCE, 0xD1)
#define GFX_X11_LIGHT_SEA_GREEN     GFX_RGBConvert(0x20, 0xB2, 0xAA)
#define GFX_X11_CADEL_BLUE          GFX_RGBConvert(0x5F, 0x9E, 0xA0)
#define GFX_X11_DARK_CYAN           GFX_RGBConvert(0x00, 0x8B, 0x8B)
#define GFX_X11_TEAL                GFX_RGBConvert(0x00, 0x80, 0x80)
/*********************************************************************
 * X11: BLUE COLORS
 *********************************************************************/
#define GFX_X11_LIGHT_STEEL_BLUE    GFX_RGBConvert(0xB0, 0xC4, 0xDE)
#define GFX_X11_POWDER_BLUE         GFX_RGBConvert(0xB0, 0xE0, 0xE6)
#define GFX_X11_LIGHT_BLUE          GFX_RGBConvert(0xAD, 0xD8, 0xE6)
#define GFX_X11_SKY_BLUE            GFX_RGBConvert(0x87, 0xCE, 0xEB)
#define GFX_X11_LIGHT_SKY_BLUE      GFX_RGBConvert(0x87, 0xCE, 0xFA)
#define GFX_X11_DEEP_SKY_BLUE       GFX_RGBConvert(0x00, 0xBF, 0xFF)
#define GFX_X11_DODGER_BLUE         GFX_RGBConvert(0x1E, 0x90, 0xFF)
#define GFX_X11_CORNFLOWER_BLUE     GFX_RGBConvert(0x64, 0x95, 0xED)
#define GFX_X11_STEEL_BLUE          GFX_RGBConvert(0x46, 0x82, 0xB4)
#define GFX_X11_ROYAL_BLUE          GFX_RGBConvert(0x41, 0x69, 0xE1)
#define GFX_X11_BLUE                GFX_RGBConvert(0x00, 0x00, 0xFF)
#define GFX_X11_MEDIUM_BLUE         GFX_RGBConvert(0x00, 0x00, 0xCD)
#define GFX_X11_DARK_BLUE           GFX_RGBConvert(0x00, 0x00, 0x8B)
#define GFX_X11_NAVY                GFX_RGBConvert(0x00, 0x00, 0x80)
#define GFX_X11_MIDNIGHT_BLUE       GFX_RGBConvert(0x19, 0x19, 0x70)
/*********************************************************************
 * X11: BLUE COLORS
 *********************************************************************/
#define GFX_X11_LAVENDER            GFX_RGBConvert(0xE6, 0xE6, 0xFA)
#define GFX_X11_THISTLE             GFX_RGBConvert(0xD8, 0xBF, 0xD8)
#define GFX_X11_PLUM                GFX_RGBConvert(0xDD, 0xA0, 0xDD)
#define GFX_X11_VIOLET              GFX_RGBConvert(0xEE, 0x82, 0xEE)
#define GFX_X11_ORCHID              GFX_RGBConvert(0xDA, 0x70, 0xD6)
#define GFX_X11_FUCHSIA             GFX_RGBConvert(0xFF, 0x00, 0xFF)
#define GFX_X11_MAGENTA             GFX_RGBConvert(0xFF, 0x00, 0xFF)
#define GFX_X11_MEDIUM_ORCHID       GFX_RGBConvert(0xBA, 0x55, 0xD3)
#define GFX_X11_MEDIUM_PURPLE       GFX_RGBConvert(0x93, 0x70, 0xDB)
#define GFX_X11_BLUE_VIOLET         GFX_RGBConvert(0x8A, 0x2B, 0xE2)
#define GFX_X11_DARK_VIOLET         GFX_RGBConvert(0x94, 0x00, 0xD3)
#define GFX_X11_DARK_ORCHID         GFX_RGBConvert(0x99, 0x32, 0xCC)
#define GFX_X11_DARK_MAGENTA        GFX_RGBConvert(0x8B, 0x00, 0x8B)
#define GFX_X11_PURPLE              GFX_RGBConvert(0x80, 0x00, 0x80)
#define GFX_X11_INDIGO              GFX_RGBConvert(0x4B, 0x00, 0x82)
#define GFX_X11_DARK_SLATE_BLUE     GFX_RGBConvert(0x48, 0x3D, 0x8B)
#define GFX_X11_SLATE_BLUE          GFX_RGBConvert(0x6A, 0x5A, 0xCD)
#define GFX_X11_MEDIUM_SLATE_BLUE   GFX_RGBConvert(0x7B, 0x68, 0xEE)
/*********************************************************************
 * X11: WHITE/GRAY/BLACK COLORS
 *********************************************************************/
#define GFX_X11_WHITE               GFX_RGBConvert(0xFF, 0xFF, 0xFF)
#define GFX_X11_SNOW                GFX_RGBConvert(0xFF, 0xFA, 0xFA)
#define GFX_X11_HONEYDEW            GFX_RGBConvert(0xF0, 0xFF, 0xF0)
#define GFX_X11_MINT_CREAM          GFX_RGBConvert(0xF5, 0xFF, 0xFA)
#define GFX_X11_AZURE               GFX_RGBConvert(0xF0, 0xFF, 0xFF)
#define GFX_X11_ALICE_BLUE          GFX_RGBConvert(0xF0, 0xF8, 0xFF)
#define GFX_X11_GHOST_WHITE         GFX_RGBConvert(0xF8, 0xF8, 0xFF)
#define GFX_X11_WHITE_SMOKE         GFX_RGBConvert(0xF5, 0xF5, 0xF5)
#define GFX_X11_SEASHELL            GFX_RGBConvert(0xFF, 0xF5, 0xEE)
#define GFX_X11_BEIGE               GFX_RGBConvert(0xF5, 0xF5, 0xDC)
#define GFX_X11_OLD_LACE            GFX_RGBConvert(0xFD, 0xF5, 0xE6)
#define GFX_X11_FLORAL_WHITE        GFX_RGBConvert(0xFF, 0xFA, 0xF0)
#define GFX_X11_IVORY               GFX_RGBConvert(0xFF, 0xFF, 0xF0)
#define GFX_X11_ANTIQUE_WHITE       GFX_RGBConvert(0xFA, 0xEB, 0xD7)
#define GFX_X11_LINEN               GFX_RGBConvert(0xFA, 0xF0, 0xE6)
#define GFX_X11_LAVENDOR_BLUSH      GFX_RGBConvert(0xFF, 0xF0, 0xF5)
#define GFX_X11_MISTY_ROSE          GFX_RGBConvert(0xFF, 0xE4, 0xE1)
#define GFX_X11_GAINSBORO           GFX_RGBConvert(0xDC, 0xDC, 0xDC)
#define GFX_X11_LIGHT_GRAY          GFX_RGBConvert(0xD3, 0xD3, 0xD3)
#define GFX_X11_SILVER              GFX_RGBConvert(0xC0, 0xC0, 0xC0)
#define GFX_X11_DARK_GREY           GFX_RGBConvert(0xA9, 0xA9, 0xA9)
#define GFX_X11_GREY                GFX_RGBConvert(0x80, 0x80, 0x80)
#define GFX_X11_DIM_GREY            GFX_RGBConvert(0x69, 0x69, 0x69)
#define GFX_X11_LIGHT_SLATE_GREY    GFX_RGBConvert(0x77, 0x88, 0x99)
#define GFX_X11_SLATE_GREY          GFX_RGBConvert(0x70, 0x80, 0x90)
#define GFX_X11_DARK_SLATE_GREY     GFX_RGBConvert(0x2F, 0x4F, 0x4F)
#define GFX_X11_BLACK               GFX_RGBConvert(0x00, 0x00, 0x00)


#endif  // _GFX_COLORS_X11_H

