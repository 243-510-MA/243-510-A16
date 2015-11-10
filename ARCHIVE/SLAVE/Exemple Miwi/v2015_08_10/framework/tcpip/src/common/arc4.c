/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    arc4.c

  Summary:
    Library for Microchip TCP/IP Stack
    - Provides encryption and decryption capabilities for the ARCFOUR
      algorithm, typically used as a bulk cipher for SSL
    - Reference: http://tools.ietf.org/html/draft-kaukonen-cipher-arcfour-01

  Description:
    ARCFOUR Cryptography Library

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

#define __ARCFOUR_C_

#include "tcpip/tcpip.h"

#if defined(STACK_USE_SSL_SERVER) || defined(STACK_USE_SSL_CLIENT)

/*****************************************************************************
  Function:
    void ARCFOURInitialize(ARCFOUR_CTX* ctx, uint8_t * key, uint16_t len)

  Summary:
    Initializes an ARCFOUR encryption stream.

  Description:
    This function initializes an ARCFOUR encryption stream.  Call this
    function to set up the initial state of the encryption context and the
    S-box.  The stream will be initialized to its zero state with the
    supplied key.

    This function can be used to initialize for encryption and decryption.

  Precondition:
    None

  Parameters:
    ctx - A pointer to the allocated encryption context structure
    key - A pointer to the key to be used
    len - The length of the data in key

  Returns:
    None

  Remarks:
    For security, the key should be destroyed after this call.
 ***************************************************************************/
void ARCFOURInitialize(ARCFOUR_CTX * ctx, uint8_t * key, uint16_t len)
{
    uint8_t temp, i, j, *Sbox;

    // Initialize the context indicies
    i = 0;
    j = 0;
    Sbox = ctx->Sbox;

    // Initialize each S-box element with its index
    do {
        Sbox[i] = i;
        i++;
    } while (i != 0u);

    // Fill in the S-box
    do {
        j = j + Sbox[i] + key[i % len];
        temp = Sbox[i];
        Sbox[i] = Sbox[j];
        Sbox[j] = temp;
        i++;
    } while (i != 0u);

    // Reset the context indicies
    ctx->i = 0;
    ctx->j = 0;
}

/*****************************************************************************
  Function:
    void ARCFOURCrypt(ARCFOUR_CTX* ctx, uint8_t * data, uint16_t len)

  Summary:
    Processes an array of data with the ARCFOUR algorithm.

  Description:
    This function uses the current ARCFOUR context to either encrypt or
    decrypt data in place.  The algorithm is the same for both processes,
    so this function can perform either procedure.

  Precondition:
    The encryption context ctx has been initialized with ARCFOURInitialize.

  Parameters:
    ctx - A pointer to the initialized encryption context structure
    data - The data to be encrypted or decrypted (in place)
    len - The length of data

  Returns:
    None
 ***************************************************************************/
void ARCFOURCrypt(ARCFOUR_CTX * ctx, uint8_t * data, uint16_t len)
{
    uint8_t temp, temp2, i, j, *Sbox;

    // Buffer context variables in local RAM for faster access
    i = ctx->i;
    j = ctx->j;
    Sbox = ctx->Sbox;

    // Loop over each byte.  Extract its key and XOR
    while (len--) {
        i++;
        temp = Sbox[i];
        j += temp;
        temp2 = Sbox[j];
        Sbox[i] = temp2;
        Sbox[j] = temp;
        temp += temp2;
        temp2 = Sbox[temp];

        *data++ ^= temp2;
    }

    // Save the new context
    ctx->i = i;
    ctx->j = j;
}

#endif // #if defined(STACK_USE_SSL_SERVER) || defined(STACK_USE_SSL_CLIENT)
