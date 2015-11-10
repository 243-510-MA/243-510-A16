/*******************************************************************************
 Simple 4-Wire Resistive Touch Screen Driver for the Microchip Graphics Library

  Company:
    Microchip Technology Inc.

  File Name:
    drv_touch_resistive_adc.c

  Summary:
    This module implements the driver for the 4-simple wire resistive
    touch screen.
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

#include "system.h"

#include <stdint.h>
#include "gfx/gfx.h"
#include "driver/touch_screen/drv_touch_screen.h"
#include "driver/touch_screen/drv_touch_resistive_adc.h"

// Error checking
#ifndef RESISTIVETOUCH_ADPCFG_YPOS
#error "The macro RESISTIVETOUCH_ADPCFG_YPOS is not defined (analog port pins (ANSx or ADxPCFG)). Define in system_config.h"
#endif
#ifndef RESISTIVETOUCH_ADPCFG_XPOS
#error "The macro RESISTIVETOUCH_ADPCFG_XPOS is not defined (analog port pins (ANSx or ADxPCFG)). Define in system_config.h"
#endif

#ifndef RESISTIVETOUCH_XP_Signal
#error "The macro RESISTIVETOUCH_XP_Signal is not defined (LATx bit). Define in system_config.h"
#endif
#ifndef RESISTIVETOUCH_XM_Signal
#error "The macro RESISTIVETOUCH_XM_Signal is not defined (LATx bit). Define in system_config.h"
#endif
#ifndef RESISTIVETOUCH_XP_Direction
#error "The macro RESISTIVETOUCH_XP_Direction is not defined (TRISx bit). Define in system_config.h"
#endif
#ifndef RESISTIVETOUCH_XM_Direction
#error "The macro RESISTIVETOUCH_XM_Direction is not defined (TRISx bit). Define in system_config.h"
#endif

#ifndef RESISTIVETOUCH_YP_Signal
#error "The macro RESISTIVETOUCH_YP_Signal is not defined (LATx bit). Define in system_config.h"
#endif
#ifndef RESISTIVETOUCH_YM_Signal
#error "The macro RESISTIVETOUCH_YM_Signal is not defined (LATx bit). Define in system_config.h"
#endif
#ifndef RESISTIVETOUCH_YP_Direction
#error "The macro RESISTIVETOUCH_YP_Direction is not defined (TRISx bit). Define in system_config.h"
#endif
#ifndef RESISTIVETOUCH_YM_Direction
#error "The macro RESISTIVETOUCH_YM_Direction is not defined (TRISx bit). Define in system_config.h"
#endif

#ifndef RESISTIVETOUCH_ADC_XPOS
#error "The macro RESISTIVETOUCH_ADC_XPOS is not defined (analog signal channel). Define in system_config.h"
#endif
#ifndef RESISTIVETOUCH_ADC_YPOS
#error "The macro RESISTIVETOUCH_ADC_YPOS is not defined (analog signal channel). Define in system_config.h"
#endif

#ifndef RESISTIVETOUCH_ADC_INPUT_SEL
#error "The macro RESISTIVETOUCH_ADC_INPUT_SEL is not defined (analog signal input select register (ADxCHS). Define in system_config.h"
#endif
#ifndef RESISTIVETOUCH_ADC_START
#error "The macro RESISTIVETOUCH_ADC_START is not defined (A/D start bit). Define in system_config.h"
#endif
#ifndef RESISTIVETOUCH_ADC_DONE
#error "The macro RESISTIVETOUCH_ADC_DONE is not defined (A/D done bit). Define in system_config.h"
#endif

// Checking for deprecated macros

#ifdef TOUCHSCREEN_RESISTIVE_SWAP_XY
#error TOUCHSCREEN_RESISTIVE_SWAP_XY is deprecated, use RESISTIVETOUCH_SWAP_XY instead.
#endif
#ifdef TOUCHSCREEN_RESISTIVE_FLIP_Y
#error TOUCHSCREEN_RESISTIVE_FLIP_Y is deprecated, use RESISTIVETOUCH_FLIP_Y instead.
#endif
#ifdef TOUCHSCREEN_RESISTIVE_FLIP_X
#error TOUCHSCREEN_RESISTIVE_FLIP_X is deprecated, use RESISTIVETOUCH_FLIP_X instead.
#endif

#ifdef TOUCH_ADC_INPUT_SEL
#error TOUCH_ADC_INPUT_SEL is deprecated, use RESISTIVETOUCH_ADC_INPUT_SEL instead.
#endif
#ifdef TOUCH_ADC_START
#error TOUCH_ADC_START is deprecated, use RESISTIVETOUCH_ADC_START instead.
#endif
#ifdef TOUCH_ADC_DONE
#error TOUCH_ADC_DONE is deprecated, use RESISTIVETOUCH_ADC_DONE instead.
#endif

#ifdef ADC_POT_PCFG
#error ADC_POT_PCFG potentiometer measurement support is removed in this driver.
#endif
#ifdef ADPCFG_XPOS
#error ADPCFG_XPOS is deprecated, use RESISTIVETOUCH_ADC_XPOS instead.
#endif
#ifdef ADPCFG_YPOS
#error ADPCFG_YPOS is deprecated, use RESISTIVETOUCH_ADC_YPOS instead.
#endif

// The following are moved to be defined internally to this driver.
// Any definitions, will be undefined and replaced with the one set here.
#ifdef ResistiveTouchScreen_XPlus_Drive_High
#warning No need to define this macro:ResistiveTouchScreen_XPlus_Drive_High(). 
#undef ResistiveTouchScreen_XPlus_Drive_High
#endif
#ifdef ResistiveTouchScreen_XPlus_Drive_Low
#warning No need to define this macro:ResistiveTouchScreen_XPlus_Drive_Low(). 
#undef ResistiveTouchScreen_XPlus_Drive_Low
#endif
#ifdef ResistiveTouchScreen_XPlus_Config_As_Input
#warning No need to define this macro:ResistiveTouchScreen_XPlus_Config_As_Input(). 
#undef ResistiveTouchScreen_XPlus_Config_As_Input
#endif
#ifdef ResistiveTouchScreen_XPlus_Config_As_Output
#warning No need to define this macro:ResistiveTouchScreen_XPlus_Config_As_Output(). 
#undef ResistiveTouchScreen_XPlus_Config_As_Output
#endif

#ifdef ResistiveTouchScreen_XMinus_Drive_High
#warning No need to define this macro:ResistiveTouchScreen_XMinus_Drive_High(). 
#undef ResistiveTouchScreen_XMinus_Drive_High
#endif
#ifdef ResistiveTouchScreen_XMinus_Drive_Low
#warning No need to define this macro:ResistiveTouchScreen_XMinus_Drive_Low(). 
#undef ResistiveTouchScreen_XMinus_Drive_Low
#endif
#ifdef ResistiveTouchScreen_XMinus_Config_As_Input
#warning No need to define this macro:ResistiveTouchScreen_XMinus_Config_As_Input(). 
#undef ResistiveTouchScreen_XMinus_Config_As_Input
#endif
#ifdef ResistiveTouchScreen_XMinus_Config_As_Output
#warning No need to define this macro:ResistiveTouchScreen_XMinus_Config_As_Output(). 
#undef ResistiveTouchScreen_XMinus_Config_As_Output
#endif

#ifdef ResistiveTouchScreen_YPlus_Drive_High
#warning No need to define this macro:ResistiveTouchScreen_YPlus_Drive_High(). 
#undef ResistiveTouchScreen_YPlus_Drive_High
#endif
#ifdef ResistiveTouchScreen_YPlus_Drive_Low
#warning No need to define this macro:ResistiveTouchScreen_YPlus_Drive_Low(). 
#undef ResistiveTouchScreen_YPlus_Drive_Low
#endif
#ifdef ResistiveTouchScreen_YPlus_Config_As_Input
#warning No need to define this macro:ResistiveTouchScreen_YPlus_Config_As_Input(). 
#undef ResistiveTouchScreen_YPlus_Config_As_Input
#endif
#ifdef ResistiveTouchScreen_YPlus_Config_As_Output
#warning No need to define this macro:ResistiveTouchScreen_YPlus_Config_As_Output(). 
#undef ResistiveTouchScreen_YPlus_Config_As_Output
#endif

#ifdef ResistiveTouchScreen_YMinus_Drive_High
#warning No need to define this macro:ResistiveTouchScreen_YMinus_Drive_High(). 
#undef ResistiveTouchScreen_YMinus_Drive_High
#endif
#ifdef ResistiveTouchScreen_YMinus_Drive_Low
#warning No need to define this macro:ResistiveTouchScreen_YMinus_Drive_Low(). 
#undef ResistiveTouchScreen_YMinus_Drive_Low
#endif
#ifdef ResistiveTouchScreen_YMinus_Config_As_Input
#warning No need to define this macro:ResistiveTouchScreen_YMinus_Config_As_Input(). 
#undef ResistiveTouchScreen_YMinus_Config_As_Input
#endif
#ifdef ResistiveTouchScreen_YMinus_Config_As_Output
#warning No need to define this macro:ResistiveTouchScreen_YMinus_Config_As_Output(). 
#undef ResistiveTouchScreen_YMinus_Config_As_Output
#endif


// Default Calibration Inset Value (percentage of vertical or horizontal resolution)
// Calibration Inset = ( CALIBRATIONINSET / 2 ) % , Range of 0 to 20% with 0.5% resolution
// Example with CALIBRATIONINSET == 20, the calibration points are measured
// 10% from the corners.
#ifndef CALIBRATIONINSET
    #define CALIBRATIONINSET   20       // range 0 <= CALIBRATIONINSET <= 40 
#endif

#define CAL_X_INSET    (((GFX_MaxXGet()+1)*(CALIBRATIONINSET>>1))/100)
#define CAL_Y_INSET    (((GFX_MaxYGet()+1)*(CALIBRATIONINSET>>1))/100)
#define SAMPLE_POINTS   4

//////////////////////// Resistive Touch Driver Version ////////////////////////////
// The first four bits is the calibration inset, next 8 bits is assigned the version 
// number and 0xF is assigned to this 4-wire resistive driver.
const uint16_t mchpTouchScreenVersion = 0xF110 | CALIBRATIONINSET;

//////////////////////// A/D Sampling Mode ///////////////////////
// first some error check
#if defined (RESISTIVETOUCH_MANUAL_SAMPLE_MODE) &&  defined (RESISTIVETOUCH_AUTO_SAMPLE_MODE)
    #error Cannot have two resistive touch modes enabled.
#endif
#ifndef RESISTIVETOUCH_MANUAL_SAMPLE_MODE
    // enable auto sampling mode
    #define RESISTIVETOUCH_AUTO_SAMPLE_MODE
    // else manual sampling mode is enabled 
#endif

//////////////////////// GUI Color Assignments ///////////////////////
// Set the colors used in the calibration screen, defined by 
// system.h, system_config.h, gfx_config.h or gfx_colors.h
#if (GFX_CONFIG_COLOR_DEPTH == 1)
    #define RESISTIVETOUCH_FOREGROUNDCOLOR BLACK	   
    #define RESISTIVETOUCH_BACKGROUNDCOLOR WHITE	   
#elif (GFX_CONFIG_COLOR_DEPTH == 4)
    #define RESISTIVETOUCH_FOREGROUNDCOLOR BLACK	   
    #define RESISTIVETOUCH_BACKGROUNDCOLOR WHITE	   
#elif (GFX_CONFIG_COLOR_DEPTH == 8) || (GFX_CONFIG_COLOR_DEPTH == 16) || (GFX_CONFIG_COLOR_DEPTH == 24)
    #define RESISTIVETOUCH_FOREGROUNDCOLOR BRIGHTRED	   
    #define RESISTIVETOUCH_BACKGROUNDCOLOR WHITE	   
#endif

#ifndef RESISTIVETOUCH_ANALOG
    #define RESISTIVETOUCH_ANALOG  0
#endif
#ifndef RESISTIVETOUCH_DIGITAL
    #define RESISTIVETOUCH_DIGITAL 1 
#endif

// X port definitions
#define ResistiveTouchScreen_XPlus_Drive_High()         RESISTIVETOUCH_XP_Signal    = 1
#define ResistiveTouchScreen_XPlus_Drive_Low()          RESISTIVETOUCH_XP_Signal    = 0
#define ResistiveTouchScreen_XPlus_Config_As_Input()    RESISTIVETOUCH_XP_Direction = 1
#define ResistiveTouchScreen_XPlus_Config_As_Output()   RESISTIVETOUCH_XP_Direction = 0

#define ResistiveTouchScreen_XMinus_Drive_High()        RESISTIVETOUCH_XM_Signal    = 1
#define ResistiveTouchScreen_XMinus_Drive_Low()         RESISTIVETOUCH_XM_Signal    = 0
#define ResistiveTouchScreen_XMinus_Config_As_Input()   RESISTIVETOUCH_XM_Direction = 1
#define ResistiveTouchScreen_XMinus_Config_As_Output()  RESISTIVETOUCH_XM_Direction = 0

// Y port definitions
#define ResistiveTouchScreen_YPlus_Drive_High()         RESISTIVETOUCH_YP_Signal    = 1
#define ResistiveTouchScreen_YPlus_Drive_Low()          RESISTIVETOUCH_YP_Signal    = 0
#define ResistiveTouchScreen_YPlus_Config_As_Input()    RESISTIVETOUCH_YP_Direction = 1
#define ResistiveTouchScreen_YPlus_Config_As_Output()   RESISTIVETOUCH_YP_Direction = 0

#define ResistiveTouchScreen_YMinus_Drive_High()        RESISTIVETOUCH_YM_Signal    = 1
#define ResistiveTouchScreen_YMinus_Drive_Low()         RESISTIVETOUCH_YM_Signal    = 0
#define ResistiveTouchScreen_YMinus_Config_As_Input()   RESISTIVETOUCH_YM_Direction = 1
#define ResistiveTouchScreen_YMinus_Config_As_Output()  RESISTIVETOUCH_YM_Direction = 0


//////////////////////// LOCAL PROTOTYPES ////////////////////////////
void    TouchGetCalPoints(void);
void 	TouchStoreCalibration(void);
void 	TouchCheckForCalibration(void);
void 	TouchLoadCalibration(void);
void    TouchCalculateCalPoints(void);

#ifdef ENABLE_DEBUG_TOUCHSCREEN
void    TouchScreenResistiveTestXY(void);
#endif

extern NVM_READ_FUNC           pCalDataRead;                // function pointer to data read
extern NVM_WRITE_FUNC          pCalDataWrite;               // function pointer to data write
extern NVM_SECTORERASE_FUNC    pCalDataSectorErase;         // function pointer to data sector erase

//////////////////////// GLOBAL VARIABLES ////////////////////////////
#ifndef RESISTIVETOUCH_PRESS_THRESHOLD
    // you may define the threshold with a value, define the new value in the 
    // system_config.h
    #define RESISTIVETOUCH_PRESS_THRESHOLD     256   // between 0-0x03ff the lesser this value
                                                     // the lighter the screen must be pressed
#endif
#define CALIBRATION_DELAY   300                      // delay between calibration touch points

// Current ADC values for X and Y channels
volatile int16_t  adcX   = -1;
volatile int16_t  adcY   = -1;
volatile int16_t  adcPot =  0;

// coefficient values
volatile long   _trA;
volatile long   _trB;
volatile long   _trC;
volatile long   _trD;

// copy of the stored or sampled raw points (this is the calibration data stored)
/*      xRawTouch[0] - x sample from upper left corner; 
        xRawTouch[1] - x sample from upper right corner
        xRawTouch[2] - x sample from lower right corner
        xRawTouch[3] - x sample from lower left corner
        yRawTouch[0] - y sample from upper left corner; 
        yRawTouch[1] - y sample from upper right corner
        yRawTouch[2] - y sample from lower right corner
        yRawTouch[3] - y sample from lower left corner
*/
volatile int16_t  xRawTouch[SAMPLE_POINTS] = {TOUCHCAL_ULX, TOUCHCAL_URX, TOUCHCAL_LRX, TOUCHCAL_LLX};   
volatile int16_t  yRawTouch[SAMPLE_POINTS] = {TOUCHCAL_ULY, TOUCHCAL_URY, TOUCHCAL_LRY, TOUCHCAL_LLY};   

// WARNING: Watch out when selecting the value of SCALE_FACTOR 
// since a big value will overflow the signed int type 
// and the multiplication will yield incorrect values.
#ifndef RESISTIVETOUCH_CALIBRATION_SCALE_FACTOR
    // default scale factor is 256
    #define RESISTIVETOUCH_CALIBRATION_SCALE_FACTOR 8
#endif

// use this scale factor to avoid working in floating point numbers
#define SCALE_FACTOR (1<<RESISTIVETOUCH_CALIBRATION_SCALE_FACTOR)

typedef enum
{
    IDLE,
    SET_X,
    RUN_X,
    GET_X,
    RUN_CHECK_X,
    CHECK_X,
    SET_Y,
    RUN_Y,
    GET_Y,
    CHECK_Y,
    SET_VALUES,
} TOUCH_STATES;

volatile TOUCH_STATES state = IDLE;

/*********************************************************************
* Function: void TouchDetectPosition(void)
********************************************************************/
int16_t TouchDetectPosition(void)
{
    static int16_t    tempX, tempY;
    int16_t           temp;

    switch(state)
    {
        case IDLE:
            adcX = -1;
            adcY = -1;
            break;

        case SET_VALUES:
#ifdef RESISTIVETOUCH_MANUAL_SAMPLE_MODE
            TOUCH_ADC_START = 0; // stop sampling
#endif
            if (!RESISTIVETOUCH_ADC_DONE)
                break;

            if ((uint16_t) RESISTIVETOUCH_PRESS_THRESHOLD < (uint16_t) ADC1BUF0)
            {
                adcX = -1;
                adcY = -1;
            }
            else
            {
                adcX = tempX;
                adcY = tempY;
            }

            state = SET_X;
            return 1; // touch screen acquisition is done

        case SET_X:

            RESISTIVETOUCH_ADC_INPUT_SEL = RESISTIVETOUCH_ADC_XPOS;

            ResistiveTouchScreen_XPlus_Config_As_Input();
            ResistiveTouchScreen_YPlus_Config_As_Input();
            ResistiveTouchScreen_XMinus_Config_As_Input();
            ResistiveTouchScreen_YMinus_Drive_Low();
            ResistiveTouchScreen_YMinus_Config_As_Output();

            RESISTIVETOUCH_ADPCFG_YPOS = RESISTIVETOUCH_DIGITAL; // set to digital pin
            RESISTIVETOUCH_ADPCFG_XPOS = RESISTIVETOUCH_ANALOG; // set to analog pin

            RESISTIVETOUCH_ADC_START = 1; // run conversion
            state = CHECK_X;
            break;

        case CHECK_X:
        case CHECK_Y:
#ifdef RESISTIVETOUCH_MANUAL_SAMPLE_MODE
            TOUCH_ADC_START = 0; // stop sampling
#endif
            if (RESISTIVETOUCH_ADC_DONE == 0)
            {
                break;
            }

            if ((uint16_t) RESISTIVETOUCH_PRESS_THRESHOLD > (uint16_t) ADC1BUF0)
            {
                if (state == CHECK_X)
                {
                    ResistiveTouchScreen_YPlus_Drive_High();
                    ResistiveTouchScreen_YPlus_Config_As_Output();
                    tempX = -1;
                    state = RUN_X;
                }
                else
                {
                    ResistiveTouchScreen_XPlus_Drive_High();
                    ResistiveTouchScreen_XPlus_Config_As_Output();
                    tempY = -1;
                    state = RUN_Y;
                }
            }
            else
            {
                adcX = -1;
                adcY = -1;
                state = SET_X;
                return 1; // touch screen acquisition is done
            }

        case RUN_X:
        case RUN_Y:
            RESISTIVETOUCH_ADC_START = 1;
            state = (state == RUN_X) ? GET_X : GET_Y;
            // no break needed here since the next state is either GET_X or GET_Y
            break;

        case GET_X:
        case GET_Y:
#ifdef RESISTIVETOUCH_MANUAL_SAMPLE_MODE
            TOUCH_ADC_START = 0; // stop sampling
#endif
            if (!RESISTIVETOUCH_ADC_DONE)
                break;

            temp = ADC1BUF0;
            if (state == GET_X)
            {
                if (temp != tempX)
                {
                    tempX = temp;
                    state = RUN_X;
                    break;
                }
            }
            else
            {
                if (temp != tempY)
                {
                    tempY = temp;
                    state = RUN_Y;
                    break;
                }
            }

            if (state == GET_X)
                ResistiveTouchScreen_YPlus_Config_As_Input();
            else
                ResistiveTouchScreen_XPlus_Config_As_Input();
            RESISTIVETOUCH_ADC_START = 1;
            state = (state == GET_X) ? SET_Y : SET_VALUES;
            break;

        case SET_Y:
#ifdef RESISTIVETOUCH_MANUAL_SAMPLE_MODE
            TOUCH_ADC_START = 0; // stop sampling
#endif
            if (!RESISTIVETOUCH_ADC_DONE)
                break;

            if ((uint16_t) RESISTIVETOUCH_PRESS_THRESHOLD < (uint16_t) ADC1BUF0)
            {
                adcX = -1;
                adcY = -1;

                state = SET_X;
                return 1; // touch screen acquisition is done
            }

            RESISTIVETOUCH_ADC_INPUT_SEL = RESISTIVETOUCH_ADC_YPOS;

            ResistiveTouchScreen_XPlus_Config_As_Input();
            ResistiveTouchScreen_YPlus_Config_As_Input();
            ResistiveTouchScreen_XMinus_Drive_Low();
            ResistiveTouchScreen_XMinus_Config_As_Output();
            ResistiveTouchScreen_YMinus_Config_As_Input();

            RESISTIVETOUCH_ADPCFG_YPOS = RESISTIVETOUCH_ANALOG; // set to analog pin
            RESISTIVETOUCH_ADPCFG_XPOS = RESISTIVETOUCH_DIGITAL; // set to digital pin

            RESISTIVETOUCH_ADC_START = 1; // run conversion

            state = CHECK_Y;
            break;

        default:
            state = SET_X;
            return 1; // touch screen acquisition is done
    }

    return 0;                       // touch screen acquisition is not done
}

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
* Overview: Initializes the A/D channel used for the touch detection.
*
* Note: none
*
********************************************************************/
void Touch_ADCInit(void)
{

#if defined (RESISTIVETOUCH_AUTO_SAMPLE_MODE)
    // Initialize ADC for auto sampling mode
    AD1CON1 = 0;                // reset
    AD1CON2 = 0;                // AVdd, AVss, int every conversion, MUXA only

    AD1CON3bits.SAMC = 0x1F;    // 31 Tad auto-sample
    AD1CON3bits.ADCS = 0xFF;    // Tad = 256*Tcy

    AD1CON1bits.SSRC = 0b111;   // use auto-convert
    AD1CON1bits.ADON = 1;       // enable A/D


#elif defined (RESISTIVETOUCH_MANUAL_SAMPLE_MODE)
    // Initialize ADC for manual sampling mode
    AD1CON1 = 0;                // reset
    AD1CON2 = 0;                // AVdd, AVss, int every conversion, MUXA only

    AD1CON3bits.SAMC = 0x01;    // 1 Tad auto-sample
    AD1CON3bits.ADCS = 0x01;    // Tad = 2*Tcy

    AD1CON1bits.SSRC = 0;       // Clearing SAMP bit ends sampling and starts conversion
    AD1CON1bits.ADON = 1;       // enable A/D

#else
    #error Resistive Touch sampling mode is not set.
#endif

}

/*********************************************************************
* Function: void TouchHardwareInit(void)
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: Initializes touch screen module.
*
* Note: none
*
********************************************************************/
void TouchHardwareInit(void *initValues)
{
    Touch_ADCInit();

    // set the used D/A port to be analog
    RESISTIVETOUCH_ADPCFG_XPOS = RESISTIVETOUCH_ANALOG;
    RESISTIVETOUCH_ADPCFG_YPOS = RESISTIVETOUCH_ANALOG;
    
    AD1CSSL = 0;            // No scanned inputs
    state = SET_X;          // set the state of the statemachine to start the sampling

}

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
int16_t TouchGetX(void)
{
    long    result;

    result = TouchGetRawX();

    if(result >= 0)
    {
        result = (long)((((long)_trC*result) + _trD)>>RESISTIVETOUCH_CALIBRATION_SCALE_FACTOR);
#ifdef RESISTIVETOUCH_FLIP_X
	result = GFX_MaxXGet() - result;
#endif	
    }
    return ((int16_t)result);
}

/*********************************************************************
* Function: int16_t TouchGetRawX()
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
int16_t TouchGetRawX(void)
{
#ifdef RESISTIVETOUCH_SWAP_XY
    return adcY;
#else
    return adcX;
#endif
}
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
int16_t TouchGetY(void)
{

    long    result;

    result = TouchGetRawY();
    
    if(result >= 0)
    {
        result = (long)((((long)_trA*result) + (long)_trB)>>RESISTIVETOUCH_CALIBRATION_SCALE_FACTOR);

#ifdef RESISTIVETOUCH_FLIP_Y
        result = GFX_MaxYGet() - result;
#endif	
	}
    return ((int16_t)result);
}

/*********************************************************************
* Function: int16_t TouchGetRawY()
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
int16_t TouchGetRawY(void)
{
#ifdef RESISTIVETOUCH_SWAP_XY
    return adcX;
#else
    return adcY;
#endif
}

/*********************************************************************
* Function: void TouchStoreCalibration(void)
*
* PreCondition: Non-volatile memory initialization function must be called before
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: stores calibration parameters into non-volatile memory
*
* Note: none
*
********************************************************************/
void TouchStoreCalibration(void)
{
#ifdef ENABLE_DEBUG_TOUCHSCREEN
    int16_t temp[9];
#endif

    if (pCalDataWrite != NULL)
    {
        // the upper left X sample address is used since it is the first one
        // and this assumes that all stored values are located in one
        // sector
        if (pCalDataSectorErase != NULL)
            pCalDataSectorErase(ADDRESS_RESISTIVE_TOUCH_ULX);

        pCalDataWrite(ADDRESS_RESISTIVE_TOUCH_ULX, (uint8_t*)&xRawTouch[0], 2);
        pCalDataWrite(ADDRESS_RESISTIVE_TOUCH_ULY, (uint8_t*)&yRawTouch[0], 2);

        pCalDataWrite(ADDRESS_RESISTIVE_TOUCH_URX, (uint8_t*)&xRawTouch[1], 2);
        pCalDataWrite(ADDRESS_RESISTIVE_TOUCH_URY, (uint8_t*)&yRawTouch[1], 2);

        pCalDataWrite(ADDRESS_RESISTIVE_TOUCH_LLX, (uint8_t*)&xRawTouch[3], 2);
        pCalDataWrite(ADDRESS_RESISTIVE_TOUCH_LLY, (uint8_t*)&yRawTouch[3], 2);

        pCalDataWrite(ADDRESS_RESISTIVE_TOUCH_LRX, (uint8_t*)&xRawTouch[2], 2);
        pCalDataWrite(ADDRESS_RESISTIVE_TOUCH_LRY, (uint8_t*)&yRawTouch[2], 2);

        pCalDataWrite(ADDRESS_RESISTIVE_TOUCH_VERSION, (uint8_t*)&mchpTouchScreenVersion, 2);

    }

#ifdef ENABLE_DEBUG_TOUCHSCREEN
    // check if programming was a success
    // this will check if the calibration data was written properly

    pCalDataRead(ADDRESS_RESISTIVE_TOUCH_ULX, (uint8_t*)&temp[0], 2);
    pCalDataRead(ADDRESS_RESISTIVE_TOUCH_ULY, (uint8_t*)&temp[1], 2);
    pCalDataRead(ADDRESS_RESISTIVE_TOUCH_URX, (uint8_t*)&temp[2], 2);
    pCalDataRead(ADDRESS_RESISTIVE_TOUCH_URY, (uint8_t*)&temp[3], 2);

    pCalDataRead(ADDRESS_RESISTIVE_TOUCH_LLX, (uint8_t*)&temp[6], 2);
    pCalDataRead(ADDRESS_RESISTIVE_TOUCH_LLY, (uint8_t*)&temp[7], 2);
    pCalDataRead(ADDRESS_RESISTIVE_TOUCH_LRX, (uint8_t*)&temp[4], 2);
    pCalDataRead(ADDRESS_RESISTIVE_TOUCH_LRY, (uint8_t*)&temp[5], 2);

    pCalDataRead(ADDRESS_RESISTIVE_TOUCH_VERSION, (uint8_t*)&temp[8], 2);

    if (
            (temp[0] != xRawTouch[0]) ||
            (temp[1] != yRawTouch[0]) ||
            (temp[2] != xRawTouch[1]) ||
            (temp[3] != yRawTouch[1]) ||
            (temp[4] != xRawTouch[2]) ||
            (temp[5] != yRawTouch[2]) ||
            (temp[6] != xRawTouch[3]) ||
            (temp[7] != yRawTouch[3]) ||
            (temp[8] != mchpTouchScreenVersion)
       )
    {
        while(1);
    }

#endif


}

/*********************************************************************
* Function: void TouchLoadCalibration(void)
*
* PreCondition: Non-volatile memory initialization function must be called before
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: loads calibration parameters from non-volatile memory
*
* Note: none
*
********************************************************************/
void TouchLoadCalibration(void)
{
    if (pCalDataRead != NULL)
    {

        pCalDataRead(ADDRESS_RESISTIVE_TOUCH_ULX, (uint8_t*)&xRawTouch[0], 2);
    	pCalDataRead(ADDRESS_RESISTIVE_TOUCH_ULY, (uint8_t*)&yRawTouch[0], 2);

    	pCalDataRead(ADDRESS_RESISTIVE_TOUCH_URX, (uint8_t*)&xRawTouch[1], 2);
    	pCalDataRead(ADDRESS_RESISTIVE_TOUCH_URY, (uint8_t*)&yRawTouch[1], 2);

    	pCalDataRead(ADDRESS_RESISTIVE_TOUCH_LLX, (uint8_t*)&xRawTouch[3], 2);
    	pCalDataRead(ADDRESS_RESISTIVE_TOUCH_LLY, (uint8_t*)&yRawTouch[3], 2);

    	pCalDataRead(ADDRESS_RESISTIVE_TOUCH_LRX, (uint8_t*)&xRawTouch[2], 2);
    	pCalDataRead(ADDRESS_RESISTIVE_TOUCH_LRY, (uint8_t*)&yRawTouch[2], 2);

    }

    TouchCalculateCalPoints();
    
}

/*********************************************************************
* Function: void TouchGetCalPoints(void)
*
* PreCondition: InitGraph() must be called before
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: gets values for 3 touches
*
* Note: none
*
********************************************************************/
void TouchCalculateCalPoints(void)
{
    long trA, trB, trC, trD;                    // variables for the coefficients
    long trAhold, trBhold, trChold, trDhold;
    long test1, test2;                          // temp variables (must be signed type)

    int16_t    xPoint[SAMPLE_POINTS], yPoint[SAMPLE_POINTS];

    yPoint[0] = yPoint[1] = CAL_Y_INSET; 
    yPoint[2] = yPoint[3] = (GFX_MaxYGet() - CAL_Y_INSET);
    xPoint[0] = xPoint[3] = CAL_X_INSET;
    xPoint[1] = xPoint[2] = (GFX_MaxXGet() - CAL_X_INSET);

    // calculate points transfer functiona
    // based on two simultaneous equations solve for the
    // constants

    // use sample points 1 and 4
    // Dy1 = aTy1 + b; Dy4 = aTy4 + b
    // Dx1 = cTx1 + d; Dy4 = aTy4 + b

    test1 = (long)yPoint[0] - (long)yPoint[3];
    test2 = (long)yRawTouch[0] - (long)yRawTouch[3];

    trA = ((long)((long)test1 * SCALE_FACTOR) / test2);
    trB = ((long)((long)yPoint[0] * SCALE_FACTOR) - (trA * (long)yRawTouch[0]));

    test1 = (long)xPoint[0] - (long)xPoint[2];
    test2 = (long)xRawTouch[0] - (long)xRawTouch[2];

    trC = ((long)((long)test1 * SCALE_FACTOR) / test2);
    trD = ((long)((long)xPoint[0] * SCALE_FACTOR) - (trC * (long)xRawTouch[0]));

    trAhold = trA;
    trBhold = trB;
    trChold = trC;
    trDhold = trD;

    // use sample points 2 and 3
    // Dy2 = aTy2 + b; Dy3 = aTy3 + b
    // Dx2 = cTx2 + d; Dy3 = aTy3 + b

    test1 = (long)yPoint[1] - (long)yPoint[2];
    test2 = (long)yRawTouch[1] - (long)yRawTouch[2];

    trA = ((long)(test1 * SCALE_FACTOR) / test2);
    trB = ((long)((long)yPoint[1] * SCALE_FACTOR) - (trA * (long)yRawTouch[1]));

    test1 = (long)xPoint[1] - (long)xPoint[3];
    test2 = (long)xRawTouch[1] - (long)xRawTouch[3];

    trC = ((long)((long)test1 * SCALE_FACTOR) / test2);
    trD = ((long)((long)xPoint[1] * SCALE_FACTOR) - (trC * (long)xRawTouch[1]));

    // get the average and use the average
    _trA = (trA + trAhold) >> 1;
    _trB = (trB + trBhold) >> 1;
    _trC = (trC + trChold) >> 1;
    _trD = (trD + trDhold) >> 1;

}

/*********************************************************************
* Function: void TouchGetCalPoints(void)
*
* PreCondition: InitGraph() must be called before
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: gets values for 3 touches
*
* Note: none
*
********************************************************************/
void TouchCalHWGetPoints(void)
{
    #define TOUCH_DIAMETER	10
    #define SAMPLE_POINTS   4

    GFX_XCHAR   calStr1[] = {'o','n',' ','t','h','e',' ','f','i','l','l','e','d',0};
    GFX_XCHAR   calStr2[] = {'c','i','r','c','l','e',0};
    GFX_XCHAR  	calTouchPress[] = {'P','r','e','s','s',' ','&',' ','R','e','l','e','a','s','e',0};

    GFX_XCHAR   calRetryPress[] = {'R','e','t','r','y',0};
    GFX_XCHAR   *pMsgPointer;
    int16_t   counter;

    uint16_t    dx[SAMPLE_POINTS], dy[SAMPLE_POINTS];
    uint16_t    textHeight, msgX, msgY;
    int16_t   tempX, tempY;

    GFX_FontSet((void *) &DRV_TOUCHSCREEN_FONT);
    GFX_ColorSet(RESISTIVETOUCH_FOREGROUNDCOLOR);

    textHeight = GFX_TextStringHeightGet((void *) &DRV_TOUCHSCREEN_FONT);

    while
    (
        !GFX_TextStringDraw
            (
                (GFX_MaxXGet() - GFX_TextStringWidthGet((GFX_XCHAR *)calStr1, (void *) &DRV_TOUCHSCREEN_FONT)) >> 1,
                (GFX_MaxYGet() >> 1),
                (GFX_XCHAR *)calStr1,
                0
            )
    );

    while
    (
        !GFX_TextStringDraw
            (
                (GFX_MaxXGet() - GFX_TextStringWidthGet((GFX_XCHAR *)calStr2, (void *) &DRV_TOUCHSCREEN_FONT)) >> 1,
                ((GFX_MaxYGet() >> 1) + textHeight),
                (GFX_XCHAR *)calStr2,
                0
            )
    );

    // calculate center points (locate them at 15% away from the corners)
    // draw the four touch points

    dy[0] = dy[1] = CAL_Y_INSET;
    dy[2] = dy[3] = (GFX_MaxYGet() - CAL_Y_INSET);
    dx[0] = dx[3] = CAL_X_INSET;
    dx[1] = dx[2] = (GFX_MaxXGet() - CAL_X_INSET);


    msgY = ((GFX_MaxYGet() >> 1) - textHeight);
    pMsgPointer = calTouchPress;
	
    // get the four samples or calibration points
    for(counter = 0; counter < SAMPLE_POINTS;)
    {
    
        // redraw the filled circle to unfilled (previous calibration point)
        if (counter > 0)
        {
            GFX_FillStyleSet(GFX_FILL_STYLE_COLOR);
            GFX_ColorSet(RESISTIVETOUCH_BACKGROUNDCOLOR);
	    while(!(GFX_CircleFillDraw(dx[counter-1], dy[counter-1], TOUCH_DIAMETER-3)));
        }

	// draw the new filled circle (new calibration point)
        GFX_ColorSet(RESISTIVETOUCH_FOREGROUNDCOLOR);
	while(!(GFX_CircleDraw(dx[counter], dy[counter], TOUCH_DIAMETER)));
	while(!(GFX_CircleFillDraw(dx[counter], dy[counter], TOUCH_DIAMETER-3)));

	// show points left message
        msgX = (GFX_MaxXGet() - GFX_TextStringWidthGet((GFX_XCHAR *)pMsgPointer, (void *) &DRV_TOUCHSCREEN_FONT)) >> 1;
	TouchShowMessage(pMsgPointer, RESISTIVETOUCH_FOREGROUNDCOLOR, msgX, msgY);

        // Wait for press
        do {} 
    	    while((TouchGetRawX() == -1) && (TouchGetRawY() == -1));

        tempX = TouchGetRawX();
        tempY = TouchGetRawY();

        // wait for release
        do {}
            while((TouchGetRawX() != -1) && (TouchGetRawY() != -1));

        // check if the touch was detected properly
        if ((tempX == -1) || (tempY == -1))
        {
            // cannot proceed retry the touch, display RETRY PRESS message
            // remove the previous string
	    TouchShowMessage(pMsgPointer, RESISTIVETOUCH_BACKGROUNDCOLOR, msgX, msgY);
            pMsgPointer = calRetryPress;
            // show the retry message
            msgX = (GFX_MaxXGet() - GFX_TextStringWidthGet((GFX_XCHAR *)pMsgPointer, (void *) &DRV_TOUCHSCREEN_FONT)) >> 1;
	    TouchShowMessage(pMsgPointer, RESISTIVETOUCH_FOREGROUNDCOLOR, msgX, msgY);
        }
        else
        {    

            // remove the previous string
	    TouchShowMessage(pMsgPointer, RESISTIVETOUCH_BACKGROUNDCOLOR, msgX, msgY);
            pMsgPointer = calTouchPress;
#ifdef RESISTIVETOUCH_FLIP_Y
            yRawTouch[3 - counter] = tempY; 
#else
            yRawTouch[counter] = tempY; 
#endif

#ifdef RESISTIVETOUCH_FLIP_X
            xRawTouch[3 - counter] = tempX; 
#else
            xRawTouch[counter] = tempX; 
#endif
            counter++;

        }

        // Wait for release
        do {} 
        while((TouchGetRawX() != -1) && (TouchGetRawY() != -1));

        __delay_ms(CALIBRATION_DELAY);
      
    }

    TouchCalculateCalPoints();
 
#ifdef ENABLE_DEBUG_TOUCHSCREEN
    TouchScreenResistiveTestXY();
#endif
}


/*********************************************************************
* Function: void TouchScreenResistiveTestXY(void)
*
* PreCondition: TouchHardwareInit has been called
*
* Input: none
*
* Output: none
*
* Side Effects: none
*
* Overview: prints raw x,y calibrated x,y and calibration factors to screen
*
* Note: modify pre-processor macro to include/exclude this test code
*       a common place to call this from is at the end of TouchCalHWGetPoints()
*
********************************************************************/
#ifdef ENABLE_DEBUG_TOUCHSCREEN
#include <stdio.h>
void TouchScreenResistiveTestXY(void)
{
    #define BUFFCHARLEN 60
    char buffChar[BUFFCHARLEN];
#if (GFX_CONFIG_FONT_CHAR_SIZE == 16)
    uint16_t buffCharW[BUFFCHARLEN];
    unsigned char i;
#endif
    int16_t tempXX, tempYY,tempXX2,tempYY2, calXX, calYY;
    tempXX = tempYY = -1;
    tempXX2 = tempYY2 = 0;

    // store the last calibration
    TouchStoreCalibration();

    while(1)
    {
       
        // use this to always show the values even if not pressing the screen
        tempXX = TouchGetRawX();
        tempYY = TouchGetRawY();
        
        calXX = TouchGetX();
        calYY = TouchGetY();
        
        if((tempXX != tempXX2)||(tempYY != tempYY2))
        {
            GFX_ColorSet(RESISTIVETOUCH_BACKGROUNDCOLOR);
            GFX_ScreenClear();
            GFX_ColorSet(RESISTIVETOUCH_FOREGROUNDCOLOR);
            sprintf(    buffChar,
                        "raw_x=%d, raw_y=%d",
                        (uint16_t)tempXX,
                        (uint16_t)tempYY);
			
#if (GFX_CONFIG_FONT_CHAR_SIZE == 16)

            for(i = 0; i < BUFFCHARLEN;i++)
            {
                buffCharW[i] = buffChar[i];
            }
            while(  GFX_TextStringDraw( 0, 0, (GFX_XCHAR*)buffCharW, 0) !=
                    GFX_STATUS_SUCCESS);
#else
            while(  GFX_TextStringDraw( 0, 0, (GFX_XCHAR*)buffChar, 0) !=
                    GFX_STATUS_SUCCESS);
#endif
            
            sprintf(    buffChar,
                        "cal_x=%d, cal_y=%d",
                        (uint16_t)calXX,
                        (uint16_t)calYY);

#if (GFX_CONFIG_FONT_CHAR_SIZE == 16)

            for(i = 0; i < BUFFCHARLEN;i++)
            {
                buffCharW[i] = buffChar[i];
            }
            while(  GFX_TextStringDraw( 0, 40, (GFX_XCHAR*)buffCharW, 0) !=
                    GFX_STATUS_SUCCESS);
#else
            while(  GFX_TextStringDraw( 0, 40, (GFX_XCHAR*)buffChar, 0) !=
                    GFX_STATUS_SUCCESS);
#endif
            
            sprintf(    buffChar,
                        "_tr:A=%d,B=%d",
                        (uint16_t)_trA,
                        (uint16_t)_trB);

#if (GFX_CONFIG_FONT_CHAR_SIZE == 16)

            for(i = 0; i < BUFFCHARLEN;i++)
            {
                buffCharW[i] = buffChar[i];
            }
            while(  GFX_TextStringDraw( 0, 80, (GFX_XCHAR*)buffCharW, 0) !=
                    GFX_STATUS_SUCCESS);
#else
            while(  GFX_TextStringDraw( 0, 80, (GFX_XCHAR*)buffChar, 0) !=
                    GFX_STATUS_SUCCESS);
#endif

            sprintf(    buffChar,
                        "_tr:C=%d,D=%d",
                        (uint16_t)_trC,
                        (uint16_t)_trD);

#if (GFX_CONFIG_FONT_CHAR_SIZE == 16)

            for(i = 0; i < BUFFCHARLEN;i++)
            {
                buffCharW[i] = buffChar[i];
            }
            while(  GFX_TextStringDraw( 0, 100, (GFX_XCHAR*)buffCharW, 0) !=
                    GFX_STATUS_SUCCESS);
#else
            while(  GFX_TextStringDraw( 0, 100, (GFX_XCHAR*)buffChar, 0) !=
                    GFX_STATUS_SUCCESS);
#endif

        }
        
        tempXX2=tempXX;
        tempYY2=tempYY;

        do{tempXX = TouchGetRawX(); tempYY = TouchGetRawY();}
    	while((tempXX == -1) && (tempYY == -1));   
    }
}
#endif //#ifdef ENABLE_DEBUG_TOUCHSCREEN


