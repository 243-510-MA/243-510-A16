/*******************************************************************************
 Module for Microchip Graphics Library - Graphic Object Layer

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_gol_meter.c

  Summary:
    This implements the meter object of the GOL.

  Description:
    Refer to Microchip Graphics Library for complete documentation of the
    Meter Object.
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "gfx/gfx_gol_meter.h"


/* Internal Used Constants */
#define ARC225_DEGREE 225
#define ARC180_DEGREE 180 // defines one arc1 limit (used to determine colors)
#define ARC135_DEGREE 135 // defines one arc2 limit (used to determine colors)
#define ARC090_DEGREE 90  // defines one arc3 limit (used to determine colors)
#define ARC045_DEGREE 45  // defines one arc4 limit (used to determine colors)
#define ARC000_DEGREE 0   // defines one arc5 limit (used to determine colors)

/* Internal Functions */
// used to calculate the meter dimensions
void    MtrCalcDimensions(GFX_GOL_METER *pMeter);
void GFX_GOL_MeterValueToCoordinatePosition(
                                GFX_GOL_METER *pMeter,
                                int16_t angle,
                                uint16_t radius,
                                int16_t *pX,
                                int16_t *pY);
void GFX_GOL_MeterStartEndGet(
                                GFX_GOL_METER *pMeter,
                                int16_t *start,
                                int16_t *end);
int16_t GFX_GOL_MeterAngleGet(
                                GFX_GOL_METER *pMeter,
                                int16_t start,
                                int16_t end);

uint16_t GFX_GOL_MeterConvertIntToString(
                                GFX_XCHAR *pStrVal,
                                int16_t value,
                                uint16_t stringSize);

typedef union
{
    struct
    {
        int8_t int8Data[4];
    };

    struct
    {
        int16_t int16Data[2];
    };

    int32_t int32Data;
}METER_INT32_UNION;
// which is dependent on meter type

// *****************************************************************************
/*  Function:
    GFX_GOL_METER  *GFX_GOL_MeterCreate(
                                uint16_t                ID,
                                uint16_t                left,
                                uint16_t                top,
                                uint16_t                right,
                                uint16_t                bottom,
                                uint16_t                state,
                                GFX_GOL_METER_DRAW_TYPE type,
                                int16_t                 value,
                                int16_t                 minValue,
                                int16_t                 maxValue,
                                GFX_RESOURCE_HDR        *pTitleFont,
                                GFX_RESOURCE_HDR        *pValueFont,
                                GFX_XCHAR               *pText,
                                GFX_GOL_OBJ_SCHEME      *pScheme)

    Summary:
        This function creates a GFX_GOL_METER object with the parameters
        given. It automatically attaches the new object into a global
        linked list of objects and returns the address of the object.

    Description:
        This function creates a GFX_GOL_METER object with the parameters
        given. It automatically attaches the new object into a global
        linked list of objects and returns the address of the object.

        This function returns the pointer to the newly created object.
        If the object is not successfully created, it returns NULL.

        The behavior of GFX_GOL_MeterCreate() will be undefined if one
        of the following is true:
        - left >= right
        - top >= bottom
        - type is not one of the defined types
        - pTitleFont and pValueFont is not defined to a valid font
          GFX_RESOURC_HDR
        - pScheme is not pointing to a GFX_GOL_OBJ_SCHEME
        - pText is an unterminated string

*/
// *****************************************************************************
GFX_GOL_METER  *GFX_GOL_MeterCreate(
                                uint16_t                ID,
                                uint16_t                left,
                                uint16_t                top,
                                uint16_t                right,
                                uint16_t                bottom,
                                uint16_t                state,
                                GFX_GOL_METER_DRAW_TYPE type,
                                int16_t                 value,
                                int16_t                 minValue,
                                int16_t                 maxValue,
                                GFX_RESOURCE_HDR        *pTitleFont,
                                GFX_RESOURCE_HDR        *pValueFont,
                                GFX_XCHAR               *pText,
                                GFX_GOL_OBJ_SCHEME      *pScheme)
{
    int16_t             degreeStart, degreeEnd, angle;

    GFX_GOL_METER       *pMtr = NULL;

    pMtr = (GFX_GOL_METER *)GFX_malloc(sizeof(GFX_GOL_METER));
    if(pMtr == NULL)
        return (NULL);

    pMtr->hdr.ID        = ID;                       // unique id assigned for referencing
    pMtr->hdr.pNxtObj   = NULL;                     // initialize pointer to NULL
    pMtr->hdr.type      = GFX_GOL_METER_TYPE;       // set object type
    pMtr->hdr.left      = left;                     // left,top coordinate
    pMtr->hdr.top       = top;                      //
    pMtr->hdr.right     = right;                    // right,bottom coordinate
    pMtr->hdr.bottom    = bottom;                   //
    pMtr->hdr.state     = state;                    // state
    pMtr->pText         = pText;                    // title text

    pMtr->hdr.DrawObj   = GFX_GOL_MeterDraw;        // draw function
    pMtr->hdr.FreeObj   = NULL;                     // free function
    pMtr->hdr.actionGet = GFX_GOL_MeterActionGet;   // message function
    pMtr->hdr.actionSet = GFX_GOL_MeterActionSet;   // default message function

    // set the range
    GFX_GOL_MeterRangeSet(pMtr, minValue, maxValue);
    //pMtr->minValue      = minValue;                 // minimum value
    //pMtr->maxValue      = maxValue;                 // maximum value

    // set the initial value
    GFX_GOL_MeterValueSet(pMtr, value);
    //pMtr->value         = value;                    // current value


    // Set the color scheme to be used
    pMtr->hdr.pGolScheme = pScheme;

    // Set the Title Font to be used
    GFX_GOL_MeterTitleFontSet(pMtr, pTitleFont);

    // Set the Value Font to be used
    GFX_GOL_MeterValueFontSet(pMtr, pValueFont);

    // Set the type
    GFX_GOL_MeterTypeSet(pMtr, type);

    // calculate dimensions of the meter	
    MtrCalcDimensions(pMtr);

    // calculate the needle initial position, this must be called
    // after the dimensions are calculates so the radius has the
    // correct value
    GFX_GOL_MeterStartEndGet(pMtr, &degreeStart, &degreeEnd);
    angle = GFX_GOL_MeterAngleGet(pMtr, degreeStart, degreeEnd);
    GFX_GOL_MeterValueToCoordinatePosition(
            pMtr, angle, pMtr->radius,
            &(pMtr->xPos), &(pMtr->yPos));

    GFX_GOL_ObjectAdd((GFX_GOL_OBJ_HEADER *)pMtr);

    return (pMtr);
}

// *****************************************************************************
/*  Function:
    void MtrCalcDimensions(GFX_GOL_METER *pMeter)

    Summary:
        This function calculates the dimension of the object
        based on the left, top, right, bottom parameters.

    Description:
        This function calculates the dimension of the object
        based on the left, top, right, bottom parameters.
        This is an internal function to the object.

*/
// *****************************************************************************
void MtrCalcDimensions(GFX_GOL_METER *pMeter)
{
    int16_t   tempHeight, tempWidth;
    int16_t   left, top, right, bottom;
    GFX_XCHAR   tempChar[2] = {'8',0};

    left   = pMeter->hdr.left;
    right  = pMeter->hdr.right;
    top    = pMeter->hdr.top;
    bottom = pMeter->hdr.bottom;

    // get the text width reference. This is used to scale the meter
    if(pMeter->pText != NULL)
    {
        tempHeight = (pMeter->hdr.pGolScheme->EmbossSize << 1) +
                        GFX_TextStringHeightGet(pMeter->hdr.pGolScheme->pFont);
    }
    else
    {
        tempHeight = (pMeter->hdr.pGolScheme->EmbossSize << 1);
    }

    tempWidth = (pMeter->hdr.pGolScheme->EmbossSize << 1) +
                    (GFX_TextStringWidthGet(
                        tempChar,
                        pMeter->hdr.pGolScheme->pFont) *
                    GFX_CONFIG_OBJECT_METER_SCALE_COUNT);

    // Meter size is dependent on the width or height.
    // The radius is also adjusted to add space for the scales

    switch (pMeter->type)
    {
        case GFX_GOL_METER_QUARTER_TYPE:

            // choose the radius
            if((right - left - tempWidth) >
               (bottom - top -
                (GFX_TextStringHeightGet(pMeter->pTitleFont) +
                 GFX_TextStringHeightGet(pMeter->hdr.pGolScheme->pFont)) -
                 (pMeter->hdr.pGolScheme->EmbossSize << 1)))
            {
                pMeter->radius = bottom -
                               top - (  GFX_TextStringHeightGet(pMeter->pTitleFont) +
                                        GFX_TextStringHeightGet(pMeter->hdr.pGolScheme->pFont) +
                                        (pMeter->hdr.pGolScheme->EmbossSize << 1));
            }
            else
            {
                pMeter->radius = right - left -
                               (GFX_TextStringWidthGet(
                                    tempChar,
                                    pMeter->hdr.pGolScheme->pFont) *
                                 (GFX_CONFIG_OBJECT_METER_SCALE_COUNT + 1)
                               ) -
                               pMeter->hdr.pGolScheme->EmbossSize;
            }

            pMeter->radius -= (((pMeter->radius) >> 2) +
                             pMeter->hdr.pGolScheme->EmbossSize);

            // center the meter on the given dimensions
            pMeter->xCenter = ((left + right) >> 1) -
                            ((pMeter->radius + tempWidth +
                                (pMeter->radius >> 2)) >> 1);
            pMeter->yCenter = ((top + bottom) >> 1) +
                            ((pMeter->radius + (pMeter->radius >> 2)) >> 1);

            break;
            
        case GFX_GOL_METER_HALF_TYPE:

            // choose the radius
            if((right - left) >> 1 > (bottom - top))
            {
                pMeter->radius = (bottom - top) -
                               ((tempHeight << 1) + ((bottom - top) >> 3));
                pMeter->yCenter = ((bottom + top) >> 1) +
                                ((pMeter->radius + ((bottom - top) >> 3)) >> 1);
            }
            else
            {
                pMeter->radius = ((right - left) >> 1) -
                               (tempWidth + ((right - left) >> 3));
                pMeter->yCenter = ((bottom + top) >> 1) +
                                ((pMeter->radius + ((right - left) >> 3)) >> 1);
            }

            // center the meter on the given dimensions
            pMeter->xCenter = (left + right) >> 1;

            break;
        case GFX_GOL_METER_WHOLE_TYPE:
        default:

            // choose the radius
            if( (right - left - tempWidth) >
                (bottom - top - tempHeight -
                    GFX_TextStringHeightGet(pMeter->pTitleFont)))
            {
                pMeter->radius = ((bottom - top -
                                 tempHeight -
                                 GFX_TextStringHeightGet(pMeter->pTitleFont)) >> 1)
                               -
                               ((tempHeight + bottom - top) >> 3);
            }
            else
                pMeter->radius = ((right - left) >> 1)
                               -
                               (tempWidth + ((right - left) >> 3));

            // center the meter on the given dimensions
            pMeter->xCenter = (left + right) >> 1;
            pMeter->yCenter = ((bottom + top) >> 1) - (tempHeight >> 1);

            break;
    }

}

// *****************************************************************************
/*  Function:
    void GFX_GOL_MeterValueToCoordinatePosition(
                                GFX_GOL_METER *pMeter,
                                int16_t angle,
                                uint16_t radius,
                                int16_t *pX,
                                int16_t *pY)

    Summary:
        This function calculates the pixel position of a segment
        from the center of a cicle with the given radius.

    Description:
        This function calculates the pixel position of a segment
        from the center of a cicle with the given radius.
        This is an internal function to the object.

*/
// *****************************************************************************
void __attribute__ ((always_inline)) GFX_GOL_MeterValueToCoordinatePosition(
                                GFX_GOL_METER *pMeter,
                                int16_t angle,
                                uint16_t radius,
                                int16_t *pX,
                                int16_t *pY)
{
    // given the angle, use sine and cosine values to calculate the
    // pixel position of the point. This is used to calculate the
    // line for the needle as well as the positions of the scales
    // of the meter.
    // Look up table is used in sine and cosine functions

    *pX = ( (int32_t)GFX_CosineGet(angle) * radius) >> 8;
    *pY = (((int32_t)GFX_SineGet(angle)   * radius) >> 8) * -1;

    *pX += pMeter->xCenter;
    *pY += pMeter->yCenter;

}

// *****************************************************************************
/*  Function:
    void GFX_GOL_MeterStartEndGet(
                                GFX_GOL_METER *pMeter,
                                int16_t *pStart,
                                int16_t *pEnd)

    Summary:
        This function calculates the start and end angles of the
        meter.

    Description:
        This function calculates the start and end angles of the
        meter. This is a internal function of the object. This
        function is intended to help the object calculate the
        start and end angles of the scales of the object.

*/
// *****************************************************************************
void __attribute__ ((always_inline)) GFX_GOL_MeterStartEndGet(
                                GFX_GOL_METER *pMeter,
                                int16_t *pStart,
                                int16_t *pEnd)
{
    switch(pMeter->type)
    {
        case GFX_GOL_METER_QUARTER_TYPE:
            *pStart = 0;
            *pEnd   = 90;
            break;
        case GFX_GOL_METER_HALF_TYPE:
            *pStart = 0;
            *pEnd   = 180;
            break;
        case GFX_GOL_METER_WHOLE_TYPE:
        default:
            *pStart = -45;
            *pEnd   = 225;
            break;
    }
}

// *****************************************************************************
/*  Function:
    int16_t GFX_GOL_MeterAngleGet(
                                GFX_GOL_METER *pMeter,
                                int16_t start,
                                int16_t end)

    Summary:
        This function calculates the angle based on the current
        meter value and its corresponding start and end angles.

    Description:
        This function calculates the angle based on the current
        meter value and its corresponding start and end angles.
        This is a internal function of the object. This
        function is intended to help the object calculate the
        start and end angles of the scales of the object.

*/
// *****************************************************************************
int16_t __attribute__ ((always_inline)) GFX_GOL_MeterAngleGet(
                                GFX_GOL_METER *pMeter,
                                int16_t start,
                                int16_t end)
{
    METER_INT32_UNION   dTemp;
    
    dTemp.int32Data = 0;
    dTemp.int16Data[1] = pMeter->value - pMeter->minValue;
    dTemp.int32Data /= (pMeter->maxValue - pMeter->minValue);
    dTemp.int32Data *= (end - start);

    return (end - (dTemp.int16Data[1]));

}

// *****************************************************************************
/*  Function:
    uint16_t GFX_GOL_MeterConvertIntToString(
                                GFX_XCHAR *pStrVal,
                                int16_t value,
                                uint16_t stringSize)

    Summary:
        This function converts the given value to string.

    Description:
        This function converts the given value to string.
        This function is dependent on the settings of
        GFX_CONFIG_OBJECT_METER_SCALE_COUNT and
        GFX_CONFIG_OBJECT_METER_DECIMAL_PLACES macros.

        This function will return the number of characters
        set in the string buffer pointed to by pStrVal.

*/
// *****************************************************************************
uint16_t GFX_GOL_MeterConvertIntToString(
                                GFX_XCHAR *pStrVal,
                                int16_t value,
                                uint16_t stringSize)
{
    uint16_t j = 0;

    // this implements sprintf(pStrVal, "%d", value); faster
    // note that this is just for values >= 0, while sprintf
    // covers negative values.


    if (stringSize > 0)
    {
        pStrVal = pStrVal + stringSize;

        // terminate the string
        *pStrVal-- = 0;

        do
        {
            *(pStrVal - j) = (value % 10) + '0';
#if (GFX_CONFIG_OBJECT_METER_DECIMAL_PLACES == 0)
            // remove the leading zeroes
            if(((value /= 10) == 0) || (j >= GFX_CONFIG_OBJECT_METER_SCALE_COUNT))
                break;
#else
            value /= 10;
            if(j >= GFX_CONFIG_OBJECT_METER_SCALE_COUNT)
                break;
#endif
            j++;

#if (GFX_CONFIG_OBJECT_METER_DECIMAL_PLACES != 0)
            // add the decimal point if specified
            if (GFX_CONFIG_OBJECT_METER_DECIMAL_PLACES == j)
            {
                *(pStrVal - j) = '.';
                j++;
            }
#endif

        } while(j <= GFX_CONFIG_OBJECT_METER_SCALE_COUNT);

    }

    // return the count of characters
    return (j);
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_MeterValueSet(
                                GFX_GOL_METER *pObject,
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
void GFX_GOL_MeterValueSet(GFX_GOL_METER *pObject, int16_t value)
{
    if(value < pObject->minValue)
    {
        pObject->value = pObject->minValue;
        return;
    }

    if(value > pObject->maxValue)
    {
        pObject->value = pObject->maxValue;
        return;
    }

    pObject->value = value;
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_MeterIncrement(
                                GFX_GOL_METER *pObject,
                                uint16_t delta)

    Summary:
        This function increments the meter value by the delta
        value set.

    Description:
        This function increments the meter value by the given
        delta value set. If the delta given exceeds the maximum value
        of the meter, the value will remain to be at maximum.

        Object must be redrawn after this function is called to reflect
        the changes to the object.

*/
// *****************************************************************************
void __attribute__ ((always_inline)) GFX_GOL_MeterIncrement(
                                GFX_GOL_METER *pObject, 
                                uint16_t delta)
{
    GFX_GOL_MeterValueSet(pObject, pObject->value + delta);
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_MeterDecrement(
                                GFX_GOL_METER *pObject,
                                uint16_t delta)

    Summary:
        This function decrements the meter value by the delta
        value set.

    Description:
        This function decrements the meter value by the given
        delta value set. If the delta given exceeds the maximum value
        of the meter, the value will remain to be at maximum.
 
        Object must be redrawn after this function is called to reflect
        the changes to the object.

*/
// *****************************************************************************
void __attribute__ ((always_inline)) GFX_GOL_MeterDecrement(
                                GFX_GOL_METER *pObject,
                                uint16_t delta)
{
    GFX_GOL_MeterValueSet(pObject, pObject->value - delta);
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_MeterRangeSet(
                                GFX_GOL_METER *pObject,
                                int16_t minValue,
                                int16_t maxValue)

    Summary:
        This function sets the range of the meter.

    Description:
        This function sets the range of the meter.
        When the range is modified, object must be completely redrawn to
        reflect the change. minValue should always be less than
        maxValue.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.
        minValue - new minimum value of the object.
        maxValue - new maximum value of the object.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void GFX_GOL_MeterRangeSet(
                                GFX_GOL_METER *pObject,
                                int16_t minValue,
                                int16_t maxValue)
{
    pObject->minValue = minValue;
    pObject->maxValue = maxValue;
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_MeterScaleColorsSet(
                                GFX_GOL_METER *pObject,
                                GFX_COLOR color1,
                                GFX_COLOR color2,
                                GFX_COLOR color3,
                                GFX_COLOR color4,
                                GFX_COLOR color5,
                                GFX_COLOR color6)

    Summary:
        This function sets the arc colors of the object.

    Description:
        After the object is created, this function must be called to
        set the arc colors of the object.

        Scale colors can be used to highlight values of the meter.
        User can set these colors to define the arc colors and scale colors.
        This also sets the color of the meter value when displayed.
        The color settings are set to the following angles:
	<TABLE>
        Color Boundaries    Type Whole      Type Half       Type Quarter
     	##################  ############    ############    ##################
             Arc 6          225 to 180       not used         not used
             Arc 5          179 to 135       179 to 135       not used
             Arc 4          134 to  90       134 to  90       not used
             Arc 3          89  to  45       89  to  45       89 to 45
             Arc 2          44  to   0       44  to   0       44 to  0
             Arc 1          -45 to  -1       not used         not used
 	</TABLE>

        As the meter is drawn colors are changed depending on the
        angle of the scale and label being drawn.

*/
// *****************************************************************************
void __attribute__ ((always_inline)) GFX_GOL_MeterScaleColorsSet(
                                GFX_GOL_METER *pObject,
                                GFX_COLOR color1,
                                GFX_COLOR color2,
                                GFX_COLOR color3,
                                GFX_COLOR color4,
                                GFX_COLOR color5,
                                GFX_COLOR color6)
{
    pObject->color1 = color1;
    pObject->color2 = color2;
    pObject->color3 = color3;
    pObject->color4 = color4;
    pObject->color5 = color5;
    pObject->color6 = color6;

}

// *****************************************************************************
/*  Function:
    GFX_GOL_TRANSLATED_ACTION GFX_GOL_MeterActionGet(
                                void *pObject, GFX_GOL_MESSAGE *pMessage)

    Summary:
        This function evaluates the message from a user if the
        message will affect the object or not.

    Description:
        This function evaluates the message from a user if the
        message will affect the object or not.
 
*/
// *****************************************************************************
GFX_GOL_TRANSLATED_ACTION GFX_GOL_MeterActionGet(
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage)
{
    GFX_GOL_METER *pMtr;

    pMtr = (GFX_GOL_METER *)pObject;

    // Evaluate if the message is for the meter
    // Check if disabled first
    if(GFX_GOL_ObjectStateGet(pMtr, GFX_GOL_METER_DISABLED_STATE))
        return (GFX_GOL_OBJECT_ACTION_INVALID);

    if(pMessage->type == TYPE_SYSTEM)
    {
        if(pMessage->param1 == pMtr->hdr.ID)
        {
            if(pMessage->uiEvent == EVENT_SET)
            {
                return (GFX_GOL_METER_ACTION_SET);
            }
        }
    }

    return (GFX_GOL_OBJECT_ACTION_INVALID);


}

// *****************************************************************************
/*  Function:
    void GFX_GOL_MeterActionSet(GFX_GOL_TRANSLATED_ACTION  translatedMsg,
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
void GFX_GOL_MeterActionSet(
                                GFX_GOL_TRANSLATED_ACTION  translatedMsg,
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage)
{
    GFX_GOL_METER *pMtr;

    pMtr = (GFX_GOL_METER *)pObject;

    if(translatedMsg == GFX_GOL_METER_ACTION_SET)
    {
        // set the value
        GFX_GOL_MeterValueSet(pMtr, pMessage->param2);
        // update the meter
        GFX_GOL_ObjectStateSet(pMtr, GFX_GOL_METER_UPDATE_DRAW_STATE);
    }
}


// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_GOL_MeterDraw(void *pObject)

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
GFX_STATUS GFX_GOL_MeterDraw(void *pObject)
{
    typedef enum
    {
        IDLE,
        FRAME_DRAW,
        NEEDLE_DRAW,
        NEEDLE_ERASE,
        TEXT_DRAW,
        TEXT_DRAW_RUN,
        ARC_DRAW_SETUP,
        ARC_DRAW,
        SCALE_COMPUTE,
        SCALE_LABEL_DRAW,
        SCALE_DRAW,
#ifndef GFX_CONFIG_OBJECT_METER_DISPLAY_VALUES_DISABLE
        VALUE_ERASE,
        VALUE_DRAW,
        VALUE_DRAW_RUN,
#endif
    } MTR_DRAW_STATES;

    static MTR_DRAW_STATES      state = IDLE;
    static int16_t              x1, y1, x2, y2, rad1, rad2;
    static int16_t              temp, j, i, angle;
    static GFX_XCHAR            strVal[GFX_CONFIG_OBJECT_METER_SCALE_COUNT + 1];    // add one more space here for the NULL character
    static int16_t              degreeStart, degreeEnd;
    static int16_t              startX, startY;
#ifndef GFX_CONFIG_OBJECT_METER_DISPLAY_VALUES_DISABLE
    static GFX_XCHAR            tempXchar[2] = {'8',0};        // NULL is pre-defined here
#endif
    static float                radian;

    GFX_GOL_METER               *pMtr;
    GFX_FILL_STYLE              fillStyle;
    uint8_t                     octant;
    uint16_t                    boxXStart, boxYStart, boxXEnd, boxYEnd;

    pMtr = (GFX_GOL_METER *)pObject;

    while(1)
    {

        if (GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
            return (GFX_STATUS_FAILURE);

        switch(state)
        {
            // location of this case is strategic so the partial redraw of the Meter will be faster        
            case FRAME_DRAW:

                if(GFX_GOL_PanelDraw() != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }
                state = TEXT_DRAW;
                break;

            case IDLE:

                /* These selects the parameters of the meter
                   that are dependent on the shape.
                */
                GFX_GOL_MeterStartEndGet(pMtr, &degreeStart, &degreeEnd);

#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                GFX_DRIVER_SetupDrawUpdate( pMtr->hdr.left,
                                            pMtr->hdr.top,
                                            pMtr->hdr.right,
                                            pMtr->hdr.bottom);
#endif
                if(GFX_GOL_ObjectStateGet(pMtr, GFX_GOL_METER_HIDE_STATE))
                {

                    // Hide the GFX_GOL_BUTTON (remove from screen)
                    if (GFX_GOL_ObjectHideDraw(&(pMtr->hdr)) != GFX_STATUS_SUCCESS)
                        return (GFX_STATUS_FAILURE);

#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                    GFX_DRIVER_CompleteDrawUpdate(  pMtr->hdr.left,
                                                    pMtr->hdr.top,
                                                    pMtr->hdr.right,
                                                    pMtr->hdr.bottom);
#endif
                    return (GFX_STATUS_SUCCESS);
                }
    
                // Check if we need to draw the whole object
                GFX_LineStyleSet(GFX_LINE_STYLE_THIN_SOLID);

                if(GFX_GOL_ObjectStateGet(pMtr, GFX_GOL_METER_DRAW_STATE))
                {
                    // set up the background
                    GFX_GOL_PanelBackgroundSet(&pMtr->hdr);

#if defined (GFX_CONFIG_ALPHABLEND_DISABLE) && !defined (GFX_CONFIG_GRADIENT_DISABLE)
                // scroll bar will not draw gradient on the main
                // panel, only on the thumb
                fillStyle = GFX_FILL_STYLE_COLOR;
#else
                fillStyle = pMtr->hdr.pGolScheme->fillStyle;
#endif

                    // set parameters to draw the frame
                    GFX_GOL_PanelParameterSet(
                            pMtr->hdr.left,
                            pMtr->hdr.top,
                            pMtr->hdr.right,
                            pMtr->hdr.bottom,
                            0,
                            pMtr->hdr.pGolScheme->Color0,
                            pMtr->hdr.pGolScheme->EmbossLtColor,
                            pMtr->hdr.pGolScheme->EmbossDkColor,
                            NULL,
                            fillStyle,
                            pMtr->hdr.pGolScheme->EmbossSize - 1
                        );
                    state = FRAME_DRAW;
                    break;
                }
                else
                {
                    state = NEEDLE_ERASE;
                    break;
                }
    
            case TEXT_DRAW:
    
                // draw the meter title
                GFX_ColorSet(pMtr->hdr.pGolScheme->TextColor1);
                GFX_FontSet(pMtr->pTitleFont);
                temp = GFX_TextStringWidthGet(pMtr->pText, pMtr->pTitleFont);
    
                // set the start location of the meter title

                switch(pMtr->type)
                {
                    case GFX_GOL_METER_QUARTER_TYPE:
                        startX = ((pMtr->hdr.right + pMtr->hdr.left) >> 1) - (temp >> 1);
                        startY = pMtr->hdr.bottom -
                                 pMtr->hdr.pGolScheme->EmbossSize -
                                 GFX_TextStringHeightGet(pMtr->pTitleFont);
                        break;
                    case GFX_GOL_METER_HALF_TYPE:
                        startX = pMtr->xCenter - (temp >> 1);
                        startY = pMtr->yCenter + 3;
                        break;
                    case GFX_GOL_METER_WHOLE_TYPE:
                    default:
#ifndef GFX_CONFIG_OBJECT_METER_DISPLAY_VALUES_DISABLE
                        startX = pMtr->xCenter - (temp >> 1);
                        startY = pMtr->yCenter +
                                 pMtr->radius +
                                 GFX_TextStringHeightGet(pMtr->pValueFont);
#else
                        startX = pMtr->xCenter - (temp >> 1);
                        startY = pMtr->yCenter +
                                 pMtr->radius + 
                                 GFX_TextStringHeightGet(pMtr->hdr.pGolScheme->pFont);
#endif
                        break;
                }
                state = TEXT_DRAW_RUN;
    
            case TEXT_DRAW_RUN:
    
                // render the title of the meter
                if (GFX_TextStringDraw(
                        startX,
                        startY,
                        pMtr->pText,
                        0)
                    != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }

                state = ARC_DRAW_SETUP;
    
            case ARC_DRAW_SETUP:
    
                // check if we need to draw the arcs
                if(!GFX_GOL_ObjectStateGet(pMtr, GFX_GOL_METER_RING_STATE))
                {
    
                    // if meter is not RING type, for scale label colors use
                    // the three colors (normal, critical and danger)
                    i = degreeEnd;
                    state = SCALE_COMPUTE;
                    break;
                }
                else
                {
    
                    // set the arc radii: x1 smaller radius and x2 as the larger radius
                    x1 = pMtr->radius + 2;
                    x2 = pMtr->radius + (pMtr->radius >> 2) + 2;
                    if (pMtr->type == GFX_GOL_METER_WHOLE_TYPE)
                        temp = 6;
                    else if(pMtr->type == GFX_GOL_METER_HALF_TYPE)
                        temp = 5;
                    else if(pMtr->type == GFX_GOL_METER_QUARTER_TYPE)
                        temp = 3;
                    else
                        temp = 6;

                    state = ARC_DRAW;
                    break;
                }
    
            case ARC_DRAW:

                while(temp)
                {
                    // decide which arc will be drawn
                    switch(temp)
                    {
                        case 6:
                            GFX_ColorSet(pMtr->color1);
                            octant = 0x20;
                            break;
    
                        case 5:
                            GFX_ColorSet(pMtr->color2);
                            octant = 0x40;
                            break;
    
                        case 4:
                            GFX_ColorSet(pMtr->color3);
                            octant = 0x80;
                            break;
    
                        case 3:
                            GFX_ColorSet(pMtr->color4);
                            octant = 0x01;
                            break;
    
                        case 2:
                            GFX_ColorSet(pMtr->color5);
                            octant = 0x02;
                            break;
    
                        case 1:
                            GFX_ColorSet(pMtr->color6);
                            octant = 0x04;
                            break;
    
                        default:
                            octant = 0xEE;
                            break;
                    }

                    if(GFX_ThickBevelDraw(
                                    pMtr->xCenter,
                                    pMtr->yCenter,
                                    pMtr->xCenter,
                                    pMtr->yCenter,
                                    x1, x2, octant) != GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }
                    temp--;
                    if (pMtr->type != GFX_GOL_METER_WHOLE_TYPE)
                        if (temp == 1)
                            break;
                }
    
                // set the color for the scale labels
                GFX_ColorSet(pMtr->hdr.pGolScheme->Color1);
                i = degreeEnd;
                state = SCALE_COMPUTE;
            
            case SCALE_COMPUTE:

                if(i >= degreeStart)
                {
                    if(!GFX_GOL_ObjectStateGet(pMtr, GFX_GOL_METER_RING_STATE))
                    {
                        if(i >= ARC180_DEGREE)
                        {
                            GFX_ColorSet(pMtr->color1);
                        }
                        else if(i >= ARC135_DEGREE)
                        {
                            GFX_ColorSet(pMtr->color2);
                        }
                        else if(i >= ARC090_DEGREE)
                        {
                            GFX_ColorSet(pMtr->color3);
                        }
                        else if(i >= ARC045_DEGREE)
                        {
                            GFX_ColorSet(pMtr->color4);
                        }
                        else if(i >= ARC000_DEGREE)
                        {
                            GFX_ColorSet(pMtr->color5);
                        }
                        else
                        {
                            GFX_ColorSet(pMtr->color6);
                        }
                    }
    
                    // compute for the effective radius of the scales
                    if((i % 45) == 0)
                        rad1 = pMtr->radius + (pMtr->radius >> 2) + 2;
                    else
                        rad1 = pMtr->radius + (pMtr->radius >> 3) + 3;

                    rad2 = (pMtr->radius + 3);
    
                    // compute the starting x and y positions of the scales
                    GFX_GOL_MeterValueToCoordinatePosition(
                            pMtr, i, rad1, &x1, &y1);
                    GFX_GOL_MeterValueToCoordinatePosition(
                            pMtr, i, rad2, &x2, &y2);

                    state = SCALE_DRAW;
                    break;
                }
                else
                {
                    state = NEEDLE_ERASE;
                    break;
                }
    
            case SCALE_DRAW:

                // now draw the scales
                if(GFX_LineDraw(x1, y1, x2, y2) != GFX_STATUS_SUCCESS)
                    return (GFX_STATUS_FAILURE);
                
                if((i % 45) == 0)
                {
    
                    // draw the scale labels
                    // reusing radian, x2 and y2
                    // compute for the actual angle of needle to be shown in screen				
                    radian = (degreeEnd - degreeStart) - (i - (degreeStart));
    
                    // compute the values of the label to be shown per 45 degree
                    temp = (pMtr->maxValue - pMtr->minValue) * (radian / (degreeEnd - degreeStart));
    
                    // adjust for the minimum or offset value
                    temp += pMtr->minValue;
    
                    // this implements sprintf(strVal, "%d", temp); faster
                    // note that this is just for values >= 0, while sprintf covers negative values.
                    j = GFX_GOL_MeterConvertIntToString(strVal, temp, GFX_CONFIG_OBJECT_METER_SCALE_COUNT + 1 );

                    // set location of the scale text
                    x2 = GFX_TextStringWidthGet((&strVal[GFX_CONFIG_OBJECT_METER_SCALE_COUNT - j]), pMtr->hdr.pGolScheme->pFont);
                    y2 = GFX_TextStringHeightGet(pMtr->hdr.pGolScheme->pFont);
    
                    if(i == -45)
                    {
                        startX = x1 + 3;
                        startY = y1;
                    }
                    else if(i == 0)
                    {
                        startX = x1 + 3;
                        startY = y1 - (y2 >> 1);
                    }
                    else if(i == 45)
                    {
                        startX = x1 + 3;
                        startY = y1 - (y2 >> 1) - 3;
                    }
                    else if(i == 90)
                    {
                        startX = x1 - (x2 >> 1);
                        startY = y1 - y2 - 3;
                    }
                    else if(i == 135)
                    {
                        startX = x1 - x2;
                        startY = y1 - y2;
                    }
                    else if(i == 180)
                    {
                        startX = x1 - x2 - 3;
                        startY = y1 - (y2 >> 1);
                    }
                    else if(i == 225)
                    {
                        startX = x1 - (x2 + 3);
                        startY = y1;
                    }
    
                    state = SCALE_LABEL_DRAW;
                    GFX_FontSet(pMtr->hdr.pGolScheme->pFont);
                }
                else
                {
                    i -= GFX_CONFIG_OBJECT_METER_DEGREECOUNT;
                    state = SCALE_COMPUTE;
                    break;
                }
    
            case SCALE_LABEL_DRAW:

                if (GFX_TextStringDraw(
                        startX,
                        startY,
                        &strVal[GFX_CONFIG_OBJECT_METER_SCALE_COUNT - j],
                        0 )
                    != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }

                i -= GFX_CONFIG_OBJECT_METER_DEGREECOUNT;
                state = SCALE_COMPUTE;
                break;
    
            case NEEDLE_ERASE:

                if(GFX_GOL_ObjectStateGet(pMtr, GFX_GOL_METER_UPDATE_DRAW_STATE))
                {
    
                    // to update the needle, redraw the old position with background color
                    GFX_ColorSet(pMtr->hdr.pGolScheme->Color0);
                    GFX_GOL_ObjectBackGroundSet(&pMtr->hdr);

#if defined (GFX_CONFIG_ALPHABLEND_DISABLE) && !defined (GFX_CONFIG_GRADIENT_DISABLE)
                    // object do not support gradient
                    GFX_FillStyleSet(GFX_FILL_STYLE_COLOR);

#else
                    GFX_FillStyleSet(pMtr->hdr.pGolScheme->fillStyle);
#endif

                    if (pMtr->xCenter <= pMtr->xPos)
                    {
                        boxXStart = pMtr->xCenter;
                        boxXEnd   = pMtr->xPos;
                    } else
                    {
                        boxXEnd   = pMtr->xCenter;
                        boxXStart = pMtr->xPos;
                    }
                    if (pMtr->yCenter <= pMtr->yPos)
                    {
                        boxYStart = pMtr->yCenter;
                        boxYEnd   = pMtr->yPos;
                    } else
                    {
                        boxYEnd   = pMtr->yCenter;
                        boxYStart = pMtr->yPos;
                    }

                    if(GFX_RectangleFillDraw(
                            boxXStart, boxYStart,
                            boxXEnd, boxYEnd) != GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }

                }
    
                state = NEEDLE_DRAW;
    
            case NEEDLE_DRAW:

                if (GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
                    return (GFX_STATUS_FAILURE);

                // At this point, pMtr->value is assumed to contain the new value of the meter.
                // calculate the new angle:
                angle = GFX_GOL_MeterAngleGet(pMtr, degreeStart, degreeEnd);

                GFX_GOL_MeterValueToCoordinatePosition(
                        pMtr,
                        angle,
                        pMtr->radius,
                        &(pMtr->xPos),
                        &(pMtr->yPos));

                // now draw the needle with the new position
                // needle will be drawn with color6 parameter
                GFX_ColorSet(pMtr->color6);

                GFX_LineStyleSet(GFX_LINE_STYLE_THIN_SOLID);
                if(GFX_LineDraw(
                        pMtr->xCenter,
                        pMtr->yCenter,
                        pMtr->xPos,
                        pMtr->yPos) != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }
                
                GFX_LineStyleSet(GFX_LINE_STYLE_THIN_SOLID);
                
#ifndef GFX_CONFIG_OBJECT_METER_DISPLAY_VALUES_DISABLE
                state = VALUE_ERASE;
#else
                // reset the line to normal
                GFX_LineStyleSet(GFX_LINE_STYLE_THIN_SOLID);
                state = IDLE;
#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                GFX_DRIVER_CompleteDrawUpdate(  pMtr->hdr.left,
                                                pMtr->hdr.top,
                                                pMtr->hdr.right,
                                                pMtr->hdr.bottom);
#endif
                return (GFX_STATUS_SUCCESS);
#endif
#ifndef GFX_CONFIG_OBJECT_METER_DISPLAY_VALUES_DISABLE
    
            case VALUE_ERASE:
                if (GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
                    return (GFX_STATUS_FAILURE);
    
                // reset the line to normal
                GFX_LineStyleSet(GFX_LINE_STYLE_THIN_SOLID);
    
                // display the value
                // erase previous value first. The temp>>1 accomodates fonts with characters that has unequal widths
                temp = GFX_TextStringWidthGet(tempXchar, pMtr->pValueFont);
                temp = temp * GFX_CONFIG_OBJECT_METER_SCALE_COUNT + (temp >> 1);
    
                GFX_ColorSet(pMtr->hdr.pGolScheme->Color0);
                GFX_GOL_ObjectBackGroundSet(&pMtr->hdr);

#if defined (GFX_CONFIG_ALPHABLEND_DISABLE) && !defined (GFX_CONFIG_GRADIENT_DISABLE)
                    // object do not support gradient 
                    GFX_FillStyleSet(GFX_FILL_STYLE_COLOR);

#else
                    GFX_FillStyleSet(pMtr->hdr.pGolScheme->fillStyle);
#endif

                switch (pMtr->type)
                {
                    case GFX_GOL_METER_QUARTER_TYPE:

                        if(GFX_RectangleFillDraw(
                                pMtr->xCenter - 1,
                                pMtr->yCenter -
                                    GFX_TextStringHeightGet(pMtr->pValueFont),
                                pMtr->xCenter + temp,
                                pMtr->yCenter + 1) != GFX_STATUS_SUCCESS)
                        {
                            return (GFX_STATUS_FAILURE);
                        }
                        break;

                    case GFX_GOL_METER_HALF_TYPE:

                        if(GFX_RectangleFillDraw(
                                pMtr->xCenter - (temp >> 1),
                                pMtr->yCenter -
                                    GFX_TextStringHeightGet(pMtr->pValueFont),
                                pMtr->xCenter + (temp >> 1),
                                pMtr->yCenter)
                                != GFX_STATUS_SUCCESS)
                        {
                            return (GFX_STATUS_FAILURE);
                        }
                        break;

                    case GFX_GOL_METER_WHOLE_TYPE:
                    default:
                        if(GFX_RectangleFillDraw(
                                pMtr->xCenter - (temp >> 1),
                                pMtr->yCenter + pMtr->radius,
                                pMtr->xCenter + (temp >> 1),
                                pMtr->yCenter + pMtr->radius +
                                    GFX_TextStringHeightGet(pMtr->pValueFont))
                                != GFX_STATUS_SUCCESS)
                        {
                            return (GFX_STATUS_FAILURE);
                        }
                        break;
                }
                state = VALUE_DRAW;
    
            case VALUE_DRAW:

                if (GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
                    return (GFX_STATUS_FAILURE);

                if(angle >= ARC180_DEGREE)
                {
                    GFX_ColorSet(pMtr->color1);
                }
                else if(angle >= ARC135_DEGREE)
                {
                    GFX_ColorSet(pMtr->color2);
                }
                else if(angle >= ARC090_DEGREE)
                {
                    GFX_ColorSet(pMtr->color3);
                }
                else if(angle >= ARC045_DEGREE)
                {
                    GFX_ColorSet(pMtr->color4);
                }
                else if(angle >= ARC000_DEGREE)
                {
                    GFX_ColorSet(pMtr->color5);
                }
                else
                {
                    GFX_ColorSet(pMtr->color6);
                }
    
                // display the current value
                GFX_FontSet(pMtr->pValueFont);
    
                // this implements sprintf(strVal, "%03d", pMtr->value); faster
                // note that this is just for values >= 0, while sprintf covers negative values.
                i = pMtr->value;

                j = GFX_GOL_MeterConvertIntToString(strVal, i,  GFX_CONFIG_OBJECT_METER_SCALE_COUNT + 1);

                temp = GFX_TextStringWidthGet(&strVal[GFX_CONFIG_OBJECT_METER_SCALE_COUNT - j], pMtr->pValueFont);

                switch (pMtr->type)
                {
                    case GFX_GOL_METER_QUARTER_TYPE:
                        startX = pMtr->xCenter;
                        startY = pMtr->yCenter -
                                GFX_TextStringHeightGet(pMtr->pValueFont);
                        break;
                    case GFX_GOL_METER_HALF_TYPE:
                        startX = pMtr->xCenter - (temp >> 1);
                        startY = pMtr->yCenter -
                                 GFX_TextStringHeightGet(pMtr->pValueFont);
                        break;
                    case GFX_GOL_METER_WHOLE_TYPE:
                    default:
                        startX = pMtr->xCenter - (temp >> 1);
                        startY = pMtr->yCenter + pMtr->radius;
                        break;
                }
                state = VALUE_DRAW_RUN;
    
            case VALUE_DRAW_RUN:
                if (GFX_TextStringDraw(
                        startX,
                        startY,
                        &strVal[GFX_CONFIG_OBJECT_METER_SCALE_COUNT - j],
                        0 )
                    != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }

                state = IDLE;
#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                GFX_DRIVER_CompleteDrawUpdate(  pMtr->hdr.left,
                                                pMtr->hdr.top,
                                                pMtr->hdr.right,
                                                pMtr->hdr.bottom);
#endif
                return (GFX_STATUS_SUCCESS);
#endif // end of #ifdef GFX_CONFIG_OBJECT_METER_DISPLAY_VALUES_DISABLE
        }   // endo of switch()
    }  // end of while(1)
}



