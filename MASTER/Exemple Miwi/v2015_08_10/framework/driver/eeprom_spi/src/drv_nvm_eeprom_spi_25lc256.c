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

#include <stdint.h>
#include "system.h"
#include "driver/eeprom_spi/drv_nvm_eeprom_spi_25lc256.h"

typedef union
{
    struct
    {
        uint8_t uint8Address[2];
    };
    uint16_t uint16Address;
} MCHP25LC256_ADDRESS;

typedef union
{
    struct
    {
        uint8_t uint16Byte[2];
    };
    uint16_t uint16Word;
} MCHP25LC256_UINT16;

/************************************************************************
* EEPROM Commands                                                       
************************************************************************/
typedef enum {

    EEPROM_CMD_READ  = (unsigned)0x03, // read memory
    EEPROM_CMD_WRITE = (unsigned)0x02, // write memory
    EEPROM_CMD_WRDI  = (unsigned)0x04, // reset the write enable latch (disable writes)
    EEPROM_CMD_WREN  = (unsigned)0x06, // set the write enable latch (enable writes)
    EEPROM_CMD_RDSR  = (unsigned)0x05, // read status register
    EEPROM_CMD_WRSR  = (unsigned)0x01  // write status register
    
} DRV_NVM_FLASH_SPI_MCHP25LC256_COMMANDS;

static DRV_SPI_INIT_DATA eepromInitData;

/************************************************************************
* Macros
************************************************************************/
#define EEPROM_PAGE_SIZE    (unsigned)64
#define EEPROM_PAGE_MASK    (unsigned)0x003f

#define SPILOCK(ch)                         DRV_SPI_Lock(ch)
#define SPIUNLOCK(ch)                       DRV_SPI_Unlock(ch)
#define SPIINITIALIZE(ch, initData)         DRV_SPI_Initialize(ch, initData);
#define PUTSPIBYTE(ch, byte)                {                           \
                                                DRV_SPI_Put(ch, byte);  \
                                            }

#define GETSPIBYTE(ch, data)                {                           \
                                                data = DRV_SPI_Get(ch); \
                                            }


/******************************************************************************
  Function:
    void DRV_NVM_MCHP25AA02E48_Initialize( const SYS_MODULE_INDEX index,
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
void DRV_NVM_MCHP25LC256_Initialize(DRV_SPI_INIT_DATA *pInitData)
{
    // initialize the SPI channel to be used
    SPIINITIALIZE(pInitData->channel, pInitData);
    memcpy(&eepromInitData, pInitData, sizeof(DRV_SPI_INIT_DATA));
}

/******************************************************************************
  Function:
    void DRV_NVM_MCHP25LC256_WriteEnable( void )
    
  Summary:
    Enables the device for writes. 
    
  Description:
    This is an internal function called within the module to enable the 
    device for writes.
    
  Parameters:
    None

  Returns:
    None
******************************************************************************/
void DRV_NVM_MCHP25LC256_WriteEnable(void)
{
    MCHP25LC256CSLow();
    PUTSPIBYTE(eepromInitData.channel, EEPROM_CMD_WREN);
    MCHP25LC256CSHigh();
}

/******************************************************************************
  Function:
    static inline __attribute__((__always_inline__)) union _MCHP25LC256Status_ ReadStatusRegister(void)

  Summary:
    Reads the status register of the device.

  Description:
    This is an internal inline function called within the module to
    read the status register of the device.

  Parameters:
    None

  Returns:
    Returns a byte indicating the status of the device.
******************************************************************************/
static inline __attribute__((__always_inline__)) union _MCHP25LC256Status_ ReadStatusRegister(void)
{
    uint8_t temp;

    MCHP25LC256CSLow();

    PUTSPIBYTE(eepromInitData.channel, EEPROM_CMD_RDSR);
    GETSPIBYTE(eepromInitData.channel, temp);

    MCHP25LC256CSHigh();
    
    return ((union _MCHP25LC256Status_)temp);

}

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
union _MCHP25LC256Status_ DRV_NVM_MCHP25LC256_ReadStatusRegister(void)
{
    union _MCHP25LC256Status_ temp;

    while(!SPILOCK(eepromInitData.channel));

    DRV_SPI_Initialize(eepromInitData.channel, (DRV_SPI_INIT_DATA *)&eepromInitData);
    temp = ReadStatusRegister();

    SPIUNLOCK(eepromInitData.channel);

    return (union _MCHP25LC256Status_)temp;
}

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
uint8_t DRV_NVM_MCHP25LC256_Write(uint16_t address, uint8_t *pData, uint16_t nCount)
{
    uint16_t    addr;
    uint8_t     *pD;
    uint16_t    counter, ret;

    addr = address;
    pD = pData;

    // WRITE
    DRV_NVM_MCHP25LC256_WriteEnable();
    
    while(!SPILOCK(eepromInitData.channel));

    DRV_SPI_Initialize(eepromInitData.channel, (DRV_SPI_INIT_DATA *)&eepromInitData);
    
    MCHP25LC256CSLow();

    PUTSPIBYTE(eepromInitData.channel, EEPROM_CMD_WRITE);
    PUTSPIBYTE(eepromInitData.channel, ((MCHP25LC256_ADDRESS)addr).uint8Address[1]);
    PUTSPIBYTE(eepromInitData.channel, ((MCHP25LC256_ADDRESS)addr).uint8Address[0]);

    for(counter = 0; counter < nCount; counter++)
    {
        PUTSPIBYTE(eepromInitData.channel, *pD++);
        address++;

        // check for page rollover
        if((((MCHP25LC256_ADDRESS)addr).uint8Address[0] & EEPROM_PAGE_MASK) == 0)
        {
            MCHP25LC256CSHigh();

            // Wait for completion of the write operation
            while(ReadStatusRegister().Bits.WIP);

            // Start writing of the next page
            DRV_NVM_MCHP25LC256_WriteEnable();
            MCHP25LC256CSLow();

            PUTSPIBYTE(eepromInitData.channel, EEPROM_CMD_WRITE);
            PUTSPIBYTE(eepromInitData.channel, ((MCHP25LC256_ADDRESS)addr).uint8Address[1]);
            PUTSPIBYTE(eepromInitData.channel, ((MCHP25LC256_ADDRESS)addr).uint8Address[0]);
            
        }
    }

    MCHP25LC256CSHigh();

    // Wait for write end
    while(ReadStatusRegister().Bits.WIP);

    // VERIFY
    ret = 1;
    for(counter = 0; counter < nCount; counter++)
    {
        MCHP25LC256CSLow()
        PUTSPIBYTE(eepromInitData.channel, EEPROM_CMD_READ);
        PUTSPIBYTE(eepromInitData.channel, ((MCHP25LC256_ADDRESS)address).uint8Address[1]);
        PUTSPIBYTE(eepromInitData.channel, ((MCHP25LC256_ADDRESS)address).uint8Address[0]);
        GETSPIBYTE(eepromInitData.channel, temp);
        MCHP25LC256CSHigh();
        
        if(*pData != temp)
        {
            ret = 0;
            break;
        }
        pData++;
        address++;
    }
    
    SPIUNLOCK(eepromInitData.channel);
   
    return (ret);
}

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
void DRV_NVM_MCHP25LC256_Read(uint16_t address, uint8_t *pData, uint16_t nCount)
{
    while(!SPILOCK(eepromInitData.channel));

    DRV_SPI_Initialize(eepromInitData.channel, (DRV_SPI_INIT_DATA *)&eepromInitData);
    
    MCHP25LC256CSLow();

    PUTSPIBYTE(eepromInitData.channel, EEPROM_CMD_READ);
    PUTSPIBYTE(eepromInitData.channel, ((MCHP25LC256_ADDRESS) address).uint8Address[1]);
    PUTSPIBYTE(eepromInitData.channel, ((MCHP25LC256_ADDRESS) address).uint8Address[0]);
    
    while(nCount--)
    {
        GETSPIBYTE(eepromInitData.channel, *pData++);
    }

    MCHP25LC256CSHigh();
    SPIUNLOCK(eepromInitData.channel);
}

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
void DRV_NVM_MCHP25LC256_WriteStatusRegister(uint8_t newStatus)
{

    while(!SPILOCK(eepromInitData.channel));
    
    SPIINITIALIZE(eepromInitData.channel, (DRV_SPI_INIT_DATA *)&eepromInitData);
    
    DRV_NVM_MCHP25LC256_WriteEnable();

    MCHP25LC256CSLow();

    // send write status register command
    PUTSPIBYTE(eepromInitData.channel, EEPROM_CMD_WRSR);

    // program the new status bits
    PUTSPIBYTE(eepromInitData.channel, newStatus);

    MCHP25LC256CSHigh();

    // Wait for write end
    while(ReadStatusRegister().Bits.WIP);

    SPIUNLOCK(eepromInitData.channel);
}
