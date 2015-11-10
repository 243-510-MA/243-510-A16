/*******************************************************************************
 Graphics Resource Types Header for Microchip Graphics Library

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_types_resource.h

  Summary:
    This is the definition of the Graphics Resource Types used with
    the Microchip Graphics Library.

  Description:
    This header defines the different structures used for the Graphics
    Resources used in the Graphics Library. This also contains helpful
    macros as well as data types used.

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

#ifndef GFX_TYPES_RESOURCE_H
// DOM-IGNORE-BEGIN
    #define GFX_TYPES_RESOURCE_H
// DOM-IGNORE-END

/*****************************************************************************
 * SECTION: Includes
 *****************************************************************************/
#include <stdint.h>
#include "system_config.h"
#include "gfx/gfx_types_palette.h"
#include "gfx/gfx_types_font.h"
#include "gfx/gfx_types_image.h"

// *****************************************************************************
/* 
    <GROUP primitive_types>

    Typedef:
        GFX_RESOURCE

    Summary:
        Specifies the different resource types in the library.

    Description:
        This enumeration lists the different types of resources, how they
        are accessed and how the data of the resource will be interpreted 
        when rendered.
        
        A resource in the library has three major characteristics:
        1. Location or source of the resource - The following are the 
                recognized sources of resources
                    - internal flash memory type 
                    - internal RAM type
                    - external memory type 
                    - external EDS memory type  
        2. Data types of the resource - The following are the supported types of
                resources:
                - image
                - font
                - palette 
                - binary data
        3. Data compression type of the resource - the following are 
                the supported compression of data:
                - RLE - Run length encoded compression. This type of
                        compression is only available for 8 and 4 bpp
                        bitmaps.
                - IPU - Compressed data is encoded using the DEFLATE 
                        algorithm with fixed Huffman codes; dynamic 
                        Huffman codes are not supported.
        
        The first four types indicates the location of the resource.
        The next five types indicates which kind of resource and the next
        3 types indicates if the resource data is compressed or not.
        
        By combining these three groups, a resource can be described fully 
        and accessed appropriately in the library.
        For example: 
        By combining the location, type and compression into one type, 
        the library can pass the resource type parameter when rendering 
        the resource.
        GFX_RESOURCE_MCHP_MBITMAP_EXTERNAL_RLE  = 
            (   GFX_RESOURCE_TYPE_MCHP_MBITMAP | 
                GFX_RESOURCE_MEMORY_EXTERNAL   | 
                GFX_RESOURCE_COMP_RLE
            )

        Each type will determine how the library will access the resource 
        when rendering.

    Remarks:
        None.
        
*/
// *****************************************************************************
typedef enum
{
    // A location type: Internal flash memory.
    GFX_RESOURCE_MEMORY_FLASH
            /*DOM-IGNORE-BEGIN*/  = 0x0000 /*DOM-IGNORE-END*/,  
    // A location type: External memory.
    GFX_RESOURCE_MEMORY_EXTERNAL         
            /*DOM-IGNORE-BEGIN*/  = 0x0001 /*DOM-IGNORE-END*/,  
    // A location type: Random Access Memory (RAM).
    GFX_RESOURCE_MEMORY_RAM              
            /*DOM-IGNORE-BEGIN*/  = 0x0004 /*DOM-IGNORE-END*/,  
    // A location type: Resource is external and accessed through
    // Enhanced Parallel Master Port (EPMP), memory size and base 
    // addresses are are set in the configuration.
    GFX_RESOURCE_MEMORY_EDS_EPMP         
            /*DOM-IGNORE-BEGIN*/  = 0x0005 /*DOM-IGNORE-END*/,  

    // A data type: Microchip bitmap type of resource.
    GFX_RESOURCE_TYPE_MCHP_MBITMAP       
            /*DOM-IGNORE-BEGIN*/  = 0x0000 /*DOM-IGNORE-END*/,  
    // A data type: Image of type JPEG.
    GFX_RESOURCE_TYPE_JPEG    
            /*DOM-IGNORE-BEGIN*/  = 0x0100 /*DOM-IGNORE-END*/,  
    // A data type: Binary data type.
    GFX_RESOURCE_TYPE_BINARY   
            /*DOM-IGNORE-BEGIN*/  = 0x0200 /*DOM-IGNORE-END*/,  
    // A data type: Font type of data
    GFX_RESOURCE_TYPE_FONT               
            /*DOM-IGNORE-BEGIN*/  = 0x0300 /*DOM-IGNORE-END*/,  
    // A data type: Palette type of data
    GFX_RESOURCE_TYPE_PALETTE            
            /*DOM-IGNORE-BEGIN*/  = 0x0400 /*DOM-IGNORE-END*/,  

    // A compression type: Data resource has no compression.
    GFX_RESOURCE_COMP_NONE               
            /*DOM-IGNORE-BEGIN*/  = 0x0000 /*DOM-IGNORE-END*/,  
    // A compression type: Data resource is compressed 
    // with RLE.
    GFX_RESOURCE_COMP_RLE                
            /*DOM-IGNORE-BEGIN*/  = 0x1000 /*DOM-IGNORE-END*/,  
    // A compression type: Data resource compressed with  
    //                     DEFLATE algorithm (for IPU).
    GFX_RESOURCE_COMP_IPU                
            /*DOM-IGNORE-BEGIN*/  = 0x2000 /*DOM-IGNORE-END*/,  
                                                    

    // These are common resource combinations used by the graphics library
    // Microchip bitmap image type, located in flash and no compression.
    GFX_RESOURCE_MCHP_MBITMAP_FLASH_NONE = (GFX_RESOURCE_TYPE_MCHP_MBITMAP | GFX_RESOURCE_MEMORY_FLASH | GFX_RESOURCE_COMP_NONE),
    // Microchip bitmap image type, located in flash and RLE compressed.
    GFX_RESOURCE_MCHP_MBITMAP_FLASH_RLE  = (GFX_RESOURCE_TYPE_MCHP_MBITMAP | GFX_RESOURCE_MEMORY_FLASH | GFX_RESOURCE_COMP_RLE),
    // Microchip bitmap image type, located in flash and compressed for IPU
    GFX_RESOURCE_MCHP_MBITMAP_FLASH_IPU  = (GFX_RESOURCE_TYPE_MCHP_MBITMAP | GFX_RESOURCE_MEMORY_FLASH | GFX_RESOURCE_COMP_IPU),

    // Microchip bitmap image type, located in external memory and no compression.
    GFX_RESOURCE_MCHP_MBITMAP_EXTERNAL_NONE    = (GFX_RESOURCE_TYPE_MCHP_MBITMAP | GFX_RESOURCE_MEMORY_EXTERNAL | GFX_RESOURCE_COMP_NONE),
    // Microchip bitmap image type, located in external memory and RLE compressed.
    GFX_RESOURCE_MCHP_MBITMAP_EXTERNAL_RLE     = (GFX_RESOURCE_TYPE_MCHP_MBITMAP | GFX_RESOURCE_MEMORY_EXTERNAL | GFX_RESOURCE_COMP_RLE),
    // Microchip bitmap image type, located in external memory and compressed for IPU.
    GFX_RESOURCE_MCHP_MBITMAP_EXTERNAL_IPU     = (GFX_RESOURCE_TYPE_MCHP_MBITMAP | GFX_RESOURCE_MEMORY_EXTERNAL | GFX_RESOURCE_COMP_IPU),

    // Microchip bitmap image type, located in EDS memory and no compression.
    GFX_RESOURCE_MCHP_MBITMAP_EDS_EPMP_NONE    = (GFX_RESOURCE_TYPE_MCHP_MBITMAP | GFX_RESOURCE_MEMORY_EDS_EPMP | GFX_RESOURCE_COMP_NONE),
    // Microchip bitmap image type, located in EDS memory and RLE compressed.
    GFX_RESOURCE_MCHP_MBITMAP_EDS_EPMP_RLE     = (GFX_RESOURCE_TYPE_MCHP_MBITMAP | GFX_RESOURCE_MEMORY_EDS_EPMP | GFX_RESOURCE_COMP_RLE),
    // Microchip bitmap image type, located in EDS memory and compressed for IPU.
    GFX_RESOURCE_MCHP_MBITMAP_EDS_EPMP_IPU     = (GFX_RESOURCE_TYPE_MCHP_MBITMAP | GFX_RESOURCE_MEMORY_EDS_EPMP | GFX_RESOURCE_COMP_IPU),

    // JPEG image type, located in flash and no compression.
    GFX_RESOURCE_JPEG_FLASH_NONE         = (GFX_RESOURCE_TYPE_JPEG | GFX_RESOURCE_MEMORY_FLASH | GFX_RESOURCE_COMP_NONE),
    // JPEG image type, located in external memory no compression.
    GFX_RESOURCE_JPEG_EXTERNAL_NONE      = (GFX_RESOURCE_TYPE_JPEG | GFX_RESOURCE_MEMORY_EXTERNAL | GFX_RESOURCE_COMP_NONE),
    // JPEG image type, located in EDS memory and no compression.
    GFX_RESOURCE_JPEG_EDS_EPMP_NONE      = (GFX_RESOURCE_TYPE_JPEG | GFX_RESOURCE_MEMORY_EDS_EPMP | GFX_RESOURCE_COMP_NONE),

    // Binary image type, located in flash and no compression.
    GFX_RESOURCE_BINARY_FLASH_NONE       = (GFX_RESOURCE_TYPE_BINARY | GFX_RESOURCE_MEMORY_FLASH | GFX_RESOURCE_COMP_NONE),
    // Binary image type, located in flash memory and compressed for IPU.
    GFX_RESOURCE_BINARY_FLASH_IPU        = (GFX_RESOURCE_TYPE_BINARY | GFX_RESOURCE_MEMORY_FLASH | GFX_RESOURCE_COMP_IPU),

    // Binary image type, located in external memory and no compression.
    GFX_RESOURCE_BINARY_EXTERNAL_NONE    = (GFX_RESOURCE_TYPE_BINARY | GFX_RESOURCE_MEMORY_EXTERNAL | GFX_RESOURCE_COMP_NONE),
    // Binary image type, located in external memory and compressed for IPU.
    GFX_RESOURCE_BINARY_EXTERNAL_IPU     = (GFX_RESOURCE_TYPE_BINARY | GFX_RESOURCE_MEMORY_EXTERNAL | GFX_RESOURCE_COMP_IPU),

    // Binary image type, located in EDS memory and no compression.
    GFX_RESOURCE_BINARY_EDS_EPMP_NONE    = (GFX_RESOURCE_TYPE_BINARY | GFX_RESOURCE_MEMORY_EDS_EPMP | GFX_RESOURCE_COMP_NONE),
    // Binary image type, located in EDS memory and compressed for IPU.
    GFX_RESOURCE_BINARY_EDS_EPMP_IPU     = (GFX_RESOURCE_TYPE_BINARY | GFX_RESOURCE_MEMORY_EDS_EPMP | GFX_RESOURCE_COMP_IPU),

    // Font type, located in flash and no compression.
    GFX_RESOURCE_FONT_FLASH_NONE         = (GFX_RESOURCE_TYPE_FONT | GFX_RESOURCE_MEMORY_FLASH | GFX_RESOURCE_COMP_NONE),
    // Font type, located in external memory and no compression.
    GFX_RESOURCE_FONT_EXTERNAL_NONE      = (GFX_RESOURCE_TYPE_FONT | GFX_RESOURCE_MEMORY_EXTERNAL | GFX_RESOURCE_COMP_NONE),
    // Font type, located in RAM and no compression.
    GFX_RESOURCE_FONT_RAM_NONE           = (GFX_RESOURCE_TYPE_FONT | GFX_RESOURCE_MEMORY_RAM | GFX_RESOURCE_COMP_NONE),
    // Font type, located in EDS memory and no compression.
    GFX_RESOURCE_FONT_EDS_NONE           = (GFX_RESOURCE_TYPE_FONT | GFX_RESOURCE_MEMORY_EDS_EPMP | GFX_RESOURCE_COMP_NONE),

    // Palette type, located in flash and no compression.
    GFX_RESOURCE_PALETTE_FLASH_NONE      = (GFX_RESOURCE_TYPE_PALETTE | GFX_RESOURCE_MEMORY_FLASH | GFX_RESOURCE_COMP_NONE),
    // Palette type, located in external memory and no compression.
    GFX_RESOURCE_PALETTE_EXTERNAL_NONE   = (GFX_RESOURCE_TYPE_PALETTE | GFX_RESOURCE_MEMORY_EXTERNAL | GFX_RESOURCE_COMP_NONE),
    // Palette type, located in RAM and no compression.
    GFX_RESOURCE_PALETTE_RAM_NONE        = (GFX_RESOURCE_TYPE_PALETTE | GFX_RESOURCE_MEMORY_RAM | GFX_RESOURCE_COMP_NONE),

} GFX_RESOURCE;

#define GFX_TYPE_MASK           0x0F00
#define GFX_COMP_MASK           0xF000
#define GFX_MEM_MASK            0x000F

#define MCHP_BITMAP_NORMAL      0x00                // no compression, palette is present
                                                    // for color depth = 8, 4 and 1 BPP
#define MCHP_BITMAP_PALETTE_STR 0x10                // palette is provided as a separate
                                                    // object (see PALETTE_HEADER) for
                                                    // color depth = 8, 4, and 1 BPP,
                                                    // ID to the palette is embedded in the bitmap.

// *****************************************************************************
/* 
    <GROUP primitive_types>

    Typedef:
        GFX_RESOURCE_IMAGE

    Summary:
        Defines the structure used for the image type resource.
        
    Description:
        Defines the structure used for the image type resource.

    Remarks:
        None.
        
*/
// *****************************************************************************
typedef struct
{
    // This defines the location of the image resource in memory.
    // Depending on the type, the address location is interpreted accordingly.
    union
    {
        uint32_t                extAddress;         // An external address.
        uint8_gfx_image_prog    *progByteAddress;   // An 8-bit addresses  
                                                    // in the program section.
        uint16_gfx_image_prog   *progWordAddress;   // A 16-bit addresses in 
                                                    // the program section.
        const char              *constAddress;      // An addresses in constant
                                                    // space in flash.
        char                    *ramAddress;        // An addresses in RAM.
#if defined(__PIC24F__)
        __eds__ char            *edsAddress;        // An addresses in EDS.
#endif
    }location;
    
    uint16_t            width;                      // The width of the image.
    uint16_t            height;                     // The height of the image.

    // A generic parameter 1 that changes in usage depending on the
    // type of the resource.
    union
    {
        uint32_t        compressedSize;             // Parameters used for the GFX_RESOURCE.
                                                    // Depending on the GFX_RESOURCE type
                                                    // definition of param1 can change. For
                                                    // IPU and RLE compressed images, param1
                                                    // indicates the compressed size of the image.
        uint32_t        reserved;
    } parameter1;

    // A generic parameter 2 that changes in usage depending on the
    // type of the resource.
    union
    {
        uint32_t        rawSize;                    // Parameters used for the GFX_RESOURCE.
                                                    // Depending on the GFX_RESOURCE type
                                                    // definition of param2 can change. For
                                                    // IPU and RLE compressed images, param2
                                                    // indicates the uncompressed size of the image.
        uint32_t        reserved;
    } parameter2;

    uint8_t             colorDepth;                 // The color depth of the image.
    uint8_t             type;                       // The type of image, information on how to 
                                                    // render the image  
                                                    // 0x00 - no compression, palette is present    
                                                    //       for color depth = 8, 4 and 1 BPP   
                                                    // 0x10 - palette is provided as a separate   
                                                    //        object (see PALETTE_HEADER) for    
                                                    //        color depth = 8, 4, and 1 BPP,    
                                                    //        ID to the palette is embedded
                                                    //        color depth = 8, 4, and 1 BPP,    
                                                    //        in the bitmap.
                                                    // 0xYY - reserved
    uint16_t            paletteID;                  // The palette ID, 
                                                    // if type == MCHP_BITMAP_PALETTE_STR (0x10),
                                                    // this represents the unique 
                                                    // ID of the palette being used 
    
}GFX_RESOURCE_IMAGE;

// *****************************************************************************
/* 
    <GROUP primitive_types>

    Typedef:
        GFX_RESOURCE_FONT

    Summary:
        Defines the structure used for the font type resource.
        
    Description:
        Defines the structure used for the font type resource.

    Remarks:
        None.
        
*/
// *****************************************************************************
typedef struct
{
    // This defines the location of the font resource in memory.
    // Depending on the type, the address location is interpreted accordingly.
    union
    {
        uint32_t                extAddress;         // An external address. 
        GFX_FONT_SPACE char     *progByteAddress;   // An 8-bit addresses  
                                                    // in the program section.
        char                    *ramAddress;        // An addresses in RAM.
#if defined(__PIC24F__)
        __eds__ char            *edsAddress;        // An addresses in EDS.
#endif
    }location;
    GFX_FONT_HEADER             header;             // The header that describes
                                                    // the font resource.
}GFX_RESOURCE_FONT;

// *****************************************************************************
/* 
    <GROUP primitive_types>

    Typedef:
        GFX_RESOURCE_PALETTE

    Summary:
        Defines the structure used for the palette type resource.
        
    Description:
        Defines the structure used for the palette type resource.

    Remarks:
        None.
        
*/
// *****************************************************************************
typedef struct
{
    // This defines the location of the palette resource in memory.
    // Depending on the type, the address location is interpreted accordingly.
    union
    {
        uint32_t                extAddress;         // An external address. 
        const GFX_PALETTE_ENTRY *constByteAddress;  // An addresses in constant
                                                    // space in flash.
        GFX_PALETTE_ENTRY       *ramAddress;        // An addresses in RAM.
    }location;    
    
    uint16_t numberOfEntries;                       // The number of color entries
                                                    // in the palette resource.
    uint16_t paletteID;                             // Unique ID of the palette that will match the ID
                                                    // in the GFX_RESOURCE_HDR if the image has palette
                                                    // removed and supplied as a separate object.

}GFX_RESOURCE_PALETTE;

// *****************************************************************************
/* 
    <GROUP primitive_types>

    Typedef:
        GFX_RESOURCE_BINARY

    Summary:
        Defines the structure used for the binary type resource.
        
    Description:
        Defines the structure used for the binary type resource.

    Remarks:
        None.
        
*/
// *****************************************************************************
typedef struct
{
    // This defines the location of the binary resource in memory.
    // Depending on the type, the address location is interpreted accordingly.
     union
    {
        uint32_t                extAddress;         // An external address. 
        uint8_gfx_image_prog    *progByteAddress;   // An 8-bit addresses  
                                                    // in the program section.
        uint16_gfx_image_prog   *progWordAddress;   // A 16-bit addresses in 
                                                    // the program section.
        const char              *constAddress;      // An addresses in constant
                                                    // space in flash.
        char                    *ramAddress;        // for addresses in RAM.
#if defined(__PIC24F__)
        __eds__ char            *edsAddress;        // for addresses in EDS.
#endif
    }location;

    uint32_t                    size;               // The size of the binary data in bytes.
    uint32_t                    param1;             // Parameters used for the GFX_RESOURCE. Depending on the GFX_RESOURCE type
                                                    // definition of param1 can change. For IPU and RLE compressed images, param1
                                                    // indicates the compressed size of the image.
    uint32_t                    param2;             // Parameters used for the GFX_RESOURCE. Depending on the GFX_RESOURCE type

}GFX_RESOURCE_BINARY;

// *****************************************************************************
/* 
    <GROUP primitive_types>

    Typedef:
        GFX_RESOURCE_HDR

    Summary:
        Defines the structure used for the resource types.
        
    Description:
        Defines the common structure used for all the graphics resources.

    Remarks:
        None.
        
*/
// *****************************************************************************
typedef struct
{
    GFX_RESOURCE                type;       // Graphics resource type, determines the type 
                                            // and location of data
    uint16_t                    ID;         // memory ID, user defined value to differentiate
                                            // between graphics resources of the same type
                                            //   When using EDS_EPMP the following ID values are
                                            //   reserved and used by the Microchip display driver
                                            //   0 - reserved (do not use)
                                            //   1 - reserved for base address of EPMP CS1
                                            //   2 - reserved for base address of EPMP CS2
    
    // This defines the type of the resource.
    // Depending on the type, the resource is accessed and rendered accordingly.
    union
    {
        GFX_RESOURCE_IMAGE      image;      // Resource is an image.
        GFX_RESOURCE_FONT       font;       // Resource is font.
        GFX_RESOURCE_BINARY     binary;     // Resource is binary.
        GFX_RESOURCE_PALETTE    palette;    // Resource is palette.
    }resource;

}GFX_RESOURCE_HDR;

#endif // GFX_TYPES_RESOURCE_H

