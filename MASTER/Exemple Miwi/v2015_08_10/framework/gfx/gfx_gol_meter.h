/*******************************************************************************
 Module for Microchip Graphics Library - Graphic Object Layer

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_gol_meter.h

  Summary:
    This is the header file for the meter object of the GOL.

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

#ifndef _GFX_GOL_METER_H
// DOM-IGNORE-BEGIN
    #define _GFX_GOL_METER_H
// DOM-IGNORE-END

#include <stdint.h>
#include <gfx/gfx_gol.h>

// *****************************************************************************
/* 
    <GROUP gol_types>

    Typedef:
        GFX_GOL_METER_STATE

    Summary:
        Specifies the different states of the Meter object.

    Description:
        This enumeration specifies the different states of the Meter
        object used in the library.

        For the Property State bits, more than one of these values may be 
        OR'd together to create a complete property state.  
        
        For the Draw State bits, hide draw bit has higher priority than the 
        draw bit. Any of these two can be combined with other draw bits to
        create a complete draw state.
        
        To test a value of any of the state types, the bit of interest must be
        AND'ed with value and checked to see if the result is non-zero.

    Remarks:
        None.
        
*/
// *****************************************************************************
typedef enum
{
    // Property bit for disabled state.
    GFX_GOL_METER_DISABLED_STATE    
        /*DOM-IGNORE-BEGIN*/  = 0x0002 /*DOM-IGNORE-END*/,  
    // Property bit for ring type, scales are drawn over the ring.
    // The default state of this state bit is disabled. Only scales are drawn.
    GFX_GOL_METER_RING_STATE   
        /*DOM-IGNORE-BEGIN*/  = 0x0004 /*DOM-IGNORE-END*/,  
    // Draw bit to indicate update of the meter-hand only.
    GFX_GOL_METER_UPDATE_DRAW_STATE   
        /*DOM-IGNORE-BEGIN*/  = 0x1000 /*DOM-IGNORE-END*/,  
    // Draw bit to indicate the whole object must be drawn.
    GFX_GOL_METER_DRAW_STATE          
        /*DOM-IGNORE-BEGIN*/  = 0x4000 /*DOM-IGNORE-END*/,  
    // Draw bit to indicate the object must be removed from the screen.
    GFX_GOL_METER_HIDE_STATE
        /*DOM-IGNORE-BEGIN*/  = 0x8000 /*DOM-IGNORE-END*/,  
} GFX_GOL_METER_STATE;

// *****************************************************************************
/* 
    <GROUP gol_types>

    Typedef:
        GFX_GOL_METER_DRAW_TYPE

    Summary:
        Specifies the different types of Meter object.

    Description:
        This enumeration specifies the different typer of the Meter
        object used in the library.

        - GFX_GOL_METER_WHOLE_TYPE - The meter will be drawn using all the 
            arc colors (color1 to color6). 
        - GFX_GOL_METER_HALF_TYPE - The meter will be drawn using 
            arc colors (color5, color4, color3, color2).
        - GFX_GOL_METER_QUARTER_TYPE - The meter will be drawn using 
            arc colors (color3, color2).

    Remarks:
        None.
        
*/
// *****************************************************************************
typedef enum
{
    // draw circular meter
    GFX_GOL_METER_WHOLE_TYPE
        /*DOM-IGNORE-BEGIN*/  = 0 /*DOM-IGNORE-END*/,  
    // draw semi circle meter
    GFX_GOL_METER_HALF_TYPE 
        /*DOM-IGNORE-BEGIN*/  = 1 /*DOM-IGNORE-END*/,  
    // draw quarter of a circle meter
    GFX_GOL_METER_QUARTER_TYPE 
        /*DOM-IGNORE-BEGIN*/  = 2 /*DOM-IGNORE-END*/,  
} GFX_GOL_METER_DRAW_TYPE;

// *****************************************************************************
/*
    <GROUP gol_meter_object>

    Function:
        GFX_CONFIG_OBJECT_METER_DISPLAY_VALUES_DISABLE

    Summary:
        This compile time option macro disables displaying the
        values of the meter.

    Description:
        This is a Meter compile time option to disable the display
        of the values.  This disables the display of the values.
        Displaying the values will increase the rendering time
        of the object.

        The default setting is that values are displayed. To
        disable displaying the values, define this macro in
        the gfx_config.h file.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
//#define GFX_CONFIG_OBJECT_METER_DISPLAY_VALUES_DISABLE

#ifndef GFX_CONFIG_OBJECT_METER_SCALE_COUNT
// *****************************************************************************
/*
    <GROUP gol_meter_object>

    Function:
        GFX_CONFIG_OBJECT_METER_SCALE_COUNT

    Summary:
        This compile time option macro sets the number of characters
        allocated for the scale labels and the current value when
        displayed.

    Description:
        This is a Meter compile time option to define how many characters
        will be allocated for the scale labels and the current value
        when displayed.

        The value set is dependent on factors shown in the table below.
 	<TABLE>
    	minValue    maxValue    GFX_CONFIG_OBJECT_METER_DECIMAL_PLACES      GFX_CONFIG_OBJECT_METER_SCALE_COUNT
     	##########  ##########  ########################################    #####################################
        m           n           0                                           >= n (number of digits)
        m           n           > 0                                         >= n + 1 (number of digits)
        </TABLE>
        where: n, m - The number of digits of the minValue and maxValue
                      respectively.

        Based on the table above, the value set for the 
        GFX_CONFIG_OBJECT_METER_SCALE_COUNT should be always greater than
        or equal to the maximum number of characters that needs to be shown.
        When GFX_CONFIG_OBJECT_METER_DECIMAL_PLACES is not 0, the value
        must add one more character count for the decimal point.

        Example:
        (GFX_CONFIG_OBJECT_METER_DECIMAL_PLACES = 0)
        if maxValue-minValue = 10 - 500, GFX_CONFIG_OBJECT_METER_SCALE_COUNT should be 3.
        if maxValue-minValue = 0 - 90, GFX_CONFIG_OBJECT_METER_SCALE_COUNT = 2

        (GFX_CONFIG_OBJECT_METER_DECIMAL_PLACES > 0)
        if maxValue-minValue = 10 - 500, GFX_CONFIG_OBJECT_METER_SCALE_COUNT should be 4.
        if maxValue-minValue = 0 - 90, GFX_CONFIG_OBJECT_METER_SCALE_COUNT = 3

        Default value is 4. Define this macro in the gfx_config.h to change
        the value.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
    #define GFX_CONFIG_OBJECT_METER_SCALE_COUNT  4
#endif

#ifndef GFX_CONFIG_OBJECT_METER_DECIMAL_PLACES
// *****************************************************************************
/*
    <GROUP gol_meter_object>

    Function:
        GFX_CONFIG_OBJECT_METER_DECIMAL_PLACES

    Summary:
        This is a macro that sets the number of decimal places in the
        scale and the value displayed.

    Description:
        This is a macro that sets the number of decimal places in the
        scale and the value displayed. This macro also calculates the
        divide value needed to scale down the minValue, maxValue and
        current value being displayed.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
#define GFX_CONFIG_OBJECT_METER_DECIMAL_PLACES 0
#endif

// DOM-IGNORE-BEGIN

#ifndef GFX_CONFIG_OBJECT_METER_DEGREECOUNT
// *****************************************************************************
/*
    Function:
        GFX_CONFIG_OBJECT_METER_DEGREECOUNT

    Summary:
        This is an internal macro that sets degrees per scale when
        meter is rendered.

    Description:
        This is a Meter compile time option to define how many
        degrees per scale, computed per octant.
        Example: for 5 division per octant 45/5 = 9.
                 So every 9 degrees a scale is drawn
                 for a 5 scale division per octant.

        Default value is 9. Define this macro in the gfx_config.h to change
        the value.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
    #define GFX_CONFIG_OBJECT_METER_DEGREECOUNT 9
#endif

// *****************************************************************************
/*
    Function:
        GFX_CONFIG_OBJECT_METER_RESOLUTION

    Summary:
        This is an internal macro that sets the factor that scales
        down the minimum, maximum and current value of the meter.

    Description:
        This is a Meter compile time option to define the factor
        that the meter widget will divide minValue, maxValue
        and current value.

    Precondition:
        None.
 
    Parameters:
        None.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
#if (GFX_CONFIG_OBJECT_METER_DECIMAL_PLACES == 0)
    #define GFX_CONFIG_OBJECT_METER_RESOLUTION   1
#elif (GFX_CONFIG_OBJECT_METER_DECIMAL_PLACES == 1)
    #define GFX_CONFIG_OBJECT_METER_RESOLUTION   10
#elif (GFX_CONFIG_OBJECT_METER_DECIMAL_PLACES == 2)
    #define GFX_CONFIG_OBJECT_METER_RESOLUTION   100
#elif (GFX_CONFIG_OBJECT_METER_DECIMAL_PLACES == 3)
    #define GFX_CONFIG_OBJECT_METER_RESOLUTION   1000
#elif (GFX_CONFIG_OBJECT_METER_DECIMAL_PLACES == 4)
    #define GFX_CONFIG_OBJECT_METER_RESOLUTION   10000
#elif (GFX_CONFIG_OBJECT_METER_DECIMAL_PLACES == 5)
    #define GFX_CONFIG_OBJECT_METER_RESOLUTION   100000
#endif


// DOM-IGNORE-END

// *****************************************************************************
/* 
    <GROUP gol_types>

    Typedef:
        GFX_GOL_METER

    Summary:
        Defines the structure used for the Meter object.
        
    Description:
        Defines the parameters required for a Meter Object.
        Depending on the type selected the meter is drawn with 
        the defined shape parameters and values set on the 
        given fields.
        
    Remarks:
        None.
        
*/
// ***************************************************************************** 
typedef struct
{
    GFX_GOL_OBJ_HEADER  hdr;            // Generic header for all Objects (see OBJ_HEADER).
    GFX_XCHAR           *pText;         // The text label of the meter.
    GFX_GOL_METER_DRAW_TYPE  type;      // sets the type of the meter
    int16_t             value;          // Current value of the meter.
    int16_t             minValue;       // minimum value the meter can display
    int16_t             maxValue;       // maximum value the meter can display 
                                        // (range is maxValue - minValue)
    int16_t             xCenter;        // The x coordinate center position.
                                        // This is computed automatically.
    int16_t             yCenter;        // The y coordinate center position. 
                                        // This is computed automatically.
    int16_t             radius;         // Radius of the meter, also defines the 
                                        // needle length.

    // The following are computed automatically.
    int16_t             xPos;           // The current x position of the needle. 
                                        // This is computed automatically.
    int16_t             yPos;           // The current y position of the needle. 
                                        // This is computed automatically.
    GFX_COLOR           color1;         // Arc1 and scale1 color parameter.
    GFX_COLOR           color2;         // Arc2 and scale2 color parameter
    GFX_COLOR           color3;         // Arc3 and scale3 color parameter
    GFX_COLOR           color4;         // Arc4 and scale4 color parameter
    GFX_COLOR           color5;         // Arc5 and scale5 color parameter
    GFX_COLOR           color6;         // Arc6 and scale6 color parameter

    // The following pointers define fonts used in meter widget, they
    // can be different from the scheme font. Note that the sizes of these
    // fonts are not checked with the meter dimension. 
    // In cases where font sizes are larger than the meter dimension, 
    // some overlaps will occur.
    GFX_RESOURCE_HDR    *pTitleFont;    // Pointer to the font used in the 
                                        // title of the meter
    GFX_RESOURCE_HDR    *pValueFont;    // Pointer to the font used in the 
                                        // current reading (if displayed)
                                        // of the meter
} GFX_GOL_METER;

// *****************************************************************************
/*  
    <GROUP gol_meter_object>

    Function:
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

        After the meter is successfully created, GFX_GOL_MeterScaleColorsSet()
        should be called to set the colors of the meter. If this
        is not called, the colors used will be random.

    Precondition:
        None.

    Parameters:
        ID - Unique user defined ID for the object instance.
        left - Left most position of the object.
        top - Top most position of the object.
        right - Right most position of the object.
        bottom - Bottom most position of the object.
        state - Sets the initial state of the object.
        type - Specifies the type of Meter to be drawn (see GFX_GOL_METER_TYPE).
        value - Initial value set to the meter.
        minValue - The minimum value the meter will display.
        maxValue - The maximum value the meter will display.
        pTitleFont - Pointer to the font used for the Title.
        pValueFont - Pointer to the font used for the value.
        pText - Pointer to the text label of the meter.
        pScheme - Pointer to the style scheme used.

    Returns:
        Pointer to the newly created object.

    Example:
        <code>


            #define ID_METER 101

            extern const FONT_FLASH GOLMediumFont; 		// medium font
            extern const FONT_FLASH GOLSmallFont; 		// small font

            GFX_GOL_OBJ_SCHEME *pMeterScheme;
            GFX_GOL_METER *pMtr;

            // assume pMeterScheme is initialized to a scheme in memory.

            // draw object after creation
            state = GFX_GOL_METER_DRAW_STATE | GFX_GOL_METER_RING_STATE;

            pMtr = GFX_GOL_MeterCreate(
                        ID_METER,                   // assign ID
                        30, 50, 150, 180,           // set dimension
                        state,
                        GFX_GOL_METER_WHOLE_TYPE,   // type of meter
                        0,                          // set initial value
                        0, 100,                     // set min and max value
                        &GOLMediumFont,             // set title font
                        &GOLSmallFont,              // set value font
                        "Speed",                    // Text Label
                        pMeterScheme);              // style scheme

            // check if meter was created
            if (pMtr == NULL)
                return 0;

            // Change range colors: Normal values to WHITE
            //                      Critical values to BLUE
            //                      Danger values to RED
            // assume that WHITE, GREEN, YELLOW and RED have been defined.
            GFX_GOL_MeterScaleColorSet(pMtr, WHITE, WHITE, WHITE,
                                             GREEN, YELLOW, RED);

            // use GFX_GOL_ObjectListDraw() to draw the meter created
            while(!GFX_GOL_ObjectListDraw());
        
        </code>

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
                                GFX_GOL_OBJ_SCHEME      *pScheme);

// *****************************************************************************
/*  
    <GROUP gol_meter_object>

    Function:
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

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.
        value - the new value of the object.

    Returns:
        None.

    Example:
        <code>
            GFX_GOL_METER *pMeter;
            uint16_t ctr = 0;

            // create slider here and initialize parameters
            GFX_GOL_ObjectStateSet(pMeter, GFX_GOL_METER_DRAW_STATE);
            GFX_GOL_ObjectListDraw();

            while("some condition")
            {
                GFX_GOL_MeterValueSet(pMeter, ctr);
                GFX_GOL_ObjectStateSet( pMeter,
                                        GFX_GOL_METER_UPDATE_DRAW_STATE);

                // redraw the scroll bar
                GFX_GOL_ObjectListDraw();

               // update ctr here
               ctr = "some source of value";
            }
        </code>

*/
// *****************************************************************************
void GFX_GOL_MeterValueSet(GFX_GOL_METER *pObject, int16_t value);

// *****************************************************************************
/*  
    <GROUP gol_meter_object>

    Function:
        uint16_t GFX_GOL_MeterValueGet(
                                GFX_GOL_METER *pObject)

    Summary:
        This function returns the current value of the meter.

    Description:
        This function returns the current value of the meter.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.

    Returns:
        The current value of the object.

    Example:
        <code>
            #define MAXVALUE 100;

            GFX_GOL_METER *pMeter;
            uint32_t ctr = 0;

            // create scroll bar here and initialize parameters
            pMeter = GFX_GOL_MeterCreate(....)
            GFX_GOL_ObjectStateSet(pMeter, GFX_GOL_METER_DRAW_STATE);

            // draw the scroll bar
            GFX_GOL_ObjectListDraw();

            // a routine that updates the position of the thumb through some
            // conditions
            while("some condition")
            {
                GFX_GOL_MeterValueSet(pMeter, ctr);
                GFX_GOL_ObjectStateSet( pMeter,
                                        GFX_GOL_METER_UPDATE_DRAW_STATE);

                // update the screen
                GFX_GOL_ObjectListDraw();

                // update ctr here
                ctr = "some source of value";
            }

            if (GFX_GOL_MeterValueGet(pScrollBar) > MAXVALUE)
                return 0;
            else
                "do something else"
        </code>

*/
// *****************************************************************************
#define GFX_GOL_MeterValueGet(pMtr) (((GFX_GOL_METER*)pMtr)->value)

// *****************************************************************************
/*  
    <GROUP gol_meter_object>

    Function:
        void GFX_GOL_MeterIncrement(
                                GFX_GOL_METER *pObject,
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

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.

    Returns:
        None.

    Example:
        <code>

            void ControlSpeed(  GFX_GOL_METER* pObj,
                                int setSpeed,
                                int curSpeed)
            {
                // set page size to 1
                GFX_GOL_MeterValueSet(pObj, 1);

                if (setSpeed < curSpeed)
                {
                    while(GFX_GOL_MeterValueGet(pObj) < SetSpeed)
                        GFX_GOL_MeterIncrement(pObj, 1);  // increment by 1
                }
                else if (setSpeed > curSpeed)
                {
                    while(GFX_GOL_MeterValueGet(pObj) > SetSpeed)
                        GFX_GOL_MeterDecrement(pObj, 1);  // decrement by 1
                }
            }
        </code>

*/
// *****************************************************************************
void GFX_GOL_MeterIncrement(GFX_GOL_METER *pObject, uint16_t delta);

// *****************************************************************************
/*  
    <GROUP gol_meter_object>

    Function:
        void GFX_GOL_MeterDecrement(
                                GFX_GOL_METER *pObject,
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

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.

    Returns:
        None.

    Example:
        See GFX_GOL_MeterIncrement().

*/
// *****************************************************************************
void GFX_GOL_MeterDecrement(GFX_GOL_METER *pObject, uint16_t delta);

// *****************************************************************************
/*  
    <GROUP gol_meter_object>

    Function:
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
                                int16_t maxValue);

// *****************************************************************************
/*  
    <GROUP gol_meter_object>

    Function:
        void GFX_GOL_MeterMinimumValueGet(
                                GFX_GOL_METER *pObject)

    Summary:
        This function returns the minimum value set for the meter.

    Description:
        This function returns the minimum value set for the meter.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.

    Returns:
        The current minimum value set for the object.

    Example:
        None.

*/
// *****************************************************************************
#define GFX_GOL_MeterMinimumValueGet(pObject) (((GFX_GOL_METER *)pObject)->minValue)

// *****************************************************************************
/*  
    <GROUP gol_meter_object>

    Function:
        void GFX_GOL_MeterMaximumValueGet(
                                GFX_GOL_METER *pObject)

    Summary:
        This function returns the maximum value set for the meter.

    Description:
        This function returns the maximum value set for the meter.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.

    Returns:
        The current maximum value set for the object.

    Example:
        None.

*/
// *****************************************************************************
#define GFX_GOL_MeterMaximumValueGet(pObject) (((GFX_GOL_METER *)pObject)->maxValue)

// *****************************************************************************
/*  
    <GROUP gol_meter_object>

    Function:
        void GFX_GOL_MeterTitleFontSet(
                                GFX_GOL_METER *pObject,
                                GFX_RESOURCE_HDR *pNewFont)

    Summary:
        This function sets the meter title font used.

    Description:
        This function sets the meter title font used. Font pointer must
        be initialized to a valid GFX_RESOURCE_HDR.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.
        pNewFont - pointer to the selected font.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
#define GFX_GOL_MeterTitleFontSet(pObject, pNewFont) (((GFX_GOL_METER *)pObject)->pTitleFont = pNewFont)

// *****************************************************************************
/*  
    <GROUP gol_meter_object>

    Function:
        void GFX_GOL_MeterValueFontSet(
                                GFX_GOL_METER *pObject,
                                GFX_RESOURCE_HDR *pNewFont)

    Summary:
        This function sets the meter's displayed value font used.

    Description:
        This function sets the meter's displayed value font used.
        Font pointer must be initialized to a valid GFX_RESOURCE_HDR.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.
        pNewFont - pointer to the selected font.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
#define GFX_GOL_MeterValueFontSet(pObject, pNewFont) (((GFX_GOL_METER *)pObject)->pValueFont = pNewFont)

// *****************************************************************************
/*  
    <GROUP gol_meter_object>

    Function:
        void GFX_GOL_MeterTypeSet(
                                GFX_GOL_METER *pObject,
                                GFX_GOL_METER_DRAW_TYPE type)

    Summary:
        This function sets the meter draw type.

    Description:
        This function sets the meter draw type.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.
        type - the draw type selected (see GFX_GOL_METER_DRAW_TYPE for more
               information).

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
#define GFX_GOL_MeterTypeSet(pObject, type) (((GFX_GOL_METER *)pObject)->type = type)

// *****************************************************************************
/*  
    <GROUP gol_meter_object>

    Function:
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

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.
        color1 - color assigned to Arc 1 and Scale 1.
        color2 - color assigned to Arc 2 and Scale 2.
        color3 - color assigned to Arc 3 and Scale 3.
        color4 - color assigned to Arc 4 and Scale 4.
        color5 - color assigned to Arc 5 and Scale 5.
        color6 - color assigned to Arc 6 and Scale 6.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void GFX_GOL_MeterScaleColorsSet(
                                GFX_GOL_METER *pObject,
                                GFX_COLOR color1,
                                GFX_COLOR color2,
                                GFX_COLOR color3,
                                GFX_COLOR color4,
                                GFX_COLOR color5,
                                GFX_COLOR color6);

// *****************************************************************************
/*  
    <GROUP gol_meter_object>

    Function:
        GFX_GOL_TRANSLATED_ACTION GFX_GOL_MeterActionGet(
                                void *pObject, 
                                GFX_GOL_MESSAGE *pMessage)

    Summary:
        This function evaluates the message from a user if the
        message will affect the object or not.

    Description:
        This function evaluates the message from a user if the
        message will affect the object or not. The table below enumerates
        the action for each event of the touch screen and keyboard inputs.

	<TABLE>
    	Translated Message              Input Source    Set/Clear State Bit         Description
     	##################              ############    #######################     ##################################################################################################################################################################
        GFX_GOL_METER_ACTION_SET        System          EVENT_SET                   If event set occurs and the meter id is sent in parameter 1.
        GFX_GOL_OBJECT_ACTION_INVALID   Any             Any                         If the message did not affect the object.
        </TABLE>

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - The pointer to the object where the message will be
                   evaluated to check if the message will affect the object.
        pMessage - Pointer to the the message from the user interface.

    Returns:
        - GFX_GOL_METER_ACTION_SET  – Meter id is given in parameter 1 for
                                      a TYPE_SYSTEM message.
        - GFX_GOL_OBJECT_ACTION_INVALID – Object is not affected

    Example:
        None.

*/
// *****************************************************************************
GFX_GOL_TRANSLATED_ACTION GFX_GOL_MeterActionGet(
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage);

// *****************************************************************************
/*  
    <GROUP gol_meter_object>

    Function:
        void GFX_GOL_MeterActionSet(
                                GFX_GOL_TRANSLATED_ACTION  translatedMsg,
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
        The following state changes are supported:
        <TABLE>
        Translated Message          Input Source    Set/Clear State Bit             Description
        ##################          ############    ############################    #####################
        GFX_GOL_METER_ACTION_SET    System          Set GFX_GOL_METER_DRAW_STATE    Meter will be redrawn
                                                                                        to update the 
                                                                                        needle position 
                                                                                        and value displayed.
                                                                                        The new value is 
                                                                                        given in the pMessage
                                                                                        parameter 2 (param2).
                                                                                        While parameter 1 (param1)
                                                                                        of the message holds the 
                                                                                        ID of the object.
        </TABLE>

    Precondition:
        Object must exist in memory.

    Parameters:
        translatedMsg - The action of the object based on the message.
        pObject       - The pointer to the object whose state will be modified.
        pMessage      - The pointer to the original message.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void GFX_GOL_MeterActionSet(
                                GFX_GOL_TRANSLATED_ACTION  translatedMsg,
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage);

// *****************************************************************************
/*  
    <GROUP gol_meter_object>

    Function:
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
GFX_STATUS GFX_GOL_MeterDraw(void *pObject);


#endif // _GFX_GOL_METER_H
