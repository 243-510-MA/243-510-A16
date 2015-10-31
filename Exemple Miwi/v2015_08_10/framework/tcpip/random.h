/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    random.h

  Summary:
    

  Description:
    Secure Random Number Generator Headers

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

#ifndef __RANDOM_H_
#define __RANDOM_H_

#if defined(STACK_USE_RANDOM)

#define STACK_USE_SHA1

//Entropy is generated as follows:
//  1.  For every packet received, the last byte of the remote
//      MAC address and the four bytes of TickGet() are added
//      to a SHA-1 Hash (the seed)
//  2.  Every time a random byte is requested, the hash is
//      calculated and the hash sum is hashed into the seed.
//  3.  Up to 20 bytes are returned from this hash sum before
//      a new hash is calculated.
//  4.  Every time new entropy is added, the remaining random
//      output is discarded and new random bytes will be
//      generated upon future calls to RandomGet().

void RandomInit(void);
uint8_t RandomGet(void);
void RandomAdd(uint8_t data);

#endif

#endif
