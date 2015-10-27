/*******************************************************************************
 Module for Microchip Graphics Library - Graphic Library Documentation

  Company:
    Microchip Technology Inc.

  File Name:
    gfx_doc.h

  Summary:
    Documentation file for the Microchip Graphics Library.

  Description:
    This module organizes the help file of the Microchip Graphics Library.
    The Primitive and Object Layer APIs are grouped into functionalities.
    The usage of some features are also discussed here.
    
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
        Section: Using the Library Documentation
================================================================================
*******************************************************************************/

//@@using_the_graphics_library_doc
/*
    <TITLE Using The Library>

    Summary: 
        This topic describes the architecture of the Graphics Library and the 
        components comprising each layers. Information on how to use the 
        Primitive Layer and Object Layer is also presented.
        
*/

//@@using_the_graphics_library_library_overview_doc
/*
    <TITLE Library Overview>
    <GROUP using_the_graphics_library_doc>

    Summary: 
        This section describes the architecture of the library and its basic
        components.
        
    Description:
        The Graphics Library is composed of the following layers:
        - 1. Application Layer - This is the program that utilizes the Graphics 
                Library.
        - 2. User Message Interface - This is a layer should be implemented by user 
                to provide messages for the library.
        - 3. Graphics Object Layer - This layer renders the control objects such as 
                button, list box, progress bar, meter and so on.
        - 4. Graphics Primitives Layer - This layer implements the primitive 
                rendering functions.
        - 5. Device Display Driver - This layer is the graphics display driver 
                component that is optimized to the actual display module used.
        - 6. Graphics Display Module - This is the actual display module.        

*/

//@@using_the_graphics_object_layer_graphics_object_doc
/*
    <TITLE Graphics Objects>
    <GROUP using_the_graphics_library_library_overview_doc>

    Summary: 
        This section describes the basics of the Graphics Objects.
        
    Description:
        The Graphics Object Layer is composed of objects that can act as control
        widgets in an application. There are several pre-defined objects in the 
        library. Users are not limited to these predefined objects. It is possible
        to create custom objects. Creating custom objects will require a deeper 
        understanding of the library's architecture. Creating custom objects
        is described in an application note mentioned at the Release Notes section
        of this documentation.
        
        Each object will have its own create function.
        In the create function the following functions are initialized:
            - DRAW_FUNC - This is the function that renders the object on 
                    the screen.
            - FREE_FUNC - This is the function that frees the memory used 
                    by the object when the object is removed from memory.
            - ACTIONGET_FUNC - This function is optional. This function   
                    returns the a translated action of the object. A
                    translated action is actually a planned action 
                    that indicates the intended change in the state of the 
                    object. The translated action is basically the response
                    of the object to the user interaction on the object. 
                    For example: 
                    When a user presses on a button object on a screen, 
                    a valid planned action of the object is to change its 
                    state to a pressed state. Translated actions are specific 
                    to each object. See GFX_GOL_TRANSLATED_ACTION enumeration
                    for the listing of all translated actions of all objects
                    in the library. This list do not include user defined
                    actions. Translated actions are determined using the 
                    messaging scheme of the library. Messaging is discussed
                    in a separate section of this documentation.
                    When this function is not defined, the create function
                    of the object must set the function pointer to NULL.
            - ACTIONSET_FUNC - This function is optional. This function 
                    performs the translated action returned by the 
                    ACTIONGET_FUNC. This is the actual function that changes 
                    the state of the object. When this function is not 
                    defined, the create function of the object must set 
                    the function pointer to NULL.

        Each of these functions are implemented in each object.
        The draw function of the object is specific to the object. The draw
        function should not be called directly by the application. Application 
        should instead use the GFX_GOL_ObjectListDraw() so the management of 
        the object rendering will be done by the Graphics Object Layer.
 */
 
//@@using_the_graphics_object_object_layer_rendering_doc
/*
    <TITLE Object Layer Rendering>
    <GROUP using_the_graphics_library_library_overview_doc>

    Summary: 
        This section describes how objects are rendered.
        
    Description:   
    
        <b> Object List Rendering </b>
    
        Initially, application will create the objects that it will use by
        calling the create function of the objects. As the objects are created, 
        a linked list of objects is created. This linked list of objects is 
        the list that the library will use to manage the rendering of the objects.
        The application will only need to call GFX_GOL_ObjectListDraw() function
        to execute the rendering of all the objects. It is also through this 
        list that the library is able to parse all the objects to determine
        if an object is affected by the user action. This will be discussed in 
        the messaging portion of this documentation.
        
        Each object, has a state variable that is composed of state bits. There are
        two main groups in the state bits:
            - Property State Bits - this defines the general shape of the object.
            - Draw State Bits - this determines if the object will be redrawn. 
                The redrawing can be a full object redraw or partial object redraw.
                Depending on the object, definition, a partial redraw means that
                only portion of the object is redrawn. This makes the redraw of
                the object fast and efficient. For example, redrawing the full
                progress bar object will be longer than just updating the level 
                of the progress bar.
        The Property state bits are usually defined once at the object creation.
        The Draw state bits, on the other hand dynamically changes and usually
        changes because of user action on the object. These user actions are encoded
        as messages. The messages are created in user interface drivers such as 
        touch screen or key pads on the system (see messaging section for details).
    
        The objects in the library implements the rendering sequence using rendering 
        state machine. The rendering state is not to be confused with the Draw State
        Bits of the objects. Rendering states are defined in each object's rendering 
        function. An object is basically drawn using combination of primitive calls. 
        Bars, lines, and text can be drawn to represent an object. The purpose of 
        these rendering states is to divide the primitive calls into steps. By doing 
        this, each primitive call is now a step in the process. If one of the primitive
        call cannot proceed due to busy hardware resource, the object's rendering
        sequence can be paused. Since, the length of the delay in the rendering cannot 
        be predicted, it is best to pause the rendering of the object and give back
        control of the CPU to application. Each of the object's rendering function
        coupled with GFX_GOL_ObjectListDraw(), has this capability. This effectively
        pauses the rendering of the object. 

        For the rendering to continue, GFX_GOL_ObjectListDraw() is called again by 
        the application. When this is called, the specific object rendering function
        that was called is then resumed and in the object rendering function, the 
        specific step (or primitive call) is executed again. This basically continues
        the rendering exactly where it left off.
        
        <b> Object Draw Callback Feature </b>
        To allow application to control the rendering of the objects, a callback 
        function is provided (see GFX_GOL_DRAW_CALLBACK_FUNC).  The callback
        function is an application implemented function. The library provides the
        GFX_GOL_DrawCallbackSet(GFX_GOL_DRAW_CALLBACK_FUNC) API to set the 
        callback function. GFX_GOL_DRAW_CALLBACK_FUNC parameter in this function
        is the pointer to the defined callback.
        
        The callback allows the application to insert application defined 
        rendering. 
        
        For example: 
        A digital signal is sampled on a pin. The value of the signal determines 
        if a scroll bar will increment or decrement its value. To implement the 
        change in the value of a scroll bar, a callback function can be 
        implemented to do that. 
        
        <code>
            //*********************************************************************
            //  Main
            //*********************************************************************
            int main(void)
            {
                // GOL message structure
                GFX_GOL_MESSAGE msg;

                //Initialize board, drivers and graphics library
                SYSTEM_InitializeBoard();
                GFX_Initialize();

                // set the message callback function pointer
                GFX_GOL_MessageCallbackSet(APP_ObjectMessageCallback);
                
                // set the draw callback function pointer
                GFX_GOL_DrawCallbackSet(APP_ObjectDrawCallback);
    
                ....

                while(1)
                {
                    // Draw GOL objects
                    if(GFX_GOL_ObjectListDraw() == GFX_STATUS_SUCCESS)
                    {

                        // Get message from touch screen
                        TouchGetMsg(&msg);              
                        // Process message
                        GFX_GOL_ObjectMessage(&msg);    

                    }
                }

            }

            //*********************************************************************
            //  The draw callback is called within GFX_GOL_ObjectListDraw()
            //*********************************************************************
            bool APP_ObjectDrawCallback(void)
            {
                // assume pScrollBar is pointer to the scrollbar
   
                bool signalAssert;
    
                // assume signal is sampled at a pin RC1
                if (PORTCbits.RC1 == 1)
                    signalAssert = true;
                else
                    signalAssert - false;
        
                if (signalAssert == true)    
                {
                    // increment the value
                    GFX_GOL_ScrollBarPositionIncrement(pScrollBar);
                }
                else
                {
                    // increment the value
                    GFX_GOL_ScrollBarPositionDecrement(pScrollBar);
                }

                // redraw only the thumb
                GFX_GOL_ObjectStateSet(pScrollBar, GFX_GOL_SCROLLBAR_DRAW_THUMB_STATE);        

                return (true);
            }        
        </code>
        
        
        Since the callback is called in GFX_GOL_ObjectListDraw() and 
        GFX_GOL_ObjectListDraw() is called in the main loop, the callback
        can potentially be executed unnecessarily if the sampled signal
        frequency of change is slow. To counter that, the callback can be
        modified to be sampling the signal after some pre-defined time delay.
        
        <code>
        
            //*********************************************************************
            //  The draw callback is called within GFX_GOL_ObjectListDraw()
            //*********************************************************************
            bool APP_ObjectDrawCallback(void)
            {
                // assume pScrollBar is pointer to the scrollbar
                // assume tick is a system counter, DEFINED_DELAY is
                // a predefined delay tuned to the signal frequency
   
                       bool signalAssert;
                static uint32_t prevtick = 0;
    
                if (tick - prevtick > DEFINED_DELAY)
                {
                    prevtick = tick;
                    
                    // assume signal is sampled at a pin RC1
                    if (PORTCbits.RC1 == 1)
                        signalAssert = true;
                    else
                        signalAssert - false;
        
                    if (signalAssert == true)    
                    {
                        // increment the value
                        GFX_GOL_ScrollBarPositionIncrement(pScrollBar);
                    }
                    else
                    {
                        // increment the value
                        GFX_GOL_ScrollBarPositionDecrement(pScrollBar);
                    }

                    // redraw only the thumb
                    GFX_GOL_ObjectStateSet(pScrollBar, GFX_GOL_SCROLLBAR_DRAW_THUMB_STATE);        
                }
                
                return (true);
            }        
        </code>        

*/

//@@using_the_graphics_object_layer_object_layer_messaging_doc
/*
    <TITLE Object Layer Messaging >
    <GROUP using_the_graphics_library_library_overview_doc>

    Summary: 
        This section describes the basics of object messages.
        
    Description:        
        User inputs can also change the state of objects. This is done through
        messaging. The library passes user action to the objects using the 
        message structure (see GFX_GOL_MESSAGE). The purpose of the messaging
        is to translate user action (i.e. a user press on a touch screen) to 
        a change in state of the objects in the screen. At the same time, this
        user action will cause an application function to execute.
        
        For example: A press on a button on the screen will result in an LED
        to be turned on. While pressing on the button, the button on the 
        screen also changes states and is redrawn to a pressed state.
        When the user let's go of the press, the button redraws back to an
        unpressed state and the LED turns off.
        
        The mechanism of messaging and object state change is explained 
        in the next section.
        
        <b> Messaging Structure </b>
        The messages are encoded in the GFX_GOL_MESSAGE structure. 
        
        <code>
            typedef struct
            {
                uint8_t   type;                 // Specifies the type of input device.
                uint8_t   uiEvent;              // An event that occurred in the input
                                                // device.
                int16_t   param1;               // Parameter 1, definition and usage is
                                                // dependent on the type of input device.
                int16_t   param2;               // Parameter 2, definition and usage is
                                                // dependent on the type of input device.
            } GFX_GOL_MESSAGE;        
        </code>

        The type defines the type of input device. See INPUT_DEVICE_TYPE for the 
        currently supported type of devices. The uiEvent is also a set of predefined
        events. See INPUT_DEVICE_EVENT for a list of supported events. The two
        parameters, param1 and param2, changes in usage depending on the type 
        of input device and event. 
        
        The messages are encoded in the drivers for the input devices. The drivers
        provide functions for the application to call. These functions should 
        return the message whenever there is a valid user action on the input
        device. 
        
        The example code below shows TouchGetMsg() as the function provided
        by the touch screen driver to retrieve user inputs on the touchscreen.
        <code>
            // GOL message structure
            GFX_GOL_MESSAGE msg;
            
            ...
            // set the message callback function pointer
            GFX_GOL_MessageCallbackSet(APP_ObjectMessageCallback);
            ...
            
            while(1)
            {
                // Draw GOL objects
                if(GFX_GOL_ObjectListDraw() == GFX_STATUS_SUCCESS)
                {

                    // Get message from touch screen
                    TouchGetMsg(&msg);              
                    // Process message
                    GFX_GOL_ObjectMessage(&msg);    

                }
            }
        </code>

        <b> Graphics Object Message Function </b>
        
        After the input device driver encodes the message, the message
        is sent by the application to the graphics library through the 
        GFX_GOL_ObjectMessage(). This function parses the active list 
        of objects and determines if the message affects one or more of
        the objects. The mechanism to check each object is simple:
        the GFX_GOL_ObjectMessage() calls each of the object's ACTIONGET_FUNC.
        The ACTIONGET_FUNC of an object is the function that determines if
        the message affects the object. This function returns a translated 
        action (see GFX_GOL_TRANSLATED_ACTION enumeration).
        
        If the object is not affected it replies with 
        GFX_GOL_OBJECT_ACTION_INVALID. If the object is affected, it replies 
        with the appropriate translated action. 

        <b> Object Message Callback Feature </b>
        
        If the object is affected, GFX_GOL_ObjectMessage() then calls the 
        message callback function. The message callback function is an 
        application defined function. This is set initially using the
        GFX_GOL_MessageCallbackSet(GFX_GOL_MESSAGE_CALLBACK_FUNC) call where 
        GFX_GOL_MESSAGE_CALLBACK_FUNC is a pointer to the defined callback
        function. 
        
        The message callback is an opportunity for the application to 
        respond to user inputs. It is also an opportunity to block the 
        library from performing default state change to objects. See
        the documentation of the callback function for details.

        The message callback returns true if the application wants to 
        perform the default action set of the objects. If the callback 
        returns false, the application assumes all changes in the states
        of the object.
        
        After the callback is executed, with a return value of true the 
        ACTIONSET_FUNC of the affected object is called by 
        GFX_GOL_ObjectMessage(). This function performs the state change
        of the object due to the user input. The object is then redrawn
        when GFX_GOL_ObjectListDraw() is called by the application.
        

        Below is an example usage of the message callback function.
        A scroll bar is incremented and decremented by two buttons.
        <code>
        
            //*********************************************************************
            //  The message callback is called within GFX_GOL_ObjectMessage()
            //*********************************************************************
            
            // assume that 2 buttons and a scroll bar is present in the system
            
            bool APP_MsgCallback(
                            uint16_t objMsg, 
                            GFX_GOL_OBJ_HEADER *pObj, 
                            GFX_GOL_MESSAGE *pMsg)
            {
                uint16_t           objectID;
                GFX_GOL_SCROLLBAR  *pScrollBar;

                objectID = GFX_GOL_ObjectIDGet(pObj);

                if(objectID == ID_BTN1)
                {
                    // check if button is pressed
                    if(objMsg == GFX_GOL_BUTTON_ACTION_PRESSED)
                    {
                        // find slider pointer
                        pScrollBar = (GFX_GOL_SCROLLBAR *)GFX_GOL_ObjectFind(ID_SLD1);
                        // decrement the slider position
                        GFX_GOL_ScrollBarPositionDecrement(pScrollBar);
                        // redraw only the thumb
                        GFX_GOL_ObjectStateSet(pScrollBar, GFX_GOL_SCROLLBAR_DRAW_THUMB_STATE);
                    }
                }

                if(objectID == ID_BTN2)
                {
                    if(objMsg == GFX_GOL_BUTTON_ACTION_PRESSED)
                    {
                        // find slider pointer
                        pScrollBar = (GFX_GOL_SCROLLBAR *)GFX_GOL_ObjectFind(ID_SLD1);    
                        // increment the slider position
                        GFX_GOL_ScrollBarPositionIncrement(pScrollBar);
                        // redraw only the thumb
                        GFX_GOL_ObjectStateSet(pScrollBar, GFX_GOL_SCROLLBAR_DRAW_THUMB_STATE);
                    }
                }
                return (true);
            }        
        
        </code>
        
        Notice that the callback do not check if the scroll bar was affected by the 
        message. Since the callback returns true, the scroll bar ACTIONSET_FUNC
        will be the one that will perform the state change of the scroll bar if 
        the user directly interacted with the scroll bar.
        
        <b> Graphics Object Action Get and Action Set Functions </b>
        
        Each object has a predefined translated actions (see 
        GFX_GOL_TRANSLATED_ACTION enumeration). These actions are returned 
        by the object's ACTIONGET_FUNC. The ACTIONGET_FUNC is called by the 
        GFX_GOL_ObjectMessage() function. 
        
        The ACTIONGET_FUNC evaluates the message to determine if the user 
        action affected the object. If the object is affected, it replies 
        with one of the predefined translated action of the object. 

        For a button object the following translated actions are returned 
        when the button is affected:
            - GFX_GOL_BUTTON_ACTION_PRESSED - button is pressed 
            - GFX_GOL_BUTTON_ACTION_STILLPRESSED - button is continuously pressed 
            - GFX_GOL_BUTTON_ACTION_RELEASED - button is released 
            - GFX_GOL_BUTTON_ACTION_CANCELPRESS - button button press canceled            
        When the button is not affected it returns: 
            - GFX_GOL_OBJECT_ACTION_INVALID - invalid message response

        All objects returns GFX_GOL_OBJECT_ACTION_INVALID when they are not 
        affected by the message.
        
        the ACTIONSET_FUNC on the other hand performs the actual state change 
        of the object. The draw state bits of the object are modified to indicate
        that the object will be redrawn in the next call to the 
        GFX_GOL_ObjectListDraw().
        
*/

/*******************************************************************************
================================================================================
        Section: How the Library Works Documentation
================================================================================
*******************************************************************************/

//@@how_the_graphics_library_works_doc
/*
    <TITLE How the Library Works>
    <GROUP using_the_graphics_library_doc>

    Summary: 
        This section describes how the Graphics Library works and how
        the Primitive and Object layers are used.
        
*/

/*******************************************************************************
================================================================================
        Section: Using the Primitive Layer Documentation
================================================================================
*******************************************************************************/

//@@using_the_primitive_layer_doc
/*
    <TITLE Using the Primitive Layer>
    <GROUP how_the_graphics_library_works_doc>

    Summary: 
        This section contains information on how to use the Primitive Layer 
        of the Graphics Library.
        
*/

//@@primitive_line_draw_doc
/*
    <TITLE Line Rendering>
    <GROUP using_the_primitive_layer_doc>

    Summary:
        This section describes how to render lines.

    Description: 
        When rendering lines, two things needs to be set:
        1. Set the color (GFX_ColorSet())
        2. Set the line style (GFX_LineStyleSet())
        
        After the color and style is set, GFX_LineDraw() can be called
        to render the line. GFX_LineDraw() can be called multiple times to 
        render lines that has the same color and line style. GFX_ColorSet()
        and GFX_LineStyleSet() will only be called when the color or the line
        style needs to be changed.
        For example: When one of the lines to be rendered has to change color, 
        the GFX_ColorSet() function must be called to change the color. 
        
        Remarks:
        Alpha blended lines as well as anti-aliased lines are not yet supported.
        
*/

//@@primitive_polygon_draw_doc
/*
    <TITLE Polygon Rendering>
    <GROUP using_the_primitive_layer_doc>

    Summary:
        This section describes how to render polygons.

*/

//@@unfilled_polygon_draw_doc
/*
    <TITLE Unfilled Polygon Rendering>
    <GROUP primitive_polygon_draw_doc>

    Summary:
        This section describes how to render unfilled polygons.

    Description: 
        Unfilled polygons are rectangles and rounded rectangles. Circles is a 
        special case of a rounded rectangle. Rendering unfilled polygons uses 
        the line styles.
        
        Similar to rendering lines, two things needs to be set:
        1. Set the color (GFX_ColorSet())
        2. Set the line style (GFX_LineStyleSet())
        3. Call the specific polygon function

*/        

//@@filled_polygon_draw_doc
/*
    <TITLE Filled Polygon Rendering>
    <GROUP primitive_polygon_draw_doc>

    Summary:
        This section describes how to render filled polygons.

    Description: 
        Filled polygons are filled rectangles and filled rounded rectangles. 
        Bar is a special case of a filled rectangle and filled circle is a 
        special case of a filled rounded rectangle.
            
        For filled polygons, the fill styles can be set:
        <TABLE>
            Fill Style                          Description
            ##################                  ############
            GFX_FILL_STYLE_NONE                 The effect of this fill type is the 
                                                    same as unfilled polygon.
            GFX_FILL_STYLE_ALPHA_COLOR          The fill will be the color set by 
                                                    the last call to GFX_ColorSet() 
                                                    function.
            GFX_FILL_STYLE_GRADIENT_DOWN        Sets that the fill style is alpha blended. 
                                                    The alpha blending effect will  
                                                    be dependent on the background type 
                                                    set (see GFX_BackgroundTypeSet()). 
                                                    The level of alpha blending will 
                                                    depend on the last 
                                                    GFX_AlphaBlendingValueSet() call.
            GFX_FILL_STYLE_GRADIENT_UP          The effect is a gradient fill in the 
                                                    vertical up direction.
            GFX_FILL_STYLE_GRADIENT_UP          The effect is a gradient fill in the 
                                                    vertical down direction.
            GFX_FILL_STYLE_GRADIENT_RIGHT       The effect is a gradient fill in the 
                                                    horizontal right direction.
            GFX_FILL_STYLE_GRADIENT_LEFT        The effect is a gradient fill in the 
                                                    horizontal left direction.
            GFX_FILL_STYLE_GRADIENT_DOUBLE_VER  The effect is a gradient fill in the 
                                                    vertical direction but with two 
                                                    transitions.
            GFX_FILL_STYLE_GRADIENT_DOUBLE_HOR  The effect is a gradient fill in the 
                                                    horizontal direction but with two 
                                                    transitions..
        </TABLE>        
       
        <align center>  
        <img name="bevelgradient.jpg" />
        
        <img name="bargradient.jpg" />
        </align>  
        
        
        To render alpha blended filled polygons:
        1. Set the color (GFX_ColorSet())
        2. Set the background type (GFX_BackgroundTypeSet())
        3. Set the alpha blending value (GFX_AlphaBlendingValueSet())
        4. Set the fill style to alpha blending (GFX_FillStyleSet())
        5. Call the specific polygon fill function
            - GFX_RectangleFillDraw()
            - GFX_RectangleRoundFillDraw()
            - GFX_BarDraw()
            - GFX_CircleFillDraw()
        
        To render gradient filled polygons:
        1. Set the color (GFX_ColorSet())
        2. Set the two gradient start and end colors (GFX_GradientColorSet())
        3. Set the fill style to one of the gradient fill types (GFX_FillStyleSet())
        4. Call the specific polygon fill function
            - GFX_RectangleFillDraw()
            - GFX_RectangleRoundFillDraw()
            - GFX_BarDraw()
            - GFX_CircleFillDraw()

        The following functions renders the same shapes:
        - GFX_RectangleRoundDraw(x, y, x, y, radius) is equivalent to
          GFX_CircleDraw(x, y, radius). 
        - GFX_RectangleRoundFillDraw(x, y, x, y, radius) is equivalent to
          GFX_CircleFillDraw(x, y, radius).
        - GFX_RectangleFillDraw(x1, y1, x2, y2) is equivalent to
          GFX_BarDraw(x1, y1, x2, y2).
        
        Remarks:
        - Alpha blended unfilled polygons are not yet supported.
        - Anti-aliased unfilled polygons are not yet supported.
        - Alpha blended, gradient fills are not yet supported.
        - Background is not needed when using gradient fills.
                
*/

//@@primitive_text_draw_doc
/*
    <TITLE Text Rendering and Font Features>
    <GROUP using_the_primitive_layer_doc>

    Summary:
        This section describes how to render text and strings and other features
        that are available for font resources.

*/

//@@text_draw_doc
/*
    <TITLE Text Rendering>
    <GROUP primitive_text_draw_doc>

    Summary:
        This section describes how to render text and strings.

    Description: 
        Text rendering in the Graphics Library requires a font table. The font table
        is considered one of the resources that is used in the library 
        (see GFX_RESOURCE_FONT of the GFX_RESOURCE_HDR structure).
        
        Once the font resource is available, text rendering can be performed by one
        of the following functions:
        - GFX_TextCharDraw() - Use this to render a single character.
        - GFX_TextStringDraw() - Use this to render a string of characters.
        - GFX_TextStringBoxDraw() - Use this to render formatted string of characters.

        *Character Rendering*
        
        To render a character:
        1. Set the color (GFX_ColorSet())
        2. Set the font resource to use (GFX_FontSet())
        3. Set the location where the character will be rendered 
           (GFX_TextCursorPositionSet())
        4. Render the character (GFX_TextCharDraw())

        *String Rendering*
        
        To render a string:
        1. Set the color (GFX_ColorSet())
        2. Set the font resource to use (GFX_FontSet())
        3. Render the string (GFX_TextStringDraw())
        
        GFX_TextStringDraw() parameters sets the location where the string will 
        be rendered. The length parameter allows versatility in rendering.
        When this parameter is set to 0, the rendering will terminate 
        when the string terminator is encountered. When the length is non-zero, 
        the rendering can be performed with the length as the terminating condition. 
        This allows applications to render a portion of a longer string. 
        
        <img name="stringrenderingexample.jpg" />

        Multiple lines of strings are also supported. The string can contain the 
        new line characters and rendering results in multiple lines of text.
        The use GFX_TextStringDraw() will always render strings left aligned. 
        To render text with alignment use GFX_TextStringBoxDraw().

        *Formatted String Rendering*
        
        To render string with formatting:
        1. Set the color (GFX_ColorSet())
        2. Set the font resource to use (GFX_FontSet())
        3. Render the string (GFX_TextStringBoxDraw())
        
        Parameters of GFX_TextStringBoxDraw() determines the rectangular area where
        the string will be rendered. The alignment (see GFX_ALIGNMENT) specifies
        how the text will be aligned in the defined text area. The termination of the
        rendering is the same as the GFX_TextStringDraw() where the length parameter  
        can be used to determine how many characters will be rendered. 
        
        <img name="stringboxrenderingexample.jpg" />
        
        Added to these rendering functions, support functions are also provided 
        for versatility:
        - GFX_TextStringWidthGet() - Use this to get the length of a string in pixels.
        - GFX_TextStringHeightGet() - Use this to get the height of the given font.
            All characters in a given font resource will have a constant height.
        - GFX_FontSet() - Use this to set the font resource to use.    
        - GFX_FontGet() - Use this to check the currently used font resource.    
*/ 

//@@antialiased_text_doc
/*
    <TITLE Anti-aliased Fonts>
    <GROUP primitive_text_draw_doc>

    Summary:
        This section describes the anti-aliasing feature of the library.
        
    Description: 
        The Graphics Library supports rendering of anti-aliased font resource.
        Anti-aliasing is a technique used to make the edges of text appear smooth. 
        This is useful especially with characters like 'A', 'O', etc which has slant 
        or curved lines. Since the pixels of the display are arranged in rectangular 
        fashion, slant edges can't be represented smoothly. To make them appear smooth, 
        a pixel adjacent to the pixels is painted with an average of the foreground 
        and background colors as depicted in figure below. 

        <img name="antialiasedfont.jpg" />
        
        When anti-aliasing is turned off, the pixels abruptly changes from background 
        color to foreground color shown in the figure below. To implement anti-aliasing, 
        adjacent pixels transitions from background to foreground color using 25% or 
        75% mid-color values from background to foreground colors. This feature in 
        fonts will require roughly twice the size of memory storage required for font 
        glyphs with no anti-aliasing. 

        <img name="nonantialiasedfont.jpg" />
        
        Since the average of foreground and background colors needs to be calculated 
        at runtime, the rendering of anti-aliased fonts take more time than rendering 
        normal fonts. 
        
        Anti-aliasing transparency can be set to one of the following using 
        GFX_FontAntiAliasSet() (see GFX_FONT_ANTIALIAS_TYPE):
        - GFX_FONT_ANTIALIAS_OPAQUE - Mid colors are calculated once while rendering 
            each character which is ideal for rendering text over a constant background.
            This is the default setting of the library.
        - GFX_FONT_ANTIALIAS_TRANSLUCENT - The mid values are calculated for every 
            pixel and this feature is useful while rendering text over an image or 
            on a non-constant color background. 
        
        As a result, rendering anti-aliased text takes longer with translucent type 
        than compared to opaque type.
        
        To use anti-aliasing, the font resource must contain glyphs that are 
        anti-aliased enabled. The Graphics Resource Converter (GRC) tool that comes
        with the installation of the Graphics Library can generate such font resources.
        
        Remarks:
        - Application may use anti-aliased and non-anti-aliased fonts at  the same time.
*/

//@@extended_glyph_text_doc
/*
    <TITLE Extended Glyphs>
    <GROUP primitive_text_draw_doc>

    Summary:
        This section describes the extended glyphs feature of the library.
        
    Description: 
        Extended glyphs are needed to render characters of certain languages which 
        use more than one byte to represent a single character. 
        For example: Asian languages like Thai, Hindi, etc. In these character set, 
        more than one glyph overlaps each other to form a single character of that 
        language as shown in the figure below. 
        
        <img name="extendedglyphfont.jpg" />
        
        Extended glyph fonts can be generated using the Graphics Resource Converter 
        (GRC) tool that comes with the installation of the Graphics Library.
        
        Remarks: 
        - The fonts used with extended glyphs are normal ANSI fonts and not Unicode fonts.        
        
                
*/

//@@primitive_double_buffering_doc
/*
    <TITLE Double Buffering Features>
    <GROUP using_the_primitive_layer_doc>

    Summary:
        This section describes how to enable and use double buffering in the 
        Graphics Primitive Layer.
        
    Description: 
        
        Manipulating pixels on the screen requires direct writes to the 'Frame 
        Buffer'. While these changes are being executed, the screen is also 
        being refreshed. This means that the changes are displayed immediately  
        as the frame buffer is being updated. This is not a suitable scheme  
        when the changes are slow, for example, decoding an image or having a 
        large number of widgets on a screen. The display may appear slow in 
        that case.  One solution to this problem is to use a double-buffering 
        scheme supported by the Microchip Graphics Library. 
        
        The double buffering scheme do not render directly to the frame buffer, 
        instead, rendering is performed to a separate buffer, called the 
        'Draw Buffer'. After all the changes are made, the Draw Buffer and 
        Frame Buffer are swapped. After the swap, the Draw Buffer becomes the 
        new Frame Buffer and the Frame Buffer becomes the new Draw Buffer. 
        This swapping appears as an instant change in the contents of the 
        screen. 
        
        The time it takes to render to the 'Draw Buffer' does not change but
        since rendering occurs in the buffer that is not shown on the screen
        the user will not see the pixels being changed. Instead what the users
        see is the instant switch of the screen contents when the two buffers
        are swapped. The net effect is that the user experience is better since
        the user do not see the slow rendering.
        
        The next rendering will now occur on the new 'Draw Buffer' while the 
        user views the screen with pixels from the 'Frame Buffer'. The cycle
        of rendering to the 'Draw Buffer' and displaying the 'Frame Buffer'
        continues whenever new pixels are being drawn.
        
        The use of double buffering feature will double the requirement of the
        memory for the buffers. Since this feature is dependent on the availability 
        of the memory resources for buffers, the feature can only be used when
        the Display Driver Layer of the library supports double buffering 
        features.
        
        The feature is enabled by default in configurations where double buffering
        is supported. To disable the feature, define GFX_CONFIG_DOUBLE_BUFFERING_DISABLE
        in the gfx_config.h header file.

        the following steps shows how the double buffering feature is enabled
        and used when rendering primitive shapes. However, for the synchronization
        there are two ways to perform it. It is differentiated in step 3.
        There are two ways to use the feature in rendering primitive shapes.
        Method A - use this method when the driver layer that implements the
        management of the swapping of buffers, uses some synchronization timing
        to perform the swap (usually timed to the vertical synchronization period
        of the display):
        1. Enable the feature (GFX_DoubleBufferEnable())
        2. Render primitive shapes. When rendering the shapes there are two
           ways to perform rendering and synchronization.
               - Render and register each new area. 
                   - Render an area
                   - Register the rectangular area that was 
                     modified (GFX_DoubleBufferAreaMark())
               - Render all primitives and rectangular areas and register 
                 the whole screen. This method will take more time since 
                 more pixels are being synchronized.
                   - Render shapes and areas.
                   - Register the whole screen to be updated
                     (GFX_DoubleBufferSyncAllStatusSet())                 
        3. Synchronize the buffers so they will contain the same pixel information.
           There are also two ways to perform this.
               - Synchronize using a defined synchronization point. In this method,
                 the driver is set up to perform the point of synchronization. For 
                 TFT displays, the vertical synchronization period is the best
                 place to perform this since it will remove any possibility of
                 showing portion of the screen with different data. 
                 This will happen when the refresh is in the middle of the screen
                 and the buffers are swapped. 
                   - GFX_DoubleBufferSynchronizeRequest()
                   - Wait until the synchronization has finished using 
                     GFX_DoubleBufferSynchronizeStatusGet()    
                     i.e.: while(GFX_DoubleBufferSynchronizeStatusGet() == GFX_FEATURE_ENABLED);
               - Synchronize the buffers immediately.
                   - GFX_DoubleBufferSynchronize() 
        4. Repeat steps 2-3 for more rendering sequences.

        If double buffering is disabled during run time, simply call 
        GFX_DoubleBufferDisable(). This function will perform one last synchronization
        in order to update the screen with recent rendering since the last 
        synchronization.
        
        <img name="double_buffering_flow.jpg" />
        
        
*/


/*******************************************************************************
================================================================================
        Section: Using the Graphics Object Layer Documentation
================================================================================
*******************************************************************************/

//@@using_the_object_layer_doc
/*
    <TITLE Using the Graphics Object Layer>
    <GROUP how_the_graphics_library_works_doc>

    Summary: 
        This section contains information on how to use the Graphics Object Layer 
        of the Graphics Library.
        
*/

//@@using_the_widgets_doc
/*
    <TITLE Object Rendering and Style Schemes>
    <GROUP using_the_object_layer_doc>

    Summary:
        This section describes how style schemes are used in rendering the
        objects of the Graphics Library.

    Description:

        <b> Dynamic Memory Usage </b>

        All the objects in the library can be created dynamically at run time.
        This is performed by each object's create function. The table below 
        shows the dynamic memory requirement for each object.
              
        <TABLE>
            Object Name                         RAM Requirement (in bytes per instance)
            ##################                  #######################################
            GFX_GOL_BUTTON                      40 
            GFX_GOL_CHECKBOX                    32 
            GFX_GOL_DIGITALMETER                40 
            GFX_GOL_EDITBOX                     36 
            GFX_GOL_GROUPBOX                    34 
            GFX_GOL_LISTBOX                     38 
            GFX_GOL_METER                       62 
            GFX_GOL_PICTURE                     50 
            GFX_GOL_PROGRESSBAR                 32 
            GFX_GOL_RADIOBUTTON                 36 
            GFX_GOL_SCROLLBAR                   40 
            GFX_GOL_STATICTEXT                  30 
            GFX_GOL_TEXTENTRY*                  44 
            GFX_GOL_WINDOW                      34 
            GFX_GOL_TEXTENTRY_KEYMEMBER*        24 

        </TABLE>        
        
        Each instance of an object will use require the number of bytes given. 
        The total dynamic memory required per object is computed by multiplying
        the number of instances of the object by the required memory per instance.
        
        For example: 10 instances of buttons will need 10*40 = 400 bytes.
        
        For Text Entry, the number of keys will contribute to the required 
        dynamic memory. Each instance of the text entry object will require 24 bytes
        per key. Therefore, for a 3x3 keys, there will be 24 * 9 bytes needed per 
        instance. Therefore, for a text entry with 9 keys, the total per instance 
        is 44 + (24 * 9) bytes.

        <b> Style Scheme </b>

        All the objects in the library utilizes the style scheme to define
        how the object will rendered on the screen. The style scheme defines
        the colors, images, fonts and text that will be used. Choosing the 
        appropriate colors and settings allows the object to be rendered in 
        so many ways.
             
        Style schemes are applied to the objects by assigning the style scheme
        pointer to the object at creation. When the objects are rendered, the 
        current settings of the style scheme will then be used to render the
        object. Objects cannot be created without a valid style scheme. This
        means that it cannot be null or undefined when the object is created.
        Style scheme can be modified anytime. It can be replaced using the 
        GFX_GOL_ObjectStyleSchemeSet(). 

        Style scheme is divided into these groups 
        (See GFX_GOL_OBJ_SCHEME for details):
            1. Text - The parameter that defines the text used in the object
                - pFont - defines the font used in the style of the object.
            2. Colors - The parameters that defines the shape of the object. 
                - EmbossDkColor - defines the dark emboss color.
                - EmbossLtColor - defines the light emboss color.
                - TextColor0 - defines the first text color option.
                - TextColor1 - defines the second text color option.
                - TextColorDisabled - defines the third text color option.
                - Color0 - defines the first face color option.
                - Color1 - defines the second face color option.
                - ColorDisabled - defines the third face color option.
                - EmbossDkColor - defines the dark emboss color.
            3. Background - The parameters that describes the background of the object.
                    The background information defines how the object will be drawn
                    with the background taken into account.
                - CommonBkColor - Background color used to hide when the background 
                    type (GFX_BACKGROUND_TYPE) is set to 
                    GFX_BACKGROUND_COLOR or GFX_BACKGROUND_NONE.
                - CommonBkLeft - the horizontal starting position of the background.
                - CommonBkTop - the vertical starting position of the background.
                - CommonBkType - specifies the type of background to use.
                - pCommonBkImage - pointer to the background image used when
                    the background type is set to GFX_BACKGROUND_IMAGE.
            4. Fill - The parameters that defines how the fill will be performed.
                - fillStyle - specified the fill style to be used (see GFX_FILL_STYLE).
                - AlphaValue - alpha value used for alpha blending. This will be
                    used then fillStyle is set to GFX_FILL_STYLE_ALPHA_COLOR.
                - gradientStartColor - start color of the gradient fill. This will 
                    be used then fillStyle is set to any of the gradient fill
                    styles.
                - gradientEndColor - end color of the gradient fill.  This will 
                    be used then fillStyle is set to any of the gradient fill
                    styles.
            5. Style - The parameter that defines the 3-D effect of the object.
                - EmbossSize - defines the emboss size of the panel for 3-D effect. 
                    This parameter should be set to 0 when not used.

        <b> Object Panel </b>

        Most of the objects in the Graphics Library utilizes a generic panel
        to define the object's shape. When using the provided objects in the
        library, the details of how the panel is utilized can be transparent 
        to the users. In cases, where users defines their own objects the 
        following detailed discussion of the panel is important.
        
        A panel is a rectangular shape that is  utilized by the objects to 
        define its dimension. The panel is then drawn with the GOL_PANEL_PARAM 
        to define the shape's colors. The colors are taken from the currently 
        assigned style scheme to the object. 
        
        <align center> 
        <img name="object_panel_style.jpg" /> 
        </align>

        The assignment of the panel colors from the style scheme is shown below:
        
        <b><i> Style Scheme to Panel Assignment </i></b>      
            <TABLE>
                <b>Style Scheme Parameter</b>       <b>Panel Parameter</b>
                ##############################      ###########################
                EmbossDkColor                       panelEmbossDkColor
                EmbossLtColor                       panelEmbossLtColor
                Color0,                             panelFaceColor
                    Color1,                     
                    ColorDisabled              
                fillStyle                           panelFillStyle
                EmbossSize                          panelEmbossSize      
                TextColor0,                         Not set in the panel.
                    TextColor1,                         
                    TextColorDisabled,                   
                    pFont,                
                    CommonBkColor,              
                    CommonBkLeft,               
                    CommonBkTop,                
                    CommonBkType,               
                    pCommonBkImage            
                AlphaValue,                         panelApha - use is dependent on the
                                                        panelFillStyle setting.
                gradientStartColor,                 panelGradientStartColor - use is 
                                                        dependent on the
                                                        panelFillStyle setting.
                gradientEndColor                    panelGradientEndColor - use is 
                                                        dependent on the
                                                        panelFillStyle setting.
                 \--                                panelImage - set by the object
                                                        that supports images.
            </TABLE>         
        
        The text parameters are not handled in the panel. These are handled 
        by each object that supports texts.  
        
        See the section on panel rendering for the summary of APIs that support
        panel rendering.
        

        <b> Background Feature </b>

        The Primitive Layer provides a global background information variable.
        This allows users to set a background and easily refresh parts of 
        the screen that that background occupies (see GFX_BACKGROUND for
        details). The style scheme of the objects also allows association
        of the object to a background. This feature allows the objects to 
        be easily refreshed on the screen.
 
        The requirement of the background feature is that the background 
        should encompass the object. In other words, all the pixels of 
        the object should be within the background dimension. Once this
        requirement is fulfilled, the parameters of the object that defines 
        the location of the object (left, top, right bottom) is used to 
        refresh the background pixels. Because of this Alpha blending, 
        hiding or basic refresh of the object is optimized.

        To add background information on the object the following style scheme
        parameters should be set:
        1. CommonBkLeft - The left most pixel location of the background.
        2. CommonBkTop  - The top most pixel location of the background.
        3. CommonBkType - Should be set to one of the GFX_BACKGROUND_TYPE.
        4. pCommonBkImage - If the CommonBkType is set to GFX_BACKGROUND_IMAGE,
            this should be set to the location of the image resource.

        <b> Alpha Blending </b>

        When alpha blending is enabled, the object can be alpha blended with
        the background. Objects can also be removed from the screen easily
        by re-drawing only the areas that the object occupies.
 
        <align center>  
        <img name="object_panel_style_alphablended.jpg" />
        </align>

        <b> Alpha Blending Objects</b>
 
        To implement an alpha blended object the following style scheme
        parameters should be set:
        1. fillStyle - Set to GFX_FILL_STYLE_ALPHA_COLOR.
        2. AlphaValue - Set the alpha blending value. Set to 25, 50 or 75
            when using primitive layer implementation of alpha blending.

        <b> Gradient Fills </b>
        Objects can also be rendered with gradient fill on the panels.
        However, enabling gradient fill that are alpha blended is not supported
        yet. When AlphaValue is not 0 or 100 and fillStyle set to any of the
        gradient fill types, the alpha value will be ignored and assumed
        to be 100.

        To implement an gradient filled object the following style scheme
        parameters should be set:
        1. fillStyle - Set to any of the gradient fill style
           (see GFX_FILL_STYLE for details):
            - GFX_FILL_STYLE_GRADIENT_DOWN          
            - GFX_FILL_STYLE_GRADIENT_UP         
            - GFX_FILL_STYLE_GRADIENT_RIGHT     
            - GFX_FILL_STYLE_GRADIENT_LEFT
            - GFX_FILL_STYLE_GRADIENT_DOUBLE_VER
            - GFX_FILL_STYLE_GRADIENT_DOUBLE_HOR
        2. gradientStartColor - Set the gradient fill start color.
        3. gradientEndColor - Set the gradient fill end color.

        <align center>  
        <img name="object_panel_style_gradientfill.jpg" />
        </align>
        
    Remarks:
        - Use of alpha blending on overlapping objects is not supported.
        - Alpha blended gradient fills is not supported.
        - Background feature can only be used if the background encompass
          the dimension of the object.
        - Certain objects do not support alpha blending and/or gradient fills.
          Refer to specific object documentation for details.
 
*/

/*******************************************************************************
================================================================================
        Section: Configuring the Library Documentation
================================================================================
*******************************************************************************/

//@@configuring_the_graphics_library_doc
/*
    <TITLE Configuring the Library>

    Summary: 
        This section describes how the library can be configured.
        
    Description:
        The library can be configured to be used with a specific combination
        of features. This section enumerates the available configuration options
        as well as examples of combination of configurations.
        
*/

//@@configuring_options_graphics_library_doc
/*
    <TITLE Configuration Options >
    <GROUP configuring_the_graphics_library_doc>
    
    Summary: 
        This section summarizes all the available configuration options
        of the Graphics Library.
        
    Description:
        The following configuration options are available to the 
        use the Graphics Library. 
        
*/

//@@configuration_examples_for_graphics_library_doc
/*
    <TITLE Configuration Examples >
    <GROUP configuring_the_graphics_library_doc>
    
    Summary: 
        This section shows example of configuration combination when 
        using the library.

*/
        
//@@configuration_examples_1_doc
/*
    <TITLE Example 1 >
    <GROUP configuration_examples_for_graphics_library_doc>
    
    Summary: 
        Configuration example 1.
        
    Description:
        - non-blocking is enabled on accelerated functions
        - alpha blending 
        - gradient fills
        - palette is not used
        - focus on objects that supports it is enabled
        - double buffering is disabled
        - characters size is 16 bits to support UNICODE characters 
        - fonts are sourced from two places
            - from internal flash 
            - from external memory
            - fonts from RAM is disabled
        - font that are anti-aliased is enabled  
        - font sourced from external memory is allocated 51 bytes of buffer
        - images are sourced from two places
            - from internal flash 
            - from external memory
            - fonts from RAM is disabled
        - images that are RLE encoded is enabled
        - images that are DEFLATE (for IPU module) encoded is enabled
        - images can be rendered with transparent color feature
        - touch screen support is enabled
        - keyboard support is enabled
        - malloc and free functions are used to manage memory for object layer
        <code>
            // Note: Configuration to disable the features are commented out
            //       to show the specific macros that disables them.
            
            //#define GFX_CONFIG_NONBLOCKING_DISABLE
            //#define GFX_CONFIG_ALPHABLEND_DISABLE
            //#define GFX_CONFIG_GRADIENT_DISABLE
            //#define GFX_CONFIG_FOCUS_DISABLE
            #define GFX_CONFIG_PALETTE_DISABLE
            #define GFX_CONFIG_PALETTE_EXTERNAL_DISABLE
            #define GFX_CONFIG_DOUBLE_BUFFERING_DISABLE

            #define GFX_CONFIG_FONT_CHAR_SIZE 16
            #define GFX_CONFIG_COLOR_DEPTH 16
            
            //#define GFX_CONFIG_FONT_FLASH_DISABLE  
            //#define GFX_CONFIG_FONT_EXTERNAL_DISABLE
            #define GFX_CONFIG_FONT_RAM_DISABLE      
            //#define GFX_CONFIG_FONT_ANTIALIASED_DISABLE 

            //#define GFX_CONFIG_IMAGE_FLASH_DISABLE 
            //#define GFX_CONFIG_IMAGE_EXTERNAL_DISABLE
            #define GFX_CONFIG_IMAGE_RAM_DISABLE 

            //#define GFX_CONFIG_RLE_DECODE_DISABLE
            //#define GFX_CONFIG_IPU_DECODE_DISABLE

            //#define GFX_CONFIG_TRANSPARENT_COLOR_DISABLE 
            #define GFX_EXTERNAL_FONT_RASTER_BUFFER_SIZE 51

            //#define GFX_CONFIG_USE_TOUCHSCREEN_DISABLE    
            //#define GFX_CONFIG_USE_KEYBOARD_DISABLE       

            #define GFX_malloc(size)    malloc(size)
            #define GFX_free(pObj)      free(pObj)
                
        </code>
*/
        
//@@configuration_examples_2_doc
/*
    <TITLE Example 2 >
    <GROUP configuration_examples_for_graphics_library_doc>
    
    Summary: 
        Configuration example 2.
    
    Description:
        - all primitive functions are blocking
        - the following are all disabled
            - alpha blending 
            - gradient fills
            - palette 
            - focus on objects
            - double buffering
            - anti-aliased fonts
            - RLE and DEFLATE encoded images
            - transparent color
            - touchscreen
            - keyboard
        - characters size is 8 bits 
        - images are sourced only from internal flash
        - fonts are sourced only from internal flash and since fonts are sourced
          from internal flash, external font raster buffer is not needed
        - Object layer is not used so malloc and free are never referenced
        <code>
            // Note: Configuration to disable the features are commented out
            //       to show the specific macros that disables them.
            
            #define GFX_CONFIG_NONBLOCKING_DISABLE
            #define GFX_CONFIG_ALPHABLEND_DISABLE
            #define GFX_CONFIG_GRADIENT_DISABLE
            #define GFX_CONFIG_FOCUS_DISABLE
            #define GFX_CONFIG_PALETTE_DISABLE
            #define GFX_CONFIG_PALETTE_EXTERNAL_DISABLE
            #define GFX_CONFIG_DOUBLE_BUFFERING_DISABLE

            #define GFX_CONFIG_FONT_CHAR_SIZE 8
            #define GFX_CONFIG_COLOR_DEPTH 16
            
            //#define GFX_CONFIG_FONT_FLASH_DISABLE  
            #define GFX_CONFIG_FONT_EXTERNAL_DISABLE
            #define GFX_CONFIG_FONT_RAM_DISABLE      
            #define GFX_CONFIG_FONT_ANTIALIASED_DISABLE 

            //#define GFX_CONFIG_IMAGE_FLASH_DISABLE 
            #define GFX_CONFIG_IMAGE_EXTERNAL_DISABLE
            #define GFX_CONFIG_IMAGE_RAM_DISABLE 

            #define GFX_CONFIG_RLE_DECODE_DISABLE
            #define GFX_CONFIG_IPU_DECODE_DISABLE

            #define GFX_CONFIG_TRANSPARENT_COLOR_DISABLE 
            //#define GFX_EXTERNAL_FONT_RASTER_BUFFER_SIZE 51

            #define GFX_CONFIG_USE_TOUCHSCREEN_DISABLE    
            #define GFX_CONFIG_USE_KEYBOARD_DISABLE       

            //#define GFX_malloc(size)    malloc(size)
            //#define GFX_free(pObj)      free(pObj)
                
        </code>        
        
*/


/*******************************************************************************
================================================================================
        Primitive Layer Documentation
================================================================================
*******************************************************************************/

//@@graphics_primitive_layer
/*
    <TITLE Graphics Primitive Layer>

    Summary: 
        This section describes the API and usage of the 
        Graphics Library Primitive Layer.
*/

//@@graphics_primitive_layer_api
/*
    <TITLE Graphics Primitive Layer API>
    <GROUP graphics_primitive_layer>

    Summary: 
        Graphics Library Primitive Layer Interface.
*/

//@@init_functions
/*
    <TITLE Initialization Functions>
    <GROUP graphics_primitive_layer_api>

    Summary: 
        The following API are used to initialize the layer.

*/

//@@line_functions
/*
    <TITLE Line Rendering Functions>
    <GROUP graphics_primitive_layer_api>

    Summary: 
        The following API are used to render lines.

*/

//@@polygon_functions
/*
    <TITLE Polygon Rendering Functions>
    <GROUP graphics_primitive_layer_api>

    Summary: 
        The following API are used to render polygon shapes.

*/

//@@polygon_fill_functions
/*
    <TITLE Polygon Fill Rendering Functions>
    <GROUP graphics_primitive_layer_api>

    Description: 
        The following API are used to render filled polygon shapes.

*/

//@@image_functions
/*
    <TITLE Image Rendering Functions>
    <GROUP graphics_primitive_layer_api>

    Summary: 
        The following API are used to render images.

*/

//@@text_functions
/*
    <TITLE Text Rendering Functions>
    <GROUP graphics_primitive_layer_api>

    Summary: 
        The following API are used to render strings and characters.

*/

//@@style_functions
/*
    <TITLE Rendering Style Functions>
    <GROUP graphics_primitive_layer_api>

    Summary: 
        The following API are used to set and get rendering styles.

*/

//@@color_functions
/*
    <TITLE Color Functions>
    <GROUP graphics_primitive_layer_api>

    Summary: 
        The following API are used to set and get colors.

*/

//@@palette_functions
/*
    <TITLE Palette Functions>
    <GROUP graphics_primitive_layer_api>

    Summary: 
        The following API are used to use the palette features.

*/

//@@background_functions
/*
    <TITLE Background Functions>
    <GROUP graphics_primitive_layer_api>

    Summary: 
        The following API are used to set and manipulate the background.

*/

//@@double_buffer_functions
/*
    <TITLE Double Buffering Functions>
    <GROUP graphics_primitive_layer_api>

    Summary: 
        The following API are used to manage double buffering.

*/

//@@external_resources_functions
/*
    <TITLE External Resources Functions>
    <GROUP graphics_primitive_layer_api>

    Summary: 
        The following API are used to access external resources in the Graphics
        Library.
        
*/

//@@primitive_types
/*
    <TITLE Data Types and Constants>
    <GROUP graphics_primitive_layer>

    Summary: 
        The following are the types and constants used in the 
        Graphics Primitive Layer.

*/

/*******************************************************************************
================================================================================
        Object Layer Documentation
================================================================================
*******************************************************************************/

//@@graphics_object_layer
/*
    <TITLE Graphics Object Layer>

    Summary: 
        Graphics Object Layer Interface.
*/

//@@gol_objects
/*
    <TITLE GOL Objects>
    <GROUP graphics_object_layer>

    Summary: 
        The Graphics Object Layer (GOL) contains the Advanced Graphics 
        Objects or commonly known as widgets. 
*/

//@@gol_objects_states
/*
    <TITLE GOL Object States>
    <GROUP graphics_object_layer>

    Summary: 
        Objects rendered on the display are based on their current 
        Property States and the Drawing States.

*/

//@@gol_objects_management
/*
    <TITLE GOL Object Management>
    <GROUP graphics_object_layer>

    Summary: 
        API for managing objects and list of objects. 

*/

//@@gol_objects_rendering
/*
    <TITLE GOL Object Rendering>
    <GROUP graphics_object_layer>

    Summary: 
        API to render objects in the frame buffer. 
        
*/

//@@gol_objects_panel_rendering
/*
    <TITLE GOL Object Panel Rendering>
    <GROUP graphics_object_layer>

    Summary: 
        These are the API to render panels of objects in the frame buffer.
        These functions are used internally by the object rendering
        functions. Application should not call these functions directly.
        For those application that creates their own custom objects 
        that uses panels, these function should be used to render
        the panels.
        
*/

//@@gol_objects_messages
/*
    <TITLE GOL Object Messaging>
    <GROUP graphics_object_layer>

    Summary: 
        The library provides an interface to accept messages from 
        the input devices.

*/

//@@gol_objects_style_scheme
/*
    <TITLE GOL Object Style Scheme>
    <GROUP graphics_object_layer>

    Summary: 
        API to set and manage style scheme used by objects.

*/

//@@gol_types
/*
    <TITLE Data Types and Constants>
    <GROUP graphics_object_layer>

    Summary: 
        The following are the types and constants used in the 
        Graphics Object Layer.

*/

/*******************************************************************************
// The following are declarations for Object Groups
*******************************************************************************/

//@@gol_button_object
/*
    <TITLE Button Object>
    <GROUP gol_objects>

    Summary: 
        Button is an object that emulates a press and release effect 
        when operated upon.

    Description:
        Button supports Keyboard and Touchscreen inputs, replying to their 
        events with the pre-defined actions (see GFX_GOL_ButtonActionGet()
        and GFX_GOL_ButtonActionSet() for details).

        The button object is rendered using the assigned style scheme. The 
        following figure illustrates the usage of the style schemes in the 
        button object.
        
        <align center>
        <img name="object_button_style.jpg" />
        </align>
        
        Aside from the basic color styles, the object can also be drawn with 
        gradient fills and alpha blended fills.
        
        Buttons can also be drawn with rectangular or rounded edges.

        <align center>  
        <img name="object_button_style_edges.jpg" />
        </align>  
        
        Another variation in the rendering of button object is the two-tones mode.
        
        <align center>  
        <img name="object_button_style_twotone.jpg" />
        </align>  
                
        Buttons also allows alignment on the text used. The text alignment behavior
        of the object is the same as GFX_TextStringBoxDraw().
        
        Instead of using text to name buttons, images or icons can also be used.
        Since there are two main states of the button (pressed and released state),
        the object allows usage of two different images. One is assigned to the pressed 
        state and the other assigned to the released state.

        When only one image is used, both presses and released pointers should be 
        assigned to the same image. If one state is set to NULL, then that state will
        not render any image.

        <align center>  
        <img name="object_button_style_images.jpg" />
        </align>  
        
        Another feature of the button object is to turn off the panel rendering of the 
        object. This is useful when icons are used to define the buttons. Also to 
        complete the feature set, the transparent color feature of images can also 
        be enabled. This allows applications to use images with the flat background 
        color ignored when rendering.
        
        Remarks:
        - Alpha blending of gradient fills is not supported by the object.
        
*/

//@@gol_checkbox_object
/*
    <TITLE Check Box Object>
    <GROUP gol_objects>

    Summary: 
        Check Box is an object that simulates a check box on paper. 
        Usually it is used as an option setting where the checked or
        filled state means the option is enabled and the unfilled or 
        unchecked state means the option is disabled.

    Description:
        Check Box supports Keyboard and Touchscreen inputs, replying to their 
        events with the pre-defined actions (see GFX_GOL_CheckBoxActionGet()
        and GFX_GOL_CheckBoxActionSet() for details).

        The Check Box object is rendered using the assigned style scheme. The 
        following figure illustrates the color assignments.

        <img name="object_checkbox_style.jpg" />

        When creating the object, the alignment of the text of the object
        can be formatted with the same options that GFX_TextStringBoxDraw() allows.


*/

//@@gol_digitalmeter_object
/*
    <TITLE Digital Meter Object>
    <GROUP gol_objects>

    Summary: 
        Digital Meter is an object that can be used to display a value 
        of a sampled variable. This object is ideal when fast refresh 
        of the value is needed. The object refreshes only the digits 
        that needs to change. 
        
    Description:
        Digital Meter is an object that can be used to display a value 
        of a sampled variable. This object is ideal when fast refresh 
        of the value is needed. The object refreshes only the digits 
        that needs to change. 

        Digital Meter supports only Touchscreen inputs, replying to the 
        events with the pre-defined actions (see GFX_GOL_DigitalMeterActionGet()
        for details). There is no default action set function in this object.
        Application can create specific responses to the system action in the 
        message callback function. 
        
        The DigitalMeter object is rendered using the assigned style scheme. 
        The following figure illustrates the color assignments for the 
        digital meter.        
        
        <align center>  
        <img name="object_digitalmeter_style.jpg" />
        </align>  

*/

//@@gol_editbox_object
/*
    <TITLE Edit Box Object>
    <GROUP gol_objects>

    Summary: 
        Edit Box is is an object that emulates a cell or a text area 
        that can be edited dynamically.

    Description:
        Edit Box supports only keyboard inputs, replying to the 
        events with the pre-defined actions (see GFX_GOL_EditBoxActionGet()
        and GFX_GOL_EditBoxActionSet() for details).
        
        The Edit Box object is rendered using the assigned style scheme. The 
        following figure illustrates the color assignments.      
        
        <align center>  
        <img name="object_editbox_style.jpg" />
        </align>  

        When creating the object, the alignment of the text of the object 
        can be formatted with the same options that GFX_TextStringBoxDraw() allows.

*/

//@@gol_groupbox_object
/*
    <TITLE Group Box Object>
    <GROUP gol_objects>

    Summary: 
        Group Box is an object that can be used to group objects together 
        in the screen.

    Description:
        Group Box supports only Touchscreen inputs, replying to the 
        events with the pre-defined actions (see GFX_GOL_GroupBoxActionGet()
        for details). There is no default action set function in this object.
        Application can create specific responses to the system action in the 
        message callback function. 
        
        The Group Box object is rendered using the assigned style scheme. The 
        following figure illustrates the color assignments.      
        
        <align center>  
        <img name="object_groupbox_style.jpg" />
        </align>  

*/

//@@gol_listbox_object
/*
    <TITLE List Box Object>
    <GROUP gol_objects>

    Summary: 
        List Box is an object that defines a scrollable area where items 
        are listed. User can select a single item or multiple of items.

    Description:
        List Box supports Keyboard and Touchscreen inputs, replying to their 
        events with the pre-defined actions (see GFX_GOL_ListBoxActionGet()
        and GFX_GOL_ListBoxActionSet() for details).

        The List Box object is rendered using the assigned style scheme. The 
        following figure illustrates the usage of the style schemes in the 
        object.
        
        <align center>  
        <img name="object_listbox_style.jpg" />
        </align>  

        When creating the object, the alignment of the text of the object 
        can be formatted with the same options that
        GFX_TextStringBoxDraw() allows.

*/

//@@gol_meter_object
/*
    <TITLE Meter Object>
    <GROUP gol_objects>

    Summary: 
        Meter is an object that can be used to graphically display 
        a sampled input.

    Description:
        There are three meter types that you can draw:
        1. MTR_WHOLE_TYPE
        2. MTR_HALF_TYPE
        3. MTR_QUARTER_TYPE

        Meter supports system inputs, replying to their 
        events with the pre-defined actions (see GFX_GOL_MeterActionGet()
        and GFX_GOL_MeterActionSet() for details).
        
        The Meter object is rendered using the assigned style scheme, value 
        range colors (see GFX_GOL_MeterScaleColorSet() for details) and type
        (see GFX_GOL_METER_DRAW_TYPE). The following figure illustrates the 
        assignments for a half type meter.        
        
        <align center>  
        <img name="object_meter_style.jpg" />
        </align>  

*/

//@@gol_picture_object
/*
    <TITLE Picture Control Object>
    <GROUP gol_objects>

    Summary: 
        Picture Control is an object that can be used to transform an image 
        to be an object in the screen and have control on the rendering. 
        This object can be used to create animation using a series of bitmaps.

    Description:
        Picture Control object supports Touchscreen inputs, replying to the 
        events with the pre-defined actions (see GFX_GOL_PictureActionGet()
        for details).

        The Picture object is rendered using the assigned style scheme. The 
        following figure illustrates the usage of the style schemes in the 
        object.
        
        <align center>  
        <img name="object_picture_style.jpg" />
        </align>  

*/

//@@gol_progressbar_object
/*
    <TITLE Progress Bar Object>
    <GROUP gol_objects>

    Summary: 
        Progress Bar is an object that can be used to display the progress 
        of a task such as a data download or transfer.

    Description:
        Progress Bar is an object that can be used to display the progress 
        of a task such as a data download or transfer.

        Progress Bar supports only Touchscreen inputs, replying to the 
        events with the pre-defined actions (see GFX_GOL_ProgressBarActionGet()
        for details).

        The Progress Bar object is rendered using the assigned style scheme. 
        The following figure illustrates the usage of the style schemes in 
        the object.
        
        <align center>  
        <img name="object_progressbar_style.jpg" />
        </align>  

*/

//@@gol_radiobutton_object
/*
    <TITLE Radio Button Object>
    <GROUP gol_objects>

    Summary: 
        Radio Button is an object that can be used to offer set of choices 
        to the user. Only one of the choices is selectable. Changing 
        selection automatically removes the selection on the previous option.

    Description:
        Radio Button is an object that can be used to offer set of choices 
        to the user. Only one of the choices is selectable. Changing 
        selection automatically removes the selection on the previous option.

        Radio Button supports Keyboard and Touchscreen inputs, replying to their 
        events with the pre-defined actions (see GFX_GOL_RadioButtonActionGet()
        and GFX_GOL_RadioButtonActionSet() for details).

        The Radio Button object is rendered using the assigned style scheme. 
        The following figure illustrates the usage of the style schemes in 
        the object.
        
        <align center>  
        <img name="object_radiobutton_style.jpg" />
        </align>  

        When creating the object, the alignment of the text of the object 
        can be formatted with the same options that GFX_TextStringBoxDraw() allows.

*/

//@@gol_scrollbar_object
/*
    <TITLE Scroll Bar Object>
    <GROUP gol_objects>

    Summary: 
        Scroll Bar is an object that can be used to display a value or 
        scrolling location in a predefined area. 

    Description:
        Scroll Bar is an object that can be used to display a value or 
        scrolling location in a predefined area. 

        Scroll Bar supports Keyboard and Touchscreen inputs, replying to their 
        events with the pre-defined actions (see GFX_GOL_ScrollBarActionGet()
        and GFX_GOL_ScrollBarActionSet() for details).

        The Scroll Bar object is rendered using the assigned style scheme. 
        The following figure illustrates the usage of the style schemes in 
        the object.
        
        <align center>  
        <img name="object_scrollbar_style.jpg" />
        </align>  

*/

//@@gol_statictext_object
/*
    <TITLE Static Text Object>
    <GROUP gol_objects>

    Summary: 
        Static Text is an object that can be used to display a single 
        or multi-line string of text in a defined area.

    Description:
        Static Text is an object that can be used to display a single 
        or multi-line string of text in a rectangular area defined by 
        the dimension of the object. The area defined will also serve
        as the writable region, where any pixels that exceeds the area's
        dimension will be clipped.

        Static Text supports Touchscreen inputs only, replying to the 
        events with the pre-defined actions (see GFX_GOL_StaticTextActionGet()
        for details).

        The Static Text object is rendered using the assigned style scheme. 
        The following figure illustrates the usage of the style schemes in 
        the object.
        
        <align center>  
        <img name="object_statictext_style.jpg" />
        </align>  

        Static Text can be rendered with an option to have a frame that will
        show the boundaries of the defined region (see GFX_GOL_STATICTEXT_FRAME_STATE
        for details). 
        
        Another option for the object is to render the object with a background.
        The types of background that can be used is described in the style scheme
        used in the object (see GFX_BACKGROUND_TYPE for types of available background).
        
        When creating the object, the alignment of the text of the object 
        can be formatted with the same options that GFX_TextStringBoxDraw() allows.

*/

//@@gol_textentry_object
/*
    <TITLE Text Entry Object>
    <GROUP gol_objects>

    Summary: 
        Text Entry is an object that can be used to emulate a key pad entry with 
        a display area for the entered characters. 
        
    Description:
        Text Entry is an object that can be used to emulate a key pad entry with 
        a display area for the entered characters. The object has a feature where 
        you can define a key to reply with a translated message that signifies a 
        command key was pressed. A command key example can be your enter or 
        carriage return key or an escape key. Multiple keys can be assigned command 
        keys. Application can utilize the command key to define the behavior of the 
        program based on a command key press.    
    
        Static Text supports Touchscreen inputs only, replying to the 
        events with the pre-defined actions (see GFX_GOL_TextEntryActionGet() 
        and GFX_GOL_TextEntryActionSet() for details).

        The Text Entry object is rendered using the assigned style scheme. The 
        following figure illustrates the color assignments. 
        
        <align center>  
        <img name="object_textentry_style.jpg" />
        </align>  

        When creating the object, the alignment of the text of the object 
        can be formatted with the same options that GFX_TextStringBoxDraw() allows.

*/

//@@gol_window_object
/*
    <TITLE Window Object>
    <GROUP gol_objects>

    Summary: 
        Window is an object that can be used to encapsulate objects into a group. 
        
    Description:
        Window is an object that can be used to encapsulate objects into a group. 
        Unlike the Group Box object, the Window object has additional features such 
        as displaying an icon or a small bitmap on its Title Bar. It also has 
        additional controls for both Title Bar and Client Area.
    
        Window supports Touchscreen inputs only, replying to the 
        events with the pre-defined actions (see GFX_GOL_WindowActionGet() 
        for details).

        The Window object is rendered using the assigned style scheme. The 
        following figure illustrates the color assignments. 
        
        <align center>  
        <img name="object_window_style.jpg" />
        </align>  

        When creating the object, the alignment of the text of the object 
        in the title area can be formatted with the same options that 
        GFX_TextStringBoxDraw() allows.

*/

/*******************************************************************************
================================================================================
        Driver Layer Documentation
================================================================================
*******************************************************************************/

//@@graphics_driver_layer
/*
    <TITLE Graphics Driver Layer>

    Summary:
        Graphics Driver Layer Interface.
*/

//@@graphics_driver_layer_api
/*
    <TITLE Graphics Driver Layer API>
    <GROUP graphics_driver_layer>

    Summary:
        Graphics Library Driver Layer Interface.
*/

//@@driver_layer_types
/*
    <TITLE Data Types and Constants>
    <GROUP graphics_driver_layer>

    Summary:
        The following are the types and constants used in the
        Graphics Driver Layer.

*/

//@@driver_init_functions
/*
    <TITLE Driver Layer Initialization Functions>
    <GROUP graphics_driver_layer_api>

    Summary:
        The following API are used to initialize the layer.

*/

//@@driver_config_functions
/*
    <TITLE Driver Layer Configuration Functions>
    <GROUP graphics_driver_layer_api>

    Summary:
        The following API are used to set the configuration of the layer.

*/

//@@driver_rendering_functions
/*
    <TITLE Driver Layer Rendering Functions>
    <GROUP graphics_driver_layer_api>

    Summary:
        The following API are the basic driver layer rendering functions.

*/

//@@driver_hardware_functions
/*
    <TITLE Driver Layer Hardware Functions>
    <GROUP graphics_driver_layer_api>

    Summary:
        The following API are used for hardware settings of the display module.

*/
