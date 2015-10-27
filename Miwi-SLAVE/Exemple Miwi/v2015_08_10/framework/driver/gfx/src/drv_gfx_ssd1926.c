/*******************************************************************************
 Display Driver for Microchip Graphics Library - Display Driver Layer

  Company:
    Microchip Technology Inc.

  File Name:
    drv_gfx_ssd1926.c

  Summary:
    Display Driver for use with the Microchip Graphics Library.

  Description:
    This module implements the display driver for the following controllers:
    *  Solomon Systech. SSD1926 
    This module implements the basic Display Driver Layer API required by the 
    Microchip Graphics Library to enable, initialize and render pixels 
    to the display controller. 
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

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <stdint.h>
#include "system_config.h"
#include "system.h"

#if defined(GFX_USE_DISPLAY_CONTROLLER_SSD1926)

#include <libpic30.h>                        
#include "driver/gfx/drv_gfx_display.h"      
#include "driver/gfx/drv_gfx_ssd1926.h"
#include "driver/gfx/drv_gfx_tcon.h"
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
void        DRV_SSD1926_RegisterSet(uint16_t index, uint8_t value);
uint8_t     DRV_SSD1926_RegisterGet(uint16_t index);
void        DRV_SSD1926_AddressSet(uint32_t address);

void        DRV_SSD1926_IOPortWriteData(uint8_t value);
void        DRV_SSD1926_IOPortSet(uint8_t mask, uint8_t level);
void        DRV_SSD1926_IOPortInit();
GFX_STATUS  DRV_SSD1926_Bar2DDraw(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom);
GFX_STATUS  DRV_SSD1926_BarAlpha2DDraw(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom);
GFX_STATUS  DRV_SSD1926_Line2DDraw(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

// *****************************************************************************
// *****************************************************************************
// Section: Helper Macros and Functions
// *****************************************************************************
// *****************************************************************************

#define RED8(color16)   (uint8_t) ((color16 & 0xF800) >> 8)
#define GREEN8(color16) (uint8_t) ((color16 & 0x07E0) >> 3)
#define BLUE8(color16)  (uint8_t) ((color16 & 0x001F) << 3)

// *****************************************************************************
/*  Function:
    void DRV_GFX_PixelWrite(GFX_COLOR color)

    Summary:
        This function is a driver level pixel write function.

    Description:
        This function is a driver level pixel write function written at
        the currently set address. Address is set using DRV_SSD1926_AddressSet().

*/
// *****************************************************************************
#ifdef USE_16BIT_PMP
#define DRV_GFX_PixelWrite(color)	DRV_GFX_PMPWrite(color)
#else
inline void __attribute__((always_inline)) DRV_GFX_PixelWrite(uint16_t color)
{
    DRV_GFX_PMPWrite(((DRIVER_UINT16_UNION)color).uint168BitValue[1]);
    DRV_GFX_PMPWrite(((DRIVER_UINT16_UNION)color).uint168BitValue[0]);

}
#endif

// *****************************************************************************
/*  Function:
    void DRV_SSD1926_AddressSet(uint32_t address)

    Summary:
        This function is a driver level address set function.

    Description:
        This function is a driver level address set function. This
        function is used for writing or reading a pixel from the display
        controller.

*/
// *****************************************************************************
void DRV_SSD1926_AddressSet(uint32_t address)
{
#ifdef USE_16BIT_PMP

    DRIVER_UINT16_UNION temp;

    DisplaySetCommand(); // set RS line to low for command

    temp.uint168BitValue[0] = ((DRIVER_UINT32_UNION) address).uint328BitValue[1];
    temp.uint168BitValue[1] = ((DRIVER_UINT32_UNION) address).uint328BitValue[2] | 0x80;
    DRV_GFX_PMPWrite(temp.uint16Value);

    temp.uint168BitValue[0] = 0x01;
    temp.uint168BitValue[1] = ((DRIVER_UINT32_UNION) address).uint328BitValue[0];
    DRV_GFX_PMPWrite(temp.uint16Value);

    DisplaySetData();    // set RS line to high for data

#else

    DisplaySetCommand(); // set RS line to low for command

    DRV_GFX_PMPWrite(((DRIVER_UINT32_UNION) address).uint328BitValue[2] | 0x80);
    DRV_GFX_PMPWrite(((DRIVER_UINT32_UNION) address).uint328BitValue[1]);
    DRV_GFX_PMPWrite(((DRIVER_UINT32_UNION) address).uint328BitValue[0]);

    DisplaySetData();    // set RS line to high for data

#endif
}

// *****************************************************************************
/*  Function:
    void DRV_SSD1926_RegisterSet(uint16_t index, uint8_t value)

    Summary:
        This function is a driver level register write function.

    Description:
        This function is a driver level register write function. This
        function is used for writing commands or initializing registers
        of the display controller.

*/
// *****************************************************************************
void DRV_SSD1926_RegisterSet(uint16_t index, uint8_t value)
{
#ifdef USE_16BIT_PMP

    DisplaySetCommand(); // set RS line to low for command
    DisplayEnable();     // enable SSD1926

    DRV_GFX_PMPWrite(((DRIVER_UINT16_UNION)index).uint168BitValue[1]);
    DRV_GFX_PMPWrite(index << 8);

    DisplaySetData();    // set RS line to high for data

    if(index & 0x0001)
        DRV_GFX_PMPWrite(value);
    else
        DRV_GFX_PMPWrite(value << 8);

    DisplayDisable();   // disable SSD1926

#else

    DisplaySetCommand(); // set RS line to low for command
    DisplayEnable();     // enable SSD1926

    DRV_GFX_PMPWrite(0x00);    // register access
    DRV_GFX_PMPWrite(((DRIVER_UINT16_UNION)index).uint168BitValue[1]);
    DRV_GFX_PMPWrite(((DRIVER_UINT16_UNION)index).uint168BitValue[0]);

    DisplaySetData();    // set RS line to high for data
    DRV_GFX_PMPWrite(value);

    DisplayDisable();   // disable SSD1926
#endif
}
// *****************************************************************************
/*  Function:
    uint8_t DRV_SSD1926_RegisterGet(uint16_t index)

    Summary:
        This function is a driver level register read function.

    Description:
        This function is a driver level register read function. This
        function is used for reading status or register contents of
        the display controller.

*/
// *****************************************************************************
uint8_t DRV_SSD1926_RegisterGet(uint16_t index)
{
#ifdef USE_16BIT_PMP

    uint16_t    value;

    DisplaySetCommand(); // set RS line to low for command
    DisplayEnable();     // enable SSD1926

    DRV_GFX_PMPWrite(((DRIVER_UINT16_UNION)index).uint168BitValue[1]);
    DRV_GFX_PMPWrite(index << 8);

    DisplaySetData();    // set RS line to high for data

    value = DRV_GFX_PMPRead();
    value = DRV_GFX_PMPRead();

    DisplayDisable();   // disable SSD1926

    if(index & 0x0001)
        value &= 0x00ff;
    else
        value = (value >> 8) & 0x00ff;

#else
    uint8_t   value;

    DisplaySetCommand(); // set RS line to low for command
    DisplayEnable();     // enable SSD1926

    DRV_GFX_PMPWrite(0x00);    // register access
    DRV_GFX_PMPWrite(((DRIVER_UINT16_UNION)index).uint168BitValue[1]);
    DRV_GFX_PMPWrite(((DRIVER_UINT16_UNION)index).uint168BitValue[0]);

    DisplaySetData();    // set RS line to high for data

    value = DRV_GFX_PMPRead();
    value = DRV_GFX_PMPRead();

    DisplayDisable();   // disable SSD1926

#endif
    return (value);
}

// *****************************************************************************
/*  Function:
    void DRV_GFX_DisplayBrightness(uint16_t level)

    Summary:
        This function sets the display brightness.

    Description:
        This function sets the display brightness. The levels are
        0-100 where 0 means that the display is turned off and 100
        means that the display is at its brightest mode.
        This usually controls the backlight of the display.

        In cases where the backlight is hooked up to a PWM signal
        the level will determine the frequency and period of the PWM signal.
        In cases there control is only on or off, 0 means off and
        level greater than 0 means on.


*/
// *****************************************************************************
void DRV_GFX_DisplayBrightness(uint16_t level)
{
    // If the brightness can be controlled (for example through PWM)
    // add code that will control the PWM here.

    if (level > 0)
    {
        DisplayBacklightOn();           
    }    
    else if (level == 0)
    {
        DisplayBacklightOff();
    }    
        
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
    /* 
        This is the reset routine of the display controller. 
        DRV_GFX_Initialize() recommended sequence:
        1. Initialize the interface to the controller  - DRV_GFX_PMPInitialize()
        2. Initialize the registers of the controller make the enabling of 
           the display control last. In other words, the sync signals 
           are enabled only after all the registers that needs to be set 
           for the synchronization control are initialized.
        3. Before, the display is turned on, initialize the display buffer 
           to a single color so you will not see a snowy image on the screen. 
           This is done here using:
              GFX_ColorSet(0);
              ClearDevice();
           where 0 is the black color.
        4. Enable the display controller refresh of the screen
           This is done here using:
              DRV_SSD1926_RegisterSet(REG_POWER_SAVE_CONFIG, 0x00);    //  wake up
        5. Initialize the Timing Controller (TCON) if needed.      
        6. Enable the Backlight 
        
        Most display panels will have the power-up sequence:
        1. Enable Analog Power Supply
        2. Enable Digital Power Supply - to start synchronization signals
        3. Enable Backlight - to show the contents of the display buffer
        For power-down, the reverse is followed.
        
    */

    /////////////////////////////////////////////////////////////////////
    // Initialize the device
    /////////////////////////////////////////////////////////////////////
    DRV_GFX_PMPInitialize();

    // give time for the controller to power up
    __delay_ms(500);

    /////////////////////////////////////////////////////////////////////
    // PLL SETUP
    // Crystal frequency x M / N = 80 MHz
    // for 4 MHz crystal:
    /////////////////////////////////////////////////////////////////////
    DRV_SSD1926_RegisterSet(REG_PLL_CONFIG_0, 0x0a);         // set N = 10
    DRV_SSD1926_RegisterSet(REG_PLL_CONFIG_1, 0xc8);         // set M = 200
    DRV_SSD1926_RegisterSet(REG_PLL_CONFIG_2, 0xae);         // must be programmed to 0xAE
    DRV_SSD1926_RegisterSet(REG_PLL_CONFIG_0, 0x8a);         // enable PLL
    
    /////////////////////////////////////////////////////////////////////
    // VIDEO BUFFER MEMORY CLOCK SETUP
    // Memory frequency = PLL frequency / (MCLK + 1)
    /////////////////////////////////////////////////////////////////////
    DRV_SSD1926_RegisterSet(REG_MEMCLK_CONFIG, 0x00);        // set MCLK = 0 (80 MHz)
    
    /////////////////////////////////////////////////////////////////////

    // PIXEL OUTPUT CLOCK SETUP (LCD_SHIFT SIGNAL)
    // Pixel clock = Memory frequency * (PCLK + 1) / 0x100000
    /////////////////////////////////////////////////////////////////////
    DRV_SSD1926_RegisterSet(REG_PCLK_FREQ_RATIO_0, 0x00);    // set PCLK = 0x020000
    DRV_SSD1926_RegisterSet(REG_PCLK_FREQ_RATIO_1, 0x00);    // Pixel clock = 10 MHz
    DRV_SSD1926_RegisterSet(REG_PCLK_FREQ_RATIO_2, 0x02);

    /////////////////////////////////////////////////////////////////////
    // Panel Configuration (reg 10h)
    // TFT display with 18 bit or 24-bit RGB parallel interface.
    /////////////////////////////////////////////////////////////////////
    uint8_t panelType  = 0; 
    uint8_t panelWidth = 0; 

#if (DISP_DATA_WIDTH == 18)
    panelWidth |= 0x20;
#elif (DISP_DATA_WIDTH == 24)
    panelWidth |= 0x30;
#else
#error "Define DISP_DATA_WIDTH in system_config.h (valid values: 18, 24)"
#endif

#if (GFX_LCD_TYPE == GFX_LCD_TFT)
    panelType |= 0x41;
#elif (GFX_LCD_TYPE == GFX_LCD_CSTN)
    panelType |= 0x40;
#elif (GFX_LCD_TYPE == GFX_LCD_MSTN)
    panelType |= 0x00;
#else
#error "Define GFX_LCD_TYPE in system_config.h (valid values: GFX_LCD_TFT, GFX_LCD_CSTN, GFX_LCD_MSTN)"
#endif

    DRV_SSD1926_RegisterSet(REG_PANEL_TYPE, panelType|panelWidth);

    /////////////////////////////////////////////////////////////////////
    // Horizontal total HT (reg 12h)
    /////////////////////////////////////////////////////////////////////
    #define HT  (DISP_HOR_PULSE_WIDTH + DISP_HOR_BACK_PORCH + DISP_HOR_FRONT_PORCH + DISP_HOR_RESOLUTION)
    DRV_SSD1926_RegisterSet(REG_HORIZ_TOTAL_0, HT / 8);
    DRV_SSD1926_RegisterSet(REG_HORIZ_TOTAL_1, HT % 8);

    /////////////////////////////////////////////////////////////////////
    // Horizontal display period HDP (reg 14h)
    /////////////////////////////////////////////////////////////////////
    DRV_SSD1926_RegisterSet(REG_HDP, DISP_HOR_RESOLUTION / 8 - 1);

    /////////////////////////////////////////////////////////////////////
    // Horizontal display period start HDPS (regs 16h, 17h)
    /////////////////////////////////////////////////////////////////////
    #define HDPS (DISP_HOR_PULSE_WIDTH + DISP_HOR_BACK_PORCH)
    DRV_SSD1926_RegisterSet(REG_HDP_START_POS0, HDPS & 0x00FF);
    DRV_SSD1926_RegisterSet(REG_HDP_START_POS1, (HDPS >> 8) & 0x00FF);

    /////////////////////////////////////////////////////////////////////
    // Horizontal syncronization pulse width HPW (reg 20h)
    /////////////////////////////////////////////////////////////////////
    DRV_SSD1926_RegisterSet(REG_HSYNC_PULSE_WIDTH, DISP_HOR_PULSE_WIDTH - 1);

    /////////////////////////////////////////////////////////////////////
    // Vertical total VT (regs 18h, 19h)
    /////////////////////////////////////////////////////////////////////
    #define VT  (DISP_VER_PULSE_WIDTH + DISP_VER_BACK_PORCH + DISP_VER_FRONT_PORCH + DISP_VER_RESOLUTION)
    DRV_SSD1926_RegisterSet(REG_VERT_TOTAL0, VT & 0x00FF);
    DRV_SSD1926_RegisterSet(REG_VERT_TOTAL1, (VT >> 8) & 0x00FF);

    /////////////////////////////////////////////////////////////////////
    // Vertical display period VDP (regs 1ch, 1dh)
    /////////////////////////////////////////////////////////////////////
    #define VDP (DISP_VER_RESOLUTION - 1)
    DRV_SSD1926_RegisterSet(REG_VDP0, VDP & 0x00FF);
    DRV_SSD1926_RegisterSet(REG_VDP1, (VDP >> 8) & 0x00FF);

    /////////////////////////////////////////////////////////////////////
    // Vertical display period start VDPS (regs 1eh, 1fh)
    /////////////////////////////////////////////////////////////////////
    #define VDPS  (DISP_VER_PULSE_WIDTH + DISP_VER_BACK_PORCH)
    DRV_SSD1926_RegisterSet(REG_VDP_START_POS0, VDPS & 0x00FF);
    DRV_SSD1926_RegisterSet(REG_VDP_START_POS1, (VDPS >> 8) & 0x00FF);

    /////////////////////////////////////////////////////////////////////
    // Vertical syncronization pulse width VPW (reg 24h)
    /////////////////////////////////////////////////////////////////////
    DRV_SSD1926_RegisterSet(REG_VSYNC_PULSE_WIDTH, DISP_VER_PULSE_WIDTH - 1);

    /////////////////////////////////////////////////////////////////////
    // ROTATION MODE
#if (DISP_ORIENTATION == 0)
    #define WIN_START_ADDR  0ul
    #define ROTATION    0

#elif (DISP_ORIENTATION == 90)
    #define WIN_START_ADDR  ((((uint32_t) GFX_MaxXGet() + 1) >> 1) - 1)
    #define ROTATION    1

#elif (DISP_ORIENTATION == 180)
    #define WIN_START_ADDR  (((((uint32_t) GFX_MaxXGet() + 1) * (GFX_MaxYGet() + 1)) >> 1) - 1)
    #define ROTATION    2

#elif (DISP_ORIENTATION == 270)
    #define WIN_START_ADDR  ((((uint32_t) GFX_MaxXGet() + 1) * GFX_MaxYGet()) >> 1)
    #define ROTATION    3
#endif

    /////////////////////////////////////////////////////////////////////
    // Special Effects Register (reg 71h)
    /////////////////////////////////////////////////////////////////////
    DRV_SSD1926_RegisterSet(REG_SPECIAL_EFFECTS, 0x40 | ROTATION);
    
    /////////////////////////////////////////////////////////////////////
    // Main Window Display Start Address (regs 74h, 75h, 76h)
    /////////////////////////////////////////////////////////////////////
    DRV_SSD1926_RegisterSet(REG_MAIN_WIN_DISP_START_ADDR0, (WIN_START_ADDR      ) & 0x00FF);
    DRV_SSD1926_RegisterSet(REG_MAIN_WIN_DISP_START_ADDR1, (WIN_START_ADDR >> 8 ) & 0x00FF);
    DRV_SSD1926_RegisterSet(REG_MAIN_WIN_DISP_START_ADDR2, (WIN_START_ADDR >> 16) & 0x00FF);

    /////////////////////////////////////////////////////////////////////
    // Main Window Display Offset (regs 78h, 79h)
    /////////////////////////////////////////////////////////////////////
    #define WIN_OFFSET  ((GFX_MaxXGet() + 1) >> 1)

    DRV_SSD1926_RegisterSet(REG_MAIN_WIN_ADDR_OFFSET0, (WIN_OFFSET     ) & 0x00FF);
    DRV_SSD1926_RegisterSet(REG_MAIN_WIN_ADDR_OFFSET1, (WIN_OFFSET >> 8) & 0x00FF);

    /////////////////////////////////////////////////////////////////////
    // Display Mode Register (reg 70h)
    /////////////////////////////////////////////////////////////////////
    DRV_SSD1926_RegisterSet(REG_DISPLAY_MODE, 0x04);         // 16 BPP, enable RAM content to screen
    
    /////////////////////////////////////////////////////////////////////
    // RGB Settings Register (reg 1a4h)
    /////////////////////////////////////////////////////////////////////
    DRV_SSD1926_RegisterSet(REG_RGB_SETTING, 0xc0);          // RGB format

    /////////////////////////////////////////////////////////////////////
    // LSHIFT Polarity Register (reg 38h)
    /////////////////////////////////////////////////////////////////////
#ifdef DISP_INV_LSHIFT
    DRV_SSD1926_RegisterSet(REG_LSHIFT_POLARITY, 0x01); // 1 means falling trigger
#endif

    /////////////////////////////////////////////////////////////////////
    // 2D Register Initialization
    // These registers are not changed unless 2D routines are
    // modified to use brush width and height as well as use of
    // other 2D registers.
    /////////////////////////////////////////////////////////////////////
    /* Brush width */
    DRV_SSD1926_RegisterSet(REG_2D_214, 1);
    DRV_SSD1926_RegisterSet(REG_2D_215, 0);

    /* Brush height */
    DRV_SSD1926_RegisterSet(REG_2D_218, 1);
    DRV_SSD1926_RegisterSet(REG_2D_219, 0);

    /* Display width */
    DRV_SSD1926_RegisterSet(REG_2D_1f8, ((GFX_MaxXGet() + 1)     ) & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1f9, ((GFX_MaxXGet() + 1) >> 8) & 0xFF);

    /* Display 2d width */
    DRV_SSD1926_RegisterSet(REG_2D_1d8, ((GFX_MaxXGet() + 1)     ) & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1d9, ((GFX_MaxXGet() + 1) >> 8) & 0xFF);

    /////////////////////////////////////////////////////////////////////
    // Clear the display buffer with all zeros so the display will not
    // show garbage data when initially powered up
    /////////////////////////////////////////////////////////////////////
    GFX_ColorSet(0);
    GFX_ScreenClear();
    
    /////////////////////////////////////////////////////////////////////
    // Power Saving Configuration Register (reg a0h)
    /////////////////////////////////////////////////////////////////////
    DRV_SSD1926_RegisterSet(REG_POWER_SAVE_CONFIG, 0x00);    //  wake up
    
    // check until the controller is really awake
    while(DRV_SSD1926_RegisterGet(REG_POWER_SAVE_CONFIG) == 0x00);
    
    /////////////////////////////////////////////////////////////////////
    // LCD Power Control Register (reg adh)
    // If LCD_POWER is connected to the glass DISPON or RESET
    /////////////////////////////////////////////////////////////////////
    DRV_SSD1926_RegisterSet(REG_GPIO_STATUS_CONTROL1, 0x80); // release the glass from reset
    
    /////////////////////////////////////////////////////////////////////
    // Panel TCON Programming - set up the TCON first before turning
    // on the display. 
    /////////////////////////////////////////////////////////////////////
#ifdef USE_TCON_MODULE
    DRV_SSD1926_IOPortInit();
    DRV_TCON_Initialize(TCONWrite);
#endif
    
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
    uint32_t   address;

    address = (((uint32_t) (GFX_MaxXGet() + 1)) * y + x) << 1;

    DisplayEnable();      // enable SSD1926
    DRV_SSD1926_AddressSet(address);
    DRV_GFX_PixelWrite(GFX_ColorGet());
    DisplayDisable();    // disable SSD1926

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
    uint32_t    address;
    DRIVER_UINT16_UNION value;

    address = (((uint32_t) (GFX_MaxXGet() + 1)) * y + x) << 1;
    DisplayEnable();
    DRV_SSD1926_AddressSet(address);

#if defined(USE_GFX_PMP)
#ifdef USE_8BIT_PMP

    // this first two reads are a dummy reads
    ((DRIVER_UINT16_UNION)value).uint168BitValue[1] = DRV_GFX_PMPSingleRead();
    ((DRIVER_UINT16_UNION)value).uint168BitValue[0] = DRV_GFX_PMPSingleRead();

    // these are the reads that will get the actual pixel data
    ((DRIVER_UINT16_UNION)value).uint168BitValue[1] = DRV_GFX_PMPSingleRead();
    ((DRIVER_UINT16_UNION)value).uint168BitValue[0] = DRV_GFX_PMPSingleRead();

#endif 
#ifdef USE_16BIT_PMP

    value = DRV_GFX_PMPRead();
    value = DRV_GFX_PMPRead();

#endif 
#elif defined (USE_GFX_EPMP)
#ifdef USE_8BIT_PMP

    // this first two reads are a dummy reads
    ((DRIVER_UINT16_UNION)value).uint168BitValue[1] = DRV_GFX_PMPSingleRead();
    ((DRIVER_UINT16_UNION)value).uint168BitValue[0] = DRV_GFX_PMPSingleRead();

    // these are the reads that will get the actual pixel data
    ((DRIVER_UINT16_UNION)value).uint168BitValue[1] = DRV_GFX_PMPSingleRead();
    ((DRIVER_UINT16_UNION)value).uint168BitValue[0] = DRV_GFX_PMPSingleRead();

#endif 
#ifdef USE_16BIT_PMP
    value.uint16Value = DRV_GFX_PMPRead();
    value.uint16Value = DRV_GFX_PMPRead();
    value.uint16Value = DRV_GFX_PMPRead();
#endif 
#endif

    DisplayDisable();
    return ((GFX_COLOR)value.uint16Value);
}

// *****************************************************************************
/*  Function:
    uint16_t GFX_PixelArrayPut(
                                uint16_t x,
                                uint16_t y,
                                GFX_COLOR *pPixel,
                                uint16_t numPixels)

    Summary:
        Renders an array of pixels to the frame buffer.

    Description:
        This renders an array of pixels starting from the
        location defined by x and y with the length
        defined by numPixels. The rendering will be performed
        in the increasing x direction. If the length is more
        than 1 line of the screen, the rendering may continue
        to the next line depending on the way the memory of
        the display controller is arranged.
        For example, in some display controller, if the parameters are
           GFX_PixelArrayPut(0, 0, ptrToArray, (320*240));
        The array is rendered on all the pixels of a QVGA screen.

        This function also supports transparent color feature.
        When the feature is enabled the pixel with the transparent
        color will not be rendered and will be skipped.
        x and y must define a location within the display
        buffer.

*/
// *****************************************************************************
uint16_t GFX_PixelArrayPut(
                                uint16_t x,
                                uint16_t y,
                                GFX_COLOR *pPixel,
                                uint16_t numPixels)
{
    uint16_t    z = 0;
    uint16_t    newLeft, newTop, newRight, newBottom;
    uint32_t    address;

    newLeft   = x;
    newTop    = y;
    newRight  = x + numPixels - 1;
    newBottom = y;

    // Set start address
    address = ((uint32_t) (GFX_MaxXGet() + 1) * newTop + newLeft) << 1;

    DisplayEnable();      // enable SSD1926
    DRV_SSD1926_AddressSet(address);

#ifndef GFX_CONFIG_TRANSPARENT_COLOR_DISABLE
    if (GFX_TransparentColorStatusGet() == GFX_FEATURE_ENABLED)
    {
        for(z = 0; z < numPixels; z++)
        {
            // Write pixel to screen
            // to skip the pixel location a read is performed so the
            // internal address counter in the SSD1926 is incremented.
            // If this read is not done a new DRV_SSD1926_AddressSet() call
            // must be performed to write to the correct pixel location.
            if (GFX_TransparentColorGet() == *pPixel)
            {
#if defined (USE_GFX_PMP)
                DRV_GFX_PMP16BitRead();
#elif defined (USE_GFX_EPMP)
                DRV_GFX_PMPRead();
#endif
            }
            else
            {
                DRV_GFX_PixelWrite(*pPixel);
            }
            pPixel++;
        }
    }
    else
#endif
    {
        for(z = 0; z < numPixels; z++)
        {
            // Write pixel to screen
            DRV_GFX_PixelWrite(*pPixel);
            pPixel++;
        }
    }
    DisplayDisable();
    return z;
}


// *****************************************************************************
/*  Function:
    uint16_t GFX_PixelArrayGet(
                                uint16_t x,
                                uint16_t y,
                                GFX_COLOR *pPixel,
                                uint16_t numPixels)

    Summary:
        Renders an array of pixels to the frame buffer.

    Description:
        This retrieves an array of pixels from the display buffer
        starting from the location defined by x and y with
        the length defined by numPixels. All the retrieved pixels
        must be inside the display buffer.

        The function behavoir will be undefined is one of the
        following is true:
        1. x and y location is outside the frame buffer.
        2. x + numPixels exceeds the frame buffer.
        3. Depending on how the frame buffer memory is arranged
           x + numPixels exceeds the width of the frame buffer.

*/
// *****************************************************************************
uint16_t GFX_PixelArrayGet(
                                uint16_t x,
                                uint16_t y,
                                GFX_COLOR *pPixel,
                                uint16_t numPixels)
{

    uint32_t            address;
    uint16_t            z;
    DRIVER_UINT16_UNION value;

    address = (((uint32_t) (GFX_MaxXGet() + 1)) * y + x) << 1;

    DisplayEnable();
    DRV_SSD1926_AddressSet(address);


    // dummy reads
#if defined(USE_GFX_PMP)
#ifdef USE_8BIT_PMP
        // this first two reads are a dummy reads
        value.uint168BitValue[1] = DRV_GFX_PMPSingleRead();
        value.uint168BitValue[0] = DRV_GFX_PMPSingleRead();
#else
        // dummy read
        *pPixel = DRV_GFX_PMPRead();
#endif
#endif

#if defined(USE_GFX_EPMP)
        // dummy read
        *pPixel = DRV_GFX_PMPRead();
#endif

    for(z = 0; z < numPixels; z++)
    {

#ifdef USE_16BIT_PMP
        *pPixel = DRV_GFX_PMPRead();

#else

#if defined(USE_GFX_PMP)
        // these are the reads that will get the actual pixel data
        value.uint168BitValue[1] = DRV_GFX_PMPSingleRead();
        value.uint168BitValue[0] = DRV_GFX_PMPSingleRead();
#endif
    
#if defined(USE_GFX_EPMP)
        value.uint168BitValue[1] = DRV_GFX_PMPRead();
        value.uint168BitValue[0] = DRV_GFX_PMPRead();
#endif
        *pPixel = value.uint16Value;
#endif

        pPixel++;
    }
    DisplayDisable();
    return (z);

}

// *****************************************************************************
/*  Function:
    GFX_STATUS  DRV_SSD1926_Line2DDraw(
                                uint16_t x1,
                                uint16_t y1,
                                uint16_t x2,
                                uint16_t y2)

    Summary:
        This function renders a solid line using the accelerated feature
        of the display driver.

    Description:
        This function renders a solid line shape using the accelerated
        feature of the display driver with the currently set color.

*/
// *****************************************************************************
GFX_STATUS  __attribute__ ((always_inline)) DRV_SSD1926_Line2DDraw(

                                uint16_t x1,
                                uint16_t y1,
                                uint16_t x2,
                                uint16_t y2)
{
#ifdef GFX_CONFIG_NONBLOCKING_DISABLE
    while(GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT);
#else
    if(GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
        return (GFX_STATUS_FAILURE);
#endif

    /* Line Boundaries */
    DRV_SSD1926_RegisterSet(REG_2D_1e4, x1 & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1e5, (x1 >> 8) & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1e8, y1 & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1e9, (y1 >> 8) & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1ec, x2 & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1ed, (x2 >> 8) & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1f0, y2 & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1f1, (y2 >> 8) & 0xFF);

    /* Source & Destination Window Start Addresses */
    DRV_SSD1926_RegisterSet(REG_2D_1d4, 0);
    DRV_SSD1926_RegisterSet(REG_2D_1d5, 0);
    DRV_SSD1926_RegisterSet(REG_2D_1d6, 0);
    DRV_SSD1926_RegisterSet(REG_2D_1f4, 0);
    DRV_SSD1926_RegisterSet(REG_2D_1f5, 0);
    DRV_SSD1926_RegisterSet(REG_2D_1f6, 0);

    /*
      The following are initialized at the initialization routine
      to save time in rendering DRV_SSD1926_Line2DDraw(). If 2D 
      acceleration features that modify these registers are enabled,
      these registers might need to be programmed here.
     */
#if 0
    /* Display width */
    DRV_SSD1926_RegisterSet(REG_2D_1f8, (GFX_MaxXGet() + 1) & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1f9, ((GFX_MaxXGet() + 1) >> 8) & 0xFF);

    /* Display 2d width */
    DRV_SSD1926_RegisterSet(REG_2D_1d8, (GFX_MaxYGet() + 1) & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1d9, ((GFX_MaxYGet() + 1) >> 8) & 0xFF);
#endif

    /* Set Color */
    DRV_SSD1926_RegisterSet(REG_2D_1fe,   RED8(GFX_ColorGet()));
    DRV_SSD1926_RegisterSet(REG_2D_1fd, GREEN8(GFX_ColorGet()));
    DRV_SSD1926_RegisterSet(REG_2D_1fc,  BLUE8(GFX_ColorGet()));

    /* 16bpp */
    DRV_SSD1926_RegisterSet(REG_2D_1dd, 0x00);

    /* Line command */
    DRV_SSD1926_RegisterSet(REG_2D_1d1, 0x01);

    /* Draw2d command */
    DRV_SSD1926_RegisterSet(REG_2D_1d2, 0x01);

#ifdef GFX_CONFIG_NONBLOCKING_DISABLE
    while(GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT);
#endif

    return (GFX_STATUS_SUCCESS);

}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_SolidLineDraw(
                    uint16_t x1,
                    uint16_t y1,
                    uint16_t x2,
                    uint16_t y2)

    Summary:
        This function renders the thin and thick solid line.
        This is an internal function and should not be called
        by the application.

    Description:
        This function renders the thin and thick solid lines
        (see GFX_LINE_STYLE).

*/
// *****************************************************************************
GFX_STATUS GFX_SolidLineDraw(   uint16_t x1, uint16_t y1,
                                uint16_t x2, uint16_t y2)
{
    /*
     * NOTE: This function is an internal function and should not
     *       be called by the application.
     */
    
    typedef enum
    {
        GFX_SSD1926_SOLID_LINE_IDLE_STATE = 0x0D90,
        GFX_SSD1926_SOLID_LINE_SINGLE_DRAW_STATE,
        GFX_SSD1926_SOLID_LINE_THICK1_DRAW_STATE,
        GFX_SSD1926_SOLID_LINE_THICK2_DRAW_STATE,
    } GFX_SSD1926_SOLID_LINE_DRAW_STATE;

    static GFX_SSD1926_SOLID_LINE_DRAW_STATE state = GFX_SSD1926_SOLID_LINE_IDLE_STATE;
    static int16_t  deltaX, deltaY;
    static uint16_t thickAdjustX, thickAdjustY, steep;
    static uint16_t lineType, lineThickness;

    while(1)
    {

#ifdef GFX_CONFIG_NONBLOCKING_DISABLE
        while(GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT);
#else
        if(GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
            return (GFX_STATUS_FAILURE);
#endif

        switch (state)
        {
            case GFX_SSD1926_SOLID_LINE_IDLE_STATE:
                // Decide if line is toward x or y axis
                // this will set which way we make the thickness
                // go to x or y direction.
                lineType      = (GFX_LineStyleGet() & GFX_LINE_TYPE_MASK);
                lineThickness = (GFX_LineStyleGet() & GFX_LINE_THICKNESS_MASK);

                if(lineThickness)
                {
                    deltaX = x2 - x1;
                    if(deltaX < 0)
                        deltaX = -deltaX;
                    deltaY = y2 - y1;
                    if(deltaY < 0)
                        deltaY = -deltaY;
                    if(deltaX < deltaY)
                    {
                        steep = 1;
                        thickAdjustX = 1;
                        thickAdjustY = 0;
                    }
                    else
                    {
                        steep = 0;
                        thickAdjustX = 0;
                        thickAdjustY = 1;
                    }
                }

                state = GFX_SSD1926_SOLID_LINE_SINGLE_DRAW_STATE;
                // no break here since the next state is the one that we want

            case GFX_SSD1926_SOLID_LINE_SINGLE_DRAW_STATE:

                // draw the line
                if(DRV_SSD1926_Line2DDraw(x1, y1, x2, y2) == 
                        GFX_STATUS_FAILURE)
                {
                    return (GFX_STATUS_FAILURE);
                }
                if (lineThickness)
                {
                    state = GFX_SSD1926_SOLID_LINE_THICK1_DRAW_STATE;
                    // no break here since the next state is the one that we want
                }
                else
                {
                    state = GFX_SSD1926_SOLID_LINE_IDLE_STATE;
                    return (GFX_STATUS_SUCCESS);
                }

            case GFX_SSD1926_SOLID_LINE_THICK1_DRAW_STATE:

                // draw one of the thickness adjustment line
                if (DRV_SSD1926_Line2DDraw(
                            x1 + thickAdjustX, y1 + thickAdjustY,
                            x2 + thickAdjustX, y2 + thickAdjustY) == 
                        GFX_STATUS_FAILURE)
                {
                    return (GFX_STATUS_FAILURE);
                }
                state = GFX_SSD1926_SOLID_LINE_THICK2_DRAW_STATE;
                // no break here since the next state is the one that we want

            case GFX_SSD1926_SOLID_LINE_THICK2_DRAW_STATE:

                // draw one of the thickness adjustment line
                if (DRV_SSD1926_Line2DDraw(
                            x1 - thickAdjustX, y1 - thickAdjustY,
                            x2 - thickAdjustX, y2 - thickAdjustY) == 
                        GFX_STATUS_FAILURE)
                {
                    return (GFX_STATUS_FAILURE);
                }
                state = GFX_SSD1926_SOLID_LINE_IDLE_STATE;
                return GFX_STATUS_SUCCESS;
            default:
                // this case should never happen
                return GFX_STATUS_FAILURE;
                
        } // end of switch
    } // end of while loop

}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_BarDraw(
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom)

    Summary:
        This function renders a bar shape using the currently set fill
        style and color.

    Description:
        This function renders a bar shape with the currently set
        fill style (See GFX_FillStyleGet() and GFX_FillStyleSet() for
        details of fill style):
        - solid color - when the fill style is set to
                        GFX_FILL_STYLE_COLOR
        - alpha blended fill - when the fill style is set to
                        GFX_FILL_STYLE_ALPHA_COLOR.

        Any other selected fill style will be ignored and will assume
        a solid color fill will be used. The parameters left, top, right
        bottom will define the shape dimension.

        When fill style is set to GFX_FILL_STYLE_ALPHA_COLOR, the bar
        can also be rendered with an option to select the type of
        background.
        GFX_BACKGROUND_NONE - the bar will be rendered with no alpha blending.
        GFX_BACKGROUND_COLOR - the bar will be alpha blended with the
                               currently set background color.
        GFX_BACKGROUND_IMAGE - the bar will be alpha blended with the
                               currently set background image.
        GFX_BACKGROUND_DISPLAY_BUFFER - the bar will be alpha blended
                               with the current contents of the frame buffer.

        The background type is set by the GFX_BackgroundTypeSet().

        The rendering of this shape becomes undefined when any one of the
        following is true:
        - Any of the following pixel locations left,top or right,bottom
          falls outside the frame buffer.
        - Colors are not set before this function is called.
        - When right < left
        - When bottom < top
        - When pixel locations defined by left, top and/or right, bottom
          are not on the frame buffer.

*/
// *****************************************************************************
GFX_STATUS GFX_BarDraw(         uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom)
{

#ifdef GFX_CONFIG_NONBLOCKING_DISABLE
    while(GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT);
#else
    if(GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
        return (GFX_STATUS_FAILURE);
#endif

#ifndef GFX_CONFIG_ALPHABLEND_DISABLE
    if (GFX_FillStyleGet() == GFX_FILL_STYLE_ALPHA_COLOR)
    {
        if(GFX_AlphaBlendingValueGet() != 100)
        {
            if (GFX_BarAlphaDraw(left,top,right,bottom))
                return (GFX_STATUS_SUCCESS);
            else
                return (GFX_STATUS_FAILURE);
        }
    }
#endif

    return (DRV_SSD1926_Bar2DDraw(left, top, right, bottom));

}

// *****************************************************************************
/*  Function:
    GFX_STATUS  DRV_SSD1926_Bar2DDraw(
                                    uint16_t left,
                                    uint16_t top,
                                    uint16_t right,
                                    uint16_t bottom)

    Summary:
        This function renders a bar shape using the accelerated feature of
        the display driver.

    Description:
        This function renders a bar shape using the accelerated feature of
        the display driver with the currently set color.

*/
// *****************************************************************************
GFX_STATUS  __attribute__ ((always_inline)) DRV_SSD1926_Bar2DDraw(
                                    uint16_t left,
                                    uint16_t top,
                                    uint16_t right,
                                    uint16_t bottom)
{

    uint32_t    address;
    uint16_t    width, height;

    width = right - left + 1;
    height = bottom - top + 1;

    address = top * (GFX_MaxXGet() + (uint32_t) 1) + left;

    // In case you are wondering what this pixel rendering does,
    // this call to GFX_PixelPut() primes the color to the currently
    // set color. By doing this call, we do not need to set up the
    // pattern for the rectangle (or bar) fill.
    GFX_PixelPut(left, top);

    /* Source, Destination & Brush Window Start Addresses */
    // source
    DRV_SSD1926_RegisterSet(REG_2D_1d4, address & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1d5, (address >> 8) & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1d6, (address >> 16) & 0xFF);
    // destination
    DRV_SSD1926_RegisterSet(REG_2D_1f4, address & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1f5, (address >> 8) & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1f6, (address >> 16) & 0xFF);
    // brush
    DRV_SSD1926_RegisterSet(REG_2D_204, address & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_205, (address >> 8) & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_206, (address >> 16) & 0xFF);

    /* Source & Destination Window Width */
    // destination
    DRV_SSD1926_RegisterSet(REG_2D_1ec, width & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1ed, (width >> 8) & 0xFF);
    // source
    DRV_SSD1926_RegisterSet(REG_2D_1e4, width & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1e5, (width >> 8) & 0xFF);

    /* Source & Destination Window Height */
    // destination
    DRV_SSD1926_RegisterSet(REG_2D_1f0, height & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1f1, (height >> 8) & 0xFF);
    // source
    DRV_SSD1926_RegisterSet(REG_2D_1e8, height & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1e9, (height >> 8) & 0xFF);

    /*  The following are initialized at the initialization routine
        to save time in rendering DRV_SSD1926_Bar2DDraw(). If other
        2D acceleration features that modifies these registers are
        enabled, these registers might need to be programmed here.
     */
#if 0
    /* Brush width */
    DRV_SSD1926_RegisterSet(REG_2D_214, 1);
    DRV_SSD1926_RegisterSet(REG_2D_215, 0);

    /* Brush height */
    DRV_SSD1926_RegisterSet(REG_2D_218, 1);
    DRV_SSD1926_RegisterSet(REG_2D_219, 0);

    /* Display width */
    DRV_SSD1926_RegisterSet(REG_2D_1f8, (GFX_MaxXGet() + 1) & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1f9, ((GFX_MaxXGet() + 1) >> 8) & 0xFF);

    /* Display 2d width */
    DRV_SSD1926_RegisterSet(REG_2D_1d8, (GFX_MaxXGet() + 1) & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1d9, ((GFX_MaxXGet() + 1) >> 8) & 0xFF);
#endif

    /* ROP3 Command */
    DRV_SSD1926_RegisterSet(REG_2D_1fc, 0xF0);

    /* 16bpp */
    DRV_SSD1926_RegisterSet(REG_2D_1dd, 0x00);

    /* ROP command */
    DRV_SSD1926_RegisterSet(REG_2D_1d1, 0x09);

    /* Draw2d command */
    DRV_SSD1926_RegisterSet(REG_2D_1d2, 0x01);

    // when non-blocking make sure the function is done rendering
    // before returning
#ifdef GFX_CONFIG_NONBLOCKING_DISABLE
    while(GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT);
#endif

    return (GFX_STATUS_SUCCESS);

}

#ifndef GFX_CONFIG_ALPHABLEND_DISABLE

// *****************************************************************************
/*  Function:
    GFX_STATUS  DRV_SSD1926_BarAlpha2DDraw(
                                    uint16_t left,
                                    uint16_t top,
                                    uint16_t right,
                                    uint16_t bottom)

    Summary:
        This function renders an alpha blended bar shape using the
        accelerated feature of the display driver.

    Description:
        This function renders an alpha blended bar shape using
        the accelerated feature of the display driver using the
        currently set color.

*/
// *****************************************************************************
GFX_STATUS  __attribute__ ((always_inline)) DRV_SSD1926_BarAlpha2DDraw(
                                    uint16_t left,
                                    uint16_t top,
                                    uint16_t right,
                                    uint16_t bottom)
{

    volatile uint32_t    address;
    uint16_t    width, height, alpha;
    
    width = right - left + 1;
    height = bottom - top + 1;

    alpha = GFX_AlphaBlendingValueGet();

    // derived from the ratio alpha/100 =  SSD1926 alpha/255
    alpha = ((alpha<<8)/100);

    address = ((uint32_t)top * (GFX_MaxXGet() + (uint32_t) 1)) + left;

    /* **************** */
    /* Destination Info */
    /* **************** */
    
    // destination window start address
    DRV_SSD1926_RegisterSet(REG_2D_1f4, address & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1f5, (address >> 8) & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1f6, (address >> 16) & 0xFF);
    // destination window width
    DRV_SSD1926_RegisterSet(REG_2D_1ec, width & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1ed, (width >> 8) & 0xFF);
    // destination window height
    DRV_SSD1926_RegisterSet(REG_2D_1f0, height & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1f1, (height >> 8) & 0xFF);

    /*
      The following are initialized at the initialization routine
      to save time in rendering DRV_SSD1926_BarAlpha2DDraw(). If
      2D acceleration features that modify these registers are
      enabled, these registers might need to be programmed here.
     */
#if 0
    // destination window line address offset
    DRV_SSD1926_RegisterSet(REG_2D_1f8, (GFX_MaxXGet() + 1) & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1f9, ((GFX_MaxXGet() + 1) >> 8) & 0xFF);
#endif

    /* Destination 16bpp */
    DRV_SSD1926_RegisterSet(REG_2D_1dd, 0x00);

    /* **************** */
    /* Source Info      */
    /* **************** */
    // source window start address
    // source address is the next line after the buffer
    address = ((uint32_t)(  GFX_MaxXGet() +
                            (uint32_t)1) * (GFX_MaxYGet() + 
                                           (uint32_t)1) << 1) >> 1;

    DRV_SSD1926_RegisterSet(REG_2D_1d4, address & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1d5, (address >> 8) & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1d6, (address >> 16) & 0xFF);
    // source window width
    DRV_SSD1926_RegisterSet(REG_2D_1e4, width & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1e5, (width >> 8) & 0xFF);
    // source window height
    DRV_SSD1926_RegisterSet(REG_2D_1e8, height & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1e9, (height >> 8) & 0xFF);

    /*
      The following are initialized at the initialization routine
      to save time in rendering DRV_SSD1926_BarAlpha2DDraw(). If
      2D acceleration features that modify these registers are
      enabled, these registers might need to be programmed here.
     */
#if 0
    // source window line address offset
    DRV_SSD1926_RegisterSet(REG_2D_1d8, (GFX_MaxXGet() + 1) & 0xFF);
    DRV_SSD1926_RegisterSet(REG_2D_1d9, ((GFX_MaxXGet() + 1) >> 8) & 0xFF);
#endif

    /* Source 16bpp */
    DRV_SSD1926_RegisterSet(REG_2D_1dc, 0x00);

    /* Alpha Blend Equation (2'b00 = use Microsoft pre-blend see SS1926 Application Note for details */
    DRV_SSD1926_RegisterSet(REG_2D_1fc, 0x00);

    /* Alpha Blend command */
    DRV_SSD1926_RegisterSet(REG_2D_1d0, alpha);

    /* Alpha Blend command */
    DRV_SSD1926_RegisterSet(REG_2D_1d1, 0x07);

    /* Draw2d start command */
    DRV_SSD1926_RegisterSet(REG_2D_1d2, 0x01);

    // this portion makes sure that the alpha blending is done before we
    // exit since any modification of the registers that is used by
    // alpha blending breaks the rendering.
    while(GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT);

    return (GFX_STATUS_SUCCESS);

}
#endif //#ifndef GFX_CONFIG_ALPHABLEND_DISABLE

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
    // make sure the driver has no pending or ongoing rendering
    while(DRV_SSD1926_RegisterGet(REG_2D_220) == 0);

    // Call the 3D Bar Draw directly since GFX_ScreenClear() is pure
    // color fill of the screen.
    while(DRV_SSD1926_Bar2DDraw(0, 0, GFX_MaxXGet(), GFX_MaxYGet()) != GFX_STATUS_SUCCESS);
    
    // make sure the screen clear is done before exiting
    while(DRV_SSD1926_RegisterGet(REG_2D_220) == 0);

    GFX_LinePositionSet(0, 0);

    return (GFX_STATUS_SUCCESS);

}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_RenderStatusGet()

    Summary:
        This function returns the driver's status on rendering.

    Description:
        This function returns the driver's rendering status. Valid values
        returned are GFX_STATUS_BUSY_BIT or GFX_STATUS_READY_BIT
        (see GFX_LINE_STYLE).

*/
// *****************************************************************************
GFX_STATUS_BIT GFX_RenderStatusGet(void)
{
    uint16_t status;

    // SSD1926 Draw 2D Command Status
    // 1 - ready for new command
    // 0 - busy with previous command
    status = (DRV_SSD1926_RegisterGet(REG_2D_220) & 0x01);
    if (status == 0x01)
        return GFX_STATUS_READY_BIT;
    else
        return GFX_STATUS_BUSY_BIT;

}


// *****************************************************************************
// *****************************************************************************
// Section: SSD1926 pins configured as Bit-Banged SPI
// *****************************************************************************
// *****************************************************************************

// To emulate the SPI communication the following pins in the
// SSD1926 GPIO ports are assigned to SPI signals. BB means BitBang
typedef enum
{
    SSD1926_GPIO_BB_CS  = 0x01,   // chip select
    SSD1926_GPIO_BB_SCL = 0x02,   // SPI clock
    SSD1926_GPIO_BB_SDO = 0x04,   // SPI Out
    SSD1926_GPIO_BB_DC  = 0x08,   // Data/Command pin
} SSD1926_GPIO_BITBANG_SPI_PORT;

#ifndef SSD1926_IOPORT_SPI_PERIOD
    // define this macro globally if desired bitbang SPI period
    // is not the one that is defined here.
    #define SSD1926_IOPORT_SPI_PERIOD 1
#endif

/************************************************************************
* Function: void DRV_SSD1926_IOPortInit(void)
*
* Overview: This initializes the SSD1926 GPIO ports.
*
* Input: None
*
* Output: none
*
************************************************************************/
void DRV_SSD1926_IOPortInit(void)
{
    DRV_SSD1926_RegisterSet(0xAC, 0x1F);     // initialize all to high
    DRV_SSD1926_RegisterSet(0xA8, 0x1F);     // set the io port as outputs
}

// *****************************************************************************
/*  Function:
    GFX_STATUS DRV_SSD1926_IOPortSet(
                                uint8_t mask,
                                uint8_t level)

    Summary:
        This function sets the SSD1926 IO ports.

    Description:
        This function sets the SSD1926 IO ports specified by the mask
        and the value set to level.

*/
// *****************************************************************************
void DRV_SSD1926_IOPortSet(
                                uint8_t mask,
                                uint8_t level)
{
    static uint8_t value = 0xFF;

    if(level == 0)
    {
        value &= ~mask;
    }
    else
    {
        value |= mask;
    }
    // 0xAC is the address of the IO port in SSD1926
    DRV_SSD1926_RegisterSet(0xAC, value);

}

// *****************************************************************************
/*  Function:
    GFX_STATUS DRV_SSD1926_IOPortWriteData(
                                uint8_t value)

    Summary:
        This function writes the SPI data write sequence to
        the SSD1926 IO ports.

    Description:
        This function writes the SPI data write sequence to
        the SSD1926 IO ports. The data written is specified by
        the parameter value.

*/
// *****************************************************************************
void DRV_SSD1926_IOPortWriteData(uint8_t value)
{
    uint8_t    mask;

    mask = 0x80;

    // Note: The sequence below produces an SPI clock with
    //       approximately: SSD1926_IOPORT_SPI*2 period.
    while(mask)
    {
        DRV_SSD1926_IOPortSet(SSD1926_GPIO_BB_SCL, 0);
        if(mask & value)
        {
            DRV_SSD1926_IOPortSet(SSD1926_GPIO_BB_SDO, 1);
        }
        else
        {
            DRV_SSD1926_IOPortSet(SSD1926_GPIO_BB_SDO, 0);
        }
        __delay_us(SSD1926_IOPORT_SPI_PERIOD);
        DRV_SSD1926_IOPortSet(SSD1926_GPIO_BB_SCL, 1);
        __delay_us(SSD1926_IOPORT_SPI_PERIOD);
        mask >>= 1;
    }
}

// *****************************************************************************
/*  Function:
    GFX_STATUS DRV_SSD1926_IOPortWriteCommand(
                                uint16_t index,
                                uint16_t value)

    Summary:
        This function emulates the SPI write to the SSD1926 IO ports.

    Description:
        This function emulates the SPI write to the SSD1926 IO ports.
        The SSD1926 IO port is connected to an Timer Controller (TCON)
        which supports SPI interface to write to its registers.

        The index is the address of the TCON register and the value is the
        data written TCON register.

*/
// *****************************************************************************
void DRV_SSD1926_IOPortWriteCommand(uint16_t index, uint16_t value)
{
    typedef union
    {
        uint8_t  indexByte[2];
        uint16_t indexValue;
    }  GFX_TCON_DATA;

    DRV_SSD1926_IOPortSet(SSD1926_GPIO_BB_CS, 0);

    // Index
    DRV_SSD1926_IOPortSet(SSD1926_GPIO_BB_DC, 0);
    DRV_SSD1926_IOPortWriteData(((GFX_TCON_DATA)index).indexByte[1]);
    DRV_SSD1926_IOPortWriteData(((GFX_TCON_DATA)index).indexByte[0]);

    DRV_SSD1926_IOPortSet(SSD1926_GPIO_BB_CS, 1);
    __delay_us(10);
    DRV_SSD1926_IOPortSet(SSD1926_GPIO_BB_CS, 0);

    // Data
    DRV_SSD1926_IOPortSet(SSD1926_GPIO_BB_DC, 1);
    DRV_SSD1926_IOPortWriteData(((GFX_TCON_DATA)value).indexByte[1]);
    DRV_SSD1926_IOPortWriteData(((GFX_TCON_DATA)value).indexByte[0]);

    DRV_SSD1926_IOPortSet(SSD1926_GPIO_BB_CS, 1);
    __delay_us(10);
}


#endif // #if (GFX_USE_DISPLAY_CONTROLLER_SSD1926) 
