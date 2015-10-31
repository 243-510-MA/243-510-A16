/*******************************************************************************
 Module for Microchip Graphics Library - Graphic Object Layer

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_gol_static_text.c

  Summary:
    This implements the static text object of the GOL.

  Description:
    Refer to Microchip Graphics Library for complete documentation of the
    Static Text Object.
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

#include <stdlib.h>
#include <stdint.h>
#include "gfx/gfx_gol_static_text.h"

// *****************************************************************************
/*  Function:
    GFX_GOL_STATICTEXT  *GFX_GOL_StaticTextCreate(
                                uint16_t            ID,
                                uint16_t            left,
                                uint16_t            top,
                                uint16_t            right,
                                uint16_t            bottom,
                                uint16_t            state,
                                GFX_XCHAR           *pText,
                                GFX_ALIGNMENT       alignment,
                                GFX_GOL_OBJ_SCHEME  *pScheme)

    Summary:
        This function creates a GFX_GOL_STATICTEXT object with the parameters
        given. It automatically attaches the new object into a global
        linked list of objects and returns the address of the object.


    Description:
        This function creates a GFX_GOL_STATICTEXT object with the parameters
        given. It automatically attaches the new object into a global
        linked list of objects and returns the address of the object.

        This function returns the pointer to the newly created object.
        If the object is not successfully created, it returns NULL.

        The text can be configured to have text aligned. See GFX_ALIGNMENT
        for details. Text can also have multiple lines by inserting
        the new line character to the text string supplied to the object.
        Any string that exceeds the dimension of the object will be clipped.

        The behavior of GFX_GOL_StaticTextCreate() will be undefined if one
        of the following is true:
        - left >= right
        - top >= bottom
        - pScheme is not pointing to a GFX_GOL_OBJ_SCHEME
        - pText is an unterminated string

*/
// *****************************************************************************
GFX_GOL_STATICTEXT  *GFX_GOL_StaticTextCreate(
                                uint16_t            ID,
                                uint16_t            left,
                                uint16_t            top,
                                uint16_t            right,
                                uint16_t            bottom,
                                uint16_t            state,
                                GFX_XCHAR           *pText,
                                GFX_ALIGNMENT       alignment,
                                GFX_GOL_OBJ_SCHEME  *pScheme)
{
    GFX_GOL_STATICTEXT  *pSt = NULL;

    pSt = (GFX_GOL_STATICTEXT *)GFX_malloc(sizeof(GFX_GOL_STATICTEXT));
    if(pSt == NULL)
        return (pSt);

    pSt->hdr.ID        = ID;                        // unique id assigned for referencing
    pSt->hdr.pNxtObj   = NULL;                      // initialize pointer to NULL
    pSt->hdr.type      = GFX_GOL_STATICTEXT_TYPE;   // set object type
    pSt->hdr.left      = left;                      // left,top corner
    pSt->hdr.top       = top;
    pSt->hdr.right     = right;                     // right buttom corner
    pSt->hdr.bottom    = bottom;
    pSt->hdr.state     = state;
    pSt->hdr.DrawObj   = GFX_GOL_StaticTextDraw;    // draw function
    pSt->hdr.actionGet = GFX_GOL_StaticTextActionGet; // action get function
    pSt->hdr.actionSet = NULL;                      // default action function
    pSt->hdr.FreeObj   = NULL;                      // free function

    // set the text if it exists
    if(pText != NULL)
    {
        GFX_GOL_StaticTextSet(pSt, pText);
    }
    else
    {
        pSt->pText = NULL;
    }
    GFX_GOL_StaticTextAlignmentSet(pSt, alignment);

    // Set the color scheme to be used
    pSt->hdr.pGolScheme = (GFX_GOL_OBJ_SCHEME *)pScheme;

    GFX_GOL_ObjectAdd((GFX_GOL_OBJ_HEADER *)pSt);
    return (pSt);
}

// *****************************************************************************
/*  Function:
    GFX_XCHAR *GFX_GOL_StaticTextGet(
                                GFX_GOL_STATICTEXT *pObject)

    Summary:
        This function returns the address of the current text
        string used by the object.

    Description:
        This function returns the address of the current text
        string used by the object.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.

    Returns:
        Pointer to text string.

    Example:
        <code>
            GFX_XCHAR *pChar;
            GFX_GOL_STATICTEXT OBJECT_ARRAY[2];

            pChar = GFX_GOL_StaticTextGet(OBJECT_ARRAY[0]);
       </code>

*/
// *****************************************************************************
GFX_XCHAR __attribute__ ((always_inline)) *GFX_GOL_StaticTextGet(
                                GFX_GOL_STATICTEXT *pObject)
{
    return (pObject->pText);
}

// *****************************************************************************
/*  Function:
    GFX_XCHAR *GFX_GOL_StaticTextSet(
                                GFX_GOL_STATICTEXT *pObject,
                                GFX_XCHAR *pText)

    Summary:
        This function sets the address of the current text
        string used by the object.

    Description:
        This function sets the address of the current text
        string used by the object.

*/
// *****************************************************************************
void __attribute__ ((always_inline)) GFX_GOL_StaticTextSet(
                                GFX_GOL_STATICTEXT *pObject,
                                GFX_XCHAR *pText)
{
    pObject->pText = pText;
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_StaticTextAlignmentSet(
                                GFX_GOL_STATICTEXT *pObject,
                                GFX_ALIGNMENT align)

    Summary:
        This function sets the text alignment of the text
        string used by the object.

    Description:
        This function sets the text alignment of the text
        string used by the object.

*/
// *****************************************************************************
void __attribute__ ((always_inline)) GFX_GOL_StaticTextAlignmentSet(
                                GFX_GOL_STATICTEXT *pObject,
                                GFX_ALIGNMENT align)
{
    pObject->alignment = align;
}

// *****************************************************************************
/*  Function:
    GFX_ALIGNMENT GFX_GOL_StaticTextAlignmentGet(
                                GFX_GOL_STATICTEXT *pObject)

    Summary:
        This function returns the text alignment of the text
        string used by the object.

    Description:
        This function returns the text alignment of the text
        string used by the object.

*/
// *****************************************************************************
GFX_ALIGNMENT __attribute__ ((always_inline)) GFX_GOL_StaticTextAlignmentGet(
                                GFX_GOL_STATICTEXT *pObject)
{
    return (pObject->alignment);
}

// *****************************************************************************
/*  Function:
    GFX_GOL_TRANSLATED_ACTION GFX_GOL_StaticTextActionGet(
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage);

    Summary:
        This function sets the text alignment of the text
        string used by the object.

    Description:
        This function evaluates the message from a user if the
        message will affect the object or not.
 
*/
// *****************************************************************************
GFX_GOL_TRANSLATED_ACTION GFX_GOL_StaticTextActionGet(
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage)
{

    GFX_GOL_STATICTEXT *pSt;

    pSt = (GFX_GOL_STATICTEXT *)pObject;

    // Evaluate if the message is for the static text
    // Check if disabled first
    if(GFX_GOL_ObjectStateGet(pSt, GFX_GOL_STATICTEXT_DISABLED_STATE))
        return (GFX_GOL_OBJECT_ACTION_INVALID);

#ifndef GFX_CONFIG_USE_TOUCHSCREEN_DISABLE
    if(pMessage->type == TYPE_TOUCHSCREEN)
    {
        // Check if it falls in static text control borders
        if
        (
            (pSt->hdr.left   < pMessage->param1) &&
            (pSt->hdr.right  > pMessage->param1) &&
            (pSt->hdr.top    < pMessage->param2) &&
            (pSt->hdr.bottom > pMessage->param2)
        )
        {
            return (GFX_GOL_STATICTEXT_ACTION_SELECTED);
        }
    }

#endif
    return (GFX_GOL_OBJECT_ACTION_INVALID);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_GOL_StaticTextDraw(void *pObject)

    Summary:
        This function renders the object on the screen based on the
        current state of the object.

    Description:
        This function renders the object on the screen based on the
        current state of the object. Location of the object is
        determined by the left, top, right and bottom parameters.
        The colors used are dependent on the state of the object.
        The font used is determined by the style scheme set.

        Text alignment based on the alignment parameter set on the object.

        When rendering objects of the same type, each object
        must be rendered completely before the rendering of the
        next object is started. This is to avoid incomplete
        object rendering.

        Normally, application will just call GFX_GOL_ObjectListDraw()
        to allow the Graphics Library to manage all object rendering.
        See GFX_GOL_ObjectListDraw() for more information on object rendering.

*/
// *****************************************************************************
GFX_STATUS GFX_GOL_StaticTextDraw(void *pObject)
{
    typedef enum
    {
        DRAW_STATIC_TEXT_IDLE,
        DRAW_STATIC_TEXT_CLEANAREA,
        DRAW_STATIC_TEXT_INIT,
        DRAW_STATIC_TEXT_DRAWTEXT,
    } ST_DRAW_STATES;

    static ST_DRAW_STATES   state = DRAW_STATIC_TEXT_IDLE;
    GFX_GOL_STATICTEXT      *pSt;
    
    static uint16_t         adjustBackground = 0;

#ifdef GFX_CONFIG_ALPHABLEND_DISABLE
#ifndef GFX_CONFIG_TRANSPARENT_COLOR_DISABLE
    uint16_t                transparentColorStatus;
    GFX_COLOR               transparentColorSet = 0;
#endif
#endif

    pSt = (GFX_GOL_STATICTEXT *)pObject;

    while(1)
    {
        if (GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
            return (GFX_STATUS_FAILURE);
    
        switch(state)
        {
            case DRAW_STATIC_TEXT_IDLE:

#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                GFX_DRIVER_SetupDrawUpdate( \
                        pSt->hdr.left,      \
                        pSt->hdr.top,       \
                        pSt->hdr.right,     \
                        pSt->hdr.bottom);
#endif

                if (GFX_GOL_ObjectStateGet(pSt, GFX_GOL_STATICTEXT_HIDE_STATE))
                {

                // Hide the GFX_GOL_STATICTEXT (remove from screen)
                if (GFX_GOL_ObjectHideDraw(&(pSt->hdr)) != GFX_STATUS_SUCCESS)
                    return (GFX_STATUS_FAILURE);

                    // State is still IDLE STATE so no need to set state
#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                GFX_DRIVER_CompleteDrawUpdate(  \
                        pSt->hdr.left,          \
                        pSt->hdr.top,           \
                        pSt->hdr.right,         \
                        pSt->hdr.bottom);
#endif
                    return (GFX_STATUS_SUCCESS);
                }
    
                if(GFX_GOL_ObjectStateGet(pSt, GFX_GOL_STATICTEXT_DRAW_STATE))
                {
                    // show frame if specified to be shown
                    GFX_LineStyleSet(GFX_LINE_STYLE_THIN_SOLID);
    
                    if(GFX_GOL_ObjectStateGet(pSt, GFX_GOL_STATICTEXT_FRAME_STATE))
                    {
                        adjustBackground = 1;
                        // set the line style to solid thin line
                        GFX_LineStyleSet(GFX_LINE_STYLE_THIN_SOLID);

                        if(!GFX_GOL_ObjectStateGet(pSt, GFX_GOL_STATICTEXT_DISABLED_STATE))
                        {    
                            // show enabled color
                            GFX_ColorSet(pSt->hdr.pGolScheme->Color1);
                            if(GFX_RectangleDraw(
                                pSt->hdr.left,
                                pSt->hdr.top,
                                pSt->hdr.right,
                                pSt->hdr.bottom) != GFX_STATUS_SUCCESS)
                            {
                                return (GFX_STATUS_FAILURE);
                            }
                        }
                        else
                        {
                            // show disabled color
                            GFX_ColorSet(pSt->hdr.pGolScheme->ColorDisabled);
                            if(GFX_RectangleDraw(
                                pSt->hdr.left,
                                pSt->hdr.top,
                                pSt->hdr.right,
                                pSt->hdr.bottom) != GFX_STATUS_SUCCESS)
                            {
                                return (GFX_STATUS_FAILURE);
                            }
                        }
                    }
                    else
                    {
                        adjustBackground = 0;
                    }
                }

                // check if there is no background 
                if(GFX_GOL_ObjectStateGet(pSt, GFX_GOL_STATICTEXT_NOBACKGROUNDCOLORFILL_STATE))
                {
                    state = DRAW_STATIC_TEXT_INIT;
                    break;
                }
                else
                {
                    state = DRAW_STATIC_TEXT_CLEANAREA;
                }
    
            case DRAW_STATIC_TEXT_CLEANAREA:

                /* The refreshing of the static area is dependent on the
                   features set.

                 Case 1:
                    When fill style is GFX_FILL_STYLE_NONE, text cannot be
                    changed and refreshed. If text changes then use one
                    of the following fill styles:
                        GFX_FILL_STYLE_COLOR
                        GFX_FILL_STYLE_ALPHA_COLOR
                        GFX_FILL_STYLE_GRADIENT_DOWN,
                        GFX_FILL_STYLE_GRADIENT_UP,
                        GFX_FILL_STYLE_GRADIENT_RIGHT,
                        GFX_FILL_STYLE_GRADIENT_LEFT,
                        GFX_FILL_STYLE_GRADIENT_DOUBLE_VER,
                        GFX_FILL_STYLE_GRADIENT_DOUBLE_HOR,
                        or
                        use (or enable) the common background image
                        if available in the application (see case 2).
                 Case 2:
                    When common background image is set, then
                    GFX_FILL_STYLE_NONE can be used. Redrawing the
                    static text will redraw the background which in turn
                    refreshes the text. Therefore, in this case the text
                    can be modified.
                 Case 3:
                    Alpha blending cannot be used with gradient fills.
                 Therefore, do use gradient fills when alpha blending of
                 panels are enabled.

                 */

                // check if fill style is GFX_FILL_STYLE_NONE and common
                // background is present
                if ((pSt->hdr.pGolScheme->pCommonBkImage != NULL) &&
                    (pSt->hdr.pGolScheme->fillStyle == GFX_FILL_STYLE_NONE))
                {
#ifdef GFX_CONFIG_ALPHABLEND_DISABLE
#ifndef GFX_CONFIG_TRANSPARENT_COLOR_DISABLE
                    if (GFX_TransparentColorStatusGet() == GFX_FEATURE_ENABLED)
                    {
                        // disable the transparent color since we want the
                        // background to refresh with all pixels being drawn
                        GFX_TransparentColorDisable();
                        transparentColorStatus = 1;
                        transparentColorSet = GFX_TransparentColorGet();
                    }
#endif
#endif
                    if (GFX_ImagePartialDraw(
                            pSt->hdr.left + adjustBackground, pSt->hdr.top + adjustBackground,
                            pSt->hdr.left - pSt->hdr.pGolScheme->CommonBkLeft + adjustBackground,
                            pSt->hdr.top - pSt->hdr.pGolScheme->CommonBkTop + adjustBackground,
                            pSt->hdr.right - pSt->hdr.left + 1 - (adjustBackground << 1),
                            pSt->hdr.bottom - pSt->hdr.top + 1 - (adjustBackground << 1),
                            pSt->hdr.pGolScheme->pCommonBkImage
                        ) != GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }

#ifdef GFX_CONFIG_ALPHABLEND_DISABLE
#ifndef GFX_CONFIG_TRANSPARENT_COLOR_DISABLE
                    // after rendering the background, we need to restore the
                    // transparent color if previously enabled.
                    if (transparentColorStatus)
                    {
                        GFX_TransparentColorEnable(transparentColorSet);
                    }
#endif
#endif
                }
                else
                {

                    if (pSt->hdr.pGolScheme->fillStyle != GFX_FILL_STYLE_NONE)
                    {
                        // clean area where text will be placed.
                        GFX_ColorSet(pSt->hdr.pGolScheme->CommonBkColor);
                        GFX_GOL_ObjectBackGroundSet(&pSt->hdr);

                        if(GFX_RectangleFillDraw(
                            pSt->hdr.left + 1,
                            pSt->hdr.top + 1,
                            pSt->hdr.right - 1,
                            pSt->hdr.bottom - 1) != GFX_STATUS_SUCCESS)
                        {
                            return (GFX_STATUS_FAILURE);
                        }
                    }
                }
                state = DRAW_STATIC_TEXT_INIT;
    
            case DRAW_STATIC_TEXT_INIT:
                if (GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
                    return (GFX_STATUS_FAILURE);
    
                // set the text color
                if(!GFX_GOL_ObjectStateGet(pSt, GFX_GOL_STATICTEXT_DISABLED_STATE))
                {
                    GFX_ColorSet(pSt->hdr.pGolScheme->TextColor0);
                }
                else
                {
                    GFX_ColorSet(pSt->hdr.pGolScheme->TextColorDisabled);
                }
    
                // use the font specified in the object
                GFX_FontSet(pSt->hdr.pGolScheme->pFont);
                state = DRAW_STATIC_TEXT_DRAWTEXT;

            case DRAW_STATIC_TEXT_DRAWTEXT:

                if (GFX_TextStringBoxDraw(                      \
                        pSt->hdr.left + 1,                      \
                        pSt->hdr.top + 1,                       \
                        pSt->hdr.right  - pSt->hdr.left - 1,    \
                        pSt->hdr.bottom - pSt->hdr.top  - 1,    \
                        pSt->pText,                             \
                        0,                                      \
                        pSt->alignment)  != GFX_STATUS_SUCCESS)
                    return (GFX_STATUS_FAILURE);

                state = DRAW_STATIC_TEXT_IDLE;    // go back to IDLE state
#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                GFX_DRIVER_CompleteDrawUpdate(  \
                        pSt->hdr.left,          \
                        pSt->hdr.top,           \
                        pSt->hdr.right,         \
                        pSt->hdr.bottom);
#endif
                return (GFX_STATUS_SUCCESS);

        } // end of switch()
    } // end of while(1)    
}

