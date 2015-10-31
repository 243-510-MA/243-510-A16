/*******************************************************************************
 Module for Microchip Graphics Library - Graphic Object Layer

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_gol_picture.h

  Summary:
    This is the header file for the picture object of the GOL.

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

#ifndef _GFX_GOL_PICTURECONTROL_H
// DOM-IGNORE-BEGIN
    #define _GFX_GOL_PICTURECONTROL_H
// DOM-IGNORE-END

#include <stdint.h>
#include <gfx/gfx_gol.h>

// *****************************************************************************
/* 
    <GROUP gol_types>

    Typedef:
        GFX_GOL_PICTURECONTROLCONTROL_STATE

    Summary:
        Specifies the different states of the Picture Control object.

    Description:
        This enumeration specifies the different states of the Picture
        Control object used in the library.

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
    GFX_GOL_PICTURECONTROL_DISABLED_STATE    
        /*DOM-IGNORE-BEGIN*/  = 0x0002 /*DOM-IGNORE-END*/,  
    // Property bit to indicate that the object will have a frame.
    GFX_GOL_PICTURECONTROL_FRAME_STATE
        /*DOM-IGNORE-BEGIN*/  = 0x0004 /*DOM-IGNORE-END*/,  
    // Property bit to indicate Picture is streaming. This feature is available
    // only when the hardware can support streaming of images.
    GFX_GOL_PICTURECONTROL_STREAM_STATE 
        /*DOM-IGNORE-BEGIN*/  = 0x0008 /*DOM-IGNORE-END*/,  
    // Draw bit to indicate object must be redrawn.
    GFX_GOL_PICTURECONTROL_DRAW_STATE     
        /*DOM-IGNORE-BEGIN*/  = 0x4000 /*DOM-IGNORE-END*/,  
    // Draw bit to indicate object must be removed from screen.
    GFX_GOL_PICTURECONTROL_HIDE_STATE  
        /*DOM-IGNORE-BEGIN*/  = 0x8000 /*DOM-IGNORE-END*/,  
} GFX_GOL_PICTURECONTROLCONTROL_STATE;

// *****************************************************************************
/* 
    <GROUP gol_types>

    Typedef:
        GFX_GOL_PICTURECONTROL

    Summary:
        Defines the structure used for the Picture Control object.
        
    Description:
        Defines the parameters required for a Picture Control object.
        Object is drawn with the defined shape parameters and values 
        set on the given fields.
        
    Remarks:
        None.
        
*/
// *****************************************************************************
typedef struct
{
    GFX_GOL_OBJ_HEADER      hdr;         // Generic header for all Objects (see GFX_GOL_OBJ_HEADER).
    GFX_RESOURCE_HDR        *pImage;     // Pointer to the image
    int8_t                  scaleFactor; // Scale factor for the bitmap
    uint16_t                imageLeft;   // image left position when drawn
    uint16_t                imageTop;    // image top position when drawn
    uint16_t                imageRight;  // image right position when drawn
    uint32_t                *stream;
    uint8_t                 count;       // Count for the number of bitmaps to be streamed
    uint8_t                 delay;       // Delay in between the streaming of bitmaps
    uint16_t                imageBottom; // image bottom position when drawn
    GFX_PARTIAL_IMAGE_PARAM partial;     // structure containing partial image data
} GFX_GOL_PICTURECONTROL;


// *****************************************************************************
/*  
    <GROUP gol_picture_object>

    Function:
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
        parial image parameters. See GFX_ImagePartialDraw() for
        details on the partial image rendering.

        The behavior of GFX_GOL_PictureControlCreate() will be
        undefined if one of the following is true:
        - left >= right
        - top >= bottom
        - pScheme is not pointing to a GFX_GOL_OBJ_SCHEME
        - pImage is not pointing to a GFX_RESOURCE_HDR.

    Precondition:
        None.

    Parameters:
        ID - Unique user defined ID for the object instance.
        left - Left most position of the object.
        top - Top most position of the object.
        right - Right most position of the object.
        bottom - Bottom most position of the object.
        state - Sets the initial state of the object.
        scaleFactor - Sets the scaling factor when the image is rendered. 
            This feature is only available in certain builds.
        pImage - Pointer to the image by the object
        pScheme - Pointer to the style scheme used.

    Returns:
        Pointer to the newly created object.

    Example:
        None.

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
                                GFX_GOL_OBJ_SCHEME  *pScheme);

// *****************************************************************************
/*  
    <GROUP gol_picture_object>

    Function:
        GFX_RESOURCE_HDR *GFX_GOL_PictureControlImageGet(
                                GFX_GOL_PICTURECONTROL *pObject)

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
#define GFX_GOL_PictureControlImageGet(pObject)    \
                    (((GFX_GOL_PICTURECONTROL*)pObject)->pImage)

// *****************************************************************************
/*  
    <GROUP gol_picture_object>

    Function:
        void GFX_GOL_PictureControlImageSet(
                                GFX_GOL_PICTURECONTROL *pObject,
                                GFX_RESOURCE_HDR *pImage)

    Summary:
        This function sets the image to be in the object.

    Description:
        This function sets the image to be in the object.

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - pointer to the object.
        pImage  - pointer to the image resource.

    Returns:
        None.

    Example:
        <code>
            // assume OrigImage and NewImage are valid GFX_RESOURCE_HDR
            // pointers for images
            
            GFX_RESOURCE_HDR *pOrigIcon = &OrigImage;
            GFX_RESOURCE_HDR *pNewIcon = &NewImage;
            GFX_GOL_PICTURECONTROL  *pPicture;

            pPicture = GFX_GOL_PictureControlCreate(
                                10, 
                                0, 0, 
                                GFX_MaxXGet(), GFX_MaxYGet(),
                                GFX_GOL_PICTURECONTROL_DRAW_STATE,
                                pOrigIcon,
                                NULL);    
                                    
            // change the image used
            GFX_GOL_PictureControlImageSet(pPicture , pNewIcon);
        </code>

*/
// *****************************************************************************
#define GFX_GOL_PictureControlImageSet(pObject, pImage)    \
                    (((GFX_GOL_PICTURECONTROL*)pObject)->pImage = pImage)

// *****************************************************************************
/*  
    <GROUP gol_picture_object>

    Function:
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
 
    Precondition:
        Object must exist in memory.
        The image pointer of the object must be initialized properly.

    Parameters:
        xOffset - x offset of the smaller portion of a large image
        yOffset - y offset of the smaller portion of a large image
        partialWidth - width of the selected portion of the image
        partialHeight - height of the selected portion of the image

     Returns:
        None.

    Example:
        <code>
            // assume pLargeImage is a valid GFX_RESOURCE_HDR
            // assume BigImage has a height and width of 100 pixels.

            GFX_RESOURCE_HDR *pLargeImage = &BigImage;
            GFX_GOL_PICTURECONTROL  *pPicture;
            uint16_t         width, height;
            uint16_t         xOffset, yOffset;
            uint16_t         objectWidth, objectHeight;

            objectWidth  = 60  - 50; // 10 pixels
            objectHeight = 120 - 90; // 30 pixels

            // -1 is needed since the object dimension is inclusive
            pPicture = GFX_GOL_PictureControlCreate(
                                10,
                                50, 90,
                                50 + objectWidth - 1,
                                90 + objectHeight - 1,
                                GFX_GOL_PICTURECONTROL_DRAW_STATE,
                                largeImage,
                                NULL);

            // set the parameters of the partial image to be
            // shown on the image
 
            // get the large image dimensions
            width  = GFX_ImageWidthGet(pLargeImage);
            height = GFX_ImageHeightGet(pLargeImage);

            // get the offset so the middle of the large image with
            // the width and height matching the object will be used.
            xOffset = (width  - objectWidth)  >> 1;
            yOffset = (height - objectHeight) >> 1;

            GFX_GOL_PictureControlPartialSet(  pPicture,
                                        xOffset, yOffset,
                                        objectWidth, objectHeight);
        </code>

*/
// *****************************************************************************
void GFX_GOL_PictureControlPartialSet(
                                GFX_GOL_PICTURECONTROL *pObject,
                                uint16_t xOffset,
                                uint16_t yOffset,
                                uint16_t partialWidth,
                                uint16_t partialHeight);

// *****************************************************************************
/* 
    <GROUP gol_picture_object>

    Function:
    void GFX_GOL_PictureControlScaleSet(
                                GFX_GOL_PICTURECONTROL pObject,
                                int8_t scale)

  Summary:
    Sets the scale factor used to render the image used in the object.

  Description:
    This function sets the scale factor used to render the image used in
    the object using scale.

  Precondition:
    None.

  Parameters:
    pObject - Pointer to the object
    scale   - Scale factor that will be used to display the image.

  Returns:
    None.

  Example:
    None.

  Remarks:
    None.
*/
// *****************************************************************************
void GFX_GOL_PictureControlScaleSet(
                                GFX_GOL_PICTURECONTROL pObject,
                                int8_t scale);

// *****************************************************************************
/*  
    <GROUP gol_picture_object>

    Function:
        GFX_GOL_TRANSLATED_ACTION GFX_GOL_PictureControlActionGet(
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
    	Translated Message                      Input Source    Set/Clear State Bit                     Description
     	##################                      ############    #######################                 ##################################################################################################################################################################
        GFX_GOL_PICTURECONTROL_ACTION_SELECTED  Touch Screen    EVENT_PRESS, EVENT_RELEASE, EVENT_MOVE  If events occurs and the x,y position falls in the area of the picture.
        GFX_GOL_OBJECT_ACTION_INVALID           Any             Any                                     If the message did not affect the object.
	</TABLE>

    Precondition:
        Object must exist in memory.

    Parameters:
        pObject - The pointer to the object where the message will be
                   evaluated to check if the message will affect the object.
        pMessage - Pointer to the the message from the user interface.

    Returns:
        - GFX_GOL_PICTURECONTROL_ACTION_SELECTED   – Object is selected
        - GFX_GOL_OBJECT_ACTION_INVALID     – Object is not affected

    Example:
        None.

*/
// *****************************************************************************
GFX_GOL_TRANSLATED_ACTION GFX_GOL_PictureControlActionGet(
                                void *pObject,
                                GFX_GOL_MESSAGE *pMessage);

// *****************************************************************************
/*  
    <GROUP gol_picture_object>

    Function:
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

        When the image size The text on the face of the
        GFX_GOL_PICTURECONTROL is drawn on top of
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
GFX_STATUS GFX_GOL_PictureControlDraw(void *pObject);


#endif // _GFX_GOL_PICTURECONTROL_H
