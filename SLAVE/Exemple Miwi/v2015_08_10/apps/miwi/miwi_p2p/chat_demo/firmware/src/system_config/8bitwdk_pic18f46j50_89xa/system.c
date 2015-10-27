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

//Configuration parameters
#pragma config OSC = INTOSC
#pragma config WDTEN = OFF
#pragma config XINST = OFF
#pragma config PLLDIV = 2
#pragma config CPUDIV = OSC2_PLL2

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

    // primary internal oscillator
    OSCCON = 0x7B;
    WDTCONbits.SWDTEN = 0;

    INTCON = 0;
    //Peripheral pin select definitions
    EECON2 = 0x55;
    EECON2 = 0xAA;
    PPSCONbits.IOLOCK = 0;

    RPINR3 = 6;
    
    #if !defined(SENSOR_PORT_UART)
        // use LCD
        RPINR21 = 19;           //Mapping SDI2 to RD2
        RPOR23 = 9;             //Mapping SDO2 to RD6
        RPOR21 = 10;            //Mapping SCK2 to RD4
    #endif
    #if defined(SENSOR_PORT_UART)
        // use UART
        RPINR16 = 19;
        RPOR17 = 5;
    #endif
    EECON2 = 0x55;
    EECON2 = 0xAA;
    PPSCONbits.IOLOCK = 1;

    //all digital pins, defaults
    ANCON0 = 0xFF;
    ANCON1 = 0x3F;
    INTCON2bits.RBPU = 0;

    TRISA = 0xFF;
    TRISB = 0xFF;
    TRISC = 0xFF;
    TRISD = 0xFF;
    TRISE = 0xFF;

    LATA = 0;
    LATB = 0;
    //LATC = 0;
    LATD = 0;
    LATE = 0;

   

    //push button, led definitions
    BUTTON_1_TRIS = 1;
    BUTTON_2_TRIS = 1;
    LED_1_TRIS = 0;
    LED_2_TRIS = 0;

    
    //Trasceiver definitions
    PHY_RESETn_TRIS = 1;
    IRQ1_INT_TRIS = 1;
    IRQ0_INT_TRIS = 1;
    #if defined(USE_IRQ0_AS_INTERRUPT)
        PHY_IRQ0_En = 1;
        PHY_IRQ0 = 0;
        //Enable the effective INT edge
    #endif

    //spi definitions
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

    //CS, INT pin definitions
    Data_nCS_TRIS = 0;
    Config_nCS_TRIS = 0;
    Data_nCS = 1;
    Config_nCS = 1;
    INTCON2bits.INTEDG3 = 1;

    //EEPROM definitions
    RF_EEnCS_TRIS = 0;
    RF_EEnCS = 1;

    #if defined(ENABLE_NVM)
        EE_nCS_TRIS = 0;
        EE_nCS = 1;

    #endif

    //Enable global interrupt
    INTCONbits.GIEH = 1;
    

    #if defined(SENSOR_PORT_LCD)
        LCD_Initialize();
    #endif

}



    

