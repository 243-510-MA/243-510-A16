/*******************************************************************************
 Module for Microchip Graphics Library - Display Controller Driver for 
                                         PIC24FJ256DA210 Device Family

  Company:
    Microchip Technology Inc.

  File Name:
    drv_gfx_da210.c

  Summary:
    Display Controller Driver for the Graphics Module of PIC24FJ256DA210 
    Device Family.

  Description:
    This module is the display driver for the graphics module found in PIC24F
    devices (i.e. PIC24FJ256DA210 Family of Devices). This implements the 
    Display Driver Layer when using the PIC24F devices with graphics module  
    with the Microchip Graphics Library. 
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

#if defined (GFX_USE_DISPLAY_CONTROLLER_MCHP_DA210)

#include <xc.h>
#include <libpic30.h>                        
#include "driver/gfx/drv_gfx_display.h"      
#include "driver/gfx/drv_gfx_da210.h"
#include "driver/gfx/drv_gfx_tcon.h"
#include "gfx/gfx_primitive.h"

#ifndef GFX_CONFIG_PALETTE_DISABLE
    #include "gfx/gfx_palette.h"
#endif

// *****************************************************************************
// *****************************************************************************
// Section: Global Variables
// *****************************************************************************
// *****************************************************************************

    // work areas base addresses
    static volatile uint32_t   drvGfxWorkArea1BaseAddress;
    static volatile uint32_t   drvGfxWorkArea2BaseAddress;

    // display buffer base address
    // For Double Buffering:
    // when mode is DISABLED, frame and draw buffers will use the same address
    // When mode is ENABLED, frame and draw buffers will not be the same address
    // Address (or buffer) used to display pixels on the screen will always
    // be the frame buffer (see G1DPADRH and G1DPADRL registers for description)
    static volatile uint32_t   drvGfxFrameBufferBaseAddress;
    static volatile uint32_t   drvGfxDrawBufferBaseAddress;

#ifndef GFX_CONFIG_DOUBLE_BUFFERING_DISABLE

    static volatile uint32_t        drvGfxFrameBufferArray[2];
    static volatile unsigned int    drvGfxDrawBufferIndex;
    static volatile unsigned int    drvGfxFrameBufferIndex;

#endif 

#ifndef GFX_CONFIG_PALETTE_DISABLE

    const GFX_PALETTE_ENTRY         *pPendingPaletteEntry;
    static volatile uint8_t         PaletteBpp;
    static volatile uint16_t        PendingStartEntry;
    static volatile uint16_t        PendingLength;
    static volatile bool            PendingPaletteFlag;
    static volatile GFX_PALETTE_ENTRY paletteEntry[256];

#endif


#ifndef GFX_CONFIG_IPU_DECODE_DISABLE

    #if (GFX_IPU_TEMP_DATA_TRANSFER_ARRAY_SIZE > 1024)
        #error "GFX_IPU_TEMP_DATA_TRANSFER_ARRAY_SIZE must be <= 1024"
    #endif
    
    #ifndef GFX_DECOMPRESSED_BUFFER_SIZE
        #error "GFX_DECOMPRESSED_BUFFER_SIZE is not defined. This must be defined in Hardware Profile."
    #endif
    
#endif 

// check if we can use the driver OutChar
#if (DISP_ORIENTATION == 0)
    #if (defined(GFX_CONFIG_FONT_ANTIALIASED_DISABLE))
        #define USE_DRV_FONT
    #endif
#endif


#define GFX_FLIP(a,b) { int16_t t=a; a=b; b=t; }
#define MASK_50_PERCENT         (0b1111011111011110)
#define MASK_25_PERCENT         (0b1110011110011100)
#define GFX_GET_RED(color)      ((color) & 0xF800) 
#define GFX_GET_GREEN(color)    ((color) & 0x07E0) 
#define GFX_GET_BLUE(color)     ((color) & 0x001F) 

/////////////////////// LOCAL FUNCTIONS PROTOTYPES ////////////////////////////
static uint16_t     GFX_FreeCommandSpaceCheck(
                                uint16_t spaceNeeded);
#ifndef GFX_CONFIG_PALETTE_DISABLE
static GFX_STATUS   DRV_GFX_ColorLookUpTableSet(
                                const GFX_PALETTE_ENTRY *pPaletteEntry,
                                uint16_t start,
                                uint16_t count);
#endif

#if  !defined (GFX_CONFIG_PALETTE_DISABLE)  || !defined (GFX_CONFIG_DOUBLE_BUFFERING_DISABLE)
static void DRV_GFX_VerticalBlankInterruptStart(void);
#endif

//uint16_t 	OutCharEds(GFX_XCHAR ch);

#define DrvGetX() (G1CHRX)
#define DrvGetY() (G1CHRY)

// *****************************************************************************
// *****************************************************************************
// Section: Helper Macros and Functions
// *****************************************************************************
// *****************************************************************************

// This macro calculates the x,y position based on the orientation of the 
// display.
#if (DISP_ORIENTATION == 0)
#define GFX_OrientationPixelAdjust(x, y)
#elif (DISP_ORIENTATION == 90)
#define GFX_OrientationPixelAdjust(x, y)                                \
                                    {                                   \
                                        int16_t t = x;                  \
                                        x = y;                          \
                                        y = GFX_MaxXGet() - t;          \
                                    }
#elif (DISP_ORIENTATION == 180)
#define GFX_OrientationPixelAdjust(x, y)                                \
                                    {                                   \
                                        x = GFX_MaxXGet() - x;          \
                                        y = GFX_MaxYGet() - y;          \
                                    }
#elif (DISP_ORIENTATION == 270)
#define GFX_OrientationPixelAdjust(x, y)                                \
                                    {                                   \
                                        int16_t t = x;                  \
                                        x = GFX_MaxYGet() - y;          \
                                        y = t;                          \
                                    }
#endif

// This macro calculates the x,y position based on the orientation of the
// display and the given width and height of the block.
#if (DISP_ORIENTATION == 0)
#define GFX_OrientationLineAdjust(x, y, width, height)
#elif (DISP_ORIENTATION == 90)
#define GFX_OrientationLineAdjust(x, y, width, height)                  \
                                    {                                   \
                                        int16_t t = x;                  \
                                        x = y;                          \
                                        y = DISP_VER_RESOLUTION - t - height; \
                                    }                                   
#elif (DISP_ORIENTATION == 180)
#define GFX_OrientationLineAdjust(x, y, width, height)                  \
                                    {                                   \
                                        x = DISP_HOR_RESOLUTION - x - width; \
                                        y = DISP_VER_RESOLUTION - y - height;\
                                    }
#elif (DISP_ORIENTATION == 270)
#define GFX_OrientationLineAdjust(x, y, width, height)                  \
                                    {                                   \
                                        int16_t t = x;                  \
                                        x = DISP_HOR_RESOLUTION - y - width; \
                                        y = t;                          \
                                    }
#endif

// This macro calculates the x,y position based on the orientation of the
// display and the given width and height of the block.
#if (DISP_ORIENTATION == 0)
#define GFX_OrientationBlockAdjust(left, top, right, bottom, width, height) \
                                    {                                   \
                                        width  = right  - left + 1;     \
                                        height = bottom - top  + 1;     \
                                    }
#elif (DISP_ORIENTATION == 90)
#define GFX_OrientationBlockAdjust(left, top, right, bottom, width, height) \
                                    {                                   \
                                        height  = right  - left + 1;    \
                                        width   = bottom - top  + 1;    \
                                        bottom = DISP_VER_RESOLUTION - left; \
                                        left   = top;                   \
                                        right  = left + width;          \
                                        top    = bottom - height;       \
                                    }
#elif (DISP_ORIENTATION == 180)
#define GFX_OrientationBlockAdjust(left, top, right, bottom, width, height) \
                                    {                                   \
                                        width  = right  - left + 1;    \
                                        height = bottom - top  + 1;    \
                                        right  = DISP_HOR_RESOLUTION - left; \
                                        bottom = DISP_VER_RESOLUTION - top;  \
                                        left   = right - width;         \
                                        top    = bottom - height;       \
                                    }
#elif (DISP_ORIENTATION == 270)
#define GFX_OrientationBlockAdjust(left, top, right, bottom, width, height) \
                                    {                                   \
                                        height  = right  - left + 1;    \
                                        width   = bottom - top  + 1;    \
                                        right  = DISP_HOR_RESOLUTION - top; \
                                        top    = left;                  \
                                        bottom = top + height;          \
                                        left   = right - width;         \
                                    }
#endif


#ifndef GFX_CONFIG_DOUBLE_BUFFERING_DISABLE

// *****************************************************************************
/*  Function:
    uint32_t DRV_GFX_DrawBufferAddressGet(void)

    Summary:
        This function returns the address location of the draw buffer.

    Description:
        This function returns the address location of the draw buffer.

*/
// *****************************************************************************
inline uint32_t __attribute__ ((always_inline)) DRV_GFX_DrawBufferAddressGet(void)
{
    return drvGfxDrawBufferBaseAddress;
}
	
// *****************************************************************************
/*  Function:
    uint32_t DRV_GFX_FrameBufferAddressGet(void)

    Summary:
        This function returns the address location of the frame buffer.

    Description:
        This function returns the address location of the frame buffer.

*/
// *****************************************************************************
inline uint32_t __attribute__ ((always_inline)) DRV_GFX_FrameBufferAddressGet(void)
{
    return (drvGfxFrameBufferBaseAddress);
}

#endif 

// *****************************************************************************
/*  Function:
    uint16_t GFX_FreeCommandSpaceCheck(uint16_t spaceNeeded)

    Summary:
        This function checks if the number of command spaces needed
        are available in the command buffer.

    Description:
        This function checks if the number of command spaces needed
        are available in the command buffer.
        Returns 0 if the number of command spaces available is less than the
        number of command spaces needed.
        Returns 1 if the number of command spaces available is equal or more
        than the number of command spaces needed.


*/
// *****************************************************************************
static inline uint16_t __attribute__ ((always_inline)) GFX_FreeCommandSpaceCheck(
                                uint16_t spaceNeeded)
{
#ifdef GFX_CONFIG_NONBLOCKING_DISABLE
    while((GFX_COMMAND_QUEUE_LENGTH - _GCMDCNT) < spaceNeeded);
    return (1);
#else
    if ((GFX_COMMAND_QUEUE_LENGTH - _GCMDCNT) < spaceNeeded)
        return (0);
    else
        return (1);
#endif

}

// *****************************************************************************
/*  Function:
    void DRV_GFX_EPMPInitialize(void)

    Summary:
        Initializes the Enhanced Parallel Master Port (EPMP).
 
    Description:
        Initializes the Enhanced Parallel Master Port (EPMP) for
        Alternate Master Bypass Mode. Used when external memory
        is used for graphics module operations.

*/
// *****************************************************************************
void DRV_GFX_EPMPInitialize(void)
{
	/* Note: When using the EPMP to access external RAM or Flash, PMA0-PMA16 will only access a range of 
	         256K RAM. To increase this range enable higher Address lines. To do this set the GFX_EPMP_CS1_MEMORY_SIZE
			 or GFX_EPMP_CS2_MEMORY_SIZE in the Hardware Profile.
	*/

#if defined (GFX_EPMP_CS1_BASE_ADDRESS) || defined (GFX_EPMP_CS2_BASE_ADDRESS)

    volatile uint16_t gfxAccessTime; 
	     uint16_t deviceAccessTime;

    /* Depends on the selected clock source frequency calculate the
       divider if using external memory, calcAMWAIT is the minimum EPMP
       Alternate Master Transfer from
            (1000000000ul) / (uint32_t)((CLKDIVbits.G1CLKSEL == 1) ?
                            96000000ul : 48000000ul);
       there is a factor of 100 to take care of the two decimal places
       on the calculation

    */
    if (CLKDIVbits.G1CLKSEL == 1)
        gfxAccessTime = 1041; // from 1/96 Mhz 
    else
        gfxAccessTime = 2083; // from 1/48 Mhz 

    /* Note:
        When using devices that has Alternate PMP pin configuration 
        (ALTPMP) there are address pins that are remapped to other
        pins to free up some analog pins. Make sure the pins assigned
        to address lines PMA[x] are set to digital mode.
        See Device Data Sheet for details.
    */
    // Assuming ALTPMP = 0 
    ANSDbits.ANSD7 = 0;   // PMD15
    ANSDbits.ANSD6 = 0;   // PMD14
    ANSFbits.ANSF0 = 0;   // PMD11
    
    ANSBbits.ANSB15 = 0;  // PMA0
    ANSBbits.ANSB14 = 0;  // PMA1
    ANSGbits.ANSG9  = 0;  // PMA2
    ANSBbits.ANSB13 = 0;  // PMA10
    ANSBbits.ANSB12 = 0;  // PMA11
    ANSBbits.ANSB11 = 0;  // PMA12
    ANSBbits.ANSB10 = 0;  // PMA13
    ANSAbits.ANSA7 = 0;   // PMA17
    ANSGbits.ANSG6 = 0;   // PMA18

    // uncomment corresponding lines if PMA22:19] is/are used
    // as device address lines
    //ANSEbits.ANSE9 = 0;   // PMA19
    //ANSGbits.ANSG7 = 0;   // PMA20
    //ANSGbits.ANSG8 = 0;   // PMA21
    //ANSCbits.ANSC4 = 0;   // PMA22


    PMCON1bits.ADRMUX = 0;  // address is not multiplexed
    PMCON1bits.MODE = 3;    // master mode
    PMCON1bits.CSF = 0;     // PMCS1 pin used for chip select 1,
                            // PMCS2 pin used for chip select 2
    PMCON1bits.ALP = 0;     // Don't care (not used)
    PMCON1bits.ALMODE = 0;  // Don't care (not used)
    PMCON1bits.BUSKEEP = 0; // bus keeper is not used

#if defined (GFX_EPMP_CS1_BASE_ADDRESS)
    // CS1 start address
    PMCS1BS = (GFX_EPMP_CS1_BASE_ADDRESS>>8);
    
    // check if how many address lines to use
    // Note:
    //      - GFX_EPMP_CS1_MEMORY_SIZE is word (2 bytes) count
    //      - assumes that the PMA[x] signal/pin is mapped to an actual device
#if   (((GFX_EPMP_CS1_MEMORY_SIZE) > 0x10000ul)  && ((GFX_EPMP_CS1_MEMORY_SIZE) <= 0x20000ul))
    PMCON3 |= 0x0001; // PMA16 address line is enabled
#elif (((GFX_EPMP_CS1_MEMORY_SIZE) > 0x20000ul)  && ((GFX_EPMP_CS1_MEMORY_SIZE) <= 0x40000ul))
    PMCON3 |= 0x0003; // PMA[17:16] address lines are enabled
#elif (((GFX_EPMP_CS1_MEMORY_SIZE) > 0x40000ul)  && ((GFX_EPMP_CS1_MEMORY_SIZE) <= 0x80000ul))
    PMCON3 |= 0x0007; // PMA[18:16] address lines are enabled
#elif (((GFX_EPMP_CS1_MEMORY_SIZE) > 0x80000ul)  && ((GFX_EPMP_CS1_MEMORY_SIZE) <= 0x100000ul))
    PMCON3 |= 0x000F; // PMA[19:16] address lines are enabled
#elif (((GFX_EPMP_CS1_MEMORY_SIZE) > 0x100000ul) && ((GFX_EPMP_CS1_MEMORY_SIZE) <= 0x200000ul))
    PMCON3 |= 0x001F; // PMA[20:16] address lines are enabled
#elif (((GFX_EPMP_CS1_MEMORY_SIZE) > 0x200000ul) && ((GFX_EPMP_CS1_MEMORY_SIZE) <= 0x400000ul))
    PMCON3 |= 0x003F; // PMA[21:16] address lines are enabled
#elif (((GFX_EPMP_CS1_MEMORY_SIZE) > 0x400000ul) && ((GFX_EPMP_CS1_MEMORY_SIZE) <= 0x800000ul))
    PMCON3 |= 0x007F; // PMA[22:16] address lines are enabled
#endif           

    PMCS1CFbits.CSDIS = 0; // enable CS
    PMCS1CFbits.CSP = EPMPCS1_CS_POLARITY;  // CS1 polarity
    PMCS1CFbits.BEP = EPMPCS1_BE_POLARITY;  // byte enable polarity
    PMCS1CFbits.WRSP = EPMPCS1_WR_POLARITY; // write strobe polarity
    PMCS1CFbits.RDSP = EPMPCS1_RD_POLARITY; // read strobe polarity
    PMCS1CFbits.CSPTEN = 1;                 // enable CS port
    PMCS1CFbits.SM = 0;                     // read and write strobes on separate lines
    PMCS1CFbits.PTSZ = 2;                   // data bus width is 16-bit

    PMCS1MDbits.ACKM = 0;                   // PMACK is not used


    // Calculate the AMWAIT cycles to satisfy the access time of the device
    deviceAccessTime = EPMPCS1_ACCESS_TIME * 100;

    if ((deviceAccessTime) < gfxAccessTime)
        PMCS1MDbits.AMWAIT = 0;
    else if (deviceAccessTime >= gfxAccessTime) 
	{
        PMCS1MDbits.AMWAIT = (deviceAccessTime / gfxAccessTime);
        if ((deviceAccessTime % gfxAccessTime) > 0)
            PMCS1MDbits.AMWAIT += 1;
    }

#else
    PMCS1CFbits.CSDIS = 1; // disable CS1 functionality
#endif //#if defined (GFX_EPMP_CS1_BASE_ADDRESS)

#if defined (GFX_EPMP_CS2_BASE_ADDRESS)

    PMCS2BS = (GFX_EPMP_CS2_BASE_ADDRESS >> 8); // CS2 start address

    // check if how many address lines to use
    // Note:
    //      - GFX_EPMP_CS2_MEMORY_SIZE is in word (2 bytes) count
    //      - assumes that the PMA[x] signal/pin is mapped to an actual device
#if   (((GFX_EPMP_CS2_MEMORY_SIZE) > 0x10000ul)  && ((GFX_EPMP_CS2_MEMORY_SIZE) <= 0x20000ul))
    PMCON3 |= 0x0001; // PMA16 address line is enabled
#elif (((GFX_EPMP_CS2_MEMORY_SIZE) > 0x20000ul)  && (GFX_EPMP_CS2_MEMORY_SIZE) <= 0x40000ul))
    PMCON3 |= 0x0003; // PMA[17:16] address lines are enabled
#elif (((GFX_EPMP_CS2_MEMORY_SIZE) > 0x40000ul)  && ((GFX_EPMP_CS2_MEMORY_SIZE) <= 0x80000ul))
    PMCON3 |= 0x0007; // PMA[18:16] address lines are enabled
#elif (((GFX_EPMP_CS2_MEMORY_SIZE) > 0x80000ul)  && ((GFX_EPMP_CS2_MEMORY_SIZE) <= 0x100000ul))
    PMCON3 |= 0x000F; // PMA[19:16] address lines are enabled
#elif (((GFX_EPMP_CS2_MEMORY_SIZE) > 0x100000ul) && ((GFX_EPMP_CS2_MEMORY_SIZE) <= 0x200000ul))
    PMCON3 |= 0x001F; // PMA[20:16] address lines are enabled
#elif (((GFX_EPMP_CS2_MEMORY_SIZE) > 0x200000ul) && ((GFX_EPMP_CS2_MEMORY_SIZE) <= 0x400000ul))
    PMCON3 |= 0x003F; // PMA[21:16] address lines are enabled
#elif (((GFX_EPMP_CS2_MEMORY_SIZE) > 0x400000ul) && ((GFX_EPMP_CS2_MEMORY_SIZE) <= 0x800000ul))
    PMCON3 |= 0x007F; // PMA[22:16] address lines are enabled
#endif           

    PMCS2CFbits.CSDIS = 0; // enable CS

    PMCS2CFbits.CSP = EPMPCS2_CS_POLARITY;  // CS2 polarity
    PMCS2CFbits.BEP = EPMPCS2_BE_POLARITY;  // byte enable polarity
    PMCS2CFbits.WRSP = EPMPCS2_WR_POLARITY; // write strobe polarity
    PMCS2CFbits.RDSP = EPMPCS2_RD_POLARITY; // read strobe polarity
    PMCS2CFbits.CSPTEN = 1;                 // enable CS port
    PMCS2CFbits.SM = 0;                     // read and write strobes
                                            // on separate lines
    PMCS2CFbits.PTSZ = 2;                   // data bus width is 16-bit

    PMCS2MDbits.ACKM = 0;                   // PMACK is not used

    // Calculate the AMWAIT cycles to satisfy the access time of the device
    deviceAccessTime = EPMPCS2_ACCESS_TIME * 100;

    if ((deviceAccessTime) < gfxAccessTime)
        PMCS2MDbits.AMWAIT = 0;
    else if (deviceAccessTime >= gfxAccessTime) 
    {
        PMCS2MDbits.AMWAIT = (deviceAccessTime / gfxAccessTime);
        if ((deviceAccessTime % gfxAccessTime) > 0)
            PMCS2MDbits.AMWAIT += 1;
    }

#else	
    PMCS2CFbits.CSDIS = 1; // disable CS2 functionality
    PMCS2BS = 0x0000;
#endif //#if defined (GFX_EPMP_CS2_BASE_ADDRESS)

    PMCON2bits.RADDR = 0xFF;    // set CS2 end address
    PMCON4 = 0xFFFF;            // PMA0 - PMA15 address lines are enabled

    PMCON3bits.PTWREN = 1;      // enable write strobe port
    PMCON3bits.PTRDEN = 1;      // enable read strobe port
    PMCON3bits.PTBE0EN = 1;     // enable byte enable port
    PMCON3bits.PTBE1EN = 1;     // enable byte enable port
    PMCON3bits.AWAITM = 0;      // set address latch pulses width to 1/2 Tcy
    PMCON3bits.AWAITE = 0;      // set address hold time to 1/4 Tcy

    __delay_ms(100);

    PMCON2bits.MSTSEL = 3;      // select EPMP bypass mode (for Graphics operation)
    PMCON1bits.PMPEN = 1;       // enable the module

    __delay_ms(100);

#endif // end of #if defined (GFX_EPMP_CS1_BASE_ADDRESS) || defined (GFX_EPMP_CS2_BASE_ADDRESS)

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

    /* Switch On display if IO controlled; if controlled
       by the Graphics Module, hook up the power-on signal of the display to
       GPWR pin of the Graphics Module and this code should not compile */

    DisplayPowerOn();
    DisplayPowerConfig();

    /* Set up the Graphics Module clock source and the GCLK frequency. */
    _G1CLKSEL = 1;                      // 96MHz Enable
    _GCLKDIV = GFX_GCLK_DIVIDER;        // value used is dependent
                                        // on the display panel specification

    DRV_GFX_EPMPInitialize();

/************ End - Project Specific Code - Relocate ************/

    G1CON1   = 0; /* Switch off the module */
    G1CON2   = 0;
    G1CON3   = 0;
    G1IE     = 0;
    G1IR     = 0;
    G1CLUT   = 0;
    G1MRGN   = 0;
    G1CLUTWR = 0;
    G1CHRX   = 0;
    G1CHRY   = 0;
    G1CMDL   = 0;
    G1CMDH   = 0;
    
    // set the  processing unit bit per pixel
    _PUBPP   = GFX_BITS_PER_PIXEL;
    // set the  display controller bit per pixel
    _DPBPP   = GFX_BITS_PER_PIXEL;
    // set the LCD type used (TFT, MSTN or CSTN)
    _DPMODE  = GFX_LCD_TYPE;

#if (GFX_LCD_TYPE == GFX_LCD_MSTN) || (GFX_LCD_TYPE == GFX_LCD_CSTN)
    // set the display width
    _DPGWDTH = STN_DISPLAY_WIDTH;
#endif

    /* Port configurations */
#ifdef GFX_DISPLAYENABLE_ENABLE
    // set the display enable polarity
_DPENPOL = GFX_DISPLAYENABLE_POLARITY;
_DPENOE = 1;
#endif
    
#ifdef GFX_HSYNC_ENABLE
    // set the HSYNC signal polarity
    _DPHSPOL = GFX_HSYNC_POLARITY;
    _DPHSOE = 1;
#endif
    
#ifdef GFX_VSYNC_ENABLE
    // set the VSYNC signal polarity
    _DPVSPOL = GFX_VSYNC_POLARITY;
    _DPVSOE = 1;
#endif
    
#ifdef GFX_DISPLAYPOWER_ENABLE
    // enable the GPWR pin, when this is enabled it can be used
    // to control external power circuitry for the display
    _DPPWROE = 1;
#endif
    
    /* Display timing signal configurations */
    // set the clock polarity	
    _DPCLKPOL = GFX_CLOCK_POLARITY;
    
    // set the display buffer dimension
    G1DPW = DISP_HOR_RESOLUTION;
    G1DPH = DISP_VER_RESOLUTION;
    
    // set the work area dimension
    G1PUW = DISP_HOR_RESOLUTION;
    G1PUH = DISP_VER_RESOLUTION;
    
    /* Note:
    	In some display panel the definition of porches
        (front and back porches) varies.
    	Example TFT display definitions (for horizontal timing):
            1. Horizontal Cycle = horizontal front porch + 
                                  horizontal back porch  +
                                  horizontal display period
            2. Horizontal Cycle = horizontal front porch +
                                  horizontal back porch +
                                  horizontal display period +
                                  horizontal sync pulse width
            In example (1) the horizontal sync pulse width must not exceed 
            the horizontal back porch. For the vertical timing, the equations
            are the same (replace horizontal with vertical).

            For the Microchip graphics controller: the definition follows 
            example (2). To accomodate displays like example (1), adjust
            the back porches and pulse widths accordingly. Refer to
            FRM Section 43. Graphics Controller Module (GFX)
            (Document #:DS39731).
    */
    #define HT  (DISP_HOR_PULSE_WIDTH + DISP_HOR_BACK_PORCH +   \
                 DISP_HOR_FRONT_PORCH + DISP_HOR_RESOLUTION)
    #define VT  (DISP_VER_PULSE_WIDTH + DISP_VER_BACK_PORCH +   \
                 DISP_VER_FRONT_PORCH + DISP_VER_RESOLUTION)

    G1DPWT = HT;
    G1DPHT = VT;

    // set the horizontal pulse width
    _HSLEN = DISP_HOR_PULSE_WIDTH;
    _HSST  = 0; 
    
    // set the verrizontal pulse width
    _VSLEN = DISP_VER_PULSE_WIDTH;
    _VSST  = 0; 

    // set the horizontal & vertical start position
    _HENST = _HSST + DISP_HOR_PULSE_WIDTH + DISP_HOR_BACK_PORCH;
    _VENST = _VSST + DISP_VER_PULSE_WIDTH + DISP_VER_BACK_PORCH;

    // set the active pixel and active line start position
    _ACTPIX  = _HENST;
    _ACTLINE = _VENST;
	
    // initialize the work areas and display buffer base addresses
    drvGfxWorkArea1BaseAddress   = GFX_DISPLAY_BUFFER_START_ADDRESS;
    drvGfxWorkArea2BaseAddress   = GFX_DISPLAY_BUFFER_START_ADDRESS;

    // initialize the frame buffer and the draw buffer base addresses
    drvGfxFrameBufferBaseAddress = GFX_DISPLAY_BUFFER_START_ADDRESS;
    drvGfxDrawBufferBaseAddress  = GFX_DISPLAY_BUFFER_START_ADDRESS;

    // Set the display buffer base address (SFR) (or the start
    // addresses in RAM)
    GFX_SetDisplayArea(GFX_DISPLAY_BUFFER_START_ADDRESS);
    GFX_SetWorkArea1(drvGfxFrameBufferBaseAddress);
    GFX_SetWorkArea2(drvGfxFrameBufferBaseAddress);

    /* Switch On the Graphics Module */
    _GDBEN = 0xFFFF;
    _DPPINOE = 1;

#ifndef GFX_CONFIG_PALETTE_DISABLE
    // initialize the color look up table (CLUT) if enabled
    DRV_GFX_PaletteInitialize();
#endif
  
    _DPSTGER = 0;       // display data stagger (set to none)
    _GFX1IF  = 0;       // clear graphics interrupt flag
    _GFX1IE  = 0;       // graphics interrupt are not enabled
    _G1EN    = 1;       // turn on the graphics module

    __delay_ms(100);
    
    // Turn on the display refresh sequence. This control signal
    // will be reflected in GPWR pin if  _DPPWROE is set to 1.
    // GPWR will immediately be set to 1 after DPPOWER bit is set.
    // Actual refresh sequence will be delayed and the delay is set 
    // by GICLUTWR<15:0> x 64 GCLK Cycles. 
    // Refer to the Graphics Module FRM document for details.
    // Section 43.7 DISPLAY POWER SEQUENCING
    _DPPOWER = 1;					

#ifndef GFX_CONFIG_DOUBLE_BUFFERING_DISABLE

    // initialize buffers for double buffering feature
    // at startup double buffering should be disabled.
    // So the initialization will result in the same
    // draw and frame buffer.
    GFX_DrawBufferInitialize(0, GFX_BUFFER1);
    GFX_DrawBufferInitialize(1, GFX_BUFFER2);

    // both frame and draw buffer points to the same area at
    // initialization
    GFX_DrawBufferSet(0);
    GFX_FrameBufferSet(0);

#endif 

    // clear the screen to all black first to remove the noise screen
    GFX_ColorSet(0);

    GFX_BarDraw(0, 0, GFX_MaxXGet(), GFX_MaxYGet());

#ifdef USE_TCON_MODULE
    // Panel Timing Controller (TCON) Programming
    DRV_TCON_Initialize(TCONWrite);
#endif

    __delay_ms(100);

    // disable clipping (default setting)
    // initialize the text area to be set for the whole screen
    GFX_CHR_SetTextAreaStart(0, 0);
    GFX_CHR_SetTextAreaEnd(GFX_MaxXGet(), GFX_MaxYGet());

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
    uint16_t ROPBlockInline(
                                uint32_t srcAddr,
                                uint32_t dstAddr,
                                uint32_t srcOffset,
                                uint32_t dstOffset,
                                uint16_t srcType,
                                uint16_t dstType,
                                uint16_t copyOp,
                                uint16_t rop,
                                uint16_t color,
                                uint16_t width,
                                uint16_t height)

    Summary:
        This function combines a rectangular area of pixels to another
        rectangular area of pixels with raster operations.

    Description:
        This function combines a rectangular area of pixels to another
        rectangular area of pixels with raster operations.
        There are three copy operations and 15 raster operations.

        The available raster operations are the following:
        - Solid black color fill
        - not (Source or Destination)
        - (not Source) and Destination
        - not Source
        - Source and (not Destination)
        - not Destination
        - Source xor Destination
        - not (Source and Destination)
        - Source and Destination
        - not (Source xor Destination)
        - Destination
        - (not Source) or Destination
        - Source
        - Source or (not Destination)
        - Source or Destination
        - Solid white color fill

*/
// *****************************************************************************
inline uint16_t __attribute__ ((always_inline)) ROPBlockInline(
                                uint32_t srcAddr,
                                uint32_t dstAddr,
                                uint32_t srcOffset,
                                uint32_t dstOffset,
                                uint16_t srcType,
                                uint16_t dstType,
                                uint16_t copyOp,
                                uint16_t rop,
                                uint16_t color,
                                uint16_t width,
                                uint16_t height)
{
    uint16_t workArea1TempHigh, workArea1TempLow, workArea2TempHigh, workArea2TempLow;
    
    while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
    GFX_WaitForGpu();
    
    // store the work area settings temporarily
    workArea1TempLow  = G1W1ADRL;
    workArea1TempHigh = G1W1ADRH;
    workArea2TempLow  = G1W2ADRL;
    workArea2TempHigh = G1W2ADRH;

    GFX_SetWorkArea1(srcAddr);
    GFX_SetWorkArea2(dstAddr);
	
    GFX_RCC_SetSrcOffset(srcOffset);
    GFX_RCC_SetDestOffset(dstOffset);

    GFX_RCC_SetSize(width, height);
	
    if ((copyOp == RCC_TRANSPARENT_COPY) || (copyOp == RCC_SOLID_FILL))
        GFX_RCC_SetColor(color);

    if ((srcType == GFX_DATA_CONTINUOUS) || (srcType > 0))
        srcType = RCC_SRC_ADDR_CONTINUOUS;
    if ((dstType == GFX_DATA_CONTINUOUS) || (dstType > 0))
        dstType = RCC_DEST_ADDR_CONTINUOUS;
	GFX_RCC_StartCopy(copyOp, rop, srcType, dstType);

    while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
    GFX_WaitForGpu();

    // restore the work area settings 
    G1W1ADRL = workArea1TempLow;
    G1W1ADRH = workArea1TempHigh;
    G1W2ADRL = workArea2TempLow;
    G1W2ADRH = workArea2TempHigh;

    return (1);
	
}	

// *****************************************************************************
/*  Function:
    uint16_t DRV_GFX_BitBlt(
                                uint32_t srcAddr,
                                uint32_t dstAddr,
                                uint32_t srcOffset,
                                uint32_t dstOffset,
                                uint16_t srcType,
                                uint16_t dstType,
                                uint16_t copyOp,
                                uint16_t rop,
                                uint16_t color,
                                uint16_t width,
                                uint16_t height)

    Summary:
        This function combines a rectangular area of pixels to another
        rectangular area of pixels with raster operations.

    Description:
        This function combines a rectangular area of pixels to another
        rectangular area of pixels with raster operations.
        There are three copy operations and 15 raster operations.

        The available raster operations are the following:
        - Solid black color fill
        - not (Source or Destination)
        - (not Source) and Destination
        - not Source
        - Source and (not Destination)
        - not Destination
        - Source xor Destination
        - not (Source and Destination)
        - Source and Destination
        - not (Source xor Destination)
        - Destination
        - (not Source) or Destination
        - Source
        - Source or (not Destination)
        - Source or Destination
        - Solid white color fill

*/
// *****************************************************************************
uint16_t DRV_GFX_BitBlt(
                                uint32_t srcAddr,
                                uint32_t dstAddr,
                                uint32_t srcOffset,
                                uint32_t dstOffset,
                                uint16_t srcType,
                                uint16_t dstType,
                                uint16_t copyOp,
                                uint16_t rop,
                                uint16_t color,
                                uint16_t width,
                                uint16_t height)
{
    return (ROPBlockInline( srcAddr, dstAddr,       \
                            srcOffset, dstOffset,   \
                            srcType, dstType,       \
                            copyOp, rop,            \
                            color, width, height));
}	

// *****************************************************************************
/*  Function:
    uint16_t DRV_GFX_BlockOffsetCopy(
                                uint32_t srcAddr,
                                uint32_t dstAddr,
                                uint32_t srcOffset,
                                uint32_t dstOffset,
                                uint16_t width,
                                uint16_t height)

    Summary:
        This function copies a window (a rectangular area) of pixels from one
        location to another location.

    Description:
        This function copies a window (a rectangular area) of pixels from one
        location to another location.

        A window is a rectangular area with the given width and height
        located in a frame buffer. The source and destination
        addresses may or may not be be located in the same frame buffer.

        The source window's location on the frame buffer is defined by the
        offset specified by the srcOffset parameter.

        The destination window's location on the frame buffer is defined by the
        offset specified by the srcOffset parameter.

        The size of the copied area is then defined by the given
        width and height parameters.

*/
// *****************************************************************************
uint16_t DRV_GFX_BlockOffsetCopy(
                                uint32_t srcAddr,
                                uint32_t dstAddr,
                                uint32_t srcOffset,
                                uint32_t dstOffset,
                                uint16_t width,
                                uint16_t height)
{

    return (ROPBlockInline( srcAddr, dstAddr,
                            srcOffset, dstOffset,
                            (GFX_DATA_DISCONTINUOUS), 
                            (GFX_DATA_DISCONTINUOUS),
                            RCC_COPY, RCC_ROP_C,
                            0, width, height));

}


// *****************************************************************************
/*  Function:
    uint16_t DRV_GFX_BlockCopy(
                                uint32_t srcAddr,
                                uint32_t dstAddr,
                                uint16_t srcX,
                                uint16_t srcY,
                                uint16_t dstX,
                                uint16_t dstY,
                                uint16_t width,
                                uint16_t height)

    Summary:
        This function copies a window (a rectangular area) of pixels from one
        location to another location.

    Description:
        This function copies a window (a rectangular area) of pixels from one
        location to another location.

        A window is a rectangular area with the given width and height
        located in a frame buffer. The source and destination
        addresses may or may not be be located in the same frame buffer.

        The source window's location on the frame buffer is defined by the
        left, top corner specified by srcX and srcY.

        The destination window's location on the frame buffer is defined by the
        left, top corner specified by dstX and dstY.

        The size of the copied area is then defined by the given
        width and height parameters.

*/
// *****************************************************************************
uint16_t DRV_GFX_BlockCopy(
                                uint32_t srcAddr,
                                uint32_t dstAddr,
                                uint16_t srcX,
                                uint16_t srcY,
                                uint16_t dstX,
                                uint16_t dstY,
                                uint16_t width,
                                uint16_t height)
{
    uint32_t srcOffset, dstOffset;
    uint16_t  sLeft, sTop, dLeft, dTop, actualWidth, actualHeight;

    // calculate the offset based on the given x and y locations.
    // depending on the screen orientation, the actual x,y locations
    // are calculated.
    #if (DISP_ORIENTATION == 90) || (DISP_ORIENTATION == 270)
        actualHeight = width;
        actualWidth  = height;
    #else
        actualHeight = height;
        actualWidth  = width;
    #endif

    #if (DISP_ORIENTATION == 90)

        sLeft =   srcY;
        sTop  =   DISP_VER_RESOLUTION - srcX - actualHeight;
        dLeft =   dstY;
        dTop  =   DISP_VER_RESOLUTION - dstX - actualHeight;

    #elif (DISP_ORIENTATION == 180)

        sLeft =   DISP_HOR_RESOLUTION - srcX - actualWidth;
        sTop  =   DISP_VER_RESOLUTION - srcY - actualHeight;
        dLeft =   DISP_HOR_RESOLUTION - dstX - actualWidth;
        dTop  =   DISP_VER_RESOLUTION - dstY - actualHeight;

    #elif (DISP_ORIENTATION == 270)

        sTop  =   srcX;
        sLeft =   DISP_HOR_RESOLUTION - srcY - actualWidth;
        dTop  =   dstX;
        dLeft =   DISP_HOR_RESOLUTION - dstY - actualWidth;

    #else

        sLeft = srcX;
        sTop  = srcY;
        dLeft = dstX;
        dTop  = dstY;

    #endif

    // offsets are calculated in terms of pixel count. The effective address
    // gets resolved when the color depth used is taken into account.
    // the color depth is the value programmed into the 
    // PUBPP<2:0>: GPU bits-per-pixel Setting bits
    srcOffset = (uint32_t)((sTop * (uint32_t)DISP_HOR_RESOLUTION) + sLeft);
    dstOffset = (uint32_t)((dTop * (uint32_t)DISP_HOR_RESOLUTION) + dLeft);

    // do the block copy with RCC_COPY mode and RCC_ROP_C which is copy source
    // color value is irrelevant here since we are doing a direct source copy
    DRV_GFX_BitBlt(   srcAddr, dstAddr, srcOffset, dstOffset, 
                GFX_DATA_DISCONTINUOUS, GFX_DATA_DISCONTINUOUS,
                RCC_COPY, RCC_ROP_C, 0, actualWidth, actualHeight);

    return (1);
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
    // adjust (x,y) due to orientation set
    GFX_OrientationPixelAdjust(x,y);

    // wait until FIFO has enought entries for commands
    while(GFX_FreeCommandSpaceCheck(4) == 0);

    GFX_RCC_SetColor(GFX_ColorGet());
    GFX_RCC_SetDestOffset((y * (uint32_t)DISP_HOR_RESOLUTION) + x);
    GFX_RCC_SetSize(1, 1);
    GFX_RCC_StartCopy(  RCC_SOLID_FILL,
                        RCC_ROP_C,
                        RCC_SRC_ADDR_DISCONTINUOUS,
                        RCC_DEST_ADDR_DISCONTINUOUS);
    
    return (GFX_STATUS_SUCCESS);

    /* Note: No need to wait for  complete execution of the command even
             for Blocking Mode. The next commands will be in the queue &
             hence will execute only after the completion of this command.
    */
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
    volatile uint16_t getcolor = 0;
    
    if(x > GFX_MaxXGet() || y > GFX_MaxYGet())
    {
        return ((GFX_COLOR)0); /* return 1 */
    }

    // adjust (x,y) due to orientation set
    GFX_OrientationPixelAdjust(x,y);

    /*
     Wait till previous commands are fully executed.
     This is needed since the destination (work area 2) is modified to 
     point to the color variable of this function. When that changes, 
     prior commands still in the buffer that uses work area 2 will be
     affected.

     */
    while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
    GFX_WaitForGpu();

    GFX_SetWorkArea2((uint16_t)&getcolor);

    GFX_RCC_SetSrcOffset((y * (uint32_t)DISP_HOR_RESOLUTION) + x);
    GFX_RCC_SetDestOffset(0);
    GFX_RCC_SetSize(1, 1);
    GFX_RCC_StartCopy(
                        RCC_COPY,
                        RCC_ROP_C,
                        RCC_SRC_ADDR_DISCONTINUOUS,
                        RCC_DEST_ADDR_DISCONTINUOUS);

    /* Wait till the command is fully executed */
    while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
    GFX_WaitForGpu();
    
    // need to reset the work area back to the display buffer so
    // next rendering functions will be able to render to the right place
#ifndef GFX_CONFIG_DOUBLE_BUFFERING_DISABLE
    GFX_SetWorkArea2(drvGfxDrawBufferBaseAddress);
#else
    GFX_SetWorkArea2(drvGfxFrameBufferBaseAddress);
#endif

    return ((GFX_COLOR)getcolor);
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
    uint16_t    width, height;

#if (DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 180)
    width = numPixels;
    height = 1;
#elif (DISP_ORIENTATION == 90) || (DISP_ORIENTATION == 270)
    height = numPixels;
    width = 1;
#endif

    // Adjust x,y position based on the screen orientation and
    // width and height of the block
    GFX_OrientationLineAdjust(x, y, width, height);

    while(GFX_FreeCommandSpaceCheck(4) == 0);
    
    // source of data is the passed array
    GFX_SetWorkArea1((uint16_t) pPixel);

#ifndef GFX_CONFIG_TRANSPARENT_COLOR_DISABLE
    if (GFX_TransparentColorStatusGet() == GFX_FEATURE_ENABLED)
        GFX_RCC_SetColor(GFX_TransparentColorGet());
#endif

    GFX_RCC_SetSrcOffset(0);
    GFX_RCC_SetDestOffset((y * (uint32_t) DISP_HOR_RESOLUTION) + x);
    GFX_RCC_SetSize(width, height);

#ifndef GFX_CONFIG_TRANSPARENT_COLOR_DISABLE
    if (GFX_TransparentColorStatusGet() == GFX_FEATURE_ENABLED)
    {
        GFX_RCC_StartCopy(  RCC_TRANSPARENT_COPY,
                            RCC_ROP_C,
                            RCC_SRC_ADDR_CONTINUOUS,
                            RCC_DEST_ADDR_DISCONTINUOUS);
    } else
#endif
    {
        GFX_RCC_StartCopy(  RCC_COPY,
                            RCC_ROP_C,
                            RCC_SRC_ADDR_CONTINUOUS,
                            RCC_DEST_ADDR_DISCONTINUOUS);
    }

    // Since this function modified the starting address of work area 1
    // we need to make sure it finishes the copy so succedding functions
    // that modifies starting address of work area 1 will not break
    // the copy.
    while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
    GFX_WaitForGpu();

    // reset the work area to the display buffer
    GFX_SetWorkArea1(drvGfxFrameBufferBaseAddress);

    return numPixels;
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
    uint16_t    width, height;

    while(GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT);
    
    #if (DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 180)
        width   =   numPixels;
        height  =   1; 
    #elif (DISP_ORIENTATION == 90) || (DISP_ORIENTATION == 270)
        height  =   numPixels;
        width   =   1; 
    #endif

    // Adjust x,y position based on the screen orientation and
    // width and height of the block
    GFX_OrientationLineAdjust(x, y, width, height);
   
    // source of data is the passed array
    GFX_SetWorkArea2((uint16_t)pPixel);

    GFX_RCC_SetDestOffset(0);
    GFX_RCC_SetSrcOffset((uint32_t)(y * (uint32_t)DISP_HOR_RESOLUTION) + x);
    GFX_RCC_SetSize(width, height);
    GFX_RCC_StartCopy(  RCC_COPY,
                        RCC_ROP_C,
                        RCC_SRC_ADDR_DISCONTINUOUS,
                        RCC_DEST_ADDR_CONTINUOUS);

    // Since this function modified the starting address of work area 2
    // we need to make sure it finishes the copy so succedding functions
    // that modifies starting address of work area 2 will not break
    // the copy.
    while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
    GFX_WaitForGpu();

    // reset the work area to the display buffer
    GFX_SetWorkArea2(drvGfxFrameBufferBaseAddress);

    return (numPixels);
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
GFX_STATUS GFX_BarDraw(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom)
{

    uint32_t srcOffsetAddr;
    uint16_t width = 0, height = 0;
    uint16_t nLeft, nTop;
#if (DISP_ORIENTATION != 0)
    uint16_t nRight, nBottom;
#endif


#ifndef GFX_CONFIG_ALPHABLEND_DISABLE
    if (GFX_FillStyleGet() == GFX_FILL_STYLE_ALPHA_COLOR)
    {
        if (GFX_AlphaBlendingValueGet() == 0)
        {
            // do nothing
            return (GFX_STATUS_SUCCESS);
        }
        else if (GFX_AlphaBlendingValueGet() != 100)
        {
            return (GFX_BarAlphaDraw(left, top, right, bottom));
        }
    }
#endif

#if (DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 180)
    width   =   right - left + 1;
    height  =   bottom - top + 1;
#elif (DISP_ORIENTATION == 90) || (DISP_ORIENTATION == 270)
    height  =   right - left + 1;
    width   =   bottom - top + 1;
#endif

#if (DISP_ORIENTATION == 90)
    nBottom =   DISP_VER_RESOLUTION - left;
    nLeft   =   top;
    nRight  =   nLeft + width;
    nTop    =   nBottom - height;
#elif (DISP_ORIENTATION == 180)
    nRight  =   DISP_HOR_RESOLUTION - left;
    nBottom =   DISP_VER_RESOLUTION - top;
    nLeft   =   nRight - width;
    nTop    =   nBottom - height;
#elif (DISP_ORIENTATION == 270)
    nRight  =   DISP_HOR_RESOLUTION - top;
    nTop    =   left;
    nBottom =   nTop + height;
    nLeft   =   nRight - width;
#else
    nTop = top;
    nLeft = left;
#endif

    // since we need four commands to render a bar check if there is
    // enough command space on the  queue
    if (GFX_FreeCommandSpaceCheck(4) == 0)
        return (GFX_STATUS_FAILURE);

    srcOffsetAddr = (uint32_t)(nTop * (uint32_t)DISP_HOR_RESOLUTION) + nLeft;
   
    GFX_RCC_SetColor(GFX_ColorGet());
    GFX_RCC_SetDestOffset(srcOffsetAddr);
    GFX_RCC_SetSize(width, height);
    GFX_RCC_StartCopy(  RCC_SOLID_FILL,
                        RCC_ROP_C,
                        RCC_SRC_ADDR_DISCONTINUOUS,
                        RCC_DEST_ADDR_DISCONTINUOUS);

    /* Note: No need to wait for  complete execution of the command even for Blocking Mode. 
             The next commands will be in the queue & hence will execute only after the completion of this command. 
    */

     return (GFX_STATUS_SUCCESS);
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

    GFX_FILL_STYLE fillType;

    fillType = GFX_FillStyleGet();
    GFX_FillStyleSet(GFX_FILL_STYLE_COLOR);

    while(GFX_FreeCommandSpaceCheck(4) == 0);

    GFX_RCC_SetColor(GFX_ColorGet());
    GFX_RCC_SetDestOffset(0);
    GFX_RCC_SetSize(DISP_HOR_RESOLUTION, DISP_VER_RESOLUTION);
    GFX_RCC_StartCopy(  RCC_SOLID_FILL,
                        RCC_ROP_C,
                        RCC_SRC_ADDR_DISCONTINUOUS,
                        RCC_DEST_ADDR_CONTINUOUS);

    GFX_LinePositionSet(0,0);

    GFX_FillStyleSet(fillType);
    
    return (GFX_STATUS_SUCCESS);

}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_StyledLineDraw(
                    uint16_t x1,
                    uint16_t y1,
                    uint16_t x2,
                    uint16_t y2)

    Summary:
        This function renders the thin and thick dotted and dashed lines.
        This is an internal function and should not be called by the application.

    Description:
        This function renders the thin and thick dotted and dashed lines
        (see GFX_LINE_STYLE).

*/
// *****************************************************************************
GFX_STATUS GFX_StyledLineDraw(
                                uint16_t x1,
                                uint16_t y1,
                                uint16_t x2,
                                uint16_t y2)
{
    /*
     * NOTE: This function is an internal function and should not
     *       be called by the application.
     */
    
    typedef enum
    {
        GFX_DA210_LINE_IDLE_STATE = 0x0D80,
        GFX_DA210_LINE_SOLID_DRAW_STATE,
        GFX_DA210_LINE_VERTICAL_DRAW_STATE,
        GFX_DA210_LINE_HORIZONTAL_DRAW_STATE,
        GFX_DA210_LINE_DIAGONAL_DRAW_STATE,
        GFX_DA210_LINE_EXIT_STATE,

    } GFX_DA210_LINE_DRAW_STATE;

    static GFX_DA210_LINE_DRAW_STATE state = GFX_DA210_LINE_IDLE_STATE;
    static int16_t   deltaX, deltaY;
    static int16_t   error, stepErrorLT, stepErrorGE;
    static int16_t   stepX, stepY;
    static int16_t   steep;
    static int16_t   style, type;
    static uint16_t  lineThickness, lineType;
    static uint16_t  left, top, right, bottom;
    static int16_t   temp;

#ifdef GFX_CONFIG_NONBLOCKING_DISABLE
    while(GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
        Nop();
#else
    if (GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
        return (GFX_STATUS_FAILURE);
#endif

    lineType      = (GFX_LineStyleGet() & GFX_LINE_TYPE_MASK);
    lineThickness = (GFX_LineStyleGet() & GFX_LINE_THICKNESS_MASK);

    while(1)
    {
        switch(state)
        {
            case GFX_DA210_LINE_IDLE_STATE:
                if (x1 == x2)
                {
                    // &&&&&&&&&&&&&&&&&&&&&&
                    // Vertical Line case
                    // &&&&&&&&&&&&&&&&&&&&&&

                    left = right = x1;
                    if (y1 > y2)
                    {
                        top    = y2;
                        bottom = y1;
                    }
                    else
                    {
                        top    = y1;
                        bottom = y2;
                    }
                    style = 0;
                    type = 1;
                    state = GFX_DA210_LINE_VERTICAL_DRAW_STATE;
                    break;
                }
                else if (y1 == y2)
                {
                    // &&&&&&&&&&&&&&&&&&&&&&
                    // Horizontal Line case
                    // &&&&&&&&&&&&&&&&&&&&&&

                    top = bottom = y1;
                    if (x1 > x2)
                    {
                        left  = x2;
                        right = x1;
                    }
                    else
                    {
                        left  = x1;
                        right = x2;
                    }
                    style = 0;
                    type = 1;
                    state = GFX_DA210_LINE_HORIZONTAL_DRAW_STATE;
                    break;
                }
                else
                {
                    // &&&&&&&&&&&&&&&&&&&&&&
                    // Diagonal Line case
                    // &&&&&&&&&&&&&&&&&&&&&&
                    stepX = 0;
                    deltaX = x2 - x1;
                    if(deltaX < 0)
                    {
                        deltaX = -deltaX;
                        --stepX;
                    }
                    else
                    {
                        ++stepX;
                    }
                    stepY = 0;
                    deltaY = y2 - y1;
                    if(deltaY < 0)
                    {
                        deltaY = -deltaY;
                        --stepY;
                    }
                    else
                    {
                        ++stepY;
                    }
                    steep = 0;
                    if(deltaX < deltaY)
                    {
                        ++steep;
                        temp = deltaX;
                        deltaX = deltaY;
                        deltaY = temp;

                        temp = stepX;
                        stepX = stepY;
                        stepY = temp;

                        left = y1;
                        top  = x1;
                    }
                    else
                    {
                        left = x1;
                        top  = y1;
                    }
                    right  = x2;
                    bottom = y2;

                    // If the current error greater or equal zero
                    stepErrorGE = deltaX << 1;
                    // If the current error less than zero
                    stepErrorLT = deltaY << 1;
                    // Error for the first pixel
                    error = stepErrorLT - deltaX;
                    style = 0;
                    type = 1;
                    state = GFX_DA210_LINE_DIAGONAL_DRAW_STATE;
                    break;
                }

            case GFX_DA210_LINE_EXIT_STATE:
                state = GFX_DA210_LINE_IDLE_STATE;
                return (GFX_STATUS_SUCCESS);

            case GFX_DA210_LINE_VERTICAL_DRAW_STATE:

                for(temp = top; temp < bottom + 1; temp++)
                {
                    if((++style) == lineType)
                    {
                        type ^= 1;
                        style = 0;
                    }
                    if(type)
                    {
                        if(lineThickness == GFX_LINE_THICKNESS_MASK)
                        {
                            if (GFX_BarDraw(left - 1, temp, left + 1, temp) == GFX_STATUS_FAILURE)
                                return (GFX_STATUS_FAILURE);
                        }
                        else
                            GFX_PixelPut(left, temp);
                    }
                } // end of for loop
                state = GFX_DA210_LINE_EXIT_STATE;
                break;

            case GFX_DA210_LINE_HORIZONTAL_DRAW_STATE:

                for(temp = left; temp < right + 1; temp++)
                {
                    if((++style) == lineType)
                    {
                        type ^= 1;
                        style = 0;
                    }
                    if(type)
                    {
                        if(lineThickness == GFX_LINE_THICKNESS_MASK)
                        {
                            if (GFX_BarDraw(temp, top - 1, temp, top + 1) == GFX_STATUS_FAILURE)
                                return (GFX_STATUS_FAILURE);
                        }
                        else
                            GFX_PixelPut(temp, top);
                    }
                } // end of for loop
                state = GFX_DA210_LINE_EXIT_STATE;
                break;

            case GFX_DA210_LINE_DIAGONAL_DRAW_STATE:
                while(deltaX >= 0)
                {
                    if(type)
                    {
                        if(steep)
                        {
                            if(lineThickness == GFX_LINE_THICKNESS_MASK)
                            {
                                if (GFX_BarDraw(top - 1, left, top + 1, left) == GFX_STATUS_FAILURE)
                                    return (GFX_STATUS_FAILURE);
                            }
                            else
                                GFX_PixelPut(top, left);
                        }
                        else
                        {
                            if(lineThickness == GFX_LINE_THICKNESS_MASK)
                            {
                                if (GFX_BarDraw(left, top - 1, left, top + 1) == GFX_STATUS_FAILURE)
                                    return (GFX_STATUS_FAILURE);
                            }
                            else
                                GFX_PixelPut(left, top);
                        }
                    }

                    if(error >= 0)
                    {
                        top += stepY;
                        error -= stepErrorGE;
                    }
                    left += stepX;
                    error += stepErrorLT;
                    if((++style) == lineType)
                    {
                        type ^= 1;
                        style = 0;
                    }
                    // decrement the difference
                    deltaX--;
                }   // end of while
                state = GFX_DA210_LINE_EXIT_STATE;
                break;

            default:
                // this case should never happen
                state = GFX_DA210_LINE_IDLE_STATE;
                return (GFX_STATUS_SUCCESS);
        } // end of switch
    } // end of while(1)
}

#ifdef USE_DRV_FONT

// *****************************************************************************
/*  Function:
    void GFX_DRV_FontSet(GFX_RESOURCE_HDR *pFont)

    Summary:
        This function sets the current font used when rendering
        strings and characters on the driver layer.

    Description:
        This function sets the current font used when rendering
        strings and characters on the driver layer.

    Precondition:
        None.

    Parameters:
        pFont - pointer to the font to be used.

    Returns:
        The status of the set font action.

    Example:
        None.

*/
// *****************************************************************************
void GFX_DRV_FontSet(GFX_RESOURCE_HDR *pFont)
{
    if ((pFont->type & GFX_MEM_MASK) ==  GFX_RESOURCE_MEMORY_EDS_EPMP )
        // set the font address in the CHRGPU
        GFX_CHR_SetFont(pFont->resource.font.location.extAddress);
}

// *****************************************************************************
/*  Function:
    uint16_t GFX_DRV_TextStringWidthGet(
                                GFX_XCHAR *pString,
                                GFX_RESOURCE_HDR *pFont)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        This function is intended to allow driver level implementation of
        the string length calculation routine. In some cases, hardware
        is present to perform rendering and help useful Graphics Library
        routines. When this is present, we take advantage by implementing
        some primitive layer routines in the driver code.

    Precondition:
        None.

    Parameters:
        pString - Pointer to the string.
        pFont - Pointer to the font selected for the string.

    Returns:
        The width of the string in pixels.

    Example:
        None.

*/
// *****************************************************************************
uint16_t GFX_DRV_TextStringWidthGet(
                                GFX_XCHAR *pString,
                                GFX_RESOURCE_HDR *pFont)
{
    GFX_FONT_SPACE GFX_FONT_HEADER *pHeader;
    uint32_t        edsFontAddr;
    uint16_t        fontFirstChar;
    uint16_t        fontLastChar;
    uint16_t        textWidth;
    uint8_t         temp, TempGPUBpp;
    static GFX_XCHAR    ch, *pStr;
    uint32_t        offset;

    pHeader = (GFX_FONT_SPACE GFX_FONT_HEADER *)&(pFont->resource.font.header);
    fontFirstChar = pHeader->firstChar;
    fontLastChar = pHeader->lastChar;

    edsFontAddr = ((GFX_RESOURCE_HDR*)pFont)->resource.font.location.extAddress;

    textWidth = 0;
    pStr = (GFX_XCHAR*) pString;
    ch = *pStr;
    
    // make GPU operate on 8 bpp so the font table is easier to operate on
    TempGPUBpp = _PUBPP;
    _PUBPP = GFX_8_BPP;

    while((GFX_XCHAR)15 < (GFX_XCHAR)(ch = *pStr++))
    {
        if((GFX_XCHAR)ch < (GFX_XCHAR)fontFirstChar)
            continue;
        if((GFX_XCHAR)ch > (GFX_XCHAR)fontLastChar)
            continue;

        // grab the width of the character
        offset = sizeof(GFX_FONT_HEADER) + 
                    ((  (GFX_XCHAR)ch -
                        (GFX_XCHAR)fontFirstChar) * sizeof(GFX_FONT_GLYPH_ENTRY)
                    );
        while(!DRV_GFX_BitBlt(  edsFontAddr,
                                (uint32_t)(uint16_t)&temp,
                                offset,
                                0,
                                GFX_DATA_CONTINUOUS,
                                GFX_DATA_CONTINUOUS,
                                RCC_COPY, RCC_ROP_C,
                                0,
                                1,
                                1));

        while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
        GFX_WaitForGpu();


        textWidth += temp;
    }
    
    _PUBPP = TempGPUBpp;
    return (textWidth);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_DRV_TextCharRender(GFX_XCHAR ch)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        This function is intended to allow driver level implementation of
        text rendering. This functions sets the font used when rendering text.

        In some cases, hardware is present to perform rendering and help
        useful Graphics Library routines. When this is present, we take
        advantage by implementing some primitive layer routines in the
        driver code.

        This function in Primitive Layer is a dummy function. Actual
        implementation is in the driver layer.  If the driver do not
        implement this function, then this function should never be called.

    Precondition:
        None.

    Parameters:
        ch - Character ID of the character to be rendered.

    Returns:
        Status of the character rendering.

    Example:
        None.

*/
// *****************************************************************************
GFX_STATUS GFX_DRV_TextCharRender(GFX_XCHAR ch)
{

    if (GFX_FreeCommandSpaceCheck(4) == 0)
        return (GFX_STATUS_FAILURE);

    // set the color if the color is being changed
    GFX_CHR_SetFgColor(GFX_ColorGet());
    // set the position and render the character
    GFX_CHR_SetPrintPos (
                            GFX_TextCursorPositionXGet(),
                            GFX_TextCursorPositionYGet()
                        );
    // render the character
    GFX_CHR_PrintChar(ch, CHR_TRANSPARENT);

    // Note: The registers will only reflect the new x,y positions when
    // the character is rendered. When it is on the command FIFO they are
    // not updated. So this is a problem with the text cursor position
    // in hardware.
    GFX_TextCursorPositionSet(DrvGetX(), DrvGetY());

    return (GFX_STATUS_SUCCESS);

}

#endif //#ifdef USE_DRV_FONT


#ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE

// *****************************************************************************
/*  Function:
    void GFX_ImageFlash1BPPDraw(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Renders a 1bpp image stored in internal flash memory.

    Precondition:
        None.

    Parameters:
        left - Horizontal starting position of the image.
        top  - Vertical starting position of the image.
        pImage - Pointer to the image to be rendered.
        pPartialImageData - pointer to the partial image information when
                            the image is partially rendered.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void GFX_ImageFlash1BPPDraw(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData
                           )
{
    register uint8_gfx_image_prog   *flashAddress;
    register uint8_gfx_image_prog   *tempFlashAddress;
    uint8_t                         temp = 0;
    uint16_t                        x, y, yc;
    uint16_t                        palette[2], color;
    uint8_t                         mask;
    uint16_t                        outputSize;
    uint16_t                        xLimit;
    GFX_MCHP_BITMAP_HEADER          bitmapHdr;
    uint16_t                        *pData;
#ifndef GFX_CONFIG_PALETTE_DISABLE
    uint8_t                         *pByteData = NULL; 
#if (DISP_ORIENTATION == 180) || (DISP_ORIENTATION == 90)   
#if (GFX_CONFIG_COLOR_DEPTH == 4) 
    uint8_t                         nibblePos = 0;
#endif
#if (GFX_CONFIG_COLOR_DEPTH == 1)
    uint8_t                         bitPos = 0;
#endif
#endif
#endif

    uint16_t                        addressOffset = 0, adjOffset;
    uint8_t                         maskOffset = 0;     // adjusted offset for partial image rendering

    GFX_ImageHeaderGet(pImage, (GFX_MCHP_BITMAP_HEADER *)&bitmapHdr);

    // Move pointer to image
    flashAddress = pImage->resource.image.location.progByteAddress;

    if (pImage->resource.image.type == MCHP_BITMAP_NORMAL)
    {
#ifndef GFX_CONFIG_PALETTE_DISABLE
        if (GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
        {
            palette[0] = 0;
            palette[1] = 1;
            // address MUST be skipped since the palette is embedded in the image
            // but since the system CLUT is enabled, we will ignore the palette
            flashAddress += 4;
        }
        else
#endif
        {
            palette[0] = *((uint16_gfx_image_prog *)flashAddress);
            flashAddress += 2;
            palette[1] = *((uint16_gfx_image_prog *)flashAddress);
            flashAddress += 2;
        }
    }
    else
    {
        palette[0] = 0;
        palette[1] = 1;
        // palette is stripped so skip the ID
        flashAddress += 2;
    }

    if (bitmapHdr.width & 0x07)
        adjOffset = 1; 
    else
        adjOffset = 0;

    // compute for addressOffset this is the offset needed to jump to the 
    // next line 
    addressOffset = ((bitmapHdr.width >> 3) + adjOffset);

    if(pPartialImageData->width != 0)
    {

        // adjust the flashAddress to the starting pixel location
        // adjust one address if the data is not byte aligned
        flashAddress += ((uint16_gfx_image_prog)(pPartialImageData->yoffset)*((bitmapHdr.width >> 3) + adjOffset));

        // adjust flashAddress for x offset (if xoffset is zero address stays the same)
        flashAddress += ((pPartialImageData->xoffset) >> 3);


        bitmapHdr.height = pPartialImageData->height;
        bitmapHdr.width = pPartialImageData->width;

        // check the bit position of the starting pixel
        maskOffset = pPartialImageData->xoffset & 0x07;
    }
    
    outputSize = bitmapHdr.width;

    // area of the buffer to use is only the area that
    // spans the viewable area
    if ((outputSize > GFX_MaxXGet()+1) ||
        (left + outputSize > GFX_MaxXGet()+1))
    {
        outputSize = GFX_MaxXGet() + 1 - left;
    }
		
    while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
    GFX_WaitForGpu();
    GFX_SetWorkArea1((uint16_t)GFX_RenderingBufferGet());
    GFX_RCC_SetSrcOffset(0);

    yc = top;
    // store current line data address 
    tempFlashAddress = flashAddress;

    // some pre-calculations
    if (outputSize <= GFX_MaxXGet() - left)
        xLimit = outputSize;
    else
        xLimit = GFX_MaxXGet() - left + 1;

#ifndef GFX_CONFIG_TRANSPARENT_COLOR_DISABLE
    if (GFX_TransparentColorStatusGet() == GFX_FEATURE_ENABLED)
        GFX_RCC_SetColor(GFX_TransparentColorGet());
#endif

    for(y = 0; (y < bitmapHdr.height) && (yc <= GFX_MaxYGet()); y++)
    {
        // get flash address location of current line being processed
        flashAddress = tempFlashAddress;
        if (maskOffset)
        {
            // grab the first set of data then set up the mask
            // to the starting pixel
            temp = *flashAddress++;
            mask = 0x01 << maskOffset;
        }
        else
            mask = 0;

		// get the location of the line buffer 
#if ((DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 270))
        pData =  (uint16_t*)(GFX_RenderingBufferGet());
#ifndef GFX_CONFIG_PALETTE_DISABLE
        if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
    	{
            pByteData = (uint8_t*) GFX_RenderingBufferGet();
        }
#endif
#elif ((DISP_ORIENTATION == 180) || (DISP_ORIENTATION == 90))
        pData = (uint16_t*)(GFX_RenderingBufferGet() + (outputSize - 1));
           	
#ifndef GFX_CONFIG_PALETTE_DISABLE
        if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
        {
            pByteData = (uint8_t *)(GFX_RenderingBufferGet());
            #if (GFX_CONFIG_COLOR_DEPTH == 8)
                pByteData += outputSize;
                pByteData--;
            #endif

            #if (GFX_CONFIG_COLOR_DEPTH == 4)

                pByteData += outputSize / 2;
                nibblePos = (outputSize & 0x01);
                if(nibblePos == 0)
                {
                    pByteData--;
                }
            #endif

            #if (GFX_CONFIG_COLOR_DEPTH == 1)

                pByteData += (outputSize / 8) ;
                bitPos = (outputSize & 0x07);
                if(bitPos != 0)
                {
                    // why -1, since bit position starts
                    // at 0 (or first bit is at 0 position)
                    bitPos -= 1;
                }
                else
                {
                    bitPos = 7;
                    pByteData--;
                }
            #endif
                
        }
#endif
#endif

        // check if previous RCCGPU copies are done so we do
        // not overwrite the buffers still being copied.
        while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
        GFX_WaitForGpu();

        // process the pixels of the current line
        for(x = 0; x < xLimit; x++)
        {            
            // Read 8 pixels from flash
            if(mask == 0)
            {
                temp = *flashAddress++;
                mask = 0x01;
            }

            // Set color
            if(mask & temp)
            {
                color = palette[1];
            }
            else
            {
                color = palette[0];
            }

#if ((DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 270))
#ifndef GFX_CONFIG_PALETTE_DISABLE
            if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
            {
                #if (GFX_CONFIG_COLOR_DEPTH == 8)
                    *pByteData++ = (uint8_t)color;
                #endif

                #if (GFX_CONFIG_COLOR_DEPTH == 4)
                    if(x & 0x01)
                    {
                        *pByteData++ |= color << 4;
                    }else
                    {
                        *pByteData = color & 0x0F;
                    }
                #endif

                #if (GFX_CONFIG_COLOR_DEPTH == 1)
                    {
                        uint8_t pos = (uint8_t)x & 0x07;

                        if(pos == 0)
                            *pByteData = color;
                        else
                            *pByteData |= color << pos;

                        if(pos == 7)
                            pByteData++;
                    }
                #endif
            }
            else
            {
                *pData++ = color;
            }
#else
            *pData++ = color;
#endif
#elif ((DISP_ORIENTATION == 180) || (DISP_ORIENTATION == 90))   
#ifndef GFX_CONFIG_PALETTE_DISABLE
            if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
    	    {
                #if (GFX_CONFIG_COLOR_DEPTH == 8)
                    *pByteData-- = (uint8_t)color;
                #endif

                #if (GFX_CONFIG_COLOR_DEPTH == 4)
                    if (nibblePos == 1)
                    {
                        *pByteData = ((*pByteData & 0xF0) | (color & 0x0F));
                        pByteData--;
                        nibblePos = 0;
                    }else
                    {
                        *pByteData = color << 4;
                        nibblePos = 1;
                    }
                #endif

                #if (GFX_CONFIG_COLOR_DEPTH == 1)
                    {
                        *pByteData &= ~(1 << bitPos);
                        *pByteData |= (color << bitPos);

                        if (bitPos == 0)
                        {
                            bitPos = 7;
                            pByteData--;
                        }
                        else
                        {
                            bitPos--;
                        }
                    }
                #endif
            }
            else
            {
                *pData-- = color;
            }
#else
            *pData-- = color;
#endif
#endif			

            mask <<= 1;

        } // for(x = 0; x < xLimit; x++)

        if (GFX_RenderToDisplayBufferDisableFlagGet() == 1)
        {
            return;
        }

#if ((DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 180))
        GFX_RCC_SetSize(x, 1);
#else	
        GFX_RCC_SetSize(1, x);
#endif
			
#if (DISP_ORIENTATION == 0) 
        GFX_RCC_SetDestOffset((yc * (uint32_t)DISP_HOR_RESOLUTION) + left);
#elif (DISP_ORIENTATION == 90)
        GFX_RCC_SetDestOffset(((GFX_MaxXGet() + 1 - (left + outputSize)) * (uint32_t)DISP_HOR_RESOLUTION) + yc);
#elif (DISP_ORIENTATION == 180)
        GFX_RCC_SetDestOffset(((GFX_MaxYGet() - yc) * (uint32_t)DISP_HOR_RESOLUTION) + GFX_MaxXGet() + 1 - (left+outputSize));
#elif (DISP_ORIENTATION == 270)
        GFX_RCC_SetDestOffset((left * (uint32_t)DISP_HOR_RESOLUTION) + (GFX_MaxYGet() - yc));
#endif	 

#ifndef GFX_CONFIG_TRANSPARENT_COLOR_DISABLE
        if (GFX_TransparentColorStatusGet() == GFX_FEATURE_ENABLED)
        {
            GFX_RCC_StartCopy(  RCC_TRANSPARENT_COPY,
                                RCC_ROP_C,
                                RCC_SRC_ADDR_CONTINUOUS,
                                RCC_DEST_ADDR_DISCONTINUOUS);
        } 		    
        else
#endif	    
        {
            GFX_RCC_StartCopy(  RCC_COPY,
                                RCC_ROP_C,
                                RCC_SRC_ADDR_CONTINUOUS,
                                RCC_DEST_ADDR_DISCONTINUOUS);
        } 

        yc++;
        tempFlashAddress += addressOffset;

    } // for(y = 0; (y < bitmapHdr.height) && (yc < GFX_MaxYGet()); y++)

	// make sure the GPUs are done before exiting
    while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
	GFX_WaitForGpu();

    GFX_SetWorkArea1(drvGfxFrameBufferBaseAddress);

}

#if (GFX_CONFIG_COLOR_DEPTH >= 4)

// *****************************************************************************
/*  Function:
    void GFX_ImageFlash4BPPDraw(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Renders a 4bpp image stored in internal flash memory.

    Precondition:
        None.

    Parameters:
        left - Horizontal starting position of the image.
        top  - Vertical starting position of the image.
        pImage - Pointer to the image to be rendered.
        pPartialImageData - pointer to the partial image information when
                            the image is partially rendered.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void GFX_ImageFlash4BPPDraw(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)
{
    register uint8_gfx_image_prog   *flashAddress;
    register uint8_gfx_image_prog   *tempFlashAddress;
    uint16_t                        x, y, yc;
    GFX_MCHP_BITMAP_HEADER          bitmapHdr;
    uint8_t                         temp = 0;
    uint16_t                        palette[16], color;
    uint16_t                        *pData;
    uint16_t                        outputSize;
    uint16_t                        xLimit;
    uint16_t                        counter;
#ifndef GFX_CONFIG_PALETTE_DISABLE
    uint8_t            	            *pByteData = NULL;
#if (GFX_CONFIG_COLOR_DEPTH == 4) && ((DISP_ORIENTATION == 180) || (DISP_ORIENTATION == 90))   
    uint8_t                         nibblePos = 0;
#endif
#endif
    uint16_t                        nibbleOffset = 0;
    uint16_t                        addressOffset = 0, adjOffset;

    GFX_ImageHeaderGet(pImage, (GFX_MCHP_BITMAP_HEADER *)&bitmapHdr);

    // Move pointer to image data
    flashAddress = pImage->resource.image.location.progByteAddress;

    // Read palette
    if (pImage->resource.image.type == MCHP_BITMAP_NORMAL)
    {
#ifndef GFX_CONFIG_PALETTE_DISABLE
        if (GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
        {
            // address MUST be skipped since the palette is embedded in the image
            // but since the system CLUT is enabled, we will ignore the palette
            flashAddress += 32;
        }
        else
#endif
        {
            for(counter = 0; counter < 16; counter++)
            {
                palette[counter] = *((uint16_gfx_image_prog *)flashAddress);
                flashAddress += 2;
            }
        }

    }
    else
    {
        // palette is stripped so skip the ID
        flashAddress += 2;
    }

    
    if (bitmapHdr.width & 0x01)
        adjOffset = 1; 
    else
        adjOffset = 0;

    // compute for addressOffset this is the offset needed to jump to the 
    // next line 
    addressOffset = ((bitmapHdr.width >> 1) + adjOffset);

    if(pPartialImageData->width != 0)
    {
        // check the bit position of the starting pixel
        nibbleOffset = pPartialImageData->xoffset & 0x01;

        // adjust the flashAddress to the starting pixel location
        // adjust one address if the data is not byte aligned
        flashAddress += ((uint16_gfx_image_prog)(pPartialImageData->yoffset)*((bitmapHdr.width >> 1) + adjOffset));

        // adjust flashAddress for x offset (if xoffset is zero address stays the same)
        flashAddress += ((pPartialImageData->xoffset - nibbleOffset) >> 1);

        bitmapHdr.height = pPartialImageData->height;
        bitmapHdr.width = pPartialImageData->width + nibbleOffset;

    }

    outputSize = bitmapHdr.width;

    // area of the buffer to use is only the area that
    // spans the viewable area
    if ((outputSize > (GFX_MaxXGet()+1)) || ((left + outputSize) > (GFX_MaxXGet()+1)))
        outputSize = GFX_MaxXGet() + 1 - left;

    while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
    GFX_WaitForGpu();

    GFX_SetWorkArea1((uint16_t)GFX_RenderingBufferGet());

#if ((DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 270))
    GFX_RCC_SetSrcOffset(nibbleOffset);
#elif ((DISP_ORIENTATION == 180) || (DISP_ORIENTATION == 90))   
    GFX_RCC_SetSrcOffset(0);
#endif    

    yc = top;

    // some pre-calculations
    if (outputSize <= (GFX_MaxXGet() - left))
        xLimit = outputSize;
    else
        xLimit = GFX_MaxXGet() - left + 1;

    // store current line data address 
    tempFlashAddress = flashAddress;

#ifndef GFX_CONFIG_TRANSPARENT_COLOR_DISABLE
    if (GFX_TransparentColorStatusGet() == GFX_FEATURE_ENABLED)
        GFX_RCC_SetColor(GFX_TransparentColorGet());
#endif	    

    for(y = 0; (y < bitmapHdr.height) && (yc <= GFX_MaxYGet()); y++)
    {
        // get flash address location of current line being processed
        flashAddress = tempFlashAddress;

        // get the location of the line buffer
#if ((DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 270))
        pData =  (uint16_t*)(GFX_RenderingBufferGet());
#ifndef GFX_CONFIG_PALETTE_DISABLE
        if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
        {
            pByteData = (uint8_t*)GFX_RenderingBufferGet();
        }    
#endif
#elif ((DISP_ORIENTATION == 180) || (DISP_ORIENTATION == 90))   
        pData =  (uint16_t*)(GFX_RenderingBufferGet() + (outputSize - 1));

#ifndef GFX_CONFIG_PALETTE_DISABLE
        if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
        {
            pByteData = (uint8_t*)(GFX_RenderingBufferGet());

            #if (GFX_CONFIG_COLOR_DEPTH == 8)
                pByteData += outputSize;
                pByteData--;
            #endif

            #if (GFX_CONFIG_COLOR_DEPTH == 4)

                pByteData += outputSize / 2;
                nibblePos = (outputSize & 0x01);
                if(nibblePos == 0)
                {
                    pByteData--;
                }
            #endif
        }
#endif
#endif

        // check if previous RCCGPU copies are done so we do
        // not overwrite the buffers still being copied.
        while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
        GFX_WaitForGpu();

        // process the pixels of the current line
        for(x = 0; x < xLimit; x++)
        {
            // take one pixel data when needed 
            if ((x & 0x01) == 0)
            	temp = *flashAddress++; 

#ifndef GFX_CONFIG_PALETTE_DISABLE
            if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
                color = temp >> ((x&0x0001)*4) & 0x000F;
            else 
                color = palette[(temp >> ((x&0x0001)*4)) & 0x000F];
#else
            color = palette[(temp >> ((x&0x0001)*4)) & 0x000F];
#endif	            

#if ((DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 270))
            #ifndef GFX_CONFIG_PALETTE_DISABLE
                if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
                {
                    #if (GFX_CONFIG_COLOR_DEPTH == 8)
                        *pByteData++ = (uint8_t)color;
                    #endif

                    #if (GFX_CONFIG_COLOR_DEPTH == 4)
                        if(x & 0x01)
                        {
                            *pByteData++ |= color << 4;
                        }
                        else
                        {
                            *pByteData = color & 0x0F;
                        }
                    #endif
                }
                else
                {
                    *pData++ = color;
                }
            #else
                *pData++ = color;
            #endif
#elif ((DISP_ORIENTATION == 180) || (DISP_ORIENTATION == 90))   
            #ifndef GFX_CONFIG_PALETTE_DISABLE
                if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
                {
                    #if (GFX_CONFIG_COLOR_DEPTH == 8)
                        *pByteData-- = (uint8_t)color;
                    #endif

                    #if (GFX_CONFIG_COLOR_DEPTH == 4)
                        if (nibblePos == 1)
                        {
                            *pByteData = ((*pByteData & 0xF0) | (color & 0x0F));
                            pByteData--;
                            nibblePos = 0;
                        }else
                        {
                            *pByteData = color << 4;
                            nibblePos = 1;
                        }
                    #endif
                }
                else
                {
                    *pData-- = color;
                }
            #else
                *pData-- = color;
            #endif
#endif			           	    
       	    		
        } // for(x = 0; x < xLimit; x++)

        if (GFX_RenderToDisplayBufferDisableFlagGet() == 1)
        {
            return;
        }

#if ((DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 180))
        GFX_RCC_SetSize((x - nibbleOffset), 1);
#else	
        GFX_RCC_SetSize(1, (x - nibbleOffset));
#endif
	
#if (DISP_ORIENTATION == 0)
        GFX_RCC_SetDestOffset((yc * (uint32_t)DISP_HOR_RESOLUTION) + left);
#elif (DISP_ORIENTATION == 90)
        GFX_RCC_SetDestOffset(((GFX_MaxXGet() + 1 - (left + outputSize - nibbleOffset)) * (uint32_t)DISP_HOR_RESOLUTION) + yc);
#elif (DISP_ORIENTATION == 180)
        GFX_RCC_SetDestOffset(((GFX_MaxYGet() - yc) * (uint32_t)DISP_HOR_RESOLUTION) + GFX_MaxXGet() + 1 - (left + outputSize - nibbleOffset));
#elif (DISP_ORIENTATION == 270)
        GFX_RCC_SetDestOffset((left * (uint32_t)DISP_HOR_RESOLUTION) + (GFX_MaxYGet() - yc));
#endif

#ifndef GFX_CONFIG_TRANSPARENT_COLOR_DISABLE
        if (GFX_TransparentColorStatusGet() == GFX_FEATURE_ENABLED)
        {
            GFX_RCC_StartCopy(  RCC_TRANSPARENT_COPY,
                                RCC_ROP_C,
                                RCC_SRC_ADDR_CONTINUOUS,
                                RCC_DEST_ADDR_DISCONTINUOUS);
        } 		    
        else
#endif
        {
            GFX_RCC_StartCopy(  RCC_COPY,
                                RCC_ROP_C,
                                RCC_SRC_ADDR_CONTINUOUS,
                                RCC_DEST_ADDR_DISCONTINUOUS);
        }

        // update the vertical position counter
        yc++;

        // adjust for the next line data address 
        tempFlashAddress += addressOffset;
					
    } // for(y = 0; (y < bitmapHdr.height) && (yc < GFX_MaxYGet()); y++)

    // make sure the GPUs are done before exiting
    while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
    GFX_WaitForGpu();

    GFX_SetWorkArea1(drvGfxFrameBufferBaseAddress);

}
#endif //#if (COLOR_DEPTH >= 4)

#if (GFX_CONFIG_COLOR_DEPTH >= 8)

// *****************************************************************************
/*  Function:
    void GFX_ImageFlash8BPPDraw(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Renders a 8bpp image stored in internal flash memory.

    Precondition:
        None.

    Parameters:
        left - Horizontal starting position of the image.
        top  - Vertical starting position of the image.
        pImage - Pointer to the image to be rendered.
        pPartialImageData - pointer to the partial image information when
                            the image is partially rendered.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void GFX_ImageFlash8BPPDraw(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)
{
    register uint8_gfx_image_prog   *flashAddress;
    register uint8_gfx_image_prog   *tempFlashAddress;
    GFX_MCHP_BITMAP_HEADER          bitmapHdr;
    uint16_t                        x, y, yc;
    uint8_t                         temp = 0;
    uint16_t                        palette[256];
    uint16_t                        counter;
    uint16_t                        outputSize;
    uint16_t                        xLimit;
    uint16_t                        *pData;

    #ifndef GFX_CONFIG_PALETTE_DISABLE
    uint8_t            	            *pByteData = NULL; // To supress warning 
    #endif

    uint16_t                        addressOffset;

    GFX_ImageHeaderGet(pImage, (GFX_MCHP_BITMAP_HEADER *)&bitmapHdr);

    // Move pointer to image data
    flashAddress = pImage->resource.image.location.progByteAddress;

    if (pImage->resource.image.type == MCHP_BITMAP_NORMAL)
    {
#ifndef GFX_CONFIG_PALETTE_DISABLE
        if (GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
        {
            // address MUST be skipped since the palette is embedded in the image
            // but since the system CLUT is enabled, we will ignore the palette
            flashAddress += 512;
        }
        else
#endif
        {
            for(counter = 0; counter < 256; counter++)
            {
                palette[counter] = *((uint16_gfx_image_prog *)flashAddress);
                flashAddress += 2;
            }
        }
    }
    else
    {
        // palette is stripped so skip the ID
        flashAddress += 2;
    }

    // compute for addressOffset this is the offset needed to jump to the 
    // next line 
    addressOffset = bitmapHdr.width;

    if(pPartialImageData->width != 0)
    {
        // adjust the flashAddress to the starting pixel location
        // adjust one address if the data is not byte aligned
        flashAddress += ((uint16_gfx_image_prog)(pPartialImageData->yoffset)*(bitmapHdr.width));

        // adjust flashAddress for x offset (if xoffset is zero address stays the same)
        flashAddress += (pPartialImageData->xoffset);

        bitmapHdr.height = pPartialImageData->height;
        bitmapHdr.width = pPartialImageData->width;

    }

    outputSize = bitmapHdr.width;

    // area of the buffer to use is only the area that
    // spans the viewable area
    if ((outputSize > GFX_MaxXGet()+1) || (left + outputSize > GFX_MaxXGet()+1))
        outputSize = GFX_MaxXGet() + 1 - left;

    while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
    GFX_WaitForGpu();
    GFX_SetWorkArea1((uint16_t)GFX_RenderingBufferGet());
    GFX_RCC_SetSrcOffset(0);
    
    yc = top;

    // store current line data address 
    tempFlashAddress = flashAddress;

    // some pre-calculations
    if (outputSize <= GFX_MaxXGet() - left)
        xLimit = outputSize;
    else
        xLimit = GFX_MaxXGet() - left + 1;

#ifndef GFX_CONFIG_TRANSPARENT_COLOR_DISABLE
    if (GFX_TransparentColorStatusGet() == GFX_FEATURE_ENABLED)
        GFX_RCC_SetColor(GFX_TransparentColorGet());
#endif

    for(y = 0; (y < bitmapHdr.height) && (yc <= GFX_MaxYGet()); y++)
    {
        // get flash address location of current line being processed
        flashAddress = tempFlashAddress;

        // get the location of the line buffer
#if ((DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 270))
        pData =  (uint16_t*)(GFX_RenderingBufferGet());
        #ifndef GFX_CONFIG_PALETTE_DISABLE
            if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
            {
                pByteData = (uint8_t*)GFX_RenderingBufferGet();
            }    
        #endif

#elif ((DISP_ORIENTATION == 180) || (DISP_ORIENTATION == 90))   
	    pData =  (uint16_t*)(GFX_RenderingBufferGet() + (outputSize - 1));

        #ifndef GFX_CONFIG_PALETTE_DISABLE
            if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
            {
                pByteData = (uint8_t*)GFX_RenderingBufferGet();
                pByteData += (outputSize - 1);
            }
        #endif
#endif

        // check if previous RCCGPU copies are done so we do
        // not overwrite the buffers still being copied.
        while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
    	GFX_WaitForGpu();
    
    	// process the pixels of the current line
        for(x = 0; x < xLimit; x++)
        {
    
            // get pixel data
            temp = *flashAddress++; 

#if ((DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 270))
            #ifndef GFX_CONFIG_PALETTE_DISABLE
                if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
                {
                    *pByteData++ = (uint8_t)temp;
                }
                else
                {
                    *pData++ = palette[temp];
                }
            #else
                *pData++ = palette[temp];
            #endif
#elif ((DISP_ORIENTATION == 180) || (DISP_ORIENTATION == 90))   
            #ifndef GFX_CONFIG_PALETTE_DISABLE
                if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
                {
                    *pByteData-- = (uint8_t)temp;
                }
                else
                {
                    *pData-- = palette[temp];
                }
            #else
                *pData-- = palette[temp];
            #endif
#endif			
        } // for(x = 0; x < xLimit; x++)

        if (GFX_RenderToDisplayBufferDisableFlagGet() == 1)
        {
            return;
        }

#if ((DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 180))
    GFX_RCC_SetSize(x, 1);
#else	
    GFX_RCC_SetSize(1, x);
#endif

#if (DISP_ORIENTATION == 0) 
        GFX_RCC_SetDestOffset((yc * (uint32_t)DISP_HOR_RESOLUTION) + left);
#elif (DISP_ORIENTATION == 90)
        GFX_RCC_SetDestOffset(((GFX_MaxXGet() + 1 - (left + outputSize)) * (uint32_t)DISP_HOR_RESOLUTION) + yc);
#elif (DISP_ORIENTATION == 180)
        GFX_RCC_SetDestOffset(((GFX_MaxYGet() - yc) * (uint32_t)DISP_HOR_RESOLUTION) + GFX_MaxXGet() + 1 - (left+outputSize));
#elif (DISP_ORIENTATION == 270)
        GFX_RCC_SetDestOffset((left * (uint32_t)DISP_HOR_RESOLUTION) + (GFX_MaxYGet() - yc));
#endif
			
#ifndef GFX_CONFIG_TRANSPARENT_COLOR_DISABLE
        if (GFX_TransparentColorStatusGet() == GFX_FEATURE_ENABLED)
    	{
            GFX_RCC_StartCopy(  RCC_TRANSPARENT_COPY,
                                RCC_ROP_C,
                                RCC_SRC_ADDR_CONTINUOUS,
                                RCC_DEST_ADDR_DISCONTINUOUS);
        }
        else
#endif
        {
            GFX_RCC_StartCopy(  RCC_COPY,
                                RCC_ROP_C,
                                RCC_SRC_ADDR_CONTINUOUS,
                                RCC_DEST_ADDR_DISCONTINUOUS);
        } 
    
        yc++;

        // adjust for the next line data address 
        tempFlashAddress += (addressOffset);

    } // for(y = 0; (y < bitmapHdr.height) && (yc < GFX_MaxYGet()); y++)

    // make sure the GPUs are done before exiting
    while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
    GFX_WaitForGpu();

    GFX_SetWorkArea1(drvGfxFrameBufferBaseAddress);

}

#endif //#if (COLOR_DEPTH >= 8)
#endif // #ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE

#ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE

// *****************************************************************************
/*  Function:
    void GFX_ImageExternal1BPPDraw(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Renders a 1bpp image stored in external memory.

    Precondition:
        None.

    Parameters:
        left - Horizontal starting position of the image.
        top  - Vertical starting position of the image.
        pImage - Pointer to the image to be rendered.
        pPartialImageData - pointer to the partial image information when
                            the image is partially rendered.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void GFX_ImageExternal1BPPDraw(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)
{
    register uint32_t       memOffset = 0;
    GFX_MCHP_BITMAP_HEADER  bitmapHdr;
    uint8_t                 lineBuffer[((GFX_MaxXGet() + (uint32_t) 1) / 8) + 1];
    uint16_t                palette[2], color;
    uint8_t                 *pData; 
    uint16_t                outputSize;
    uint16_t                *pBufAddr, *pData2;
    uint16_t                lineBuffer2[(GFX_MaxXGet() + (uint32_t) 1)];
    uint16_t                xLimit;

    uint8_t                 mask, temp = 0;
    uint16_t                x, y;
    uint16_t                yc;
#ifndef GFX_CONFIG_PALETTE_DISABLE
    uint8_t            	    *pByteData = NULL; 
#if (DISP_ORIENTATION == 180) || (DISP_ORIENTATION == 90)   
#if (GFX_CONFIG_COLOR_DEPTH == 4) 
    uint8_t                 nibblePos = 0;
#endif
#if (GFX_CONFIG_COLOR_DEPTH == 1)
    uint8_t                 bitPos = 0;
#endif
#endif
#endif

    uint8_t                 maskOffset = 0;     // adjusted offset for partial image rendering
    uint16_t                addressOffset = 0, adjOffset;

    GFX_ImageHeaderGet(pImage, (GFX_MCHP_BITMAP_HEADER *)&bitmapHdr);

    if (pImage->resource.image.type == MCHP_BITMAP_NORMAL)
    {
#ifndef GFX_CONFIG_PALETTE_DISABLE
        if (GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
        {
            palette[0] = 0;
            palette[1] = 1;
        }
        else
#endif        
        {
            // Get palette (2 entries)
            GFX_ExternalResourceCallback(pImage, 0, 2 * sizeof(uint16_t), palette);
        }
        // Set offset to the image data
        memOffset = 2 * sizeof(uint16_t);
    }
    else
    {
        palette[0] = 0;
        palette[1] = 1;

        // palette is stripped so skip the ID
        memOffset = sizeof(uint16_t);

    }

    // Get the offset adjustment when the width is not divisible by 8
    if (bitmapHdr.width & 0x07)
        adjOffset = 1;
    else
        adjOffset = 0;

    // compute for addressOffset this is the offset needed to jump to the next line
    addressOffset = ((bitmapHdr.width >> 3) + adjOffset);
	
    if(pPartialImageData->width != 0)
    {
        // adjust to the location of the starting data for partial image rendering 
        memOffset += ((uint32_t)pPartialImageData->yoffset*addressOffset);
        memOffset += (pPartialImageData->xoffset>>3);

        bitmapHdr.height = pPartialImageData->height;
        bitmapHdr.width = pPartialImageData->width;

        // check the bit position of the starting pixel
        maskOffset = pPartialImageData->xoffset & 0x07;
    }

    outputSize = bitmapHdr.width;

    // area of the buffer to use is only the area that
    // spans the viewable area
    if ((outputSize > GFX_MaxXGet()+1) || (left + outputSize > GFX_MaxXGet()+1))
        outputSize = GFX_MaxXGet() + 1 - left;

    pBufAddr = lineBuffer2;

    while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
    GFX_WaitForGpu();
    GFX_SetWorkArea1((uint16_t)pBufAddr);
    GFX_RCC_SetSrcOffset(0);

    // some pre-calculations
    if (outputSize <= GFX_MaxXGet() - left)
        xLimit = outputSize;
    else
        xLimit = GFX_MaxXGet() - left + 1;

    yc = top;

#ifndef GFX_CONFIG_TRANSPARENT_COLOR_DISABLE
    if (GFX_TransparentColorStatusGet() == GFX_FEATURE_ENABLED)
        GFX_RCC_SetColor(GFX_TransparentColorGet());
#endif	   

    for(y = 0; (y < bitmapHdr.height) && (yc <= GFX_MaxYGet()); y++)
    {
	    // Get line
	    GFX_ExternalResourceCallback(pImage, memOffset, bitmapHdr.width, lineBuffer);
    	memOffset += addressOffset;
	    pData = lineBuffer;

        if (maskOffset)
        {
            // grab the first set of data then set up the mask to the starting pixel
            temp = *pData++;
            mask = 0x01 << maskOffset;
        }
        else
            mask = 0;
    	
        // get the location of the line buffer 
#if ((DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 270))
        pData2 = lineBuffer2;
        #ifndef GFX_CONFIG_PALETTE_DISABLE
            if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
    	    {
                pByteData = (uint8_t*)lineBuffer2;
            }
        #endif
#elif ((DISP_ORIENTATION == 180) || (DISP_ORIENTATION == 90))   
        pData2 = &lineBuffer2[outputSize-1];
        #ifndef GFX_CONFIG_PALETTE_DISABLE
            if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
            {
                pByteData = (uint8_t *)lineBuffer2;

                #if (GFX_CONFIG_COLOR_DEPTH == 8)
                    pByteData += outputSize;
                    pByteData--;
                #endif

                #if (GFX_CONFIG_COLOR_DEPTH == 4)

                    pByteData += outputSize / 2;
                    nibblePos = (outputSize & 0x01);
                    if(nibblePos == 0)
                    {
                        pByteData--;
                    }
                #endif

                #if (GFX_CONFIG_COLOR_DEPTH == 1)

                    pByteData += (outputSize / 8) ;
                    bitPos = (outputSize & 0x07);
                    if(bitPos != 0)
                        bitPos -= 1;  // why -1, since bit position starts at 0 (or first bit is at 0 position)    
                    else
                    {
                        bitPos = 7;
                        pByteData--;
                    }				
                #endif
                
            }
        #endif
#endif

        // process the pixels of the current line
        for(x = 0; x < xLimit; x++)
        {

            if (mask == 0)
            {
                temp = *pData++;
                mask = 0x01;
            }
            
            if (mask & temp)
            {
                color = palette[1];
            }
            else
            {
                color = palette[0];
            }

#if ((DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 270))
            #ifndef GFX_CONFIG_PALETTE_DISABLE
                if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
                {
                    #if (GFX_CONFIG_COLOR_DEPTH == 8)
                        *pByteData++ = (uint8_t)color;
                    #endif

                    #if (GFX_CONFIG_COLOR_DEPTH == 4)
                        if(x & 0x01)
                        {
                            *pByteData++ |= color << 4;
                        }else
                        {
                            *pByteData = color & 0x0F;
                        }
                    #endif

                    #if (GFX_CONFIG_COLOR_DEPTH == 1)
                    {
                        uint8_t pos = (uint8_t)x & 0x07;
                            
                        if(pos == 0)
                            *pByteData = color;
                        else
                            *pByteData |= color << pos;

                        if(pos == 7)
                            *pByteData++;
                    }
                    #endif
                }
                else
                {
                    *pData2++ = color;
                }
            #else
                *pData2++ = color;
            #endif
#elif ((DISP_ORIENTATION == 180) || (DISP_ORIENTATION == 90))
            #ifndef GFX_CONFIG_PALETTE_DISABLE
                if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
                {
                    #if (GFX_CONFIG_COLOR_DEPTH == 8)
                        *pByteData-- = (uint8_t)color;
                    #endif

                    #if (GFX_CONFIG_COLOR_DEPTH == 4)
                        if (nibblePos == 1)
                        {
                            *pByteData = ((*pByteData & 0xF0) | (color & 0x0F));
                            pByteData--;
                            nibblePos = 0;
                        }else
                        {
                            *pByteData = color << 4;
                            nibblePos = 1;
                        }
                    #endif
                        
                    #if (GFX_CONFIG_COLOR_DEPTH == 1)
                    {
                        *pByteData &= ~(1 << bitPos);
                        *pByteData |= (color << bitPos);

                        if (bitPos == 0)
                        {
                            bitPos = 7;
                            pByteData--;
                        }
                        else
                        {
                            bitPos--;
                        }
                    }
                    #endif
                }
                else
    	        {
                    *pData2-- = color;
    	        }    
            #else
                *pData2-- = color;
            #endif
#endif

            // Shift to the next pixel 
            mask <<= 1;
        } // for(x = 0; x < xLimit; x++)

        while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
    	GFX_WaitForGpu();

#if ((DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 180))
        GFX_RCC_SetSize(x, 1);
#else	
        GFX_RCC_SetSize(1, x);
#endif

#if (DISP_ORIENTATION == 0) 
        GFX_RCC_SetDestOffset((yc * (uint32_t)DISP_HOR_RESOLUTION) + left);
#elif (DISP_ORIENTATION == 90)
        GFX_RCC_SetDestOffset(((GFX_MaxXGet() + 1 - (left + outputSize)) * (uint32_t)DISP_HOR_RESOLUTION) + yc);
#elif (DISP_ORIENTATION == 180)
        GFX_RCC_SetDestOffset(((GFX_MaxYGet() - yc) * (uint32_t)DISP_HOR_RESOLUTION) + GFX_MaxXGet() + 1 - (left+outputSize));
#elif (DISP_ORIENTATION == 270)
        GFX_RCC_SetDestOffset((left * (uint32_t)DISP_HOR_RESOLUTION) + (GFX_MaxYGet() - yc));
#endif

#ifndef GFX_CONFIG_TRANSPARENT_COLOR_DISABLE
        if (GFX_TransparentColorStatusGet() == GFX_FEATURE_ENABLED)
        {
            GFX_RCC_StartCopy(  RCC_TRANSPARENT_COPY,
                                RCC_ROP_C,
                                RCC_SRC_ADDR_CONTINUOUS,
                                RCC_DEST_ADDR_DISCONTINUOUS);
        } 		    
        else
#endif	    
        {
            GFX_RCC_StartCopy(  RCC_COPY,
                                RCC_ROP_C,
                                RCC_SRC_ADDR_CONTINUOUS,
                                RCC_DEST_ADDR_DISCONTINUOUS);
        }

        // update the vertical position counter
        yc++;

    }  // for(y = 0; (y < bitmapHdr.height) && (yc < GFX_MaxYGet()); y++)
    
    // make sure the GPUs are done before exiting
    while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
    GFX_WaitForGpu();

    GFX_SetWorkArea1(drvGfxFrameBufferBaseAddress);

}

#if (GFX_CONFIG_COLOR_DEPTH >= 4)
// *****************************************************************************
/*  Function:
    void GFX_ImageExternal4BPPDraw(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Renders a 4bpp image stored in external memory.

    Precondition:
        None.

    Parameters:
        left - Horizontal starting position of the image.
        top  - Vertical starting position of the image.
        pImage - Pointer to the image to be rendered.
        pPartialImageData - pointer to the partial image information when
                            the image is partially rendered.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void GFX_ImageExternal4BPPDraw(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)
{
    register uint32_t       memOffset;
    GFX_MCHP_BITMAP_HEADER  bitmapHdr;
    uint8_t                 readLineBuffer[((GFX_MaxXGet() + (uint32_t) 1) / 2) + 1];
    uint16_t                x, y, yc;

    uint8_t                 temp = 0;
    uint16_t                palette[16], color;
    uint8_t                 *pReadData;
    uint16_t                *pData;
    uint16_t                outputSize;
    uint16_t                xLimit;

    uint16_t                lineBuffer[(GFX_MaxXGet() + (uint32_t) 1)];

    int16_t                 byteWidth;

#ifndef GFX_CONFIG_PALETTE_DISABLE
    uint8_t                 *pByteData = NULL;
#if (GFX_CONFIG_COLOR_DEPTH == 4) && ((DISP_ORIENTATION == 180) || (DISP_ORIENTATION == 90))   
    uint8_t                 nibblePos = 0;
#endif
#endif

    uint16_t                nibbleOffset = 0; 
	
    GFX_ImageHeaderGet(pImage, (GFX_MCHP_BITMAP_HEADER *)&bitmapHdr);

    if (pImage->resource.image.type == MCHP_BITMAP_NORMAL)
    {
#ifndef GFX_CONFIG_PALETTE_DISABLE
        if (GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
        {
            // using CLUT ignore the palette
            memOffset = 16 * sizeof(uint16_t);
        }
        else
#endif        
        {
            // Get palette (16 entries)
            GFX_ExternalResourceCallback(pImage, 0, 16 * sizeof(uint16_t), palette);
            // Set offset to the image data
            memOffset = 16 * sizeof(uint16_t);
        }
    }
    else
    {
        // palette is stripped so skip the id
        memOffset = 2;
    }
    
    // Get the image line width in bytes
    byteWidth = (bitmapHdr.width) >> 1;
    if(bitmapHdr.width & 0x01)
        byteWidth++;
	
    if(pPartialImageData->width != 0)
    {
        // check the bit position of the starting pixel
        nibbleOffset = pPartialImageData->xoffset & 0x01;

        // adjust to the location of the starting data for partial image rendering 
        memOffset += ((uint32_t)pPartialImageData->yoffset * byteWidth);
        memOffset += ((pPartialImageData->xoffset - nibbleOffset) >> 1);

        bitmapHdr.height = pPartialImageData->height;
        bitmapHdr.width = pPartialImageData->width + nibbleOffset;

    }

    outputSize = bitmapHdr.width;

    // area of the buffer to use is only the area that
    // spans the viewable area
    if ((outputSize > (GFX_MaxXGet()+1)) || ((left + outputSize) > (GFX_MaxXGet()+1)))
        outputSize = GFX_MaxXGet() + 1 - left;

    while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
    GFX_WaitForGpu();

    GFX_SetWorkArea1((uint16_t)lineBuffer);

#if ((DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 270))
    GFX_RCC_SetSrcOffset(nibbleOffset);
#elif ((DISP_ORIENTATION == 180) || (DISP_ORIENTATION == 90))   
    GFX_RCC_SetSrcOffset(0);
#endif    

    yc = top;

    // some pre-calculations
    if (outputSize <= (GFX_MaxXGet() - left))
        xLimit = outputSize;
    else
        xLimit = GFX_MaxXGet() - left + 1;

#ifndef GFX_CONFIG_TRANSPARENT_COLOR_DISABLE
    if (GFX_TransparentColorStatusGet() == GFX_FEATURE_ENABLED)
        GFX_RCC_SetColor(GFX_TransparentColorGet());
#endif	    

    for(y = 0; (y < bitmapHdr.height) && (yc <= GFX_MaxYGet()); y++)
    {
    	// Get line
        GFX_ExternalResourceCallback(pImage, memOffset, byteWidth, readLineBuffer);
    	memOffset += byteWidth;
    	
        pReadData = readLineBuffer;

#if ((DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 270))
        pData = lineBuffer;
        #ifndef GFX_CONFIG_PALETTE_DISABLE
            if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
            {
                pByteData = (uint8_t*)lineBuffer;
            }
        #endif
#elif ((DISP_ORIENTATION == 180) || (DISP_ORIENTATION == 90))   
        pData = &lineBuffer[outputSize-1];

        #ifndef GFX_CONFIG_PALETTE_DISABLE
            if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
            {
                pByteData = (uint8_t*)lineBuffer;

                #if (GFX_CONFIG_COLOR_DEPTH == 8)
                    pByteData += outputSize;
                    pByteData--;
                #endif

                #if (GFX_CONFIG_COLOR_DEPTH == 4)
                    pByteData += outputSize / 2;
                    nibblePos = (outputSize & 0x01);
                    if(nibblePos == 0)
                    {
                        pByteData--;
                    }
                #endif
            }
        #endif
#endif

        // check if previous RCCGPU copies are done so we do
        // not overwrite the buffers still being copied.
        while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
    	GFX_WaitForGpu();

        // process the pixels of the current line
        for(x = 0; x < xLimit; x++)
        {            
            // take one pixel data when needed 
            if ((x & 0x01) == 0)
            	temp = *pReadData++; 

#ifndef GFX_CONFIG_PALETTE_DISABLE
            if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
                color = temp >> ((x&0x0001)*4) & 0x000F;
            else 
                color = palette[(temp >> ((x&0x0001)*4)) & 0x000F];
#else
            color = palette[(temp >> ((x&0x0001)*4)) & 0x000F];
#endif	            

#if ((DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 270))
            #ifndef GFX_CONFIG_PALETTE_DISABLE
                if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
                {
                    #if (GFX_CONFIG_COLOR_DEPTH == 8)
                        *pByteData++ = (uint8_t)color;
                    #endif

                    #if (GFX_CONFIG_COLOR_DEPTH == 4)
                        if(x & 0x01)
                        {
                            *pByteData++ |= color << 4;
                        } else
                        {
                            *pByteData = color & 0x0F;
                        }
                    #endif
                }
                else
                {
                    *pData++ = color;
                }
            #else
                *pData++ = color;
            #endif
#elif ((DISP_ORIENTATION == 180) || (DISP_ORIENTATION == 90))   
            #ifndef GFX_CONFIG_PALETTE_DISABLE
                if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
                {
                    #if (GFX_CONFIG_COLOR_DEPTH == 8)
                        *pByteData-- = (uint8_t)color;
                    #endif

                    #if (GFX_CONFIG_COLOR_DEPTH == 4)
                        if (nibblePos == 1)
                        {
                            *pByteData = ((*pByteData & 0xF0) | (color & 0x0F));
                            pByteData--;
                            nibblePos = 0;
                        }else
                        {
                            *pByteData = color << 4;
                            nibblePos = 1;
                        }
                    #endif
                }
                else
                {
                    *pData-- = color;
                }
            #else
                *pData-- = color;
            #endif
#endif			           	    
       	    		
        } // for(x = 0; x < xLimit; x++)

#if ((DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 180))
        GFX_RCC_SetSize((x - nibbleOffset), 1);
#else
        GFX_RCC_SetSize(1, x);
#endif
	
#if (DISP_ORIENTATION == 0) 
        GFX_RCC_SetDestOffset((yc * (uint32_t)DISP_HOR_RESOLUTION) + left);
#elif (DISP_ORIENTATION == 90)
        GFX_RCC_SetDestOffset(((GFX_MaxXGet() + 1 - (left + outputSize - nibbleOffset)) * (uint32_t)DISP_HOR_RESOLUTION) + yc);
#elif (DISP_ORIENTATION == 180)
        GFX_RCC_SetDestOffset(((GFX_MaxYGet() - yc) * (uint32_t)DISP_HOR_RESOLUTION) + GFX_MaxXGet() + 1 - (left + outputSize - nibbleOffset));
#elif (DISP_ORIENTATION == 270)
        GFX_RCC_SetDestOffset((left * (uint32_t)DISP_HOR_RESOLUTION) + (GFX_MaxYGet() - yc));
#endif

#ifndef GFX_CONFIG_TRANSPARENT_COLOR_DISABLE
        if (GFX_TransparentColorStatusGet() == GFX_FEATURE_ENABLED)
        {
            GFX_RCC_StartCopy(  RCC_TRANSPARENT_COPY,
                                RCC_ROP_C,
                                RCC_SRC_ADDR_CONTINUOUS,
                                RCC_DEST_ADDR_DISCONTINUOUS);
        } 		    
        else
#endif
        {
            GFX_RCC_StartCopy(  RCC_COPY,
                                RCC_ROP_C,
                                RCC_SRC_ADDR_CONTINUOUS,
                                RCC_DEST_ADDR_DISCONTINUOUS);
        }

        // update the vertical position counter
        yc++;
    }  // for(y = 0; (y < bitmapHdr.height) && (yc < GFX_MaxYGet()); y++)

    // make sure the GPUs are done before exiting
    while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
    GFX_WaitForGpu();

    GFX_SetWorkArea1(drvGfxFrameBufferBaseAddress);

}
#endif //#if (COLOR_DEPTH >= 4)

#if (GFX_CONFIG_COLOR_DEPTH >= 8)
// *****************************************************************************
/*  Function:
    void GFX_ImageExternal8BPPDraw(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Renders a 8bpp image stored in external memory.

    Precondition:
        None.

    Parameters:
        left - Horizontal starting position of the image.
        top  - Vertical starting position of the image.
        pImage - Pointer to the image to be rendered.
        pPartialImageData - pointer to the partial image information when
                            the image is partially rendered.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void __attribute__((weak)) GFX_ImageExternal8BPPDraw(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)
{
    register uint32_t       memOffset;
    GFX_MCHP_BITMAP_HEADER  bitmapHdr;
    uint8_t                 lineBuffer[(GFX_MaxXGet() + (uint32_t) 1)];
    uint16_t                lineBuffer2[(GFX_MaxXGet() + (uint32_t) 1)];

    uint16_t                palette[256];
    uint8_t                 *pData;
    uint16_t                *pData2;
    uint16_t                outputSize, addressOffset;
    uint16_t                xLimit;

    uint8_t                 temp = 0;
    uint16_t                x, y;
    uint16_t                yc;

    #ifndef GFX_CONFIG_PALETTE_DISABLE
    uint8_t                 *pByteData = NULL;
    #endif

    GFX_ImageHeaderGet(pImage, (GFX_MCHP_BITMAP_HEADER *)&bitmapHdr);
    
    if (pImage->resource.image.type == MCHP_BITMAP_NORMAL)
    {
#ifndef GFX_CONFIG_PALETTE_DISABLE
        if (GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
        {
            // using CLUT ignore the palette
            memOffset = 256 * sizeof(uint16_t);
        }
        else
#endif        
        {
            // Get palette (256 entries)
            GFX_ExternalResourceCallback(pImage, 0, 256 * sizeof(uint16_t), palette);
            // Set offset to the image data
            memOffset = 256 * sizeof(uint16_t);
	}
    }
    else
    {
        // palette is stripped so skip the id
        memOffset = 2;
    }

   // compute for addressOffset this is the offset needed to jump to the
    // next line
    addressOffset = bitmapHdr.width;

    if(pPartialImageData->width != 0)
    {
         memOffset += ((uint32_t)pPartialImageData->yoffset*(bitmapHdr.width));
         memOffset += pPartialImageData->xoffset;
         bitmapHdr.height = pPartialImageData->height;
         bitmapHdr.width = pPartialImageData->width;
    }

    outputSize = bitmapHdr.width;

    // area of the buffer to use is only the area that
    // spans the viewable area
    if ((outputSize > GFX_MaxXGet() + 1) || (left + outputSize - 1 > GFX_MaxXGet()))
        outputSize = GFX_MaxXGet() + 1 - left;

    while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
    GFX_WaitForGpu();
    GFX_SetWorkArea1((uint16_t) lineBuffer2);
    GFX_RCC_SetSrcOffset(0);

    yc = top;

    // some pre-calculations
    if (outputSize < GFX_MaxXGet() - left)
        xLimit = outputSize;
    else
        xLimit = GFX_MaxXGet() - left + 1;

#ifndef GFX_CONFIG_TRANSPARENT_COLOR_DISABLE
    if (GFX_TransparentColorStatusGet() == GFX_FEATURE_ENABLED)
        GFX_RCC_SetColor(GFX_TransparentColorGet());
#endif	    

    for(y = 0; (y < bitmapHdr.height) && (yc <= GFX_MaxYGet()); y++)
    {

        // Get line
        GFX_ExternalResourceCallback(   pImage,
                                        memOffset,
                                        bitmapHdr.width,
                                        lineBuffer);
        //memOffset += bitmapHdr.width;
        memOffset += addressOffset;
    	
      	pData = lineBuffer;
        // get the location of the line buffer 
#if ((DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 270))
        pData2 = lineBuffer2;
#elif ((DISP_ORIENTATION == 180) || (DISP_ORIENTATION == 90))   
        pData2 = &lineBuffer2[outputSize-1];
            
        #ifndef GFX_CONFIG_PALETTE_DISABLE
            if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
            {
                pData2 -= outputSize/2;
            }
        #endif
#endif

#ifndef GFX_CONFIG_PALETTE_DISABLE
        if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
        {
            #if ((DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 270))
                pByteData = (uint8_t*)lineBuffer2;
            #elif ((DISP_ORIENTATION == 180) || (DISP_ORIENTATION == 90))
                pByteData = (uint8_t*)lineBuffer2;
                pByteData += (outputSize - 1);
            #endif           	
        }
#endif

        // process the pixels of the current line
        for(x = 0; x < xLimit; x++)
        {
            // take one pixel data 
            temp = *pData++;
                      
#if ((DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 270))
            #ifndef GFX_CONFIG_PALETTE_DISABLE
                if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
                {
                    *pByteData++ = (uint8_t)temp;
                }
                else
                {
                    *pData2++ = palette[temp];
                }
            #else
                *pData2++ = palette[temp];
            #endif
#elif ((DISP_ORIENTATION == 180) || (DISP_ORIENTATION == 90))   
            #ifndef GFX_CONFIG_PALETTE_DISABLE
                if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
                {
                    *pByteData-- = (uint8_t)temp;
                }
                else
                {
                    *pData2-- = palette[temp];
                }
            #else
                *pData2-- = palette[temp];
            #endif
#endif			
        } // for(x = 0; (x < outputSize) && (x + left < GFX_MaxXGet()); x++)

        while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
        GFX_WaitForGpu();

#if ((DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 180))
        GFX_RCC_SetSize(x, 1);
#else
        GFX_RCC_SetSize(1, x);
#endif

#if (DISP_ORIENTATION == 0) 
        GFX_RCC_SetDestOffset((yc * (uint32_t)DISP_HOR_RESOLUTION) + left);
#elif (DISP_ORIENTATION == 90)
        GFX_RCC_SetDestOffset(((GFX_MaxXGet() + 1 - (left + outputSize)) * (uint32_t)DISP_HOR_RESOLUTION) + yc);
#elif (DISP_ORIENTATION == 180)
        GFX_RCC_SetDestOffset(((GFX_MaxYGet() - yc) * (uint32_t)DISP_HOR_RESOLUTION) + GFX_MaxXGet() + 1 - (left+outputSize));
#elif (DISP_ORIENTATION == 270)
        GFX_RCC_SetDestOffset((left * (uint32_t)DISP_HOR_RESOLUTION) + (GFX_MaxYGet() - yc));
#endif

#ifndef GFX_CONFIG_TRANSPARENT_COLOR_DISABLE
        if (GFX_TransparentColorStatusGet() == GFX_FEATURE_ENABLED)
        {
            GFX_RCC_StartCopy(  RCC_TRANSPARENT_COPY,
                                RCC_ROP_C,
                                RCC_SRC_ADDR_CONTINUOUS,
                                RCC_DEST_ADDR_DISCONTINUOUS);
        }
        else
#endif
        {
            GFX_RCC_StartCopy(  RCC_COPY,
                                RCC_ROP_C,
                                RCC_SRC_ADDR_CONTINUOUS,
                                RCC_DEST_ADDR_DISCONTINUOUS);
        } 

        // update the vertical position counter
        yc++;
    }  // for(y = 0; (y < bitmapHdr.height) && (yc < GFX_MaxYGet()); y++)

    // make sure the GPUs are done before exiting
    while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
    GFX_WaitForGpu();

    GFX_SetWorkArea1(drvGfxFrameBufferBaseAddress);

}
#endif //#if (COLOR_DEPTH >= 8)
#endif //#ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE

// *****************************************************************************
/*  Function:
    void GFX_ImageEDS1BPPDraw(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Renders a 1bpp image stored in EDS memory.

    Precondition:
        None.

    Parameters:
        left - Horizontal starting position of the image.
        top  - Vertical starting position of the image.
        pImage - Pointer to the image to be rendered.
        pPartialImageData - pointer to the partial image information when
                            the image is partially rendered.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void __attribute__((weak)) GFX_ImageEDS1BPPDraw(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)
{
    uint8_t             TempGPUBpp;
    uint8_t             lineBuffer[((GFX_MaxXGet() + (uint32_t) 1) / 8) + 1];
    uint16_t            lineBuffer2[(GFX_MaxXGet() + (uint32_t) 1)];

    uint16_t            palette[2], color;
    uint8_t             *pData;
    uint16_t	        *pData2;
    uint32_t 	        offsetCount;
    uint16_t	        outputSize;
    uint16_t	        xLimit;
    GFX_MCHP_BITMAP_HEADER   bitmapHdr;
    uint8_t             mask, temp = 0;
    uint16_t            x, y;
    uint16_t            yc;
    
    uint16_t            lineSize;           // this includes the padding

	#ifndef GFX_CONFIG_PALETTE_DISABLE
    uint8_t            	*pByteData = NULL; 
#if (DISP_ORIENTATION == 180) || (DISP_ORIENTATION == 90)   
#if (GFX_CONFIG_COLOR_DEPTH == 4) 
    uint8_t nibblePos = 0;
#endif
#if (GFX_CONFIG_COLOR_DEPTH == 1)
    uint8_t bitPos = 0;
#endif
#endif
#endif

    GFX_ImageHeaderGet(pImage, (GFX_MCHP_BITMAP_HEADER *)&bitmapHdr);

    if (pImage->resource.image.type == MCHP_BITMAP_NORMAL)
    {
#ifndef GFX_CONFIG_PALETTE_DISABLE
        if (GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
        {
            // using CLUT ignore the palette    
            // 32 is used here since the palette size is 2 words or 32 bits
            offsetCount = 32;
        }
        else
#endif
        {   

            // get the palette
            // make sure there are no pending RCC GPU commands
            while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
            GFX_WaitForGpu();

            GFX_SetWorkArea1(pImage->resource.image.location.extAddress);
            GFX_SetWorkArea2((uint16_t)palette);
            GFX_RCC_SetSrcOffset(0);
            GFX_RCC_SetDestOffset(0);
            GFX_RCC_SetSize(2, 1);
            GFX_RCC_StartCopy(  RCC_COPY,
                                RCC_ROP_C,
                                RCC_SRC_ADDR_CONTINUOUS,
                                RCC_DEST_ADDR_CONTINUOUS);

            // 32 is used here since the palette size is 2 words or 32 bits
            offsetCount = 32;
        }
    }
    else
    {
        // palette is stripped, skip the id
        offsetCount = 16;
    }

        

	// Get the image line width in bytes
    lineSize = bitmapHdr.width;
    // account for padding    
    if (lineSize&0x07)
        lineSize += (8 - (lineSize&0x07));

    if(pPartialImageData->width != 0)
    {
        // adjust to the location of the starting data for partial image rendering 
        offsetCount += ((uint32_t)pPartialImageData->yoffset*lineSize);
        offsetCount += (pPartialImageData->xoffset);

        bitmapHdr.height = pPartialImageData->height;
        bitmapHdr.width = pPartialImageData->width;
    }

    yc = top;

    outputSize = bitmapHdr.width;
	
    // area of the buffer to use is only the area that
    // spans the viewable area
    if ((outputSize > (GFX_MaxXGet()+1)) || (left + outputSize > (GFX_MaxXGet()+1)))
        outputSize = GFX_MaxXGet() + 1 - left;

    // some pre-calculations
    if (outputSize <= GFX_MaxXGet() - left)
        xLimit = outputSize;
    else
        xLimit = GFX_MaxXGet() - left + 1;

#ifndef GFX_CONFIG_TRANSPARENT_COLOR_DISABLE
    if (GFX_TransparentColorStatusGet() == GFX_FEATURE_ENABLED)
        GFX_RCC_SetColor(GFX_TransparentColorGet());
#endif	    

    for(y = 0; (y < bitmapHdr.height) && (yc <= GFX_MaxYGet()); y++)
    {
	    	    
        // copy one line from the source
        // make sure there are no pending RCC GPU commands &
        // make sure the GPUs are not operating since changing the base addresses
        // will break the currently executing GPU command.
        while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
        GFX_WaitForGpu();

        // the image data is 1 nibble per pixel and normally
        // if the data is outside the PIC (in extended EDS) we cannot do
        // GPU operation with less than 8bpp color depth.
        // But since this will be read and the destination is in internal RAM we can use
        // 1bpp on the GPU operation.
        TempGPUBpp = _PUBPP;
        _PUBPP = GFX_1_BPP;

        // copy one line of data to the buffer
        GFX_SetWorkArea1(pImage->resource.image.location.extAddress);
        GFX_SetWorkArea2((uint32_t)(uint16_t)lineBuffer);

        GFX_RCC_SetSrcOffset(offsetCount);
        GFX_RCC_SetDestOffset(0);

        GFX_RCC_SetSize(bitmapHdr.width, 1);
        GFX_RCC_StartCopy(  RCC_COPY,
                            RCC_ROP_C,
                            RCC_SRC_ADDR_CONTINUOUS,
                            RCC_DEST_ADDR_CONTINUOUS);

        // wait until copying is done
        while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
        GFX_WaitForGpu();

        // reset the _PUBPP to what it was
        _PUBPP = TempGPUBpp;

        pData = lineBuffer;
        mask = 0;

        // get the location of the line buffer
#if ((DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 270))
        pData2 = lineBuffer2;
        #ifndef GFX_CONFIG_PALETTE_DISABLE
            if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
            {
                pByteData = (uint8_t*)lineBuffer2;
            }
        #endif
#elif ((DISP_ORIENTATION == 180) || (DISP_ORIENTATION == 90))   
        pData2 = &lineBuffer2[outputSize-1];
           	
        #ifndef GFX_CONFIG_PALETTE_DISABLE
            if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
            {
                pByteData = (uint8_t *)lineBuffer2;

                #if (GFX_CONFIG_COLOR_DEPTH == 8)
                    pByteData += outputSize;
                    pByteData--;
                #endif

                #if (GFX_CONFIG_COLOR_DEPTH == 4)

                    pByteData += outputSize / 2;
                    nibblePos = (outputSize & 0x01);
                    if(nibblePos == 0)
                    {
                        pByteData--;
                    }
                #endif

                #if (GFX_CONFIG_COLOR_DEPTH == 1)

                    pByteData += (outputSize / 8) ;
                    bitPos = (outputSize & 0x07);
                    if(bitPos != 0)
                        bitPos -= 1;  // why -1, since bit position starts at 0 (or first bit is at 0 position)    
                    else
                    {
                        bitPos = 7;
                        pByteData--;
                    }				
                #endif
                
            }
	#endif
#endif

        // process the pixels on the current line
        for(x = 0; x < xLimit; x++)
        {
            if (mask == 0)
            {
                temp = *pData++;
                mask = 0x01;
            }

            if (mask & temp)
            {
                #ifndef GFX_CONFIG_PALETTE_DISABLE
                    if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
	            	color = 1;
                    else
                        color = palette[1];
                #else
                    color = palette[1];
                #endif
            }
            else
            {
#ifndef GFX_CONFIG_PALETTE_DISABLE
                if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
                    color = 0;
               	else 
                    color = palette[0];
#else	
                color = palette[0];
#endif
            }
           
#if ((DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 270))
            #ifndef GFX_CONFIG_PALETTE_DISABLE
                if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
                {
                    #if (GFX_CONFIG_COLOR_DEPTH == 8)
                        *pByteData++ = (uint8_t)color;
                    #endif

                    #if (GFX_CONFIG_COLOR_DEPTH == 4)
                        if(x & 0x01)
                        {
                            *pByteData++ |= color << 4;
                        }else
                        {
                            *pByteData = color & 0x0F;
                        }
                    #endif

                    #if (GFX_CONFIG_COLOR_DEPTH == 1)
                        {
                            uint8_t pos = (uint8_t)x & 0x07;

                            if(pos == 0)
                                *pByteData = color;
                            else
                                *pByteData |= color << pos;

                            if(pos == 7)
                                pByteData++;
                        }
                    #endif
                }
                else
                {
                    *pData2++ = color;
                }
            #else
                *pData2++ = color;
            #endif
#elif ((DISP_ORIENTATION == 180) || (DISP_ORIENTATION == 90))   
            #ifndef GFX_CONFIG_PALETTE_DISABLE
                if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
    	        {
                    #if (GFX_CONFIG_COLOR_DEPTH == 8)
                        *pByteData-- = (uint8_t)color;
                    #endif

                    #if (GFX_CONFIG_COLOR_DEPTH == 4)
                        if (nibblePos == 1)
                        {
                            *pByteData = ((*pByteData & 0xF0) | (color & 0x0F));
                            pByteData--;
                            nibblePos = 0;
                        }else
                        {
                            *pByteData = color << 4;
                            nibblePos = 1;
                        }
                    #endif
                        
                    #if (GFX_CONFIG_COLOR_DEPTH == 1)
                    {
                        *pByteData &= ~(1 << bitPos);
                        *pByteData |= (color << bitPos);

                        if (bitPos == 0)
                        {
                            bitPos = 7;
                            pByteData--;
                        }
                        else
                        {
                            bitPos--;
                        }
                    }
                    #endif
    	        }
    	        else
                {
                    *pData2-- = color;
                }
            #else
                *pData2-- = color;
            #endif
#endif			           	     

       	    // Shift to the next pixel
            mask <<= 1;

        } // for(x = 0; x < xLimit; x++)

        // make sure there are no pending RCC GPU commands
    while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
	GFX_WaitForGpu();

	GFX_SetWorkArea1((uint32_t)(uint16_t)lineBuffer2);
#ifndef GFX_CONFIG_DOUBLE_BUFFERING_DISABLE
        GFX_SetWorkArea2(drvGfxDrawBufferBaseAddress);
#else
        GFX_SetWorkArea2(drvGfxFrameBufferBaseAddress);
#endif
        GFX_RCC_SetSrcOffset(0);

#if (DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 180)
        GFX_RCC_SetSize(x, 1);
#else
        GFX_RCC_SetSize(1, x);
#endif	

#if (DISP_ORIENTATION == 0)
        GFX_RCC_SetDestOffset((yc * (uint32_t)DISP_HOR_RESOLUTION) + left);
#elif (DISP_ORIENTATION == 90)
        GFX_RCC_SetDestOffset(((GFX_MaxXGet() + 1 - (left + outputSize)) * (uint32_t)DISP_HOR_RESOLUTION) + yc);
#elif (DISP_ORIENTATION == 180)
        GFX_RCC_SetDestOffset(((GFX_MaxYGet() - yc) * (uint32_t)DISP_HOR_RESOLUTION) + GFX_MaxXGet() + 1 - (left+outputSize));
#elif (DISP_ORIENTATION == 270)
        GFX_RCC_SetDestOffset((left * (uint32_t)DISP_HOR_RESOLUTION) + (GFX_MaxYGet() - yc));
#endif	 
		
#ifndef GFX_CONFIG_TRANSPARENT_COLOR_DISABLE
        if (GFX_TransparentColorStatusGet() == GFX_FEATURE_ENABLED)
        {
            GFX_RCC_StartCopy(  RCC_TRANSPARENT_COPY,
                                RCC_ROP_C,
                                RCC_SRC_ADDR_CONTINUOUS,
                                RCC_DEST_ADDR_DISCONTINUOUS);
        } 		    
        else
#endif	    
        {
            GFX_RCC_StartCopy(  RCC_COPY,
                                RCC_ROP_C,
                                RCC_SRC_ADDR_CONTINUOUS,
                                RCC_DEST_ADDR_DISCONTINUOUS);
        } 			
        yc++;
                    
        // adjust the offset counter (padding is already taken cared 
        // of in calculating lineSize
        offsetCount += lineSize;

    } //for(y = 0; (y < bitmapHdr.height) && (yc < GFX_MaxYGet()); y++)

    // make sure the GPUs are done before exiting
    while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
    GFX_WaitForGpu();

}

#if (GFX_CONFIG_COLOR_DEPTH >= 4)
// *****************************************************************************
/*  Function:
    void GFX_ImageEDS4BPPDraw(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Renders a 4bpp image stored in EDS memory.

    Precondition:
        None.

    Parameters:
        left - Horizontal starting position of the image.
        top  - Vertical starting position of the image.
        pImage - Pointer to the image to be rendered.
        pPartialImageData - pointer to the partial image information when
                            the image is partially rendered.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void __attribute__((weak)) GFX_ImageEDS4BPPDraw(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)
{
    uint8_t     TempGPUBpp;
    uint8_t     lineBuffer[((GFX_MaxXGet() + (uint32_t) 1) / 2) + 1];
    uint16_t    lineBuffer2[(GFX_MaxXGet() + (uint32_t) 1)];

    uint16_t    palette[16], color;
    uint8_t     *pData; 
    uint16_t    *pData2;
    uint16_t    outputSize;
    uint16_t    xLimit;

    uint8_t     temp = 0;
    uint16_t    x, y;
    uint16_t    yc;
    GFX_MCHP_BITMAP_HEADER  bitmapHdr;
    uint32_t    offsetCount;

    uint16_t    lineSize;

#ifndef GFX_CONFIG_PALETTE_DISABLE
    uint8_t            	*pByteData = NULL; 
#if (GFX_CONFIG_COLOR_DEPTH == 4) && ((DISP_ORIENTATION == 180) || (DISP_ORIENTATION == 90))   
    uint8_t nibblePos = 0;
#endif
#endif

    GFX_ImageHeaderGet(pImage, (GFX_MCHP_BITMAP_HEADER *)&bitmapHdr);
    
    if (pImage->resource.image.type == MCHP_BITMAP_NORMAL)
    {
#ifndef GFX_CONFIG_PALETTE_DISABLE
        if (GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
        {    
            // 64 is used here since the palette size is 16 words or 64 nibbles
            offsetCount = 64;
        }
        else
#endif    
        {
            // get the palette 
            // make sure there are no pending RCC GPU commands
            while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
            GFX_WaitForGpu();

            GFX_SetWorkArea1(pImage->resource.image.location.extAddress);
            GFX_SetWorkArea2((uint16_t)palette);
            GFX_RCC_SetSrcOffset(0);
            GFX_RCC_SetDestOffset(0);
            GFX_RCC_SetSize(16, 1);
            GFX_RCC_StartCopy(  RCC_COPY,
                                RCC_ROP_C,
                                RCC_SRC_ADDR_CONTINUOUS,
                                RCC_DEST_ADDR_CONTINUOUS);

            // 64 is used here since the palette size is 16 words or 64 nibbles
            offsetCount = 64;
        }
    }
    else
    {
        // palette is stripped so skip the ID
        offsetCount = 4;
    }

    // Get the image line width in bytes
    lineSize = bitmapHdr.width;
    // account for padding    
    lineSize += (lineSize&0x01);

    if(pPartialImageData->width != 0)
    {
        // adjust to the location of the starting data for partial image rendering 
        offsetCount += ((uint32_t)pPartialImageData->yoffset*lineSize);
        offsetCount += (pPartialImageData->xoffset);

        bitmapHdr.height = pPartialImageData->height;
        bitmapHdr.width = pPartialImageData->width;
    }

    yc = top;

    outputSize = bitmapHdr.width;
    	
    // area of the buffer to use is only the area that
    // spans the viewable area
    if ((outputSize > (GFX_MaxXGet()+1)) || (left + outputSize > (GFX_MaxXGet()+1)))
        outputSize = GFX_MaxXGet() + 1 - left;

    // some pre-calculations
    if (outputSize <= GFX_MaxXGet() - left)
        xLimit = outputSize;
    else
        xLimit = GFX_MaxXGet() - left + 1;

#ifndef GFX_CONFIG_TRANSPARENT_COLOR_DISABLE
    if (GFX_TransparentColorStatusGet() == GFX_FEATURE_ENABLED)
        GFX_RCC_SetColor(GFX_TransparentColorGet());
#endif	    

    for(y = 0; (y < bitmapHdr.height) && (yc <= GFX_MaxYGet()); y++)
    {
        // copy one line from the source
        // make sure there are no pending RCC GPU commands &
        // make sure the GPUs are not operating since changing the base addresses
        // will break the currently executing GPU command.
        while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
        GFX_WaitForGpu();
	    
        // the image data is 1 nibble per pixel and normally
        // if the data is outside the PIC (in extended EDS) we cannot do
        // GPU operation with less than 8bpp color depth.
        // But since this will be read and the destination is in internal RAM we can use
        // 4bpp on the GPU operation.
        TempGPUBpp = _PUBPP;
        _PUBPP = GFX_4_BPP;

        GFX_SetWorkArea1(pImage->resource.image.location.extAddress);
        GFX_SetWorkArea2((uint32_t)(uint16_t)lineBuffer);
        GFX_RCC_SetSrcOffset(offsetCount);
        GFX_RCC_SetDestOffset(0);
        GFX_RCC_SetSize(bitmapHdr.width, 1);
        GFX_RCC_StartCopy(  RCC_COPY,
                            RCC_ROP_C,
                            RCC_SRC_ADDR_CONTINUOUS,
                            RCC_DEST_ADDR_CONTINUOUS);

        // wait until copying is done
        while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
        GFX_WaitForGpu();
	    
        // reset the _PUBPP to what it was
        _PUBPP = TempGPUBpp;

        pData = lineBuffer;

#if ((DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 270))
        pData2 = lineBuffer2;
        #ifndef GFX_CONFIG_PALETTE_DISABLE
            if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
            {
                pByteData = (uint8_t*)lineBuffer2;
            }
        #endif
#elif ((DISP_ORIENTATION == 180) || (DISP_ORIENTATION == 90))
        pData2 = &lineBuffer2[outputSize-1];

        #ifndef GFX_CONFIG_PALETTE_DISABLE
            if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
            {
                pByteData = (uint8_t*)lineBuffer2;

                #if (GFX_CONFIG_COLOR_DEPTH == 8)
                    pByteData += outputSize;
                    pByteData--;
                #endif

                #if (GFX_CONFIG_COLOR_DEPTH == 4)
                    pByteData += outputSize / 2;
                    nibblePos = (outputSize & 0x01);
                    if(nibblePos == 0)
                    {
                        pByteData--;
                    }
                #endif
            }
            #endif
        #endif

        // process the pixels on the current line
        for(x = 0; x < xLimit; x++)
        {
            // take one pixel data when needed 
            if ((x & 0x01) == 0)
            	temp = *pData++; 

#ifndef GFX_CONFIG_PALETTE_DISABLE
            if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
                color = temp >> ((x&0x0001)*4) & 0x000F;
            else
                color = palette[(temp >> ((x&0x0001)*4)) & 0x000F];
#else
            color = palette[(temp >> ((x&0x0001)*4)) & 0x000F];
#endif	            
            
#if ((DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 270))
            #ifndef GFX_CONFIG_PALETTE_DISABLE
                if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
                {
                    #if (GFX_CONFIG_COLOR_DEPTH == 8)
                        *pByteData++ = (uint8_t)color;
                    #endif

                    #if (GFX_CONFIG_COLOR_DEPTH == 4)
                        if(x & 0x01)
                        {
                            *pByteData++ |= color << 4;
                        }else
                        {
                            *pByteData = color & 0x0F;
                        }
                    #endif
                }
                else
                {
                    *pData2++ = color;
                }
            #else
                *pData2++ = color;
            #endif
#elif ((DISP_ORIENTATION == 180) || (DISP_ORIENTATION == 90))   
            #ifndef GFX_CONFIG_PALETTE_DISABLE
                if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
    	        {
                    #if (GFX_CONFIG_COLOR_DEPTH == 8)
                        *pByteData-- = (uint8_t)color;
                    #endif

                    #if (GFX_CONFIG_COLOR_DEPTH == 4)
                        if (nibblePos == 1)
                        {
                            *pByteData = ((*pByteData & 0xF0) | (color & 0x0F));
                            pByteData--;
                            nibblePos = 0;
                        }else
                        {
                            *pByteData = color << 4;
                            nibblePos = 1;
                        }
                    #endif
                }
    	        else
    	        {
                    *pData2-- = color;
                }
            #else
                *pData2-- = color;
            #endif
#endif		

        } // for(x = 0; x < xLimit; x++)

        // make sure there are no pending RCC GPU commands
        while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
        GFX_WaitForGpu();
        GFX_SetWorkArea1((uint32_t)(uint16_t)lineBuffer2);
#ifndef GFX_CONFIG_DOUBLE_BUFFERING_DISABLE
        GFX_SetWorkArea2(drvGfxDrawBufferBaseAddress);
#else
        GFX_SetWorkArea2(drvGfxFrameBufferBaseAddress);
#endif
        GFX_RCC_SetSrcOffset(0);

#if ((DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 180))
        GFX_RCC_SetSize(x, 1);
#else
        GFX_RCC_SetSize(1, x);
#endif

#if (DISP_ORIENTATION == 0)
        GFX_RCC_SetDestOffset((yc * (uint32_t)DISP_HOR_RESOLUTION) + left);
#elif (DISP_ORIENTATION == 90)
        GFX_RCC_SetDestOffset(((GFX_MaxXGet() + 1 - (left + outputSize)) * (uint32_t)DISP_HOR_RESOLUTION) + yc);
#elif (DISP_ORIENTATION == 180)
        GFX_RCC_SetDestOffset(((GFX_MaxYGet() - yc) * (uint32_t)DISP_HOR_RESOLUTION) + GFX_MaxXGet() + 1 - (left+outputSize));
#elif (DISP_ORIENTATION == 270)
        GFX_RCC_SetDestOffset((left * (uint32_t)DISP_HOR_RESOLUTION) + (GFX_MaxYGet() - yc));
#endif
		
#ifndef GFX_CONFIG_TRANSPARENT_COLOR_DISABLE
        if (GFX_TransparentColorStatusGet() == GFX_FEATURE_ENABLED)
        {
            GFX_RCC_StartCopy(  RCC_TRANSPARENT_COPY,
                                RCC_ROP_C,
                                RCC_SRC_ADDR_CONTINUOUS,
                                RCC_DEST_ADDR_DISCONTINUOUS);
        }
        else
#endif
        {
            GFX_RCC_StartCopy(  RCC_COPY,
                                RCC_ROP_C,
                                RCC_SRC_ADDR_CONTINUOUS,
                                RCC_DEST_ADDR_DISCONTINUOUS);
        }

        yc++;
       
        // adjust the offset counter (adjust the offset if there are padded bits)
        offsetCount += lineSize;

    } //for(y = 0; (y < bitmapHdr.height) && (yc < GFX_MaxYGet()); y++)

    // make sure the GPUs are done before exiting
    while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
    GFX_WaitForGpu();

}

#endif //#if (COLOR_DEPTH >= 4)
    
#if (GFX_CONFIG_COLOR_DEPTH >= 8)
// *****************************************************************************
/*  Function:
    void GFX_ImageEDS8BPPDraw(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Renders a 8bpp image stored in EDS memory.

    Precondition:
        None.

    Parameters:
        left - Horizontal starting position of the image.
        top  - Vertical starting position of the image.
        pImage - Pointer to the image to be rendered.
        pPartialImageData - pointer to the partial image information when
                            the image is partially rendered.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void __attribute__((weak)) GFX_ImageEDS8BPPDraw(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)
{
    uint8_t             TempGPUBpp;
    uint8_t             lineBuffer[(GFX_MaxXGet() + (uint32_t) 1)];
    uint16_t            lineBuffer2[(GFX_MaxXGet() + (uint32_t) 1)];
    
    uint16_t            palette[256];
    uint8_t             *pData;
    uint16_t		*pData2;
    uint16_t 		outputSize;
    uint16_t            xLimit;

    uint8_t             temp = 0;
    uint16_t            x, y;
    uint16_t            yc;
    GFX_MCHP_BITMAP_HEADER       bitmapHdr;
    uint32_t 		offsetCount;
    uint16_t            lineSize;

    #ifndef GFX_CONFIG_PALETTE_DISABLE
    uint8_t            	*pByteData = NULL; // To supress warning 
    #endif


    GFX_ImageHeaderGet(pImage, (GFX_MCHP_BITMAP_HEADER *)&bitmapHdr);
    
    if (pImage->resource.image.type == MCHP_BITMAP_NORMAL)
    {
#ifndef GFX_CONFIG_PALETTE_DISABLE
        if (GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
        {    
            // 512 is used here since the palette size is 256 words or 512 bytes
            offsetCount = 512;
        }
        else
#endif    
        {
            // get the palette (256 entries)
            // make sure there are no pending RCC GPU commands
            while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
            GFX_WaitForGpu();

            GFX_SetWorkArea1(pImage->resource.image.location.extAddress);
            GFX_SetWorkArea2((uint16_t)palette);
            GFX_RCC_SetSrcOffset(0);
            GFX_RCC_SetDestOffset(0);
            GFX_RCC_SetSize(256, 1);
            GFX_RCC_StartCopy(  RCC_COPY,
                                RCC_ROP_C,
                                RCC_SRC_ADDR_CONTINUOUS,
                                RCC_DEST_ADDR_CONTINUOUS);

            // 512 is used here since the palette size is 256 words or 512 bytes
            offsetCount = 512;
        }
    }
    else
    {
        // palette is stripped so offset is 2 bytes due to the ID
        offsetCount = 2;
    }    
    
    lineSize = bitmapHdr.width;

    if(pPartialImageData->width != 0)
    {
        // adjust to the location of the starting data for partial image rendering 
        offsetCount += ((uint32_t)pPartialImageData->yoffset*lineSize);
        offsetCount += (pPartialImageData->xoffset);

        bitmapHdr.height = pPartialImageData->height;
        bitmapHdr.width = pPartialImageData->width;
    }
	
    yc = top;

    outputSize = bitmapHdr.width;

    // area of the buffer to use is only the area that
    // spans the viewable area
    if ((outputSize > (GFX_MaxXGet() + 1)) || (left + outputSize - 1 > GFX_MaxXGet()))
        outputSize = GFX_MaxXGet() + 1 - left;

    // some pre-calculations
    if (outputSize < GFX_MaxXGet() - left)
        xLimit = outputSize;
    else
        xLimit = GFX_MaxXGet() - left + 1;

#ifndef GFX_CONFIG_TRANSPARENT_COLOR_DISABLE
    if (GFX_TransparentColorStatusGet() == GFX_FEATURE_ENABLED)
        GFX_RCC_SetColor(GFX_TransparentColorGet());
#endif
	
    for(y = 0; (y < bitmapHdr.height) && (yc <= GFX_MaxYGet()); y++)
    {
        // copy one line from the source
        // make sure there are no pending RCC GPU commands & 
        // make sure the GPUs are not operating since changing the base addresses
        // will break the currently executing GPU command.
        while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
        GFX_WaitForGpu();
	    
        // since the image data is 1 byte per pixel, set the GPU bpp to 8 bpp.
        TempGPUBpp = _PUBPP;
        _PUBPP = GFX_8_BPP;

        GFX_SetWorkArea1(pImage->resource.image.location.extAddress);
        GFX_SetWorkArea2((uint32_t)(uint16_t)lineBuffer);
        // 512 is used here since the palette size is 256 words or 512 bytes
        GFX_RCC_SetSrcOffset(offsetCount);
        GFX_RCC_SetDestOffset(0);
        GFX_RCC_SetSize(bitmapHdr.width, 1);
        GFX_RCC_StartCopy(  RCC_COPY,
                            RCC_ROP_C,
                            RCC_SRC_ADDR_CONTINUOUS,
                            RCC_DEST_ADDR_CONTINUOUS);

        // wait until copying is done
        while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
        GFX_WaitForGpu();
	    
        // reset the _PUBPP to what it was
        _PUBPP = TempGPUBpp;

       	pData = lineBuffer;

        // get the location of the line buffer 
#if ((DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 270))
        pData2 = lineBuffer2;
#elif ((DISP_ORIENTATION == 180) || (DISP_ORIENTATION == 90))   
        pData2 = &lineBuffer2[outputSize-1];
            
        #ifndef GFX_CONFIG_PALETTE_DISABLE
            if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
            {
                pData2 -= outputSize/2;
            }
        #endif
#endif

#ifndef GFX_CONFIG_PALETTE_DISABLE
        if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
        {
            #if ((DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 270))
                pByteData = (uint8_t*)lineBuffer2;
            #elif ((DISP_ORIENTATION == 180) || (DISP_ORIENTATION == 90))   
                pByteData = (uint8_t*)lineBuffer2;
                pByteData += (outputSize - 1);
            #endif           	
        }
#endif
       	

        // process the pixels of the current line
        for(x = 0; x < xLimit; x++)
        {
            // take one pixel data 
       	    temp = *pData++;

            // Write pixel to buffer
#if ((DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 270))
            #ifndef GFX_CONFIG_PALETTE_DISABLE
                if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
                {
                    *pByteData++ = (uint8_t)temp;
                }
                else
                {
                    *pData2++ = palette[temp];
                }
            #else
                *pData2++ = palette[temp];
            #endif
#elif ((DISP_ORIENTATION == 180) || (DISP_ORIENTATION == 90))
            #ifndef GFX_CONFIG_PALETTE_DISABLE
                if(GFX_PaletteStatusGet() == GFX_FEATURE_ENABLED)
                {
                    *pByteData-- = (uint8_t)temp;
                }
                else
                {
                    *pData2-- = palette[temp];
                }
            #else
                *pData2-- = palette[temp];
            #endif
#endif					  

        } // for(x = 0; x < xLimit; x++)

        // make sure there are no pending RCC GPU commands
        while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
        GFX_WaitForGpu();
        GFX_SetWorkArea1((uint32_t)(uint16_t)lineBuffer2);
#ifndef GFX_CONFIG_DOUBLE_BUFFERING_DISABLE
        GFX_SetWorkArea2(drvGfxDrawBufferBaseAddress);
#else
        GFX_SetWorkArea2(drvGfxFrameBufferBaseAddress);
#endif
        GFX_RCC_SetSrcOffset(0);

#if (DISP_ORIENTATION == 90) || (DISP_ORIENTATION == 270)
        GFX_RCC_SetSize(1, x);
#else
        GFX_RCC_SetSize(x, 1);
#endif

#if (DISP_ORIENTATION == 0)
        GFX_RCC_SetDestOffset((yc * (uint32_t)DISP_HOR_RESOLUTION) + left);
#elif (DISP_ORIENTATION == 90)
        GFX_RCC_SetDestOffset(((GFX_MaxXGet() + 1 - (left + outputSize)) * (uint32_t)DISP_HOR_RESOLUTION) + yc);
#elif (DISP_ORIENTATION == 180)
        GFX_RCC_SetDestOffset(((GFX_MaxYGet() - yc) * (uint32_t)DISP_HOR_RESOLUTION) + GFX_MaxXGet() + 1 - (left+outputSize));
#elif (DISP_ORIENTATION == 270)
        GFX_RCC_SetDestOffset((left * (uint32_t)DISP_HOR_RESOLUTION) + (GFX_MaxYGet() - yc));
#endif
	
#ifndef GFX_CONFIG_TRANSPARENT_COLOR_DISABLE
        if (GFX_TransparentColorStatusGet() == GFX_FEATURE_ENABLED)
        {
            GFX_RCC_StartCopy(RCC_TRANSPARENT_COPY, RCC_ROP_C, RCC_SRC_ADDR_CONTINUOUS, RCC_DEST_ADDR_DISCONTINUOUS);
        } 		    
        else
#endif
        {
            GFX_RCC_StartCopy(RCC_COPY, RCC_ROP_C, RCC_SRC_ADDR_CONTINUOUS, RCC_DEST_ADDR_DISCONTINUOUS);
        }

        // update the vertical position counter
        yc++;

        offsetCount += lineSize;

    } //for(y = 0; (y < bitmapHdr.height) && (yc < GFX_MaxYGet()); y++)

    // make sure the GPUs are done before exiting
    while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
    GFX_WaitForGpu();

}

#endif //#if (COLOR_DEPTH >= 8)
       
#if (GFX_CONFIG_COLOR_DEPTH == 16)
// *****************************************************************************
/*  Function:
    void GFX_ImageEDS16BPPDraw(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Renders a 16bpp image stored in EDS memory.

    Precondition:
        None.

    Parameters:
        left - Horizontal starting position of the image.
        top  - Vertical starting position of the image.
        pImage - Pointer to the image to be rendered.
        pPartialImageData - pointer to the partial image information when
                            the image is partially rendered.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void __attribute__((weak)) GFX_ImageEDS16BPPDraw(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)
{
    register uint16_t       x, y;

    uint16_t                lineBuffer[(GFX_MaxXGet() + (uint32_t) 1)];
    uint16_t                lineBuffer2[(GFX_MaxXGet() + (uint32_t) 1)];
    uint16_t                outputSize, actualY;
    uint16_t                *pSrc;
    int16_t                 xc, yc;
    GFX_MCHP_BITMAP_HEADER  bitmapHdr;
    uint16_t                lineSize;
    uint32_t                offsetCount;

    GFX_ImageHeaderGet(pImage, (GFX_MCHP_BITMAP_HEADER *)&bitmapHdr);

    lineSize = bitmapHdr.width;
    offsetCount = 0;

    if(pPartialImageData->width != 0)
    {
        // adjust to the location of the starting data for partial image rendering 
        offsetCount += ((uint32_t)pPartialImageData->yoffset*lineSize);
        offsetCount += (pPartialImageData->xoffset);

        bitmapHdr.height = pPartialImageData->height;
        bitmapHdr.width = pPartialImageData->width;
    }
    
    outputSize = bitmapHdr.width;
			
    // area of the buffer to use is only the area that
    // spans the viewable area
    if ((outputSize > (GFX_MaxXGet()+1)) || (left + outputSize > (GFX_MaxXGet()+1)))
        outputSize = GFX_MaxXGet() + 1 - left;

    // When the DISP_ORIENTATION = 0, the image can be transferred to the display buffer
    // using one transfer. If doing partial image or using other orientation,
    // the transfers are performed line by line.
#if (DISP_ORIENTATION == 0) 
    if (pPartialImageData->width == 0)
    {
        // Since the data in memory is oriented in the same way as the display we
    	// can move the whole block. The source is continuous and destination is  
    	// discontinuous since it is the display buffer. There is no offset on
    	// source since the image data immediately starts (16 bpp image has no palette).

        // Known issue:
        // When the image exceeds the width of the display buffer, the portion of the
        // image that is beyond the width will go to the next line. 

#ifndef GFX_CONFIG_TRANSPARENT_COLOR_DISABLE
#ifndef GFX_CONFIG_DOUBLE_BUFFERING_DISABLE
        DRV_GFX_BitBlt(((GFX_RESOURCE_HDR*)pImage)->resource.image.location.extAddress, drvGfxDrawBufferBaseAddress,
                 0, (top * (uint32_t)DISP_HOR_RESOLUTION) + left,
                 GFX_DATA_CONTINUOUS, GFX_DATA_DISCONTINUOUS, 
                 RCC_TRANSPARENT_COPY, RCC_ROP_C, GFX_TransparentColorGet(),
                 bitmapHdr.width, bitmapHdr.height);
#else
	DRV_GFX_BitBlt(((GFX_RESOURCE_HDR*)pImage)->resource.image.location.extAddress, drvGfxFrameBufferBaseAddress,
                 0, (top * (uint32_t)DISP_HOR_RESOLUTION) + left,
                 GFX_DATA_CONTINUOUS, GFX_DATA_DISCONTINUOUS,
                 RCC_TRANSPARENT_COPY, RCC_ROP_C, GFX_TransparentColorGet(),
                 bitmapHdr.width, bitmapHdr.height);
#endif

#else

#ifndef GFX_CONFIG_DOUBLE_BUFFERING_DISABLE

        DRV_GFX_BitBlt(((GFX_RESOURCE_HDR*)pImage)->resource.image.location.extAddress, drvGfxDrawBufferBaseAddress,
                  0, (top * (uint32_t)DISP_HOR_RESOLUTION) + left,
                  GFX_DATA_CONTINUOUS, GFX_DATA_DISCONTINUOUS, 
                  RCC_COPY, RCC_ROP_C,    
                  0, bitmapHdr.width, bitmapHdr.height);
#else

        DRV_GFX_BitBlt(((GFX_RESOURCE_HDR*)pImage)->resource.image.location.extAddress, drvGfxFrameBufferBaseAddress,
                  0, (top * (uint32_t)DISP_HOR_RESOLUTION) + left,
                  GFX_DATA_CONTINUOUS, GFX_DATA_DISCONTINUOUS, 
                  RCC_COPY, RCC_ROP_C,    
                  0, bitmapHdr.width, bitmapHdr.height);
#endif
#endif				  
	}
	else
#endif   // #if (DISP_ORIENTATION == 0) 
	{
        // copy the data line by line and block copy it into the destination
        actualY = top;

#ifndef GFX_CONFIG_TRANSPARENT_COLOR_DISABLE
        if (GFX_TransparentColorStatusGet() == GFX_FEATURE_ENABLED)
            GFX_RCC_SetColor(GFX_TransparentColorGet());
#endif

	    for(y = 0; (y < bitmapHdr.height) && (actualY <= GFX_MaxYGet()); y++)
        {
            // make sure there are no pending RCC GPU commands &
            // make sure the GPUs are not operating since changing the base addresses
            // will break the currently executing GPU command.
            while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
	        GFX_WaitForGpu();
	        
            // copy one line from the source
            GFX_SetWorkArea1(pImage->resource.image.location.extAddress);   // image location is the source
            GFX_SetWorkArea2((uint16_t)lineBuffer);                         // buffer is the destination
            GFX_RCC_SetSize(outputSize, 1);                                 // copy one line of the image
            GFX_RCC_SetSrcOffset(offsetCount);                              // set the offset of the source
            
            GFX_RCC_SetDestOffset(0);                                       // no offset for the destination

            // initiate the copy, the source is continuous and the
            // destination is also continuous
            GFX_RCC_StartCopy(  RCC_COPY, RCC_ROP_C,            \
                                RCC_SRC_ADDR_CONTINUOUS,        \
                                RCC_DEST_ADDR_CONTINUOUS);

            // make sure the copy was executed fully
            while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
            GFX_WaitForGpu();

            // swap data
            for (x = 0; x < (outputSize); x += 1)
            {
#if (DISP_ORIENTATION == 0) || (DISP_ORIENTATION == 270)
                lineBuffer2[x] = lineBuffer[x];
#else
                lineBuffer2[outputSize-x-1] = lineBuffer[x];
#endif
            }
            pSrc = lineBuffer2;
			
            GFX_SetWorkArea1((uint16_t)pSrc);
#ifndef GFX_CONFIG_DOUBLE_BUFFERING_DISABLE
            GFX_SetWorkArea2(drvGfxDrawBufferBaseAddress);
#else
            GFX_SetWorkArea2(drvGfxFrameBufferBaseAddress);
#endif
            GFX_RCC_SetSrcOffset(0);

            // adjust the x and y coordinate positions based on the screen rotation
#if (DISP_ORIENTATION == 0)
            xc = left;
            yc = actualY;
#elif (DISP_ORIENTATION == 90)
            xc = actualY;
            yc = GFX_MaxXGet() + 1 - (left + outputSize);
#elif (DISP_ORIENTATION == 180)
            xc = GFX_MaxXGet() + 1 - (left + outputSize);
            yc = GFX_MaxYGet() - actualY;
#elif (DISP_ORIENTATION == 270)
            xc = GFX_MaxYGet() - actualY;
            yc = left;
#endif
			GFX_RCC_SetDestOffset((yc*(uint32_t)DISP_HOR_RESOLUTION)+xc);
		
#if (DISP_ORIENTATION == 90) || (DISP_ORIENTATION == 270)
            GFX_RCC_SetSize(1, outputSize);
#else
            GFX_RCC_SetSize(outputSize, 1);
#endif
			
#ifndef GFX_CONFIG_TRANSPARENT_COLOR_DISABLE
            if (GFX_TransparentColorStatusGet() == GFX_FEATURE_ENABLED)
            {
                GFX_RCC_StartCopy(  RCC_TRANSPARENT_COPY,
                                    RCC_ROP_C,
                                    RCC_SRC_ADDR_CONTINUOUS,
                                    RCC_DEST_ADDR_DISCONTINUOUS);
            }
            else
#endif
            {
                GFX_RCC_StartCopy(  RCC_COPY,
                                    RCC_ROP_C,
                                    RCC_SRC_ADDR_CONTINUOUS,
                                    RCC_DEST_ADDR_DISCONTINUOUS);
            }

            actualY++;		
            offsetCount += lineSize;
        } // for(y = 0; (y < bitmapHdr.height) && (y < GFX_MaxYGet()); y++)
    }

    // make sure the GPUs are done before exiting
    while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
    GFX_WaitForGpu();
}
#endif //#if (COLOR_DEPTH == 16)

// *****************************************************************************
/*  Function:
    uint16_t DRV_GFX_ImageDraw(
                                int16_t left,
                                int16_t top,
                                GFX_RESOURCE_HDR *image,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)

    Summary:
        This function is a driver specific image draw function.

    Description:
        This function is a driver specific image draw function.
        This function is implemented in drivers that supports
        accelerated rendering. Execution of image draw functions
        is faster when implemented in driver layer.

*/
// *****************************************************************************
uint16_t DRV_GFX_ImageDraw(
                                int16_t left,
                                int16_t top,
                                GFX_RESOURCE_HDR *image,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData)
{
    uint8_t         colorDepth;
    uint16_t        colorTemp;
    uint16_t        resType;
    GFX_PARTIAL_IMAGE_PARAM *pParImageData = pPartialImageData;

    if (GFX_FreeCommandSpaceCheck(4) == 0)
        return (GFX_STATUS_FAILURE);

    // Save current color
    colorTemp = GFX_ColorGet();

    resType = ((GFX_RESOURCE_HDR *)image)->type;

#ifndef GFX_CONFIG_IPU_DECODE_DISABLE

    if((resType & GFX_COMP_MASK) == GFX_RESOURCE_COMP_IPU)
    {
        GFX_RESOURCE_HDR    *piHdr;
        GFX_RESOURCE_HDR    NewiHdr;     // NewiHdr is the resource's header placed in RAM

        piHdr = (GFX_RESOURCE_HDR *)image;
        NewiHdr.type = (GFX_RESOURCE_MEMORY_EDS_EPMP | GFX_RESOURCE_TYPE_MCHP_MBITMAP | GFX_RESOURCE_COMP_IPU);
        NewiHdr.ID = 1;
        NewiHdr.resource.image.location.extAddress = GFX_DECOMPRESSED_DATA_RAM_ADDRESS;
        NewiHdr.resource.image.colorDepth = ((GFX_RESOURCE_HDR *)image)->resource.image.colorDepth;
        NewiHdr.resource.image.type       = ((GFX_RESOURCE_HDR *)image)->resource.image.type;
        NewiHdr.resource.image.paletteID  = ((GFX_RESOURCE_HDR *)image)->resource.image.paletteID;
        NewiHdr.resource.image.width      = ((GFX_RESOURCE_HDR *)image)->resource.image.width;
        NewiHdr.resource.image.height     = ((GFX_RESOURCE_HDR *)image)->resource.image.height;

        // case 1: source (image) is from EDS memory or RAM
        if(((resType & GFX_MEM_MASK) == GFX_RESOURCE_MEMORY_EDS_EPMP) || ((resType & GFX_MEM_MASK) == GFX_RESOURCE_MEMORY_RAM))
        {
            while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
            GFX_WaitForGpu();
            
            GFX_SetWorkArea1(piHdr->resource.image.location.edsAddress);
            
            GFX_SetWorkArea2(GFX_DECOMPRESSED_DATA_RAM_ADDRESS);
            GFX_IPU_SetSrcOffset(0);
            GFX_IPU_SetDestOffset(0);
            GFX_IPU_Inflate(piHdr->resource.image.parameter2.rawSize);

            while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
            GFX_WaitForGpu();

            if((GFX_IPU_GetDecompressionDone() || GFX_IPU_GetFinalBlock()) == 0)
            {
                return -1;
            }
            // re-assign to the newly decoded image in memory
            image = &NewiHdr;
        }    

#ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE
        // case 2: source (image) is from flash memory
        else if((resType & GFX_MEM_MASK) == GFX_RESOURCE_MEMORY_FLASH)
        {
            uint8_t                 aTemp[GFX_IPU_TEMP_DATA_TRANSFER_ARRAY_SIZE];
            volatile uint16_t       quotient, remainder, i;
            uint32_t                srcaddress, offset;
            uint8_gfx_image_prog    *pFlashData;
            uint8_t                 savedpubpp = _PUBPP;
            
            pFlashData = piHdr->resource.image.location.progByteAddress;
            quotient = piHdr->resource.image.parameter1.compressedSize / GFX_IPU_TEMP_DATA_TRANSFER_ARRAY_SIZE;
            remainder = piHdr->resource.image.parameter1.compressedSize - (quotient * GFX_IPU_TEMP_DATA_TRANSFER_ARRAY_SIZE);
            
            while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
            GFX_WaitForGpu();

            _PUBPP = GFX_8_BPP;

            srcaddress = (uint32_t)(uint16_t)aTemp;
            offset = 0;
            
            while(quotient)
            {
                for(i=0; i < GFX_IPU_TEMP_DATA_TRANSFER_ARRAY_SIZE; i++)
                {
                    aTemp[i] = pFlashData[i + offset];                
                }
                
                while(GFX_FreeCommandSpaceCheck(4) == 0);
                
                DRV_GFX_BitBlt(srcaddress, GFX_COMPRESSED_DATA_RAM_ADDRESS, 0, offset,  \
                         GFX_DATA_CONTINUOUS, GFX_DATA_CONTINUOUS, RCC_COPY, RCC_ROP_C, \
                         0, GFX_IPU_TEMP_DATA_TRANSFER_ARRAY_SIZE, 1);

                offset += GFX_IPU_TEMP_DATA_TRANSFER_ARRAY_SIZE;
                
                quotient--;
            }

            for(i=0; i < remainder; i++)
            {
                aTemp[i] = pFlashData[i + offset];                
            }
            
            while(GFX_FreeCommandSpaceCheck(4) == 0);
            DRV_GFX_BitBlt(srcaddress, GFX_COMPRESSED_DATA_RAM_ADDRESS, 0, offset,      \
                     GFX_DATA_CONTINUOUS, GFX_DATA_CONTINUOUS, RCC_COPY, RCC_ROP_C,     \
                     0, remainder, 1);

            while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
            GFX_WaitForGpu();

            _PUBPP = savedpubpp;


            GFX_SetWorkArea1(GFX_COMPRESSED_DATA_RAM_ADDRESS);
            GFX_SetWorkArea2(GFX_DECOMPRESSED_DATA_RAM_ADDRESS);
            GFX_IPU_SetSrcOffset(0);
            GFX_IPU_SetDestOffset(0);
            GFX_IPU_Inflate(piHdr->resource.image.parameter2.rawSize);

            while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
            GFX_WaitForGpu();

            if((GFX_IPU_GetDecompressionDone() || GFX_IPU_GetFinalBlock()) == 0)
            {
                return -1;
            }
            // re-assign to the newly decoded image in memory    
            image = &NewiHdr;

        }
#endif // #ifndef GFX_CONFIG_IMAGE_FLASH_DISABLE

#ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE
        // case 3: source (image) is from external memory
        else if((resType & GFX_MEM_MASK) == GFX_RESOURCE_MEMORY_EXTERNAL)
        {
            uint8_t aTemp[GFX_IPU_TEMP_DATA_TRANSFER_ARRAY_SIZE];
            uint16_t  quotient, remainder;
            uint32_t srcaddress, offset;
            uint8_t  savedpubpp = _PUBPP;
            
            quotient  = piHdr->resource.image.parameter1.compressedSize / GFX_IPU_TEMP_DATA_TRANSFER_ARRAY_SIZE;
            remainder = piHdr->resource.image.parameter1.compressedSize - (quotient * GFX_IPU_TEMP_DATA_TRANSFER_ARRAY_SIZE);
            
            while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
            GFX_WaitForGpu();

            _PUBPP = GFX_8_BPP;

            srcaddress = (uint32_t)(uint16_t)aTemp;
            offset = 0;
            
            while(quotient)
            {
                GFX_ExternalResourceCallback(image, offset, GFX_IPU_TEMP_DATA_TRANSFER_ARRAY_SIZE, (void*)aTemp);
                
                while(GFX_FreeCommandSpaceCheck(4) == 0);
                DRV_GFX_BitBlt(srcaddress, GFX_COMPRESSED_DATA_RAM_ADDRESS, 0, offset,      \
                         GFX_DATA_CONTINUOUS, GFX_DATA_CONTINUOUS, RCC_COPY, RCC_ROP_C,     \
                         0, GFX_IPU_TEMP_DATA_TRANSFER_ARRAY_SIZE, 1);


                offset += GFX_IPU_TEMP_DATA_TRANSFER_ARRAY_SIZE;
                quotient--;
            }

            GFX_ExternalResourceCallback(image, offset, remainder, (void*)aTemp);
            
            while(GFX_FreeCommandSpaceCheck(4) == 0);
            DRV_GFX_BitBlt(srcaddress, GFX_COMPRESSED_DATA_RAM_ADDRESS, 0, offset,          \
                     GFX_DATA_CONTINUOUS, GFX_DATA_CONTINUOUS, RCC_COPY, RCC_ROP_C,         \
                     0, remainder, 1);


            while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
            GFX_WaitForGpu();

            _PUBPP = savedpubpp;

            GFX_SetWorkArea1(GFX_COMPRESSED_DATA_RAM_ADDRESS);
            GFX_SetWorkArea2(GFX_DECOMPRESSED_DATA_RAM_ADDRESS);
            GFX_IPU_SetSrcOffset(0);
            GFX_IPU_SetDestOffset(0);

            GFX_IPU_Inflate(piHdr->resource.image.parameter2.rawSize);

            while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
            GFX_WaitForGpu();

            if((GFX_IPU_GetDecompressionDone() || GFX_IPU_GetFinalBlock()) == 0)
            {
                return -1;
            }
            // re-assign to the newly decoded image in memory
            image = &NewiHdr;
        }
#endif // #ifndef GFX_CONFIG_IMAGE_EXTERNAL_DISABLE
        else
        {
            // catch all, just in case (this should not happen)
            return -1;
        }

    }


#endif 

    // Read color depth
    colorDepth = ((GFX_RESOURCE_HDR *)image)->resource.image.colorDepth;
            
    // Draw picture
    switch(colorDepth)
    {
        case 1:     GFX_ImageEDS1BPPDraw(   left, top,
                                            (GFX_RESOURCE_HDR *)image,
                                            pParImageData);
                    break;
        
#if (GFX_CONFIG_COLOR_DEPTH >= 4)
        case 4:     GFX_ImageEDS4BPPDraw(   left, top,
                                            (GFX_RESOURCE_HDR *)image,
                                            pParImageData);
                    break;
#endif
                    
#if (GFX_CONFIG_COLOR_DEPTH >= 8)
        case 8:     GFX_ImageEDS8BPPDraw(   left, top,
                                            (GFX_RESOURCE_HDR *)image,
                                            pParImageData);
                    break;
#endif
                    
#if (GFX_CONFIG_COLOR_DEPTH == 16)
        case 16:    GFX_ImageEDS16BPPDraw(  left, top,
                                            (GFX_RESOURCE_HDR *)image,
                                            pParImageData);
                    break;
#endif
                    
        default:    break;
    }

    // Restore current color
    GFX_ColorSet(colorTemp);
   
    return (1);
}

// *****************************************************************************
/*  Function:
    uint8_t DRV_GFX_Inflate(
                                uint32_t srcAddress,
                                uint32_t destAddress,
                                uint32_t nBytes)

    Summary:
        This function inflates (decompress) compresses data in memory.

    Description:
        This function inflates (decompress) compresses data in memory.
        The compressed source data is located in srcAddress and the
        resulting inflated data is placed in destAddress.

        Source compressed data must be encoded using the
        DEFLATE algorithm with fixed Huffman codes.

*/
// *****************************************************************************
uint8_t DRV_GFX_Inflate(
                                uint32_t srcAddress,
                                uint32_t destAddress,
                                uint32_t nBytes)
{
    while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
    while(GFX_IsPuGpuBusy());
    
    GFX_SetWorkArea1(srcAddress);
    GFX_IPU_SetSrcOffset(0);
    GFX_SetWorkArea2(destAddress);
    GFX_IPU_SetDestOffset(0);
    GFX_IPU_Inflate(nBytes);
    
    while(!GFX_IPU_GetDecompressionDone())
    {
        if(GFX_IPU_GetDecompressionError())
        {
            return GFX_IPU_GetDecompressionError();
        }
    }
    
    return GFX_IPU_GetDecompressionError();
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_RenderStatusGet()

    Summary:
        This function returns the driver's status on rendering.

    Description:
        This function returns the driver's rendering status. 
        (see GFX_STATUS_BIT).
 
*/
// *****************************************************************************
GFX_STATUS_BIT GFX_RenderStatusGet(void)
{
    // Since the hardware for this driver has a 16 command FIFO, and
    // each hardware acceleration will have to use the FIFO, this driver
    // needs to know how many available free space is present to be
    // able to decide if a rendering can be executed or not.
    // Also the number of command space needed is varying per
    // accelerated function. 

    return GFX_STATUS_READY_BIT;

}

#ifndef GFX_CONFIG_DOUBLE_BUFFERING_DISABLE

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_DrawBufferInitialize(
                                uint16_t index,
                                uint32_t address)

    Summary:
        This function initializes the address of the draw buffer
        specified by the given index.

    Description:
        For system with multiple buffers, this function is used to
        initialize the array of buffers. The address of the draw buffer
        will be associated with the specified index. Use this function
        to initialize or modify the array of frame buffers in the system
        at run time.
        For systems with single buffer this function will have no effect.
        The size of the buffer is defined by the dimension of the screen
        and the color depth used.

*/
// *****************************************************************************
GFX_STATUS GFX_DrawBufferInitialize(
                                uint16_t index,
                                uint32_t address)
{
    drvGfxFrameBufferArray[index] = address;
    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_DrawBufferSet(uint16_t index)

    Summary:
        This function sets the draw buffer with the given index number.

    Description:
        This function sets the draw buffer with the given index number.
        For systems with single buffer, frame buffer and draw buffer are
        the same buffer. Calls to this function will have no effect and
        will always return GFX_STATUS_SUCCESS.

*/
// *****************************************************************************
GFX_STATUS GFX_DrawBufferSet(uint16_t index)
{
    drvGfxDrawBufferIndex = index;
    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    uint16_t GFX_DrawBufferGet(void)

    Summary:
        This function returns the index of the current draw buffer.

    Description:
        This function returns the index of the current draw buffer.
        Draw buffer is the buffer where rendering is performed.
        For systems with single buffer this function will always
        return 0.

*/
// *****************************************************************************
uint16_t GFX_DrawBufferGet(void)
{
    return (drvGfxDrawBufferIndex);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_FrameBufferSet(uint16_t index)

    Summary:
        This function sets the frame buffer with the given index number.

    Description:
        This function sets the frame buffer with the given index number.
        This is the buffer that is displayed on the screen.
        For systems with single buffer, frame buffer and draw buffer are
        the same buffer. Calls to this function will have no effect and
        will always return GFX_STATUS_SUCCESS.

*/
// *****************************************************************************
GFX_STATUS GFX_FrameBufferSet(uint16_t index)
{
    drvGfxFrameBufferIndex = index;
    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    uint16_t GFX_FrameBufferGet(void)

    Summary:
        This function returns the index of the current frame buffer.

    Description:
        This function returns the index of the current frame buffer.
        Frame buffer is the buffer that is currently displayed in the screen.
        For systems with single buffer this function will always
        return 0.

*/
// *****************************************************************************
uint16_t GFX_FrameBufferGet(void)
{
    return (drvGfxFrameBufferIndex);
}

// *****************************************************************************
/*  Function:
    void GFX_DRV_DoubleBufferDisable(void)

    Summary:
        This function is the driver specific double buffering disabling
        steps to initialize buffers to a state where double buffering
        is disabled.

    Description:
        This function is the driver specific double buffering disabling
        steps to initialize buffers to a state where double buffering
        is disabled.

*/
// *****************************************************************************
void GFX_DRV_DoubleBufferDisable(void)
{
    drvGfxDrawBufferBaseAddress = drvGfxFrameBufferBaseAddress;
    GFX_SetDisplayArea(drvGfxFrameBufferBaseAddress);
    GFX_SetWorkArea1(drvGfxFrameBufferBaseAddress);
    GFX_SetWorkArea2(drvGfxFrameBufferBaseAddress);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_DoubleBufferSynchronize(void)

    Summary:
        This function synchronizes the frame buffer and the draw buffer
        contents when double buffering feature is enabled.

    Description:
        This function synchronizes the frame buffer and the draw buffer
        contents when double buffering feature is enabled. Contents of
        both frame and draw buffer will be the same after the function exits.

*/
// *****************************************************************************
GFX_STATUS GFX_DoubleBufferSynchronize(void)
{
    GFX_RECTANGULAR_AREA *pArea;
    uint32_t srcBufferAddress, dstBufferAddress, address;
    unsigned int temp, width, height, left, top, right, bottom;

    // check if feature is enabled
    if (GFX_DoubleBufferStatusGet() == GFX_FEATURE_DISABLED)
    {
        return (GFX_STATUS_SUCCESS);
    }

    // swap the two buffers (draw becomes frame and frame becomes draw)
    temp                   = drvGfxDrawBufferIndex;
    drvGfxDrawBufferIndex  = drvGfxFrameBufferIndex;
    drvGfxFrameBufferIndex = temp;

    // get the addresses of the frame buffers 
    // Note that for double buffering we always use the array of buffers
    // as the source of addresses for both frame and draw buffers
    srcBufferAddress = drvGfxFrameBufferArray[drvGfxFrameBufferIndex];
    dstBufferAddress = drvGfxFrameBufferArray[drvGfxDrawBufferIndex];

    // set the buffer to be displayed
    GFX_SetDisplayArea(srcBufferAddress);

    // make sure all commands in the command FIFO are executed since
    // the base addresses will be modified
    while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
    while(GFX_IsPuGpuBusy());

    // set the draw buffer address to the new address
    drvGfxDrawBufferBaseAddress = dstBufferAddress;

    // on this part we need to update the contents of the new draw buffer to
    // match the contents of the frame buffer.
    GFX_SetWorkArea1(srcBufferAddress);
    GFX_SetWorkArea2(dstBufferAddress);

    if(GFX_DoubleBufferSyncAllStatusGet() == GFX_FEATURE_ENABLED ||
       GFX_DoubleBufferSyncAreaCountGet() > GFX_MAX_INVALIDATE_AREAS)
    {

        GFX_DoubleBufferSyncAreaCountSet(0);
        GFX_RCC_SetSrcOffset(0);
        GFX_RCC_SetDestOffset(0);
        GFX_RCC_SetSize(    (uint16_t)DISP_HOR_RESOLUTION,
                            (uint16_t)DISP_VER_RESOLUTION);
        GFX_RCC_StartCopy(  RCC_COPY,
                            RCC_ROP_C,
                            RCC_SRC_ADDR_CONTINUOUS,
                            RCC_DEST_ADDR_CONTINUOUS);

        GFX_DoubleBufferSyncAllStatusClear();
        
    }
    else
    {
        temp = GFX_DoubleBufferSyncAreaCountGet();
        while(temp > 0)
        {
            pArea = GFX_DoubleBufferAreaGet(temp);

            left   = pArea->left;
            top    = pArea->top;
            right  = pArea->right;
            bottom = pArea->bottom;
            width  = right  - left + 1;
            height = bottom - top  + 1;

            // adjust the location on the screen in case the screen is rotated
            GFX_OrientationBlockAdjust(left, top, right, bottom, width, height);

            address = ((uint32_t)top * DISP_HOR_RESOLUTION) + left;

            // make sure there is space in the command FIFO for 4 commands
            while(GFX_FreeCommandSpaceCheck(4) == 0);

            GFX_RCC_SetSrcOffset(address);
            GFX_RCC_SetDestOffset(address);
            GFX_RCC_SetSize(width, height);
            GFX_RCC_StartCopy(  RCC_COPY,
                                RCC_ROP_C,
                                RCC_SRC_ADDR_DISCONTINUOUS,
                                RCC_DEST_ADDR_DISCONTINUOUS);

            temp--;
            GFX_DoubleBufferSyncAreaCountSet(temp);
        }

        GFX_DoubleBufferSyncAreaCountSet(0);

    }

    while(GFX_FreeCommandSpaceCheck(GFX_COMMAND_QUEUE_LENGTH) == 0);
    while(GFX_IsPuGpuBusy());

    GFX_SetWorkArea1(drvGfxDrawBufferBaseAddress);
    GFX_SetWorkArea2(drvGfxDrawBufferBaseAddress);

    // reset the synchronize flag
    GFX_DoubleBufferSynchronizeCancel();

    return (GFX_STATUS_SUCCESS);


}

// *****************************************************************************
/*  Function:
    GFX_FEATURE_STATUS GFX_DoubleBufferSynchronizeRequest(void)

    Summary:
        This function requests synchronization of the contents
        of the draw and frame buffer on the display driver layer.

    Description:
        This function requests synchronization of the contents of the
        draw and frame buffer on the display driver layer. The contents
        will be synchronized on the next vertical blanking period.

*/
// *****************************************************************************
GFX_STATUS GFX_DoubleBufferSynchronizeRequest(void)
{
    GFX_STATUS status = GFX_STATUS_SUCCESS;

    if (GFX_DoubleBufferStatusGet() == GFX_FEATURE_ENABLED)
    {
        // schedule synchronization
        GFX_DoubleBufferSynchronizeSet();
        DRV_GFX_VerticalBlankInterruptStart();
    }
    return (status);

}

#endif 

#ifndef GFX_CONFIG_PALETTE_DISABLE

// *****************************************************************************
/*
    Function:
        GFX_STATUS DRV_GFX_PaletteInitialize()

    Summary:
        This function initializes the palette feature in the driver.

    Description:
        This function initializes and enables the palette feature
        in the driver. This function should be called in the driver
        initialization routine and should be ready for usage after
        the driver has been initialized.

*/
// *****************************************************************************
void DRV_GFX_PaletteInitialize(void)
{
    pPendingPaletteEntry = NULL;
    PendingPaletteFlag = false;

    DRV_GFX_PaletteColorDepthSet(GFX_CONFIG_COLOR_DEPTH);

    while(_CLUTBUSY != 0)
    {
        Nop();
    }
    _CLUTRWEN = 0;
    _CLUTEN = 0;
    DRV_GFX_PaletteEnable();
}

// *****************************************************************************
/*
    Function:
        GFX_STATUS DRV_GFX_PaletteEnable()

     Summary:
        This function enables the palette mode feature of the library.

    Description:
        This function enables the palette mode feature of the library.
        This is a driver implemented function since the palette mode
        is dependent on the hardware used to support the feature.

*/
// *****************************************************************************
GFX_STATUS DRV_GFX_PaletteEnable(void)
{
    if(PaletteBpp <= GFX_16_BPP)
    {
        _PUBPP = PaletteBpp;
        _DPBPP = PaletteBpp;
        _CLUTEN = 1;
    }
    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*
    Function:
        GFX_STATUS DRV_GFX_PaletteDisable()

     Summary:
        This function disables the palette mode feature of the library.

    Description:
        This function disables the palette mode feature of the library.
        This is a driver implemented function since the palette mode
        is dependent on the hardware used to support the feature.

*/
// *****************************************************************************
GFX_STATUS DRV_GFX_PaletteDisable(void)
{
    _CLUTEN = 0;
    _PUBPP  = GFX_BITS_PER_PIXEL;
    _DPBPP  = GFX_BITS_PER_PIXEL;

    return (GFX_STATUS_SUCCESS);

}

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
GFX_FEATURE_STATUS GFX_PaletteStatusGet(void)
{
    if (_CLUTEN)
        return (GFX_FEATURE_ENABLED);
    else
        return (GFX_FEATURE_DISABLED);

}

// *****************************************************************************
/*
    Function:
        GFX_STATUS DRV_GFX_PaletteColorDepthSet()

     Summary:
        This function sets the color depth of the palette to be used
        with the palette mode feature of the library.

    Description:
        This function sets the color depth of the palette to be used
        with the palette mode feature of the library.
        This is a driver implemented function since the palette mode
        is dependent on the hardware used to support the feature.

*/
// *****************************************************************************
GFX_STATUS DRV_GFX_PaletteColorDepthSet(uint8_t depth)
{
    switch(depth)
    {
        case 1:         PaletteBpp = GFX_1_BPP;
                        break;

        case 2:         PaletteBpp = GFX_2_BPP;
                        break;

        case 4:         PaletteBpp = GFX_4_BPP;
                        break;

        case 8:         PaletteBpp = GFX_8_BPP;
                        break;

        default:        PaletteBpp = GFX_16_BPP;
                        return GFX_STATUS_FAILURE;
    }
    
    return GFX_STATUS_SUCCESS;
}

// *****************************************************************************
/*
    Function:
        GFX_STATUS DRV_GFX_PaletteSet()

    Summary:
        This is a driver specific palette set function that initializes
        the variables to enable the programming of the color look up
        table (CLUT) at the next vertical synchronization event.

    Description:
        This function sets up the variables to program the color look up
        table (CLUT) at the next vertical synchoronization period. The vertical
        synchronization is used since the display is not actively
        driven. This will reduce unwanted pixel colors in the screen
        during the CLUT programming.
 
        The start and count must be within bounds of the supported
        palette size. The function will return GFX_STATUS_FAILURE
        if the start value and/or start + count exceeds the size
        of the palette. Size of the palette is dependent on the
        color depth set by GFX_CONFIG_COLOR_DEPTH configuration.

*/
// *****************************************************************************
GFX_STATUS DRV_GFX_PaletteSet(
                                const GFX_PALETTE_ENTRY *pPaletteEntry,
                                uint16_t start,
                                uint16_t count)
{
    pPendingPaletteEntry = pPaletteEntry;
    PendingStartEntry = start;
    PendingLength = count;
    PendingPaletteFlag = true;

    DRV_GFX_VerticalBlankInterruptStart();

    return (GFX_STATUS_SUCCESS);

}

// *****************************************************************************
/*
    Function:
        GFX_STATUS DRV_GFX_ColorLookUpTableSet()

    Summary:
        This is the color look up table set up function.

    Description:
        This function programs the color look up table (CLUT) of the
        graphics module. This function is only executed when the
        palette feature is enabled.

        The start and count must be within bounds of the supported
        palette size. The function will return GFX_STATUS_FAILURE
        if the start value and/or start + count exceeds the size
        of the palette. Size of the palette is dependent on the
        color depth set by GFX_CONFIG_COLOR_DEPTH configuration.

*/
// *****************************************************************************
inline GFX_STATUS __attribute__ ((always_inline)) DRV_GFX_ColorLookUpTableSet(
                                const GFX_PALETTE_ENTRY *pPaletteEntry,
                                uint16_t start,
                                uint16_t count)
{
    uint16_t counter, address;

    address = start;

    // check for bounds and make sure palette is initialized
    if(pPaletteEntry == NULL)
        return (GFX_STATUS_FAILURE);
    
    if ((start + count) > 256)
        return (GFX_STATUS_FAILURE);
    
    _CLUTRWEN = 1;

    // before we proceed, make sure all commands in queue have been executed
    // this is to avoid pixels being set to wrong colors
    while(_CLUTBUSY);

    for(counter = 0; counter < count; counter++)
    {
        // prep the registers to program an entry
        _CLUTADR = address;
        _CLUTWR  = pPaletteEntry[address].value;

        // adjust the address
        address++;

        while(_CLUTBUSY);
    }
    _CLUTRWEN = 0;

    return (GFX_STATUS_SUCCESS);
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
GFX_STATUS GFX_PaletteSet(
                                GFX_RESOURCE_HDR *pPalette,
                                uint16_t start,
                                uint16_t count)

{
    GFX_STATUS status = GFX_STATUS_SUCCESS;

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
            return (GFX_STATUS_FAILURE);
            break;

    }

    // wait here until the palette has changed
    while(PendingPaletteFlag == true);
    return (status);
}



#endif 

// *****************************************************************************
/*
    Function:
        void _GFX1Interrupt()

    Summary:
        This is the ISR to handle the vertical margin or vertical
        synchronization interrupt.

    Description:
        This is the ISR to handle the vertical margin or vertical
        synchronization interrupt.

*/
// *****************************************************************************
void __attribute__((interrupt, shadow, no_auto_psv)) _GFX1Interrupt(void)
{
    // reset the graphics module interrupt
    _GFX1IF = 0;

    if(_VMRGNIF)
    {

#ifndef GFX_CONFIG_PALETTE_DISABLE

        if (PendingPaletteFlag == true)
        {
            DRV_GFX_ColorLookUpTableSet(pPendingPaletteEntry,
                                        PendingStartEntry,
                                        PendingLength);

            pPendingPaletteEntry = NULL;
            PendingPaletteFlag = false;
        }

#endif

#ifndef GFX_CONFIG_DOUBLE_BUFFERING_DISABLE

        if ((GFX_DoubleBufferStatusGet() == GFX_FEATURE_ENABLED) &&
            (GFX_DoubleBufferSynchronizeStatusGet() == GFX_FEATURE_ENABLED))
        {
            // synchronize the draw and frame buffer
            GFX_DoubleBufferSynchronize();
            // reset the synchronize request flag
            GFX_DoubleBufferSynchronizeCancel();
        }

#endif
        // disable the vertical interrupt
        // this will only be enabled when there is a request for it
        // see double buffering synchronization request
        _VMRGNIF = 0;
        _VMRGNIE = 0;

    }

}

#if  !defined (GFX_CONFIG_PALETTE_DISABLE)  || !defined (GFX_CONFIG_DOUBLE_BUFFERING_DISABLE)

// *****************************************************************************
/*
    Function:
        void DRV_GFX_VerticalBlankInterruptStart()

    Summary:
        This is function sets up the vertical margin or vertical
        synchronization interrupt.

    Description:
        This is function sets up the vertical margin or vertical
        synchronization interrupt. 

*/
// *****************************************************************************
void DRV_GFX_VerticalBlankInterruptStart(void)
{
#if (GFX_LCD_TYPE == GFX_LCD_TFT)

        _VMRGNIF = 0;
        _VMRGNIE = 1;
        _GFX1IE = 1;

#else
    // if we want to use palette in non-TFT this code will change the
    // palette when interrupt is started

#ifndef GFX_CONFIG_PALETTE_DISABLE

    if (PendingPaletteFlag == true)
    {
        DRV_GFX_ColorLookUpTableSet(pPendingPaletteEntry,
                                    PendingStartEntry,
                                    PendingLength);

        pPendingPaletteEntry = NULL;
        PendingPaletteFlag = false;
    }

#endif
#endif

}

#endif
#endif //#if defined (GFX_USE_DISPLAY_CONTROLLER_MCHP_DA210)



