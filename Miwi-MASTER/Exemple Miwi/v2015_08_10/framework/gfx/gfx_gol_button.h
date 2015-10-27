/*******************************************************************************
 Module for Microchip Graphics Library - Graphic Object Layer

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_gol_button.h

  Summary:
    This is the header file for the button object of the GOL.

  Description:
    Refer to Microchip Graphics Library for complete documentation of the
    GFX_GOL_BUTTON Object.
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

#ifndef _GFX_GOL_BUTTON_H
// DOM-IGNORE-BEGIN
    #define _GFX_GOL_BUTTON_H
// DOM-IGNORE-END

#include <stdint.h>
#include "gfx/gfx_gol.h"

// *****************************************************************************
/* 
    <GROUP gol_types>

    Typedef:
        GFX_GOL_BUTTON_STATE

    Summary:
        Specifies the different states of the Button object.

    Description:
        This enumeration specifies the different states of the Button
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
    GFX_GOL_BUTTON_FOCUSED_STATE    
        /*DOM-IGNORE-BEGIN*/  = 0x0001 /*DOM-IGNORE-END*/,  
    // Property bit for disabled state.
    GFX_GOL_BUTTON_DISABLED_STATE  
        /*DOM-IGNORE-BEGIN*/  = 0x0002 /*DOM-IGNORE-END*/,  
    // Property bit for press state.
    GFX_GOL_BUTTON_PRESSED_STATE  
        /*DOM-IGNORE-BEGIN*/  = 0x0004 /*DOM-IGNORE-END*/,  
    // Property bit to indicate object will have a toggle behavior.
    GFX_GOL_BUTTON_TOGGLE_STATE
        /*DOM-IGNORE-BEGIN*/  = 0x0008 /*DOM-IGNORE-END*/,  
    // Property bit to indicate the object is a two tone type.
    GFX_GOL_BUTTON_TWOTONE_STATE  
        /*DOM-IGNORE-BEGIN*/  = 0x0100 /*DOM-IGNORE-END*/,  
    // Property bit to indicate the object will be drawn without a panel
    // (for faster drawing when the object image used is larger than the object's panel).
    GFX_GOL_BUTTON_NOPANEL_STATE
        /*DOM-IGNORE-BEGIN*/  = 0x0200 /*DOM-IGNORE-END*/,  
    // Draw bit to indicate focus must be redrawn.
    GFX_GOL_BUTTON_DRAW_FOCUS_STATE
        /*DOM-IGNORE-BEGIN*/  = 0x2000 /*DOM-IGNORE-END*/,  
    // Draw bit to indicate object must be redrawn.
    GFX_GOL_BUTTON_DRAW_STATE  
        /*DOM-IGNORE-BEGIN*/  = 0x4000 /*DOM-IGNORE-END*/,  
    // Draw bit to indicate object must be removed from screen.
    GFX_GOL_BUTTON_HIDE_STATE  
        /*DOM-IGNORE-BEGIN*/  = 0x8000 /*DOM-IGNORE-END*/,  
} GFX_GOL_BUTTON_STATE;

// *****************************************************************************
/* 
    <GROUP gol_types>

    Typedef:
        GFX_GOL_BUTTON

    Summary:
        Defines the structure used for the Button object.
        
    Description:
        Defines the structure used for the Button object.
        1. Width is determined by right - left parameter in 
           GFX_GOL_OBJ_HEADER.
        2. Height is determined by top - bottom parameter in 
           GFX_GOL_OBJ_HEADER.
        3. radius - specifies if the GFX_GOL_BUTTON will have a rounded
                    edge. If zero then the GFX_GOL_BUTTON will have
                    sharp (cornered) edge.
        4. If 2*radius = height = width, the GFX_GOL_BUTTON is a 
           circular GFX_GOL_BUTTON.

    Remarks:
        None.
        
*/
// *****************************************************************************
typedef struct
{
    GFX_GOL_OBJ_HEADER  hdr;            // Generic header for all Objects (see GFX_GOL_OBJ_HEADER).
    uint16_t            radius;         // Radius for rounded buttons.
    uint16_t            textWidth;      // Computed text width, done at creation.
    uint16_t            textHeight;     // Computed text height, done at creation.
    GFX_XCHAR           *pText;         // Pointer to the text used.
    GFX_ALIGNMENT       alignment;      // text alignment
    GFX_RESOURCE_HDR    *pPressImage;   // Pointer to bitmap used.
    GFX_RESOURCE_HDR    *pReleaseImage; // Pointer to bitmap used.
} GFX_GOL_BUTTON;

// *****************************************************************************
/*  
    <GROUP gol_button_object>

    Function:
        GFX_GOL_BUTTON  *GFX_GOL_ButtonCreate(
                                uint16_t            ID,
                                uint16_t            left,
                                uint16_t            top,
                                uint16_t            right,
                                uint16_t            bottom,
                                uint16_t            radius,
                                uint16_t            state,
                                GFX_RESOURCE_HDR    *pPressImage,
                                GFX_RESOURCE_HDR    *pReleaseImage,
                                GFX_XCHAR           *pText,
                                GFX_ALIGNMENT       alignment,
                                GFX_GOL_OBJ_SCHEME  *pScheme)
    Summary:
        This function creates a GFX_GOL_BUTTON object with the parameters
        given. It automatically attaches the new object into a global
        linked list of objects and returns the address of the object.

    Description:
        This function creates a GFX_GOL_BUTTON object with the parameters
        given. It automatically attaches the new object into a global
        linked list of objects and returns the address of the object.

        This function returns the pointer to the newly created object.
        If the object is not successfully created, it returns NULL.

        The object allows setting two images. One for the pressed state and
        the other for the release state. If no image is to be used for the
        object set both pointers to NULL.

        If only one image is used for both pressed and released state,
        set both pPressImage and pReleaseImage to the same image.

        The behavior of GFX_GOL_ButtonCreate() will be undefined if one
        of the following is true:
        - left >= right
        - top >= bottom
        - pScheme is not pointing to a GFX_GOL_OBJ_SCHEME
        - pPressImage and pReleaseImage is not pointing to a GFX_RESOURCE_HDR.
        - pText is an unterminated string

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
        pPressImage - Pointer to the image used on the face of the object
                when it is in the pressed state.
        pReleaseImage - Pointer to the image used on the face of the object
                when it is in the pressed state.
        pText - Pointer to the text of the object.
        alignment - text alignment of the text used in the object.
        pScheme - Pointer to the style scheme used.

    Returns:
        Pointer to the newly created object.

    Example:
        <code>
            GFX_GOL_OBJ_SCHEME *pScheme;
            GFX_GOL_BUTTON *buttons[3];
            GFX_GOL_BUTTON_STATE state;

            // assume pScheme is initialized to a scheme in memory.
            state = GFX_GOL_BUTTON_DRAW_STATE;

            buttons[0] = GFX_GOL_ButtonCreate(
                    1, 20, 64, 50, 118, 0,
                    state, NULL, NULL, "ON",
                    GFX_ALIGN_HCENTER | GFX_ALIGN_VCENTER,
                    pScheme);
            // check if GFX_GOL_BUTTON 0 is created
            if (buttons[0] == NULL)
               return 0;

            buttons[1] = GFX_GOL_ButtonCreate(
                    2, 52, 64, 82, 118, 0,
                    state, NULL, NULL, "OFF",
                    GFX_ALIGN_LEFT | GFX_ALIGN_VCENTER,
                    pScheme);
            // check if GFX_GOL_BUTTON 1 is created
            if (buttons[1] == NULL)
               return 0;

            buttons[2] = GFX_GOL_ButtonCreate(
                    3, 84, 64, 114, 118, 0,
                    state, NULL, NULL, "HI",
                    GFX_ALIGN_RIGHT | GFX_ALIGN_VCENTER,
                    pScheme);
            // check if GFX_GOL_BUTTON 2 is created
            if (buttons[2] == NULL)
               return 0;

            return 1;
        </code>

*/
// *****************************************************************************
GFX_GOL_BUTTON  *GFX_GOL_ButtonCreate(
                                uint16_t            ID,
                                uint16_t            left,
                                uint16_t            top,
                                uint16_t            right,
                                uint16_t            bottom,
                                uint16_t            radius,
                                uint16_t            state,
                                GFX_RESOURCE_HDR    *pPressImage,
                                GFX_RESOURCE_HDR    *pReleaseImage,
                                GFX_XCHAR           *pText,
                                GFX_ALIGNMENT       alignment,
                                GFX_GOL_OBJ_SCHEME  *pScheme);

// *****************************************************************************
/*  
    <GROUP gol_button_object>

    Function:
        void GFX_GOL_ButtonPressStateImageSet(
                                GFX_GOL_BUTTON *pObject,
                                GFX_RESOURCE_HDR *pImage)

    Summary:
        This function sets the image to be used when in the pressed
        state.

    Description:
        This function sets the image to be used when in the pressed
        state.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.
        pImage  - pointer to the image resource.

    Returns:
        None.

    Example:
        <code>
            // assume ImageIcon is a valid GFX_RESOURCE_HDR

            GFX_RESOURCE_HDR *pMyIcon = &ImageIcon;
            GFX_GOL_BUTTON *pButton;

            GFX_GOL_ButtonPressStateImageSet(pButton , pMyIcon);
        </code>

*/
// *****************************************************************************
#define GFX_GOL_ButtonPressStateImageSet(pObject, pImage)        \
            (((GFX_GOL_BUTTON *)pObject)->pPressImage = pImage)

// *****************************************************************************
/*  
    <GROUP gol_button_object>

    Function:
        void GFX_GOL_ButtonReleaseStateImageSet(
                                GFX_GOL_BUTTON *pObject,
                                GFX_RESOURCE_HDR *pImage)

    Summary:
        This function sets the image to be used when in the released
        state.

    Description:
        This function sets the image to be used when in the released
        state.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.
        pImage  - pointer to the image resource.

    Returns:
        None.

    Example:
        <code>
            // assume ImageIcon is a valid GFX_RESOURCE_HDR

            GFX_RESOURCE_HDR *pMyIcon = &ImageIcon;
            GFX_GOL_BUTTON *pButton;

            GFX_GOL_ButtonReleaseStateImageSet(pButton , pMyIcon);
        </code>

*/
// *****************************************************************************
#define GFX_GOL_ButtonReleaseStateImageSet(pObject, pImage)      \
            (((GFX_GOL_BUTTON *)pObject)->pReleaseImage = pImage)

// *****************************************************************************
/*  
    <GROUP gol_button_object>

    Function:
        GFX_RESOURCE_HDR *GFX_GOL_ButtonPressStateImageGet(
                                GFX_GOL_BUTTON *pObject)

    Summary:
        This function gets the image used when in the pressed state.

    Description:
        This function gets the image used when in the pressed state.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.

    Returns:
        Pointer to the image resource.

    Example:
        None.

*/
// *****************************************************************************
#define GFX_GOL_ButtonPressStateImageGet(pObject, pImage) \
            (((GFX_GOL_BUTTON *)pObject)->pPressImage)

// *****************************************************************************
/*  
    <GROUP gol_button_object>

    Function:
        GFX_RESOURCE_HDR *GFX_GOL_ButtonReleaseStateImageGet(
                                GFX_GOL_BUTTON *pObject)

    Summary:
        This function gets the image used when in the released state.

    Description:
        This function gets the image used when in the released state.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.

    Returns:
        Pointer to the image resource.

    Example:
        None.

*/
// *****************************************************************************
#define GFX_GOL_ButtonReleaseStateImageGet(pObject, pImage) \
            (((GFX_GOL_BUTTON *)pObject)->pReleaseImage)

// *****************************************************************************
/*  
    <GROUP gol_button_object>

    Function:
        GFX_XCHAR *GFX_GOL_ButtonTextGet(
                                GFX_GOL_BUTTON *pObject)

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
        <code>
            GFX_XCHAR *pChar;
            GFX_GOL_BUTTON GFX_GOL_BUTTON[2];

            pChar = GFX_GOL_ButtonTextGet(GFX_GOL_BUTTON[0]);
       </code>

*/
// *****************************************************************************
#define GFX_GOL_ButtonTextGet(pObject)  (((GFX_GOL_BUTTON *)pObject)->pText)

// *****************************************************************************
/*  
    <GROUP gol_button_object>

    Function:
        GFX_XCHAR *GFX_GOL_ButtonTextSet(
                                GFX_GOL_BUTTON *pObject,
                                GFX_XCHAR *pText)

    Summary:
        This function sets the address of the current text
        string used by the object.

    Description:
        This function sets the address of the current text
        string used by the object.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.
        pText - pointer to the text string to be used.

    Returns:
        None.

    Example:
        <code>

           GFX_XCHAR Label0[] = “ON”;
           GFX_XCHAR Label1[] = “OFF”;
           GFX_GOL_BUTTON GFX_GOL_BUTTON[2];

           GFX_GOL_ButtonTextSet(GFX_GOL_BUTTON[0], Label0);
           GFX_GOL_ButtonTextSet(GFX_GOL_BUTTON[1], Label1);

       </code>

*/
// *****************************************************************************
void GFX_GOL_ButtonTextSet(GFX_GOL_BUTTON *pObject, GFX_XCHAR * pText);

// *****************************************************************************
/*  
    <GROUP gol_button_object>

    Function:
        GFX_ALIGNMENT GFX_GOL_ButtonTextAlignmentGet(
                                GFX_GOL_BUTTON *pObject)

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
GFX_ALIGNMENT GFX_GOL_ButtonTextAlignmentGet(GFX_GOL_BUTTON *pObject);

// *****************************************************************************
/*  
    <GROUP gol_button_object>

    Function:
        void GFX_GOL_ButtonTextAlignmentSet(
                                GFX_GOL_BUTTON *pObject,
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
void GFX_GOL_ButtonTextAlignmentSet(
                                GFX_GOL_BUTTON *pObject,
                                GFX_ALIGNMENT align);

// *****************************************************************************
/*  
    <GROUP gol_button_object>

    Function:
        GFX_GOL_TRANSLATED_ACTION GFX_GOL_ButtonActionGet(
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
     	GFX_GOL_BUTTON_ACTION_PRESSED       Touch Screen    EVENT_PRESS, EVENT_MOVE     If events occurs and the x,y position falls in the face of the GFX_GOL_BUTTON while the GFX_GOL_BUTTON is not pressed.
                                            Keyboard        EVENT_KEYSCAN               If event occurs and parameter1 passed matches the object's ID and parameter 2 passed matches SCAN_CR_PRESSED or SCAN_SPACE_PRESSED while the GFX_GOL_BUTTON is not pressed.
        GFX_GOL_BUTTON_ACTION_STILLPRESSED  Touch Screen    EVENT_STILLPRESS            If event occurs and the x,y position does not change from the previous press position in the face of the GFX_GOL_BUTTON.
        GFX_GOL_BUTTON_ACTION_RELEASED      Touch Screen    EVENT_RELEASE               If the event occurs and the x,y position falls in the face of the GFX_GOL_BUTTON while the GFX_GOL_BUTTON is pressed.
                                            Keyboard        EVENT_KEYSCAN               If event occurs and parameter1 passed matches the object's ID and parameter 2 passed matches SCAN_CR_RELEASED or SCAN_SPACE_RELEASED while the GFX_GOL_BUTTON is pressed.
        GFX_GOL_BUTTON_ACTION_CANCELPRESS   Touch Screen    EVENT_MOVE                  If the event occurs outside the face of the GFX_GOL_BUTTON and the GFX_GOL_BUTTON is currently pressed.
        GFX_GOL_OBJECT_ACTION_INVALID       Any             Any                         If the message did not affect the object.
	</TABLE> 

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - The pointer to the object where the message will be
                   evaluated to check if the message will affect the object.
        pMessage - Pointer to the the message from the user interface.
 
    Returns:
        - GFX_GOL_BUTTON_ACTION_PRESSED - Object is pressed
        - GFX_GOL_BUTTON_ACTION_RELEASED - Object is released
        - GFX_GOL_BUTTON_ACTION_CANCELPRESS - Object will be released, 
                                              user cancels press action
                                              on the GFX_GOL_BUTTON
        - GFX_GOL_OBJECT_ACTION_INVALID - Object is not affected
 
    Example:
        None.
        
*/
// *****************************************************************************
GFX_GOL_TRANSLATED_ACTION GFX_GOL_ButtonActionGet(
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage);

// *****************************************************************************
/*  
    <GROUP gol_button_object>

    Function:
        void GFX_GOL_ButtonActionSet(
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
     	##################                  ############	##################################  ############################################
     	GFX_GOL_BUTTON_ACTION_PRESSED       Touch Screen,	Set GFX_GOL_BUTTON_PRESSED_STATE    Button will be redrawn in the pressed state.
                                            Keyboard
        GFX_GOL_BUTTON_ACTION_RELEASED      Touch Screen,	Clear GFX_GOL_BUTTON_PRESSED_STATE  Button will be redrawn in the released state.
                                            Keyboard
        GFX_GOL_BUTTON_ACTION_CANCELPRESS   Touch Screen,	Clear GFX_GOL_BUTTON_PRESSED_STATE  Button will be redrawn in the released state.
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
void GFX_GOL_ButtonActionSet(
                                GFX_GOL_TRANSLATED_ACTION translatedMsg,
                                void *pObject, 
                                GFX_GOL_MESSAGE *pMessage);

// *****************************************************************************
/*  
    <GROUP gol_button_object>

    Function:
        GFX_STATUS GFX_GOL_ButtonDraw(void *pObject)

    Summary:
        This function renders the object on the screen based on the
        current state of the object.

    Description:
        This function renders the object on the screen based on the
        current state of the object. Location of the object is
        determined by the left, top, right and bottom parameters.
        The colors used are dependent on the state of the object.
        The font used is determined by the style scheme set.

        The text on the face of the GFX_GOL_BUTTON is drawn on top of
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
GFX_STATUS GFX_GOL_ButtonDraw(void *pObject);

#endif // _GFX_GOL_BUTTON_H
