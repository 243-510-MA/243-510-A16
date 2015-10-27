/*******************************************************************************
 Module for Microchip Graphics Library - Graphic Object Layer

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_gol_text_entry.c

  Summary:
    This implements the text entry object of the GOL.

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

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "gfx/gfx_gol_text_entry.h"

// *****************************************************************************
/*  Function:
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
                                GFX_GOL_OBJ_SCHEME  *pScheme)
{
    GFX_GOL_TEXTENTRY   *pTe = NULL;                    //Text entry
    pTe = (GFX_GOL_TEXTENTRY *)GFX_malloc(sizeof(GFX_GOL_TEXTENTRY));
    if(pTe == NULL)
        return (NULL);

    pTe->hdr.ID         = ID;
    pTe->hdr.pNxtObj    = NULL;
    pTe->hdr.type       = GFX_GOL_TEXTENTRY_TYPE;   // set object type
    pTe->hdr.left       = left;                     // left parameter of the text-entry
    pTe->hdr.top        = top;                      // top parameter of the text-entry
    pTe->hdr.right      = right;                    // right parameter of the text-entry
    pTe->hdr.bottom     = bottom;                   // bottom parameter of the text-entry
    pTe->hdr.state      = state;                    // State of the Text-Entry
    pTe->horizontalKeys = horizontalKeys;           // number of horizontal keys
    pTe->verticalKeys   = verticalKeys;             // number of vertical keys
    pTe->CurrentLength  = 0;                        // current length of text
    pTe->pHeadOfList    = NULL;

    // set the text to be displayed buffer length is
    // also initialized in this call
    GFX_GOL_TextEntryBufferSet(pTe, pBuffer, bufferLength);
    pTe->alignment      = alignment;

    pTe->pActiveKey     = NULL;

    // draw function
    pTe->hdr.DrawObj    = GFX_GOL_TextEntryDraw;
    // action get function
    pTe->hdr.actionGet  = GFX_GOL_TextEntryActionGet;
    // default action function
    pTe->hdr.actionSet  = GFX_GOL_TextEntryActionSet;
    // free function
    pTe->hdr.FreeObj    = GFX_GOL_TextEntryKeyMemberListDelete;

    // Set the color scheme to be used
    pTe->hdr.pGolScheme = (GFX_GOL_OBJ_SCHEME *)pScheme;

    // Set the font to be used (if this is NULL text using pDisplayFont
    // will not show up
    pTe->pDisplayFont = pDisplayFont;

    //check if either values of horizontal keys and vertical keys are equal to zero
    if((pTe->horizontalKeys != 0) || (pTe->verticalKeys != 0))
    {
        //create the key members, return null if not successful
        if(GFX_GOL_TextEntryKeyListCreate(pTe, pText) == NULL)
        {
            GFX_GOL_TextEntryKeyMemberListDelete(pTe);
            GFX_free(pTe);
            return (NULL);
        }
    }

    //Add this new widget object to the GOL list	
    GFX_GOL_ObjectAdd((GFX_GOL_OBJ_HEADER *)pTe);

    return (pTe);

}   

// *****************************************************************************
/*  Function:
    void GFX_GOL_TextEntryBufferClear(
                                GFX_GOL_TEXTENTRY *pObject)

    Summary:
        This function will clear the data in the display.

    Description:
        This function will clear the data in the display. Object must
        be redrawn to reflect the change in the buffer. Use the
        drawing state bit GFX_GOL_TEXTENTRY_UPDATE_TEXT_STATE to update
        the text on the screen.
    
*/
// *****************************************************************************
void GFX_GOL_TextEntryBufferClear(GFX_GOL_TEXTENTRY *pObject)
{
    uint16_t    i;

    //clear the buffer
    for(i = 0; i < (pObject->outputLenMax); i++)
    {
        *(pObject->pTeOutput + i) = 0;
    }

    pObject->CurrentLength = 0;
}

// *****************************************************************************
/*  Function:
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
        to MaxSize+1. The +1 is used for the string terminator.

*/
// *****************************************************************************
void GFX_GOL_TextEntryBufferSet(
                                GFX_GOL_TEXTENTRY *pObject,
                                GFX_XCHAR *pText,
                                uint16_t MaxSize)
{
    uint16_t    count = 0;
    GFX_XCHAR   *pTemp;

    pTemp = pText;

    while(*pTemp != 0)
    {
        if(count > MaxSize)
            break;
        pTemp++;
        count++;
    }

    // terminate the string
    *pTemp = 0;

    pObject->CurrentLength = count;
    pObject->outputLenMax = MaxSize;
    pObject->pTeOutput = pText;
}

// *****************************************************************************
/*  Function:
    GFX_XCHAR *GFX_GOL_TextEntryBufferGet(
                                GFX_GOL_TEXTENTRY *pObject)

    Summary:
        This function returns the buffer used to display text.

    Description:
        This function returns the buffer used to display text.

*/
// *****************************************************************************
GFX_XCHAR __attribute__ ((always_inline)) *GFX_GOL_TextEntryBufferGet(
                                GFX_GOL_TEXTENTRY *pObject)
{
    return (pObject->pTeOutput);
}


// *****************************************************************************
/*  Function:
    bool GFX_GOL_TextEntryKeyIsPressed(
                                GFX_GOL_TEXTENTRY *pObject,
                                uint16_t index)

    Summary:
        This function will test if a key given by its index in the
        object is pressed.

    Description:
        This function will test if a key given by its index in the
        object is pressed.

*/
// *****************************************************************************
bool GFX_GOL_TextEntryKeyIsPressed(GFX_GOL_TEXTENTRY *pObject, uint16_t index)
{
    GFX_GOL_TEXTENTRY_KEYMEMBER *pTemp;

    pTemp = pObject->pHeadOfList;

    //search the key using the given index
    while(index != pTemp->index)
    {
        // catch all check
        if(pTemp == NULL)
            return (false);
        pTemp = pTemp->pNextKey;
    }

    if(pTemp->state == GFX_GOL_TEXTENTRY_KEY_PRESSED_STATE)
    {
        return (true);
    }
    else
    {
        return (false);
    }
}

// *****************************************************************************
/*  Function:
    bool GFX_GOL_TextEntryKeyCommandSet(
                                GFX_GOL_TEXTENTRY *pObject,
                                uint16_t index,
                                GFX_GOL_TEXTENTRY_KEY_COMMAND_TYPE command)
    Summary:
        This function will assign a command to a key with the given index.

    Description:
        This function will assign a command to a key with the given index.
        (See GFX_GOL_TEXTENTRY_KEY_COMMAND_TYPE)

*/
// *****************************************************************************
bool GFX_GOL_TextEntryKeyCommandSet(
                                GFX_GOL_TEXTENTRY *pObject,
                                uint16_t index,
                                GFX_GOL_TEXTENTRY_KEY_COMMAND_TYPE command)
{
    GFX_GOL_TEXTENTRY_KEYMEMBER *pTemp;

    pTemp = pObject->pHeadOfList;

    //search the key using the given index
    while(index != pTemp->index)
    {
        // catch all check
        if(pTemp == NULL)
            return (false);
        pTemp = pTemp->pNextKey;
    }

    pTemp->command = command;
    return (true);
}

// *****************************************************************************
/*  Function:
    bool GFX_GOL_TextEntryKeyCommandSet(
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
                                uint16_t index)
{
    GFX_GOL_TEXTENTRY_KEYMEMBER *pTemp;

    pTemp = pObject->pHeadOfList;

    //search the key using the given index
    while(index != pTemp->index)
    {
        // catch all check
        if(pTemp == NULL)
            return (0);
        pTemp = pTemp->pNextKey;
    }

    return (pTemp->command);
}

// *****************************************************************************
/*  Function:
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
                                GFX_XCHAR *pText)
{
    GFX_GOL_TEXTENTRY_KEYMEMBER *pTemp;

    pTemp = pObject->pHeadOfList;

    //search the key using the given index
    while(index != pTemp->index)
    {
        // catch all check
        if(pTemp == NULL)
            return (false);
        pTemp = pTemp->pNextKey;
    }

	// Set the the text 
    pTemp->pKeyName = pText;

    return (true);
}

// *****************************************************************************
/*  Function:
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

*/
// *****************************************************************************
GFX_GOL_TEXTENTRY_KEYMEMBER *GFX_GOL_TextEntryKeyListCreate(
                                GFX_GOL_TEXTENTRY *pObject,
                                GFX_XCHAR *pText[])
{
    int16_t     NumberOfKeys, width, height;
    int16_t     keyTop, keyLeft;
    uint16_t    rowcount, colcount;
    uint16_t    index = 0;

    GFX_GOL_TEXTENTRY_KEYMEMBER   *pKl = NULL;    // this is a linked list
    GFX_GOL_TEXTENTRY_KEYMEMBER   *pTail = NULL;

    // determine starting positions of the keys
    keyTop = pObject->hdr.top +
             GFX_TextStringHeightGet(pObject->pDisplayFont) +
             (pObject->hdr.pGolScheme->EmbossSize << 1);
    keyLeft = pObject->hdr.left;

    //calculate the total number of keys, and width and height of each key
    NumberOfKeys = pObject->horizontalKeys * pObject->verticalKeys;
    width  = (pObject->hdr.right  - keyLeft + 1) / pObject->horizontalKeys;
    height = (pObject->hdr.bottom - keyTop  + 1) / pObject->verticalKeys;

    /* create the list and calculate the coordinates of each bottom,
     * and the textwidth/textheight of each font */

    //Add a list for each key
    for(colcount = 0; colcount < pObject->verticalKeys; colcount++)
    {
        for(rowcount = 0; rowcount < pObject->horizontalKeys; rowcount++)
        {

            //get storage for new entry
            pKl = (GFX_GOL_TEXTENTRY_KEYMEMBER *) \
                            GFX_malloc(sizeof(GFX_GOL_TEXTENTRY_KEYMEMBER));
            if(pKl == NULL)
            {
                GFX_GOL_TextEntryKeyMemberListDelete(pObject);
                return (NULL);
            }
            if(pObject->pHeadOfList == NULL)
                pObject->pHeadOfList = pKl;
            if(pTail == NULL)
            {
                pTail = pKl;
            }
            else
            {
                pTail->pNextKey = pKl;
                pTail = pTail->pNextKey;
            }

            //set the index for the new list
            pKl->index = index;

            // set update flag to off
            pKl->update = false;

            //calculate the x-y coordinate for each key
            pKl->left 	= keyLeft + (rowcount * width);
            pKl->top 	= keyTop  + (colcount * height);
            pKl->right 	= keyLeft + ((rowcount + 1) * width);
            pKl->bottom = keyTop  + ((colcount + 1) * height);

            // checks for dimensions exceeding screen area
            if (pKl->right  > GFX_MaxXGet())
                pKl->right  = GFX_MaxXGet();
            if (pKl->bottom > GFX_MaxYGet())
                pKl->bottom = GFX_MaxYGet();

            //Add the text to the list and increase the index
            pKl->pKeyName = pText[index++];

            //set the COMMAND to NULL for all keys
            pKl->command = GFX_GOL_TEXTENTRY_NONE_COM;

            //calculate the textwidth, textheight
            pKl->textWidth = 0;
            pKl->textHeight = 0;

            if(pKl->pKeyName != NULL)
            {
                // Calculate the text width & height
                pKl->textWidth  = GFX_TextStringWidthGet(pKl->pKeyName, pObject->hdr.pGolScheme->pFont);
                pKl->textHeight = GFX_TextStringHeightGet(pObject->hdr.pGolScheme->pFont);
            } //end if

        } //end for	
    } //end for	

    pTail->pNextKey = NULL;

    return (pKl);
}

// *****************************************************************************
/*  Function:
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
void GFX_GOL_TextEntryKeyMemberListDelete(void *pObject)
{
    GFX_GOL_TEXTENTRY_KEYMEMBER *pCurItem;
    GFX_GOL_TEXTENTRY_KEYMEMBER *pItem;
    GFX_GOL_TEXTENTRY           *pTe;

    pTe = (GFX_GOL_TEXTENTRY *)pObject;
    pCurItem = pTe->pHeadOfList;

    while(pCurItem != NULL)
    {
        pItem = pCurItem;
        pCurItem = pCurItem->pNextKey;
        GFX_free(pItem);
    }

    pTe->pHeadOfList = NULL;
}

// *****************************************************************************
/*  Function:
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

*/
// *****************************************************************************
void GFX_GOL_TextEntryLastCharDelete(GFX_GOL_TEXTENTRY *pObject)
{
    if (pObject->CurrentLength > 0)
    {
        pObject->CurrentLength -= 1;
        // set the last character to string terminator
        *(pObject->pTeOutput + pObject->CurrentLength) = 0;
    }

}

// *****************************************************************************
/*  Function:
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

*/
// *****************************************************************************
void GFX_GOL_TextEntrySpaceCharAdd(GFX_GOL_TEXTENTRY *pObject)
{
    //first determine if the array has not overflown
    if((pObject->CurrentLength) < (pObject->outputLenMax - 1))
    {
        *(pObject->pTeOutput + (pObject->CurrentLength)) = 0x20;
        *(pObject->pTeOutput + (pObject->CurrentLength) + 1) = 0;
        (pObject->CurrentLength)++;
    }   	
}

// *****************************************************************************
/*  Function:
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

*/
// *****************************************************************************
void GFX_GOL_TextEntryCharAdd(GFX_GOL_TEXTENTRY *pObject)
{
    GFX_XCHAR *pPoint;

    //first determine if the array has not overflown
    if ((pObject->CurrentLength) < (pObject->outputLenMax - 1))
    {
        pPoint = (pObject->pActiveKey)->pKeyName;
        while (*(pPoint) != 0)
        {
            *(pObject->pTeOutput + (pObject->CurrentLength)) = *(pPoint)++;
        }
        
        (pObject->CurrentLength)++;
        
        // add the string terminator 
        *(pObject->pTeOutput + pObject->CurrentLength) = 0;
    }
    else
    {
        // it is full ignore the added key
        return;
    }

}

// *****************************************************************************
/*  Function:
    GFX_GOL_TRANSLATED_ACTION GFX_GOL_TextEntryActionGet(
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage);

    Summary:
        This function evaluates the message from a user if the
        message will affect the object or not.

    Description:
        This function evaluates the message from a user if the
        message will affect the object or not. 
	
*/
// *****************************************************************************
GFX_GOL_TRANSLATED_ACTION GFX_GOL_TextEntryActionGet(
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage)
{
    int16_t                     NumberOfKeys, param1, param2;
    GFX_GOL_TEXTENTRY_KEYMEMBER *pKeyTemp = NULL;
    GFX_GOL_TEXTENTRY           *pTe;

    pTe = (GFX_GOL_TEXTENTRY *)pObject;

    // Check if disabled first
    if(GFX_GOL_ObjectStateGet(pTe, GFX_GOL_TEXTENTRY_DISABLED_STATE))
        return (GFX_GOL_OBJECT_ACTION_INVALID);

#ifndef GFX_CONFIG_USE_TOUCHSCREEN_DISABLE

    //find the total number of keys
    NumberOfKeys = (pTe->horizontalKeys) * (pTe->verticalKeys);
    param1 = pMessage->param1;
    param2 = pMessage->param2;

    if((pMessage->type == TYPE_TOUCHSCREEN))
    {

        // Check if it falls in the panel of the TextEntry
        if
        (
            (pTe->hdr.left   < pMessage->param1) &&
            (pTe->hdr.right  > pMessage->param1) &&
            (pTe->hdr.top + (GFX_TextStringHeightGet(pTe->pDisplayFont) + (pTe->hdr.pGolScheme->EmbossSize << 1)) < pMessage->param2) &&
            (pTe->hdr.bottom > pMessage->param2)
        )
        {

            /* If it fell inside the TextEntry panel, go through the link list and check which one was pressed
               At this point the touch screen event is either EVENT_MOVE or EVENT_PRESS.
            */

            //point to the head of the link list
            pKeyTemp = pTe->pHeadOfList;

            while(pKeyTemp != NULL)
            {
                if
                (
                    (pKeyTemp->left   < param1) &&
                    (pKeyTemp->right  > param1) &&
                    (pKeyTemp->top    < param2) &&
                    (pKeyTemp->bottom > param2)
                )
                {
                    if(pMessage->uiEvent == EVENT_RELEASE)
                    {
                        pTe->pActiveKey = pKeyTemp;
                        pKeyTemp->update = true;

                        if(pTe->pActiveKey->state == GFX_GOL_TEXTENTRY_KEY_PRESSED_STATE)
                        {
                            if(pKeyTemp->command == GFX_GOL_TEXTENTRY_NONE_COM)
                                return (GFX_GOL_TEXTENTRY_ACTION_ADD_CHAR);

                            //command for a GFX_GOL_TEXTENTRY_DELETE_COM key
                            if(pKeyTemp->command == GFX_GOL_TEXTENTRY_DELETE_COM)
                                return (GFX_GOL_TEXTENTRY_ACTION_DELETE);

                            //command for a GFX_GOL_TEXTENTRY_SPACE_COM key 0x20
                            if(pKeyTemp->command == GFX_GOL_TEXTENTRY_SPACE_COM)
                                return (GFX_GOL_TEXTENTRY_ACTION_SPACE);

                            //command for a GFX_GOL_TEXTENTRY_ENTER_COM key
                            if(pKeyTemp->command == GFX_GOL_TEXTENTRY_ENTER_COM)
                                return (GFX_GOL_TEXTENTRY_ACTION_ENTER);
                        }

                        // this is a catch all backup
                        return (GFX_GOL_TEXTENTRY_ACTION_RELEASED);
                    }
                    else
                    {

                        // to shift the press to another key make sure that there are no other
                        // keys currently pressed. If there is one it must be released first.
                        // check if there are previously pressed keys
                        if(GFX_GOL_ObjectStateGet(pTe, GFX_GOL_TEXTENTRY_KEY_PRESSED_STATE))
                        {

                            // there is a key being pressed.
                            if(pKeyTemp->index != pTe->pActiveKey->index)
                            {

                                // release the currently pressed key first
                                pTe->pActiveKey->update = true;
                                return (GFX_GOL_TEXTENTRY_ACTION_RELEASED);
                            }
                        }
                        else
                        {

                            // check if the active key is not pressed
                            // if not, set to press since the current touch event
                            // is either move or press
                            // check if there is an active key already set
                            // if none, set the current key as active and return a pressed mesage
                            if(pTe->pActiveKey == NULL)
                            {
                                pTe->pActiveKey = pKeyTemp;
                                pKeyTemp->update = true;
                                return (GFX_GOL_TEXTENTRY_ACTION_PRESSED);
                            }

                            if(pTe->pActiveKey->state != GFX_GOL_TEXTENTRY_KEY_PRESSED_STATE)
                            {
                                pTe->pActiveKey = pKeyTemp;
                                pKeyTemp->update = true;
                                return (GFX_GOL_TEXTENTRY_ACTION_PRESSED);
                            }
                            else
                            {
                                return (GFX_GOL_OBJECT_ACTION_INVALID);
                            }
                        }
                    }
                }
                else
                {

                    // if the key is in the pressed state and current touch is not here
                    // then it has to be redrawn
                    if(pKeyTemp->state == GFX_GOL_TEXTENTRY_KEY_PRESSED_STATE)
                    {
                        pTe->pActiveKey = pKeyTemp;
                        pKeyTemp->update = true;
                        return (GFX_GOL_TEXTENTRY_ACTION_RELEASED);
                    }
                }

                //access the next link list
                pKeyTemp = pKeyTemp->pNextKey;
            }   //end while
        }
        else
        {
            if((pMessage->uiEvent == EVENT_MOVE) &&
               (GFX_GOL_ObjectStateGet(pTe, GFX_GOL_TEXTENTRY_KEY_PRESSED_STATE)))
            {
                pTe->pActiveKey->update = true;
                return (GFX_GOL_TEXTENTRY_ACTION_RELEASED);
            }
        }
    }

    return (GFX_GOL_OBJECT_ACTION_INVALID);
#endif // GFX_CONFIG_USE_TOUCHSCREEN_DISABLE
}

// *****************************************************************************
/*  Function:
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
        by on the GFX_GOL_ObjectMessageCallback(). When the user message
        is determined to affect the object, application can perform the
        state change and return 0 on the GFX_GOL_ObjectMessageCallback().
        	
*/
// *****************************************************************************
void GFX_GOL_TextEntryActionSet(
                                uint16_t translatedMsg,
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage)
{
    GFX_GOL_TEXTENTRY *pTe;

    pTe = (GFX_GOL_TEXTENTRY *)pObject;

    switch(translatedMsg)
    {
        case GFX_GOL_TEXTENTRY_ACTION_DELETE:
            GFX_GOL_TextEntryLastCharDelete(pTe);
            GFX_GOL_ObjectStateSet(pTe, GFX_GOL_TEXTENTRY_UPDATE_KEY_STATE |
                                        GFX_GOL_TEXTENTRY_UPDATE_TEXT_STATE);
            break;

        case GFX_GOL_TEXTENTRY_ACTION_SPACE:
            GFX_GOL_TextEntrySpaceCharAdd(pTe);
            GFX_GOL_ObjectStateSet(pTe, GFX_GOL_TEXTENTRY_UPDATE_KEY_STATE |
                                        GFX_GOL_TEXTENTRY_UPDATE_TEXT_STATE);
            break;

        case GFX_GOL_TEXTENTRY_ACTION_ENTER:
            GFX_GOL_ObjectStateSet(pTe, GFX_GOL_TEXTENTRY_UPDATE_KEY_STATE);
            break;

        case GFX_GOL_TEXTENTRY_ACTION_ADD_CHAR:
            GFX_GOL_TextEntryCharAdd(pTe);
            GFX_GOL_ObjectStateSet(pTe, GFX_GOL_TEXTENTRY_UPDATE_KEY_STATE |
                                        GFX_GOL_TEXTENTRY_UPDATE_TEXT_STATE);
            break;

        case GFX_GOL_TEXTENTRY_ACTION_PRESSED:
            (pTe->pActiveKey)->state = GFX_GOL_TEXTENTRY_KEY_PRESSED_STATE;
            GFX_GOL_ObjectStateSet(pTe, GFX_GOL_TEXTENTRY_KEY_PRESSED_STATE |
                                        GFX_GOL_TEXTENTRY_UPDATE_KEY_STATE);
            return;

        case GFX_GOL_TEXTENTRY_ACTION_RELEASED:
            (pTe->pActiveKey)->state = 0;
            // reset pressed
            GFX_GOL_ObjectStateClear(pTe, GFX_GOL_TEXTENTRY_KEY_PRESSED_STATE);
            // redraw
            GFX_GOL_ObjectStateSet(pTe, GFX_GOL_TEXTENTRY_UPDATE_KEY_STATE);
            return;
        default:
            // do nothing
            return;
    }

    if(pTe->pActiveKey != NULL)
        (pTe->pActiveKey)->state = 0;
    GFX_GOL_ObjectStateClear(pTe, GFX_GOL_TEXTENTRY_KEY_PRESSED_STATE);
}

// *****************************************************************************
/*  Function:
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
GFX_STATUS GFX_GOL_TextEntryDraw(void *pObject)
{
    static GFX_GOL_TEXTENTRY_KEYMEMBER    *pKeyTemp = NULL;
    static GFX_COLOR    faceClr, embossLtClr, embossDkClr;
    static GFX_XCHAR    XcharTmp;

    static uint16_t     CountOfKeys = 0;
    static uint16_t     counter = 0, embossSize;
    static GFX_XCHAR    hideChar[2] = {0x2A, 0x00};

    static GFX_COLOR    color1, color2;
           uint16_t     leftPos, textWidth;

    typedef enum
    {
        DRAW_TEXTENTRY_START,
        DRAW_TEXTENTRY_HIDE,
        DRAW_TEXTENTRY_PANEL,
        DRAW_TEXTENTRY_EDITBOX_INIT,
        DRAW_TEXTENTRY_EDITBOX,
        DRAW_TEXTENTRY_KEY_INIT,
        DRAW_TEXTENTRY_KEY_PANEL_SET,
        DRAW_TEXTENTRY_KEY_PANEL_DRAW,
        DRAW_TEXTENTRY_KEY_TEXT,
        DRAW_TEXTENTRY_KEY_UPDATE,
        DRAW_TEXTENTRY_STRING_INIT,
        DRAW_TEXTENTRY_STRING_UPDATE_PREPARE,
        DRAW_TEXTENTRY_STRING_UPDATE,
        DRAW_TEXTENTRY_EDITBOX_CLEAR,
        DRAW_TEXTENTRY_EDITBOX_CHAR_UPDATE,
    } TE_DRAW_STATES;

    static TE_DRAW_STATES state = DRAW_TEXTENTRY_START;
    GFX_GOL_TEXTENTRY *pTe;

    pTe = (GFX_GOL_TEXTENTRY *)pObject;

    while(1)
    {
        if (GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
            return (GFX_STATUS_FAILURE);

	switch(state)
	{
            case DRAW_TEXTENTRY_START:
                if(GFX_GOL_ObjectStateGet(pTe, GFX_GOL_TEXTENTRY_HIDE_STATE))
                {
                        GFX_ColorSet(pTe->hdr.pGolScheme->CommonBkColor);
                        state = DRAW_TEXTENTRY_HIDE;
                        // no break here so it falls through
                        // to the DRAW_TEXTENTRY_HIDE state.
                }
                else
                {
                    if(GFX_GOL_ObjectStateGet(
                            pTe,
                            ((GFX_GOL_TEXTENTRY_UPDATE_KEY_STATE  |
                            GFX_GOL_TEXTENTRY_UPDATE_TEXT_STATE) |
                            GFX_GOL_TEXTENTRY_DRAW_STATE)))
                    {
#ifndef GFX_CONFIG_ALPHABLEND_DISABLE
                        GFX_GOL_PanelAlphaParameterSet(pTe->hdr.pGolScheme->AlphaValue);
#endif
                        // set the background information
                        GFX_BackgroundSet(
                                pTe->hdr.pGolScheme->CommonBkLeft,
                                pTe->hdr.pGolScheme->CommonBkTop,
                                pTe->hdr.pGolScheme->pCommonBkImage,
                                pTe->hdr.pGolScheme->CommonBkColor);
                        GFX_BackgroundTypeSet(pTe->hdr.pGolScheme->CommonBkType);

                        if(GFX_GOL_ObjectStateGet(pTe, GFX_GOL_TEXTENTRY_DRAW_STATE))
                        {
                            if (TE_ROUNDEDBUTTON_RADIUS == 0)
                            {
                                color1 = pTe->hdr.pGolScheme->EmbossDkColor;
                                color2 = pTe->hdr.pGolScheme->EmbossLtColor;
                            }
                            else
                            {
                                color1 = pTe->hdr.pGolScheme->Color0;
                                color2 = pTe->hdr.pGolScheme->Color0;
                            }
                            embossSize = pTe->hdr.pGolScheme->EmbossSize;

                            GFX_LineStyleSet(GFX_LINE_STYLE_THIN_SOLID);

                            /************DRAW THE WIDGET PANEL******************/
                            GFX_GOL_PanelParameterSet
                            (
                                pTe->hdr.left,
                                pTe->hdr.top,
                                pTe->hdr.right,
                                pTe->hdr.bottom,
                                0,
                                pTe->hdr.pGolScheme->Color0, //face color of panel
                                color1,                      //emboss dark color
                                color2,                      //emboss light color
                                NULL,
                                pTe->hdr.pGolScheme->fillStyle,
                                embossSize
                            );
                            state = DRAW_TEXTENTRY_PANEL;
                            break;
                        }

                        // update the keys (if TE_UPDATE_TEXT is also
                        // set it will also be redrawn)
                        // at the states after the keys are updated
                        else if(GFX_GOL_ObjectStateGet(
                                            pTe,
                                            GFX_GOL_TEXTENTRY_UPDATE_KEY_STATE))
                        {
                            state = DRAW_TEXTENTRY_KEY_INIT;
                            break;
                        }

                        // check if updating only the text displayed
                        else if(GFX_GOL_ObjectStateGet(
                                            pTe,
                                            GFX_GOL_TEXTENTRY_UPDATE_TEXT_STATE))
                        {
                            state = DRAW_TEXTENTRY_STRING_INIT;
                            break;
                        }
                    }
                    return (GFX_STATUS_SUCCESS);
                }

            /*hide the widget*/
            case DRAW_TEXTENTRY_HIDE:

                // Hide the GFX_GOL_TEXTENTRY (remove from screen)
                if (GFX_GOL_ObjectHideDraw(&(pTe->hdr)) != GFX_STATUS_SUCCESS)
                    return (GFX_STATUS_FAILURE);

                state = DRAW_TEXTENTRY_START;
                return (GFX_STATUS_SUCCESS);

            /*Draw the widget of the Text-Entry*/
            case DRAW_TEXTENTRY_PANEL:
                if(GFX_GOL_PanelDraw() != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }
                state = DRAW_TEXTENTRY_EDITBOX_INIT;

            case DRAW_TEXTENTRY_EDITBOX_INIT:

                //Draw the editbox
                GFX_GOL_PanelParameterSet
                (
                        pTe->hdr.left,
                        pTe->hdr.top,
                        pTe->hdr.right,
                        pTe->hdr.top +
                            GFX_TextStringHeightGet(pTe->pDisplayFont) +
                            embossSize,
                        0,
                        pTe->hdr.pGolScheme->Color1,
                        pTe->hdr.pGolScheme->EmbossDkColor,
                        pTe->hdr.pGolScheme->EmbossLtColor,
                        NULL,
                        pTe->hdr.pGolScheme->fillStyle,
                        embossSize
                );

                state = DRAW_TEXTENTRY_EDITBOX;

            case DRAW_TEXTENTRY_EDITBOX:
                if(GFX_GOL_PanelDraw() != GFX_STATUS_SUCCESS)
                    return (GFX_STATUS_FAILURE);
                state = DRAW_TEXTENTRY_KEY_INIT;

            /* ********************************************************** */
            /*                  Update the keys                           */
            /* ********************************************************** */
            case DRAW_TEXTENTRY_KEY_INIT:
                embossLtClr = pTe->hdr.pGolScheme->EmbossLtColor;
                embossDkClr = pTe->hdr.pGolScheme->EmbossDkColor;
                faceClr = pTe->hdr.pGolScheme->Color0;

                // if the active key update flag is set,
                // only one needs to be redrawn
                if((GFX_GOL_ObjectStateGet(
                                    pTe,
                                    GFX_GOL_TEXTENTRY_DRAW_STATE) !=
                        GFX_GOL_TEXTENTRY_DRAW_STATE) &&
                   (pTe->pActiveKey->update == true))
                {
                    CountOfKeys = (pTe->horizontalKeys * pTe->verticalKeys) - 1;
                    pKeyTemp = pTe->pActiveKey;
                }
                else
                {
                    CountOfKeys = 0;
                    pKeyTemp = pTe->pHeadOfList;
                }
                state = DRAW_TEXTENTRY_KEY_PANEL_SET;

            case DRAW_TEXTENTRY_KEY_PANEL_SET:
                if(CountOfKeys < (pTe->horizontalKeys * pTe->verticalKeys))
                {
                    // check if we need to draw the panel
                    if(GFX_GOL_ObjectStateGet(
                                        pTe,
                                        GFX_GOL_TEXTENTRY_DRAW_STATE) !=
                       GFX_GOL_TEXTENTRY_DRAW_STATE)
                     {
                        if(pKeyTemp->update == true)
                        {
                            // set the colors needed
                            if(GFX_GOL_ObjectStateGet(
                                pTe,
                                GFX_GOL_TEXTENTRY_KEY_PRESSED_STATE))
                            {
                                embossLtClr = pTe->hdr.pGolScheme->EmbossDkColor;
                                embossDkClr = pTe->hdr.pGolScheme->EmbossLtColor;
                                faceClr = pTe->hdr.pGolScheme->Color1;
                            }
                            else
                            {
                                embossLtClr = pTe->hdr.pGolScheme->EmbossLtColor;
                                embossDkClr = pTe->hdr.pGolScheme->EmbossDkColor;
                                faceClr = pTe->hdr.pGolScheme->Color0;
                            }
                        }
                        else
                        {
                            state = DRAW_TEXTENTRY_KEY_UPDATE;
                            break;
                        }
                    }

                    if(GFX_GOL_ObjectStateGet(
                        pTe,
                        GFX_GOL_TEXTENTRY_DISABLED_STATE))
                    {
                        faceClr = pTe->hdr.pGolScheme->ColorDisabled;
                    }

#ifndef GFX_CONFIG_GRADIENT_DISABLE
                    // set the gradient parameters
                    GFX_GOL_PanelGradientParameterSet(
                        pTe->hdr.pGolScheme->gradientStartColor,
                        pTe->hdr.pGolScheme->gradientEndColor);
#endif

                    // set up the panel
                    GFX_GOL_PanelParameterSet
                    (
                        pKeyTemp->left   + TE_ROUNDEDBUTTON_RADIUS,
                        pKeyTemp->top    + TE_ROUNDEDBUTTON_RADIUS,
                        pKeyTemp->right  - TE_ROUNDEDBUTTON_RADIUS,
                        pKeyTemp->bottom - TE_ROUNDEDBUTTON_RADIUS,
                        TE_ROUNDEDBUTTON_RADIUS,
                        faceClr,
                        embossLtClr,
                        embossDkClr,
                        NULL,
                        pTe->hdr.pGolScheme->fillStyle,
                        embossSize
                    );

                    state = DRAW_TEXTENTRY_KEY_PANEL_DRAW;
                }
                else
                {
                    state = DRAW_TEXTENTRY_STRING_INIT;
                    break;
                }

            case DRAW_TEXTENTRY_KEY_PANEL_DRAW:
                if(GFX_GOL_PanelDraw() != GFX_STATUS_SUCCESS)
                    return (GFX_STATUS_FAILURE);

                // reset the update flag since the key panel is already redrawn
                pKeyTemp->update = false;

                //set color of text
                // if the object is disabled, draw the disabled colors
                if(GFX_GOL_ObjectStateGet(
                                    pTe,
                                    GFX_GOL_TEXTENTRY_DISABLED_STATE))
                {
                    GFX_ColorSet(pTe->hdr.pGolScheme->TextColorDisabled);
                }
                else
                {
                    if( (GFX_GOL_ObjectStateGet(
                            pTe,
                            GFX_GOL_TEXTENTRY_DRAW_STATE) !=
                            GFX_GOL_TEXTENTRY_DRAW_STATE
                        )
                        &&
                        (GFX_GOL_ObjectStateGet(
                            pTe,
                            GFX_GOL_TEXTENTRY_KEY_PRESSED_STATE)
                        )
                       == GFX_GOL_TEXTENTRY_KEY_PRESSED_STATE)
                    {
                        GFX_ColorSet(pTe->hdr.pGolScheme->TextColor1);
                    }
                    else
                    {
                        GFX_ColorSet(pTe->hdr.pGolScheme->TextColor0);
                    }
                }

                // set the font to be used
                GFX_FontSet(pTe->hdr.pGolScheme->pFont);

                state = DRAW_TEXTENTRY_KEY_TEXT;

            case DRAW_TEXTENTRY_KEY_TEXT:

                if (GFX_TextStringBoxDraw(
                    pKeyTemp->left,
                    pKeyTemp->top,
                    pKeyTemp->right - pKeyTemp->left + 1,
                    pKeyTemp->bottom - pKeyTemp->top + 1,
                    pKeyTemp->pKeyName,
                    0,
                    GFX_ALIGN_CENTER) != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }

                state = DRAW_TEXTENTRY_KEY_UPDATE;

            case DRAW_TEXTENTRY_KEY_UPDATE:

                // update loop variables
                CountOfKeys++;
                pKeyTemp = pKeyTemp->pNextKey;

                state = DRAW_TEXTENTRY_KEY_PANEL_SET;
                break;

            /* ********************************************************** */
            /*                  Update the displayed string               */
            /* ********************************************************** */
            case DRAW_TEXTENTRY_STRING_INIT:

                // check if text indeed needs to be updated
                if (!GFX_GOL_ObjectStateGet(pTe, GFX_GOL_TEXTENTRY_DRAW_STATE))
                {
                    // if the max length is reached, no need to update.
                    // if the state does not indicate update of text then
                    // no need to update.
                    if((pTe->CurrentLength == pTe->outputLenMax) ||
                       (!GFX_GOL_ObjectStateGet(
                            pTe,
                            GFX_GOL_TEXTENTRY_UPDATE_TEXT_STATE)
                       ))
                    {
                        state = DRAW_TEXTENTRY_START;
                        return (GFX_STATUS_SUCCESS);
                    }
                }

                // set the clipping region
                // set the clipping area first
                GFX_TextAreaLeftSet(  pTe->hdr.left  + embossSize);
                GFX_TextAreaTopSet(   pTe->hdr.top   + embossSize);
                GFX_TextAreaRightSet( pTe->hdr.right - embossSize);
                GFX_TextAreaBottomSet(pTe->hdr.top   +
                        embossSize +
                        GFX_TextStringHeightGet(pTe->pDisplayFont));

                if(GFX_GOL_ObjectStateGet(pTe, GFX_GOL_TEXTENTRY_DRAW_STATE))
                {
                    counter = 0;
                    state = DRAW_TEXTENTRY_STRING_UPDATE_PREPARE; //DRAW_TEXTENTRY_STRING_UPDATE;
                    break;
                }
                else if(GFX_GOL_ObjectStateGet(
                                pTe,
                                GFX_GOL_TEXTENTRY_UPDATE_TEXT_STATE))
                {

                    // erase the current text by drawing a bar
                    // over the edit box area
                    GFX_ColorSet(pTe->hdr.pGolScheme->Color1);
                    GFX_GOL_ObjectBackGroundSet(&pTe->hdr);

                    // we have to make sure we finish the bar draw
                    // first before we continue.
                    state = DRAW_TEXTENTRY_EDITBOX_CLEAR;
                    // no break here since the next state is what we want
                }
                else
                {
                    state = DRAW_TEXTENTRY_START;
                    return (GFX_STATUS_SUCCESS);
                }

            case DRAW_TEXTENTRY_EDITBOX_CLEAR:

                if (GFX_RectangleFillDraw(
                    pTe->hdr.left  + embossSize,
                    pTe->hdr.top   + embossSize,
                    pTe->hdr.right - embossSize,
                    pTe->hdr.top   +
                        embossSize +
                        GFX_TextStringHeightGet(pTe->pDisplayFont))
                    != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }
                state = DRAW_TEXTENTRY_STRING_UPDATE_PREPARE;

            case DRAW_TEXTENTRY_STRING_UPDATE_PREPARE:

                // position the cursor to the start of string rendering
                // notice that we need to remove the characters first
                // before we position the cursor when
                // deleting characters

                if(GFX_GOL_ObjectStateGet(
                                    pTe,
                                    GFX_GOL_TEXTENTRY_ECHO_HIDE_STATE))
                {
                    textWidth = GFX_TextStringWidthGet(
                                    hideChar,
                                    pTe->pDisplayFont) * (pTe->CurrentLength);
                }
                else
                {
                    textWidth = GFX_TextStringWidthGet(
                                    pTe->pTeOutput,
                                    pTe->pDisplayFont);
                }

                // position the text (due to alignment)
                switch(pTe->alignment & GFX_ALIGN_HORIZONTAL_MASK)
                {
                    case GFX_ALIGN_LEFT:
                        // text is aligned left, render the first character
                        // immediately after the left location
                        leftPos = pTe->hdr.left + 5 + embossSize;
                        break;
                    case GFX_ALIGN_RIGHT:
                        // text is aligned right, starting position of the
                        // first character is calculated from the right position
                        // subtracted by the width of the string
                        leftPos = (int16_t)(pTe->hdr.right - 4 - embossSize - textWidth);
                        break;
                    case GFX_ALIGN_HCENTER:
                    default:
                        // use GFX_TEXT_ALIGN_HCENTER as default when not set

                        // text is aligned horizontal center, starting position of
                        // the first character is dependent on the width of the string
                        // and the width of the box.
                        leftPos = (int16_t)(pTe->hdr.left +
                                            ((pTe->hdr.right -
                                                 pTe->hdr.left) >> 1) -
                                            (textWidth >> 1));
                        break;
                }

                GFX_TextCursorPositionSet(leftPos, pTe->hdr.top + embossSize);

                counter = 0;
                state = DRAW_TEXTENTRY_STRING_UPDATE;
                // add a break here to force a check if the last bar
                // rendered is still ongoing it will wait for it to finish.
                break;

            case DRAW_TEXTENTRY_STRING_UPDATE:

                //output the text
                GFX_ColorSet(pTe->hdr.pGolScheme->TextColor1);
                GFX_FontSet(pTe->pDisplayFont);

                // this is manually doing the GFX_TextStringDraw() function but with
                // the capability to replace the characters to the
                // * character when hide echo is enabled.
                XcharTmp = *((pTe->pTeOutput) + counter);
                if(XcharTmp < (GFX_XCHAR)15)
                {

                    // update is done time to return to start
                    // and exit with success
                    state = DRAW_TEXTENTRY_START;
                    return (GFX_STATUS_SUCCESS);
                }
                else
                {
                    if(GFX_GOL_ObjectStateGet(
                                    pTe,
                                    GFX_GOL_TEXTENTRY_ECHO_HIDE_STATE))
                        while(GFX_TextCharDraw(0x2A) != GFX_STATUS_SUCCESS);
                    else
                        while(GFX_TextCharDraw(XcharTmp) != GFX_STATUS_SUCCESS);
                    state = DRAW_TEXTENTRY_EDITBOX_CHAR_UPDATE;
                }

            case DRAW_TEXTENTRY_EDITBOX_CHAR_UPDATE:

                if (GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
                    return (GFX_STATUS_FAILURE);
                counter++;
                state = DRAW_TEXTENTRY_STRING_UPDATE;
                break;
        } //end switch

    } // end of while(1)

}
