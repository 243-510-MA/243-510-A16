/*******************************************************************************
 Module for Microchip Graphics Library - Graphic Object Layer

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_gol_text_entry.h

  Summary:
    This is the header file for the text entry object of the GOL.

  Description:
    Refer to Microchip Graphics Library for complete documentation of the
    Text Entry Object.
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

#ifndef _GFX_GOL_TEXT_ENTRY_H
// DOM-IGNORE-BEGIN
    #define _GFX_GOL_TEXT_ENTRY_H
// DOM-IGNORE-END

#include <stdbool.h>
#include <stdint.h>
#include "gfx/gfx_gol.h"

// *****************************************************************************
/* 
    <GROUP gol_types>

    Typedef:
        GFX_GOL_TEXTENTRY_STATE

    Summary:
        Specifies the different states of the Text Entry object.

    Description:
        This enumeration specifies the different states of the Text Entry
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
    GFX_GOL_TEXTENTRY_DISABLED_STATE
        /*DOM-IGNORE-BEGIN*/  = 0x0002 /*DOM-IGNORE-END*/,
    // Property bit for press state.
    GFX_GOL_TEXTENTRY_KEY_PRESSED_STATE 
        /*DOM-IGNORE-BEGIN*/  = 0x0004 /*DOM-IGNORE-END*/,
    // Bit to hide the entered characters and instead
    // echo "*" characters to the display.
    GFX_GOL_TEXTENTRY_ECHO_HIDE_STATE   
            /*DOM-IGNORE-BEGIN*/  = 0x0008 /*DOM-IGNORE-END*/,
    // Draw bit to indicate redraw of the text displayed is needed.
    GFX_GOL_TEXTENTRY_UPDATE_TEXT_STATE 
        /*DOM-IGNORE-BEGIN*/  = 0x1000 /*DOM-IGNORE-END*/,
    // Draw bit to indicate redraw of a key is needed.
    GFX_GOL_TEXTENTRY_UPDATE_KEY_STATE  
        /*DOM-IGNORE-BEGIN*/  = 0x2000 /*DOM-IGNORE-END*/,
    // Draw bit to indicate object must be redrawn.
    GFX_GOL_TEXTENTRY_DRAW_STATE        
        /*DOM-IGNORE-BEGIN*/  = 0x4000 /*DOM-IGNORE-END*/,
    // Draw bit to indicate object must be removed from screen.
    GFX_GOL_TEXTENTRY_HIDE_STATE        
        /*DOM-IGNORE-BEGIN*/  = 0x8000 /*DOM-IGNORE-END*/,
} GFX_GOL_TEXTENTRY_STATE;

// *****************************************************************************
/* 
    <GROUP gol_types>

    Typedef:
        GFX_GOL_TEXTENTRY_KEY_COMMAND_TYPE

    Summary:
        Specifies the different commands available for command keys
        of the Text Entry object.

    Description:
        This enumeration specifies the different commands available for
        the command keys of the object.

        The GFX_GOL_TEXTENTRY_ENTER_COM command can be used to customize 
        application code in the message callback function. Use the returned 
        translated GFX_GOL_TEXTENTRY_ACTION_ENTER to detect the key pressed was
        assigned the enter command. Refer to GFX_GOL_TextEntryActionGet() 
        for details.

    Remarks:
        None.
        
*/
// *****************************************************************************
typedef enum
{
    // This type is used when the key is not assigned to any command.
    GFX_GOL_TEXTENTRY_NONE_COM
        /*DOM-IGNORE-BEGIN*/    = 0x00 /*DOM-IGNORE-END*/,    
    // This type is used to assign a "delete" command on a key.
    GFX_GOL_TEXTENTRY_DELETE_COM      
        /*DOM-IGNORE-BEGIN*/    = 0x01 /*DOM-IGNORE-END*/,    
    // This type is used to assign an insert "space" command on a key.
    GFX_GOL_TEXTENTRY_SPACE_COM       
        /*DOM-IGNORE-BEGIN*/    = 0x02 /*DOM-IGNORE-END*/,    
    // This type is used to assign an "enter" 
    // (carriage return) command on a key.
    GFX_GOL_TEXTENTRY_ENTER_COM 
        /*DOM-IGNORE-BEGIN*/    = 0x03 /*DOM-IGNORE-END*/,    
} GFX_GOL_TEXTENTRY_KEY_COMMAND_TYPE;

// DOM-IGNORE-BEGIN
/*********************************************************************
  Optional settings
  Set this parameter in the build configuration to enable rounded
  buttons.
*********************************************************************/
#ifndef TE_ROUNDEDBUTTON_RADIUS    
    #define TE_ROUNDEDBUTTON_RADIUS 0
#endif
// DOM-IGNORE-END

// *****************************************************************************
/* 
    <GROUP gol_types>

    Typedef:
        GFX_GOL_TEXTENTRY_KEYMEMBER

    Summary:
        Defines the structure used to describe a key in the Text Entry object.
        
    Description:
        Defines the structure used to describe a key in the Text Entry object.
        Strings displayed on each key is assigned here as well as the commands
        if key is assigned a command key.
        
    Remarks:
        None.
        
*/
// *****************************************************************************
typedef struct
{
    uint16_t        left;       // Left position of the key
    uint16_t        top;        // Top position of the key
    uint16_t        right;      // Right position of the key
    uint16_t        bottom;     // Bottom position of the key
    uint16_t        index;      // Index of the key in the list
    uint16_t        state;      // State of the key. Either Pressed 
                                // (GFX_GOL_TEXTENTRY_KEY_PRESSED_STATE) or Released (0)
    bool            update;     // flag to indicate key is to be redrawn with the 
                                // current state
    GFX_GOL_TEXTENTRY_KEY_COMMAND_TYPE  command;    // Command of the key. Either 
                                // GFX_GOL_TEXTENTRY_DELETE_COM, 
                                // GFX_GOL_TEXTENTRY_SPACE_COM or 
                                // GFX_GOL_TEXTENTRY_ENTER_COM.
    GFX_XCHAR       *pKeyName;  // Pointer to the custom text assigned to the key. 
                                // This is displayed over the face of the key.
    int16_t         textWidth;  // Computed text width, done at creation. Used to 
                                // predict size and position of text on the key face.
    int16_t         textHeight; // Computed text height, done at creation. Used to 
                                // predict size and position of text on the key face.
    void            *pNextKey;  // Pointer to the next key parameters.
} GFX_GOL_TEXTENTRY_KEYMEMBER;

// *****************************************************************************
/* 
    <GROUP gol_types>

    Typedef:
        GFX_GOL_TEXTENTRY

    Summary:
        Defines the structure used for the Text Entry object.
        
    Description:
        Defines the parameters required for a Text Entry object.
        Object is drawn with the defined shape parameters and values 
        set on the given fields. 
        
    Remarks:
        None.
        
*/
// *****************************************************************************
typedef struct
{
    GFX_GOL_OBJ_HEADER              hdr;            // Generic header for all objects (see GFX_GOL_OBJ_HEADER).
    uint16_t                        horizontalKeys; // Number of horizontal keys.
    uint16_t                        verticalKeys;   // Number of vertical keys.
    GFX_XCHAR                       *pTeOutput;     // Pointer to the buffer
                                                    // assigned by the user
                                                    // which holds the text
                                                    // shown in the editbox.
    GFX_ALIGNMENT                   alignment;      // Defines the text alignment.

    uint16_t                        CurrentLength;  // Current length of the
                                                    // string in the buffer. The 
                                                    // maximum value of this is equal
                                                    // to outputLenMax. User creates 
                                                    // and manages the buffer. Buffer 
                                                    // can also be managed using the 
                                                    // APIs provided to add a character, 
                                                    // delete the last character or
                                                    // clear the buffer.

    uint16_t                        outputLenMax;   // Maximum expected length
                                                    // of output buffer. Object will 
                                                    // update this parameter when 
                                                    // adding, removing characters 
                                                    // or clearing the buffer and 
                                                    // switching buffers.

    GFX_GOL_TEXTENTRY_KEYMEMBER     *pActiveKey;    // Pointer to the active
                                                    // key. This is only used
                                                    // by the object. User must
                                                    // not change the value of
                                                    // this parameter directly.
    GFX_GOL_TEXTENTRY_KEYMEMBER     *pHeadOfList;   // Pointer to head
                                                    // of the list
    GFX_RESOURCE_HDR                *pDisplayFont;  // Pointer to the font used
                                                    // in displaying the text.
} GFX_GOL_TEXTENTRY;

// *****************************************************************************
/*  
    <GROUP gol_textentry_object>
    
    Function:
        GFX_GOL_TEXTENTRY *GFX_GOL_TextEntryCreate(
                                uint16_t            ID,
                                uint16_t            left,
                                uint16_t            top,
                                uint16_t            right,
                                uint16_t            bottom,
                                uint16_t            state,
                                uint16_t            horizontalKeys,
                                uint16_t            verticalKeys,
                                GFX_XCHAR           *pText[],
                                GFX_XCHAR           *pBuffer,
                                GFX_ALIGNMENT       alignment,
                                uint16_t            bufferLength,
                                GFX_RESOURCE_HDR    *pDisplayFont,
                                GFX_GOL_OBJ_SCHEME  *pScheme);

    Summary:
        This function creates a GFX_GOL_TEXTENTRY object with the parameters
        given. It automatically attaches the new object into a global
        linked list of objects and returns the address of the object.


    Description:
        This function creates a GFX_GOL_TEXTENTRY object with the parameters
        given. It automatically attaches the new object into a global
        linked list of objects and returns the address of the object.

        This function returns the pointer to the newly created object.
        If the object is not successfully created, it returns NULL.

        The behavior of GFX_GOL_TextEntryCreate() will be undefined if one
        of the following is true:
        - left >= right
        - top >= bottom
        - pScheme is not pointing to a GFX_GOL_OBJ_SCHEME
        - horizontal key or vertical key count is 0
        - pText is an unterminated string
        - pBuffer is initialized to an allocated memory.

    Precondition:
        None.

    Parameters:
        ID - Unique user defined ID for the object instance.
        left - Left most position of the object.
        top - Top most position of the object.
        right - Right most position of the object.
        bottom - Bottom most position of the object.
        radius - Radius of the rounded edge. When using gradient
                 buttons and radius != 0, emboss size <= radius.
                 If this is not met, the the GFX_GOL_BUTTON face will
                 not have gradient effect.
        state - Sets the initial state of the object.
        horizontalKeys - Number of horizontal keys
        verticalKeys - Number of vertical keys
        pText	- array of pointer to the custom "text" assigned by the user.
        pBuffer - pointer to the buffer that holds the text to be displayed.
        alignment - text alignment of the text used in the object.
        bufferLength - length of the buffer assigned by the user. The choice
                       of the length should include the string null terminator.
                       For example: if the bufferLength is set to 3, only
                       two characters can be shown on the object since the
                       last character will be the string terminator character.
        pDisplayFont - pointer to the font image to be used on the
                       edit box section of the object.
        pScheme - Pointer to the style scheme used.

    Returns:
        Pointer to the newly created object.

    Example:
        <code>
            #define ID_TEXTENTRY 0x20
            #define TEBUFFERSIZE 20    // 20 characters

            GFX_XCHAR delKey[] = {'d','e','l',0};
            GFX_XCHAR spaceKey[] = {'s','p','c',0};
            GFX_XCHAR enterKey[] = {'e','n','t','e','r',0};
            GFX_XCHAR exitKey[] = {'M','a','i','n',0};
            GFX_XCHAR key1[] = {'1',0};
            GFX_XCHAR key2[] = {'2',0};
            GFX_XCHAR key3[] = {'3',0};
            GFX_XCHAR key4[] = {'4',0};
            GFX_XCHAR key5[] = {'5',0};
            GFX_XCHAR key6[] = {'6',0};
            GFX_XCHAR key7[] = {'7',0};
            GFX_XCHAR key8[] = {'8',0};
            GFX_XCHAR key9[] = {'9',0};
            GFX_XCHAR key0[] = {'0',0};
            GFX_XCHAR keystar[] = {'*',0};
            GFX_XCHAR keypound[] = {'#',0};

            GFX_XCHAR *pKeyNames[] = { key1,    key2, key3,     delKey,
                                       key4,    key5, key6,     spaceKey,
                                       key7,    key8, key9,     enterKey,
                                       keystar, key0, keypound, exitKey
                                     };

            // assume pScheme is initialized
            // myFont is a font in memory
            GFX_GOL_OBJ_SCHEME *pScheme;
            GFX_GOL_TEXTENTRY  *pTe;
            GFX_RESOURCE_HDR   *pFont = &myFont;

            pTe = GFX_GOL_TextEntryCreate(
                ID_TEXTENTRY,                   // ID
                30,80,235,160,                  // dimension
                GFX_GOL_TEXTENTRY_DRAW_STATE,   // draw the object
                4,                              // number of horizontal keys
                4,                              // number of vertical keys
                pKeyNames,                      // pointer to the array of key names
                "Enter Code",                   // initial text
                GFX_ALIGN_CENTER,               // align text on the center
                TEBUFFERSIZE,                   // size of the buffer for text
                pFont,                          // pointer to the font of the
                                                // diplayed text
                pScheme);                       // use given scheme
        </code>

*/
// *****************************************************************************
GFX_GOL_TEXTENTRY *GFX_GOL_TextEntryCreate(
                                uint16_t            ID,
                                uint16_t            left,
                                uint16_t            top,
                                uint16_t            right,
                                uint16_t            bottom,
                                uint16_t            state,
                                uint16_t            horizontalKeys,
                                uint16_t            verticalKeys,
                                GFX_XCHAR           *pText[],
                                GFX_XCHAR           *pBuffer,
                                GFX_ALIGNMENT       alignment,
                                uint16_t            bufferLength,
                                GFX_RESOURCE_HDR    *pDisplayFont,
                                GFX_GOL_OBJ_SCHEME  *pScheme);

// *****************************************************************************
/*  
    <GROUP gol_textentry_object>
    
    Function:
        void GFX_GOL_TextEntryBufferSet(
                                GFX_GOL_TEXTENTRY *pObject,
                                GFX_XCHAR *pText,
                                uint16_t MaxSize)

    Summary:
        This function sets the buffer used to display text.

    Description:
        This function sets the buffer used to display text. If the
        buffer is initialized with a string, the string must be
        a null terminated string. If the string length is greater
        than MaxSize, string will be truncated to MaxSize.
        pText must point to a valid memory location with size equal
        to MaxSize. The total number of characters that will be
        displayed on the object will be MaxSize - 1 since the
        last character will be the string terminator character.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.
        pText- pointer to the new text buffer to be displayed.
        maxSize - maximum length of the new buffer to be used.

    Returns:
        None.
 
    Example:
        None.

*/
// *****************************************************************************
void GFX_GOL_TextEntryBufferSet(
                                GFX_GOL_TEXTENTRY *pObject,
                                GFX_XCHAR *pText,
                                uint16_t MaxSize);

// *****************************************************************************
/*  
    <GROUP gol_textentry_object>
    
    Function:
        GFX_XCHAR *GFX_GOL_TextEntryBufferGet(
                                GFX_GOL_TEXTENTRY *pObject)

    Summary:
        This function returns the buffer used to display text.

    Description:
        This function returns the buffer used to display text. 

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.

    Returns:
        The pointer to the text buffer used to display text.
 
    Example:
        None.

*/
// *****************************************************************************
GFX_XCHAR *GFX_GOL_TextEntryBufferGet(
                                GFX_GOL_TEXTENTRY *pObject);

// *****************************************************************************
/*  
    <GROUP gol_textentry_object>
    
    Function:
        void GFX_GOL_TextEntryBufferClear(
                                GFX_GOL_TEXTENTRY *pObject)

    Summary:
        This function will clear the data in the display.

    Description:
        This function will clear the data in the display. Object must
        be redrawn to reflect the change in the buffer. Use the
        drawing state bit GFX_GOL_TEXTENTRY_UPDATE_TEXT_STATE to update
        the text on the screen.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void GFX_GOL_TextEntryBufferClear(GFX_GOL_TEXTENTRY *pObject);

// *****************************************************************************
/*  
    <GROUP gol_textentry_object>
    
    Function:
        bool GFX_GOL_TextEntryKeyIsPressed(
                                GFX_GOL_TEXTENTRY *pObject,
                                uint16_t index)

    Summary:
        This function will test if a key given by its index in the
        object is pressed.

    Description:
        This function will test if a key given by its index in the
        object is pressed.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.
        index - index or position of key that is being tested.

    Returns:
        TRUE - if the key is pressed.
        FALSE - if the key is not pressed.

    Example:
        None.

*/
// *****************************************************************************
bool GFX_GOL_TextEntryKeyIsPressed(GFX_GOL_TEXTENTRY *pObject, uint16_t index);

// *****************************************************************************
/*  
    <GROUP gol_textentry_object>
    
    Function:
        bool GFX_GOL_TextEntryKeyCommandSet(
                                GFX_GOL_TEXTENTRY *pObject,
                                uint16_t index,
                                GFX_GOL_TEXTENTRY_KEY_COMMAND_TYPE command)
    Summary:
        This function will assign a command to a key with the given index.

    Description:
        This function will assign a command to a key with the given index.
        (See GFX_GOL_TEXTENTRY_KEY_COMMAND_TYPE)

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.
        index - index or position of the key.
        command - command assigned for the key.

    Returns:
        TRUE - if the assignment was a success.
        FALSE - if the assignment was not successful.

    Example:
        None.

*/
// *****************************************************************************
bool GFX_GOL_TextEntryKeyCommandSet(
                                GFX_GOL_TEXTENTRY *pObject,
                                uint16_t index,
                                GFX_GOL_TEXTENTRY_KEY_COMMAND_TYPE command);

// *****************************************************************************
/*  
    <GROUP gol_textentry_object>
    
    Function:
        GFX_GOL_TEXTENTRY_KEY_COMMAND_TYPE GFX_GOL_TextEntryKeyCommandSet(
                                GFX_GOL_TEXTENTRY *pObject,
                                uint16_t index,
                                GFX_GOL_TEXTENTRY_KEY_COMMAND_TYPE command)
    Summary:
        This function will return the currently assigned command to
        the key with the given index.

    Description:
        This function will return the currently assigned command to
        the key with the given index.
        (See GFX_GOL_TEXTENTRY_KEY_COMMAND_TYPE)

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.
        index - index or position of the key.

    Returns:
        Command assigned to the key (See GFX_GOL_TEXTENTRY_KEY_COMMAND_TYPE).

    Example:
        None.

*/
// *****************************************************************************
GFX_GOL_TEXTENTRY_KEY_COMMAND_TYPE GFX_GOL_TextEntryKeyCommandGet(
                                GFX_GOL_TEXTENTRY *pObject,
                                uint16_t index);

// *****************************************************************************
/*  
    <GROUP gol_textentry_object>
    
    Function:
        bool GFX_GOL_TextEntryKeyTextSet(
                                GFX_GOL_TEXTENTRY *pObject,
                                uint16_t index,
                                GFX_XCHAR *pText)
    Summary:
        This function will set the text assigned to a key with
        the given index.

    Description:
        This function will set the text assigned to a key with
        the given index.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.
        index - index or position of the key.
        pText - pointer to the text that will be assigned to the key.

    Returns:
        TRUE - if the assignment was a success.
        FALSE - if the assignment was not successful.

    Example:
        None.

*/
// *****************************************************************************
bool GFX_GOL_TextEntryKeyTextSet(
                                GFX_GOL_TEXTENTRY *pObject,
                                uint16_t index,
                                GFX_XCHAR *pText);

// *****************************************************************************
/*  
    <GROUP gol_textentry_object>
    
    Function:
        GFX_GOL_TEXTENTRY_KEYMEMBER *GFX_GOL_TextEntryKeyListCreate(
                                GFX_GOL_TEXTENTRY *pObject,
                                GFX_XCHAR *pText[])
    Summary:
        This function will create the list of key members that holds the
        information on each key.

    Description:
        This function will create the list of key members that holds the
        information on each key. The number of keys is determined by the
        equation (keycount = verticalKeys*horizontalKeys). The object
        creates the  information holder for each key automatically and
        assigns each entry in the *pText[] array with the first entry
        automatically assigned to the key with an index of 1.

        The number of entries to *pText[] must be at least equal to
        keycount. The last key is assigned with an index of keycount-1.

        No checking is performed on the length of *pText[] entries to
        match (verticalKeys*horizontalKeys).

        The function behavior is undefined if the pText[] array is less
        than the keycount value.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.
        pText - pointer to the text defined by the application.

    Returns:
        Returns the pointer to the newly created key member list.
        A NULL is returned if the list is not created succesfully.

    Example:
        None.

*/
// *****************************************************************************
GFX_GOL_TEXTENTRY_KEYMEMBER *GFX_GOL_TextEntryKeyListCreate(
                                GFX_GOL_TEXTENTRY *pObject,
                                GFX_XCHAR *pText[]);

// *****************************************************************************
/*  
    <GROUP gol_textentry_object>
    
    Function:
        void GFX_GOL_TextEntryKeyMemberListDelete(
                                void *pObject)

    Summary:
        This function will delete the key member list assigned to
        the object.

    Description:
        This function will delete the key member list assigned to
        the object from memory. Pointer to the key member list is
        then initialized to NULL. All memory resources allocated to
        the key member list is freed.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void GFX_GOL_TextEntryKeyMemberListDelete(void *pObject);

// *****************************************************************************
/*  
    <GROUP gol_textentry_object>
    
    Function:
        void GFX_GOL_TextEntryLastCharDelete(
                                GFX_GOL_TEXTENTRY *pObject)

    Summary:
        This function will remove the last character of the buffer
        and replace it with a string terminator.

    Description:
        This function will remove the last character of the buffer
        and replace it with a string terminator. Drawing states
        GFX_GOL_TEXTENTRY_UPDATE_TEXT_STATE or GFX_GOL_TEXTENTRY_DRAW_STATE
        must be set to see the effect of the addition.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void GFX_GOL_TextEntryLastCharDelete(GFX_GOL_TEXTENTRY *pObject);

// *****************************************************************************
/*  
    <GROUP gol_textentry_object>
    
    Function:
        void GFX_GOL_TextEntrySpaceCharAdd(
                                GFX_GOL_TEXTENTRY *pObject)

    Summary:
        This function will insert a space character to the end of
        the buffer.

    Description:
        This function will insert a space character to the end of
        the buffer. Drawing states GFX_GOL_TEXTENTRY_UPDATE_TEXT_STATE or
        GFX_GOL_TEXTENTRY_DRAW_STATE must be set to see the effect
        of the addition.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void GFX_GOL_TextEntrySpaceCharAdd(GFX_GOL_TEXTENTRY *pObject);

// *****************************************************************************
/*  
    <GROUP gol_textentry_object>
    
    Function:
        void GFX_GOL_TextEntryCharAdd(
                                GFX_GOL_TEXTENTRY *pObject)

    Summary:
        This function will insert a character to the end of
        the buffer.

    Description:
        This function will insert a character to the end of
        the buffer. Drawing states GFX_GOL_TEXTENTRY_UPDATE_TEXT_STATE or
        GFX_GOL_TEXTENTRY_DRAW_STATE must be set to see the effect
        of the addition.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void GFX_GOL_TextEntryCharAdd(GFX_GOL_TEXTENTRY *pObject);

// *****************************************************************************
/*  
    <GROUP gol_textentry_object>
    
    Function:
        GFX_GOL_TRANSLATED_ACTION GFX_GOL_TextEntryActionGet(
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
    	Translated Message                  Input Source    Set/Clear State Bit         Description
        ##################                  ############    #######################     ##################################################################################################################################################################
        GFX_GOL_TEXTENTRY_ACTION_PRESS      Touch Screen    EVENT_PRESS, EVENT_MOVE     If the event occurs and the x,y position falls in the face of one of the keys of the object while the key is unpressed.
        GFX_GOL_TEXTENTRY_ACTION_RELEASED   Touch Screen    EVENT_MOVE                  If the event occurs and the x,y position falls outside the face of one of the keys of the object while the key is pressed.
        GFX_GOL_TEXTENTRY_ACTION_RELEASED   Touch Screen    EVENT_RELEASE               If the event occurs and the x,y position falls does not falls inside any of the faces of the keys of the object.
        GFX_GOL_TEXTENTRY_ACTION_ADD_CHAR   Touch Screen    EVENT_RELEASE, EVENT_MOVE   If the event occurs and the x,y position falls in the face of one of the keys of the object while the key is unpressed and the key is associated with no commands.
        GFX_GOL_TEXTENTRY_ACTION_DELETE     Touch Screen    EVENT_RELEASE, EVENT_MOVE   If the event occurs and the x,y position falls in the face of one of the keys of the object while the key is unpressed and the key is associated with delete command.
        GFX_GOL_TEXTENTRY_ACTION_SPACE      Touch Screen    EVENT_RELEASE, EVENT_MOVE   If the event occurs and the x,y position falls in the face of one of the keys of the object while the key is unpressed and the key is associated with space command.
        GFX_GOL_TEXTENTRY_ACTION_ENTER      Touch Screen    EVENT_RELEASE, EVENT_MOVE   If the event occurs and the x,y position falls in the face of one of the keys of the object while the key is unpressed and the key is associated with enter command.
        GFX_GOL_OBJECT_ACTION_INVALID       Any             Any                         If the message did not affect the object.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - The pointer to the object where the message will be
                   evaluated to check if the message will affect the object.
        pMessage - Pointer to the the message from the user interface.

    Returns:
        - GFX_GOL_TEXTENTRY_ACTION_PRESS    - A key is pressed
        - GFX_GOL_TEXTENTRY_ACTION_RELEASED - A key was released (generic for 
                                              keys with no commands or
                                              characters assigned)
        - GFX_GOL_TEXTENTRY_ACTION_ADD_CHAR - A key was released with character
                                              assigned
        - GFX_GOL_TEXTENTRY_ACTION_DELETE   - A key was released with delete
                                              command assigned
        - GFX_GOL_TEXTENTRY_ACTION_SPACE    - A key was released with space
                                              command assigned
        - GFX_GOL_TEXTENTRY_ACTION_ENTER    - A key was released with enter
                                              command assigned
        - GFX_GOL_OBJECT_ACTION_INVALID - Text Entry is not affected

    Example:
        None.

*/
// *****************************************************************************
GFX_GOL_TRANSLATED_ACTION GFX_GOL_TextEntryActionGet(
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage);

// *****************************************************************************
/*  
    <GROUP gol_textentry_object>
    
    Function:
        void GFX_GOL_TextEntryActionSet(
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
    	Translated Message                  Input Source    Set/Clear State Bit                             Description
     	##################                  ############    ###################                             ###########
        GFX_GOL_TEXTENTRY_ACTION_ADD_CHAR   Touch Screen,   Set GFX_GOL_TEXTENTRY_UPDATE_TEXT_STATE,        Add a character in the buffer and update the text displayed.
        GFX_GOL_TEXTENTRY_UPDATE_KEY_STATE,                 Clear GFX_GOL_TEXTENTRY_KEY_PRESSED_STATE
     	GFX_GOL_TEXTENTRY_ACTION_SPACE      Touch Screen,   Set GFX_GOL_TEXTENTRY_UPDATE_TEXT_STATE,        Insert a space character in the buffer and update the text displayed.
        GFX_GOL_TEXTENTRY_UPDATE_KEY_STATE,                 Clear GFX_GOL_TEXTENTRY_KEY_PRESSED_STATE
     	GFX_GOL_TEXTENTRY_ACTION_DELETE     Touch Screen,   Set GFX_GOL_TEXTENTRY_UPDATE_TEXT_STATE,        Delete the most recent character in the buffer and update the text displayed.
        GFX_GOL_TEXTENTRY_UPDATE_KEY_STATE,                 Clear GFX_GOL_TEXTENTRY_KEY_PRESSED_STATE
     	GFX_GOL_TEXTENTRY_ACTION_ENTER      Touch Screen,   Set GFX_GOL_TEXTENTRY_UPDATE_TEXT_STATE,        User can define the use of this event in the message callback. Object will just update the key.
        GFX_GOL_TEXTENTRY_UPDATE_KEY_STATE,                 Clear GFX_GOL_TEXTENTRY_KEY_PRESSED_STATE
        GFX_GOL_TEXTENTRY_ACTION_RELEASED   Touch Screen,   Clear GFX_GOL_TEXTENTRY_KEY_PRESSED_STATE       A Key in the object will be redrawn in the unpressed state.
                                                            Set GFX_GOL_TEXTENTRY_UPDATE_KEY_STATE
        GFX_GOL_TEXTENTRY_ACTION_PRESSED    Touch Screen,   Set GFX_GOL_TEXTENTRY_KEY_PRESSED_STATE         A Key in the object will be redrawn in the pressed state.
        GFX_GOL_TEXTENTRY_UPDATE_KEY_STATE
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
void GFX_GOL_TextEntryActionSet(
                                uint16_t translatedMsg,
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage);

// *****************************************************************************
/*  
    <GROUP gol_textentry_object>
    
    Function:
        GFX_STATUS GFX_GOL_TextEntryDraw(void *pObject)

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
GFX_STATUS GFX_GOL_TextEntryDraw(void *pObject);

#endif // _GFX_GOL_TEXT_ENTRY_H
