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

#ifndef _MCHP25LC256_EEPROM_H
    #define _MCHP25LC256_EEPROM_H
/************************************************************************
 * Section:  Includes                                                       
 ************************************************************************/
#include <stdint.h>
#include "system.h"
#include "driver/spi/drv_spi.h"

/************************************************************************
* EEPROM Commands                                                       
************************************************************************/
    #define EEPROM_PAGE_SIZE    (unsigned)64
    #define EEPROM_PAGE_MASK    (unsigned)0x003f
    #define EEPROM_CMD_READ     (unsigned)0x03
    #define EEPROM_CMD_WRITE    (unsigned)0x02
    #define EEPROM_CMD_WRDI     (unsigned)0x04
    #define EEPROM_CMD_WREN     (unsigned)0x06
    #define EEPROM_CMD_RDSR     (unsigned)0x05
    #define EEPROM_CMD_WRSR     (unsigned)0x01

/************************************************************************
* Structure STATREG and union _MCHP25LC256Status_                            
*                                                                       
* Overview: provide bits and byte access to EEPROM status value      
*                                                                       
************************************************************************/

struct STATREG
{
    uint8_t WIP : 1;
    uint8_t WEL : 1;
    uint8_t BP0 : 1;
    uint8_t BP1 : 1;
    uint8_t RESERVED : 3;
    uint8_t WPEN : 1;
};

union _MCHP25LC256Status_
{
    struct STATREG  Bits;
    uint8_t         Char;
};

/************************************************************************
* Macro: MCHP25LC256CSLow()                                                   
*                                                                       
* Preconditions: CS IO must be configured as output
*                                                                       
* Overview: this macro pulls down CS line                    
*                                                                       
* Input: none                                                          
*                                                                       
* Output: none                                                         
*                                                                       
************************************************************************/
    #define MCHP25LC256CSLow()   MCHP25LC256_CS_LAT = 0;

/************************************************************************
* Macro: MCHP25LC256CSHigh()
*                                                                       
* Preconditions: CS IO must be configured as output
*                                                                       
* Overview: this macro set CS line to high level
*                                                                       
* Input: none                                                          
*                                                                       
* Output: none
*                                                                       
************************************************************************/
    #define MCHP25LC256CSHigh()  MCHP25LC256_CS_LAT = 1;

/******************************************************************************
  Function:
    void DRV_NVM_MCHP25LC256_Initialize( const SYS_MODULE_INDEX index,
                                         const SYS_MODULE_INIT * const init )

  Summary:
    Initializes hardware and data for the given instance of the NVM module

  Description:
    This routine initializes hardware for the instance of the NVM module,
    using the hardware initialization given data.  It also initializes all
    necessary internal data.

  Parameters:
    pInitData - Pointer to the data structure containing all data
                necessary to initialize the hardware. This pointer may
                be null if no data is required and static initialization
                values are to be used.

  Returns:
    None
******************************************************************************/
void DRV_NVM_MCHP25LC256_Initialize(DRV_SPI_INIT_DATA *pInitData);

/******************************************************************************
  Function:
    uint8_t DRV_NVM_MCHP25LC256_Write(    uint16_t address,
                                          uint8_t *pData, 
                                          uint16_t nCount )

  Summary:
    Writes an array of bytes to a specified address.
    
  Description:
    This routine writes the array of bytes from the location pointed to by 
    pData to the given address. The number of bytes to be written is specified
    by nCount.

  Parameters:
    address - starting address of the array to be written
    pData   - pointer to the source of the array
    nCount  - specifies the number of bytes to be written

  Returns:
    1 - if the write is successful 
    0 - if the write was not successful
******************************************************************************/
uint8_t DRV_NVM_MCHP25LC256_Write(uint16_t address, uint8_t *pData, uint16_t nCount);

/******************************************************************************
  Function:
    void DRV_NVM_MCHP25LC256_Read(  uint16_t address, 
                                    uint8_t *pData, 
                                    uint16_t nCount )

  Summary:
    Reads an array of bytes from the specified address.
    
  Description:
    This routine reads an array of bytes from the specified address location. The
    read array is saved to the location pointed to by pData. The number of bytes 
    to be read is specified by nCount.

  Parameters:
    address - starting address of the array to be read
    pData   - pointer to the destination of the read array
    nCount  - specifies the number of bytes to be read

  Returns:
    None
******************************************************************************/
void DRV_NVM_MCHP25LC256_Read(uint16_t address, uint8_t *pData, uint16_t nCount);

/******************************************************************************
  Function:
    union _MCHP25LC256Status_ DRV_NVM_MCHP25LC256_ReadStatusRegister( void )
    
  Summary:
    Reads the status register of the device.
    
  Description:
    This function reads the status register of the device.


  Parameters:
    None

  Returns:
    The current value of the status register.
******************************************************************************/
union _MCHP25LC256Status_ DRV_NVM_MCHP25LC256_ReadStatusRegister(void);

/******************************************************************************
  Function:
    void DRV_NVM_MCHP25LC256_WriteStatusRegister(uint8_t newStatus)

  Summary:
    Programs the status register.
    
  Description:
    This routine programs the status register of the flash device
    with the new value specified by newStatus. Only bits that are
    actuall writable will be modified.

  Parameters:
    newStatus - the new status that will be used.

  Returns:
    None
******************************************************************************/
void DRV_NVM_MCHP25LC256_WriteStatusRegister(uint8_t newStatus);


#endif //_MCHP25LC256_EEPROM_H

