/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_wifi_spi.c

  Summary:
    Module for Microchip TCP/IP Stack

  Description:
    MRF24W Driver SPI Interface Routines

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

/*******************************************************************************
 *                              INCLUDES
 *******************************************************************************/

#include "tcpip/tcpip.h"

#if defined(WF_CS_TRIS)

/*******************************************************************************
 *                              DEFINES
 *******************************************************************************/

/* used for assertions */
#if defined(WF_DEBUG)
#define WF_MODULE_NUMBER WF_MODULE_WF_SPI
#endif

/* Indicate here if the MRF24W is sharing the SPI bus with another device.  For the */
/* Microchip demos only the PIC18 on the PICDEM.net 2 board shares the SPI bus.        */
#if defined(__XC8)
#define SPI_IS_SHARED
#endif

#if defined(SPI_IS_SHARED)
/* SPI context save variables */
static uint8_t SPIONSave;
#if defined( __XC8)
static uint8_t SPICON1Save;
static uint8_t SPISTATSave;
#elif defined( __XC16 )
static uint16_t SPICON1Save;
static uint16_t SPICON2Save;
static uint16_t SPISTATSave;
#elif defined( __XC32 )
static uint32_t SPICON1Save;
static uint32_t SPISTATSave;
#else
#error Cannot define SPI context save variables.
#endif
#endif /* SPI_IS_SHARED */

/*******************************************************************************
 *                              SPI Definitions
 *******************************************************************************/

#if defined (__XC8)
#define ClearSPIDoneFlag()  {WF_SPI_IF = 0;}
#define WaitForDataByte()   {while (!WF_SPI_IF); WF_SPI_IF = 0;}
#define SPI_ON_BIT          (WF_SPICON1bits.SSPEN)
#elif defined(__XC16)
#define ClearSPIDoneFlag()

static inline __attribute__((__always_inline__)) void WaitForDataByte(void)
{
    while ((WF_SPISTATbits.SPITBF == 1) || (WF_SPISTATbits.SPIRBF == 0));
}

#define SPI_ON_BIT          (WF_SPISTATbits.SPIEN)
#elif defined( __XC32 )
#define ClearSPIDoneFlag()

static inline __attribute__((__always_inline__)) void WaitForDataByte(void)
{
    while (!WF_SPISTATbits.SPITBE || !WF_SPISTATbits.SPIRBF);
}

#define SPI_ON_BIT (WF_SPICON1bits.ON)
#else
#error Determine SPI flag mechanism
#endif

/*******************************************************************************
 *                            LOCAL FUNCTION PROTOTYPES
 ********************************************************************************/

#if defined (SPI_IS_SHARED)
static void SaveSpiContext(void);
static void RestoreSpiContext(void);
#endif
static void ConfigureSpiMRF24W(void);

/*******************************************************************************
  Function:
    void WF_SpiInit(void)

  Summary:
    Initializes the SPI interface to the MRF24W device.

  Description:
    Configures the SPI interface for communications with the MRF24W.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    This function is called by WFHardwareInit.
 *******************************************************************************/
void WF_SpiInit(void)
{
    /* disable the spi interrupt */
#if defined( __XC32 )
    WF_SPI_IE_CLEAR = WF_SPI_INT_BITS;
#else
    WF_SPI_IE = 0;
#endif
#if defined( __XC8)
    WF_SPI_IP = 0;
#endif

    // Set up the SPI module on the PIC for communications with the MRF24W
    WF_CS_IO = 1;
    WF_CS_TRIS = 0; // Drive SPI MRF24W chip select pin
#if defined( __XC8)
    WF_SCK_TRIS = 0; /* SPI Clock is an output */
    WF_SDO_TRIS = 0; /* SPI Data Out is an output */
    WF_SDI_TRIS = 1; /* SPI Data In is an input */
#else
    // We'll let the module control the pins.
#endif

#if !defined( SPI_IS_SHARED )
    ConfigureSpiMRF24W();
#endif

    /* clear the completion flag */
    ClearSPIDoneFlag();
}

/*
  PIC32 SPI clock speed:
  ---------------------
    Fsck =        Fpb
           ------------------
           2 * (SPIxBRG + 1)

    Note that the maximum possible baud rate is
    Fpb/2 (SPIXBRG = 0) and the minimum possible baud
    rate is Fpb /1024.
 */

/*******************************************************************************
  Function:
    void ConfigureSpiMRF24W(void)

  Summary:
    Configures the SPI interface to the MRF24W.

  Description:
    Configures the SPI interface for communications with the MRF24W.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    1) If the SPI bus is shared with other peripherals this function is called
    each time an SPI transaction occurs by WF_SpiEnableChipSelect.  Otherwise it
    is called once during initialization by WF_SpiInit.

    2) Maximum SPI clock rate for the MRF24W is 25MHz.
 *******************************************************************************/
static void ConfigureSpiMRF24W(void)
{
    /*----------------------------------------------------------------*/
    /* After we save context, configure SPI for MRF24W communications */
    /*----------------------------------------------------------------*/
    /* enable the SPI clocks          */
    /* set as master                  */
    /* clock idles high               */
    /* ms bit first                   */
    /* 8 bit tranfer length           */
    /* data changes on falling edge   */
    /* data is sampled on rising edge */
    /* set the clock divider          */
#if defined(__XC8)
#if defined(PICDEMNET2)
#if defined(MRF24WG)
    WF_SPICON1 = 0x20; // SSPEN bit is set, SPI in master mode, (0x30 is for FOSC/4),
    // IDLE state is low level (0x32 is for FOSC/64)
    WF_SPISTATbits.CKE = 1; // Transmit data on falling edge of clock
    WF_SPISTATbits.SMP = 1; // Input sampled at end of data output time
#else
    WF_SPICON1 = 0x30; // SSPEN bit is set, SPI in master mode, (0x30 is for FOSC/4),
    // IDLE state is high level (0x32 is for FOSC/64)
    WF_SPISTATbits.CKE = 0; // Transmit data on falling edge of clock
    WF_SPISTATbits.SMP = 1; // Input sampled at end? of data output time
#endif
#else
    WF_SPICON1 = 0x10; // Disable SPI Master (SSPEN = 0), FOSC/4, idle state for clock = low),
    WF_SPISTATbits.CKE = 0; // Transmit data on falling edge of clock
    WF_SPISTATbits.SMP = 1; // Input sampled at end of data output time
    WF_SPICON1 = 0x30; // Now set SSPEN (SPI in master mode)
#endif
#elif defined(__XC16)
    WF_SPICON1 = 0x027B; // Fcy Primary prescaler 1:1, secondary prescaler 2:1, CKP = 1, CKE = 0, SMP = 1
    WF_SPICON2 = 0x0000;
    WF_SPISTAT = 0x8000; // Enable the module
#elif defined( __XC32 )
    WF_SPI_BRG = ((SYS_CLK_FrequencyPeripheralGet() / 2) / WF_MAX_SPI_FREQ) - 1;
    WF_SPICON1 = 0x00000260; // sample at end, data change idle to active, clock idle high, master
    WF_SPICON1bits.ON = 1;
#else
#error Configure SPI for the selected processor
#endif
}

/*******************************************************************************
  Function:
    void WF_SpiEnableChipSelect(void)

  Summary:
    Enables the MRF24W SPI chip select.

  Description:
    Enables the MRF24W SPI chip select as part of the sequence of SPI
    communications.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    If the SPI bus is shared with other peripherals then the current SPI context
    is saved.
 *******************************************************************************/
void WF_SpiEnableChipSelect(void)
{
#if defined(__XC8)
    static volatile uint8_t dummy;
#endif

#if defined(SPI_IS_SHARED)
    SaveSpiContext();
    ConfigureSpiMRF24W();
#endif

    /* set Slave Select low (enable SPI chip select on MRF24W) */
    WF_CS_IO = 0;

    /* clear any pending interrupts */
#if defined(__XC8)
    dummy = WF_SSPBUF;
    ClearSPIDoneFlag();
#endif
}

/*******************************************************************************
  Function:
    void WF_SpiDisableChipSelect(void)

  Summary:
    Disables the MRF24W SPI chip select.

  Description:
    Disables the MRF24W SPI chip select as part of the sequence of SPI
    communications.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    If the SPI bus is shared with other peripherals then the current SPI context
    is restored.
 *******************************************************************************/
void WF_SpiDisableChipSelect(void)
{
    /* Disable the interrupt */
#if defined( __XC32 )
    WF_SPI_IE_CLEAR = WF_SPI_INT_BITS;
#else
    WF_SPI_IE = 0;
#endif

    /* set Slave Select high ((disable SPI chip select on MRF24W)   */
    WF_CS_IO = 1;

#if defined(SPI_IS_SHARED)
    RestoreSpiContext();
#endif
}

/*******************************************************************************
  Function:
    void WFSpiTxRx(void)

  Summary:
    Transmits and receives SPI bytes

  Description:
    Transmits and receives N bytes of SPI data.

  Precondition:
    None

  Parameters:
    p_txBuf - pointer to SPI tx data
    txLen   - number of bytes to Tx
    p_rxBuf - pointer to where SPI rx data will be stored
    rxLen   - number of SPI rx bytes caller wants copied to p_rxBuf

  Returns:
    None

  Remarks:
    Will clock out the larger of txLen or rxLen, and pad if necessary.
 *******************************************************************************/
void WFSpiTxRx(uint8_t *p_txBuf,
        uint16_t txLen,
        uint8_t *p_rxBuf,
        uint16_t rxLen)
{
#if defined(__XC8)
    static uint16_t byteCount; /* avoid local variables in functions called from interrupt routine */
    static uint16_t i;
#else
    uint16_t byteCount;
    uint16_t i;
#endif

#if defined(WF_DEBUG) && defined(WF_USE_POWER_SAVE_FUNCTIONS)
    /* Cannot communicate with MRF24W when it is in hibernate mode */
    {
        static uint8_t state; /* avoid local vars in functions called from interrupt */
        WF_GetPowerSaveState(&state);
        WF_ASSERT(state != WF_PS_HIBERNATE);
    }
#endif

    /* total number of byte to clock is whichever is larger, txLen or rxLen */
    byteCount = (txLen >= rxLen) ? txLen : rxLen;

    for (i = 0; i < byteCount; ++i)
    {
        /* if still have bytes to transmit from tx buffer */
        if (txLen > 0)
        {
            WF_SSPBUF = *p_txBuf++;
            --txLen;
        }
            /* else done writing bytes out from tx buffer */
        else
        {
            WF_SSPBUF = 0xff; /* clock out a "don't care" byte */
        }

        /* wait until tx/rx byte to completely clock out */
        WaitForDataByte();

        /* if still have bytes to read into rx buffer */
        if (rxLen > 0)
        {
            *p_rxBuf++ = WF_SSPBUF;
            --rxLen;
        }
            /* else done reading bytes into rx buffer */
        else
        {
            WF_SSPBUF; /* read and throw away byte */
        }
    } /* end for loop */
}

#if defined(__XC8)
/*******************************************************************************
  Function:
    void WFSpiTxRx_Rom(void)

  Summary:
    Transmits and receives SPI bytes

  Description:
    Specific to the PIC18, transmits bytes from ROM storage and receives SPI data
    bytes.

  Precondition:
    None

  Parameters:
    p_txBuf - pointer to SPI ROM tx data
    txLen   - number of bytes to Tx
    p_rxBuf - pointer to where SPI rx data will be stored
    rxLen   - number of SPI rx bytes caller wants copied to p_rxBuf

  Returns:
    None

  Remarks:
    Will clock out the larger of txLen or rxLen, and pad if necessary.
 *******************************************************************************/
void WFSpiTxRx_Rom(ROM uint8_t *p_txBuf,
        uint16_t txLen,
        uint8_t *p_rxBuf,
        uint16_t rxLen)
{
    static uint16_t byteCount; /* avoid local variables in functions called from interrupt routine */
    static uint16_t i;
    static uint8_t rxTrash;

    /* total number of byte to clock is whichever is larger, txLen or rxLen */
    byteCount = (txLen >= rxLen) ? txLen : rxLen;

    for (i = 0; i < byteCount; ++i)
    {
        /* if still have bytes to transmit from tx buffer */
        if (txLen > 0)
        {
            WF_SSPBUF = *p_txBuf++;
            --txLen;
        }
            /* else done writing bytes out from tx buffer */
        else
        {
            WF_SSPBUF = 0xff; /* clock out a "don't care" byte */
        }

        // wait until tx/rx byte completely clocked out
        WaitForDataByte();

        /* if still have bytes to read into rx buffer */
        if (rxLen > 0)
        {
            *p_rxBuf++ = WF_SSPBUF;
            --rxLen;
        }
            /* else done reading bytes into rx buffer */
        else
        {
            rxTrash = WF_SSPBUF; /* read and throw away byte */
        }
    } /* end for loop */
}
#endif

#if defined(SPI_IS_SHARED)
/*******************************************************************************
  Function:
    void SaveSpiContext(void)

  Summary:
    Saves SPI context.

  Description:
    Saves the SPI context (mainly speed setting) before using the SPI to
    access MRF24W.  Turn off the SPI module before reconfiguring it.
    We only need this function if SPI lines are shared.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    Only called if SPI_IS_SHARED is defined
 *******************************************************************************/
static void SaveSpiContext(void)
{
    // Save SPI state (clock speed)
    SPICON1Save = WF_SPICON1;
#if defined( __XC16 )
    SPICON2Save = WF_SPICON2;
#endif
    SPISTATSave = WF_SPISTAT;
    SPIONSave = SPI_ON_BIT;
    SPI_ON_BIT = 0;
}

/*******************************************************************************
  Function:
    void RestoreSpiContext(void)

  Summary:
    Restores SPI context.

  Description:
    Restores the SPI context (mainly speed setting) after using the SPI to
    access MRF24W.  Turn off the SPI module before reconfiguring it.
    We only need this function if SPI lines are shared.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    Only called if SPI_IS_SHARED is defined
 *******************************************************************************/
static void RestoreSpiContext(void)
{
    SPI_ON_BIT = 0;
    WF_SPICON1 = SPICON1Save;
#if defined( __XC16 )
    WF_SPICON2 = SPICON2Save;
#endif
    WF_SPISTAT = SPISTATSave;
    SPI_ON_BIT = SPIONSave;
}
#endif /* SPI_IS_SHARED */

#endif /* WF_CS_TRIS */
