/*******************************************************************************
 Resistive Touch Screen Driver for Microchip Graphics Library

  Company:
    Microchip Technology Inc.

  File Name:
    drv_touch_screen.h

  Summary:
    Header file for the touch screen driver.
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

#ifndef _DRV_TOUCH_SCREEN_H
#define _DRV_TOUCH_SCREEN_H

#include <stdint.h>
#include "gfx/gfx.h"

typedef void    (*NVM_READ_FUNC       )(uint32_t, uint8_t*, uint16_t); // typedef for read function pointer
typedef uint8_t (*NVM_WRITE_FUNC      )(uint32_t, uint8_t*, uint16_t); // typedef for write function pointer
typedef void    (*NVM_SECTORERASE_FUNC)(uint32_t);                     // typedef for sector erase function pointer

/*********************************************************************
* Function: int16_t TouchDetectPosition(void)
*
* PreCondition: none
*
* Input: none
*
* Output: Returns 1 if touch sampling is done.
*         Returns 0 if the touch sampling is not finished.  
*
* Side Effects: none
*
* Overview: Process the detection of touch
*
* Note: none
*
********************************************************************/
int16_t TouchDetectPosition(void);

/*********************************************************************
* Function: void TouchInit(NVM_WRITE_FUNC pWriteFunc, NVM_READ_FUNC pReadFunc, NVM_SECTORERASE_FUNC pSectorErase, void *initValues)
*
* PreCondition: none
*
* Input: pWriteFunc - non-volatile memory write function pointer
*        pReadFunc - non-volatile memory read function pointer
*        pSectorErase - non-volatile memory sector function pointer
*
* Output: none
*
* Side Effects: none
*
* Overview: Initializes the touch screen hardware.
*
* Note: none
*
********************************************************************/
void TouchInit(NVM_WRITE_FUNC pWriteFunc, NVM_READ_FUNC pReadFunc, NVM_SECTORERASE_FUNC pSectorErase, void *initValues);

/*********************************************************************
* Function: int16_t TouchGetX()
*
* PreCondition: none
*
* Input: none
*
* Output: x coordinate
*
* Side Effects: none
*
* Overview: returns x coordinate if touch screen is pressed
*           and -1 if not
*
* Note: none
*
********************************************************************/
int16_t TouchGetX(void);
/*********************************************************************
* Function: int16_t TouchGetRawX()
*
* PreCondition: none
*
* Input: none
*
* Output: x raw value
*
* Side Effects: none
*
* Overview: returns x coordinate if touch screen is pressed
*           and -1 if not
*
* Note: none
*
********************************************************************/
int16_t TouchGetRawX(void);

/*********************************************************************
* Function: int16_t TouchGetY()
*
* PreCondition: none
*
* Input: none
*
* Output: y coordinate
*
* Side Effects: none
*
* Overview: returns y coordinate if touch screen is pressed
*           and -1 if not
*
* Note: none
*
********************************************************************/
int16_t TouchGetY(void);

/*********************************************************************
* Function: int16_t TouchGetRawY()
*
* PreCondition: none
*
* Input: none
*
* Output: raw y value
*
* Side Effects: none
*
* Overview: returns y coordinate if touch screen is pressed
*           and -1 if not
*
* Note: none
*
********************************************************************/
int16_t TouchGetRawY(void);

/*********************************************************************
* Function: void TouchGetMsg(GFX_GOL_MESSAGE* pMsg)
*
* PreCondition: none
*
* Input: pointer to the message structure to be populated
*
* Output: none
*
* Side Effects: none
*
* Overview: populates GOL message structure
*
* Note: none
*
********************************************************************/
void TouchGetMsg(GFX_GOL_MESSAGE *pMsg);

/*********************************************************************
* Function: void TouchCalibration()
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: Runs the calibration routine. 
*
* Note: none
*
********************************************************************/
void TouchCalibration(void);


/*********************************************************************
* Function: void Touch_ADCInit(void)
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: Initializes the ADC for the touch detection.
*
* Note: none
*
********************************************************************/
void Touch_ADCInit(void);


// macro to draw repeating text
#define TouchShowMessage(pStr, color, x, y)             \
                {                                       \
                    GFX_ColorSet(color);                    \
                    while(!GFX_TextStringDraw(x,y,pStr,0));        \
                }	

#endif //_DRV_TOUCH_SCREEN_H
