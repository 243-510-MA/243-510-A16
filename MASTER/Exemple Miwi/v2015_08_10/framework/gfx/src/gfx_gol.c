/*******************************************************************************
 Module for Microchip Graphics Library - Graphic Object Layer

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_gol.c

  Summary:
    Graphics Object Layer of the Microchip Graphics Library.

  Description:
    This module implements the common routines for the Graphics Object Layer
    of the Microchip Graphics Library. The routines are independent of the
    Display Driver Layer and should be compatible with any Display Driver
    that is compliant with the requirements of the Display Driver
    Layer of the Graphics Library.
    The module utilizes the Graphics Primitive Layer to render the objects.
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

#include "gfx/gfx.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Function pointer to the message callback function
static GFX_GOL_MESSAGE_CALLBACK_FUNC    pGfxGOLMessageCallbackFunction = NULL;

// Function pointer to the draw callback function
static GFX_GOL_DRAW_CALLBACK_FUNC       pGfxGOLDrawCallbackFunction = NULL;

// Pointer to the current linked list of objects displayed and receiving messages
static GFX_GOL_OBJ_HEADER               *pGfxGolObjectList = NULL;

// Pointer to the object receiving keyboard input
static GFX_GOL_OBJ_HEADER               *pGfxObjectFocused = NULL; 

// Variables for panel drawing. Used by GFX_GOL_PanelDraw
static GOL_PANEL_PARAM                  GfxPanel;

/////////////////////// LOCAL FUNCTIONS PROTOTYPES ////////////////////////////
bool GFX_GOL_DefaultObjectDrawCallback(void);


// *****************************************************************************
/*  Function:
    void GFX_GOL_MessageCallbackSet(
                                GFX_GOL_MESSAGE_CALLBACK_FUNC pFunc)
    Summary:
        This function sets the message callback function that the
        application will use to evaluate user inputs that affects
        the objects and application behavior.

    Description:
        This function sets the message callback function that the
        application will use to evaluate user inputs that affects
        the objects and application behavior. The callback function
        location is specified by the function pointer supplied in
        the call. See GFX_GOL_MESSAGE_CALLBACK_FUNC definition for
        details on the message callback function.

*/
// *****************************************************************************
void GFX_GOL_MessageCallbackSet( GFX_GOL_MESSAGE_CALLBACK_FUNC pFunc)
{
    pGfxGOLMessageCallbackFunction = pFunc;
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_DrawCallbackSet(
                                GFX_GOL_DRAW_CALLBACK_FUNC pFunc)
    Summary:
        This function sets the draw callback function that the
        application will use to render application specific rendering.

    Description:
        This function sets the draw callback function that the
        application will use to call primitive function to implement
        application specific shapes.See GFX_GOL_DRAW_CALLBACK_FUNC
        definition for details on the draw callback function.

*/
// *****************************************************************************
void GFX_GOL_DrawCallbackSet( GFX_GOL_DRAW_CALLBACK_FUNC pFunc)
{
    pGfxGOLDrawCallbackFunction = pFunc;
}

#ifndef GFX_CONFIG_FOCUS_DISABLE

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_GOL_ObjectCanBeFocused(GFX_GOL_OBJ_HEADER *pObject)

    Summary:
        Checks if the object can be focused.

    Description:
        This function checks if the object can be focused or not.
        If the object can be focused, it returns GFX_STATUS_SUCCESS.
        If it cannot be focused, it returns GFX_STATUS_FAILURE.
        Selected objects have the focus feature. Refer to the object
        documentation for details.

        Objects that do not support focus feature will ignore any focus
        settings.

        If the object is disabled it cannot be set to focused state.

*/
// *****************************************************************************
GFX_STATUS GFX_GOL_ObjectCanBeFocused(GFX_GOL_OBJ_HEADER *pObject)
{
    if
    (
        (pObject->type == GFX_GOL_BUTTON_TYPE)      ||
        (pObject->type == GFX_GOL_CHECKBOX_TYPE)    ||
        (pObject->type == GFX_GOL_RADIOBUTTON_TYPE) ||
        (pObject->type == GFX_GOL_LISTBOX_TYPE)     ||
        (pObject->type == GFX_GOL_SCROLLBAR_TYPE)   ||
        (pObject->type == GFX_GOL_EDITBOX_TYPE)
    )
    {
        if(!GFX_GOL_ObjectStateGet(pObject, GFX_GOL_DISABLED))
            return (GFX_STATUS_SUCCESS);
    }

    return (GFX_STATUS_FAILURE);
}

// *****************************************************************************
/*  Function:
    GFX_GOL_OBJ_HEADER *GFX_GOL_ObjectFocusPrevGet(void)

    Summary:
        This function returns the pointer to the previous object
        in the active list of objects which can be focused.

    Description:
        This function returns the pointer to the previous object
        in the active list of objects which can be focused.

        The reference point is the currently focused object. If there
        is no currently focused object, the searched starts from the
        beginning of the active list of objects.

        Objects that can be focused are those objects that can receive
        keyboard inputs.

        If there is no object capable of receiving keyboard
        inputs (i.e. none can be focused) NULL is returned.

*/
// *****************************************************************************
GFX_GOL_OBJ_HEADER *GFX_GOL_ObjectFocusPrevGet(void)
{
    GFX_GOL_OBJ_HEADER  *pPrevObj;
    GFX_GOL_OBJ_HEADER  *pCurObj;
    GFX_GOL_OBJ_HEADER  *pFocusedObj;

    if(pGfxGolObjectList == NULL)
        return (NULL);

    if(pGfxObjectFocused == NULL)
    {
        pFocusedObj = pGfxGolObjectList;
    }
    else
    {
        pFocusedObj = pGfxObjectFocused;
    }

    pPrevObj = NULL;
    pCurObj = pFocusedObj;

    while(1)
    {
        if(GFX_GOL_ObjectCanBeFocused(pCurObj) == GFX_STATUS_SUCCESS)
            pPrevObj = pCurObj;

        if(pCurObj->pNxtObj == NULL)
            if(pGfxGolObjectList == pFocusedObj)
                return (pPrevObj);

        if(pCurObj->pNxtObj == pFocusedObj)
            return (pPrevObj);

        pCurObj = (GFX_GOL_OBJ_HEADER *)pCurObj->pNxtObj;

        if(pCurObj == NULL)
            pCurObj = pGfxGolObjectList;
    }
}

// *****************************************************************************
/*  Function:
    GFX_GOL_OBJ_HEADER *GFX_GOL_ObjectFocusNextGet(void)

    Summary:
        This function returns the pointer to the next object
        in the active list of objects which can be focused.

    Description:
        This function returns the pointer to the next object
        in the active list of objects which can be focused.

        The reference point is the currently focused object. If there
        is no currently focused object, the searched starts from the
        beginning of the active list of objects.

        Objects that can be focused are those objects that can receive
        keyboard inputs.

        If there is no object capable of receiving keyboard
        inputs (i.e. none can be focused) NULL is returned.

*/
// *****************************************************************************
GFX_GOL_OBJ_HEADER *GFX_GOL_ObjectFocusNextGet(void)
{
    GFX_GOL_OBJ_HEADER  *pNextObj, *pObjStart;

    if(pGfxGolObjectList == NULL)
        return (NULL);

    if(pGfxObjectFocused == NULL)
    {
        pNextObj = pGfxGolObjectList;
    }
    else
    {
        pNextObj = pGfxObjectFocused;
    }
    pObjStart = pNextObj;
	
    do
    {
        pNextObj = (GFX_GOL_OBJ_HEADER *)pNextObj->pNxtObj;

        if(pNextObj == NULL)
            pNextObj = pGfxGolObjectList;

        if(GFX_GOL_ObjectCanBeFocused(pNextObj) == GFX_STATUS_SUCCESS)
            break;
    } while(pNextObj != pObjStart);

	// if we reached the starting point and the starting point cannot
	// be focused, then all objects cannot be focused. return NULL
    if(GFX_GOL_ObjectCanBeFocused(pNextObj) == GFX_STATUS_FAILURE)
        return NULL;

    return (pNextObj);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS *GFX_GOL_ObjectFocusSet(GFX_GOL_OBJ_HEADER *pObject)

    Summary:
        This function sets the object to be focused.

    Description:
        This function sets the specified object to be focused.

        If the object cannot accept keyboard messages, the object will
        not be set to focused state. If the object can accept keyboard
        messages, then the focus state will be set and will be marked
        to be redrawn to show the focus when the focus feature is enabled.

        Objects that can be focused are those objects that can receive
        keyboard inputs.

*/
// *****************************************************************************
GFX_STATUS GFX_GOL_ObjectFocusSet(GFX_GOL_OBJ_HEADER *pObject)
{
    if (GFX_GOL_ObjectCanBeFocused(pObject) == GFX_STATUS_FAILURE)
        return GFX_STATUS_FAILURE;

    if (pGfxObjectFocused != NULL) {
        GFX_GOL_ObjectStateClear(pGfxObjectFocused, GFX_GOL_FOCUSED);
        GFX_GOL_ObjectStateSet(pGfxObjectFocused, GFX_GOL_DRAW_FOCUS);
    }

    GFX_GOL_ObjectStateSet(pObject, GFX_GOL_DRAW_FOCUS | GFX_GOL_FOCUSED);
    pGfxObjectFocused = pObject;

    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    GFX_GOL_OBJ_HEADER *GFX_GOL_ObjectFocusGet(void)

    Summary:
        This function returns the pointer to the object that is currently
        receiving keyboard input (or focused).

    Description:
        This function returns the pointer to the object that is currently
        receiving keyboard input (or focused).

        If there are no object that can accept keyboard messages, then the
        function will return NULL.

        Objects that can be focused are those objects that can receive
        keyboard inputs.
    
*/
// *****************************************************************************
GFX_GOL_OBJ_HEADER __attribute__ ((always_inline)) *GFX_GOL_ObjectFocusGet(void)
{
    return (pGfxObjectFocused);
}

#endif //#ifndef GFX_CONFIG_FOCUS_DISABLE

// *****************************************************************************
/*  Function:
    GFX_GOL_OBJ_TYPE GFX_GOL_ObjectTypeGet(GFX_GOL_OBJ_HEADER *pObject)

    Summary:
        This function returns the object type.

    Description:
        This function returns the object type. The object type is one
        of the defined enumerated types of GFX_GOL_OBJ_TYPE.

*/
// *****************************************************************************
GFX_GOL_OBJ_TYPE __attribute__ ((always_inline)) GFX_GOL_ObjectTypeGet(
                            GFX_GOL_OBJ_HEADER *pObject)
{
    return (pObject->type);
}

// *****************************************************************************
/*  Function:
    uint16_t GFX_GOL_ObjectIDGet(GFX_GOL_OBJ_HEADER *pObject)

    Summary:
        This function returns the object ID.

    Description:
        This function returns the user defined ID assigned to the object.

*/
// *****************************************************************************
uint16_t __attribute__ ((always_inline)) GFX_GOL_ObjectIDGet(GFX_GOL_OBJ_HEADER *pObject)
{
    return (pObject->ID);
}

// *****************************************************************************
/*  Function:
    GFX_GOL_OBJ_HEADER *GFX_GOL_ObjectNextGet(GFX_GOL_OBJ_HEADER *pObject)

    Summary:
        This function returns the next object in the list after the
        specified object.

    Description:
        This function returns the next object in the list after the
        specified object.

*/
// *****************************************************************************
GFX_GOL_OBJ_HEADER __attribute__ ((always_inline)) *GFX_GOL_ObjectNextGet(GFX_GOL_OBJ_HEADER *pObject)
{
    return (pObject->pNxtObj);
}

// *****************************************************************************
/*  Function:
    GFX_GOL_OBJ_HEADER *GFX_GOL_ObjectFind(uint16_t ID)

    Summary:
        This function returns the pointer to object in the list
        with the user defined ID assigned to it.

    Description:
        This function returns the pointer to object in the list
        with the user defined ID assigned to it.

*/
// *****************************************************************************
GFX_GOL_OBJ_HEADER *GFX_GOL_ObjectFind(uint16_t ID)
{
    GFX_GOL_OBJ_HEADER  *pNextObj;

    pNextObj = pGfxGolObjectList;
    while(pNextObj != NULL)
    {
        if(pNextObj->ID == ID)
        {
            return (pNextObj);
        }

        pNextObj = (GFX_GOL_OBJ_HEADER *)pNextObj->pNxtObj;
    }

    return (NULL);
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_ObjectAdd(GFX_GOL_OBJ_HEADER *pObject)

    Summary:
        This function adds an object to the tail of the
        currently active list.

    Description:
        This function adds an object to the tail of the
        currently active list. The new list tail is set
        to point to NULL after the new object is added.
 
*/
// *****************************************************************************
void GFX_GOL_ObjectAdd(GFX_GOL_OBJ_HEADER *pObject)
{
    GFX_GOL_OBJ_HEADER  *pNextObj;

    if(pGfxGolObjectList == NULL)
    {
        pGfxGolObjectList = pObject;
    }
    else
    {
        pNextObj = pGfxGolObjectList;
        while(pNextObj->pNxtObj != NULL)
        {
            pNextObj = (GFX_GOL_OBJ_HEADER *)pNextObj->pNxtObj;
        }

        pNextObj->pNxtObj = (void *)pObject;
    }

    pObject->pNxtObj = NULL;
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_GOL_ObjectDelete(GFX_GOL_OBJ_HEADER *pObject)

    Summary:
        This function removes an object from the currently
        active list.

    Description:
        This function removes an object from the currently
        active list. Aside from the removal of the object
        from the list, the RAM resources consumed by the object
        is also freed.

*/
// *****************************************************************************
GFX_STATUS GFX_GOL_ObjectDelete(GFX_GOL_OBJ_HEADER *pObject)
{
    if(!pGfxGolObjectList)
        return (false);

    if(pObject == pGfxGolObjectList)
    {
        pGfxGolObjectList = (GFX_GOL_OBJ_HEADER *)pObject->pNxtObj;
    }
    else
    {
        GFX_GOL_OBJ_HEADER  *curr;
        GFX_GOL_OBJ_HEADER  *prev;

        curr = (GFX_GOL_OBJ_HEADER *)pGfxGolObjectList->pNxtObj;
        prev = (GFX_GOL_OBJ_HEADER *)pGfxGolObjectList;

        while(curr)
        {
            if(curr == pObject)
                break;

            prev = (GFX_GOL_OBJ_HEADER *)curr;
            curr = (GFX_GOL_OBJ_HEADER *)curr->pNxtObj;
        }

        if(!curr)
            return (false);

        prev->pNxtObj = curr->pNxtObj;
    }

    if(pObject->FreeObj)
        pObject->FreeObj(pObject);

    GFX_free(pObject);

    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_GOL_ObjectByIDDelete(uint16_t id)

    Summary:
        This function removes an object with the given user
        defined ID from the currently active list.

    Description:
        This function removes an object with the given user
        defined ID from the currently active list.
        Aside from the removal of the object from the list,
        the RAM resources consumed by the object is also freed.

        If there is no object with the given ID, the function exits
        with GFX_STATUS_FAILURE.

*/
// *****************************************************************************
GFX_STATUS GFX_GOL_ObjectByIDDelete(uint16_t id)
{
    GFX_GOL_OBJ_HEADER  *pObject;

    pObject = GFX_GOL_ObjectFind(id);

    if(!pObject)
        return (GFX_STATUS_FAILURE);

    return (GFX_GOL_ObjectDelete(pObject));
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_GOL_ObjectListNew(void)

    Summary:
        This function removes an object with the given user
        defined ID from the currently active list.

    Description:
        This function starts a new linked list of objects and
        resets the keyboard focus to none. This function assigns
        the current active list and current focused object
        (receiving keyboard inputs) object pointers to NULL.
        Any keyboard inputs at this point will be ignored.

        This function does not erase the objects in the previous
        list. Application must save the previous list to
        another pointer if to be referenced later. If not needed
        anymore, memory used by that list should be freed by
        GFX_GOL_ObjectListFree() function. In this case, freeing
        the list with GFX_GOL_ObjectListFree() function has the same
        effect as GFX_GOL_ObjectListNew() where the current active
        list is empty.
   
*/
// *****************************************************************************
GFX_STATUS __attribute__ ((always_inline)) GFX_GOL_ObjectListNew(void)
{
    pGfxGolObjectList    = NULL;
    pGfxObjectFocused = NULL;
    return (GFX_STATUS_SUCCESS);
}


// *****************************************************************************
/*  Function:
    GFX_GOL_OBJ_HEADER *GFX_GOL_ObjectListGet(void)

    Summary:
        This function returns the current active list.

    Description:
        This function returns the pointer to the current active.

*/
// *****************************************************************************
GFX_GOL_OBJ_HEADER __attribute__ ((always_inline)) *GFX_GOL_ObjectListGet(void)
{
    return (pGfxGolObjectList);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS *GFX_GOL_ObjectListSet(GFX_GOL_OBJ_HEADER *pList)

    Summary:
        This function sets the active list to the new list.

    Description:
        This function sets the active list to the new list.
        The previous list will still exist in memory. Application
        must save the previous list before the set is called if the
        previous list will be referenced later. If the previous list
        is not needed anymore, then the list must be removed from
        memory by GFX_GOL_ObjectListFree() function.

        Setting the active list to the new list will reset the focused
        pointer object to NULL.

*/
// *****************************************************************************
GFX_STATUS __attribute__ ((always_inline)) GFX_GOL_ObjectListSet(
                                GFX_GOL_OBJ_HEADER *pList)
{
    pGfxGolObjectList    = pList;
    pGfxObjectFocused = NULL;
    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_GOL_ObjectListFree(void)

    Summary:
        This function sets the active list to the new list.

    Description:
        This function frees all the memory used by objects in
        the active list and initializes the active list pointer
        to NULL to start a new empty list. This function must
        be called only inside the GFX_GOL_ObjectDrawCallback()
        function when using GFX_GOL_ObjectListDraw() and
        GFX_GOL_ObjectMessage() functions. This requirement
        assures that primitive rendering settings are not altered
        by the rendering state machines of the objects.

*/
// *****************************************************************************
GFX_STATUS GFX_GOL_ObjectListFree(void)
{
    GFX_GOL_OBJ_HEADER  *pNextObj;
    GFX_GOL_OBJ_HEADER  *pCurrentObj;

    pCurrentObj = pGfxGolObjectList;
    while(pCurrentObj != NULL)
    {
        pNextObj = (GFX_GOL_OBJ_HEADER *)pCurrentObj->pNxtObj;

        // check if there are additional items to free
        if(pCurrentObj->FreeObj)
            pCurrentObj->FreeObj(pCurrentObj);

        GFX_free(pCurrentObj);
        pCurrentObj = pNextObj;
    }

    GFX_GOL_ObjectListNew();

    return (GFX_STATUS_SUCCESS);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_GOL_ObjectListDraw(void)

    Summary:
        This function redraws all objects in the current active list
        that has the rendering state bits set.

    Description:
        This function loops through the active list and redraws
        objects that need to be redrawn. Partial redrawing or
        full redraw is performed depending on the drawing states
        of the objects.

        GFX_GOL_ObjectDrawCallback() function is called by
        GFX_GOL_ObjectListDraw() when drawing of objects in the
        active list is completed. GFX_GOL_ObjectDrawCallback()
        is an application implemented function that allows the
        application the opportunity to insert application specific
        rendering using Primitive Layer rendering functions.

        The GFX_GOL_ObjectListDraw() function can return with 
        GFX_STATUS_FAILURE. In this case, it indicates that the currently
        redrawn object is not able to continue. Application needs to call
        GFX_GOL_ObjectListDraw() again to continue the redraw of the objects
        in the list.

*/
// *****************************************************************************
GFX_STATUS GFX_GOL_ObjectListDraw(void)
{
    static GFX_GOL_OBJ_HEADER  *pCurrentObj = NULL;
           GFX_STATUS          status;

#ifndef GFX_CONFIG_DOUBLE_BUFFERING_DISABLE
    static uint8_t         DisplayUpdated = 0;

    // check if an update is scheduled. If yes, exit since the frame and draw
    // buffers are not yet synchronized.
    if(GFX_DoubleBufferSynchronizeStatusGet() == GFX_FEATURE_ENABLED)
    {
        return (GFX_STATUS_FAILURE);
    }
#endif

    // When we have reached the end of the list check to see
    // if a callback exists. If it does call the callback
    if(pCurrentObj == NULL)
    {
        if (pGfxGOLDrawCallbackFunction != NULL)
        {
            if(pGfxGOLDrawCallbackFunction() == true)
            {
                // It's last object jump to head
                pCurrentObj = pGfxGolObjectList;

#ifndef GFX_CONFIG_DOUBLE_BUFFERING_DISABLE
                if(DisplayUpdated)
                {
                    GFX_DoubleBufferSynchronizeRequest();
                    DisplayUpdated = 0;
                    return(GFX_STATUS_FAILURE);
                }
#endif
            }
            else
            {
                // drawing is not done
                return (GFX_STATUS_FAILURE);
            }
        }
        else
        {
            return (GFX_STATUS_SUCCESS);
        }
    }

    status = GFX_STATUS_FAILURE;

    // this is the loop that parses the list and redraws any
    // object that needs to be redrawn.
    while(pCurrentObj != NULL)
    {
        if(GFX_GOL_ObjectIsRedrawSet(pCurrentObj) == true)
        {
            status = pCurrentObj->DrawObj(pCurrentObj);
            
            if(status == GFX_STATUS_SUCCESS)
            {
                GFX_GOL_ObjectDrawDisable(pCurrentObj);

#ifndef GFX_CONFIG_DOUBLE_BUFFERING_DISABLE
                GFX_DoubleBufferAreaMark(   pCurrentObj->left,          \
                                            pCurrentObj->top,           \
                                            pCurrentObj->right,         \
                                            pCurrentObj->bottom);
                DisplayUpdated = 1;
#endif
            }
            else
            {
                // drawing is not done
                return (GFX_STATUS_FAILURE);
            }
        }
        pCurrentObj = (GFX_GOL_OBJ_HEADER *)pCurrentObj->pNxtObj;
    }

    return (GFX_STATUS_SUCCESS);             // drawing is completed

}

// *****************************************************************************
/*  Function:
    void GFX_GOL_ObjectRectangleRedraw(
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom)

    Summary:
        This function marks all objects in the active list intersected
        by the given rectangular area to be redrawn.

    Description:
        This function marks all objects in the active list intersected
        by the given rectangular area to be redrawn.

        After calling this function, the next call to GFX_GOL_ObjectListDraw()
        will redraw all objects that are marked for redraw.
    
*/
// *****************************************************************************
void GFX_GOL_ObjectRectangleRedraw(
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom)
{

    GFX_GOL_OBJ_HEADER  *pCurrentObj;
    int                 overlapX, overlapY;

    pCurrentObj = pGfxGolObjectList;

    while(pCurrentObj != NULL)
    {
        overlapX = overlapY = 0;

        // check overlaps in x direction
        if (((pCurrentObj->left <= left)  && (pCurrentObj->right >= left))  ||   \
            ((pCurrentObj->left <= right) && (pCurrentObj->right >= right)) ||   \
            ((pCurrentObj->left <= left)  && (pCurrentObj->right >= right)) ||   \
            ((pCurrentObj->left >= left)  && (pCurrentObj->right <= right))      \
          )
            overlapX = 1;

        // check overlaps in y direction
        if (((pCurrentObj->top <= top)    && (pCurrentObj->bottom >= top))    ||   \
            ((pCurrentObj->top <= bottom) && (pCurrentObj->bottom >= bottom)) ||   \
            ((pCurrentObj->top <= top)    && (pCurrentObj->bottom >= bottom)) ||   \
            ((pCurrentObj->top >= top)    && (pCurrentObj->bottom <= bottom))      \
          )
            overlapY = 1;

        // When any portion of the widget is touched by the defined
        // rectangle the x and y overlaps will exist.
        if (overlapX & overlapY) {
            GFX_GOL_ObjectDrawEnable(pCurrentObj);
        }

        pCurrentObj = (GFX_GOL_OBJ_HEADER *)pCurrentObj->pNxtObj;
    }   //end of while

}

// *****************************************************************************
/*  Function:
    void GFX_GOL_ObjectListHide(void)

    Summary:
        This function marks all objects in the active list to be hidden.

    Description:
        This function marks all objects in the active list to be hidden.

        After calling this function, the next call to GFX_GOL_ObjectListDraw()
        will hide all objects.

*/
// *****************************************************************************
void GFX_GOL_ObjectListHide(void)
{

    GFX_GOL_OBJ_HEADER  *pCurrentObj;

    pCurrentObj = pGfxGolObjectList;

    while(pCurrentObj != NULL)
    {
        // set the hide state bit
        pCurrentObj->state |= 0x8000;
        pCurrentObj = (GFX_GOL_OBJ_HEADER *)pCurrentObj->pNxtObj;
    }   //end of while

}

// *****************************************************************************
/*  Function:
    void GFX_GOL_ObjectDrawEnable(GFX_GOL_OBJ_HEADER *pObject)

    Summary:
        This function sets the object to be redraw.

    Description:
        This function sets the object to be redrawn. For the redraw
        to be effective, the object must be in the current active list.
        If not, the redraw action will not be performed until the list
        where the object is currently inserted will be set as the active
        list.

*/
// *****************************************************************************
void __attribute__((always_inline)) GFX_GOL_ObjectDrawEnable(GFX_GOL_OBJ_HEADER *pObject)
{
    pObject->state |= 0x7c00;
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_ObjectDrawDisable(GFX_GOL_OBJ_HEADER *pObject)

    Summary:
        This function resets the drawing state bits of the object.

    Description:
        This function resets the drawing state bits of the object.
        This function can be called to cancel any drawing state bits
        that has been set or clears all the drawing state bits after
        the object has been redrawn.

*/
// *****************************************************************************
void __attribute__ ((always_inline)) GFX_GOL_ObjectDrawDisable(GFX_GOL_OBJ_HEADER *pObject)
{
    pObject->state &= 0x03ff;
}

// *****************************************************************************
/*  Function:
    bool GFX_GOL_ObjectIsRedrawSet(
                                GFX_GOL_OBJ_HEADER *pObject)

    Summary:
        This function checks if the object needs to be redrawn or not.

    Description:
        This function checks if the object needs to be redrawn or not.
        The function returns true if it is to be redrawn
        or false if it is not to be redrawn.

*/
// *****************************************************************************
bool __attribute__ ((always_inline)) GFX_GOL_ObjectIsRedrawSet(
                                GFX_GOL_OBJ_HEADER *pObject)
{
    if ((uint16_t)(pObject->state & 0xfc00) > 0x00)
        return (true);
    else
        return (false);
}

// *****************************************************************************
/*  Function:
    void  GFX_GOL_ObjectMessage(GFX_GOL_MESSAGE *pMessage)

    Summary:
        This function process the received message from the user
        to determine the affected objects. Depending on the message
        and the affected objects, object states are modified based
        on the default behaviour or user defined behaviour.

    Description:
        This function receives a GFX_GOL_MESSAGE message from user and
        loops through the active list of objects to check which object
        is affected by the message. For affected objects the message
        is translated and GFX_GOL_ObjectMessageCallback() is called.
        In the call back function, user has the ability to implement
        action for the message. If the call back function returns
        non-zero, OBJMsgDefault() is called to process message for
        the object by default.
        If zero is returned OBJMsgDefault() is not called.
        Please refer to GOL Messages section for details.

*/
// *****************************************************************************
void GFX_GOL_ObjectMessage(GFX_GOL_MESSAGE *pMsg)
{
    GFX_GOL_OBJ_HEADER        *pCurrentObj;
    GFX_GOL_TRANSLATED_ACTION  translatedMsg;

    if(pMsg->uiEvent == EVENT_INVALID)
        return;

    pCurrentObj = pGfxGolObjectList;

    while(pCurrentObj != NULL)
    {
        if(pCurrentObj->actionGet)
        {
            translatedMsg = pCurrentObj->actionGet(pCurrentObj, pMsg);

            if(translatedMsg != GFX_GOL_OBJECT_ACTION_INVALID)
            {

                // if callback function exists call the callback
                if(pGfxGOLMessageCallbackFunction != NULL)
                {
                    if(pGfxGOLMessageCallbackFunction(translatedMsg, pCurrentObj, pMsg) == true)
                        if(pCurrentObj->actionSet)
                            pCurrentObj->actionSet(translatedMsg, pCurrentObj, pMsg);
                }
            }
        }

        pCurrentObj = (GFX_GOL_OBJ_HEADER *)pCurrentObj->pNxtObj;
    }
}

// *****************************************************************************
/*  Function:
    void GFX_Initialize(void)

    Summary:
        Initialize the Graphics Library.

    Description:
        This function initialize the Graphics Library primitive layer and
        Graphics Object Layer if it is enabled. The following default settings
        are set when this function is called.
        1. color - Set to 0 (see GFX_COLOR).
        2. font - Set to NULL. GFX_FontSet() must be called prior to any text
                 rendering.
        3. line type - Set to GFX_LINE_TYPE_THIN_SOLID (see GFX_LINE_STYLE).
        4. fill type - Set to GFX_FILL_TYPE_COLOR (see GFX_FILL_STYLE).
        5. text anti-alias type - Set to GFX_FONT_ANTIALIAS_OPAQUE
                 (see  GFX_FONT_ANTIALIAS_TYPE). This only
                 affects fonts with anti-aliasing enabled.

*/
// *****************************************************************************
void  GFX_Initialize(void)
{

    // Initialize the primitive layer
    GFX_Primitive_Initialize();

    GFX_GOL_DrawCallbackSet(GFX_GOL_DefaultObjectDrawCallback);

}

// *****************************************************************************
/*  Function:
    void GFX_GOL_PanelParameterSet(
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom,
                                uint16_t radius,
                                GFX_COLOR faceClr,
                                GFX_COLOR embossLtClr,
                                GFX_COLOR embossDkClr,
                                GFX_RESOURCE_HDR *pBitmap,
                                GFX_FILL_STYLE fillStyle,
                                uint16_t embossSize)

    Summary:
        This function sets the parameters to draw a panel.

    Description:
        This function sets the parameters to draw a panel.
        Panel is not an object. It is a routine to draw a basic
        component of objects. The actual drawing of the panel
        is performed by the GFX_GOL_PanelDraw(). After the parameters
        are set, call GFX_GOL_PanelDraw() to render the panel.
        The panel is drawn using the following:
        1. Panel width is determined by right - left.
        2. Panel height is determined by top - bottom.
        3. Panel radius - specifies if the panel will have a rounded
                          edge. If zero then the panel will have
                          sharp (cornered) edge.
        4. If 2*radius = height = width, the panel is circular.
        5. If the panel is drawn with an image, pBitmap should point
           to an image resource.
        6. If the panel face is drawn with the fill style specified
           by fillStyle. When gradient fill is used, set the gradient
           colors using GFX_GOL_PanelGradientParameterSet(). When
           alpha blending fill is used, set the alpha blending value
           using GFX_GOL_PanelAlphaParameterSet().

*/
// *****************************************************************************
void __attribute__ ((always_inline))  GFX_GOL_PanelParameterSet(
                                uint16_t left,
                                uint16_t top,
                                uint16_t right,
                                uint16_t bottom,
                                uint16_t radius,
                                GFX_COLOR faceClr,
                                GFX_COLOR embossLtClr,
                                GFX_COLOR embossDkClr,
                                GFX_RESOURCE_HDR *pBitmap,
                                GFX_FILL_STYLE fillStyle,
                                uint16_t embossSize)
{
        GfxPanel.panelLeft          = left;
        GfxPanel.panelTop           = top;
        GfxPanel.panelRight         = right;
        GfxPanel.panelBottom        = bottom;
        GfxPanel.panelRadius        = radius;
        GfxPanel.panelFaceColor     = faceClr;
        GfxPanel.panelEmbossLtColor = embossLtClr;
        GfxPanel.panelEmbossDkColor = embossDkClr;
        GfxPanel.pPanelImage        = pBitmap;
        GfxPanel.panelEmbossSize    = embossSize;
        GfxPanel.panelFillStyle     = fillStyle;

}

// *****************************************************************************
/*  Function:
    void GFX_GOL_PanelBackgroundSet(GFX_GOL_OBJ_HEADER *pObjectHeader)

    Summary:
        This function sets panel background information.

    Description:
        This function sets panel background information.
        This is an internal function and should not be called
        by the application.

*/
// *****************************************************************************
void __attribute__((always_inline)) GFX_GOL_PanelBackgroundSet(GFX_GOL_OBJ_HEADER *pObjectHeader)
{

#ifndef GFX_CONFIG_GRADIENT_DISABLE
    GFX_GOL_PanelGradientParameterSet(
            pObjectHeader->pGolScheme->gradientStartColor,
            pObjectHeader->pGolScheme->gradientEndColor);
#endif

#ifndef GFX_CONFIG_ALPHABLEND_DISABLE
    GFX_GOL_PanelAlphaParameterSet(pObjectHeader->pGolScheme->AlphaValue);
#endif
    // set the background information
    GFX_BackgroundSet(
        pObjectHeader->pGolScheme->CommonBkLeft,
        pObjectHeader->pGolScheme->CommonBkTop,
        pObjectHeader->pGolScheme->pCommonBkImage,
        pObjectHeader->pGolScheme->CommonBkColor);

    GFX_BackgroundTypeSet(pObjectHeader->pGolScheme->CommonBkType);

}


// *****************************************************************************
/*  Function:
    void GFX_GOL_PanelGradientParameterSet(
                                GFX_COLOR startColor,
                                GFX_COLOR endColor)

    Summary:
        This function sets the gradient fill start and end colors
        of a panel.

    Description:
        This function sets the gradient fill start and end colors
        of a panel. This along with the parameters set by the function
        GFX_GOL_PanelParameterSet() will determine how the panel will
        be drawn. The actual drawing of the panel is performed by
        GFX_GOL_PanelDraw().

*/
// *****************************************************************************
#ifndef GFX_CONFIG_GRADIENT_DISABLE
void __attribute__ ((always_inline)) GFX_GOL_PanelGradientParameterSet(
                                GFX_COLOR startColor,
                                GFX_COLOR endColor)
{
    GfxPanel.panelGradientStartColor = startColor;
    GfxPanel.panelGradientEndColor   = endColor;
}
#endif

// *****************************************************************************
/*  Function:
    void GFX_GOL_PanelAlphaParameterSet(
                                uint16_t alphaValue)

    Summary:
        This function sets the alpha blending value when using
        alpha blending in panels.

    Description:
        This function sets the alpha blending value when using
        alpha blending in panels. This along with the parameters set
        by the function GFX_GOL_PanelParameterSet() will determine
        how the panel will be drawn. The actual drawing of the panel
        is performed by GFX_GOL_PanelDraw().

*/
// *****************************************************************************
#ifndef GFX_CONFIG_ALPHABLEND_DISABLE
void __attribute__ ((always_inline)) GFX_GOL_PanelAlphaParameterSet(
                                uint16_t alphaValue)
{
    GfxPanel.panelAlpha = alphaValue;
}
#endif

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_GOL_PanelDraw(void)

    Summary:
        This function draws the panel.

    Description:
        This function draws the panel used by object rendering functions.
        This function implements both blocking and non-blocking modes.
        This function integrates, alpha blending as well as gradient
        fills on the panel.

*/
// *****************************************************************************
GFX_STATUS GFX_GOL_PanelDraw(void)
{

#ifdef GFX_CONFIG_NONBLOCKING_DISABLE
    
    uint16_t    counter;

    if (GfxPanel.panelEmbossSize > 0)
    {
        if(GfxPanel.panelRadius)
	{
            // draw upper left portion of the embossed area
	    GFX_ColorSet(GfxPanel.panelEmbossLtColor);
	    while(GFX_ThickBevelDraw(
                GfxPanel.panelLeft, GfxPanel.panelTop,
                GfxPanel.panelRight, GfxPanel.panelBottom,
                GfxPanel.panelRadius - GfxPanel.panelEmbossSize,
                GfxPanel.panelRadius, 0xE1) != GFX_STATUS_SUCCESS);

            // draw lower right portion of the embossed area
            GFX_ColorSet(GfxPanel.panelEmbossDkColor);
            while (GFX_ThickBevelDraw(
                    GfxPanel.panelLeft, GfxPanel.panelTop,
                    GfxPanel.panelRight, GfxPanel.panelBottom,
                    GfxPanel.panelRadius - GfxPanel.panelEmbossSize,
                    GfxPanel.panelRadius, 0x1E) != GFX_STATUS_SUCCESS);
        }
	else
	{
	
            // object is rectangular panel draw the embossed areas
	    counter = 1;
	    GFX_ColorSet(GfxPanel.panelEmbossLtColor);
	    while(counter < GfxPanel.panelEmbossSize)
	    {
                while(GFX_BarDraw(
                            GfxPanel.panelLeft + counter,
                            GfxPanel.panelTop + counter,
                            GfxPanel.panelRight - counter,
                            GfxPanel.panelTop + counter
                        ) != GFX_STATUS_SUCCESS);    // draw top

	       while(GFX_BarDraw(
                            GfxPanel.panelLeft + counter,
                            GfxPanel.panelTop + counter,
                            GfxPanel.panelLeft + counter,
                            GfxPanel.panelBottom - counter
                        ) != GFX_STATUS_SUCCESS);    // draw left
	            counter++;
            }
	
	    counter = 1;
	    GFX_ColorSet(GfxPanel.panelEmbossDkColor);
	    while(counter < GfxPanel.panelEmbossSize)
	    {
                while(GFX_BarDraw(
                            GfxPanel.panelLeft + counter,
                            GfxPanel.panelBottom - counter,
                            GfxPanel.panelRight - counter,
                            GfxPanel.panelBottom - counter
                        ) != GFX_STATUS_SUCCESS);    // draw bottom
                    
                while(GFX_BarDraw(
                            GfxPanel.panelRight - counter,
                            GfxPanel.panelTop + counter,
                            GfxPanel.panelRight - counter,
                            GfxPanel.panelBottom - counter
                        ) != GFX_STATUS_SUCCESS);    // draw right
	        counter++;
	    }
        }
    }
	
    // draw the face color
    GFX_ColorSet(GfxPanel.panelFaceColor);

    switch (GfxPanel.panelFillStyle)
    {
#if !defined(GFX_CONFIG_ALPHABLEND_DISABLE)
        case GFX_FILL_STYLE_ALPHA_COLOR:
            GFX_AlphaBlendingValueSet(GfxPanel.panelAlpha);
            break;
#endif

#if !defined (GFX_CONFIG_GRADIENT_DISABLE)
        case GFX_FILL_STYLE_GRADIENT_DOWN:
        case GFX_FILL_STYLE_GRADIENT_UP:
        case GFX_FILL_STYLE_GRADIENT_RIGHT:
        case GFX_FILL_STYLE_GRADIENT_LEFT:
        case GFX_FILL_STYLE_GRADIENT_DOUBLE_VER:
        case GFX_FILL_STYLE_GRADIENT_DOUBLE_HOR:
            GFX_GradientColorSet(
                    GfxPanel.panelGradientStartColor,
                    GfxPanel.panelGradientEndColor);
            break;
#endif
        case GFX_FILL_STYLE_NONE:
        case GFX_FILL_STYLE_COLOR:
        default:
            break;
    }

    GFX_FillStyleSet(GfxPanel.panelFillStyle);


    if(GfxPanel.panelRadius)
    {
        while(GFX_RectangleRoundFillDraw(
                    GfxPanel.panelLeft,
                    GfxPanel.panelTop,
                    GfxPanel.panelRight,
                    GfxPanel.panelBottom,
                    GfxPanel.panelRadius - GfxPanel.panelEmbossSize
                ) != GFX_STATUS_SUCCESS);
    }
    else
    {
        while(GFX_RectangleFillDraw(
                    GfxPanel.panelLeft + GfxPanel.panelEmbossSize,
                    GfxPanel.panelTop + GfxPanel.panelEmbossSize,
                    GfxPanel.panelRight - GfxPanel.panelEmbossSize,
                    GfxPanel.panelBottom - GfxPanel.panelEmbossSize
                ) != GFX_STATUS_SUCCESS);
    }

#if (GFX_CONFIG_COLOR_DEPTH == 1)
    if (GfxPanel.panelFaceColor == GfxPanel.panelEmbossDkColor)
    {
        GFX_ColorSet(GfxPanel.panelEmbossLtColor);
        if (GfxPanel.panelRadius)
            while (
                    GFX_RectangleRoundDraw(GfxPanel.panelLeft,
                    GfxPanel.panelTop,
                    GfxPanel.panelRight,
                    GfxPanel.panelBottom,
                    GfxPanel.panelRadius -
                    (GfxPanel.panelEmbossSize - 1))
                    != GFX_STATUS_SUCCESS);
        else
            while (
                    GFX_RectangleDraw(GfxPanel.panelLeft +
                    (GfxPanel.panelEmbossSize - 1),
                    GfxPanel.panelTop +
                    (GfxPanel.panelEmbossSize - 1),
                    GfxPanel.panelRight -
                    (GfxPanel.panelEmbossSize - 1),
                    GfxPanel.panelBottom -
                    (GfxPanel.panelEmbossSize - 1)
                    ) != GFX_STATUS_SUCCESS);
    }
	
#endif
	
    // draw bitmap
    if(GfxPanel.pPanelImage != NULL)
    {
        while(GFX_ImageDraw
        (
            (((GfxPanel.panelRight + GfxPanel.panelLeft) -
                (GFX_ImageWidthGet(GfxPanel.pPanelImage))) >> 1) + 1,
            (((GfxPanel.panelBottom + GfxPanel.panelTop) -
                (GFX_ImageHeightGet(GfxPanel.pPanelImage))) >> 1) + 1,
            GfxPanel.pPanelImage
        ) != GFX_STATUS_SUCCESS);
   }

#ifndef GFX_CONFIG_ALPHABLEND_DISABLE
    //Disable Alpha Blending
    GFX_AlphaBlendingValueSet(100);
    GFX_FillStyleSet(GFX_FILL_STYLE_COLOR);
#endif

    // check if we need to draw the frame
    if
    (
        (GfxPanel.pPanelImage == NULL) ||
        (
            ((GfxPanel.panelRight - GfxPanel.panelLeft + GfxPanel.panelRadius) >= GFX_ImageWidthGet((void *)GfxPanel.pPanelImage)) &&
            ((GfxPanel.panelBottom - GfxPanel.panelTop + GfxPanel.panelRadius) >= GFX_ImageHeightGet((void *)GfxPanel.pPanelImage))
        )
    )
    {

        // draw the outline
        GFX_ColorSet(GfxPanel.panelEmbossDkColor);
        while(GFX_BevelDraw(
                GfxPanel.panelLeft, GfxPanel.panelTop,
                GfxPanel.panelRight, GfxPanel.panelBottom,
                GfxPanel.panelRadius) != GFX_STATUS_SUCCESS);
    }

    return (GFX_STATUS_SUCCESS);

#else

    typedef enum
    {
        BEGIN,
        ARC1,
        DRAW_EMBOSS1,
        DRAW_EMBOSS2,
        DRAW_EMBOSS3,
        DRAW_EMBOSS4,
        SETUP_DRAW_FACECOLOR,
        DRAW_FACECOLOR,
#if (GFX_CONFIG_COLOR_DEPTH == 1)
        DRAW_INNERFRAME,
#endif
        DRAW_FRAME,
        DRAW_IMAGE,
    } ROUND_PANEL_DRAW_STATES;

    static ROUND_PANEL_DRAW_STATES state = BEGIN;
    static uint16_t counter;

    while(1)
    {
        if (GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
            return (GFX_STATUS_FAILURE);

        switch(state)
        {
            case BEGIN:

                if (GfxPanel.panelEmbossSize == 0)
                {
                    state = SETUP_DRAW_FACECOLOR;
                    break;
                }
                if(GfxPanel.panelRadius)
                {

                    // draw upper left portion of the embossed area
                    GFX_ColorSet(GfxPanel.panelEmbossLtColor);
                    if(GFX_ThickBevelDraw(
                            GfxPanel.panelLeft, GfxPanel.panelTop,
                            GfxPanel.panelRight, GfxPanel.panelBottom,
                            GfxPanel.panelRadius - GfxPanel.panelEmbossSize,
                            GfxPanel.panelRadius, 0xE1) == GFX_STATUS_FAILURE)
                        return (GFX_STATUS_FAILURE);
                    state = ARC1;
                }
                else
                {
                    state = DRAW_EMBOSS1;
                    counter = 1;
                    goto rnd_panel_draw_emboss;
                }

            case ARC1:

                // draw upper left portion of the embossed area
                GFX_ColorSet(GfxPanel.panelEmbossDkColor);
                if(GFX_ThickBevelDraw(
                        GfxPanel.panelLeft, GfxPanel.panelTop,
                        GfxPanel.panelRight, GfxPanel.panelBottom,
                        GfxPanel.panelRadius - GfxPanel.panelEmbossSize,
                        GfxPanel.panelRadius, 0x1E) == GFX_STATUS_FAILURE)
                {
                    return (GFX_STATUS_FAILURE);
                }
                state = SETUP_DRAW_FACECOLOR;
                goto rnd_panel_setup_facecolor;

            // now draw the upper portion of the embossed area
            case DRAW_EMBOSS1:
                rnd_panel_draw_emboss : GFX_ColorSet(GfxPanel.panelEmbossLtColor);
                while(counter < GfxPanel.panelEmbossSize)
                {

                    // draw top
                    if(GFX_BarDraw(
                        GfxPanel.panelLeft + counter,
                        GfxPanel.panelTop + counter,
                        GfxPanel.panelRight - counter,
                        GfxPanel.panelTop + counter) == GFX_STATUS_FAILURE)
                    {
                        return (GFX_STATUS_FAILURE);
                    }

                    counter++;
                }

                counter = 1;
                state = DRAW_EMBOSS2;
                break;

            case DRAW_EMBOSS2:
                while(counter < GfxPanel.panelEmbossSize)
                {

                    // draw left   	
                    if(GFX_BarDraw(
                        GfxPanel.panelLeft + counter,
                        GfxPanel.panelTop + counter,
                        GfxPanel.panelLeft + counter,
                        GfxPanel.panelBottom - counter) == GFX_STATUS_FAILURE)
                    {
                        return (GFX_STATUS_FAILURE);
                    }

                    counter++;
                }

                counter = 1;
                state = DRAW_EMBOSS3;
                break;

            // now draw the lower portion of the embossed area
            case DRAW_EMBOSS3:
                GFX_ColorSet(GfxPanel.panelEmbossDkColor);
                while(counter < GfxPanel.panelEmbossSize)
                {

                    // draw bottom
                    if(GFX_BarDraw(
                        GfxPanel.panelLeft + counter,
                        GfxPanel.panelBottom - counter,
                        GfxPanel.panelRight - counter,
                        GfxPanel.panelBottom - counter) == GFX_STATUS_FAILURE)
                    {
                        return (GFX_STATUS_FAILURE);
                    }

                    counter++;
                }

                counter = 1;
                state = DRAW_EMBOSS4;
                break;

            case DRAW_EMBOSS4:
                while(counter < GfxPanel.panelEmbossSize)
                {

                    // draw right	   	
                    if(GFX_BarDraw(
                        GfxPanel.panelRight - counter,
                        GfxPanel.panelTop + counter,
                        GfxPanel.panelRight - counter,
                        GfxPanel.panelBottom - counter) == GFX_STATUS_FAILURE)
                    {
                        return (GFX_STATUS_FAILURE);
                    }

                    counter++;
                }

                state = SETUP_DRAW_FACECOLOR;
                break;


            // draw the face color
            case SETUP_DRAW_FACECOLOR:
                rnd_panel_setup_facecolor :

                // draw the face color
                GFX_ColorSet(GfxPanel.panelFaceColor);

                switch (GfxPanel.panelFillStyle)
                {
#if !defined(GFX_CONFIG_ALPHABLEND_DISABLE)
                    case GFX_FILL_STYLE_ALPHA_COLOR:
                        GFX_AlphaBlendingValueSet(GfxPanel.panelAlpha);
                        break;
#endif
                        
#if !defined (GFX_CONFIG_GRADIENT_DISABLE)
                    case GFX_FILL_STYLE_GRADIENT_DOWN:
                    case GFX_FILL_STYLE_GRADIENT_UP:
                    case GFX_FILL_STYLE_GRADIENT_RIGHT:
                    case GFX_FILL_STYLE_GRADIENT_LEFT:
                    case GFX_FILL_STYLE_GRADIENT_DOUBLE_VER:
                    case GFX_FILL_STYLE_GRADIENT_DOUBLE_HOR:
                        GFX_GradientColorSet(
                                GfxPanel.panelGradientStartColor,
                                GfxPanel.panelGradientEndColor);
                        break;
#endif
                    case GFX_FILL_STYLE_NONE:
                    case GFX_FILL_STYLE_COLOR:
                    default:
                        break;
                }

                GFX_FillStyleSet(GfxPanel.panelFillStyle);

                state = DRAW_FACECOLOR;

            // draw the face color
            case DRAW_FACECOLOR:

                if(GfxPanel.panelRadius)
                {
                    if(GFX_RectangleRoundFillDraw(  
                        GfxPanel.panelLeft,
                        GfxPanel.panelTop,
                        GfxPanel.panelRight,
                        GfxPanel.panelBottom,
                        GfxPanel.panelRadius - GfxPanel.panelEmbossSize) == GFX_STATUS_FAILURE)
                    {
                        return (GFX_STATUS_FAILURE);
                    }
                }
                else
                {
                    if(GFX_RectangleFillDraw(
                        GfxPanel.panelLeft + GfxPanel.panelEmbossSize,
                        GfxPanel.panelTop + GfxPanel.panelEmbossSize,
                        GfxPanel.panelRight - GfxPanel.panelEmbossSize,
                        GfxPanel.panelBottom - GfxPanel.panelEmbossSize) == GFX_STATUS_FAILURE)
                    {
                        return (GFX_STATUS_FAILURE);
                    }
                }

                state = DRAW_IMAGE;
                break;

            case DRAW_IMAGE:
                if(GfxPanel.pPanelImage != NULL)
                {
                    if(GFX_ImageDraw(
                        ((  GfxPanel.panelRight + GfxPanel.panelLeft -
                            GFX_ImageWidthGet(GfxPanel.pPanelImage)) >> 1) + 1,
                        ((  GfxPanel.panelBottom + GfxPanel.panelTop -
                            GFX_ImageHeightGet(GfxPanel.pPanelImage)) >> 1) + 1,
                        GfxPanel.pPanelImage) == GFX_STATUS_FAILURE)
                    {
                        return (GFX_STATUS_FAILURE);
                    }
                }
#ifndef GFX_CONFIG_ALPHABLEND_DISABLE
                //Disable Alpha Blending
                GFX_AlphaBlendingValueSet(100);
                GFX_FillStyleSet(GFX_FILL_STYLE_COLOR);
#endif


#if (GFX_CONFIG_COLOR_DEPTH == 1)
                state = DRAW_INNERFRAME;
                break;
#else
                state = DRAW_FRAME;
#endif
                break;

#if (GFX_CONFIG_COLOR_DEPTH == 1)
            case DRAW_INNERFRAME:
                if(GfxPanel.panelFaceColor == GfxPanel.panelEmbossDkColor)
                {
                    GFX_ColorSet(GfxPanel.panelEmbossLtColor);
                    if(GfxPanel.panelRadius)
                    {
                        if(GFX_RectangleRoundDraw(
                            GfxPanel.panelLeft,
                            GfxPanel.panelTop,
                            GfxPanel.panelRight,
                            GfxPanel.panelBottom,
                            GfxPanel.panelRadius -
                                (GfxPanel.panelEmbossSize - 1)) == GFX_STATUS_FAILURE)
                        {
                            return (GFX_STATUS_FAILURE);
                        }    
                    }
                    else
                    {
                        if(GFX_RectangleDraw(
                            GfxPanel.panelLeft +
                                (GfxPanel.panelEmbossSize - 1),
                            GfxPanel.panelTop +
                                (GfxPanel.panelEmbossSize - 1),
                            GfxPanel.panelRight -
                                (GfxPanel.panelEmbossSize - 1),
                            GfxPanel.panelBottom -
                                (GfxPanel.panelEmbossSize - 1)
                            ) == GFX_STATUS_FAILURE)
                        {
                            return (GFX_STATUS_FAILURE);
                        }
                    }
                }

                state = DRAW_FRAME;
                break;
#endif

            case DRAW_FRAME:

                // check if we need to draw the frame
                if((GfxPanel.pPanelImage == NULL) ||
                    (
                        ((GfxPanel.panelRight - GfxPanel.panelLeft + GfxPanel.panelRadius) >= GFX_ImageWidthGet(GfxPanel.pPanelImage)) &&
                        ((GfxPanel.panelBottom - GfxPanel.panelTop + GfxPanel.panelRadius) >= GFX_ImageHeightGet(GfxPanel.pPanelImage))
                    ))
                {
                    // draw the outline frame
#if (GFX_CONFIG_COLOR_DEPTH == 1)
                    // When in 1BPP mode, the outline should always be the light color 
                    // Ideally WHITE.   
                    if (GfxPanel.panelEmbossLtColor > GfxPanel.panelEmbossDkColor)
                        GFX_ColorSet(GfxPanel.panelEmbossLtColor);
                    else
                        GFX_ColorSet(GfxPanel.panelEmbossDkColor);
#else
                    GFX_ColorSet(GfxPanel.panelEmbossDkColor);
#endif
                    if (GFX_BevelDraw(
                        GfxPanel.panelLeft,
                        GfxPanel.panelTop,
                        GfxPanel.panelRight,
                        GfxPanel.panelBottom,
                        GfxPanel.panelRadius) == GFX_STATUS_FAILURE)
                    {
                        return (GFX_STATUS_FAILURE);
                    }
                }

                state = BEGIN;
                return (1);
        }   // end of switch
    }       // end of while
#endif 
}


// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_GOL_TwoTonePanelDraw(void)

    Summary:
        This function renders the two-tone panel.

    Description:
        This function renders the two-tone panel. Panel parameters are
        set by the GFX_GOL_PanelParameterSet(). The function returns
        success (GFX_STATUS_SUCCESS) when the panel is rendered. If
        the function returned not success this function must be
        called again until success is returned.

*/
// *****************************************************************************
GFX_STATUS GFX_GOL_TwoTonePanelDraw(void)
{
// In this panel draw task the emboss light and dark colors are used as
// the panel face colors and the panel face color is used as an outline color

#ifdef GFX_CONFIG_NONBLOCKING_DISABLE

    uint16_t    counter;

    GFX_ColorSet(GfxPanel.panelFaceColor);
    if(GfxPanel.panelRadius)
    {
        // draw the outline
        GFX_ThickBevelDraw( GfxPanel.panelLeft,
                            GfxPanel.panelTop,
                            GfxPanel.panelRight,
                            GfxPanel.panelBottom,
                            GfxPanel.panelRadius - GfxPanel.panelEmbossSize,
                            GfxPanel.panelRadius, 0xFF);
    }
    else
    {
        // object is rectangular panel draw the outline embossed areas
        counter = 1;
        while(counter < GfxPanel.panelEmbossSize)
        {
            GFX_BarDraw(    GfxPanel.panelLeft + counter,
                            GfxPanel.panelTop + counter,
                            GfxPanel.panelRight - counter,
                            GfxPanel.panelTop + counter);    // draw top

            GFX_BarDraw(    GfxPanel.panelLeft + counter,
                            GfxPanel.panelTop + counter,
                            GfxPanel.panelLeft + counter,
                            GfxPanel.panelBottom - counter);    // draw left

            GFX_BarDraw(    GfxPanel.panelLeft + counter,
                            GfxPanel.panelBottom - counter,
                            GfxPanel.panelRight - counter,
                            GfxPanel.panelBottom - counter);    // draw bottom

            GFX_BarDraw(    GfxPanel.panelRight - counter,
                            GfxPanel.panelTop + counter,
                            GfxPanel.panelRight - counter,
                            GfxPanel.panelBottom - counter);    // draw right
            counter++;
        }        
    }
	
	// draw the top half of the face 
    GFX_ColorSet(GfxPanel.panelEmbossLtColor);

    switch (GfxPanel.panelFillStyle)
    {
#if !defined(GFX_CONFIG_ALPHABLEND_DISABLE)
        case GFX_FILL_STYLE_ALPHA_COLOR:
            GFX_AlphaBlendingValueSet(GfxPanel.panelAlpha);
            GFX_FillStyleSet(GfxPanel.panelFillStyle);
            break;
#endif

#if !defined (GFX_CONFIG_GRADIENT_DISABLE)
        case GFX_FILL_STYLE_GRADIENT_DOWN:
        case GFX_FILL_STYLE_GRADIENT_UP:
        case GFX_FILL_STYLE_GRADIENT_RIGHT:
        case GFX_FILL_STYLE_GRADIENT_LEFT:
        case GFX_FILL_STYLE_GRADIENT_DOUBLE_VER:
        case GFX_FILL_STYLE_GRADIENT_DOUBLE_HOR:
            // ignore this case, two tone panels will not support
            // gradients
#endif
        case GFX_FILL_STYLE_NONE:
        case GFX_FILL_STYLE_COLOR:
        default:
            GFX_FillStyleSet(GFX_FILL_STYLE_COLOR);
            break;
    }

    if(GfxPanel.panelRadius)
    {
    	GFX_BevelDrawTypeSet(GFX_DRAW_TOPBEVEL);
        GFX_RectangleRoundFillDraw( GfxPanel.panelLeft,
                                    GfxPanel.panelTop,
                                    GfxPanel.panelRight,
                                    GfxPanel.panelBottom,
                                    GfxPanel.panelRadius - GfxPanel.panelEmbossSize);
    }
    else
    {
        GFX_RectangleFillDraw(    GfxPanel.panelLeft + GfxPanel.panelEmbossSize,
                        GfxPanel.panelTop + GfxPanel.panelEmbossSize,
                        GfxPanel.panelRight - GfxPanel.panelEmbossSize,
                        (GfxPanel.panelTop + ((GfxPanel.panelBottom - GfxPanel.panelTop) >> 1)));
    }

	// draw the bottom half of the face 
    GFX_ColorSet(GfxPanel.panelEmbossDkColor);
    if(GfxPanel.panelRadius)
    {
    	GFX_BevelDrawTypeSet(GFX_DRAW_BOTTOMBEVEL);
        GFX_RectangleRoundFillDraw( GfxPanel.panelLeft,
                                    GfxPanel.panelTop,
                                    GfxPanel.panelRight,
                                    GfxPanel.panelBottom,
                                    GfxPanel.panelRadius - GfxPanel.panelEmbossSize);
    }
    else
    {
        GFX_BarDraw(    GfxPanel.panelLeft + GfxPanel.panelEmbossSize,
                        (GfxPanel.panelTop + ((GfxPanel.panelBottom - GfxPanel.panelTop) >> 1)) + 1,
                        GfxPanel.panelRight - GfxPanel.panelEmbossSize,
                        GfxPanel.panelBottom - GfxPanel.panelEmbossSize);
    }
	
    GFX_BevelDrawTypeSet(GFX_DRAW_FULLBEVEL);

#if (GFX_CONFIG_COLOR_DEPTH == 1)
    if(GfxPanel.panelFaceColor == GfxPanel.panelEmbossDkColor)
    {
        GFX_ColorSet(GfxPanel.panelEmbossLtColor);
        if(GfxPanel.panelRadius)
            GFX_RectangleRoundDraw
            (
                GfxPanel.panelLeft,
                GfxPanel.panelTop,
                GfxPanel.panelRight,
                GfxPanel.panelBottom,
                GfxPanel.panelRadius - (GfxPanel.panelEmbossSize - 1)
            );
        else
            GFX_RectangleRoundDraw
            (
                GfxPanel.panelLeft + (GfxPanel.panelEmbossSize - 1),
                GfxPanel.panelTop + (GfxPanel.panelEmbossSize - 1),
                GfxPanel.panelRight - (GfxPanel.panelEmbossSize - 1),
                GfxPanel.panelBottom - (GfxPanel.panelEmbossSize - 1),
                0
            );
    }

#endif

    // draw bitmap
    if(GfxPanel.pPanelImage != NULL)
    {
        GFX_ImageDraw
        (
            (((GfxPanel.panelRight + GfxPanel.panelLeft) -
                (GFX_ImageWidthGet((void *)GfxPanel.pPanelImage))) >> 1) + 1,
            (((GfxPanel.panelBottom + GfxPanel.panelTop) -
                (GFX_ImageHeightGet((void *)GfxPanel.pPanelImage))) >> 1) + 1,
            GfxPanel.pPanelImage
        );
    }

    return (GFX_STATUS_SUCCESS);

#else

    typedef enum
    {
        BEGIN,
        DRAW_EMBOSS1,
        DRAW_EMBOSS2,
        DRAW_EMBOSS3,
        DRAW_EMBOSS4,
        DRAW_FACECOLOR_SETUP,
        DRAW_FACECOLOR1,
        DRAW_FACECOLOR2,
#if (GFX_CONFIG_COLOR_DEPTH == 1)
        DRAW_INNERFRAME,
#endif
        DRAW_IMAGE,
    } ROUND_PANEL_DRAW_STATES;

    static ROUND_PANEL_DRAW_STATES state = BEGIN;
    static uint16_t counter;

    while(1)
    {
        if (GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
            return (GFX_STATUS_FAILURE);

        switch(state)
        {
            case BEGIN:
                if(GfxPanel.panelRadius)
                {

                    // draw the outline
                    GFX_ColorSet(GfxPanel.panelFaceColor);
                    if(GFX_ThickBevelDraw(
                                GfxPanel.panelLeft,
                                GfxPanel.panelTop,
                                GfxPanel.panelRight,
                                GfxPanel.panelBottom,
                                GfxPanel.panelRadius - GfxPanel.panelEmbossSize,
                                GfxPanel.panelRadius, 0xFF) != GFX_STATUS_SUCCESS)
                        return (GFX_STATUS_FAILURE);
                    state = DRAW_FACECOLOR_SETUP;
                }
                else
                {
                    state = DRAW_EMBOSS1;
                    counter = 1;
                    goto rnd_panel_draw_emboss;
                }

            // now draw the upper portion of the embossed area
            case DRAW_EMBOSS1:
                rnd_panel_draw_emboss : GFX_ColorSet(GfxPanel.panelFaceColor);
                while(counter < GfxPanel.panelEmbossSize)
                {

                    // draw top
                    if(GFX_BarDraw(
                                GfxPanel.panelLeft + counter,
                                GfxPanel.panelTop + counter,
                                GfxPanel.panelRight - counter,
                                GfxPanel.panelTop + counter) != GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }

                    counter++;
                }

                counter = 1;
                state = DRAW_EMBOSS2;
                break;

            case DRAW_EMBOSS2:
                while(counter < GfxPanel.panelEmbossSize)
                {

                    // draw left   	
                    if(GFX_BarDraw(
                                GfxPanel.panelLeft + counter,
                                GfxPanel.panelTop + counter,
                                GfxPanel.panelLeft + counter,
                                GfxPanel.panelBottom - counter) != GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }

                    counter++;
                }

                counter = 1;
                state = DRAW_EMBOSS3;
                break;

            // now draw the lower portion of the embossed area
            case DRAW_EMBOSS3:
                //GFX_ColorSet(GfxPanel.panelEmbossDkColor);
                while(counter < GfxPanel.panelEmbossSize)
                {

                    // draw bottom
                    if(GFX_BarDraw(
                                GfxPanel.panelLeft + counter,
                                GfxPanel.panelBottom - counter,
                                GfxPanel.panelRight - counter,
                                GfxPanel.panelBottom - counter) != GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }

                    counter++;
                }

                counter = 1;
                state = DRAW_EMBOSS4;
                break;

            case DRAW_EMBOSS4:
                while(counter < GfxPanel.panelEmbossSize)
                {

                    // draw right	   	
                    if(GFX_BarDraw(
                                GfxPanel.panelRight - counter,
                                GfxPanel.panelTop + counter,
                                GfxPanel.panelRight - counter,
                                GfxPanel.panelBottom - counter) != GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }

                    counter++;
                }

                state = DRAW_FACECOLOR_SETUP;
                break;

            // set up the face color parameters
            case DRAW_FACECOLOR_SETUP:
                GFX_ColorSet(GfxPanel.panelEmbossLtColor);

                switch (GfxPanel.panelFillStyle)
                {
#if !defined(GFX_CONFIG_ALPHABLEND_DISABLE)
                    case GFX_FILL_STYLE_ALPHA_COLOR:
                        GFX_AlphaBlendingValueSet(GfxPanel.panelAlpha);
                        GFX_FillStyleSet(GfxPanel.panelFillStyle);
                        break;
#endif

#if !defined (GFX_CONFIG_GRADIENT_DISABLE)
                    case GFX_FILL_STYLE_GRADIENT_DOWN:
                    case GFX_FILL_STYLE_GRADIENT_UP:
                    case GFX_FILL_STYLE_GRADIENT_RIGHT:
                    case GFX_FILL_STYLE_GRADIENT_LEFT:
                    case GFX_FILL_STYLE_GRADIENT_DOUBLE_VER:
                    case GFX_FILL_STYLE_GRADIENT_DOUBLE_HOR:
                        // ignore this case, two tone panels will not support
                        // gradients
#endif
                    case GFX_FILL_STYLE_NONE:
                    case GFX_FILL_STYLE_COLOR:
                    default:
                        GFX_FillStyleSet(GFX_FILL_STYLE_COLOR);
                        break;
                }
                state = DRAW_FACECOLOR1;

            // draw the top half of the face
            case DRAW_FACECOLOR1:
                if(GfxPanel.panelRadius)
                {
                    GFX_BevelDrawTypeSet(GFX_DRAW_TOPBEVEL);
                    if(GFX_RectangleRoundFillDraw(
                                GfxPanel.panelLeft,
                                GfxPanel.panelTop,
                                GfxPanel.panelRight,
                                GfxPanel.panelBottom,
                                GfxPanel.panelRadius - GfxPanel.panelEmbossSize)
                            != GFX_STATUS_SUCCESS)
                        return (GFX_STATUS_FAILURE);
                }
                else
                {
                    if(GFX_RectangleFillDraw(
                                GfxPanel.panelLeft + GfxPanel.panelEmbossSize,
                                GfxPanel.panelTop + GfxPanel.panelEmbossSize,
                                GfxPanel.panelRight - GfxPanel.panelEmbossSize,
                                (GfxPanel.panelTop + ((GfxPanel.panelBottom - GfxPanel.panelTop) >> 1)))
                            != GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }
                }

                state = DRAW_FACECOLOR2;
                break;

           	// draw the bottom half of the face 
            case DRAW_FACECOLOR2:
                GFX_ColorSet(GfxPanel.panelEmbossDkColor);
                if(GfxPanel.panelRadius)
                {
	            GFX_BevelDrawTypeSet(GFX_DRAW_BOTTOMBEVEL);
                    if(GFX_RectangleRoundFillDraw(
                                GfxPanel.panelLeft,
                                GfxPanel.panelTop,
                                GfxPanel.panelRight,
                                GfxPanel.panelBottom,
                                GfxPanel.panelRadius - GfxPanel.panelEmbossSize)
                            != GFX_STATUS_SUCCESS)
                        return (GFX_STATUS_FAILURE);
                }
                else
                {
                    if
                    (GFX_BarDraw(
                            GfxPanel.panelLeft + GfxPanel.panelEmbossSize,
                            (GfxPanel.panelTop + ((GfxPanel.panelBottom - GfxPanel.panelTop) >> 1)) + 1,
                            GfxPanel.panelRight - GfxPanel.panelEmbossSize,
                            GfxPanel.panelBottom - GfxPanel.panelEmbossSize)
                            != GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }
                }
		GFX_BevelDrawTypeSet(GFX_DRAW_FULLBEVEL);
                state = DRAW_IMAGE;
                break;

            case DRAW_IMAGE:
                if(GfxPanel.pPanelImage != NULL)
                {
                    if
                    (GFX_ImageDraw(
                                ((GfxPanel.panelRight + GfxPanel.panelLeft -
                                    GFX_ImageWidthGet((void *)GfxPanel.pPanelImage)) >> 1) + 1,
                                ((GfxPanel.panelBottom + GfxPanel.panelTop -
                                    GFX_ImageHeightGet((void *)GfxPanel.pPanelImage)) >> 1) + 1,
                                GfxPanel.pPanelImage)
                            != GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }
                }

#if (GFX_CONFIG_COLOR_DEPTH == 1)
                state = DRAW_INNERFRAME;
                break;
#else
                state = BEGIN;
                return (GFX_STATUS_SUCCESS);
#endif
                break;

#if (GFX_CONFIG_COLOR_DEPTH == 1)

            case DRAW_INNERFRAME:
                if(GfxPanel.panelFaceColor == GfxPanel.panelEmbossDkColor)
                {
                    GFX_ColorSet(GfxPanel.panelEmbossLtColor);
                    if(GfxPanel.panelRadius)
                    {
                        if(GFX_RectangleRoundDraw
                            (
                                GfxPanel.panelLeft,
                                GfxPanel.panelTop,
                                GfxPanel.panelRight,
                                GfxPanel.panelBottom,
                                GfxPanel.panelRadius - 
                                (GfxPanel.panelEmbossSize - 1)
                            )
                                != GFX_STATUS_SUCCESS)
                        {
                            return (GFX_STATUS_FAILURE);
                        }    
                    }
                    else
                    {
                        if(GFX_RectangleRoundDraw
                            (
                                GfxPanel.panelLeft + (GfxPanel.panelEmbossSize - 1),
                                GfxPanel.panelTop + (GfxPanel.panelEmbossSize - 1),
                                GfxPanel.panelRight - (GfxPanel.panelEmbossSize - 1),
                                GfxPanel.panelBottom - (GfxPanel.panelEmbossSize - 1),
                                0
                            ) != GFX_STATUS_SUCCESS)
                        {
                            return (GFX_STATUS_SUCCESS);
                        }
                    }
                }
                state = BEGIN;
                return (1);
#endif
        }   // end of switch
    }       // end of while
#endif 
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_GOL_ObjectHideDraw(GFX_GOL_OBJ_HEADER *pObject)

    Summary:
        This function performs the hiding of an object from the screen.

    Description:
        This function performs the hiding of an object from the screen.
        If the object's style scheme is set to have a background, the
        background is taken into account.

*/
// *****************************************************************************
GFX_STATUS __attribute__ ((always_inline)) GFX_GOL_ObjectHideDraw(
                                GFX_GOL_OBJ_HEADER *pObject)
{
    // This is a routine that performs hiding of an object.
    // This takes into account if the object has background
    // set into it's style scheme.

    // check the background set
    switch(pObject->pGolScheme->CommonBkType)
    {
        case GFX_BACKGROUND_IMAGE:
            if (GFX_ImagePartialDraw(
                pObject->left, pObject->top,
                pObject->left -
                    pObject->pGolScheme->CommonBkLeft,
                pObject->top -
                    pObject->pGolScheme->CommonBkTop,
                pObject->right - pObject->left + 1,
                pObject->bottom - pObject->top + 1,
                pObject->pGolScheme->pCommonBkImage)
                != GFX_STATUS_SUCCESS)
            {
                return (GFX_STATUS_FAILURE);
            }
            break;

        case GFX_BACKGROUND_NONE:
        case GFX_BACKGROUND_COLOR:
        default :
            GFX_ColorSet(pObject->pGolScheme->CommonBkColor);
            if (GFX_BarDraw(
                pObject->left,
                pObject->top,
                pObject->right,
                pObject->bottom) != GFX_STATUS_SUCCESS)
            {
                return (GFX_STATUS_FAILURE);
            }
            break;
    }
    return (GFX_STATUS_SUCCESS);

}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_GOL_ObjectBackGroundSet(GFX_GOL_OBJ_HEADER *pObject)

    Summary:
        This function sets the background information.

    Description:
        This function sets the background information. This is an internal
        function and should not be called by the application. This function
        is used by object's drawing functions to set the background
        information.
 
*/
// *****************************************************************************
void GFX_GOL_ObjectBackGroundSet(GFX_GOL_OBJ_HEADER *pObjectHeader)
{

#ifndef GFX_CONFIG_ALPHABLEND_DISABLE
    // when alpha blending is enabled
    // set up the background info
    GFX_AlphaBlendingValueSet(pObjectHeader->pGolScheme->AlphaValue);
#endif

#ifndef GFX_CONFIG_GRADIENT_DISABLE

    GFX_GradientColorSet(   pObjectHeader->pGolScheme->gradientStartColor,
                            pObjectHeader->pGolScheme->gradientEndColor);
#endif
    
    GFX_FillStyleSet(pObjectHeader->pGolScheme->fillStyle);

    GFX_BackgroundSet(pObjectHeader->pGolScheme->CommonBkLeft,
                      pObjectHeader->pGolScheme->CommonBkTop,
                      pObjectHeader->pGolScheme->pCommonBkImage,
                      pObjectHeader->pGolScheme->CommonBkColor);
    
    GFX_BackgroundTypeSet(pObjectHeader->pGolScheme->CommonBkType);

}

// *****************************************************************************
/*  Function:
    bool GFX_GOL_DefaultObjectDrawCallback(GFX_GOL_OBJ_HEADER *pObject)

    Summary:
        This is the default draw callback function.

    Description:
        This is the default draw callback function. This function is 
        set as the draw callback by GFX_GOL_DrawCallbackSet() in 
        GFX_Initialize() when the object layer is enabled.

        This will allow users to use the library without creating
        the draw callback function if the application code will
        not peform any functions during draw callback.
  
*/
// *****************************************************************************
bool GFX_GOL_DefaultObjectDrawCallback(void)
{
    return (true);
}



