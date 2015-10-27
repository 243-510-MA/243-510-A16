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
 
#if !defined (_SHA512_H)
#define _SHA512_H
 
#include <stdint.h>

/****************************************************************************
  Section:
    Data Types
  ***************************************************************************/
// Enumeration for selecting digest bit length
typedef enum
{
    SHA2_384,           // SHA-384 hash
    SHA2_512            // SHA-512 hash
} SHA512_BIT_LENGTH;

// Context storage for hash operation
typedef struct
{
    uint64_t h[8];                                              // Hash state
    uint32_t totalBytes;                                        // Total number of bytes hashed so far
    uint8_t partialBlock[128] __attribute__((aligned(4)));      // Beginning of the next 128 byte block
    uint64_t * workingBuffer;                                   // 80 word uint64_t working buffer
    SHA512_BIT_LENGTH length;                                   // Type of hash being calculated (SHA-384 or SHA-512)
} SHA512_CONTEXT;

/****************************************************************************
  Section:
    Function Prototypes
  ***************************************************************************/

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
void SHA512_Initialize (SHA512_CONTEXT * context, SHA512_BIT_LENGTH length, uint64_t * workingBuffer);

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
void SHA512_DataAdd (SHA512_CONTEXT * context, uint8_t * data, uint32_t len);

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
void SHA512_Calculate (SHA512_CONTEXT * context, uint8_t * result);

#endif      // _SHA512_H
