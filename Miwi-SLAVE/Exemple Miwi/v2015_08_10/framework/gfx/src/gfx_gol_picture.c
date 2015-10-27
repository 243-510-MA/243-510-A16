/*******************************************************************************
 Module for Microchip Graphics Library - Graphic Object Layer

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_gol_picture.c

  Summary:
    This implements the picture object of the GOL.

  Description:
    Refer to Microchip Graphics Library for complete documentation of the
    Picture Object.
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
#include "gfx/gfx_gol_picture.h"

// *****************************************************************************
/*  Function:
    GFX_GOL_PICTURECONTROL  *GFX_GOL_PictureControlCreate(
                                uint16_t            ID,
                                uint16_t            left,
                                uint16_t            top,
                                uint16_t            right,
                                uint16_t            bottom,
                                uint16_t            state,
                                GFX_RESOURCE_HDR    *pImage,
                                GFX_GOL_OBJ_SCHEME  *pScheme)

    Summary:
        This function creates a GFX_GOL_PICTURECONTROL object with the parameters
        given. It automatically attaches the new object into a global
        linked list of objects and returns the address of the object.

    Description:
        This function creates a GFX_GOL_PICTURECONTROL object with the parameters
        given. It automatically attaches the new object into a global
        linked list of objects and returns the address of the object.

        This function returns the pointer to the newly created object.
        If the object is not successfully created, it returns NULL.

        The object allows creation with the image set to NULL. In this
        case, nothing will be drawn when the object is set to be drawn
        and the frame is not enabled (See GFX_GOL_PICTURECONTROL_FRAME_STATE).
        If the frame is enabled, then only the frame will be drawn.

        When the assigned image's dimension is larger than the dimension
        of the object, partial image parameters will be set in such a
        way that the upper left most corner of the image that has the
        same dimension as the object will be used in the object. This
        is the default behavior.

        The partial parameters can be modified by calling the
        GFX_GOL_PictureControlPartialSet() function with the desired 
        partial image parameters. See GFX_ImagePartialDraw() for details
        on the partial image rendering.

        The behavior of GFX_GOL_PictureControlCreate() will be undefined
        if one of the following is true:
        - left >= right
        - top >= bottom
        - pScheme is not pointing to a GFX_GOL_OBJ_SCHEME
        - pImage is not pointing to a GFX_RESOURCE_HDR.

*/
// *****************************************************************************
GFX_GOL_PICTURECONTROL  *GFX_GOL_PictureControlCreate(
                                uint16_t            ID,
                                uint16_t            left,
                                uint16_t            top,
                                uint16_t            right,
                                uint16_t            bottom,
                                uint16_t            state,
                                int8_t              scaleFactor,
                                GFX_RESOURCE_HDR    *pImage,
                                GFX_GOL_OBJ_SCHEME  *pScheme)
{
    GFX_GOL_PICTURECONTROL *pPicture = NULL;
  //  uint16_t        width, height;

    pPicture = (GFX_GOL_PICTURECONTROL *)GFX_malloc(sizeof(GFX_GOL_PICTURECONTROL));
    if(pPicture == NULL)
        return (pPicture);

    pPicture->hdr.ID        = ID;
    pPicture->hdr.pNxtObj   = NULL;
    pPicture->hdr.type      = GFX_GOL_PICTURECONTROL_TYPE;
    pPicture->hdr.left      = left;
    pPicture->hdr.top       = top;
    pPicture->hdr.right     = right;
    pPicture->hdr.bottom    = bottom;
    pPicture->hdr.state     = state;
    pPicture->hdr.DrawObj   = GFX_GOL_PictureControlDraw;     // draw function
    pPicture->hdr.FreeObj   = NULL;                           // free function
    pPicture->hdr.actionGet = GFX_GOL_PictureControlActionGet;// action get function
    pPicture->hdr.actionSet = NULL;                           // default action function

    // When the image's dimension supplied to the object is larger
    // than the specified dimension of the object, draw only a
    // portion of the object that fits into the object dimension.
    GFX_GOL_PictureControlImageSet(pPicture, pImage);

    if (pImage != NULL)
    {

        // set the default setting of the partial image to use the
        // object's dimensions.
        GFX_GOL_PictureControlPartialSet(  pPicture,
                                    0, 0,
                                    right  - left + 1,
                                    bottom - top + 1);

    }

    // Set the style scheme to be used
    pPicture->hdr.pGolScheme = (GFX_GOL_OBJ_SCHEME *)pScheme;

    GFX_GOL_ObjectAdd((GFX_GOL_OBJ_HEADER *)pPicture);

    return (pPicture);
}

// *****************************************************************************
/*  Function:
    void GFX_GOL_PictureControlPartialSet(
                                GFX_GOL_PICTURECONTROL *pObject,
                                uint16_t xOffset,
                                uint16_t yOffset,
                                uint16_t partialWidth,
                                uint16_t partialHeight)

    Summary:
        This function sets the partial image parameters to be
        in the object.

    Description:
        This function sets the partial image parameters to be used
        in the object. This function allows usage of the object to
        specify a rectangular area of an image to be drawn as part
        of the object. This is useful when an image is already
        included in a larger image. To save memory, a separate image
        is not necessary for the picture object.
        The location of the smaller image in the larger image can be
        specified to show up in the picture object.

        This function will result in an undefined behavior when one
        of the following is true:
        - xOffset - value must not be greater than the image width.
        - yOffset - value must not be greater than the image height.
        - partialWidth - value must not be greater than image
                         width - xoffset + 1. Value must also be
                         less than the actual image width.
        - partialHeight - value must not be greater than image
                          height - yoffset + 1. Value must also be
                          less than the actual image height.

*/
// *****************************************************************************
void GFX_GOL_PictureControlPartialSet(
                                GFX_GOL_PICTURECONTROL *pObject,
                                uint16_t xOffset,
                                uint16_t yOffset,
                                uint16_t partialWidth,
                                uint16_t partialHeight)
{
    uint16_t width, height;

    width  = GFX_ImageWidthGet(pObject->pImage);
    height = GFX_ImageHeightGet(pObject->pImage);

    // This portion checks if the image is larger than the object
    // dimension. If the image dimension is smaller, it will be centered
    // on the object.

    // check horizontal
    if (width <= (pObject->hdr.right - pObject->hdr.left + 1))
    {
        // center the image in the horizontal dimension
        pObject->imageLeft       = ( pObject->hdr.left  + 1 +
                                     pObject->hdr.right -
                                     width) >> 1;
        pObject->imageRight      = pObject->imageLeft + width - 1;

        pObject->partial.width   = width;
        pObject->partial.xoffset = 0;
    }
    else
    {
        pObject->imageLeft       = pObject->hdr.left;
        pObject->imageRight      = pObject->imageLeft + partialWidth - 1;
        pObject->partial.width   = partialWidth;
        pObject->partial.xoffset = xOffset;
    }

    // check vertical
    if (height <= (pObject->hdr.bottom - pObject->hdr.top + 1))
    {
        // center the image in the vertical dimension
        pObject->imageTop        = ( pObject->hdr.top    + 1 +
                                     pObject->hdr.bottom -
                                     height) >> 1;
        pObject->imageBottom     = pObject->imageTop + height - 1;
        pObject->partial.height  = height;
        pObject->partial.yoffset = 0;
    }
    else
    {
        pObject->imageTop        = pObject->hdr.top;
        pObject->imageBottom     = pObject->imageTop + partialHeight - 1;
        pObject->partial.height  = partialHeight;
        pObject->partial.yoffset = yOffset;
    }
    
}

// *****************************************************************************
/* Function:
   void GFX_GOL_PictureControlScaleSet(
                                GFX_GOL_PICTURECONTROL pObject,
                                uint16_t scale)

  Summary:
    Sets the scale factor used to render the image used in the object.

  Description:
    This feature is not supported in this release of the Graphics Library.
    This function sets the scale factor used to render the image used in
    the object using scale.

*/
// *****************************************************************************
void GFX_GOL_PictureControlScaleSet(
                                GFX_GOL_PICTURECONTROL pObject,
                                int8_t scale)
{
    // This feature is not supported in this version
}

// *****************************************************************************
/*  Function:
    GFX_GOL_TRANSLATED_ACTION GFX_GOL_PictureControlActionGet(
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
GFX_GOL_TRANSLATED_ACTION GFX_GOL_PictureControlActionGet(
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage)
{
    GFX_GOL_PICTURECONTROL *pPict;

    pPict = (GFX_GOL_PICTURECONTROL *)pObject;

    // Evaluate if the message is for the picture
    // Check if disabled first
    if(GFX_GOL_ObjectStateGet(pPict, GFX_GOL_PICTURECONTROL_DISABLED_STATE))
        return (GFX_GOL_OBJECT_ACTION_INVALID);

#ifndef GFX_CONFIG_USE_TOUCHSCREEN_DISABLE
    if(pMessage->type == TYPE_TOUCHSCREEN)
    {

        // Check if it falls in the picture area
        if
        (
            (pPict->hdr.left   < pMessage->param1) &&
            (pPict->hdr.right  > pMessage->param1) &&
            (pPict->hdr.top    < pMessage->param2) &&
            (pPict->hdr.bottom > pMessage->param2)
        )
        {
            return (GFX_GOL_PICTURECONTROL_ACTION_SELECTED);
        }
    }

#endif
    return (GFX_GOL_OBJECT_ACTION_INVALID);
}

// *****************************************************************************
/*  Function:
    GFX_STATUS GFX_GOL_PictureControlDraw(void *pObject)

    Summary:
        This function renders the object on the screen based on the
        current state of the object.

    Description:
        This function renders the object on the screen based on the
        current state of the object. Location of the object is
        determined by the left, top, right and bottom parameters.
        The colors used are dependent on the state of the object.
        The font used is determined by the style scheme set.

        The text on the face of the GFX_GOL_PICTURECONTROL is drawn on top of
        the bitmap. Text alignment based on the alignment parameter
        set on the object.

        When rendering objects of the same type, each object
        must be rendered completely before the rendering of the
        next object is started. This is to avoid incomplete
        object rendering.

        Normally, application will just call GFX_GOL_ObjectListDraw()
        to allow the Graphics Library to manage all object rendering.
        See GFX_GOL_ObjectListDraw() for more information on object rendering.

*/
// *****************************************************************************
GFX_STATUS GFX_GOL_PictureControlDraw(void *pObject)
{
    typedef enum
    {
        DRAW_PICTURE_START,
        DRAW_PICTURE_HIDE,
        DRAW_PICTURE_PANEL_BACKGROUND,
        DRAW_PICTURE_IMAGE,
        DRAW_PICTURE_FRAME
    } PICT_DRAW_STATES;

    static PICT_DRAW_STATES state = DRAW_PICTURE_START;
    GFX_GOL_PICTURECONTROL  *pPict;

    pPict = (GFX_GOL_PICTURECONTROL *)pObject;

    if (GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
        return (GFX_STATUS_FAILURE);

    while(1)
    {
        switch(state)
        {
            case DRAW_PICTURE_START:
                if(GFX_GOL_ObjectStateGet(
                        pPict,
                        GFX_GOL_PICTURECONTROL_HIDE_STATE))
                {
                    state = DRAW_PICTURE_HIDE;
                    break;
                }

                GFX_GOL_PanelBackgroundSet(&pPict->hdr);

                // Note that the face color of the panel should match
                // the color used in the uncovered areas
                // which is the CommonBkColor.
                GFX_GOL_PanelParameterSet
                (
                    pPict->hdr.left,
                    pPict->hdr.top,
                    pPict->hdr.right,
                    pPict->hdr.bottom,
                    0,
                    pPict->hdr.pGolScheme->CommonBkColor,
                    pPict->hdr.pGolScheme->EmbossDkColor,
                    pPict->hdr.pGolScheme->EmbossLtColor,
                    NULL,
                    pPict->hdr.pGolScheme->fillStyle, 
                    0
                );

                state = DRAW_PICTURE_PANEL_BACKGROUND;

            case DRAW_PICTURE_PANEL_BACKGROUND:

                if(GFX_GOL_PanelDraw() != GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }

                state = DRAW_PICTURE_IMAGE;

            case DRAW_PICTURE_IMAGE:
                if(pPict->pImage != NULL)
                {
                    if (GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
                        return (GFX_STATUS_FAILURE);

                    if(GFX_ImagePartialDraw(
                            pPict->imageLeft,
                            pPict->imageTop,
                            pPict->partial.xoffset,
                            pPict->partial.yoffset,
                            pPict->partial.width,
                            pPict->partial.height,
                            pPict->pImage)
                        != GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }
                }

                state = DRAW_PICTURE_FRAME;

            case DRAW_PICTURE_FRAME:
                if(GFX_GOL_ObjectStateGet(
                        pPict,
                        GFX_GOL_PICTURECONTROL_FRAME_STATE))
                {
                    GFX_LineStyleSet(GFX_LINE_STYLE_THIN_SOLID);
                    GFX_ColorSet(pPict->hdr.pGolScheme->TextColor0);
                    if(GFX_RectangleDraw(
                            pPict->hdr.left,
                            pPict->hdr.top,
                            pPict->hdr.right,
                            pPict->hdr.bottom) != GFX_STATUS_SUCCESS)
                    {
                        return (GFX_STATUS_FAILURE);
                    }
                }

                state = DRAW_PICTURE_START;
                return (GFX_STATUS_SUCCESS);

            case DRAW_PICTURE_HIDE:
                
                if (    GFX_GOL_ObjectHideDraw(&(pPict->hdr)) !=
                        GFX_STATUS_SUCCESS)
                {
                    return (GFX_STATUS_FAILURE);
                }
                
                state = DRAW_PICTURE_START;
                return (GFX_STATUS_SUCCESS);

            default:
                // this should not happen
                return (GFX_STATUS_SUCCESS);

        } // end of switch()...
    } // end of while(1)...

    return (GFX_STATUS_SUCCESS);
}


