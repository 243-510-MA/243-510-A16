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

#include "system.h"
#include "system_config.h"


//Configuration bits for PIC18F87J11
#pragma config FOSC = HSPLL
#pragma config WDTEN = OFF
#pragma config WDTPS = 128
#pragma config XINST = OFF

//Configuration bits for PIC16
/*
 #pragma config FOSC = HS
 #pragma config WDTE = OFF
 #pragma config PWRTE = OFF
 #pragma config MCLRE = ON
 #pragma config CP = OFF
 #pragma config CPD = OFF
 #pragma config BOREN = SBODEN
 #pragma config CLKOUTEN = OFF
 #pragma config IESO = OFF
 #pragma config FCMEN = OFF


 */


/*********************************************************************
 * Function:        void SYSTEM_Initialize( void )
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    Board is initialized for P2P usage
 *
 * Overview:        This function configures the board
 *
 * Note:            This routine needs to be called before the function
 *                  to initialize P2P stack or any other function that
 *                  operates on the stack
 ********************************************************************/
void SYSTEM_Initialize(void)
{

    // primary external oscillator
    OSCCON = 0x70;
    //OSCTUNEbits.PLLEN = 1;

    // set up the analogue port
    
    WDTCONbits.ADSHR = 1;
    ANCON0 = 0b11011111;        // all digital pin
    ANCON1 = 0xFF;
    WDTCONbits.ADSHR = 0;
    

    TRISA = 0xFF;
    TRISB = 0xFF;
    TRISC = 0xFF;
    TRISD = 0x00;

    LATA = 0;
    LATB = 0;
    LATC = 0;
    LATD = 0;

    // set I/O ports
    BUTTON_1_TRIS = 1;
    BUTTON_2_TRIS = 1;
    LED_1_TRIS = 0;
    LED_2_TRIS = 0;

    
    PHY_CS_TRIS = 0;
    PHY_CS = 1;
    PHY_RESETn_TRIS = 0;
    PHY_RESETn = 1;
    RF_INT_TRIS = 1;

    SDI_TRIS = 1;
    SDO_TRIS = 0;
    SCK_TRIS = 0;

    #if defined(HARDWARE_SPI)

        SSP1STAT = 0xC0;
        SSP1CON1 = 0x21;

    #else

        SPI_SDO = 0;
        SPI_SCK = 0;

    #endif
        
    INTCON2bits.INTEDG3 = 0;
        
    #if defined(ENABLE_NVM)

        EE_nCS_TRIS = 0;
        EE_nCS = 1;

    #endif

    INTCONbits.GIEH = 1;     
    RFIF = 0;
    RFIE = 1;

    LCD_Initialize();


}




void UserInterruptHandler(void)
{

}








