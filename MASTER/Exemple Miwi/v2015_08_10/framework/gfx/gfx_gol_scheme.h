/*******************************************************************************
 Module for Microchip Graphics Library - Graphic Object Layer Scheme

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_gol_scheme.h

  Summary:
    Graphics Object Layer Scheme routines/macros of the Microchip Graphics Library.

  Description:
    This module implements the common routines for the Graphics Object Layer
    of the Microchip Graphics Library. The routines are independent of the
    Display Driver Layer and should be compatible with any Display Driver
    that is compliant with the requirements of the Display Driver
    Layer of the Graphics Library.
    The module utilizes the Graphics Primitive Layer to render the objects.
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

#ifndef _GFX_GOL_SCHEME_H
// DOM-IGNORE-BEGIN
      #define _GFX_GOL_SCHEME_H
// DOM-IGNORE-END
/*********************************************************************
 * Section: Includes
 *********************************************************************/
#include <stdbool.h>
#include <stdint.h>
#include "system_config.h"
#include "gfx/gfx_colors.h"

// *****************************************************************************
/*
    <GROUP gol_types>

    Typedef:
        GFX_GOL_OBJ_SCHEME

    Summary:
        The structure that defines the style scheme used by objects.

    Description:
        This structure specifies the style scheme components of an object.
        All objects will use the style scheme when rendering. Refer to
        specific object documentation on how the style scheme colors are
        utilized by the object.

        The style scheme allows objects to show 3D effects as well as
        feedback to users. For example, in Button objects, a press and
        release effect on the Buttons are easily shown by manipulating
        the colors when the object is drawn with a pressed state and
        released state. The style scheme also allows effects such as
        gradients and alpha blending. When using alpha blending, the
        style scheme requires objects to be associated with background
        information.

        A background can be a flat color background or an image. The usage
        of a background requires the background dimension to be larger than
        the object. In other words, the object should be drawn within the
        background dimension. Multiple objects can share a common background.
        As long as all the objects are drawn within the dimension of the
        background they can share a common background.

        The supported background types are (See GFX_BACKGROUND_TYPE):
        - GFX_BACKGROUND_COLOR - this type will set the common background
                                 to be a flat color. The color used is
                                 specified by CommonBkType.
        - GFX_BACKGROUND_IMAGE - this type will set the common background to
                                 be an image. The image used is specified
                                 by pCommonBkImage.

        When an object is associated with a background, it can be easily
        hidden or redrawn with some effects (for example alpha blending
        with the background). The background information allows the redrawing
        of the background with the object without the need to manually
        refreshing the background using primitive calls by the application.

        Note that alpha blending of gradient fill is not supported.

    Remarks:
        None.

 */
// *****************************************************************************
typedef struct
{
    GFX_COLOR           EmbossDkColor;              // Emboss dark color used for 3d effect.
    GFX_COLOR           EmbossLtColor;              // Emboss light color used for 3d effect.
    GFX_COLOR           TextColor0;                 // Character color 0 used for objects that supports text.
    GFX_COLOR           TextColor1;                 // Character color 1 used for objects that supports text.
    GFX_COLOR           TextColorDisabled;          // Character color used when object is in a disabled state.
    GFX_COLOR           Color0;                     // Color 0 usually assigned to an Object state.
    GFX_COLOR           Color1;                     // Color 1 usually assigned to an Object state.
    GFX_COLOR           ColorDisabled;              // Color used when an Object is in a disabled state.
    GFX_RESOURCE_HDR    *pFont;                     // Font selected for the scheme.

    GFX_FILL_STYLE      fillStyle;                  // must be set to a gradient type when using gradient

    GFX_COLOR           CommonBkColor;              // Background color used to hide Objects.
    uint16_t            CommonBkLeft;               // Horizontal starting position of the background.
    uint16_t            CommonBkTop;                // Vertical starting position of the background.
    GFX_BACKGROUND_TYPE CommonBkType;               // Specifies the type of background to use.
    GFX_RESOURCE_HDR    *pCommonBkImage;            // Pointer to the background image used. Set this

#ifndef GFX_CONFIG_ALPHABLEND_DISABLE
    uint16_t            AlphaValue;                 // Alpha value used for alpha blending
#endif
     
#ifndef GFX_CONFIG_GRADIENT_DISABLE
    GFX_COLOR           gradientStartColor;         // start color of the gradient fill
    GFX_COLOR           gradientEndColor;           // end color of the gradient fill
#endif

    uint16_t            EmbossSize;                 // Emboss size of the panel for 3-D effect. Set to zero
                                                    // if not used.

} GFX_GOL_OBJ_SCHEME;

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        void GFX_GOL_SchemeEmbossDarkColorSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_COLOR darkColor)

    Summary:
        This function sets the dark emboss color.

    Description:
        This function sets the dark emboss color of the style scheme.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme to modify.
        darkColor - new dark emboss color value.

    Returns:
        None.

*/
// *****************************************************************************
extern inline void __attribute__((always_inline)) GFX_GOL_SchemeEmbossDarkColorSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_COLOR darkColor)
{
    pScheme->EmbossDkColor = darkColor;
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        GFX_COLOR GFX_GOL_SchemeEmbossDarkColorGet(
                                GFX_GOL_OBJ_SCHEME *pScheme)

    Summary:
        This function returns the dark emboss color set.

    Description:
        This function returns the dark emboss color of the
        style scheme.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme.

    Returns:
        The current dark emboss color set.

*/
// *****************************************************************************
extern inline GFX_COLOR __attribute__((always_inline)) GFX_GOL_SchemeEmbossDarkColorGet(
                            GFX_GOL_OBJ_SCHEME *pScheme)
{
    return (pScheme->EmbossDkColor);
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        void GFX_GOL_SchemeEmbossLightColorSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_COLOR lightColor)

    Summary:
        This function sets the light emboss color.

    Description:
        This function sets the light emboss color of the
        style scheme.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme to modify.
        lightColor - new light emboss color value.

    Returns:
        None.

*/
// *****************************************************************************
extern inline void __attribute__((always_inline)) GFX_GOL_SchemeEmbossLightColorSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_COLOR lightColor)
{
    pScheme->EmbossLtColor = lightColor;
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        GFX_COLOR GFX_GOL_SchemeEmbossLightColorGet(
                                GFX_GOL_OBJ_SCHEME *pScheme)

    Summary:
        This function returns the light emboss color set.

    Description:
        This function returns the light emboss color of the
        style scheme.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme.

    Returns:
        The current light emboss color set.

*/
// *****************************************************************************
extern inline GFX_COLOR __attribute__((always_inline)) GFX_GOL_SchemeEmbossLightColorGet(
                                GFX_GOL_OBJ_SCHEME *pScheme)
{
    return (pScheme->EmbossLtColor);
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        void GFX_GOL_SchemeEmbossSizeSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                uint16_t size)

    Summary:
        This function sets the emboss size.

    Description:
        This function sets the emboss size of the
        style scheme. If no emboss is desired, set to zero.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme to modify.
        size - new emboss size value.

    Returns:
        None.

*/
// *****************************************************************************
extern inline void __attribute__((always_inline)) GFX_GOL_SchemeEmbossSizeSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                uint16_t size)
{
    pScheme->EmbossSize = size;
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        uint16_t GFX_GOL_SchemeEmbossSizeGet(
                                GFX_GOL_OBJ_SCHEME *pScheme)

    Summary:
        This function returns the emboss size set.

    Description:
        This function returns the emboss size of the
        style scheme.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme.

    Returns:
        The current emboss size set.

*/
// *****************************************************************************
extern inline uint16_t __attribute__((always_inline)) GFX_GOL_SchemeEmbossSizeGet(
                                GFX_GOL_OBJ_SCHEME *pScheme)
{
    return (pScheme->EmbossSize);
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        void GFX_GOL_SchemeEmbossSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_COLOR darkColor,
                                GFX_COLOR lightColor,
                                uint16_t size)

    Summary:
        This function sets emboss style.

    Description:
        This function sets emboss style of the
        style scheme.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme to modify.
        darkColor - new dark emboss color value.
        lightColor - new light emboss color value.
        size - new emboss size value.

    Returns:
        None.

*/
// *****************************************************************************
extern inline void __attribute__((always_inline)) GFX_GOL_SchemeEmbossSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_COLOR darkColor,
                                GFX_COLOR lightColor,
                                uint16_t size)
{
    pScheme->EmbossDkColor = darkColor;
    pScheme->EmbossLtColor = lightColor;
    pScheme->EmbossSize = size;
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        void GFX_GOL_SchemeTextColor0Set(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_COLOR textColor)

    Summary:
        This function sets the first text color.

    Description:
        This function sets the first text color of the
        style scheme.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme to modify.
        textColor - new color value.

    Returns:
        None.

*/
// *****************************************************************************
extern inline void __attribute__((always_inline)) GFX_GOL_SchemeTextColor0Set(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_COLOR textColor)
{
    pScheme->TextColor0 = textColor;
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        GFX_COLOR GFX_GOL_SchemeTextColor0Get(
                                GFX_GOL_OBJ_SCHEME *pScheme)

    Summary:
        This function returns the first text color set.

    Description:
        This function returns the first text color of the
        style scheme.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme.

    Returns:
        The current first text color set.

*/
// *****************************************************************************
extern inline GFX_COLOR __attribute__((always_inline)) GFX_GOL_SchemeTextColor0Get(
                                GFX_GOL_OBJ_SCHEME *pScheme)
{
    return (pScheme->TextColor0);
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        void GFX_GOL_SchemeTextColor1Set(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_COLOR textColor)

    Summary:
        This function sets the second text color.

    Description:
        This function sets the second text color of the
        style scheme.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme to modify.
        textColor - new color value.

    Returns:
        None.

*/
// *****************************************************************************
extern inline void __attribute__((always_inline)) GFX_GOL_SchemeTextColor1Set(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_COLOR textColor)
{
    pScheme->TextColor1 = textColor;
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        GFX_COLOR GFX_GOL_SchemeTextColor1Get(
                                GFX_GOL_OBJ_SCHEME *pScheme)

    Summary:
        This function returns the second text color set.

    Description:
        This function returns the second text color of the
        style scheme.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme.

    Returns:
        The current second text color set.

*/
// *****************************************************************************
extern inline GFX_COLOR __attribute__((always_inline)) GFX_GOL_SchemeTextColor1Get(
                                GFX_GOL_OBJ_SCHEME *pScheme)
{
    return (pScheme->TextColor1);
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        void GFX_GOL_SchemeTextColorDisableSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_COLOR disableColor)

    Summary:
        This function sets the disable text color.

    Description:
        This function sets the disable text color of the
        style scheme.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme to modify.
        textColor - new disable text color value.

    Returns:
        None.

*/
// *****************************************************************************
extern inline void __attribute__((always_inline)) GFX_GOL_SchemeTextColorDisableSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_COLOR disableColor)
{
    pScheme->TextColorDisabled = disableColor;
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        GFX_COLOR GFX_GOL_SchemeTextColorDisableGet(
                                GFX_GOL_OBJ_SCHEME *pScheme)

    Summary:
        This function returns the disable text color set.

    Description:
        This function returns the disable text color of the
        style scheme.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme.

    Returns:
        The current disable text color set.

*/
// *****************************************************************************
extern inline GFX_COLOR __attribute__((always_inline)) GFX_GOL_SchemeTextColorDisableGet(
                                GFX_GOL_OBJ_SCHEME *pScheme)
{
    return (pScheme->TextColorDisabled);
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        void GFX_GOL_SchemeTextColorSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_COLOR textColor0,
                                GFX_COLOR textColor1,
                                GFX_COLOR disableColor)

    Summary:
        This function sets text color style.

    Description:
        This function sets text color style of the
        style scheme.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme to modify.
        textColor0 - new first colorvalue.
        textColor1 - new second color value.
        disableColor - new disable text color value.

    Returns:
        None.

*/
// *****************************************************************************
extern inline void __attribute__((always_inline)) GFX_GOL_SchemeTextColorSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_COLOR textColor0,
                                GFX_COLOR textColor1,
                                GFX_COLOR disableColor)
{
    pScheme->TextColor0 = textColor0;
    pScheme->TextColor1 = textColor1;
    pScheme->TextColorDisabled = disableColor;
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        void GFX_GOL_SchemeColor0Set(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_COLOR color)

    Summary:
        This function sets the first style color.

    Description:
        This function sets the first style color of the
        style scheme.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme to modify.
        textColor - new color value.

    Returns:
        None.

*/
// *****************************************************************************
extern inline void __attribute__((always_inline)) GFX_GOL_SchemeColor0Set(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_COLOR color)
{
    pScheme->Color0 = color;
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        GFX_COLOR GFX_GOL_SchemeColor0Get(
                                GFX_GOL_OBJ_SCHEME *pScheme)

    Summary:
        This function returns the first style color set.

    Description:
        This function returns the first style color of the
        style scheme.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme.

    Returns:
        The current first style color set.

*/
// *****************************************************************************
extern inline GFX_COLOR __attribute__((always_inline)) GFX_GOL_SchemeColor0Get(
                                GFX_GOL_OBJ_SCHEME *pScheme)
{
    return (pScheme->Color0);
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        void GFX_GOL_SchemeColor1Set(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_COLOR color)

    Summary:
        This function sets the second style color.

    Description:
        This function sets the second style color of the
        style scheme.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme to modify.
        textColor - new color value.

    Returns:
        None.

*/
// *****************************************************************************
extern inline void __attribute__((always_inline)) GFX_GOL_SchemeColor1Set(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_COLOR color)
{
    pScheme->Color1 = color;
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        GFX_COLOR GFX_GOL_SchemeColor1Get(
                                GFX_GOL_OBJ_SCHEME *pScheme)

    Summary:
        This function returns the second style color set.

    Description:
        This function returns the second style color of the
        style scheme.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme.

    Returns:
        The current second style color set.

*/
// *****************************************************************************
extern inline GFX_COLOR __attribute__((always_inline)) GFX_GOL_SchemeColor1Get(
                                GFX_GOL_OBJ_SCHEME *pScheme)
{
    return (pScheme->Color1);
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        void GFX_GOL_SchemeColorDisabledSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_COLOR disableColor)

    Summary:
        This function sets the disable style color.

    Description:
        This function sets the disable style color of the
        style scheme.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme to modify.
        textColor - new disable style color value.

    Returns:
        None.

*/
// *****************************************************************************
extern inline void __attribute__((always_inline)) GFX_GOL_SchemeColorDisabledSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_COLOR disableColor)
{
    pScheme->ColorDisabled = disableColor;
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        GFX_COLOR GFX_GOL_SchemeColorDisabledGet(
                                GFX_GOL_OBJ_SCHEME *pScheme)

    Summary:
        This function returns the disable style color set.

    Description:
        This function returns the disable style color of the
        style scheme.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme.

    Returns:
        The current disable style color set.

*/
// *****************************************************************************
extern inline GFX_COLOR __attribute__((always_inline)) GFX_GOL_SchemeColorDisabledGet(
                                GFX_GOL_OBJ_SCHEME *pScheme)
{
    return (pScheme->ColorDisabled);
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        void GFX_GOL_SchemeColorSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_COLOR color0,
                                GFX_COLOR color1,
                                GFX_COLOR disableColor)

    Summary:
        This function sets color style.

    Description:
        This function sets colors of the
        style scheme.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme to modify.
        color0 - new first color value.
        color1 - new second color value.
        disableColor - new disable text color value.

    Returns:
        None.

*/
// *****************************************************************************
extern inline void __attribute__((always_inline)) GFX_GOL_SchemeColorSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_COLOR color0,
                                GFX_COLOR color1,
                                GFX_COLOR disableColor)
{
    pScheme->Color0 = color0;
    pScheme->Color1 = color1;
    pScheme->ColorDisabled = disableColor;

}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        void GFX_GOL_SchemeFontSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_RESOURCE_HDR *font)

    Summary:
        This function sets the style scheme font.

    Description:
        This function sets the style scheme font to be
        used.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme to modify.
        pFont - pointer to the font to be used.

    Returns:
        None.

*/
// *****************************************************************************
extern inline void __attribute__((always_inline)) GFX_GOL_SchemeFontSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_RESOURCE_HDR *pFont)
{
    pScheme->pFont = pFont;
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        GFX_RESOURCE_HDR GFX_GOL_SchemeFontGet(
                                GFX_GOL_OBJ_SCHEME *pScheme)

    Summary:
        This function returns the font set.

    Description:
        This function returns the font set in the
        style scheme.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme.

    Returns:
        The current font set.

*/
// *****************************************************************************
extern inline GFX_RESOURCE_HDR*__attribute__((always_inline)) GFX_GOL_SchemeFontGet(
                                GFX_GOL_OBJ_SCHEME *pScheme)
{
    return (pScheme->pFont);
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        void GFX_GOL_SchemeFillStyleSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_FILL_STYLE style)

    Summary:
        This function sets the fill style.

    Description:
        This function sets the fill style of the
        style scheme.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme to modify.
        style - the fill style to be used.

    Returns:
        None.

*/
// *****************************************************************************
extern inline void __attribute__((always_inline)) GFX_GOL_SchemeFillStyleSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_FILL_STYLE style)
{
    pScheme->fillStyle = style;
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        GFX_FILL_STYLE GFX_GOL_SchemeFillStyleGet(
                                GFX_GOL_OBJ_SCHEME *pScheme)

    Summary:
        This function returns the fill style set.

    Description:
        This function returns the fill style set in the
        style scheme.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme.

    Returns:
        The current fill style set.

*/
// *****************************************************************************
extern inline GFX_FILL_STYLE __attribute__((always_inline)) GFX_GOL_SchemeFillStyleGet(
                                GFX_GOL_OBJ_SCHEME *pScheme)
{
    return (pScheme->fillStyle);
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        void GFX_GOL_SchemeBackgroundColorSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_COLOR color)

    Summary:
        This function sets the common background color.

    Description:
        This function sets the common background color of the
        style scheme.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme to modify.
        color - the new background color to be used.

    Returns:
        None.

*/
// *****************************************************************************
extern inline void __attribute__((always_inline)) GFX_GOL_SchemeBackgroundColorSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_COLOR color)
{
    pScheme->CommonBkColor = color;
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        GFX_COLOR GFX_GOL_SchemeBackgroundColorGet(
                                GFX_GOL_OBJ_SCHEME *pScheme)

    Summary:
        This function returns the common background color set.

    Description:
        This function returns the common background color set in the
        style scheme.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme.

    Returns:
        The current common background color set.

*/
// *****************************************************************************
extern inline GFX_COLOR __attribute__((always_inline)) GFX_GOL_SchemeBackgroundColorGet(
                                GFX_GOL_OBJ_SCHEME *pScheme)
{
    return (pScheme->CommonBkColor);
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        void GFX_GOL_SchemeBackgroundImageSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage)

    Summary:
        This function sets the background image and its placement
        in the display buffer.

    Description:
        When rendering an object that is drawn on top of a brackground
        image, the background image information can be associated with
        the style scheme and allow easy refresh of the background image.
        This feature is required when alpha blending the object with
        the background image.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme to modify.
        left - the left most position of the image in the display
               buffer.
        top - the top most position of the image in the display
              buffer.
        pImage - the pointer to the structure of the image to
                 be used.

    Returns:
        None.

*/
// *****************************************************************************
extern inline void __attribute__((always_inline)) GFX_GOL_SchemeBackgroundImageSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                uint16_t left,
                                uint16_t top,
                                GFX_RESOURCE_HDR *pImage)
{
    pScheme->CommonBkLeft = left;
    pScheme->CommonBkTop = top;
    pScheme->pCommonBkImage = pImage;
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        void GFX_GOL_SchemeBackgroundTypeSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_BACKGROUND_TYPE type)

    Summary:
        This function sets the type of background used.

    Description:
        This function sets the type of background used
        by the object. Associating the style scheme with
        the background of the object enables features like
        alpha blending and gradient fills. This also allows
        refreshing of the background to be done automatically
        by the object's rendering function.

        The following background types are supported:
        - GFX_BACKGROUND_NONE - no background is used.

        - GFX_BACKGROUND_COLOR - Background is set to a color.
        - GFX_BACKGROUND_IMAGE - Background is set to an image.
        - GFX_BACKGROUND_DISPLAY_BUFFER - Background is set to
            the current contents of the display buffer.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme to modify.
        type - the background type to be used.

    Returns:
        None.

*/
// *****************************************************************************
extern inline void __attribute__((always_inline)) GFX_GOL_SchemeBackgroundTypeSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_BACKGROUND_TYPE type)
{
    pScheme->CommonBkType = type;
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        GFX_BACKGROUND_TYPE GFX_GOL_SchemeBackgroundTypeGet(
                                GFX_GOL_OBJ_SCHEME *pScheme)

    Summary:
        This function returns the current background type set.

    Description:
        This function returns the current background set in the
        style scheme.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme.

    Returns:
        The current background type set.

*/
// *****************************************************************************
extern inline GFX_BACKGROUND_TYPE __attribute__((always_inline)) GFX_GOL_SchemeBackgroundTypeGet(
                                GFX_GOL_OBJ_SCHEME *pScheme)
{
    return (pScheme->CommonBkType);
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        void GFX_GOL_SchemeAlphaPrecentSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                uint16_t percent)

    Summary:
        This function sets the alpha blending level.

    Description:
        This function sets the alpha blending level of the
        style scheme.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme to modify.
        percent - the alpha blending level be used.

    Returns:
        None.

*/
// *****************************************************************************
#if !defined(GFX_CONFIG_ALPHABLEND_DISABLE)
extern inline void __attribute__((always_inline)) GFX_GOL_SchemeAlphaPrecentSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                uint16_t percent)
{
    pScheme->AlphaValue = percent;
}
#endif

#if !defined (GFX_CONFIG_GRADIENT_DISABLE)


// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        void GFX_GOL_SchemeGradientStartColorSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_COLOR startColor)

    Summary:
        This function sets the gradient start color.

    Description:
        This function sets the gradient start color of the
        style scheme's gradient fill.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme to modify.
        startColor - the start color to be used.

    Returns:
        None.

*/
// *****************************************************************************
extern inline void __attribute__((always_inline)) GFX_GOL_SchemeGradientStartColorSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_COLOR startColor)
{
    pScheme->gradientStartColor = startColor;
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        GFX_COLOR GFX_GOL_SchemeGradientStartColorGet(
                                GFX_GOL_OBJ_SCHEME *pScheme)

    Summary:
        This function returns the current gradient start color set.

    Description:
        This function returns the current current gradient fill
        start color set in the style scheme.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme.

    Returns:
        The current gradient fill start color set.

*/
// *****************************************************************************
extern inline GFX_COLOR __attribute__((always_inline)) GFX_GOL_SchemeGradientStartColorGet(
                                GFX_GOL_OBJ_SCHEME *pScheme)
{
    return (pScheme->gradientStartColor);
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        void GFX_GOL_SchemeGradientEndColorSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_COLOR endColor)

    Summary:
        This function sets the gradient end color.

    Description:
        This function sets the gradient end color of the
        style scheme's gradient fill.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme to modify.
        endColor - the end color to be used.

    Returns:
        None.

*/
// *****************************************************************************
extern inline void __attribute__((always_inline)) GFX_GOL_SchemeGradientEndColorSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_COLOR endColor)
{
    pScheme->gradientEndColor = endColor;
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        GFX_COLOR GFX_GOL_SchemeGradientEndColorGet(
                                GFX_GOL_OBJ_SCHEME *pScheme)

    Summary:
        This function returns the current gradient end color set.

    Description:
        This function returns the current current gradient fill
        end color set in the style scheme.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme.

    Returns:
        The current gradient fill end color set.

*/
// *****************************************************************************
extern inline GFX_COLOR __attribute__((always_inline)) GFX_GOL_SchemeGradientEndColorGet(
                                GFX_GOL_OBJ_SCHEME *pScheme)
{
    return (pScheme->gradientEndColor);
}

// *****************************************************************************
/*
    <GROUP gol_objects_style_scheme>

    Function:
        void GFX_GOL_SchemeGradientColorSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_COLOR startColor,
                                GFX_COLOR endColor)

    Summary:
        This function sets graient fill start and end colors.

    Description:
        This function sets graient fill start and end colors
        of the style scheme.

    Precondition:
        None.

    Parameters:
        pScheme - pointer to the style scheme to modify.
        startColor - the start color to be used.
        endColor - the end color to be used.

    Returns:
        None.

*/
// *****************************************************************************
extern inline void __attribute__((always_inline)) GFX_GOL_SchemeGradientColorSet(
                                GFX_GOL_OBJ_SCHEME *pScheme,
                                GFX_COLOR startColor,
                                GFX_COLOR endColor)
{
    pScheme->gradientStartColor = startColor;
    pScheme->gradientEndColor = endColor;
}
#endif

#endif 
