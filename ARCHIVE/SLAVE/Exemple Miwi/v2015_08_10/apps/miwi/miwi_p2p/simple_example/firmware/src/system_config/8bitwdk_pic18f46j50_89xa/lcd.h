/********************************************************************
 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the "Company") for its PIC(R) Microcontroller is intended and
 supplied to you, the Company's customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *******************************************************************/
#ifndef __LCDBLOCKING_H
#define __LCDBLOCKING_H

#include "system_config.h"

extern uint8_t LCDText[16*2+1];
/*********************************************************************
* Function:         void LCD_Initialize(void)
*
* PreCondition:     none
*
* Input:	    none
*
* Output:	    none
*
* Side Effects:	    LCD is configured
*
* Overview:         Configure the LCD
*
* Note:             None
********************************************************************/
void LCD_Initialize(void);

/*********************************************************************
* Function:         void LCD_Update(void)
*
* PreCondition:     none
*
* Input:	    none
*
* Output:	    none
*
* Side Effects:	    LCD is updated with message
*
* Overview:         LCD displays message
*
* Note:             None
********************************************************************/
void LCD_Update(void);

/*********************************************************************
* Function:         void LCD_Erase(void)
*
* PreCondition:     none
*
* Input:	    none
*
* Output:	    none
*
* Side Effects:	    LCD display cleared
*
* Overview:         Clears LCD display
*
* Note:             None
********************************************************************/
void LCD_Erase(void);

/*********************************************************************
* Function: void LCDDisplay( char * message, uint8_t value, bool delay  )
*
* Overview: Displays the message on the LCD
*
* PreCondition: None
*
* Input:  message, variable if needed and delay
*
* Output: Prints the message on the LCD
*
********************************************************************/
void LCDDisplay(char * message, uint8_t value, bool delay);

/*********************************************************************
* Function:        void LCDTRXCount(uint8_t txCount, uint8_t rxCount)
*
* PreCondition:    LCD has been initialized
*
* Input:           txCount - the total number of transmitted messages
*                  rxCount - the total number of received messages
*
* Output:          None
*
* Side Effects:    None
*
* Overview:        This function display the total numbers of TX and
*                  RX messages on the LCD, if applicable.
*
* Note:            
********************************************************************/
void LCDTRXCount(uint8_t, uint8_t);

#if defined(SENSOR_PORT_UART) && defined(SENSOR_PORT_LCD)
    /*********************************************************************
    * Function:         void I2CTask(void)
    *
    * PreCondition:     none
    *
    * Input:            none
    *
    * Output:           none
    *
    * Side Effects:	none
    *
    * Overview:         Used when I2C is used for writing to LCD
    *
    * Note:             None
    ********************************************************************/
        void I2CTask(void);

     /*********************************************************************
    * Function:         void I2CWrite(uint8_t Commandlen, uint8_t BufferLen)
    *
    * PreCondition:     none
    *
    * Input:            none
    *
    * Output:           none
    *
    * Side Effects:	none
    *
    * Overview:         Writes to I2C
    *
    * Note:             None
    ********************************************************************/
        void I2CWrite(uint8_t Commandlen, uint8_t BufferLen);
#endif
#if defined(ENABLE_MANUALBACKLIGHT)
    /*********************************************************************
    * Function:         void LCDBacklightON(void)
    *
    * PreCondition:     LCD Initialization
    *
    * Input:            none
    *
    * Output:           none
    *
    * Side Effects:	LCD backlight is turned ON
    *
    * Overview:         LCD backlight is turned ON to highlight the dislpay
    *                   message
    *
    * Note:             None
    ********************************************************************/
    void LCDBacklightON(void);

    /*********************************************************************
    * Function:         void LCDBacklightOFF(void)
    *
    * PreCondition:     LCD Initialization
    *
    * Input:            none
    *
    * Output:           none
    *
    * Side Effects:	LCD backlight is turned off
    *
    * Overview:         LCD backight is turned off to save power
    *
    * Note:             None
    ********************************************************************/
    void LCDBacklightOFF(void);

#endif

#endif
