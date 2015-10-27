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


#define SPI_WRT_STATUS  0x01
#define SPI_WRITE       0x02
#define SPI_READ        0x03
#define SPI_DIS_WRT     0x04
#define SPI_RD_STATUS   0x05
#define SPI_EN_WRT      0x06

#define EEPROM_MAC_ADDR 0xFA

extern uint8_t myLongAddress[];


/*********************************************************************
* Function:        void EEPROMRead(uint8_t *dest, uint8_t addr, uint8_t count)
*
* PreCondition:     none
*
* Input:            uint8_t *dest  - Destination buffer.
*                   uint8_t addr   - Address to start reading from.
*                   uint8_t count  - Number of bytes to read.
*
* Output:           none
*
* Side Effects:	    none
*
* Overview:         Following routine reads bytes from the EEProm and puts
*                   them in a buffer.
*                    
*
* Note:			    
**********************************************************************/   
void EEPROMRead(uint8_t *dest, uint8_t addr, uint8_t count)
{
    MAC_nCS = 0;
    SPIPut2(SPI_READ);
    SPIPut2(addr);
    while( count )
    {
        *dest++ = SPIGet2();
        count--;
    }
    MAC_nCS = 1;
} 


/*********************************************************************
* Function:         void Read_MAC_Address(void)
*
* PreCondition:     none
*
* Input:            none
*
* Output:           none
*
* Side Effects:	    none
*
* Overview:         Following routine reads the MAC Address form the
*                   EEProm and loads it into myLongAddress[].
*                    
*
* Note:			    
**********************************************************************/  
void Read_MAC_Address(void)
{
    uint8_t i;
    
    if( MY_ADDRESS_LENGTH > 6 )
    {
        for(i = 0; i < 3; i++)
        {
            EEPROMRead(&(myLongAddress[MY_ADDRESS_LENGTH-1-i]), EEPROM_MAC_ADDR+i, 1);
        }

        if( MY_ADDRESS_LENGTH > 7 )
        {
            myLongAddress[4] = 0xFF;

        }
        myLongAddress[3] = 0xFE;

        for(i = 0; i < 3; i++)
        {
            EEPROMRead(&(myLongAddress[2-i]), EEPROM_MAC_ADDR+3+i, 1);
        }
    }
    else
    {
        for(i = 0; i < MY_ADDRESS_LENGTH; i++)
        {
            EEPROMRead(&(myLongAddress[i]), EEPROM_MAC_ADDR+5-i, 1);
        }
    }
}



    	