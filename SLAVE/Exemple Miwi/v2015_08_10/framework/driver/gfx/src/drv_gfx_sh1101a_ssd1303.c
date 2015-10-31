/*******************************************************************************
 Display Driver for Microchip Graphics Library - Display Driver Layer

  Company:
    Microchip Technology Inc.

  File Name:
    drv_gfx_sh1101a_ssd1303.c

  Summary:
    Display Driver for use with the Microchip Graphics Library.

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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "system.h"
#include <stdint.h>

#if defined(USE_GFX_DISPLAY_CONTROLLER_SH1101A) || defined (USE_GFX_DISPLAY_CONTROLLER_SSD1303)

#include <libpic30.h>
#include "driver/gfx/drv_gfx_display.h"
#include "driver/gfx/drv_gfx_sh1101a_ssd1303.h"
#include "gfx/gfx_primitive.h"

#if defined (USE_GFX_PMP)
    #include "driver/gfx/drv_gfx_pmp.h"
#elif defined (USE_GFX_EPMP)
    #include "driver/gfx/drv_gfx_epmp.h"
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Local Functions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*  Function:
    void DRV_GFX_AddressSet(    uint8_t page,
                                uint8_t lowerAddr,
                                uint8_t higherAddr)

    Summary:
        This function sets the address of the pixel to be read
        or modified.

    Description:
        This function sets the address of the pixel to be read or
        modified. The address can be set once for contiguous pixels
        to be modified. Auto increment of the internal address
        pointer in the controller will be performed after every
        read or write.

*/
// *****************************************************************************
inline void __attribute__((always_inline)) DRV_GFX_AddressSet(
                                uint8_t page,
                                uint8_t lowerAddr,
                                uint8_t higherAddr)
{
    DisplaySetCommand();    
    DRV_GFX_PMPWrite(page); 
    DRV_GFX_PMPWrite(lowerAddr); 
    DRV_GFX_PMPWrite(higherAddr);
    DisplaySetData();
}


// *****************************************************************************
/*  Function:
    void DRV_GFX_Initialize(void)

    Summary:
        Initialize the graphics display driver.

    Description:
        This function initializes the graphics display driver. This
        function will be called by the application.

*/
// *****************************************************************************
void DRV_GFX_Initialize(void)
{
    // Initialize the device
    DRV_GFX_PMPInitialize();

    DisplayEnable();
    DisplaySetCommand();

#if defined(USE_GFX_DISPLAY_CONTROLLER_SH1101A) 

    // Setup Display
    DRV_GFX_PMPWrite(0xAE);             // turn off the display (AF=ON, AE=OFF)
    DRV_GFX_PMPWrite(0xDB);             // set  VCOMH
    DRV_GFX_PMPWrite(0x23);
    DRV_GFX_PMPWrite(0xD9);             // set  VP
    DRV_GFX_PMPWrite(0x22);

    // Re-map
    DRV_GFX_PMPWrite(0xA1);             // [A0]:column address 0 is map to SEG0

    // [A1]:column address 131 is map to SEG0
    // COM Output Scan Direction
    DRV_GFX_PMPWrite(0xC8);             // C0 is COM0 to COMn, C8 is COMn to COM0

    // COM Pins Hardware Configuration
    DRV_GFX_PMPWrite(0xDA);             // set pins hardware configuration
    DRV_GFX_PMPWrite(0x12);

    // Multiplex Ratio
    DRV_GFX_PMPWrite(0xA8);             // set multiplex ratio
    DRV_GFX_PMPWrite(0x3F);             // set to 64 mux

    // Display Clock Divide
    DRV_GFX_PMPWrite(0xD5);             // set display clock divide
    DRV_GFX_PMPWrite(0xA0);             // set to 100Hz

    // Contrast Control Register
    DRV_GFX_PMPWrite(0x81);             // Set contrast control
    DRV_GFX_PMPWrite(0x60);             // display 0 ~ 127; 2C

    // Display Offset
    DRV_GFX_PMPWrite(0xD3);             // set display offset
    DRV_GFX_PMPWrite(0x00);             // no offset

    //Normal or Inverse Display
    DRV_GFX_PMPWrite(0xA6);             // Normal display
    DRV_GFX_PMPWrite(0xAD);             // Set DC-DC
    DRV_GFX_PMPWrite(0x8B);             // 8B=ON, 8A=OFF

    // Display ON/OFF
    DRV_GFX_PMPWrite(0xAF);             // AF=ON, AE=OFF
    __delay_ms(150);

    // Entire Display ON/OFF
    DRV_GFX_PMPWrite(0xA4);             // A4=ON

    // Display Start Line
    DRV_GFX_PMPWrite(0x40);             // Set display start line

    // Lower Column Address
    DRV_GFX_PMPWrite(0x00 + DRV_GFX_OFFSET);    // Set lower column address

    // Higher Column Address
    DRV_GFX_PMPWrite(0x10);             // Set higher column address
    __delay_ms(1);

#elif defined (USE_GFX_DISPLAY_CONTROLLER_SSD1303)

    // Setup Display
    DRV_GFX_PMPWrite(0xAE);             // turn off the display (AF=ON, AE=OFF)
    DRV_GFX_PMPWrite(0xDB);             // set  VCOMH
    DRV_GFX_PMPWrite(0x23);
    DRV_GFX_PMPWrite(0xD9);             // set  VP
    DRV_GFX_PMPWrite(0x22);
    DRV_GFX_PMPWrite(0xAD);             // Set DC-DC
    DRV_GFX_PMPWrite(0x8B);             // 8B=ON, 8A=OFF
    __delay_ms(1);

    // Display ON/OFF
    DRV_GFX_PMPWrite(0xAF);             // AF=ON, AE=OFF

    // Init OLED display using SSD1303 driver
    // Display Clock Divide
    DRV_GFX_PMPWrite(0xD5);             // set display clock divide
    DRV_GFX_PMPWrite(0xA0);             // set to 100Hz

    // Display Offset
    DRV_GFX_PMPWrite(0xD3);             // set display offset
    DRV_GFX_PMPWrite(0x00);             // no offset

    // Multiplex Ratio
    DRV_GFX_PMPWrite(0xA8);             // set multiplex ratio
    DRV_GFX_PMPWrite(0x3F);             // set to 64 mux

    // Display Start Line
    DRV_GFX_PMPWrite(0x40);             // Set display start line

    // Re-map
    DRV_GFX_PMPWrite(0xA0);             // [A0]:column address 0 is map to SEG0

    // [A1]:column address 131 is map to SEG0
    // COM Output Scan Direction
    DRV_GFX_PMPWrite(0xC8);             // C0 is COM0 to COMn, C8 is COMn to COM0

    // COM Pins Hardware Configuration
    DRV_GFX_PMPWrite(0xDA);             // set pins hardware configuration
    DRV_GFX_PMPWrite(0x12);

    // Contrast Control Register
    DRV_GFX_PMPWrite(0x81);             // Set contrast control
    DRV_GFX_PMPWrite(0x60);             // display 0 ~ 127; 2C

    // Entire Display ON/OFF
    DRV_GFX_PMPWrite(0xA4);             // A4=ON

    //Normal or Inverse Display
    DRV_GFX_PMPWrite(0xA6);             // Normal display

#ifdef DISABLE_DC_DC_CONVERTER
    DRV_GFX_PMPWrite(0x8A);
#else
    DRV_GFX_PMPWrite(0x8B);             // 8B=ON, 8A=OFF
#endif

    // Lower Column Address
    DRV_GFX_PMPWrite(0x00 + OFFSET);    // Set lower column address

    // Higher Column Address
    DRV_GFX_PMPWrite(0x10);             // Set higher column address
    __delay_ms(1);
#else
    #error The controller is not supported.
#endif

    DisplayDisable();
    DisplaySetData();
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_PixelPut(
                    uint16_t    x,
                    uint16_t    y)

    Summary:
        Draw the pixel on the given position.

    Description:
        This routine draws the pixel on the given position.
        The color used is the color set by the last call to
        GFX_ColorSet().

        If position is not on the frame buffer, then the behavior
        is undefined. If color is not set, before this function
        is called, the output is undefined.

*/
// *****************************************************************************
GFX_STATUS GFX_PixelPut(uint16_t x, uint16_t y)
{
    uint8_t    page, add, lAddr, hAddr;
    uint8_t    mask, display;

    // Assign a page address
    if(y < 8)
        page = 0xB0;
    else if(y < 16)
        page = 0xB1;
    else if(y < 24)
        page = 0xB2;
    else if(y < 32)
        page = 0xB3;
    else if(y < 40)
        page = 0xB4;
    else if(y < 48)
        page = 0xB5;
    else if(y < 56)
        page = 0xB6;
    else
        page = 0xB7;

    add = x + DRV_GFX_OFFSET;
    lAddr = 0x0F & add;                         // Low address
    hAddr = 0x10 | (add >> 4);                  // High address

    // Calculate mask from rows basically do a y%8 and remainder is bit position
    add = y >> 3;                               // Divide by 8
    add <<= 3;                                  // Multiply by 8
    add = y - add;                              // Calculate bit position
    mask = 1 << add;                            // Left shift 1 by bit position

    DisplayEnable();

    DRV_GFX_AddressSet(page, lAddr, hAddr);     // Set the address (sets the page,
                                                // lower and higher column address pointers)

    display = DRV_GFX_PMPSingleRead();          // Read to initiate Read transaction on PMP and dummy read
                                                // (requirement for data synchronization in the controller)
    display = DRV_GFX_PMPSingleRead();          // Read again as a requirement for data synchronization in the display controller
    display = DRV_GFX_PMPSingleRead();          // Read actual data from from display buffer

    if(GFX_ColorGet() > 0)                      // If non-zero for pixel on
        display |= mask;                        // or in mask
    else
        // If 0 for pixel off
        display &= ~mask;                       // and with inverted mask

    DRV_GFX_AddressSet(page, lAddr, hAddr);     // Set the address (sets the page,

    // lower and higher column address pointers)
    DRV_GFX_PMPWrite(display);              // restore the uint8_t with manipulated bit
    DisplayDisable();

    return (GFX_STATUS_SUCCESS);

}

// *****************************************************************************
/*  Function:
    GFX_COLOR GFX_PixelGet(
                    uint16_t    x,
                    uint16_t    y)

    Summary:
        Gets color of the pixel on the given position.

    Description:
        This routine gets the pixel on the given position.

        If position is not on the frame buffer, then the behavior
        is undefined.

*/
// *****************************************************************************
GFX_COLOR GFX_PixelGet(uint16_t x, uint16_t y)
{
    uint8_t    page, add, lAddr, hAddr;
    uint8_t    mask, temp, display;

    // Assign a page address
    if(y < 8)
        page = 0xB0;
    else if(y < 16)
        page = 0xB1;
    else if(y < 24)
        page = 0xB2;
    else if(y < 32)
        page = 0xB3;
    else if(y < 40)
        page = 0xB4;
    else if(y < 48)
        page = 0xB5;
    else if(y < 56)
        page = 0xB6;
    else
        page = 0xB7;

    add = x + DRV_GFX_OFFSET;
    lAddr = 0x0F & add;                 // Low address
    hAddr = 0x10 | (add >> 4);          // High address

    // Calculate mask from rows basically do a y%8 and remainder is bit position
    temp = y >> 3;                      // Divide by 8
    temp <<= 3;                         // Multiply by 8
    temp = y - temp;                    // Calculate bit position
    mask = 1 << temp;                   // Left shift 1 by bit position
    DisplayEnable();

    DRV_GFX_AddressSet(page, lAddr, hAddr); // Set the address (sets the page,

    // lower and higher column address pointers)
    display = DRV_GFX_PMPSingleRead();	// Read to initiate Read transaction on PMP
                                        // Dummy Read (requirement for data
                                        // synchronization in the controller)
    display = DRV_GFX_PMP16BitRead();   // Read data from display buffer
    DisplayDisable();

    return (display & mask);            // mask all other bits and return
                                        // the result
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_ScreenClear(void)

    Summary:
        Clears the screen to the currently set color (GFX_ColorSet()).

    Description:
        This function clears the screen with the current color and sets
        the line cursor position to (0, 0).

        If color is not set, before this function is called, the output
        is undefined.

        If the function returns GFX_STATUS_FAILURE, clearing is not
        yet finished. Application must call the function again to continue
        the clearing.

*/
// *****************************************************************************
GFX_STATUS GFX_ScreenClear(void)
{
    uint8_t    i, j;
    GFX_COLOR  color;

    if (GFX_ColorGet() == 0)
        color = 0x00;
    else
        color = 0xFF;

    DisplayEnable();

    for(i = 0xB0; i < 0xB8; i++)
    {
        // Go through all 8 pages
        DRV_GFX_AddressSet(i, 0x00, 0x10);
        for(j = 0; j < 132; j++)
        {   // Write to all 132 bytes
            DRV_GFX_PMPWrite(color);
        }
    }

    DisplayDisable();
    GFX_LinePositionSet(0, 0);
    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_RenderStatusGet()

    Summary:
        This function returns the driver's status on rendering.

    Description:
        The controller has no hardware accelerated rendering routines.
        Therefore, this function will always return GFX_STATUS_READY_BIT.
        This means that the driver's rendering status is always ready.
        
        Valid values returned are GFX_STATUS_BUSY_BIT or
        GFX_STATUS_READY_BIT.

*/
// *****************************************************************************
GFX_STATUS_BIT GFX_RenderStatusGet(void)
{
    return GFX_STATUS_READY_BIT;
}


#endif // #if defined(USE_GFX_DISPLAY_CONTROLLER_SH1101A) || defined (USE_GFX_DISPLAY_CONTROLLER_SSD1303)

