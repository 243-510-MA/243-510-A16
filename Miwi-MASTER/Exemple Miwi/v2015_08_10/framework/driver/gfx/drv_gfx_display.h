/*******************************************************************************
 Module for Microchip Graphics Library - Display Driver Layer

  Company:
    Microchip Technology Inc.

  File Name:
    drv_gfx_display.h

  Summary:
    Display driver header file.

  Description:
    This module declares all display driver layer API.
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

#ifndef _DRV_GFX_DISPLAY_H
// DOM-IGNORE-BEGIN
#define _DRV_GFX_DISPLAY_H
// DOM-IGNORE-END

#include <stdint.h>
#include "system_config.h"
#include "gfx/gfx_types_macros.h"

// DOM-IGNORE-BEGIN
/*********************************************************************
 Overview: Data types for drivers. This will facilitate easy
           access smaller chunks of larger data types when sending
           or receiving data (for example byte sized send/receive
           over parallel 8-bit interface.
*********************************************************************/
// DOM-IGNORE-END
typedef union
{
    uint8_t  uint328BitValue[4];
    uint16_t uint3216BitValue[2];
    uint32_t uint32Value;
}DRIVER_UINT32_UNION;

typedef union
{
    uint8_t  uint168BitValue[2];
    uint16_t uint16Value;
}DRIVER_UINT16_UNION;

// DOM-IGNORE-END

// *****************************************************************************
/*  
    <GROUP driver_init_functions>
 
    Function:
        void DRV_GFX_Initialize(void)

    Summary:
        Initialize the graphics display driver.

    Description:
        This function initializes the graphics display driver. This
        function will be called by the application.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void DRV_GFX_Initialize(void);


// *****************************************************************************
/*
    <GROUP driver_config_functions>

    Function:
        uint16_t GFX_MaxXGet(void)

    Summary:
        This function returns the maximum horizontal pixel coordinate.

    Description:
        This function returns the maximum horizontal pixel coordinate of
        the chosen display. The value returned is equal to the horizontal
        dimension minus 1. The horizontal dimension is also dependent on
        the rotation of the screen. Rotation can be set in the hardware
        configuration. 

   Precondition:
        None.

    Parameters:
        None

    Returns:
        Maximum horizontal pixel coordinate.

    Example:
        None.

*/
// *****************************************************************************
#if (DISP_ORIENTATION == 90)
    #define GFX_MaxXGet() (DISP_VER_RESOLUTION - 1)
#elif (DISP_ORIENTATION == 270)
    #define GFX_MaxXGet() (DISP_VER_RESOLUTION - 1)
#elif (DISP_ORIENTATION == 0)
    #define GFX_MaxXGet() (DISP_HOR_RESOLUTION - 1)
#elif (DISP_ORIENTATION == 180)
    #define GFX_MaxXGet() (DISP_HOR_RESOLUTION - 1)
#endif


// *****************************************************************************
/*
    <GROUP driver_config_functions>

    Function:
        uint16_t GFX_MaxYGet(void)

    Summary:
        This function returns the maximum vertical pixel coordinate.

    Description:
        This function returns the maximum vertical pixel coordinate of
        the chosen display. The value returned is equal to the vertical
        dimension minus 1. The vertical dimension is also dependent on
        the rotation of the screen. Rotation can be set in the hardware
        configuration. 

   Precondition:
        None.

    Parameters:
        None

    Returns:
        Maximum vertical pixel coordinate.

    Example:
        None.

*/
// *****************************************************************************
#if (DISP_ORIENTATION == 90)
    #define GFX_MaxYGet() (DISP_HOR_RESOLUTION - 1)
#elif (DISP_ORIENTATION == 270)
    #define GFX_MaxYGet() (DISP_HOR_RESOLUTION - 1)
#elif (DISP_ORIENTATION == 0)
    #define GFX_MaxYGet() (DISP_VER_RESOLUTION - 1)
#elif (DISP_ORIENTATION == 180)
    #define GFX_MaxYGet() (DISP_VER_RESOLUTION - 1)
#endif

// *****************************************************************************
/*  
    <GROUP driver_rendering_functions>

    Function:
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

    Precondition:
        Color must be set by GFX_ColorSet().

    Parameters:
        x - Horizontal position of the pixel.
        y - Vertical position of the pixel.

    Returns:
        Status of the character rendering. (see GFX_STATUS).

    Example:
        <code>

            void RedBarDraw(
                            int left,
                            int top,
                            int right,
                            int bottom)
            {
                int   x, y;

                // assume RED is a red color
                GFX_ColorSet(RED);
 
                for(y = top; y < bottom + 1; y++)
                    for(x = left; x < right + 1; x++)
                        GFX_PixelPut(x, y);
            }

        </code>

*/
// *****************************************************************************
GFX_STATUS GFX_PixelPut(uint16_t x, uint16_t y);

// *****************************************************************************
/*
    <GROUP driver_rendering_functions>

    Function:
        GFX_COLOR GFX_PixelGet(
                    uint16_t    x,
                    uint16_t    y)

    Summary:
        Gets color of the pixel on the given position.

    Description:
        This routine gets the pixel on the given position.

        If position is not on the frame buffer, then the behavior
        is undefined.

    Precondition:
        None.

    Parameters:
        x - Horizontal position of the pixel.
        y - Vertical position of the pixel.

    Returns:
        The color of the specified pixel.

    Example:
        <code>

            void FillAreaWithBlue(
                            int left,
                            int top,
                            int right,
                            int bottom,
                            GFX_COLOR maskColor)
            {
                int   x, y;

                // assume BLUE is color blue
                GFX_ColorSet(BLUE);

                for(y = top; y < bottom + 1; y++)
                    for(x = left; x < right + 1; x++)
                    {
                        // change only the pixel when pixel color is
                        // maskColor
                        if (GFX_PixelGet(x, y) == maskColor)
                            GFX_PixelPut(x, y);
                    }
            }

        </code>

*/
// *****************************************************************************
GFX_COLOR GFX_PixelGet(uint16_t x, uint16_t y);

// *****************************************************************************
/*
    <GROUP driver_rendering_functions>

    Function:
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

    Precondition:
        None.

    Parameters:
        x - Horizontal starting position of the array of pixels.
        y - Vertical starting position of the array of pixels.
        pPixel - pointer to the array of pixel data.
        numPixels - length of pixel array to render.

    Returns:
         Returns the number of pixels rendered.

    Example:
        <code>

            void RedBarDraw(
                            int left,
                            int top,
                            int right,
                            int bottom)
            {
                int         x, y, length;
                GFX_COLOR   array;

                // assume RED is a red color

                length = (right - left + 1);

                // prepare a line of RED pixels
                for(x = 0; x < length; x++)
                {
                    array[x] = RED;
                }

                for(y = top; y < bottom + 1; y++)
                {
                    GFX_PixelArrayPut(left, y, array, length);
                }
            }

        </code>

*/
// *****************************************************************************
uint16_t GFX_PixelArrayPut(
                                uint16_t x,
                                uint16_t y,
                                GFX_COLOR *pPixel,
                                uint16_t numPixels);

// *****************************************************************************
/*
    <GROUP driver_rendering_functions>

    Function:
        uint16_t GFX_PixelArrayGet(
                                uint16_t x,
                                uint16_t y,
                                GFX_COLOR *pPixel,
                                uint16_t numPixels)

    Summary:
        Reads an array of pixels from the frame buffer.

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

    Precondition:
        None.

    Parameters:
        x - Horizontal starting position of the array of pixels.
        y - Vertical starting position of the array of pixels.
        pPixel - pointer to the retrieved array of pixel data.
        numPixels - length of pixels to be retrieved.

    Returns:
         Returns the actual number of pixels retrieved.

    Example:
        None.

*/
// *****************************************************************************
uint16_t GFX_PixelArrayGet(
                                uint16_t x,
                                uint16_t y,
                                GFX_COLOR *pPixel,
                                uint16_t numPixels);

// *****************************************************************************
/*
    <GROUP driver_rendering_functions>

    Function:
        GFX_STATUS_BITS GFX_RenderStatusGet()

    Summary:
        This function returns the driver's status on rendering.

    Description:
        This function returns the driver's rendering status. Valid values
        returned are GFX_STATUS_BUSY_BIT or GFX_STATUS_READY_BIT
        (see GFX_STATUS_BITS).

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        Returns GFX_STATUS_BITS.

    Example:
        None.

*/
// *****************************************************************************
GFX_STATUS_BIT GFX_RenderStatusGet(void);

// *****************************************************************************
/*
    <GROUP driver_rendering_functions>

    Function:
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

    Precondition:
        None.

    Parameters:
        left - horizontal start position of the image
        top - vertical start position of the image
        image - pointer to the image resource
        pPartialImageData - pointer to the partial image parameters

    Returns:
        The status of the image draw.
        - Returns 0 when device is busy and the image is not
          yet completely drawn. Function must be called again to complete
          the rendering.
        - Returns 1 when the image is completely drawn.

    Example:
        None.

*/
// *****************************************************************************
uint16_t DRV_GFX_ImageDraw(
                                int16_t left,
                                int16_t top,
                                GFX_RESOURCE_HDR *image,
                                GFX_PARTIAL_IMAGE_PARAM *pPartialImageData);

// *****************************************************************************
/*
    <GROUP driver_hardware_functions>

    Function:
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

    Precondition:
        None.

    Parameters:
        level - 0-100 where 0 is the darkest mode and 100 is the brightest mode.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void DRV_GFX_DisplayBrightness(uint16_t level);

#ifndef GFX_CONFIG_DOUBLE_BUFFERING_DISABLE

// *****************************************************************************
/*  Function:
    GFX_FEATURE_STATUS DRV_GFX_DoubleBufferSynchronizeRequest(void)

    Summary:
        This function requests synchronization of the contents
        of the draw and frame buffer on the display driver layer.

    Description:
        This function requests synchronization of the contents of the
        draw and frame buffer on the display driver layer. The contents
        will be synchronized on the next vertical blanking period.

    Precondition:
        Double buffering feature must be enabled.

    Parameters:
        None.

    Returns:
        Status of the double buffer synchronization request.
        GFX_STATUS_SUCCESS - the synchronization request was successfull.
        GFX_STATUS_FAILURE - the synchronization request was not successfull.

    Example:
        None.

*/
// *****************************************************************************
GFX_STATUS DRV_GFX_DoubleBufferSynchronizeRequest(void);

// *****************************************************************************
/*  Function:
    uint32_t DRV_GFX_DrawBufferAddressGet(void)

    Summary:
        This function returns the address location of the draw buffer.

    Description:
        This function returns the address location of the draw buffer.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        The address location of the current draw buffer.

    Example:
        None.

*/
// *****************************************************************************
uint32_t DRV_GFX_DrawBufferAddressGet(void);

// *****************************************************************************
/*  Function:
    uint32_t DRV_GFX_FrameBufferAddressGet(void)

    Summary:
        This function returns the address location of the frame buffer.

    Description:
        This function returns the address location of the frame buffer.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        The address location of the current frame buffer.

    Example:
        None.

*/
// *****************************************************************************
uint32_t DRV_GFX_FrameBufferAddressGet(void);

#endif  

#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE

// *****************************************************************************
/*
    <GROUP driver_rendering_functions>

    Function:
    void DRV_GFX_SetupDrawUpdate(
                                uint16_t startx,
                                uint16_t starty,
                                uint16_t endx,
                                uint16_t endy)

    Summary:
        This function sets up the partial update for bi-stable displays.

    Description:
        This function sets up the partial update for bi-stable displays.

    Precondition:
        None.

    Parameters:
        startx - the horizontal starting position of the partital area
                 that is being updated.
        starty - the vertical starting position of the partital area
                 that is being updated.
        endx - the horizontal ending position of the partital area
               that is being updated.
        endy - the vertical ending position of the partital area
               that is being updated.
 
    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void DRV_GFX_SetupDrawUpdate(
                                uint16_t startx,
                                uint16_t starty,
                                uint16_t endx,
                                uint16_t endy);

// *****************************************************************************
/*
    <GROUP driver_rendering_functions>

    Function:
        void DRV_GFX_CompleteDrawUpdate(
                                uint16_t startx,
                                uint16_t starty,
                                uint16_t endx,
                                uint16_t endy)

    Summary:
        This function completes the partial update for bi-stable displays.

    Description:
        This function completes the partial update for bi-stable displays.

    Precondition:
        None.

    Parameters:
        startx - the horizontal starting position of the partital area
                 that is being updated.
        starty - the vertical starting position of the partital area
                 that is being updated.
        endx - the horizontal ending position of the partital area
               that is being updated.
        endy - the vertical ending position of the partital area
               that is being updated.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void DRV_GFX_CompleteDrawUpdate(
                                uint16_t startx,
                                uint16_t starty,
                                uint16_t endx,
                                uint16_t endy);
#endif

#endif
