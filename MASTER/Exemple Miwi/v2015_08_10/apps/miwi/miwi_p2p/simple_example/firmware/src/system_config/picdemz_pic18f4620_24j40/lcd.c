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

#define __LCDBLOCKING_C

#include "system.h"

uint8_t LCDText[16*2+1];

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
void LCD_Initialize(void)
{

}

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
void LCD_Update(void)
{

}

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
void LCD_Erase(void)
{
    
}

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
 * Note:            This routine is only effective if Explorer16 or
 *                  PIC18 Explorer demo boards are used
 ********************************************************************/
void LCDTRXCount(uint8_t txCount, uint8_t rxCount)
{

}



/*********************************************************************
 * Function:        void LCDDisplay(char *text, uint8_t value, bool delay)
 *
 * PreCondition:    LCD has been initialized
 *
 * Input:           text - text message to be displayed on LCD
 *                  value - the text message allows up to one byte
 *                          of variable in the message
 *                  delay - whether need to display the message for
 *                          2 second without change
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function display the text message on the LCD,
 *                  including up to one uint8_t variable, if applicable.
 *
 * Note:            This routine is only effective if Explorer16 or
 *                  PIC18 Explorer demo boards are used
 ********************************************************************/
void LCDDisplay(char *text, uint8_t value, bool delay)
{

}

void UserInterruptHandler(void)
{

}
