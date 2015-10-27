/*******************************************************************************
 Module for Microchip Graphics Library - Graphic Object Layer

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_gol_group_box.c

  Summary:
    This implements the group box object of the GOL.

  Description:
    Refer to Microchip Graphics Library for complete documentation of the
    GroupBox Object.
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
#include "gfx/gfx_gol_group_box.h"

// *****************************************************************************
/*  Function:
    GFX_GOL_GROUPBOX  *GFX_GOL_GroupboxCreate(
                                uint16_t            ID,
                                uint16_t            left,
                                uint16_t            top,
                                uint16_t            right,
                                uint16_t            bottom,
                                uint16_t            state,
                                GFX_RESOURCE_HDR    *pImage,
                                GFX_XCHAR           *pText,
                                GFX_ALIGNMENT       alignment,
                                GFX_GOL_OBJ_SCHEME  *pScheme)
    Summary:
        This function creates a GFX_GOL_GROUPBOX object with the parameters
        given. It automatically attaches the new object into a global
        linked list of objects and returns the address of the object.

    Description:
        This function creates a GFX_GOL_GROUPBOX object with the parameters
        given. It automatically attaches the new object into a global
        linked list of objects and returns the address of the object.

        This function returns the pointer to the newly created object.
        If the object is not successfully created, it returns NULL.

*/
// *****************************************************************************
GFX_GOL_GROUPBOX  *GFX_GOL_GroupboxCreate
        (
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
    GFX_GOL_GROUPBOX    *pGb = NULL;

    pGb = (GFX_GOL_GROUPBOX *)GFX_malloc(sizeof(GFX_GOL_GROUPBOX));
    if(pGb == NULL)
        return (pGb);

    pGb->hdr.ID         = ID;
    pGb->hdr.pNxtObj    = NULL;
    pGb->hdr.type       = GFX_GOL_GROUPBOX_TYPE;
    pGb->hdr.left       = left;
    pGb->hdr.top        = top;
    pGb->hdr.right      = right;
    pGb->hdr.bottom     = bottom;
    pGb->hdr.state      = state;
    pGb->hdr.DrawObj    = GFX_GOL_GroupboxDraw;         // draw function
    pGb->hdr.FreeObj    = NULL;                            // free function
    pGb->hdr.actionGet  = GFX_GOL_GroupboxActionGet;    // action get function
    pGb->hdr.actionSet  = NULL;                         // default action function


    // Set alignement
    GFX_GOL_GroupboxTextAlignmentSet(pGb, alignment);

    // Set the style scheme to be used
    pGb->hdr.pGolScheme = (GFX_GOL_OBJ_SCHEME *)pScheme;

    if(pText != NULL)
    {
        // Set the text width & height
        GFX_GOL_GroupboxTextSet(pGb, pText);
    } 
    else
    {
        pGb->pText      = NULL;
        pGb->textWidth  = 0;
        pGb->textHeight = 0;
    }

    GFX_GOL_ObjectAdd((GFX_GOL_OBJ_HEADER *)pGb);

    return (pGb);
}

// *****************************************************************************
/*  Function:
    GFX_XCHAR *GFX_GOL_GroupboxTextSet(
                                GFX_GOL_GROUPBOX *pObject,
                                GFX_XCHAR *pText)

    Summary:
        This function sets the address of the current text
        string used by the object.

    Description:
        This function sets the address of the current text
        string used by the object.

*/
// *****************************************************************************
void GFX_GOL_GroupboxTextSet(
                                GFX_GOL_GROUPBOX *pObject,
                                GFX_XCHAR * pText)
{
    pObject->pText = pText;

    // Set the text width & height
    pObject->textWidth = GFX_TextStringWidthGet(
                                    pText,
                                    pObject->hdr.pGolScheme->pFont);

    pObject->textHeight = GFX_TextStringHeightGet(
                                    pObject->hdr.pGolScheme->pFont);
        
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_GroupboxTextAlignmentSet(
                                GFX_GOL_GROUPBOX *pObject,
                                GFX_ALIGNMENT align)

    Summary:
        This function sets the text alignment of the text
        string used by the object.

    Description:
        This function sets the text alignment of the text
        string used by the object.

*/
// *****************************************************************************
void __attribute__ ((always_inline)) GFX_GOL_GroupboxTextAlignmentSet(
                                GFX_GOL_GROUPBOX *pObject,
                                GFX_ALIGNMENT align)
{
    pObject->alignment = align;
}
// *****************************************************************************
/*  Function:
    GFX_ALIGNMENT GFX_GOL_GroupboxTextAlignmentGet(
                                GFX_GOL_GROUPBOX *pObject)

    Summary:
        This function returns the text alignment of the text
        string used by the object.

    Description:
        This function returns the text alignment of the text
        string used by the object.

*/
// *****************************************************************************
GFX_ALIGNMENT __attribute__ ((always_inline)) GFX_GOL_GroupboxTextAlignmentGet(
                                GFX_GOL_GROUPBOX *pObject)
{
    return (pObject->alignment);
}

/********************************************************************************
  Function:
        GFX_GOL_TRANSLATED_ACTION GFX_GOL_GroupboxActionGet(
                                    void *pObject,
                                    GFX_GOL_MESSAGE *pMessage);

 Summary:
        This function evaluates the message from a user if the
        message will affect the object or not.

    Description:
        This function evaluates the message from a user if the
        message will affect the object or not. 

  ********************************************************************************/
GFX_GOL_TRANSLATED_ACTION GFX_GOL_GroupboxActionGet(
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage)
{

    GFX_GOL_GROUPBOX *pGb;

    pGb = (GFX_GOL_GROUPBOX *)pObject;

    // Evaluate if the message is for the button
    // Check if disabled first
    if(GFX_GOL_ObjectStateGet(pGb, GFX_GOL_GROUPBOX_DISABLED_STATE))
        return (GFX_GOL_OBJECT_ACTION_INVALID);

#ifndef GFX_CONFIG_USE_TOUCHSCREEN_DISABLE
    if(pMessage->type == TYPE_TOUCHSCREEN)
    {

        // Check if it falls to the left or right of the center of the thumb's face
        if
        (
            (pGb->hdr.left   < pMessage->param1) &&
            (pGb->hdr.right  > pMessage->param1) &&
            (pGb->hdr.top    < pMessage->param2) &&
            (pGb->hdr.bottom > pMessage->param2)
        )
        {
            if( (pMessage->uiEvent == EVENT_PRESS)      ||
                (pMessage->uiEvent == EVENT_RELEASE))
            {
                return (GFX_GOL_GROUPBOX_ACTION_SELECTED);
            }
        }
    }
#endif

    return (GFX_GOL_OBJECT_ACTION_INVALID);
}

#if (GFX_CONFIG_COLOR_DEPTH == 1)
    #define THREE_D_EFFECT  0
#else
    #define THREE_D_EFFECT  1
#endif


// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_GOL_GroupboxDraw(void *pObject)

    Summary:
        This function renders the object on the screen based on the
        current state of the object.

    Description:
        This function renders the object on the screen based on the
        current state of the object. Location of the object is
        determined by the left, top, right and bottom parameters.
        The colors used are dependent on the state of the object.
        The font used is determined by the style scheme set.

        The text on the face of the GFX_GOL_GROUPBOX is drawn on top of
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
GFX_STATUS GFX_GOL_GroupboxDraw(void *pObject)
{
    typedef enum
    {
        GB_STATE_IDLE,
        GB_STATE_HIDETEXT,
        GB_STATE_SETDIMENSION,
        GB_STATE_DRAWTEXT,
        GB_STATE_DRAWTOPRIGHT,
        GB_STATE_DRAWTOPLEFT,
        GB_STATE_DRAWSIDELEFT,
        GB_STATE_DRAWSIDERIGHT,
        GB_STATE_DRAWBOTTOM,
#if (GFX_CONFIG_COLOR_DEPTH != 1)
        GB_STATE_2DRAWTOPRIGHT,
        GB_STATE_2DRAWTOPLEFT,
        GB_STATE_2DRAWSIDELEFT,
        GB_STATE_2DRAWSIDERIGHT,
        GB_STATE_2DRAWBOTTOM,
#endif
    } GB_DRAW_STATES;

    static GB_DRAW_STATES   state = GB_STATE_IDLE;
    // used to draw lines that start/stops at text.
    static int16_t          textLeft, textRight, top;
    GFX_GOL_GROUPBOX        *pGb;

    pGb = (GFX_GOL_GROUPBOX *)pObject;

    while(1)
    {

        if (GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
            return (GFX_STATUS_FAILURE);

        switch(state)
        {
            case GB_STATE_IDLE:

                if(GFX_GOL_ObjectStateGet(
                        pGb,
                        GFX_GOL_GROUPBOX_HIDE_STATE))
                {
                    // Hide the object (remove from screen)
                    if (GFX_GOL_ObjectHideDraw(&(pGb->hdr)) != GFX_STATUS_SUCCESS)
                        return (GFX_STATUS_FAILURE);

                    return (GFX_STATUS_SUCCESS);
                }

                if(GFX_GOL_ObjectStateGet(
                        pGb,
                        GFX_GOL_GROUPBOX_DRAW_STATE))
                {
                    state = GB_STATE_HIDETEXT;
                }
                else
                {
                    // do nothing 
                    return (GFX_STATUS_SUCCESS);
                }
    
            case GB_STATE_HIDETEXT:
                // hide the text first
                // need to refresh the background of the area that the text
                // occupies
                if(pGb->pText != NULL)
                {                      
                    // needed when dynamically changing the alignment of text
                    
                    // this object only refresh the constant color background
                    // or an image background
                    if (
                            pGb->hdr.pGolScheme->CommonBkType ==
                            GFX_BACKGROUND_IMAGE)
                    {
                        // draw the partial image of the background where
                        // the text is located
                        if (GFX_ImagePartialDraw(
                            pGb->hdr.left, pGb->hdr.top,
                            pGb->hdr.left -
                                pGb->hdr.pGolScheme->CommonBkLeft,
                            pGb->hdr.top -
                                pGb->hdr.pGolScheme->CommonBkTop,
                            pGb->hdr.right - pGb->hdr.left + 1,
                            pGb->hdr.top + pGb->textHeight + 1,
                            pGb->hdr.pGolScheme->pCommonBkImage)
                            != GFX_STATUS_SUCCESS)
                        {
                            return (GFX_STATUS_FAILURE);
                        }

                    }
                    else if (
                                (pGb->hdr.pGolScheme->CommonBkType ==
                                 GFX_BACKGROUND_COLOR) ||
                                (pGb->hdr.pGolScheme->CommonBkType ==
                                 GFX_BACKGROUND_NONE)
                            )
                    {
                        GFX_ColorSet(pGb->hdr.pGolScheme->CommonBkColor);
                        if(GFX_BarDraw(
                                pGb->hdr.left,
                                pGb->hdr.top,
                                pGb->hdr.right,
                                pGb->hdr.top +
                                    pGb->textHeight) != GFX_STATUS_SUCCESS)
                        {
                            return (GFX_STATUS_FAILURE);
                        }
                    }
                }
    
                state = GB_STATE_SETDIMENSION;
                break;
    
            case GB_STATE_SETDIMENSION:
    
                if(GFX_GOL_ObjectStateGet(
                        pGb,
                        GFX_GOL_GROUPBOX_DISABLED_STATE))
                {
                    // set color to inactive color
                    GFX_ColorSet(
                            pGb->hdr.pGolScheme->TextColorDisabled);
                }
                else
                {
                    // active color
                    GFX_ColorSet(
                            pGb->hdr.pGolScheme->TextColor0);
                }
    
                if(pGb->pText == NULL)
                {   // there is no text, use full dimensions
                    top = pGb->hdr.top;
                    textLeft = pGb->hdr.left + 1;
                    textRight = textLeft;
                    state = GB_STATE_DRAWTOPRIGHT;  // go to drawing of right top line
                    break;
                }
                else
                {   // text is present, set up dimensions with text
                    GFX_FontSet(pGb->hdr.pGolScheme->pFont);

                    // adjust lines on top
                    top = pGb->hdr.top + (pGb->textHeight >> 1);

                    switch (pGb->alignment & GFX_ALIGN_HORIZONTAL_MASK)
                    {
                        case GFX_ALIGN_RIGHT:
                            // do right aligned
                            textLeft = pGb->hdr.right - pGb->textWidth - 2;
                            textRight = pGb->hdr.right - 2;
                            break;

                        case GFX_ALIGN_LEFT:
                            // do left aligned
                            textLeft = pGb->hdr.left + 2;
                            textRight = pGb->hdr.left + 2 + pGb->textWidth;
                            break;

                        case GFX_ALIGN_HCENTER:
                        default:
                            // do center aligned
                            textLeft = (    pGb->hdr.left +
                                            pGb->hdr.right -
                                            pGb->textWidth) >> 1;
                            textRight = textLeft + pGb->textWidth;
                            break;
                    }
                    state = GB_STATE_DRAWTEXT;
                }
    
            case GB_STATE_DRAWTEXT:

                if (GFX_TextStringBoxDraw(                      \
                        textLeft,
                        pGb->hdr.top,
                        pGb->textWidth,
                        pGb->textHeight,
                        pGb->pText,
                        0,
                        GFX_ALIGN_CENTER)  != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }

#if (GFX_CONFIG_COLOR_DEPTH == 1)
                GFX_ColorSet(WHITE);
#else
                GFX_ColorSet(pGb->hdr.pGolScheme->EmbossDkColor);
#endif
                state = GB_STATE_DRAWTOPRIGHT;
                break;
    
            case GB_STATE_DRAWTOPRIGHT:
                // top line at right
                if(GFX_LineDraw(
                        textRight,
                        top + THREE_D_EFFECT,
                        pGb->hdr.right,
                        top + THREE_D_EFFECT) != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }
                state = GB_STATE_DRAWTOPLEFT;
                break;
    
            case GB_STATE_DRAWTOPLEFT:
                // top line at left
                if(GFX_LineDraw(
                        pGb->hdr.left + THREE_D_EFFECT,
                        top + THREE_D_EFFECT,
                        textLeft,
                        top + THREE_D_EFFECT) != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }
                state = GB_STATE_DRAWSIDELEFT;
                break;
    
            case GB_STATE_DRAWSIDELEFT:
                // side line at left
                if(GFX_LineDraw(
                        pGb->hdr.left + THREE_D_EFFECT,
                        top + THREE_D_EFFECT,
                        pGb->hdr.left + THREE_D_EFFECT,
                        pGb->hdr.bottom) != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }
                state = GB_STATE_DRAWSIDERIGHT;
                break;
    
            case GB_STATE_DRAWSIDERIGHT:
                // side line at right
                if(GFX_LineDraw(
                        pGb->hdr.right,
                        top + THREE_D_EFFECT,
                        pGb->hdr.right,
                        pGb->hdr.bottom) != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }
                state = GB_STATE_DRAWBOTTOM;
                break;
    
            case GB_STATE_DRAWBOTTOM:
                // bottom line
                if(GFX_LineDraw(
                        pGb->hdr.left + THREE_D_EFFECT,
                        pGb->hdr.bottom,
                        pGb->hdr.right,
                        pGb->hdr.bottom) != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }
#if (GFX_CONFIG_COLOR_DEPTH == 1)
                state = GB_STATE_IDLE;
                return 1;
#else
                state = GB_STATE_2DRAWTOPLEFT;
                break;
#endif
#if (GFX_CONFIG_COLOR_DEPTH != 1)
    
            case GB_STATE_2DRAWTOPLEFT:
                // 2nd line top line at left
                GFX_ColorSet(pGb->hdr.pGolScheme->EmbossLtColor);
                if(GFX_LineDraw(
                        pGb->hdr.left,
                        top,
                        textLeft,
                        top) != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }
                state = GB_STATE_2DRAWTOPRIGHT;
                break;
    
            case GB_STATE_2DRAWTOPRIGHT:
                // 2nd line top line at right
                if(GFX_LineDraw(
                        textRight,
                        top,
                        pGb->hdr.right,
                        top) != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }
                state = GB_STATE_2DRAWSIDELEFT;
                break;
    
            case GB_STATE_2DRAWSIDELEFT:
                // 2nd line left
                if(GFX_LineDraw(
                        pGb->hdr.left,
                        top,
                        pGb->hdr.left,
                        pGb->hdr.bottom - 1) != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }
                state = GB_STATE_2DRAWSIDERIGHT;
                break;
    
            case GB_STATE_2DRAWSIDERIGHT:
                // 2nd line right
                if(GFX_LineDraw(
                        pGb->hdr.right - 1,
                        top + 2,
                        pGb->hdr.right - 1,
                        pGb->hdr.bottom - 1) != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }
                state = GB_STATE_2DRAWBOTTOM;
                break;
    
            case GB_STATE_2DRAWBOTTOM:
                // 2nd line bottom
                if(GFX_LineDraw(
                        pGb->hdr.left + 2,
                        pGb->hdr.bottom - 1,
                        pGb->hdr.right - 1,
                        pGb->hdr.bottom - 1) != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }                 
                state = GB_STATE_IDLE;
                return 1;
#endif // end of #if (COLOR_DEPTH != 1)

        } // end of switch()
    } // end of while(1)
}

