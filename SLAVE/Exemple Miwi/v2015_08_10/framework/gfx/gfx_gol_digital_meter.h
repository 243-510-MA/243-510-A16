/*******************************************************************************
 Module for Microchip Graphics Library - Graphic Object Layer

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_gol_digital_meter.h

  Summary:
    This is the header file for the digital meter object of the GOL.

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

#ifndef _GFX_GOL_DIGITALMETER_H
// DOM-IGNORE-BEGIN
    #define _GFX_GOL_DIGITALMETER_H
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include <stdint.h>
#include <gfx/gfx_gol.h>

// *****************************************************************************
/* 
    <GROUP gol_types>

    Typedef:
        GFX_GOL_DIGITALMETER_STATE

    Summary:
        Specifies the different states of the Digital Meter object.

    Description:
        This enumeration specifies the different states of the Digital Meter
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
    GFX_GOL_DIGITALMETER_DISABLED_STATE    
        /*DOM-IGNORE-BEGIN*/  = 0x0002 /*DOM-IGNORE-END*/,  
    // Property bit to indicate frame is displayed.
    GFX_GOL_DIGITALMETER_FRAME_STATE 
        /*DOM-IGNORE-BEGIN*/  = 0x0010 /*DOM-IGNORE-END*/,  
    // Draw bit to indicate object must be redrawn.
    GFX_GOL_DIGITALMETER_DRAW_STATE
        /*DOM-IGNORE-BEGIN*/  = 0x4000 /*DOM-IGNORE-END*/,  
    // Draw bit to indicate that only text must be redrawn.
    GFX_GOL_DIGITALMETER_UPDATE_STATE
        /*DOM-IGNORE-BEGIN*/  = 0x2000 /*DOM-IGNORE-END*/,  
    // Draw bit to indicate the object must be removed from the screen.
    GFX_GOL_DIGITALMETER_HIDE_STATE 
        /*DOM-IGNORE-BEGIN*/  = 0x8000 /*DOM-IGNORE-END*/,  
} GFX_GOL_DIGITALMETER_STATE;

// DOM-IGNORE-BEGIN

#ifndef GFX_CONFIG_OBJECT_DIGITALMETER_INDENT_COUNT
/***************************************************************************
* Overview: This is a DigitalMeter compile time option to define
*            the indent constant for the text used in the frame.
***************************************************************************/
    #define GFX_GOL_DIGITALMETER_INDENT   0x02
#endif

#ifndef GFX_CONFIG_OBJECT_DIGITALMETER_WIDTH
/***************************************************************************
* Overview: This is a DigitalMeter compile time option to define
*           the user should change this value depending on the number
*           of digits he wants to display.
***************************************************************************/
    #define GFX_GOL_DIGITALMETER_WIDTH    16
#endif

// DOM-IGNORE-END

// *****************************************************************************
/* 
    <GROUP gol_types>

    Typedef:
        GFX_GOL_DIGITALMETER

    Summary:
        Defines the structure used for the Digital Meter object.
        
    Description:
        Defines the parameters required for a Digital Meter Object.
        Depending on the type selected the meter is drawn with 
        the defined shape parameters and values set on the 
        given fields.
        
    Remarks:
        None.
        
*/
// ***************************************************************************** 
typedef struct {
    GFX_GOL_OBJ_HEADER   hdr;           // Generic header for all Objects (see GFX_OBJ_HEADER).
    uint16_t             textHeight;    // Pre-computed text height
    uint32_t             Cvalue;        // Current value
    uint32_t             Pvalue;        // Previous value
    uint8_t              NoOfDigits;    // Number of digits to be displayed
    uint8_t              DotPos;        // Position of decimal point
    GFX_ALIGNMENT        alignment;     // text alignment
} GFX_GOL_DIGITALMETER;


// *****************************************************************************
/*  
    <GROUP gol_digitalmeter_object>
    
    Function:
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

     Precondition:
        None.

    Parameters:
        instance - Device instance
        ID - Unique user defined ID for the object instance.
        left - Left most position of the object.
        top - Top most position of the object.
        right - Right most position of the object.
        bottom - Bottom most position of the object.
        state - Sets the initial state of the object.
        value - Sets the initial value to be displayed
        NoOfDigits - Sets the number of digits to be displayed
        DotPos - Sets the position of decimal point in the display
        alignment - text alignment of the text used in the object.
        pScheme - Pointer to the style scheme. Set to NULL if
               default style scheme is used.

    Returns:
        Pointer to the newly created object.

    Example:
        <code>
            GFX_GOL_OBJ_SCHEME *pScheme;
            GFX_GOL_DIGITALMETER *pDm;

            pScheme = GFX_GOL_SchemeCreate();
            state = GFX_GOL_DIGITALMETER_DRAW_STATE |
                    GFX_GOL_DIGITALMETER_FRAME_STATE;
            GFX_GOL_DigitalMeterCreate(
                    ID_DIGITALMETER1,   // ID
                    30,80,235,160,      // dimension
                    state,              // has frame and center aligned
                    789,4,1,            // to display 078.9
                    GFX_ALIGN_CENTER,
                    pScheme);           // use given scheme

            // draw the objects
            while(GFX_GOL_ObjectListDraw() != GFX_STATUS_SUCCESS);
        </code>

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
                                GFX_GOL_OBJ_SCHEME  *pScheme);

// *****************************************************************************
/*  
    <GROUP gol_digitalmeter_object>
    
    Function:
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

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.
        value - the new value of the object.

    Returns:
        None.

    Example:
        <code>
            GFX_GOL_DIGITALMETER *pMeter;
            uint16_t ctr = 0;

            // create slider here and initialize parameters
            GFX_GOL_ObjectStateSet(pMeter, GFX_GOL_DIGITALMETER_DRAW_STATE);
            GFX_GOL_ObjectListDraw();

            while("some condition")
            {
                GFX_GOL_DigitalMeterValueSet(pMeter, ctr);
                GFX_GOL_ObjectStateSet( pMeter,
                                        GFX_GOL_DIGITALMETER_UPDATE_STATE);

                // redraw the scroll bar
                GFX_GOL_ObjectListDraw();

               // update ctr here
               ctr = "some source of value";
            }
        </code>

*/
// *****************************************************************************
void GFX_GOL_DigitalMeterValueSet(
                                GFX_GOL_DIGITALMETER *pObject,
                                int16_t value);

// *****************************************************************************
/*  
    <GROUP gol_digitalmeter_object>
    
    Function:
        uint16_t GFX_GOL_DigitalMeterValueGet(
                                GFX_GOL_DIGITALMETER *pObject)

    Summary:
        This function returns the current value of the digital meter.

    Description:
        This function returns the current value of the digital meter.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.

    Returns:
        The current value of the object.

    Example:
        <code>
            #define MAXVALUE 100;

            GFX_GOL_DIGITALMETER *pMeter;
            uint32_t ctr = 0;

            // create scroll bar here and initialize parameters
            pMeter = GFX_GOL_DigitalMeterCreate(....)
            GFX_GOL_ObjectStateSet(pMeter, GFX_GOL_DIGITALMETER_DRAW_STATE);

            // draw the scroll bar
            GFX_GOL_ObjectListDraw();

            // a routine that updates the position of the thumb through some
            // conditions
            while("some condition")
            {
                GFX_GOL_DigitalMeterValueSet(pMeter, ctr);
                GFX_GOL_ObjectStateSet( pMeter,
                                        GFX_GOL_DIGITALMETER_UPDATE_STATE);

                // update the screen
                GFX_GOL_ObjectListDraw();

                // update ctr here
                ctr = "some source of value";
            }

            if (GFX_GOL_DigitalMeterValueGet(pScrollBar) > MAXVALUE)
                return 0;
            else
                "do something else"
        </code>

*/
// *****************************************************************************
#define GFX_GOL_DigitalMeterValueGet(pObject) \
                                (((GFX_GOL_DIGITALMETER*)pObject)->value)

// *****************************************************************************
/*  
    <GROUP gol_digitalmeter_object>
    
    Function:
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

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.

    Returns:
        None.

    Example:
        <code>

            void ControlSpeed(  GFX_GOL_DIGITALMETER* pObj,
                                int setSpeed,
                                int curSpeed)
            {
                // set page size to 1
                GFX_GOL_DigitalMeterValueSet(pObj, 1);

                if (setSpeed < curSpeed)
                {
                    while(GFX_GOL_DigitalMeterValueGet(pObj) < SetSpeed)
                    {
                        // increment by 1
                        GFX_GOL_DigitalMeterIncrement(pObj, 1);  
                    }
                }
                else if (setSpeed > curSpeed)
                {
                    while(GFX_GOL_DigitalMeterValueGet(pObj) > SetSpeed)
                    {
                        // decrement by 1
                        GFX_GOL_DigitalMeterDecrement(pObj, 1);
                    }
                }
            }
        </code>

*/
// *****************************************************************************
void GFX_GOL_DigitalMeterIncrement(
                                GFX_GOL_DIGITALMETER *pObject,
                                uint16_t delta);

// *****************************************************************************
/* 
    <GROUP gol_digitalmeter_object>
    
    Function:
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

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.

    Returns:
        None.

    Example:
        See GFX_GOL_DigitalMeterIncrement().

*/
// *****************************************************************************
void GFX_GOL_DigitalMeterDecrement(
                                GFX_GOL_DIGITALMETER *pObject,
                                uint16_t delta);

// *****************************************************************************
/*  
    <GROUP gol_digitalmeter_object>
    
    Function:
        GFX_ALIGNMENT GFX_GOL_DigitalMeterTextAlignmentGet(
                                GFX_GOL_DIGITALMETER *pObject)

    Summary:
        This function returns the text alignment of the text
        string used by the object.

    Description:
        This function returns the text alignment of the text
        string used by the object.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.

    Returns:
        The text alignment set in the object. See GFX_ALIGNMENT for more
        details.

    Example:
        None.

*/
// *****************************************************************************
#define GFX_GOL_DigitalMeterTextAlignmentGet(pObject)                \
                (((GFX_GOL_DIGITALMETER *)pObject)->alignment)

// *****************************************************************************
/*  
    <GROUP gol_digitalmeter_object>
    
    Function:
        void GFX_GOL_DigitalMeterTextAlignmentSet(
                                GFX_GOL_DIGITALMETER *pObject,
                                GFX_ALIGNMENT align)

    Summary:
        This function sets the text alignment of the text
        string used by the object.

    Description:
        This function sets the text alignment of the text
        string used by the object.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - Pointer to the object.
        align - The alignment set for the text in the object.
                See GFX_ALIGNMENT for more details.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
#define GFX_GOL_DigitalMeterTextAlignmentSet(pObject, align)         \
                (((GFX_GOL_DIGITALMETER *)pObject)->alignment = align)

// *****************************************************************************
/*  
    <GROUP gol_digitalmeter_object>
    
    Function:
        GFX_GOL_TRANSLATED_ACTION GFX_GOL_DigitalMeterActionGet(
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
    	Translated Message                      Input Source    Set/Clear State Bit         Description
     	##################                      ############    #######################     #####################################################################################
        GFX_GOL_DIGITALMETER_ACTION_SELECTED    Touch Screen    EVENT_PRESS, EVENT_RELEASE  If events occurs and the x,y position falls in the area of the <link Digital Meter>.
        GFX_GOL_OBJECT_ACTION_INVALID           Any             Any                         If the message did not affect the object.
         </TABLE>

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - The pointer to the object where the message will be
                   evaluated to check if the message will affect the object.
        pMessage - Pointer to the the message from the user interface.

    Returns:
        - GFX_GOL_DIGITALMETER_ACTION_SELECTED - Object is selected.
        - GFX_GOL_OBJECT_ACTION_INVALID - Object is not affected

    Example:
        None.

*/
// *****************************************************************************
GFX_GOL_TRANSLATED_ACTION GFX_GOL_DigitalMeterActionGet(
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage);

// *****************************************************************************
/*  
    <GROUP gol_digitalmeter_object>
    
    Function:
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
GFX_STATUS GFX_GOL_DigitalMeterDraw(void *pObject);

#endif // _GFX_GOL_DIGITALMETER_H
