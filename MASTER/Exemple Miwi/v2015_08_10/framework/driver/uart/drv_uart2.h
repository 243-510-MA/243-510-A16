/*******************************************************************************
  UART2 Generated Driver API Header File 

  Company:
    Microchip Technology Inc.

  File Name:
    drv_uart2.h

  Summary:
    This is the header file for UART2 driver

  Description:
    This header file provides APIs for driver for UART2. 
*******************************************************************************/

//This will be the disclaimer

#ifndef _DRV_UART2_H
#define _DRV_UART2_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "system.h"
#include "system_config.h"
#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif


// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************
        
// *****************************************************************************
/* UART2 Driver Hardware Flags

  Summary
    Specifies the status of the hardware receive or transmit

  Description
    This type specifies the status of the hardware receive or transmit.

  Remarks:
    More than one of these values may be OR'd together to create a complete
    status value.  To test a value of this type, the bit of interest must be
    AND'ed with value and checked to see if the result is non-zero.
*/

typedef enum
{
    /* Indicates that Receive buffer has data, at least one more character can be read */
    DRV_UART2_RX_DATA_AVAILABLE
        /*DOM-IGNORE-BEGIN*/  = (1 << 0) /*DOM-IGNORE-END*/,
    
    /* Indicates that Receive buffer has overflowed */
    DRV_UART2_RX_OVERRUN_ERROR
        /*DOM-IGNORE-BEGIN*/  = (1 << 1) /*DOM-IGNORE-END*/,

    /* Indicates that Framing error has been detected for the current character */
    DRV_UART2_FRAMING_ERROR
        /*DOM-IGNORE-BEGIN*/  = (1 << 2) /*DOM-IGNORE-END*/,

    /* Indicates that Parity error has been detected for the current character */
    DRV_UART2_PARITY_ERROR
        /*DOM-IGNORE-BEGIN*/  = (1 << 3) /*DOM-IGNORE-END*/,

    /* Indicates that Receiver is Idle */
    DRV_UART2_RECEIVER_IDLE
        /*DOM-IGNORE-BEGIN*/  = (1 << 4) /*DOM-IGNORE-END*/,

    /* Indicates that the last transmission has completed */
    DRV_UART2_TX_COMPLETE
        /*DOM-IGNORE-BEGIN*/  = (1 << 8) /*DOM-IGNORE-END*/,

    /* Indicates that Transmit buffer is full */
    DRV_UART2_TX_FULL
        /*DOM-IGNORE-BEGIN*/  = (1 << 9) /*DOM-IGNORE-END*/

}DRV_UART2_STATUS;

// *****************************************************************************
/* UART2 Driver Transfer Flags

  Summary
    Specifies the status of the receive or transmit

  Description
    This type specifies the status of the receive or transmit operation.

  Remarks:
    More than one of these values may be OR'd together to create a complete
    status value.  To test a value of this type, the bit of interest must be
    AND'ed with value and checked to see if the result is non-zero.
*/

typedef enum
{
    /* Indicates that the core driver buffer is full */
    DRV_UART2_TRANSFER_STATUS_RX_FULL
        /*DOM-IGNORE-BEGIN*/  = (1 << 0) /*DOM-IGNORE-END*/,

    /* Indicates that at least one byte of Data has been received */
    DRV_UART2_TRANSFER_STATUS_RX_DATA_PRESENT
        /*DOM-IGNORE-BEGIN*/  = (1 << 1) /*DOM-IGNORE-END*/,

    /* Indicates that the core driver receiver buffer is empty */
    DRV_UART2_TRANSFER_STATUS_RX_EMPTY
        /*DOM-IGNORE-BEGIN*/  = (1 << 2) /*DOM-IGNORE-END*/,

    /* Indicates that the core driver transmitter buffer is full */
    DRV_UART2_TRANSFER_STATUS_TX_FULL
        /*DOM-IGNORE-BEGIN*/  = (1 << 3) /*DOM-IGNORE-END*/,

    /* Indicates that the core driver transmitter buffer is empty */
    DRV_UART2_TRANSFER_STATUS_TX_EMPTY
        /*DOM-IGNORE-BEGIN*/  = (1 << 4) /*DOM-IGNORE-END*/

} DRV_UART2_TRANSFER_STATUS;

// *****************************************************************************
// *****************************************************************************
// Section: UART2 Driver Routines
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************
/* Function: void DRV_UART2_InitializerDefault(void)

  Summary:
    Initializes the UART instance : 2

  Description:
    This routine initializes the UART driver instance for : 2
    index, making it ready for clients to open and use it.

  Precondition:
    None.

  Return:
    None.

  Input:
    None.

  Example:
    <code>
        const uint8_t writeBuffer[35] = "1234567890ABCDEFGHIJKLMNOP\n" ;
        unsigned int numBytes = 0;
        int  writebufferLen = strlen((char *)writeBuffer);
        DRV_UART2_InitializerDefault();
        while(numBytes < writebufferLen)
        {    
            int bytesToWrite = DRV_UART2_TXBufferSizeGet();
            numBytes = DRV_UART2_Write ( writeBuffer+numBytes, bytesToWrite)  ;
            DRV_UART2_TasksTX ( );
            if (!DRV_UART2_TXBufferisFull())
            {
                //continue other operation
            }
        }
    </code>

  Remarks:
    This routine must be called before any other UART routine is called.
*/

void DRV_UART2_InitializerDefault(void);


// *****************************************************************************
/* Function:
    void __attribute__ ( ( interrupt, no_auto_psv ) ) _U2TXInterrupt ( void );

  Summary:
    Maintains the driver's transmitter state machine and implements its ISR

  Description:
    This routine is used to maintain the driver's internal transmitter state
    machine.This interrupt service routine is called when the state of the 
    transmitter needs to be maintained in a non polled manner.

  Precondition:
    DRV_UART2_InitializerDefault function should have been called 
    for the ISR to execute correctly.

  Parameters:
    None.

  Returns:
    None.
*/

void __attribute__ ( ( interrupt, no_auto_psv ) ) _U2TXInterrupt ( void );


// *****************************************************************************
/* Function:
    void DRV_UART2_TasksTX ( void );
 
  Summary:
    Maintains the driver's transmitter state machine in a polled manner

  Description:
    This routine is used to maintain the driver's internal transmitter state
    machine.This routine is called when the state of the transmitter needs to be
    maintained in a polled manner.

  Precondition:
    DRV_UART2_InitializerDefault function should have been called 
    before calling this function in a polled loop.

  Parameters:
    None.

  Returns:
    None.

  Example: 
    Refer to DRV_UART2_InitializerDefault() for an example
*/
void DRV_UART2_TasksTX ( void );


// *****************************************************************************
/* Function:
    void __attribute__ ( ( interrupt, no_auto_psv ) ) _U2RXInterrupt ( void );

  Summary:
    Maintains the driver's transmitter state machine and implements its ISR

  Description:
    This routine is used to maintain the driver's internal transmitter state
    machine.This interrupt service routine is called when the state of the 
    transmitter needs to be maintained in a non polled manner.

  Precondition:
    DRV_UART2_InitializerDefault function should have been called 
    for the ISR to execute correctly.

  Parameters:
    None.

  Returns:
    None.
*/

void __attribute__ ( ( interrupt, no_auto_psv ) ) _U2RXInterrupt ( void );


// *****************************************************************************
/* Function:
    void DRV_UART2_TasksRX ( void );

  Summary:
    Maintains the driver's receiver state machine in a polled manner.

  Description:
    This routine is used to maintain the driver's internal receiver state
    machine. This routine is called when the state of the receiver needs to be
    maintained in a polled manner.

  Precondition:
    DRV_UART2_InitializerDefault function should have been called 
    before calling this function in a polled loop.

  Parameters:
    None.

  Returns:
    None.

  Example: 
    <code>
    const uint8_t readBuffer[35];
    unsigned int size, numBytes = 0;
    unsigned int readbufferLen = sizeof(readBuffer);
    DRV_UART2__InitializerDefault();
    
    while(numBytes < readbufferLen)        
    {   
        while(!DRV_UART2_RXBufferIsEmpty());
        numBytes += DRV_UART2_Read ( readBuffer + numBytes , readbufferLen ) ;
        DRV_UART2_TasksRX ( );
        status = DRV_UART2_TransferStatus ( ) ;
        if (status & DRV_UART2_TRANSFER_STATUS_RX_FULL)
        {
            //continue other operation
        }
    }
    </code>
*/

void DRV_UART2_TasksRX ( void );


// *****************************************************************************
/* Function:
    void __attribute__ ( ( interrupt, no_auto_psv ) ) _U2ErrInterrupt ( void );

  Summary:
    Maintains the driver's error-handling state machine and implements its ISR

  Description:
    This routine is used to maintain the driver's internal error-handling state
    machine.This ISR routine is called when the state of the errors needs to be
    maintained in a non-polled manner.

  Precondition:
    DRV_UART2_InitializerDefault function should have been called 
    for the ISR to execute correctly.

  Parameters:
    None.

  Returns:
    None.

*/

void __attribute__ ( ( interrupt, no_auto_psv ) ) _U2ErrInterrupt ( void );


// *****************************************************************************
/* Function:
    void DRV_UART2_TasksError ( void );

  Summary:
    Maintains the driver's error-handling state machine in a polled manner.

  Description:
    This routine is used to maintain the driver's internal error-handling state
    machine.This routine is called when the state of the errors needs to be
    maintained in a polled manner.

  Precondition:
    DRV_UART2_InitializerDefault function should have been called 
    before calling this function in a polled loop.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    while (true)
    {
        DRV_UART2_TasksError ();

        // Do other tasks
    }
    </code>
*/
void DRV_UART2_TasksError ( void );


// *****************************************************************************
// *****************************************************************************
// Section: UART Driver Client Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Function:
    uint8_t DRV_UART2_ReadByte( void)

  Summary:
    Reads a byte of data from the UART2

  Description:
    This routine reads a byte of data from the UART2.

  Precondition:
    DRV_UART2_InitializerDefault function should have been called 
    before calling this function. The transfer status should be checked to see 
    if the receiver is not empty before calling this function.

  Parameters:
    None.

  Returns:
    A data byte received by the driver.

  Example:
    <code>
    char            myBuffer[MY_BUFFER_SIZE];
    unsigned int    numBytes;

    numBytes = 0;
    do
    {
        if( DRV_UART2_TRANSFER_STATUS_RX_DATA_PRESENT & DRV_UART2_TransferStatus() )
        {
            myBuffer[numBytes++] = DRV_UART2_ReadByte();
        }

        // Do something else...

    } while( numBytes < MY_BUFFER_SIZE);
    </code>
*/

uint8_t DRV_UART2_ReadByte( void);

//*******************************************************************************
/*  Function:
    unsigned int DRV_UART2_Read(uint8_t *buffer, const unsigned int numbytes )

  Summary:
    Returns the number of bytes read by the UART2 peripheral

  Description:
    This routine returns the number of bytes read by the Peripheral and fills the
    application read buffer with the read data.

  Precondition:
    DRV_UART2_InitializerDefault function should have been called 
    before calling this function

  Parameters:
    buffer       - Buffer into which the data read from the UART2

    numbytes     - Total number of bytes that need to be read from the UART2
                   (must be equal to or less than the size of the buffer)

  Returns:
    Number of bytes actually copied into the caller's buffer or -1 if there
    is an error.

  Example:
    <code>
    char                     myBuffer[MY_BUFFER_SIZE];
    unsigned int             numBytes;
    DRV_UART2_TRANSFER_STATUS status ;

    // Pre-initialize myBuffer with MY_BUFFER_SIZE bytes of valid data.

    numBytes = 0;
    while( numBytes < MY_BUFFER_SIZE);
    {
        status = DRV_UART2_TransferStatus ( ) ;
        if (status & DRV_UART2_TRANSFER_STATUS_RX_FULL)
        {
            numBytes += DRV_UART2_Read( myBuffer + numBytes, MY_BUFFER_SIZE - numBytes )  ;
            if(numBytes < readbufferLen)
            {
                continue;
            }
            else
            {
                break;
            }
        }
        else
        {
            continue;
        }

        // Do something else...
    }
    </code>
*/

unsigned int DRV_UART2_Read( uint8_t *buffer ,  const unsigned int numbytes);

// *****************************************************************************
/* Function:
    void DRV_UART2_WriteByte( const uint8_t byte)

  Summary:
    Writes a byte of data to the UART2

  Description:
    This routine writes a byte of data to the UART2.

  Precondition:
    DRV_UART2_InitializerDefault function should have been called 
    before calling this function. The transfer status should be checked to see if
    transmitter is not full before calling this function.

  Parameters:
    byte         - Data byte to write to the UART2

  Returns:
    None.

  Example:
    <code>
    char            myBuffer[MY_BUFFER_SIZE];
    unsigned int    numBytes;

    // Pre-initialize myBuffer with MY_BUFFER_SIZE bytes of valid data.

    numBytes = 0;
    while( numBytes < MY_BUFFER_SIZE);
    {
        if( !(DRV_UART2_TRANSFER_STATUS_TX_FULL & DRV_UART2_TransferStatus()) )
        {
            DRV_UART2_WriteByte(handle, myBuffer[numBytes++]);
        }

        // Do something else...
    }
    </code>
*/

void DRV_UART2_WriteByte( const uint8_t byte);

//*******************************************************************************
/*  Function:
    unsigned int DRV_UART2_Write( uint8_t *buffer, const unsigned int numbytes )

  Summary:
    Returns the number of bytes written into the internal buffer

  Description:
    This API transfers the data from application buffer to internal buffer and 
    returns the number of bytes added in that queue

  Precondition:
    DRV_UART2_InitializerDefault function should have been called 
    before calling this function

  Example:
    <code>
    char                     myBuffer[MY_BUFFER_SIZE];
    unsigned int             numBytes;
    DRV_UART2_TRANSFER_STATUS status ;

    // Pre-initialize myBuffer with MY_BUFFER_SIZE bytes of valid data.

    numBytes = 0;
    while( numBytes < MY_BUFFER_SIZE);
    {
        status = DRV_UART2_TransferStatus ( ) ;
        if (status & DRV_UART2_TRANSFER_STATUS_TX_EMPTY)
        {
            numBytes += DRV_UART2_Write ( myBuffer + numBytes, MY_BUFFER_SIZE - numBytes )  ;
            if(numBytes < writebufferLen)
            {
                continue;
            }
            else
            {
                break;
            }
        }
        else
        {
            continue;
        }

        // Do something else...
    }
    </code>

  Remarks:
    None
*/

unsigned int DRV_UART2_Write( const uint8_t *buffer , const unsigned int numbytes );

// *****************************************************************************
/* Function:
    DRV_UART2_TRANSFER_STATUS DRV_UART2_TransferStatus (void)

  Summary:
    Returns the transmitter and receiver transfer status

  Description:
    This returns the transmitter and receiver transfer status.

  Precondition:
    DRV_UART2_InitializerDefault function should have been called 
    before calling this function

  Parameters:
    None.

  Returns:
    A DRV_UART2_TRANSFER_STATUS value describing the current status 
    of the transfer.

  Example:
    Refer to DRV_UART2_Read and DRV_UART2_Write for example

  Remarks:
    The returned status may contain a value with more than one of the bits
    specified in the DRV_UART2_TRANSFER_STATUS enumeration set.  
    The caller should perform an "AND" with the bit of interest and verify if the
    result is non-zero (as shown in the example) to verify the desired status
    bit.
*/

DRV_UART2_TRANSFER_STATUS DRV_UART2_TransferStatus (void );

// *****************************************************************************
/* Function:
  uint8_t DRV_UART2_Peek(uint16_t offset)

  Summary:
    Returns the character in the read sequence at the offset provided, without
    extracting it

  Description:
    This routine returns the character in the read sequence at the offset provided,
    without extracting it
 
 Example: 
    <code>
    const uint8_t readBuffer[5];
    unsigned int data, numBytes = 0;
    unsigned int readbufferLen = sizeof(readBuffer);
    DRV_UART2_InitializerDefault();
    
    while(numBytes < readbufferLen)        
    {   
        DRV_UART2_TasksRX ( );
        //Check for data at a particular place in the buffer
        data = DRV_UART2_Peek(3);
        if(data == 5)
        {
            //discard all other data if byte that is wanted is received.    
            //continue other operation
            numBytes += DRV_UART2_Read ( readBuffer + numBytes , readbufferLen ) ;
        }
        else
        {
            break;
        }
    }
    </code>
 
 Parameters:
    None.

*/
uint8_t DRV_UART2_Peek(uint16_t offset);

// *****************************************************************************
/* Function:
  unsigned int DRV_UART2_RXBufferSizeGet (void)

  Summary:
    Returns the size of the receive buffer

  Description:
    This routine returns the size of the receive buffer.

 Parameters:
    None.

 Example: 
    <code>
    const uint8_t readBuffer[5];
    unsigned int size, numBytes = 0;
    unsigned int readbufferLen = sizeof(readBuffer);
    DRV_UART2__InitializerDefault();
    
    while(size < readbufferLen)
	{
	    DRV_UART2_TasksRX ( );
	    size = DRV_UART2_RXBufferSizeGet();
	}
    numBytes = DRV_UART2_Read ( readBuffer , readbufferLen ) ;
    </code>
 
 Returns:
    Size of receive buffer.
*/
unsigned int DRV_UART2_RXBufferSizeGet(void);

// *****************************************************************************
/* Function:
  unsigned int DRV_UART2_TXBufferSizeGet (void)

  Summary:
    Returns the size of the transmit buffer

  Description:
    This routine returns the size of the transmit buffer.

 Parameters:
    None.
 
 Examples:
    Refer to DRV_UART2_InitializerDefault(); for example.

 Returns:
    Size of transmit buffer.
*/
unsigned int DRV_UART2_TXBufferSizeGet(void);

// *****************************************************************************
/* Function:
  bool DRV_UART2_RXBufferIsEmpty (void)

  Summary:
    Returns the status of the receive buffer

  Description:
    This routine returns if the receive buffer is empty or not.

 Parameters:
    None.
 
 Example:
    <code>
    char                     myBuffer[MY_BUFFER_SIZE];
    unsigned int             numBytes;
    DRV_UART2_TRANSFER_STATUS status ;

    // Pre-initialize myBuffer with MY_BUFFER_SIZE bytes of valid data.

    numBytes = 0;
    while( numBytes < MY_BUFFER_SIZE);
    {
        status = DRV_UART2_TransferStatus ( ) ;
        if (!DRV_UART2_RXBufferIsEmpty())
        {
            numBytes += DRV_UART2_Read( myBuffer + numBytes, MY_BUFFER_SIZE - numBytes )  ;
            if(numBytes < readbufferLen)
            {
                continue;
            }
            else
            {
                break;
            }
        }
        else
        {
            continue;
        }

        // Do something else...
    }
    </code>
 
 Returns:
    True if the receive buffer is empty
    False if the receive buffer is not empty
*/
bool DRV_UART2_RXBufferIsEmpty (void);

// *****************************************************************************
/* Function:
    bool DRV_UART2_TXBufferIsFull (void)

  Summary:
    Returns the status of the transmit buffer

  Description:
    This routine returns if the transmit buffer is full or not.

 Parameters:
    None.
 
 Example:
    Refer to DRV_UART2_InitializerDefault() for example.
 
 Returns:
    True if the transmit buffer is full
    False if the transmit buffer is not full
*/
bool DRV_UART2_TXBufferIsFull (void);


// *****************************************************************************
/* Function:
    DRV_UART2_STATUS DRV_UART2_Status (void)

  Summary:
    Returns the transmitter and receiver status

  Description:
    This returns the transmitter and receiver status.

  Precondition:
    DRV_UART2_InitializerDefault function should have been called 
    before calling this function

  Parameters:
    None.

  Returns:
    A DRV_UART2_STATUS value describing the current status 
    of the transfer.

  Example:
    <code>
        while(!(DRV_UART2_Status() & DRV_UART2_TX_COMPLETE ))
        {
           // Wait for the tranmission to complete
        }
    </code>

  Remarks:
    The returned status may contain a value with more than one of the bits
    specified in the DRV_UART2_STATUS enumeration set.  
    The caller should perform an "AND" with the bit of interest and verify if the
    result is non-zero (as shown in the example) to verify the desired status
    bit.
*/

DRV_UART2_STATUS DRV_UART2_Status (void );


#ifdef __cplusplus  // Provide C++ Compatability

    }

#endif
    
#endif  // _DRV_UART2_H

//*******************************************************************************
/*
  End of File
*/

