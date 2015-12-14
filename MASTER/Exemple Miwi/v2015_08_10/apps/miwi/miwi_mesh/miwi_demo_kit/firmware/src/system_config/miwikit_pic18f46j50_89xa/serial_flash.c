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


#define SPI_READ            0x03
#define SPI_SECTOR_ERASE    0x20
#define SPI_BLOCK_ERASE     0x52
#define SPI_CHIP_ERASE      0x60
#define SPI_BYTE_PROGRAM    0x02
#define SPI_AUTO_ADDRESS_INC 0xAF
#define SPI_READ_STATUS_REG 0x05
#define SPI_WRITE_ENABLE    0x06
#define SPI_READ_ID         0x90
#define SPI_WRITE_DISABLE   0x04

#define READ_MANUFACTURER_ID 0xBF
#define READ_DEVICE_ID      0x49

/*********************************************************************
* Function:         SSTRead(uint8_t *dest, uint8_t *addr, uint8_t count)
*
* PreCondition:     none
*
* Input:            uint8_t *dest  - Destination buffer.
*                   uint8_t *addr   - Address to start reading from.
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
void SSTRead(uint8_t *dest, uint8_t *addr, uint8_t count)
{
    EE_nCS = 0;
    SPIPut2(SPI_READ);
    SPIPut2(*addr);
    SPIPut2(*addr++);
    SPIPut2(*addr++);
    while( count )
    {
        *dest++ = SPIGet2();
        count--;
    }
    EE_nCS = 1;
} 

/*********************************************************************
* Function:         void SSTWrite(uint8_t *src, uint8_t *addr, uint8_t count)
*
* PreCondition:     none
*
* Input:            uint8_t *src  - Source buffer.
*                   uint8_t *addr   - Address to start reading from.
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
void SSTWrite(uint8_t *src, uint8_t *addr, uint8_t count)
{
    EE_nCS = 0;
    SPIPut2(SPI_WRITE_ENABLE);
    SPIPut2(SPI_AUTO_ADDRESS_INC);
    SPIPut2(*addr);
    SPIPut2(*addr++);
    SPIPut2(*addr++);
    while( count )
    {
        SPIPut(*src++);
        count--;
        if(count != 0) SPIPut2(SPI_AUTO_ADDRESS_INC);
    }
    SPIPut2(SPI_WRITE_DISABLE);
    EE_nCS = 1;
} 

/*********************************************************************
* Function:         void SSTGetID(uint8_t dest)
*
* PreCondition:     none
*
* Input:            uint8_t dest  - Destination buffer.
*                   
* Output:           none
*
* Side Effects:	    none
*
* Overview:         Following routine reads bytes from the SST Flash and puts
*                   them in a buffer.
*                    
*
* Note:			    
**********************************************************************/   
void SSTGetID(uint8_t *dest)
{
    EE_nCS = 0;
    SPIPut2(SPI_READ_ID);
    SPIPut2(0x00);
    SPIPut2(0x00);
    SPIPut2(0x00);
    *dest = SPIGet2();
    EE_nCS = 1;
} 
