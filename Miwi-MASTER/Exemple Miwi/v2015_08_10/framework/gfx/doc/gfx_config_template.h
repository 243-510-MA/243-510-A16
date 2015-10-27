/*******************************************************************************
 Module for Microchip Graphics Library

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_config.h

  Summary:
    This header file template defines all the Graphics Library 
    configurations available. 
    
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2014 released Microchip Technology Inc.  All rights reserved.

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

#ifndef _GRAPHICS_CONFIG_H
    #define _GRAPHICS_CONFIG_H

//@@configuring_options_graphics_library_doc
/*
    <TITLE Configuring the Library>

    Summary: 
        The configuration of the Microchip Graphics Library based 
        on the file gfx_config.h. This file (or the definitions 
        it describes) must be included in a header named system_config.h, 
        which will be included directly by the library source files.

        The gfx_config.h header file contains the configuration 
        selection for this Microchip Graphics Library. 
        Based on the selections made, the library will enable or disable
        selected features. These configuration settings will apply to all 
        instances of the library. This header can be placed anywhere; 
        however, the path of this header needs to be present in the include 
        search path for a successful build.
        
*/


//////////////////// COMPILE OPTIONS ////////////////////

// *****************************************************************************
/* 
    <GROUP  configuring_options_graphics_library_doc>

    Macro:
        GFX_CONFIG_NONBLOCKING_DISABLE

    Summary:
        Blocking and Non-Blocking configuration selection.

    Description:
        Enabling this macro will disable the non-blocking mode
        of the library. Non-blocking is enabled to allow 
        
        Basic rendering functions such as GFX_LineDraw(), GFX_RectangleDraw(),
        GFX_BarDraw() etc... are functions implemented in the Primitive Layer.
        These functions can also be implemented in the device driver layer if
        the display device supports hardware acceleration of the function.
        Applications that calls these functions can take advantage of
        the hardware accelerated primitives. How these functions are used will
        depend on the configuration setting for the blocking or non-blocking
        mode.
        
        All primitive rendering functions returns a status.
            - GFX_STATUS_FAILURE – when the primitive was not successfully
                                   rendered
            - GFX_STATUS_SUCCESS – when the primitive was successfully rendered
                                
        When using Graphics Library non-blocking mode is by default enabled. 
        If this mode is to be disabled, add this line in the configuration:
        #define GFX_CONFIG_NONBLOCKING_DISABLE 
        
        When using a display controller with hardware accelerated primitives
        (like SSD1926 which is on the Graphics PICtail™ Plus Board Version 3
        (AC164127-3) faster primitive rendering on Line, Rectangle and
        Bar functions will be performed. Compiling with the Blocking or
        Non-Blocking mode set will still use the accelerated primitives but the
        application code that directly call the primitive functions will have to
        be coded accordingly.
        
        To explain the two modes when directly calling the primitive functions
        please take a look at the example below.
        
        Case 1: Non-Blocking disabled

        <code>
            // all primitives are blocking calls
            GFX_LineDraw(a,b);
            GFX_RectangleDraw(c,d,e,f);
            GFX_BarDraw(c+2, d+2, e-2, f-2);
        </code>

        Case 2: Non-Blocking enabled
        
        <code>
            // all primitives are non-blocking calls
            while(GFX_LineDraw(a,b) != GFX_STATUS_SUCCESS);
            while(GFX_RectangleDraw(c,d,e,f) != GFX_STATUS_SUCCESS);
            while(GFX_BarDraw(c+2, d+2, e-2, f-2) != GFX_STATUS_SUCCESS);
        </code>
        
        If the while check is not in place, it possible that the only primitive
        that you will see in the screen is the Line. For case 2, one can also
        be creative in the application code and implement some form of
        non-blocking scheme and make use of the time while waiting for the
        primitives to render.
        
        Another example for case 2:
        <code>
            GFX_STATUS DrawMyFigure(
                        uint16_t a, 
                        uint16_t b, 
                        uint16_t c, 
                        uint16_t d, 
                        uint16_t e, 
                        uint16_t f)
            {
                typedef enum {
                    DRAW_LINE,
                    DRAW_RECT,
                    DRAW_BAR,
                } DRAW_MYFIGURE_STATES;
    
                static DRAW_MYFIGURE_STATES state = DRAW_LINE;
                
                // checks if the hardware is still busy
                if (GFX_RenderStatusGet() == GFX_STATUS_BUSY_BIT)
                    return (GFX_STATUS_FAILURE);                    
            
                switch(state)
                {
                    case DRAW_LINE:
                        if (GFX_LineDraw(a, b) != GFX_STATUS_SUCCESS)
                            return GFX_STATUS_FAILURE;
                        state = DRAW_RECT;
                    case DRAW_RECT:
                        if(GFX_RectangleDraw(c,d,e,f) != GFX_STATUS_SUCCESS)
                            return GFX_STATUS_FAILURE;
                        state = DRAW_BAR;
                    case DRAW_BAR:
                        if(GFX_BarDraw(c+2, d+2, e-2, f-2) !=
                                    GFX_STATUS_SUCCESS)
                            return GFX_STATUS_FAILURE;
                        state = DRAW_LINE;
                        return GFX_STATUS_SUCCESS;
                    default:
                        // this case should not occur
                        return GFX_STATUS_SUCCESS;
                }
            }
        </code>
        
        This non-blocking code can be used in the application and the
        application can do other tasks whenever DrawMyFigure() 
        returns GFX_STATUS_FAILURE. Application should call DrawMyFigure() again
        until it return a GFX_STATUS_SUCCESS signifying that the Line, Rectangle 
        and Bar shapes were drawn successfully.

        To disable the non-blocking feature, add this macro 
        in the configuration.
        
    Remarks:
        None.
        
*/
// *****************************************************************************
#define GFX_CONFIG_NONBLOCKING_DISABLE

// *****************************************************************************
/* 
    <GROUP  configuring_options_graphics_library_doc>

    Macro:
        GFX_CONFIG_ALPHABLEND_DISABLE

    Summary:
        Macro that disables the Alpha Blending feature.

    Description:
        Alpha blending feature is available to fill functions.
        
        To disable the alpha blending feature, add this macro 
        in the configuration.
        
    Remarks:
        None.
        
*/
// *****************************************************************************
#define GFX_CONFIG_ALPHABLEND_DISABLE

// *****************************************************************************
/* 
    <GROUP  configuring_options_graphics_library_doc>

    Macro:
        GFX_CONFIG_GRADIENT_DISABLE

    Summary:
        Macro that disables the gradient fill feature.

    Description:
        Gradient fill is available to fill functions.
        
        To disable the gradient feature, add this macro in the configuration.
        
    Remarks:
        None.
        
*/
// *****************************************************************************
#define GFX_CONFIG_GRADIENT_DISABLE

// *****************************************************************************
/* 
    <GROUP  configuring_options_graphics_library_doc>

    Macro:
        GFX_CONFIG_PALETTE_DISABLE

    Summary:
        Macro that disables the palette feature.

    Description:
        The use of a palette is an option to run the application with 
        256 16, or 2 colors only. When using this feature, the color
        depth set for the application should be 8, 4 or 1 bpp 
        (see GFX_CONFIG_COLOR_DEPTH for color depth settings).
        
        To disable the palette feature, add this macro in the configuration.
        
    Remarks:
        None.
        
*/
// *****************************************************************************
#define GFX_CONFIG_PALETTE_DISABLE

// *****************************************************************************
/* 
    <GROUP  configuring_options_graphics_library_doc>

    Macro:
        GFX_CONFIG_FONT_ANTIALIASED_DISABLE

    Summary:
        Macro that disables the use of anti-aliased fonts.
        
    Description:
        Anti-aliased fonts are supported in the library. The fonts 
        must be generated using the Graphics Resource Converter (GRC). 
        The GRC is a utility used in the Graphics Library to generate 
        application resources.
        
        To disable the anti-aliased fonts feature, add this macro in 
        the configuration.
        
    Remarks:
        None.
        
*/
// *****************************************************************************
#define GFX_CONFIG_FONT_ANTIALIASED_DISABLE 

// *****************************************************************************
/* 
    <GROUP  configuring_options_graphics_library_doc>

    Macro:
        GFX_CONFIG_TRANSPARENT_COLOR_DISABLE

    Summary:
        Macro that disables the use of anti-aliased fonts.
        
    Description:
        Transparent color is a feature in GFX_ImageDraw() where
        pixels that equals the color set in the transparent 
        color variable will not be rendered. This is useful in
        rendering rounded icons or images to the screen with
        complex background.
        
        To disable the transparent color in images, add this macro in 
        the configuration.
        
    Remarks:
        None.
        
*/
// *****************************************************************************
#define GFX_CONFIG_TRANSPARENT_COLOR_DISABLE 

// *****************************************************************************
/* 
    <GROUP  configuring_options_graphics_library_doc>

    Macro:
        GFX_CONFIG_PALETTE_EXTERNAL_DISABLE

    Summary:
        Macro that disables the palette feature that are sourced in external
        resources.

    Description:
        Similar to fonts and images, Palettes are considered resources. 
        The use of palette also allow the application to locate the palette
        resources in external sources. This macro disables the code that 
        implements externally sourced palettes.
        
        To disable the palette to be sourced from external resources, 
        add this macro in the configuration.
        
    Remarks:
        None.
        
*/
// *****************************************************************************
#define GFX_CONFIG_PALETTE_EXTERNAL_DISABLE

// *****************************************************************************
/* 
    <GROUP  configuring_options_graphics_library_doc>

    Macro:
        GFX_CONFIG_FOCUS_DISABLE

    Summary:
        Macro that disables the focus feature in objects.

    Description:
        This compile option allows keyboard input focus. 
        - GFX_GOL_ObjectFocusSet()
        - GFX_GOL_ObjectFocusGet()
        - GFX_GOL_ObjectFocusNextGet()
        - GFX_GOL_ObjectFocusPrevGet()
        functions will be available. Focus is also changed by touch screen.
               
        To disable the focus feature in objects, add this macro in 
        the configuration.
        
    Remarks:
        None.
        
*/
// *****************************************************************************
#define GFX_CONFIG_FOCUS_DISABLE

// *****************************************************************************
/* 
    <GROUP  configuring_options_graphics_library_doc>

    Macro:
        GFX_CONFIG_FONT_CHAR_SIZE

    Summary:
        Macro that sets the size of the characters used in the 
        fonts.

    Description:
        This configuration sets the text character size used in the library.

        To enable support for unicode fonts, GFX_CONFIG_FONT_CHAR_SIZE
        must be defined as 16 (size is 16 bits). For standard ascii
        fonts, this can be defined as 8. This changes the
        GFX_XCHAR definition. See GFX_XCHAR for details.

        <TABLE>
            <b>Set in configuration</b>             <b>GFX_XCHAR</b>	            <b>Description</b>
            ####################################    #############################	##########################
            #define GFX_CONFIG_FONT_CHAR_SIZE 16    #define GFX_XCHAR uint16_t	    Use multi-byte characters (0-2^16 range).
            #define GFX_CONFIG_FONT_CHAR_SIZE 8     #define GFX_XCHAR uint8_t	    Use byte characters (0-255 range).
        </TABLE>
               
        If this macro is not set, the default size used is 8-bits.

        <code>
            // example to set characters to use 16 bits
            #define GFX_CONFIG_FONT_CHAR_SIZE 16

            // example to set characters to use 8 bits
            #define GFX_CONFIG_FONT_CHAR_SIZE 8
        </code>
        
        
    Remarks:
        None.
        
*/
// *****************************************************************************
#define GFX_CONFIG_FONT_CHAR_SIZE /* DOM-IGNORE-BEGIN */ 16 /* DOM-IGNORE-END */

// *****************************************************************************
/* 
    <GROUP  configuring_options_graphics_library_doc>

    Macro:
        GFX_CONFIG_FONT_FLASH_DISABLE

    Summary:
        Macro that disables sourcing of font resources from internal 
        flash memory.
        
    Description:
        Font data can be placed in multiple locations. They can be
        placed in FLASH memory, RAM and external memory.
        To reduce code size, any one of these locations, when not
        used, can be disabled by defining the macros at build time.
        
        This macro disables the use of font resources from internal flash
        memory.
               
        To disable this feature, add this macro in the configuration.
        
    Remarks:
        None.
        
*/
// *****************************************************************************
#define GFX_CONFIG_FONT_FLASH_DISABLE  

// *****************************************************************************
/* 
    <GROUP  configuring_options_graphics_library_doc>

    Macro:
        GFX_CONFIG_FONT_EXTERNAL_DISABLE

    Summary:
        Macro that disables sourcing of font resources from external 
        sources.
        
    Description:
        Font data can be placed in multiple locations. They can be
        placed in FLASH memory, RAM and external memory.
        To reduce code size, any one of these locations, when not
        used, can be disabled by defining the macros at build time.
        
        This macro disables the use of font resources from external 
        memory.
               
        To disable this feature, add this macro in the configuration.
        
    Remarks:
        None.
        
*/
// *****************************************************************************
#define GFX_CONFIG_FONT_EXTERNAL_DISABLE

// *****************************************************************************
/* 
    <GROUP  configuring_options_graphics_library_doc>

    Macro:
        GFX_CONFIG_FONT_RAM_DISABLE

    Summary:
        Macro that disables sourcing of font resources from RAM 
        sources.
        
    Description:
        Font data can be placed in multiple locations. They can be
        placed in FLASH memory, RAM and external memory.
        To reduce code size, any one of these locations, when not
        used, can be disabled by defining the macros at build time.
        
        This macro disables the use of font resources from RAM.
               
        To disable this feature, add this macro in the configuration.
        
    Remarks:
        None.
        
*/
// *****************************************************************************
#define GFX_CONFIG_FONT_RAM_DISABLE

// *****************************************************************************
/* 
    <GROUP  configuring_options_graphics_library_doc>

    Macro:
        GFX_CONFIG_IMAGE_FLASH_DISABLE

    Summary:
        Macro that disables sourcing of image resources from internal 
        flash memory.
        
    Description:
        Images can be placed in multiple locations. They can be
        placed in FLASH memory, RAM and external memory.
        To reduce code size, any one of these locations, when not
        used, can be disabled by defining the macros at build time.
        
        This macro disables the use of image resources from internal flash
        memory.
               
        To disable this feature, add this macro in the configuration.
        
    Remarks:
        None.
        
*/
// *****************************************************************************
#define GFX_CONFIG_IMAGE_FLASH_DISABLE  

// *****************************************************************************
/* 
    <GROUP  configuring_options_graphics_library_doc>

    Macro:
        GFX_CONFIG_IMAGE_EXTERNAL_DISABLE

    Summary:
        Macro that disables sourcing of image resources from external 
        sources.
        
    Description:
        Images can be placed in multiple locations. They can be
        placed in FLASH memory, RAM and external memory.
        To reduce code size, any one of these locations, when not
        used, can be disabled by defining the macros at build time.
        
        This macro disables the use of image resources from external 
        memory.
               
        To disable this feature, add this macro in the configuration.
        
    Remarks:
        None.
        
*/
// *****************************************************************************
#define GFX_CONFIG_IMAGE_EXTERNAL_DISABLE

// *****************************************************************************
/* 
    <GROUP  configuring_options_graphics_library_doc>

    Macro:
        GFX_CONFIG_IMAGE_RAM_DISABLE

    Summary:
        Macro that disables sourcing of image resources from RAM 
        sources.
        
    Description:
        Images can be placed in multiple locations. They can be
        placed in FLASH memory, RAM and external memory.
        To reduce code size, any one of these locations, when not
        used, can be disabled by defining the macros at build time.
        
        This macro disables the use of image resources from RAM.
               
        To disable this feature, add this macro in the configuration.
        
    Remarks:
        None.
        
*/
// *****************************************************************************
#define GFX_CONFIG_IMAGE_RAM_DISABLE

// *****************************************************************************
/* 
    <GROUP  configuring_options_graphics_library_doc>

    Macro:
        GFX_CONFIG_RLE_DECODE_DISABLE

    Summary:
        Macro that disables RLE compression of images.
        
    Description:
        Images that have 8bpp or 4 bpp color depth has the option to be 
        RLE compressed. GFX_ImageDraw() will be able to render these 
        images. This feature is enabled by default.

        <code>
            // example to use images that are NOT RLE encoded and sourced from
            // internal flash only
            #define GFX_CONFIG_RLE_DECODE_DISABLE
            #define GFX_CONFIG_IMAGE_EXTERNAL_DISABLE
            #define GFX_CONFIG_IMAGE_RAM_DISABLE
        </code>
        
        To disable this feature, add this macro in the configuration.
        
    Remarks:
        None.
        
*/
// *****************************************************************************
#define GFX_CONFIG_RLE_DECODE_DISABLE

// *****************************************************************************
/* 
    <GROUP  configuring_options_graphics_library_doc>

    Macro:
        GFX_CONFIG_IPU_DECODE_DISABLE

    Summary:
        Macro that disables RLE compression of images.
        
    Description:
        Images can also be compressed using the DEFLATE algorithm. Using the 
        drivers that supports DEFLATE (IPU of PIC24FJ256DA210 Family of devices), 
        GFX_ImageDraw() will be able to render these images. 
        This feature is enabled by default.
                
        To disable this feature, add this macro in the configuration.
        
    Remarks:
        None.
        
*/
// *****************************************************************************
#define GFX_CONFIG_IPU_DECODE_DISABLE

// *****************************************************************************
/* 
    <GROUP  configuring_options_graphics_library_doc>

    Macro:
        GFX_CONFIG_COLOR_DEPTH

    Summary:
        Macro that sets the color depth of the application.
        
    Description:
        This macro sets the color depth used in the application. The 
        library functions are also set to use the set color depth. 
        The valid values for the color depth are the following:
        1, 4, 8, 16 and 24 bpp. Usage of each is dependent on the 
        support available on the display driver used. See the specific
        display driver documentation to verify support of the chosen color
        depth.

        If this macro is not set, a build error will be generated.
        
    Remarks:
        None.
        
*/
// *****************************************************************************
#define GFX_CONFIG_COLOR_DEPTH 16

// *****************************************************************************
/* 
    <GROUP  configuring_options_graphics_library_doc>

    Macro:
        GFX_CONFIG_DOUBLE_BUFFERING_DISABLE

    Summary:
        Macro disables the support for double buffering in rendering of 
        pixels to the frame buffer.
        
    Description:
        In cases where display drivers has the resources for more than 
        one display buffer, double buffering can be implemented in the
        driver. This allows application to hide the rendering effects
        by rendering on a hidden buffer and displaying another one.
        Once the rendering is done, the buffer are swapped. This gives
        an instantaneous change in the buffers which makes the change in 
        the screen contents fast.
        The display driver must support the feature for this mode
        to work.
        
        In drivers where this feature is not supported, this macro 
        has no effect. In drivers that supports this feature, 
        adding this macro will disable the feature.
        
    Remarks:
        None.
        
*/
// *****************************************************************************
#define GFX_CONFIG_DOUBLE_BUFFERING_DISABLE

// *****************************************************************************
/* 
    <GROUP  configuring_options_graphics_library_doc>

    Macro:
        GFX_EXTERNAL_FONT_RASTER_BUFFER_SIZE

    Summary:
        Macro sets the size of the external font raster buffer.
        
    Description:
        This macro sets the size of the external font raster buffer. 
        This buffer is used to store the character glyph when the 
        glyph is being read from external source.
        The value that is needed will be calculated by the Graphics
        Resource Converter (GRC). The value will be shown in the 
        external resource reference c file that the GRC generates.

        <code>
            // example to set the buffer size.
            // value is taken from the output of the GRC 
            #define GFX_EXTERNAL_FONT_RASTER_BUFFER_SIZE 100
        </code>

        A warning will be issued when building the application when:
            - when the configuration do not define this macro
            - when the defined value is less than the required size
                    
        This macro will have no effect when fonts that are sourced 
        externally is not used.
        
    Remarks:
        None.
        
*/
// *****************************************************************************
#define GFX_EXTERNAL_FONT_RASTER_BUFFER_SIZE  /* DOM-IGNORE-BEGIN */ 51 /* DOM-IGNORE-END */

// *****************************************************************************
/* 
    <GROUP  configuring_options_graphics_library_doc>

    Macro:
        GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE

    Summary:
        Macro enables the support for auto refresh in bi-state
        display.
        
    Description:
        When using bi-state displays, this macro allows for widget 
        auto-update. This feature is only enabled in objects
        of the Object Layer. The direct calls to primitive rendering
        routines will need to be manually updated.

        To enable this feature, add this macro in the configuration.
        This macro has no effect in display drivers that do not support
        bistable auto refresh.
        
    Remarks:
        None.
        
*/
// *****************************************************************************
#define   GFX_CONFIG_BISTABLE_DISPLAY_AUTO_REFRESH_ENABLE

// *****************************************************************************
/* 
    <GROUP  configuring_options_graphics_library_doc>

    Macro:
        GFX_CONFIG_IMAGE_PADDING_DISABLE

    Summary:
        Macro disables the padding of bits on images converted by the 
        Graphics Resource Converter (GRC).
        
    Description:
        When converting images for use in the Graphics Library, the
        Graphics Resource Converter has an option to set the images
        to be padded or not padded. When images are padded, each 
        horizontal line will start on a byte boundary. 
        
        Unpadded images allows the least resource space for an image. 
        Unpadded images also allows support for display controllers 
        with windowing and auto-increment features.

        The images are created with padding by default. When the images
        are set to be not padded, application must define the macro in 
        the configuration to correctly process the images.

        Padded and unpadded images cannot be combined in one application.
        
    Remarks:
        None.
        
*/
// *****************************************************************************
#define GFX_CONFIG_IMAGE_PADDING_DISABLE

// *****************************************************************************
/* 
    <GROUP  configuring_options_graphics_library_doc>

    Macro:
        GFX_CONFIG_FONT_PROG_SPACE_ENABLE

    Summary:
        Macro enables the relocation of fonts in the program 
        space.
        
    Description:
        For XC16 builds only: When placing fonts in internal
        data memory, there is a 32K limit for data space. The total
        data should not exceed 32K. When this is unavoidable, the macro
        GFX_CONFIG_FONT_PROG_SPACE_ENABLE will relocate the font in
        program space. This will remove the 32K restriction but at
        the expense of slower access.
        
        To enable this feature, add this macro in the configuration.
        
    Remarks:
        None.
        
*/
// *****************************************************************************
#define GFX_CONFIG_FONT_PROG_SPACE_ENABLE

// *****************************************************************************
/* 
    <GROUP  configuring_options_graphics_library_doc>

    Macro:
        GFX_CONFIG_UNICODE_INDEX_TABLE_ENABLE

    Summary:
        Macro enables creation of an index table that maps the original 
        unicode character ID to the new character ID when font filtering 
        was applied when the font was generated by the Graphics Resource
        Converter (GRC).
        
    Description:
        Font filtering is used to save flash space storage of fonts. Filtering
        will remove from the font table characters that are not used in the
        application. The generated font table will only include all characters
        that are specified by the font filter file when the font table was
        generated by the GRC. The generated font table will not contain the 
        same character ID for the characters included in the table. 
        To aid in the usage of the filtered font an index table is generated 
        by GRC to show the mapping of the original character ID to the new
        character ID. 
        
        For example:
        A font is needed with the following characters only:
        + , - , 1, 2, 3, 4, 5, 6, 7, 8, 9, 0
        
        Generating the font table that will contain only these characters 
        will regenerate the character IDs of the filtered set.
        The table below shows the original and the new ID:
        ---------------------------------------
        character   orignal ID        new ID
        ---------------------------------------
            +           43              43  
            -           45              44  
            0           48              45  
            1           49              46  
            2           50              47  
            3           51              48  
            4           52              49  
            5           53              50  
            6           54              51  
            7           55              52  
            8           56              53  
            9           57              54  
            
        If the application needs to show integers from 0 to 9999, instead of 
        creating all possible strings of all numbers from 0 to 9999, one can 
        simply create a function that looks up the generated table to create
        strings from 0 to 9999.
                  
        To enable this feature, add this macro in the configuration.
        
    Remarks:
        None.
        
*/
// *****************************************************************************
#define GFX_CONFIG_UNICODE_INDEX_TABLE_ENABLE

// *****************************************************************************
/* 
    <GROUP  configuring_options_graphics_library_doc>

    Macro:
        GFX_malloc()

    Summary:
        Macro that defines the malloc function for versatility when 
        using Operating Systems.
        
    Description:
        This macro definition allows the application to replace the 
        malloc function to an equivalent function implemented in the
        operating system used.
        
        When using the library with object layer and without any 
        replacement functions for the malloc function, this macro
        must be defined as:
        <code>
            #define GFX_malloc(size)    malloc(size)
        </code>
        
    Remarks:
        None.
        
*/
// *****************************************************************************
#define GFX_malloc(size)    malloc(size)

// *****************************************************************************
/* 
    <GROUP  configuring_options_graphics_library_doc>

    Macro:
        GFX_free()

    Summary:
        Macro that defines the free function for versatility when 
        using Operating Systems.
        
    Description:
        This macro definition allows the application to replace the 
        free function to an equivalent function implemented in the
        operating system used.

        When using the library with object layer and without any 
        replacement functions for the malloc function, this macro
        must be defined as:
        <code>
            #define GFX_free(pObj)      free(pObj)
        </code>
        
    Remarks:
        None.
        
*/
// *****************************************************************************
#define GFX_free(pObj)      free(pObj)

// *****************************************************************************
/* 
    <GROUP  configuring_options_graphics_library_doc>

    Macro:
        GFX_CONFIG_USE_KEYBOARD_DISABLE

    Summary:
        This macro disables the keyboard support in objects.
        
    Description:
        Depending on the input devices used, messages that objects will
        process are encoded in the input device drivers. 
        The keyboard is one of the input devices that is supported 
        in selected objects.

        This support is enabled by default.
        To disable this feature, add this macro in the configuration.

    Remarks:
        None.
        
*/
// *****************************************************************************
#define GFX_CONFIG_USE_KEYBOARD_DISABLE

// *****************************************************************************
/* 
    <GROUP  configuring_options_graphics_library_doc>

    Macro:
        GFX_CONFIG_USE_TOUCHSCREEN_DISABLE

    Summary:
        This macro disables the resistive touchscreen support in objects.
        
    Description:
        Depending on the input devices used, messages that objects will
        process are encoded in the input device drivers. 
        The resistive touchscreen is one of the input devices that is 
        supported in selected objects.

        This support is enabled by default.
        To disable this feature, add this macro in the configuration.

    Remarks:
        None.
        
*/
// *****************************************************************************
#define GFX_CONFIG_USE_TOUCHSCREEN_DISABLE			


#endif // _GRAPHICS_CONFIG_H
