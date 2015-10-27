/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
 spi_eeprom.c

  Summary:
    

  Description:
    Data SPI EEPROM Access Routines

 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) <2014> released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
//DOM-IGNORE-END

#define __SPI_EEPROM_C_

#include "tcpip/tcpip.h"

// If the CS line is not defined, spi_eeprom.c's content will not be compiled.
// If you are using a serial EEPROM please define the CS pin as EEPROM_CS_TRIS
// in system_config.h
#if defined(EEPROM_CS_TRIS)

// IMPORTANT SPI NOTE: The code in this file expects that the SPI interrupt
//      flag (EEPROM_SPI_IF) be clear at all times.  If the SPI is shared with
//      other hardware, the other code should clear the EEPROM_SPI_IF when it is
//      done using the SPI.

// SPI Serial EEPROM buffer size.  To enhance performance while
// cooperatively sharing the SPI bus with other peripherals, bytes
// read and written to the memory are locally buffered. Legal
// sizes are 1 to the EEPROM page size.
#define EEPROM_BUFFER_SIZE  (32)

// Must be the EEPROM write page size, or any binary power of 2 divisor.  If
// using a smaller number, make sure it is at least EEPROM_BUFFER_SIZE big for
// max performance.  Microchip 25LC256 uses 64 byte page size, 25LC1024 uses
// 256 byte page size, so 64 is compatible with both.
#define EEPROM_PAGE_SIZE  (64)

// EEPROM SPI opcodes
#define OPCODE_READ   0x03 // Read data from memory array beginning at selected address
#define OPCODE_WRITE  0x02 // Write data to memory array beginning at selected address
#define OPCODE_WRDI   0x04 // Reset the write enable latch (disable write operations)
#define OPCODE_WREN   0x06 // Set the write enable latch (enable write operations)
#define OPCODE_RDSR   0x05 // Read Status register
#define OPCODE_WRSR   0x01 // Write Status register

#define EEPROM_MAX_SPI_FREQ  (10000000ul) // Hz

#if defined (__XC8)
#define ClearSPIDoneFlag()  {EEPROM_SPI_IF = 0;}
#define WaitForDataByte()   {while(!EEPROM_SPI_IF); EEPROM_SPI_IF = 0;}
#define SPI_ON_BIT          (EEPROM_SPICON1bits.SSPEN)
#elif defined(__XC16)
#define ClearSPIDoneFlag()

static inline __attribute__((__always_inline__)) void WaitForDataByte(void)
{
    while ((EEPROM_SPISTATbits.SPITBF == 1) || (EEPROM_SPISTATbits.SPIRBF == 0));
}

#define SPI_ON_BIT  (EEPROM_SPISTATbits.SPIEN)
#elif defined( __XC32 )
#define ClearSPIDoneFlag()

static inline __attribute__((__always_inline__)) void WaitForDataByte(void)
{
    while (!EEPROM_SPISTATbits.SPITBE || !EEPROM_SPISTATbits.SPIRBF);
}

#define SPI_ON_BIT  (EEPROM_SPICON1bits.ON)
#else
#error Determine SPI flag mechanism
#endif

static void DoWrite(void);

static uint32_t EEPROMAddress;
static uint8_t EEPROMBuffer[EEPROM_BUFFER_SIZE];
static uint8_t vBytesInBuffer;

/*********************************************************************
 * Function:        void XEEInit(unsigned char speed)
 *
 * PreCondition:    None
 *
 * Input:           speed - not used (included for compatibility only)
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Initialize SPI module to communicate to serial
 *                  EEPROM.
 *
 * Note:            Code sets SPI clock to Fosc/16.
 ********************************************************************/
#if (defined(HPC_EXPLORER) || defined(PIC18_EXPLORER)) && !defined(__18F87J10) && !defined(__18F87J11) && !defined(__18F87J50)
#define PROPER_SPICON1  (0x20) /* SSPEN bit is set, SPI in master mode, FOSC/4, IDLE state is low level */
#elif defined(__PIC24F__) || defined(__PIC24FK__)
#define PROPER_SPICON1  (0x0013 | 0x0120) /* 1:1 primary prescale, 4:1 secondary prescale, CKE=1, MASTER mode */
#elif defined(__dsPIC30F__)
#define PROPER_SPICON1  (0x0017 | 0x0120) /* 1:1 primary prescale, 3:1 secondary prescale, CKE=1, MASTER mode */
#elif defined(__dsPIC33F__) || defined(__PIC24H__) || defined (__dsPIC33E__)|| defined(__PIC24E__)
#define PROPER_SPICON1  (0x0003 | 0x0120) /* 1:1 primary prescale, 8:1 secondary prescale, CKE=1, MASTER mode */
#elif defined(__XC32)
#define PROPER_SPICON1  (_SPI2CON_ON_MASK | _SPI2CON_CKE_MASK | _SPI2CON_MSTEN_MASK)
#else
#define PROPER_SPICON1  (0x21) /* SSPEN bit is set, SPI in master mode, FOSC/16, IDLE state is low level */
#endif

void XEEInit(void)
{
    EEPROM_CS_IO = 1;
    EEPROM_CS_TRIS = 0; // Drive SPI EEPROM chip select pin

    EEPROM_SCK_TRIS = 0; // Set SCK pin as an output
    EEPROM_SDI_TRIS = 1; // Make sure SDI pin is an input
    EEPROM_SDO_TRIS = 0; // Set SDO pin as an output

    ClearSPIDoneFlag();
#if defined(__XC16)
    EEPROM_SPICON1 = PROPER_SPICON1; // See PROPER_SPICON1 definition above
    EEPROM_SPICON2 = 0;
    EEPROM_SPISTAT = 0; // clear SPI
    EEPROM_SPISTATbits.SPIEN = 1;
#elif defined(__XC32)
    EEPROM_SPIBRG = (SYS_CLK_FrequencyPeripheralGet() - 1ul) / 2ul / EEPROM_MAX_SPI_FREQ;
    EEPROM_SPICON1 = PROPER_SPICON1;
#elif defined(__XC8)
    EEPROM_SPICON1 = PROPER_SPICON1; // See PROPER_SPICON1 definition above
    EEPROM_SPISTATbits.CKE = 1; // Transmit data on rising edge of clock
    EEPROM_SPISTATbits.SMP = 0; // Input sampled at middle of data output time
#endif
}

/*********************************************************************
 * Function:        XEE_RESULT XEEBeginRead(uint32_t address)
 *
 * PreCondition:    None
 *
 * Input:           address - Address at which read is to be performed.
 *
 * Output:          XEE_SUCCESS
 *
 * Side Effects:    None
 *
 * Overview:        Sets internal address counter to given address.
 *
 * Note:            None
 ********************************************************************/
XEE_RESULT XEEBeginRead(uint32_t address)
{
    // Save the address and emptry the contents of our local buffer
    EEPROMAddress = address;
    vBytesInBuffer = 0;
    return XEE_SUCCESS;
}

/*********************************************************************
 * Function:        uint8_t XEERead(void)
 *
 * PreCondition:    XEEInit() && XEEBeginRead() are already called.
 *
 * Input:           None
 *
 * Output:          uint8_t that was read
 *
 * Side Effects:    None
 *
 * Overview:        Reads next byte from EEPROM; internal address
 *                  is incremented by one.
 *
 * Note:            None
 ********************************************************************/
uint8_t XEERead(void)
{
    // Check if no more bytes are left in our local buffer
    if (vBytesInBuffer == 0u) {
        // Get a new set of bytes
        XEEReadArray(EEPROMAddress, EEPROMBuffer, EEPROM_BUFFER_SIZE);
        EEPROMAddress += EEPROM_BUFFER_SIZE;
        vBytesInBuffer = EEPROM_BUFFER_SIZE;
    }

    // Return a byte from our local buffer
    return EEPROMBuffer[EEPROM_BUFFER_SIZE - vBytesInBuffer--];
}

/*********************************************************************
 * Function:        XEE_RESULT XEEEndRead(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          XEE_SUCCESS
 *
 * Side Effects:    None
 *
 * Overview:        This function does nothing.
 *
 * Note:            Function is used for backwards compatability with
 *                  I2C EEPROM module.
 ********************************************************************/
XEE_RESULT XEEEndRead(void)
{
    return XEE_SUCCESS;
}

/*********************************************************************
 * Function:        XEE_RESULT XEEReadArray(uint32_t address,
 *                                          uint8_t *buffer,
 *                                          uint16_t length)
 *
 * PreCondition:    XEEInit() is already called.
 *
 * Input:           address     - Address from where array is to be read
 *                  buffer      - Caller supplied buffer to hold the data
 *                  length      - Number of bytes to read.
 *
 * Output:          XEE_SUCCESS
 *
 * Side Effects:    None
 *
 * Overview:        Reads desired number of bytes in sequential mode.
 *                  This function performs all necessary steps
 *                  and releases the bus when finished.
 *
 * Note:            None
 ********************************************************************/
XEE_RESULT XEEReadArray(uint32_t address,
        uint8_t *buffer,
        uint16_t length)
{
    volatile uint8_t Dummy;
    uint8_t vSPIONSave;
#if defined(__XC8)
    uint8_t SPICON1Save;
#elif defined(__XC16)
    uint16_t SPICON1Save;
#else
    uint32_t SPICON1Save;
#endif

    // Save SPI state (clock speed)
    SPICON1Save = EEPROM_SPICON1;
    vSPIONSave = SPI_ON_BIT;

    // Configure SPI
    SPI_ON_BIT = 0;
    EEPROM_SPICON1 = PROPER_SPICON1;
    SPI_ON_BIT = 1;

    EEPROM_CS_IO = 0;

    // Send READ opcode
    EEPROM_SSPBUF = OPCODE_READ;
    WaitForDataByte();
    Dummy = EEPROM_SSPBUF;

    // Send address
#if defined(USE_EEPROM_25LC1024)
    EEPROM_SSPBUF = ((TCPIP_UINT32_VAL *) & address)->v[2];
    WaitForDataByte();
    Dummy = EEPROM_SSPBUF;
#endif

    EEPROM_SSPBUF = ((TCPIP_UINT32_VAL *) & address)->v[1];
    WaitForDataByte();
    Dummy = EEPROM_SSPBUF;

    EEPROM_SSPBUF = ((TCPIP_UINT32_VAL *) & address)->v[0];
    WaitForDataByte();
    Dummy = EEPROM_SSPBUF;

    while (length--) {
        EEPROM_SSPBUF = 0;
        WaitForDataByte();
        Dummy = EEPROM_SSPBUF;
        if (buffer != NULL)
            *buffer++ = Dummy;
    };

    EEPROM_CS_IO = 1;

    // Restore SPI state
    SPI_ON_BIT = 0;
    EEPROM_SPICON1 = SPICON1Save;
    SPI_ON_BIT = vSPIONSave;

    return XEE_SUCCESS;
}

/*********************************************************************
 * Function:        XEE_RESULT XEEBeginWrite(uint32_t address)
 *
 * PreCondition:    None
 *
 * Input:           address     - address to be set for writing
 *
 * Output:          XEE_SUCCESS
 *
 * Side Effects:    None
 *
 * Overview:        Modifies internal address counter of EEPROM.
 *
 * Note:            Unlike XEESetAddr() in xeeprom.c for I2C EEPROM
 *                  memories, this function is used only for writing
 *                  to the EEPROM.  Reads must use XEEBeginRead(),
 *                  XEERead(), and XEEEndRead().
 *                  This function does not use the SPI bus.
 ********************************************************************/
XEE_RESULT XEEBeginWrite(uint32_t address)
{
    vBytesInBuffer = 0;
    EEPROMAddress = address;
    return XEE_SUCCESS;
}

/*********************************************************************
 * Function:        XEE_RESULT XEEWrite(uint8_t val)
 *
 * PreCondition:    XEEInit() && XEEBeginWrite() are already called.
 *
 * Input:           val - Byte to be written
 *
 * Output:          XEE_SUCCESS
 *
 * Side Effects:    None
 *
 * Overview:        Writes a byte to the write cache, and if full,
 *                  commits the write.  Also, if a write boundary is
 *                  reached the write is committed.  When finished
 *                  writing, XEEEndWrite() must be called to commit
 *                  any unwritten bytes from the write cache.
 *
 * Note:            None
 ********************************************************************/
XEE_RESULT XEEWrite(uint8_t val)
{
    EEPROMBuffer[vBytesInBuffer++] = val;
    if (vBytesInBuffer >= sizeof (EEPROMBuffer))
        DoWrite();
    else if ((((uint8_t) EEPROMAddress + vBytesInBuffer) & (EEPROM_PAGE_SIZE - 1)) == 0u)
        DoWrite();

    return XEE_SUCCESS;
}

/*****************************************************************************
  Function:
    XEE_RESULT XEEWriteArray(uint8_t *val, uint16_t wLen)

  Summary:
    Writes an array of bytes to the EEPROM part.

  Description:
    This function writes an array of bytes to the EEPROM at the address
    specified when XEEBeginWrite() was called.  Page boundary crossing is
    handled internally.

  Precondition:
    XEEInit() was called once and XEEBeginWrite() was called.

  Parameters:
    vData - The array to write to the next memory location
    wLen - The length of the data to be written

  Returns:
    None

  Remarks:
    The internal write cache is flushed at completion, so it is unnecessary
    to call XEEEndWrite() after calling this function.  However, if you do
    so, no harm will be done.
 ***************************************************************************/
void XEEWriteArray(uint8_t *val, uint16_t wLen)
{
    while (wLen--)
        XEEWrite(*val++);

    XEEEndWrite();
}

/*********************************************************************
 * Function:        XEE_RESULT XEEEndWrite(void)
 *
 * PreCondition:    XEEInit() && XEEBeginWrite() are already called.
 *
 * Input:           None
 *
 * Output:          XEE_SUCCESS
 *
 * Side Effects:    None
 *
 * Overview:        Commits any last uncommitted bytes in cache to
 *                  physical storage.
 *
 * Note:            Call this function when you no longer need to
 *                  write any more bytes at the selected address.
 ********************************************************************/
XEE_RESULT XEEEndWrite(void)
{
    if (vBytesInBuffer)
        DoWrite();

    return XEE_SUCCESS;
}

static void DoWrite(void)
{
    uint8_t i;
    uint8_t vSPIONSave;
#if defined(__XC8)
    uint8_t SPICON1Save;
#elif defined(__XC16)
    uint16_t SPICON1Save;
#else
    uint32_t SPICON1Save;
#endif

    // Save SPI state
    SPICON1Save = EEPROM_SPICON1;
    vSPIONSave = SPI_ON_BIT;

    // Configure SPI
    SPI_ON_BIT = 0;
    EEPROM_SPICON1 = PROPER_SPICON1;
    SPI_ON_BIT = 1;

    // Set the Write Enable latch
    EEPROM_CS_IO = 0;
    EEPROM_SSPBUF = OPCODE_WREN;
    WaitForDataByte();
    EEPROM_SSPBUF;
    EEPROM_CS_IO = 1;

    // Send WRITE opcode
    EEPROM_CS_IO = 0;
    EEPROM_SSPBUF = OPCODE_WRITE;
    WaitForDataByte();
    EEPROM_SSPBUF;

    // Send address
#if defined(USE_EEPROM_25LC1024)
    EEPROM_SSPBUF = ((TCPIP_UINT32_VAL*) & EEPROMAddress)->v[2];
    WaitForDataByte();
    EEPROM_SSPBUF;
#endif

    EEPROM_SSPBUF = ((TCPIP_UINT32_VAL*) & EEPROMAddress)->v[1];
    WaitForDataByte();
    EEPROM_SSPBUF;

    EEPROM_SSPBUF = ((TCPIP_UINT32_VAL*) & EEPROMAddress)->v[0];
    WaitForDataByte();
    EEPROM_SSPBUF;

    for (i = 0; i < vBytesInBuffer; i++) {
        // Send the byte to write
        EEPROM_SSPBUF = EEPROMBuffer[i];
        WaitForDataByte();
        EEPROM_SSPBUF;
    }

    // Begin the write
    EEPROM_CS_IO = 1;

    // Update write address and clear write cache
    EEPROMAddress += vBytesInBuffer;
    vBytesInBuffer = 0;

    // Restore SPI State
    SPI_ON_BIT = 0;
    EEPROM_SPICON1 = SPICON1Save;
    SPI_ON_BIT = vSPIONSave;

    // Wait for write to complete
    while (XEEIsBusy());
}

/*********************************************************************
 * Function:        bool XEEIsBusy(void)
 *
 * PreCondition:    XEEInit() is already called.
 *
 * Input:           None
 *
 * Output:          false if EEPROM is not busy
 *                  true if EEPROM is busy
 *
 * Side Effects:    None
 *
 * Overview:        Reads the status register
 *
 * Note:            None
 ********************************************************************/
bool XEEIsBusy(void)
{
    volatile TCPIP_UINT8_VAL result;
    uint8_t vSPIONSave;
#if defined(__XC8)
    uint8_t SPICON1Save;
#elif defined(__XC16)
    uint16_t SPICON1Save;
#else
    uint32_t SPICON1Save;
#endif

    // Save SPI state
    SPICON1Save = EEPROM_SPICON1;
    vSPIONSave = SPI_ON_BIT;

    // Configure SPI
    SPI_ON_BIT = 0;
    EEPROM_SPICON1 = PROPER_SPICON1;
    SPI_ON_BIT = 1;

    EEPROM_CS_IO = 0;
    // Send RDSR - Read Status Register opcode
    EEPROM_SSPBUF = OPCODE_RDSR;
    WaitForDataByte();
    result.Val = EEPROM_SSPBUF;

    // Get register contents
    EEPROM_SSPBUF = 0;
    WaitForDataByte();
    result.Val = EEPROM_SSPBUF;
    EEPROM_CS_IO = 1;

    // Restore SPI State
    SPI_ON_BIT = 0;
    EEPROM_SPICON1 = SPICON1Save;
    SPI_ON_BIT = vSPIONSave;

    return result.bits.b0;
}

#endif // #if defined(EEPROM_CS_TRIS)
