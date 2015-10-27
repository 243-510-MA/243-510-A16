/*******************************************************************************
 Module for Microchip Graphics Library - Graphic Object Layer

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_gol_scroll_bar.h

  Summary:
    This is the header file for the scroll bar object of the GOL.

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

#ifndef _SCROLLBAR_H
// DOM-IGNORE-BEGIN
    #define _SCROLLBAR_H
// DOM-IGNORE-END

#include <stdint.h>
#include "gfx/gfx_gol.h"

// *****************************************************************************
/* 
    <GROUP gol_types>

    Typedef:
        GFX_GOL_SCROLLBAR_STATE

    Summary:
        Specifies the different states of the Scroll Bar object.

    Description:
        This enumeration specifies the different states of the Scroll Bar
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
    // Property bit for focus state.
    GFX_GOL_SCROLLBAR_FOCUSED_STATE
        /*DOM-IGNORE-BEGIN*/  = 0x0001 /*DOM-IGNORE-END*/,
    // Property bit for disabled state.
    GFX_GOL_SCROLLBAR_DISABLED_STATE
        /*DOM-IGNORE-BEGIN*/  = 0x0002 /*DOM-IGNORE-END*/,
    // Property bit to indicate the scroll bar is drawn with 
    // vertical orientation
    GFX_GOL_SCROLLBAR_VERTICAL_STATE
        /*DOM-IGNORE-BEGIN*/  = 0x0004 /*DOM-IGNORE-END*/,
    // Property bit to indicate the scroll bar is in slider mode.
    GFX_GOL_SCROLLBAR_SLIDER_MODE_STATE
        /*DOM-IGNORE-BEGIN*/  = 0x0010 /*DOM-IGNORE-END*/,
    // Draw bit to indicate that only the thumb area will be redrawn.
    GFX_GOL_SCROLLBAR_DRAW_THUMB_STATE  
        /*DOM-IGNORE-BEGIN*/  = 0x1000 /*DOM-IGNORE-END*/,
    // Draw bit to indicate focus must be redrawn.
    GFX_GOL_SCROLLBAR_DRAW_FOCUS_STATE
        /*DOM-IGNORE-BEGIN*/  = 0x2000 /*DOM-IGNORE-END*/,
    // Draw bit to indicate object must be redrawn.
    GFX_GOL_SCROLLBAR_DRAW_STATE        
        /*DOM-IGNORE-BEGIN*/  = 0x4000 /*DOM-IGNORE-END*/,
    // Draw bit to indicate object must be removed from screen.
    GFX_GOL_SCROLLBAR_HIDE_STATE       
        /*DOM-IGNORE-BEGIN*/  = 0x8000 /*DOM-IGNORE-END*/,
} GFX_GOL_SCROLLBAR_STATE;

// *****************************************************************************
/* 
    <GROUP gol_types>

    Typedef:
        GFX_GOL_SCROLLBAR

    Summary:
        Defines the structure used for the Scroll Bar object.
        
    Description:
        Defines the parameters required for a Scroll Bar object.
        Object is drawn with the defined shape parameters and values 
        set on the given fields. Depending on the 
        GFX_GOL_SCROLLBAR_SLIDER_MODE_STATE state bit slider or scrollbar 
        mode is set. If GFX_GOL_SCROLLBAR_SLIDER_MODE_STATE is set, mode 
        is slider; if not set mode is scroll bar. For scrollbar mode, focus 
        rectangle is not drawn.
        
    Remarks:
        None.
        
*/
// *****************************************************************************
typedef struct
{
    GFX_GOL_OBJ_HEADER  hdr;        // Generic header for all Objects (see GFX_GOL_OBJ_HEADER).
    int16_t             currPos;    // Position of the slider relative to minimum.
    uint16_t            prevPos;    // Previous position of the slider relative to minimum.
    uint16_t            range;      // User defined range of the slider. Minimum value at 0 and maximum at 0x7FFF.
    int16_t             pos;        // Position of the slider in range domain.
    uint16_t            page;       // User specified resolution to incrementally change the position
                                    // in range domain.
    uint16_t            thWidth;    // Thumb width. This is computed internally.
                                    // User must not change this value.
    uint16_t            thHeight;   // Thumb width. This is computed internally.
                                    // User must not change this value.
} GFX_GOL_SCROLLBAR;


// *****************************************************************************
/*  
    <GROUP gol_scrollbar_object>
    
    Function:
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
                                GFX_GOL_OBJ_SCHEME  *pScheme);

    Summary:
        This function creates a GFX_GOL_SCROLLBAR object with the parameters
        given. It automatically attaches the new object into a global
        linked list of objects and returns the address of the object.


    Description:
        This function creates a GFX_GOL_SCROLLBAR object with the parameters
        given. It automatically attaches the new object into a global
        linked list of objects and returns the address of the object.

        This function returns the pointer to the newly created object.
        If the object is not successfully created, it returns NULL.

        The object can be configured as a scroll bar or a slider. Use the
        state bit GFX_GOL_SCROLLBAR_SLIDER_MODE_STATE to enable the usage
        of the object as a slider. If this state bit is not enabled,
        the object is set up as a scroll bar.

        The object can also be configured with vertical orientation.
        Use the state bit GFX_GOL_SCROLLBAR_VERTICAL_STATE to set up the
        object with vertical orientation. If this state bit is not set,
        the object is used with horizontal orientation.

        The behavior of GFX_GOL_ScrollBarCreate() will be undefined if one
        of the following is true:
        - left >= right
        - top >= bottom
        - pScheme is not pointing to a GFX_GOL_OBJ_SCHEME
        - page is set to zero.
        - range is set to zero.
        - page > range.

    Precondition:
        None.

    Parameters:
        ID - Unique user defined ID for the object instance.
        left - Left most position of the object.
        top - Top most position of the object.
        right - Right most position of the object.
        bottom - Bottom most position of the object.
        state - Sets the initial state of the object.
        range - This specifies the maximum value of the slider when the
                thumb is on the rightmost position for a horizontal orientation
                and bottom most position for the vertical orientation. Minimum
                value is always at zero.
        page - This is the incremental change of the slider when user action
               requests to move the slider thumb. This value is added or
               subtracted to the current position of the thumb.
        pos - This defines the initial position of the thumb.
        pScheme - The pointer to the style scheme used for the Object.
                  Set to NULL if default style scheme is used.

    Returns:
        Pointer to the newly created object.

    Example:
        <code>

            // assume pScheme is initialized

            GFX_GOL_OBJ_SCHEME *pScheme;
            GFX_GOL_SCROLLBAR  *srollBarArray[3];
            uint16_t           state;

            // create a slider with
            // range = [0 - 50000]
            // delta = 500
            // initial position = 25000

            state = GFX_GOL_SCROLLBAR_DRAW_STATE;
            srollBarArray[0] = GFX_GOL_ScrollBarCreate(  5,
                                    150, 145, 285, 181,
                                    state,
                                    50000, 500, 25000,
                                    pScheme);
            if (slider[0] == NULL)
                return 0;

           // create a slider with
           // range = [0 - 100]
           // delta = 20
           // initial position = 0

           state =  GFX_GOL_SCROLLBAR_DRAW_STATE |
                    GFX_GOL_SCROLLBAR_SLIDER_MODE_STATE;
           srollBarArray[1] = GFX_GOL_ScrollBarCreate(  6,
                                   150, 190, 285, 220,
                                   state,
                                   100, 20, 0,
                                   pScheme);
           if (slider[1] == NULL)
                return 0;

           // create a vertical scroll bars with
           // range = [0 - 30]
           // delta = 2
           // initial position = 20

           state =  GFX_GOL_SCROLLBAR_DRAW_STATE |
                    GFX_GOL_SCROLLBAR_VERTICAL_STATE;
           srollBarArray[2] = GFX_GOL_ScrollBarCreate(  7,
                                   120, 145, 140, 220,
                                   state,
                                   30, 2, 20,
                                   pScheme);
           if (slider[2] == NULL)
               return 0;

           // draw the sliders
           while (GFX_GOL_ObjectListDraw() == 0);

           return 1;

        </code>

*/
// *****************************************************************************
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
                                GFX_GOL_OBJ_SCHEME  *pScheme);

// *****************************************************************************
/*  
    <GROUP gol_scrollbar_object>
    
    Function:
        void GFX_GOL_ScrollBarRangeSet(
                                GFX_GOL_SCROLLBAR *pObject,
                                uint16_t range)

    Summary:
        This function sets the range of the thumb of the scroll bar.

    Description:
        This function sets the range of the thumb of the scroll bar.
        When the range is modified, object must be completely redrawn to
        reflect the change.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.
        range  - new range of the scroll bar.

    Returns:
        None.

    Example:
        <code>
            GFX_GOL_SCROLLBAR *pSld;

            GFX_GOL_ScrollBarRangeSet(pSld, 100);

            // to completely redraw the object when
            // GFX_GOL_ObjectListDraw() is executed.
            GFX_GOL_ObjectStateSet(pSld, GFX_GOL_SCROLLBAR_DRAW_STATE);
        </code>

*/
// *****************************************************************************
void GFX_GOL_ScrollBarRangeSet(GFX_GOL_SCROLLBAR *pObject, uint16_t range);

// *****************************************************************************
/*  
    <GROUP gol_scrollbar_object>
    
    Function:
        uint16_t GFX_GOL_ScrollBarRangeGet(
                                GFX_GOL_SCROLLBAR *pObject)

    Summary:
        This function returns the range of the thumb of the scroll bar.

    Description:
        This function returns the range of the thumb of the scroll bar.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.

    Returns:
        The range of the scroll bar.

    Example:
        <code>
            GFX_GOL_SCROLLBAR *pSld;
            uint16_t range;

            range = GFX_GOL_ScrollBarRangeGet(pSld);
        </code>

*/
// *****************************************************************************
uint16_t GFX_GOL_ScrollBarRangeGet(
                                GFX_GOL_SCROLLBAR *pObject);

// *****************************************************************************
/*  
    <GROUP gol_scrollbar_object>
    
    Function:
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

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.
        page - value of the page size of the object.

    Returns:
        None.

    Example:
        See GFX_GOL_ScrollBarPositionIncrement() for code example.

*/
// *****************************************************************************
void GFX_GOL_ScrollBarPageSet(GFX_GOL_SCROLLBAR *pObject, uint16_t page);

// *****************************************************************************
/*  
    <GROUP gol_scrollbar_object>
    
    Function:
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

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.

    Returns:
        The page size of the object.

    Example:
        <code>

            uint16_t page;
            GFX_GOL_SCROLLBAR *pScrollBar;

            // assume pScrollBar is initialized to a scroll bar in memory
             page = GFX_GOL_ScrollBarPageGet(pScrollBar);
        </code>

*/
// *****************************************************************************
uint16_t GFX_GOL_ScrollBarPageGet(GFX_GOL_SCROLLBAR *pObject);

// *****************************************************************************
/*  
    <GROUP gol_scrollbar_object>
    
    Function:
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

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.
        position - the new position of the scroll bar thumb.

    Returns:
        None.

    Example:
        <code>
            GFX_GOL_SCROLLBAR *pScrollBar;
            uint16_t ctr = 0;

            // create slider here and initialize parameters
            GFX_GOL_ObjectStateSet(pScrollBar, GFX_GOL_SCROLLBAR_DRAW_STATE);
            GFX_GOL_ObjectListDraw();

            while("some condition")
            {
                GFX_GOL_ScrollBarPositionSet(pScrollBar, ctr);
                GFX_GOL_ObjectStateSet( pScrollBar,
                                        GFX_GOL_SCROLLBAR_DRAW_THUMB_STATE);

                // redraw the scroll bar
                GFX_GOL_ObjectListDraw();

               // update ctr here
               ctr = "some source of value";
            }
        </code>

*/
// *****************************************************************************
void GFX_GOL_ScrollBarPositionSet(GFX_GOL_SCROLLBAR *pObject, uint16_t position);

// *****************************************************************************
/*  
    <GROUP gol_scrollbar_object>
    
    Function:
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

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.

    Returns:
        The current position of the scroll bar thumb.

    Example:
        <code>
            #define MAXVALUE 100;

            GFX_GOL_SCROLLBAR *pScrollBar;
            uint32_t ctr = 0;

            // create scroll bar here and initialize parameters
            pScrollBar = GFX_GOL_ScrollBarCreate(....)
            GFX_GOL_ObjectStateSet(pScrollBar, GFX_GOL_SCROLLBAR_DRAW_STATE);

            // draw the scroll bar
            GFX_GOL_ObjectListDraw();

            // a routine that updates the position of the thumb through some
            // conditions
            while("some condition")
            {
                GFX_GOL_ScrollBarPositionSet(pScrollBar, ctr);
                GFX_GOL_ObjectStateSet( pScrollBar,
                                        GFX_GOL_SCROLLBAR_DRAW_THUMB_STATE);

                // update the screen
                GFX_GOL_ObjectListDraw();

                // update ctr here
                ctr = "some source of value";
            }

            if (GFX_GOL_ScrollBarPositionGet(pScrollBar) > MAXVALUE)
                return 0;
            else
                "do something else"
        </code>

*/
// *****************************************************************************
uint16_t GFX_GOL_ScrollBarPositionGet(GFX_GOL_SCROLLBAR *pObject);

// *****************************************************************************
/*  
    <GROUP gol_scrollbar_object>
    
    Function:
        void GFX_GOL_ScrollBarPositionIncrement(
                                GFX_GOL_SCROLLBAR *pObject)

    Summary:
        This function increments the scroll bar position by the delta
        change (page) value set.

    Description:
        This function increments the scroll bar position by the delta
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
void GFX_GOL_ScrollBarPositionIncrement(GFX_GOL_SCROLLBAR *pObject);

// *****************************************************************************
/*  
    <GROUP gol_scrollbar_object>
    
    Function:
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
                    {
                        // increment by 1
                        GFX_GOL_ScrollBarPositionIncrement(pObj);  
                    }

                }
                else if (setSpeed > curSpeed)
                {
                    while(GFX_GOL_ScrollBarPositionGet(pObj) > SetSpeed)
                    {
                        // decrement by 1
                        GFX_GOL_ScrollBarPositionDecrement(pObj);  
                    }
                }
            }
        </code>

*/
// *****************************************************************************
void GFX_GOL_ScrollBarPositionDecrement(GFX_GOL_SCROLLBAR *pObject);

// *****************************************************************************
/*  
    <GROUP gol_scrollbar_object>
    
    Function:
        GFX_GOL_TRANSLATED_ACTION GFX_GOL_ScrollBarActionGet(
                                void *pObject, GFX_GOL_MESSAGE *pMessage)

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
        GFX_GOL_SCROLLBAR_ACTION_INC    Touch Screen    EVENT_PRESS, EVENT_MOVE     If events occurs and the x,y position falls in the area of the slider and the slider position is to the LEFT of the x,y position for a horizontal slider or BELOW the x,y position for a vertical slider.
                                        Keyboard        EVENT_KEYSCAN               If event occurs and parameter1 passed matches the object�s ID and parameter 2 passed matches SCAN_UP_PRESSED or SCAN_LEFT_PRESSED.
        GFX_GOL_SCROLLBAR_ACTION_DEC    Touch Screen    EVENT_PRESS, EVENT_MOVE     If events occurs and the x,y position falls in the area of the slider and the slider position is to the RIGHT of the x,y position for a horizontal slider or ABOVE the x,y position for a vertical slider.
                                        Keyboard        EVENT_KEYSCAN               If event occurs and parameter1 passed matches the object�s ID and parameter 2 passed matches SCAN_DOWN_PRESSED or SCAN_RIGHT_PRESSED.
        GFX_GOL_OBJECT_ACTION_PASSIVE   Touch Screen    EVENT_RELEASE               If events occurs and the x,y position falls in the area of the slider.
        GFX_GOL_OBJECT_ACTION_INVALID   Any             Any                         If the message did not affect the object.
	</TABLE>

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - The pointer to the object where the message will be
                   evaluated to check if the message will affect the object.
        pMessage - Pointer to the the message from the user interface.

    Returns:
        - GFX_GOL_SCROLLBAR_ACTION_INC  – Increment scroll bar position.
        - GFX_GOL_SCROLLBAR_ACTION_DEC  – Decrement scroll bar position.
        - GFX_GOL_OBJECT_ACTION_PASSIVE – Object bar is not affected
        - GFX_GOL_OBJECT_ACTION_INVALID – Object is not affected

    Example:
        None.

*/
// *****************************************************************************
GFX_GOL_TRANSLATED_ACTION GFX_GOL_ScrollBarActionGet(
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage);

// *****************************************************************************
/*  
    <GROUP gol_scrollbar_object>
    
    Function:
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
        using the application defined GFX_GOL_MESSAGE_CALLBACK_FUNC.
        When the user message is determined to affect the object, application
        can perform the state change in the message callback function.
        The following state changes are supported:
        <TABLE>
        Translated Message              Input Source        Set/Clear State Bit                     Description
        ##################              ############        ###################                     ###########
        GFX_GOL_SCROLLBAR_ACTION_INC    Touch Screen,       Set GFX_GOL_SCROLLBAR_DRAW_THUMB_STATE  Scroll Bar will be redrawn with thumb in the incremented position.
                                        Keyboard
        GFX_GOL_SCROLLBAR_ACTION_DEC    Touch Screen,       Set GFX_GOL_SCROLLBAR_DRAW_THUMB_STATE  Scroll Bar will be redrawn with thumb in the decremented position.
                                        Keyboard
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
void GFX_GOL_ScrollBarActionSet(
                                GFX_GOL_TRANSLATED_ACTION  translatedMsg,
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage);

// *****************************************************************************
/*  
    <GROUP gol_scrollbar_object>
    
    Function:
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
GFX_STATUS GFX_GOL_ScrollBarDraw(void *pObject);

#endif //_SCROLLBAR_H
