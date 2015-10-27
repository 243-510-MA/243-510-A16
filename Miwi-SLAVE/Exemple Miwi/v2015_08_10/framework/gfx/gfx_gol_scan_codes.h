/*******************************************************************************
 Module for Microchip Graphics Library - Graphic Object Layer

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_gol_scan_codes.h

  Summary:
    This is the header file for the AT keyboard codes.

  Description:
    This header file contains scan codes for the standard AT keyboard that
    can be used to process user action in GOL messages.
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

#ifndef _GFX_GOL_SCAN_CODES_H
// DOM-IGNORE-BEGIN
    #define _GFX_GOL_SCAN_CODES_H
// DOM-IGNORE-END

// *****************************************************************************
/*
    <GROUP gol_types>

    Typedef:
        GFX_AT_SCAN_CODES

    Summary:
        The following macros are the supported AT Keyboard scan codes.

    Description:
        The following macros are the supported AT Keyboard scan codes.

    Remarks:
        None.

*/
// *****************************************************************************
typedef enum
{
    // Carriage return pressed.
    SCAN_CR_PRESSED = 0x1C,
    // Carriage return released.
    SCAN_CR_RELEASED = 0x9C,
    // Carriage return alternate pressed.
    SCAN_CRA_PRESSED = 0x2C,
    // Carriage return alternate released.
    SCAN_CRA_RELEASED = 0xAC,
    // Delete key pressed.
    SCAN_DEL_PRESSED = 0x53,
    // Delete key released.
    SCAN_DEL_RELEASED = 0xD3,
    // Back space key pressed.
    SCAN_BS_PRESSED = 0x0E,
    // Back space key released.
    SCAN_BS_RELEASED = 0x8E,
    // Tab key pressed.
    SCAN_TAB_PRESSED = 0x0F,
    // Tab key released.
    SCAN_TAB_RELEASED = 0x8F,
    // Home key pressed.
    SCAN_HOME_PRESSED = 0x47,
    // Home key released.
    SCAN_HOME_RELEASED = 0xC7,
    // End key pressed.
    SCAN_END_PRESSED = 0x4F,
    // End key released.
    SCAN_END_RELEASED = 0xCF,
    // Page up key pressed.
    SCAN_PGUP_PRESSED = 0x49,
    // Page up key released.
    SCAN_PGUP_RELEASED = 0xC9,
    // Page down key pressed.
    SCAN_PGDOWN_PRESSED = 0x51,
    // Page down key released.
    SCAN_PGDOWN_RELEASED = 0xD1,
    // Up key pressed.
    SCAN_UP_PRESSED = 0x48,
    // Up key released.
    SCAN_UP_RELEASED = 0xC8,
    // Down key pressed.
    SCAN_DOWN_PRESSED = 0x50,
    // Down key released.
    SCAN_DOWN_RELEASED = 0xD0,
    // Left key pressed.
    SCAN_LEFT_PRESSED = 0x4B,
    // Left key released.
    SCAN_LEFT_RELEASED = 0xCB,
    // Right key pressed.
    SCAN_RIGHT_PRESSED = 0x4D,
    // Right key released.
    SCAN_RIGHT_RELEASED = 0xCD,
    // Space key pressed.
    SCAN_SPACE_PRESSED = 0x39,
    // Space key released.
    SCAN_SPACE_RELEASED = 0xB9,
} GFX_AT_SCAN_CODES;


#endif // _GFX_GOL_SCAN_CODES_H
