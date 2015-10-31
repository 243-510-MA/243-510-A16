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
#include "crypto_hash/sha512.h"
#include "./sha512_private.h"

const uint64_t SHA512_K[] = { \
    0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc, \
    0x3956c25bf348b538, 0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118, \
    0xd807aa98a3030242, 0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2, \
    0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 0xc19bf174cf692694, \
    0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65, \
    0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5, \
    0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4, \
    0xc6e00bf33da88fc2, 0xd5a79147930aa725, 0x06ca6351e003826f, 0x142929670a0e6e70, \
    0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df, \
    0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b, \
    0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30, \
    0xd192e819d6ef5218, 0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8, \
    0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8, \
    0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3, \
    0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec, \
    0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b, \
    0xca273eceea26619c, 0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178, \
    0x06f067aa72176fba, 0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b, \
    0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c, \
    0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817
};

// *****************************************************************************
/*
  Function:
    void SHA512_Initialize (SHA512_CONTEXT * context, SHA512_BIT_LENGTH length, uint32_t * workingBuffer);

  Summary:
    Initializes a SHA-256 context to perform a SHA-512 hash.

  Description:
    This routine initializes a hash context for the SHA-512 hash.
    
  Precondition:
    None.

  Parameters:
    context - The context to initialize.
    length - Digest bit length to use with the SHA-512 algorithm.  SHA2_384 or SHA2_512.
    workingBuffer - A working buffer used by the module to calculate the hash.  If 
        the CRYPTO_HASH_CONFIG_SHA_SMALL_RAM macro is defined in sha_config.h, this 
        buffer must contain 16 uint64_t words.  Otherwise, this buffer must contain 
        80 64-bit words, but performance will be slightly improved.

  Returns:
    None.

  Example:
    <code>
    // Initialization for CRYPTO_HASH_CONFIG_SHA_SMALL_RAM
    uint64_t buffer[16];
    SHA512_CONTEXT context;
    SHA512_Initialize (&context, SHA2_512, buffer);
    </code>

  Remarks:
    You must initialize a context before calculating a SHA-512 hash.
*/
void SHA512_Initialize (SHA512_CONTEXT * context, SHA512_BIT_LENGTH length, uint64_t * workingBuffer)
{
    if (length == SHA2_512)
    {
        // Initialize context to produce 512-bit digest
        context->h[0] = 0x6a09e667f3bcc908;
        context->h[1] = 0xbb67ae8584caa73b;
        context->h[2] = 0x3c6ef372fe94f82b;
        context->h[3] = 0xa54ff53a5f1d36f1;
        context->h[4] = 0x510e527fade682d1;
        context->h[5] = 0x9b05688c2b3e6c1f;
        context->h[6] = 0x1f83d9abfb41bd6b;
        context->h[7] = 0x5be0cd19137e2179;
    }
    else
    {
        // Initialize context to produce 384-bit digest
        context->h[0] = 0xcbbb9d5dc1059ed8;
        context->h[1] = 0x629a292a367cd507;
        context->h[2] = 0x9159015a3070dd17;
        context->h[3] = 0x152fecd8f70e5939;
        context->h[4] = 0x67332667ffc00b31;
        context->h[5] = 0x8eb44a8768581511;
        context->h[6] = 0xdb0c2e0d64f98fa7;
        context->h[7] = 0x47b5481dbefa4fa4;
    }

    context->length = length;
    context->totalBytes = 0;
    context->workingBuffer = workingBuffer;
}

// *****************************************************************************
/*
  Function:
    void SHA512_DataAdd (SHA512_CONTEXT * context, uint8_t * data, uint32_t len);

  Summary:
    Adds data to a hash being calculated.

  Description:
    This routine adds data to a SHA-512 hash being calculated.  When the data 
    length reaches a block size (128 bytes), this function will calculate the hash 
    over that block and store the current hash value in the hash context.
    
  Precondition:
    The hash context must be initialized with SHA512_Initialize.

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
    uint64_t buffer[16];
    SHA512_CONTEXT context;

    SHA512_Initialize (&context, SHA2_512, buffer);

    SHA512_DataAdd (&context, data, 6);
    </code>

  Remarks:
    None.
*/
void SHA512_DataAdd (SHA512_CONTEXT * context, uint8_t * data, uint32_t len)
{
    uint8_t * blockPtr;

    blockPtr = context->partialBlock + (context->totalBytes & 0x7f);

    // Update the total number of bytes
    context->totalBytes += len;

    // Copy data into the partial block
    while(len != 0u)
    {
        *blockPtr++ = *data++;

        // If the partial block is full, hash the data and start over
        if(blockPtr == context->partialBlock + 128)
        {
            SHA512_HashBlock (context);
            blockPtr = context->partialBlock;
        }

        len--;
    }
}

void SHA512_HashBlock (SHA512_CONTEXT * context)
{
    uint64_t a, b, c, d, e, f, g, h;
    uint8_t i;
    uint64_t t1, t2;
    uint8_t * dataPtr = context->partialBlock;;
    uint64_t * schedulePtr;
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
        t1 <<= 8;
        t1 += *dataPtr++;
        t1 <<= 8;
        t1 += *dataPtr++;
        t1 <<= 8;
        t1 += *dataPtr++;
        t1 <<= 8;
        t1 += *dataPtr++;
        *(schedulePtr + i) = t1;
    }

#if !defined (CRYPTO_HASH_CONFIG_SHA_SMALL_RAM)
    for (i = 16; i < 80; i++)
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
    for (i = 0; i < 80; i++)
    {
#if defined (CRYPTO_HASH_CONFIG_SHA_SMALL_RAM)
        j = i & 0xf;
        if (i >= 16)
        {
            *(schedulePtr + j) = SHA2_SIG_L1(*(schedulePtr + ((14 + j) & 0xf))) + *(schedulePtr + ((9 + j) & 0xf)) + SHA2_SIG_L0(*(schedulePtr + ((1 + j) & 0xf))) + *(schedulePtr + j);
        }
        t1 = h + SHA2_SIG_U1(e) + SHA2_CH(e,f,g) + SHA512_K[i] + *(schedulePtr + j);
#else
        t1 = h + SHA2_SIG_U1(e) + SHA2_CH(e,f,g) + SHA512_K[i] + *(schedulePtr + i);
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
    void SHA512_Calculate (SHA512_CONTEXT * context, uint8_t * result);

  Summary:
    Finishes calculating a hash.

  Description:
    This routine finishes calculating a SHA-512 hash.  It will automatically add 
    the padding required by the hashing algorithm and return the hash digest.
    
  Precondition:
    The hash context must be initialized with SHA512_Initialize.

  Parameters:
    context - The context of the hash being calculated.
    result - A buffer to store the calculated hash digest.  48 bytes for SHA-384,
        64 bytes for SHA-512.

  Returns:
    None.

  Example:
    <code>
    // Initialization for CRYPTO_HASH_CONFIG_SHA_SMALL_RAM
    uint8_t data[] = "Hello.";
    uint64_t buffer[16];
    SHA512_CONTEXT context;
    uint8_t digest[64];

    SHA512_Initialize (&context, SHA2_512, buffer);

    SHA512_DataAdd (&context, data, 6);
    SHA512_Calculate (&context, digest);
    </code>

  Remarks:
    None.
*/
void SHA512_Calculate (SHA512_CONTEXT * context, uint8_t * result)
{
    uint8_t * blockPtr = context->partialBlock + (context->totalBytes & 0x7F);
    uint8_t * endPtr;
    uint8_t i;

    *blockPtr++ = 0x80;

    endPtr = context->partialBlock + 112;

    if(blockPtr > endPtr)
    {
        endPtr += 16;
        while (blockPtr < endPtr)
        {
            *blockPtr++ = 0x00;
        }
        SHA512_HashBlock(context);
        blockPtr = context->partialBlock;
        endPtr -= 16;
    }

    // Set the offset to 5 bytes before the end of the block
    endPtr += 11;
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
    SHA512_HashBlock(context);

    // Format the result in big-endian format
    for (i = 0; i < 6; i++)
    {
        SHA512_OutputData (result + (i << 3), context->h[i]);
    }

    if (context->length == SHA2_512)
    {
        SHA512_OutputData (result + 48, context->h[6]);
        SHA512_OutputData (result + 56, context->h[7]);
    }
}

void SHA512_OutputData (uint8_t * output, uint64_t value)
{
    *(output++) = (uint8_t)(value >> 56);
    *(output++) = (uint8_t)(value >> 48);
    *(output++) = (uint8_t)(value >> 40);
    *(output++) = (uint8_t)(value >> 32);
    *(output++) = (uint8_t)(value >> 24);
    *(output++) = (uint8_t)(value >> 16);
    *(output++) = (uint8_t)(value >> 8);
    *(output++) = (uint8_t)value;
}

