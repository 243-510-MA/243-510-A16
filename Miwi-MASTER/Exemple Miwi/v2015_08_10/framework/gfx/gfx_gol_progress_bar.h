/*******************************************************************************
 Module for Microchip Graphics Library - Graphic Object Layer

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_gol_progress_bar.h

  Summary:
    This is the header file for the progress bar object of the GOL.

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

#ifndef _GFX_GOL_PROGRESSBAR_H
// DOM-IGNORE-BEGIN
    #define _GFX_GOL_PROGRESSBAR_H
// DOM-IGNORE-END

#include <stdint.h>
#include "gfx/gfx_gol.h"

// *****************************************************************************
/* 
    <GROUP gol_types>

    Typedef:
        GFX_GOL_PROGRESSBAR_STATE

    Summary:
        Specifies the different states of the Progress Bar object.

    Description:
        This enumeration specifies the different states of the Progress
        Bar object used in the library.

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
    // Property bit to indicate object is disabled.
    GFX_GOL_PROGRESSBAR_DISABLED_STATE    
        /*DOM-IGNORE-BEGIN*/  = 0x0002 /*DOM-IGNORE-END*/,  
    // Property bit for vertical orientation. 
    //    - When this state bit is 0 - object is rendered with horizontal orientation.
    //    - When this state bit is 1 - object is rendered with vertical orientation.
    GFX_GOL_PROGRESSBAR_VERTICAL_STATE   
        /*DOM-IGNORE-BEGIN*/  = 0x0004 /*DOM-IGNORE-END*/,  
    // Property bit that will suppress rendering of progress in text.
    GFX_GOL_PROGRESSBAR_NOPROGRESS_STATE 
        /*DOM-IGNORE-BEGIN*/  = 0x0008 /*DOM-IGNORE-END*/,  
    // Draw bit to indicate that the progress bar portion must be redrawn.
    GFX_GOL_PROGRESSBAR_DRAW_BAR_STATE   
        /*DOM-IGNORE-BEGIN*/  = 0x2000 /*DOM-IGNORE-END*/,  
    // Draw bit to indicate object must be redrawn.
    GFX_GOL_PROGRESSBAR_DRAW_STATE
        /*DOM-IGNORE-BEGIN*/  = 0x4000 /*DOM-IGNORE-END*/,  
    // Draw bit to indicate object must be removed from screen.
    GFX_GOL_PROGRESSBAR_HIDE_STATE     
        /*DOM-IGNORE-BEGIN*/  = 0x8000 /*DOM-IGNORE-END*/,  
} GFX_GOL_PROGRESSBAR_STATE;


// *****************************************************************************
/* 
    <GROUP gol_types>

    Typedef:
        GFX_GOL_PROGRESSBAR

    Summary:
        Defines the structure used for the Progress Bar object.
        
    Description:
        Defines the parameters required for a Progress Bar object.
        Object is drawn with the defined shape parameters and values 
        set on the given fields.
        
    Remarks:
        None.
        
*/
// *****************************************************************************
typedef struct
{
    GFX_GOL_OBJ_HEADER  hdr;        // Generic header for all Objects (see GFX_GOL_OBJ_HEADER).
    uint16_t            pos;        // Current progress position.
    uint16_t            prevPos;    // Previous progress position.
    uint16_t            range;      // Sets the range of the object.
} GFX_GOL_PROGRESSBAR;

// *****************************************************************************
/*  
    <GROUP gol_progressbar_object>
    
    Function:
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

    Precondition:
        None.

    Parameters:
        ID - Unique user defined ID for the object instance.
        left - Left most position of the object.
        top - Top most position of the object.
        right - Right most position of the object.
        bottom - Bottom most position of the object.
        state - Sets the initial state of the object.
        pos - Defines the initial position of the progress.
        range - This specifies the maximum value of the progress
                bar when the progress bar is at 100% position.
        pScheme - Pointer to the style scheme used.

    Returns:
        Pointer to the newly created object.

    Example:
        <code>

            GFX_GOL_PROGRESSBAR *pPBar;
            void CreateProgressBar()
            {
                pPBar = PbCreate(ID_PROGRESSBAR1,   // ID
                                50,90,270,140,      // dimension
                                GFX_GOL_PROGRESSBAR_DRAW_STATE, // Draw the object
                                25,                 // position
                                50,                 // set the range
                                NULL);              // use default GOL scheme
            
            }
        </code>

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
                                GFX_GOL_OBJ_SCHEME  *pScheme);

// *****************************************************************************
/*  
    <GROUP gol_progressbar_object>
    
    Function:
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

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.
        position - the new position of the scroll bar thumb.

    Returns:
        None.

    Example:
        <code>
                GFX_GOL_PROGRESSBAR *pPb;
                uint8_t  direction = 1;

		// this code increments and decrements the progress bar by 1
		// assume progress bar was created and initialized before
		while (1)
                {
                    if(direction)
                    {
                        if(pPb ->pos == pPb ->range)
                            direction = 0;
                        else
                            GFX_GOL_ProgressBarPositionSet(
                                pPb,
                                GFX_GOL_ProgressBarPositionGet(pPb)+1);
                    }
                    else
                    {
                        if(pPb ->pos == 0)
                            direction = 1;
                        else
                            GFX_GOL_ProgressBarPositionSet(
                                pPb,
                                GFX_GOL_ProgressBarPositionGet(pPb)-1);
                    }
                }
        </code>

*/
// *****************************************************************************
void GFX_GOL_ProgressBarPositionSet(GFX_GOL_PROGRESSBAR *pObject, uint16_t position);

// *****************************************************************************
/*  
    <GROUP gol_progressbar_object>
    
    Function:
        uint16_t GFX_GOL_ProgressBarPositionGet(
                                GFX_GOL_PROGRESSBAR *pObject)

    Summary:
        This function returns the current position of the progress bar.

    Description:
        This function returns the current position of the progress bar.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.

    Returns:
        The current position of the scroll bar thumb.

    Example:
        See GFX_GOL_ProgressBarPositionSet() example.

*/
// *****************************************************************************
#define GFX_GOL_ProgressBarPositionGet(pObject) (((GFX_GOL_PROGRESSBAR*)pObject)->pos)

// *****************************************************************************
/*  
    <GROUP gol_progressbar_object>
    
    Function:
        GFX_GOL_TRANSLATED_ACTION GFX_GOL_ProgressBarActionGet(
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage);

    Summary:
        This function evaluates the message from a user if the
        message will affect the object or not.

    Description:
        This function evaluates the message from a user if the
        message will affect the object or not. The table below enumerates
        the action for each event of the touch screen and keyboard inputs.

	<TABLE>
    	Translated Message                      Input Source    Set/Clear State Bit         Description
     	##################                      ############    #######################     ######################################################################
        GFX_GOL_PROGRESSBAR_ACTION_SELECTED     Touch Screen    EVENT_PRESS, EVENT_RELEASE, If events occurs and the x,y position falls in the area of the object.
                                                                    EVENT_MOVE
        GFX_GOL_OBJECT_ACTION_INVALID           Any             Any                         If the message did not affect the object.
	</TABLE>

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - The pointer to the object where the message will be
                   evaluated to check if the message will affect the object.
        pMessage - Pointer to the the message from the user interface.

    Returns:
        - GFX_GOL_PROGRESSBAR_ACTION_SELECTED – Object is selected
        - GFX_GOL_OBJECT_ACTION_INVALID – Object is not affected

    Example:
        None.

*/
// *****************************************************************************
GFX_GOL_TRANSLATED_ACTION GFX_GOL_ProgressBarActionGet(
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage);

// *****************************************************************************
/*  
    <GROUP gol_progressbar_object>
    
    Function:
        void GFX_GOL_ProgressBarRangeSet(
                                GFX_GOL_PROGRESSBAR *pObject,
                                uint16_t range)

    Summary:
        This function sets the range of the progress bar.

    Description:
        This function sets the range of the progress bar.
        When the range is modified, object must be completely redrawn to
        reflect the change.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.
        range  - new range of the object.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void GFX_GOL_ProgressBarRangeSet(
                                GFX_GOL_PROGRESSBAR *pObject,
                                uint16_t range);

// *****************************************************************************
/*  
    <GROUP gol_progressbar_object>
    
    Function:
        void GFX_GOL_ProgressBarRangeGet(
                                GFX_GOL_PROGRESSBAR *pObject)

    Summary:
        This function returns the range of the progress bar.

    Description:
        This function returns the range of the progress bar.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.

    Returns:
        The current range used by the object.

    Example:
        None.

*/
// *****************************************************************************
#define GFX_GOL_ProgressBarRangeGet(pObject) (pObject->range)

// *****************************************************************************
/*  
    <GROUP gol_progressbar_object>
    
    Function:
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
GFX_STATUS GFX_GOL_ProgressBarDraw(void *pObject);

#endif // _GFX_GOL_PROGRESSBAR_H
