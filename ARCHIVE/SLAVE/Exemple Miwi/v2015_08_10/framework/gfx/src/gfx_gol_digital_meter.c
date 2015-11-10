/*******************************************************************************
 Module for Microchip Graphics Library - Graphic Object Layer

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_gol_digital_meter.c

  Summary:
    This implements the digital meter object of the GOL.

  Description:
    Refer to Microchip Graphics Library for complete documentation of the
    Digital Meter Object.
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
#include "gfx/gfx_gol_digital_meter.h"


/************************************************************************************
* Function: NumberToString(uint32_t Value,XCHAR *pText, uint8_t NoOfDigits, uint8_t DotPos )
*
* Notes: convert the number to string
*
*************************************************************************************/
static void NumberToString(
                                uint32_t Value,
                                GFX_XCHAR *pText,
                                uint8_t NoOfDigits,
                                uint8_t DotPos)
{
    uint8_t    i;
    uint8_t    bIndex;

    for(bIndex = 0; bIndex < NoOfDigits; bIndex++)
    {
        pText[NoOfDigits - bIndex - 1] = '0' + (Value % 10);
        Value /= 10;
    }

    if(DotPos != 0 && DotPos <= NoOfDigits)
    {
        for(i = 0; i < DotPos; i++)
        {
            pText[NoOfDigits - i] = pText[NoOfDigits - 1 - i];
        }

        pText[NoOfDigits - DotPos] = '.';
        pText[NoOfDigits + 1] = '\0';
    }

    // If dot position is 0 or greater than number of digits,
    // then don't put dot in the display
    else
    {
        pText[NoOfDigits] = '\0';
    }
}

// *****************************************************************************
/*  Function:
    GFX_GOL_DIGITALMETER  *GFX_GOL_DigitalMeterCreate(
                                    uint16_t ID,
                                    uint16_t left,
                                    uint16_t top,
                                    uint16_t right,
                                    uint16_t bottom,
                                    uint16_t state,
                                    uint32_t value,
                                    uint8_t NoOfDigits,
                                    uint8_t DotPos,
                                    GFX_ALIGNMENT alignment
                                    GFX_GOL_OBJ_SCHEME *pScheme)

    Summary:
        This function creates a GFX_GOL_DIGITALMETER object with the parameters
        given. It automatically attaches the new object into a global
        linked list of objects and returns the address of the object.

    Description:
        This function creates a GFX_GOL_DIGITALMETER object with the parameters
        given. It automatically attaches the new object into a global
        linked list of objects and returns the address of the object.

        This function returns the pointer to the newly created object.
        If the object is not successfully created, it returns NULL.

*/
// *****************************************************************************
GFX_GOL_DIGITALMETER *GFX_GOL_DigitalMeterCreate(
                                uint16_t            ID,
                                uint16_t            left,
                                uint16_t            top,
                                uint16_t            right,
                                uint16_t            bottom,
                                uint16_t            state,
                                uint32_t            value,
                                uint8_t             NoOfDigits,
                                uint8_t             DotPos,
                                GFX_ALIGNMENT       alignment,
                                GFX_GOL_OBJ_SCHEME  *pScheme)
{
    GFX_GOL_DIGITALMETER *pDm = NULL;

    pDm = GFX_malloc(sizeof(GFX_GOL_DIGITALMETER));

    if(pDm == NULL)
        return (pDm);

    pDm->hdr.ID         = ID;                       // unique id assigned for referencing
    pDm->hdr.pNxtObj    = NULL;                     // initialize pointer to NULL
    pDm->hdr.type       = GFX_GOL_DIGITALMETER_TYPE;// set object type
    pDm->hdr.left       = left;                     // left,top corner
    pDm->hdr.top        = top;
    pDm->hdr.right      = right;                    // right buttom corner
    pDm->hdr.bottom     = bottom;
    pDm->Cvalue         = value;                    // initial value to be displayed
    pDm->hdr.state      = state;                    // initial state of the object
    pDm->NoOfDigits     = NoOfDigits;               // number of digits to be displayed
    pDm->DotPos         = DotPos;                   // position of decimal point

    pDm->hdr.DrawObj    = GFX_GOL_DigitalMeterDraw;      // draw function
    pDm->hdr.FreeObj    = NULL;                          // free function
    pDm->hdr.actionGet  = GFX_GOL_DigitalMeterActionGet; // action get function
    pDm->hdr.actionSet  = NULL;                          // default action function

    // set the alignment
    GFX_GOL_DigitalMeterTextAlignmentSet(pDm, alignment);

    // set the color scheme to be used
    pDm->hdr.pGolScheme = pScheme;
    pDm->textHeight = 0;

    // Set the text height
    pDm->textHeight = GFX_TextStringHeightGet(pDm->hdr.pGolScheme->pFont);

    GFX_GOL_ObjectAdd((GFX_GOL_OBJ_HEADER *)pDm);

    return (pDm);
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_DigitalMeterValueSet(
                                GFX_GOL_DIGITALMETER *pObject,
                                int16_t value)

    Summary:
        This function sets the value of the meter.

    Description:
        This function sets the value of the meter. The value used
        should be within the range set for the object.  The new value
        is checked to be in the minimum and maximum value range.
        If the value set is less than the minimum value, the value
        that will be set is the minimum value. If the value set is
        more than the maximum value, then the value that will be
        set is the maximum value.

*/
// *****************************************************************************
void GFX_GOL_DigitalMeterValueSet(
                                GFX_GOL_DIGITALMETER *pObject,
                                int16_t value)
{
    // store the previous and current value to be displayed
    pObject->Pvalue = pObject->Cvalue;
    pObject->Cvalue = value;

    pObject->textHeight = GFX_TextStringHeightGet(
                            pObject->hdr.pGolScheme->pFont);
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_DigitalMeterIncrement(
                                GFX_GOL_DIGITALMETER *pObject,
                                uint16_t delta)

    Summary:
        This function increments the meter value by the delta
        value set.

    Description:
        This function increments the scroll bar position by the given
        delta value set. If the delta given exceeds the maximum value
        of the meter, the value will remain to be at maximum.

        Object must be redrawn after this function is called to reflect
        the changes to the object.

*/
// *****************************************************************************
void __attribute__ ((always_inline)) GFX_GOL_DigitalMeterIncrement(
                                GFX_GOL_DIGITALMETER *pObject,
                                uint16_t delta)
{
    // store the previous and current value to be displayed
    pObject->Pvalue = pObject->Cvalue;
    pObject->Cvalue += delta;
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_DigitalMeterDecrement(
                                GFX_GOL_DIGITALMETER *pObject,
                                uint16_t delta)

    Summary:
        This function decrements the meter value by the delta
        value set.

    Description:
        This function decrements the meter value by the given
        delta value set. If the delta given is less than the minimum
        value of the meter, the value will remain to be at minimum.

        Object must be redrawn after this function is called to reflect
        the changes to the object.

*/
// *****************************************************************************
void GFX_GOL_DigitalMeterDecrement(
                                GFX_GOL_DIGITALMETER *pObject,
                                uint16_t delta)
{
    // store the previous and current value to be displayed
    pObject->Pvalue = pObject->Cvalue;
    pObject->Cvalue -= delta;
}

// *****************************************************************************
/*  Function:
    GFX_GOL_TRANSLATED_ACTION GFX_GOL_DigitalMeterActionGet(
                                void *pObject, GFX_GOL_MESSAGE *pMessage)

    Summary:
        This function evaluates the message from a user if the
        message will affect the object or not.

    Description:
        This function evaluates the message from a user if the
        message will affect the object or not.

*/
// *****************************************************************************
GFX_GOL_TRANSLATED_ACTION GFX_GOL_DigitalMeterActionGet(
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage)
{

    GFX_GOL_DIGITALMETER *pDm;

    pDm = (GFX_GOL_DIGITALMETER *)pObject;

    // Evaluate if the message is for the static text
    // Check if disabled first
    if(GFX_GOL_ObjectStateGet(pDm, GFX_GOL_DIGITALMETER_DISABLED_STATE))
        return (GFX_GOL_OBJECT_ACTION_INVALID);

#ifndef GFX_CONFIG_USE_TOUCHSCREEN_DISABLE
    if(pMessage->type == TYPE_TOUCHSCREEN)
    {

        // Check if it falls in static text control borders
        if
        (
            (pDm->hdr.left   < pMessage->param1) &&
            (pDm->hdr.right  > pMessage->param1) &&
            (pDm->hdr.top    < pMessage->param2) &&
            (pDm->hdr.bottom > pMessage->param2)
        )
        {
            return (GFX_GOL_DIGITALMETER_ACTION_SELECTED);
        }
    }

#endif
    return (GFX_GOL_OBJECT_ACTION_INVALID);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_GOL_DigitalMeterDraw(void *pObject)

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
GFX_STATUS GFX_GOL_DigitalMeterDraw(void *pObject)
{
    typedef enum
    {
        DM_STATE_IDLE,
        DM_STATE_PANEL,
        DM_STATE_FRAME,
        DM_STATE_DRAW_FRAME,
        DM_STATE_INIT,
        DM_STATE_SETALIGN,
        DM_STATE_SETTEXT,
        DM_STATE_ERASETEXT,
        DM_STATE_DRAWTEXT,
        DM_STATE_WRAPUP
    } DM_DRAW_STATES;

    GFX_GOL_DIGITALMETER    *pDm = NULL;

    static DM_DRAW_STATES   state = DM_STATE_IDLE;
    static int16_t          charCtr = 0;
    static GFX_XCHAR        CurValue[GFX_GOL_DIGITALMETER_WIDTH];
    static GFX_XCHAR        PreValue[GFX_GOL_DIGITALMETER_WIDTH];
    static int16_t          textWidth = 0;
    static GFX_XCHAR        ch = 0, pch = 0;
    static uint16_t         charWidth;
           GFX_XCHAR        tempStr[2] = {0, 0};


    pDm = (GFX_GOL_DIGITALMETER *)pObject;

    while(1)
    {

        if (GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
            return (GFX_STATUS_FAILURE);
    
        switch(state)
        {
            case DM_STATE_DRAW_FRAME:
                if(GFX_RectangleDraw(
                        pDm->hdr.left,
                        pDm->hdr.top,
                        pDm->hdr.right,
                        pDm->hdr.bottom) != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }
                state = DM_STATE_INIT;
                break;

            case DM_STATE_IDLE:
    
#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                GFX_DRIVER_SetupDrawUpdate( pDm->hdr.left,
                                            pDm->hdr.top,
                                            pDm->hdr.right,
                                            pDm->hdr.bottom);
#endif

                if(GFX_GOL_ObjectStateGet(pDm, GFX_GOL_DIGITALMETER_HIDE_STATE))
                {
                    // Hide the GFX_GOL_BUTTON (remove from screen)
                    if (GFX_GOL_ObjectHideDraw(&(pDm->hdr)) != GFX_STATUS_SUCCESS)
                        return (GFX_STATUS_FAILURE);

#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                    GFX_DRIVER_CompleteDrawUpdate(  pDm->hdr.left,
                                                    pDm->hdr.top,
                                                    pDm->hdr.right,
                                                    pDm->hdr.bottom);
#endif
                    return (GFX_STATUS_SUCCESS);
                }

                // check if the whole object is to be redrawn
                if( GFX_GOL_ObjectStateGet(
                        pDm,
                        GFX_GOL_DIGITALMETER_DRAW_STATE))
                {
                    GFX_GOL_ObjectBackGroundSet(&pDm->hdr);

#if defined (GFX_CONFIG_ALPHABLEND_DISABLE) && !defined (GFX_CONFIG_GRADIENT_DISABLE)
                    // object do not support gradient
                    GFX_FillStyleSet(GFX_FILL_STYLE_COLOR);
#else
                    GFX_FillStyleSet(pDm->hdr.pGolScheme->fillStyle);
#endif
                    GFX_ColorSet(pDm->hdr.pGolScheme->CommonBkColor);
                    state = DM_STATE_PANEL;
                    // no break here since the next state
                    // is the state that we want
                }
                else
                {
                    state = DM_STATE_INIT;
                    break;
                }

            case DM_STATE_PANEL:

                if(GFX_RectangleFillDraw(
                        pDm->hdr.left,
                        pDm->hdr.top,
                        pDm->hdr.right,
                        pDm->hdr.bottom) != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }

                if( GFX_GOL_ObjectStateGet(
                        pDm,
                        GFX_GOL_DIGITALMETER_FRAME_STATE)
                  )
                {
                    state = DM_STATE_FRAME;
                    // no break here since the next state
                    // is the state that we want
                }
                else
                {
                    state = DM_STATE_INIT;
                    break;
                }
    
            case DM_STATE_FRAME:
    
                // show frame if specified to be shown
                GFX_LineStyleSet(GFX_LINE_STYLE_THIN_SOLID);
                if(!GFX_GOL_ObjectStateGet(
                        pDm,
                        GFX_GOL_DIGITALMETER_DISABLED_STATE)
                  )
                {
                    // show enabled color
                    GFX_ColorSet(pDm->hdr.pGolScheme->Color1);
                }
                else
                {
                    // show disabled color
                    GFX_ColorSet(pDm->hdr.pGolScheme->ColorDisabled);
                }
                state = DM_STATE_DRAW_FRAME;
                break;
    
            case DM_STATE_INIT:
                if (GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
                    return (GFX_STATUS_FAILURE);
    
                // set clipping area, text will only
                // appear inside the text area.
                GFX_TextAreaLeftSet(pDm->hdr.left +
                                        GFX_GOL_DIGITALMETER_INDENT);
                GFX_TextAreaTopSet(pDm->hdr.top);
                GFX_TextAreaRightSet(pDm->hdr.right -
                                        GFX_GOL_DIGITALMETER_INDENT);
                GFX_TextAreaBottomSet(pDm->hdr.bottom);
    
                // set the text color
                if(!GFX_GOL_ObjectStateGet(
                        pDm,
                        GFX_GOL_DIGITALMETER_DISABLED_STATE)
                  )
                {
                    GFX_ColorSet(pDm->hdr.pGolScheme->TextColor0);
                }
                else
                {
                    GFX_ColorSet(pDm->hdr.pGolScheme->TextColorDisabled);
                }
    
                // convert the values to be displayed in string format
                NumberToString(
                        pDm->Pvalue,
                        PreValue,
                        pDm->NoOfDigits,
                        pDm->DotPos);

                NumberToString(
                        pDm->Cvalue,
                        CurValue,
                        pDm->NoOfDigits,
                        pDm->DotPos);
    
                // use the font specified in the object
                GFX_FontSet(pDm->hdr.pGolScheme->pFont);
    
                state = DM_STATE_SETALIGN;  // go to drawing of text
    
            case DM_STATE_SETALIGN:
                if(!charCtr)
                {
   
                    // set position of the next character (based on alignment and next character)
                    textWidth = GFX_TextStringWidthGet(CurValue, pDm->hdr.pGolScheme->pFont);

                    // set the position of the character
                    // based on the alignment
                    int16_t verticalPos;

                    // get the vertical position
                    // when the object height is less than the
                    // font height just set the position to top
                    verticalPos = ( pDm->hdr.top +
                                    pDm->hdr.bottom -
                                    pDm->textHeight) >> 1;
                    if (verticalPos < 0)
                        verticalPos = pDm->hdr.top;
                                

                    switch (pDm->alignment & GFX_ALIGN_HORIZONTAL_MASK)
                    {
                        case GFX_ALIGN_LEFT:

                            GFX_TextCursorPositionSet(
                                    pDm->hdr.left +
                                        GFX_GOL_DIGITALMETER_INDENT,
                                    verticalPos);
                            break;

                        case GFX_ALIGN_RIGHT:

                            GFX_TextCursorPositionSet(
                                    (   pDm->hdr.right -
                                        textWidth -
                                        GFX_GOL_DIGITALMETER_INDENT
                                    ),
                                    verticalPos);

                            break;

                        case GFX_ALIGN_CENTER:
                        default:

                            GFX_TextCursorPositionSet(
                                    (   pDm->hdr.left  +
                                        pDm->hdr.right -
                                        textWidth
                                    ) >> 1,
                                    verticalPos);

                            break;

                    }
                }
                pch = *(PreValue + charCtr);
                ch = *(CurValue + charCtr);
                state = DM_STATE_SETTEXT;    
    
            case DM_STATE_SETTEXT:
    
                if (ch != 0)
                {
                    if(GFX_GOL_ObjectStateGet(
                            pDm,
                            GFX_GOL_DIGITALMETER_DRAW_STATE)
                      )
                    {
                        GFX_ColorSet(pDm->hdr.pGolScheme->CommonBkColor);
                    }
    	    	    else if(GFX_GOL_ObjectStateGet(
                            pDm,
                            GFX_GOL_DIGITALMETER_UPDATE_STATE)
                           )
                    {
                        if(pch != ch)
    	                {
                            GFX_ColorSet(pDm->hdr.pGolScheme->CommonBkColor);
    	                }        
                        else
                        {
                            state = DM_STATE_DRAWTEXT;
                            break;
                        }
                    }
                    GFX_GOL_ObjectBackGroundSet(&pDm->hdr);
#if defined (GFX_CONFIG_ALPHABLEND_DISABLE) && !defined (GFX_CONFIG_GRADIENT_DISABLE)
                    // object do not support gradient
                    GFX_FillStyleSet(GFX_FILL_STYLE_COLOR);
#else
                    GFX_FillStyleSet(pDm->hdr.pGolScheme->fillStyle);
#endif

                    // get the width of the character to be erased
                    tempStr[1] = 0;
                    tempStr[0] = pch;
                    charWidth = GFX_TextStringWidthGet(tempStr, pDm->hdr.pGolScheme->pFont);

                    state = DM_STATE_ERASETEXT;
                    break;
                }
                else 
                {
                    state = DM_STATE_WRAPUP;
                    break;
                }
    
            case DM_STATE_ERASETEXT:
                if(GFX_RectangleFillDraw(
                        GFX_TextCursorPositionXGet(),
                        pDm->hdr.top + 1,
                        GFX_TextCursorPositionXGet() + charWidth, //textWidth,
                        pDm->hdr.bottom - 1) != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }

                state = DM_STATE_DRAWTEXT;
    
            case DM_STATE_DRAWTEXT:

                // set color of the text
                GFX_ColorSet(pDm->hdr.pGolScheme->TextColor0);
                
                // render the character
                if (GFX_TextCharDraw(ch) != GFX_STATUS_SUCCESS)
                    return (GFX_STATUS_FAILURE);

                // update to next character
                charCtr++;
                
                ch    = *(CurValue + charCtr);
                pch   = *(PreValue + charCtr);
                state = DM_STATE_SETTEXT;
                break;
    
            case DM_STATE_WRAPUP:
    
                // end of text string is reached no more lines to display
                charCtr = 0;
                state = DM_STATE_IDLE;
#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                GFX_DRIVER_CompleteDrawUpdate(  pDm->hdr.left,
                                                pDm->hdr.top,
                                                pDm->hdr.right,
                                                pDm->hdr.bottom);
#endif
                return (GFX_STATUS_SUCCESS);
    
        } // end of switch()
    } // end of while(1)
}


