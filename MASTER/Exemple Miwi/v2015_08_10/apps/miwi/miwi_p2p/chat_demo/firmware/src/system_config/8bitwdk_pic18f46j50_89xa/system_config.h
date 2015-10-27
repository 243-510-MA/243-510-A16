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

#ifndef SYSTEM_CONFIG_H
#define	SYSTEM_CONFIG_H

#include "miwi_config.h"          //MiWi Application layer configuration file
#include "miwi_config_p2p.h"      //MiWi Protocol layer configuration file
#include "config_89xa.h"          //Tranceiver configuration file
              
#define USE_EXTERNAL_EEPROM

#if defined (ENABLE_SECURITY)
#define SOFTWARE_SECURITY
#endif


// Define EEPROM_SHARE_SPI if external EEPROM shares the same SPI with transceiver
#define EEPROM_SHARE_SPI

// 8-bit wireless demo board can use the sensor port to connect to either a LCD or a RS232 serial port
#define SENSOR_PORT_LCD
#define SENSOR_PORT_UART
#define ENABLE_MANUALBACKLIGHT

//TRANSCEIVER DEFINITIONS
#define IRQ1_INT_PIN                PORTBbits.RB3
#define IRQ1_INT_TRIS               TRISBbits.TRISB3
#define PHY_IRQ1                    INTCON3bits.INT3IF
#define PHY_IRQ1_En                 INTCON3bits.INT3IE
#define Config_nCS                  LATEbits.LATE1
#define Config_nCS_TRIS             TRISEbits.TRISE1
#define Data_nCS                    LATCbits.LATC2
#define Data_nCS_TRIS               TRISCbits.TRISC2
#define IRQ0_INT_PIN                PORTDbits.RD3
#define IRQ0_INT_TRIS               TRISDbits.TRISD3
#define RF_EEnCS_TRIS               TRISEbits.TRISE0
#define RF_EEnCS                    LATEbits.LATE0
#define PHY_RESETn                  LATDbits.LATD7
#define PHY_RESETn_TRIS             TRISDbits.TRISD7

//If using both IRQ0 and IRQ1 then uncomment the following lines
//Also define the PHY_IRQ0 connections
//#define PHY_IRQ0
//#define PHY_IRQ0_En
//#define USE_IRQ0_AS_INTERRUPT in config_89xa.h



//SPI DEFINITIONS
#define SPI_SDI                     PORTBbits.RB5
#define SDI_TRIS                    TRISBbits.TRISB5
#define SPI_SDO                     LATCbits.LATC7
#define SDO_TRIS                    TRISCbits.TRISC7
#define SPI_SCK                     LATBbits.LATB4
#define SCK_TRIS                    TRISBbits.TRISB4

//PUSH BUTTON DEFINITIONS
#define PUSH_BUTTON_1               PORTBbits.RB0
#define BUTTON_1_TRIS               TRISBbits.TRISB0
#define PUSH_BUTTON_2               PORTBbits.RB2
#define BUTTON_2_TRIS               TRISBbits.TRISB2

//LED DEFINITIONS
#define LED_1                       LATAbits.LATA2
#define LED_1_TRIS                  TRISAbits.TRISA2
#define LED_2                       LATAbits.LATA3
#define LED_2_TRIS                  TRISAbits.TRISA3

//EEPROM DEFINITIONS
#define EE_nCS_TRIS                 TRISDbits.TRISD5
#define EE_nCS                      LATDbits.LATD5
#define TMRL                        TMR0L

//LCD DEFINITIONS
// Following definitions are used for LCD display on the demo board
#define SUPPORT_TWO_SPI

#define SPInCS_TRIS                 TRISCbits.TRISC6
#define SPInCS_LAT                  LATCbits.LATC6
#define SPISSPSTAT                  SSP2STAT
#define SPISSPCON                   SSP2CON1
#define SPISSPBUF                   SSP2BUF
#define SPIWCOL                     SSP2CON1bits.WCOL
#define SPISSPIE                    PIE3bits.SSP2IE
#define SPISSPIF                    PIR3bits.SSP2IF



#endif
/* SYSTEM_CONFIG_H */
//EOF

