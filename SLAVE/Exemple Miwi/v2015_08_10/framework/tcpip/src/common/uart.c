/**
  Company:
    Microchip Technology Inc.

  File Name:
    uart.c

  Summary:
    

  Description:
    UART access routines for C18 and C30

 */

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

#define __UART_C_

#include "tcpip/tcpip.h"

#if defined(STACK_USE_UART)

uint8_t ReadStringUART(uint8_t *Dest, uint8_t BufferLen)
{
    uint8_t c;
    uint8_t count = 0;

    while (BufferLen--) {
        *Dest = '\0';

        while (!DataRdyUART());
        c = ReadUART();

        if (c == '\r' || c == '\n')
            break;

        count++;
        *Dest++ = c;
    }

    return count;
}

#if defined(__XC8) // PIC18

char BusyUART(void)
{
    return !TXSTAbits.TRMT;
}

void CloseUART(void)
{
    RCSTA &= 0x4F; // Disable the receiver
    TXSTAbits.TXEN = 0; // and transmitter

    PIE1 &= 0xCF; // Disable both interrupts
}

char DataRdyUART(void)
{
    if (RCSTAbits.OERR) {
        RCSTAbits.CREN = 0;
        RCSTAbits.CREN = 1;
    }
    return PIR1bits.RCIF;
}

char ReadUART(void)
{
    return RCREG; // Return the received data
}

void WriteUART(char data)
{
    TXREG = data; // Write the data byte to the USART
}

void getsUART(char *buffer, unsigned char len)
{
    char i; // Length counter
    unsigned char data;

    for (i = 0; i < len; i++) // Only retrieve len characters
    {
        while (!DataRdyUART()); // Wait for data to be received

        data = ReadUART(); // Get a character from the USART
        // and save in the string
        *buffer = data;
        buffer++; // Increment the string pointer
    }
}

void putsUART(char *data)
{
    do { // Transmit a byte
        while (BusyUART());
        WriteUART(*data);
    } while (*data++);
}

void putrsUART(const rom char *data)
{
    do { // Transmit a byte
        while (BusyUART());
        WriteUART(*data);
    } while (*data++);
}
#elif defined(__XC16) // PIC24F, PIC24H, dsPIC30, dsPIC33

/***************************************************************************
 * Function Name     : putsUART2                                            *
 * Description       : This function puts the data string to be transmitted *
 *                     into the transmit buffer (till NULL character)       *
 * Parameters        : unsigned int * address of the string buffer to be    *
 *                     transmitted                                          *
 * Return Value      : None                                                 *
 ***************************************************************************/
void putsUART2(unsigned int *buffer)
{
    char * temp_ptr = (char *) buffer;

    /* transmit till NULL character is encountered */

    if (U2MODEbits.PDSEL == 3) /* check if TX is 8bits or 9bits */ {
        while (*buffer != '\0') {
            while (U2STAbits.UTXBF); /* wait if the buffer is full */
            U2TXREG = *buffer++; /* transfer data word to TX reg */
        }
    } else {
        while (*temp_ptr != '\0') {
            while (U2STAbits.UTXBF); /* wait if the buffer is full */
            U2TXREG = *temp_ptr++; /* transfer data byte to TX reg */
        }
    }
}

/******************************************************************************
 * Function Name     : getsUART2                                               *
 * Description       : This function gets a string of data of specified length *
 *                     if available in the UxRXREG buffer into the buffer      *
 *                     specified.                                              *
 * Parameters        : unsigned int length the length expected                 *
 *                     unsigned int *buffer  the received data to be           *
 *                                  recorded to this array                     *
 *                     unsigned int uart_data_wait timeout value               *
 * Return Value      : unsigned int number of data bytes yet to be received    *
 ******************************************************************************/
unsigned int getsUART2(unsigned int length, unsigned int *buffer,
        unsigned int uart_data_wait)
{
    unsigned int wait = 0;
    char *temp_ptr = (char *) buffer;

    while (length) /* read till length is 0 */ {
        while (!DataRdyUART2()) {
            if (wait < uart_data_wait)
                wait++; /* wait for more data */
            else
                return (length); /* Time out- Return words/bytes to be read */
        }
        wait = 0;
        if (U2MODEbits.PDSEL == 3) /* check if TX/RX is 8bits or 9bits */
            *buffer++ = U2RXREG; /* data word from HW buffer to SW buffer */
        else
            *temp_ptr++ = U2RXREG & 0xFF; /* data byte from HW buffer to SW buffer */

        length--;
    }

    return (length); /* number of data yet to be received i.e.,0 */
}

/*********************************************************************
 * Function Name     : DataRdyUart2                                   *
 * Description       : This function checks whether there is any data *
 *                     that can be read from the input buffer, by     *
 *                     checking URXDA bit                             *
 * Parameters        : None                                           *
 * Return Value      : char if any data available in buffer           *
 *********************************************************************/
char DataRdyUART2(void)
{
    return (U2STAbits.URXDA);
}

/*************************************************************************
 * Function Name     : BusyUART2                                          *
 * Description       : This returns status whether the transmission       *
 *                     is in progress or not, by checking Status bit TRMT *
 * Parameters        : None                                               *
 * Return Value      : char info whether transmission is in progress      *
 *************************************************************************/
char BusyUART2(void)
{
    return (!U2STAbits.TRMT);
}

/***************************************************************************
 * Function Name     : ReadUART2                                            *
 * Description       : This function returns the contents of UxRXREG buffer *
 * Parameters        : None                                                 *
 * Return Value      : unsigned int value from UxRXREG receive buffer       *
 ***************************************************************************/
unsigned int ReadUART2(void)
{
    if (U2MODEbits.PDSEL == 3)
        return (U2RXREG);
    else
        return (U2RXREG & 0xFF);
}

/*********************************************************************
 * Function Name     : WriteUART2                                     *
 * Description       : This function writes data into the UxTXREG,    *
 * Parameters        : unsigned int data the data to be written       *
 * Return Value      : None                                           *
 *********************************************************************/
void WriteUART2(unsigned int data)
{
    if (U2MODEbits.PDSEL == 3)
        U2TXREG = data;
    else
        U2TXREG = data & 0xFF;
}

#endif

#endif // STACK_USE_UART
