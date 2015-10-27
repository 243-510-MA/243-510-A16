/*******************************************************************************
 Module for Microchip Graphics Library - Graphic Object Layer

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_gol_edit_box.h

  Summary:
    This is the header file for the edit box object of the GOL.

  Description:
    Refer to Microchip Graphics Library for complete documentation of the
    Edit Box Object.
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

#ifndef _GFX_GOL_EDITBOX_H
// DOM-IGNORE-BEGIN
    #define _GFX_GOL_EDITBOX_H
// DOM-IGNORE-END

#include <stdint.h>
#include "gfx/gfx_gol.h"

// *****************************************************************************
/* 
    <GROUP gol_types>

    Typedef:
        GFX_GOL_EDITBOX_STATE

    Summary:
        Specifies the different states of the Edit Box object.

    Description:
        This enumeration specifies the different states of the Edit Box
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
    // Property bit for focus state. Cursor caret will be drawn when
    // GFX_GOL_EDITBOX_ENABLE_CARET_STATE is also set.
    GFX_GOL_EDITBOX_FOCUSED_STATE    
        /*DOM-IGNORE-BEGIN*/  = 0x0001 /*DOM-IGNORE-END*/,  
    // Property bit to indicate object is disabled.
    GFX_GOL_EDITBOX_DISABLED_STATE
        /*DOM-IGNORE-BEGIN*/  = 0x0002 /*DOM-IGNORE-END*/,  
    // Property bit to indicate cursor caret will always be shown.
    GFX_GOL_EDITBOX_ENABLE_CARET_STATE
        /*DOM-IGNORE-BEGIN*/  = 0x0004 /*DOM-IGNORE-END*/,  
    // Draw bit to indicate the cursor caret will be drawn if 
    // GFX_GOL_EDITBOX_FOCUSED_STATE state bit is set and 
    // erase when GFX_GOL_EDITBOX_FOCUSED_STATE state bit is not set.
    GFX_GOL_EDITBOX_DRAW_CARET_STATE  
        /*DOM-IGNORE-BEGIN*/  = 0x1000 /*DOM-IGNORE-END*/,  
    // Draw bit to indicate focus must be redrawn.
    GFX_GOL_EDITBOX_DRAW_FOCUS_STATE
        /*DOM-IGNORE-BEGIN*/  = 0x2000 /*DOM-IGNORE-END*/,  
    // Draw bit to indicate object must be redrawn.
    GFX_GOL_EDITBOX_DRAW_STATE  
        /*DOM-IGNORE-BEGIN*/  = 0x4000 /*DOM-IGNORE-END*/,  
    // Draw bit to indicate object must be removed from screen.
    GFX_GOL_EDITBOX_HIDE_STATE 
        /*DOM-IGNORE-BEGIN*/  = 0x8000 /*DOM-IGNORE-END*/,  
} GFX_GOL_EDITBOX_STATE;


// *****************************************************************************
/* 
    <GROUP gol_types>

    Typedef:
        GFX_GOL_EDITBOX

    Summary:
        Defines the structure used for the Edit Box object.
        
    Description:
        Defines the parameters required for a Edit Box Object.
       Object is drawn with the defined shape parameters and values 
        set on the given fields.
        
    Remarks:
        None.
        
*/
// *****************************************************************************
typedef struct
{
    GFX_GOL_OBJ_HEADER  hdr;            // Generic header for all Objects (see GFX_GOL_OBJ_HEADER).
    uint16_t            textHeight;     // Pre-computed text height.
    GFX_XCHAR           *pText;         // Pointer to text buffer.
    GFX_ALIGNMENT       alignment;      // text alignment
    uint16_t            charMax;        // Maximum number of characters in the edit box.
    uint16_t            length;         // Current text length.
} GFX_GOL_EDITBOX;

// *****************************************************************************
/*  
    <GROUP gol_editbox_object>
    
    Function:
        GFX_GOL_EDITBOX  *GFX_GOL_EditBoxCreate(
                                uint16_t            ID,
                                uint16_t            left,
                                uint16_t            top,
                                uint16_t            right,
                                uint16_t            bottom,
                                uint16_t            state,
                                GFX_XCHAR           *pText,
                                uint16_t            charMax,
                                GFX_ALIGNMENT       alignment,
                                GFX_GOL_OBJ_SCHEME  *pScheme)
    Summary:
        This function creates a GFX_GOL_EDITBOX object with the parameters
        given. It automatically attaches the new object into a global
        linked list of objects and returns the address of the object.

    Description:
        This function creates a GFX_GOL_EDITBOX object with the parameters
        given. It automatically attaches the new object into a global
        linked list of objects and returns the address of the object.

        This function returns the pointer to the newly created object.
        If the object is not successfully created, it returns NULL.

        The behavior of GFX_GOL_ListBoxCreate() will be undefined if one
        of the following is true:
        - left >= right
        - top >= bottom
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
        pText - Pointer to the text of the object.
        charMax - Defines the maximum number of characters in the edit box.
        alignment - text alignment of the text used in the object.
        pScheme - Pointer to the style scheme used.

    Returns:
        Pointer to the newly created object.

    Example:
        <code>
            // assume pScheme to be initialized with the style scheme

            #define MAX_CHAR_COUNT 15

            GFX_GOL_OBJ_SCHEME      *pScheme;
            GFX_GOL_EDITBOX         *pEb;
            GFX_GOL_EDITBOX_STATE   state;

            GFX_XCHAR               TextBuffer[MAX_CHAR_COUNT + 1];

            // assume pScheme is initialized to a scheme in memory.
            state = GFX_GOL_EDITBOX_DRAW_STATE;

            pEb = GFX_GOL_EditBoxCreate(
                    1, 20, 64, 50, 118,
                    state, TextBuffer, MAX_CHAR_COUNT
                    GFX_ALIGN_CENTER,
                    pScheme);

            // check if object is not created
            if (pEb == NULL)
               return 0;

        </code>

*/
// *****************************************************************************
GFX_GOL_EDITBOX  *GFX_GOL_EditBoxCreate(
                                uint16_t            ID,
                                uint16_t            left,
                                uint16_t            top,
                                uint16_t            right,
                                uint16_t            bottom,
                                uint16_t            state,
                                GFX_XCHAR           *pText,
                                uint16_t            charMax,
                                GFX_ALIGNMENT       alignment,
                                GFX_GOL_OBJ_SCHEME  *pScheme);

// *****************************************************************************
/*  
    <GROUP gol_editbox_object>
    
    Function:
        GFX_XCHAR *GFX_GOL_EditBoxTextGet(
                                GFX_GOL_EDITBOX *pObject)

    Summary:
        This function returns the address of the current text
        string used by the object.

    Description:
        This function returns the address of the current text
        string used by the object.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.

    Returns:
        Pointer to text string.

    Example:
        None.

*/
// *****************************************************************************
#define GFX_GOL_EditBoxTextGet(pObject)  (((GFX_GOL_EDITBOX *)pObject)->pText)

// *****************************************************************************
/*  
    <GROUP gol_editbox_object>
    
    Function:
        GFX_XCHAR *GFX_GOL_EditBoxTextSet(
                                GFX_GOL_EDITBOX *pObject,
                                GFX_XCHAR *pText)

    Summary:
        This function sets the text in the object.

    Description:
        This function sets the text in the object. This function
        copies the text located in the address pointed to by pText
        to the object text buffer.

        The string length must be less than or equal to the maximum
        characters allowed in the object. The object will truncate
        the string once it reaches the maximum length.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.
        pText - pointer to the text string to be copied.

    Returns:
        None.

    Example:
        <code>
            #define MAX_EDITBOX_CHAR_LENGTH 20
            GFX_XCHAR Label0[] = "This is really a long string";
 
            // assume pEb is a pointer initialized to an edit box that
            // as a buffer with 20 characters allowed.

            GFX_GOL_EditBoxTextSet(pEb, Label0);

            // at this point the edit box contains
            // This is really a lon" - truncated to only 20 characters.
        </code>

*/
// *****************************************************************************
void GFX_GOL_EditBoxTextSet(GFX_GOL_EDITBOX *pObject, GFX_XCHAR* pText);

// *****************************************************************************
/*  
    <GROUP gol_editbox_object>
    
    Function:
        GFX_STATUS GFX_GOL_EditBoxCharAdd(
                                GFX_GOL_EDITBOX *pObject,
                                GFX_XCHAR ch)

    Summary:
        This function adds a character at the end of the text used by
        the object.

    Description:
        This function adds a character at the end of the text used by
        the object. When the object contains the maximum number of
        characters any addition call to this function will
        not affect the text in the object.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - Pointer to the object.
        ch - The character to be added.

    Returns:
        The status of the addition.
        - GFX_STATUS_SUCCESS - when the addition was successful.
        - GFX_STATUS_FAILURE - when the addition was not successful.

    Example:
        None.

*/
// *****************************************************************************
GFX_STATUS GFX_GOL_EditBoxCharAdd(
                                GFX_GOL_EDITBOX *pObject,
                                GFX_XCHAR ch);

// *****************************************************************************
/*  
    <GROUP gol_editbox_object>
    
    Function:
        GFX_STATUS GFX_GOL_EditBoxCharRemove(
                                GFX_GOL_EDITBOX *pObject)

    Summary:
        This function removes a character at the end of the text used by
        the object.

    Description:
        This function removes a character at the end of the text used by
        the object.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - Pointer to the object.

    Returns:
        The status of the addition.
        - GFX_STATUS_SUCCESS - when the removal was successful.
        - GFX_STATUS_FAILURE - when the removal has no effect and the buffer
                               is empty.

    Example:
        None.

*/
// *****************************************************************************
GFX_STATUS GFX_GOL_EditBoxCharRemove(
                                GFX_GOL_EDITBOX *pObject);

// *****************************************************************************
/*  
    <GROUP gol_editbox_object>
    
    Function:
        GFX_ALIGNMENT GFX_GOL_EditBoxTextAlignmentGet(
                                GFX_GOL_EDITBOX *pObject)

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
#define GFX_GOL_EditBoxTextAlignmentGet(pObject)                \
                (((GFX_GOL_EDITBOX *)pObject)->alignment)

// *****************************************************************************
/*  
    <GROUP gol_editbox_object>
    
    Function:
        void GFX_GOL_EditBoxTextAlignmentSet(
                                GFX_GOL_EDITBOX *pObject,
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
#define GFX_GOL_EditBoxTextAlignmentSet(pObject, align)         \
                (((GFX_GOL_EDITBOX *)pObject)->alignment = align)

// *****************************************************************************
/*  
    <GROUP gol_editbox_object>
    
    Function:
        GFX_GOL_TRANSLATED_ACTION GFX_GOL_EditBoxActionGet(
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
    	Translated Message                  Input Source    Set/Clear State Bit             Description
     	##################                  ############    #######################         ###################################################
        GFX_GOL_EDITBOX_ACTION_ADD_CHAR     Keyboard        EVENT_CHARCODE                  New character should be added.
        GFX_GOL_EDITBOX_ACTION_DEL_CHAR     Keyboard        EVENT_KEYPRESS                  Last character should be removed.
        GFX_GOL_EDITBOX_ACTION_TOUCHSCREEN  Touch Screen    GFX_GOL_EDITBOX_FOCUSED_STATE   Focus will be drawn, caret displayed when enabled.
        GFX_GOL_OBJECT_ACTION_INVALID       Any             Any                             If the message did not affect the object.
	</TABLE>

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - The pointer to the object where the message will be
                   evaluated to check if the message will affect the object.
        pMessage - Pointer to the the message from the user interface.

    Returns:
        - GFX_GOL_EDITBOX_ACTION_ADD_CHAR    - New character should be added
        - GFX_GOL_EDITBOX_ACTION_DEL_CHAR    - Last character should be removed.
        - GFX_GOL_EDITBOX_ACTION_TOUCHSCREEN - focus will be drawn when enabled.
                                               Careet will be drawn when
                                               enabled.
        - GFX_GOL_OBJECT_ACTION_INVALID - Object is not affected

    Example:
        None.

*/
// *****************************************************************************
GFX_GOL_TRANSLATED_ACTION GFX_GOL_EditBoxActionGet(
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage);

// *****************************************************************************
/*  
    <GROUP gol_editbox_object>
    
    Function:
        void GFX_GOL_EditBoxActionSet(
                                GFX_GOL_TRANSLATED_ACTION translatedMsg,
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
    	Translated Message                  Input Source	Set/Clear State Bit                 Description
     	##################                  ############	###################                 #######################################################
     	GFX_GOL_EDITBOX_ACTION_ADD_CHAR     Keyboard        Set GFX_GOL_EDITBOX_DRAW_STATE      New character is added and Edit Box will be redrawn.
        GFX_GOL_EDITBOX_ACTION_DEL_CHAR     KeyBoard        Set GFX_GOL_EDITBOX_DRAW_STATE      Last character is removed and Edit Box will be redrawn.
        GFX_GOL_EDITBOX_ACTION_TOUCHSCREEN  Touch Screen    Set GFX_GOL_EDITBOX_FOCUSED_STATE   When enabled, focus will be redrawn, caret will also
                                                                                                    be redrawn if enabled.
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
void GFX_GOL_EditBoxActionSet(
                                GFX_GOL_TRANSLATED_ACTION translatedMsg,
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage);

// *****************************************************************************
/*  
    <GROUP gol_editbox_object>
    
    Function:
        GFX_STATUS GFX_GOL_EditBoxDraw(void *pObject)

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
GFX_STATUS GFX_GOL_EditBoxDraw(void *pObject);

#endif // _GFX_GOL_EDITBOX_H

