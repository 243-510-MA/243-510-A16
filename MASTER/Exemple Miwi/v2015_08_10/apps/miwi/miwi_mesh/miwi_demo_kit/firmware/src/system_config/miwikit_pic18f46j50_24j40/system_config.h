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

#ifndef _SYSTEM_CONFIG_H
    #define _SYSTEM_CONFIG_H
 
#include "miwi_config.h"        //Include miwi application layer configuration file
#include "miwi_config_mesh.h"   //Include protocol layer configuration file
#include "config_24j40.h"       //Transceiver configuration file
 
   
#define SW1             1
#define SW2             2	
    
// There are three ways to use NVM to store data: External EPROM, Data EEPROM and
// programming space, with following definitions:
//      #define USE_EXTERNAL_EEPROM
//      #define USE_DATA_EEPROM
//      #define USE_PROGRAMMING_SPACE
// Each demo board has defined the method of using NVM, as
// required by Network Freezer feature.
#define USE_EXTERNAL_EEPROM

#define SUPPORT_TWO_SPI

// Define EEPROM_SHARE_SPI if external EEPROM shares the SPI
// bus with RF transceiver
//#define EEPROM_SHARE_SPI


// MRF24J40 Pin Definitions
#define RFIF                INTCONbits.INT0IF
#define RFIE                INTCONbits.INT0IE
#define PHY_CS              LATAbits.LATA5
#define PHY_CS_TRIS         TRISAbits.TRISA5
#define RF_INT_PIN          PORTBbits.RB0
#define RF_INT_TRIS         TRISBbits.TRISB0
#define PHY_WAKE            LATDbits.LATD1
#define PHY_WAKE_TRIS       TRISDbits.TRISD1
#define PHY_RESETn          LATDbits.LATD0
#define PHY_RESETn_TRIS     TRISDbits.TRISD0


// EEProm Pin Definitions
#define RF_EEnCS            LATDbits.LATD5
#define RF_EEnCS_TRIS	    TRISDbits.TRISD5

// SPI1 Pin Definitions
#define SPI_SDI             PORTBbits.RB5
#define SDI_TRIS            TRISBbits.TRISB5
#define SPI_SDO             LATCbits.LATC7
#define SDO_TRIS            TRISCbits.TRISC7
#define SPI_SCK             LATBbits.LATB4
#define SCK_TRIS            TRISBbits.TRISB4

// SPI2 Pin Definitions
#define SPI_SDI2            PORTDbits.RD6
#define SDI2_TRIS           TRISDbits.TRISD6
#define SPI_SDO2            LATDbits.LATD2
#define SDO2_TRIS           TRISDbits.TRISD2
#define SPI_SCK2            LATDbits.LATD4
#define SCK2_TRIS           TRISDbits.TRISD4

#define SPI2SSPIF           PIR3bits.SSP2IF
#define SPI2WCOL            SSP2CON1bits.WCOL
#define SPI2SSPBUF          SSP2BUF

// Switch and LED Pin Definitions
#define SW1_PORT            PORTBbits.RB1
#define SW1_TRIS            TRISBbits.TRISB1
#define SW2_PORT            PORTBbits.RB2
#define SW2_TRIS            TRISBbits.TRISB2

//pin assignables
//B2 Buzzer
//B3 GYRO
//A0
//A1
//A2
//E0
//E1

#define DOOR                LATDbits.LATD7
#define DOOR_TRIS           TRISDbits.TRISD7         

#define PROJECTOR           LATAbits.LATA3
#define PROJECTOR_TRIS      TRISAbits.TRISA3

#define LED0                LATCbits.LATC2
#define LED0_TRIS           TRISCbits.TRISC2
#define LED1                LATAbits.LATA6
#define LED1_TRIS           TRISAbits.TRISA6
#define LED2                LATAbits.LATA7
#define LED2_TRIS           TRISAbits.TRISA7

//Buzzer
#define Buzzer                LATBbits.LATB2
#define Buzzer_TRIS           TRISBbits.TRISB2
//Buzzer

//GYRO
#define GYRO                LATBbits.LATB3
#define GYRO_TRIS           TRISBbits.TRISB3
//GYRO

// External EEPROM Pin Definitions
//#define EE_nCS_TRIS         TRISDbits.TRISD5
#define MAC_nCS             LATDbits.LATD5
#define TMRL                TMR0L

//External SST Serial Flash Definitions
#define EE_nCS              LATEbits.LATE2
#define EE_nCS_TRIS         TRISEbits.TRISE2

// LCD Pin Definitions
#define LCD_CS_TRIS         TRISDbits.TRISD7
#define LCD_CS              LATDbits.LATD7
#define LCD_RS_TRIS         TRISDbits.TRISD3
#define LCD_RS              LATDbits.LATD3
#define LCD_RESET_TRIS      TRISEbits.TRISE0
#define LCD_RESET           LATEbits.LATE0
#define LCD_BKLT_TRIS       TRISEbits.TRISE1
#define LCD_BKLT            LATEbits.LATE1
#endif
