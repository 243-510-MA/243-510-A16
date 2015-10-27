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
#ifndef __EEPROM_H
    #define __EEPROM_H
    
    #define SPI_WRT_STATUS  0x01
#ifndef  SPI_WRITE
    #define SPI_WRITE       0x02
#endif
#ifndef SPI_READ
    #define SPI_READ        0x03
#endif
    #define SPI_DIS_WRT     0x04
    #define SPI_RD_STATUS   0x05
    #define SPI_EN_WRT      0x06


    #define EEPROM_MAC_ADDR 0xFA
/*********************************************************************
* Function: void EEPROM_Read(uint8_t *dest, uint8_t addr, uint8_t count)
*
* Overview: Reads the EEPROM Starting from "addr" for "count" bytes and
* copies data to the address location "dest"
*
* PreCondition: None
*
* Input:  None
*
* Output: Buttons State
*
********************************************************************/
void EEPROM_Read(uint8_t *dest, uint8_t addr, uint8_t count);

#endif
