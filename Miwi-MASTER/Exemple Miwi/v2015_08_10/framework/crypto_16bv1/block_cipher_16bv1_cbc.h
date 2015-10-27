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

#if !defined (_BLOCK_CIPHER_16BV1_CBC_H)
#define _BLOCK_CIPHER_16BV1_CBC_H
 
#include <stdint.h>
#include "system_config.h"

// Context structure for a cipher-block chaining operation
typedef struct
{
    uint8_t __attribute__((aligned)) remainingData[CRYPTO_CONFIG_16BV1_BLOCK_MAX_SIZE];             // Buffer to store data until more is avaliable if there is not enough to encrypt an entire block.
    uint32_t blockSize;                                                         // Block size of the cipher algorithm being used with the block cipher mode module
    BLOCK_CIPHER_FunctionEncrypt encrypt;                                       // Encrypt function for the algorithm being used with the block cipher mode module
    BLOCK_CIPHER_FunctionDecrypt decrypt;                                       // Decrypt function for the algorithm being used with the block cipher mode module
    uint8_t bytesRemaining;                                                     // Number of bytes remaining in the remainingData buffer
    uint8_t state;                                                              // The CBC mode specific state for the thread
} BLOCK_CIPHER_16BV1_CBC_CONTEXT;

// *****************************************************************************
/* Function:
    BLOCK_CIPHER_ERRORS BLOCK_CIPHER_16BV1_CBC_Initialize (BLOCK_CIPHER_HANDLE handle, 
	    BLOCK_CIPHER_16BV1_CBC_CONTEXT * context, 
		BLOCK_CIPHER_FunctionEncrypt encryptFunction, 
		BLOCK_CIPHER_FunctionDecrypt decryptFunction, uint32_t blockSize, 
		uint8_t * initializationVector, void * key, CRYPTO_KEY_TYPE keyType, 
		CRYPTO_KEY_MODE keyMode);

  Summary:
    Initializes a CBC context for encryption/decryption.
  
  Description:
    Initializes a CBC context for encryption/decryption.  The user will specify 
    details about the algorithm being used in CBC mode.
  
  Precondition:
    Any required initialization needed by the block cipher algorithm must
    have been performed.

  Parameters:
    handle - A handle that is passed to the block cipher's encrypt/decrypt
        functions to specify which instance of the block cipher module to use.
    context - Pointer to a context structure for this stream.
    encryptFunction - Selects the algorithm that will be used
    decryptFunction - only for SW HW crypto API compatibility
    blockSize - The block size of the block cipher algorithm being used in CBC mode.
    initializationVector - The initialization vector for this operation.  The length
        of this vector must be equal to the block size of your block cipher.
    key -  Pointer to the cryptographic key location
    keyType - The storage type of the key
    keyMode - The length and algorithm of the key
    // Error type
    BLOCK_CIPHER_ERRORS error;

  Returns:
    Returns a member of the BLOCK_CIPHER_ERRORS enumeration:
      * BLOCK_CIPHER_ERROR_NONE - no error.
      * BLOCK_CIPHER_ERROR_UNSUPPORTED_KEY_TYPE - The specified key type is not
        supported by the firmware implementation being used
      * BLOCK_CIPHER_ERROR_INVALID_HANDLE - The specified handle was invalid
    
  Example:
    <code>
    // Initialize the CBC block cipher module for use with AES.
    SYS_MODULE_OBJ sysObject;
    BLOCK_CIPHER_HANDLE handle;
    BLOCK_CIPHER_16BV1_CBC_CONTEXT context;
    // Initialization vector for CBC mode
    static uint8_t initialization_vector[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    // The encryption key
    static uint8_t AESKey128[] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
    BLOCK_CIPHER_ERRORS error;
    
    sysObject = BLOCK_CIPHER_16BV1_Initialize (BLOCK_CIPHER_INDEX, NULL);
    if (sysObject != SYS_MODULE_OBJ_STATIC)
    {
        // error
    }

    handle = BLOCK_CIPHER_16BV1_Open (BLOCK_CIPHER_INDEX, 0);
    if (handle == BLOCK_CIPHER_HANDLE_INVALID)
    {
        // error
    }

    // Initialize the block cipher module
    error = BLOCK_CIPHER_16BV1_CBC_Initialize (handle, &context, CRYPTO_16BV1_ALGORITHM_AES, CRYPTO_16BV1_ALGORITHM_AES, AES_BLOCK_SIZE, initialization_vector, &AESKey128, CRYPTO_KEY_SOFTWARE, CRYPTO_AES_128_KEY);

    if (error != BLOCK_CIPHER_ERROR_NONE)
    {
        // error
    }
    </code>
*/
BLOCK_CIPHER_ERRORS BLOCK_CIPHER_16BV1_CBC_Initialize (BLOCK_CIPHER_HANDLE handle, BLOCK_CIPHER_16BV1_CBC_CONTEXT * context, BLOCK_CIPHER_FunctionEncrypt encryptFunction, BLOCK_CIPHER_FunctionDecrypt decryptFunction, uint32_t blockSize, uint8_t * initializationVector, void * key, CRYPTO_KEY_TYPE keyType, CRYPTO_KEY_MODE keyMode);

/**************************************************************************************************************************************************************************************
  Function:
    BLOCK_CIPHER_ERRORS BLOCK_CIPHER_16BV1_CBC_Encrypt (BLOCK_CIPHER_HANDLE handle,
    uint8_t * cipherText, uint32_t * numCipherBytes, uint8_t * plainText,
    uint32_t numPlainBytes, uint32_t options);
    
  Summary:
    Encrypts plain text using cipher-block chaining mode.
  Description:
    Encrypts plain text using cipher-block chaining mode.
  Conditions:
    The CBC context must be initialized with the block cipher
    encrypt/decrypt functions and the block cipher algorithm's block size.
    The block cipher module must be initialized, if necessary.
  Input:
    handle -          A handle that is passed to the block cipher's
                      encrypt/decrypt functions to specify which instance of
                      the block cipher module to use. 
    cipherText -      The cipher text produced by the encryption. This
                      buffer must be a multiple of the block size, even if
                      the plain text buffer size is not. This buffer should
                      always be larger than the plain text buffer.
    numCipherBytes -  Pointer to a uint32_t; the number of bytes encrypted
                      will be returned in this parameter.
    plainText -       The plain test to encrypt.
    numPlainBytes -   The number of plain text bytes that must be encrypted.
                      If the number of plain text bytes encrypted is not
                      evenly divisible by the block size, the remaining
                      bytes will be cached in the CBC context structure
                      until additional data is provided.
    options -         Block cipher encryption options that the user can
                      specify, or'd together. Valid options for this
                      function are
                      * BLOCK_CIPHER_OPTION_PAD_NONE
                      * BLOCK_CIPHER_OPTION_PAD_NULLS
                      * BLOCK_CIPHER_OPTION_PAD_8000
                      * BLOCK_CIPHER_OPTION_PAD_NUMBER
                      * BLOCK_CIPHER_OPTION_STREAM_START
                      * BLOCK_CIPHER_OPTION_STREAM_CONTINUE
                      * BLOCK_CIPHER_OPTION_STREAM_COMPLETE
                      * BLOCK_CIPHER_OPTION_CIPHER_TEXT_POINTER_ALIGNED
                      * BLOCK_CIPHER_OPTION_PLAIN_TEXT_POINTER_ALIGNED
  Return:
    Returns a member of the BLOCK_CIPHER_ERRORS enumeration:
      * BLOCK_CIPHER_ERROR_NONE - no error.
      * BLOCK_CIPHER_ERROR_INVALID_HANDLE - The specified handle was invalid

  Example:
    <code>
    // ***************************************************************
    // Encrypt data in CBC mode with the AES algorithm.
    // ***************************************************************
    
    // System module object variable (for initializing AES)
    SYS_MODULE_OBJ sysObject;
    
    // Block cipher handle variable, to describe which AES stream to use
    BLOCK_CIPHER_HANDLE handle;
    
    // CBC mode context
    BLOCK_CIPHER_16BV1_CBC_CONTEXT context;
    
    // Initialization vector for CBC mode
    static uint8_t initialization_vector[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    
    // Plain text to encrypt
    static uint8_t plain_text[] = { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a, \
                                    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51, \
                                    0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef, \
                                    0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10};
    // The encryption key
    static uint8_t AESKey128[] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
    
    // Buffer to contain encrypted plaintext
    uint8_t cipher_text[sizeof(plain_text)];
    // The number of bytes that were encrypted
    uint32_t num_bytes_encrypted;
    // Error type
    BLOCK_CIPHER_ERRORS error;
    
    // Initialization call for the AES module
    sysObject = BLOCK_CIPHER_16BV1_Initialize (BLOCK_CIPHER_INDEX, NULL);
    if (sysObject != SYS_MODULE_OBJ_STATIC)
    {
        // error
    }

    // Driver open call for the AES module
    handle = BLOCK_CIPHER_16BV1_Open (BLOCK_CIPHER_INDEX, 0);
    if (handle == BLOCK_CIPHER_HANDLE_INVALID)
    {
        // error
    }
    
    // Initialize the Block Cipher context with the AES module encryption/decryption functions and the AES block size
    error = BLOCK_CIPHER_16BV1_CBC_Initialize (handle, &context, CRYPTO_16BV1_ALGORITHM_AES, CRYPTO_16BV1_ALGORITHM_AES, AES_BLOCK_SIZE, initialization_vector, &AESKey128, CRYPTO_KEY_SOFTWARE, CRYPTO_AES_128_KEY);

    if (error != BLOCK_CIPHER_ERROR_NONE)
    {
        // error
    }

    //Encrypt the data
	BLOCK_CIPHER_16BV1_CBC_Encrypt (handle, cipher_text, &num_bytes_encrypted, (void *) plain_text, sizeof(plain_text), BLOCK_CIPHER_OPTION_STREAM_START|BLOCK_CIPHER_OPTION_STREAM_COMPLETE|BLOCK_CIPHER_OPTION_PAD_NONE);

    while (((state = BLOCK_CIPHER_16BV1_GetState(handle)) != BLOCK_CIPHER_STATE_IDLE) && (state != BLOCK_CIPHER_STATE_ERROR))
    {
        BLOCK_CIPHER_16BV1_Tasks();
    }

    if (state == BLOCK_CIPHER_STATE_ERROR)
    {
        while(1);
    }
    </code>
                                                                                                                                                                                       
  **************************************************************************************************************************************************************************************/
BLOCK_CIPHER_ERRORS BLOCK_CIPHER_16BV1_CBC_Encrypt (BLOCK_CIPHER_HANDLE handle, uint8_t * cipherText, uint32_t * numCipherBytes, uint8_t * plainText, uint32_t numPlainBytes, uint32_t options);

// *****************************************************************************
/* Function:
    BLOCK_CIPHER_ERRORS BLOCK_CIPHER_16BV1_CBC_Decrypt (BLOCK_CIPHER_HANDLE handle,
        uint8_t * plainText, uint32_t * numPlainBytes, uint8_t * cipherText,
        uint32_t numCipherBytes, uint32_t options)

    Summary:
    Decrypts cipher text using cipher-block chaining mode.
  Description:
    Decrypts cipher text using cipher-block chaining mode.
  Conditions:
    The CBC context must be initialized with the block cipher
    encrypt/decrypt functions and the block cipher algorithm's block size.
    The block cipher module must be initialized, if necessary.
  Input:
    handle -          A handle that is passed to the block cipher's
                      encrypt/decrypt functions to specify which instance of
                      the block cipher module to use. 
    plainText -       The plain test produced by the decryption. This buffer
                      must be a multiple of the block cipher's block size,
                      even if the cipher text passed in is not.
    numPlainBytes -   Pointer to a uint32_t; the number of bytes decrypted
                      will be returned in this parameter.
    cipherText -      The cipher text that will be decrypted. This buffer
                      must be a multiple of the block size, unless this is
                      the end of the stream (the
                      BLOCK_CIPHER_OPTION_STREAM_COMPLETE option must be set
                      in this case).
    numCipherBytes -  The number of cipher text bytes to decrypt.
    options -         Block cipher encryption options that the user can
                      specify, or'd together. Valid options for this
                      function are
                      * BLOCK_CIPHER_OPTION_STREAM_START
                      * BLOCK_CIPHER_OPTION_STREAM_COMPLETE
                      * BLOCK_CIPHER_OPTION_CIPHER_TEXT_POINTER_ALIGNED
                      * BLOCK_CIPHER_OPTION_PLAIN_TEXT_POINTER_ALIGNED
  Return:
    Returns a member of the BLOCK_CIPHER_ERRORS enumeration:
      * BLOCK_CIPHER_ERROR_NONE - no error.
      * BLOCK_CIPHER_ERROR_INVALID_HANDLE - The specified handle was invalid
    
  Example:
    <code>
    // ***************************************************************
    // Decrypt data in CBC mode with the AES algorithm.
    // ***************************************************************
    
    // System module object variable (for initializing AES)
    SYS_MODULE_OBJ sysObject;
    
    // Drive handle variable, to describe which AES module to use
    BLOCK_CIPHER_HANDLE handle;
    
    // CBC mode context
    BLOCK_CIPHER_16BV1_CBC_CONTEXT context;

    // Initialization vector for CBC mode
    static uint8_t initialization_vector[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

    // Cipher text to decrypt
    static uint8_t cipher_text[] = { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a, \
                                     0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51, \
                                     0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef, \
                                     0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10};
    // The encryption key
    static uint8_t AESKey128[] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
    // Buffer to contain encrypted plaintext
    uint8_t plain_text[sizeof(cipher_text)];
    // The number of bytes that were decrypted
    uint32_t num_bytes_decrypted;
    // Error type
    BLOCK_CIPHER_ERRORS error;
    
    // Initialization call for the AES module
    sysObject = BLOCK_CIPHER_16BV1_Initialize (BLOCK_CIPHER_INDEX, NULL);
    if (sysObject != SYS_MODULE_OBJ_STATIC)
    {
        // error
    }

    // Driver open call for the AES module
    handle = BLOCK_CIPHER_16BV1_Open (BLOCK_CIPHER_INDEX, 0);
    if (handle == BLOCK_CIPHER_HANDLE_INVALID)
    {
        // error
    }

    // Initialize the Block Cipher context with the AES module encryption/decryption functions and the AES block size
    error = BLOCK_CIPHER_16BV1_CBC_Initialize (handle, &context, CRYPTO_16BV1_ALGORITHM_AES, CRYPTO_16BV1_ALGORITHM_AES, AES_BLOCK_SIZE, initialization_vector, &AESKey128, CRYPTO_KEY_SOFTWARE, CRYPTO_AES_128_KEY);

    if (error != BLOCK_CIPHER_ERROR_NONE)
    {
        // error
    }

    // Decrypt the data
	BLOCK_CIPHER_16BV1_CBC_Decrypt (handle, plain_text, &num_bytes_decrypted, cipher_text, sizeof(cipher_text), BLOCK_CIPHER_OPTION_STREAM_START|BLOCK_CIPHER_OPTION_STREAM_COMPLETE);
 
    while (((state = BLOCK_CIPHER_16BV1_GetState(handle)) != BLOCK_CIPHER_STATE_IDLE) && (state != BLOCK_CIPHER_STATE_ERROR))
    {
        BLOCK_CIPHER_16BV1_Tasks();
    }

    if (state == BLOCK_CIPHER_STATE_ERROR)
    {
        while(1);
    }
    </code>
*/
BLOCK_CIPHER_ERRORS BLOCK_CIPHER_16BV1_CBC_Decrypt (BLOCK_CIPHER_HANDLE handle, uint8_t * plainText, uint32_t * numPlainBytes, uint8_t * cipherText, uint32_t numCipherBytes, uint32_t options);

#endif      // _BLOCK_CIPHER_16BV1_CBC_H
