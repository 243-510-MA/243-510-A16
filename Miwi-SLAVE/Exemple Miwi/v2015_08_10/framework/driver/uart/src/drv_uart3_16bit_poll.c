/*******************************************************************************
  UART3 Generated Driver API Header File 

  Company:
    Microchip Technology Inc.

  File Name:
    drv_uart3_16bit_poll.c

  Summary:
    This is the source file for UART3 driver

  Description:
    This source file provides APIs for driver for UART3. 
*******************************************************************************/

//This will be the disclaimer


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "driver/uart/drv_uart3.h"
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

static DRV_UART_OBJECT drv_uart3_obj ;

// *****************************************************************************
/* UART Driver Queue Length

  Summary:
    Defines the length of the Transmit and Receive Buffers

*/

#ifndef DRV_UART3_CONFIG_TX_BYTEQ_LENGTH
        #define DRV_UART3_CONFIG_TX_BYTEQ_LENGTH 4
#endif
#ifndef DRV_UART3_CONFIG_RX_BYTEQ_LENGTH
        #define DRV_UART3_CONFIG_RX_BYTEQ_LENGTH 4
#endif

// *****************************************************************************
/* Default values of the static overrides

  Summary:
   Checks for the definitions, if definitions found for stop bits, data bits,
   parity and baud rate those definitions are used, otherwise default values are
   used.

*/

#if defined(DRV_UART3_CONFIG_8N1)
        #define DRV_UART3_PDS 0
#elif defined(DRV_UART3_CONFIG_8N2)
        #define DRV_UART3_PDS 1
#elif defined(DRV_UART3_CONFIG_8E1)
        #define DRV_UART3_PDS 2
#elif defined(DRV_UART3_CONFIG_8E2)
        #define DRV_UART3_PDS 3
#elif defined(DRV_UART3_CONFIG_8O1)
        #define DRV_UART3_PDS 4
#elif defined(DRV_UART3_CONFIG_8O2)
        #define DRV_UART3_PDS 5
#elif defined(DRV_UART3_CONFIG_9N1)
        #define DRV_UART3_PDS 6
#elif defined(DRV_UART3_CONFIG_9N2)
        #define DRV_UART3_PDS 7
#else
        #define DRV_UART3_PDS 0
#endif

#if !defined(DRV_UART3_CONFIG_BAUD_RATE)
        #define DRV_UART3_CONFIG_BAUD_RATE 9600
#endif

#define BRG_TEMP (unsigned long)(SYS_CLK_FrequencyPeripheralGet() / (4 * (unsigned long)DRV_UART3_CONFIG_BAUD_RATE))
 
#define BRG_TEMP_10 (unsigned long)(SYS_CLK_FrequencyPeripheralGet()*10 / (4 * (unsigned long)DRV_UART3_CONFIG_BAUD_RATE))

// *****************************************************************************

/* UART Driver Queue

  Summary:
    Defines the Transmit and Receive Buffers

*/

static uint8_t uart3_txByteQ[DRV_UART3_CONFIG_TX_BYTEQ_LENGTH] ;
static uint8_t uart3_rxByteQ[DRV_UART3_CONFIG_RX_BYTEQ_LENGTH] ;

// *****************************************************************************
/* UART Hardware FIFO Buffer Length

  Summary:
    Defines the length of the Transmit and Receive FIFOs
 
*/

#define DRV_UART3_TX_FIFO_LENGTH 1
#define DRV_UART3_RX_FIFO_LENGTH 1 

// *****************************************************************************
// *****************************************************************************
// Section: Driver Interface Function Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function: void DRV_UART3_InitializerDefault(void)

  Summary:
    Initializes the UART instance : 3

*/

void DRV_UART3_InitializerDefault (void)
{
   U3MODE = (0x8008 & 0xFFFC)| DRV_UART3_PDS;
   // UTXEN disabled; UTXINV disabled; URXISEL RX_ONE_CHAR; ADDEN disabled; UTXISEL0 TX_ONE_CHAR; UTXBRK COMPLETED; OERR NO_ERROR_cleared; 
   U3STA = 0x0000;
   U3BRG = BRG_TEMP  - 1 + ((BRG_TEMP_10 - BRG_TEMP*10)+5)/10;


   U3STAbits.UTXEN = 1;

   drv_uart3_obj.txHead = uart3_txByteQ;
   drv_uart3_obj.txTail = uart3_txByteQ;
   drv_uart3_obj.rxHead = uart3_rxByteQ;
   drv_uart3_obj.rxTail = uart3_rxByteQ;
   drv_uart3_obj.rxStatus.s.empty = true;
   drv_uart3_obj.txStatus.s.empty = true;
   drv_uart3_obj.txStatus.s.full = false;
   drv_uart3_obj.rxStatus.s.full = false;
}

// *****************************************************************************
/* Function:
    void DRV_UART3_TasksTX ( void )

  Summary:
    Maintains the driver's transmitter state machine in a polled manner

*/

void DRV_UART3_TasksTX ( void )
{ 
    if(drv_uart3_obj.txStatus.s.empty)
    {
        return;
    }

    IFS5bits.U3TXIF = false;

    int count = 0;
    while((count < DRV_UART3_TX_FIFO_LENGTH)&& !(U3STAbits.UTXBF == 1))
    {
        count++;

        U3TXREG = *drv_uart3_obj.txHead;

        drv_uart3_obj.txHead++;

        if(drv_uart3_obj.txHead == (uart3_txByteQ + DRV_UART3_CONFIG_TX_BYTEQ_LENGTH))
        {
            drv_uart3_obj.txHead = uart3_txByteQ;
        }

        drv_uart3_obj.txStatus.s.full = false;

        if(drv_uart3_obj.txHead == drv_uart3_obj.txTail)
        {
            drv_uart3_obj.txStatus.s.empty = true;
            break;
        }
    }
}
// *****************************************************************************
/* Function:
    void DRV_UART3_TasksRX ( void )

  Summary:
    Maintains the driver's receiver state machine in a polled manner.

*/
void DRV_UART3_TasksRX ( void )
{
    int count = 0;

    while((count < DRV_UART3_RX_FIFO_LENGTH) && (U3STAbits.URXDA == 1))
    {
        count++;

        *drv_uart3_obj.rxTail = U3RXREG;

        drv_uart3_obj.rxTail++;

        if(drv_uart3_obj.rxTail == (uart3_rxByteQ + DRV_UART3_CONFIG_RX_BYTEQ_LENGTH))
        {
            drv_uart3_obj.rxTail = uart3_rxByteQ;
        }

        drv_uart3_obj.rxStatus.s.empty = false;
        
        if(drv_uart3_obj.rxTail == drv_uart3_obj.rxHead)
        {
            //Sets the flag RX full
            drv_uart3_obj.rxStatus.s.full = true;
            break;
        }
        
    }

    IFS5bits.U3RXIF = false;
   
}
// *****************************************************************************
/* Function:
    void DRV_UART3_TasksError ( void );

  Summary:
    Maintains the driver's error-handling state machine in a polled manner.

*/
void DRV_UART3_TasksError ( void )
{
    if ((U3STAbits.OERR == 1))
    {
        U3STAbits.OERR = 0;
    }

    IFS5bits.U3ERIF = false;
}

// *****************************************************************************
// *****************************************************************************
// Section: UART Driver Client Routines
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/* Function:
    uint8_t DRV_UART3_ReadByte( void)

  Summary:
    Reads a byte of data from the UART3

*/

uint8_t DRV_UART3_ReadByte( void)
{
    uint8_t data = 0;

    data = *drv_uart3_obj.rxHead;

    drv_uart3_obj.rxHead++;

    if (drv_uart3_obj.rxHead == (uart3_rxByteQ + DRV_UART3_CONFIG_RX_BYTEQ_LENGTH))
    {
        drv_uart3_obj.rxHead = uart3_rxByteQ;
    }

    if (drv_uart3_obj.rxHead == drv_uart3_obj.rxTail)
    {
        drv_uart3_obj.rxStatus.s.empty = true;
    }

    drv_uart3_obj.rxStatus.s.full = false;

    return data;
}

//*******************************************************************************
/*  Function:
    int DRV_UART3_Read(uint8_t *buffer, const unsigned int numbytes )

  Summary:
    Returns the number of bytes read by the UART3 peripheral

*/

unsigned int DRV_UART3_Read( uint8_t *buffer ,  const unsigned int numbytes)
{
    unsigned int numBytesRead = 0 ;
    while ( numBytesRead < ( numbytes ))
    {
        if( drv_uart3_obj.rxStatus.s.empty)
        {
            break;
        }
        else
        {
            buffer[numBytesRead++] = DRV_UART3_ReadByte () ;
        }
    }

    return numBytesRead ;
}

// *****************************************************************************
/* Function:
    void DRV_UART3_WriteByte( const uint8_t byte)

  Summary:
    Writes a byte of data to the UART3

*/

void DRV_UART3_WriteByte( const uint8_t byte)
{
    *drv_uart3_obj.txTail = byte;

    drv_uart3_obj.txTail++;
    
    if (drv_uart3_obj.txTail == (uart3_txByteQ + DRV_UART3_CONFIG_TX_BYTEQ_LENGTH))
    {
        drv_uart3_obj.txTail = uart3_txByteQ;
    }

    drv_uart3_obj.txStatus.s.empty = false;

    if (drv_uart3_obj.txHead == drv_uart3_obj.txTail)
    {
        drv_uart3_obj.txStatus.s.full = true;
    }

	
}

//*******************************************************************************
/*  Function:
    unsigned int DRV_UART3_Write( uint8_t *buffer, const unsigned int numbytes )

  Summary:
    Returns the number of bytes written into the internal buffer

*/

unsigned int DRV_UART3_Write( const uint8_t *buffer , const unsigned int numbytes )
{
    unsigned int numBytesWritten = 0 ;

    while ( numBytesWritten < ( numbytes ))
    {
        if((drv_uart3_obj.txStatus.s.full))
        {
            break;
        }
        else
        {
            DRV_UART3_WriteByte (buffer[numBytesWritten++] ) ;
        }
    }

    return numBytesWritten ;

} 

// *****************************************************************************
/* Function:
    DRV_UART3_TRANSFER_STATUS DRV_UART3_TransferStatus (void)

  Summary:
    Returns the transmitter and receiver transfer status

*/
DRV_UART3_TRANSFER_STATUS DRV_UART3_TransferStatus (void )
{
    DRV_UART3_TRANSFER_STATUS status = 0;

    if(drv_uart3_obj.txStatus.s.full)
    {
        status |= DRV_UART3_TRANSFER_STATUS_TX_FULL;
    }

    if(drv_uart3_obj.txStatus.s.empty)
    {
        status |= DRV_UART3_TRANSFER_STATUS_TX_EMPTY;
    }

    if(drv_uart3_obj.rxStatus.s.full)
    {
        status |= DRV_UART3_TRANSFER_STATUS_RX_FULL;
    }

    if(drv_uart3_obj.rxStatus.s.empty)
    {
        status |= DRV_UART3_TRANSFER_STATUS_RX_EMPTY;
    }
    else
    {
        status |= DRV_UART3_TRANSFER_STATUS_RX_DATA_PRESENT;
    }
    return status;
}

// *****************************************************************************
/* Function:
  uint8_t DRV_UART3_Peek(uint16_t offset)

  Summary:
    Returns the character in the read sequence at the offset provided, without
    extracting it

*/
uint8_t DRV_UART3_Peek(uint16_t offset)
{
    if( (drv_uart3_obj.rxHead + offset) > (uart3_rxByteQ + DRV_UART3_CONFIG_RX_BYTEQ_LENGTH))
    {
      return uart3_rxByteQ[offset - (uart3_rxByteQ + DRV_UART3_CONFIG_RX_BYTEQ_LENGTH - drv_uart3_obj.rxHead)];
    }
    else
    {
      return *(drv_uart3_obj.rxHead + offset);
    }
}

// *****************************************************************************
/* Function:
  unsigned int DRV_UART3_RXBufferSizeGet (void)

  Summary:
    Returns the size of the receive buffer

*/
unsigned int DRV_UART3_RXBufferSizeGet(void)
{
    if(drv_uart3_obj.rxHead > drv_uart3_obj.rxTail)
    {
        return(DRV_UART3_CONFIG_RX_BYTEQ_LENGTH - (int)(drv_uart3_obj.rxHead) - (int)(drv_uart3_obj.rxTail));
    }
    else
    {
        return(drv_uart3_obj.rxTail - drv_uart3_obj.rxHead);
    }
}

// *****************************************************************************
/* Function:
  unsigned int DRV_UART3_TXBufferSizeGet (void)

  Summary:
    Returns the size of the transmit buffer

*/
unsigned int DRV_UART3_TXBufferSizeGet(void)
{
     if(drv_uart3_obj.txHead > drv_uart3_obj.txTail)
    {
        return(DRV_UART3_CONFIG_TX_BYTEQ_LENGTH - (int)drv_uart3_obj.txHead - (int)drv_uart3_obj.txTail);
    }
    else
    {
        return(drv_uart3_obj.txTail - drv_uart3_obj.txHead);
    }
}

// *****************************************************************************
/* Function:
  bool DRV_UART3_RXBufferIsEmpty (void)

  Summary:
    Returns the status of the receive buffer

*/
bool DRV_UART3_RXBufferIsEmpty (void)
{
    return(drv_uart3_obj.rxStatus.s.empty);
}

// *****************************************************************************
/* Function:
    bool DRV_UART3_TXBufferIsFull (void)

  Summary:
    Returns the status of the transmit buffer

*/
bool DRV_UART3_TXBufferIsFull (void)
{
    return(drv_uart3_obj.txStatus.s.full);
}


// *****************************************************************************
/* Function:
    DRV_UART3_STATUS DRV_UART3_Status (void)

  Summary:
    Returns the status of the transmit and receive

*/
DRV_UART3_STATUS DRV_UART3_Status (void)
{
    return U3STA;
}

//*******************************************************************************
/*
  End of File
*/
