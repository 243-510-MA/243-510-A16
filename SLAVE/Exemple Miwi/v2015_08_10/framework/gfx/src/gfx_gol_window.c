/*******************************************************************************
 Module for Microchip Graphics Library - Graphic Object Layer

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_gol_window.c

  Summary:
    This implements the window object of the GOL.

  Description:
    Refer to Microchip Graphics Library for complete documentation of the
    Window Object.
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
#include "gfx/gfx_gol_window.h"

// DOM-IGNORE-BEGIN
// Height of the title bar
#ifndef GOL_OBJECT_WINDOW_TITLE_HEIGHT
#define GOL_OBJECT_WINDOW_TITLE_HEIGHT    35
#endif
// DOM-IGNORE-END

// *****************************************************************************
/*  Function:
    GFX_GOL_WINDOW  *GFX_GOL_WindowCreate(
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
        This function creates a GFX_GOL_WINDOW object with the parameters
        given. It automatically attaches the new object into a global
        linked list of objects and returns the address of the object.

    Description:
        This function creates a GFX_GOL_WINDOW object with the parameters
        given. It automatically attaches the new object into a global
        linked list of objects and returns the address of the object.

        This function returns the pointer to the newly created object.
        If the object is not successfully created, it returns NULL.

*/
// *****************************************************************************
GFX_GOL_WINDOW  *GFX_GOL_WindowCreate(
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
{
    GFX_GOL_WINDOW  *pW;

    pW = (GFX_GOL_WINDOW *)GFX_malloc(sizeof(GFX_GOL_WINDOW));
    if(pW == NULL)
        return (pW);

    pW->hdr.ID      = ID;
    pW->hdr.pNxtObj = NULL;
    pW->hdr.type    = GFX_GOL_WINDOW_TYPE;
    pW->hdr.left    = left;
    pW->hdr.top     = top;
    pW->hdr.right   = right;
    pW->hdr.bottom  = bottom;
    pW->pImage      = pImage;
    pW->pText       = pText;
    pW->hdr.state   = state;

    pW->hdr.DrawObj = GFX_GOL_WindowDraw;       // draw function
    pW->hdr.FreeObj = NULL;                     // free function
    pW->hdr.actionGet = GFX_GOL_WindowActionGet;// action get function
    pW->hdr.actionSet = NULL;                   // default action function

    // Set the style scheme to be used
    pW->hdr.pGolScheme = pScheme;

    if(pText != NULL)
        pW->textHeight = GFX_TextStringHeightGet(pW->hdr.pGolScheme->pFont);
    else
        pW->textHeight = 0;

    // set alignment
    GFX_GOL_WindowTextAlignmentSet(pW, alignment);
    
    GFX_GOL_ObjectAdd((GFX_GOL_OBJ_HEADER *)pW);

    return (pW);
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_WindowTextSet(
                                GFX_GOL_WINDOW *pObject,
                                GFX_XCHAR *pText)

    Summary:
        This function sets the address of the current text
        string used by the object.

    Description:
        This function sets the address of the current text
        string used by the object.

*/
// *****************************************************************************
void GFX_GOL_WindowTextSet(GFX_GOL_WINDOW *pObject, GFX_XCHAR *pText)
{
    pObject->pText = pText;
    pObject->textHeight = GFX_TextStringHeightGet(
                                pObject->hdr.pGolScheme->pFont);
}

// *****************************************************************************
/*  Function:
    GFX_ALIGNMENT GFX_GOL_WindowTextAlignmentGet(
                                GFX_GOL_WINDOW *pObject)

    Summary:
        This function returns the text alignment of the text
        string used by the object.

    Description:
        This function returns the text alignment of the text
        string used by the object.

*/
// *****************************************************************************
GFX_ALIGNMENT __attribute__ ((always_inline)) GFX_GOL_WindowTextAlignmentGet(
                                GFX_GOL_WINDOW *pObject)
{
    return (pObject->alignment);
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_WindowTextAlignmentSet(
                                GFX_GOL_WINDOW *pObject,
                                GFX_ALIGNMENT alignment)

    Summary:
        This function sets the text alignment of the text
        string used by the object.

    Description:
        This function sets the text alignment of the text
        string used by the object.

*/
// *****************************************************************************
void __attribute__ ((always_inline)) GFX_GOL_WindowTextAlignmentSet(
                                GFX_GOL_WINDOW *pObject,
                                GFX_ALIGNMENT alignment)
{
    pObject->alignment = alignment;
}

// *****************************************************************************
/*  Function:
    GFX_GOL_TRANSLATED_ACTION GFX_GOL_WindowActionGet(
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
GFX_GOL_TRANSLATED_ACTION GFX_GOL_WindowActionGet(
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage)
{
    GFX_GOL_WINDOW *pW;
    uint16_t embossSize;

    pW = (GFX_GOL_WINDOW *)pObject;

    // Evaluate if the message is for the window
    // Check if disabled first
    if(GFX_GOL_ObjectStateGet(pW, GFX_GOL_WINDOW_DISABLED_STATE))
        return (GFX_GOL_OBJECT_ACTION_INVALID);

#ifndef GFX_CONFIG_USE_TOUCHSCREEN_DISABLE
    if(pMessage->type == TYPE_TOUCHSCREEN)
    {

        embossSize = pW->hdr.pGolScheme->EmbossSize;

        // Check if it falls in the title bar area
        if(
            (pW->hdr.left  < pMessage->param1) &&
            (pW->hdr.right > pMessage->param1) &&
            (pW->hdr.top   < pMessage->param2) &&
            (pW->hdr.top + GOL_OBJECT_WINDOW_TITLE_HEIGHT + embossSize >
                pMessage->param2))
        {
            return (GFX_GOL_WINDOW_ACTION_TITLE);
        }

        // Check if it falls in the client area
        if(
            (pW->hdr.left   + embossSize < pMessage->param1) &&
            (pW->hdr.right  - embossSize > pMessage->param1) &&
            (pW->hdr.top    + embossSize +
                    GOL_OBJECT_WINDOW_TITLE_HEIGHT  < pMessage->param2) &&
            (pW->hdr.bottom - embossSize > pMessage->param2)
        )
        {
            return (GFX_GOL_WINDOW_ACTION_CLIENT);
        }
    }

#endif
    return (GFX_GOL_OBJECT_ACTION_INVALID);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_GOL_WindowDraw(void *pObject)

    Summary:
        This function renders the object on the screen based on the
        current state of the object.

    Description:
        This function renders the object on the screen based on the
        current state of the object. Location of the object is
        determined by the left, top, right and bottom parameters.
        The colors used are dependent on the state of the object.
        The font used is determined by the style scheme set.

        The text on the face of the GFX_GOL_WINDOW is drawn on top of
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
GFX_STATUS GFX_GOL_WindowDraw(void *pObject)
{
    typedef enum
    {
        WND_DRAW_IDLE,
        WND_DRAW_HIDE,
        WND_DRAW_TITLE_BAR_SET,
        WND_DRAW_TITLE_BAR,
        WND_DRAW_TITLE_BAR_BITMAP,
        WND_DRAW_SET_TITLE_BAR_TEXT,
        WND_DRAW_TITLE_BAR_TEXT,
        WND_DRAW_SET_CLIENT_AREA,
        WND_DRAW_CLIENT_AREA
    } WND_DRAW_STATES;

    static int16_t         temp = 0;
           GFX_GOL_WINDOW  *pW;
    static WND_DRAW_STATES state = WND_DRAW_IDLE;
    static uint16_t        embossSize;

    pW = (GFX_GOL_WINDOW *)pObject;

    while(1)
    {
    
        if (GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
            return (GFX_STATUS_FAILURE);

        switch(state)
        {
            case WND_DRAW_IDLE:
#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                GFX_DRIVER_SetupDrawUpdate( pW->hdr.left,
                                            pW->hdr.top,
                                            pW->hdr.right,
                                            pW->hdr.bottom);
#endif
                if(GFX_GOL_ObjectStateGet(
                        pW,
                        GFX_GOL_WINDOW_HIDE_STATE))
                {
                    GFX_ColorSet(pW->hdr.pGolScheme->CommonBkColor);
                    state = WND_DRAW_HIDE;
                    // no break; here so it falls through to WND_DRAW_HIDE
                }

                embossSize = pW->hdr.pGolScheme->EmbossSize;
                if(GFX_GOL_ObjectStateGet(
                        pW,
                        GFX_GOL_WINDOW_DRAW_CLIENT_STATE))
                {
                    state = WND_DRAW_SET_CLIENT_AREA;
                    break;
                }
                else if(GFX_GOL_ObjectStateGet(
                        pW,
                        GFX_GOL_WINDOW_DRAW_TITLE_STATE))
                {
                    state = WND_DRAW_TITLE_BAR_SET;
                    break;
                }
                else
                {
#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                    GFX_DRIVER_CompleteDrawUpdate(  pW->hdr.left,
                                                    pW->hdr.top,
                                                    pW->hdr.right,
                                                    pW->hdr.bottom);
#endif
                    return (GFX_STATUS_SUCCESS);
                }

            case WND_DRAW_HIDE:

                // Hide the object (remove from screen)
                if (GFX_GOL_ObjectHideDraw(&(pW->hdr)) != GFX_STATUS_SUCCESS)
                    return (GFX_STATUS_FAILURE);

                state = WND_DRAW_IDLE;

#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                GFX_DRIVER_CompleteDrawUpdate(   pW->hdr.left,
                                                 pW->hdr.top,
                                                 pW->hdr.right,
                                                 pW->hdr.bottom);
#endif
                return (GFX_STATUS_SUCCESS);
				
            case WND_DRAW_SET_CLIENT_AREA:
                
                GFX_LineStyleSet(GFX_LINE_STYLE_THIN_SOLID);
                
                // set panel parameters
                GFX_GOL_PanelParameterSet(
                        pW->hdr.left,
                        pW->hdr.top,
                        pW->hdr.right,
                        pW->hdr.bottom,
                        0,
                        pW->hdr.pGolScheme->CommonBkColor,
                        pW->hdr.pGolScheme->EmbossLtColor,
                        pW->hdr.pGolScheme->EmbossDkColor,
                        NULL,
                        pW->hdr.pGolScheme->fillStyle,
                        embossSize);

                // set the background information
                GFX_GOL_PanelBackgroundSet(&pW->hdr);

                state = WND_DRAW_CLIENT_AREA;

            case WND_DRAW_CLIENT_AREA:

                if(GFX_GOL_PanelDraw() != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }

                if(GFX_GOL_ObjectStateGet(
                        pW,
                        GFX_GOL_WINDOW_DRAW_TITLE_STATE))
                {
                    state = WND_DRAW_TITLE_BAR_SET;
                }
                else
		{
                    state = WND_DRAW_IDLE;
#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                    GFX_DRIVER_CompleteDrawUpdate(  pW->hdr.left,
                                                    pW->hdr.top,
                                                    pW->hdr.right,
                                                    pW->hdr.bottom);
#endif
                    return (GFX_STATUS_SUCCESS);
                }
                // no break here since the next state is what we want

            case WND_DRAW_TITLE_BAR_SET:

                if(!GFX_GOL_ObjectStateGet(
                        pW,
                        GFX_GOL_WINDOW_DISABLED_STATE))
                {
                    if(GFX_GOL_ObjectStateGet(
                            pW,
                            GFX_GOL_WINDOW_FOCUSED_STATE))
                    {
                        GFX_ColorSet(pW->hdr.pGolScheme->Color1);
                    }
                    else
                    {
                        GFX_ColorSet(pW->hdr.pGolScheme->Color0);
                    }
                }
                else
                {
                    GFX_ColorSet(pW->hdr.pGolScheme->ColorDisabled);
                }

                GFX_GOL_ObjectBackGroundSet(&pW->hdr);
                state = WND_DRAW_TITLE_BAR;

            case WND_DRAW_TITLE_BAR:

                if(GFX_RectangleFillDraw(
                            pW->hdr.left  + embossSize,
                            pW->hdr.top   + embossSize,
                            pW->hdr.right - embossSize,
                            pW->hdr.top   + embossSize +
                                GOL_OBJECT_WINDOW_TITLE_HEIGHT)
                        != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }

                state = WND_DRAW_TITLE_BAR_BITMAP;
                break;

            case WND_DRAW_TITLE_BAR_BITMAP:
                if(pW->pImage != NULL)
                {
                    if(GFX_ImageDraw(
                            pW->hdr.left + embossSize,
                            pW->hdr.top  + embossSize +
                            ((   GOL_OBJECT_WINDOW_TITLE_HEIGHT -
                                 GFX_ImageHeightGet(pW->pImage)
                            ) >> 1),
                            pW->pImage) != GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }
                }

                if(pW->pText != NULL)
                {
                    state = WND_DRAW_SET_TITLE_BAR_TEXT;
                        break;
                }
                else
                {
                    state = WND_DRAW_IDLE;

#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                    GFX_DRIVER_CompleteDrawUpdate(  pW->hdr.left,
                                                    pW->hdr.top,
                                                    pW->hdr.right,
                                                    pW->hdr.bottom);
#endif
                    return (1);
                }

            case WND_DRAW_SET_TITLE_BAR_TEXT:

                GFX_FontSet(pW->hdr.pGolScheme->pFont);

                if(!GFX_GOL_ObjectStateGet(
                        pW,
                        GFX_GOL_WINDOW_DISABLED_STATE))
                {
                    if(GFX_GOL_ObjectStateGet(pW, GFX_GOL_WINDOW_FOCUSED_STATE))
                    {
                        GFX_ColorSet(pW->hdr.pGolScheme->TextColor1);
                    }
                    else
                    {
                        GFX_ColorSet(pW->hdr.pGolScheme->TextColor0);
                    }
                }
                else
                {
                    GFX_ColorSet(pW->hdr.pGolScheme->TextColorDisabled);
                }

                // get the starting position of the title in x direction
                temp = pW->hdr.left + embossSize;

                // account the image if present
                if(pW->pImage != NULL)
                {
                    temp += GFX_ImageWidthGet(pW->pImage);
                }

                state = WND_DRAW_TITLE_BAR_TEXT;

            case WND_DRAW_TITLE_BAR_TEXT:
                
                // draw the title
                if (GFX_TextStringBoxDraw(
                        temp,
                        pW->hdr.top + embossSize,
                        pW->hdr.right - embossSize - temp + 1,  // width
                        GOL_OBJECT_WINDOW_TITLE_HEIGHT,         // height
                        pW->pText,
                        0,
                        pW->alignment) != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }

                state = WND_DRAW_IDLE;
#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                GFX_DRIVER_CompleteDrawUpdate(   pW->hdr.left,
                                                 pW->hdr.top,
                                                 pW->hdr.right,
                                                 pW->hdr.bottom);
#endif
		return (GFX_STATUS_SUCCESS);

        }   //end of switch
    }       //end of while
}

