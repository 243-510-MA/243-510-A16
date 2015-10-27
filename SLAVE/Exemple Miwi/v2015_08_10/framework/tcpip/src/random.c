/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    random.c

  Summary:
    Library for Microchip TCP/IP Stack
     - Provides a cryptographically secure method for generating
       random data

  Description:
    Random Number Generator

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

#define __RANDOM_C_

#include "tcpip/tcpip.h"

#if defined(STACK_USE_SSL_SERVER) || defined(STACK_USE_SSL_CLIENT)

static HASH_SUM randHash;
static uint8_t output[20];
static uint8_t bCount;

/*********************************************************************
 * Function:        void RandomInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          Random number generator is initialized.
 *
 * Side Effects:    None
 *
 * Overview:        Sets up the random generator structure.
 *
 * Note:            Data may not be secure until several packets have
 *                  been received.
 ********************************************************************/
void RandomInit(void)
{
    unsigned char i;
    unsigned long dw;

    SHA1Initialize(&randHash);

    // Add some starting entropy to the pool.  This is slow.
    for (i = 0; i < 5; i++) {
        dw = GenerateRandomDWORD();
        RandomAdd(((uint8_t *) & dw)[0]);
        RandomAdd(((uint8_t *) & dw)[1]);
        RandomAdd(((uint8_t *) & dw)[2]);
        RandomAdd(((uint8_t *) & dw)[3]);
    }

    bCount = 20;
}

/*********************************************************************
 * Function:        uint8_t RandomGet(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          A random byte is generated
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
uint8_t RandomGet(void)
{
    if (bCount >= 20u) { //we need to get new random bytes
        SHA1Calculate(&randHash, output);
        RandomAdd(output[0]);
        bCount = 0;
    }

    //return the random byte
    return output[bCount++];
}

/*********************************************************************
 * Function:        void RandomAdd(uint8_t data)
 *
 * PreCondition:    None
 *
 * Input:           a random byte to add to the seed
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Hashes the byte and an asynchronous timer value
 *
 * Note:            None
 ********************************************************************/
void RandomAdd(uint8_t data)
{
    uint32_t dTemp;

    SHA1AddData(&randHash, &data, 1);
    dTemp = TickGet();
    SHA1AddData(&randHash, (uint8_t *) & dTemp, 1);

    bCount = 20;
}

#endif // #if defined(STACK_USE_SSL_SERVER) || defined(STACK_USE_SSL_CLIENT)
