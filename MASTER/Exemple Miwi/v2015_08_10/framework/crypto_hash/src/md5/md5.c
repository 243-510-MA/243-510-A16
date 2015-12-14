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

#include "crypto_hash/md5.h"
#include "./md5_private.h"
#include <stdarg.h>

/****************************************************************************
  Section:
    Functions and variables required for MD5
  ***************************************************************************/

// Array of pre-defined R vales for MD5
const static uint8_t _MD5_r[64] = {7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
                                      5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
                                      4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
                                      6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21};

// Array of pre-defined K values for MD5
const static uint32_t _MD5_k[64] = { 0xD76AA478, 0xE8C7B756, 0x242070DB, 0xC1BDCEEE, 0xF57C0FAF, 0x4787C62A, 0xA8304613, 0xFD469501,
                                        0x698098D8, 0x8B44F7AF, 0xFFFF5BB1, 0x895CD7BE, 0x6B901122, 0xFD987193, 0xA679438E, 0x49B40821,
                                        0xF61E2562, 0xC040B340, 0x265E5A51, 0xE9B6C7AA, 0xD62F105D, 0x02441453, 0xD8A1E681, 0xE7D3FBC8,
                                        0x21E1CDE6, 0xC33707D6, 0xF4D50D87, 0x455A14ED, 0xA9E3E905, 0xFCEFA3F8, 0x676F02D9, 0x8D2A4C8A,
                                        0xFFFA3942, 0x8771F681, 0x6D9D6122, 0xFDE5380C, 0xA4BEEA44, 0x4BDECFA9, 0xF6BB4B60, 0xBEBFBC70,
                                        0x289B7EC6, 0xEAA127FA, 0xD4EF3085, 0x04881D05, 0xD9D4D039, 0xE6DB99E5, 0x1FA27CF8, 0xC4AC5665,
                                        0xF4292244, 0x432AFF97, 0xAB9423A7, 0xFC93A039, 0x655B59C3, 0x8F0CCC92, 0xFFEFF47D, 0x85845DD1,
                                        0x6FA87E4F, 0xFE2CE6E0, 0xA3014314, 0x4E0811A1, 0xF7537E82, 0xBD3AF235, 0x2AD7D2BB, 0xEB86D391 };

/*****************************************************************************
  Function:
    void MD5_Initialize(MD5_CONTEXT* context)

  Description:
    Initializes a new MD5 hash.

  Precondition:
    None

  Parameters:
    context - pointer to the allocated MD5_CONTEXT object to initialize as MD5

  Returns:
      None
  ***************************************************************************/
void MD5_Initialize(MD5_CONTEXT* context)
{
    context->h0 = 0x67452301;
    context->h1 = 0xefcdab89;
    context->h2 = 0x98badcfe;
    context->h3 = 0x10325476;
    context->bytesSoFar = 0;
}

/*****************************************************************************
  Function:
    void MD5_DataAdd(MD5_CONTEXT* context, uint8_t* data, uint16_t len)

  Description:
    Adds data to an MD5 hash calculation.

  Precondition:
    The hash context has already been initialized.

  Parameters:
    context - a pointer to the hash context structure
    data - the data to add to the hash
    len - the length of the data to add

  Returns:
      None
  ***************************************************************************/
void MD5_DataAdd(MD5_CONTEXT* context, uint8_t* data, uint16_t len)
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
            MD5_HashBlock(context);
            blockPtr = context->partialBlock;
        }

        len--;
    }

}

/*****************************************************************************
  Function:
    void MD5_ROMDataAdd(MD5_CONTEXT* context, ROM uint8_t* data, uint16_t len)

  Description:
    Adds data to an MD5 hash calculation.

  Precondition:
    The hash context has already been initialized.

  Parameters:
    context - a pointer to the hash context structure
    data - the data to add to the hash
    len - the length of the data to add

  Returns:
      None

  Remarks:
      This function is aliased to MD5_DataAdd on non-PIC18 platforms.
  ***************************************************************************/
#if defined(__18CXX)
void MD5_ROMDataAdd(MD5_CONTEXT* context, ROM uint8_t* data, uint16_t len)
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
            MD5_HashBlock(context);
            blockPtr = context->partialBlock;
        }

        len--;
    }

}
#endif

void MD5_HashBlock(MD5_CONTEXT * context)
{
    uint32_t a, b, c, d, f, temp;
    uint8_t i, j;
    uint32_t test;

    // Set up a, b, c, d
    a = context->h0;
    b = context->h1;
    c = context->h2;
    d = context->h3;

    // Main mixer loop for 64 operations
    for(i = 0; i < 64u; i++)
    {
        if(i <= 15u)
        {
            f = (b & c) | ((~b) & d);
            j = i;
        }
        else if(i > 15u && i <= 31u)
        {
            f = (d & b) | ((~d) & c);
            j = (5 * i + 1) & 0x0f;
        }
        else if(i > 31u && i <= 47u)
        {
            f = (b ^ c ^ d);
            j = (3 * i + 5) & 0x0f;
        }
        else
        {
            f = c ^ (b | (~d));
            j = (7 * i) & 0x0f;
        }

        // Calculate the new mixer values
        temp = d;
        d = c;
        c = b;
        j *= 4;
        test = (*(uint32_t*)&context->partialBlock[j]);
        b = leftRotate32bit(a+f+_MD5_k[i]+(*(uint32_t*)&context->partialBlock[j]),_MD5_r[i]) + b;
        a = temp;
    }

    // Add the new hash to the sum
    context->h0 += a;
    context->h1 += b;
    context->h2 += c;
    context->h3 += d;

}

/*****************************************************************************
  Function:
    void MD5_Calculate(MD5_CONTEXT* context, uint8_t* result)

  Summary:
    Calculates an MD5 hash

  Description:
    This function calculates the hash sum of all input data so far.  It is
    non-destructive to the hash context, so more data may be added after
    this function is called.

  Precondition:
    The hash context has been properly initialized.

  Parameters:
    context - the current hash context
    result - 16 byte array in which to store the resulting hash

  Returns:
      None
  ***************************************************************************/
void MD5_Calculate(MD5_CONTEXT* context, uint8_t* result)
{
    uint32_t h0, h1, h2, h3;
    uint8_t offset;

    // Initialize the hash variables
    h0 = context->h0;
    h1 = context->h1;
    h2 = context->h2;
    h3 = context->h3;

    // Find out how far along we are in the partial block
    offset = ( context->bytesSoFar & 0x3f );

    // Add one more bit and 7 zeros
    context->partialBlock[offset++] = 0x80;

    // If there's 8 or more bytes left to 64, then this is the last block
    if(offset > 56u)
    {// If there's not enough space, then zero fill this and add a new block
        // Zero pad the remainder
        for( ; offset < 64u; context->partialBlock[offset++] = 0x00);

        // Calculate a hash on this block and add it to the sum
        MD5_HashBlock(context);
        offset = 0;
    }

    // Zero fill the rest of the block
    for( ; offset < 56u; context->partialBlock[offset++] = 0x00);

    // Fill in the size, in bits, in big-endian
    context->partialBlock[56] = context->bytesSoFar << 3;
    context->partialBlock[57] = context->bytesSoFar >> 5;
    context->partialBlock[58] = context->bytesSoFar >> 13;
    context->partialBlock[59] = context->bytesSoFar >> 21;
    context->partialBlock[60] = context->bytesSoFar >> 29;
    context->partialBlock[61] = 0;
    context->partialBlock[62] = 0;
    context->partialBlock[63] = 0;

    // Calculate a hash on this final block and add it to the sum
    MD5_HashBlock(context);

    // Format the result in little-endian format
    for (offset = 0; offset < 4; offset++)
    {
        *result++ = *(((uint8_t *)&context->h0) + offset);
    }
    for (offset = 0; offset < 4; offset++)
    {
        *result++ = *(((uint8_t *)&context->h1) + offset);
    }
    for (offset = 0; offset < 4; offset++)
    {
        *result++ = *(((uint8_t *)&context->h2) + offset);
    }
    for (offset = 0; offset < 4; offset++)
    {
        *result++ = *(((uint8_t *)&context->h3) + offset);
    }
}



