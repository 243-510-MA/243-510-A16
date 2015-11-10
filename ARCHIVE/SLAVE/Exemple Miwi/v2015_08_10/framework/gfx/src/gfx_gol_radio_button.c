/*******************************************************************************
 Module for Microchip Graphics Library - Graphic Object Layer

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_gol_radio_button.c

  Summary:
    This implements the radio button object of the GOL.

  Description:
    Refer to Microchip Graphics Library for complete documentation of the
    RadioButton Object.
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
#include "gfx/gfx_gol_radio_button.h"
#include "gfx/gfx_gol_scan_codes.h"

#define RB_INDENT   2           // Indent for the text from title bar border

// *****************************************************************************
/*  Function:
    GFX_GOL_RADIOBUTTON  *GFX_GOL_RadioButtonCreate(
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
        This function creates a GFX_GOL_RADIOBUTTON object with the
        parameters given. It automatically attaches the new object
        into a global linked list of objects and returns the address
        of the object.

    Description:
        This function creates a GFX_GOL_RADIOBUTTON object with the
        parameters given. It automatically attaches the new object
        into a global linked list of objects and returns the address
        of the object.

        This function returns the pointer to the newly created object.
        If the object is not successfully created, it returns NULL.

        The behavior of GFX_GOL_RadioButtonCreate() will be undefined
        if one of the following is true:
        - left >= right
        - top >= bottom
        - pScheme is not pointing to a GFX_GOL_OBJ_SCHEME
        - pText is an unterminated string

*/
// *****************************************************************************
GFX_GOL_RADIOBUTTON  *GFX_GOL_RadioButtonCreate(
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
    GFX_GOL_RADIOBUTTON *pObject = NULL;
    GFX_GOL_RADIOBUTTON *pointer;

    // This pointer is used to create linked list of radio buttons for the group
    static GFX_GOL_RADIOBUTTON *_pListButtons = NULL;


    pObject = (GFX_GOL_RADIOBUTTON *)GFX_malloc(sizeof(GFX_GOL_RADIOBUTTON));
    if(pObject == NULL)
        return (pObject);

    pObject->hdr.ID = ID;
    pObject->hdr.pNxtObj = NULL;
    pObject->hdr.type = GFX_GOL_RADIOBUTTON_TYPE;
    pObject->hdr.left = left;
    pObject->hdr.top = top;
    pObject->hdr.right = right;
    pObject->hdr.bottom = bottom;
    pObject->pText = pText;
    pObject->hdr.state = state;
    pObject->hdr.DrawObj = GFX_GOL_RadioButtonDraw;         // draw function
    pObject->hdr.FreeObj = NULL;                            // free function
    pObject->hdr.actionGet = GFX_GOL_RadioButtonActionGet;  // action get function
    pObject->hdr.actionSet = GFX_GOL_RadioButtonActionSet;  // default action function

    // set this new object to be the last radio button in the list
    pObject->pNext = NULL;

    if(GFX_GOL_ObjectStateGet(pObject, GFX_GOL_RADIOBUTTON_GROUP_STATE))
    {

        // If it's first button in the group start new button's list
        _pListButtons = pObject;

        // Attach the button to the list (for the first one,
        // it will basically point to itself)
        pObject->pHead = (GFX_GOL_OBJ_HEADER *)_pListButtons;
    }
    else
    {
        // Attach the button to the list

        // point to the head of the list
        pObject->pHead = (GFX_GOL_OBJ_HEADER *)_pListButtons;
        // get the head of the list
        pointer = _pListButtons;

        // find the end of the list and attach the new object
        while(pointer->pNext != NULL)
        {
            pointer = (GFX_GOL_RADIOBUTTON *)pointer->pNext;
        }

        pointer->pNext = (GFX_GOL_OBJ_HEADER *)pObject;
    }

    // Set the style scheme to be used
    pObject->hdr.pGolScheme = (GFX_GOL_OBJ_SCHEME *)pScheme;

    // Set the text and the alignment
    GFX_GOL_RadioButtonTextSet(pObject, pText);
    GFX_GOL_RadioButtonTextAlignmentSet(pObject, alignment);

    GFX_GOL_ObjectAdd((GFX_GOL_OBJ_HEADER *)pObject);

    return (pObject);
}

// *****************************************************************************
/*  Function:
    GFX_XCHAR GFX_GOL_RadioButtonCheckSet(
                                GFX_GOL_RADIOBUTTON *pObject,
                                uint16_t id)

    Summary:
        This function sets the ID of the currently checked
        radio button in the group.

    Description:
        This function sets the ID of the currently checked
        radio button in the group. When there is only one
        member of the group, then that member will have
        the check.

        When the id given does not exist in the group, the
        function will do nothing.

*/
// *****************************************************************************
void GFX_GOL_RadioButtonCheckSet(GFX_GOL_RADIOBUTTON *pObject, uint16_t id)
{
    GFX_GOL_OBJ_HEADER *pointer;

    pointer = (GFX_GOL_OBJ_HEADER*)pObject->pHead;

    while(pointer != NULL)
    {
        if(pointer->ID == id)
        {
            // set check and redraw
            GFX_GOL_ObjectStateSet(
                    pointer,
                    GFX_GOL_RADIOBUTTON_CHECKED_STATE |
                    GFX_GOL_RADIOBUTTON_DRAW_CHECK_STATE);
        }
        else
        {
            // reset checked
            GFX_GOL_ObjectStateClear(
                    pointer,
                    GFX_GOL_RADIOBUTTON_CHECKED_STATE);
            // redraw
            GFX_GOL_ObjectStateSet(
                    pointer,
                    GFX_GOL_RADIOBUTTON_DRAW_CHECK_STATE);
        }

        pointer = ((GFX_GOL_RADIOBUTTON*)pointer)->pNext;
    }
}

// *****************************************************************************
/*  Function:
    uint16_t GFX_GOL_RadioButtonCheckGet(
                                GFX_GOL_RADIOBUTTON *pObject)

    Summary:
        This function returns the ID of the currently checked
        radio button in the group.

    Description:
        This function returns the ID of the currently checked
        radio button in the group. When there is only one
        member of the group, then that member will have
        the check.

        When no member of the group is checked, then the id
        returned is (-1 or 0xFFFF).

*/
// *****************************************************************************
uint16_t GFX_GOL_RadioButtonCheckGet(
                                GFX_GOL_RADIOBUTTON *pObject)
{
    GFX_GOL_OBJ_HEADER *pointer;

    pointer = (GFX_GOL_OBJ_HEADER *)pObject->pHead;

    while(pointer != NULL)
    {
        if(GFX_GOL_ObjectStateGet(pointer, GFX_GOL_RADIOBUTTON_CHECKED_STATE))
        {
            return (pointer->ID);
        }

        pointer = ((GFX_GOL_RADIOBUTTON*)pointer)->pNext;
    }

    return (-1);
}

// *****************************************************************************
/*  Function:
    GFX_XCHAR *GFX_GOL_RadioButtonTextSet(
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
void GFX_GOL_RadioButtonTextSet(
                                GFX_GOL_RADIOBUTTON *pObject,
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
    void GFX_GOL_RadioButtonTextAlignmentSet(
                                GFX_GOL_RADIOBUTTON *pObject,
                                GFX_ALIGNMENT align)

    Summary:
        This function sets the text alignment of the text
        string used by the object.

    Description:
        This function sets the text alignment of the text
        string used by the object.

*/
// *****************************************************************************
void __attribute__ ((always_inline)) GFX_GOL_RadioButtonTextAlignmentSet(
                                GFX_GOL_RADIOBUTTON *pObject,
                                GFX_ALIGNMENT align)
{
    pObject->alignment = align;
}

// *****************************************************************************
/*  Function:
    GFX_ALIGNMENT GFX_GOL_RadioButtonTextAlignmentGet(
                                GFX_GOL_RADIOBUTTON *pObject)

    Summary:
        This function returns the text alignment of the text
        string used by the object.

    Description:
        This function returns the text alignment of the text
        string used by the object.

*/
// *****************************************************************************
GFX_ALIGNMENT __attribute__ ((always_inline)) GFX_GOL_RadioButtonTextAlignmentGet(
                                GFX_GOL_RADIOBUTTON *pObject)
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
        using the application defined GFX_GOL_MESSAGE_CALLBACK_FUNC.
        When the user message is determined to affect the object, application
        can perform the state change in the message callback function.

*/
// *****************************************************************************
void GFX_GOL_RadioButtonActionSet(
                                GFX_GOL_TRANSLATED_ACTION translatedMsg,
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage)
{
    GFX_GOL_OBJ_HEADER  *pointer;
    GFX_GOL_RADIOBUTTON *pRb;

    pRb = (GFX_GOL_RADIOBUTTON *)pObject;

#ifndef GFX_CONFIG_FOCUS_DISABLE
#ifndef GFX_CONFIG_USE_TOUCHSCREEN_DISABLE
    if(pMessage->type == TYPE_TOUCHSCREEN)
    {
        if(!GFX_GOL_ObjectStateGet(pRb, GFX_GOL_RADIOBUTTON_FOCUSED_STATE))
        {
            GFX_GOL_ObjectFocusSet((GFX_GOL_OBJ_HEADER *)pRb);
        }
    }

#endif
#endif

    if(translatedMsg == GFX_GOL_RADIOBUTTON_ACTION_CHECKED)
    {

        // Uncheck radio buttons in the group
        pointer = pRb->pHead;

        while(pointer != NULL)
        {
            if(GFX_GOL_ObjectStateGet(
                    pointer,
                    GFX_GOL_RADIOBUTTON_CHECKED_STATE))
            {
                // reset check
                GFX_GOL_ObjectStateClear(
                        pointer,
                        GFX_GOL_RADIOBUTTON_CHECKED_STATE);
                // redraw
                GFX_GOL_ObjectStateSet(
                        pointer,
                        GFX_GOL_RADIOBUTTON_DRAW_CHECK_STATE);
            }

            pointer = ((GFX_GOL_RADIOBUTTON*)pointer)->pNext;
        }

        // set check and redraw
        GFX_GOL_ObjectStateSet(pRb, 
                GFX_GOL_RADIOBUTTON_CHECKED_STATE |
                GFX_GOL_RADIOBUTTON_DRAW_CHECK_STATE);
    }
}

// *****************************************************************************
/*  Function:
    GFX_GOL_TRANSLATED_ACTION GFX_GOL_RadioButtonActionGet(
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
GFX_GOL_TRANSLATED_ACTION GFX_GOL_RadioButtonActionGet(
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage)
{

    GFX_GOL_RADIOBUTTON *pRb;

    pRb = (GFX_GOL_RADIOBUTTON *)pObject;

    // Evaluate if the message is for the radio button
    // Check if disabled first
    if(GFX_GOL_ObjectStateGet(pRb, GFX_GOL_RADIOBUTTON_DISABLED_STATE))
        return (GFX_GOL_OBJECT_ACTION_INVALID);

#ifndef GFX_CONFIG_USE_TOUCHSCREEN_DISABLE
    if(pMessage->type == TYPE_TOUCHSCREEN)
    {
        if(pMessage->uiEvent == EVENT_PRESS)
        {

            // Check if it falls in the radio button borders
            if
            (
                (pRb->hdr.left   < pMessage->param1) &&
                (pRb->hdr.right  > pMessage->param1) &&
                (pRb->hdr.top    < pMessage->param2) &&
                (pRb->hdr.bottom > pMessage->param2)
            )
            {
                if(!GFX_GOL_ObjectStateGet(
                        pRb,
                        GFX_GOL_RADIOBUTTON_CHECKED_STATE))
                {
                    return (GFX_GOL_RADIOBUTTON_ACTION_CHECKED);
                }
            }
        }

        return (GFX_GOL_OBJECT_ACTION_INVALID);
    }

#endif
#ifndef GFX_CONFIG_USE_KEYBOARD_DISABLE
    if(pMessage->type == TYPE_KEYBOARD)
    {
        if((uint16_t)pMessage->param1 == pRb->hdr.ID)
        {
            if(pMessage->uiEvent == EVENT_KEYSCAN)
            {
                if( (pMessage->param2 == SCAN_SPACE_PRESSED) ||
                    (pMessage->param2 == SCAN_CR_PRESSED))
                {
                    if(!GFX_GOL_ObjectStateGet(
                            pRb,
                            GFX_GOL_RADIOBUTTON_CHECKED_STATE))
                    {
                        return (GFX_GOL_RADIOBUTTON_ACTION_CHECKED);
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
    GFX_STATUS GFX_GOL_RadioButtonDraw(void *pObject)

    Summary:
        This function renders the object on the screen based on the
        current state of the object.

    Description:
        This function renders the object on the screen based on the
        current state of the object. Location of the object is
        determined by the left, top, right and bottom parameters.
        The colors used are dependent on the state of the object.
        The font used is determined by the style scheme set.

        The text on the GFX_GOL_RADIOBUTTON is drawn with the text
        alignment based on the alignment parameter set on the object.

        When rendering objects of the same type, each object
        must be rendered completely before the rendering of the
        next object is started. This is to avoid incomplete
        object rendering.

        Normally, application will just call GFX_GOL_ObjectListDraw()
        to allow the Graphics Library to manage all object rendering.
        See GFX_GOL_ObjectListDraw() for more information on object rendering.

*/
// *****************************************************************************
GFX_STATUS GFX_GOL_RadioButtonDraw(void *pObject)
{
    typedef enum
    {
        DRAW_RADIOBUTTON_START,
        DRAW_RADIOBUTTON_PANEL,
        DRAW_RADIOBUTTON_HIDE,
        DRAW_RADIOBUTTON_UPDATE,
        DRAW_RADIOBUTTON0,
        DRAW_RADIOBUTTON1,
        DRAW_RADIOBUTTON_TEXT,
        DRAW_RADIOBUTTON_TEXT_RUN,
        DRAW_RADIOBUTTON_CHECK,
        DRAW_RADIOBUTTON_CHECK_RUN,
        DRAW_RADIOBUTTON_FOC,
        DRAW_RADIOBUTTON_FOC_ACTUAL
    } RB_DRAW_STATES;

        #define SIN45   46341

    static RB_DRAW_STATES   state = DRAW_RADIOBUTTON_START;
    int16_t                 checkIndent;
    static int16_t          radius;
    static int16_t          x, y, temp;

    GFX_COLOR               faceClr;
    GFX_GOL_RADIOBUTTON     *pRb;
    GFX_FILL_STYLE          fillStyle;

    pRb = (GFX_GOL_RADIOBUTTON *)pObject;

    while(1)
    {

        if (GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
            return (GFX_STATUS_FAILURE);
    
        switch(state)
        {
            case DRAW_RADIOBUTTON_START:
#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                GFX_DRIVER_SetupDrawUpdate( pRb->hdr.left,
                                            pRb->hdr.top,
                                            pRb->hdr.right,
                                            pRb->hdr.bottom);
#endif
                GFX_GOL_PanelBackgroundSet(&pRb->hdr);

                if(GFX_GOL_ObjectStateGet(
                        pRb,
                        GFX_GOL_RADIOBUTTON_DRAW_STATE))
                {

                    GFX_GOL_PanelParameterSet
                    (
                        pRb->hdr.left,
                        pRb->hdr.top,
                        pRb->hdr.right,
                        pRb->hdr.bottom,
                        0,
                        pRb->hdr.pGolScheme->CommonBkColor,
                        0,
                        0,
                        NULL,
                        pRb->hdr.pGolScheme->fillStyle,
                        0
                    );
                    state = DRAW_RADIOBUTTON_PANEL;
                }
                else if(GFX_GOL_ObjectStateGet(
                        pRb,
                        GFX_GOL_RADIOBUTTON_HIDE_STATE))
            	{
                    state = DRAW_RADIOBUTTON_HIDE;
                    break;
	        }
                else
                {
                    state = DRAW_RADIOBUTTON_UPDATE;
                    break;
                }

            case DRAW_RADIOBUTTON_PANEL:

                if(GFX_GOL_PanelDraw() != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }
                state = DRAW_RADIOBUTTON_UPDATE;
                // no break here since the next state is what we want

            case DRAW_RADIOBUTTON_UPDATE:

                radius = ((pRb->hdr.bottom - pRb->hdr.top) >> 1) - RB_INDENT;
                x = pRb->hdr.left +
                        ((pRb->hdr.bottom - pRb->hdr.top) >> 1) + RB_INDENT;
                y = (pRb->hdr.bottom + pRb->hdr.top) >> 1;
    
                if(GFX_GOL_ObjectStateGet(pRb, GFX_GOL_RADIOBUTTON_DRAW_STATE))
                {
                    state = DRAW_RADIOBUTTON0;
                }
                else if (GFX_GOL_ObjectStateGet(
                            pRb,
                            GFX_GOL_RADIOBUTTON_DRAW_CHECK_STATE))
                {
                    state = DRAW_RADIOBUTTON_CHECK;
                    break;
                }
                else if (GFX_GOL_ObjectStateGet(
                            pRb,
                            GFX_GOL_RADIOBUTTON_DRAW_FOCUS_STATE))
                {
                    state = DRAW_RADIOBUTTON_FOC;
                    break;
                }
                else
            	{
#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                    GFX_DRIVER_CompleteDrawUpdate(  pRb->hdr.left,
                                                    pRb->hdr.top,
	                                            pRb->hdr.right,
	                                            pRb->hdr.bottom);
#endif
                    return (GFX_STATUS_SUCCESS);
                }
    
            case DRAW_RADIOBUTTON0:
                if(!GFX_GOL_ObjectStateGet(
                        pRb,
                        GFX_GOL_RADIOBUTTON_DISABLED_STATE))
                {
                    faceClr = pRb->hdr.pGolScheme->Color0;
                }
                else
                {
                    faceClr = pRb->hdr.pGolScheme->ColorDisabled;
                }

#if defined (GFX_CONFIG_ALPHABLEND_DISABLE) && !defined (GFX_CONFIG_GRADIENT_DISABLE)
                // gradient feature is not supported on this object's
                // check rendering
                fillStyle = GFX_FILL_STYLE_COLOR;
#else
                fillStyle = pRb->hdr.pGolScheme->fillStyle;
#endif

                GFX_GOL_PanelParameterSet
                (
                    x,
                    y,
                    x,
                    y,
                    radius,
                    faceClr,
                    pRb->hdr.pGolScheme->EmbossDkColor,
                    pRb->hdr.pGolScheme->EmbossLtColor,
                    NULL,
                    fillStyle,
                    pRb->hdr.pGolScheme->EmbossSize
                );

                GFX_LineStyleSet(GFX_LINE_STYLE_THIN_SOLID);
                state = DRAW_RADIOBUTTON1;
    
            case DRAW_RADIOBUTTON1:
                if(GFX_GOL_PanelDraw() != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }
    
                state = DRAW_RADIOBUTTON_TEXT;
    
            case DRAW_RADIOBUTTON_TEXT:
                if(pRb->pText != NULL)
                {
                    GFX_FontSet(pRb->hdr.pGolScheme->pFont);
    
                    if(GFX_GOL_ObjectStateGet(
                            pRb,
                            GFX_GOL_RADIOBUTTON_DISABLED_STATE))
                    {
                        GFX_ColorSet(pRb->hdr.pGolScheme->TextColorDisabled);
                    }
                    else
                    {
                        GFX_ColorSet(pRb->hdr.pGolScheme->TextColor0);
                    }
                    state = DRAW_RADIOBUTTON_TEXT_RUN;
                }
                else
                {
                    state = DRAW_RADIOBUTTON_CHECK;
                    break;
                }
            
            case DRAW_RADIOBUTTON_TEXT_RUN:

                temp = pRb->hdr.left + pRb->hdr.pGolScheme->EmbossSize +
                    (radius << 1) + RB_INDENT;
                if (GFX_TextStringBoxDraw(
                        temp,
                        pRb->hdr.top + pRb->hdr.pGolScheme->EmbossSize,
                        pRb->hdr.right - temp -
                             pRb->hdr.pGolScheme->EmbossSize,
                        pRb->hdr.bottom -
                             pRb->hdr.top -
                             pRb->hdr.pGolScheme->EmbossSize,
                        pRb->pText,
                        0,
                        pRb->alignment) != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }
            
                state = DRAW_RADIOBUTTON_CHECK;
                break;
    
            case DRAW_RADIOBUTTON_CHECK:

                // set the background info
                GFX_GOL_ObjectBackGroundSet(&pRb->hdr);

                if(GFX_GOL_ObjectStateGet(
                        pRb,
                        GFX_GOL_RADIOBUTTON_CHECKED_STATE))
                {

                    GFX_FillStyleSet(GFX_FILL_STYLE_COLOR);
                    if(GFX_GOL_ObjectStateGet(
                            pRb,
                            GFX_GOL_RADIOBUTTON_DISABLED_STATE))
                    {
                        GFX_ColorSet(pRb->hdr.pGolScheme->TextColorDisabled);
                    }
                    else
                    {
#if (GFX_CONFIG_COLOR_DEPTH == 1)
                        GFX_ColorSet(BLACK);
#else
                        GFX_ColorSet(pRb->hdr.pGolScheme->TextColor0);
#endif
                    }
                }
                else
                {
                    if(GFX_GOL_ObjectStateGet(
                            pRb,
                            GFX_GOL_RADIOBUTTON_DISABLED_STATE))
                    {
                        GFX_ColorSet(pRb->hdr.pGolScheme->ColorDisabled);
                    }
                    else
                    {
#if (GFX_CONFIG_COLOR_DEPTH == 1)
                        GFX_ColorSet(WHITE);
#else
                        GFX_ColorSet(pRb->hdr.pGolScheme->Color0);
#endif
                    }
                }

#if defined (GFX_CONFIG_ALPHABLEND_DISABLE) && !defined (GFX_CONFIG_GRADIENT_DISABLE)
                // gradient feature is not supported on this object's
                // check rendering
                GFX_FillStyleSet(GFX_FILL_STYLE_COLOR);
#endif
                GFX_BackgroundTypeSet(pRb->hdr.pGolScheme->CommonBkType);
                state = DRAW_RADIOBUTTON_CHECK_RUN;
    
            case DRAW_RADIOBUTTON_CHECK_RUN:
                checkIndent = (pRb->hdr.bottom - pRb->hdr.top) >> 2;

                if(GFX_CircleFillDraw(
                        x, y,
                        radius - checkIndent) != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }
    
                state = DRAW_RADIOBUTTON_FOC;
                break;
    
            case DRAW_RADIOBUTTON_FOC:
                if(GFX_GOL_ObjectStateGet(
                        pRb,
                        (GFX_GOL_RADIOBUTTON_DRAW_STATE |
                        GFX_GOL_RADIOBUTTON_DRAW_FOCUS_STATE)))
                {
                    if(GFX_GOL_ObjectStateGet(
                            pRb,
                            GFX_GOL_RADIOBUTTON_FOCUSED_STATE))
                    {
                        GFX_LineStyleSet(GFX_LINE_STYLE_THIN_DASHED);
                        GFX_ColorSet(pRb->hdr.pGolScheme->TextColor0);
                    }
                    else
                    {
                        GFX_ColorSet(pRb->hdr.pGolScheme->CommonBkColor);
                    }
    
                    state = DRAW_RADIOBUTTON_FOC_ACTUAL;
                    break;
                }
                state = DRAW_RADIOBUTTON_START;
#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                GFX_DRIVER_CompleteDrawUpdate(  pRb->hdr.left,
                                                pRb->hdr.top,
                                                pRb->hdr.right,
                                                pRb->hdr.bottom);
#endif
                return (GFX_STATUS_SUCCESS);
    
            case DRAW_RADIOBUTTON_FOC_ACTUAL:
                if(GFX_RectangleDraw(
                        pRb->hdr.left,
                        pRb->hdr.top,
                        pRb->hdr.right,
                        pRb->hdr.bottom) != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }
                GFX_LineStyleSet(GFX_LINE_STYLE_THIN_SOLID);
                state = DRAW_RADIOBUTTON_START;

#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                GFX_DRIVER_CompleteDrawUpdate(  pRb->hdr.left,
                                                pRb->hdr.top,
                                                pRb->hdr.right,
                                                pRb->hdr.bottom);
#endif
                return (GFX_STATUS_SUCCESS);

            case DRAW_RADIOBUTTON_HIDE:

                if (GFX_GOL_ObjectHideDraw(&(pRb->hdr)) != GFX_STATUS_SUCCESS)
                    return (GFX_STATUS_FAILURE);

                state = DRAW_RADIOBUTTON_START;

#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                    GFX_DRIVER_CompleteDrawUpdate(  pRb->hdr.left,
                                                    pRb->hdr.top,
	                                            pRb->hdr.right,
	                                            pRb->hdr.bottom);
#endif
                return (GFX_STATUS_SUCCESS);


        } // end of switch()
    } // end of while(1)

}

