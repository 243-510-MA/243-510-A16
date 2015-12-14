/*******************************************************************************
 Module for Microchip Graphics Library - Graphic Object Layer

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_gol_list_box.h

  Summary:
    This is the header file for the list box object of the GOL.

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

#ifndef _GFX_GOL_LISTBOX_H
// DOM-IGNORE-BEGIN
    #define _GFX_GOL_LISTBOX_H
// DOM-IGNORE-END

#include <stdint.h>
#include <gfx/gfx_gol.h>

// *****************************************************************************
/* 
    <GROUP gol_types>

    Typedef:
        GFX_GOL_LISTBOX_STATE

    Summary:
        Specifies the different states of the List Box object.

    Description:
        This enumeration specifies the different states of the List Box
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
    GFX_GOL_LISTBOX_FOCUSED_STATE
        /*DOM-IGNORE-BEGIN*/  = 0x0001 /*DOM-IGNORE-END*/,  
    // Property bit to indicate object is disabled.
    GFX_GOL_LISTBOX_DISABLED_STATE
        /*DOM-IGNORE-BEGIN*/  = 0x0002 /*DOM-IGNORE-END*/,  
    // Property bit to indicate only one item can be selected.
    GFX_GOL_LISTBOX_SINGLE_SELECT_STATE
        /*DOM-IGNORE-BEGIN*/  = 0x0010 /*DOM-IGNORE-END*/,  
    // Draw bit to indicate selected items of the object must be redrawn.
    GFX_GOL_LISTBOX_DRAW_ITEMS_STATE   
        /*DOM-IGNORE-BEGIN*/  = 0x1000 /*DOM-IGNORE-END*/,  
    // Draw bit to indicate focus must be redrawn.
    GFX_GOL_LISTBOX_DRAW_FOCUS_STATE  
        /*DOM-IGNORE-BEGIN*/  = 0x2000 /*DOM-IGNORE-END*/,  
    // Draw Bit to indicate object must be redrawn.
    GFX_GOL_LISTBOX_DRAW_STATE
        /*DOM-IGNORE-BEGIN*/  = 0x4000 /*DOM-IGNORE-END*/,  
    // Draw bit to indicate object must be removed from screen.
    GFX_GOL_LISTBOX_HIDE_STATE           
        /*DOM-IGNORE-BEGIN*/  = 0x8000 /*DOM-IGNORE-END*/,  
} GFX_GOL_LISTBOX_STATE;

// *****************************************************************************
/* 
    <GROUP gol_types>

    Typedef:
        GFX_GOL_LISTBOX_ITEM_STATUS

    Summary:
        Defines the types used to indicate the status of an item.
        
    Description:
        Defines the types used to indicate the status of an item
        in the List Box.
        
    Remarks:
        None.
        
*/
// ***************************************************************************** 
typedef enum
{
    // Item is selected.
    GFX_GOL_LISTBOX_ITEM_STATUS_SELECTED
        /*DOM-IGNORE-BEGIN*/  = 0x0001 /*DOM-IGNORE-END*/, 
    // Item is to be redrawn.
    GFX_GOL_LISTBOX_ITEM_STATUS_REDRAW   
        /*DOM-IGNORE-BEGIN*/  = 0x0002 /*DOM-IGNORE-END*/, 
} GFX_GOL_LISTBOX_ITEM_STATUS;

// *****************************************************************************
/* 
    <GROUP gol_types>

    Typedef:
        GFX_GOL_LISTITEM

    Summary:
        The structure that defines each item in the list box.
        
    Description:
        Each item in the list box is described by this structure.
        The items are arranged as a linked list of this type.
        
    Remarks:
        None.
        
*/
// ***************************************************************************** 
typedef struct
{
    void                *pPrevItem;     // Pointer to the next item
    void                *pNextItem;     // Pointer to the next item
    GFX_GOL_LISTBOX_ITEM_STATUS status; // Specifies the status of the item.
    GFX_XCHAR           *pText;         // Pointer to the text for the item
    GFX_RESOURCE_HDR    *pImage;        // Pointer to the image used
    uint16_t            data;           // Some data associated with the item
} GFX_GOL_LISTITEM;

// *****************************************************************************
/* 
    <GROUP gol_types>

    Typedef:
        GFX_GOL_LISTBOX

    Summary:
        Defines the structure used for the List Box object.
        
    Description:
        Defines the parameters required for a List Box Object.
        Object is drawn with the defined shape parameters and values 
        set on the given fields.
        
    Remarks:
        None.
        
*/
// *****************************************************************************
typedef struct
{
    GFX_GOL_OBJ_HEADER  hdr;            // Generic header for all Objects (see GFX_GOL_OBJ_HEADER).
    GFX_GOL_LISTITEM    *pItemList;     // Pointer to the list of items.
    GFX_GOL_LISTITEM    *pFocusItem;    // Pointer to the focused item.
    uint16_t            itemsNumber;    // Number of items in the list box.
    int16_t             scrollY;        // Scroll displacement for the list.
    int16_t             textHeight;     // Pre-computed text height.
    GFX_ALIGNMENT       alignment;      // items alignment
} GFX_GOL_LISTBOX;

// *****************************************************************************
/*  
    <GROUP gol_listbox_object>

    Function:
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

    Precondition:
        None.

    Parameters:
        ID - Unique user defined ID for the object instance.
        left - Left most position of the object.
        top - Top most position of the object.
        right - Right most position of the object.
        bottom - Bottom most position of the object.
        state - Sets the initial state of the object.
        pText - Pointer to the text of the object. This is used for
                the items of the object.
        alignment - text alignment of the text used in the object.
        pScheme - Pointer to the style scheme used.

    Returns:
        Pointer to the newly created object.

    Example:
        <code>

            #define LISTBOX_ID   10
            const XCHAR ItemList[] = "Line1\n" "Line2\n";

            GFX_GOL_OBJ_SCHEME  *pScheme;
            GFX_GOL_LISTBOX     *pLb;
            GFX_XCHAR           *pTemp;
            uint16_t            state, counter;

            //assume scheme is a valid style scheme in memory
            pScheme = &scheme;
            state = GFX_GOL_LISTBOX_DRAW_STATE;

            // create an empty listbox with default style scheme
            pLb = GFX_GOL_ListBoxCreate(    LISTBOX_ID,     // ID number
                                            10,10,150,200,  // dimension
                                            state,          // initial state
                                            NULL,           // set items to be empty
                                            GFX_ALIGN_CENTER,
                                            NULL);          // use default style scheme
            // check if Listbox was created
            if (pLb == NULL)
               return 0;

            // create the list of items to be placed in the listbox
            // Add items (each line will become one item,
            // lines must be separated by '\n' character)
            pTemp = ItemList;
            counter = 0;

            while(*pTemp)
            {
                // since each item is appended NULL is assigned to
                // GFX_GOL_LISTITEM pointer.
                if(NULL == GFX_GOL_ListBoxItemAdd(
                            pLb,
                            NULL,
                            pTemp,
                            NULL,
                            0,
                            counter))
                {
                    break;
                }

                while(*pTemp++ > (unsigned GFX_XCHAR)31);

                if(*(pTemp-1) == 0)
                    break;
                counter++;
            }


        </code>

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
                                GFX_GOL_OBJ_SCHEME  *pScheme);

// *****************************************************************************
/*  
    <GROUP gol_listbox_object>

    Function:
        GFX_GOL_LISTITEM *GFX_GOL_ListBoxItemListGet(
                                GFX_GOL_LISTBOX *pObject)

    Summary:
        This function returns the pointer to the item list of the list box.

    Description:
        This function returns the pointer to the item list of the list box.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - Pointer to the object.

    Returns:
        The pointer to the item list of the list box.

    Example:
        <code>

            // assume:
            // pLb is a initialized to a list box in memory
            // myIcon is a valid image in memory

            GFX_GOL_LISTITEM *pList, *pItem;
            GFX_RESOURCE_HDR *pMyIcon = &myIcon;

            pList = GFX_GOL_ListBoxItemListGet(pLb);
            pItem = pList;

            // set the image for all the items to myIcon
            // set the last item image to NULL

            do
            {
                GFX_GOL_ListBoxImageSet(pItem, &myIcon);
            }
            while(pItem->pNextItem != NULL);

            // get the last item's image and set to null if not null
            if (GFX_GOL_ListBoxItemImageGet(pItem) != NULL)
                GFX_GOL_ListBoxImageSet(pItem, NULL);

        </code>

*/
// *****************************************************************************
#define GFX_GOL_ListBoxItemListGet(pObject)        \
                ((GFX_GOL_LISTITEM *)((GFX_GOL_LISTBOX *)pObject)->pItemList)

// *****************************************************************************
/*  
    <GROUP gol_listbox_object>

    Function:
        void GFX_GOL_ListBoxItemImageSet(
                                GFX_GOL_LISTITEM *pItem,
                                GFX_RESOURCE_HDR *pImage)

    Summary:
        This function sets the image for a list box item.
 
    Description:
        This function sets the image for a list box item.
 
    Precondition:
        Object must exist in memory.

    Parameters:
        pItem - pointer to the list box item.
        pImage  - pointer to the image resource.

    Returns:
        None.

    Example:
        See GFX_GOL_ListBoxItemAdd() and GFX_GOL_ListBoxItemListGet()
        example.
 
*/
// *****************************************************************************
#define GFX_GOL_ListBoxItemImageSet(pItem, pImage)  (((GFX_GOL_LISTITEM *)pItem)->pImage = pImage)

// *****************************************************************************
/*  
    <GROUP gol_listbox_object>

    Function:
        GFX_RESOURCE_HDR *GFX_GOL_ListBoxItemImageGet(
                                GFX_GOL_LISTITEM *pItem)

    Summary:
        This function gets the image set for a list box item.

    Description:
        This function gets the image set for a list box item.

    Precondition:
        Object must exist in memory.

    Parameters:
        pItem - pointer to the list box item.

    Returns:
        Pointer to the image used for the item.

    Example:
        See GFX_GOL_ListBoxItemListGet() example.

*/
// *****************************************************************************
#define GFX_GOL_ListBoxItemImageGet(pItem)  (((GFX_GOL_LISTITEM *)pItem)->pImage)

// *****************************************************************************
/*  
    <GROUP gol_listbox_object>

    Function:
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

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - Pointer to the object.
        pPrevItem - Pointer to the item after which a new item must
                be inserted, if this pointer is NULL, the item
                will be appended at the end of the items list.
        pText - Pointer to the text that will be inserted. Text must
                persist in memory for as long as it is referenced
                by an item in the list box.
        pImage - Pointer to the image for the item. Image must
                persist in memory for as long as it is referenced
                by the an item in the list box.
        status - This parameter specifies if the item being added
                will be selected or redrawn
                (GFX_GOL_LISTBOX_ITEM_STATUS_SELECTED or
                GFX_GOL_LISTBOX_ITEM_STATUS_REDRAW). Refer to
                GFX_GOL_LISTITEM structure for details.
        data -  User assigned data associated with the item.

    Returns:
        The pointer to the created item.

    Example:
        <code>

            const GFX_XCHAR ItemList[] = "Line1\n" "Line2\n" "Line3\n";

            extern GFX_RESOURCE_HDR myIcon;
            GFX_GOL_LISTBOX         *pLb;
            GFX_GOL_LISTITEM        *pItem, *pItemList;
            GFX_XCHAR               *pTemp;

            // Assume that pLb is pointing to an existing list box in memory
            // that is empty (no list).

            // Create the list of the list box

            // Initialize this to NULL to indicate that items will be added
            // at the end of the list if the list exist on the list box or
            // start a new list if the list box is empty.
            pItem = NULL;
            pTemp = ItemList;
            pItem = GFX_GOL_ListBoxItemAdd(
                            pLb,
                            pItem,
                            pTemp,
                            NULL,
                            GFX_GOL_LISTBOX_ITEM_STATUS_SELECTED,
                            1);
            if(pItem == NULL)
                return 0;
            GFX_GOL_ListBoxImageSet(pItem, &myIcon);

            // Adjust pTemp to point to the next line
            while((uint16_t)*pTemp++ > (uint16_t)31);

            // add the next item
            pItem = GFX_GOL_ListBoxItemAdd(
                            pLb,
                            pItem,
                            pTemp,
                            NULL,
                            0,
                            2)
            if(pItem == NULL)
                return 0;
            GFX_GOL_ListBoxImageSet(pItem, &myIcon);

            // Adjust pTemp to point to the next line
            while((uint16_t)*pTemp++ > (uint16_t)31);

            // this time insert the next item after the first item on the list
            pItem = LbGetItemList(pLb);
            pItem = GFX_GOL_ListBoxItemAdd(
                            pLb,
                            pItem,
                            pTemp,
                            NULL,
                            0,
                            3)
            if(pItem == NULL)
                return 0;
            GFX_GOL_ListBoxImageSet(pItem, &myIcon);

        </code>

*/
// *****************************************************************************
GFX_GOL_LISTITEM *GFX_GOL_ListBoxItemAdd(
                                GFX_GOL_LISTBOX     *pObject,
                                GFX_GOL_LISTITEM    *pPrevItem,
                                GFX_XCHAR           *pText,
                                GFX_RESOURCE_HDR    *pImage,
                                uint16_t            status,
                                uint16_t            data);

// *****************************************************************************
/*  
    <GROUP gol_listbox_object>

    Function:
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

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.
        pItem - pointer to the list box item that will be removed.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void GFX_GOL_ListBoxItemRemove(
                                GFX_GOL_LISTBOX *pObject,
                                GFX_GOL_LISTITEM *pItem);

// *****************************************************************************
/*  
    <GROUP gol_listbox_object>

    Function:
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
void GFX_GOL_ListBoxItemListRemove(void *pObject);

// *****************************************************************************
/*  
    <GROUP gol_listbox_object>

    Function:
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

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - The pointer to the object.
        pFromItem - The starting point of the search. If this is set
                to NULL, the search will start at the beginning of
                the item list.

    Returns:
        The pointer to the first selected item found. NULL if there are no
        items selected.

    Example:
        None.

*/
// *****************************************************************************
GFX_GOL_LISTITEM *GFX_GOL_ListBoxSelectionGet(
                                GFX_GOL_LISTBOX *pObject,
                                GFX_GOL_LISTITEM *pFromItem);

// *****************************************************************************
/*  
    <GROUP gol_listbox_object>

    Function:
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

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - The pointer to the object.
        pItem - The pointer to the item that will have the selection status
                changed.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void GFX_GOL_ListBoxSelectionChange(
                                GFX_GOL_LISTBOX *pObject,
                                GFX_GOL_LISTITEM *pItem);

// *****************************************************************************
/*  
    <GROUP gol_listbox_object>

    Function:
        void GFX_GOL_ListBoxItemSelectStatusSet(
                                GFX_GOL_LISTBOX *pObject,
                                GFX_GOL_LISTITEM *pItem)

    Summary:
        This function sets the selection status of the item to selected.

    Description:
        This function sets the selection status of the item to selected.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - The pointer to the object.
        pItem - The pointer the item that will have the selected status
                set to selected.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
#define GFX_GOL_ListBoxItemSelectStatusSet(pObject, pItem)                  \
                                                                            \
                if(!(pItem->status & GFX_GOL_LISTBOX_ITEM_STATUS_SELECTED)) \
                    GFX_GOL_ListBoxSelectionChange((LISTBOX *)pObject, pItem);

// *****************************************************************************
/*  
    <GROUP gol_listbox_object>

    Function:
        void GFX_GOL_ListBoxItemSelectStatusClear(
                                GFX_GOL_LISTBOX *pObject,
                                GFX_GOL_LISTITEM *pItem)

    Summary:
        This function clears the selection status of the item.

    Description:
        This function clears the selection status of the item.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - The pointer to the object.
        pItem - The pointer the item that will have the selected status
                cleared.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
#define GFX_GOL_ListBoxItemSelectStatusClear(pObject, pItem)                \
                                                                            \
                if(pItem->status & GFX_GOL_LISTBOX_ITEM_STATUS_SELECTED)    \
                    GFX_GOL_ListBoxSelectionChange((LISTBOX *)pObject, pItem);

// *****************************************************************************
/*  
    <GROUP gol_listbox_object>

    Function:
        uint16_t  GFX_GOL_ListBoxItemCountGet(
                                GFX_GOL_LISTBOX *pObject)

    Summary:
        This function returns the count of items in the list box.

    Description:
        This function returns the count of items in the list box.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - The pointer to the object.

    Returns:
        The number of items in the list box.

    Example:
        None.

*/
// *****************************************************************************
#define GFX_GOL_ListBoxItemCountGet(pObject)                \
                (((GFX_GOL_LISTBOX *)pObject)->itemsNumber)

// *****************************************************************************
/*  
    <GROUP gol_listbox_object>

    Function:
        uint16_t  GFX_GOL_ListBoxVisibleItemCountGet(
                                GFX_GOL_LISTBOX *pObject)

    Summary:
        This function returns the count of items visible in the
        list box.

    Description:
        This function returns the count of items visible in the
        list box.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - The pointer to the object.

    Returns:
        The number of visible items in the list box.

    Example:
        None.

*/
// *****************************************************************************
uint16_t GFX_GOL_ListBoxVisibleItemCountGet(GFX_GOL_LISTBOX *pObject);

// *****************************************************************************
/*  
    <GROUP gol_listbox_object>

    Function:
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

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - The pointer to the object.
        index - The index number of the item to be focused.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
void GFX_GOL_ListBoxItemFocusSet(
                                GFX_GOL_LISTBOX *pObject,
                                uint16_t index);

// *****************************************************************************
/*  
    <GROUP gol_listbox_object>

    Function:
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
        The index of the focused item in the list box.
 
    Example:
        None.

*/
// *****************************************************************************
int16_t GFX_GOL_ListBoxItemFocusGet(
                                GFX_GOL_LISTBOX *pObject);

// *****************************************************************************
/*  
    <GROUP gol_listbox_object>

    Function:
        GFX_ALIGNMENT GFX_GOL_ListBoxTextAlignmentGet(
                                GFX_GOL_LISTBOX *pObject)

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
#define GFX_GOL_ListBoxTextAlignmentGet(pObject)                \
                (((GFX_GOL_LISTBOX *)pObject)->alignment)

// *****************************************************************************
/*  
    <GROUP gol_listbox_object>

    Function:
        void GFX_GOL_ListBoxTextAlignmentSet(
                                GFX_GOL_LISTBOX *pObject,
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
#define GFX_GOL_ListBoxTextAlignmentSet(pObject, align)         \
                (((GFX_GOL_LISTBOX *)pObject)->alignment = align)

// *****************************************************************************
/*  
    <GROUP gol_listbox_object>

    Function:
        GFX_GOL_TRANSLATED_ACTION GFX_GOL_ListBoxActionGet(
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
        GFX_GOL_LISTBOX_ACTION_TOUCHSCREEN  Touch Screen    Any                         Item is selected using touch screen.
        GFX_GOL_LISTBOX_ACTION_MOVE         Keyboard        EVENT_KEYSCAN               Focus is moved to the next item depending on the key pressed (UP or DOWN key).
        GFX_GOL_LISTBOX_ACTION_SELECTED     Keyboard        EVENT_KEYSCAN               Selection status (GFX_GOL_LISTBOX_ITEM_STATUS_SELECTED) is set to the currently focused item.
        GFX_GOL_OBJECT_ACTION_INVALID       Any             Any                         If the message did not affect the object.							  
        </TABLE> 

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - The pointer to the object where the message will be
                   evaluated to check if the message will affect the object.
        pMessage - Pointer to the the message from the user interface.
 
    Returns:
        - GFX_GOL_LISTBOX_ACTION_TOUCHSCREEN – Item is selected using touch
                                               screen
        - GFX_GOL_LISTBOX_ACTION_MOVE        – Focus is moved to the next
                                               item depending on the key
                                               pressed (UP or DOWN key).
        - GFX_GOL_LISTBOX_ACTION_SELECTED    – Selection is set to the
                                               currently focused item
        - GFX_GOL_OBJECT_ACTION_INVALID      – Object is not affected
 
    Example:
        None.
        
*/
// *****************************************************************************
GFX_GOL_TRANSLATED_ACTION GFX_GOL_ListBoxActionGet(
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage);

// *****************************************************************************
/*  
    <GROUP gol_listbox_object>

    Function:
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
        The following state changes are supported:
 	<TABLE>
    	Translated Message                  Input Source	Set/Clear State Bit                     Description
     	##################                  ############	###################                     ###########
        GFX_GOL_LISTBOX_ACTION_TOUCHSCREEN  Touch Screen    Set GFX_GOL_LISTBOX_FOCUSED_STATE,      If focus is enabled, the focus state bit GFX_GOL_LISTBOX_FOCUSED_STATE will be set.
                                                            Set GFX_GOL_LISTBOX_DRAW_FOCUS_STATE    GFX_GOL_LISTBOX_DRAW_FOCUS_STATE draw state bit will force the List Box to be redrawn with focus.
                                                            Set GFX_GOL_LISTBOX_DRAW_ITEMS_STATE    List Box will be redrawn with selected item(s).
        GFX_GOL_LISTBOX_ACTION_MOVE         KeyBoard        Set GFX_GOL_LISTBOX_DRAW_ITEMS_STATE    List Box will be redrawn with focus on one item.
        GFX_GOL_LISTBOX_ACTION_SELECTED     KeyBoard        Set GFX_GOL_LISTBOX_DRAW_ITEMS_STATE    List Box will be redrawn with selection on the current item focused.
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
void GFX_GOL_ListBoxActionSet(
                                GFX_GOL_TRANSLATED_ACTION translatedMsg,
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage);

// *****************************************************************************
/*  
    <GROUP gol_listbox_object>

    Function:
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
GFX_STATUS GFX_GOL_ListBoxDraw(void *pObject);

#endif // _GFX_GOL_LISTBOX_H
