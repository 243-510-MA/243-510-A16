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
#include "driver/nvm/nvm_flash_spi_sst26vf0xxb.h"

// NOTE: This driver is written to support SPI Mode only of
//       SST26VF0XXB devices

#ifndef  NVM_SST26_ChipSelectEnable
    #error "NVM_SST26_ChipSelectEnable() is not defined. Please define in system_config.h."
#endif
#ifndef  NVM_SST26_ChipSelectDisable
    #error "NVM_SST26_ChipSelectDisable() is not defined. Please define in system_config.h."
#endif


// This macro allows verification of programmed data to the flash.
// When this is enabled, additional time is spent on the programming
// of the device and can be removed to reduce programming time.
// Comment this macro out if verification is done by the application or 
// verification is not needed.
//#define SST26VF0XXB_CONFIG_WRITE_VERIFICATION_ENABLE

// internal type definition
typedef union
{
    struct
    {
        uint8_t uint8Address[4];
    };

    uint32_t uint32Address;
} SST26_ADDRESS;

typedef union
{
    struct
    {
        uint8_t uint16Byte[2];
    };

    uint16_t uint16FullData;
} SST26_UINT16;

/************************************************************************
* SST26 Commands
************************************************************************/
typedef enum {

    SST26_CMD_READ  = 0x03,         // read memory
    SST26_CMD_WRITE = 0x02,         // program a page (can be any number between 1 - 256 bytes)

    SST26_CMD_SER   = 0x20,         // erase sector (4 KByte of memory)
    SST26_CMD_ERASE = 0xC7,         // chip erase

    SST26_CMD_RDSR  = 0x05,         // read status register
    SST26_CMD_WRSR  = 0x01,         // write status register

    SST26_CMD_WREN  = 0x06,         // write enable
    SST26_CMD_WRDI  = 0x04,         // write disable
    
    SST26_CMD_RSTEN = 0x66,         // reset enable
    SST26_CMD_RST   = 0x99,         // reset command
    SST26_CMD_RSTQIO = 0xFF,        // reset mode to SPI

    SST26_CMD_WBPR  = 0x42,         // write to block protect register
    SST26_CMD_LBPR  = 0x8D,         // global block protection lock command
    SST26_CMD_ULBPR = 0x98,         // global block protection unlock command
    SST26_CMD_NVWLDR = 0xE8,        // non-volatile write lockdown register write command

    SST26_CMD_JEDECID = 0x9F        // JEDEC ID read command
    
} NVM_FLASH_SPI_SST26_COMMANDS;

static DRV_SPI_INIT_DATA spiInitData;

// internal functions & macros
void    NVM_SST26VF0XXB_WriteEnable(void);
void    NVM_SST26VF0XXB_WriteByte(uint8_t data, uint32_t address);
uint8_t NVM_SST26VF0XXB_ReadByte(uint32_t address);
uint8_t NVM_SST26VF0XXB_WriteSector(uint32_t address, uint8_t *pData, uint16_t nCount);

void NVM_SST26VF0XXB_SPIModeSet(void);


#define NVM_SST26VF0XXB_IsWriteBusy()        (ReadStatusRegister() & 0x80)
#define SPILOCK(ch)                         DRV_SPI_Lock(ch)
#define SPIUNLOCK(ch)                       DRV_SPI_Unlock(ch)
#define SPIINITIALIZE(initData)             DRV_SPI_Initialize(initData);

/******************************************************************************
  Function:
    void NVM_SST26VF0XXB_Initialize( const SYS_MODULE_INIT * const init )

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
void NVM_SST26VF0XXB_Initialize(DRV_SPI_INIT_DATA *pInitData)
{
    // initialize the SPI channel to be used
    SPIINITIALIZE(pInitData);
    memcpy(&spiInitData, pInitData, sizeof(DRV_SPI_INIT_DATA));

    NVM_SST26VF0XXB_SPIModeSet();

}

/******************************************************************************
  Function:
    void NVM_SST26VF0XXB_WriteEnable( void )
    
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
void NVM_SST26VF0XXB_WriteEnable(void)
{
    NVM_SST26_ChipSelectEnable();
    DRV_SPI_Put(spiInitData.channel, SST26_CMD_WREN);
    NVM_SST26_ChipSelectDisable();
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

    NVM_SST26_ChipSelectEnable();

    DRV_SPI_Put(spiInitData.channel, SST26_CMD_RDSR);
    temp = DRV_SPI_Get(spiInitData.channel);

    NVM_SST26_ChipSelectDisable();
    
    return temp;

}


/******************************************************************************
  Function:
    void NVM_SST26VF0XXB_ReadStatusRegister( void )
    
  Summary:
    Reads the status register of the device.
    
  Description:
    This function reads the status register of the device.


  Parameters:
    None

  Returns:
    The current value of the status register.
******************************************************************************/
uint8_t NVM_SST26VF0XXB_ReadStatusRegister(void)
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
    void NVM_SST26VF0XXB_WriteByte( uint8_t data,
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
void NVM_SST26VF0XXB_WriteByte(uint8_t data, uint32_t address)
{
    NVM_SST26VF0XXB_WriteEnable();

    NVM_SST26_ChipSelectEnable();

    DRV_SPI_Put(spiInitData.channel, SST26_CMD_WRITE);
    DRV_SPI_Put(spiInitData.channel, ((SST26_ADDRESS)address).uint8Address[2]);
    DRV_SPI_Put(spiInitData.channel, ((SST26_ADDRESS)address).uint8Address[1]);
    DRV_SPI_Put(spiInitData.channel, ((SST26_ADDRESS)address).uint8Address[0]);
    DRV_SPI_Put(spiInitData.channel, data);

    NVM_SST26_ChipSelectDisable();

    // Wait for write end
    while(NVM_SST26VF0XXB_IsWriteBusy());
    
}

/******************************************************************************
  Function:
    uint8_t NVM_SST26VF0XXB_ReadByte( uint32_t address )

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
uint8_t NVM_SST26VF0XXB_ReadByte(uint32_t address)
{
    uint8_t temp;
    
    NVM_SST26_ChipSelectEnable();

    DRV_SPI_Put(spiInitData.channel, SST26_CMD_READ);
    DRV_SPI_Put(spiInitData.channel, ((SST26_ADDRESS)address).uint8Address[2]);
    DRV_SPI_Put(spiInitData.channel, ((SST26_ADDRESS)address).uint8Address[1]);
    DRV_SPI_Put(spiInitData.channel, ((SST26_ADDRESS)address).uint8Address[0]);
    temp = DRV_SPI_Get(spiInitData.channel);

    NVM_SST26_ChipSelectDisable();

    return (temp);
}

/******************************************************************************
  Function:
    uint8_t NVM_SST26VF0XXB_WriteSector( uint32_t address,
                                            uint8_t *pData,
                                            uint16_t nCount )

  Summary:
    Performs the sector write of the SPI flash device.

  Description:
    This routine is internal to this module. This is called by the
    NVM_SST26VF0XXB_Write() function to perform the actual
    programming.

  Parameters:
    address - starting address of the array to be written
    pData   - pointer to the source of the array
    nCount  - specifies the number of bytes to be written

  Returns:
    1 - if the write is successful
    0 - if the write was not successful 
******************************************************************************/
uint8_t NVM_SST26VF0XXB_WriteSector(uint32_t address, uint8_t *pData, uint16_t nCount)
{
    uint32_t    addr;
    uint8_t     *pD;
    uint16_t    counter, ret;

    addr = address;
    pD = pData;

    // do a write enable first
    NVM_SST26VF0XXB_WriteEnable();

    // set up address
    NVM_SST26_ChipSelectEnable();

    DRV_SPI_Put(spiInitData.channel, SST26_CMD_WRITE);
    DRV_SPI_Put(spiInitData.channel, ((SST26_ADDRESS)addr).uint8Address[2]);
    DRV_SPI_Put(spiInitData.channel, ((SST26_ADDRESS)addr).uint8Address[1]);
    DRV_SPI_Put(spiInitData.channel, ((SST26_ADDRESS)addr).uint8Address[0]);

    for (counter = 0; counter < nCount; counter++)
    {
        DRV_SPI_Put(spiInitData.channel, *pD);
        pD++;
    }
    NVM_SST26_ChipSelectDisable();
    // check status of the page write
    while(NVM_SST26VF0XXB_IsWriteBusy());

    // Write Disable
    NVM_SST26_ChipSelectEnable();

    DRV_SPI_Put(spiInitData.channel, SST26_CMD_WRDI);

    NVM_SST26_ChipSelectDisable();

    ret = 1;

#ifdef SST26VF0XXB_CONFIG_WRITE_VERIFICATION_ENABLE
    // Since data verification takes time,
    // this code is disabled by default
    // to have faster programming time
    for(counter = 0; counter < nCount; counter++)
    {
        if(*pData != NVM_SST26VF0XXB_ReadByte(address))
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
    uint8_t NVM_SST26VF0XXB_Write(   uint32_t address,
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
uint8_t NVM_SST26VF0XXB_Write(uint32_t address, uint8_t *pData, uint16_t nCount)
{

    uint32_t    addr;
    uint8_t     *pD;
    uint16_t    counter, sendCount, ret = 0;

    while(!SPILOCK(spiInitData.channel));

    DRV_SPI_Initialize((DRV_SPI_INIT_DATA *)&spiInitData);

    addr = address;
    pD = pData;

    for (counter = 0; counter < nCount; )
    {
        sendCount = 256 - (addr & 0xFF);
        if (sendCount > (nCount - counter))
            sendCount = (nCount - counter);

        ret = NVM_SST26VF0XXB_WriteSector(addr, pD, sendCount);
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
    void NVM_SST26VF0XXB_Read(   uint32_t address,
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
void NVM_SST26VF0XXB_Read(uint32_t address, uint8_t *pData, uint16_t nCount)
{
    while(!SPILOCK(spiInitData.channel));

    SPIINITIALIZE((DRV_SPI_INIT_DATA *)&spiInitData);

    NVM_SST26_ChipSelectEnable();

    DRV_SPI_Put(spiInitData.channel, SST26_CMD_READ);
    DRV_SPI_Put(spiInitData.channel, ((SST26_ADDRESS)address).uint8Address[2]);
    DRV_SPI_Put(spiInitData.channel, ((SST26_ADDRESS)address).uint8Address[1]);
    DRV_SPI_Put(spiInitData.channel, ((SST26_ADDRESS)address).uint8Address[0]);

    DRV_SPI_GetBuffer(spiInitData.channel, pData, nCount);

    NVM_SST26_ChipSelectDisable();
    SPIUNLOCK(spiInitData.channel);
}

/******************************************************************************
  Function:
    void NVM_SST26VF0XXB_ChipErase( )

  Summary:
    Erase the flash device.
    
  Description:
    This routine erases the whole memory of the flash device.
    
  Parameters:
    None

  Returns:
    None
 ******************************************************************************/
void NVM_SST26VF0XXB_ChipErase(void)
{

    while(!SPILOCK(spiInitData.channel));
    SPIINITIALIZE((DRV_SPI_INIT_DATA *)&spiInitData);

    // unlock the global block protection
    NVM_SST26VF0XXB_WriteEnable();

    NVM_SST26_ChipSelectEnable();
    DRV_SPI_Put(spiInitData.channel, SST26_CMD_ULBPR);
    NVM_SST26_ChipSelectDisable();

    // perform the erase
    NVM_SST26VF0XXB_WriteEnable();

    NVM_SST26_ChipSelectEnable();
    DRV_SPI_Put(spiInitData.channel, SST26_CMD_ERASE);
    NVM_SST26_ChipSelectDisable();

    // Wait for write end
    while(NVM_SST26VF0XXB_IsWriteBusy());

    SPIUNLOCK(spiInitData.channel);

}

/******************************************************************************
  Function:
    void NVM_SST26VF0XXB_SectorErase( uint32_t address )

  Summary:
    Erase the sector specified by the given address.
    
  Description:
    This routine erases the sector of where the given address resides.
    
  Parameters:
    None

  Returns:
    None
******************************************************************************/
void NVM_SST26VF0XXB_SectorErase(uint32_t address)
{
    while(!SPILOCK(spiInitData.channel));
    SPIINITIALIZE((DRV_SPI_INIT_DATA *)&spiInitData);

    NVM_SST26VF0XXB_WriteEnable();
    
    // Note: This sector erase do not check for block protection.
    //       This function undo the block protection and erases
    //       the sector of the given address.
    NVM_SST26_ChipSelectEnable();
    DRV_SPI_Put(spiInitData.channel, SST26_CMD_ULBPR);
    NVM_SST26_ChipSelectDisable();
    
    NVM_SST26VF0XXB_WriteEnable();

    NVM_SST26_ChipSelectEnable();

    DRV_SPI_Put(spiInitData.channel, SST26_CMD_SER);
    DRV_SPI_Put(spiInitData.channel, ((SST26_ADDRESS)address).uint8Address[2]);
    DRV_SPI_Put(spiInitData.channel, ((SST26_ADDRESS)address).uint8Address[1]);
    DRV_SPI_Put(spiInitData.channel, ((SST26_ADDRESS)address).uint8Address[0]);

    NVM_SST26_ChipSelectDisable();
    
    // Wait for write end
    while(NVM_SST26VF0XXB_IsWriteBusy());

    SPIUNLOCK(spiInitData.channel);

}

/******************************************************************************
  Function:
    void NVM_SST26VF0XXB_WriteStatusRegister(uint8_t newStatus)

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
void NVM_SST26VF0XXB_WriteStatusRegister(uint8_t newStatus)
{

    while(!SPILOCK(spiInitData.channel));
    
    SPIINITIALIZE((DRV_SPI_INIT_DATA *)&spiInitData);

    NVM_SST26_ChipSelectEnable();

    // send write status register command
    DRV_SPI_Put(spiInitData.channel, SST26_CMD_WRSR);

    // send dummy data
    DRV_SPI_Put(spiInitData.channel, 0x00);

    // program the new configuration
    DRV_SPI_Put(spiInitData.channel, newStatus);

    NVM_SST26_ChipSelectDisable();

    // Wait for write end
    while(NVM_SST26VF0XXB_IsWriteBusy());

    SPIUNLOCK(spiInitData.channel);
}

/******************************************************************************
  Function:
    void NVM_SST26VF0XXB_Reset(void)


  Summary:
    Reset the SST26VF0XXB device.

  Description:
    This routine resets the SPI Flash module. Any ongoing writes may
    result in corrupted data.

  Parameters:
    none

  Returns:
    none
 ******************************************************************************/
void NVM_SST26VF0XXB_Reset(void)
{

    while(!SPILOCK(spiInitData.channel));

    DRV_SPI_Initialize((DRV_SPI_INIT_DATA *)&spiInitData);

    NVM_SST26_ChipSelectEnable();

    // send reset enable command
    DRV_SPI_Put(spiInitData.channel, SST26_CMD_RSTEN);

    // send reset command
    DRV_SPI_Put(spiInitData.channel, SST26_CMD_RST);

    NVM_SST26_ChipSelectDisable();

    SPIUNLOCK(spiInitData.channel);

    return;

}

/******************************************************************************
  Function:
    void NVM_SST26VF0XXB_SPIModeSet(void)


  Summary:
    Set SST26VF0XXB device to SPI mode.

  Description:
    This routine sets the SPI Flash module to use SPI mode.

  Parameters:
    none

  Returns:
    none
 ******************************************************************************/
void NVM_SST26VF0XXB_SPIModeSet(void)
{

    while(!SPILOCK(spiInitData.channel));

    DRV_SPI_Initialize((DRV_SPI_INIT_DATA *)&spiInitData);

    NVM_SST26_ChipSelectEnable();
    // send reset enable command
    DRV_SPI_Put(spiInitData.channel, SST26_CMD_RSTQIO);
    NVM_SST26_ChipSelectDisable();
    
    SPIUNLOCK(spiInitData.channel);

    return;

}

/******************************************************************************
  Function:
    void NVM_SST26VF0XXB_JEDEC_ID_Check(
                                uint8_t *pManufacturer_Id, 
                                uint8_t *pDevice_Type, 
                                uint8_t *pDevice_Id)

  Summary:
    Read the Joint Electron Device Engineering Council (JEDEC)
    ID.

  Description:
    This routine reads the ID for the device assigned by Joint 
    Electron Device Engineering Council (JEDEC).

  Parameters:
    pManufacturer_Id - the pointer to the manufacturing ID variable.
    pDevice_Type - the pointer to the device type variable.
    pDevice_Id - the pointer to the device ID variable.

  Returns:
    none
 ******************************************************************************/
void NVM_SST26VF0XXB_JEDEC_ID_Check(uint8_t *pManufacturer_Id, uint8_t *pDevice_Type, uint8_t *pDevice_Id)
{
 
    NVM_SST26_ChipSelectEnable();
    // send JEDEC ID command
    DRV_SPI_Put(spiInitData.channel, SST26_CMD_JEDECID);
    *pManufacturer_Id = DRV_SPI_Get(spiInitData.channel);
    *pDevice_Type = DRV_SPI_Get(spiInitData.channel);
    *pDevice_Id = DRV_SPI_Get(spiInitData.channel);

    /*  
        for SST25VF064B
        Manufacturer_Id = 0xBF
        Device_Type = 0x26
        Device_Id = 0x43;      
     
        for SST25VF032B
        Manufacturer_Id = 0xBF
        Device_Type = 0x26
        Device_Id = 0x42;

        for SST25VF016B
        Manufacturer_Id = 0xBF
        Device_Type = 0x26
        Device_Id = 0x41;

    */


    NVM_SST26_ChipSelectDisable();

}
