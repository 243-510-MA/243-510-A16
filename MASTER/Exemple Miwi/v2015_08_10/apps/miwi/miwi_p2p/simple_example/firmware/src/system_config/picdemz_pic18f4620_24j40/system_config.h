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
#include "config_24j40.h"               //MiMAC API


#define USE_DATA_EEPROM

// Transceiver Configuration - Supports only MRF24J40

#define RFIF                        INTCONbits.INT0IF
#define RFIE                        INTCONbits.INT0IE
#define PHY_CS                      LATCbits.LATC0
#define PHY_CS_TRIS                 TRISCbits.TRISC0
#define PHY_RESETn                  LATCbits.LATC2
#define PHY_RESETn_TRIS             TRISCbits.TRISC2
#define PHY_WAKE                    LATCbits.LATC1
#define PHY_WAKE_TRIS               TRISCbits.TRISC1

#define PUSH_BUTTON_1               PORTBbits.RB5
#define PUSH_BUTTON_2               PORTBbits.RB4
#define LED_1                       LATAbits.LATA0
#define LED_2                       LATAbits.LATA1

#define BUTTON_1_TRIS               TRISBbits.TRISB5
#define BUTTON_2_TRIS               TRISBbits.TRISB4
#define LED_1_TRIS                  TRISAbits.TRISA0
#define LED_2_TRIS                  TRISAbits.TRISA1

#define RF_INT_PIN                  PORTBbits.RB0
#define RF_INT_TRIS                 TRISBbits.TRISB0

#define SPI_SDI                     PORTCbits.RC4
#define SDI_TRIS                    TRISCbits.TRISC4
#define SPI_SDO                     LATCbits.LATC5
#define SDO_TRIS                    TRISCbits.TRISC5
#define SPI_SCK                     LATCbits.LATC3
#define SCK_TRIS                    TRISCbits.TRISC3

#define TMRL                        TMR0L





#endif	/* SYSTEM_CONFIG_H */

