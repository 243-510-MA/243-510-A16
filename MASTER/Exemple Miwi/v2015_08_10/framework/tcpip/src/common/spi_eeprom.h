/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    spi_eeprom.h

  Summary:
    

  Description:
    External Serial Data EEPROM Access Defs.

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

#ifndef __SPI_EEPROM_H_
#define __SPI_EEPROM_H_

typedef bool XEE_RESULT;
#define XEE_SUCCESS false

#if defined(EEPROM_CS_TRIS)
void XEEInit(void);
XEE_RESULT XEEBeginWrite(uint32_t address);
XEE_RESULT XEEWrite(uint8_t val);
void XEEWriteArray(uint8_t *val, uint16_t wLen);
XEE_RESULT XEEEndWrite(void);
XEE_RESULT XEEBeginRead(uint32_t address);
uint8_t XEERead(void);
XEE_RESULT XEEReadArray(uint32_t address, uint8_t *buffer, uint16_t length);
XEE_RESULT XEEEndRead(void);
bool XEEIsBusy(void);
#endif

// If the above funtions are called without EEPROM_CS_TRIS's definition, it
// means that you either have an error in your system_config.h or
// tcpip_config.h definitions. The code is attempting to call a function
// that can't possibly work because you have not specified what pins and SPI
// module the physical SPI EEPROM chip is connected to. Alternatively, if
// you don't have an SPI EERPOM chip, it means you have enabled a stack
// feature that requires SPI EEPROM hardware. In this case, you need to edit
// tcpip_config.h and disable this stack feature. The linker error tells you
// which object file this error was generated from. It should be a clue as
// to what feature you need to disable.

#endif
