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


//CONFIGURATION PARAMETERS

#pragma config FNOSC = PRI
#pragma config POSCMOD = XT
#pragma config JTAGEN = OFF
#pragma config FWDTEN = OFF
#pragma config WDTPS = PS1024


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

    // Make RB0 as Digital input
    AD1PCFGbits.PCFG2 = 1;

    // set I/O ports
    BUTTON_1_TRIS = 1;
    BUTTON_2_TRIS = 1;
    LED_1_TRIS = 0;
    LED_2_TRIS = 0;

    #if defined(MRF24J40) || defined(MRF49XA) || defined(MRF24XA)
        PHY_CS_TRIS = 0;
        PHY_CS = 1;
        PHY_RESETn_TRIS = 0;
        PHY_RESETn = 1;
    #endif

    RF_INT_TRIS = 1;

    SDI_TRIS = 1;
    SDO_TRIS = 0;
    SCK_TRIS = 0;
    SPI_SDO = 0;
    SPI_SCK = 0;

    #if defined(MRF49XA)
        nFSEL_TRIS = 0;
        FINT_TRIS = 1;

        nFSEL = 1;
    #elif defined(MRF24J40)
        PHY_WAKE_TRIS = 0;
        PHY_WAKE = 1;
    #elif defined(MRF89XA)
        Data_nCS_TRIS = 0;
        Config_nCS_TRIS = 0;
        Data_nCS = 1;
        Config_nCS = 1;
        IRQ1_INT_TRIS = 1;
        IRQ0_INT_TRIS = 1;

    #endif


    

    #if defined(HARDWARE_SPI)
        SPI1CON1 = 0b0000000100111110;
        SPI1STAT = 0x8000;

        SPI2CON1 = 0b0000000100111110;
        SPI2STAT = 0x8000;
    #endif

   

    #if defined (MRF89XA)
        #if(!defined(__PIC32MX__))
            INTCON2bits.INT1EP = 0;
            INTCON2bits.INT2EP = 0;

            IPC7bits.INT2IP2 = 1;
            IPC7bits.INT2IP1 = 0;
            IPC7bits.INT2IP0 = 0;

            IPC5bits.INT1IP2 = 1;
            IPC5bits.INT1IP1 = 0;
            IPC5bits.INT1IP0 = 0;
        #endif
    #else
        #if(!defined(__PIC32MX__))
            INTCON2bits.INT1EP = 1;
        #endif
    #endif

    // Make RB0 as Digital input
    AD1PCFGbits.PCFG2 = 1;

    #if defined(ENABLE_NVM)

        EE_nCS_TRIS = 0;
        EE_nCS = 1;

    #endif

    #if defined(MRF89XA)
        PHY_IRQ1 = 0;
        PHY_IRQ0 = 0;
        PHY_RESETn_TRIS = 1;
    #else
        RFIF = 0;
        if( RF_INT_PIN == 0 )
        {
            RFIF = 1;
        }
    #endif



    LCD_Initialize();

}


