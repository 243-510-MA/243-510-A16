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
#include "config_89xa.h"         //Tranceiver configuration file

#define SOFTWARE_SECURITY

#define USE_EXTERNAL_EEPROM


//TRANSCEIVER DEFINITIONS
#define PHY_IRQ1                    IFS1bits.INT2IF
#define PHY_IRQ1_En                 IEC1bits.INT2IE
#define PHY_IRQ0                    IFS1bits.INT1IF
#define PHY_IRQ0_En                 IEC1bits.INT1IE

#define IRQ1_INT_PIN                PORTEbits.RE9
#define IRQ1_INT_TRIS               TRISEbits.TRISE9
#define IRQ0_INT_PIN                PORTEbits.RE8
#define IRQ0_INT_TRIS               TRISEbits.TRISE8

//On Explorer 16 with PICtail plus connections we can use both IRQ0 and IRQ1 as interrupt lines
//instead of using IRQ0 as port pin and IRQ1 as interrupt pin to the PIC
//Enable USE_IRQ0_AS_INTERRUPT in config_89xa.h

#define PHY_IRQ1_TRIS               TRISEbits.TRISE9
#define Config_nCS                  LATBbits.LATB1
#define Config_nCS_TRIS             TRISBbits.TRISB1
#define Data_nCS                    LATBbits.LATB2
#define Data_nCS_TRIS               TRISBbits.TRISB2
#define PHY_RESETn                  LATGbits.LATG2
#define PHY_RESETn_TRIS             TRISGbits.TRISG2
                                     //instead of using IRQ0 as port pin and IRQ1 as interrupt pin to the PIC

//SPI DEFINITIONS
#define SPI_SDI                     PORTFbits.RF7
#define SDI_TRIS                    TRISFbits.TRISF7
#define SPI_SDO                     LATFbits.LATF8
#define SDO_TRIS                    TRISFbits.TRISF8
#define SPI_SCK                     LATFbits.LATF6
#define SCK_TRIS                    TRISFbits.TRISF6
      
//PUSH BUTTON, LED PIN DEFINITIONS
#define PUSH_BUTTON_1               PORTDbits.RD6
#define PUSH_BUTTON_2               PORTDbits.RD7
#define BUTTON_1_TRIS               TRISDbits.TRISD6
#define BUTTON_2_TRIS               TRISDbits.TRISD7
#define LED_1                       LATAbits.LATA7
#define LED_2                       LATAbits.LATA6
#define LED_1_TRIS                  TRISAbits.TRISA7
#define LED_2_TRIS                  TRISAbits.TRISA6

// Define SUPPORT_TWO_SPI if external EEPROM use the second SPI
// port alone, not sharing SPI port with the transceiver
#define SUPPORT_TWO_SPI

// External EEPROM SPI chip select pin definition
#define EE_nCS_TRIS                 TRISDbits.TRISD12
#define EE_nCS                      LATDbits.LATD12

#define TMRL TMR2

//LCD DEFINITIONS
// Following definitions are used for LCD display on the demo board
#define LCD_DATA0_TRIS              (TRISEbits.TRISE0)		// Multiplexed with LED6
#define LCD_DATA0_IO                (LATEbits.LATE0)
#define LCD_DATA1_TRIS              (TRISEbits.TRISE1)
#define LCD_DATA1_IO                (LATEbits.LATE1)
#define LCD_DATA2_TRIS              (TRISEbits.TRISE2)
#define LCD_DATA2_IO                (LATEbits.LATE2)
#define LCD_DATA3_TRIS              (TRISEbits.TRISE3)		// Multiplexed with LED3
#define LCD_DATA3_IO                (LATEbits.LATE3)
#define LCD_DATA4_TRIS              (TRISEbits.TRISE4)		// Multiplexed with LED2
#define LCD_DATA4_IO                (LATEbits.LATE4)
#define LCD_DATA5_TRIS              (TRISEbits.TRISE5)
#define LCD_DATA5_IO                (LATEbits.LATE5)
#define LCD_DATA6_TRIS              (TRISEbits.TRISE6)
#define LCD_DATA6_IO                (LATEbits.LATE6)
#define LCD_DATA7_TRIS              (TRISEbits.TRISE7)
#define LCD_DATA7_IO                (LATEbits.LATE7)
#define LCD_RD_WR_TRIS              (TRISDbits.TRISD5)
#define LCD_RD_WR_IO                (LATDbits.LATD5)
#define LCD_RS_TRIS                 (TRISBbits.TRISB15)
#define LCD_RS_IO                   (LATBbits.LATB15)
#define LCD_E_TRIS                  (TRISDbits.TRISD4)
#define LCD_E_IO                    (LATDbits.LATD4)


#endif	/* SYSTEM_CONFIG_H */

