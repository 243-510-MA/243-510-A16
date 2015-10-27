/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    arc4.h

  Summary:
    

  Description:
    ARCFOUR Cryptography Headers

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

#ifndef __ARCFOUR_H_
#define __ARCFOUR_H_

// Encryption Context for ARCFOUR module.
// The program need not access any of these values directly, but rather
// only store the structure and use ARCFOURInitialize to set it up.

typedef struct {
    uint8_t i; // The iterator variable
    uint8_t j; // The co-iterator
    uint8_t *Sbox; // A pointer to a 256 byte S-box array
} ARCFOUR_CTX;

void ARCFOURInitialize(ARCFOUR_CTX* ctx, uint8_t * key, uint16_t len);
void ARCFOURCrypt(ARCFOUR_CTX* ctx, uint8_t * data, uint16_t len);

#endif
