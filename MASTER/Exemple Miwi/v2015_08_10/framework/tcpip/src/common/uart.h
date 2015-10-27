/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    uart.h

  Summary:
    

  Description:
    UART access routines for C18 and C30

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

#ifndef __UART_H_
#define __UART_H_

//#include "system_config.h"

#if defined(__XC8) // PIC18
char BusyUART(void);
void CloseUART(void);
char DataRdyUART(void);
char ReadUART(void);
void WriteUART(char data);
void getsUART(char *buffer, unsigned char len);
void putsUART(char *data);
void putrsUART(const rom char *data);
#elif defined(__XC16) // PIC24F, PIC24H, dsPIC30, dsPIC33
void putsUART2(unsigned int *buffer);
#define putrsUART2(x) putsUART2( (unsigned int *)x)
unsigned int getsUART2(unsigned int length, unsigned int *buffer,
        unsigned int uart_data_wait);
char DataRdyUART2(void);
char BusyUART2(void);
unsigned int ReadUART2(void);
void WriteUART2(unsigned int data);
#elif defined(__XC32)
#define putrsUART2(x) putsUART2(x)
#endif

#endif
