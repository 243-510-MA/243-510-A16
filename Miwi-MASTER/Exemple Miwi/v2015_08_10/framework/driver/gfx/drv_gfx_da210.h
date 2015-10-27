/*******************************************************************************
 Module for Microchip Graphics Library - Display Controller Driver for
                                         PIC24FJ256DA210 Device Family
  Company:
    Microchip Technology Inc.

  File Name:
    drv_gfx_da210.h

  Summary:
    Display Driver header file for use with the Microchip Graphics Library.

  Description:
    This module implements the display driver for the following controllers:
    *  built in grahics controller in PIC24FJ256DA210 Device Family
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

#ifndef _MCHP_DISPLAY_DRIVER_H
#define _MCHP_DISPLAY_DRIVER_H

#include <stdint.h>
#include "system_config.h"

#if defined (GFX_USE_DISPLAY_CONTROLLER_MCHP_DA210)

/*********************************************************************
* Error Checking
*********************************************************************/
    #ifndef DISP_HOR_RESOLUTION
        #error "DISP_HOR_RESOLUTION must be defined in system_config.h"
    #endif
    #ifndef DISP_VER_RESOLUTION
        #error "DISP_VER_RESOLUTION must be defined in system_config.h"
    #endif
    #ifndef DISP_ORIENTATION
        #error "DISP_ORIENTATION must be defined in system_config.h"
    #endif

/*********************************************************************
* Overview: Horizontal synchronization timing in pixels
*                  (from the glass datasheet).
*********************************************************************/

    #ifndef DISP_HOR_PULSE_WIDTH
        #error "DISP_HOR_PULSE_WIDTH must be defined in system_config.h"
    #endif
    #ifndef DISP_HOR_BACK_PORCH
        #error "DISP_HOR_BACK_PORCH must be defined in system_config.h"
    #endif
    #ifndef DISP_HOR_FRONT_PORCH
        #error "DISP_HOR_FRONT_PORCH must be defined in system_config.h"
    #endif

/*********************************************************************
* Overview: Vertical synchronization timing in lines
*                  (from the glass datasheet).
*********************************************************************/
    #ifndef DISP_VER_PULSE_WIDTH
        #error "DISP_VER_PULSE_WIDTH must be defined in system_config.h"
    #endif
    #ifndef DISP_VER_BACK_PORCH
        #error "DISP_VER_BACK_PORCH must be defined in system_config.h"
    #endif
    #ifndef DISP_VER_FRONT_PORCH
        #error "DISP_VER_FRONT_PORCH must be defined in system_config.h"
    #endif

/*********************************************************************
* Overview: Some unsupported feature combination.
*
*********************************************************************/
#if !defined(GFX_CONFIG_PALETTE_DISABLE) && !defined (GFX_CONFIG_ALPHABLEND_DISABLE)
    #error "Alpha Blending can only be used when GFX_CONFIG_COLOR_DEPTH is 16BPP."
#endif

/*********************************************************************
* Overview: This typedef defines a structure so we can easily access
*           the words, bytes and double words for manipulation.
*********************************************************************/
typedef union
{
    uint8_t     uint8Address[4];
    uint16_t    uint16Address[2];
    uint32_t    uint32Address;
} MCHPGFX_ADDRESS;


/*********************************************************************
* Overview: Additional hardware-accelerated functions can be implemented
*           in the driver. These definitions exclude the PutPixel()-based
*           functions in the primitives layer (Primitive.c file) from compilation.
*********************************************************************/

//extern volatile uint32_t drvFrameBufferBaseAddress;
//extern volatile uint32_t _workArea1BaseAddr;
//extern volatile uint32_t _workArea2BaseAddr;

/*********************************************************************
* Overview: Sets the type of display glass used. Define this in the Hardware Profile.
*			- #define GFX_LCD_TYPE    GFX_LCD_TFT - sets type TFT display
*			- #define GFX_LCD_TYPE    GFX_LCD_CSTN - sets type color STN display
*			- #define GFX_LCD_TYPE    GFX_LCD_MSTN - sets type mon STN display
*			- #define GFX_LCD_TYPE    GFX_LCD_OFF - display is turned off
*
*********************************************************************/
#ifndef GFX_LCD_TYPE
#define GFX_LCD_TYPE
#endif

#define GFX_LCD_TFT  0x01			// Type TFT Display 
#define GFX_LCD_CSTN 0x03			// Type Color STN Display 
#define GFX_LCD_MSTN 0x02			// Type Mono STN Display 
#define GFX_LCD_OFF  0x00			// display is turned off

/*********************************************************************
* Overview: Sets the STN glass data width. Define this in the Hardware Profile.
*			- #define STN_DISPLAY_WIDTH    STN_DISPLAY_WIDTH_4 - use 4-bit wide interface
*			- #define STN_DISPLAY_WIDTH    STN_DISPLAY_WIDTH_8 - Use 8-bit wide interface
*			- #define STN_DISPLAY_WIDTH    STN_DISPLAY_WIDTH_16 - Use 16-bit wide interface
*
*********************************************************************/
#ifndef STN_DISPLAY_WIDTH
#define STN_DISPLAY_WIDTH
#endif

#define STN_DISPLAY_WIDTH_4  0x00	// display interface is 4 bits wide
#define STN_DISPLAY_WIDTH_8  0x01	// display interface is 8 bits wide
#define STN_DISPLAY_WIDTH_16 0x02	// display interface is 16 bits wide

#define GFX_1_BPP  0x00
#define GFX_2_BPP  0x01
#define GFX_4_BPP  0x02
#define GFX_8_BPP  0x03
#define GFX_16_BPP 0x04

#if (GFX_CONFIG_COLOR_DEPTH == 1)
    #define GFX_BITS_PER_PIXEL                  GFX_1_BPP
    #define GFX_COLOR_MASK                      0x0001
#elif (GFX_CONFIG_COLOR_DEPTH == 2)
    #define GFX_BITS_PER_PIXEL                  GFX_2_BPP
    #define GFX_COLOR_MASK                      0x0003
#elif (GFX_CONFIG_COLOR_DEPTH == 4)
    #define GFX_BITS_PER_PIXEL                  GFX_4_BPP
    #define GFX_COLOR_MASK                      0x000F
#elif (GFX_CONFIG_COLOR_DEPTH == 8)
    #define GFX_BITS_PER_PIXEL                  GFX_8_BPP
    #define GFX_COLOR_MASK                      0x00FF
#else
    #define GFX_BITS_PER_PIXEL                  GFX_16_BPP
    #define GFX_COLOR_MASK                      0xFFFF
#endif

#define GFX_COMMAND_QUEUE_LENGTH            16

#define GFX_ACTIVE_HIGH                     1
#define GFX_ACTIVE_LOW                      0

/* RCC GPU Commands */
#define RCC_SRCADDR                         0x62000000ul
#define RCC_DESTADDR                        0x63000000ul
#define RCC_RECTSIZE                        0x64000000ul
#define RCC_COLOR                           0x66000000ul
#define RCC_STARTCOPY                       0x67000000ul

/*********************************************************************
* Overview: Type of Rectangle Copy Operations. Select one of the following
*			rectangle copy operations and together with the ROP; the source,
*           destination, current color set and transparency are evaluated
*           on each pixel and the result written to the destination. 
*			- RCC_COPY - Copies the source data to the destination address with
*                        the selected ROP. 
*			- RCC_SOLID_FILL - Fills the specified rectangle with the current color set. 
*			- RCC_TRANSPARENT_COPY - Operation is the same as the COPY operation except that
*									 the source data is compared against the current color 
*									 set. If the values match, the source data is not 
*									 written to the destination. The source image is, 
*									 therefore, transparent at such a location, allowing
*
*********************************************************************/
#define RCC_COPY                            0x00000080ul
// <COMBINE RCC_COPY>
#define RCC_SOLID_FILL                      0x00000000ul
// <COMBINE RCC_COPY>
#define RCC_TRANSPARENT_COPY                0x00000300ul

/*********************************************************************
* Overview: Raster Operation (ROP) option. Select one of the following
*			16 raster operation options whenever Rectangle Copy
*			Processing Unit (RCCGPU) is used. The raster operation
*           is performed on the source (S) and destination (D) data.
*			and the result written to the destination (D).
*			- RCC_ROP_0 - 0 (BLACK)
*			- RCC_ROP_1 - not (S or D)
*			- RCC_ROP_2 - (not S) and D
*			- RCC_ROP_3 - not (S)
*			- RCC_ROP_4 - S and not (D))
*			- RCC_ROP_5 - not (D)
*			- RCC_ROP_6 - S xor D
*			- RCC_ROP_7 - not (S and D)
*			- RCC_ROP_8 - S and D
*			- RCC_ROP_9 - not (S xor D)
*			- RCC_ROP_A - D
*			- RCC_ROP_B - not (S) or D
*			- RCC_ROP_C - S
*			- RCC_ROP_D - S or not (D)
*			- RCC_ROP_E - S or D
*			- RCC_ROP_F - 1 (WHITE)
*
*********************************************************************/
#define RCC_ROP_0                           0x00000000ul		
// <COMBINE RCC_ROP_0>
#define RCC_ROP_1                           0x00000008ul		// not (S or D)
// <COMBINE RCC_ROP_0>
#define RCC_ROP_2                           0x00000010ul		// (not S) and D
// <COMBINE RCC_ROP_0>
#define RCC_ROP_3                           0x00000018ul		// not (S)
// <COMBINE RCC_ROP_0>
#define RCC_ROP_4                           0x00000020ul		// S and not (D)
// <COMBINE RCC_ROP_0>
#define RCC_ROP_5                           0x00000028ul		// not (D)
// <COMBINE RCC_ROP_0>
#define RCC_ROP_6                           0x00000030ul		// S xor D
// <COMBINE RCC_ROP_0>
#define RCC_ROP_7                           0x00000038ul		// not (S and D)
// <COMBINE RCC_ROP_0>
#define RCC_ROP_8                           0x00000040ul		// S and D
// <COMBINE RCC_ROP_0>
#define RCC_ROP_9                           0x00000048ul		// not (S xor D)
// <COMBINE RCC_ROP_0>
#define RCC_ROP_A                           0x00000050ul		// D
// <COMBINE RCC_ROP_0>
#define RCC_ROP_B                           0x00000058ul		// not (S) or D
// <COMBINE RCC_ROP_0>
#define RCC_ROP_C                           0x00000060ul		// S 
// <COMBINE RCC_ROP_0>
#define RCC_ROP_D                           0x00000068ul		// S or not (D)
// <COMBINE RCC_ROP_0>
#define RCC_ROP_E                           0x00000070ul		// S or D
// <COMBINE RCC_ROP_0>
#define RCC_ROP_F                           0x00000078ul		// 1 (WHITE)
// <COMBINE RCC_ROP_0>

/*********************************************************************
* Overview: Source (S) and Destination (D) data type. When performing 
*           executing commands on the Rectangle Copy Processing Unit 
*           (RCCGPU). The source and destination data can be treated 
*           as a continuous block of data in memory or a discontinuous
*			data in memory. This gives flexibility to the operation
*           where an copy operation can be performed to data already
*           present in the display buffer or anywhere else in data memory.
*           Both source and destination data can be set to 
*           continuous or discontinuous data. These macros
*           are only used in RCCGPU operations.
*			- RCC_SRC_ADDR_CONTINUOUS - source data is continuous
*			- RCC_SRC_ADDR_DISCONTINUOUS - source data is discontinuous
*			- RCC_DEST_ADDR_CONTINUOUS - destination data is continuous
*			- RCC_DEST_ADDR_DISCONTINUOUS - destination data is discontinuous
*
*********************************************************************/
#define RCC_SRC_ADDR_CONTINUOUS             0x00000002ul		
// <COMBINE RCC_SRC_ADDR_CONTINUOUS>
#define RCC_SRC_ADDR_DISCONTINUOUS          0x00000000ul		
// <COMBINE RCC_SRC_ADDR_CONTINUOUS>
#define RCC_DEST_ADDR_CONTINUOUS            0x00000004ul		
// <COMBINE RCC_SRC_ADDR_CONTINUOUS>
#define RCC_DEST_ADDR_DISCONTINUOUS         0x00000000ul		

#define GFX_SetCommand(c)                   { G1CMDL = ((MCHPGFX_ADDRESS)((uint32_t)c)).uint16Address[0]; G1CMDH = ((MCHPGFX_ADDRESS)((uint32_t)c)).uint16Address[1]; Nop(); }

#define GFX_SetDisplayArea(a)               { drvGfxFrameBufferBaseAddress = (uint32_t)a; G1DPADRL = ((MCHPGFX_ADDRESS)((uint32_t)a)).uint16Address[0]; G1DPADRH = ((MCHPGFX_ADDRESS)((uint32_t)a)).uint16Address[1]; }
#define GFX_SetWorkArea1(a)                 { G1W1ADRL = ((MCHPGFX_ADDRESS)((uint32_t)a)).uint16Address[0]; G1W1ADRH = ((MCHPGFX_ADDRESS)((uint32_t)a)).uint16Address[1]; }
#define GFX_SetWorkArea2(a)                 { G1W2ADRL = ((MCHPGFX_ADDRESS)((uint32_t)a)).uint16Address[0]; G1W2ADRH = ((MCHPGFX_ADDRESS)((uint32_t)a)).uint16Address[1]; }

#define GFX_RCC_SetSrcOffset(srcOffset)     GFX_SetCommand(RCC_SRCADDR  | (uint32_t)(srcOffset));      /* Don't do error checking or error corrections */
#define GFX_RCC_SetDestOffset(dstOffset)    GFX_SetCommand(RCC_DESTADDR | (uint32_t)(dstOffset));      /* Don't do error checking or error corrections */
#define GFX_RCC_SetSize(width, height)      GFX_SetCommand(RCC_RECTSIZE | (((uint32_t)(width)) << 12) | (uint32_t)height)     /* Don't do error checking or error corrections */  
#define GFX_RCC_SetColor(color)             GFX_SetCommand(RCC_COLOR    | (color & GFX_COLOR_MASK)) /* Don't do error checking or error corrections */
#define GFX_RCC_StartCopy(type_of_copy, rop, src_addr_type, dest_addr_type)      GFX_SetCommand(RCC_STARTCOPY | type_of_copy | rop | src_addr_type | dest_addr_type)     /* Don't do error checking or error corrections */

#define CHR_FGCOLOR                         0x50000000ul
#define CHR_BGCOLOR                         0x51000000ul
#define CHR_FONTBASE                        0x52000000ul
#define CHR_PRINTCHAR                       0x53000000ul
#define CHR_TEXTAREASTART                   0x58000000ul
#define CHR_TEXTAREAEND                     0x59000000ul
#define CHR_PRINTPOS                        0x5A000000ul

#define CHR_TRANSPARENT                     0x00800000ul
#define CHR_OPAQUE                          0x00000000ul

#define GFX_CHR_SetFgColor(color)           GFX_SetCommand(CHR_FGCOLOR | color)
#define GFX_CHR_SetBgColor(color)           GFX_SetCommand(CHR_BGCOLOR | color)
#define GFX_CHR_SetFont(fontaddress)        { GFX_SetCommand(CHR_FONTBASE | fontaddress); }

#define GFX_CHR_SetTextAreaStart(x, y)      GFX_SetCommand(CHR_TEXTAREASTART | ((uint32_t)x) << 12 | y)
#define GFX_CHR_SetTextAreaEnd(x, y)        GFX_SetCommand(CHR_TEXTAREAEND | ((uint32_t)x) << 12 | y)
#define GFX_CHR_SetPrintPos(x, y)           GFX_SetCommand(CHR_PRINTPOS | ((uint32_t)x) << 12 | y) 
#define GFX_CHR_PrintChar(character, transparency)   GFX_SetCommand(CHR_PRINTCHAR | transparency | character)   

#define IPU_SRCOFFSET                       0x71000000ul
#define IPU_DSTOFFSET                       0x72000000ul
#define IPU_DSTSIZE                         0x74000000ul

#define GFX_IPU_SetSrcOffset(offset)        (GFX_SetCommand(IPU_SRCOFFSET | (uint32_t) offset))
#define GFX_IPU_SetDestOffset(offset)       (GFX_SetCommand(IPU_DSTOFFSET | (uint32_t) offset))
#define GFX_IPU_Inflate(uint8_ts)           (GFX_SetCommand(IPU_DSTSIZE   | (uint32_t) uint8_ts ))

#define GFX_IPU_GetFinalBlock()             (G1IPU & 0x0001)
#define GFX_IPU_GetDecompressionDone()      (G1IPU & 0x0002)
#define GFX_IPU_GetDecompressionError()     (G1IPU & 0x003C)
#define GFX_IPU_GetWrapAroundError()        (G1IPU & 0x0004)
#define GFX_IPU_GetBlockLengthError()       (G1IPU & 0x0008)
#define GFX_IPU_GetUndefinedBlockError()    (G1IPU & 0x0010)
#define GFX_IPU_GetHuffmannError()          (G1IPU & 0x0020)

#define GFX_IsRccGpuBusy()                  (G1STAT & 0x0040) //(_RCCBUSY)
#define GFX_IsIpuGpuBusy()                  (G1STAT & 0x0080) //(_IPUBUSY)
#define GFX_IsChrGpuBusy()                  (G1STAT & 0x0020) //(_CHRBUSY)
#define GFX_IsPuGpuBusy()                   (G1STAT & 0x8000) //(_PUBUSY)
#define GFX_WaitForRccGpu()                 while ((G1STAT & 0x0040) == 0x0040); 
#define GFX_WaitForIpuGpu()                 while ((G1STAT & 0x0080) == 0x0080);
#define GFX_WaitForChrGpu()                 while ((G1STAT & 0x0020) == 0x0020); 
#define GFX_WaitForGpu()                    while ((G1STAT & 0x8000) == 0x8000); 

#define GFX_DATA_CONTINUOUS                 (0x01)
#define GFX_DATA_DISCONTINUOUS              (0x00)

/* Definition of macros

 * GFX_DISPLAY_BUFFER_START_ADDRESS - This defines the constant used to set the
 * location of the display buffer location. for the PIC24FJ256DA210 the
 * address used must be consistent with the "DATA SPACE MEMORY MAP FOR
 * PIC24FJ256DA210 FAMILY DEVICES"
 * GFX_DISPLAY_BUFFER_LENGTH - This defines the size of the display buffer in
 * bytes.

*/

// Calculate Display Buffer Size required in bytes

#define GFX_DISPLAY_PIXEL_COUNT ((uint32_t)DISP_HOR_RESOLUTION*DISP_VER_RESOLUTION)

#if (GFX_CONFIG_COLOR_DEPTH == 16)
    #define GFX_REQUIRED_DISPLAY_BUFFER_SIZE_IN_BYTES       (GFX_DISPLAY_PIXEL_COUNT*2)
#elif (GFX_CONFIG_COLOR_DEPTH == 8)
    #define GFX_REQUIRED_DISPLAY_BUFFER_SIZE_IN_BYTES       (GFX_DISPLAY_PIXEL_COUNT)
#elif (GFX_CONFIG_COLOR_DEPTH == 4)
    #define GFX_REQUIRED_DISPLAY_BUFFER_SIZE_IN_BYTES       (GFX_DISPLAY_PIXEL_COUNT/2)
#elif (GFX_CONFIG_COLOR_DEPTH == 1)
    #define GFX_REQUIRED_DISPLAY_BUFFER_SIZE_IN_BYTES       (GFX_DISPLAY_PIXEL_COUNT/8)
#endif

#define GFX_BUFFER1 (GFX_DISPLAY_BUFFER_START_ADDRESS)
#ifndef GFX_CONFIG_DOUBLE_BUFFERING_DISABLE
    #define GFX_BUFFER2 (GFX_DISPLAY_BUFFER_START_ADDRESS + GFX_REQUIRED_DISPLAY_BUFFER_SIZE_IN_BYTES)
#endif

#ifndef GFX_CONFIG_IPU_DECODE_DISABLE
    /*
      Notes:
      When using IPU with internal memory only, make sure the buffers can fit in
      the internal memory.
      IPU Buffer locations:

      - Using double buffering
        GFX_DECOMPRESSED_DATA_RAM_ADDRESS = GFX_DISPLAY_BUFFER_START_ADDRESS + (GFX_DISPLAY_BUFFER_LENGTH*2)
        GFX_COMPRESSED_DATA_RAM_ADDRESS = GFX_DECOMPRESSED_DATA_RAM_ADDRESS + GFX_DECOMPRESSED_BUFFER_SIZE

      - NOT Using double buffering
        GFX_DECOMPRESSED_DATA_RAM_ADDRESS = GFX_DISPLAY_BUFFER_START_ADDRESS + GFX_DISPLAY_BUFFER_LENGTH
        GFX_COMPRESSED_DATA_RAM_ADDRESS = GFX_DECOMPRESSED_DATA_RAM_ADDRESS + GFX_DECOMPRESSED_BUFFER_SIZE

      Since the IPU buffers are located right after the display buffer(s) the memory locations
      are dependent of the starting address of the display buffer.
      To locate the IPU buffers manually (i.e. setting the location manually) define the
      following macros in the the system_config.h:
        #define GFX_DECOMPRESSED_DATA_RAM_ADDRESS ????
        #define GFX_COMPRESSED_DATA_RAM_ADDRESS   ????

      The following macro values can be obtained from the output of the Graphics Resource Converter (GRC)
      when generating image files for IPU:

      #define GFX_COMPRESSED_BUFFER_SIZE              (13950)   // value used in based on the GRC output
      #define GFX_DECOMPRESSED_BUFFER_SIZE            (19216)   // value used in based on the GRC output
      #define GFX_IPU_TEMP_DATA_TRANSFER_ARRAY_SIZE   (1024)    // 1024 is the maximum value allowed for the transfer array

    */
    // Error checks 
    #ifndef GFX_IPU_TEMP_DATA_TRANSFER_ARRAY_SIZE
            #error "GFX_IPU_TEMP_DATA_TRANSFER_ARRAY_SIZE must be defined in system_config.h. See documentation in drv_gfx_da210.h"
    #endif

    #ifndef GFX_COMPRESSED_BUFFER_SIZE
            #error "GFX_COMPRESSED_BUFFER_SIZE must be defined in system_config.h. See documentation in drv_gfx_da210.h"
    #endif

    #ifndef GFX_DECOMPRESSED_BUFFER_SIZE
            #error "GFX_DECOMPRESSED_BUFFER_SIZE must be defined in system_config.h. See documentation in drv_gfx_da210.h"
    #endif

#ifndef GFX_CONFIG_DOUBLE_BUFFERING_DISABLE
        #ifndef GFX_DECOMPRESSED_DATA_RAM_ADDRESS
	        #define GFX_DECOMPRESSED_DATA_RAM_ADDRESS   (GFX_DISPLAY_BUFFER_START_ADDRESS + (GFX_DISPLAY_BUFFER_LENGTH*2))
        #endif	
        #ifndef GFX_COMPRESSED_DATA_RAM_ADDRESS       
            #define GFX_COMPRESSED_DATA_RAM_ADDRESS     (GFX_DISPLAY_BUFFER_START_ADDRESS + (GFX_DISPLAY_BUFFER_LENGTH*2) + GFX_DECOMPRESSED_BUFFER_SIZE)     
        #endif    	
    #else
        #ifndef GFX_DECOMPRESSED_DATA_RAM_ADDRESS
	        #define GFX_DECOMPRESSED_DATA_RAM_ADDRESS   (GFX_DISPLAY_BUFFER_START_ADDRESS + GFX_DISPLAY_BUFFER_LENGTH)
        #endif	
        #ifndef GFX_COMPRESSED_DATA_RAM_ADDRESS       
            #define GFX_COMPRESSED_DATA_RAM_ADDRESS     (GFX_DISPLAY_BUFFER_START_ADDRESS + GFX_DISPLAY_BUFFER_LENGTH + GFX_DECOMPRESSED_BUFFER_SIZE)     
        #endif    	
    #endif
    
#endif

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

    Precondition:
        None.

    Parameters:
        srcAddr - the base address of the data to be moved
        dstAddr - the base address of the new location of the moved data
        srcOffset - offset of the data to be moved with respect to the
                    source base address.
        dstOffset - offset of the new location of the moved data respect
                    to the source base address.
        width - width of the block of data to be moved
        height - height of the block of data to be moved

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
uint16_t DRV_GFX_BlockOffsetCopy(
                                uint32_t srcAddr,
                                uint32_t dstAddr,
                                uint32_t srcOffset,
                                uint32_t dstOffset,
                                uint16_t width,
                                uint16_t height);

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

    Precondition:
        None.

    Parameters:
        srcAddr - Base Address of the source window,
        dstPage - Base Address of the destination window,
        srcX - x location of the left top corner of the source window
               respect to the srcPage.
        srcY - y location of the left top corner of the source window
               respect to the srcPage.
        dstX - x location of the left top corner of the destination window
               respect to the dstPage.
        dstY - y location of the left top corner of the destination window
               respect to the dstPage.
        width - the width in pixels of the window to copy
        height - the height in pixels of the window to copy

    Returns:
        None.

    Example:
        None.

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
                                uint16_t height);

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

    Precondition:
        None.

    Parameters:
        srcAddr - the base address of the data to be moved
        dstAddr - the base address of the new location of the moved data
        srcOffset - offset of the data to be moved with respect to the
					 source base address.
        dstOffset - offset of the new location of the moved data respect
					 to the source base address.
        srcType - sets the source type
                  (GFX_DATA_CONTINUOUS or GFX_DATA_DISCONTINUOUS)
        dstType - sets the destination type
                  (GFX_DATA_CONTINUOUS or GFX_DATA_DISCONTINUOUS)
        copyOp - sets the type of copy operation
			- RCC_SOLID_FILL: Solid fill of the set color
			- RCC_COPY: direct copy of source to destination
			- RCC_TRANSPARENT_COPY: copy with transparency.
                                     Transparency color is set by color
        rop - sets the raster operation equation
			- RCC_ROP_0: Solid black color fill
			- RCC_ROP_1: not (Source or Destination)
			- RCC_ROP_2: (not Source) and Destination
			- RCC_ROP_3: not Source
			- RCC_ROP_4: Source and (not Destination)
			- RCC_ROP_5: not Destination
			- RCC_ROP_6: Source xor Destination
			- RCC_ROP_7: not (Source and Destination)
			- RCC_ROP_8: Source and Destination
			- RCC_ROP_9: not (Source xor Destination)
			- RCC_ROP_A: Destination
			- RCC_ROP_B: (not Source) or Destination
			- RCC_ROP_C: Source
			- RCC_ROP_D: Source or (not Destination)
			- RCC_ROP_E: Source or Destination
			- RCC_ROP_F: Solid white color fill
        color - color value used when transparency operation is set
                or using solid color fill
        width - width of the block of data to be moved
        height - height of the block of data to be moved

    Returns:
        - Returns 0 when device is busy and operation is not completely performed.
        - Returns 1 when the operation is completely performed.

    Example:
        None.

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
                                uint16_t height);

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

    Precondition:
        Source compressed data must be encoded using the
        DEFLATE algorithm with fixed Huffman codes.

    Parameters:
        SrcAddress  - location of the compressed data.
        DestAddress - location of the decompressed data.
        nBytes      - Number of bytes to be decompressed.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
uint8_t DRV_GFX_Inflate(
                                uint32_t srcAddress,
                                uint32_t destAddress,
                                uint32_t nBytes);


typedef struct
{
    uint16_t X;
    uint16_t Y;
    uint16_t W;
    uint16_t H;
} RectangleArea;

#ifndef GFX_CONFIG_PALETTE_DISABLE

    #include "gfx/gfx_palette.h"

#endif

#endif //#if defined (GFX_USE_DISPLAY_CONTROLLER_MCHP_DA210)

#endif // _MCHP_DISPLAY_DRIVER_H
