/*******************************************************************************
 Module for Microchip Graphics Library - Graphic Primitive Layer

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_primitive.h

  Summary:
    Primitive Layer of the Microchip Graphics Library.

  Description:
    This module implements the primitive rendering routines of the Microchip
    Graphics Library. The routines are independent of the Display Driver Layer
    and should be compatible with any Display Driver that is compliant with
    the requirements of the Display Driver Layer of the Graphics Library.
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

#ifndef _GFX_PRIMITIVE_H
// DOM-IGNORE-BEGIN
    #define _GFX_PRIMITIVE_H
// DOM-IGNORE-END

#include <stdint.h>
#include "system_config.h" 
#include "gfx/gfx_types_macros.h"
#include "gfx/gfx_primitive_local.h"
#ifndef GFX_CONFIG_PALETTE_DISABLE
#include "gfx/gfx_palette.h"
#endif

// DOM-IGNORE-BEGIN
// *****************************************************************************
/*  Overview: Graphics Library helper function for trigonometric sine
              cosine angles are defined in a sine/cosine table.
              This type enumeration specifies if the value retrieved
              from that look up table is for a sine(angle) or
              cosine(angle) value.
*/
// *****************************************************************************
typedef enum
{
    GFX_TRIG_SINE_TYPE    = 0,
    GFX_TRIG_COSINE_TYPE  = 1
} GFX_TRIG_FUNCTION_TYPE;

// constants used for circle/arc computation
#define SIN45   	46341   // sin(45) * 2^16)
#define ONEP25  	81920   // 1.25 * 2^16
// DOM-IGNORE-END

// *****************************************************************************
/*  
    <GROUP init_functions>
    
    Function:
        void GFX_Initialize(void)

    Summary:
        Initialize the Graphics Library.

    Description:
        This function initialize the Graphics Library primitive layer and
        Graphics Object Layer if it is enabled. The following default settings
        are set when this function is called.
        1. font - Set to NULL. GFX_FontSet() must be called prior to any text
                 rendering.
        2. line type - Set to GFX_LINE_TYPE_THIN_SOLID (see GFX_LINE_STYLE).
        3. fill type - Set to GFX_FILL_TYPE_COLOR (see GFX_FILL_STYLE).
        4. text anti-alias type - Set to GFX_FONT_ANTIALIAS_OPAQUE
                 (see  GFX_FONT_ANTIALIAS_TYPE). This only
                 affects fonts with anti-aliasing enabled.
        5. Set transparent color feature in image draw functions
           to be disabled.
        6. Set alpha blending value to 100 (or no alpha blending) if
           alphe blending feature is enabled.
        7. Set background information to no background.

        This function does not clear the screen and does not assign
        any color to the currently set color. Application should
        set the color and clear the screen.


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
void GFX_Initialize(void);

// DOM-IGNORE-BEGIN
// *****************************************************************************
/*  
    <GROUP init_functions>

    Function:
        void GFX_Primitive_Initialize(void)

    Summary:
        Initialize the Primitive Layer of the Graphics Library.

    Description:
        This function initialize the Primitive Layer of the Graphics
        Library. The following default settings are set when this function
        is called.
        1. font - Set to NULL. GFX_FontSet() must be called prior to any text
                 rendering.
        2. line type - Set to GFX_LINE_TYPE_THIN_SOLID (see GFX_LINE_STYLE).
        3. fill type - Set to GFX_FILL_TYPE_COLOR (see GFX_FILL_STYLE).
        4. text anti-alias type - Set to GFX_FONT_ANTIALIAS_OPAQUE
                 (see  GFX_FONT_ANTIALIAS_TYPE). This only
                 affects fonts with anti-aliasing enabled.
        5. Set transparent color feature in image draw functions
           to be disabled.
        6. Set alpha blending value to 100 (or no alpha blending) if
           alphe blending feature is enabled.
        7. Set background information to no background.

        This function does not clear the screen and does not assign
        any color to the currently set color. Application should
        set the color and clear the screen.

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
extern inline void GFX_Primitive_Initialize(void);
// DOM-IGNORE-END

// *****************************************************************************
/*  
    <GROUP init_functions>

    Function:
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

    Precondition:
        Color must be set by GFX_ColorSet().
 
    Precondition:
        None.

    Parameters:
        None.

    Returns:
        The status of the screen clearing.
        GFX_STATUS_SUCCESS - screen was cleared.
        GFX_STATUS_FAILURE - screen is not yet cleared

    Example:
        <code>
            GFX_ColorSet(BLACK);
            GFX_ScreenClear();
        </code>

*/
// *****************************************************************************
GFX_STATUS GFX_ScreenClear(void);

// *****************************************************************************
/*  
    <GROUP line_functions>

    Function:
        int16_t GFX_LinePositionXGet()

    Summary:
        This function returns the current x position of the line cursor.

    Description:
        This function returns the current x position of the line cursor.
        Line cursor is used as a starting point of the line rendered
        by the GFX_LineToDraw() and GFX_LineToRelativeDraw() functions.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        The current line cursor x position.

    Example:
        <code>

            // implementation of the GFX_LineToRelativeDraw()
            GFX_STATUS GFX_LineToRelativeDraw(
                                            int16_t dX,
                                            int16_t dY)
            {
                return (GFX_LineDraw(   GFX_LinePositionXGet(),
                                        GFX_LinePositionYGet(),
                                        GFX_LinePositionXGet() + dX,
                                        GFX_LinePositionYGet() + dY));
            }
        </code>

*/
// *****************************************************************************
int16_t GFX_LinePositionXGet(void);

// *****************************************************************************
/*  
    <GROUP line_functions>

    Function:
        int16_t GFX_LinePositionYGet()

    Summary:
        This function returns the current y position of the line cursor.

    Description:
        This function returns the current y position of the line cursor.
        Line cursor is used as a starting point of the line rendered
        by the GFX_LineToDraw() and GFX_LineToRelativeDraw() functions.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        The current line cursor y position.

    Example:
        See GFX_LinePositionXGet().

*/
// *****************************************************************************
int16_t GFX_LinePositionYGet(void);

// *****************************************************************************
/*  
    <GROUP line_functions>

    Function:
        GFX_STATUS GFX_LinePositionSet(
                    uint16_t x,
                    uint16_t y)

    Summary:
        This function sets the line cursor to a new position.
 
    Description:
        This function sets the line cursor to a new (x,y) position.
        Line cursor is used as a starting point of the line rendered
        by the GFX_LineToDraw() and GFX_LineToRelativeDraw() functions.

        If x and/or y does not lie on the frame buffer, then the behavior
        of GFX_LineToDraw() and GFX_LineToRelativeDraw() functions are 
        undefined. 

    Precondition:
        None.

    Parameters:
        x - new x coordinate position of the line cursor.
        y - new y coordinate position of the line cursor.

    Returns:
        Status of the line position set.
        GFX_STATUS_SUCCESS - line position set done.
        GFX_STATUS_FAILURE - line position set not done.

    Example:
        See GFX_LinePositionXGet().

*/
// *****************************************************************************
GFX_STATUS GFX_LinePositionSet(uint16_t x, uint16_t y);

// *****************************************************************************
/*  
    <GROUP line_functions>

    Function:
        GFX_STATUS GFX_LinePositionRelativeSet(
                    int16_t dX,
                    int16_t dY)

    Summary:
        This function sets the line cursor to a new position relative to
        the current position.

    Description:
        This function sets the line cursor to a new (x,y) position
        relative to the current cursor position. The new position is
        calculated by (x+dX, y + dY).

        Line cursor is used as a starting point of the line rendered
        by the GFX_LineToDraw() and GFX_LineToRelativeDraw() functions.
        Note that the parameters dX and dY are signed integers.
        This allows the new line cursor position to be placed to any
        direction from the current line cursor position.

        If (x+dX) and/or (y+dY) results in a position that is not on the
        frame buffer, then the behavior of GFX_LineToDraw() and
        GFX_LineToRelativeDraw() functions are undefined. If color is not set,
        before this function is called, the output is undefined.

    Precondition:
        None.

    Parameters:
        dX - the offset for the x position that will define the new
             x-coordinate position of the line cursor.
        dY - the offset for the y position that will define the new
             y-coordinate position of the line cursor.

    Returns:
        Status of the relative line position set.
        GFX_STATUS_SUCCESS - relative line position set done.
        GFX_STATUS_FAILURE - relative line position set not done.

    Example:
        See GFX_LineToDraw().

*/
// *****************************************************************************
GFX_STATUS GFX_LinePositionRelativeSet(int16_t x, int16_t y);

// *****************************************************************************
/*  
    <GROUP color_functions>

    Function:
        void GFX_ColorSet(GFX_COLOR color)
    
    Summary:
        This function sets the color to be used in rendering primitive
        shapes and text.

    Description:
        This function sets the color to be used to render primitive shapes
        and text. Any primitive shape that is set to any of the following
        line style and fill style will be rendering using the set color.
        
        GFX_LINE_STYLE:
        - GFX_LINE_STYLE_THIN_SOLID
        - GFX_LINE_STYLE_THIN_DOTTED
        - GFX_LINE_STYLE_THIN_DASHED
        - GFX_LINE_STYLE_THICK_SOLID
        - GFX_LINE_STYLE_THICK_DOTTED
        - GFX_LINE_STYLE_THICK_DASHED
        
        GFX_FILL_STYLE:
        - GFX_FILL_STYLE_COLOR
        
        Rendering text using the following text rendering functions will
        also use the set color:
        - GFX_TextCharDraw()
        - GFX_TextStringDraw()
        - GFX_TextStringBoxDraw() 
        
    Precondition:
        None.

    Parameters:
        color - the rendering color set to render primitive shapes.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void GFX_ColorSet(GFX_COLOR newColor);

// *****************************************************************************
/*  
    <GROUP color_functions>

    Function:
        void GFX_ColorGet(GFX_COLOR color)

    Summary:
        This function returns the color currently set to render primitive
        shapes and text.

    Description:
        This function returns the color currently set to render primitive
        shapes and text (See GFX_ColorSet() for more information).

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        The currently set color.

    Example:
        None.

*/
// *****************************************************************************
GFX_COLOR GFX_ColorGet();

// *****************************************************************************
/*  
    <GROUP color_functions>

    Function:
        GFX_STATUS GFX_TransparentColorEnable(GFX_COLOR color)

    Summary:
        This function sets the transparent color used in GFX_ImageDraw()
        functions and enables the transparent color feature.

    Description:
        This function sets the transparent color used in GFX_ImageDraw()
        functions and enables the transparent color feature.

        When GFX_ImageDraw() or GFX_ImagePartialDraw() is called, any
        pixels in the image that matches the color value will not be
        rendered to the frame buffer.

        The transparent color feature can only be enabled when the
        color depth used is 24 or 16 bpp.

        <img name="transparencycolorexample.jpg" />

    Precondition:
        Transparent color feature must be enabled at build time.
        This is enabled by default and can be disabled by defining
        the macro GFX_CONFIG_TRANSPARENT_COLOR_DISABLE in the system.

    Parameters:
        color - the color value selected as the transparent color.

    Returns:
        The status of the transparent color set action.

    Example:
        <code>
            // assume ScreenBackground and RibbonIcon are valid
            // image resources
            // assume BLACK is a valid GFX_COLOR value

            GFX_TransparentColorEnable(BLACK);
            GFX_ImageDraw(0,0, (void*)&ScreenBackground);
            GFX_ImageDraw(0,0, (void*)&RibbonIcon);
        </code>

*/
// *****************************************************************************
GFX_STATUS GFX_TransparentColorEnable(GFX_COLOR color);

// *****************************************************************************
/*  
    <GROUP color_functions>

    Function:
        GFX_STATUS GFX_TransparentColorDisable(void)

    Summary:
        This function disables the transparent color feature used
        in GFX_ImageDraw() and GFX_ImagePartialDraw() functions.

    Description:
        This function disables the transparent color feature used in
        GFX_ImageDraw() and GFX_ImagePartialDraw() functions.

        The transparent color feature can only be enabled when the
        color depth used is 24 or 16 bpp.

    Precondition:
        Transparent color feature must be enabled at build time.
        This is enabled by default and can be disabled by defining
        the macro GFX_CONFIG_TRANSPARENT_COLOR_DISABLE in the system.

    Parameters:
        None.

    Returns:
        The status of the transparent color disable action.

    Example:
        <code>
            // assume ScreenBackground and RibbonIcon are valid
            // image resources
            // assume BLACK is a valid GFX_COLOR value

            GFX_TransparentColorEnable(BLACK);
            GFX_ImageDraw(0,0, (void*)&ScreenBackground);
            GFX_ImageDraw(50,50, (void*)&RibbonIcon);

            // disable the transparent color feature since the
            // next image to render contains black pixels that
            // we want to render
            GFX_TransparentColorDisable();
            GFX_ImageDraw(50,50, (void*)&OverlayImage);

        </code>

*/
// *****************************************************************************
GFX_STATUS GFX_TransparentColorDisable(void);

// *****************************************************************************
/* 
    <GROUP color_functions>

    Function:
        GFX_COLOR GFX_TransparentColorGet(void)

    Summary:
        This returns the current transparent color set for the
        transparent color feature used in GFX_ImageDraw() and
        GFX_ImagePartialDraw() functions.

    Description:
        This returns the current transparent color set for the
        transparent color feature used in GFX_ImageDraw() and
        GFX_ImagePartialDraw() functions.

        The transparent color feature can only be enabled when the
        color depth used is 24 or 16 bpp.

    Precondition:
        Transparent color feature must be enabled at build time.
        This is enabled by default and can be disabled by defining
        the macro GFX_CONFIG_TRANSPARENT_COLOR_DISABLE in the system.

    Parameters:
        None.

    Returns:
        The current transparent color used.

    Example:
        <code>
            // assume ScreenBackground and RibbonIcon are valid
            // image resources
            // assume BLACK is a valid GFX_COLOR value

            GFX_TransparentColorEnable(BLACK);
            GFX_ImageDraw(0,0, (void*)&ScreenBackground);
            GFX_ImageDraw(50,50, (void*)&RibbonIcon);

            // disable the transparent color feature since the
            // next image to render contains black pixels that
            // we want to render
            if (GFX_TransparentColorGet() == BLACK)
                GFX_TransparentColorDisable();
            GFX_ImageDraw(50,50, (void*)&OverlayImage);

        </code>

*/
// *****************************************************************************
GFX_COLOR GFX_TransparentColorGet(void);

// *****************************************************************************
/*  
    <GROUP color_functions>

    Function:
        GFX_FEATURE_STATUS GFX_TransparentColorStatusGet(void)

    Summary:
        This returns the current state of the transparent color feature
        used in GFX_ImageDraw() and GFX_ImagePartialDraw() functions.

    Description:
        This returns the current transparent color set for the
        transparent color feature used in GFX_ImageDraw() and
        GFX_ImagePartialDraw() functions.

        The transparent color feature can only be enabled when the
        color depth used is 24 or 16 bpp.

    Precondition:
        Transparent color feature must be enabled at build time.
        This is enabled by default and can be disabled by defining
        the macro GFX_CONFIG_TRANSPARENT_COLOR_DISABLE in the system.

    Parameters:
        None.

    Returns:
        The current transparent feature status (see GFX_FEATURE_STATUS
        for details).

    Example:
        <code>
            // assume ScreenBackground and RibbonIcon are valid
            // image resources
            // assume BLACK is a valid GFX_COLOR value

            if (GFX_TransparentColorStatusGet == GFX_FEATURE_DISABLED)
                GFX_TransparentColorEnable(BLACK);
            GFX_ImageDraw(0,0, (void*)&ScreenBackground);
            GFX_ImageDraw(50,50, (void*)&RibbonIcon);

            // disable the transparent color feature since the
            // next image to render contains black pixels that
            // we want to render
            if (GFX_TransparentColorGet == BLACK)
                GFX_TransparentColorDisable();
            GFX_ImageDraw(50,50, (void*)&OverlayImage);

        </code>

*/
// *****************************************************************************
GFX_FEATURE_STATUS GFX_TransparentColorStatusGet(void);

// DOM-IGNORE-BEGIN
// *****************************************************************************
/*  
    <GROUP text_functions>

    Function:
        void GFX_TextAreaLeftSet(uint16_t left)

    Summary:
        This sets the minimum horizontal limit for text area when rendering
        text. This is an internal function and should not be called by the
        application.

    Description:
        This function sets the minimum horizontal limit for text rendering.
        Text rendering will not occur on this value but rather allow
        rendering to left + 1.

    Precondition:
        None.

    Parameters:
        left - minimum horizontal limit.

    Returns:
        None.

    Example:
        <code>
            // Initialize the text region to be full screen
            GFX_TextAreaLeftSet(0);
            GFX_TextAreaTopSet(0);
            GFX_TextAreaRightSet(GFX_MaxXGet());
            GFX_TextAreaBottomSet(GFX_MaxYGet());
        </code>

*/
// *****************************************************************************
void GFX_TextAreaLeftSet(uint16_t left);
// DOM-IGNORE-END

// DOM-IGNORE-BEGIN
// *****************************************************************************
/*  
    <GROUP text_functions>

    Function:
        void GFX_TextAreaTopSet(uint16_t top)

    Summary:
        This sets the minimum vertical limit for text area when rendering
        text. This is an internal function and should not be called by the
        application.

    Description:
        This function sets the minimum vertical limit for text rendering.
        Text rendering will not occur on this value but rather allow
        rendering to top + 1.

    Precondition:
        None.

    Parameters:
        top - minimum vertical limit.

    Returns:
        None.

    Example:
        <code>
            // Initialize the text region to be full screen
            GFX_TextAreaLeftSet(0);
            GFX_TextAreaTopSet(0);
            GFX_TextAreaRightSet(GFX_MaxXGet());
            GFX_TextAreaBottomSet(GFX_MaxYGet());
        </code>

*/
// *****************************************************************************
void GFX_TextAreaTopSet(uint16_t top);
// DOM-IGNORE-END

// DOM-IGNORE-BEGIN
// *****************************************************************************
/*  
    <GROUP text_functions>

    Function:
        void GFX_TextAreaRightSet(uint16_t right)

    Summary:
        This sets the maximum horizontal limit for text area when rendering
        text. This is an internal function and should not be called by the
        application.

    Description:
        This function sets the maximum horizontal limit for text rendering.
        Text rendering will not occur on this value but rather allow
        rendering to right - 1.

    Precondition:
        None.

    Parameters:
        right - maximum horizontal limit.

    Returns:
        None.

    Example:
        <code>
            // Initialize the text region to be full screen
            GFX_TextAreaLeftSet(0);
            GFX_TextAreaTopSet(0);
            GFX_TextAreaRightSet(GFX_MaxXGet());
            GFX_TextAreaBottomSet(GFX_MaxYGet());
        </code>

*/
// *****************************************************************************
void GFX_TextAreaRightSet(uint16_t right);
// DOM-IGNORE-END

// DOM-IGNORE-BEGIN
// *****************************************************************************
/*  
    <GROUP text_functions>

    Function:
        void GFX_TextAreaBottomSet(uint16_t bottom)

    Summary:
        This sets the maximum vertical limit for text area when rendering
        text. This is an internal function and should not be called by the
        application.

    Description:
        This function sets the maximum vertical limit for text rendering.
        Text rendering will not occur on this value but rather allow
        rendering to bottom - 1.

    Precondition:
        None.

    Parameters:
        top - maximum vertical limit.

    Returns:
        None.

    Example:
        <code>
            // Initialize the text region to be full screen
            GFX_TextAreaLeftSet(0);
            GFX_TextAreaTopSet(0);
            GFX_TextAreaRightSet(GFX_MaxXGet());
            GFX_TextAreaBottomSet(GFX_MaxYGet());
        </code>

*/
// *****************************************************************************
void GFX_TextAreaBottomSet(uint16_t bottom);
// DOM-IGNORE-END

// DOM-IGNORE-BEGIN
// *****************************************************************************
/*  
    <GROUP text_functions>

    Function:
        uint16_t GFX_TextAreaLeftGet(uint16_t left)

    Summary:
        This returns the currently set minimum horizontal limit for text
        area when rendering text. This is an internal function and should
        not be called by the application.

    Description:
        This function returns the currently set minimum horizontal limit for
        text rendering. Text rendering will not occur on this value but
        rather allow rendering to left + 1.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        Minimum horizontal limit.

    Example:
        None.

*/
// *****************************************************************************
uint16_t GFX_TextAreaLeftGet(void);
// DOM-IGNORE-END

// DOM-IGNORE-BEGIN
// *****************************************************************************
/*  
    <GROUP text_functions>

    Function:
        uint16_t GFX_TextAreaTopGet(uint16_t top)

    Summary:
        This returns the currently set minimum vertical limit for text
        area when rendering text. This is an internal function and should
        not be called by the application.

    Description:
        This function returns the currently set minimum vertical limit for
        text rendering. Text rendering will not occur on this value but
        rather allow rendering to top + 1.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        Minimum vertical limit.

    Example:
        None.


*/
// *****************************************************************************
//<GROUP text_functions>
uint16_t GFX_TextAreaTopGet(void);
// DOM-IGNORE-END

// DOM-IGNORE-BEGIN
// *****************************************************************************
/*  Function:
        uint16_t GFX_TextAreaRightGet(uint16_t bottom)

    Summary:
        This returns the currently set maximum horizontal limit for text
        area when rendering text. This is an internal function and should
        not be called by the application.

    Description:
        This function returns the currently set maximum horizontal limit for
        text rendering. Text rendering will not occur on this value but
        rather allow rendering to right - 1.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        Maximum horizontal limit.

    Example:
        None.

*/
// *****************************************************************************
uint16_t GFX_TextAreaRightGet(void);
// DOM-IGNORE-END

// DOM-IGNORE-BEGIN
// *****************************************************************************
/*  
    <GROUP text_functions>

    Function:
        uint16_t GFX_TextAreaBottomGet(uint16_t bottom)

    Summary:
        This returns the currently set maximum vertical limit for text
        area when rendering text. This is an internal function and should
        not be called by the application.

    Description:
        This function returns the currently set maximum vertical limit for
        text rendering. Text rendering will not occur on this value but
        rather allow rendering to bottom - 1.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        Maximum vertical limit.

    Example:
        None.

*/
// *****************************************************************************
uint16_t GFX_TextAreaBottomGet(void);
// DOM-IGNORE-END

// DOM-IGNORE-BEGIN
// *****************************************************************************
/*
    <GROUP text_functions>

    Function:
        GFX_STATUS GFX_TextCharRender(
                                GFX_XCHAR ch,
                                GFX_FONT_OUTCHAR *pParam)

    Summary:
        This internal function performs the actual rendering of the
        character glyph.

    Description:
        This is the function called to render the character
        glyph if using the primitive layer text rendering routine
        GFX_TextCharDraw(). This is an internal function and should
        not be called by the application. This function is created
        for versatility of implementing hardware accelerated text
        rendering.

    Precondition:
        None.

    Parameters:
        ch - character ID that of the character that will
             be rendered.
        pParam - pointer to the character information
                 structure GFX_FONT_OUTCHAR.

    Returns:
        Status of the rendering.
        GFX_STATUS_SUCCESS - rendering is done.
        GFX_STATUS_FAILURE - rendering is not done.

    Example:
        None.

*/
// *****************************************************************************
GFX_STATUS GFX_TextCharRender(
                                GFX_XCHAR ch,
                                GFX_FONT_OUTCHAR *pParam);
// DOM-IGNORE-END

// *****************************************************************************
/*  
    <GROUP text_functions>

    Function:
        int16_t GFX_TextCursorPositionXGet()

    Summary:
        This function returns the current x position of the text cursor.

    Description:
        This function returns the current x position of the text cursor.
        Text cursor is used as a starting point of the line rendered
        by the GFX_TextCharDraw() function.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        The current text cursor x position.

    Example:
        None.

*/
// *****************************************************************************
int16_t GFX_TextCursorPositionXGet();

// *****************************************************************************
/*  
    <GROUP text_functions>

    Function:
        int16_t GFX_TextCursorPositionYGet()

    Summary:
        This function returns the current y position of the text cursor.

    Description:
        This function returns the current y position of the text cursor.
        Text cursor is used as a starting point of the line rendered
        by the GFX_TextCharDraw() function.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        The current text cursor y position.

    Example:
        None.

*/
// *****************************************************************************
int16_t GFX_TextCursorPositionYGet();

// *****************************************************************************
/*  
    <GROUP text_functions>
    
    Function:
        GFX_STATUS GFX_TextCursorPositionSet(
                    int16_t x,
                    int16_t y)

    Summary:
        This function sets the text cursor to a new position.

    Description:
        This function sets the text cursor to a new (x,y) position.
        Text cursor is used as a starting point of the character rendered
        by the GFX_TextCharDraw() function.

        If x and/or y does not lie on the frame buffer, then the behavior
        of GFX_TextCharDraw() function is undefined.

    Precondition:
        None.

    Parameters:
        x - new x coordinate position of the text cursor.
        y - new y coordinate position of the text cursor.

    Returns:
        Status of the text cursor position set.
        GFX_STATUS_SUCCESS - text cursor position set done.
        GFX_STATUS_FAILURE - text cursor position set is not done.

    Example:
        <code>
            GFX_TextCursorPositionSet(10,10);
            // write letter 'A' starting from 10, 10 position
            GFX_TextCharDraw('A');
        </code>

*/
// *****************************************************************************
GFX_STATUS GFX_TextCursorPositionSet(int16_t x, int16_t y);

// *****************************************************************************
/*
    <GROUP text_functions>

    Function:
        GFX_STATUS GFX_TextCharDraw(
                                GFX_XCHAR ch)

    Summary:
        This function renders the given character using the
        currently set color using the currently set font.

    Description:
        This function renders the given character using the currently set font,
        and color to the location defined by the text cursor position. The
        color is set by GFX_ColorSet() while the font is set by GFX_FontSet().
        The text cursor position is set by GFX_TextCursorPositionSet()

        The rendering of the character becomes undefined when any one of the
        following is true:
        - Text cursor position is set to locations outside the frame buffer.
        - Color is not set, before this function is called.
        - Font is not set, before this function is called.

    Precondition:
        Color must be set by GFX_ColorSet().
        Font must be set by GFX_FontSet().
        Text cursor position must be set by GFX_TextCursorPositionSet().

    Parameters:
        ch - character ID that of the character that will be rendered.

    Returns:
        The status of the character rendering.
        GFX_STATUS_SUCCESS - the character was rendered
        GFX_STATUS_FAILURE - the character was not rendered, function must
                             be called again to render the character.
        GFX_STATUS_ERROR   - the character ID passed is not a valid
                               or points to the character glyph that does
                               does not exist on the font table.

    Example:
        <code>
            // assume textString is a string of characters
            // assume WHITE is a valid GFX_COLOR data
            // assume pMyFont is a valid initialized font resource pointer
            static uint16_t counter = 0;
            GFX_XCHAR   ch;
            GFX_STATUS  status;

            GFX_ColorSet(WHITE);
            GFX_FontSet(pMyFont);

            // render characters until null character
            while((GFX_XCHAR)(ch = *(textString + counter)) != 0)
            {
                status = GFX_TextCharDraw(ch);
                if (status != GFX_STATUS_SUCCESS)
                    return (GFX_STATUS_FAILURE);
                counter++;
            }
        </code>

*/
// *****************************************************************************
GFX_STATUS    GFX_TextCharDraw(GFX_XCHAR ch);

// *****************************************************************************
/*  
    <GROUP text_functions>

    Function:
        GFX_STATUS GFX_TextStringDraw(
                                uint16_t x,
                                uint16_t y,
                                GFX_XCHAR *pString,
                                uint16_t length)

    Summary:
        This function renders the given string of character using the
        currently set color using the currently set font.

    Description:
        This function renders the given string of character using the currently
        set font, and color to the location defined by the given x,y position.
        The color is set by GFX_ColorSet() while the font is set by
        GFX_FontSet().

        The text rendered by this function is always left aligned.
        When a newline character is encountered, the next character
        after the newline character will be rendered left aligned
        on the next line. The next line position is determined by the
        height of the font used.

        When the length parameter is set to 0, the string will be rendered
        until the null character is reached.
        When the length parameter is greater than 0, the string will be
        rendered until one of the following cases occurs:
        - null character is reached before the total rendered characters
          is less than the value of length
        - total rendered characters is equal to the value of length before
          the null character is reached.

        The rendering of the character becomes undefined when any one of the
        following is true:
        - x,y position is set to locations outside the frame buffer.
        - Color is not set, before this function is called.
        - Font is not set, before this function is called.

    Precondition:
        Color must be set by GFX_ColorSet().
        Font must be set by GFX_FontSet().

    Parameters:
        x - Horizontal starting position of the string.
        y - Vertical position position of the string.
        pString - Pointer to the location of the string that will be rendered.
        length - Total number of characters to be rendered. When set to 0,
                 the function will terminate until the null character is
                 detected.

    Returns:
        The status of the string rendering.
        GFX_STATUS_SUCCESS - the string was rendered
        GFX_STATUS_FAILURE - the string was not rendered, or is not yet
                               finished. The function must be called again
                               to render the remaining characters.

    Example:
        <code>
            // assume WHITE is a valid GFX_COLOR data
            // assume pMyFont is a valid initialized font resource pointer

            static uint16_t counter = 0;
            GFX_XCHAR   charArray[] = "Test String";
            GFX_XCHAR   pChar = NULL;
            GFX_STATUS  status;

            GFX_ColorSet(WHITE);
            GFX_FontSet(pMyFont);

            // render the whole string
            GFX_TextStringDraw(10, 10, charArray, 0);

            // render ONLY the "Test" portion of the string
            GFX_TextStringDraw(10, 30, charArray, 4);

            // render ONLY the "String" portion of the string
            pChar = charArray;
            pChar += 5;
            GFX_TextStringDraw(10, 50, pChar, 0);

            // doing this also renders ONLY the "String" portion of the string
            GFX_TextStringDraw(10, 70, pChar, 6);
            
        </code>

*/
// *****************************************************************************
GFX_STATUS GFX_TextStringDraw(
                                uint16_t x,
                                uint16_t y,
                                GFX_XCHAR *pString,
                                uint16_t length);

// *****************************************************************************
/*  
    <GROUP text_functions>
    
    Function:
        GFX_STATUS FX_TextStringBoxDraw(
                                uint16_t x,
                                uint16_t y,
                                uint16_t width,
                                uint16_t height,
                                GFX_XCHAR *pString,
                                uint16_t length,
                                GFX_ALIGNMENT align)

    Summary:
        This function renders the given string using the currently set color
        and font into a rectangular area.

    Description:
        This function renders the given string using the currently
        color and font into the rectangular area defined by the given
        parameters x,y, width and height. The x,y parameters defines the
        left, top pixel position of the rectangular area. The width and
        height defines the size of the rectangular area. The rectangular
        area will define the area where the text will be rendered. Meaning
        all pixels EXCLUSIVE of the defined rectangle will be the rendering
        area for the text. If the given string exceeds the rectangular area,
        any pixels falling outside and along the defined rectangle (INCLUSIVE
        of the rectangle) will not be drawn.

        The color is set by GFX_ColorSet() while the font is set by
        GFX_FontSet().

        Multiple lines of strings can be rendered. To define string composed
        of multiple lines, end each line with a new line character
        (character ID 0x0A). Each line will be rendered according to the
        text alignment (See GFX_TEXT_ALIGNMENT).
        
        <img name="stringboxrenderingexample.jpg" /> 

        When the length parameter is set to 0, the string will be rendered
        until the null character is reached.
        When the length parameter is greater than 0, the string will be
        rendered until one of the following cases occurs:
        - null character is reached before the total rendered characters
          is less than the value of length
        - total rendered characters is equal to the value of length before
          the null character is reached.

        The rendering of the character becomes undefined when any one of the
        following is true:
        - x, y, width and height defines an area partially or fully outside
          outside the frame buffer.
        - Color is not set, before this function is called.
        - Font is not set, before this function is called.

    Precondition:
        Color must be set by GFX_ColorSet().
        Font must be set by GFX_FontSet().

    Parameters:
        x - Horizontal starting position of the rectangular area.
        y - Vertical position position of the rectangular area.
        width - Defines the width of the rectangular area.
        height - Defines the height of the rectangular area.
        pString - Pointer to the location of the string that will be rendered.
                  String can have multiple lines where each line is terminated
                  by a new line character.
        length - Total number of characters to be rendered. When set to 0,
                 the function will terminate until the null character is
                 detected.
        align - The alignment of the rendered text.

    Returns:
        The status of the string rendering.
        GFX_STATUS_SUCCESS - the string was rendered
        GFX_STATUS_FAILURE - the string was not rendered, or is not yet
                               finished. The function must be called again
                               to render the remaining characters or lines.

    Example:
        <code>
            // assume APP_DEMO_FONT to be a valid font resource
            GFX_XCHAR StringMsg[] = "Hello World";

            GFX_ColorSet(BLACK);
            GFX_ScreenClear();

            GFX_ColorSet(WHITE);
            GFX_FontSet((GFX_RESOURCE_HDR*)&APP_DEMO_FONT);

            // displayed message will be centered on the screen
            while(GFX_TextStringBoxDraw( 0, 0,
                                         GFX_MaxXGet(), GFX_MaxYGet(),
                                         StringMsg,
                                         0, GFX_ALIGN_CENTER) !=
                    GFX_STATUS_SUCCESS);

        </code>

*/
// *****************************************************************************
GFX_STATUS GFX_TextStringBoxDraw(
                                uint16_t x,
                                uint16_t y,
                                uint16_t width,
                                uint16_t height,
                                GFX_XCHAR *pString,
                                uint16_t length,
                                GFX_ALIGNMENT align);

// *****************************************************************************
/*  
    <GROUP text_functions>

    Function:
        uint16_t GFX_TextStringHeightGet(
                                GFX_RESOURCE_HDR *pFont);

    Summary:
        This function returns the height of the given font.

    Description:
        This function returns the height of the given font in pixels.
        The given font must be present in the system.

        This function return value is undefined if the given pointer does
        not point to a valid font.

    Precondition:
        None.

    Parameters:
        pFont - Pointer to the specified font.

    Returns:
        The height of the specified font in pixels.

    Example:
        <code>
            // assume pMyFont is a pointer initialized to a valid font
            uint16_t height, x, y;

            // center the text on the screen
 
            height = GFX_TextStringHeightGet(pMyFont);
            width  = GFX_TextStringWidthGet (   (GFX_XCHAR *)"Hello World", 
                                                 pMyFont
                                            );
            y = (GFX_MaxYGet() - height) >> 1;
            x = (GFX_MaxXGet() - width) >> 1;
            GFX_TextStringDraw(x, y, (GFX_XCHAR *)"Hello World");

            GFX_TextStringDraw(x, y, (GFX_XCHAR *)"Hello World");

        </code>

*/
// *****************************************************************************
extern inline uint16_t __attribute__ ((always_inline)) GFX_TextStringHeightGet(
                                GFX_RESOURCE_HDR *pFont)
{
    return (pFont->resource.font.header.height);
}

// *****************************************************************************
/*  
    <GROUP text_functions>

    Function:
        uint16_t GFX_TextStringWidthGet(
                                GFX_XCHAR* pString,
                                GFX_RESOURCE_HDR *pFont);

    Summary:
        This function returns the width of the given string using the
        given font.

    Description:
        This function returns the width of the given string using the
        given font in pixels. The given font must be present in the system.

        The pixel length is measured from the first printable character
        until the last printable character. This means that if the string
        is composed of multiple lines, the length returned is only valid
        for the first line of characters.

        This function return value is undefined if the given pointer does
        not point to a valid font or one or more characters in the given
        string does not exist on the given font.

    Precondition:
        None.

    Parameters:
        pString - Pointer to the specified string.
        pFont - Pointer to the specified font.

    Returns:
        The width of the specified string using the specified font in pixels.

    Example:
        <code>
            // assume pMyFont is a pointer initialized to a valid font
            uint16_t height, x, y;

            // center the text on the screen

            height = GFX_TextStringHeightGet(pMyFont);
            width  = GFX_TextStringWidthGet (   (GFX_XCHAR *)"Hello World",
                                                 pMyFont
                                            );
            y = (GFX_MaxYGet() - height) >> 1;
            x = (GFX_MaxXGet() - width) >> 1;
            GFX_TextStringDraw(x, y, (GFX_XCHAR *)"Hello World");

        </code>

*/
// *****************************************************************************
uint16_t GFX_TextStringWidthGet(
                                GFX_XCHAR *textString,
                                GFX_RESOURCE_HDR *pFont);

// *****************************************************************************
/*  
    <GROUP text_functions>

    Function:
        GFX_STATUS GFX_FontSet(GFX_RESOURCE_HDR *pFont)

    Summary:
        This function sets the current font used when rendering
        strings and characters.

    Description:
        This function sets the current font used to render strings and
        characters.

    Precondition:
        None.

    Parameters:
        pFont - pointer to the font to be used.

    Returns:
        The status of the set font action.

    Example:
        <code>
            extern const GFX_RESOURCE_HDR Font25;
            uint16_t width, height;
            GFX_XCHAR myString[] = "Microchip Technology Inc.";
 
            GFX_FontSet((GFX_RESOURCE_HDR*) &Font25);
            GFX_ColorSet(GFX_X11_GREEN);
            width = GFX_TextStringWidthGet(myString, (GFX_RESOURCE_HDR*) &Font25);
            height = GFX_TextStringHeightGet((GFX_RESOURCE_HDR*) &Font25);

            GFX_TextStringDraw( (GFX_MaxXGet() - width) >> 1,
                                (GFX_MaxYGet() - height) >> 1,
                                myString,
                                0);
        </code>

*/
// *****************************************************************************
GFX_STATUS GFX_FontSet(GFX_RESOURCE_HDR *pFont);

// *****************************************************************************
/*  
    <GROUP text_functions>

    Function:
        GFX_RESOURCE_HDR GFX_FontGet(void)

    Summary:
        This function returns the current font used when rendering
        strings and characters.

    Description:
        This function returns the current font used to render strings and
        characters.

    Precondition:
        GFX_FontSet() must be called prior to any call to this function.
        If this function is called first, the returned value is undefined.

    Parameters:
        None.

    Returns:
        The pointer to the current font used in rendering
        strings and characters.

    Example:
        None.

*/
// *****************************************************************************
GFX_RESOURCE_HDR* GFX_FontGet(void);

// *****************************************************************************
/*  
    <GROUP text_functions>

    Function:
        GFX_STATUS GFX_FontAntiAliasSet(GFX_FONT_ANTIALIAS_TYPE type)

    Summary:
        This function sets the font anti-aliasing mode.

    Description:
        This function sets the font anti-aliasing mode used when
        rendering anti-aliased strings and characters.

    Precondition:
        None.

    Parameters:
        type - anti-aliasing mode selected. See GFX_FONT_ANTIALIAS_TYPE
               for details.

    Returns:
        The status of the set anti-aliasing mode action.

    Example:
        <code>
            extern const GFX_RESOURCE_HDR antiAliasedFont25;
            uint16_t width, height;
            GFX_XCHAR myString[] = "Microchip Technology Inc.";
 
            GFX_FontAntiAliasSet(GFX_FONT_ANTIALIAS_TRANSLUCENT);
            GFX_FontSet((GFX_RESOURCE_HDR*) &antiAliasedFont25);
            GFX_ColorSet(GFX_X11_GREEN);
            width = GFX_TextStringWidthGet( myString, 
                                            (GFX_RESOURCE_HDR*) &Font25);
            height = GFX_TextStringHeightGet((GFX_RESOURCE_HDR*) &antiAliasedFont25);

            GFX_TextStringDraw( (GFX_MaxXGet() - width) >> 1,
                                (GFX_MaxYGet() - height) >> 1,
                                myString,
                                0);
        </code>

*/
// *****************************************************************************
GFX_STATUS GFX_FontAntiAliasSet(GFX_FONT_ANTIALIAS_TYPE type);

// *****************************************************************************
/*  
    <GROUP text_functions>

    Function:
        GFX_FONT_ANTIALIAS_TYPE GFX_FontAntiAliasGet(void)

    Summary:
        This function returns the current font font anti-aliasing mode.

    Description:
        This function returns the font anti-aliasing mode used when
        rendering anti-aliased strings and characters. Default setting
        at initialization is GFX_FONT_ANTIALIAS_OPAQUE.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        The font anti-aliasing mode used.

    Example:
        <code>
            extern const GFX_RESOURCE_HDR antiAliasedFont25;
            uint16_t width, height;
            GFX_XCHAR myString[] = "Microchip Technology Inc.";

            if (GFX_FontAntiAliasGet() != GFX_FONT_ANTIALIAS_OPAQUE)
                GFX_FontAntiAliasSet(GFX_FONT_ANTIALIAS_OPAQUE);

            GFX_FontSet((GFX_RESOURCE_HDR*) &antiAliasedFont25);
            GFX_ColorSet(GFX_X11_GREEN);
            width = GFX_TextStringWidthGet(myString, (GFX_RESOURCE_HDR*) &Font25);
            height = GFX_TextStringHeightGet((GFX_RESOURCE_HDR*) &antiAliasedFont25);

            GFX_TextStringDraw( (GFX_MaxXGet() - width) >> 1,
                                (GFX_MaxYGet() - height) >> 1,
                                myString,
                                0);

        </code>

*/
// *****************************************************************************
GFX_FONT_ANTIALIAS_TYPE GFX_FontAntiAliasGet(void);

// *****************************************************************************
/*  
    <GROUP style_functions>

    Function:
        GFX_STATUS GFX_LineStyleSet(
                    GFX_LINE_STYLE style)

    Summary:
        Set the line style to be used when rendering lines.

    Description:
        This function sets the line style to be used (see GFX_LINE_STYLE)
        when rendering lines. All calls to the following functions
        - GFX_LineDraw()
        - GFX_LineToDraw()
        - GFX_LineToRelativeDraw()
        - GFX_CircleDraw()
        - GFX_RectangleDraw()
        - GFX_PolygonDraw()
        will use the line style set by this function. In addition,
        all unfilled shapes that does specify the line style to be used
        will use the line style specified by this function.

    Precondition:
        None.

    Parameters:
        style - The specified line style to be used.

    Returns:
        Status of the line style set.
        GFX_STATUS_SUCCESS - line setting done.
        GFX_STATUS_FAILURE - line setting not done.

    Example:
        <code>
            GFX_ColorSet(BRIGHTRED);
            GFX_LineStyleSet(GFX_LINE_STYLE_THIN_SOLID);
            GFX_LineDraw(10, 10,100,  10);
        </code>

*/
// *****************************************************************************
GFX_STATUS GFX_LineStyleSet(GFX_LINE_STYLE style);

// *****************************************************************************
/*  
    <GROUP style_functions>

    Function:
        GFX_LINE_STYLE GFX_LineStyleGet(void)

    Summary:
        Return the line style used when rendering lines.

    Description:
        This function returns the line style used (see GFX_LINE_STYLE)
        when rendering lines.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        The current line style used (see GFX_LINE_STYLE).

    Example:
        None.

*/
// *****************************************************************************
GFX_LINE_STYLE GFX_LineStyleGet(void);

// *****************************************************************************
/*  
    <GROUP line_functions>

    Function:
        GFX_STATUS GFX_LineDraw(
                    uint16_t x1,
                    uint16_t y1,
                    uint16_t x2,
                    uint16_t y2)

    Summary:
        This function renders a line from x1,y1 to x2,y2 using the currently
        set line style (see GFX_LineStyleSet()).

    Description:
        This function renders a line from x1,y1 to x2,y2 using the currently
        set line style set by GFX_LineStyleSet(). The color used is the 
        color set by the last call to GFX_ColorSet(). 
 
        If x1,y1 and/or x2,y2 is not on the frame buffer, then the behavior 
        is undefined. If color is not set, before this function 
        is called, the output is undefined.

    Precondition:
        Color must be set by GFX_ColorSet().
        Line style must be set by GFX_LineStyleSet().

    Parameters:
        x1 - x coordinate of the line start point.
        y1 - y coordinate of the line start point.
        x2 - x coordinate of the line end point.
        y2 - y coordinate of the line end point.

    Returns:
        Status of the line rendering.
        GFX_STATUS_SUCCESS - line rendering done.
        GFX_STATUS_FAILURE - line rendering is not done.

    Example:
        <code>
            GFX_ColorSet(BRIGHTRED);
            GFX_LineStyleSet(GFX_LINE_STYLE_THIN_DOTTED);
            GFX_LineDraw(10, 10,100,  10);
        </code>

*/
// *****************************************************************************
GFX_STATUS GFX_LineDraw(
                                uint16_t x1,
                                uint16_t y1,
                                uint16_t x2,
                                uint16_t y2);

// *****************************************************************************
/*  
    <GROUP line_functions>

    Function:
        GFX_STATUS GFX_LineToRelativeDraw(
                                int16_t dX,
                                int16_t dY)

    Summary:
        This function renders a line from current line cursor position (x,y)
        to (x+dX,y+dY) using the currently set line style (see
        GFX_LineStyleSet()).

    Description:
        This function renders a line from current line cursor position (x,y)
        to (x+dX,y+dY) using the currently set line style set by
        GFX_LineStyleSet(). The color used is the color set by the last
        call to GFX_ColorSet(). Note that the parameters dX and dY are signed
        integers. This allows the line to be drawn from the line cursor
        to any direction.

        If (x+dX) and/or (y+dY) results in a position that is not on the
        frame buffer, then the behavior is undefined. If color is not set,
        before this function is called, the output is undefined.

    Precondition:
        Color must be set by GFX_ColorSet().
        Line style must be set by GFX_LineStyleSet().

    Parameters:
        dX - the offset for the x starting position that will define the
             x-coordinate of the end of the line.
        dY - the offset for the y starting position that will define the
             y-coordinate of the end of the line.

    Returns:
        Status of the line rendering.
        GFX_STATUS_SUCCESS - line rendering done.
        GFX_STATUS_FAILURE - line rendering is not done.

    Example:
        <code>
            GFX_ColorSet(BRIGHTRED);
            GFX_LineStyleSet(GFX_LINE_STYLE_THIN_DOTTED);

            GFX_LinePositionSet(100, 100);
            GFX_LineToRelativeDraw(  0, -90);
            GFX_LineToRelativeDraw(-90,   0);
            GFX_LineToRelativeDraw( 90,  90);
            GFX_LineToRelativeDraw(-90,   0);
            GFX_LineToRelativeDraw( 90, -90);
            GFX_LinePositionSet(10, 10);
            GFX_LineToRelativeDraw(  0,  90);
         </code>

*/
// *****************************************************************************
GFX_STATUS GFX_LineToRelativeDraw(
                                int16_t dX,
                                int16_t dY);

// *****************************************************************************
/*  
    <GROUP line_functions>

    Function:
        GFX_STATUS GFX_LineToDraw(
                                int16_t x2,
                                int16_t y2)

    Summary:
        This function renders a line from current line cursor position (x,y)
        to (x2,y2) using the currently set line style (see GFX_LineStyleSet()).

    Description:
        This function renders a line from current line cursor position (x,y)
        to (x2,y2) using the currently set line style set by
        GFX_LineStyleSet(). The color used is the color set by the last
        call to GFX_ColorSet().

        If x2 and/or y2 does not lie on the frame buffer, then the behavior
        is undefined. If color is not set, before this function is called,
        the output is undefined.

    Precondition:
        Color must be set by GFX_ColorSet().
        Line style must be set by GFX_LineStyleSet().

    Parameters:
        x2 - x coordinate of the line end point.
        y2 - y coordinate of the line end point.
        
    Returns:
        Status of the line rendering.
        GFX_STATUS_SUCCESS - line rendering done.
        GFX_STATUS_FAILURE - line rendering is not done.

    Example:
        <code>
            GFX_ColorSet(BRIGHTRED);
            GFX_LineStyleSet(GFX_LINE_STYLE_THICK_DOTTED);

            GFX_LinePositionSet(100, 100);
            GFX_LineToDraw(100,  10);
            GFX_LinePositionRelativeSet(-90,  0);
            GFX_LineToDraw(100,  10);
            GFX_LinePositionRelativeSet(  0, 90);
            GFX_LineToDraw( 10, 100);
            GFX_LinePositionRelativeSet( 90,-90);
            GFX_LineToDraw( 10, 100);
            GFX_LinePositionRelativeSet(  0,-90);
            GFX_LineToDraw(100, 100);
            GFX_LinePositionRelativeSet( -90, 0);
            GFX_LineToDraw( 10,  10);

        </code>

*/
// *****************************************************************************
GFX_STATUS GFX_LineToDraw(  
                                int16_t x2,
                                int16_t y2);

// DOM-IGNORE-BEGIN
// *****************************************************************************
/*  
    <GROUP line_functions>

    Function:
        GFX_STATUS GFX_SolidLineDraw(
                                uint16_t x1,
                                uint16_t y1,
                                uint16_t x2,
                                uint16_t y2)

    Summary:
        This function renders the thin and thick solid line.
        This is an internal function and should not be called by the application.

    Description:
        This function renders the thin and thick solid lines
        (see GFX_LINE_STYLE).

    Precondition:
        None.

    Parameters:
        x1 - x coordinate of the line start point.
        y1 - y coordinate of the line start point.
        x2 - x coordinate of the line end point.
        y2 - y coordinate of the line end point.

    Returns:
        Status of the line rendering.
        GFX_STATUS_SUCCESS - line rendering done.
        GFX_STATUS_FAILURE - line rendering is not done.

    Example:
        None.

*/
// *****************************************************************************
GFX_STATUS GFX_SolidLineDraw(
                                uint16_t x1,
                                uint16_t y1,
                                uint16_t x2,
                                uint16_t y2);
// DOM-IGNORE-END

// DOM-IGNORE-BEGIN
// *****************************************************************************
/*  
    <GROUP line_functions>

    Function:
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

    Precondition:
        None.

    Parameters:
        x1 - x coordinate of the line start point.
        y1 - y coordinate of the line start point.
        x2 - x coordinate of the line end point.
        y2 - y coordinate of the line end point.

    Returns:
        Status of the line rendering.
        GFX_STATUS_SUCCESS - line rendering done.
        GFX_STATUS_FAILURE - line rendering is not done.

    Example:
        None.

*/
// *****************************************************************************
GFX_STATUS GFX_StyledLineDraw(
                                uint16_t x1,
                                uint16_t y1,
                                uint16_t x2,
                                uint16_t y2);
// DOM-IGNORE-END

// *****************************************************************************
/*  
    <GROUP polygon_functions>

    Function:
        GFX_STATUS GFX_PolygonDraw(
                                uint16_t sides,
                                uint16_t *pPoints)

    Summary:
        This function renders a polygon using the currently set line
        style and color.

    Description:
        This function renders a polygon using the currently set line
        style (see GFX_LineStyleSet()) and color (see GFX_ColorSet()). The
        shape of the polygon is determined by the polygon points (an
        ordered array of x,y pairs) where the pair count is equal to the
        parameter sides.

        If any of the x,y pairs do not lie on the frame buffer, then the
        behavior is undefined. If color is not set, before this function
        is called, the output is undefined.

    Precondition:
        Color must be set by GFX_ColorSet().
        Line style must be set by GFX_LineStyleSet().

    Parameters:
        sides - the number of sides of the polygon.
        pPoints - Pointer to the array of polygon points. The array defines
                  the x,y points of the polygon. The sequence should be
                  x0, y0, x1, y1, x2, y2, ... xn, yn where n is
                  the # of polygon sides.

    Returns:
        Status of the polygon rendering.
        GFX_STATUS_SUCCESS - polygon rendering done.
        GFX_STATUS_FAILURE - polygon rendering is not done.

    Example:
        <code>
            uint16_t OpenShapeXYPoints[6] = {10, 10, 20, 10, 20, 20};
            uint16_t ClosedShapeXYPoints[8] = {10, 10, 20, 10, 20, 20, 10, 10};

            GFX_ColorSet(WHITE);                        // set color
            SetLineType(GFX_LINE_STYLE_THIN_DOTTED);    // set line style
            GFX_PolygonDraw(3, OpenShapeXYPoints);	// draw an open shape
            GFX_PolygonDraw(4, ClosedShapeXYPoints);    // draw a  closed shape
        </code>

*/
// *****************************************************************************
GFX_STATUS GFX_PolygonDraw(
                                uint16_t numPoints,
                                uint16_t *polyPoints);

// DOM-IGNORE-BEGIN
// *****************************************************************************
/*  
    <GROUP polygon_functions>

    Function:
        GFX_STATUS GFX_BevelDraw(
                                uint16_t x1,
                                uint16_t y1,
                                uint16_t x2,
                                uint16_t y2,
                                uint16_t rad)

    Summary:
        This function renders an unfilled bevel shape using the currently
        set line style (see GFX_LineStyleSet()).

    Description:
        This is an internal function. Application should not call this
        function directly.

        This function renders an unfilled bevel shape using the given
        x1, y1, x2, y2 and rad parameters to define the shape dimension.
        rad defines the rounded corner shape. The bevel is rendered using the
        currently set line style by GFX_LineStyleSet(). The color used is the
        color set by the last call to GFX_ColorSet().

        Left most pixel location is defined by x1 - rad.
        Top most pixel location is defined by y1 - rad.
        Right most pixel location is defined by x2 + rad.
        Bottom most pixel location is defined by y2 + rad.
        When rad = 0, there are no rounded corners. In this case
        (x1,y1) will define the left, top corner and (x2,y2) will
        define the right, bottom corner of the shape.

        When x1 = x2 and y1 = y2, with radius > 0, a circular object is drawn.
        When x1 < x2 and y1 < y2 and rad (radius) = 0, a rectangular
        object is drawn.

        <img name="Bevel.jpg" />

        The rendering of the bevel is undefined when any one of the following
        is true:
        - Any of the following pixel locations x1-rad ,y1 - rad, x2+rad,
          y2 + rad falls outside the frame buffer.
        - Color is not set, before this function is called.
        - x2 < x1
        - y2 < y1

    Precondition:
        Color must be set by GFX_ColorSet().
        Line style must be set by GFX_LineStyleSet().

    Parameters:
        x1 - Along with rad (x1 - rad), defines the left most
             pixel of the shape.
        y1 - Along with rad (y1 - rad), defines the top most
             pixel of the shape.
        x2 - Along with rad (x2 + rad), defines the right most
             pixel of the shape.
        y2 - Along with rad (y2 + rad), defines the bottom most
             pixel of the shape.
        rad - defines the rounded corners. When this is set
              to zero, there will be no rounded corners.

    Returns:
        Status of the bevel rendering.
        GFX_STATUS_SUCCESS - bevel rendering done.
        GFX_STATUS_FAILURE - brvel rendering is not done.

    Example:
        None.

*/
// *****************************************************************************
GFX_STATUS GFX_BevelDraw(   uint16_t x1, uint16_t y1,
                            uint16_t x2, uint16_t y2,
                            uint16_t rad);
// DOM-IGNORE-END

// *****************************************************************************
/*  
    <GROUP polygon_functions>

    Function:
        GFX_STATUS GFX_RectangleDraw(
                            uint16_t left,
                            uint16_t top,
                            uint16_t right,
                            uint16_t bottom)

    Summary:
        This function renders a rectangular shape using the
        currently set line style and color.

    Description:
        This function renders a rectangular shape using the given left, top,
        right and bottom parameters to define the shape dimension. The shape
        is rendered using the currently set line style by GFX_LineStyleSet().
        The color used is the color set by the last call to GFX_ColorSet().

        The rendering of this shape becomes undefined when any one of the
        following is true:
        - Any of the following pixel locations left,top, right,bottom falls
          outside the frame buffer.
        - Color is not set, before this function is called.
        - right < left
        - bottom < top

    Precondition:
        Color must be set by GFX_ColorSet().
        Line style must be set by GFX_LineStyleSet().

    Parameters:
        left - defines the left most pixel of the shape.
        top - defines the top most pixel of the shape.
        right - defines the right most pixel of the shape.
        bottom - defines the bottom most pixel of the shape.

    Returns:
        Status of the rectangle rendering.
        GFX_STATUS_SUCCESS - rectangle rendering done.
        GFX_STATUS_FAILURE - rectangle rendering is not done.

    Example:
        <code>
            // draw a bright red rectangle
            GFX_LineStyleSet(GFX_LINE_STYLE_THIN_SOLID);
            GFX_ColorSet(BRIGHTRED);
            GFX_RectangleDraw(30, 30, 88, 88);

            // draw a bright blue round rectangle
            GFX_LineStyleSet(GFX_LINE_STYLE_THIN_DASHED);
            GFX_ColorSet(BRIGHTBLUE);
            GFX_RectangleRoundDraw(130, 30, 188, 88, 15);
        </code>

*/
// *****************************************************************************
GFX_STATUS GFX_RectangleDraw(
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom);

// *****************************************************************************
/*  
    <GROUP polygon_functions>

    Function:
        GFX_STATUS GFX_RectangleRoundDraw(
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom,
                                uint16_t radius)

    Summary:
        This function renders a rounded corner rectangular shape using the
        currently set line style and color.

    Description:
        This function renders a rectangular shape with rounded corner using
        the given left, top, right, bottom and radius parameters to define the
        shape dimension. radius defines the rounded corner shape. The shape
        is rendered using the currently set line style by GFX_LineStyleSet().
        The color used is the color set by the last call to GFX_ColorSet().

        Left most pixel location is defined by left - radius.
        Top most pixel location is defined by top - radius.
        Right most pixel location is defined by right + radius.
        Bottom most pixel location is defined by bottom + radius.
        When radius = 0, there are no rounded corners. In this case
        (left,top) will define the left, top corner and (right,bottom) will
        define the right, bottom corner of the shape.

        When left = right and top = bottom, with radius > 0, a circular
        object is drawn. When left < right and top < bottom and radius = 0,
        a rectangular object is drawn.

        <img name="roundrectangleexample.jpg" />

        The rendering of this shape becomes undefined when any one of the
        following is true:
        - Any of the following pixel locations left-rad , top-rad, right+rad,
          bottom+rad falls outside the frame buffer.
        - Color is not set, before this function is called.
        - right < left
        - bottom < top

    Precondition:
        Color must be set by GFX_ColorSet().
        Line style must be set by GFX_LineStyleSet().

    Parameters:
        left - Along with rad (left - rad), defines the left most
             pixel of the shape.
        top - Along with rad (top - rad), defines the top most
             pixel of the shape.
        right - Along with rad (right + rad), defines the right most
             pixel of the shape.
        bottom - Along with rad (bottom + rad), defines the bottom most
             pixel of the shape.
        radius - defines the rounded corners. When this is set
              to zero, there will be no rounded corners.

    Returns:
        Status of the rectangle rendering.
        GFX_STATUS_SUCCESS - rectangle rendering done.
        GFX_STATUS_FAILURE - rectangle rendering is not done.

    Example:
        <code>
            // draw a bright red rectangle
            GFX_LineStyleSet(GFX_LINE_STYLE_THIN_SOLID);
            GFX_ColorSet(BRIGHTRED);
            GFX_RectangleDraw(30, 30, 88, 88);

            // draw a bright blue round rectangle
            GFX_LineStyleSet(GFX_LINE_STYLE_THIN_DASHED);
            GFX_ColorSet(BRIGHTBLUE);
            GFX_RectangleRoundDraw(130, 30, 188, 88, 15);
        </code>

*/
// *****************************************************************************
GFX_STATUS GFX_RectangleRoundDraw(
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom,
                                uint16_t radius);

// *****************************************************************************
/*  
    <GROUP polygon_functions>

    Function:
        GFX_STATUS GFX_CircleDraw(
                                uint16_t x,
                                uint16_t y,
                                uint16_t radius)

    Summary:
        This function renders a circular shape using the
        currently set line style and color.

    Description:
        This function renders a circular shape using the center (x,y) and
        radius. The shape is rendered using the currently set line style by
        GFX_LineStyleSet(). The color used is the color set by the last
        call to GFX_ColorSet().

        When x,y falls outside the buffer, the behavior is undefined.
        When color is not set before this function is called, the bahavior is
        undefined. When any of the following x+radius, x-radius, y+radius
        and y-radius falls outside the buffer, the behavior is undefined.

    Precondition:
        Color must be set by GFX_ColorSet().
        Line style must be set by GFX_LineStyleSet().

    Parameters:
        x - defines the x-coordinate position of the center of the circle.
        y - defines the y-coordinate position of the center of the circle.
        radius - defines the radius of the circle.

    Returns:
        Status of the circle rendering.
        GFX_STATUS_SUCCESS - circle rendering done.
        GFX_STATUS_FAILURE - circle rendering is not done.

    Example:
        <code>
            // draw a circle using bright red solid line
            GFX_LineStyleSet(GFX_LINE_STYLE_THIN_SOLID);
            GFX_ColorSet(BRIGHTRED);
            GFX_CircleDraw(50, 50, 40);
        </code>

*/
// *****************************************************************************
GFX_STATUS GFX_CircleDraw(
                                uint16_t x,
                                uint16_t y,
                                uint16_t radius);

// *****************************************************************************
/*
    <GROUP polygon_fill_functions>
    
    Function:
        GFX_STATUS GFX_RectangleFillDraw(
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom)

    Summary:
        This function renders a filled rectangular shape using the
        currently set fill style and colors.

    Description:
        This function renders a filled rectangular shape with the currently
        set fill style (see GFX_FILL_STYLE) with the given left, top, right,
        and bottom parameters to define the shape dimension. The shape is
        rendered depending on the fill style. If a flat color is used, color
        must be set (see GFX_ColorSet()) before calling this function. If
        gradient color is used, gradient start and end color must be set
        (see GFX_GradientColorSet()) before calling this function.
        After the fill style and colors are set, multiple calls to this
        function can be performed.

        The rendering of this shape becomes undefined when any one of the
        following is true:
        - Any of the following pixel locations left,top or right,bottom
          falls outside the frame buffer.
        - Fill style is not set (GFX_FillStyleSet(), before this function
          is called.
        - Colors are not set before this function is called.
        - When right < left
        - When bottom < top
        - When pixel locations defined by left, top and/or right, bottom
          are not on the frame buffer.

    Precondition:
        Fill style must be set by GFX_FillStyleSet().
        Color must be set by GFX_ColorSet().

    Parameters:
        left - defines the left most pixel of the shape.
        top - defines the top most pixel of the shape.
        right - defines the right most pixel of the shape.
        bottom - defines the bottom most pixel of the shape.

    Returns:
        Status of the rectangle rendering.
        GFX_STATUS_SUCCESS - rectangle fill rendering done.
        GFX_STATUS_FAILURE - rectangle fill rendering is not done.

    Example:
        <code>
            // assume BLUE and RED are macros that define GFX_COLOR types
            GFX_STATUS status;

            // render a rectangle with gradient colors from BLUE to RED
            GFX_FillStyleSet(GFX_FILL_STYLE_GRADIENT_LEFT);
            GFX_GradientColorSet(BLUE, RED);
            status = GFX_RectangleFillDraw(10, 110, 100, 200);

            // render an alpha blended rounded rectangle with
            // the current contents of the frame buffer
            GFX_FillStyleSet(GFX_FILL_STYLE_ALPHA_COLOR);
            GFX_AlphaBlendingValueSet(50);
            GFX_BackgroundTypeSet(GFX_BACKGROUND_DISPLAY_BUFFER);
            GFX_ColorSet(RED);
            status = GFX_RectangleRoundFillDraw(10, 110, 100, 200, 10);

            // render an alpha blended rectangle with an image
            GFX_FillStyleSet(GFX_FILL_STYLE_ALPHA_COLOR);
            GFX_AlphaBlendingValueSet(50);
            GFX_BackgroundTypeSet(GFX_BACKGROUND_IMAGE);
            GFX_ColorSet(RED);
            status = GFX_RectangleFillDraw(10, 110, 100, 200);

            // render a plain RED rounded rectangle 
            GFX_FillStyleSet(GFX_FILL_STYLE_COLOR);
            GFX_ColorSet(RED);
            status = GFX_RectangleFillDraw(10, 110, 100, 200);

        </code>

*/
// *****************************************************************************
GFX_STATUS GFX_RectangleFillDraw(
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom);

// *****************************************************************************
/*  
    <GROUP polygon_fill_functions>

    Function:
        GFX_STATUS GFX_RectangleRoundFillDraw(
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom,
                                uint16_t radius)

    Summary:
        This function renders a filled rectangular shape with round corners
        using the currently set fill style and colors.

    Description:
        This function renders a filled rounded rectangular shape with the
        currently set fill style (see GFX_FILL_STYLE) with the given left,
        top, right, bottom and radius parameters to define the shape dimension.
        The shape is rendered depending on the fill style. If a flat color
        is used, color must be set (see GFX_ColorSet()) before calling
        this function. If gradient color is used, gradient start and end
        color must be set (see GFX_GradientColorSet()) before calling this
        function. After the fill style and colors are set, multiple calls
        to this function can be performed.

        When left = right and top = bottom, with radius > 0, a circular
        object is drawn. When left < right and top < bottom and radius = 0,
        a rectangular object is drawn.

        <img name="roundrectanglefillexample.jpg" />

        The rendering of this shape becomes undefined when any one of the
        following is true:
        - Any of the following pixel locations left,top or right,bottom
          falls outside the frame buffer.
        - Fill style is not set (GFX_FillStyleSet(), before this function
          is called.
        - Colors are not set before this function is called.
        - When right < left
        - When bottom < top
        - When pixel locations defined by left, top and/or right, bottom
          are not on the frame buffer.

    Precondition:
        Fill style must be set by GFX_FillStyleSet().
        Color must be set by GFX_ColorSet().

    Parameters:
        left - defines the left most pixel of the shape.
        top - defines the top most pixel of the shape.
        right - defines the right most pixel of the shape.
        bottom - defines the bottom most pixel of the shape.
        radius - defines the radius of the rounded corner. A zero value
                 will result in a rectangular shape drawn.

    Returns:
        Status of the rectangle rendering.
        GFX_STATUS_SUCCESS - rectangle fill rendering done.
        GFX_STATUS_FAILURE - rectangle fill rendering is not done.

    Example:
        <code>
            // assume BLUE and RED are macros that define GFX_COLOR types
            GFX_STATUS status;

            GFX_FillStyleSet(GFX_FILL_STYLE_GRADIENT_DOUBLE_VER);
            GFX_GradientColorSet(BLUE, RED);
            status = GFX_RectangleFillDraw(50, 110, 150, 200, 20);
            if (status == GFX_STATUS_SUCCESS)
                // Filled rounded rectangle shape was drawn.
            else
                // Filled rounded rectangle shape is not drawn or not yet
                // finished rendering. To finish the rendering call the
                // function again with the same parameters.
        </code>

*/
// *****************************************************************************
GFX_STATUS GFX_RectangleRoundFillDraw(
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom,
                                uint16_t radius);

// *****************************************************************************
/*  
    <GROUP polygon_fill_functions>

    Function:
        GFX_STATUS GFX_CircleFillDraw(
                                uint16_t x,
                                uint16_t y,
                                uint16_t radius)

    Summary:
        This function renders a filled circle shape using the currently
        set fill style and colors.

    Description:
        This function renders a filled circle shape with the currently set
        fill style (see GFX_FILL_STYLE) with the given left, top, right and
        bottom parameters to define the shape dimension.
        The shape is rendered depending on the fill style. If a flat color
        is used, color must be set (see GFX_ColorSet()) before calling
        this function. If gradient color is used, gradient start and end
        color must be set (see GFX_GradientColorSet()) before calling this
        function. After the fill style and colors are set, multiple calls
        to this function can be performed.

        The rendering of this shape becomes undefined when any one of the
        following is true:
        - Any of the following pixel locations left,top or right,bottom
          falls outside the frame buffer.
        - Fill style is not set (GFX_FillStyleSet(), before this function
          is called.
        - Colors are not set before this function is called.
        - When the center defined by x,y is not on the frame buffer.

    Precondition:
        Fill style must be set by GFX_FillStyleSet().
        Color must be set by GFX_ColorSet().

    Parameters:
        x - defines the x-coordinate position of the center of the circle.
        y - defines the y-coordinate position of the center of the circle.
        radius - defines the radius of the circle.


    Returns:
        Status of the circle rendering.
        GFX_STATUS_SUCCESS - circle fill rendering done.
        GFX_STATUS_FAILURE - circle fill rendering is not done.

    Example:
        <code>
            // assume BLUE and RED are macros that define GFX_COLOR types
            GFX_STATUS status;

            GFX_FillStyleSet(GFX_FILL_STYLE_GRADIENT_UP);
            GFX_GradientColorSet(BLUE, RED);
            status = GFX_CircleFillDraw(50, 110, 150, 200, 20);
            if (status == GFX_STATUS_SUCCESS)
                // Filled circle was drawn.
            else
                // Filled circle is not drawn or not yet
                // finished rendering. To finish the rendering call the
                // function again with the same parameters.
        </code>

*/
// *****************************************************************************
GFX_STATUS GFX_CircleFillDraw(
                                uint16_t x,
                                uint16_t y,
                                uint16_t radius);

// *****************************************************************************
/*  
    <GROUP style_functions>

    Function:
        GFX_STATUS GFX_FillStyleSet(
                                GFX_FILL_STYLE style)

    Summary:
        Set the fill style to be used when rendering filled shapes.

    Description:
        This function sets the fill style to be used (see GFX_FILL_STYLE)
        when rendering filled shapes. All calls to the following:
        - GFX_RectangleFillDraw()
        - GFX_RectangleRoundFillDraw()
        - GFX_CircleFillDraw()
        will use the fill style set by this function.

    Precondition:
        None.

    Parameters:
        style - The specified fill style to be used.

    Returns:
        Status of the fill style set.
        GFX_STATUS_SUCCESS - fill style set done.
        GFX_STATUS_FAILURE - fill style set failed.

    Example:
        <code>
            GFX_GradientColorSet(BRIGHTRED, BRIGHTBLUE);

            GFX_FillStyleSet(GFX_FILL_STYLE_GRADIENT_DOWN);
            GFX_RectangleFillDraw(  10, 10,  100,  100);

            GFX_FillStyleSet(GFX_FILL_STYLE_GRADIENT_UP);
            GFX_RectangleFillDraw( 110, 10,  200,  100);

            GFX_FillStyleSet(GFX_FILL_STYLE_GRADIENT_RIGHT);
            GFX_RectangleFillDraw( 210, 10,  300,  100);

            GFX_FillStyleSet(GFX_FILL_STYLE_GRADIENT_LEFT);
            GFX_RectangleFillDraw(  10, 110,  100,  200);
        </code>

*/
// *****************************************************************************
GFX_STATUS GFX_FillStyleSet(GFX_FILL_STYLE style);

// *****************************************************************************
/*  
    <GROUP style_functions>

    Function:
        GFX_FILL_STYLE GFX_FillStyleGet(void)

    Summary:
        Return the fill style used when rendering filled shapes.

    Description:
        This function returns the fill style used (see GFX_FILL_STYLE)
        when rendering filled shapes.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        The current fill style used (see GFX_FILL_STYLE).

    Example:
        None.

*/
// *****************************************************************************
GFX_FILL_STYLE GFX_FillStyleGet(void);

// *****************************************************************************
/*  
    <GROUP style_functions>

    Function:
        GFX_STATUS GFX_GradientColorSet(
                                GFX_COLOR startColor,
                                GFX_COLOR endColor)

    Summary:
        Sets the gradient fill start and end color.

    Description:
        This function sets the gradient fill start and end colors
        (see GFX_FILL_STYLE) when rendering gradient filled shapes.
        All subsequent calls to the following using gradient fills:
        - GFX_RectangleFillDraw()
        - GFX_RectangleRoundFillDraw()
        - GFX_CircleFillDraw()
        will use the start and end colors set by this function.

    Precondition:
        None.

    Parameters:
        startColor - Gradient start color to be used.
        endColor - Gradient end color to be used.

    Returns:
        Status of the color set.
        GFX_STATUS_SUCCESS - gradient color set done.
        GFX_STATUS_FAILURE - gradient color set failed.

    Example:
        See GFX_FillStyleSet().

*/
// *****************************************************************************
GFX_STATUS GFX_GradientColorSet(
                                GFX_COLOR startColor,
                                GFX_COLOR endColor);

// *****************************************************************************
/*  
    <GROUP style_functions>

    Function:
        GFX_COLOR GFX_GradientStartColorGet(void)

    Summary:
        Return the gradient start color when rendering gradient filled shapes.

    Description:
        This function returns the start color used when rendering
        gradient filled shapes.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        The current gradient start color used.

    Example:
        None.

*/
// *****************************************************************************
GFX_COLOR GFX_GradientStartColorGet(void);

// *****************************************************************************
/*  
    <GROUP style_functions>

    Function:
        GFX_COLOR GFX_GradientEndColorGet(void)

    Summary:
        Return the gradient end color when rendering gradient filled shapes.

    Description:
        This function returns the end color used when rendering
        gradient filled shapes.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        The current gradient end color used.

    Example:
        None.

*/
// *****************************************************************************
GFX_COLOR GFX_GradientEndColorGet(void);

// *****************************************************************************
/*  
    <GROUP polygon_fill_functions>

    Function:
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

    Precondition:
        Fill style must be set by GFX_FillStyleSet() when alpha blended fill
        is desired.
        Color must be set by GFX_ColorSet().

    Parameters:
        left - defines the left most pixel of the shape.
        top - defines the top most pixel of the shape.
        right - defines the right most pixel of the shape.
        bottom - defines the bottom most pixel of the shape.

    Returns:
        Status of the bar rendering.
        GFX_STATUS_SUCCESS - bar rendering done.
        GFX_STATUS_FAILURE - bar rendering failed.

    Example:
        <code>
            // assume RED is a macro that define GFX_COLOR types
            GFX_STATUS status;
            // assume BackGroundImage is a valid image already draw
            // on the screen
            GFX_RESOURCE_HDR *pMyBackgroundImage = &BackGroundImage;

            // render a RED bar
            GFX_FillStyleSet(GFX_FILL_STYLE_COLOR);
            GFX_ColorSet(RED);
            status = GFX_BarDraw(10, 110, 100, 200);

            // render an alpha blended bar with
            // the current contents of the frame buffer
            GFX_FillStyleSet(GFX_FILL_STYLE_ALPHA_COLOR);
            GFX_BackgroundTypeSet(GFX_BACKGROUND_DISPLAY_BUFFER);
            GFX_ColorSet(RED);
            status = GFX_BarDraw(10, 110, 100, 200);

            // render an alpha blended bar with a background image
            GFX_FillStyleSet(GFX_FILL_STYLE_ALPHA_COLOR);

            // color value here has no effect since the background set
            // is type GFX_BACKGROUND_IMAGE
            GFX_BackgroundSet(0, 0, pMyBackGroundImage, 0);

            GFX_BackgroundTypeSet(GFX_BACKGROUND_IMAGE);
            GFX_ColorSet(RED);
            status = GFX_BarDraw(10, 110, 100, 200);

        </code>

*/
// *****************************************************************************
GFX_STATUS GFX_BarDraw(         uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom);

#ifndef GFX_CONFIG_ALPHABLEND_DISABLE

// DOM-IGNORE-BEGIN
// *****************************************************************************
/*  
    <GROUP polygon_fill_functions>

    Function:
        GFX_STATUS GFX_BarAlphaDraw(
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom)

    Summary:
        This function renders a alpha blended bar shape using the 
        currently set fill style and color.

    Description:
        This function renders an alpha blended bar shape with the 
        currently set color and the currently set background type 
        (see GFX_BACKGROUND_TYPE).

        The parameters left, top, right bottom will define the 
        shape dimension.

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

    Precondition:
        Color must be set by GFX_ColorSet().
        Alpha blending value must be set by GFX_AlphaBlendingValueSet().
        The background type must be set by GFX_BackgroundTypeSet().
        
    Parameters:
        left - defines the left most pixel of the shape.
        top - defines the top most pixel of the shape.
        right - defines the right most pixel of the shape.
        bottom - defines the bottom most pixel of the shape.

    Returns:
        Status of the bar rendering.
        GFX_STATUS_SUCCESS - bar rendering done.
        GFX_STATUS_FAILURE - bar rendering failed.

    Example:
        <code>
            // assume RED is a macro that define GFX_COLOR types
            GFX_STATUS status;
            // assume BackGroundImage is a valid image already draw
            // on the screen
            GFX_RESOURCE_HDR *pMyBackgroundImage = &BackGroundImage;

            // render an alpha blended bar with
            // the current contents of the frame buffer
            GFX_AlphaBlendingValueSet(50);
            GFX_BackgroundTypeSet(GFX_BACKGROUND_DISPLAY_BUFFER);
            GFX_ColorSet(RED);
            status = GFX_BarAlphaDraw(10, 110, 100, 200);

            // render an alpha blended bar with a background image
            GFX_AlphaBlendingValueSet(75);
            GFX_FillStyleSet(GFX_FILL_STYLE_ALPHA_COLOR);

            // color value here has no effect since the background set
            // is type GFX_BACKGROUND_IMAGE
            GFX_BackgroundSet(0, 0, pMyBackGroundImage, 0);

            GFX_BackgroundTypeSet(GFX_BACKGROUND_IMAGE);
            GFX_ColorSet(RED);
            status = GFX_BarAlphaDraw(10, 110, 100, 200);

        </code>

*/
// *****************************************************************************
uint16_t GFX_BarAlphaDraw(      uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom);
// DOM-IGNORE-END

// *****************************************************************************
/*  
    <GROUP style_functions>

    Function:
        GFX_STATUS GFX_AlphaBlendingValueSet(uint16_t alpha)

    Summary:
        This function sets the alpha value for alpha blending rendering.

    Description:
        This function sets the alpha value for alpha blending rendering.
        Accepted values are dependent on the used alpha blending routines
        at build time. When using the software routines in the Primitive
        Layer, accepted values are 0, 25, 50, 75 and 100.

        If using a specific implementation in the display driver used,
        implementation may support the full range (0-100). When this is
        the case, refer to the driver alpha blending solution for details.

        Function operation will ignore unsupported values of alpha.

    Precondition:
        None.

    Parameters:
        alpha - Defines the alpha blending percentage to be used for
                alpha blending routines. Accepted values are dependent
                on the alpha blending routines used. For Primitive Layer
                implementation accepted values are:
                - 100 : no alpha blending, last color set by GFX_ColorSet()
                        will replace the pixels.
                - 75  : 75% of the last color set by GFX_ColorSet() will be
                        alpha blended to the existing pixel.
                - 50  : 50% of the last color set by GFX_ColorSet() will be
                        alpha blended to the existing pixel.
                - 25  : 25% of the last color set by GFX_ColorSet() will be
                        alpha blended to the existing pixel.
                - 0   : 0% of the last color set by GFX_ColorSet() will be
                        alpha blended to the existing pixel. This means
                        the existing pixel color will not change.

    Returns:
        Status of the alpha value set action. Return GFX_STATUS_ERROR when
        the alpha value set is unsupported.

    Example:
        <code>
 
            // render a bar with no alpha blending
            GFX_AlphaBlendingValueSet(100);
            while(GFX_RectangleFillDraw(10,10,30,30) != GFX_STATUS_SUCCESS);

            // render a bar with 50% alpha blending to a
            // background with RED color
            GFX_BackgroundSet(x, y, pBgImage, RED);
            GFX_BackgroundTypeSet(GFX_BACKGROUND_COLOR);

            GFX_AlphaBlendingValueSet(50);
            while(GFX_RectangleFillDraw(10,40,30,60) != GFX_STATUS_SUCCESS);

        </code>

*/
// *****************************************************************************
GFX_STATUS GFX_AlphaBlendingValueSet(uint16_t alpha);

// *****************************************************************************
/*  
    <GROUP style_functions>

    Function:
        uint16_t GFX_AlphaBlendingValueGet(void)

    Summary:
        This function returns the current alpha value set for
        alpha blending rendering.

    Description:
        This function returns the current alpha value set for
        alpha blending rendering. See GFX_AlphaBlendingValueSet() for
        details of alpha blending values.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        The alpha blending value.

    Example:
        None.
 
*/
// *****************************************************************************
uint16_t GFX_AlphaBlendingValueGet(void);

#endif  // #ifndef GFX_CONFIG_ALPHABLEND_DISABLE

// *****************************************************************************
/*  
    <GROUP image_functions>

    Function:
        GFX_STATUS GFX_ImagePartialDraw(
                                uint16_t destination_x,
                                uint16_t destination_y,
                                uint16_t source_x_offset,
                                uint16_t source_y_offset,
                                uint16_t source_width,
                                uint16_t source_height,
                                GFX_RESOURCE_HDR *pImage)

    Summary:
        This function renders a portion of an image to the frame buffer.

    Description:
        This function renders an image or a portion of an image to the
        frame buffer with the top, left corner of the image located at
        destination_x, destination_y. To render a full image, source_x_offset,
        source_y_offset, width and height are set to all 0. Using the
        actual image's width and height will also work if these are known.
        If they are not known, avoid the extra extra step to get the image's
        width and height by assigning 0 to the parameters. Another way to
        render a full image is to use the GFX_ImageDraw(). The image to be
        rendered is defined by the pointer pImage.
        
        <img name="putimage_partial_example.jpg" />

        The rendering of this shape becomes undefined when any one of the
        following is true:
        - destination_x, destination_y pixel position falls outside
          the frame buffer.
        - source_x_offset, source_y_offset results in a starting position
          beyond the image's dimension.
        - source_width and/or source_height is larger than the actual
          image's width and height.
        - pointer is not properly initialized to a GFX_RESOURCE_HDR object.

    Precondition:
        None.

    Parameters:
        destination_x - Horizontal starting position of the image.
        destination_y - Vertical starting position of the image.
        source_x_offset - Horizontal offset in pixels of the starting position
                          of the partial area of the image to be rendered.
                          Set to 0 along with source_y_offset when rendering
                          the full image.
        source_y_offset - Vertical offset in pixels of the starting position
                          of the partial area of the image to be rendered.
                          Set to 0 along with source_x_offset when rendering
                          the full image.GFX_ImageDraw
        source_width - The width of the partial area of the image to be
                       rendered. Set to 0 along with source_height when
                       rendering the full image.
        source_height - The height of the partial area of the image to be
                       rendered. Set to 0 along with source_width when
                       rendering the full image.
        pImage - Pointer to the image to be rendered.

    Returns:
        Status of the image rendering.
        GFX_STATUS_SUCCESS - image rendering done.
        GFX_STATUS_FAILURE - image rendering failed.

    Example:
        <code>
            // assume the backgroundImage has dimension of 320x240 pixels.
            GFX_RESOURCE_HDR *pBackgroundImage;

            pBackgroundImage = (GFX_RESOURCE_HDR *)&backgroundImage;

            // Render only 1/4 of the image. Render the lower right
            // corner of the image
            GFX_ImagePartialDraw(   10, 10,
                                    320/2,
                                    240/2,
                                    320/2,
                                    240/2,
                                    pBackgroundImage);
        </code>

*/
// *****************************************************************************
GFX_STATUS GFX_ImagePartialDraw(uint16_t destination_x,
                                uint16_t destination_y,
                                uint16_t source_x_offset,
                                uint16_t source_y_offset,
                                uint16_t source_width,
                                uint16_t source_height,
                                GFX_RESOURCE_HDR *pImage);

// *****************************************************************************
/*  
    <GROUP image_functions>

    Function:
        GFX_STATUS GFX_ImageDraw(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage)

    Summary:
        This function renders an image to the frame buffer.

    Description:
        This function renders an image to the frame buffer with the
        left-top corner of the image located at given left, top parameters.

        The rendering of this shape becomes undefined when any one of the
        following is true:
        - left, top pixel position falls outside
          the frame buffer.
        - pointer is not properly initialized to a GFX_RESOURCE_HDR object.

    Precondition:
        None.

    Parameters:
        left - Horizontal starting position of the image.
        top  - Vertical starting position of the image.
        pImage - Pointer to the image to be rendered.

    Returns:
        Status of the image rendering.
        GFX_STATUS_SUCCESS - image rendering done.
        GFX_STATUS_FAILURE - image rendering failed.

    Example:
        <code>
            // assume the backgroundImage has dimension of 320x240 pixels.
            GFX_RESOURCE_HDR *pBackgroundImage;

            pBackgroundImage = (GFX_RESOURCE_HDR *)&backgroundImage;

            // Render the image starting from (10,10) x,y position

            // corner of the image
            GFX_ImageDraw(  10, 10,
                            pBackgroundImage);
        </code>

*/
// *****************************************************************************
GFX_STATUS GFX_ImageDraw(       uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage);

// *****************************************************************************
/*  
    <GROUP image_functions>

    Function:
        GFX_STATUS GFX_ImageWidthGet(
                                GFX_RESOURCE_HDR *pImage)

    Summary:
        This function returns the width of the given image.

    Description:
        This function returns the width of the given image in pixels.
        This function results in an undefined behavior if the pointer to the
        image is invalid.

        This function return value is undefined if the given pointer does
        not point to a valid image resource.

    Precondition:
        None.

    Parameters:
        pImage - Pointer to the image.

    Returns:
        The image width in pixels.

    Example:
        None.

*/
// *****************************************************************************
extern inline int16_t __attribute__ ((always_inline)) GFX_ImageWidthGet(
                                GFX_RESOURCE_HDR *pImage)
{
    return pImage->resource.image.width;
}

// *****************************************************************************
/*  
    <GROUP image_functions>

    Function:
        GFX_STATUS GFX_ImageHeightGet(
                                GFX_RESOURCE_HDR *pImage)

    Summary:
        This function returns the height of the given image.

    Description:
        This function returns the height of the given image in pixels.
        This function results in an undefined behavior if the pointer to the
        image is invalid.

        This function return value is undefined if the given pointer does
        not point to a valid image resource.

    Precondition:
        None.

    Parameters:
        pImage - Pointer to the image.

    Returns:
        The image height in pixels.

    Example:
        None.

*/
// *****************************************************************************
extern inline int16_t __attribute__ ((always_inline)) GFX_ImageHeightGet(
                                GFX_RESOURCE_HDR *pImage)
{
    return pImage->resource.image.height;
}

// *****************************************************************************
/*  
    <GROUP image_functions>

    Function:
        GFX_STATUS GFX_ImageHeaderGet(
                                GFX_RESOURCE_HDR *pImage,
                                GFX_MCHP_BITMAP_HEADER *pBitmapHdr)

    Summary:
        This function fills the given bitmap header with the image's
        header information.

    Description:
        This function fills the given bitmap header with the image's
        header information. This function results in an undefined behavior
        if the pointer to the image is invalid.

    Precondition:
        None.

    Parameters:
        pImage - Pointer to the image.
        pBitmap - Pointer to the destination of the header information.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
extern inline void __attribute__ ((always_inline)) GFX_ImageHeaderGet(
                                GFX_RESOURCE_HDR *pImage,
                                GFX_MCHP_BITMAP_HEADER *pBitmapHdr)
{
    pBitmapHdr->colorDepth = pImage->resource.image.colorDepth;
    pBitmapHdr->height     = pImage->resource.image.height;
    pBitmapHdr->width      = pImage->resource.image.width;
}

// *****************************************************************************
/*  
    <GROUP background_functions>

    Function:
        void GFX_BackgroundSet(
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_COLOR color);

    Summary:
        This function sets the background information.

    Description:
        This function sets the background information. Note that the width
        and height of the background is only needed when the
        background is an image. When the background is a
        color (image pointer is NULL), the width and height is
        not needed here since the purpose of the background
        information is to record only the color used.

    Precondition:
        None.

    Parameters:
        left   - Horizontal starting position of the background.
        top    - Vertical starting position of the background.
        pImage - Pointer to the background image used. Set this
                 to NULL if not used.
        color  - If the background image is NULL, background is
                 assumed to be this color.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void GFX_BackgroundSet(         uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage,
                                GFX_COLOR color);

// *****************************************************************************
/*  
    <GROUP background_functions>

    Function:
        void GFX_BackgroundTypeSet(GFX_BACKGROUND_TYPE type)

    Summary:
        This function sets the background type.

    Description:
        This function sets the background type.

    Precondition:
        None.

    Parameters:
        type   - Type of background that will be used.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void GFX_BackgroundTypeSet(GFX_BACKGROUND_TYPE type);

// *****************************************************************************
/*  
    <GROUP background_functions>

    Function:
        GFX_BACKGROUND_TYPE GFX_BackgroundTypeGet(void)

    Summary:
        This function returns the type of the current background.

    Description:
        This function returns the type of the current background.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        The type of the current background set.

    Example:
        None.

*/
// *****************************************************************************
GFX_BACKGROUND_TYPE GFX_BackgroundTypeGet(void);

// *****************************************************************************
/*  
    <GROUP background_functions>

    Function:
        GFX_RESOURCE_HDR *GFX_BackgroundImageGet(void)

    Summary:
        This function returns the image used in the current background.

    Description:
        This function returns the pointer to the image used in
        the current background.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        The pointer to the image used in the current background.

    Example:
        None.

*/
// *****************************************************************************
GFX_RESOURCE_HDR *GFX_BackgroundImageGet(void);

// *****************************************************************************
/*  
    <GROUP background_functions>

    Function:
        GFX_COLOR GFX_BackgroundColorGet(void)

    Summary:
        This function returns the color used in the current background.

    Description:
        This function returns the color used in the current background.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        The color used in the current background.

    Example:
        None.

*/
// *****************************************************************************
GFX_COLOR GFX_BackgroundColorGet(void);

// *****************************************************************************
/*  
    <GROUP background_functions>

    Function:
        uint16_t GFX_BackgroundImageLeftGet(void)

    Summary:
        This function returns the horizontal starting position of
        the current background.

    Description:
        This function returns the horizontal starting position of
        the current background. This position defines the
        x position of the upper left corner of the background.


    Precondition:
        None.

    Parameters:
        None.

    Returns:
        The horizontal starting position of the current background.

    Example:
        None.

*/
// *****************************************************************************
uint16_t GFX_BackgroundImageLeftGet(void);

// *****************************************************************************
/*  
    <GROUP background_functions>

    Function:
        uint16_t GFX_BackgroundImageTopGet(void)

    Summary:
        This function returns the vertical starting position of
        the current background.

    Description:
        This function returns the vertical starting position of
        the current background. This position defines the
        y position of the upper left corner of the background.


    Precondition:
        None.

    Parameters:
        None.

    Returns:
        The vertical starting position of the current background.

    Example:
        None.

*/
// *****************************************************************************
uint16_t GFX_BackgroundImageTopGet(void);

// *****************************************************************************
/*  
    <GROUP double_buffer_functions>
    
    Function:
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

        For systems with single buffer, frame buffer and draw buffer are
        the same buffer. Calls to this function will have no effect and
        will always return GFX_STATUS_SUCCESS. The size of the buffer is
        defined by the dimension of the screen and the color depth used.

    Precondition:
        None.

    Parameters:
        index - specifies the index value of the buffer in the array
                of buffers. For single buffer systems, the given index
                will be ignored.
        address - specifies the location of the buffer in memory.

    Returns:
        Status of the buffer set.
        GFX_STATUS_SUCCESS - the buffer was successfully set.
        GFX_STATUS_FAILURE - the buffer was not successfully set.

    Example:
        None.

*/
// *****************************************************************************
GFX_STATUS GFX_DrawBufferInitialize(
                                uint16_t index,
                                uint32_t address);

// *****************************************************************************
/*  
    <GROUP double_buffer_functions>

    Function:
        uint16_t GFX_DrawBufferGet(void)

    Summary:
        This function returns the index of the current draw buffer.

    Description:
        This function returns the index of the current draw buffer.
        Draw buffer is the buffer where rendering is performed.
        For systems with single buffer this function will always
        return 0.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        The index of the current draw buffer set.

    Example:
        None.

*/
// *****************************************************************************
uint16_t GFX_DrawBufferGet(void);

// *****************************************************************************
/*  
    <GROUP double_buffer_functions>

    Function:
        GFX_STATUS GFX_DrawBufferSet(uint16_t index)

    Summary:
        This function sets the draw buffer with the given index number.

    Description:
        This function sets the draw buffer with the given index number.
        For systems with single buffer, frame buffer and draw buffer are
        the same buffer. Calls to this function will have no effect and
        will always return GFX_STATUS_SUCCESS.

    Precondition:
        None.

    Parameters:
        index - the index of the buffer to be set as the draw buffer.

    Returns:
        Status of the draw buffer set.
        GFX_STATUS_SUCCESS - the draw buffer was successfully set.
        GFX_STATUS_FAILURE - the draw buffer was not successfully set.

    Example:
        None.

*/
// *****************************************************************************
GFX_STATUS GFX_DrawBufferSet(uint16_t index);

// *****************************************************************************
/*  
    <GROUP double_buffer_functions>

    Function:
        uint16_t GFX_FrameBufferGet(void)

    Summary:
        This function returns the index of the current frame buffer.

    Description:
        This function returns the index of the current frame buffer.
        Frame buffer is the buffer that is currently displayed in the screen.
        For systems with single buffer this function will always
        return 0.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        The index of the current frame buffer set.

    Example:
        None.

*/
// *****************************************************************************
uint16_t GFX_FrameBufferGet(void);

// *****************************************************************************
/*  
    <GROUP double_buffer_functions>

    Function:
        GFX_STATUS GFX_FrameBufferSet(uint16_t index)

    Summary:
        This function sets the frame buffer with the given index number.

    Description:
        This function sets the frame buffer with the given index number.
        This is the buffer that is displayed on the screen.
        For systems with single buffer, frame buffer and draw buffer are
        the same buffer. Calls to this function will have no effect and
        will always return GFX_STATUS_SUCCESS.

    Precondition:
        None.

    Parameters:
        index - the index of the buffer to be set as the frame buffer.

    Returns:
        Status of the draw buffer set.
        GFX_STATUS_SUCCESS - the draw buffer was successfully set.
        GFX_STATUS_FAILURE - the draw buffer was not successfully set.

    Example:
        None.

*/
// *****************************************************************************
GFX_STATUS GFX_FrameBufferSet(uint16_t index);

// *****************************************************************************
/*  
    <GROUP double_buffer_functions>

    Function:
        GFX_STATUS GFX_DoubleBufferEnable(void)

    Summary:
        This function enables the double buffering feature of the graphics
        library.

    Description:
        Double buffering is a feature where two buffers are utilized to
        perform rendering on one buffer while displaying the other buffer.
        The frame buffer is the buffer that is being displayed while the
        draw buffer is used for rendering.

        When this function is called, the draw and the frame buffer
        are initialized then synchronized. After the function returns,
        the two buffers will contain the same pixel information.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        Status of the double buffering feature enabling.
        GFX_STATUS_SUCCESS - the double buffering was successfully enabled.
        GFX_STATUS_FAILURE - the double buffering was not successfully enabled.

    Example:
        None.

*/
// *****************************************************************************
GFX_STATUS GFX_DoubleBufferEnable(void);

// *****************************************************************************
/*  
    <GROUP double_buffer_functions>

    Function:
        GFX_STATUS GFX_DoubleBufferDisable(void)

    Summary:
        This function disables the double buffering feature of the graphics
        library.

    Description:
        This function disables the double buffering feature of the graphics
        library. The function sets the draw and frame buffer to use the
        same memory area. Any rendering done will immediately be reflected
        on the screen.

        Before the draw and frame buffers are set to use the same memory
        location, the function will synchronize the contents of the frame
        and draw buffers. This makes sure that all rendering performed
        after the last synchronization is synchronized and will show up
        on the screen.
 
    Precondition:
        None.

    Parameters:
        None.

    Returns:
        Status of the double buffering feature disabling.
        GFX_STATUS_SUCCESS - the double buffering was successfully disabled.
        GFX_STATUS_FAILURE - the double buffering was not successfully
                             disabled.

    Example:
        None.

*/
// *****************************************************************************
GFX_STATUS GFX_DoubleBufferDisable(void);

// *****************************************************************************
/*  
    <GROUP double_buffer_functions>

    Function:
        GFX_FEATURE_STATUS GFX_DoubleBufferStatusGet(void)

    Summary:
        This function returns the status of the double buffering feature
        of the graphics library if enabled or not.

    Description:
        This function returns the status of the double buffering feature
        of the graphics library if enabled or not.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        Status of the double buffering feature disabling.
        GFX_FEATURE_ENABLED - the double buffering feature is enabled.
        GFX_FEATURE_DISABLED - the double buffering feature is not enabled.

    Example:
        None.

*/
// *****************************************************************************
GFX_FEATURE_STATUS GFX_DoubleBufferStatusGet(void);

// *****************************************************************************
/*  
    <GROUP double_buffer_functions>

    Function:
        GFX_STATUS GFX_DoubleBufferSynchronize(void)

    Summary:
        This function synchronizes the contents of the draw and
        frame buffer immediately.

    Description:
        This function synchronizes the contents of the draw and
        frame buffer immediately. Contents of both frame and draw
        buffer will be the same after the function exits.

    Precondition:
        Double buffering feature must be enabled.

    Parameters:
        None.

    Returns:
        Status of the double buffer synchronization.
        GFX_STATUS_SUCCESS - the synchronization was successfull.
        GFX_STATUS_FAILURE - the synchronization was not successfull.

    Example:
        None.

*/
// *****************************************************************************
GFX_STATUS GFX_DoubleBufferSynchronize(void);

// *****************************************************************************
/*  
    <GROUP double_buffer_functions>

    Function:
        GFX_STATUS GFX_DoubleBufferSynchronizeRequest(void)

    Summary:
        This function requests synchronization of the contents
        of the draw and frame buffer.

    Description:
        This function requests synchronization of the contents
        of the draw and frame buffer. The contents will be synchronized
        on the next vertical blanking period.

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
GFX_STATUS GFX_DoubleBufferSynchronizeRequest(void);

// *****************************************************************************
/*  
    <GROUP double_buffer_functions>

    Function:
        GFX_FEATURE_STATUS GFX_DoubleBufferSynchronizeStatusGet(void)

    Summary:
        This function returns the status of the synchronization request
        of the draw and frame buffer.

    Description:
        This function returns the status of the synchronization request
        of the draw and frame buffer.

    Precondition:
        Double buffering feature must be enabled.

    Parameters:
        None.

    Returns:
        Status of the double buffer synchronization request.
        GFX_FEATURE_ENABLED - the synchronization request is enabled.
        GFX_FEATURE_DISABLED - the synchronization request is not enabled.

    Example:
        None.

*/
// *****************************************************************************
GFX_FEATURE_STATUS GFX_DoubleBufferSynchronizeStatusGet(void);

// *****************************************************************************
/*
    <GROUP double_buffer_functions>

    Function:
        void GFX_DoubleBufferAreaMark(
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom)

    Summary:
        This function adds the given rectangular area as an area that
        will be included in the list of areas for synchronization.

    Description:
        This function adds the given rectangular area into the list of
        rectangular areas for synchronization. When this function is called,
        the given rectangular area is assumed to contain new pixel information
        and is added into the list to be synchronized.

        Synchronization can be scheduled using the
        GFX_DoubleBufferSynchronizeRequest() or immediately performed
        using GFX_DoubleBufferSynchronize().

    Precondition:
        Double buffering feature must be enabled.

    Parameters:
        left - defines the left most pixel of the rectangular area.
        top - defines the top most pixel of the rectangular area.
        right - defines the right most pixel of the rectangular area.
        bottom - defines the bottom most pixel of the rectangular area.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void GFX_DoubleBufferAreaMark(
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom);

// DOM-IGNORE-BEGIN
// *****************************************************************************
/*  
    <GROUP double_buffer_functions>

    Function:
        GFX_FEATURE_STATUS GFX_DoubleBufferSynchronizeCancel(void)

    Summary:
        This function cancels the request for synchronization of the contents
        of the draw and frame buffer.

    Description:
        This function cancels the requests synchronization of the contents
        of the draw and frame buffer. The contents will be synchronized
        on the next vertical blanking period.

    Precondition:
        Double buffering feature must be enabled.

    Parameters:
        None.

    Returns:
        Status of the double buffer synchronization request cancel.
        GFX_STATUS_SUCCESS - the synchronization request cancel
                             was successfull.
        GFX_STATUS_FAILURE - the synchronization request cancel
                             was not successfull.

    Example:
        None.

*/
// *****************************************************************************
GFX_STATUS GFX_DoubleBufferSynchronizeCancel(void);

// *****************************************************************************
/*  
    <GROUP double_buffer_functions>

    Function:
        GFX_STATUS GFX_DoubleBufferSynchronizeSet(void)

    Summary:
        This function sets the synchronization request flag.

    Description:
        This function sets the synchronization request flag.
        This function is internal to the library and should not be
        called by the application.

    Precondition:
        Double buffering feature must be enabled.

    Parameters:
        None.

    Returns:
        Status of the double buffer synchronization flag set.
        GFX_STATUS_SUCCESS - the synchronization request flag set
                             was successfull.
        GFX_STATUS_FAILURE - the synchronization request flag set
                             was not successfull.

    Example:
        None.

*/
// *****************************************************************************
GFX_STATUS GFX_DoubleBufferSynchronizeSet(void);

// *****************************************************************************
/*
    <GROUP double_buffer_functions>

    Function:
        uint16_t GFX_DoubleBufferSyncAreaCountGet(void)

    Summary:
        This function returns the current count of rectangular areas
        that needs to be synchronized.

    Description:
        This function returns the current count of rectangular areas
        that needs to be synchronized.

    Precondition:
        Double buffering feature must be enabled.

    Parameters:
        None.

    Returns:
        The current count of rectangular areas that needs synchronization.

    Example:
        None.

*/
// *****************************************************************************
uint16_t GFX_DoubleBufferSyncAreaCountGet(void);

// *****************************************************************************
/*
    <GROUP double_buffer_functions>

    Function:
        void GFX_DoubleBufferSyncAreaCountSet(uint16_t count)

    Summary:
        This function sets the current count of rectangular areas
        that needs to be synchronized.

    Description:
        This function sts the current count of rectangular areas
        that needs to be synchronized.

    Precondition:
        Double buffering feature must be enabled.

    Parameters:
        The count of rectangular areas that will need synchronization.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void GFX_DoubleBufferSyncAreaCountSet(uint16_t count);

// *****************************************************************************
/*
    <GROUP double_buffer_functions>

    Function:
        GFX_STATUS GFX_DoubleBufferSyncAllStatusSet(void)

    Summary:
        This function sets the whole draw buffer to be unsynchronized.

    Description:
        This function sets the whole draw buffer to be unsynchronized.
        The next synchronization will copy all pixels of the
        draw buffer to the frame buffer.

    Precondition:
        Double buffering feature must be enabled.

    Parameters:
        None.

    Returns:
        Status of the synchronize all set.
        GFX_STATUS_SUCCESS - the synchronization all
                             was successfull.
        GFX_STATUS_FAILURE - the synchronization all
                             was not successfull.

    Example:
        None.

*/
// *****************************************************************************
GFX_STATUS GFX_DoubleBufferSyncAllStatusSet(void);

// *****************************************************************************
/*
    <GROUP double_buffer_functions>

    Function:
        GFX_STATUS GFX_DoubleBufferSyncAllStatusClear(void)

    Summary:
        This function clears the synchronize all status.

    Description:
        This function clears the synchronize all status.

    Precondition:
        Double buffering feature must be enabled.

    Parameters:
        None.

    Returns:
        Status of the synchronize all clear.
        GFX_STATUS_SUCCESS - the synchronization all clear
                             was successfull.
        GFX_STATUS_FAILURE - the synchronization all clear
                             was not successfull.

    Example:
        None.

*/
// *****************************************************************************
GFX_STATUS GFX_DoubleBufferSyncAllStatusClear(void);

// *****************************************************************************
/*
    <GROUP double_buffer_functions>

    Function:
        GFX_FEATURE_STATUS GFX_DoubleBufferSyncAllStatusGet(void)

    Summary:
        This function returns the status of the synchronize all flag.

    Description:
        This function returns the status of the synchronize all flag.

    Precondition:
        Double buffering feature must be enabled.

    Parameters:
        None.

    Returns:
        Status of the synchronize all flag.
        GFX_FEATURE_ENABLED - the synchronization all is set.
        GFX_FEATURE_DISABLED - the synchronization all is not set.

    Example:
        None.

*/
// *****************************************************************************
GFX_FEATURE_STATUS GFX_DoubleBufferSyncAllStatusGet(void);

// *****************************************************************************
/*  
    <GROUP double_buffer_functions>

    Function:
        GFX_RECTANGULAR_AREA GFX_DoubleBufferAreaGet(uint16_t index)

    Summary:
        This function returns rectangular area position on the
        draw buffer.

    Description:
        This function returns rectangular area position on the
        draw buffer.

    Precondition:
        Double buffering feature must be enabled.

    Parameters:
        index - the index of the rectangular area located in the array of
                areas that needs synchronization.

    Returns:
        The location of the rectangular area specified by the
        structure GFX_RECTANGULAR_AREA.

    Example:
        None.

*/
// *****************************************************************************
GFX_RECTANGULAR_AREA *GFX_DoubleBufferAreaGet(uint16_t index);

// *****************************************************************************
/*  
    <GROUP misc_functions>

    Function:
        int16_t GFX_SineCosineGet(
                               int16_t v,
                               GFX_TRIG_FUNCTION_TYPE type)

    Summary:
        This function returns sine or cosine of an angle.

    Description:
        This function returns sine or cosine of an angle. The angle
        range is -360 to 360 degrees. This function uses the Graphics
        Library look up table of 0-90 degrees. Depending on the
        given angle, the appropriate sign of the result is calculated.

    Precondition:
        None.

    Parameters:
        v - the angle used to retrieve the sine or cosine value.
            The angle must be in the range of -360 to 360 degrees.
        type - sets if the angle calculation is for a sine or cosine
            - GFX_TRIG_SINE_TYPE   - get the value of sine(v).
            - GFX_TRIG_COSINE_TYPE - return the value of cosine(v).

    Returns:
        Value of the sine or cosine of the given angle.

    Example:
        None.

*/
// *****************************************************************************
int16_t GFX_SineCosineGet(int16_t v, GFX_TRIG_FUNCTION_TYPE type);
// DOM-IGNORE-END

// DOM-IGNORE-BEGIN
// *****************************************************************************
/*  
    <GROUP misc_functions>

    Function:
        int16_t GFX_SineGet(int16_t angle)

    Summary:
        This function returns sine of an angle.

    Description:
        This function returns sine of an angle. The angle range is
        -360 to 360 degrees. This function uses the Graphics
        Library look up table of 0-90 degrees. Depending on the
        given angle, the appropriate sign of the result is calculated.

    Precondition:
        None.

    Parameters:
        v - the angle used to retrieve the sine value.
            The angle must be in the range of -360 to 360 degrees.

    Returns:
        Sine value of the given angle.

    Example:
        None.

*/
// *****************************************************************************
#define GFX_SineGet(angle) GFX_SineCosineGet(angle, GFX_TRIG_SINE_TYPE)
// DOM-IGNORE-END

// DOM-IGNORE-BEGIN
// *****************************************************************************
/*  
    <GROUP misc_functions>

    Function:
        int16_t GFX_CosineGet(int16_t angle)

    Summary:
        This function returns cosine of an angle.

    Description:
        This function returns cosine of an angle. The angle range is
        -360 to 360 degrees. This function uses the Graphics
        Library look up table of 0-90 degrees. Depending on the
        given angle, the appropriate sign of the result is calculated.

    Precondition:
        None.

    Parameters:
        v - the angle used to retrieve the cosine value.
            The angle must be in the range of -360 to 360 degrees.

    Returns:
        Cosine value of the given angle.

    Example:
        None.

*/
// *****************************************************************************
#define GFX_CosineGet(angle) GFX_SineCosineGet(angle, GFX_TRIG_COSINE_TYPE)
// DOM-IGNORE-END

// DOM-IGNORE-BEGIN
// *****************************************************************************
/*  
    <GROUP misc_functions>

    Function:
        void GFX_CirclePointGet(
                                int16_t radius,
                                int16_t angle,
                                int16_t *pX,
                                int16_t *pY)

    Summary:
        This function sets the x,y position of the point on a
        circle given the angle and the radius.

    Description:
        This function sets the x,y position of the point on a
        circle given the angle and the radius. This function is
        an internal function for the Graphics Library.
        Application should not call this function.

    Precondition:
        None.

    Parameters:
        radius - radius of the circle
        angle - the angle of the point in the circle in degrees.
        pX - the address of the variable that will hold the
             x-coordinate position of the point.
        pY - the address of the variable that will hold the
             y-coordinate position of the point.

    Returns:
        The x,y position of the point in the circle. The values are
        placed in the address pointed to by pX and pY.

    Example:
        None.

*/
// *****************************************************************************
void GFX_CirclePointGet(
                                int16_t radius,
                                int16_t angle,
                                int16_t *pX,
                                int16_t *pY);
// DOM-IGNORE-END

#ifndef GFX_CONFIG_GRADIENT_DISABLE

// DOM-IGNORE-BEGIN

// *****************************************************************************
/*  
    <GROUP polygon_fill_functions>
    
    Function:
        GFX_STATUS GFX_BarGradientDraw(
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom)

    Summary:
        This function renders a bar filled gradient colors.

    Description:
        This function renders a bar filled gradient colors. The gradient
        start and end colors are set by GFX_GradientColorSet().
        
         <img name="bargradient.jpg" />

        The rendering of this shape becomes undefined when any one of the
        following is true:
        - Any of the following pixel locations left,top or right,bottom
          falls outside the frame buffer.
        - Fill style is not set (GFX_FillStyleSet(), before this function
          is called.
        - Gradient colors are not set before this function is called.
        - When right < left
        - When bottom < top 
        - When pixel locations defined by left, top and/or right, bottom
          are not on the frame buffer.

    Precondition:
        Fill style must be set by GFX_FillStyleSet().
        Color must be set by GFX_GradientColorSet().

    Parameters:
        left - defines the left most pixel of the shape.
        top - defines the top most pixel of the shape.
        right - defines the right most pixel of the shape.
        bottom - defines the bottom most pixel of the shape.

    Returns:
        Status of the bar gradient draw.
        GFX_STATUS_SUCCESS when rendering is finished.
        GFX_STATUS_FAILURE when rendering is not yet finished or terminated
        because of an error condition.

    Example:
        None.

*/
// *****************************************************************************
GFX_STATUS GFX_BarGradientDraw(
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom);
// DOM-IGNORE-END

// DOM-IGNORE-BEGIN

// *****************************************************************************
/*  
    <GROUP polygon_fill_functions>

    Function:
        GFX_STATUS GFX_BevelGradientDraw(
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom,
                                uint16_t rad)

    Summary:
        This function renders a bar with rounded corners filled with
        gradient colors.

    Description:
        This function renders a bar with rounded corners  filled with
        gradient colors. The gradient start and end colors are set
        by GFX_GradientColorSet().
         <img name="BevelGradient.jpg" />

        The rendering of this shape becomes undefined when any one of the
        following is true:
        - Any of the following pixel locations left,top or right,bottom
          falls outside the frame buffer.
        - Fill style is not set (GFX_FillStyleSet(), before this function
          is called.
        - Gradient colors are not set before this function is called.
        - When right < left
        - When bottom < top
        - When pixel locations defined by left, top and/or right, bottom
          are not on the frame buffer.

    Precondition:
        Fill style must be set by GFX_FillStyleSet().
        Color must be set by GFX_GradientColorSet().

    Parameters:
        left - defines the left most pixel of the shape.
        top - defines the top most pixel of the shape.
        right - defines the right most pixel of the shape.
        bottom - defines the bottom most pixel of the shape.
        rad - defines the redius of the circle, that draws the rounded
              corners. When rad = 0, the object drawn is a
              rectangular gradient.

    Returns:
        Status of the bevel gradient draw.
        GFX_STATUS_SUCCESS when rendering is finished.
        GFX_STATUS_FAILURE when rendering is not yet finished or terminated
        because of an error condition.

    Example:
        None.

*/
// *****************************************************************************
GFX_STATUS GFX_BevelGradientDraw(
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom,
                                uint16_t rad);
// DOM-IGNORE-END

#endif

// *****************************************************************************
/*  
    <GROUP external_resources_functions>

    Function:
        GFX_STATUS GFX_ExternalResourceCallback(
                                GFX_RESOURCE_HDR *pResource,
                                uint32_t offset,
                                uint16_t nCount,
                                void *pBuffer)
  
    Summary:
        This function performs data fetch from external memory.

    Description:
        This function must be implemented in the application. The library will
        call this function each time when the external memory data will be
        required. 

        The application must copy requested byte quantity into the buffer 
        provided. Data start address in external memory is a sum of the
        address in GFX_RESOURCE_HDR structure and offset.

        An example of a situation where external memory will be accessed 
        is when external fonts or images are used. External resources in the
        library are defined by the type in the GFX_RESOURCE_HDR (see 
        GFX_RESOURCE for details).

    Conditions:
        None.

    Input:
        pResource -  Pointer to the external memory resource information.
        offset - Offset of the data from the location of the resource in
                 external memory.
        nCount - Number of bytes to be transferred into the buffer.
        buffer - Pointer to the buffer that will hold the retrieved data.

    Return:
        Status of the external resource callback. GFX_STATUS_SUCCESS when all
        the data was succesfully retrieved. GFX_STATUS_FAILURE when partial or
        no data was retrieved.

  Example:

    None.                                                                 

*/
// *****************************************************************************
GFX_STATUS GFX_ExternalResourceCallback(
                                GFX_RESOURCE_HDR *pResource,
                                uint32_t offset,
                                uint16_t nCount,
                                void     *pBuffer);

/* &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& */
//                         INTERNAL FUNCTIONS/MACROS
/* &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& */

// DOM-IGNORE-BEGIN
// *****************************************************************************
/*  
    <GROUP misc_functions>

    Function:
        void GFX_ImageOffsetAddressGet(
                                uint32_t address,
                                uint16_t xOffset,
                                uint16_t yOffset,
                                uint16_t width)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        This function returns the calculated address offset based on the
        width, vertical and horizontal pixel offset of a portion of an image.
        This function is used in GFX_ImageDraw() functions. The reason this
        inline function is created is to minimize error in using the same
        equation in calculating the address offset of a portion of an image
        that is being rendered.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        The calculated address offset.

    Example:
        None.

*/
// *****************************************************************************
uint32_t GFX_ImageOffsetAddressGet(
                                uint32_t address,
                                uint16_t xOffset,
                                uint16_t yOffset,
                                uint16_t width);
// DOM-IGNORE-END

// DOM-IGNORE-BEGIN
// *****************************************************************************
/*  
    <GROUP polygon_functions>

    Function:
        void GFX_ThickBevelDraw(    uint16_t xL,
                                uint16_t yT,
                                uint16_t xR,
                                uint16_t yB,
                                uint16_t r1,
                                uint16_t r2,
                                uint8_t octant)
 
    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Draws the octant arc of the beveled figure with the given centers,
        radii and octant mask.
        When octant = 0xFF and the following are true:
            1. xL = xR, yT = yB , r1 = 0 and r2 = z, a filled circle is drawn
               with a radius of z.
            2. radii have values (where r1 < r2), a full ring with thickness
               of (r2-r1) is drawn.
            3. xL != xR, yT != yB , r1 = 0 and r2 = 0 (where xR > xL and 
               yB > yT) a rectangle is drawn. xL, yT specifies the left,top
               corner and xR, yB specifies the right,bottom corner.
        When octant != 0xFF the figure drawn is the subsection of the 8 
        section figure where each non-zero bit of the octant value specifies
        the octants that will be drawn.

    Precondition:
        None.

    Parameters:
        xL, yT - location of the upper left center in the x,y coordinate
        xR, yB - location of the lower right left center in the x,y coordinate
        r1, r2 - the two concentric circle radii, r1 as the radius
                 of the smaller circle and and r2 as the radius of the
                 larger circle.
        octant - bitmask of the octant that will be drawn.
                 Moving in a clockwise direction from x = 0, y = +radius
                    bit0 : first octant   bit4 : fifth octant
                    bit1 : second octant  bit5 : sixth octant
                    bit2 : third octant   bit6 : seventh octant
                    bit3 : fourth octant  bit7 : eigth octant
 
    Returns:
        The status of the screen clearing.
        GFX_STATUS_SUCCESS - screen was cleared.
        GFX_STATUS_FAILURE - screen is not yet cleared

    Example:
        None.

*/
// *****************************************************************************
GFX_STATUS GFX_ThickBevelDraw(    uint16_t xL,
                                uint16_t yT, 
                                uint16_t xR, 
                                uint16_t yB, 
                                uint16_t r1, 
                                uint16_t r2, 
                                uint8_t octant);
// DOM-IGNORE-END

// DOM-IGNORE-BEGIN
// *****************************************************************************
/*  
    <GROUP style_functions>

    Function:
        void GFX_BevelDrawTypeSet(GFX_BEVEL_RENDER_TYPE type)

    Summary:
        This function sets the bevel rendering type.

    Description:
        This function sets the bevel rendering type. The types
        are described in GFX_BEVEL_RENDER_TYPE type enumeration.
        DRAWFULLBEVEL - draw the full shape
        DRAWTOPBEVEL - draw the upper half portion
        DRAWBOTTOMBEVEL - draw the lower half portion

    Precondition:
        None.

    Parameters:
        type   - Type of bevel rendering that will be used.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void GFX_BevelDrawTypeSet(GFX_BEVEL_RENDER_TYPE type);
// DOM-IGNORE-END

// DOM-IGNORE-BEGIN
// *****************************************************************************
/*  
    <GROUP style_functions>

    Function:
        GFX_BEVEL_RENDER_TYPE GFX_BevelDrawTypeGet(void)

    Summary:
        This function gets the bevel rendering type.

    Description:
        This function gets the bevel rendering type. The types
        are described in GFX_BEVEL_RENDER_TYPE type enumeration.

    Precondition:
        None.

    Parameters:
        None

    Returns:
        Type of bevel rendering.
        DRAWFULLBEVEL - draw the full shape
        DRAWTOPBEVEL - draw the upper half portion
        DRAWBOTTOMBEVEL - draw the lower half portion

    Example:
        None.

*/
// *****************************************************************************
GFX_BEVEL_RENDER_TYPE GFX_BevelDrawTypeGet(void);
// DOM-IGNORE-END

// *****************************************************************************
/*  
    <GROUP double_buffer_functions>

    Function:
        GFX_COLOR GFX_RenderingBufferGet(void)
 
    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Returns the location of the internal rendering buffer used in rendering
        images and filled objects with alpha blending.
        
    Precondition:
        None.

    Parameters:
        None.
         
    Returns:
        The address of the rendering buffer.

    Example:
        None.

*/
// *****************************************************************************
GFX_COLOR *GFX_RenderingBufferGet(void);

// DOM-IGNORE-BEGIN
// *****************************************************************************
/*  
    <GROUP text_functions>

    Function:
        void GFX_TextCharInfoFlashGet(
                                GFX_XCHAR ch,
                                GFX_FONT_OUTCHAR *pParam)
 
    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Retrieves the character information from flash memory. This is 
        a step performed before the character is rendered.
        
    Precondition:
        None.

    Parameters:
        ch - The ID of the character.
        pParam - The pointer to the structure where the character 
                 information will be saved.
         
    Returns:
        None.
        
    Example:
        None.

*/
// *****************************************************************************
void    GFX_TextCharInfoFlashGet (
                                GFX_XCHAR ch,
                                GFX_FONT_OUTCHAR *pParam);
// DOM-IGNORE-END

// DOM-IGNORE-BEGIN
// *****************************************************************************
/*  
    <GROUP text_functions>

    Function:
        void GFX_TextCharInfoExternalGet(
                                GFX_XCHAR ch,
                                GFX_FONT_OUTCHAR *pParam)
 
    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        Retrieves the character information from external memory. This is 
        a step performed before the character is rendered.
        
    Precondition:
        None.

    Parameters:
        ch - The ID of the character.
        pParam - The pointer to the structure where the character 
                 information will be saved.
         
    Returns:
        None.
        
    Example:
        None.

*/
// *****************************************************************************
void    GFX_TextCharInfoExternalGet(GFX_XCHAR ch,
                                GFX_FONT_OUTCHAR *pParam);
// DOM-IGNORE-END

// DOM-IGNORE-BEGIN
// *****************************************************************************
/*  
    <GROUP text_functions>

    Function:
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
                                GFX_XCHAR* textString,
                                GFX_RESOURCE_HDR *pFont);
// DOM-IGNORE-END

// DOM-IGNORE-BEGIN
// *****************************************************************************
/*  
    <GROUP text_functions>

    Function:
        void GFX_DRV_FontSet(GFX_RESOURCE_HDR *pFont)

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

    Precondition:
        None.

    Parameters:
        pFont - Pointer to the font selected.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void GFX_DRV_FontSet(GFX_RESOURCE_HDR *pFont);
// DOM-IGNORE-END

// DOM-IGNORE-BEGIN
// *****************************************************************************
/*  
    <GROUP double_buffer_functions>

    Function:
        void GFX_RenderToDisplayBufferDisable(void)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        This function disables the image draw functions from rendering
        images to the frame buffer. Instead, the image is rendered to
        the line buffer.

        This mode of image draw functions is used solely by the alpha
        blending rendering routine to buffer the image lines to allow
        manipulation of pixels for alpha blending.

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
void GFX_RenderToDisplayBufferDisable(void);
// DOM-IGNORE-END

// DOM-IGNORE-BEGIN
// *****************************************************************************
/*  
    <GROUP double_buffer_functions>

    Function:
        void GFX_RenderToDisplayBufferDisable(void)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        This function enbles the image draw functions to render
        images to the frame buffer. Images will be rendered to
        the frame buffer after this function is called.

        This mode of image draw functions is used solely by the alpha
        blending rendering routine to buffer the image lines to allow
        manipulation of pixels for alpha blending.

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
void GFX_RenderToDisplayBufferEnable(void);
// DOM-IGNORE-END

// DOM-IGNORE-BEGIN
// *****************************************************************************
/*  
    <GROUP double_buffer_functions>

    Function:
        uint16_t GFX_RenderToDisplayBufferDisableFlagGet(void)

    Summary:
        This function is an internal function and should not
        be called by the application.

    Description:
        This function returns the status of the image draw functions mode
        to render images to the frame buffer.

        This mode of image draw functions is used solely by the alpha
        blending rendering routine to buffer the image lines to allow
        manipulation of pixels for alpha blending.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        The status of the Image Draw rendering mode.

    Example:
        None.

*/
// *****************************************************************************
uint16_t GFX_RenderToDisplayBufferDisableFlagGet(void);
// DOM-IGNORE-END

#endif //#ifndef _GFX_PRIMITIVE_H
