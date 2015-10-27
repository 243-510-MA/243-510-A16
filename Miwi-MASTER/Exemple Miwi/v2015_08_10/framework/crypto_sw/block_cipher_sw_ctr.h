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

#if !defined (_BLOCK_CIPHER_SW_CTR_H)
#define _BLOCK_CIPHER_SW_CTR_H

#include <stdint.h>
#include "system_config.h"

// Context structure for the counter operation
typedef struct
{
    uint8_t __attribute__((aligned)) noncePlusCounter[CRYPTO_CONFIG_SW_BLOCK_MAX_SIZE];      // Buffer containing the initial NONCE and counter.
    uint8_t __attribute__((aligned)) counter[CRYPTO_CONFIG_SW_BLOCK_MAX_SIZE];               // Buffer containing the current counter value.
    BLOCK_CIPHER_SW_FunctionEncrypt encrypt;                                   // Encrypt function for the algorithm being used with the block cipher mode module
    BLOCK_CIPHER_SW_FunctionDecrypt decrypt;                                   // Decrypt function for the algorithm being used with the block cipher mode module
    void * keyStream;                                                       // Pointer to the key stream.  Must be a multiple of the cipher's block size, but smaller than 2^25 bytes.
    void * keyStreamCurrentPosition;                                        // Pointer to the current position in the key stream.
    uint32_t keyStreamSize;                                                 // Size of the key stream.
    uint32_t bytesRemainingInKeyStream;                                     // Number of bytes remaining in the key stream
    uint32_t blockSize;                                                     // Block size of the cipher algorithm being used with the block cipher mode module
    void * key;                                                             // Key location
    CRYPTO_SW_KEY_TYPE keyType;                                                // Format of the key
} BLOCK_CIPHER_SW_CTR_CONTEXT;


// *****************************************************************************
/* Function:
    BLOCK_CIPHER_SW_ERRORS BLOCK_CIPHER_SW_CTR_Initialize (BLOCK_CIPHER_SW_HANDLE handle,
        BLOCK_CIPHER_SW_CTR_CONTEXT * context,
        BLOCK_CIPHER_SW_FunctionEncrypt encryptFunction, 
        BLOCK_CIPHER_SW_FunctionDecrypt decryptFunction, uint32_t blockSize,
        void * key, CRYPTO_SW_KEY_TYPE keyType)

  Summary:
    Initializes a CTR context for encryption/decryption.
  
  Description:
    Initializes a CTR context for encryption/decryption.  The user will specify 
    details about the algorithm being used in CTR mode.
  
  Precondition:
    Any required initialization needed by the block cipher algorithm must
    have been performed.

  Parameters:
    handle - A handle that is passed to the block cipher's encrypt/decrypt
        functions to specify which instance of the block cipher module to use.
    context - The CTR context to initialize.
    encryptFunction - Pointer to the encryption function for the block cipher
        algorithm being used in CTR mode.
    decryptFunction - Pointer to the decryption function for the block cipher
        algorithm being used in CTR mode.
    blockSize - The block size of the block cipher algorithm being used in CTR mode.
    noncePlusCounter - A security nonce concatenated with the initial value of the
        counter for this operation.  The counter can be 32, 64, or 128 bits,
        depending on the encrypt/decrypt options selected.
    keyStream - Pointer to a buffer to contain a calculated keyStream.
    keyStreamSize - The size of the keystream buffer, in bytes.
    key - The cryptographic key location
    keyType - The storage type of the key

  Returns:
      * BLOCK_CIPHER_SW_ERROR_NONE - no error.
      * BLOCK_CIPHER_SW_ERROR_UNSUPPORTED_KEY_TYPE - The specified key type is not
        supported by the firmware implementation being used
      * BLOCK_CIPHER_SW_ERROR_INVALID_HANDLE - The specified handle was invalid
    
  Example:
    <code>
    // Initialize the CTR block cipher module for use with AES.
    SYS_MODULE_OBJ sysObject;
    BLOCK_CIPHER_SW_HANDLE handle;
    BLOCK_CIPHER_SW_CTR_CONTEXT context;
    // Initialization vector for CTR mode
    static uint8_t initialization_vector[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    //keyStream could also be allocated memory instead of fixed memory
    uint8_t keyStream[AES_SW_BLOCK_SIZE*4];
    // The encryption key
    static uint8_t AESKey128[] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
    // Structure to contain the created AES round keys
    AES_SW_ROUND_KEYS_128_BIT round_keys;
    // Error type
    BLOCK_CIPHER_SW_ERRORS error;

    sysObject = BLOCK_CIPHER_SW_Initialize (BLOCK_CIPHER_SW_INDEX, NULL);
    if (sysObject != SYS_MODULE_OBJ_STATIC)
    {
        // error
    }

    handle = BLOCK_CIPHER_SW_Open (BLOCK_CIPHER_SW_INDEX, 0);
    if (handle == BLOCK_CIPHER_SW_HANDLE_INVALID)
    {
        // error
    }
    
    //Create the AES round keys.  This only needs to be done once for each AES key.
    AES_SW_RoundKeysCreate (&round_keys, (uint8_t*)AESKey128, AES_SW_KEY_SIZE_128_BIT);

    // Initialize the block cipher module
    error = BLOCK_CIPHER_SW_CTR_Initialize (handle, &context, AES_SW_Encrypt, AES_SW_Decrypt, AES_SW_BLOCK_SIZE, initialization_vector, (void *)&keyStream, sizeof (keyStream), &round_keys, CRYPTO_SW_KEY_SOFTWARE_EXPANDED);

    if (error != BLOCK_CIPHER_SW_ERROR_NONE)
    {
        // error
    }
    </code>
*/
BLOCK_CIPHER_SW_ERRORS BLOCK_CIPHER_SW_CTR_Initialize (BLOCK_CIPHER_SW_HANDLE handle, BLOCK_CIPHER_SW_CTR_CONTEXT * context, BLOCK_CIPHER_SW_FunctionEncrypt encryptFunction, BLOCK_CIPHER_SW_FunctionDecrypt decryptFunction, uint32_t blockSize, uint8_t * noncePlusCounter, void * keyStream, uint32_t keyStreamSize, void * key, CRYPTO_SW_KEY_TYPE keyType);

/**************************************************************************************************************************************************************
  Function:
       BLOCK_CIPHER_SW_ERRORS BLOCK_CIPHER_SW_CTR_Encrypt (BLOCK_CIPHER_SW_HANDLE handle,
           uint8_t * cipherText, uint32_t * numCipherBytes, uint8_t * plainText, 
           uint32_t numBytes, uint32_t options)
    
  Summary:
    Encrypts plain text using counter mode.
  Description:
    Encrypts plain text using counter mode.
  Conditions:
    The CTR context must be initialized with the block cipher
    encrypt/decrypt functions and the block cipher algorithm's block size.
    The block cipher module must be initialized, if necessary.
    
    The noncePlusCounter parameter in the BLOCK_CIPHER_SW_CTR_CONTEXT
    structure should be initialized. The size of this vector is the same as
    the block size of the cipher you are using.
  Input:
    handle -         A handle that is passed to the block cipher's
                     encrypt/decrypt functions to specify which instance of
                     the block cipher module to use.
    cipherText -     The cipher text produced by the encryption. This buffer
                     must be at least numBytes long.
    numCipherBytes - Pointer to a uint32_t; the number of bytes encrypted
                     will be returned in this parameter.
    plainText -      The plain test to encrypt. Must be at least numBytes long.
    numPlainBytes -  The number of plain text bytes that must be encrypted.
    options -        Block cipher encryption options that the user can specify,
                     or'd together. If BLOCK_CIPHER_SW_OPTION_STREAM_START is not
                     specified then BLOCK_CIPHER_SW_OPTION_STREAM_CONTINUE is
                     assumed. Valid options for this function are
                     * BLOCK_CIPHER_SW_OPTION_STREAM_START
                     * BLOCK_CIPHER_SW_OPTION_STREAM_CONTINUE
                     * BLOCK_CIPHER_SW_OPTION_CTR_32BIT
                     * BLOCK_CIPHER_SW_OPTION_CTR_64BIT
                     * BLOCK_CIPHER_SW_OPTION_CTR_128BIT
  Return:
    Returns a member of the BLOCK_CIPHER_SW_ERRORS enumeration:
      * BLOCK_CIPHER_SW_ERROR_NONE - no error.
      * BLOCK_CIPHER_SW_ERROR_KEY_STREAM_GEN_OUT_OF_SPACE - There was not
        enough room remaining in the context->keyStream buffer to fit the
        key data requested by the numBlocks parameter.
      * BLOCK_CIPHER_SW_ERROR_CTR_COUNTER_EXPIRED - The requesting call
        has caused the counter number to run out of unique combinations.
      * BLOCK_CIPHER_SW_ERROR_INVALID_HANDLE - The specified handle was invalid

  Example:
    <code>
    // ***************************************************************
    // Encrypt data in CTR mode with the AES algorithm.
    // ***************************************************************
    
    // System module object variable (for initializing AES)
    SYS_MODULE_OBJ sysObject;
    
    // Drive handle variable, to describe which AES module to use
    BLOCK_CIPHER_SW_HANDLE handle;
    
    // CTR mode context
    BLOCK_CIPHER_SW_CTR_CONTEXT context;

    // Initialization vector for CTR mode
    static uint8_t initialization_vector[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

    // Plain text to encrypt
    static uint8_t plain_text[] = { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a, \
                                    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51, \
                                    0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef, \
                                    0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10};
    // The encryption key
    static uint8_t AESKey128[] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
    // Structure to contain the created AES round keys
    AES_SW_ROUND_KEYS_128_BIT round_keys;
    // Buffer to contain encrypted plaintext
    uint8_t cipher_text[sizeof(plain_text)];
    //keyStream could also be allocated memory instead of fixed memory
    uint8_t keyStream[AES_SW_BLOCK_SIZE*4];
    // Error type
    BLOCK_CIPHER_SW_ERRORS error;
    // Number of bytes encrypted
    uint32_t numCipherBytes;
    
    // Initialization call for the AES module
    sysObject = BLOCK_CIPHER_SW_Initialize (BLOCK_CIPHER_SW_INDEX, NULL);
    if (sysObject != SYS_MODULE_OBJ_STATIC)
    {
        // error
    }

    // Driver open call for the AES module
    handle = BLOCK_CIPHER_SW_Open (BLOCK_CIPHER_SW_INDEX, 0);
    if (handle == BLOCK_CIPHER_SW_HANDLE_INVALID)
    {
        // error
    }

    //Create the AES round keys.  This only needs to be done once for each AES key.
    AES_SW_RoundKeysCreate (&round_keys, (uint8_t*)AESKey128, AES_SW_KEY_SIZE_128_BIT);
    
    // Initialize the Block Cipher context with the AES module encryption/decryption functions and the AES block size
    error = BLOCK_CIPHER_SW_CTR_Initialize (handle, &context, AES_SW_Encrypt, AES_SW_Decrypt, AES_SW_BLOCK_SIZE, initialization_vector, (void *)&keyStream, sizeof (keyStream), &round_keys, CRYPTO_SW_KEY_SOFTWARE_EXPANDED);

    if (error != BLOCK_CIPHER_SW_ERROR_NONE)
    {
        // error
    }

    //Generate 4 blocks of key stream
    BLOCK_CIPHER_SW_CTR_KeyStreamGenerate(handle, 4, &round_keys, &context, BLOCK_CIPHER_SW_OPTION_STREAM_START);

    //Encrypt the data
    BLOCK_CIPHER_SW_CTR_Encrypt (handle, cipher_text, &numCipherBytes, (void *) plain_text, sizeof(plain_text), BLOCK_CIPHER_SW_OPTION_STREAM_CONTINUE);
    </code>
                                                                                                                                                               
  **************************************************************************************************************************************************************/
BLOCK_CIPHER_SW_ERRORS BLOCK_CIPHER_SW_CTR_Encrypt (BLOCK_CIPHER_SW_HANDLE handle, uint8_t * cipherText, uint32_t * numCipherBytes, uint8_t * plainText, uint32_t numPlainBytes, uint32_t options);

// *****************************************************************************
/* Function:
    BLOCK_CIPHER_SW_ERRORS BLOCK_CIPHER_SW_CTR_Decrypt (BLOCK_CIPHER_SW_HANDLE handle, 
        uint8_t * plainText, uint8_t * cipherText, uint32_t numBytes, 
        uint32_t options)

  Summary:
    Decrypts cipher text using counter mode.
  Description:
    Decrypts cipher text using counter mode.
  Conditions:
    The CTR context must be initialized with the block cipher
    encrypt/decrypt functions and the block cipher algorithm's block size.
    The block cipher module must be initialized, if necessary.
    
    The noncePlusCounter parameter in the BLOCK_CIPHER_SW_CTR_CONTEXT
    structure should be initialized. The size of this vector is the same as
    the block size of the cipher you are using.
  Input:
    handle -         A handle that is passed to the block cipher's
                     encrypt/decrypt functions to specify which instance of
                     the block cipher module to use.
    plainText -      The plain text produced by the decryption. This buffer
                     must be at least numBytes long.
    numPlainBytes -  Pointer to a uint32_t; the number of bytes decrypted
                     will be returned in this parameter.
    cipherText -     The cipher test to decrypt. Must be at least numBytes
                     long.
    numCipherBytes - The number of cipher text bytes that must be decrypted.
    options -        Block cipher decryption options that the user can specify,
                     or'd together. If BLOCK_CIPHER_SW_OPTION_STREAM_START is not
                     specified then BLOCK_CIPHER_SW_OPTION_STREAM_CONTINUE is
                     assumed. Valid options for this function are
                     * BLOCK_CIPHER_SW_OPTION_STREAM_START
                     * BLOCK_CIPHER_SW_OPTION_STREAM_CONTINUE
                     * BLOCK_CIPHER_SW_OPTION_CTR_32BIT
                     * BLOCK_CIPHER_SW_OPTION_CTR_64BIT
                     * BLOCK_CIPHER_SW_OPTION_CTR_128BIT
  Return:
    Returns a member of the BLOCK_CIPHER_SW_ERRORS enumeration:
      * BLOCK_CIPHER_SW_ERROR_NONE - no error.
      * BLOCK_CIPHER_SW_ERROR_KEY_STREAM_GEN_OUT_OF_SPACE - There was not
        enough room remaining in the context->keyStream buffer to fit the
        key data requested by the numBlocks parameter.
      * BLOCK_CIPHER_SW_ERROR_CTR_COUNTER_EXPIRED - The requesting call
        has caused the counter number to run out of unique combinations.
      * BLOCK_CIPHER_SW_ERROR_INVALID_HANDLE - The specified handle was invalid

  Example:
    <code>
    // ***************************************************************
    // Decrypt data in CTR mode with the AES algorithm.
    // ***************************************************************
    
    // System module object variable (for initializing AES)
    SYS_MODULE_OBJ sysObject;
    
    // Drive handle variable, to describe which AES module to use
    BLOCK_CIPHER_SW_HANDLE handle;
    
    // CTR mode context
    BLOCK_CIPHER_SW_CTR_CONTEXT context;

    // Initialization vector for CTR mode
    static uint8_t initialization_vector[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

    // Cipher text to decrypt
    static uint8_t cipher_text[] = { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a, \
                                    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51, \
                                    0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef, \
                                    0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10};
    // The decryption key
    static uint8_t AESKey128[] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
    // Structure to contain the created AES round keys
    AES_SW_ROUND_KEYS_128_BIT round_keys;
    // Buffer to contain decrypted ciphertext
    uint8_t plain_text[sizeof(cipher_text)];
    //keyStream could also be allocated memory instead of fixed memory
    uint8_t keyStream[AES_SW_BLOCK_SIZE*4];
    // Error type
    BLOCK_CIPHER_SW_ERRORS error;
    // Number of bytes decrypted
    uint32_t numPlainBytes;

    // Initialization call for the AES module
    sysObject = BLOCK_CIPHER_SW_Initialize (BLOCK_CIPHER_SW_INDEX, NULL);
    if (sysObject != SYS_MODULE_OBJ_STATIC)
    {
        // error
    }

    // Driver open call for the AES module
    handle = BLOCK_CIPHER_SW_Open (BLOCK_CIPHER_SW_INDEX, 0);
    if (handle == BLOCK_CIPHER_SW_HANDLE_INVALID)
    {
        // error
    }

    //Create the AES round keys.  This only needs to be done once for each AES key.
    AES_SW_RoundKeysCreate (&round_keys, (uint8_t*)AESKey128, AES_SW_KEY_SIZE_128_BIT);
    
    // Initialize the Block Cipher context with the AES module encryption/decryption functions and the AES block size
    error = BLOCK_CIPHER_SW_CTR_Initialize (handle, &context, AES_SW_Encrypt, AES_SW_Decrypt, AES_SW_BLOCK_SIZE, initialization_vector, (void *)&keyStream, sizeof (keyStream), &round_keys, CRYPTO_SW_KEY_SOFTWARE_EXPANDED);

    if (error != BLOCK_CIPHER_SW_ERROR_NONE)
    {
        // error
    }

    //Generate 4 blocks of key stream
    BLOCK_CIPHER_SW_CTR_KeyStreamGenerate(handle, 4, &round_keys, BLOCK_CIPHER_SW_OPTION_STREAM_START);

    // Decrypt the data
    BLOCK_CIPHER_SW_CTR_Decrypt (handle, plain_text, &numPlainBytes, (void *) cipher_text, sizeof(cipher_text), BLOCK_CIPHER_SW_OPTION_STREAM_CONTINUE);
    </code>
*/
BLOCK_CIPHER_SW_ERRORS BLOCK_CIPHER_SW_CTR_Decrypt (BLOCK_CIPHER_SW_HANDLE handle, uint8_t * plainText, uint32_t * numPlainBytes, uint8_t * cipherText, uint32_t numCipherBytes, uint32_t options);
#define BLOCK_CIPHER_SW_CTR_Decrypt      BLOCK_CIPHER_SW_CTR_Encrypt

/**************************************************************************************************************************************************************
  Function:
    BLOCK_CIPHER_SW_ERRORS BLOCK_CIPHER_SW_CTR_KeyStreamGenerate (
        BLOCK_CIPHER_SW_HANDLE handle, uint32_t numBlocks, uint32_t options)
    
  Summary:
    Generates a key stream for use with the counter mode.
  Description:
    Generates a key stream for use with the counter mode.
  Conditions:
    The CTR context must be initialized with the block cipher
    encrypt/decrypt functions and the block cipher algorithm's block size.
    The block cipher module must be initialized, if necessary.
    
    The noncePlusCounter parameter in the BLOCK_CIPHER_SW_CTR_CONTEXT
    structure should be initialized. The size of this vector is the same as
    the block size of the cipher you are using.
  Input:
    handle -     A handle that is passed to the block cipher's
                 encrypt/decrypt functions to specify which instance of
                 the block cipher module to use.
    numBlocks -  The number of blocks of key stream that should be created.
                 context->keyStream should have enough space remaining to
                 handle this request.
    options -    Block cipher encryption options that the user can specify,
                 or'd together. If BLOCK_CIPHER_SW_OPTION_STREAM_START is not
                 specified then BLOCK_CIPHER_SW_OPTION_STREAM_CONTINUE is
                 assumed. Valid options for this function are
                 * BLOCK_CIPHER_SW_OPTION_STREAM_START
                 * BLOCK_CIPHER_SW_OPTION_STREAM_CONTINUE
                 * BLOCK_CIPHER_SW_OPTION_CTR_32BIT
                 * BLOCK_CIPHER_SW_OPTION_CTR_64BIT
                 * BLOCK_CIPHER_SW_OPTION_CTR_128BIT

  Return:
    Returns a member of the BLOCK_CIPHER_SW_ERRORS enumeration:
      * BLOCK_CIPHER_SW_ERROR_NONE - no error.
      * BLOCK_CIPHER_SW_ERROR_KEY_STREAM_GEN_OUT_OF_SPACE - There was not
        enough room remaining in the context->keyStream buffer to fit the
        key data requested by the numBlocks parameter.
      * BLOCK_CIPHER_SW_ERROR_CTR_COUNTER_EXPIRED - The requesting call
        has caused the counter number to run out of unique combinations.
      * BLOCK_CIPHER_SW_ERROR_INVALID_HANDLE - The specified handle was invalid

  Example:
    <code>
    // ***************************************************************
    // Encrypt data in CTR mode with the AES algorithm.
    // ***************************************************************
    
    // System module object variable (for initializing AES)
    SYS_MODULE_OBJ sysObject;
    
    // Drive handle variable, to describe which AES module to use
    BLOCK_CIPHER_SW_HANDLE handle;
    
    // CTR mode context
    BLOCK_CIPHER_SW_CTR_CONTEXT context;

    // Initialization vector for CTR mode
    static uint8_t initialization_vector[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

    // Plain text to encrypt
    static uint8_t plain_text[] = { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a, \
                                    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51, \
                                    0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef, \
                                    0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10};
    // The encryption key
    static uint8_t AESKey128[] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
    // Structure to contain the created AES round keys
    AES_SW_ROUND_KEYS_128_BIT round_keys;
    // Buffer to contain encrypted plaintext
    uint8_t cipher_text[sizeof(plain_text)];
    //keyStream could also be allocated memory instead of fixed memory
    uint8_t keyStream[AES_SW_BLOCK_SIZE*4];
    // Error type
    BLOCK_CIPHER_SW_ERRORS error;
    // Number of bytes encrypted
    uint32_t numCipherBytes;
    
    // Initialization call for the AES module
    sysObject = BLOCK_CIPHER_SW_Initialize (BLOCK_CIPHER_SW_INDEX, NULL);
    if (sysObject != SYS_MODULE_OBJ_STATIC)
    {
        // error
    }

    // Driver open call for the AES module
    handle = BLOCK_CIPHER_SW_Open (BLOCK_CIPHER_SW_INDEX, 0);
    if (handle == BLOCK_CIPHER_SW_HANDLE_INVALID)
    {
        // error
    }

    //Create the AES round keys.  This only needs to be done once for each AES key.
    AES_SW_RoundKeysCreate (&round_keys, (uint8_t*)AESKey128, AES_SW_KEY_SIZE_128_BIT);
    
    // Initialize the Block Cipher context with the AES module encryption/decryption functions and the AES block size
    error = BLOCK_CIPHER_SW_CTR_Initialize (handle, &context, AES_SW_Encrypt, AES_SW_Decrypt, AES_SW_BLOCK_SIZE, initialization_vector, (void *)&keyStream, sizeof (keyStream), &round_keys, CRYPTO_SW_KEY_SOFTWARE_EXPANDED);

    if (error != BLOCK_CIPHER_SW_ERROR_NONE)
    {
        // error
    }

    //Generate 4 blocks of key stream
    BLOCK_CIPHER_SW_CTR_KeyStreamGenerate(handle, 4, BLOCK_CIPHER_SW_OPTION_STREAM_START);

    //Encrypt the data
    BLOCK_CIPHER_SW_CTR_Encrypt (handle, cipher_text, &numCipherBytes, (void *) plain_text, sizeof(plain_text), BLOCK_CIPHER_SW_OPTION_STREAM_CONTINUE);
    </code>
                                                                                                                                                               
  **************************************************************************************************************************************************************/
BLOCK_CIPHER_SW_ERRORS BLOCK_CIPHER_SW_CTR_KeyStreamGenerate (BLOCK_CIPHER_SW_HANDLE handle, uint32_t numBlocks, uint32_t options);

#endif      // _BLOCK_CIPHER_SW_CTR_H
