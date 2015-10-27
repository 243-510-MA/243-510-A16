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

    
    RPINR1 = 4;

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

    //All digital pins, defaults
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

    //Push button definitions
    BUTTON_1_TRIS = 1;
    BUTTON_2_TRIS = 1;
    LED_1_TRIS = 0;
    LED_2_TRIS = 0;

    //RF CS, RESET, INT pin definitions
    PHY_CS_TRIS = 0;
    PHY_CS = 1;
    PHY_RESETn_TRIS = 0;
    PHY_RESETn = 1;
    RF_INT_TRIS = 1;
    
    

    //SPI Definitions
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
   //Wake pin definitions
    PHY_WAKE_TRIS = 0;
    PHY_WAKE = 1;

    // enable INT effective edge
    INTCON2bits.INTEDG1 = 0;
    //Enable MAC Address EEPROM
    RF_EEnCS_TRIS = 0;
    RF_EEnCS = 1;

    
    //Enable Interrupts
    INTCONbits.GIEH = 1;
    RFIF = 0;
    RFIE = 1;
    #if defined(ENABLE_NVM)
        EE_nCS_TRIS = 0;
        EE_nCS = 1;
    #endif
    #if defined(SENSOR_PORT_LCD)
        LCD_Initialize();
    #endif

}



    

