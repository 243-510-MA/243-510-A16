/*******************************************************************************
 Module for Microchip Graphics Library - Graphic Object Layer

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_gol_scroll_bar.c

  Summary:
    This implements the scroll bar object of the GOL.

  Description:
    Refer to Microchip Graphics Library for complete documentation of the
    Slider Object.
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
#include "gfx/gfx_gol_scroll_bar.h"
#include "gfx/gfx_gol_scan_codes.h"

typedef union
{
    struct
    {
        uint8_t uint328BitValue[4];
    };

    struct
    {
        uint16_t uint3216BitValue[2];
    };

    uint32_t uint32Value;
}SLIDER_UINT32_UNION;

/* Internal Functions */
int16_t     GFX_GOL_ScrollBarThumbSizeSet(GFX_GOL_SCROLLBAR *pSld,
                                          int16_t high, int16_t low);
void        GFX_GOL_ScrollBarMinMaxPosGet(GFX_GOL_SCROLLBAR *pSld,
                                          uint16_t *minPos, uint16_t *maxPos);
uint16_t    GFX_GOL_ScrollBarWidthGet(GFX_GOL_SCROLLBAR *pSld);
uint16_t    GFX_GOL_ScrollBarHeightGet(GFX_GOL_SCROLLBAR *pSld);



/*********************************************************************
* Function: GFX_GOL_SCROLLBAR *SldCreate(uint16_t ID, int16_t left, int16_t top, int16_t right,
*							  int16_t bottom, uint16_t state, int16_t range, 
*							  int16_t page, int16_t pos, GFX_GOL_OBJ_SCHEME *pScheme)
*
* Notes: Creates a GFX_GOL_SCROLLBAR object and adds it to the current active list.
*        If the creation is successful, the pointer to the created Object 
*        is returned. If not successful, NULL is returned.
*
********************************************************************/
GFX_GOL_SCROLLBAR  *GFX_GOL_ScrollBarCreate(
                                uint16_t            ID,
                                uint16_t            left,
                                uint16_t            top,
                                uint16_t            right,
                                uint16_t            bottom,
                                uint16_t            state,
                                uint16_t            range,
                                uint16_t            page,
                                uint16_t            pos,
                                GFX_GOL_OBJ_SCHEME  *pScheme)
{
    GFX_GOL_SCROLLBAR  *pSld = NULL;

    pSld = (GFX_GOL_SCROLLBAR *)GFX_malloc(sizeof(GFX_GOL_SCROLLBAR));
    if(pSld == NULL)
        return (pSld);

    pSld->hdr.ID        = ID;                           // unique id assigned for referencing
    pSld->hdr.pNxtObj   = NULL;
    pSld->hdr.type      = GFX_GOL_SCROLLBAR_TYPE;       // set object type
    pSld->hdr.left      = left;                         // left and right should be equal when oriented vertically
    pSld->hdr.top       = top;                          // top and bottom should be equal when oriented horizontally
    pSld->hdr.right     = right;
    pSld->hdr.bottom    = bottom;
    pSld->hdr.state     = state;
    pSld->hdr.DrawObj   = GFX_GOL_ScrollBarDraw;        // draw function
    pSld->hdr.actionGet = GFX_GOL_ScrollBarActionGet; 	// message function
    pSld->hdr.actionSet = GFX_GOL_ScrollBarActionSet;   // default message function
    pSld->hdr.FreeObj   = NULL;                         // default free function

    // Parameters in the user defined range system (pos, page and range)

    // range of the scroll bar movement (always measured from 0 to range)
    if (range < 2)
        range = 2;
    else if((uint16_t) range > (uint16_t) 0x7FFF)
        range = 0x7FFF;
    pSld->range = range;

    // 0 refers to pSld->minPos and
    // range refers to pSld->maxpos where: minPos and maxPos are
    // the coordinate equivalent of 0 and range value

    // set the resolution
    if(page < 1)
        page = 1;
    else if(page > ((pSld->range) >> 1))
        page = (pSld->range) >> 1;
    pSld->page = page;

    // set the initial position
    pSld->pos     = pos;
    pSld->prevPos = pos;

    // Set the color scheme to be used
    pSld->hdr.pGolScheme = (GFX_GOL_OBJ_SCHEME *)pScheme;

    // calculate the thumb width and height
    pSld->thWidth = GFX_GOL_ScrollBarWidthGet(pSld);
    pSld->thHeight = GFX_GOL_ScrollBarHeightGet(pSld);

    // add the new object to the current list
    GFX_GOL_ObjectAdd((GFX_GOL_OBJ_HEADER *)pSld);
    return (pSld);
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_ScrollBarRangeSet(
                                GFX_GOL_SCROLLBAR *pObject,
                                uint16_t range)

    Summary:
        This function sets the range of the thumb of the scroll bar.

    Description:
        This function sets the range of the thumb of the scroll bar.
        When the range is modified, object must be completely redrawn to
        reflect the change.

*/
// *****************************************************************************
void GFX_GOL_ScrollBarRangeSet(GFX_GOL_SCROLLBAR *pObject, uint16_t range)
{
    uint16_t            newPos;
    SLIDER_UINT32_UNION dTemp;

    // this checks for limits of the range (minimum is 2)
    if(range < 2)
        range = 2;
    else if((uint16_t) range > (uint16_t) 0x7FFF)
        range = 0x7FFF;

    dTemp.uint32Value = range * pObject->pos;
    dTemp.uint32Value = dTemp.uint32Value / pObject->range;

    // get new range
    newPos = dTemp.uint3216BitValue[0];

    // set the new range
    pObject->range = range;

    // now check the page, adjust when necessary
    // page maximum limit is range/2, minimum is 1
    if(pObject->page > ((pObject->range) >> 1))
    {
        if(!((pObject->range) >> 1))
            pObject->page = 1;
        else
            pObject->page = (pObject->range) >> 1;
    }

    // calculate new thumb width and height
    pObject->thWidth = GFX_GOL_ScrollBarWidthGet(pObject);
    pObject->thHeight = GFX_GOL_ScrollBarHeightGet(pObject);
    GFX_GOL_ScrollBarPositionSet(pObject, newPos);
}

// *****************************************************************************
/*  Function:
    uint16_t GFX_GOL_ScrollBarRangeGet(
                                GFX_GOL_SCROLLBAR *pObject)

    Summary:
        This function returns the range of the thumb of the scroll bar.

    Description:
        This function returns the range of the thumb of the scroll bar.

*/
// *****************************************************************************
uint16_t __attribute__((always_inline)) GFX_GOL_ScrollBarRangeGet(
                                GFX_GOL_SCROLLBAR *pObject)
{
    return (pObject->range);
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_ScrollBarPageSet(
                                GFX_GOL_SCROLLBAR *pObject,
                                uint16_t page)

    Summary:
        This function sets the page size of the object.

    Description:
        This function sets the page size of the object. Page size
        defines the delta change of the thumb position when incremented
        via GFX_GOL_ScrollBarPositionIncrement() or decremented via
        GFX_GOL_ScrollBarPositionDecrement(). Page size minimum value is 1.
        Maximum value is range/2.

        Modifying the page size at run time may require a redraw of
        the object to show the visual effect of the change.

*/
// *****************************************************************************
void GFX_GOL_ScrollBarPageSet(GFX_GOL_SCROLLBAR *pObject, uint16_t page)
{
    if(page < 1)
        page = 1;
    else if(page > ((pObject->range) >> 1))
        page = (pObject->range) >> 1;

    pObject->page = page;

    // calculate new thumb width and height
    pObject->thWidth = GFX_GOL_ScrollBarWidthGet(pObject);
    pObject->thHeight = GFX_GOL_ScrollBarHeightGet(pObject);
}

// *****************************************************************************
/*  Function:
    uint16_t GFX_GOL_ScrollBarPageGet(
                                GFX_GOL_SCROLLBAR *pObject)

    Summary:
        This function returns the page size of the object.

    Description:
        This function returns the page size of the object. Page size
        defines the delta change of the thumb position when incremented
        via GFX_GOL_ScrollBarPositionIncrement() or decremented via
        GFX_GOL_ScrollBarPositionDecrement(). Page size minimum value is 1.
        Maximum value is range/2.

*/
// *****************************************************************************
uint16_t __attribute__ ((always_inline)) GFX_GOL_ScrollBarPageGet(
                                GFX_GOL_SCROLLBAR *pObject)
{
    return (pObject->page);
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_ScrollBarPositionSet(
                                GFX_GOL_SCROLLBAR *pObject,
                                uint16_t position)

    Summary:
        This function sets the position of the scroll bar thumb.

    Description:
        This function sets the position of the scroll bar thumb.
        The thumb is the rectangular area that slides left or
        right (for horizontal orientation) or slides up or down (for
        vertical orientation).

        The value used for the position should be within the range
        set for the object.

        Function will have an undefined behavior if the position
        is outside the range.

*/
// *****************************************************************************
void GFX_GOL_ScrollBarPositionSet(GFX_GOL_SCROLLBAR *pObject, uint16_t position)
{
    uint16_t            minPos, maxPos, relPos;
    SLIDER_UINT32_UNION dTemp;

    // get minimum and maximum positions
    GFX_GOL_ScrollBarMinMaxPosGet(pObject, &minPos, &maxPos);
    dTemp.uint32Value = 0;

//#ifndef SLD_INVERT_VERTICAL

    // check if the new value is still in range
    if(position <= 0)
    {
        pObject->pos = 0;                // set to zero in range domain
        if(GFX_GOL_ObjectStateGet(pObject, GFX_GOL_SCROLLBAR_VERTICAL_STATE))
        {                                // min and max in vertical is inverted
            pObject->currPos = maxPos;   // minimum position is the bottom 
        }                                // position in coordinate domain
        else
        {
            pObject->currPos = minPos;   // minimum is left most position 
                                         // in coordinate domain
        }
    }
    else if(position >= pObject->range)
    {
        pObject->pos = pObject->range;   // set to maximum value in range domain
        if(GFX_GOL_ObjectStateGet(pObject, GFX_GOL_SCROLLBAR_VERTICAL_STATE))
        {                                // min and max in vertical is inverted
            pObject->currPos = minPos;   // maximum position is the top 
        }                                // position in coordinate domain
        else
        {
            pObject->currPos = maxPos;   // maximum is right most position 
                                         // in coordinate domain
        }
    }
    else
    {
        pObject->pos = position;         // get new position in range domain
        dTemp.uint3216BitValue[1] = position;
        dTemp.uint32Value = dTemp.uint32Value / pObject->range;
        dTemp.uint32Value = (maxPos - minPos) * dTemp.uint32Value;

        // set current position in coordinate domain
        relPos = dTemp.uint3216BitValue[1] + minPos;

        // test if we need to transform min and max position
        if(GFX_GOL_ObjectStateGet(pObject, GFX_GOL_SCROLLBAR_VERTICAL_STATE))
        {                                               
            // min and max position is swapped in coordinate domain
            pObject->currPos = maxPos - (relPos - minPos);
        }
        else
        {
            // use position
            pObject->currPos = relPos;                     
        }
    }

}

// *****************************************************************************
/*  Function:
    uint16_t GFX_GOL_ScrollBarPositionGet(
                                GFX_GOL_SCROLLBAR *pObject)

    Summary:
        This function returns the current position of the scroll bar
        thumb.

    Description:
        This function returns the current position of the scroll bar
        thumb. The thumb is the rectangular area that slides left or
        right (for horizontal orientation) or slides up or down (for
        vertical orientation).
    
*/
// *****************************************************************************
uint16_t __attribute__ ((always_inline)) GFX_GOL_ScrollBarPositionGet(
                                GFX_GOL_SCROLLBAR *pObject)
{
    return (pObject->pos);
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_ScrollBarPositionIncrement(
                                GFX_GOL_SCROLLBAR *pObject)

    Summary:
        This function increments the scroll bar position by the delta
        change (page) value set.

    Description:
        This function increments the scroll bar position by the delta
        change (page) value set. Object must be redrawn after
        this function is called to reflect the changes to the object.

*/
// *****************************************************************************
void GFX_GOL_ScrollBarPositionIncrement(
                                GFX_GOL_SCROLLBAR *pObject)
{
    int16_t value;

    value = pObject->pos + pObject->page;

    if (value > pObject->range)
        value = pObject->range;

    GFX_GOL_ScrollBarPositionSet(pObject, value);
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_ScrollBarPositionDecrement(
                                GFX_GOL_SCROLLBAR *pObject)

    Summary:
        This function decrements the scroll bar position by the delta
        change (page) value set.

    Description:
        This function decrements the scroll bar position by the delta
        change (page) value set. Object must be redrawn after
        this function is called to reflect the changes to the object.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.

    Returns:
        None.

    Example:
        <code>

            void ControlSpeed(  GFX_GOL_SCROLLBAR* pObj,
                                int setSpeed,
                                int curSpeed)
            {
                // set page size to 1
                GFX_GOL_ScrollBarPageSet(pObj, 1);

                if (setSpeed < curSpeed)
                {
                    while(GFX_GOL_ScrollBarPositionGet(pObj) < SetSpeed)
                        GFX_GOL_ScrollBarPositionIncrement(pObj);  // increment by 1
                }
                else if (setSpeed > curSpeed)
                {
                    while(GFX_GOL_ScrollBarPositionGet(pObj) > SetSpeed)
                        GFX_GOL_ScrollBarPositionDecrement(pObj);  // decrement by 1
                }
            }
        </code>

*/
// *****************************************************************************
void GFX_GOL_ScrollBarPositionDecrement(
                                GFX_GOL_SCROLLBAR *pObject)
{
    int16_t value;

    value = pObject->pos - pObject->page;

    if (value < 0)
        value = 0;

    GFX_GOL_ScrollBarPositionSet(pObject, value);

}

/*********************************************************************
* Function: int16_t GFX_GOL_ScrollBarThumbSizeSet(GFX_GOL_SCROLLBAR *pSld, int16_t high, int16_t low)
*
* Notes: An INTERNAL function used to compute for the width or 
*        height of the thumb. This function is created to save
*        code size. This function is called only to dynamically
*        compute for the thumb size. Used only when slider is 
* 		 type Scrollbar. Parameter are defined as:
*		 pSld - pointer to the object
*        high - higher value to be used
*        low  - lower value to be used
*
********************************************************************/
int16_t GFX_GOL_ScrollBarThumbSizeSet(GFX_GOL_SCROLLBAR *pSld, int16_t high, int16_t low)
{
    uint16_t    temp, emboss;

    temp = (pSld->range / pSld->page);
    temp = (high - low) / temp;

    emboss = pSld->hdr.pGolScheme->EmbossSize;

    // when size is less than half of emboss size, set the
    // size to half the emboss size. This is to make sure
    // thumb will always have a size.
    if(temp < (emboss << 1))
        temp = (emboss << 1);

    return (int16_t) temp;
}

/*********************************************************************
* Function: uint16_t GFX_GOL_ScrollBarWidthGet(GFX_GOL_SCROLLBAR *pSld)
*
* Notes: An INTERNAL function that computes for the width  
*        of the thumb. This function is created to save
*        code size. This function is called only to dynamically
*        compute for the thumb size. 
*
********************************************************************/
uint16_t GFX_GOL_ScrollBarWidthGet(GFX_GOL_SCROLLBAR *pSld)
{
    uint16_t temp, emboss;

    /*
		Calculating the width is dependent on the mode type.
		If type Scrollbar, width is dependent on the ratio of the
		page/range = width/max-min (see SetThumbSize())
		if type is Slider, width is dependent on height*3/8
		
		When horizontal width is dynamic, height is contant.
	
	*/

    emboss = pSld->hdr.pGolScheme->EmbossSize;
    if (emboss < 3)
        emboss = 2;
    
    if(GFX_GOL_ObjectStateGet(pSld, GFX_GOL_SCROLLBAR_VERTICAL_STATE))
    {
        temp = pSld->hdr.right - pSld->hdr.left;
        if(GFX_GOL_ObjectStateGet(pSld, GFX_GOL_SCROLLBAR_SLIDER_MODE_STATE))
        {
            temp = temp - (emboss << 1) - 2;
        }
        else
        {
            temp = temp - (emboss << 1);
        }
    }
    else
    {
        if(GFX_GOL_ObjectStateGet(pSld, GFX_GOL_SCROLLBAR_SLIDER_MODE_STATE))
        {
            temp = (((pSld->hdr.bottom - pSld->hdr.top) -
                    (emboss << 1) - 2) * 3) >> 3;
        }
        else
        {
            temp = GFX_GOL_ScrollBarThumbSizeSet(
                                            pSld,
                                            pSld->hdr.right,
                                            pSld->hdr.left);
        }
    }

    // to avoid calculations of dividing by two, we store half the width value
    return (temp >> 1);
}

/*********************************************************************
* Function: uint16_t GFX_GOL_ScrollBarHeightGet(GFX_GOL_SCROLLBAR *pSld)
*
* Notes: An INTERNAL function that computes for the height  
*        of the thumb. This function is created to save
*        code size. This function is called only to dynamically
*        compute for the thumb size.
*
********************************************************************/
uint16_t GFX_GOL_ScrollBarHeightGet(GFX_GOL_SCROLLBAR *pSld)
{
    uint16_t temp, emboss;

    /*
		Calculating the height is dependent on the mode type.
		If type Scrollbar, width is dependent on the ratio of the
		page/range = width/max-min (see SetThumbSize())
		if type is Slider, width is dependent on width*3/8
	
		When vertical height is dynamic, width is contant.
	*/

    emboss = pSld->hdr.pGolScheme->EmbossSize;
    if (emboss < 3)
        emboss = 2;

    if(GFX_GOL_ObjectStateGet(pSld, GFX_GOL_SCROLLBAR_VERTICAL_STATE))
    {
        if(GFX_GOL_ObjectStateGet(pSld, GFX_GOL_SCROLLBAR_SLIDER_MODE_STATE))
        {
            temp = (((pSld->hdr.right - pSld->hdr.left) -
                    (emboss << 1) - 2) * 3) >> 3;
        }
        else
        {
            temp = GFX_GOL_ScrollBarThumbSizeSet(   pSld,
                                                    pSld->hdr.bottom,
                                                    pSld->hdr.top);
        }
    }
    else
    {
        temp = pSld->hdr.bottom - pSld->hdr.top;
        if(GFX_GOL_ObjectStateGet(pSld, GFX_GOL_SCROLLBAR_SLIDER_MODE_STATE))
        {
            temp = temp - (emboss << 1) - 2;
        }
        else
        {
            temp = temp - (emboss << 1);
        }
    }

    // to avoid calculations of dividing by two,
    // we store half the height value
    return (temp >> 1);
}

/*********************************************************************
* Function: void GFX_GOL_ScrollBarMinMaxPosGet(GFX_GOL_SCROLLBAR *pSld, uint16_t *min, uint16_t *max)
*
* Notes:  An INTERNAL function that computes for the minimum
*         and maximum pixel position in the screen. This function is 
*         created to save code size. Used to define the minimum 
*         & maximum position of the thumb when sliding. Parameters
*		  used are defined as:
*		  pSld - pointer to the object
*         min  - pointer to the minimum variable
*         max  - pointer to the maximum variable
*
********************************************************************/
void GFX_GOL_ScrollBarMinMaxPosGet(GFX_GOL_SCROLLBAR *pSld, uint16_t *min, uint16_t *max)
{
    uint16_t    temp;

    // calculate maximum and minimum position	
    if(GFX_GOL_ObjectStateGet(pSld, GFX_GOL_SCROLLBAR_VERTICAL_STATE))
    {
        temp = pSld->thHeight   + pSld->hdr.pGolScheme->EmbossSize;
        *min = pSld->hdr.top    + temp;
        *max = pSld->hdr.bottom - temp;
    }
    else
    {
        temp = pSld->thWidth    + pSld->hdr.pGolScheme->EmbossSize;
        *min = pSld->hdr.left   + temp;
        *max = pSld->hdr.right  - temp;
    }

    // for aestetics.
    if(GFX_GOL_ObjectStateGet(pSld, GFX_GOL_SCROLLBAR_SLIDER_MODE_STATE))
    {
        *min = *min + 2;
        *max = *max - 2;
    }
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_ScrollBarActionSet(
                                GFX_GOL_TRANSLATED_ACTION  translatedMsg,
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
void GFX_GOL_ScrollBarActionSet(
                                GFX_GOL_TRANSLATED_ACTION  translatedMsg,
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage)
{

    GFX_GOL_SCROLLBAR *pSld;

    pSld = (GFX_GOL_SCROLLBAR *)pObject;

#ifndef GFX_CONFIG_USE_TOUCHSCREEN_DISABLE

    int16_t             newPos;
    uint16_t            minPos, maxPos;
    SLIDER_UINT32_UNION dTemp;

#ifndef GFX_CONFIG_FOCUS_DISABLE

    if(pMessage->type == TYPE_TOUCHSCREEN)
    {
        if(!GFX_GOL_ObjectStateGet(pSld, GFX_GOL_SCROLLBAR_FOCUSED_STATE))
        {
            GFX_GOL_ObjectFocusSet((GFX_GOL_OBJ_HEADER *)pSld);
        }
    }

#endif 

    // if message was passive do not do anything
    if (translatedMsg == GFX_GOL_OBJECT_ACTION_PASSIVE)
        return;
		
    // get the min and max positions
    GFX_GOL_ScrollBarMinMaxPosGet(pSld, &minPos, &maxPos);

    if(pMessage->type == TYPE_TOUCHSCREEN)
    {
        if((translatedMsg == GFX_GOL_SCROLLBAR_ACTION_DEC) || (translatedMsg == GFX_GOL_SCROLLBAR_ACTION_INC))
        {

            // newPos in this context is used in the coordinate domain
            // check if Horizontal or Vertical orientation
            if(!GFX_GOL_ObjectStateGet(pSld, GFX_GOL_SCROLLBAR_VERTICAL_STATE))
            {
                // Horizontal orientation: test x position
                if(pMessage->param1 <= minPos)
                {
                    // beyond minimum, use min position
                    newPos = minPos;
                }
                else if(pMessage->param1 >= maxPos)
                {
                    // beyond maximum, use max position
                    newPos = maxPos;
                }
                else
                {
                    // within range: use x position given
                    newPos = pMessage->param1;
                }
            }
            else
            {
                // Vertical orientation: test y position
                if(pMessage->param2 <= minPos)
                {
                    // beyond minimum, use min position
                    newPos = minPos;
                }
                else if(pMessage->param2 >= maxPos)
                {
                    // beyond maximum, use max position
                    newPos = maxPos;
                }
                else
                {
                    // within range: use y position given
                    newPos = pMessage->param2;
                }
            }

            // check if we need to redraw thumb
            if(newPos != pSld->currPos)
            {
                // yes redraw is needed, translate newPos into range domain
                // first get new position in range domain
                dTemp.uint32Value = (uint32_t) (newPos - minPos) *
                                    (uint32_t) pSld->range;
                dTemp.uint32Value = dTemp.uint32Value / (maxPos - minPos);
                newPos = dTemp.uint3216BitValue[0];

                // check if we need to swap min and max in vertical
                if(GFX_GOL_ObjectStateGet(  pSld,
                                            GFX_GOL_SCROLLBAR_VERTICAL_STATE))
                {
                    // min and max is swapped in vertical orientation
                    newPos = pSld->range - newPos;
                }

                // set to new position
                GFX_GOL_ScrollBarPositionSet(pSld, newPos);

                // redraw the thumb only
                GFX_GOL_ObjectStateSet( pSld,
                                        GFX_GOL_SCROLLBAR_DRAW_THUMB_STATE);
            }
            else
                return;
        }
        else
            return;
    }

#endif // GFX_CONFIG_USE_TOUCHSCREEN_DISABLE
    
#ifndef GFX_CONFIG_USE_KEYBOARD_DISABLE
    // for keyboard
    if(pMessage->type == TYPE_KEYBOARD)
    {
        if(translatedMsg == GFX_GOL_SCROLLBAR_ACTION_INC)
        {
            // increment is requested
            GFX_GOL_ScrollBarPositionIncrement(pSld);
        }
        else if (translatedMsg == GFX_GOL_SCROLLBAR_ACTION_DEC)
        {
            // decrement is requested
            GFX_GOL_ScrollBarPositionDecrement(pSld);
        }
        else
        {
            // do nothing
            return;
        }
        // redraw the thumb only
        GFX_GOL_ObjectStateSet(pSld, GFX_GOL_SCROLLBAR_DRAW_THUMB_STATE);
    }

#endif // #ifndef GFX_CONFIG_USE_KEYBOARD_DISABLE

}

// *****************************************************************************
/*  Function:
    GFX_GOL_TRANSLATED_ACTION GFX_GOL_ScrollBarActionGet(
                                void *pObject, GFX_GOL_MESSAGE *pMessage);

    Summary:
        This function evaluates the message from a user if the
        message will affect the object or not.

    Description:
        This function evaluates the message from a user if the
        message will affect the object or not.

*/
// *****************************************************************************
GFX_GOL_TRANSLATED_ACTION GFX_GOL_ScrollBarActionGet(
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage)
{

    GFX_GOL_SCROLLBAR   *pSld;
#ifndef GFX_CONFIG_USE_TOUCHSCREEN_DISABLE
    uint16_t            temp;
#endif
    pSld = (GFX_GOL_SCROLLBAR *)pObject;

    // Evaluate if the message is for the slider
    // Check if disabled first
    if(GFX_GOL_ObjectStateGet(pSld, GFX_GOL_SCROLLBAR_DISABLED_STATE))
        return (GFX_GOL_OBJECT_ACTION_INVALID);

#ifndef GFX_CONFIG_USE_TOUCHSCREEN_DISABLE
    if(pMessage->type == TYPE_TOUCHSCREEN)
    {
        // Check if it falls to the left or right of the center of the thumb's face
        if( ((pMessage->uiEvent == EVENT_PRESS) ||
             (pMessage->uiEvent == EVENT_MOVE)) &&
            ((pSld->hdr.left     < pMessage->param1) &&
             (pSld->hdr.right    > pMessage->param1) &&
             (pSld->hdr.top      < pMessage->param2) &&
             (pSld->hdr.bottom   > pMessage->param2))
          )
        {
            if(GFX_GOL_ObjectStateGet(  pSld,
                                        GFX_GOL_SCROLLBAR_VERTICAL_STATE))
            {
                temp = (pSld->thHeight * 3);
                if( (pSld->hdr.left       < pMessage->param1) &&
                    (pSld->hdr.right      > pMessage->param1) &&
                    (pSld->currPos - temp < pMessage->param2) &&
                    (pSld->currPos + temp > pMessage->param2))
                {
                    // add this check for jitter
                    if (pMessage->param2 > pSld->currPos + 2)
                       return (GFX_GOL_SCROLLBAR_ACTION_INC);
                    else if (pMessage->param2 < pSld->currPos - 2)
                       return (GFX_GOL_SCROLLBAR_ACTION_DEC);
                    else
                       return (GFX_GOL_OBJECT_ACTION_INVALID);
                }
            }
            else
            {
                temp = (pSld->thWidth * 3);
                if( (pSld->currPos - temp < pMessage->param1) &&
                    (pSld->currPos + temp > pMessage->param1) &&
                    (pSld->hdr.top        < pMessage->param2) &&
                    (pSld->hdr.bottom     > pMessage->param2))
                {
                    // add this check for jitter
                    if (pMessage->param1 > pSld->currPos + 2)
                       return (GFX_GOL_SCROLLBAR_ACTION_INC);
                    else if (pMessage->param1 < pSld->currPos - 2)
                       return (GFX_GOL_SCROLLBAR_ACTION_DEC);
                    else
                       return (GFX_GOL_OBJECT_ACTION_INVALID);

                }
            }

        }   // end of if((pMsg->uiEvent == EVENT_PRESS) ...

        // when the event is release emit GFX_GOL_OBJECT_ACTION_PASSIVE this can 
        // be used to detect that the release event happened on
        // the scroll bar.

        if(pMessage->uiEvent == EVENT_RELEASE)
            return GFX_GOL_OBJECT_ACTION_PASSIVE;
        
        return (GFX_GOL_OBJECT_ACTION_INVALID);

    }       // end of if(pMsg->type == TYPE_TOUCHSCREEN
#endif
#ifndef GFX_CONFIG_USE_KEYBOARD_DISABLE
    if(pMessage->type == TYPE_KEYBOARD)
    {
        if((uint16_t)pMessage->param1 == pSld->hdr.ID)
        {
            if(pMessage->uiEvent == EVENT_KEYSCAN)
            {
                if( (pMessage->param2 == SCAN_RIGHT_PRESSED) ||
                    (pMessage->param2 == SCAN_UP_PRESSED))
                {
                    return (GFX_GOL_SCROLLBAR_ACTION_INC);
                }

                if( (pMessage->param2 == SCAN_LEFT_PRESSED) ||
                    (pMessage->param2 == SCAN_DOWN_PRESSED))
                {
                    return (GFX_GOL_SCROLLBAR_ACTION_DEC);
                }    
            }
        }
    }
#endif
    
    return (GFX_GOL_OBJECT_ACTION_INVALID);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_GOL_ScrollBarDraw(void *pObject)

    Summary:
        This function renders the object on the screen based on the
        current state of the object.

    Description:
        This function renders the object on the screen based on the
        current state of the object. Location of the object is
        determined by the left, top, right and bottom parameters.
        The colors used are dependent on the state of the object.
        The font used is determined by the style scheme set.

        The text on the face of the GFX_GOL_SCROLLBAR is drawn on top of
        the bitmap. Text alignment based on the alignment parameter
        set on the object.

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
GFX_STATUS GFX_GOL_ScrollBarDraw(void *pObject)
{
    typedef enum
    {
        SLD_STATE_IDLE,
        SLD_STATE_HIDE,
        SLD_STATE_PANEL,
        SLD_STATE_THUMBPATH1,
        SLD_STATE_THUMBPATH2,
        SLD_STATE_CLEARTHUMB_CHECK,
        SLD_STATE_CLEARTHUMB,
        SLD_STATE_REDRAWPATH1,
        SLD_STATE_REDRAWPATH2,
        SLD_STATE_THUMB,
        SLD_STATE_THUMBPANEL,
        SLD_STATE_FOCUS
    } SLD_DRAW_STATES;

    static GFX_COLOR colorTemp = 0;

    static SLD_DRAW_STATES  state = SLD_STATE_IDLE;
    static uint16_t         left, top, right, bottom;
    static uint16_t         midPoint, thWidth, thHeight;
    static uint16_t         minPos, maxPos, embossSize;
    static GFX_FILL_STYLE   fillStyle;
    GFX_GOL_SCROLLBAR       *pSld;

    pSld = (GFX_GOL_SCROLLBAR *)pObject;

    while(1)
    {
        if (GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
            return (GFX_STATUS_FAILURE);

        switch(state)
        {
            case SLD_STATE_IDLE:
#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                GFX_DRIVER_SetupDrawUpdate(         \
                        pSld->hdr.left,             \
                        pSld->hdr.top,              \
                        pSld->hdr.right,            \
                        pSld->hdr.bottom);
#endif

                // set the background information
                GFX_GOL_PanelBackgroundSet(&pSld->hdr);

#if defined (GFX_CONFIG_ALPHABLEND_DISABLE) && !defined (GFX_CONFIG_GRADIENT_DISABLE)
                // scroll bar will not draw gradient on the main
                // panel, only on the thumb
                fillStyle = GFX_FILL_STYLE_COLOR;
#else
                fillStyle = pSld->hdr.pGolScheme->fillStyle;
#endif

                // set the panel color
                if (!GFX_GOL_ObjectStateGet (
                                        pSld,
                                        GFX_GOL_SCROLLBAR_DISABLED_STATE))
                {
                    // select enabled color
                    colorTemp = pSld->hdr.pGolScheme->Color0;
                }
                else
                {
                    // select disabled color
                    colorTemp = pSld->hdr.pGolScheme->ColorDisabled;
                }

                if (GFX_GOL_ObjectStateGet( pSld,
                                            GFX_GOL_SCROLLBAR_HIDE_STATE))
                {
                    // **************************************
                    // To be hidden
                    // **************************************
                    // set to common background color
                    GFX_ColorSet(pSld->hdr.pGolScheme->CommonBkColor);

                    state = SLD_STATE_HIDE;
                    // no break here so it falls through to SLD_STATE_HIDE
                }
                else 
                {
                    // **************************************
                    // Not hidden
                    // **************************************

                    GFX_GOL_ScrollBarMinMaxPosGet(pSld, &minPos, &maxPos);

                    if (GFX_GOL_ObjectStateGet( 
                                            pSld,
                                            GFX_GOL_SCROLLBAR_VERTICAL_STATE))
                        midPoint = (pSld->hdr.left + pSld->hdr.right) >> 1;
                    else
                        midPoint = (pSld->hdr.top + pSld->hdr.bottom) >> 1;

                    // calculate the thumb width and height. Actually gets the
                    // half value(see calculation of width and height)
                    // GFX_GOL_ScrollBarWidthGet() and
                    // GFX_GOL_ScrollBarHeightGet()
                    thWidth = pSld->thWidth;
                    thHeight = pSld->thHeight; 
                    
                    GFX_LineStyleSet(GFX_LINE_STYLE_THIN_SOLID);

                    embossSize = pSld->hdr.pGolScheme->EmbossSize;
                    if (embossSize < 3)
                        embossSize = 1;

                    if(GFX_GOL_ObjectStateGet(
                                            pSld,
                                            GFX_GOL_SCROLLBAR_DRAW_STATE))
                    {

                        // draw the panel for the scroll bar
                        // modify the color setting if scroll bar mode or slider mode
    			GFX_GOL_PanelParameterSet(
                            pSld->hdr.left,
                            pSld->hdr.top,
                            pSld->hdr.right,
                            pSld->hdr.bottom,
                            0,
                            colorTemp,
                            (GFX_GOL_ObjectStateGet(
                                            pSld,
                                            GFX_GOL_SCROLLBAR_SLIDER_MODE_STATE)) ?
                                pSld->hdr.pGolScheme->EmbossLtColor : pSld->hdr.pGolScheme->EmbossDkColor,
                            (GFX_GOL_ObjectStateGet(
                                            pSld,
                                            GFX_GOL_SCROLLBAR_SLIDER_MODE_STATE)) ?
                                pSld->hdr.pGolScheme->EmbossDkColor : pSld->hdr.pGolScheme->EmbossLtColor,
                            NULL,
                            fillStyle,
                            embossSize);
    
    			// initialize current and previous position
    			GFX_GOL_ScrollBarPositionSet(pSld, pSld->pos);
    			pSld->prevPos = pSld->currPos;
    
    			state = SLD_STATE_PANEL;
    			break;
                    }
                    else
                    {
                        // we do not need to draw the whole object
                        // go to thumb drawing
                        state = SLD_STATE_CLEARTHUMB_CHECK;
    			break;
                    }
                }

            case SLD_STATE_HIDE:

                // Hide the GFX_GOL_SCROLLBAR (remove from screen)
                if (GFX_GOL_ObjectHideDraw(&(pSld->hdr)) != GFX_STATUS_SUCCESS)
                    return (GFX_STATUS_FAILURE);

                state = SLD_STATE_IDLE;
#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                GFX_DRIVER_CompleteDrawUpdate(  pSld->hdr.left,
                                                pSld->hdr.top,
                                                pSld->hdr.right,
                                                pSld->hdr.bottom);
#endif
                return (GFX_STATUS_SUCCESS);
				
            case SLD_STATE_PANEL:

                // draw the panel of the slider
                if(GFX_GOL_PanelDraw() != GFX_STATUS_SUCCESS)
                    return (GFX_STATUS_FAILURE);

                // check if slider or scroll bar
		if(GFX_GOL_ObjectStateGet(  
                                        pSld,
                                        GFX_GOL_SCROLLBAR_SLIDER_MODE_STATE))
                {
                    GFX_LineStyleSet(GFX_LINE_STYLE_THIN_SOLID);
                    // slider: draw thumb path next
                    state = SLD_STATE_THUMBPATH1;
                }
		else
                {
                    // scrollbar: go directly to thumb drawing
                    // thumb path is not drawn in scrollbar
                    state = SLD_STATE_THUMB;
                    break;
                }

            case SLD_STATE_THUMBPATH1:
                GFX_ColorSet(pSld->hdr.pGolScheme->EmbossDkColor);
                if(!GFX_GOL_ObjectStateGet( pSld,
                                            GFX_GOL_SCROLLBAR_VERTICAL_STATE))
                {
                    if(GFX_LineDraw(
                            minPos,
                            midPoint,
                            maxPos,
                            midPoint) != GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }
                    
                }
                else
                {
                    if(GFX_LineDraw(
                            midPoint,
                            minPos,
                            midPoint,
                            maxPos) != GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }
                }

		state = SLD_STATE_THUMBPATH2;

            case SLD_STATE_THUMBPATH2:
                GFX_ColorSet(pSld->hdr.pGolScheme->EmbossLtColor);

                if(!GFX_GOL_ObjectStateGet( pSld,
                                            GFX_GOL_SCROLLBAR_VERTICAL_STATE))
                {
                    if(GFX_LineDraw(
                            minPos,
                            midPoint + 1,
                            maxPos,
                            midPoint + 1) != GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }
                }
                else
                {
                    if(GFX_LineDraw(
                            midPoint + 1,
                            minPos,
                            midPoint + 1,
                            maxPos) != GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }
                }

                if(GFX_GOL_ObjectStateGet(  pSld,
                                            GFX_GOL_SCROLLBAR_DRAW_STATE))
                {
                    // if drawing the whole slider
                    // go straight to drawing the thumb
                    state = SLD_STATE_THUMB;
                    break;
                }
                else
                {
                    // if just drawing the thumb
                    // go to state to remove current position
                    state = SLD_STATE_CLEARTHUMB_CHECK;
                }

            case SLD_STATE_CLEARTHUMB_CHECK:
                // this removes the current thumb

                if(!GFX_GOL_ObjectStateGet(
                                    pSld,
                                    GFX_GOL_SCROLLBAR_DRAW_THUMB_STATE))
                {
                    // GFX_GOL_SCROLLBAR_DRAW_THUMB_STATE is only set when
                    // object type is GFX_GOL_SCROLLBAR
                    state = SLD_STATE_FOCUS;
                    break;
                }

                GFX_ColorSet(colorTemp);
                
                // Remove the current thumb by drawing a bar 
                // with the background 
                GFX_GOL_ObjectBackGroundSet(&pSld->hdr);

#if defined (GFX_CONFIG_ALPHABLEND_DISABLE) && !defined (GFX_CONFIG_GRADIENT_DISABLE)
                // scroll bar will not draw gradient on the main
                // panel, only on the thumb
                fillStyle = GFX_FILL_STYLE_COLOR;
#else
                fillStyle = pSld->hdr.pGolScheme->fillStyle;
#endif

                state = SLD_STATE_CLEARTHUMB;

            case SLD_STATE_CLEARTHUMB:

                GFX_FillStyleSet(fillStyle);

                if(!GFX_GOL_ObjectStateGet(
                                    pSld,
                                    GFX_GOL_SCROLLBAR_VERTICAL_STATE))
                {
                    if(GFX_RectangleFillDraw(
                            pSld->prevPos - thWidth,
                            midPoint      - thHeight,
                            pSld->prevPos + thWidth,
                            midPoint      + thHeight) != GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }
                }
                else
                {
                    if(GFX_RectangleFillDraw(
                            midPoint      - thWidth,
                            pSld->prevPos - thHeight,
                            midPoint      + thWidth,
                            pSld->prevPos + thHeight) != GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }
                }

                // check if slider or scroll bar
                if(GFX_GOL_ObjectStateGet(
                                    pSld,
                                    GFX_GOL_SCROLLBAR_SLIDER_MODE_STATE))
                {
                    state = SLD_STATE_REDRAWPATH1;
                }
                else
                {
                    // go directly to thumb drawing
                    // thumb path is not drawn in scrollbar
                    state = SLD_STATE_THUMB;
                    break;
                }

            case SLD_STATE_REDRAWPATH1:

                // redraws the lines that it covered
                GFX_ColorSet(pSld->hdr.pGolScheme->EmbossDkColor);

                // Check if the redraw area exceeds the actual dimension. 
                // This will adjust the redrawing area to just within
                // the parameters
                if(!GFX_GOL_ObjectStateGet( pSld,
                                            GFX_GOL_SCROLLBAR_VERTICAL_STATE))
                {
                    if(minPos + thWidth > pSld->prevPos)
                        left = minPos;
                    else
                        left = pSld->prevPos - thWidth;

                    if(maxPos - thWidth < pSld->prevPos)
                        right = maxPos;
                    else
                        right = pSld->prevPos + thWidth;

                    if(GFX_LineDraw(
                            left,
                            midPoint,
                            right,
                            midPoint) != GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }
                }
                else
                {
                    if(minPos + thHeight > pSld->prevPos)
                        top = minPos;
                    else
                        top = pSld->prevPos - thHeight;
					
                    if(maxPos - thHeight < pSld->prevPos)
                        bottom = maxPos;
                    else
                        bottom = pSld->prevPos + thHeight;
					
                    if(GFX_LineDraw(
                            midPoint,
                            top,
                            midPoint,
                            bottom) != GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }
                }

                state = SLD_STATE_REDRAWPATH2;

            case SLD_STATE_REDRAWPATH2:

                GFX_ColorSet(pSld->hdr.pGolScheme->EmbossLtColor);

                if(!GFX_GOL_ObjectStateGet( pSld,
                                            GFX_GOL_SCROLLBAR_VERTICAL_STATE))
                {
                    if(GFX_LineDraw(
                            left,
                            midPoint + 1,
                            right,
                            midPoint + 1) != GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }
                }
                else
                {
                    if (GFX_LineDraw(
                            midPoint + 1,
                            top,
                            midPoint + 1,
                            bottom) != GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }
                }

                state = SLD_STATE_THUMB;

            case SLD_STATE_THUMB:

                if (!GFX_GOL_ObjectStateGet(
                                        pSld,
                                        GFX_GOL_SCROLLBAR_VERTICAL_STATE))
                { // Draw the slider thumb based on the
                    // current position
                    left   = pSld->currPos - thWidth;
                    top    = midPoint - thHeight;
                    right  = pSld->currPos + thWidth;
                    bottom = midPoint + thHeight;
                }
                else
                {
                    left   = midPoint - thWidth;
                    top    = pSld->currPos - thHeight;
                    right  = midPoint + thWidth;
                    bottom = pSld->currPos + thHeight;
                }

                if (embossSize > 1)
                    embossSize -= 1;

#ifndef GFX_CONFIG_GRADIENT_DISABLE
                // scroll bar will draw gradient only on the thumb
                GFX_GOL_PanelGradientParameterSet(
                        pSld->hdr.pGolScheme->gradientStartColor,
                        pSld->hdr.pGolScheme->gradientEndColor);
#endif


                GFX_GOL_PanelParameterSet(
                    left,
                    top,
                    right,
                    bottom,
                    0, // set the parameters of the thumb
                    colorTemp,
                    pSld->hdr.pGolScheme->EmbossLtColor,
                    pSld->hdr.pGolScheme->EmbossDkColor,
                    NULL,
                    pSld->hdr.pGolScheme->fillStyle,
                    embossSize);

                state = SLD_STATE_THUMBPANEL;

            case SLD_STATE_THUMBPANEL:

                // draw the panel of the thumb
                if (GFX_GOL_PanelDraw() != GFX_STATUS_SUCCESS)
                    return (GFX_STATUS_FAILURE);

                // record the current position as previous
                pSld->prevPos = pSld->currPos;

                // check if scroll bar focus is not used
                if (!GFX_GOL_ObjectStateGet(
                                        pSld,
                                        GFX_GOL_SCROLLBAR_SLIDER_MODE_STATE))
                {
                    state = SLD_STATE_IDLE; // go back to idle state
#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                    GFX_DRIVER_CompleteDrawUpdate(
                        pSld->hdr.left,
                        pSld->hdr.top,
                        pSld->hdr.right,
                        pSld->hdr.bottom);
#endif

                    return (GFX_STATUS_SUCCESS);
                }

                if (!GFX_GOL_ObjectStateGet(
                                        pSld,
                                        GFX_GOL_SCROLLBAR_DRAW_FOCUS_STATE))
                {
                    state = SLD_STATE_IDLE;
#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                    GFX_DRIVER_CompleteDrawUpdate(pSld->hdr.left,
                            pSld->hdr.top,
                            pSld->hdr.right,
                            pSld->hdr.bottom);
#endif
                    return (GFX_STATUS_SUCCESS);
                }

                state = SLD_STATE_FOCUS;

            case SLD_STATE_FOCUS:
                if (GFX_GOL_ObjectStateGet(
                                        pSld,
                                        GFX_GOL_SCROLLBAR_SLIDER_MODE_STATE))
                { // do not draw focus when in scroll bar mode

                    GFX_LineStyleSet(GFX_LINE_STYLE_THIN_DASHED);

                    if (GFX_GOL_ObjectStateGet(
                                        pSld,
                                        GFX_GOL_SCROLLBAR_FOCUSED_STATE))
                    {
                        // draw the focus box
                        GFX_ColorSet(pSld->hdr.pGolScheme->TextColor0);
                    }
                    else
                    {
                        // remove the focus box, colorTemp
                        GFX_ColorSet(colorTemp);
                    }

                    if(GFX_RectangleDraw(
                        pSld->hdr.left   + embossSize, 
                        pSld->hdr.top    + embossSize, 
                        pSld->hdr.right  - embossSize, 
                        pSld->hdr.bottom - embossSize) 
                           != GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }

                    // reset line type
                    GFX_LineStyleSet(GFX_LINE_STYLE_THIN_SOLID);
                }

                state = SLD_STATE_IDLE; // set state to idle
#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                GFX_DRIVER_CompleteDrawUpdate(pSld->hdr.left,
                        pSld->hdr.top,
                        pSld->hdr.right,
                        pSld->hdr.bottom);
#endif

                // return as done
                return (GFX_STATUS_SUCCESS);
        }
    } // end of while(1)
}

