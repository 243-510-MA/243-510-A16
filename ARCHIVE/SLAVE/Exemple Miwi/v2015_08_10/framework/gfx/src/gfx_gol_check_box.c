/*******************************************************************************
 Module for Microchip Graphics Library - Graphic Object Layer

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_gol_check_box.c

  Summary:
    This implements the check box object of the GOL.

  Description:
    Refer to Microchip Graphics Library for complete documentation of the
    CheckBox Object.
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
#include "gfx/gfx.h"
#include "gfx/gfx_gol_check_box.h"
#include "gfx/gfx_gol_scan_codes.h"


// This is indent from outside borders
#define CB_INDENT   2

// *****************************************************************************
/*  Function:
    GFX_GOL_CHECKBOX  *GFX_GOL_CheckBoxCreate(
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
        This function creates a GFX_GOL_CHECKBOX object with the parameters
        given. It automatically attaches the new object into a global
        linked list of objects and returns the address of the object.

    Description:
        This function creates a GFX_GOL_CHECKBOX object with the parameters
        given. It automatically attaches the new object into a global
        linked list of objects and returns the address of the object.

        This function returns the pointer to the newly created object.
        If the object is not successfully created, it returns NULL.

        The behavior of GFX_GOL_CheckBoxCreate() will be undefined if one
        of the following is true:
        - left >= right
        - top >= bottom
        - pScheme is not pointing to a GFX_GOL_OBJ_SCHEME
        - pText is an unterminated string

    Precondition:
        None.

    Parameters:
        ID - Unique user defined ID for the object instance.
        left - Left most position of the object.
        top - Top most position of the object.
        right - Right most position of the object.
        bottom - Bottom most position of the object.
        state - Sets the initial state of the object.
        pText - Pointer to the text of the object.
        alignment - text alignment of the text used in the object.
        pScheme - Pointer to the style scheme used.

    Returns:
        Pointer to the newly created object.

    Example:
        <code>
            GFX_GOL_OBJ_SCHEME  *pScheme;
            GFX_GOL_CHECKBOX    *pCb[2];

            pCb[0] = GFX_GOL_CheckBoxCreate(
                                ID_CHECKBOX1,                // ID
                                20,135,150,175,              // dimension
                                GFX_GOL_CHECKBOX_DRAW_STATE, // Draw the object
                                "Scale",                     // text
                                GFX_ALIGN_CENTER,            // text alignment
                                pScheme);                    // use this scheme

            pCb[1] = GFX_GOL_CheckBoxCreate(
                                ID_CHECKBOX2,                // ID
                                170,135,300,175,             // dimension
                                GFX_GOL_CHECKBOX_DRAW_STATE, // Draw the object
                                "Animate",                   // text
                                GFX_ALIGN_LEFT | GFX_ALIGN_VCENTER, // text alignment
                                pScheme);                    // use this scheme
        </code>

*/
// *****************************************************************************
GFX_GOL_CHECKBOX *GFX_GOL_CheckBoxCreate(
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
    GFX_GOL_CHECKBOX *pObject = NULL;

    pObject = (GFX_GOL_CHECKBOX *)GFX_malloc(sizeof(GFX_GOL_CHECKBOX));
    if(pObject == NULL)
        return (pObject);

    pObject->hdr.ID         = ID;
    pObject->hdr.pNxtObj    = NULL;
    pObject->hdr.type       = GFX_GOL_CHECKBOX_TYPE;
    pObject->hdr.left       = left;
    pObject->hdr.top        = top;
    pObject->hdr.right      = right;
    pObject->hdr.bottom     = bottom;
    pObject->pText          = pText;
    pObject->alignment      = alignment;
    pObject->hdr.state      = state;

    pObject->hdr.DrawObj    = GFX_GOL_CheckBoxDraw;      // draw function
    pObject->hdr.FreeObj    = NULL;                      // free function
    pObject->hdr.actionGet  = GFX_GOL_CheckBoxActionGet; // action get function
    pObject->hdr.actionSet  = GFX_GOL_CheckBoxActionSet; // default action function

    // Set the style scheme
    pObject->hdr.pGolScheme = pScheme;

    // Set the text height
    // Set the text and the alignment
    GFX_GOL_CheckBoxTextSet(pObject, pText);
    GFX_GOL_CheckBoxTextAlignmentSet(pObject, alignment);


    GFX_GOL_ObjectAdd((GFX_GOL_OBJ_HEADER *)pObject);

    return (pObject);
}

// *****************************************************************************
/*  Function:
    GFX_XCHAR *GFX_GOL_CheckBoxTextSet(
                                GFX_GOL_CHECKBOX *pObject,
                                GFX_XCHAR *pText)

    Summary:
        This function sets the address of the current text
        string used by the object.

    Description:
        This function sets the address of the current text
        string used by the object.

*/
// *****************************************************************************
void GFX_GOL_CheckBoxTextSet(
                                GFX_GOL_CHECKBOX *pObject,
                                GFX_XCHAR * pText)
{
   if (pText == NULL)
    {
        pObject->pText = NULL;
        pObject->textHeight = 0;
    }
    else
    {
        pObject->pText = pText;
        pObject->textHeight = GFX_TextStringHeightGet(
                                    pObject->hdr.pGolScheme->pFont);
    }
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_CheckBoxTextAlignmentSet(
                                GFX_GOL_CHECKBOX *pObject,
                                GFX_ALIGNMENT align)

    Summary:
        This function sets the text alignment of the text
        string used by the object.

    Description:
        This function sets the text alignment of the text
        string used by the object.

*/
// *****************************************************************************
void __attribute__ ((always_inline)) GFX_GOL_CheckBoxTextAlignmentSet(
                                GFX_GOL_CHECKBOX *pObject,
                                GFX_ALIGNMENT align)
{
    pObject->alignment = align;
}

// *****************************************************************************
/*  Function:
    GFX_ALIGNMENT GFX_GOL_CheckBoxTextAlignmentGet(
                                GFX_GOL_CHECKBOX *pObject)

    Summary:
        This function returns the text alignment of the text
        string used by the object.

    Description:
        This function returns the text alignment of the text
        string used by the object.

*/
// *****************************************************************************
GFX_ALIGNMENT __attribute__ ((always_inline)) GFX_GOL_CheckBoxTextAlignmentGet(
                                GFX_GOL_CHECKBOX *pObject)
{
    return (pObject->alignment);
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_CheckBoxActionSet(
                                GFX_GOL_TRANSLATED_ACTION translatedMsg,
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage)

    Summary:
        This function performs the state change of the object based on the
        translated action.

    Description:
        This function performs the state change of the object based on the
        translated action. This change can be overridden by the application
        using the application defined GFX_GOL_MESSAGE_CALLBACK_FUNC.
        When the user message is determined to affect the object, application
        can perform the state change in the message callback function.
        
*/
// *****************************************************************************
void GFX_GOL_CheckBoxActionSet(
                                GFX_GOL_TRANSLATED_ACTION translatedMsg,
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage)
{
    GFX_GOL_CHECKBOX *pCb;

    pCb = (GFX_GOL_CHECKBOX *)pObject;

#ifndef GFX_CONFIG_FOCUS_DISABLE
#ifndef GFX_CONFIG_USE_TOUCHSCREEN_DISABLE
    if(pMsg->type == TYPE_TOUCHSCREEN)
    {
        if(!GFX_GOL_ObjectStateGet(pCb, GFX_GOL_CHECKBOX_FOCUSED_STATE))
        {
            GFX_GOL_ObjectFocusSet((OBJ_HEADER *)pCb);
        }
    }

#endif
#endif

    switch(translatedMsg)
    {
        case GFX_GOL_CHECKBOX_ACTION_CHECKED:
            // Set checked and redraw
            GFX_GOL_ObjectStateSet( pCb, GFX_GOL_CHECKBOX_CHECKED_STATE |
                                    GFX_GOL_CHECKBOX_DRAW_CHECK_STATE);
            break;

        case GFX_GOL_CHECKBOX_ACTION_UNCHECKED:
            // Reset check
            GFX_GOL_ObjectStateClear(pCb, GFX_GOL_CHECKBOX_CHECKED_STATE);
            // Redraw
            GFX_GOL_ObjectStateSet(pCb, GFX_GOL_CHECKBOX_DRAW_CHECK_STATE);
            break;
        default:
            // do nothing
            return;
    }
}

// *****************************************************************************
/*  Function:
    GFX_GOL_TRANSLATED_ACTION GFX_GOL_CheckBoxActionGet(
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
GFX_GOL_TRANSLATED_ACTION GFX_GOL_CheckBoxActionGet(
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage)
{
    GFX_GOL_CHECKBOX *pCb;

    pCb = (GFX_GOL_CHECKBOX *)pObject;

    // Evaluate if the message is for the check box
    // Check if disabled first
    if(GFX_GOL_ObjectStateGet(pCb, GFX_GOL_CHECKBOX_DISABLED_STATE))
        return (GFX_GOL_OBJECT_ACTION_INVALID);


#ifndef GFX_CONFIG_USE_TOUCHSCREEN_DISABLE
    if(pMessage->type == TYPE_TOUCHSCREEN)
    {

        // Check if it falls in the check box borders
        if
        (
            (pCb->hdr.left   < pMessage->param1) &&
            (pCb->hdr.right  > pMessage->param1) &&
            (pCb->hdr.top    < pMessage->param2) &&
            (pCb->hdr.bottom > pMessage->param2)
        )
        {
            if(pMessage->uiEvent == EVENT_RELEASE)
            {
                if(GFX_GOL_ObjectStateGet(pCb, GFX_GOL_CHECKBOX_CHECKED_STATE))
                    return (GFX_GOL_CHECKBOX_ACTION_UNCHECKED);
                else
                    return (GFX_GOL_CHECKBOX_ACTION_CHECKED);
            }
        }

        return (GFX_GOL_OBJECT_ACTION_INVALID);
    }

#endif
#ifndef GFX_CONFIG_USE_KEYBOARD_DISABLE
    if(pMessage->type == TYPE_KEYBOARD)
    {
        if((uint16_t)pMessage->param1 == pCb->hdr.ID)
        {
            if(pMessage->uiEvent == EVENT_KEYSCAN)
            {
                if( (pMessage->param2 == SCAN_SPACE_PRESSED) ||
                    (pMessage->param2 == SCAN_CR_PRESSED))
                {
                    if(GFX_GOL_ObjectStateGet(
                            pCb,
                            GFX_GOL_CHECKBOX_CHECKED_STATE))
                    {
                        return (GFX_GOL_CHECKBOX_ACTION_UNCHECKED);
                    }
                    else
                    {
                        return (GFX_GOL_CHECKBOX_ACTION_CHECKED);
                    }
                }
            }
        }

        return (GFX_GOL_OBJECT_ACTION_INVALID);
    }

#endif
    return (GFX_GOL_OBJECT_ACTION_INVALID);
}


// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_GOL_CheckBoxDraw(void *pObject)

    Summary:
        This function renders the object on the screen based on the
        current state of the object.

    Description:
        This function renders the object on the screen based on the
        current state of the object. Location of the object is
        determined by the left, top, right and bottom parameters.
        The colors used are dependent on the state of the object.
        The font used is determined by the style scheme set.

        The text on the GFX_GOL_CHECKBOX is drawn with the text
        alignment based on the alignment parameter set on the object.

        When rendering objects of the same type, each object
        must be rendered completely before the rendering of the
        next object is started. This is to avoid incomplete
        object rendering.

        Normally, application will just call GFX_GOL_ObjectListDraw()
        to allow the Graphics Library to manage all object rendering.
        See GFX_GOL_ObjectListDraw() for more information on object rendering.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - Pointer to the object.

    Returns:
        GFX_STATUS_SUCCESS - When the object rendering is finished.
        GFX_STATUS_FAILURE - When the object rendering is not yet finished.
                             Application needs to call this rendering
                             function again to continue the rendering.

    Example:
        None.

*/
// *****************************************************************************
GFX_STATUS GFX_GOL_CheckBoxDraw(void *pObject)
{
    typedef enum
    {
        DRAW_CHECKBOX_START,
        DRAW_CHECKBOX_BOX_DRAW,
        DRAW_CHECKBOX_RUN_DRAW,
        DRAW_CHECKBOX_TEXT_DRAW,
        DRAW_CHECKBOX_TEXT_DRAW_RUN,
        DRAW_CHECKBOX_CHECK_SET_DRAW,
        DRAW_CHECKBOX_CHECK_DRAW,
        DRAW_CHECKBOX_FOCUS_DRAW
    } CB_DRAW_STATES;

    static CB_DRAW_STATES   state = DRAW_CHECKBOX_START;
    static int16_t          checkIndent;
    static uint16_t         width;
    GFX_GOL_CHECKBOX        *pCb;

    pCb = (GFX_GOL_CHECKBOX *)pObject;

    while(1)
    {
        if (GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
            return (GFX_STATUS_FAILURE);
    
        switch(state)
        {
            case DRAW_CHECKBOX_RUN_DRAW:

                if(GFX_GOL_PanelDraw() != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }
                state = DRAW_CHECKBOX_TEXT_DRAW;

            case DRAW_CHECKBOX_TEXT_DRAW:
                if(pCb->pText != NULL)
                {
                    GFX_FontSet(pCb->hdr.pGolScheme->pFont);
    
                    if(!GFX_GOL_ObjectStateGet(
                            pCb,
                            GFX_GOL_CHECKBOX_DISABLED_STATE))
                    {
                        GFX_ColorSet(pCb->hdr.pGolScheme->TextColor0);
                    }
                    else
                    {
                        GFX_ColorSet(pCb->hdr.pGolScheme->TextColorDisabled);
                    }
                    width = pCb->hdr.bottom - pCb->hdr.top;
                    state = DRAW_CHECKBOX_TEXT_DRAW_RUN;
                }
                else
                {
                    state = DRAW_CHECKBOX_CHECK_SET_DRAW;
                    break;
                }
    
            case DRAW_CHECKBOX_TEXT_DRAW_RUN:
                if (GFX_TextStringBoxDraw(
                        pCb->hdr.left + width + CB_INDENT,
                        pCb->hdr.top + pCb->hdr.pGolScheme->EmbossSize,
                        pCb->hdr.right -
                            pCb->hdr.left -
                            (pCb->hdr.pGolScheme->EmbossSize << 1) -
                            width,
                        width -
                            (pCb->hdr.pGolScheme->EmbossSize << 1),
                        pCb->pText,
                        0,
                        pCb->alignment) != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }

                state = DRAW_CHECKBOX_CHECK_SET_DRAW;
                break;

            case DRAW_CHECKBOX_START:
#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
	        GFX_DRIVER_SetupDrawUpdate( pCb->hdr.left,
	                                    pCb->hdr.top,
	                                    pCb->hdr.right,
	                                    pCb->hdr.bottom);
#endif
                if(GFX_GOL_ObjectStateGet(pCb, GFX_GOL_CHECKBOX_HIDE_STATE))
                {
                    if (GFX_GOL_ObjectHideDraw(&(pCb->hdr)) != GFX_STATUS_SUCCESS)
                        return (GFX_STATUS_FAILURE);

#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                    GFX_DRIVER_CompleteDrawUpdate(   pCb->hdr.left,
	                                             pCb->hdr.top,
	                                             pCb->hdr.right,
	                                             pCb->hdr.bottom);
#endif
                    return (GFX_STATUS_SUCCESS);
                }
                else if(GFX_GOL_ObjectStateGet(
                        pCb,
                        GFX_GOL_CHECKBOX_DRAW_STATE))
                {

                    GFX_ColorSet(pCb->hdr.pGolScheme->CommonBkColor);
                    GFX_GOL_ObjectBackGroundSet(&pCb->hdr);

                    if(GFX_RectangleFillDraw(
                            pCb->hdr.left,
                            pCb->hdr.top,
                            pCb->hdr.right,
                            pCb->hdr.bottom) != GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }
                }
    
                state = DRAW_CHECKBOX_BOX_DRAW;
    
            case DRAW_CHECKBOX_BOX_DRAW:

                GFX_LineStyleSet(GFX_LINE_STYLE_THIN_SOLID);

                if(GFX_GOL_ObjectStateGet(
                        pCb,
                        GFX_GOL_CHECKBOX_DRAW_STATE))
                {
                    if(GFX_GOL_ObjectStateGet(
                            pCb,
                            GFX_GOL_CHECKBOX_DISABLED_STATE))
                    {
                        GFX_GOL_PanelParameterSet(
                            pCb->hdr.left + CB_INDENT,
                            pCb->hdr.top + CB_INDENT,
                            pCb->hdr.left + \
                                (pCb->hdr.bottom - pCb->hdr.top) - CB_INDENT,
                            pCb->hdr.bottom - CB_INDENT,
                            0,
                            pCb->hdr.pGolScheme->ColorDisabled,
                            pCb->hdr.pGolScheme->EmbossDkColor,
                            pCb->hdr.pGolScheme->EmbossLtColor,
                            NULL,
                            pCb->hdr.pGolScheme->fillStyle,
                            pCb->hdr.pGolScheme->EmbossSize);
                    }
                    else
                    {
                        GFX_GOL_PanelParameterSet(
                            pCb->hdr.left + CB_INDENT,
                            pCb->hdr.top + CB_INDENT,
                            pCb->hdr.left + \
                                (pCb->hdr.bottom - pCb->hdr.top) - CB_INDENT,
                            pCb->hdr.bottom - CB_INDENT,
                            0,
                            pCb->hdr.pGolScheme->Color0,
                            pCb->hdr.pGolScheme->EmbossDkColor,
                            pCb->hdr.pGolScheme->EmbossLtColor,
                            NULL,
                            pCb->hdr.pGolScheme->fillStyle,
                            pCb->hdr.pGolScheme->EmbossSize);
                    }
                    // set the background information
                    GFX_GOL_PanelBackgroundSet(&pCb->hdr);
                    state = DRAW_CHECKBOX_RUN_DRAW;
                    break;
                }
                else
                {
                    state = DRAW_CHECKBOX_CHECK_SET_DRAW;
                }
   
            case DRAW_CHECKBOX_CHECK_SET_DRAW:
                if(GFX_GOL_ObjectStateGet(
                        pCb,
                        (GFX_GOL_CHECKBOX_DRAW_STATE | GFX_GOL_CHECKBOX_DRAW_CHECK_STATE)))
                {
                    if(!GFX_GOL_ObjectStateGet(
                            pCb,
                            GFX_GOL_CHECKBOX_DISABLED_STATE))
                    {
                        if(GFX_GOL_ObjectStateGet(
                                pCb,
                                GFX_GOL_CHECKBOX_CHECKED_STATE))
                        {
                            GFX_ColorSet(pCb->hdr.pGolScheme->TextColor0);
                            GFX_FillStyleSet(GFX_FILL_STYLE_COLOR);
                        }
                        else
                        {
                            GFX_GOL_ObjectBackGroundSet(&pCb->hdr);
                            GFX_ColorSet(pCb->hdr.pGolScheme->Color0);
                        }
                    }
                    else
                    {
                        if(GFX_GOL_ObjectStateGet(
                                pCb,
                                GFX_GOL_CHECKBOX_CHECKED_STATE))
                        {
                            GFX_ColorSet(pCb->hdr.pGolScheme->TextColorDisabled);
                            GFX_FillStyleSet(GFX_FILL_STYLE_COLOR);
                        }
                        else
                        {
                            GFX_GOL_ObjectBackGroundSet(&pCb->hdr);
                            GFX_ColorSet(pCb->hdr.pGolScheme->ColorDisabled);
                        }
                    }
    
                    checkIndent = (pCb->hdr.bottom - pCb->hdr.top) >> 2;
                    state = DRAW_CHECKBOX_CHECK_DRAW;
                }   
                else
                {
                    state = DRAW_CHECKBOX_FOCUS_DRAW;
                    break;
                }
    
            case DRAW_CHECKBOX_CHECK_DRAW:
                if (    GFX_RectangleFillDraw(
                            pCb->hdr.left +
                                checkIndent + pCb->hdr.pGolScheme->EmbossSize,
                            pCb->hdr.top +
                                checkIndent + pCb->hdr.pGolScheme->EmbossSize,
                            pCb->hdr.left +
                                (pCb->hdr.bottom - pCb->hdr.top) -
                                checkIndent - pCb->hdr.pGolScheme->EmbossSize,
                            pCb->hdr.bottom -
                                checkIndent - pCb->hdr.pGolScheme->EmbossSize)
                    != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }
    
                state = DRAW_CHECKBOX_FOCUS_DRAW;
    
            case DRAW_CHECKBOX_FOCUS_DRAW:
                if(GFX_GOL_ObjectStateGet(
                        pCb,
                        (GFX_GOL_CHECKBOX_DRAW_STATE |
                        GFX_GOL_CHECKBOX_DRAW_FOCUS_STATE)))
                {
                    if(GFX_GOL_ObjectStateGet(
                            pCb,
                            GFX_GOL_CHECKBOX_FOCUSED_STATE))
                    {
                        GFX_ColorSet(pCb->hdr.pGolScheme->TextColor0);
                    }
                    else
                    {
                        GFX_ColorSet(pCb->hdr.pGolScheme->CommonBkColor);
                    }
    
                    GFX_LineStyleSet(GFX_LINE_STYLE_THIN_DASHED);
                    if(GFX_RectangleDraw(
                            pCb->hdr.left,
                            pCb->hdr.top,
                            pCb->hdr.right,
                            pCb->hdr.bottom) != GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }
    
                    GFX_LineStyleSet(GFX_LINE_STYLE_THIN_SOLID);
                }
    
                state = DRAW_CHECKBOX_START;
#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
            	GFX_DRIVER_CompleteDrawUpdate(  pCb->hdr.left,
                                                pCb->hdr.top,
                                                pCb->hdr.right,
                                                pCb->hdr.bottom);
#endif
                return (GFX_STATUS_SUCCESS);
        } // end of switch()   
    } // end of while(1)
}

