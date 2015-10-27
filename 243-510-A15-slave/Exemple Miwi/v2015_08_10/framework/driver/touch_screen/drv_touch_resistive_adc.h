/*******************************************************************************
 Simple 4-Wire Resistive Touch Screen Driver for the Microchip Graphics Library

  Company:
    Microchip Technology Inc.

  File Name:
    drv_touch_resistive_adc.h

  Summary:
    Header file for the 4-wire resistive touch screen driver.
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



#ifndef _DRV_TOUCH_RESISTIVE_ADC_H
#define _DRV_TOUCH_RESISTIVE_ADC_H


// Default calibration points
#define TOUCHCAL_ULX 0x2A
#define TOUCHCAL_ULY 0xE67E
#define TOUCHCAL_URX 0x2A
#define TOUCHCAL_URY 0xE67E
#define TOUCHCAL_LLX 0x2A
#define TOUCHCAL_LLY 0xE67E
#define TOUCHCAL_LRX 0x2A
#define TOUCHCAL_LRY 0xE67E


// use this macro to debug the touch screen panel 
// this will enable the use of debugging functions in the C file.
// It assumes that the graphics portion is working.
//#define ENABLE_DEBUG_TOUCHSCREEN


/*
 Guide in using the following macro:

 #define RESISTIVETOUCH_SWAP_XY

 The placement of the resistive touch panel with respect to the
 orientation of the screen will affect the resistive touch screen
 logic.

 The direction of the x and y values must be aligned to the logic
 of the resistive touch calculation.
 The following description shows the cases where the macros shown
 above are used to make the resistive touch calculation logic
 be consistent with the pixel directions with respect to the
 screen orientation:

  For the figure below:
  pixel value x increases to the right
  pixel value y increases going down
  In other words (0,0) is located at the left top position
  In other words (maxX,maxY) is located at the bottom right position

                 x --->
               ------------
  y = 0       | A        B |
              |            |
              |            |
              |            |
              |            |
              |            |
  y = max y   | D        C |
               ------------

 When sampling the touch position in the x or y direction:
 For x-direction
 If the voltage sampled at point A = point B or 
 if the voltage sampled at point C = point D 
 or there is a small variation in samples.
 The x-direction does not change in resistance value therefore:
    define the macro RESISTIVETOUCH_SWAP_XY

 OR
  
 For y-direction
 If the voltage sampled at point B = point C or
 if the voltage sampled at point A = point D
 or there is a small variation in samples.
 The y-direction does not change in resistance value therefore:
    define the macro RESISTIVETOUCH_SWAP_XY

 The swapping is needed to reverse the direction of the sampling 
 to move in the other direction. 
 
 Enabled the ENABLE_DEBUG_TOUCHSCREEN macro to see the raw values
 the calculated x,y position of the touch. When calibration
 is enabled, the debug screen for the touch will appear.


 */


#endif //_DRV_TOUCH_RESISTIVE_ADC_H
