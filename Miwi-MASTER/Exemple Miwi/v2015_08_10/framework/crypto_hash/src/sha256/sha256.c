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
#include "crypto_hash/sha256.h"
#include "./sha256_private.h"

const uint32_t SHA256_K[] = { \
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5, \
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, \
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, \
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967, \
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, \
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070, \
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3, \
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2 \
};

// *****************************************************************************
/*
  Function:
    void SHA256_Initialize (SHA256_CONTEXT * context, SHA256_BIT_LENGTH length, uint32_t * workingBuffer);

  Summary:
    Initializes a SHA-256 context to perform a SHA-256 hash.

  Description:
    This routine initializes a hash context for the SHA-256 hash.
    
  Precondition:
    None.

  Parameters:
    context - The context to initialize.
    length - Digest bit length to use with the SHA-256 algorithm.  SHA2_224 or SHA2_256.
    workingBuffer - A working buffer used by the module to calculate the hash.  If 
        the CRYPTO_HASH_CONFIG_SHA_SMALL_RAM macro is defined in sha_config.h, this 
        buffer must contain 16 uint32_t words.  Otherwise, this buffer must contain 
        64 32-bit words, but performance will be slightly improved.

  Returns:
    None.

  Example:
    <code>
    // Initialization for CRYPTO_HASH_CONFIG_SHA_SMALL_RAM
    uint32_t buffer[16];
    SHA256_CONTEXT context;
    SHA256_Initialize (&context, SHA2_256, buffer);
    </code>

  Remarks:
    You must initialize a context before calculating a SHA-256 hash.
*/
void SHA256_Initialize (SHA256_CONTEXT * context, SHA256_BIT_LENGTH length, uint32_t * workingBuffer)
{
    if (length == SHA2_256)
    {
        // Initialize context to produce 256-bit digest
        context->h[0] = 0x6a09e667;
        context->h[1] = 0xbb67ae85;
        context->h[2] = 0x3c6ef372;
        context->h[3] = 0xa54ff53a;
        context->h[4] = 0x510e527f;
        context->h[5] = 0x9b05688c;
        context->h[6] = 0x1f83d9ab;
        context->h[7] = 0x5be0cd19;
    }
    else
    {
        // Initialize context to produce 224-bit digest
        context->h[0] = 0xc1059ed8;
        context->h[1] = 0x367cd507;
        context->h[2] = 0x3070dd17;
        context->h[3] = 0xf70e5939;
        context->h[4] = 0xffc00b31;
        context->h[5] = 0x68581511;
        context->h[6] = 0x64f98fa7;
        context->h[7] = 0xbefa4fa4;
    }

    context->length = length;
    context->totalBytes = 0;
    context->workingBuffer = workingBuffer;
}

// *****************************************************************************
/*
  Function:
    void SHA256_DataAdd (SHA256_CONTEXT * context, uint8_t * data, uint32_t len);

  Summary:
    Adds data to a hash being calculated.

  Description:
    This routine adds data to a SHA-256 hash being calculated.  When the data 
    length reaches a block size (64 bytes), this function will calculate the hash 
    over that block and store the current hash value in the hash context.
    
  Precondition:
    The hash context must be initialized with SHA256_Initialize.

  Parameters:
    context - The context of the hash being calculated.
    data - The data being added.
    len - The length of the data being added.

  Returns:
    None.

  Example:
    <code>
    // Initialization for CRYPTO_HASH_CONFIG_SHA_SMALL_RAM
    uint8_t data[] = "Hello.";
    uint32_t buffer[16];
    SHA256_CONTEXT context;

    SHA256_Initialize (&context, SHA2_256, buffer);

    SHA256_DataAdd (&context, data, 6);
    </code>

  Remarks:
    None.
*/
void SHA256_DataAdd (SHA256_CONTEXT * context, uint8_t * data, uint32_t len)
{
    uint8_t * blockPtr;

    blockPtr = context->partialBlock + (context->totalBytes & 0x3f);

    // Update the total number of bytes
    context->totalBytes += len;

    // Copy data into the partial block
    while(len != 0u)
    {
        *blockPtr++ = *data++;

        // If the partial block is full, hash the data and start over
        if(blockPtr == context->partialBlock + 64)
        {
            SHA256_HashBlock (context);
            blockPtr = context->partialBlock;
        }

        len--;
    }

}

void SHA256_HashBlock (SHA256_CONTEXT * context)
{
    uint32_t a, b, c, d, e, f, g, h;
    uint8_t i;
    uint32_t t1, t2;
    uint8_t * dataPtr = context->partialBlock;;
    uint32_t * schedulePtr;
#if defined (CRYPTO_HASH_CONFIG_SHA_SMALL_RAM)
    uint8_t j;
#endif

    schedulePtr = context->workingBuffer;
    for (i = 0; i < 16; i++)
    {
        t1 = *dataPtr++;
        t1 <<= 8;
        t1 += *dataPtr++;
        t1 <<= 8;
        t1 += *dataPtr++;
        t1 <<= 8;
        t1 += *dataPtr++;
        *(schedulePtr + i) = t1;
    }

#if !defined (CRYPTO_HASH_CONFIG_SHA_SMALL_RAM)
    for (i = 16; i < 64; i++)
    {
        schedulePtr = context->workingBuffer + i;
        *schedulePtr = SHA2_SIG_L1(*(schedulePtr - 2)) + *(schedulePtr - 7) + SHA2_SIG_L0(*(schedulePtr - 15)) + *(schedulePtr - 16);
    }
#endif

    a = context->h[0];
    b = context->h[1];
    c = context->h[2];
    d = context->h[3];
    e = context->h[4];
    f = context->h[5];
    g = context->h[6];
    h = context->h[7];

    schedulePtr = context->workingBuffer;
    for (i = 0; i < 64; i++)
    {
#if defined (CRYPTO_HASH_CONFIG_SHA_SMALL_RAM)
        j = i & 0xf;
        if (i >= 16)
        {
            *(schedulePtr + j) = SHA2_SIG_L1(*(schedulePtr + ((14 + j) & 0xf))) + *(schedulePtr + ((9 + j) & 0xf)) + SHA2_SIG_L0(*(schedulePtr + ((1 + j) & 0xf))) + *(schedulePtr + j);
        }
        t1 = h + SHA2_SIG_U1(e) + SHA2_CH(e,f,g) + SHA256_K[i] + *(schedulePtr + j);
#else
        t1 = h + SHA2_SIG_U1(e) + SHA2_CH(e,f,g) + SHA256_K[i] + *(schedulePtr + i);
#endif
        t2 = SHA2_SIG_U0(a) + SHA2_MAJ(a,b,c);
        h = g;
        g = f;
        f = e;
        e = d + t1;
        d = c;
        c = b;
        b = a;
        a = t1 + t2;
    }

    context->h[0] += a;
    context->h[1] += b;
    context->h[2] += c;
    context->h[3] += d;
    context->h[4] += e;
    context->h[5] += f;
    context->h[6] += g;
    context->h[7] += h;
}

// *****************************************************************************
/*
  Function:
    void SHA256_Calculate (SHA256_CONTEXT * context, uint8_t * result);

  Summary:
    Finishes calculating a hash.

  Description:
    This routine finishes calculating a SHA-256 hash.  It will automatically add 
    the padding required by the hashing algorithm and return the hash digest.
    
  Precondition:
    The hash context must be initialized with SHA256_Initialize.

  Parameters:
    context - The context of the hash being calculated.
    result - A buffer to store the calculated hash digest.  32 bytes for SHA-256,
        28 bytes for SHA-224.

  Returns:
    None.

  Example:
    <code>
    // Initialization for CRYPTO_HASH_CONFIG_SHA_SMALL_RAM
    uint8_t data[] = "Hello.";
    uint32_t buffer[16];
    SHA256_CONTEXT context;
    uint8_t digest[32];

    SHA256_Initialize (&context, SHA2_256, buffer);

    SHA256_DataAdd (&context, data, 6);
    SHA256_Calculate (&context, digest);
    </code>

  Remarks:
    None.
*/
void SHA256_Calculate (SHA256_CONTEXT * context, uint8_t * result)
{
    uint8_t * blockPtr = context->partialBlock + (context->totalBytes & 0x3F);
    uint8_t * endPtr;
    uint8_t i;

    *blockPtr++ = 0x80;

    endPtr = context->partialBlock + 56;

    if(blockPtr > endPtr)
    {
        endPtr += 8;
        while (blockPtr < endPtr)
        {
            *blockPtr++ = 0x00;
        }
        SHA256_HashBlock(context);
        blockPtr = context->partialBlock;
        endPtr -= 8;
    }

    // Set the offset to 5 bytes before the end of the block
    endPtr += 3;
    while (blockPtr < endPtr)
    {
        *blockPtr++ = 0x00;
    }

    *blockPtr++ = context->totalBytes >> 29;
    *blockPtr++ = context->totalBytes >> 21;
    *blockPtr++ = context->totalBytes >> 13;
    *blockPtr++ = context->totalBytes >> 5;
    *blockPtr++ = context->totalBytes << 3;

    // Calculate a hash on this final block and add it to the sum
    SHA256_HashBlock(context);

    // Format the result in big-endian format
    for (i = 0; i < 7; i++)
    {
        SHA256_OutputData (result + (i << 2), context->h[i]);
    }

    if (context->length == SHA2_256)
    {
        SHA256_OutputData (result + 28, context->h[7]);
    }
}

void SHA256_OutputData (uint8_t * output, uint32_t value)
{
    *(output++) = (uint8_t)(value >> 24);
    *(output++) = (uint8_t)(value >> 16);
    *(output++) = (uint8_t)(value >> 8);
    *(output++) = (uint8_t)value;
}

