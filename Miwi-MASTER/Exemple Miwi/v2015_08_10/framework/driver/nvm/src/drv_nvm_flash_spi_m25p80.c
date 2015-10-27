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
#include <string.h>
#include "system.h"
#include "driver/nvm/drv_nvm_flash_spi_m25p80.h"

#ifndef  DRV_NVM_M25P80_ChipSelectEnable
    #error "DRV_NVM_M25P80_ChipSelectEnable() is not defined. Please define in system_config.h."
#endif
#ifndef  DRV_NVM_M25P80_ChipSelectDisable
    #error "DRV_NVM_M25P80_ChipSelectDisable() is not defined. Please define in system_config.h."
#endif


// This macro allows verification of programmed data to the flash.
// When this is enabled, additional time is spent on the programming
// of the device and can be removed to reduce programming time.
// Comment this macro out if verification is done by the application or 
// verification is not needed.
//#define ENABLE_M25P80_WRITE_VERIFICATION

// internal type definition
typedef union
{
    struct
    {
        uint8_t uint8Address[4];
    };

    uint32_t uint32Address;
} M25P80_ADDRESS;

typedef union
{
    struct
    {
        uint8_t uint16Byte[2];
    };

    uint16_t uint16FullData;
} M25P80_UINT16;

#define M25P80_WIP_STATUS   (unsigned) 0x01
#define M25P80_WEL_STATUS   (unsigned) 0x02

/************************************************************************
* M25P80 Commands
************************************************************************/
typedef enum {

    M25P80_CMD_READ  = 0x03,         // read memory
    M25P80_CMD_WRITE = 0x02,         // program one data byte

    M25P80_CMD_SER   = 0xD8,         // erase sector (4 KByte of memory)
    M25P80_CMD_ERASE = 0xC7,         // chip erase

    M25P80_CMD_RDSR  = 0x05,         // read status register
    M25P80_CMD_WRSR  = 0x01,         // write status register

    M25P80_CMD_WREN  = 0x06,         // write enable
    M25P80_CMD_WRDI  = 0x04,         // write disable

    M25P80_CMD_RDID  = 0x9F,         // read manufacturer's ID

} DRV_NVM_FLASH_SPI_M25P80_COMMANDS;

static DRV_SPI_INIT_DATA spiInitData;


// internal functions & macros
void    DRV_NVM_M25P80_WriteEnable(void);
void    DRV_NVM_M25P80_WriteByte(uint8_t data, uint32_t address);
uint8_t DRV_NVM_M25P80_ReadByte(uint32_t address);
uint8_t DRV_NVM_M25P80_WriteSector(uint32_t address, uint8_t *pData, uint16_t nCount);
uint8_t DRV_NVM_M25P80_CheckWriteInProgress(void);


#define DRV_NVM_M25P80_IsWriteBusy()        (ReadStatusRegister() & 0x01)
#define SPILOCK(ch)                         DRV_SPI_Lock(ch)
#define SPIUNLOCK(ch)                       DRV_SPI_Unlock(ch)
#define SPIINITIALIZE(initData)             DRV_SPI_Initialize(initData);
#define PUTSPIBYTE(ch, byte)                {                           \
                                                DRV_SPI_Put(ch, byte);  \
                                            }

#define GETSPIBYTE(ch, data)                {                           \
                                                data = DRV_SPI_Get(ch); \
                                            }

/******************************************************************************
  Function:
    void DRV_NVM_M25P80_Initialize( const SYS_MODULE_INIT * const init )

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
void DRV_NVM_M25P80_Initialize(DRV_SPI_INIT_DATA *pInitData)
{
    // initialize the SPI channel to be used
    DRV_SPI_Initialize(pInitData);
    memcpy(&spiInitData, pInitData, sizeof(DRV_SPI_INIT_DATA));

}

/******************************************************************************
  Function:
    void DRV_NVM_M25P80_WriteEnable( void )
    
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
void DRV_NVM_M25P80_WriteEnable(void)
{
    DRV_NVM_M25P80_ChipSelectEnable();
    PUTSPIBYTE(spiInitData.channel, M25P80_CMD_WREN);
    DRV_NVM_M25P80_ChipSelectDisable();
}

/******************************************************************************
  Function:
    static inline __attribute__((__always_inline__)) uint8_t ReadStatusRegister(void)

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
static inline __attribute__((__always_inline__)) uint8_t ReadStatusRegister(void)
{
    uint8_t    temp;

    DRV_NVM_M25P80_ChipSelectEnable();

    PUTSPIBYTE(spiInitData.channel, M25P80_CMD_RDSR);
    GETSPIBYTE(spiInitData.channel, temp);

    DRV_NVM_M25P80_ChipSelectDisable();

    return temp;

}

/******************************************************************************
  Function:
    void DRV_NVM_M25P80_ReadStatusRegister( void )

  Summary:
    Reads the status register of the device.

  Description:
    This function reads the status register of the device.


  Parameters:
    None

  Returns:
    The current value of the status register.
******************************************************************************/
uint8_t DRV_NVM_M25P80_ReadStatusRegister(void)
{
    uint8_t    temp;

    while(!SPILOCK(spiInitData.channel));

    SPIINITIALIZE((DRV_SPI_INIT_DATA *)&spiInitData);
    temp =  ReadStatusRegister();

    SPIUNLOCK(spiInitData.channel);
    return temp;

}

/******************************************************************************
  Function:
    void DRV_NVM_M25P80_WriteByte( uint8_t data,
                                   uint32_t address )

  Summary:
    Writes a byte to the specified address.
    
  Description:
    This routine is internal to this module. This writes a byte to the 
    given address. 

  Parameters:
    data    - byte to be written
    address - location of the programmed byte

  Returns:
    None.
******************************************************************************/
void DRV_NVM_M25P80_WriteByte(uint8_t data, uint32_t address)
{
    DRV_NVM_M25P80_WriteEnable();

    DRV_NVM_M25P80_ChipSelectEnable();

    PUTSPIBYTE(spiInitData.channel, M25P80_CMD_WRITE);
    PUTSPIBYTE(spiInitData.channel, ((M25P80_ADDRESS) address).uint8Address[2]);
    PUTSPIBYTE(spiInitData.channel, ((M25P80_ADDRESS) address).uint8Address[1]);
    PUTSPIBYTE(spiInitData.channel, ((M25P80_ADDRESS) address).uint8Address[0]);
    PUTSPIBYTE(spiInitData.channel, data);

    DRV_NVM_M25P80_ChipSelectDisable();

    // Wait for write end
    while(DRV_NVM_M25P80_IsWriteBusy());

}

/******************************************************************************
  Function:
    uint8_t DRV_NVM_M25P80_ReadByte( uint32_t address )

  Summary:
    Reads a byte from the specified address.
    
  Description:
    This routine is internal to this module. This reads a a byte from the  
    given address. 

  Parameters:
    address - location byte to be read

  Returns:
    read byte 
******************************************************************************/
uint8_t DRV_NVM_M25P80_ReadByte(uint32_t address)
{
    uint8_t    temp;
    
    DRV_NVM_M25P80_ChipSelectEnable();

    PUTSPIBYTE(spiInitData.channel, M25P80_CMD_READ);
    PUTSPIBYTE(spiInitData.channel, ((M25P80_ADDRESS) address).uint8Address[2]);
    PUTSPIBYTE(spiInitData.channel, ((M25P80_ADDRESS) address).uint8Address[1]);
    PUTSPIBYTE(spiInitData.channel, ((M25P80_ADDRESS) address).uint8Address[0]);
    GETSPIBYTE(spiInitData.channel, temp);

    DRV_NVM_M25P80_ChipSelectDisable();

    return (temp);
}

/******************************************************************************
  Function:
    uint8_t DRV_NVM_SST25VF064_WriteSector( uint32_t address,
                                            uint8_t *pData,
                                            uint16_t nCount )

  Summary:
    Performs the sector write of the SPI flash device.

  Description:
    This routine is internal to this module. This is called by the
    DRV_NVM_SST25VF064_Write() function to perform the actual
    programming.

  Parameters:
    address - starting address of the array to be written
    pData   - pointer to the source of the array
    nCount  - specifies the number of bytes to be written

  Returns:
    1 - if the write is successful
    0 - if the write was not successful 
******************************************************************************/
uint8_t DRV_NVM_M25P80_WriteSector(uint32_t address, uint8_t *pData, uint16_t nCount)
{
    uint32_t    addr;
    uint8_t     *pD;
    uint16_t    counter, ret;

    addr = address;
    pD = pData;

    // do a write enable first
    DRV_NVM_M25P80_WriteEnable();

    // set up address
    DRV_NVM_M25P80_ChipSelectEnable();

    PUTSPIBYTE(spiInitData.channel, M25P80_CMD_WRITE);
    PUTSPIBYTE(spiInitData.channel, ((M25P80_ADDRESS)addr).uint8Address[2]);
    PUTSPIBYTE(spiInitData.channel, ((M25P80_ADDRESS)addr).uint8Address[1]);
    PUTSPIBYTE(spiInitData.channel, ((M25P80_ADDRESS)addr).uint8Address[0]);

    for (counter = 0; counter < nCount; counter++)
    {
        PUTSPIBYTE(spiInitData.channel, *pD);
        pD++;
    }
    DRV_NVM_M25P80_ChipSelectDisable();
    // check status of the page write
    while(DRV_NVM_M25P80_IsWriteBusy());

    ret = 1;

#ifdef ENABLE_M25P80_WRITE_VERIFICATION
    // Since data verification takes time,
    // this code is disabled by default
    // to have faster programming time
    for(counter = 0; counter < nCount; counter++)
    {
        if(*pData != DRV_NVM_M25P80_ReadByte(address))
        {
            ret = 0;
            break;
        }
        pData++;
        address++;
    }
#endif

    return (ret);

}

/******************************************************************************
  Function:
    uint8_t DRV_NVM_M25P80_Write(   uint32_t address,
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
uint8_t DRV_NVM_M25P80_Write(uint32_t address, uint8_t *pData, uint16_t nCount)
{

    uint32_t    addr;
    uint8_t     *pD;
    uint16_t    counter, sendCount, ret = 1;

    while(!SPILOCK(spiInitData.channel));

    DRV_SPI_Initialize((DRV_SPI_INIT_DATA *)&spiInitData);

    addr = address;
    pD = pData;

    // check in case previous erase or write is still ongoing
    while(DRV_NVM_M25P80_CheckWriteInProgress() == 1);
    // send write enable command
    DRV_NVM_M25P80_WriteEnable();

    for (counter = 0; counter < nCount; )
    {
        sendCount = 256 - (addr & 0xFF);
        if (sendCount > (nCount - counter))
            sendCount = (nCount - counter);

        ret = DRV_NVM_M25P80_WriteSector(addr, pD, sendCount);
        if (ret == 0)
            break;
        else
        {
            addr    += sendCount;
            pD      += sendCount;
            counter += sendCount;
        }

    }

    SPIUNLOCK(spiInitData.channel);

    return (ret);
    
}

/******************************************************************************
  Function:
    void DRV_NVM_M25P80_Read(   uint32_t address,
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
void DRV_NVM_M25P80_Read(uint32_t address, uint8_t *pData, uint16_t nCount)
{
    while(!SPILOCK(spiInitData.channel));

    DRV_SPI_Initialize((DRV_SPI_INIT_DATA *)&spiInitData);

    DRV_NVM_M25P80_ChipSelectEnable();

    PUTSPIBYTE(spiInitData.channel, M25P80_CMD_READ);
    PUTSPIBYTE(spiInitData.channel, ((M25P80_ADDRESS) address).uint8Address[2]);
    PUTSPIBYTE(spiInitData.channel, ((M25P80_ADDRESS) address).uint8Address[1]);
    PUTSPIBYTE(spiInitData.channel, ((M25P80_ADDRESS) address).uint8Address[0]);

    DRV_SPI_GetBuffer(spiInitData.channel, pData, nCount);

    DRV_NVM_M25P80_ChipSelectDisable();
    SPIUNLOCK(spiInitData.channel);
}
/******************************************************************************
  Function:
    void DRV_NVM_M25P80_ChipErase( )

  Summary:
    Erase the flash device.
    
  Description:
    This routine erases the whole memory of the flash device.
    
  Parameters:
    None

  Returns:
    None
 ******************************************************************************/
void DRV_NVM_M25P80_ChipErase(void)
{
	// reset the BPL bits to be non-write protected, in case they are
    // write protected
    DRV_NVM_M25P80_WriteStatusRegister(0x00);

    while(!SPILOCK(spiInitData.channel));

    SPIINITIALIZE((DRV_SPI_INIT_DATA *)&spiInitData);

    // send write enable command
    DRV_NVM_M25P80_WriteEnable();

    DRV_NVM_M25P80_ChipSelectEnable();
    PUTSPIBYTE(spiInitData.channel, M25P80_CMD_ERASE);
    DRV_NVM_M25P80_ChipSelectDisable();

    // wait for BULK ERASE to be done
    while(DRV_NVM_M25P80_CheckWriteInProgress() == 1);

    SPIUNLOCK(spiInitData.channel);

    // Wait for write end
    while(DRV_NVM_M25P80_IsWriteBusy());

}

/******************************************************************************
  Function:
    void DRV_NVM_M25P80_SectorErase( uint32_t address )

  Summary:
    Erase the sector specified by the given address.
    
  Description:
    This routine erases the sector of where the given address resides.
    
  Parameters:
    None

  Returns:
    None
******************************************************************************/
void DRV_NVM_M25P80_SectorErase(uint32_t address)
{
    // Note: This sector erase do not check for block protection (BP [3:0]).
    //       This function undo the block protection and erases
    //       the sector of the given address.
    DRV_NVM_M25P80_WriteStatusRegister(0x00);
    
    while(!SPILOCK(spiInitData.channel));

    DRV_SPI_Initialize((DRV_SPI_INIT_DATA *)&spiInitData);

    DRV_NVM_M25P80_WriteEnable();

    DRV_NVM_M25P80_ChipSelectEnable();

    PUTSPIBYTE(spiInitData.channel, M25P80_CMD_SER);
    PUTSPIBYTE(spiInitData.channel, ((M25P80_ADDRESS) address).uint8Address[2]);
    PUTSPIBYTE(spiInitData.channel, ((M25P80_ADDRESS) address).uint8Address[1]);
    PUTSPIBYTE(spiInitData.channel, ((M25P80_ADDRESS) address).uint8Address[0]);

    DRV_NVM_M25P80_ChipSelectDisable();

    SPIUNLOCK(spiInitData.channel);

    // Wait for write end
    while(DRV_NVM_M25P80_IsWriteBusy());
}

/******************************************************************************
  Function:
    void DRV_NVM_M25P80_WriteStatusRegister(uint8_t newStatus)

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
void DRV_NVM_M25P80_WriteStatusRegister(uint8_t newStatus)
{

    while(!SPILOCK(spiInitData.channel));

    SPIINITIALIZE((DRV_SPI_INIT_DATA *)&spiInitData);

    // this is the sequence of the status register write
    // M25P80_CMD_WREN must be followed by SST25_CMD_WRSR
    DRV_NVM_M25P80_ChipSelectEnable();

    // send write enable command
    PUTSPIBYTE(spiInitData.channel, M25P80_CMD_WREN);

    DRV_NVM_M25P80_ChipSelectDisable();

    DRV_NVM_M25P80_ChipSelectEnable();

    // send write status register command
    PUTSPIBYTE(spiInitData.channel, M25P80_CMD_WRSR);

    // program the new status bits
    PUTSPIBYTE(spiInitData.channel, newStatus);

    DRV_NVM_M25P80_ChipSelectDisable();

    // Wait for write end
    while(DRV_NVM_M25P80_IsWriteBusy());

    SPIUNLOCK(spiInitData.channel);
}

/******************************************************************************
  Function:
    void DRV_NVM_M25P80_ReadIDRegister( void )

  Summary:
    Reads the ID register of the device.

  Description:
    This function reads the ID of the device.
	
  Parameters:
    Pointer to the buffer for the device ID information. 

  Returns:
    None	
******************************************************************************/
void DRV_NVM_M25P80_ReadIDRegister(uint8_t* pBuffer)
{
    uint8_t     deviceUniqueIDLen = 21;

    while(!SPILOCK(spiInitData.channel));

    SPIINITIALIZE((DRV_SPI_INIT_DATA *)&spiInitData);

    DRV_NVM_M25P80_ChipSelectEnable();
    PUTSPIBYTE(spiInitData.channel, M25P80_CMD_RDID);

    while(deviceUniqueIDLen)
    {
        deviceUniqueIDLen--;
        GETSPIBYTE(spiInitData.channel, *pBuffer++);
    }

    DRV_NVM_M25P80_ChipSelectDisable();

    SPIUNLOCK(spiInitData.channel);

}

/******************************************************************************
  Function:
    uint8_t DRV_NVM_M25P80_CheckWriteInProgress(void)

  Summary:
    Checks the Write In Progress device status.
    
  Description:
    This routine is internal to this module. This reads the status register of 
	the device to check the Write in Progress status bit. 

  Parameters:
    None
	
  Returns:
    1 - if the device's WIP status bit is set
	0 - if the device's WIP status bit is not set
******************************************************************************/
uint8_t DRV_NVM_M25P80_CheckWriteInProgress(void)
{
    uint8_t status;

    // check status of Write in Progress
    DRV_NVM_M25P80_ChipSelectEnable();
    PUTSPIBYTE(spiInitData.channel, M25P80_CMD_RDSR);
    GETSPIBYTE((spiInitData.channel)& M25P80_WIP_STATUS, status);
    DRV_NVM_M25P80_ChipSelectDisable();
    if ((status & M25P80_WIP_STATUS) == 0)
        return 0;
    else
        return 1;

}

