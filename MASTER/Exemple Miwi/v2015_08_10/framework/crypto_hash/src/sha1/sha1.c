/******************************************************************************
 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the "Company") for its PICmicro(r) Microcontroller is intended and
 supplied to you, the Company's customer, for use solely and
 exclusively on Microchip PICmicro Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
********************************************************************/

#include "system_config.h"
#include "crypto_hash/sha1.h"
#include "./sha1_private.h"
#include <stdarg.h>

/****************************************************************************
  Section:
    Functions and variables required for SHA-1
  ***************************************************************************/

/*****************************************************************************
  Function:
    void SHA1_Initialize(SHA1_CONTEXT* context)

  Description:
    Initializes a new SHA-1 hash.

  Precondition:
    None

  Parameters:
    context - pointer to the allocated HASH_SUM object to initialize as SHA-1
        workingBuffer - A buffer used to construct a message schedule for hash calculation

  Returns:
      None
  ***************************************************************************/
void SHA1_Initialize(SHA1_CONTEXT* context, uint32_t * workingBuffer)
{
    context->h0 = 0x67452301;
    context->h1 = 0xEFCDAB89;
    context->h2 = 0x98BADCFE;
    context->h3 = 0x10325476;
    context->h4 = 0xC3D2E1F0;
    context->bytesSoFar = 0;
    context->workingBuffer = workingBuffer;
}

/*****************************************************************************
  Function:
    void SHA1_DataAdd(SHA1_CONTEXT* context, uint8_t* data, uint16_t len)

  Description:
    Adds data to a SHA-1 hash calculation.

  Precondition:
    The hash context has already been initialized.

  Parameters:
    context - a pointer to the hash context structure
    data - the data to add to the hash
    len - the length of the data to add

  Returns:
      None
  ***************************************************************************/
void SHA1_DataAdd(SHA1_CONTEXT* context, uint8_t * data, uint16_t len)
{
    uint8_t *blockPtr;

    // Seek to the first free byte
    blockPtr = context->partialBlock + ( context->bytesSoFar & 0x3f );

    // Update the total number of bytes
    context->bytesSoFar += len;

    // Copy data into the partial block
    while(len != 0u)
    {
        *blockPtr++ = *data++;

        // If the partial block is full, hash the data and start over
        if(blockPtr == context->partialBlock + 64)
        {
            SHA1_HashBlock(context);
            blockPtr = context->partialBlock;
        }

        len--;
    }

}

/*****************************************************************************
  Function:
    void SHA1_ROMDataAdd(SHA1_CONTEXT * context, ROM uint8_t * data, uint16_t len)

  Description:
    Adds data to a SHA-1 hash calculation.

  Precondition:
    The hash context has already been initialized.

  Parameters:
    context - a pointer to the hash context structure
    data - the data to add to the hash
    len - the length of the data to add

  Returns:
      None

  Remarks:
      This function is aliased to SHA1AddData on non-PIC18 platforms.
  ***************************************************************************/
#if defined(__18CXX)
void SHA1_ROMDataAdd(SHA1_CONTEXT* context, ROM uint8_t * data, uint16_t len)
{
    uint8_t *blockPtr;

    // Seek to the first free byte
    blockPtr = context->partialBlock + ( context->bytesSoFar & 0x3f );

    // Update the total number of bytes
    context->bytesSoFar += len;

    // Copy data into the partial block
    while(len != 0u)
    {
        *blockPtr++ = *data++;

        // If the partial block is full, hash the data and start over
        if(blockPtr == context->partialBlock + 64)
        {
            SHA1_HashBlock(context);
            blockPtr = context->partialBlock;
        }

        len--;
    }

}
#endif

void SHA1_HashBlock(SHA1_CONTEXT * context)
{
    uint32_t a, b, c, d, e, f, k, temp;
    uint8_t i;
    uint32_t * schedulePtr;
    uint8_t * dataPtr = context->partialBlock;
#if defined (CRYPTO_HASH_CONFIG_SHA_SMALL_RAM)
    uint8_t j;
#endif
    
    // Set up a, b, c, d, e
    a = context->h0;
    b = context->h1;
    c = context->h2;
    d = context->h3;
    e = context->h4;

    // Set up the message schedule
    schedulePtr = context->workingBuffer;
    for(i = 0; i < 16u; i++)
    {
        temp = *dataPtr++;
        temp <<= 8;
        temp += *dataPtr++;
        temp <<= 8;
        temp += *dataPtr++;
        temp <<= 8;
        temp += *dataPtr++;
        *(schedulePtr + i) = temp;
    }

#if !defined (CRYPTO_HASH_CONFIG_SHA_SMALL_RAM)
    for (i = 16; i < 80; i++)
    {
        schedulePtr = context->workingBuffer + i;
        *(schedulePtr) = leftRotate32bit(*(schedulePtr - 3) ^ *(schedulePtr - 8) ^
                *(schedulePtr - 14) ^ *(schedulePtr - 16), 1);
    }
#endif

    schedulePtr = context->workingBuffer;

    // Main mixer loop for 80 operations
    for(i = 0; i < 80u; i++)
    {
#if defined (CRYPTO_HASH_CONFIG_SHA_SMALL_RAM)
        j = i & 0x0f;
        if (i >= 16)
        {
            *(schedulePtr + j) = leftRotate32bit (*(schedulePtr + ((13 + j) & 0xf)) ^ *(schedulePtr + ((8 + j) & 0xf)) ^ *(schedulePtr + ((2 + j) & 0xf)) ^ *(schedulePtr + j), 1);
        }
#endif

        if(i <= 19u)
        {
            f = (b & c) | ((~b) & d);
            k = 0x5A827999;
        }
        else if(i >= 20u && i <= 39u)
        {
            f = b ^ c ^ d;
            k = 0x6ED9EBA1;
        }
        else if(i >= 40u && i <= 59u)
        {
            f = (b & c) | (b & d) | (c & d);
            k = 0x8F1BBCDC;
        }
        else
        {
            f = b ^ c ^ d;
            k = 0xCA62C1D6;
        }

        // Calculate the new mixers
#if defined (CRYPTO_HASH_CONFIG_SHA_SMALL_RAM)
        temp = leftRotate32bit(a, 5) + f + e + k + *(schedulePtr + j);
#else
        temp = leftRotate32bit(a, 5) + f + e + k + *(schedulePtr + i);
#endif
        e = d;
        d = c;
        c = leftRotate32bit(b, 30);
        b = a;
        a = temp;
    }

    // Add the new hash to the sum
    context->h0 += a;
    context->h1 += b;
    context->h2 += c;
    context->h3 += d;
    context->h4 += e;
}

/*****************************************************************************
  Function:
    void SHA1_Calculate(SHA1_CONTEXT* context, uint8_t * result)

  Summary:
    Calculates a SHA-1 hash

  Description:
    This function calculates the hash sum of all input data so far.  It is
    non-destructive to the hash context, so more data may be added after
    this function is called.

  Precondition:
    The hash context has been properly initialized.

  Parameters:
    context - the current hash context
    result - 20 byte array in which to store the resulting hash

  Returns:
      None
  ***************************************************************************/
void SHA1_Calculate(SHA1_CONTEXT * context, uint8_t * result)
{
    uint32_t h0, h1, h2, h3, h4;
    uint8_t offset;

    // Initialize the hash variables
    h0 = context->h0;
    h1 = context->h1;
    h2 = context->h2;
    h3 = context->h3;
    h4 = context->h4;

    // Find out how far along we are in the partial block and copy to last block
    offset = ( context->bytesSoFar & 0x3f );

    // Add one more bit and 7 zeros
    context->partialBlock[offset++] = 0x80;

    // If there's 8 or more bytes left to 64, then this is the last block
    if(offset > 56u)
    {// If there's not enough space, then zero fill this and add a new block
        // Zero pad the remainder
        for( ; offset < 64u; context->partialBlock[offset++] = 0x00);

        // Calculate a hash on this block and add it to the sum
        SHA1_HashBlock(context);
                offset = 0;
    }

    // Zero fill the rest of the block
    for( ; offset < 56u; context->partialBlock[offset++] = 0x00);

    // Fill in the size, in bits, in big-endian
    context->partialBlock[63] = context->bytesSoFar << 3;
    context->partialBlock[62] = context->bytesSoFar >> 5;
    context->partialBlock[61] = context->bytesSoFar >> 13;
    context->partialBlock[60] = context->bytesSoFar >> 21;
    context->partialBlock[59] = context->bytesSoFar >> 29;
    context->partialBlock[58] = 0;
    context->partialBlock[57] = 0;
    context->partialBlock[56] = 0;

    // Calculate a hash on this final block and add it to the sum
    SHA1_HashBlock(context);

    // Format the result in big-endian format
    *result++ = ((uint8_t *)&context->h0)[3];
    *result++ = ((uint8_t *)&context->h0)[2];
    *result++ = ((uint8_t *)&context->h0)[1];
    *result++ = ((uint8_t *)&context->h0)[0];
    *result++ = ((uint8_t *)&context->h1)[3];
    *result++ = ((uint8_t *)&context->h1)[2];
    *result++ = ((uint8_t *)&context->h1)[1];
    *result++ = ((uint8_t *)&context->h1)[0];
    *result++ = ((uint8_t *)&context->h2)[3];
    *result++ = ((uint8_t *)&context->h2)[2];
    *result++ = ((uint8_t *)&context->h2)[1];
    *result++ = ((uint8_t *)&context->h2)[0];
    *result++ = ((uint8_t *)&context->h3)[3];
    *result++ = ((uint8_t *)&context->h3)[2];
    *result++ = ((uint8_t *)&context->h3)[1];
    *result++ = ((uint8_t *)&context->h3)[0];
    *result++ = ((uint8_t *)&context->h4)[3];
    *result++ = ((uint8_t *)&context->h4)[2];
    *result++ = ((uint8_t *)&context->h4)[1];
    *result++ = ((uint8_t *)&context->h4)[0];
}


