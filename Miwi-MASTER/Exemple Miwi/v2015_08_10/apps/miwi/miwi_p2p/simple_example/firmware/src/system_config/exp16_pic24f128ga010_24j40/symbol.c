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

/************************ HEADERS **********************************/

#include "system.h"


/************************ VARIABLES ********************************/

volatile uint8_t timerExtension1,timerExtension2;

/************************ FUNCTIONS ********************************/

/*********************************************************************
* Function:         void InitSymbolTimer()
*
* PreCondition:     none
*
* Input:            none
*
* Output:           none
*
* Side Effects:	    
*
* Overview:         This function will configure the UART for use at
*                   in 8 bits, 1 stop, no flowcontrol mode
*
* Note:			
********************************************************************/
void InitSymbolTimer()
{
    T2CON = 0b0000000000001000 | CLOCK_DIVIDER_SETTING;
    T2CONbits.TON = 1;

}


/*********************************************************************
* Function:         void InitSymbolTimer()
*
* PreCondition:     none
*
* Input:            none
*
* Output:           MIWI_TICK - the current symbol time
*
* Side Effects:	    
*
* Overview:         This function returns the current time
*
* Note:
********************************************************************/
MIWI_TICK MiWi_TickGet(void)
{
    MIWI_TICK currentTime;
    
    currentTime.word.w1 = TMR3;
    currentTime.word.w0 = TMR2;
    if( currentTime.word.w1 != TMR3 )
   {
       currentTime.word.w1 = TMR3;
       currentTime.word.w0 = TMR2;
    }

    return currentTime;
}
