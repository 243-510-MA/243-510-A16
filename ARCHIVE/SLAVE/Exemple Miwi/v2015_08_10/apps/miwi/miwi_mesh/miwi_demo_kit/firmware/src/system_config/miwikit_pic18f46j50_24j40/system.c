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

// Config Bit Settings to get 16 MHz: Internal 8 MHz / 2 = 4 * 12 = 48 / 3 = 16
#pragma config OSC = INTOSCPLL, WDTEN = OFF, XINST = OFF, WDTPS = 2048, PLLDIV = 2, CPUDIV = OSC3_PLL3



/*********************************************************************
 * Function:        void BoardInit( void )
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
 *                  to initialize stack or any other function that
 *                  operates on the stack
 ********************************************************************/
void SYSTEM_Initialize(void)
{
    /*******************************************************************/
    // Primary Internal Oscillator
    /*******************************************************************/
    OSCCON = 0x78;
    OSCTUNEbits.PLLEN = 1;
    WDTCONbits.SWDTEN = 0;

    /*******************************************************************/
    // Configure PPS Related Pins
    /*******************************************************************/
    // Unlock to config PPS
    EECON2 = 0x55;
    EECON2 = 0xAA;
    PPSCONbits.IOLOCK = 0;
    
    RPINR1 = 17;            // Mapping IRQ1 to RC6(RP17) * Used for MRF89XA
    RPINR21 = 23;			// Mapping SDI2 to RD6(RP23)
    RPOR21 = 10;			// Mapping SCK2 to RD4(RP21)
    RPOR19 = 9;			    // Mapping SDO2 to RD2(RP19)
	
	// Lock System
    EECON2 = 0x55;
    EECON2 = 0xAA;
    PPSCONbits.IOLOCK = 1;

    /*******************************************************************/
    // AN0 & AN1 Analog Pins others Digital Pins
    /*******************************************************************/
    ANCON0 = 0xFC;
    ANCON1 = 0x1F;
    
    /*******************************************************************/
    // Configure Switch and LED I/O Ports
    /*******************************************************************/
    LED0 = 0;
    LED1 = 0;
    LED2 = 0;
    LED0_TRIS = 0;
    LED1_TRIS = 0;
    LED2_TRIS = 0;
            
    SW1_TRIS = 1;
    SW2_TRIS = 1;      
								
    INTCON2bits.RBPU = 0;   // Enable PORTB Pull-ups for Switches

    /*******************************************************************/
    // Configure the Temp Sensor and VBat port
    /*******************************************************************/	
    TRISAbits.TRISA1 = 1;
    TRISAbits.TRISA0 = 1;
    
    /*******************************************************************/
    // Config RF Radio
    /*******************************************************************/
    /*******************************************************************/
    // Config MRF24J40 Pins
    /*******************************************************************/
    PHY_CS = 1;
    PHY_RESETn = 1;    
    PHY_WAKE = 1;
    
    PHY_CS_TRIS = 0;
    PHY_RESETn_TRIS = 0;        
    PHY_WAKE_TRIS = 0;
    RF_INT_TRIS = 1;
             
    // Config INT0 Edge = Falling
    INTCON2bits.INTEDG0 = 0;
    
    RFIF = 0;
    RFIE = 1;
    
    /*******************************************************************/
    // Confiure SPI1
    /*******************************************************************/     
    SDI_TRIS = 1;
    SDO_TRIS = 0;
    SCK_TRIS = 0;
    
    SSP1STAT = 0xC0;
    SSP1CON1 = 0x20;
   

    
    /*******************************************************************/    
    // Configure EEProm Pins
    /*******************************************************************/
    RF_EEnCS = 1;
    RF_EEnCS_TRIS = 0;
    EE_nCS = 1;
    EE_nCS_TRIS = 0;

    
    /*******************************************************************/
    // Configure LCD Pins
    /*******************************************************************/
    LCD_BKLT = 0;
    LCD_CS = 1;
    LCD_RS = 0;
    LCD_RESET = 0;
    LCD_BKLT_TRIS = 0;
    LCD_CS_TRIS = 0;
    LCD_RS_TRIS = 0;
    LCD_RESET_TRIS = 0;

    
    /*******************************************************************/
    // Configure SPI2
    /*******************************************************************/   
    SDI2_TRIS = 1;
    SDO2_TRIS = 0;
    SCK2_TRIS = 0;
    
    SSP2STAT = 0x00;
    SSP2CON1 = 0x31;
    
    PIR3bits.SSP2IF = 0;

    LCD_Initialize();

    /*******************************************************************/
    // Enable System Interupts
    /*******************************************************************/
    INTCONbits.GIEH = 1;
    INTCONbits.GIEL = 1;


}


