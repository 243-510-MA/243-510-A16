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

#include "miwi_config.h"                //MiWi Application layer configuration file
#include "miwi_config_p2p.h"            //MiAPP API
#include "config_24xa.h"               //MiMAC API




    #define USE_EXTERNAL_EEPROM
    #define EEPROM_SHARE_SPI


    // TRANSCEIVER DEFINITIONS
    #define RFIF                    INTCON3bits.INT3IF
    #define RFIE                    INTCON3bits.INT3IE
    #define PHY_CS                      LATCbits.LATC2
    #define PHY_CS_TRIS                 TRISCbits.TRISC2
    #define RF_INT_PIN                  PORTBbits.RB3
    #define RF_INT_TRIS                 TRISBbits.TRISB3

    //SPI DEFINITIONS
    #define SPI_SDI                     PORTCbits.RC4
    #define SDI_TRIS                    TRISCbits.TRISC4
    #define SPI_SDO                     LATCbits.LATC5
    #define SDO_TRIS                    TRISCbits.TRISC5
    #define SPI_SCK                     LATCbits.LATC3
    #define SCK_TRIS                    TRISCbits.TRISC3

    #define PHY_RESETn                  LATBbits.LATB5
    #define PHY_RESETn_TRIS             TRISBbits.TRISB5

    //PUSH BUTTON, LED DEFINITIONS
    #define PUSH_BUTTON_1               PORTBbits.RB0
    #define BUTTON_1_TRIS               TRISBbits.TRISB0
    #define PUSH_BUTTON_2               PORTAbits.RA5
    #define BUTTON_2_TRIS               TRISAbits.TRISA5


    #define LED_1                       LATDbits.LATD7
    #define LED_1_TRIS                  TRISDbits.TRISD7
    #define LED_2                       LATDbits.LATD6
    #define LED_2_TRIS                  TRISDbits.TRISD6


    #define EE_nCS_TRIS                 TRISAbits.TRISA3
    #define EE_nCS                      LATAbits.LATA3

    #define TMRL                        TMR1L


    //LCD DEFINITIONS
    // Following definitions are used for LCD display on the demo board
    #define LCD_CS_TRIS                 (TRISAbits.TRISA2)
    #define LCD_CS                      (LATAbits.LATA2)
    #define LCD_RESET_TRIS              (TRISFbits.TRISF6)
    #define LCD_RESET                   (LATFbits.LATF6)



#endif	/* SYSTEM_CONFIG_H */

