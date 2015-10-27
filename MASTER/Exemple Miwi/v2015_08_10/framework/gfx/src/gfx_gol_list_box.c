/*******************************************************************************
 Module for Microchip Graphics Library - Graphic Object Layer

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_gol_list_box.c

  Summary:
    This implements the list box object of the GOL.

  Description:
    Refer to Microchip Graphics Library for complete documentation of the
    ListBox Object.
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
#include "gfx/gfx_gol_list_box.h"
#include "gfx/gfx_gol_scan_codes.h"

// *****************************************************************************
/*  Function:
    GFX_GOL_LISTBOX *GFX_GOL_ListBoxCreate(
                                uint16_t            ID,
                                uint16_t            left,
                                uint16_t            top,
                                uint16_t            right,
                                uint16_t            bottom,
                                uint16_t            state,
                                GFX_XCHAR           *pText,
                                GFX_ALIGNMENT       alignment,
                                GFX_GOL_OBJ_SCHEME  *pScheme)
    Summary:
        This function creates a GFX_GOL_LISTBOX object with the parameters
        given. It automatically attaches the new object into a global
        linked list of objects and returns the address of the object.

    Description:
        This function creates a GFX_GOL_LISTBOX object with the parameters
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

*/
// *****************************************************************************
GFX_GOL_LISTBOX *GFX_GOL_ListBoxCreate(
                                uint16_t            ID,
                                uint16_t            left,
                                uint16_t            top,
                                uint16_t            right,
                                uint16_t            bottom,
                                uint16_t            state,
                                GFX_XCHAR           *pText,
                                GFX_ALIGNMENT       alignment,
                                GFX_GOL_OBJ_SCHEME  *pScheme)
{
    GFX_GOL_LISTBOX *pLb = NULL;
    GFX_XCHAR       *pointer;
    uint16_t        counter;

    pLb = (GFX_GOL_LISTBOX *)GFX_malloc(sizeof(GFX_GOL_LISTBOX));

    if(pLb == NULL)
        return (pLb);

    pLb->hdr.ID         = ID;
    pLb->hdr.pNxtObj    = NULL;
    pLb->hdr.type       = GFX_GOL_LISTBOX_TYPE;
    pLb->hdr.left       = left;
    pLb->hdr.top        = top;
    pLb->hdr.right      = right;
    pLb->hdr.bottom     = bottom;
    pLb->hdr.state      = state;
    pLb->pItemList      = NULL;
    pLb->scrollY        = 0;
    pLb->itemsNumber    = 0;
    pLb->hdr.DrawObj    = GFX_GOL_ListBoxDraw;          // draw function
    pLb->hdr.FreeObj    = GFX_GOL_ListBoxItemListRemove;// free function
    pLb->hdr.actionGet  = GFX_GOL_ListBoxActionGet;     // action get function
    pLb->hdr.actionSet  = GFX_GOL_ListBoxActionSet;     // default action function

    // set the alignment
    GFX_GOL_ListBoxTextAlignmentSet(pLb, alignment);

    // Set the style scheme to be used
    pLb->hdr.pGolScheme = (GFX_GOL_OBJ_SCHEME *)pScheme;

    pLb->textHeight = GFX_TextStringHeightGet(pLb->hdr.pGolScheme->pFont);

    // Add items if there's an initialization text (each line will become one item,
    // lines must be separated by '\n' character)
    if(pText != NULL)
    {
        pointer = pText;
        counter = 0;
        while(*pointer)
        {
            if(NULL == GFX_GOL_ListBoxItemAdd(
                            pLb,
                            NULL,
                            pointer,
                            NULL,
                            0,
                            counter))
            {
                break;
            }

            while((uint16_t) *pointer++ > (uint16_t)31);

            if(*(pointer - 1) == 0)
                break;
            counter++;
        }
    }

    pLb->pFocusItem = pLb->pItemList;

    GFX_GOL_ObjectAdd((GFX_GOL_OBJ_HEADER *)pLb);

    // Set focus for the object if FOCUSED state is set
#ifndef GFX_CONFIG_FOCUS_DISABLE
    if(GFX_GOL_ObjectStateGet(pLb, GFX_GOL_LISTBOX_FOCUSED_STATE))
        GFX_GOL_ObjectFocusSet((GFX_GOL_OBJ_HEADER *)pLb);
#endif
    return (pLb);
}

// *****************************************************************************
/*  Function:
    GFX_GOL_LISTITEM *GFX_GOL_ListBoxItemAdd(
                                GFX_GOL_LISTBOX     *pObject,
                                GFX_GOL_LISTITEM    *pPrevItem,
                                GFX_XCHAR           *pText,
                                GFX_RESOURCE_HDR    *pImage,
                                uint16_t            status,
                                uint16_t            data)

    Summary:
        This function adds an item to the list box.

    Description:
        This function adds an item to the list box. This function
        allocates the memory needed for the GFX_GOL_LISTITEM and
        adds it to the list box. The newly created GFX_GOL_LISTITEM
        will store the location of pText, pImage and other parameters
        describing the added item.

*/
// *****************************************************************************
GFX_GOL_LISTITEM *GFX_GOL_ListBoxItemAdd(
                                GFX_GOL_LISTBOX     *pObject,
                                GFX_GOL_LISTITEM    *pPrevItem,
                                GFX_XCHAR           *pText,
                                GFX_RESOURCE_HDR    *pImage,
                                uint16_t            status,
                                uint16_t            data)
{
    GFX_GOL_LISTITEM    *pItem;
    GFX_GOL_LISTITEM    *pCurItem;

    pItem = (GFX_GOL_LISTITEM *)GFX_malloc(sizeof(GFX_GOL_LISTITEM));

    if(pItem == NULL)
    {
        return (NULL);
    }

    pObject->itemsNumber++;

    if(pObject->pItemList == NULL)
    {
        pObject->pItemList = pItem;
        pItem->pNextItem = NULL;
        pItem->pPrevItem = NULL;
    }
    else
    {
        if(pPrevItem == NULL)
        {
            // Find last item
            pCurItem = pObject->pItemList;
            while(pCurItem->pNextItem != NULL)
                pCurItem = pCurItem->pNextItem;
        }
        else
        {
            pCurItem = pPrevItem;
        }

        // Insert a new item
        pItem->pNextItem = pCurItem->pNextItem;
        pItem->pPrevItem = pCurItem;
        if (pCurItem->pNextItem)
        {
            // backward link
            ((GFX_GOL_LISTITEM *)(pCurItem->pNextItem))->pPrevItem = pItem;
        }
        pCurItem->pNextItem = pItem;

    }

    pItem->pText = pText;
    pItem->pImage = pImage;
    pItem->status = status;
    pItem->data = data;

    return (pItem);
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_ListBoxItemRemove(
                                GFX_GOL_LISTBOX *pObject
                                GFX_GOL_LISTITEM *pItem)

    Summary:
        This function removes an item from the list box
        and free the memory used.

    Description:
        This function removes an item from the list box
        and free the memory used. The memory freed is the
        memory used for the list box. The actual text or image
        used for the item are not removed from memory.

*/
// *****************************************************************************
void GFX_GOL_ListBoxItemRemove(
                                GFX_GOL_LISTBOX *pObject,
                                GFX_GOL_LISTITEM *pItem)
{
    if(pItem->pNextItem != NULL)
    {
        ((GFX_GOL_LISTITEM *)pItem->pNextItem)->pPrevItem = pItem->pPrevItem;
        if(pItem->pPrevItem == NULL)
            pObject->pItemList = pItem->pNextItem;
    }

    if(pItem->pPrevItem != NULL)
        ((GFX_GOL_LISTITEM *)pItem->pPrevItem)->pNextItem = pItem->pNextItem;

    GFX_free(pItem);

    pObject->itemsNumber--;

    if(pObject->itemsNumber == 0)
        pObject->pItemList = NULL;
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_ListBoxItemListRemove(
                                void *pObject)

    Summary:
        This function removes the entire items list of the list box
        and free the memory used.

    Description:
        This function removes the entire items list of the list box
        and free the memory used. The memory freed is the
        memory used for the list box. The actual text or image
        used for the item are not removed from memory.

*/
// *****************************************************************************
void GFX_GOL_ListBoxItemListRemove(void *pObject)
{
    GFX_GOL_LISTITEM    *pCurItem;
    GFX_GOL_LISTITEM    *pItem;
    GFX_GOL_LISTBOX     *pLb;

    pLb = (GFX_GOL_LISTBOX *)pObject;

    pCurItem = pLb->pItemList;

    while(pCurItem != NULL)
    {
        pItem = pCurItem;
        pCurItem = pCurItem->pNextItem;
        GFX_free(pItem);
    }

    pLb->pItemList = NULL;
	pLb->itemsNumber = 0;  
}

// *****************************************************************************
/*  Function:
    GFX_GOL_LISTITEM *GFX_GOL_ListBoxSelectionGet(
                                GFX_GOL_LISTBOX *pObject,
                                GFX_GOL_LISTITEM *pFromItem)

    Summary:
        This function searches for selected items from the list box.

    Description:
        This function searches for selected items from the list box.
        A starting position can optionally be given. If starting
        position is set to NULL, search will begin from the first
        item on the item list.

        The function returns the pointer to the first selected item
        found or NULL if there are no items selected.

*/
// *****************************************************************************
GFX_GOL_LISTITEM *GFX_GOL_ListBoxSelectionGet(
                                GFX_GOL_LISTBOX *pObject,
                                GFX_GOL_LISTITEM *pFromItem)
{
    if(pFromItem == NULL)
    {
        pFromItem = pObject->pItemList;
    }

    while(pFromItem != NULL)
    {
        if(pFromItem->status & GFX_GOL_LISTBOX_ITEM_STATUS_SELECTED)
            break;
        pFromItem = pFromItem->pNextItem;
    }

    return (pFromItem);
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_ListBoxSelectionChange(
                                GFX_GOL_LISTBOX *pObject,
                                GFX_GOL_LISTITEM *pItem)

    Summary:
        This function changes the selection status of the given item
        in the list box.

    Description:
        This function changes the selection status of the given item
        in the list box. There are two cases that this function checks
        Case 1: The list box is set to multiple selection.
        - The item pointed to by pItem will toggle the selection status.
        Case 2: The list box is set to single selection.
        - If the currently selected item is not the item pointed to by
          pItem, the currently selected item will toggle to not selected.
          The item pointed to by pItem will then be set to selected.
        - If the currently selected item is the same item pointed to by
          pItem, the selection status of that item will be set to
          not selected.

        The change in the item's selection status should be redrawn
        to reflect the changes.

*/
// *****************************************************************************
void GFX_GOL_ListBoxSelectionChange(
                                GFX_GOL_LISTBOX *pObject,
                                GFX_GOL_LISTITEM *pItem)
{
    GFX_GOL_LISTITEM *pCurItem;

    // if item is not set do nothing
    if (NULL == pItem)
        return;
    
    // check if we have multiple selection
    // if we do, set all selected items to NOT selected first       
    if(GFX_GOL_ObjectStateGet(pObject, GFX_GOL_LISTBOX_SINGLE_SELECT_STATE))
    {

        // Remove selection from all items
        pCurItem = pObject->pItemList;
        while(pCurItem != NULL)
        {
            if(pCurItem->status & GFX_GOL_LISTBOX_ITEM_STATUS_SELECTED)
            {
                // toggle the selection
                pCurItem->status &= ~GFX_GOL_LISTBOX_ITEM_STATUS_SELECTED;
                // redraw the item
                pCurItem->status |= GFX_GOL_LISTBOX_ITEM_STATUS_REDRAW;
            }

            pCurItem = pCurItem->pNextItem;
        }
    }

    pItem->status ^= GFX_GOL_LISTBOX_ITEM_STATUS_SELECTED;
    pItem->status |= GFX_GOL_LISTBOX_ITEM_STATUS_REDRAW;
}

// *****************************************************************************
/*  Function:
    uint16_t  GFX_GOL_ListBoxVisibleItemCountGet(
                                GFX_GOL_LISTBOX *pObject)

    Summary:
        This function returns the count of items visible in the
        list box.

    Description:
        This function returns the count of items visible in the
        list box.

*/
// *****************************************************************************
uint16_t __attribute__ ((always_inline)) GFX_GOL_ListBoxVisibleItemCountGet(
                                GFX_GOL_LISTBOX *pObject)
{
    uint16_t count;

    count = (   (   ((GFX_GOL_LISTBOX *)pObject)->hdr.bottom -
                    ((GFX_GOL_LISTBOX *)pObject)->hdr.top -
                    2 * (pObject->hdr.pGolScheme->EmbossSize)
                ) /   
                ((GFX_GOL_LISTBOX *)pObject)->textHeight
            );

    return (count);
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_ListBoxItemFocusSet(
                                GFX_GOL_LISTBOX *pObject,
                                uint16_t index)

    Summary:
        This function sets the focus of the item with the index
        number specified by index.

    Description:
        This function sets the focus of the item with the index
        number specified by index. First item on the list is
        always indexed 0.

*/
// *****************************************************************************
void GFX_GOL_ListBoxItemFocusSet(
                                GFX_GOL_LISTBOX *pObject,
                                uint16_t index)
{
    GFX_GOL_LISTITEM *pCurItem;

    // Look for item to be focused
    pCurItem = pObject->pItemList;

    if(pCurItem == NULL)
        return;

    while(pCurItem->pNextItem != NULL)
    {
        if(index <= 0)
            break;
        index--;
        pCurItem = pCurItem->pNextItem;
    }

    if(pCurItem != NULL)
    {
        if(pObject->pFocusItem != NULL)
        {
            pObject->pFocusItem->status |= GFX_GOL_LISTBOX_ITEM_STATUS_REDRAW;
        }

        pObject->pFocusItem = pCurItem;
        pCurItem->status |= GFX_GOL_LISTBOX_ITEM_STATUS_REDRAW;
    }
}

// *****************************************************************************
/*  Function:
    int16_t GFX_GOL_ListBoxItemFocusGet(
                                GFX_GOL_LISTBOX *pObject)

    Summary:
        This function returns the index of the focused item in the
        list box.

    Description:
        This function returns the index of the focused item in the
        list box. First item on the list is always indexed 0.
        If none of the items in the list is focused, -1 is returned.


    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - The pointer to the object.

    Returns:
        The index of the focused item in teh list box.

    Example:
        None.

*/
// *****************************************************************************
int16_t GFX_GOL_ListBoxItemFocusGet(
                                GFX_GOL_LISTBOX *pObject)
{
    GFX_GOL_LISTITEM    *pCurItem;
    uint16_t            index;

    if(pObject->pFocusItem == NULL)
        return (-1);

    // Look for the focused item index
    index = 0;
    pCurItem = pObject->pItemList;
    while(pCurItem->pNextItem != NULL)
    {
        if(pCurItem == pObject->pFocusItem)
            break;
        index++;
        pCurItem = pCurItem->pNextItem;
    }

    return (index);
}

// *****************************************************************************
/*  Function:
    GFX_GOL_TRANSLATED_ACTION GFX_GOL_ListBoxActionGet(
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
GFX_GOL_TRANSLATED_ACTION GFX_GOL_ListBoxActionGet(
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage)
{
    GFX_GOL_LISTBOX *pLb;

    pLb = (GFX_GOL_LISTBOX *)pObject;

    // Evaluate if the message is for the list box
    // Check if disabled first
    if(GFX_GOL_ObjectStateGet(pLb, GFX_GOL_LISTBOX_DISABLED_STATE))
        return (GFX_GOL_OBJECT_ACTION_INVALID);

#ifndef GFX_CONFIG_USE_TOUCHSCREEN_DISABLE
    if(pMessage->type == TYPE_TOUCHSCREEN)
    {

        // Check if it falls in list box borders. We do not check
        // any touch events here so application can have the versatility
        // to define the action on any of the touch events that will be
        // peformed on the list box.
        // Once application sees the GFX_GOL_LISTBOX_ACTION_TOUCHSCREEN
        // reply (returned by this action get function when touch
        // was performed on the list box), application can define
        // any behavior for the list box.
        if(( pLb->hdr.left  < pMessage->param1) &&
           (pLb->hdr.right  > pMessage->param1) &&
           (pLb->hdr.top    < pMessage->param2) &&
           (pLb->hdr.bottom > pMessage->param2))
        {
            return (GFX_GOL_LISTBOX_ACTION_TOUCHSCREEN);
        }

        return (GFX_GOL_OBJECT_ACTION_INVALID);
    }

#endif

#ifndef GFX_CONFIG_USE_KEYBOARD_DISABLE
    if(pMessage->type == TYPE_KEYBOARD)
    {
        if((uint16_t)pMessage->param1 == pLb->hdr.ID)
        {
            // Check of the event is done here to properly detect that a
            // keyscan event occured. GFX_GOL_LISTBOX_ACTION_MOVE can move
            // the highlight (selection) of the items on the list box when
            // scan up or down was performed (example: pressing up or down
            // keys) or GFX_GOL_LISTBOX_ACTION_SELECTED can indicate to the
            // application that the highlighted item is selected by the user.
            // Application can then call the application function that 
            // corresponds to the selected item.

            if(pMessage->uiEvent == EVENT_KEYSCAN)
            {
                if( (pMessage->param2 == SCAN_UP_PRESSED) ||
                    (pMessage->param2 == SCAN_DOWN_PRESSED))
                {
                    return (GFX_GOL_LISTBOX_ACTION_MOVE);
                }

                if( (pMessage->param2 == SCAN_SPACE_PRESSED) ||
                    (pMessage->param2 == SCAN_CR_PRESSED))
                {
                    return (GFX_GOL_LISTBOX_ACTION_SELECTED);
                }
            }

            return (GFX_GOL_OBJECT_ACTION_INVALID);
        }
    }
#endif
    return (GFX_GOL_OBJECT_ACTION_INVALID);
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_ListBoxActionSet(
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

*/
// *****************************************************************************
void GFX_GOL_ListBoxActionSet(
                                GFX_GOL_TRANSLATED_ACTION translatedMsg,
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage)
{

    GFX_GOL_LISTBOX *pLb;

    pLb = (GFX_GOL_LISTBOX *)pObject;

#ifndef GFX_CONFIG_USE_TOUCHSCREEN_DISABLE

    int16_t             pos;
    GFX_GOL_LISTITEM    *pItem;

    if(pMessage->type == TYPE_TOUCHSCREEN)
    {
#ifndef GFX_CONFIG_FOCUS_DISABLE
        if(pMsg->type == TYPE_TOUCHSCREEN)
        {
            if(!GFX_GOL_ObjectStateGet(pLb, GFX_GOL_LISTBOX_FOCUSED_STATE))
            {
                GOLSetFocus((OBJ_HEADER *)pLb);
            }
        }

#endif
        if(pMessage->uiEvent == EVENT_PRESS)
        {
            pos = ( pMessage->param2 -
                    pLb->scrollY -
                    pLb->hdr.top -
                    pLb->hdr.pGolScheme->EmbossSize) / pLb->textHeight;

            pItem = pLb->pItemList;

            while(pos)
            {
                if(pItem == NULL)
                    break;
                if(pItem->pNextItem == NULL)
                    break;
                pItem = pItem->pNextItem;
                pos--;
            }

            if(pLb->pFocusItem != pItem)
            {
                pItem->status |= GFX_GOL_LISTBOX_ITEM_STATUS_REDRAW;
                pLb->pFocusItem->status |= GFX_GOL_LISTBOX_ITEM_STATUS_REDRAW;
                pLb->pFocusItem = pItem;
            }

            GFX_GOL_ListBoxSelectionChange(pLb, pItem);
            GFX_GOL_ObjectStateSet(
                    pLb,
                    GFX_GOL_LISTBOX_DRAW_ITEMS_STATE);
        }
    }

#endif

#ifndef GFX_CONFIG_USE_KEYBOARD_DISABLE
    if(pMessage->type == TYPE_KEYBOARD)
    {
        switch(translatedMsg)
        {
            case GFX_GOL_LISTBOX_ACTION_SELECTED:
                if(pLb->pFocusItem != NULL)
                    GFX_GOL_ListBoxSelectionChange(
                            pLb,
                            pLb->pFocusItem);
                GFX_GOL_ObjectStateSet(
                        pLb,
                        GFX_GOL_LISTBOX_DRAW_ITEMS_STATE);
                break;

            case GFX_GOL_LISTBOX_ACTION_MOVE:
                switch(pMessage->param2)
                {
                    case SCAN_UP_PRESSED:
                        GFX_GOL_ListBoxItemFocusSet(
                                pLb,
                                GFX_GOL_ListBoxItemFocusGet(pLb) - 1);
                        GFX_GOL_ObjectStateSet(
                                pLb,
                                GFX_GOL_LISTBOX_DRAW_ITEMS_STATE);
                        break;

                    case SCAN_DOWN_PRESSED:
                        GFX_GOL_ListBoxItemFocusSet(
                                pLb,
                                GFX_GOL_ListBoxItemFocusGet(pLb) + 1);
                        GFX_GOL_ObjectStateSet(
                                pLb,
                                GFX_GOL_LISTBOX_DRAW_ITEMS_STATE);
                        break;
                }
                break;

            default:
                // catch all for messages added by users and
                // behavior defined by users in message callback
                break;

        }
    }   // end of if
#endif
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_GOL_ListBoxDraw(void *pObject)

    Summary:
        This function renders the object on the screen based on the
        current state of the object.

    Description:
        This function renders the object on the screen based on the
        current state of the object. Location of the object is
        determined by the left, top, right and bottom parameters.
        The colors used are dependent on the state of the object.
        The font used is determined by the style scheme set.

        Text alignment based on the alignment parameter set on the
        object.

        When rendering objects of the same type, each object
        must be rendered completely before the rendering of the
        next object is started. This is to avoid incomplete
        object rendering.

        Normally, application will just call GFX_GOL_ObjectListDraw()
        to allow the Graphics Library to manage all object rendering.
        See GFX_GOL_ObjectListDraw() for more information on object rendering.
 
*/
// *****************************************************************************
GFX_STATUS GFX_GOL_ListBoxDraw(void *pObject)
{
    typedef enum
    {
        LB_STATE_START,
        LB_STATE_PANEL,
        LB_STATE_DRAW_ITEMS,
        LB_STATE_DRAW_CURRENT_ITEMS,
        LB_STATE_SET_ERASEITEM,
        LB_STATE_ERASEITEM,
        LB_STATE_SET_IMAGE_TEXT,
        LB_STATE_SET_ITEMFOCUS,
        LB_STATE_ITEMFOCUS,
        LB_STATE_GET_NEXTITEM,
        LB_STATE_BITMAP,
        LB_STATE_TEXT
    } LB_DRAW_STATES;

    static LB_DRAW_STATES   state = LB_STATE_START;
    static GFX_GOL_LISTITEM *pCurItem;
    static GFX_COLOR        tempColor;
    static int16_t          temp;
    static uint16_t         left, top, right, bottom;
    static uint16_t         embossSize, entryHeight, tempTop;
    GFX_GOL_LISTBOX         *pLb;
    GFX_XCHAR               *pChar;
    uint16_t                imageWidth;

    pLb = (GFX_GOL_LISTBOX *)pObject;

    while(1)
    {

        if (GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
            return (GFX_STATUS_FAILURE);


        switch(state)
        {
            // location of this case is strategic so the partial redraw 
            // of the ListBox will be faster        
            case LB_STATE_PANEL:

                if(GFX_GOL_PanelDraw() != GFX_STATUS_SUCCESS)
		{
                    return (GFX_STATUS_FAILURE);
                }

                state = LB_STATE_DRAW_ITEMS;
                break;

            case LB_STATE_START:
#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                GFX_DRIVER_SetupDrawUpdate( pLb->hdr.left,
                                            pLb->hdr.top,
                                            pLb->hdr.right,
                                            pLb->hdr.bottom);
#endif
                ///////////////////////////////////////////////////////////////
                // REMOVE FROM SCREEN
                ///////////////////////////////////////////////////////////////
                if(GFX_GOL_ObjectStateGet(pLb, GFX_GOL_LISTBOX_HIDE_STATE))
                {
                    // Hide the object (remove from screen)
                    if (GFX_GOL_ObjectHideDraw(&(pLb->hdr)) != GFX_STATUS_SUCCESS)
                        return (GFX_STATUS_FAILURE);


#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                    GFX_DRIVER_CompleteDrawUpdate(  pLb->hdr.left,
                                                    pLb->hdr.top,
                                                    pLb->hdr.right,
                                                    pLb->hdr.bottom);
#endif

                    return (GFX_STATUS_SUCCESS);
                }
    
                if(GFX_GOL_ObjectStateGet(
                        pLb,
                        GFX_GOL_LISTBOX_DRAW_FOCUS_STATE))
                {
                    if(pLb->pFocusItem != NULL)
                    {
                        ((GFX_GOL_LISTITEM *)pLb->pFocusItem)->status |= 
                                    GFX_GOL_LISTBOX_ITEM_STATUS_REDRAW;
                    }

                    GFX_GOL_ObjectStateSet(
                            pLb,
                            GFX_GOL_LISTBOX_DRAW_ITEMS_STATE);
                }

                embossSize = pLb->hdr.pGolScheme->EmbossSize;

                /////////////////////////////////////////////////////////////
                // DRAW PANEL
                /////////////////////////////////////////////////////////////
                if(GFX_GOL_ObjectStateGet(pLb, GFX_GOL_LISTBOX_DRAW_STATE))
                {
                    if(GFX_GOL_ObjectStateGet(
                            pLb,
                            GFX_GOL_LISTBOX_DISABLED_STATE))
                    {
                        tempColor = pLb->hdr.pGolScheme->ColorDisabled;
                    }
                    else
                    {
                        tempColor = pLb->hdr.pGolScheme->Color0;
                    }
                    
                    // set up the background
                    GFX_GOL_PanelBackgroundSet(&pLb->hdr);

                    // set panel parameters
                    GFX_GOL_PanelParameterSet
                    (
                        pLb->hdr.left,
                        pLb->hdr.top,
                        pLb->hdr.right,
                        pLb->hdr.bottom,
                        0,
                        tempColor,
                        pLb->hdr.pGolScheme->EmbossDkColor,
                        pLb->hdr.pGolScheme->EmbossLtColor,
                        NULL,
                        pLb->hdr.pGolScheme->fillStyle,
                        embossSize
                    );
    
                    state = LB_STATE_PANEL;
                    break;
                }
                else
                {
                    state = LB_STATE_DRAW_ITEMS;
                }
            
            case LB_STATE_DRAW_ITEMS:
    
                ///////////////////////////////////////////////////////////////
                // DRAW ITEMS
                ///////////////////////////////////////////////////////////////
                left   = pLb->hdr.left   + embossSize;
                top    = pLb->hdr.top    + embossSize + pLb->scrollY;
                right  = pLb->hdr.right  - embossSize;
                bottom = pLb->hdr.bottom - embossSize;

                GFX_FontSet(pLb->hdr.pGolScheme->pFont);
                    
                pCurItem = pLb->pItemList;
                state = LB_STATE_DRAW_CURRENT_ITEMS;
    
            case LB_STATE_DRAW_CURRENT_ITEMS:

                ///////////////////////////////////////////////////////////////
                // DRAW CURRENT ITEM
                ///////////////////////////////////////////////////////////////

                // check if at the end of the list of items
                // this is the exit from the drawing state machine
                if(pCurItem == NULL)
                {
                    state = LB_STATE_START;

#ifdef GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE
                    GFX_DRIVER_CompleteDrawUpdate(  pLb->hdr.left,
                                                    pLb->hdr.top,
                                                    pLb->hdr.right,
                                                    pLb->hdr.bottom);
#endif
                    return (GFX_STATUS_SUCCESS);
                }
    
                ///////////////////////////////////////////////////////////////
                // pCurItem is a valid item, check first if within the bounds
                ///////////////////////////////////////////////////////////////
                if(top >= bottom)
                {
                    state = LB_STATE_SET_ITEMFOCUS;
                    break;
                }
                // yes, still within bounds process the item
                if( (top + pLb->textHeight) >=
                    (pLb->hdr.top + embossSize)
                  )
                {

                    if(!GFX_GOL_ObjectStateGet(
                            pLb,
                            GFX_GOL_LISTBOX_DRAW_STATE))
                    {
                        if(!(   pCurItem->status &
                                GFX_GOL_LISTBOX_ITEM_STATUS_REDRAW)
                          )
                        {
                            state = LB_STATE_SET_ITEMFOCUS;
                            break;
                        }   
                    }

                    pCurItem->status &= ~GFX_GOL_LISTBOX_ITEM_STATUS_REDRAW;
                    state = LB_STATE_SET_ERASEITEM;
                }
                else
                {
                    state = LB_STATE_SET_ITEMFOCUS;
                    break;
                }
    
            case LB_STATE_SET_ERASEITEM:

                if(GFX_GOL_ObjectStateGet(
                        pLb,
                        GFX_GOL_LISTBOX_DISABLED_STATE))
                {
                    GFX_ColorSet(pLb->hdr.pGolScheme->ColorDisabled);
                }
                else
                {
                    GFX_ColorSet(pLb->hdr.pGolScheme->Color0);
                    if(pCurItem != NULL)
                        if( pCurItem->status &
                            GFX_GOL_LISTBOX_ITEM_STATUS_SELECTED)
                        {
                            GFX_ColorSet(pLb->hdr.pGolScheme->Color1);
                        }
                }
                
                // set the height of the entry so the rendering
                // will not overshoot at the bottom end of the list box
                if ((top + pLb->textHeight) > bottom)
                    entryHeight = bottom - top - 1;
                else
                    entryHeight = pLb->textHeight - 1;

                if (top < (pLb->hdr.top + embossSize))
                    tempTop = (pLb->hdr.top + embossSize);
                else
                    tempTop = top;

                // set the background information here
                GFX_GOL_ObjectBackGroundSet(&pLb->hdr);
                state = LB_STATE_ERASEITEM;

            case LB_STATE_ERASEITEM:

                if(GFX_RectangleFillDraw(
                        left,
                        tempTop,
                        right,
                        tempTop + entryHeight)
                  != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }

                state = LB_STATE_SET_IMAGE_TEXT;

            case LB_STATE_SET_IMAGE_TEXT:

                if(GFX_GOL_ObjectStateGet(
                        pLb,
                        GFX_GOL_LISTBOX_DISABLED_STATE))
                {
                    GFX_ColorSet(pLb->hdr.pGolScheme->TextColorDisabled);
                }
                else
                {
                    if(pCurItem->status & GFX_GOL_LISTBOX_ITEM_STATUS_SELECTED)
                    {
                        GFX_ColorSet(pLb->hdr.pGolScheme->TextColor1);
                    }
                    else
                    {
                        GFX_ColorSet(pLb->hdr.pGolScheme->TextColor0);
                    }
                }

                // check if there is an image and the image will not go  
                // beyond the list box (bottom check only)
                if( (GFX_GOL_ListBoxItemImageGet(pCurItem) != NULL) &&
                    ((top + GFX_ImageHeightGet(pCurItem->pImage)) < 
                     bottom))
	        {
                    state = LB_STATE_BITMAP;
                }
                else
                {
                    state = LB_STATE_TEXT;
                    break;
                }
    
            case LB_STATE_BITMAP:
                if(GFX_ImageDraw(
                    left,
                    top + ((pLb->textHeight -
                           GFX_ImageHeightGet(GFX_GOL_ListBoxItemImageGet(pCurItem))) >> 1),
                    pCurItem->pImage) != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_SUCCESS);
                }
                state = LB_STATE_TEXT;
    
            case LB_STATE_TEXT:

                // find the number of characters to render
                if (GFX_GOL_ListBoxItemImageGet(pCurItem) != NULL)
                {
                    imageWidth = GFX_ImageWidthGet(
                                    GFX_GOL_ListBoxItemImageGet(pCurItem));
                }
                else
                {
                    imageWidth = 0;
                }

                // count the number of characters to render
                pChar = pCurItem->pText;
                temp = 0;
                while(*pChar != 0x0A)
                {
                    pChar++;
                    temp++;
                }
                // show only the text that truly fit into the space
                if ((top > pLb->hdr.top) && ((top + pLb->textHeight) <= bottom))
                {
                    if (GFX_TextStringBoxDraw(
                            left + imageWidth,
                            top,
                            right - left,
                            entryHeight,
                            pCurItem->pText,
                            temp,
                            pLb->alignment | GFX_ALIGN_TOP) != GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }
                }
                state = LB_STATE_SET_ITEMFOCUS;
    
                //////////////////////////////////////////////////////////////
    
            case LB_STATE_SET_ITEMFOCUS:
                if(pLb->pFocusItem == pCurItem)
                {
                    if (GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
                        return (GFX_STATUS_FAILURE);
    
                    if(GFX_GOL_ObjectStateGet(
                            pLb,
                            GFX_GOL_LISTBOX_FOCUSED_STATE))
                    {
                        GFX_ColorSet(pLb->hdr.pGolScheme->TextColor1);
                    }
                    else
                    {
                        GFX_ColorSet(pLb->hdr.pGolScheme->TextColor0);
                    }

                    // set line style for focus lines style
                    GFX_LineStyleSet(GFX_LINE_STYLE_THIN_DOTTED);
                    temp = top;
                    state = LB_STATE_ITEMFOCUS;
                }
                else
                {
                    state = LB_STATE_GET_NEXTITEM;
                    break;
                }

            case LB_STATE_ITEMFOCUS:

                if (top < bottom)
                {
                    if( GFX_RectangleDraw(
                            left, 
                            tempTop,
                            right,
                            tempTop + entryHeight) != GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }
                }

                GFX_LineStyleSet(GFX_LINE_STYLE_THIN_SOLID);

                // **********************
                // Scroll if needed
                // **********************

                // this section checks for scrolling up
                if(temp < (pLb->hdr.top + embossSize))
                {
                    pLb->scrollY += ((pLb->hdr.top + embossSize) - temp);
                    GFX_GOL_ObjectStateSet(pLb, GFX_GOL_LISTBOX_DRAW_STATE);
                    state = LB_STATE_DRAW_ITEMS;
                    break;
                }

                // this section checks for scrolling down
                if((temp + pLb->textHeight) > bottom)
                {
                    pLb->scrollY += (
                                        (   pLb->hdr.bottom -
                                            pLb->textHeight -
                                            embossSize) - temp
                                    );
                    GFX_GOL_ObjectStateSet(pLb, GFX_GOL_LISTBOX_DRAW_STATE);
                    state = LB_STATE_DRAW_ITEMS;
                    break;
                }
                state = LB_STATE_GET_NEXTITEM;
    
            case LB_STATE_GET_NEXTITEM:

                pCurItem = (GFX_GOL_LISTITEM *)pCurItem->pNextItem;
                top += pLb->textHeight;// MoveRel(0, pLb->textHeight);

                state = LB_STATE_DRAW_CURRENT_ITEMS;
                break;    

        }   // end of switch
    } // end of while(1)
}

