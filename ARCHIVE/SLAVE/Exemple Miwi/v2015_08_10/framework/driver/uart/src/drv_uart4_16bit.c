/*******************************************************************************
  UART4 Generated Driver API Header File 

  Company:
    Microchip Technology Inc.

  File Name:
    drv_uart4_16bit.c

  Summary:
    This is the source file for UART4 driver

  Description:
    This source file provides APIs for driver for UART4. 
*******************************************************************************/

//This will be the disclaimer


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "driver/uart/drv_uart4.h"
// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/*UART Driver Queue Status

  Summary
    Defines the object required for the status of the queue.
*/

typedef union
{
    struct
    {
            uint8_t full:1;
            uint8_t empty:1;
            uint8_t reserved:6;
    }s;
    uint8_t status;
}DRV_UART_BYTEQ_STATUS;
// *****************************************************************************

/* UART Driver Hardware Instance Object

  Summary:
    Defines the object required for the maintenance of the hardware instance.

*/
typedef struct
{
    /* RX Byte Q */
    uint8_t                                      *rxTail ;

    uint8_t                                      *rxHead ;

    /* TX Byte Q */
    uint8_t                                      *txTail ;

    uint8_t                                      *txHead ;

    DRV_UART_BYTEQ_STATUS                        rxStatus ;

    DRV_UART_BYTEQ_STATUS                        txStatus ;

} DRV_UART_OBJECT ;

static DRV_UART_OBJECT drv_uart4_obj ;

// *****************************************************************************
/* UART Driver Queue Length

  Summary:
    Defines the length of the Transmit and Receive Buffers

*/

#ifndef DRV_UART4_CONFIG_TX_BYTEQ_LENGTH
        #define DRV_UART4_CONFIG_TX_BYTEQ_LENGTH 4
#endif
#ifndef DRV_UART4_CONFIG_RX_BYTEQ_LENGTH
        #define DRV_UART4_CONFIG_RX_BYTEQ_LENGTH 4
#endif

// *****************************************************************************
/* Default values of the static overrides

  Summary:
   Checks for the definitions, if definitions found for stop bits, data bits,
   parity and baud rate those definitions are used, otherwise default values are
   used.

*/

#if defined(DRV_UART4_CONFIG_8N1)
        #define DRV_UART4_PDS 0
#elif defined(DRV_UART4_CONFIG_8N2)
        #define DRV_UART4_PDS 1
#elif defined(DRV_UART4_CONFIG_8E1)
        #define DRV_UART4_PDS 2
#elif defined(DRV_UART4_CONFIG_8E2)
        #define DRV_UART4_PDS 3
#elif defined(DRV_UART4_CONFIG_8O1)
        #define DRV_UART4_PDS 4
#elif defined(DRV_UART4_CONFIG_8O2)
        #define DRV_UART4_PDS 5
#elif defined(DRV_UART4_CONFIG_9N1)
        #define DRV_UART4_PDS 6
#elif defined(DRV_UART4_CONFIG_9N2)
        #define DRV_UART4_PDS 7
#else
        #define DRV_UART4_PDS 0
#endif

#if !defined(DRV_UART4_CONFIG_BAUD_RATE)
        #define DRV_UART4_CONFIG_BAUD_RATE 9600
#endif

#define BRG_TEMP (unsigned long)(SYS_CLK_FrequencyPeripheralGet() / (4 * (unsigned long)DRV_UART4_CONFIG_BAUD_RATE))
 
#define BRG_TEMP_10 (unsigned long)(SYS_CLK_FrequencyPeripheralGet()*10 / (4 * (unsigned long)DRV_UART4_CONFIG_BAUD_RATE))

// *****************************************************************************

/* UART Driver Queue

  Summary:
    Defines the Transmit and Receive Buffers

*/

static uint8_t uart4_txByteQ[DRV_UART4_CONFIG_TX_BYTEQ_LENGTH] ;
static uint8_t uart4_rxByteQ[DRV_UART4_CONFIG_RX_BYTEQ_LENGTH] ;

// *****************************************************************************
/* UART Hardware FIFO Buffer Length

  Summary:
    Defines the length of the Transmit and Receive FIFOs
 
*/

#define DRV_UART4_TX_FIFO_LENGTH 1
#define DRV_UART4_RX_FIFO_LENGTH 1 

// *****************************************************************************
// *****************************************************************************
// Section: Driver Interface Function Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function: void DRV_UART4_InitializerDefault(void)

  Summary:
    Initializes the UART instance : 4

*/

void DRV_UART4_InitializerDefault (void)
{
   U4MODE = (0x8008 & 0xFFFC)| DRV_UART4_PDS;
   // UTXEN disabled; UTXINV disabled; URXISEL RX_ONE_CHAR; ADDEN disabled; UTXISEL0 TX_ONE_CHAR; UTXBRK COMPLETED; OERR NO_ERROR_cleared; 
   U4STA = 0x0000;
   U4BRG = BRG_TEMP  - 1 + ((BRG_TEMP_10 - BRG_TEMP*10)+5)/10;

   IEC5bits.U4RXIE = 1;

   U4STAbits.UTXEN = 1;

   drv_uart4_obj.txHead = uart4_txByteQ;
   drv_uart4_obj.txTail = uart4_txByteQ;
   drv_uart4_obj.rxHead = uart4_rxByteQ;
   drv_uart4_obj.rxTail = uart4_rxByteQ;
   drv_uart4_obj.rxStatus.s.empty = true;
   drv_uart4_obj.txStatus.s.empty = true;
   drv_uart4_obj.txStatus.s.full = false;
   drv_uart4_obj.rxStatus.s.full = false;
}

// *****************************************************************************
/* Function:
    void __attribute__ ( ( interrupt, no_auto_psv ) ) _U4TXInterrupt ( void )

  Summary:
    Maintains the driver's transmitter state machine and implements its ISR

*/

void __attribute__ ( ( interrupt, no_auto_psv ) ) _U4TXInterrupt ( void )
{ 
    if(drv_uart4_obj.txStatus.s.empty)
    {
        IEC5bits.U4TXIE = false;
        return;
    }

    IFS5bits.U4TXIF = false;

    int count = 0;
    while((count < DRV_UART4_TX_FIFO_LENGTH)&& !(U4STAbits.UTXBF == 1))
    {
        count++;

        U4TXREG = *drv_uart4_obj.txHead;

        drv_uart4_obj.txHead++;

        if(drv_uart4_obj.txHead == (uart4_txByteQ + DRV_UART4_CONFIG_TX_BYTEQ_LENGTH))
        {
            drv_uart4_obj.txHead = uart4_txByteQ;
        }

        drv_uart4_obj.txStatus.s.full = false;

        if(drv_uart4_obj.txHead == drv_uart4_obj.txTail)
        {
            drv_uart4_obj.txStatus.s.empty = true;
            break;
        }
    }
}
// *****************************************************************************
/* Function:
    void __attribute__ ( ( interrupt, no_auto_psv ) ) _U4RXInterrupt ( void )

  Summary:
    Maintains the driver's transmitter state machine and implements its ISR

*/
void __attribute__ ( ( interrupt, no_auto_psv ) ) _U4RXInterrupt ( void )
{
    int count = 0;

    while((count < DRV_UART4_RX_FIFO_LENGTH) && (U4STAbits.URXDA == 1))
    {
        count++;

        *drv_uart4_obj.rxTail = U4RXREG;

        drv_uart4_obj.rxTail++;

        if(drv_uart4_obj.rxTail == (uart4_rxByteQ + DRV_UART4_CONFIG_RX_BYTEQ_LENGTH))
        {
            drv_uart4_obj.rxTail = uart4_rxByteQ;
        }

        drv_uart4_obj.rxStatus.s.empty = false;
        
        if(drv_uart4_obj.rxTail == drv_uart4_obj.rxHead)
        {
            //Sets the flag RX full
            drv_uart4_obj.rxStatus.s.full = true;
            break;
        }
        
    }

    IFS5bits.U4RXIF = false;
   
}
// *****************************************************************************
/* Function:
    void __attribute__ ( ( interrupt, no_auto_psv ) ) _U4ErrInterrupt ( void );

  Summary:
    Maintains the driver's error-handling state machine and implements its ISR

*/
void __attribute__ ( ( interrupt, no_auto_psv ) ) _U4ErrInterrupt ( void )
{
    if ((U4STAbits.OERR == 1))
    {
        U4STAbits.OERR = 0;
    }

    IFS5bits.U4ERIF = false;
}

// *****************************************************************************
// *****************************************************************************
// Section: UART Driver Client Routines
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/* Function:
    uint8_t DRV_UART4_ReadByte( void)

  Summary:
    Reads a byte of data from the UART4

*/

uint8_t DRV_UART4_ReadByte( void)
{
    uint8_t data = 0;

    data = *drv_uart4_obj.rxHead;

    drv_uart4_obj.rxHead++;

    if (drv_uart4_obj.rxHead == (uart4_rxByteQ + DRV_UART4_CONFIG_RX_BYTEQ_LENGTH))
    {
        drv_uart4_obj.rxHead = uart4_rxByteQ;
    }

    if (drv_uart4_obj.rxHead == drv_uart4_obj.rxTail)
    {
        drv_uart4_obj.rxStatus.s.empty = true;
    }

    drv_uart4_obj.rxStatus.s.full = false;

    return data;
}

//*******************************************************************************
/*  Function:
    int DRV_UART4_Read(uint8_t *buffer, const unsigned int numbytes )

  Summary:
    Returns the number of bytes read by the UART4 peripheral

*/

unsigned int DRV_UART4_Read( uint8_t *buffer ,  const unsigned int numbytes)
{
    unsigned int numBytesRead = 0 ;
    while ( numBytesRead < ( numbytes ))
    {
        if( drv_uart4_obj.rxStatus.s.empty)
        {
            break;
        }
        else
        {
            buffer[numBytesRead++] = DRV_UART4_ReadByte () ;
        }
    }

    return numBytesRead ;
}

// *****************************************************************************
/* Function:
    void DRV_UART4_WriteByte( const uint8_t byte)

  Summary:
    Writes a byte of data to the UART4

*/

void DRV_UART4_WriteByte( const uint8_t byte)
{
    *drv_uart4_obj.txTail = byte;

    drv_uart4_obj.txTail++;
    
    if (drv_uart4_obj.txTail == (uart4_txByteQ + DRV_UART4_CONFIG_TX_BYTEQ_LENGTH))
    {
        drv_uart4_obj.txTail = uart4_txByteQ;
    }

    drv_uart4_obj.txStatus.s.empty = false;

    if (drv_uart4_obj.txHead == drv_uart4_obj.txTail)
    {
        drv_uart4_obj.txStatus.s.full = true;
    }

    if (IEC5bits.U4TXIE  == false)
    {
        IEC5bits.U4TXIE = true ;
    }
	
}

//*******************************************************************************
/*  Function:
    unsigned int DRV_UART4_Write( uint8_t *buffer, const unsigned int numbytes )

  Summary:
    Returns the number of bytes written into the internal buffer

*/

unsigned int DRV_UART4_Write( const uint8_t *buffer , const unsigned int numbytes )
{
    unsigned int numBytesWritten = 0 ;

    while ( numBytesWritten < ( numbytes ))
    {
        if((drv_uart4_obj.txStatus.s.full))
        {
            break;
        }
        else
        {
            DRV_UART4_WriteByte (buffer[numBytesWritten++] ) ;
        }
    }

    return numBytesWritten ;

} 

// *****************************************************************************
/* Function:
    DRV_UART4_TRANSFER_STATUS DRV_UART4_TransferStatus (void)

  Summary:
    Returns the transmitter and receiver transfer status

*/
DRV_UART4_TRANSFER_STATUS DRV_UART4_TransferStatus (void )
{
    DRV_UART4_TRANSFER_STATUS status = 0;

    if(drv_uart4_obj.txStatus.s.full)
    {
        status |= DRV_UART4_TRANSFER_STATUS_TX_FULL;
    }

    if(drv_uart4_obj.txStatus.s.empty)
    {
        status |= DRV_UART4_TRANSFER_STATUS_TX_EMPTY;
    }

    if(drv_uart4_obj.rxStatus.s.full)
    {
        status |= DRV_UART4_TRANSFER_STATUS_RX_FULL;
    }

    if(drv_uart4_obj.rxStatus.s.empty)
    {
        status |= DRV_UART4_TRANSFER_STATUS_RX_EMPTY;
    }
    else
    {
        status |= DRV_UART4_TRANSFER_STATUS_RX_DATA_PRESENT;
    }
    return status;
}

// *****************************************************************************
/* Function:
  uint8_t DRV_UART4_Peek(uint16_t offset)

  Summary:
    Returns the character in the read sequence at the offset provided, without
    extracting it

*/
uint8_t DRV_UART4_Peek(uint16_t offset)
{
    if( (drv_uart4_obj.rxHead + offset) > (uart4_rxByteQ + DRV_UART4_CONFIG_RX_BYTEQ_LENGTH))
    {
      return uart4_rxByteQ[offset - (uart4_rxByteQ + DRV_UART4_CONFIG_RX_BYTEQ_LENGTH - drv_uart4_obj.rxHead)];
    }
    else
    {
      return *(drv_uart4_obj.rxHead + offset);
    }
}

// *****************************************************************************
/* Function:
  unsigned int DRV_UART4_RXBufferSizeGet (void)

  Summary:
    Returns the size of the receive buffer

*/
unsigned int DRV_UART4_RXBufferSizeGet(void)
{
    if(drv_uart4_obj.rxHead > drv_uart4_obj.rxTail)
    {
        return(DRV_UART4_CONFIG_RX_BYTEQ_LENGTH - (int)(drv_uart4_obj.rxHead) - (int)(drv_uart4_obj.rxTail));
    }
    else
    {
        return(drv_uart4_obj.rxTail - drv_uart4_obj.rxHead);
    }
}

// *****************************************************************************
/* Function:
  unsigned int DRV_UART4_TXBufferSizeGet (void)

  Summary:
    Returns the size of the transmit buffer

*/
unsigned int DRV_UART4_TXBufferSizeGet(void)
{
     if(drv_uart4_obj.txHead > drv_uart4_obj.txTail)
    {
        return(DRV_UART4_CONFIG_TX_BYTEQ_LENGTH - (int)drv_uart4_obj.txHead - (int)drv_uart4_obj.txTail);
    }
    else
    {
        return(drv_uart4_obj.txTail - drv_uart4_obj.txHead);
    }
}

// *****************************************************************************
/* Function:
  bool DRV_UART4_RXBufferIsEmpty (void)

  Summary:
    Returns the status of the receive buffer

*/
bool DRV_UART4_RXBufferIsEmpty (void)
{
    return(drv_uart4_obj.rxStatus.s.empty);
}

// *****************************************************************************
/* Function:
    bool DRV_UART4_TXBufferIsFull (void)

  Summary:
    Returns the status of the transmit buffer

*/
bool DRV_UART4_TXBufferIsFull (void)
{
    return(drv_uart4_obj.txStatus.s.full);
}


// *****************************************************************************
/* Function:
    DRV_UART4_STATUS DRV_UART4_Status (void)

  Summary:
    Returns the status of the transmit and receive

*/
DRV_UART4_STATUS DRV_UART4_Status (void)
{
    return U4STA;
}

//*******************************************************************************
/*
  End of File
*/
