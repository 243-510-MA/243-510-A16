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

#if defined(ENABLE_CONSOLE)


/************************ VARIABLES ********************************/
unsigned char CharacterArray[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

//DEFINITIONS



//FUNCTIONS

/*********************************************************************
* Function:         void CONSOLE_Initialize(void)
*
* PreCondition:     none
*
* Input:            none
*
* Output:           none
*
* Side Effects:	    UART is configured
*
* Overview:         This function will configure the UART for use at
*                   in 8 bits, 1 stop, no flowcontrol mode
*
* Note:             None
********************************************************************/
void CONSOLE_Initialize(void)
{

    UART_SPBRG   = (SYS_CLK_FrequencySystemGet()/2/16)/BAUD_RATE-1;
    UART_IF = 0;
    UART_STA  = 0;
    UART_MODE = 0b0000000010000000;

    MODE_UARTEN= 1;
    STA_UTXEN = 1;

}

/*********************************************************************
* Function:         void CONSOLE_PutString(char* str)
*
* PreCondition:     none
*
* Input:            str - string that needs to be printed
*
* Output:           none
*
* Side Effects:	    str is printed to the console
*
* Overview:         This function will print the inputed ROM string
*
* Note:             Do not power down the microcontroller until
*                   the transmission is complete or the last
*                   transmission of the string can be corrupted.
********************************************************************/
void CONSOLE_PutString(char* str)
{
    uint8_t c;

    while( c = *str++ )
        CONSOLE_Put(c);
}

/*********************************************************************
* Function:         void CONSOLE_Put(uint8_t c)
*
* PreCondition:     none
*
* Input:            c - character to be printed
*
* Output:           none
*
* Side Effects:	    c is printed to the console
*
* Overview:         This function will print the inputed character
*
* Note:             Do not power down the microcontroller until
*                   the transmission is complete or the last
*                   transmission of the string can be corrupted.
********************************************************************/
void CONSOLE_Put(uint8_t c)
{
    while( !CONSOLE_IsPutReady() );
        TREG = c;

}


/*********************************************************************
* Function:         uint8_t CONSOLE_Get(void)
*
* PreCondition:     none
*
* Input:            none
*
* Output:           one byte received by UART
*
* Side Effects:	    none
*
* Overview:         This function will receive one byte from UART
*
* Note:             Do not power down the microcontroller until
*                   the transmission is complete or the last
*                   transmission of the string can be corrupted.
********************************************************************/
uint8_t CONSOLE_Get(void)
{
    char temp;
    while(UART_IF == 0);
    temp = RREG;
    UART_IF = 0;
    return temp;
}


/*********************************************************************
* Function:         void CONSOLE_PrintHex(uint8_t toPrint)
*
* PreCondition:     none
*
* Input:            toPrint - character to be printed
*
* Output:           none
*
* Side Effects:	    toPrint is printed to the console
*
* Overview:         This function will print the inputed char to
*                   the console in hexidecimal form
*
* Note:             Do not power down the microcontroller until
*                   the transmission is complete or the last
*                   transmission of the string can be corrupted.
********************************************************************/
void CONSOLE_PrintHex(uint8_t toPrint)
{
    uint8_t PRINT_VAR;
    PRINT_VAR = toPrint;
    toPrint = (toPrint>>4)&0x0F;
    CONSOLE_Put(CharacterArray[toPrint]);
    toPrint = (PRINT_VAR)&0x0F;
    CONSOLE_Put(CharacterArray[toPrint]);
    return;
}

/*********************************************************************
* Function:         void CONSOLE_PrintDec(uint8_t toPrint)
*
* PreCondition:     none
*
* Input:            toPrint - character to be printed. Range is 0-99
*
* Output:           none
*
* Side Effects:	    toPrint is printed to the console in decimal
*
*
* Overview:         This function will print the inputed char to
*                   the console in decimal form
*
* Note:             Do not power down the microcontroller until
*                   the transmission is complete or the last
*                   transmission of the string can be corrupted.
********************************************************************/
void CONSOLE_PrintDec(uint8_t toPrint)
{
    if( toPrint >= 100 )
        CONSOLE_Put(CharacterArray[toPrint/100]);
    if( toPrint >= 10 )
        CONSOLE_Put(CharacterArray[(toPrint%100)/10]);
    CONSOLE_Put(CharacterArray[toPrint%10]);
}




#endif  //ENABLE_CONSOLE

