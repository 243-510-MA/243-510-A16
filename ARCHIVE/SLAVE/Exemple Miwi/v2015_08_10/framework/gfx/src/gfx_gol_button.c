/*******************************************************************************
 Module for Microchip Graphics Library - Graphic Object Layer

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_gol_button.c

  Summary:
    This implements the GFX_GOL_BUTTON object of the GOL.

  Description:
    Refer to Microchip Graphics Library for complete documentation of the
    GFX_GOL_BUTTON Object.
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
#include "gfx/gfx_gol_button.h"
#include "gfx/gfx_gol_scan_codes.h"

// *****************************************************************************
/*  Function:
    GFX_GOL_BUTTON  *GFX_GOL_ButtonCreate(
                                uint16_t            ID,
                                uint16_t            left,
                                uint16_t            top,
                                uint16_t            right,
                                uint16_t            bottom,
                                uint16_t            radius,
                                uint16_t            state,
                                GFX_RESOURCE_HDR    *pPressImage,
                                GFX_RESOURCE_HDR    *pReleaseImage,
                                GFX_XCHAR           *pText,
                                GFX_TEXT_ALIGNMENT  alignment,
                                GFX_GOL_OBJ_SCHEME  *pScheme)
    Summary:
        This function creates a GFX_GOL_BUTTON object with the parameters
        given. It automatically attaches the new object into a global
        linked list of objects and returns the address of the object.


    Description:
        This function creates a GFX_GOL_BUTTON object with the parameters
        given. It automatically attaches the new object into a global
        linked list of objects and returns the address of the object.

        This function returns the pointer to the newly created object.
        If the object is not successfully created, it returns NULL.

        The object allows setting two images. One for the pressed state and
        the other for the release state. If no image is to be used for the
        object set both pointers to NULL.

        If only one image is used for both pressed and released state,
        set both pPressImage and pReleaseImage to the same image.

        The behavior of GFX_GOL_ButtonCreate() will be undefined if one
        of the following is true:
        - left >= right
        - top >= bottom
        - pScheme is not pointing to a GFX_GOL_OBJ_SCHEME
        - pPressImage and pReleaseImage is not pointing to a GFX_RESOURCE_HDR.
        - pText is an unterminated string

*/
// *****************************************************************************
GFX_GOL_BUTTON *GFX_GOL_ButtonCreate
(
    uint16_t            ID,
    uint16_t            left,
    uint16_t            top,
    uint16_t            right,
    uint16_t            bottom,
    uint16_t            radius,
    uint16_t            state,
    GFX_RESOURCE_HDR    *pPressImage,
    GFX_RESOURCE_HDR    *pReleaseImage,
    GFX_XCHAR           *pText,
    GFX_ALIGNMENT       alignment,
    GFX_GOL_OBJ_SCHEME  *pScheme
)
{
    GFX_GOL_BUTTON  *pB = NULL;
    pB = (GFX_GOL_BUTTON *)GFX_malloc(sizeof(GFX_GOL_BUTTON));
    if(pB == NULL)
        return (NULL);

    pB->hdr.ID = ID;                        // unique id assigned for referencing
    pB->hdr.pNxtObj = NULL;                 // initialize pointer to NULL
    pB->hdr.type = GFX_GOL_BUTTON_TYPE;     // set object type
    pB->hdr.left = left;                    // left position
    pB->hdr.top = top;                      // top position
    pB->hdr.right = right;                  // right position
    pB->hdr.bottom = bottom;                // bottom position
    pB->radius = radius;                    // radius
    pB->pPressImage = pPressImage;          // location of pressed state image
    pB->pReleaseImage = pReleaseImage;      // location of released state image
    pB->hdr.state = state;                  // state
    pB->hdr.DrawObj = GFX_GOL_ButtonDraw;   // draw function
    pB->hdr.FreeObj = NULL;  		    // free function
    pB->hdr.actionGet = GFX_GOL_ButtonActionGet; // action get function
    pB->hdr.actionSet = GFX_GOL_ButtonActionSet; // default action function

    // Set the style scheme to be used
    pB->hdr.pGolScheme = (GFX_GOL_OBJ_SCHEME *)pScheme;

    pB->textWidth = 0;
    pB->textHeight = 0;

    // set the text if it exists
    if(pText != NULL)
    {
        GFX_GOL_ButtonTextSet(pB, pText);
        GFX_GOL_ButtonTextAlignmentSet(pB, alignment);
    }
    else
    {
         pB->pText = NULL;
    }

    GFX_GOL_ObjectAdd((GFX_GOL_OBJ_HEADER *)pB);

#ifndef GFX_CONFIG_FOCUS_DISABLE
    // focus and alpha blend cannot be used together
#ifdef GFX_CONFIG_ALPHABLEND_DISABLE
    if( GFX_GOL_ObjectStateGet(pB, GFX_GOL_BUTTON_FOCUSED_STATE) ==
        GFX_GOL_BUTTON_FOCUSED_STATE)
    {
        GFX_GOL_ObjectFocusSet((GFX_GOL_OBJ_HEADER *)pB);
    }
#endif
#endif
    return (pB);
}

// *****************************************************************************
/*  Function:
    GFX_XCHAR *GFX_GOL_ButtonTextSet(
                                GFX_GOL_BUTTON *pObject,
                                GFX_XCHAR *pText)

    Summary:
        This function sets the address of the current text
        string used by the object.

    Description:
        This function sets the address of the current text
        string used by the object.

*/
// *****************************************************************************
void GFX_GOL_ButtonTextSet(GFX_GOL_BUTTON *pObject, GFX_XCHAR *pText)
{

    if (pObject->hdr.pGolScheme->pFont != NULL)
    {
        pObject->pText = pText;
        pObject->textWidth = GFX_TextStringWidthGet(
                                    pText,
                                    pObject->hdr.pGolScheme->pFont);

        pObject->textHeight = GFX_TextStringHeightGet(
                                    pObject->hdr.pGolScheme->pFont);
    }
    else
    {
        pObject->pText = NULL;
        pObject->textWidth = 0;
        pObject->textHeight = 0;
    }

}

// *****************************************************************************
/*  Function:
    void GFX_GOL_ButtonTextAlignmentSet(
                                GFX_GOL_BUTTON *pObject,
                                GFX_ALIGNMENT align)

    Summary:
        This function sets the text alignment of the text
        string used by the object.

    Description:
        This function sets the text alignment of the text
        string used by the object.

*/
// *****************************************************************************
void __attribute__ ((always_inline)) GFX_GOL_ButtonTextAlignmentSet(
                                GFX_GOL_BUTTON *pObject,
                                GFX_ALIGNMENT align)
{
    pObject->alignment = align;
}

// *****************************************************************************
/*  Function:
    GFX_ALIGNMENT GFX_GOL_ButtonTextAlignmentGet(
                                GFX_GOL_BUTTON *pObject)

    Summary:
        This function returns the text alignment of the text
        string used by the object.

    Description:
        This function returns the text alignment of the text
        string used by the object.

*/
// *****************************************************************************
GFX_ALIGNMENT __attribute__ ((always_inline)) GFX_GOL_ButtonTextAlignmentGet(
                                GFX_GOL_BUTTON *pObject)
{
    return (pObject->alignment);
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_ButtonActionSet(
                                GFX_GOL_TRANSLATED_ACTION translatedMsg,
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage)

    Summary:
        This function performs the state change of the object based on the
        translated action.

    Description:
        This function performs the state change of the object based on the
        translated action. This change can be overridden by the application
        by on the GFX_GOL_ObjectMessageCallback(). When the user message
        is determined to affect the object, application can perform the
        state change and return 0 on the GFX_GOL_ObjectMessageCallback().
        	
*/
// *****************************************************************************
void GFX_GOL_ButtonActionSet(
                                GFX_GOL_TRANSLATED_ACTION translatedMsg,
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage)
{

    GFX_GOL_BUTTON *pB;

    pB = (GFX_GOL_BUTTON *)pObject;

#ifndef GFX_CONFIG_FOCUS_DISABLE
#ifdef GFX_CONFIG_ALPHABLEND_DISABLE
#ifndef GFX_CONFIG_USE_TOUCHSCREEN_DISABLE
    if(pMessage->type == TYPE_TOUCHSCREEN)
    {
        if( GFX_GOL_ObjectStateGet(pB, GFX_GOL_BUTTON_FOCUSED_STATE) !=
            GFX_GOL_BUTTON_FOCUSED_STATE)
        {
            GFX_GOL_ObjectFocusSet((GFX_GOL_OBJ_HEADER *)pB);
        }
    }

#endif //#ifdef GFX_CONFIG_USE_TOUCHSCREEN_DISABLE
#endif //#ifdef GFX_CONFIG_ALPHABLEND_DISABLE
#endif //#ifndef GFX_CONFIG_FOCUS_DISABLE

    switch(translatedMsg)
    {
        case GFX_GOL_BUTTON_ACTION_PRESSED:
            // set pressed and redraw
            GFX_GOL_ObjectStateSet(pB, GFX_GOL_BUTTON_PRESSED_STATE | 
                                       GFX_GOL_BUTTON_DRAW_STATE);
            break;

        case GFX_GOL_BUTTON_ACTION_RELEASED:
        case GFX_GOL_BUTTON_ACTION_CANCELPRESS:
            // reset pressed
            GFX_GOL_ObjectStateClear(pB, GFX_GOL_BUTTON_PRESSED_STATE);
            // redraw
            GFX_GOL_ObjectStateSet(pB, GFX_GOL_BUTTON_DRAW_STATE);
            break;

        default:

            // catch all for GFX_GOL_BUTTON messages added by users and
            // behavior defined by users in message callback
            break;
    }
}

// *****************************************************************************
/*  Function:
    GFX_GOL_TRANSLATED_ACTION GFX_GOL_ButtonActionGet(
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage);

    Summary:
        This function evaluates the message from a user if the
        message will affect the object or not.

    Description:
        This function evaluates the message from a user if the
        message will affect the object or not. 	
*/
// *****************************************************************************
GFX_GOL_TRANSLATED_ACTION GFX_GOL_ButtonActionGet(void *pObject, GFX_GOL_MESSAGE *pMessage)
{
    GFX_GOL_BUTTON *pB;

    pB = (GFX_GOL_BUTTON *)pObject;


    // Evaluate if the message is for the GFX_GOL_BUTTON
    // Check if disabled first
    if(GFX_GOL_ObjectStateGet(pB, GFX_GOL_BUTTON_DISABLED_STATE))
        return (GFX_GOL_OBJECT_ACTION_INVALID);

#ifndef GFX_CONFIG_USE_TOUCHSCREEN_DISABLE
    if(pMessage->type == TYPE_TOUCHSCREEN)
    {

        // Check if it falls in the GFX_GOL_BUTTON's face
        if
        (
            (pB->hdr.left   < pMessage->param1) &&
            (pB->hdr.right  > pMessage->param1) &&
            (pB->hdr.top    < pMessage->param2) &&
            (pB->hdr.bottom > pMessage->param2)
        )
        {
            if(GFX_GOL_ObjectStateGet(pB, GFX_GOL_BUTTON_TOGGLE_STATE))
            {
                if(pMessage->uiEvent == EVENT_RELEASE)
                {
                    if(GFX_GOL_ObjectStateGet(pB, GFX_GOL_BUTTON_PRESSED_STATE))
                        return (GFX_GOL_BUTTON_ACTION_RELEASED);
                    else
                        return (GFX_GOL_BUTTON_ACTION_PRESSED);
                }
            }
            else
            {
                if(pMessage->uiEvent == EVENT_RELEASE)
                    return (GFX_GOL_BUTTON_ACTION_RELEASED);
                if(pMessage->uiEvent == EVENT_STILLPRESS)
                {
                    if(GFX_GOL_ObjectStateGet(pB, GFX_GOL_BUTTON_PRESSED_STATE))
                        return (GFX_GOL_BUTTON_ACTION_STILLPRESSED);
                }

                if(!GFX_GOL_ObjectStateGet(pB, GFX_GOL_BUTTON_PRESSED_STATE))
                    return (GFX_GOL_BUTTON_ACTION_PRESSED);
            }
        }
        else
        {
            if(!GFX_GOL_ObjectStateGet(pB, GFX_GOL_BUTTON_TOGGLE_STATE))
            {
                if( (pMessage->uiEvent == EVENT_MOVE) &&
                    (GFX_GOL_ObjectStateGet(pB, GFX_GOL_BUTTON_PRESSED_STATE)))
                {
                    return (GFX_GOL_BUTTON_ACTION_CANCELPRESS);
                }
            }
        }

        return (GFX_GOL_OBJECT_ACTION_INVALID);
    }

#endif
#ifndef GFX_CONFIG_USE_KEYBOARD_DISABLE
    if(pMessage->type == TYPE_KEYBOARD)
    {
        if((uint16_t)pMessage->param1 == pB->hdr.ID)
        {
            if(pMessage->uiEvent == EVENT_KEYSCAN)
            {
                if(GFX_GOL_ObjectStateGet(pB, GFX_GOL_BUTTON_TOGGLE_STATE))
                {
                    if( (pMessage->param2 == SCAN_SPACE_RELEASED) ||
                        (pMessage->param2 == SCAN_CR_RELEASED))
                    {
                        if(GFX_GOL_ObjectStateGet(pB, GFX_GOL_BUTTON_PRESSED_STATE))
                            return (GFX_GOL_BUTTON_ACTION_RELEASED);
                        else
                            return (GFX_GOL_BUTTON_ACTION_PRESSED);
                    }
                }
                else
                {
                    if( (pMessage->param2 == SCAN_SPACE_PRESSED) ||
                        (pMessage->param2 == SCAN_CR_PRESSED))
                    {
                        return (GFX_GOL_BUTTON_ACTION_PRESSED);
                    }

                    if( (pMessage->param2 == SCAN_SPACE_RELEASED) ||
                        (pMessage->param2 == SCAN_CR_RELEASED))
                    {
                        return (GFX_GOL_BUTTON_ACTION_RELEASED);
                    }
                }

                if( (pMessage->param2 == SCAN_SPACE_PRESSED) ||
                    (pMessage->param2 == SCAN_CRA_PRESSED))
                {
                    return (GFX_GOL_BUTTON_ACTION_PRESSED);
                }

                if( (pMessage->param2 == SCAN_SPACE_RELEASED) ||
                    (pMessage->param2 == SCAN_CRA_RELEASED))
                {
                    return (GFX_GOL_BUTTON_ACTION_RELEASED);
                }
            }
        }

        return (GFX_GOL_OBJECT_ACTION_INVALID);
    }

#endif
    return (GFX_GOL_OBJECT_ACTION_INVALID);
}

/*********************************************************************
* GFX_GOL_BUTTON draw states
********************************************************************/
typedef enum
{
    DRAW_BUTTON_START,
    DRAW_BUTTON_BACKGROUND,
    DRAW_BUTTON_ROUND,
    DRAW_BUTTON_TEXT,
    DRAW_BUTTON_TEXT_RUN,
    DRAW_BUTTON_FOCUS, 
} DRAW_BUTTON_STATES;

/*********************************************************************
* Function: inline uint16_t __attribute__((always_inline)) DrawButtonFocus(GFX_GOL_BUTTON *GFX_GOL_BUTTON, int16_t radius, BTN_DRAW_STATES *current_state)
********************************************************************/
inline GFX_STATUS __attribute__((always_inline)) GFX_GOL_ButtonFocusDraw(
                    GFX_GOL_BUTTON *GFX_GOL_BUTTON,
                    int16_t radius,
                    DRAW_BUTTON_STATES *current_state)
{

    *current_state = DRAW_BUTTON_FOCUS;

#ifndef GFX_CONFIG_NONBLOCKING_DISABLE
    if (GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
        return (GFX_STATUS_FAILURE);
#endif

    if(GFX_GOL_ObjectStateGet(GFX_GOL_BUTTON, GFX_GOL_BUTTON_FOCUSED_STATE))
    {
        GFX_LineStyleSet(GFX_LINE_STYLE_THIN_DASHED);
        if(GFX_GOL_ObjectStateGet(GFX_GOL_BUTTON, GFX_GOL_BUTTON_PRESSED_STATE))
        {
            GFX_ColorSet(GFX_GOL_BUTTON->hdr.pGolScheme->TextColor1);
        }
        else
        {
            GFX_ColorSet(GFX_GOL_BUTTON->hdr.pGolScheme->TextColor0);
        }

        // check if the object has rounded corners or not
        if(!GFX_GOL_BUTTON->radius)
        {
            if(GFX_RectangleDraw(
                        GFX_GOL_BUTTON->hdr.left +
                            GFX_GOL_BUTTON->hdr.pGolScheme->EmbossSize + 2,
                        GFX_GOL_BUTTON->hdr.top +
                            GFX_GOL_BUTTON->hdr.pGolScheme->EmbossSize + 2,
                        GFX_GOL_BUTTON->hdr.right -
                            GFX_GOL_BUTTON->hdr.pGolScheme->EmbossSize - 2,
                        GFX_GOL_BUTTON->hdr.bottom -
                            GFX_GOL_BUTTON->hdr.pGolScheme->EmbossSize - 2)
                    != GFX_STATUS_SUCCESS)
            {
                return (GFX_STATUS_FAILURE);
            }
        }
        else
        {

            // original center is still the same, but radius is reduced
            if(GFX_RectangleRoundDraw(
                        GFX_GOL_BUTTON->hdr.left   + radius,
                        GFX_GOL_BUTTON->hdr.top    + radius,
                        GFX_GOL_BUTTON->hdr.right  - radius,
                        GFX_GOL_BUTTON->hdr.bottom - radius,
                        radius - 2 -
                            GFX_GOL_BUTTON->hdr.pGolScheme->EmbossSize)
                    != GFX_STATUS_SUCCESS)
            {
                return (GFX_STATUS_FAILURE);
            }
        }

        GFX_LineStyleSet(GFX_LINE_STYLE_THIN_SOLID);
    }

    *current_state = DRAW_BUTTON_START;
    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_GOL_ButtonDraw(void *pObject)

    Summary:
        This function renders the object on the screen based on the
        current state of the object.

    Description:
        This function renders the object on the screen based on the
        current state of the object. Location of the object is
        determined by the left, top, right and bottom parameters.
        The colors used are dependent on the state of the object.
        The font used is determined by the style scheme set.

        The text on the face of the GFX_GOL_BUTTON is drawn on top of
        the bitmap. Text alignment based on the alignment parameter
        set on the object.

        When rendering objects of the same type, each object
        must be rendered completely before the rendering of the
        next object is started. This is to avoid incomplete
        object rendering.

        Normally, application will just call GFX_GOL_ObjectListDraw()
        to allow the Graphics Library to manage all object rendering.
        See GFX_GOL_ObjectListDraw() for more information on object rendering.

*/
// *****************************************************************************
GFX_STATUS GFX_GOL_ButtonDraw(void *pObject)
{

    static DRAW_BUTTON_STATES   state = DRAW_BUTTON_START;
    static uint16_t             width, height, radius;
    static GFX_COLOR            faceClr;
    static GFX_RESOURCE_HDR     *pImage;
           GFX_COLOR            embossLtClr, embossDkClr;
           GFX_GOL_BUTTON       *pB;

#ifdef GFX_CONFIG_ALPHABLEND_DISABLE
#ifndef GFX_CONFIG_TRANSPARENT_COLOR_DISABLE
    uint16_t                transparentColorStatus;
    GFX_COLOR               transparentColorSet = 0;
#endif
#endif

    pB = (GFX_GOL_BUTTON *)pObject;

    if (GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
        return (GFX_STATUS_FAILURE);

    switch(state)
    {
        case DRAW_BUTTON_START:
            
            // initialize the image pointer 
            pImage = NULL;

            if (GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
                return (GFX_STATUS_FAILURE);

#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
            GFX_DRIVER_SetupDrawUpdate( pB->hdr.left,       
                                        pB->hdr.top,        
                                        pB->hdr.right,      
                                        pB->hdr.bottom);    
#endif
            if(GFX_GOL_ObjectStateGet(pB, GFX_GOL_BUTTON_HIDE_STATE))
            {                       
                // Hide the GFX_GOL_BUTTON (remove from screen)
                if (GFX_GOL_ObjectHideDraw(&(pB->hdr)) != GFX_STATUS_SUCCESS)
                    return (GFX_STATUS_FAILURE);

#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                GFX_DRIVER_CompleteDrawUpdate(  pB->hdr.left,
                                                pB->hdr.top,
                                                pB->hdr.right,
                                                pB->hdr.bottom);
#endif
                return (GFX_STATUS_SUCCESS);
            }

            /* Note: The width and height adjustment considers the
                     following assumptions:
                     1. if circular width = height = radius*2
                     2. if vertical capsule width = radius*2
                     3. if horizontal capsule height = radius*2
                     4. radius must be less than or equal to width if
                        height is greater than width
                     5. radius must be less than or equal to height if
                        width is greater than height
                     6. if GFX_GOL_BUTTON is cornered, radius must be zero
            */
            // get radius
            radius = pB->radius;
            // get width
            width = (pB->hdr.right - pB->hdr.left) - (radius * 2);
            // get height
            height = (pB->hdr.bottom - pB->hdr.top) - (radius * 2);

            if(!GFX_GOL_ObjectStateGet(pB, GFX_GOL_BUTTON_NOPANEL_STATE))
            {
                if(!GFX_GOL_ObjectStateGet(pB, GFX_GOL_BUTTON_DISABLED_STATE))
                {
                    if(GFX_GOL_ObjectStateGet(pB, GFX_GOL_BUTTON_PRESSED_STATE))
                    {
                        embossDkClr = pB->hdr.pGolScheme->EmbossLtColor;
                        embossLtClr = pB->hdr.pGolScheme->EmbossDkColor;
                        faceClr = pB->hdr.pGolScheme->Color1;
                        pImage = pB->pPressImage;
                    }
                    else
                    {
                        embossLtClr = pB->hdr.pGolScheme->EmbossLtColor;
                        embossDkClr = pB->hdr.pGolScheme->EmbossDkColor;
                        faceClr = pB->hdr.pGolScheme->Color0;
                        pImage = pB->pReleaseImage;
                    }
                }
                else
                {
                    embossLtClr = pB->hdr.pGolScheme->EmbossLtColor;
                    embossDkClr = pB->hdr.pGolScheme->EmbossDkColor;
                    faceClr = pB->hdr.pGolScheme->ColorDisabled;
                    pImage = pB->pReleaseImage;
                }
                GFX_LineStyleSet(GFX_LINE_STYLE_THIN_SOLID);

                // set up the background
                GFX_GOL_PanelBackgroundSet(&pB->hdr);

                // set panel parameters
                GFX_GOL_PanelParameterSet
                (
                    pB->hdr.left + radius,
                    pB->hdr.top + radius,
                    pB->hdr.right - radius,
                    pB->hdr.bottom - radius,
                    radius,
                    faceClr,
                    embossLtClr,
                    embossDkClr,
                    pImage,
                    pB->hdr.pGolScheme->fillStyle,
                    pB->hdr.pGolScheme->EmbossSize
                );
            }
            state = DRAW_BUTTON_BACKGROUND;

        case DRAW_BUTTON_BACKGROUND:

#ifdef GFX_CONFIG_ALPHABLEND_DISABLE

            // check if there is a background image to be drawn
            if (pB->hdr.pGolScheme->pCommonBkImage != NULL)
            {
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
                if (GFX_ImagePartialDraw(
                        pB->hdr.left, pB->hdr.top,
                        pB->hdr.left - pB->hdr.pGolScheme->CommonBkLeft,
                        pB->hdr.top - pB->hdr.pGolScheme->CommonBkTop,
                        pB->hdr.right - pB->hdr.left + 1,
                        pB->hdr.bottom - pB->hdr.top + 1,
                        pB->hdr.pGolScheme->pCommonBkImage
                    ) != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }

#ifndef GFX_CONFIG_TRANSPARENT_COLOR_DISABLE
                // after rendering the background, we need to restore the 
                // transparent color if previously enabled.
                if (transparentColorStatus)
                {
                    GFX_TransparentColorEnable(transparentColorSet);
                }
#endif
            }
#endif
            // set the image to be used (depending if the object is in 
            // pressed or released state
            if(!GFX_GOL_ObjectStateGet(pB, GFX_GOL_BUTTON_DISABLED_STATE))
            {
                if(GFX_GOL_ObjectStateGet(pB, GFX_GOL_BUTTON_PRESSED_STATE))
                {
                    pImage = pB->pPressImage;
                }
                else
                {
                    pImage = pB->pReleaseImage;
                }
            }
            else
            {
                pImage = pB->pReleaseImage;
            }

            state = DRAW_BUTTON_ROUND;

        case DRAW_BUTTON_ROUND:

            if (GFX_GOL_ObjectStateGet(pB, GFX_GOL_BUTTON_NOPANEL_STATE))
            {
	        // check if there is an image to be drawn
	        if (pImage != NULL)
	        {
                    if(GFX_ImageDraw(
                        ((pB->hdr.right + pB->hdr.left -
                            GFX_ImageWidthGet((void *)pImage))  >> 1) + 1,
                        ((pB->hdr.top + pB->hdr.bottom -
                            GFX_ImageHeightGet((void *)pImage)) >> 1) + 1,
                        pImage) != GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }
          	}  
            }
            else
            {
               	if (GFX_GOL_ObjectStateGet(pB, GFX_GOL_BUTTON_TWOTONE_STATE))
	        {
                    if(GFX_GOL_TwoTonePanelDraw() != GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }
	        }	
	        else
	        {
                    if(GFX_GOL_PanelDraw() != GFX_STATUS_SUCCESS)
		    {
                        return (GFX_STATUS_FAILURE);
                    }
                }

            }
            state = DRAW_BUTTON_TEXT;

        case DRAW_BUTTON_TEXT:
            if(pB->pText != NULL)
            {
                if(!GFX_GOL_ObjectStateGet(pB, GFX_GOL_BUTTON_DISABLED_STATE))
                {
                    if(GFX_GOL_ObjectStateGet(pB, GFX_GOL_BUTTON_PRESSED_STATE))
                    {
                        GFX_ColorSet(pB->hdr.pGolScheme->TextColor1);
                    }
                    else
                    {
                        GFX_ColorSet(pB->hdr.pGolScheme->TextColor0);
                    }
                }
                else
                {
                    GFX_ColorSet(pB->hdr.pGolScheme->TextColorDisabled);
                }

                if (pB->hdr.pGolScheme->pFont != NULL)
                {
                    GFX_FontSet(pB->hdr.pGolScheme->pFont);
                    state = DRAW_BUTTON_TEXT_RUN;
                }
                else
                {
                    state = DRAW_BUTTON_FOCUS;
                    break;
                }
            }
            else
            {
                if(GFX_GOL_ButtonFocusDraw(pB, radius, &state) == GFX_STATUS_SUCCESS)
                {
#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                    GFX_DRIVER_CompleteDrawUpdate(  pB->hdr.left,
                                                    pB->hdr.top,
                                                    pB->hdr.right,
                                                    pB->hdr.bottom);
#endif
                    return (GFX_STATUS_SUCCESS);
                }
                return (GFX_STATUS_FAILURE);
            }

        case DRAW_BUTTON_TEXT_RUN:

            if (GFX_TextStringBoxDraw(                  
                    pB->hdr.left   + pB->hdr.pGolScheme->EmbossSize,    
                    pB->hdr.top    + pB->hdr.pGolScheme->EmbossSize,    
                    pB->hdr.right  - pB->hdr.left + 1 -                 
                    (pB->hdr.pGolScheme->EmbossSize << 1),              
                    pB->hdr.bottom - pB->hdr.top + 1 -                  
                    (pB->hdr.pGolScheme->EmbossSize << 1),              
                    pB->pText,
                    0,
                    pB->alignment) != GFX_STATUS_SUCCESS)
                return (GFX_STATUS_FAILURE);

            state = DRAW_BUTTON_FOCUS;

        case DRAW_BUTTON_FOCUS:
            if(GFX_GOL_ButtonFocusDraw(pB, radius, &state) == GFX_STATUS_SUCCESS)
            {
#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                GFX_DRIVER_CompleteDrawUpdate(  pB->hdr.left,
                                                pB->hdr.top,
                                                pB->hdr.right,
                                                pB->hdr.bottom);
#endif
                return (GFX_STATUS_SUCCESS);
            }
            return (GFX_STATUS_FAILURE);
    }

#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
    GFX_DRIVER_CompleteDrawUpdate(  pB->hdr.left,
                                    pB->hdr.top,
                                    pB->hdr.right,
                                    pB->hdr.bottom);
#endif
    return (GFX_STATUS_SUCCESS);
}

