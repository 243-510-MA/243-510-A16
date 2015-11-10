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
#ifndef _DRV_NVM_FLASH_SST25VF064_H
#define _DRV_NVM_FLASH_SST25VF064_H

/************************************************************************
 * Section:  Includes                                                       
 ************************************************************************/
#include <stdint.h>
#include "system.h"
#include "driver/spi/drv_spi.h"


/******************************************************************************
  Function:
    void DRV_NVM_SST25VF064_Initialize( const SYS_MODULE_INDEX index,
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
void    DRV_NVM_SST25VF064_Initialize(DRV_SPI_INIT_DATA *pInitData);

/******************************************************************************
  Function:
    uint8_t DRV_NVM_SST25VF064_Write(   uint32_t address,
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
uint8_t     DRV_NVM_SST25VF064_Write(uint32_t address, uint8_t *pData, uint16_t nCount);

/******************************************************************************
  Function:
    void DRV_NVM_SST25VF064_Read(   uint32_t address,
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
void    DRV_NVM_SST25VF064_Read(uint32_t address, uint8_t *pData, uint16_t nCount);

/******************************************************************************
  Function:
    void DRV_NVM_SST25VF064_ChipErase( )

  Summary:
    Erase the flash device.

  Description:
    This routine erases the whole memory of the flash device.

  Parameters:
    None

  Returns:
    None
 ******************************************************************************/
void    DRV_NVM_SST25VF064_ChipErase(void);

/******************************************************************************
  Function:
    void DRV_NVM_SST25VF064_SectorErase( uint32_t address )

  Summary:
    Erase the sector specified by the given address.
    
  Description:
    This routine erases the sector of where the given address resides.
    
  Parameters:
    None

  Returns:
    None
******************************************************************************/
void    DRV_NVM_SST25VF064_SectorErase(uint32_t address);

/******************************************************************************
  Function:
    uint8_t DRV_NVM_SST25VF064_ReadStatusRegister( void )

  Summary:
    Reads the status register of the device.

  Description:
    This function reads the status register of the device.


  Parameters:
    None

  Returns:
    Returns the current value of the status register.
******************************************************************************/
uint8_t DRV_NVM_SST25VF064_ReadStatusRegister(); 

/******************************************************************************
  Function:
    void DRV_NVM_SST25VF064_WriteStatusRegister(uint8_t newStatus)

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
void DRV_NVM_SST25VF064_WriteStatusRegister(uint8_t newStatus);

#endif //_DRV_NVM_FLASH_SST25VF064_H

