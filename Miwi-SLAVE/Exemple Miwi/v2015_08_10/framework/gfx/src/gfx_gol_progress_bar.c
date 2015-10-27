/*******************************************************************************
 Module for Microchip Graphics Library - Graphic Object Layer

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_gol_progress_bar.c

  Summary:
    This implements the progress bar object of the GOL.

  Description:
    Refer to Microchip Graphics Library for complete documentation of the
    ProgressBar Object.
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
#include "gfx/gfx_gol_progress_bar.h"

// internal functions

/*********************************************************************
* Function: void PbWordToString(uint16_t value, GFX_XCHAR* buffer)
*
* Input: value  - value to be converted (from 0 - 100)
*        buffer - buffer receiving string (must be at least 5 uint8_ts)
*
* Output: none
*
* Overview: converts int16_t into string with % at the end
*
********************************************************************/
void PbWordToString(uint16_t value, GFX_XCHAR *buffer)
{
    uint16_t    result;
    uint8_t    pos;

    if(value > 99)
    {
        buffer[0] = '1';
        buffer[1] = '0';
        buffer[2] = '0';
        buffer[3] = '%';
        buffer[4] = 0;
        return;
    }

    pos = 0;
    result = value / 10;
    if(result)
        buffer[pos++] = result + '0';
    result = value - 10 * result;

    buffer[pos++] = result + '0';
    buffer[pos++] = '%';
    buffer[pos++] = 0;
}

/*********************************************************************
*
********************************************************************/
void GFX_GOL_ProgressBarBackGroundSet(GFX_GOL_OBJ_HEADER *pObjectHeader)
{
#if !defined (GFX_CONFIG_ALPHABLEND_DISABLE)
    // when alpha blending is enabled
    // set up the background info
    GFX_AlphaBlendingValueSet(pObjectHeader->pGolScheme->AlphaValue);
    GFX_FillStyleSet(pObjectHeader->pGolScheme->fillStyle);
    GFX_BackgroundTypeSet(pObjectHeader->pGolScheme->CommonBkType);

#else
    // progress bar does not support gradient
    // therefore, for other backgrounds, flat color is used

    GFX_FillStyleSet(GFX_FILL_STYLE_COLOR);
    GFX_BackgroundTypeSet(GFX_BACKGROUND_COLOR);
#endif
    GFX_BackgroundSet(pObjectHeader->pGolScheme->CommonBkLeft,
                      pObjectHeader->pGolScheme->CommonBkTop,
                      pObjectHeader->pGolScheme->pCommonBkImage,
                      pObjectHeader->pGolScheme->CommonBkColor);

}

// *****************************************************************************
/*  Function:
    GFX_GOL_PROGRESSBAR  *GFX_GOL_ProgressBarCreate(
                                uint16_t            ID,
                                uint16_t            left,
                                uint16_t            top,
                                uint16_t            right,
                                uint16_t            bottom,
                                uint16_t            state,
                                uint16_t            pos,
                                uint16_t            range,
                                GFX_GOL_OBJ_SCHEME  *pScheme)

    Summary:
        This function creates a GFX_GOL_PROGRESSBAR object with the
        parameters given. It automatically attaches the new object
        into a global linked list of objects and returns the address
        of the object.

    Description:
        This function creates a GFX_GOL_PROGRESSBAR object with the
        parameters given. It automatically attaches the new object
        into a global linked list of objects and returns the address
        of the object.

        This function returns the pointer to the newly created object.
        If the object is not successfully created, it returns NULL.

        The behavior of GFX_GOL_RadioButtonCreate() will be undefined
        if one of the following is true:
        - left >= right
        - top >= bottom
        - pos > range
        - range = 0
        - pScheme is not pointing to a GFX_GOL_OBJ_SCHEME
        - pText is an unterminated string

*/
// *****************************************************************************
GFX_GOL_PROGRESSBAR  *GFX_GOL_ProgressBarCreate(
                                uint16_t            ID,
                                uint16_t            left,
                                uint16_t            top,
                                uint16_t            right,
                                uint16_t            bottom,
                                uint16_t            state,
                                uint16_t            pos,
                                uint16_t            range,
                                GFX_GOL_OBJ_SCHEME  *pScheme)
{
    GFX_GOL_PROGRESSBAR *pObject = NULL;

    pObject = (GFX_GOL_PROGRESSBAR *)GFX_malloc(sizeof(GFX_GOL_PROGRESSBAR));
    if(pObject == NULL)
        return (pObject);

    pObject->hdr.ID         = ID;
    pObject->hdr.pNxtObj    = NULL;
    pObject->hdr.type       = GFX_GOL_PROGRESSBAR_TYPE;
    pObject->hdr.left       = left;
    pObject->hdr.top        = top;
    pObject->hdr.right      = right;
    pObject->hdr.bottom     = bottom;
    pObject->hdr.state      = state;
    pObject->hdr.DrawObj    = GFX_GOL_ProgressBarDraw;      // draw function
    pObject->hdr.FreeObj    = NULL;                         // free function
    pObject->hdr.actionGet  = GFX_GOL_ProgressBarActionGet; // action get function
    pObject->hdr.actionSet  = NULL;
    pObject->range          = 100;                          // initialize
    pObject->prevPos        = 0;                            // initialize
  
    if (range != 0)
        GFX_GOL_ProgressBarRangeSet(pObject, range);
    
    GFX_GOL_ProgressBarPositionSet(pObject, pos);

    // Set the style scheme to be used
    pObject->hdr.pGolScheme = (GFX_GOL_OBJ_SCHEME *)pScheme;


    GFX_GOL_ObjectAdd((GFX_GOL_OBJ_HEADER *)pObject);
    
    return (pObject);
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_ProgressBarPositionSet(
                                GFX_GOL_PROGRESSBAR *pObject,
                                uint16_t position)

    Summary:
        This function sets the position of the progress bar.

    Description:
        This function sets the position of the progress bar.

        The value used for the position should be within the range
        set for the object.

        Function will have an undefined behavior if the position
        is outside the range.

*/
// *****************************************************************************
void GFX_GOL_ProgressBarPositionSet(
                                GFX_GOL_PROGRESSBAR *pObject,
                                uint16_t position)
{
    if(pObject->range < position)
        position = pObject->range;

    pObject->pos = position;
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_ProgressBarRangeSet(
                                GFX_GOL_PROGRESSBAR *pObject,
                                uint16_t range)

    Summary:
        This function sets the range of the progress bar.

    Description:
        This function sets the range of the progress bar.
        When the range is modified, object must be completely redrawn to
        reflect the change.

*/
// *****************************************************************************
void GFX_GOL_ProgressBarRangeSet(
                                GFX_GOL_PROGRESSBAR *pObject,
                                uint16_t range)
{
    // range cannot be assigned a zero value
    if(range != 0)
        pObject->range = range;

    if (pObject->pos > range)
    {
        pObject->pos = range;
        pObject->prevPos = range;
    }
}

// *****************************************************************************
/*  Function:
    GFX_GOL_TRANSLATED_ACTION GFX_GOL_ProgressBarActionGet(
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
GFX_GOL_TRANSLATED_ACTION GFX_GOL_ProgressBarActionGet(
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage)
{
    GFX_GOL_PROGRESSBAR *pPb;

    pPb = (GFX_GOL_PROGRESSBAR *)pObject;


    // Evaluate if the message is for the progress bar
    // Check if disabled first
    if(GFX_GOL_ObjectStateGet(pPb, GFX_GOL_PROGRESSBAR_DISABLED_STATE))
        return (GFX_GOL_OBJECT_ACTION_INVALID);

#ifndef GFX_CONFIG_USE_TOUCHSCREEN_DISABLE
    if(pMessage->type == TYPE_TOUCHSCREEN)
    {
        // Check if it falls in the progress bar border
        if
        (
            (pPb->hdr.left   < pMessage->param1) &&
            (pPb->hdr.right  > pMessage->param1) &&
            (pPb->hdr.top    < pMessage->param2) &&
            (pPb->hdr.bottom > pMessage->param2)
        )
        {
            return (GFX_GOL_PROGRESSBAR_ACTION_SELECTED);
        }
    }

#endif
    return (GFX_GOL_OBJECT_ACTION_INVALID);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_GOL_ProgressBarDraw(void *pObject)

    Summary:
        This function renders the object on the screen based on the
        current state of the object.

    Description:
        This function renders the object on the screen based on the
        current state of the object. Location of the object is
        determined by the left, top, right and bottom parameters.
        The colors used are dependent on the state of the object.
        The font used is determined by the style scheme set.

        When rendering objects of the same type, each object
        must be rendered completely before the rendering of the
        next object is started. This is to avoid incomplete
        object rendering.

        Normally, application will just call GFX_GOL_ObjectListDraw()
        to allow the Graphics Library to manage all object rendering.
        See GFX_GOL_ObjectListDraw() for more information on object rendering.

*/
// *****************************************************************************
GFX_STATUS GFX_GOL_ProgressBarDraw(void *pObject)
{
    typedef enum
    {
        DRAW_PROGRESSBAR_REMOVE,
        DRAW_PROGRESSBAR_PANEL_PREP,
        DRAW_PROGRESSBAR_PANEL_DRAW,
        DRAW_PROGRESSBAR_BAR_SET,
        DRAW_PROGRESSBAR_BAR_DRAW,
        DRAW_PROGRESSBAR_TEXT_DRAW1,
        DRAW_PROGRESSBAR_TEXT_DRAW2,
        DRAW_PROGRESSBAR_TEXT_DRAW3
    } PB_DRAW_STATES;

    static PB_DRAW_STATES   state = DRAW_PROGRESSBAR_REMOVE;
    static int32_t          x1, x2;
    static int16_t          intLeft, intTop, intRight, intBottom;
    static uint16_t         embossSize;
    static GFX_XCHAR        text[5] = {'0','0','%',0};
    GFX_GOL_PROGRESSBAR     *pPb;

    pPb = (GFX_GOL_PROGRESSBAR *)pObject;

    while(1)
    {
        if (GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
            return (GFX_STATUS_FAILURE);
    
        switch(state)
        {
            case DRAW_PROGRESSBAR_PANEL_DRAW:

                if(GFX_GOL_PanelDraw() != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }

                state = DRAW_PROGRESSBAR_BAR_SET;
                break;

            case DRAW_PROGRESSBAR_REMOVE:
#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                GFX_DRIVER_SetupDrawUpdate( pPb->hdr.left,
                                            pPb->hdr.top,
                                            pPb->hdr.right,
                                            pPb->hdr.bottom);
#endif
                if(GFX_GOL_ObjectStateGet(
                        pPb,
                        GFX_GOL_PROGRESSBAR_HIDE_STATE))
                {
                    GFX_ColorSet(pPb->hdr.pGolScheme->CommonBkColor);

                    if (GFX_GOL_ObjectHideDraw(&(pPb->hdr)) != 
                            GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }

#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                    GFX_DRIVER_CompleteDrawUpdate(  pPb->hdr.left,
                                                    pPb->hdr.top,
                                                    pPb->hdr.right,
                                                    pPb->hdr.bottom);
#endif
                    return (GFX_STATUS_SUCCESS);
                }
                else if (GFX_GOL_ObjectStateGet(
                        pPb,
                        GFX_GOL_PROGRESSBAR_DRAW_BAR_STATE))
                {
                    state = DRAW_PROGRESSBAR_BAR_SET;
                    break;
                }
                else if (GFX_GOL_ObjectStateGet(
                        pPb,
                        GFX_GOL_PROGRESSBAR_DRAW_STATE))
                {
                    // since this is a redraw set the previous position to 0
                    pPb->prevPos = 0;

                    // set up the background
                    GFX_GOL_PanelBackgroundSet(&pPb->hdr);

                    state = DRAW_PROGRESSBAR_PANEL_PREP;
                    // no break here since the next state is what we want
                }
                else
                {
                    // do nothing, states does not mean anything
                    return (GFX_STATUS_SUCCESS);
                }
    
            case DRAW_PROGRESSBAR_PANEL_PREP:
                GFX_GOL_PanelParameterSet
                (
                    pPb->hdr.left,
                    pPb->hdr.top,
                    pPb->hdr.right,
                    pPb->hdr.bottom,
                    0,
                    pPb->hdr.pGolScheme->Color0,
                    pPb->hdr.pGolScheme->EmbossDkColor,
                    pPb->hdr.pGolScheme->EmbossLtColor,
                    NULL,
                    pPb->hdr.pGolScheme->fillStyle,
                    pPb->hdr.pGolScheme->EmbossSize
                );
                state = DRAW_PROGRESSBAR_PANEL_DRAW;
                break;
    
            case DRAW_PROGRESSBAR_BAR_SET:

                embossSize = pPb->hdr.pGolScheme->EmbossSize;

            	if(GFX_GOL_ObjectStateGet(
                        pPb,
                        GFX_GOL_PROGRESSBAR_VERTICAL_STATE))
                {
    	            x1 = ((uint32_t) (pPb->range-pPb->prevPos)) *   \
                            (pPb->hdr.bottom - pPb->hdr.top - (2 * embossSize))
                            / pPb->range;
    	            x2 = ((uint32_t) (pPb->range-pPb->pos)) *       \
                            (pPb->hdr.bottom - pPb->hdr.top - (2 * embossSize))
                            / pPb->range;
                    
    	            x1 += (pPb->hdr.top + embossSize);
    	            x2 += (pPb->hdr.top + embossSize);
    	        }
    	        else
    	        { 
    	            x1 = ((uint32_t) pPb->pos) *        
                            (pPb->hdr.right - pPb->hdr.left - (2 * embossSize))
                            / pPb->range;
    	            x2 = ((uint32_t) pPb->prevPos) *    
                            (pPb->hdr.right - pPb->hdr.left - (2 * embossSize))
                            / pPb->range;

    	            x1 += (pPb->hdr.left + embossSize);
    	            x2 += (pPb->hdr.left + embossSize);
             	}   

                if(pPb->prevPos > pPb->pos)
                {
    	            GFX_ColorSet(pPb->hdr.pGolScheme->Color0);
                    if(GFX_GOL_ObjectStateGet(
                            pPb,
                            GFX_GOL_PROGRESSBAR_VERTICAL_STATE))
                    {
                        intLeft   = pPb->hdr.left   + embossSize;
                        intTop    = x1;
                        intRight  = pPb->hdr.right  - embossSize;
                        intBottom = x2;
                    } 
                    else
                    {
                        intLeft   = x1; 
                        intTop    = pPb->hdr.top    + embossSize;
                        intRight  = x2;
                        intBottom = pPb->hdr.bottom - embossSize;
                    }
                }
                else
                {
                    GFX_ColorSet(pPb->hdr.pGolScheme->Color1);
                    if(GFX_GOL_ObjectStateGet(
                            pPb,
                            GFX_GOL_PROGRESSBAR_VERTICAL_STATE))
        	    {
                        intLeft   = pPb->hdr.left   + embossSize;
                        intTop    = x2;
                        intRight  = pPb->hdr.right  - embossSize;
                        intBottom = x1;
    	    	    } 
                    else
                    {
                        intLeft   = x2; 
                        intTop    = pPb->hdr.top    + embossSize;
                        intRight  = x1;
                        intBottom = pPb->hdr.bottom - embossSize;
        	        }     
                }

                // from here on, all are primitive rendering so we set
                // the background information here once
                GFX_GOL_ProgressBarBackGroundSet(&pPb->hdr);
                
                state = DRAW_PROGRESSBAR_BAR_DRAW;

            case DRAW_PROGRESSBAR_BAR_DRAW:
   
                if(GFX_BarDraw(
                        intLeft,
                        intTop,
                        intRight,
                        intBottom) != GFX_STATUS_SUCCESS)
                {
   	                return (GFX_STATUS_FAILURE);
                }
    
                if (GFX_GOL_ObjectStateGet(
                        pPb,
                        GFX_GOL_PROGRESSBAR_NOPROGRESS_STATE) ||
                    (pPb->hdr.pGolScheme->pFont == NULL))
                {
                    state = DRAW_PROGRESSBAR_REMOVE;
                    pPb->prevPos = pPb->pos;
#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
	            GFX_DRIVER_CompleteDrawUpdate(  pPb->hdr.left,
                                                    pPb->hdr.top,
	                                            pPb->hdr.right,
	                                            pPb->hdr.bottom);
#endif
                    return (GFX_STATUS_SUCCESS);
                }

                if(GFX_GOL_ObjectStateGet(
                        pPb,
                        GFX_GOL_PROGRESSBAR_VERTICAL_STATE))
       	        {
    	            GFX_ColorSet(pPb->hdr.pGolScheme->Color0);
                    intLeft   = pPb->hdr.left + embossSize;
                    intTop    = (pPb->hdr.top + pPb->hdr.bottom -
                                    GFX_TextStringHeightGet(
                                        pPb->hdr.pGolScheme->pFont)) >> 1;
    	            intRight  = pPb->hdr.right - embossSize;
    	            intBottom = x2;
                }
    		else
    		{
    	            GFX_ColorSet(pPb->hdr.pGolScheme->Color1);
                    intLeft   = (pPb->hdr.left + pPb->hdr.right -
                                    GFX_TextStringWidthGet(
                                        text,
                                        pPb->hdr.pGolScheme->pFont)) >> 1;
    	            intTop    = pPb->hdr.top + embossSize;
    	            intRight  = x1;
    	            intBottom = pPb->hdr.bottom - embossSize;
    		}	

                state = DRAW_PROGRESSBAR_TEXT_DRAW1;
    
            case DRAW_PROGRESSBAR_TEXT_DRAW1:
                if ((intLeft <= intRight) && (intTop <= intBottom))
                {
                    if(GFX_BarDraw(
                            intLeft,
                            intTop,
                            intRight,
                            intBottom) != GFX_STATUS_SUCCESS)
                    {
                            return (GFX_STATUS_FAILURE);
                    }
                }

            	if(GFX_GOL_ObjectStateGet(
                        pPb,
                        GFX_GOL_PROGRESSBAR_VERTICAL_STATE))
       	        {
    	            GFX_ColorSet(pPb->hdr.pGolScheme->Color1);
                    intLeft   = pPb->hdr.left + embossSize;
                    intTop    = x2;
    	            intRight  = pPb->hdr.right - embossSize;
    	            intBottom = (pPb->hdr.top + pPb->hdr.bottom +
                    GFX_TextStringHeightGet(pPb->hdr.pGolScheme->pFont)) >> 1;
                }
    		else 
                {
    	            GFX_ColorSet(pPb->hdr.pGolScheme->Color0);
                    intLeft   = x1;
    	            intTop    = pPb->hdr.top + embossSize;
    	            intRight  = (pPb->hdr.left + pPb->hdr.right +
                                    GFX_TextStringWidthGet(
                                        text,
                                        pPb->hdr.pGolScheme->pFont)) >> 1;
    	            intBottom = pPb->hdr.bottom - embossSize;
                }
                state = DRAW_PROGRESSBAR_TEXT_DRAW2;
    
            case DRAW_PROGRESSBAR_TEXT_DRAW2:
                if ((intLeft <= intRight) && (intTop <= intBottom))
                {
                    if(GFX_BarDraw(
                            intLeft,
                            intTop,
                            intRight,
                            intBottom) != GFX_STATUS_SUCCESS)
                    {
                            return (GFX_STATUS_FAILURE);
                    }
                }
    
                PbWordToString((uint32_t) pPb->pos * 100 / pPb->range, text);
                GFX_ColorSet(pPb->hdr.pGolScheme->TextColor0);
    
                GFX_FontSet(pPb->hdr.pGolScheme->pFont);
                state = DRAW_PROGRESSBAR_TEXT_DRAW3;
    
            case DRAW_PROGRESSBAR_TEXT_DRAW3:

                if (GFX_TextStringBoxDraw(
                    (pPb->hdr.left + pPb->hdr.right -
                        GFX_TextStringWidthGet(
                            text,
                            pPb->hdr.pGolScheme->pFont)) >> 1,
                    (pPb->hdr.top + pPb->hdr.bottom - 
                        GFX_TextStringHeightGet(
                            pPb->hdr.pGolScheme->pFont)) >> 1,
                    GFX_TextStringWidthGet(
                        text,
                        pPb->hdr.pGolScheme->pFont),
                    GFX_TextStringHeightGet(
                        pPb->hdr.pGolScheme->pFont),
                    text,
                    0,
                    GFX_ALIGN_CENTER) != GFX_STATUS_SUCCESS)
                        return (GFX_STATUS_FAILURE);

                pPb->prevPos = pPb->pos;
                state = DRAW_PROGRESSBAR_REMOVE;
#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                GFX_DRIVER_CompleteDrawUpdate(  pPb->hdr.left,
                                                pPb->hdr.top,
                                                pPb->hdr.right,
                                                pPb->hdr.bottom);
#endif
                return (GFX_STATUS_SUCCESS);
        } // end of switch()
    } // end of while(1)
}


