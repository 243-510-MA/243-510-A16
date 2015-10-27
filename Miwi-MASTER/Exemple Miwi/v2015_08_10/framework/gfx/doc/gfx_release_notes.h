/*******************************************************************************
 Module for Microchip Graphics Library - Graphic Library Documentation

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_release_notes.h

  Summary:
    Release notes file for the Microchip Graphics Library.

  Description:
    This is the release notes summary of the Microchip Graphics Library
    releases. This summarizes all the releases of the library, known
    issues and fixes.
    
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

/*******************************************************************************
================================================================================
        Primitive Layer Documentation
================================================================================
*******************************************************************************/

//@@graphics_release_notes
/*
    <TITLE Release Notes>

    Summary: 
        This section describes the release notes of the  
        Microchip Graphics Library.

    Description: 
        
        %MODULE_LONG_NAME% v4.60 (2015-08-10)

        <b>New This Release:</b>
          - Added demo support for PIC24FJ256GB410 Family of devices.
            - apps/gfx/app_notes
            - apps/gfx/primitive_layer
            
        <b>Known Issues: </b>
          - This release does not support the following features:
            - Image Decoder routines
          - Anti-aliasing and extended glyph features are not supported when using 
            PIC24FJ256DA210 CHRGPU.
          - When using PIC24FJ256GB210 PIM with Explorer 16 board that has a 5v Lumex 
            LCD display, the S1D13517 demo does not run correctly.
          - When using XC16 Compiler V1.00, add "-fno-ivopts" compile option. This is 
            a known issue V1.00 of XC16.
          - When using ListBox Widget, the widget height should be greater than the 
            height of the font used for the widget.
          - No GDD xml template included in this GFX release.
          
        <b>Fixes: </b>
          - Fixed issue on progress bar when initializing the range, the position 
            should not reset to zero.
          - Fixed build error on list box when focus is enabled. 
          - Fixed build error on radio button when focus is enabled with touch screen
            enabled.
          - Fixed bug on list box when selection is changed and the list is empty.
          - Removed build warning on drv_gfx_tft003 driver when transparent color 
            feature is disabled.
                    
        <b>Changes: </b>
          None
                
        <b>Deprecated Items: </b>
          None  
        
        <b>Application Notes </b>
          - <extlink http://www.microchip.com/stellent/idcplg?IdcService=SS_GET_PAGE&amp;nodeId=1824&amp;appnote=en532124>AN1136</extlink> How to Use Widgets in Microchip Graphics Library
          - <extlink http://www.microchip.com/stellent/idcplg?IdcService=SS_GET_PAGE&amp;nodeId=1824&amp;appnote=en547920>AN1182</extlink> Fonts in the Microchip Graphics Library
          - <extlink http://www.microchip.com/stellent/idcplg?IdcService=SS_GET_PAGE&amp;nodeId=1824&amp;appnote=en536920>AN1227</extlink> Using a Keyboard with the Microchip Graphics Library
          - <extlink http://www.microchip.com/stellent/idcplg?IdcService=SS_GET_PAGE&amp;nodeId=1824&amp;appnote=en539276>AN1246</extlink> How to Create Widgets in Microchip Graphics Library
          - <extlink http://www.microchip.com/stellent/idcplg?IdcService=SS_GET_PAGE&amp;nodeId=1824&amp;appnote=en553721>AN1368</extlink> Developing Graphics Applications using PIC MCU with Integrated Graphics Controller
   
        <b>PIC Family </b>
          This version of the library supports PIC24 and dsPIC Families.
        
        <b>Development Tools </b>
          This graphics library release was tested with 
            - MPLAB X IDE Version 3.05
            - XC16 v1.25.
        
        <b>Documentation of Resources and Utilities </b>
          - The Graphics Library Help File and API document is located in 
            - \<mla_root_folder\>/doc/           
          - "Graphics Resource Converter" documentation is located in 
            - \<mla_root_folder\>/framework/gfx/utilities/grc
          - External Memory Programmer documentation is located in 
            - \<mla_root_folder\>/framework/gfx/utilities/memory_programmer
          where:
          - "mla_root_folder" is the location of the Microchip Libraries for Applications 
             installation.
        
        <b>Information in Adding Resources Generated by the Graphics Resource Converter (GRC) </b>
          The GRC is used when modifying or adding fonts and images to applications. GRC creates
          multiple files for resources placed in internal flash or external memory.
          The following files are generated by GRC:
          - Resources placed in internal flash. 
            - file_name_given_reference.c
            - file_name_given.h
            - file_name_given.S
          - Resources placed in external memory. 
            - file_name_given_reference.c
            - file_name_given.h
            - file_name_given.hex
        
          When adding resources in MPLAB X projects, it is necessary to add include paths to
          both C and assembler (when adding resources placed in internal flash).
        
        <img name="c_include_path.jpg" />
        
        
        <img name="assembler_include_path.jpg" />

        <b> Previous Versions Log </b>

        <b> v4.50 (2015-05-15) </b>

        New:
          - Added support for SST26FV064B, SST26FV032B and SST26VF016B
            Serial Quad I/O (SQI) flash devices. Added only the SPI mode support. 
          - Added support for DisplayTech EMB028TFTDEV 2.8" TFT 
            Development Board for Microchip Development Tools. 
          - Ported support from Graphics Library v3.xx for 16-bit 
            Enhanced Parallel Port (EPMP) interface for 16-bit parallel 
            TFT displays.
        
        Known Issues:
          - This release does not support the following features:
            - Image Decoder routines
          - Anti-aliasing and extended glyph features are not supported when using 
            PIC24FJ256DA210 CHRGPU.
          - When using PIC24FJ256GB210 PIM with Explorer 16 board that has a 5v Lumex 
            LCD display, the S1D13517 demo does not run correctly.
          - When using XC16 Compiler V1.00, add "-fno-ivopts" compile option. This is 
            a known issue V1.00 of XC16.
          - When using ListBox Widget, the widget height should be greater than the 
            height of the font used for the widget.
          - No GDD xml template included in this GFX release.
        
        Fixes:
          - Fixed issue on EditBox Widget when using common background color as
            background and string in the edit box is replaced. The old string
            should be erased first before the new string is rendered.
          - Added a default GOL draw callback function. Application code does not need 
            to create the GOL draw callback function when the default rendering for
            each Widget is used. Application code also does not need to call the 
            GFX_GOL_DrawCallbackSet() function.
          - Fixed an error on the SST25VF016 driver missing a byte when the number 
            of programmed bytes is odd.
            
        Changes:
          - Modified Graphics Resource Converter (GRC) tool to remove XC16 compiler
            warnings.
          - Replaced Graphics Library feature macros:
            GFX_LIB_CFG_USE_FOCUS to GFX_CONFIG_FOCUS_DISABLE
            GFX_LIB_CFG_COLOR_DEPTH to GFX_CONFIG_COLOR_DEPTH
            GFX_LIB_CFG_USE_PALETTE to GFX_CONFIG_PALETTE_DISABLE
          - Replaced NVM driver hardware abstraction macros (system_config.h):
            DRV_NVM_M25P80_CSLow() to DRV_NVM_M25P80_ChipSelectEnable()
            DRV_NVM_M25P80_CSHigh() to DRV_NVM_M25P80_ChipSelectDisable()
            These two macros are now required to be defined in system_config.h.
            For example, 
                #define DRV_NVM_M25P80_ChipSelectEnable()   (LATCbits.LATC2 = 0)
                #define DRV_NVM_M25P80_ChipSelectDisable()  (LATCbits.LATC2 = 1)
                     
          - Simplified Resistive Touch hardware abstraction macros:
            - Replaced Resistive Touch hardware abstraction macros:
              - TOUCHSCREEN_RESISTIVE_SWAP_XY to RESISTIVETOUCH_SWAP_XY
              - TOUCHSCREEN_RESISTIVE_FLIP_Y to RESISTIVETOUCH_FLIP_Y
              - TOUCHSCREEN_RESISTIVE_FLIP_X to RESISTIVETOUCH_FLIP_X
              - TOUCH_ADC_INPUT_SEL to RESISTIVETOUCH_ADC_INPUT_SEL
              - TOUCH_ADC_START to RESISTIVETOUCH_ADC_START
              - TOUCH_ADC_DONE to RESISTIVETOUCH_ADC_DONE
              - ADPCFG_XPOS to RESISTIVETOUCH_ADC_XPOS
              - ADPCFG_YPOS to RESISTIVETOUCH_ADC_YPOS
            - Added X port definitions:
              - RESISTIVETOUCH_XP_Signal
              - RESISTIVETOUCH_XM_Signal
              - RESISTIVETOUCH_XP_Direction
              - RESISTIVETOUCH_XM_Direction            
                For example,
                #define RESISTIVETOUCH_XP_Signal            LATBbits.LATB13
                #define RESISTIVETOUCH_XM_Signal            LATBbits.LATB11
                #define RESISTIVETOUCH_XP_Direction         TRISBbits.TRISB13
                #define RESISTIVETOUCH_XM_Direction         TRISBbits.TRISB11
            - Added Y port definitions:
              - RESISTIVETOUCH_YP_Signal
              - RESISTIVETOUCH_YM_Signal
              - RESISTIVETOUCH_YP_Direction
              - RESISTIVETOUCH_YM_Direction
                For example,
                #define RESISTIVETOUCH_YP_Signal            LATBbits.LATB12
                #define RESISTIVETOUCH_YM_Signal            LATBbits.LATB10
                #define RESISTIVETOUCH_YP_Direction         TRISBbits.TRISB12
                #define RESISTIVETOUCH_YM_Direction         TRISBbits.TRISB10
          - Removed the macro: GFX_LIB_CFG_USE_TRANSITION_EFFECTS. The transition
            effect feature was removed from Version 4.00 for 16-bit devices. In this
            release, the code supporting it is completely removed.
          - Support for potentiometer reading in resistive touch driver is removed.
          
        Deprecated Items:
          - GFX_LIB_CFG_USE_TRANSITION_EFFECTS macro
          - Removed Resistive Touch Macros:
              - ResistiveTouchScreen_XPlus_Drive_High
              - ResistiveTouchScreen_XPlus_Drive_Low
              - ResistiveTouchScreen_XPlus_Config_As_Input
              - ResistiveTouchScreen_XPlus_Config_As_Output
              - ResistiveTouchScreen_XMinus_Drive_High
              - ResistiveTouchScreen_XMinus_Drive_Low
              - ResistiveTouchScreen_XMinus_Config_As_Input
              - ResistiveTouchScreen_XMinus_Config_As_Output
              - ResistiveTouchScreen_YPlus_Drive_High
              - ResistiveTouchScreen_YPlus_Drive_Low
              - ResistiveTouchScreen_YPlus_Config_As_Input
              - ResistiveTouchScreen_YPlus_Config_As_Output
              - ResistiveTouchScreen_YMinus_Drive_High
              - ResistiveTouchScreen_YMinus_Drive_Low
              - ResistiveTouchScreen_YMinus_Config_As_Input
              - ResistiveTouchScreen_YMinus_Config_As_Output
              - USE_TOUCHSCREEN_RESISTIVE, no need to define this
                when using resistive touch screen driver.

        <b> v4.10 (2014-07-22) </b>

        New:
          - Enabled 1, 4 and 8 bpp color depth modes. 
          - Enabled Palette on PIC24FJ256DA210 family of devices.
          - Added Color Depth Demo for PIC24FJ256DA210 Development 
            Board (DM240312) and 3.2' QVGA Truly TFT Display Board (AC164127-4).
          - Modified Meter Widget. Removed accuracy state bit. Accuracy 
            is now selectable by defining the macro GFX_CONFIG_OBJECT_METER_RESOLUTION.
            Can be set to 1, 2, 3, 4, or 5 decimal places.
          - Added API for setting up scheme items 
            (See <link gol_objects_style_scheme, GOL Object Style Scheme> list of APIs).
            
        Known Issues:
          - This release does not support the following features:
            - Image Decoder routines
          - Anti-aliasing and extended glyph features are not supported when using 
            PIC24FJ256DA210 CHRGPU.
          - When using PIC24FJ256GB210 PIM with Explorer 16 board that has a 5v Lumex 
            LCD display, the S1D13517 demo does not run correctly.
          - When using XC16 Compiler V1.00, add "-fno-ivopts" compile option. This is 
            a known issue V1.00 of XC16.
          - When using ListBox Widget, the widget height should be greater than the 
            height of the font used for the widget.
          - No GDD xml template included in this GFX release.
          
        Fixes:
          - Fixed issue Meter Widget when resolution state enables the use of
            decimal point when displaying value.
          - Fixed issue on GFX_PixelArrayPut() where the currently set color
            gets over written after the function returns.
          - Fix error in Static Text when background color fill is disabled.
        
        Changes:
          - In the Meter Widget, the accuracy state bit (GFX_GOL_METER_ACCURACY_STATE)
            is removed. Instead, the accuracy can now be set using the build
            time macro GFX_CONFIG_OBJECT_METER_DECIMAL_PLACES to set
            the decimal places when the Meter value and scale is displayed.
            The number of decimal places is 0 - 5.
          - Modified double buffering feature initialization. 
            (See <link primitive_double_buffering_doc, Double Buffering Features>).
          - Modified the Property State Bit macro for disabling the background 
            color fill from GFX_GOL_STATICTEXT_NOBACKGROUND_STATE to 
            GFX_GOL_STATICTEXT_NOBACKGROUNDCOLORFILL_STATE.
          - (Since Version 4.00) Distribution of Graphics Object Default Scheme 
            is discontinued. Application is now responsible for initializing 
            style schemes.
          - (Since Version 4.00) Distribution of Default Font is discontinued. 
            Application is now responsible of creating fonts for application use.
        
        Deprecated Items:
          - None

        <b> v4.00 (2013-12-20) </b>
        New:
        - Major revision of the API set of the library. All the API names are modified.
          Refer to summary of API changes in the Changes section.
        - When rendering unfilled polygon, new API added and old API are renamed 
        <TABLE>
            <b>NEW API Name</b>                     <b>Old API Name</b>
            #############                           #######
            GFX_CircleDraw() xxx                    Circle()
            GFX_RectangleDraw()                     Rectangle()
            GFX_RectangleRoundDraw()                Bevel()
            GFX_PolygonDraw()                       DrawPoly()
        </TABLE>
            
        <code>
            // example
            GFX_LineStyleSet(GFX_LINE_STYLE_THIN_SOLID);
            GFX_ColorSet(BRIGHTRED);
            GFX_RectangleDraw(left, top, right, bottom);
            GFX_LineDraw(x1, y1, x2, y2);
            GFX_CircleDraw(x, y, center);
        </code>
            
            
        - When rendering filled polygon, new API added and old API are renamed 
        <TABLE>
            <b>NEW API Name</b>                     <b>Old API Name</b>
            #############                           #######
            GFX_CircleFillDraw()                    CircleFill()
            GFX_RectangleFillDraw()                 BarGradient()
                                                        BarAlpha()
            GFX_RectangleRoundFillDraw()            BevelFill()
            GFX_BarDraw()                           Bar()
        </TABLE>
            
        <code>
            // example
            GFX_FillStyleSet(GFX_FILL_STYLE_ALPHA_COLOR);
            GFX_ColorSet(BRIGHTRED);
            GFX_RectangleFillDraw(left, top, right, bottom);
            GFX_CircleFillDraw(x, y, center);
        </code>
            
            
        - When rendering filled polygons, the fill style is now a parameter
          (see GFX_FILL_STYLE). Alpha blending and gradient are now set 
          as a fill style.
        <code>
            // example
            GFX_FillStyleSet(GFX_FILL_STYLE_GRADIENT_DOUBLE_VER);
            GFX_GradientColorSet(BLUE, RED);
            GFX_RectangleFillDraw(50, 110, 150, 200, 20);                

            GFX_FillStyleSet(GFX_FILL_STYLE_ALPHA_COLOR);
            GFX_ColorSet(GREEN);
            GFX_RectangleRoundFillDraw(50, 110, 150, 200, 20);                
        </code>
            
        - When rendering strings, you can now render the strings into a defined
          rectangular area and align the text.
        <TABLE>
            <b>NEW API Name</b>                     <b>Old API Name</b>
            #############                           #######
            GFX_FontSet()                           SetFont()
            GFX_FontGet()                           \--
            GFX_TextCursorPositionSet()             MoveTo()
            GFX_TextCursorPositionXGet()            GetX()
            GFX_TextCursorPositionYGet()            GetY()
            GFX_FontAntiAliasSet()                  GFX_Font_SetAntiAliasType()
            GFX_FontAntiAliasGet()                  GFX_Font_GetAntiAliasType()
            deprecated                              SetFontOrientation()
            deprecated                              GetFontOrientation()
            GFX_TextCharDraw()                      OutChar()
            deprecated                              OutText()
            GFX_TextStringDraw()                    OutTextXY()
            GFX_TextStringBoxDraw()                 \--
            GFX_TextStringHeightGet()               GetTextHeight()
            GFX_TextStringWidthGet()                GetTextWidth()
        </TABLE>
            
        <code>
            // example
            GFX_XCHAR   charArray[] = "Test String";

            GFX_FontSet(myFont);
            GFX_ColorSet(BRIGHTRED);

            // render the whole string centered in the defined rectangular area
            GFX_TextStringBoxDraw(  left,  top, 
                                    width, height, 
                                    charArray, 0, 
                                    GFX_ALIGN_CENTER);
        </code> 
            
        - Added GFX_PixelArrayPut() and GFX_PixelArrayGet() to improve efficiency
          of functions that perform color fills functions as well as rendering
          of images. This leads to consolidation of image rendering functions in 
          drivers to be implemented in primitive layer. The drivers will just 
          need to implement these two functions.
              
        - Combined API to mix styles. Previous implementation of line draw will need
          to setup the line type and line thickness separately. New API will now 
          setup the line style  which is an enumeration of line thickness and 
          types (see GFX_LINE_STYLE). 
        <code>
            // example
            GFX_ColorSet(BRIGHTRED);
            GFX_LineStyleSet(GFX_LINE_STYLE_THIN_SOLID);
            GFX_LineDraw(10, 10,100,  10);

            GFX_ColorSet(BRIGHTBLUE);
            GFX_LineStyleSet(GFX_LINE_STYLE_THICK_DASHED);
            GFX_CircleDraw(100, 100,  50);
        </code> 
        
        - Removed default font implementation. Application is now responsible 
          in generating fonts.
        - Removed default style scheme implementation. Application is now responsible
          in initializing the style schemes used by the objects.
        - Button object now has two image pointers to allow easy use of a Button 
          with two images. One image will be assigned to the press state and 
          the other image assigned to the release state.
        - Added support for primitive layer background variable. This variable 
          defines the background information which optimizes the refresh of areas 
          that the background occupies.
        - Folder path for utilities:
          - Graphics Resource Converter is now in \<mla_root_folder\>/framework/gfx/utilities/grc.
          - External Memory Programmer is now in \<mla_root_folder\>/framework/gfx/utilities/memory_programmer.  
        - Restructure of the directory tree of the library. 
          - Library files are now located in: \<mla_root_folder\>/framework/gfx
          - Driver files are now located in: \<mla_root_folder\>/framework/driver/gfx
          - Demo projects are now located in: \<mla_root_folder\>/apps/gfx
          - Documentation is now located in: \<mla_root_folder\>/doc
        - Modified SPI driver to use enhanced buffering scheme. The new driver improved
          performance of reading SPI devices.  
        - Demo projects released with this version:
            - Primitive Layer Demo
            - Application Notes Demo
        - Objects not ported with this release (may or may not be added in future
          releases):
            - Chart object 
            - Grid object 
            - Analog Clock

        Known Issues:
        - This release does not support the following features:
            - Palette on PIC24FJ256DA210 family of devices.
            - Image Decoder routines
        - Anti-aliasing and extended glyph features are not supported when using 
          PIC24FJ256DA210 CHRGPU.
        - When using PIC24FJ256GB210 PIM with Explorer 16 board that has a 5v Lumex 
          LCD display, the S1D13517 demo does not run correctly.
        - When using XC16 Compiler V1.00, add "-fno-ivopts" compile option. This is 
          a known issue V1.00 of XC16.
        - When using ListBox Widget, the widget height should be greater than the 
          height of the font used for the widget.
        - No GDD xml template included in this GFX release.
        
        Fixes:
        - Fixed issue on extended glyph for certain font (such as Thai) when used
          with Static text object is clipped. 
        
        Changes:
        - Distribution of Graphics Object Default Scheme is discontinued.
          Application is now responsible for initializing style schemes.
        - Distribution of Default Font is discontinued. Application is 
          now responsible of creating fonts for application use.
        - Summary of API changes.
            - <b><i> Primitive Layer:  </i></b>      
            <TABLE>
                <b>NEW API Name</b>                     <b>Old API Name</b>
                ############################            ###########################
                GFX_Initialize()                        InitGraph()
                GFX_ScreenClear()                       ClearDevice()
                GFX_ColorSet()                          SetColor()
                GFX_ColorGet()                          GetColor()
                GFX_TransparentColorEnable()            TransparentColorEnable()
                GFX_TransparentColorDisable()           TransparentColorDisable()
                GFX_TransparentColorGet()               GetTransparentColor()
                GFX_LineDraw()                          Line()
                GFX_LinePositionRelativeSet()           MoveRel()
                GFX_LinePositionSet()                   MoveTo()
                GFX_LinePositionXGet()                  \--
                GFX_LinePositionYGet()                  \--
                GFX_LineToDraw()                        LineTo()
                GFX_LineToRelativeDraw()                LineRel()
                GFX_LineStyleSet()                      SetLineThickness(),
                                                            SetLineType()
                GFX_LineStyleGet()                      \--
                GFX_FillStyleSet()                      \--
                GFX_FillStyleGet()                      \--
                GFX_GradientColorSet()                  \--
                GFX_GradientStartColorGet()             \--
                GFX_GradientEndColorGet()               \--
                GFX_AlphaBlendingValueSet()             SetAlpha()
                GFX_AlphaBlendingValueGet()             GetAlpha()
                GFX_CircleDraw()                        Circle()
                GFX_RectangleDraw()                     Rectangle()
                GFX_RectangleRoundDraw()                Bevel()
                GFX_PolygonDraw()                       DrawPoly()
                GFX_CircleFillDraw()                    CircleFill()
                GFX_RectangleFillDraw()                 \--
                GFX_RectangleRoundFillDraw()            FillBevel()
                GFX_BarDraw()                           Bar()
                GFX_ImageDraw()                         PutImage()
                GFX_ImagePartialDraw()                  PutImagePartial()
                GFX_ImageWidthGet()                     GetImageWidth()
                GFX_ImageHeightGet()                    GetImageHeight()
                GFX_FontSet()                           SetFont()
                GFX_FontGet()                           \--
                GFX_TextCursorPositionSet()             MoveTo()
                GFX_TextCursorPositionXGet()            GetX()
                GFX_TextCursorPositionYGet()            GetY()
                GFX_FontAntiAliasSet()                  GFX_Font_SetAntiAliasType()
                GFX_FontAntiAliasGet()                  GFX_Font_GetAntiAliasType()
                deprecated                              SetFontOrientation()
                deprecated                              GetFontOrientation()
                GFX_TextCharDraw()                      OutChar()
                deprecated                              OutText()
                GFX_TextStringDraw()                    OutTextXY()
                GFX_TextStringBoxDraw()                 \--
                GFX_TextStringHeightGet()               GetTextHeight()
                GFX_TextStringWidthGet()                GetTextWidth()
                GFX_RGBConvert()                        RGBConvert()
                GFX_BackgroundSet()                     \--
                GFX_BackgroundTypeSet()                 \--
                GFX_BackgroundTypeGet()                 \--
                GFX_BackgroundColorGet()                \--
                GFX_BackgroundImageGet()                \--
                GFX_BackgroundImageLeftGet()            \--
                GFX_BackgroundImageTopGet()             \--
                GFX_ExternalResourceCallback()          ExternalMemoryCallback()
                
                GFX_DoubleBufferEnable()                SwitchOnDoubleBuffering()
                GFX_DoubleBufferDisable()               SwitchOffDoubleBuffering()
                GFX_DoubleBufferStatusGet()             \--
                GFX_DoubleBufferAreaGet()               \--
                GFX_DoubleBufferAreaMark()              InvalidateRectangle()
                GFX_DoubleBufferSyncAllStatusSet()      InvalidateAll()
                GFX_DoubleBufferSyncAllStatusGet()      \--
                GFX_DoubleBufferSyncAllStatusClear()    \--
                GFX_DoubleBufferSyncAreaCountSet()      \--
                GFX_DoubleBufferSyncAreaCountGet()      \--
                GFX_DoubleBufferSynchronizeRequest()    RequestDisplayUpdate()
                GFX_DoubleBufferSynchronizeStatusGet()  IsDisplayUpdatePending()
                GFX_DoubleBufferSynchronize()           UpdateDisplayNow()
                deprecated                              GetDrawBufferAddress()
                deprecated                              GetFrameBufferAddress()
                GFX_DrawBufferInitialize()              \--
                GFX_DrawBufferSet()                     \--
                GFX_DrawBufferGet()                     \--
                GFX_FrameBufferSet()                    \--
                GFX_FrameBufferGet()                    \--
                                                        
            </TABLE>         

            - <b><i> Object Layer:  </i></b>      
            <TABLE>
                <b>NEW API Name</b>                     <b>Old API Name</b>
                ############################            ###########################
                GFX_GOL_ObjectStateSet()                SetState()
                GFX_GOL_ObjectStateGet()                GetState()
                GFX_GOL_ObjectStateClear()              ClearState()
                GFX_GOL_ObjectAdd()                     GOLAddObject()
                GFX_GOL_ObjectFind()                    GOLFindObject()
                GFX_GOL_ObjectIDGet()                   GetObjID()
                GFX_GOL_ObjectTypeGet()                 GetObjType()
                GFX_GOL_ObjectNextGet()                 GetObjNext()
                GFX_GOL_ObjectListNew()                 GOLNewList()
                GFX_GOL_ObjectListSet()                 GOLSetList()
                GFX_GOL_ObjectListGet()                 GOLGetList()
                GFX_GOL_ObjectListFree()                GOLFree()
                GFX_GOL_ObjectDelete()                  GOLDeleteObject()
                GFX_GOL_ObjectByIDDelete()              GOLDeleteObjectByID()
                GFX_GOL_ObjectFocusSet()                GOLSetFocus()
                GFX_GOL_ObjectFocusGet()                GOLGetFocus()
                GFX_GOL_ObjectFocusNextGet()            GOLGetFocusNext()
                GFX_GOL_ObjectFocusPrevGet()            GOLGetFocusPrev()
                GFX_GOL_ObjectCanBeFocused()            GOLCanBeFocused()

                GFX_GOL_ObjectListDraw()                GOLDraw()
                GFX_GOL_ObjectListHide()                \--
                GFX_GOL_ObjectDrawEnable()              GOLRedraw()
                GFX_GOL_ObjectRectangleRedraw()         GOLRedrawRec()
                GFX_GOL_ObjectIsRedrawSet()             IsObjUpdated()
                GFX_GOL_ObjectDrawDisable()             GOLDrawComplete()

                deprecated                              GOLCreateScheme()
                GFX_GOL_ObjectStyleSchemeSet()          GOLSetScheme()
                GFX_GOL_ObjectStyleSchemeGet()          GOLGetScheme()
                deprecated                              GOLGetSchemeDefault()
                
                GFX_GOL_DrawCallbackSet()               \--
                GFX_GOL_MessageCallbackSet()            \--
                deprecated                              GOLDrawCallback()   
                deprecated                              GOLMessageCallback()   
                
                GFX_GOL_ButtonCreate()                  BtnCreate()
                GFX_GOL_ButtonDraw()                    BtnDraw()
                GFX_GOL_ButtonPressStateImageSet()      \--
                GFX_GOL_ButtonPressStateImageGet()      \--
                GFX_GOL_ButtonReleaseStateImageSet()    \--
                GFX_GOL_ButtonReleaseStateImageGet()    \--
                GFX_GOL_ButtonTextSet()                 BtnSetText()
                GFX_GOL_ButtonTextGet()                 BtnGetText()
                GFX_GOL_ButtonTextAlignmentSet()        \--
                GFX_GOL_ButtonTextAlignmentGet()        \--
                GFX_GOL_ButtonActionSet()               BtnMsgDefault()
                GFX_GOL_ButtonActionGet()               BtnTranslateMsg()

                GFX_GOL_CheckBoxCreate()                CbCreate()
                GFX_GOL_CheckBoxDraw()                  CbDraw()
                GFX_GOL_CheckBoxTextSet()               CbSetText()
                GFX_GOL_CheckBoxTextGet()               CbGetText()
                GFX_GOL_CheckBoxTextAlignmentSet()      \--
                GFX_GOL_CheckBoxTextAlignmentGet()      \--
                GFX_GOL_CheckBoxActionSet()             CbMsgDefault()
                GFX_GOL_CheckBoxActionGet()             CbTranslateMsg()

                GFX_GOL_DigitalMeterCreate()            DmCreate()
                GFX_GOL_DigitalMeterDraw()              DmDraw()
                GFX_GOL_DigitalMeterTextAlignmentSet()  \--
                GFX_GOL_DigitalMeterTextAlignmentGet()  \--
                GFX_GOL_DigitalMeterValueSet()          DmSetValue()
                GFX_GOL_DigitalMeterValueGet()          DmGetValue()
                GFX_GOL_DigitalMeterIncrement()         DmIncValue()
                GFX_GOL_DigitalMeterDecrement()         DmDecValue()
                GFX_GOL_DigitalMeterActionGet()         DmTranslateMsg()

                GFX_GOL_EditBoxCreate()                 EbCreate()
                GFX_GOL_EditBoxDraw()                   EbDraw()
                GFX_GOL_EditBoxTextSet()                EbSetText()
                GFX_GOL_EditBoxTextGet()                EbGetText()
                GFX_GOL_EditBoxTextAlignmentSet()       \--
                GFX_GOL_EditBoxTextAlignmentGet()       \--
                GFX_GOL_EditBoxActionSet()              EbMsgDefault()
                GFX_GOL_EditBoxActionGet()              EbTranslateMsg()
                GFX_GOL_EditBoxCharAdd()                EbAddChar()
                GFX_GOL_EditBoxCharRemove()             EbDeleteChar()

                GFX_GOL_GroupboxCreate()                GbCreate()
                GFX_GOL_GroupboxDraw()                  GbDraw()
                GFX_GOL_GroupboxTextSet()               GbSetText()
                GFX_GOL_GroupboxTextGet()               GbGetText()
                GFX_GOL_GroupboxTextAlignmentSet()      \--
                GFX_GOL_GroupboxTextAlignmentGet()      \--
                GFX_GOL_GroupboxActionGet()             GbTranslateMsg()

                GFX_GOL_ListBoxCreate()                 LbCreate()
                GFX_GOL_ListBoxDraw()                   LbDraw()
                GFX_GOL_ListBoxItemAdd()                LbAddItem()
                GFX_GOL_ListBoxTextAlignmentSet()       \--
                GFX_GOL_ListBoxTextAlignmentGet()       \--
                GFX_GOL_ListBoxActionSet()              LbMsgDefault()
                GFX_GOL_ListBoxActionGet()              LbTranslateMsg()
                GFX_GOL_ListBoxItemListGet()            LbGetItemList()
                GFX_GOL_ListBoxItemListRemove()         LbDelItemsList()
                GFX_GOL_ListBoxItemRemove()             LbDelItem()
                GFX_GOL_ListBoxItemFocusSet()           LbSetFocusedItem()
                GFX_GOL_ListBoxItemFocusGet()           LbGetFocusedItem()
                GFX_GOL_ListBoxItemCountGet()           LbGetCount()
                GFX_GOL_ListBoxItemImageSet()           LbSetBitmap()
                GFX_GOL_ListBoxItemImageGet()           LbGetBitmap()
                GFX_GOL_ListBoxItemSelectStatusSet()    LbSetSel()
                GFX_GOL_ListBoxItemSelectStatusClear()  LbClrSel()
                GFX_GOL_ListBoxSelectionChange()        LbChangeSel()
                GFX_GOL_ListBoxSelectionGet()           LbGetSel()
                GFX_GOL_ListBoxVisibleItemCountGet()    LbGetVisibleCount()

                GFX_GOL_MeterCreate()                   MtrCreate()
                GFX_GOL_MeterDraw()                     MtrDraw()
                GFX_GOL_MeterMaximumValueGet()          \--
                GFX_GOL_MeterMinimumValueGet()          \--
                GFX_GOL_MeterTitleFontSet()             MtrSetTitleFont()
                GFX_GOL_MeterValueFontSet()             MtrSetValueFont()
                GFX_GOL_MeterTypeSet()                  \--
                GFX_GOL_MeterActionSet()                MtrMsgDefault()
                GFX_GOL_MeterActionGet()                MtrTranslateMsg()
                GFX_GOL_MeterIncrement()                MtrIncVal()
                GFX_GOL_MeterDecrement()                MtrDecVal()
                GFX_GOL_MeterRangeSet()                 \--
                GFX_GOL_MeterScaleColorsSet()           MtrSetScaleColors()
                GFX_GOL_MeterValueSet()                 MtrSetVal()
                GFX_GOL_MeterValueGet()                 MtrGetVal()
                
                GFX_GOL_PictureControlCreate()          PictCreate()
                GFX_GOL_PictureControlDraw()            PictDraw()
                GFX_GOL_PictureControlImageSet()        PictSetBitmap()
                GFX_GOL_PictureControlImageGet()        PictGetBitmap()
                GFX_GOL_PictureControlActionGet()       PictTranslateMsg()
                GFX_GOL_PictureControlPartialSet()      \--
                GFX_GOL_PictureControlScaleSet()        PictSetScale()
             
                GFX_GOL_ProgressBarCreate()             PbCreate()
                GFX_GOL_ProgressBarDraw()               PbDraw()
                GFX_GOL_ProgressBarPositionSet()        PbSetPos()
                GFX_GOL_ProgressBarPositionGet()        PbGetPos()
                GFX_GOL_ProgressBarRangeSet()           PbSetRange()
                GFX_GOL_ProgressBarRangeGet()           PbGetRange()
                GFX_GOL_ProgressBarActionGet()          PbTranslateMsg()
             
                GFX_GOL_RadioButtonCreate()             RbCreate()
                GFX_GOL_RadioButtonDraw()               RbDraw()
                GFX_GOL_RadioButtonTextSet()            RbSetText()
                GFX_GOL_RadioButtonTextGet()            RbGetText()
                GFX_GOL_RadioButtonCheckSet()           RbSetCheck()
                GFX_GOL_RadioButtonCheckGet()           RbGetCheck()
                GFX_GOL_RadioButtonActionSet()          RbMsgDefault()
                GFX_GOL_RadioButtonActionGet()          RbTranslateMsg()
                GFX_GOL_RadioButtonTextAlignmentSet()   \--
                GFX_GOL_RadioButtonTextAlignmentGet()   \--

                GFX_GOL_ScrollBarCreate()               SldCreate()
                GFX_GOL_ScrollBarDraw()                 SldDraw()
                GFX_GOL_ScrollBarPageSet()              SldSetPage()
                GFX_GOL_ScrollBarPageGet()              SldGetPage()
                GFX_GOL_ScrollBarRangeSet()             SldSetRange()
                GFX_GOL_ScrollBarRangeGet()             SldGetRange()
                GFX_GOL_ScrollBarPositionSet()          SldSetPos()
                GFX_GOL_ScrollBarPositionGet()          SldGetPos()
                GFX_GOL_ScrollBarPositionIncrement()    SldIncPos()
                GFX_GOL_ScrollBarPositionDecrement()    SldDecPos()
                GFX_GOL_ScrollBarActionSet()            SldMsgDefault()
                GFX_GOL_ScrollBarActionGet()            SldTranslateMsg()

                GFX_GOL_StaticTextCreate()              StCreate()
                GFX_GOL_StaticTextDraw()                StDraw()
                GFX_GOL_StaticTextAlignmentSet()        \--
                GFX_GOL_StaticTextAlignmentGet()        \--
                GFX_GOL_StaticTextSet()                 StSetText()
                GFX_GOL_StaticTextGet()                 StGetText()
                GFX_GOL_StaticTextActionGet()           StTranslateMsg()

                GFX_GOL_TextEntryCreate()               TeCreate()
                GFX_GOL_TextEntryKeyListCreate()        TeCreateKeyMembers()
                GFX_GOL_TextEntryDraw()                 TeDraw()
                GFX_GOL_TextEntryBufferClear()          TeClearBuffer()
                GFX_GOL_TextEntryBufferSet()            TeSetBuffer()
                GFX_GOL_TextEntryBufferGet()            TeGetBuffer()
                GFX_GOL_TextEntryCharAdd()              TeAddChar()
                GFX_GOL_TextEntryKeyCommandSet()        TeSetKeyCommand()
                GFX_GOL_TextEntryKeyCommandGet()        TeGetKeyCommand()
                GFX_GOL_TextEntryKeyIsPressed()         TeIsKeyPressed()
                GFX_GOL_TextEntryKeyTextSet()           TeSetKeyText()
                GFX_GOL_TextEntryKeyMemberListDelete()  TeDelKeyMembers()
                GFX_GOL_TextEntryLastCharDelete()       \--
                GFX_GOL_TextEntrySpaceCharAdd()         TeSpaceChar()
                GFX_GOL_TextEntryActionSet()            TeMsgDefault()
                GFX_GOL_TextEntryActionGet()            TeTranslateMsg()

                GFX_GOL_WindowCreate()                  WndCreate()
                GFX_GOL_WindowDraw()                    WndDraw()
                GFX_GOL_WindowImageSet()                \--
                GFX_GOL_WindowImageGet()                \--
                GFX_GOL_WindowTextSet()                 WndSetText()
                GFX_GOL_WindowTextGet()                 WndGetText()
                GFX_GOL_WindowActionGet()               WndTranslateMsg()
                GFX_GOL_WindowTextAlignmentSet()        \--
                GFX_GOL_WindowTextAlignmentGet()        \--
                
            </TABLE>         

            - <b><i> Driver Layer:  </i></b>      
            <TABLE>
                <b>NEW API Name</b>             <b>Old API Name</b>
                ############################    ###########################
                GFX_PixelPut()                  PutPixel()
                GFX_PixelGet()                  GetPixel()
                GFX_MaxXGet()                   GetMaxX()
                GFX_MaxYGet()                   GetMaxY()
                GFX_DisplayBrightnessSet()      DisplayBrightness()
                deprecated                      SetClip()
                deprecated                      SetClipRgn()
                deprecated                      GetClipLeft()
                deprecated                      GetClipTop()
                deprecated                      GetClipRight()
                deprecated                      GetClipBottom()
            </TABLE>         
        
       
        Deprecated Items: 
        - Round Dial object is discontinued.
        - Transitions routines are discontinued.
        - See API Changes Table summary for other deprecated functions.

        <b> v3.06 (2012-08-22) </b>
        
        New:
        - Partial rendering of images now supported (see PutImagePartial()).
        - Double Buffering is now supported in Microchip Low-Cost Controllerless
          (LCC) Graphics Display Driver.
        - Added dsPIC33EPXXX device family support.
        - Added S1D13522 EPD Controller Driver.
        - Added E-Paper Epson Demo.
        - Added Alpha-Blend support for Bar() function.
        - Graphics Resource Converter (GRC) now allows for padding and
          non-padding bitmap images. Bitmap images are padded which means that 
          each horizontal line will start on a byte boundary. The option has 
          been added to allow for conversion of bitmap resources to be     
          non-padded which allows the least resource space and controllers with
          windowing that auto increments to use them.
        - For XC16 or C30 builds, internal fonts can now be placed program 
          memory. If the font data or a combination of font data resources 
          exceed the 32 Kbyte limit of the data memory space, a define, 
          USE_GFX_FONT_IN_PROGRAM_SECTION should be defined in graphics
          configuration header. This will place the font resource data in 
          program memory space.
        Changes:
        - Graphics Object Demo now uses images that are RLE compressed.
        - Address range check for GFX_EPMP_CS1_MEMORY_SIZE and GFX_EPMP_CS2_MEMORY_SIZE 
          in Graphics Module in PIC24FJ256DA210 Display Driver file (mchpGfxDrv.c) is 
          modified for strict checks of allocated address pins for the EPMP. See 
          "Migration Changes" below for the address lines needed to be allocated. 
        - Swapped the bit orientation for the 1 BPP bitmap images. The previous
          versions of the library expects the left most pixel at the MSBit. This 
          has been changed so the the left most pixel is located at the LSBit.
          The change was made to accommodate controllers that have windowing.
          This also makes the pixel orientation consistent with the pixel
          orientation of 4bpp images.
        Fixes:
        - Fix FillBevel() & FillCircle() to avoid rendering lines more than once.
        Deprecated Items:
          The following Resistive Touch Screen macro names are replaced for 
          readability and flexibility if use: 
        - TRIS_XPOS - replaced by ResistiveTouchScreen_XPlus_Config_As_Input()
        - TRIS_YPOS - replaced by ResistiveTouchScreen_YPlus_Config_As_Input()
        - TRIS_XNEG - replaced by ResistiveTouchScreen_XMinus_Config_As_Input()
        - TRIS_YNEG - replaced by ResistiveTouchScreen_YMinus_Config_As_Output()
        - LAT_XPOS - replaced by ResistiveTouchScreen_XPlus_Drive_High()
        - LATS_YPOS - replaced by ResistiveTouchScreen_YPlus_Drive_High()
        - LAT_XNEG - replaced by ResistiveTouchScreen_XMinus_Drive_Low()
        - LAT_YNEG - replaced by ResistiveTouchScreen_YMinus_Drive_Low()
        Migration Changes:
        - To use the new Resistive Touchscreen macros, replace the TouchScreenResistive.c 
          file with the version in this release. Then replace the hardware profile macros
          to use the new macro names. Existing hardware profile can still be used but 
          build warnings will appear.
        - If custom display driver is used and the PutImage() functions are implemented 
          for faster rendering, the new partial image rendering feature requires these 
          PutImage() functions to be modified. See PutImagePartial() API description 
          and implementation in Primitive.c for details.
        - Address range check for GFX_EPMP_CS1_MEMORY_SIZE and GFX_EPMP_CS2_MEMORY_SIZE
          that are defined in hardware profile are modified when using the Graphics
          Module in PIC24FJ256DA210 Device and using external memory for display buffer, 
          the driver file (mchpGfxDrv.c). This check allocates address pins for the 
          EPMP. Modify the GFX_EPMP_CS1_MEMORY_SIZE and GFX_EPMP_CS2_MEMORY_SIZE values 
          set in the hardware profile to match the table shown below:
        
        <TABLE>
    	    EPMP Memory Size                                                    EPMP Address Lines
         	###############################################################     ###################
            GFX_EPMP_CSx_MEMORY_SIZE <= 0x20000 (bytes)                         Use PMA[15:0]
            0x20000 (bytes) < GFX_EPMP_CSx_MEMORY_SIZE <= 0x40000(bytes)        Use PMA[16:0]
            0x40000 (bytes) < GFX_EPMP_CSx_MEMORY_SIZE <= 0x80000(bytes)        Use PMA[17:0]
            0x80000 (bytes) < GFX_EPMP_CSx_MEMORY_SIZE <= 0x100000(bytes)       Use PMA[18:0]
            0x100000 (bytes) < GFX_EPMP_CSx_MEMORY_SIZE <= 0x200000(bytes)      Use PMA[19:0]
            0x200000 (bytes) < GFX_EPMP_CSx_MEMORY_SIZE <= 0x400000(bytes)      Use PMA[20:0]
            0x400000 (bytes) < GFX_EPMP_CSx_MEMORY_SIZE <= 0x800000(bytes)      Use PMA[21:0]
            0x800000 (bytes) < GFX_EPMP_CSx_MEMORY_SIZE <= 0x1000000(bytes)     Use PMA[22:0]         	
    	</TABLE>         
        
        - 1BPP images needs to be regenerated using the "Graphics Resource Converter" 
          since the bit orientation is swapped. When rendering a 1 BPP image, the 
          PutImage() function will expect the left most pixel to be located in the LSBit of 
          each word.
        - To utilize the new feature where the fonts can be placed in the program memory, 
          to remove the 32 KByte limit for data space in XC16 or C30 builds, fonts must 
          be regenerated using the "Graphics Resource Converter" and then add #define 
          USE_GFX_FONT_IN_PROGRAM_SECTION in GraphicsConfig.h. 
        Known Issues:
        - Extended glyph for certain font (such as Thai) when used with Static text 
          widget is clipped. Future version will add additional vertical text alignment 
          to the static text widget
        - Anti-aliasing and extended glyph features are not supported when using 
          PIC24FJ256DA210 CHRGPU.
        - SPI flash programming on the Epson S1D13517 PICtail board Rev 1.1 is not 
          always reliable, the S1D13517 demo no longer uses external memory flash 
          in the example.
        - When using PIC24FJ256GB210 PIM with Explorer 16 board that has a 5v Lumex 
          LCD display, the S1D13517 demo does not run correctly.
        - When using XC16 Compiler V1.00, add "-fno-ivopts" compile option. This is 
          a known issue V1.00 of XC16.
        - When using ListBox Widget, the widget height should be greater than the 
          height of the font used for the widget.

        <b> v3.04.01 (2012-04-03) </b>
        
        New:
        - No new items on this release.
        Changes:
        - Structure of this help file is modified. Section names that list APIs now 
          has the API in the name.
        Fixes:
        - Fix BarGradient() and BevelGradient() when USE_NONBLOCKING_CONFIG
          config is enabled.
        - Added missing GbSetText() function in GroupBox widget.
        - Fix SetPalette() to work with palette stored in external memory.
        Deprecated Items:
        - TYPE_MEMORY - replaced by GFX_RESOURCE
        - EXTDATA - replaced by GFX_EXTDATA
        - BITMAP_FLASH - replaced by IMAGE_FLASH
        - BITMAP_RAM - replaced by IMAGE_RAM
        - BITMAP_EXTERNAL - replaced by GFX_EXTDATA
        - EEPROM.h and EEPROM.c are replaced by the following files:
            - MCHP25LC256.c - source code
            - MCHP25LC256.h - header file
        - In the HardwareProfile.h add #define USE_MCHP25LC256 to use the new driver.
        Migration Changes:
        - none
        Known Issues:
        - Extended glyph for certain font (such as Thai) when used with Static text 
          widget is clipped. Future version will add additional vertical text alignment 
          to the static text widget.
        - Anti-aliasing and extended glyph features are not supported when using 
          PIC24FJ256DA210 CHRGPU.
        - SPI flash programming on the Epson S1D13517 PICtail board Rev 1.1 is not 
          always reliable, the S1D13517 demo no longer uses external memory flash in 
          the example.
        - When using PIC24FJ256GB210 PIM with Explorer 15 board with a 5v Lumex LCD 
          display, the S1D13517 demo does not run correctly.

        <b> v3.04 (2012-02-15) </b>
        
        New:
        - Font Table is updated to version 2 to accommodate anti-alias font, and 
          extended glyph.
        - Added anti-aliasing support for fonts (2bpp). See Primitive demo for
          an example.
        - Added extended-glyph support for fonts. See demo in the AppNote demo - AN1182, 
          This demo now includes Hindi and Thai font. 
        - Added 32-bit CRC code for resources to be placed in external memory. This CRC
          value can be used to verify if the data in external memory is valid or not. 
          Refer to Graphics Resource Converter release notes for details. Demos 
          that uses external memory as a resource are now checking the CRC value, 
          and if invalid, automatically requests for external memory resource programming.
        - Added new demos specific to PIC24FJ256DA210:
            - Elevator Demo - This demo shows an elevator status monitor that indicates 
              the current location of the elevator car.
            - RCCGPU-IPU Demo - formerly PIC24F_DA Demo. This demo shows how RCCGPU 
              and IPU modules are used.
            - Color Depth Demo - This demos shows how 1bpp, 4bpp and 8 bpp color 
              depths applications are implemented.
            - Remote Control Demo - This demo shows how a universal remote control can 
              be implemented using RF4CE communication protocol. This demo also 
              integrates the MRF24J40MA (a certified 2.4 GHz IEEE 802.15.4 radio 
              transceiver module) for sending RF4CE messages to the paired device.
        - Added driver for Solomon Systech 132x64 OLED/PLED Display Controller SSD1305
        Changes:
        - Modified Resistive Touch Screen calibration. Now the calibration stores the 8 
          touch points to support large touch panels.
        - Modified 4-wire Resistive Touch Screen driver to support build time setting of 
          single samples and auto-sampling of resistive touch inputs.
        - Naming of internal and external resource files (files that defined fonts and 
          images) in most demos are now standardized to use the same naming convention.
        - Support for Graphics PICTail v2 (AC164127) is now discontinued.
        - Merged "JPEG" demo and "Image Decoders" demo to "Image Decoder" demo.
        - Graphics Resource Converter upgrade (Version 3.17.47) - refer to "Graphics 
          Resource Converter Help.pdf" located in 
          \<install directory\>/Microchip Solutions/Microchip/Graphics/bin/grc for details.
        - External Memory Programmer upgrade (Version 1.00.01) - refer to "External 
          Memory Programmer Help.pdf" located in 
          \<install directory\>/Microchip Solutions/Microchip/Graphics/bin/memory_programmer 
          for details.
        Fixes:
        - Fix PushRectangle() issue where one line of pixel is not being updated.
        - Fix TextEntry widget issue where the string is not displayed when the 
          allocated string buffer length is equal to the maximum string length set 
          in the widget.
        - Fonts maximum character height is now set to 2^16.
        Deprecated Items:
        - TYPE_MEMORY - replaced by GFX_RESOURCE
        - EXTDATA - replaced by GFX_EXTDATA
        - BITMAP_FLASH - replaced by IMAGE_FLASH
        - BITMAP_RAM - replaced by IMAGE_RAM
        - BITMAP_EXTERNAL - replaced by GFX_EXTDATA
        - EEPROM.h and EEPROM.c are replaced by the following files:
        - MCHP25LC256.c - source code
        - MCHP25LC256.h - header file
        - in the HardwareProfile.h add #define USE_MCHP25LC256 to use the new driver.
        Migration Changes:
        - For existing code that wants to use the new anti-aliased fonts or extended 
          glyph features: regenerate the font tables using the "Graphics Resource 
          Converter" with the check box for the required feature set to be enabled. 
          For anti-aliased fonts, add the macro #define USE_ANTIALIASED_FONTS in the 
          GraphicsConfig.h
        Known Issues:
        - Extended glyph for certain font (such as Thai) when used with Static text 
          widget is clipped. Future version will add additional vertical text alignment 
          to the static text widget
        - Anti-aliasing and extended glyph features are not supported when using 
          PIC24FJ256DA210 CHRGPU.        
        - SPI flash programming on the Epson S1D13517 PICtail board Rev 1.1 is not 
          always reliable, the S1D13517 demo no longer uses external memory flash in 
          the example.
        - When using PIC24FJ256GB210 PIM with Explorer 15 board with a 5v Lumex LCD 
          display, the S1D13517 demo does not run correctly.

        <b> v3.03 (v3.02) </b>
        
        New:
        - Added custom video playback from SD Card in SSD1926 Demo. Video frames are 
          formatted to RGB565 format.
        - Added support for 1bpp, 4bpp and 8 bpp color depth on Chart widget.
        - Added support for Display Boards from Semitron
        - Seiko 35QVW1T
        - Seiko 43WVW1T
        Changes:
        - Maximum font height is now 256 pixels.
        - Modified EditBox behavior
        - Caret is now by default enabled.
        - Caret can now be shown even if USE_FOCUS is disabled.
        - Applications can now respond to touchscreen event on EditBoxes when 
          USE_FOCUS is disabled.
        - Modified resistive touchscreen calibration sequence.
        - Graphics Resource Converter upgrade (Version 3.8.21) - refer to 
          "Graphics Resource Converter Help.pdf" located in 
          \<install directory\>/Microchip Solutions/Microchip/Graphics/bin/grc for details.
        - External Memory Programmer upgrade (Version 1.00.01) - refer to 
          "External Memory Programmer Help.pdf" located in
          \<install directory\>/Microchip Solutions/Microchip/Graphics/bin/memory_programmer 
          for details.
        Fixes:
        - Fix Low Cost Controller display driver issue when run with Resistive Touch 
          Screen driver that uses single samples.
        - Fix issue on PIC24FJ256DA210 display driver PutImage()'s issue when using 
          palette on 4 bpp and 1 bpp images.
        - Fix issue on PIC24FJ256DA210 display driver PutImage()'s missing lines when 
          the image last pixel row or column falls on the edge of the screen.
        - Fix Resistive Touch Screen driver issue on rotated screens.
        - Fix GetImageHeight() GetImageWidth() issues for images that are RLE compressed.
        - EPMP module is now disabled when memory range defined for display buffer is 
          located in internal memory.
        - Add default color definitions in gfxcolors.h for 1bpp, 4bpp 8bpp and 16 bpp. 
          Added back legacy colors.
        - Fix HX8347 driver WritePixel() macro when using 16bit PMP mode.
        - Fix PIC24FJ256DA210 display driver issue on source data (continuous and 
          discontinuous data) when doing block copies of memory using RCCGPU.
        Deprecated Items:
        - TYPE_MEMORY - replaced by GFX_RESOURCE
        - EXTDATA - replaced by GFX_EXTDATA
        - BITMAP_FLASH - replaced by IMAGE_FLASH
        - BITMAP_RAM - replaced by IMAGE_RAM 
        - BITMAP_EXTERNAL - replaced by GFX_EXTDATA
        - EEPROM.h and EEPROM.c are replaced by the following files:
        - MCHP25LC256.c - source code
        - MCHP25LC256.h - header file
        - in the HardwareProfile.h add #define USE_MCHP25LC256 to use the new driver.
        Migration Changes:
        - EditBox widget's caret behavior is now by default enabled when USE_FOCUS is 
          set. To disable, ignore all messages for the edit box by returning a zero 
          when in GOLMsgCallback().
        Known Issues:
        - PutImage() does not work when using PIC24FJ256DA210 and look up table is used
          on images located at EDS memory with color depth less than 8bpp.
        - External Memory Programmer utility does not work with Graphics 
          PICTail v2 (AC164127)
        - When using PIC24FJ256GB210 PIM with Explorer 15 board with a 5v Lumex 
          LCD display, the S1D13517 demo does not run correctly.
        - Font tables are limited to 256 pixel character height.
        
        <b> v3.02 </b>
        
        New:
        - Added custom video playback from SD Card in SSD1926 Demo. Video frames are 
          formatted to RGB565 format.
        - Added support for 1bpp, 4bpp and 8 bpp color depth on Chart widget.
        - Added support for Display Boards from Semitron
        - Seiko 35QVW1T
        - Seiko 43WVW1T
        Changes:
        - Maximum font height is now 256 pixels.
        - Modified EditBox behavior
        - Caret is now by default enabled.
        - Caret can now be shown even if USE_FOCUS is disabled.
        - Applications can now respond to touchscreen event on EditBoxes when USE_FOCUS
          is disabled.
        - Modified resistive touchscreen calibration sequence.
        - Graphics Resource Converter upgrade (Version 3.8.21) - refer to 
         "Graphics Resource Converter Help.pdf" located in 
         \<install directory\>/Microchip Solutions/Microchip/Graphics/bin/grc for details.
        - External Memory Programmer upgrade (Version 1.00.01) - refer to 
          "External Memory Programmer Help.pdf" located in 
          \<install directory\>/Microchip Solutions/Microchip/Graphics/bin/memory_programmer 
          for details.
        Fixes:
        - Fix issue on PIC24FJ256DA210 display driver PutImage()'s missing lines when 
          the image last pixel row or column falls on the edge of the screen.
        - Fix Resistive Touch Screen driver issue on rotated screens.
        - Fix GetImageHeight() GetImageWidth() issues for images that are 
          RLE compressed.
        - EPMP module is now disabled when memory range defined for display buffer is 
          located in internal memory.
        - Add default color definitions in gfxcolors.h for 1bpp, 4bpp 8bpp and 16 bpp. 
          Added back legacy colors.
        - Fix HX8347 driver WritePixel() macro when using 16bit PMP mode.
        - Fix PIC24FJ256DA210 display driver issue on source data (continuous and 
          discontinuous data) when doing block copies of memory using RCCGPU.
        Deprecated Items:
        - TYPE_MEMORY - replaced by GFX_RESOURCE
        - EXTDATA - replaced by GFX_EXTDATA
        - BITMAP_FLASH - replaced by IMAGE_FLASH
        - BITMAP_RAM - replaced by IMAGE_RAM
        - BITMAP_EXTERNAL - replaced by GFX_EXTDATA
        - EEPROM.h and EEPROM.c are replaced by the following files:
            - MCHP25LC256.c - source code
            - MCHP25LC256.h - header file
            - in the HardwareProfile.h add #define USE_MCHP25LC256 to use the new driver.
        Migration Changes:
        - EditBox widget's caret behavior is now by default enabled when USE_FOCUS
          is set. To disable, ignore all messages for the edit box by returning a zero 
          when in GOLMsgCallback().
        Known Issues:
        - PutImage() does not work when using PIC24FJ256DA210 and look up table is
          used on images located at EDS memory with color depth less than 8bpp.
        - External Memory Programmer utility does not work with 
          Graphics PICTail v2 (AC164127)
        - When using PIC24FJ256GB210 PIM with Explorer 15 board with a 5v Lumex 
          LCD display, the S1D13517 demo does not run correctly.
        - Font tables are limited to 256 pixel character height.

        <b> v3.01 (v3.00) </b>
        
        New:
        - Graphics External Memory Programmer ported to java version.
        - Two options to program boards:
        - UART option if the board supports UART interface
        - USB option if the board support USB device interface
        - when installing the USB drivers for the programmer utility Use the drivers 
          located in "\<install directory\>/Microchip/Utilities/USB Drivers/MPLABComm"
        - For detailed usage, please refer to the External Programmer help file
        - Added Analog Clock widget.
        - Added new driver Epson S1D13517 display driver with additional driver features:
            - Gradient
            - Alpha Blending
            - 16/24 bits per pixel (bpp)
        - Added new Graphics PICtail Plus Epson S1D13517 Board (AC164127-7)
        - Added new Graphics Display Truly 5.7" 640x480 Board (AC164127-8)
        - Added new Graphics Display Truly 7" 800x480 Board (AC164127-9)
        - Added 24bpp support.
        - Added a specific PIC24FJ256DA210 demo, PIC24F_DA
        - Graphics Resource Converter - refer to the Graphics Resource Converter help 
          file for release note information.
        - New PIC32MX Low-Cost Controllerless Graphics PICTail Board (AC164144)
        - Added Run Length Encoding (RLE) compression for bitmap images.
        - RLE4 - compression for 4-bit palette (16 color) images
        - RLE8 - compression for 8-bit palette (256 color) images
        - Added Transparency feature for PutImage() functions in Primitive Layer. 
          For Driver Layer, this feature is enabled in the following drivers:
            - mchpGfxDrv - Microchip Graphics Controller Driver
            - SSD1926 - Solomon Systech Display Controller Driver
        - Added new demo for Graphics PICtail Plus Epson S1D13517 Board (S1D13517 Demo)
        - Added AR1020 Resistive Touch Screen Controller beta support.
        - Added support for MikroElektronika "mikroMMB for PIC24" board.
        - Added DisplayBrightness() function for display drivers that have an option to 
          control the display back light.
        - MPLAB X demo project support (BETA)
        - Tested with MPLAB X Beta 6
        - Each demo project contains multiple configuration schemes
        - The graphics object layer uses a default scheme structure. If the 
          application wishes to use a different default scheme, the application 
          will need to define GFX_SCHEMEDEFAULT in GraphicsConfig header file.
        Changes:
        - Relocated all Graphics Demo projects under Graphics directory 
          (\<install directory\>/Graphics).
        - Works with Graphics Display Designer version 2.1
        - Works with Graphics Resource Converter version 3.3
        - Removed IPU decoding from the Primitive demo
        - Removed ImageFileConverter application from Image Decoder demo
        - Use Graphics Resource Converter to generate output for the demo.
        - Shorten file name by using abbreviated names
        - Refer to abbreviations.htm in the Microchip help directory for details
        - Change the "Alternative Configurations" directory in the demos to "Configs"
        - Changed the "Precompiled Demos" directory in the demos to 
          "Precompiled Hex"
        - Combined all application note demos (AN1136, AN1182, AN1227 and AN1246) 
          into one demo project (AppNotes).
        - Modified External Memory and JPEG demos to include USB device mode to 
          program external flash memory.
        - Moved the location of the COLOR_DEPTH setting from the HardwareProfile.h 
          to the GraphicsConfig.h
        - Removed USE_DRV_FUNCTIONNAME (example USE_DRV_LINE to implement the Line() 
          function in the driver) option to implement Primitive Layer functions in 
          the Driver Layer. The Primitive Layer functions are now modified to have weak
          attributes, so when the driver layer implements the same function, the one 
          in the driver will be used at build time.
        - Modified HardwareProfile.h for Graphics demo boards and development 
          Platforms.
        - When using Resistive Touch Screen: add macro USE_TOUCHSCREEN_RESISTIVE
        - When using AR1020 as the touch screen controller: add macro 
          USE_TOUCHSCREEN_AR1020
        - When using SPI Flash Memory (SST25VF016) in Graphics Development 
          Boards: add macro USE_SST25VF016
        - When using Parallel Flash Memory (SST39LF400) in PIC24FJ256DA210 
          Development Board: add macro USE_SST39LF400
        - When using Parallel Flash Memory (SST39LF400) in PIC24FJ256DA210 Development
          Board: add macro USE_SST39LF400
        - Added function pointers for Non-Volatile Memories when used in the Demos.
        - NVM_SECTORERASE_FUNC - function pointer to sector erase function.
        - NVM_WRITE_FUNC - function pointer to write function.
        - NVM_READ_FUNC - function pointer to read function.
        - Display Driver Layer architecture is changed. Refer to Adding New Device 
          Driver for new requirements.
        - Modified Resistive Touch Driver calibration
        - In the PIC24FJ256DA210 driver, CopyWindow() is modified to CopyBlock().
        Fixes:
        - Fixed issue on vertical Progress Bar rendering.
        - Updated demos Google map and JPEG to use the proper GFX_RESOURCE
          identifiers for JPEG resources
        - HX8347 driver now compiles and works with 'mikroMMB for PIC24'
        - Bug fixes in the digital meet and cross hair widgets
        - Removed references to the PIC24 configuration bit COE_OFF.
        - Fixed issue on PutImage() when using PIC24FJ256DA210 and look up table
          is used on images located at internal or external SPI flash with color 
          depth less than 8bpp.
        - Fixed issue on Line() in the SSD1926 and mchpGfxDrv driver files. The 
          stored coordinates after a successful rendering of a line will be at the 
          end point (x2,y2).
        Deprecated Items:
        - TYPE_MEMORY - replaced by GFX_RESOURCE
        - EXTDATA - replaced by GFX_EXTDATA
        - BITMAP_FLASH - replaced by IMAGE_FLASH
        - BITMAP_RAM - replaced by IMAGE_RAM
        - BITMAP_EXTERNAL - replaced by GFX_EXTDATA
        Migration Changes:
        - DisplayDriver.c is not used to select the display driver (the file is 
          not a part of the graphics library release package).
        - The application should include the driver(s) in the project. Multiple 
          driver files can be included in one project because the hardware profile 
          will define the driver and only that driver's source code will be used.
        - For example, a project may be designed to use the Microchip's PIC24FJ256DA210 
          graphics controller and the SSD1926 depending on the hardware profile used. 
          The project will include the following source files, SSD1926.c and
          mchpGfxDrv.c, among the graphics source files. The hardware profile will 
          contain the following macros, GFX_USE_DISPLAY_CONTROLLER_SSD1926 and 
          GFX_USE_DISPLAY_CONTROLLER_MCHP_DA210, to select the SSD1926 and 
          Microchip's PIC24FJ256DA210 graphics controller, respectively.
        - When using PIC24FJ256DA210, the driver files are now changed to:
            - mchpGfxDrv.c - source code
            - mchpGfxDrv.h - header file
            - mchpGfxDrvBuffer.c - source code that declares display buffer, work areas and 
              cache areas for IPU operations in EDS.
        - The touch screen drivers in the "Board Support Package" directory are renamed to:
            - TouchScreenResistive.c - internal resistive touch source code
            - TouchScreenResistive.h - internal resistive touch header file
            - TouchScreenAR1020.c - external AR1020 touch source code
            - TouchScreenAR1020.h - external AR1020 touch header file
        - The two original files (TouchScreen.c and TouchScreen.h) are still needed
          since they are defining common interfaces to resistive touch drivers.
        - When using the internal resistive touch module, the project will contain 
          TouchScreenResistive.c and TouchScreen.c. All modules that reference touch 
          screen APIs will include TouchScreen.h header file.
        - When using the external AR1020 touch module, the project will contain 
          TouchScreenAR1020.c and TouchScreen.c. All modules that reference touch 
          screen APIs will include TouchScreen.h header file.
        - The touch screen initialization routine API has changed. The new TouchInit 
          API has four parameters passed. Please refer to the API definition for 
          more details.
        - When using the Potentiometer on the graphics development boards, include 
          in your project the following files found in the "Board Support Package" 
          directory:
            - TouchScreenResistive.c - source code
            - TouchScreenResistive.h - header file
            - Potentiometer.h - contains the APIs for the A/D interface.
        - EEPROM.h and EEPROM.c are going to be deprecated. Use the two new files:
            - MCHP25LC256.c - source code
            - MCHP25LC256.h - header file
        - in the HardwareProfile.h add #define USE_MCHP25LC256 to use the new driver.
        - A SPI driver has been created to support projects with multiple devices 
          on one SPI channel.
        - Projects will need to include the source file drv_spi.c in projects that 
          use devices on a SPI channel.
        - SPI Flash initialization routines will need to pass a DRV_SPI_INIT_DATA 
          structure. This structure defines the SPI control and bit rate used by the 
          SPI Flash module.
        - The COLOR_DEPTH macro has been moved from the hardware profile header file
          to the GraphicsConfig.h header file.
        - For project migration please refer the graphics demos for examples.
        Known Issues:
        - PutImage() does not work when using PIC24FJ256DA210 and look up table is
          used on images located at EDS memory with color depth less than 8bpp.
        - PutImage() of when using PIC24FJ256DA210 for 8bpp images is missing 
          the last row and last column of the bitmap when the image is from external 
          memory, look up table is used and the screen is rotated 90 degrees.
        - When compiling the Analog Clock source code with C30 v3.24 the optimization 
          setting must be set to 0 (none).
        - External Memory Programmer utility does not work with Graphics PICTail 
          v2 (AC164127)
        - When using PIC24FJ256GB210 PIM with Explorer 15 board with a 5v Lumex 
          LCD display, the S1D13517 demo does not run correctly.
        - Font tables are limited to 256 pixel character height. For fonts generated 
          for external memory, the maximum height limitation is 128 pixels.
          
        <b> v2.11 </b>
        
        New:
        - Graphics Resource Converter (GRC) ported to java version.
        - Added support for Inflate Processing Unit (IPU) and Character Processing 
          Unit (CHRGPU)of the Microchip Graphics Module implemented in PIC24FJ256DA210.
        - Added new "Google Map Demo" for PIC32MX795F512L and PIC24FJ256DA210 device.
        - Added SST39LF400 Parallel Flash Memory driver in "Board Support Package". 
          This is the driver for the parallel flash on the "PIC24FJ256DA210 Development 
          Board".
        - Added demo support for PIC32 MultiMedia Expansion Board (DM320005).
        - Added GFX_IMAGE_HEADER structure. This structure defines how the image(s) 
          are accessed and processed by the Graphics Library.
        - Added a third option (#define XCHAR unsigned char) on the XCHAR usage. 
          This is provided as an option to use characters with IDs above 127 and 
          below 256. With this option, European fonts that uses characters with 
          character IDs above 127 and below 256 can now be generated and used in 
          the library.
        - Added a scheme to replace the default font GOLFontDefault in the library 
          with any user defined fonts. Refer to "Changing the default Font" for 
          details.
        Changes:
        - Added compile switches to all drivers in "Board Support Package" for options 
          to compile out when not used in specific projects.
        - Replaced TYPE_MEMORY with GFX_RESOURCE type enumeration and expanded the 
          enumeration for graphics resources (such as images and fonts). GFX_RESOURCE 
          type will determine the source and the data format of the resource 
          (compressed or uncompressed).
        - Changes on the macros used on the "Graphics JPEG Demo":
            <code> 
                // Valid values of the first field for JPEG_FLASH and JPEG_EXTERNAL structures
                #define FILE_JPEG_FLASH 2 // the JPEG file is located in internal flash
                #define FILE_JPEG_EXTERNAL 3 // the JPEG file is located in external memory
            </code>
            to
            <code> 
                // Valid values of the first field for JPEG_FLASH and JPEG_EXTERNAL structures
                #define FILE_JPEG_FLASH 0 // the JPEG file is located in internal flash
                #define FILE_JPEG_EXTERNAL 1 // the JPEG file is located in external memory
            </code>
        - Added function pointers to GOL structure OBJ_HEADER. These function 
          pointers makes it easier to add user created objects in the Graphics Library.
            - DRAW_FUNC - function pointer to object drawing function.
            - FREE_FUNC - function pointer to object free function. Only for objects 
              that needs free routines to effectively free up memory used by the 
              object create function.
            - MSG_FUNC - function pointer to object message function.
            - MSG_DEFAULT_FUNC - function pointer to object default message function.
        - Merged "Graphics External Memory Demo" and "Graphics External Memory 
          Programmer" into one demo "Graphics External Memory Programmer and Demo".
        Fixes:
        - TouchScreen driver now checks display orientation and adjusts the touch 
          to be aligned to the display orientation.
        - Fixed GOLFocusNext() issue on list that does not contain an object that can 
          be focused.
        - Removed redundant code in GOLRedrawRec().
        - Added an option in XCHAR to use unsigned char.
        Deprecated Items:
        - TYPE_MEMORY - replaced by GFX_RESOURCE
        - EXTDATA - replaced by GFX_EXTDATA
        - BITMAP_FLASH - replaced by IMAGE_FLASH
        - BITMAP_RAM - replaced by IMAGE_RAM
        - BITMAP_EXTERNAL - replaced by GFX_EXTDATA
        Migration Changes:
        - To use drivers located in "Board Support Package" directory, add the
          USE_DRIVERNAME macro in application code (in the demos these are added in
          the HardwareProfile.h) to include the drivers. Refer to the specific driver 
          code for the correct USE_DRIVERNAME macro name.
        - The new version of the Graphics Resource Converter generates graphics 
          application resources (fonts and images) using the new GFX_IMAGE_HEADER
          structure for images and new GFX_RESOURCE type defines to specify 
          location of the resources. Because of this, some structures are deprecated 
          and replaced with more appropriate structures. To remove the deprecation
          warnings, regenerate the fonts and images files using the new Graphics 
          Resource Converter.
        Known Issues:
        - Graphics SSD1926 JPEG and SD Card Demo does not support Graphics Display 
          Powertip 4.3" 480x272 Board (PH480272T_005_I11Q). As is, there's not enough 
          spare memory space to carry out the hardware JPEG decoding operation by the 
          SSD1926. A potential work around is to reduce the active display area size 
          to reserve more memory space for the JPEG decoding operation.
        - SSD1926 hardware acceleration for eclipse is disabled due to missing pixels 
          at Angle 0.
        - PIC32MX460 PIM (not Starter Kit) does not support 16-bit PMP mode with 
          Graphics PICtail  Plus Board Version 3 (SSD1926) Board. It only supports 
          8-bit PMP mode. This is due to pin mapping conflicts on the boards.
        - This version of Graphics Library is not compatible with Graphics 
          Display Designer v2.0.0.9c
        
        <b> v2.10 </b>
        
        New:
        - Added new demo "Graphics Object Layer Palette Demo" for PIC24FJ256DA210 
          device.
        - Added support for PIC32MX795F512L device.
        - Added documentation for the Grid object.
        - Added Vertical Mode to Progress Bar.
        - Added MicrochipGraphicsModule display driver.
        - Added "Board Support Package" directory. This contains common hardware 
          drivers for Microchip demo boards.
        - Added OBJ_MSG_PASSIVE as a translated message on the slider to detect a
          touch screen release message. This has no effect on the state of the slider. 
          Applications that does not qualify for touch press and touch move event 
          must now qualify the messages for the slider object to avoid processing 
          messages for touch release.
        Changes:
        - To improve speed modified gfxpmp.c and gfxepmp.c to be inline functions
          in gfxpmp.h and gfxepmp.h respectively.
        - Changed HX8347A.c to HX8347.c (both the D and A driver version is 
          implemented in the new file). To select set the DISPLAY_CONTROLLER 
          to be HX8347A or HX8347D in the hardware profile.
        - Modified malloc() and free() to be defined by macros in GraphicsConfig.h
          file. For applications using Operating System, the macros can be redefined 
          to match the OS malloc and free functions. The default settings are:
            - #define GFX_malloc(size) malloc(size)
            - #define GFX_free(ptr) free(ptr)
        - Merged GOL Demo English and Chinese demo into one demo.
        - Removed the macro "GRAPHICS_HARDWARE_PLATFORM". This is specific to 
          Microchip demo boards.
        - Abstracted the timer from the touch screen driver.
        - Moved the following hardware drivers to the "Board Support Package" directory
        - Touch screen driver: TouchScreen.c and TouchScreen.h files.
        - SPI Flash driver: SST25VF016.c and SST25VF016.h files.
        - Graphics PICtail Version 2 Parallel Flash driver: SST39VF040.c and 
          SST39VF040.h files.
        - Explorer 16 SPI EEPROM Flash driver: EEPROM.c and EEPROM.h files.
        - Graphics PICtail Version 2 Beeper driver: Beep.c and Beep.h files.
        - Revised the Seiko 3.5" 320x240 display panel schematic to revision B. 
          Corrected the pin numbering on the hirose connector. See "Schematic for 
          Graphics Display Seiko 3.5in 320x240 Board Rev B.pdf" file on the 
          /Microchip Solutions/Microchip/Graphics/Documents/Schematics directory.
        Fixes:
        - Fixed TextEntry object issue on output string maximum length.
        - Fixed Slider increment/decrement issue on Keyboard messages.
        - Fixed GOLGetFocusNext() bug when none of the objects in the list can
          be focused.
        Migration Changes:
        - pmp interface files are converted to header files and functions are now 
          inline functions to speed up pmp operations. Projects must be modified to:
            - include gfxpmp.h and gfxepmp.h source files in the project.
            - gfxepmp.c file is retained but will only contain the definition of
              the EPMP pmp_data.
        - Converted the macro: #define GRAPHICS_HARDWARE_PLATFORM HARDWARE_PLATFORM 
          where HARDWARE_PLATFORM is one of the supported hardware platforms defined 
          in the section Graphics Hardware Platform to just simply 
          #define HARDWARE_PLATFORM.
        - Since the timer module is abstracted from the touch screen driver in the 
          "Board Support Package", the timer or the module that calls for the sampling 
          of the touch screen must be implemented in the application code. Call the
          function TouchProcessTouch() to sample the touch screen driver if the user 
          has touched the touch screen or not. For example:
            <code>
                // to indicate the hardware platform used is the
                // Graphics PICtail  Plus Board Version 3
                #define GFX_PICTAIL_V3
            </code>
        - Projects which uses the following hardware drivers will need to use the 
          latest version of the drivers located in the "Board Support Package" 
          directory.
        - Touch screen driver: TouchScreen.c and TouchScreen.h files.
        - SPI Flash driver: SST25VF016.c and SST25VF016.h files.
        - Graphics PICtail Version 2 Parallel Flash driver: SST39VF040.c and 
          SST39VF040.h files.
        - Explorer 16 SPI EEPROM Flash driver: EEPROM.c and EEPROM.h files.
        - Graphics PICtail Version 2 Beeper driver: Beep.c and Beep.h files.
        - In the TouchScreen driver, the timer initialization and timer interrupt 
          sub-routine (ISR) are abstracted out of the driver. The initialization and 
          the ISR should be defined in the application code. the TouchProcessTouch() 
          function in the driver should be called in the ISR to process the touch.
        Known Issues:
        - Graphics SSD1926 JPEG and SD Card Demo does not support Graphics Display 
          Powertip 4.3" 480x272 Board (PH480272T_005_I11Q). As is, there's not enough 
          spare memory space to carry out the hardware JPEG decoding operation by 
          the SSD1926. A potential work around is to reduce the active display area 
          size to reserve more memory space for the JPEG decoding operation.
        - SSD1926 hardware acceleration for eclipse is disabled due to missing 
          pixels at Angle 0.
        - PIC32MX460 PIM (not Starter Kit) does not support 16-bit PMP mode with
          Graphics PICtail  Plus Board Version 3 (SSD1926) Board. It only supports 
          8-bit PMP mode. This is due to pin mapping conflicts on the boards.
        - This version of Graphics Library is not compatible with Graphics Display
          Designer v2.0.0.9c
          
        <b> v2.01 </b>
        
        Changes:
        - Modified drivers for abstraction of pmp and epmp interfaces. they have a 
          common header file DisplayDriverInterface.h.
        - DisplayDriverInterface.h is added to the Graphics.h file.
        - DelayMs() API is abstracted from the driver files. TimeDelay.c and 
          TimeDelay.h is added.
        Fixes:
        - Fixed background color bug in StaticText and Digital Meter object.
        - Fixed ListBox LbSetFocusedItem() bug on empty lists.
        - Graphics SSD1926 JPEG and SD Card Demo is fixed to support SD card of 
          size 2GB or bigger
        Migration Changes:
        - pmp interface is abstracted from the driver. Projects must be modified to:
            - include gfxpmp.c and gfxepmp.c source files in the project.
        - DelayMs() is abstracted from the drivers.
        - Add TimeDelay.c source file in the project.
        - Add TimeDelay.h header file in the project.
        
        <b> v2.00 </b>
        
        Changes:
        - "Graphics PICtail Board Memory Programmer" has been renamed to 
          "Graphics External Memory Programmer".
        - "Bitmap & Font Converter" utility has been renamed to "Graphics 
          Resource Converter".
        - Font format has changed. The bit order has been reversed. Necessary 
          for cross compatibility.
        - Added 2 new directories in each demo
        - Precompiled Demos - this directory contains all pre-compiled demos 
          for all hardware and PIC devices supported by the demo.
        - Alternative Configurations - this directory contains all the 
          Hardware Profiles for all hardware and PIC devices supported by the demo.
        - Moved all hardware and display parameters from GraphicsConfig.h to 
          HardwareProfile.h.HardwareProfile.h references a hardware profile file 
          in "Alternative Configurations" directory based on the PIC device selected.
        Fixes:
        - Fixed BtnSetText() bug when using Multi-Line Text in Buttons.
        - Fixed SDSectorWrite() function in SSD1926_SDCard.c in the "Graphics 
          SSD1926 JPEG and SD Card Demo".
        Migration Changes:
        - Move all hardware and display parameters from GraphicsConfig.h to 
          HardwareProfile.h
        - panel type, display controller, vertical and horizontal resolution, 
          front and back porches, synchronization signal timing and polarity 
          settings etc.
        - GRAPHICS_PICTAIL_VERSION,1,2,250,3 options are now deprecated, new usages are:
            - #define GRAPHICS_HARDWARE_PLATFORM GFX_PICTAIL_V1
            - #define GRAPHICS_HARDWARE_PLATFORM GFX_PICTAIL_V2
            - #define GRAPHICS_HARDWARE_PLATFORM GFX_PICTAIL_V3
        - The font format has changed, run Graphics Resource Converter to regenerate 
          font files, the bit order is reversed. No legacy support is provided. 
          Primitive/Driver layers now expects the new format.
        - The initialization sequence of GOLInit() relative to the flash memory 
          initialization is sensitive due to the sharing of hardware resources, 
          i.e. SPI or PMP. Care should be taken to make sure the peripheral and 
          I/O port settings are correct when accessing different devices.
        - A number of configuration options have been moved from GraphicsConfig.h 
          to HardwareProfile.h, this is required to maintain a more logical flow.
        - HardwareProfile.h now points to one of many Alternative Configuration 
          files, each one specific to a certain hardware board setup.
        - DelayMs routine in SH1101A-SSD1303.c/h is now a private function, no 
          public API is exposed. In future releases, DelayMS will be removed from 
          all drivers and be replaced by an independent module.
        - GenericTypeDefs.h has been updated with new definitions, this should 
          not impact any legacy codes.
        
        <b> v1.75b </b>
        
        Changes:
        - None.
        Fixes:
        - Fixed Line2D() bug in SSD1926.c.
        - Fixed remainder error in JPEG decoding in JpegDecoder.c.
        - Fixed pinout labels for reference design schematic "Schematic for Graphics 
          Display Powertip 4.3in 480x272 Board Rev 2.pdf".
        Migration Changes:
        - None.
        
        <b> v1.75 Release (July 10, 2009) </b>
        
        Changes:
        - Added 2D acceleration support for controllers with accelerated primitive 
          drawing functions.
        - Added Digital Meter Widget for fast display refresh using fixed width fonts.
        - Added support for selected PIC24H Family of devices.
        - Added support for selected dsPIC33 Family of devices.
        - Updated all primitive functions to return success or fail status when 
          executed. This is used to evaluate if the primitive function has finished 
          rendering.
        - Updated Solomon Systech SSD1926 driver to use 2D accelerated functions.
        - New Display Controller Driver supported:
            - Ilitek ILI9320
            - Solomon Systech SSD1289
            - Himax HX8347
            - Renesas R61580
        - New demos are added:
            - PIC24F Starter Kit Demo
            - PIC24H Starter Kit Demo 1
            - Graphics JPEG Demo using internal and external flash memory for image storage
            - Graphics SSD1926 JPEG and SD Card Demo using SD Card for image storage
        - Added JPEG support to "Font and Bitmap Converter Utility".
        - Modified Button Widget for new options:
        - Use multi-line text (set USE_BUTTON_MULTI_LINE)
        - Detect continuous touch screen press event using messaging
        - Added Touch Screen event EVENT_STILLPRESS to support continuous 
          press detection.
        - New reference design schematics added:
            - Schematic for Graphics Display DisplayTech 3.2in 240x320 Board.pdf
            - Schematic for Graphics Display Newhaven 2.2in 240x320 with HX8347.pdf
            - Schematic for Graphics Display Seiko 3.5in 320x240 Board.pdf
            - Schematic for Graphics Displays DisplayTech and Truly 3.2in 240x320 with SSD1289.pdf
            - Schematic for ILI9320.pdf
        Fixes:
        - Fixed dimension calculation for quarter sized meter.
        Migration Changes:
        - When using accelerated primitive functions while USE_NONBLOCKING_CONFIG is 
          enabled, the accelerated primitive must be checked if it succesfully 
          rendered. Refer to the coding example for details.
        - Replaced LGDP4531_R61505_S6D0129_S6D0139_SPFD5408.c and
          LGDP4531_R61505_S6D0129_S6D0139_SPFD5408.h files with 
          drvTFT001.c and drvTFT001.h respectively.

        <b> v1.65 Release (March 13, 2009) </b>
        
        Changes:
        - Added support for the new Graphics PICtail Plus Daughter Board 
          (AC164127-3). This new board comes in two components: 
          the controller board and the display board. The display board uses 
          RGB type displays driven by the controller board. This configuration
          allows easy replacement of the display glass.
        - Added application note AN1246 "How to Create Widgets".
        - New Display Controller Driver supported
        - UltraChip UC1610
        - New demos are added
        - Graphics AN1246 Demo showing the TextEntry Widget.
        - Graphics Multi-App Demo showing USB HID, USB MSD and SD MSD demos
          using the Microchip Graphics Library.
        - Modified Meter Widget for new options:
        - Set Title and Value Font at creation time
        - Added GOLGetFocusPrev() for focus control on GOL Objects.
        - Added work spaces to demo releases.
        - Graphics PICtail Plus Board 1 is obsolete. All references to 
          this board is removed from documentation.
        - New reference design schematics added:
            - Schematic for Graphics Display Ampire 5.7in 320x240 Board Rev A.pdf
            - Schematic for Graphics Display Powertip 3.5in 320x240 Board Rev B.pdf
            - Schematic for Graphics Display Powertip 4.3in 480x272 Board Rev B.pdf
            - Schematic for Graphics Display Truly 3.5in 320x240 Board Rev A.pdf
            - Schematic for Truly TOD9M0043.pdf
        Fixes:
        - Fixed drawing bug on TextEntry Widget
        - Added missing documentation on Chart and Text Entry Widgets
        Migration Changes:
        - none

        <b> v1.60 Release (December 3, 2008) </b>
        
        Changes:
        Graphics Library Help Release Notes
        Graphics Library Version 3.06.04 19
        - Added TextEntry Widget.
        - Modified Meter Widget for new options:
        - Define different fonts for value and title displayed.
        - Add option for resolution of one decimal point when displaying values.
        - Add color options to all six arcs of the Meter.
        - Meter range is not defined by a minimum value and a maximum value.
        - Added feature to a the Button Widget to allow cancelling of press by 
          moving away the touch and releasing from the Button's face.
        - Added font sizes options of 3,4,5,6 & 7 in Font & Bitmap Converter 
          Utility when converting fonts from TTF files.
        - Enhanced the architecture of the Display Device Driver Layer.
        Fixes:
        - Fixed Font & Bitmap Converter Utility generation of reference strings 
          to be set to const section.
        - Fixed panel rendering to always draw the panel face color even if bitmaps 
          are present.
        Migration Changes:
        - Added the following files in the Display Device Driver Layer to easily 
          switch from one display driver to another.
            - DisplayDriver.h
            - DisplayDriver.c
        - Modified implementation of GraphicsConfig.h file to support new Display 
          Device Driver Layer architecture.
        - Moved the definitions of pins used in device drivers implemented in all
          the demos to HardwareProfile.h file.
            - EEPROM Driver
            - Touch Screen Driver
            - Beeper Driver
            - Flash Memory Driver
            - Display Drivers
        - Modified GOL.c and GOL.h to include processing of TextEntry object when 
          enabled by application.

        <b> v1.52 Release (August 29, 2008) </b>
        
        Changes:
        - Added Chart Widget.
        - Added Property State for Window Widget. Text in Title Area can now 
          be centered.
        - Added Supplementary Library for Image Decoders.
        - Added documentation of Default Actions on widgets.
        - Replaced USE_MONOCHROME compile switch with COLOR_DEPTH to define 
          color depth of the display.
        - Added GOL_EMBOSS_SIZE to be user defined in GraphicsConfig.h.
        - Simplified initialization code for SSD1906 driver.
        Fixes:
        - Fixed touch screen algorithm.
        - Fixed file path error in Font & Bitmap Converter Utility.
        Migration Changes:
        - USE_GOL must be defined when using any Widgets.
        - New include directory paths are added to demo projects:
            - ../../../Your Project Directory Name
            - ../../Include
        - Moved all driver files to new directory
        - C files from ../Microchip/Graphics to ../Microchip/Graphics/Drivers
        - GOL_EMBOSS_SIZE can now be defined by the user in GraphicsConfig.h.
          If user does not define this in GraphicsConfig.h the default value 
          in GOL.h is used.

        <b> v1.4 Release (April 18, 2008) </b>
        
        Changes:
        - Added full support for PIC32 families.
        - Added Application Note demo on fonts.
        - Added images for end designs using PIC devices.
        Fixes:
        - Fixed GetPixel error in SSD1906 Driver.
        - Fixed SST39VF040 parallel flash driver reading instability.
        - Fixed error in 64Kbytes rollover in utility conversion of bitmaps and 
          SST39VF040 parallel flash driver.
        - Fixed milli-second delay on PIC32.
        - Fixed compile time option errors for PIC32.
        - Fixed Graphics Object Layer Demo error in PIC32 when time and date are set.
        - Fixed Picture widget bug in detecting touchscreen in translating messages.

        <b> v1.3 Release (March 07, 2008) </b>
        
        Changes:
        - none
        Fixes:
        - Fixed an inaccurate ADC reading problem with some Explorer 16 Development 
          Boards that uses 5V LCD display.
        - Editbox allocation of memory for text is corrected.
        - Fix slider SetRange() bug.
        - Set PIC32 configuration bits related to PLL to correct values.
        - Touch screen portrait mode bug is fixed.

        <b> v1.2 Release (February 15, 2008) </b>
        
        Changes:
        - Added support for Graphics PICtail Plus Board Version 2
        - Added support for foreign language fonts
        - Version 1.2 of the font and bitmap utility
        - Support for multi-language scripts
        - Support for generating font table from installed fonts
        - Support to reduce font table size by removing unused characters
        - Support to select between C30 and C32 compiler when generating bitmaps
          and font tables.
        - Added Chinese version of Graphics Object Layer Demo.
        - New Display Controller Drivers supported
        - Solomon Systech SSD1906
        - Orise Technology SPDF5408
        - Replaced USE_UNICODE compile switch to USE_MULTIBYTECHAR compile switch 
          to define XCHAR as 2-byte character.
        - Added compile switches
        - GRAPHICS_PICTAIL_VERSION - sets the PICtail board version being used.
        - USE_MONOCHROME to enable monochrome mode.
        Graphics Library Help Release Notes
        Graphics Library Version 3.06.04 21
        - USE_PORTRAIT - to enable the portrait mode of the display without changing 
          the display driver files.
        - Added beta support for PIC32 device
        Fixes:
        - Specification changes to List Box widget. Bitmap is added to List Box items.
        - Fixed List Box LbDelItemsList() error in not resetting item pointer to 
          NULL when items are removed.
        - Editbox allocation of memory for text is corrected.
        - Static Text multi-byte character failure is fixed.
        - Bar() function erroneous call to MoveTo() is removed since it causes the 
          drawing cursor to be displaced.
        - Removed SCREEN_HOR_SIZE and SCREEN_VER_SIZE macros from documentation. 
          Maximum X and Y sizes are to be obtained by GetMaxX() and GetMaxY() macros.

        <b> v1.0 Release (November 1, 2007) </b>
        
        Changes:
        - Edit Box, List Box, Meter, Dial widgets are added.
        - Button is modified. New options for the object are added.
        - Modified OBJ_REMOVE definition to OBJ_HIDE 
          (example BTN_REMOVE to BTN_HIDE).
        - Modified GOLPanelDraw() function to include rounded panels.
        - External memory support for the fonts and bitmaps is implemented.
        - SSD1339 and LGDP4531 controllers support is added.
        - Bevel(), FillBevel() and Arc() functions are added.
        - Modified Graphics Object Layer Demo.
        - Added Graphics External Memory Demo & Graphics PICtailTM Board 
          Memory Programmer Demo.
        - Added Graphics Application Note (AN1136- How to use widgets.) Demo.
        Fixes: none

        <b> v0.93 Beta release (August 29, 2007) </b>
        
        Changes: none
        Fixes:
        - In demo code the bitmap images couldn't be compiled without optimization. 
          bmp2c.exe utility output is changed to fix this bug.
        - In demo code "volatile" is added for global variables used in ISRs.
        
        <b> v0.92 Beta release (July 25, 2007) </b>
        
        Changes:
        - Keyboard and side buttons support is added.
        - Keyboard focus support is added.
        - PutImage() parameters are changed. Instead of pointer to the bitmap 
          image the pointer to BITMAP_FLASH structure must be passed.
        - GOLSuspend() and GOLResume() are removed.
        - GOLMsg() doesn't check object drawing status. It should be called if 
          GOL drawing is completed.
        - GOLStartNewList() is replaced with GOLNewList().
        - Line() function calls are replaced with Bar() function calls for 
          vertical and horizontal lines.
        - Parameter "change" is removed for SldIncVal() and SldDecVal() .
        - Some optimization and cleanup.
        - Slider API is changed:
            - SldSetVal() is changed to SldSetPos()
            - SldGetVal() is changed to SldGetPos()
            - SldIncVal() is changed to SldIncPos()
            - SldDecVal() is changed to SldDecPos()
            - SldCreate() input parameter are changed:
            - "delta" changed to "res"
        Fixes:
        - PutImage().
        - Line().
        - FillCircle().
        - For vertical slider the relation between thumb location and slider position 
          is changed. For position = 0 thumb will be located at the bottom. 
          For position = range it will be at the top.

        <b> v0.9 Beta release (July 06, 2007) </b>
        
        Changes:
        - Background color support is removed.
        - Non-blocking configuration for graphics primitives is added.
        - GetImageWidth(), GetImageHeight() are added.
        - The following functions are terminated by 
          control characters (< 32):
          - OutText()
          - OutTextXY() 
          - GetTextWidth() 
        - Graphics Objects Layer (GOL) is added.
        - Button, Slider, Checkbox, Radio Button, Static Text, Picture control, 
          Progress Bar, Window, Group Box are implemented.
        - Touch screen support is added.
        Fixes:
        - ReadFlashByte().
        - GetTextWidth().

        <b> v0.1 (June 05, 2007) </b>
        
        Changes:
        - Initial release includes driver and graphic primitive layers only.
        - Only driver for Samsung S6D0129 controller is available.
        Fixes:
        - None.
        Known Issues
        