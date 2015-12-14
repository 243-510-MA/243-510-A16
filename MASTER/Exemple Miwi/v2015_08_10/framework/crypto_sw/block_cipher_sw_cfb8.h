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

#if !defined (_BLOCK_CIPHER_SW_CFB8_H)
#define _BLOCK_CIPHER_SW_CFB8_H

#include <stdint.h>
#include "system_config.h"

// Context structure for a cipher feedback operation
typedef struct
{
    uint8_t __attribute__((aligned)) initializationVector[CRYPTO_CONFIG_SW_BLOCK_MAX_SIZE];      // Initialization vector for the CFB operation
    uint32_t blockSize;                                                         // Block size of the cipher algorithm being used with the block cipher mode module
    BLOCK_CIPHER_SW_FunctionEncrypt encrypt;                                       // Encrypt function for the algorithm being used with the block cipher mode module
    BLOCK_CIPHER_SW_FunctionDecrypt decrypt;                                       // Decrypt function for the algorithm being used with the block cipher mode module
    void * key;                                                                 // Key location
    CRYPTO_SW_KEY_TYPE keyType;                                                    // Format of the key
    uint8_t bytesRemaining;                                                     // Number of bytes remaining in the remainingData buffer
} BLOCK_CIPHER_SW_CFB8_CONTEXT;

// *****************************************************************************
/* Function:
    BLOCK_CIPHER_SW_ERRORS BLOCK_CIPHER_SW_CFB8_Initialize (BLOCK_CIPHER_SW_HANDLE handle,
        BLOCK_CIPHER_SW_CFB8_CONTEXT * context,
        BLOCK_CIPHER_SW_FunctionEncrypt encryptFunction, 
        BLOCK_CIPHER_SW_FunctionDecrypt decryptFunction, uint32_t blockSize,
        uint8_t * initialization_vector, void * key, CRYPTO_SW_KEY_TYPE keyType)

  Summary:
    Initializes a CFB context for encryption/decryption.
  
  Description:
    Initializes a CFB context for encryption/decryption.  The user will specify 
    details about the algorithm being used in CFB mode.
  
  Precondition:
    Any required initialization needed by the block cipher algorithm must
    have been performed.

  Parameters:
    handle - A handle that is passed to the block cipher's encrypt/decrypt
        functions to specify which instance of the block cipher module to use.
    context - Pointer to a context structure for this stream.
    encryptFunction - Pointer to the encryption function for the block cipher
        algorithm being used in CFB mode.
    decryptFunction - Pointer to the decryption function for the block cipher
        algorithm being used in CFB mode.
    blockSize - The block size of the block cipher algorithm being used in CFB mode.
    initializationVector - The initialization vector for this operation.  The length
        of this vector must be equal to the block size of your block cipher.
    key - The cryptographic key location
    keyType - The storage type of the key

  Returns:
      * BLOCK_CIPHER_SW_ERROR_NONE - no error.
      * BLOCK_CIPHER_SW_ERROR_UNSUPPORTED_KEY_TYPE - The specified key type is not
        supported by the firmware implementation being used
      * BLOCK_CIPHER_SW_ERROR_INVALID_HANDLE - The specified handle was invalid
    
  Example:
    <code>
    SYS_MODULE_OBJ sysObject;
    BLOCK_CIPHER_SW_HANDLE drvHandle;

    // Initialize the AES module.  There is no initialization required for the pure software AES module; this function
    // is used in this instance to create code that will be more easily portable to a hardware AES implementation.
    // For the software implementation, BLOCK_CIPHER_SW_INDEX is defined as '0' since there aren't multiple instances of it, as
    // there might be in hardware.  Hardware AES modules may have multiple encryption/decryption engines; in this case,
    // the BLOCK_CIPHER_SW_Initialize function would be used to indicate which one to use, and different macros would be defined
    // by the AES module drivers (e.g. BLOCK_CIPHER_SW_INDEX_0 and BLOCK_CIPHER_SW_INDEX_1 if there were two AES hardware modules).
    sysObject = BLOCK_CIPHER_SW_Initialize(BLOCK_CIPHER_SW_INDEX, NULL);
    if (sysObject != SYS_MODULE_OBJ_STATIC)
    {
        return false;
    }

    drvHandle = BLOCK_CIPHER_SW_Open(BLOCK_CIPHER_SW_INDEX, DRV_IO_INTENT_EXCLUSIVE);
    if (drvHandle == BLOCK_CIPHER_SW_HANDLE_INVALID)
    {
        return false;
    }
 
    //This example is from the KAT CFB8MMT128.rsp file, encrypt count 1
    //  KEY = 0d8f3dc3edee60db658bb97faf46fba3
    //  IV = e481fdc42e606b96a383c0a1a5520ebb
    //  PLAINTEXT = aacd
    //  CIPHERTEXT = 5066

    static uint8_t AESKey128[]      =       { 0x0d, 0x8f, 0x3d, 0xc3,
                                                    0xed, 0xee, 0x60, 0xdb,
                                                    0x65, 0x8b, 0xb9, 0x7f,
                                                    0xaf, 0x46, 0xfb, 0xa3
                                                  };

    static uint8_t initialization_vector[] =
                                                  { 0xe4, 0x81, 0xfd, 0xc4,
                                                    0x2e, 0x60, 0x6b, 0x96,
                                                    0xa3, 0x83, 0xc0, 0xa1,
                                                    0xa5, 0x52, 0x0e, 0xbb
                                                  };

    static uint8_t plain_text[]     =       { 0xaa, 0xcd
                                                  };

    AES_SW_ROUND_KEYS_128_BIT round_keys;
    BLOCK_CIPHER_SW_CFB8_CONTEXT context;
    uint32_t numCipherBytes;

    //We need a buffer to contain the resulting data.
    //  This buffer can be created statically or dynamically and can be
    //  of any size as long as it is larger than or equal to AES_SW_BLOCK_SIZE
    uint8_t cipher_text[sizeof(plain_text)];

    //Create the round keys.  This only needs to be done once for each key.
    //  This example is here for completeness.
    AES_SW_RoundKeysCreate (    &round_keys,
                            (uint8_t*)AESKey128,
                            AES_SW_KEY_SIZE_128_BIT
                       );

    // Initialize the Block Cipher context
    BLOCK_CIPHER_SW_CFB8_Initialize (handle, &context, AES_SW_Encrypt, AES_SW_Decrypt, AES_SW_BLOCK_SIZE, initialization_vector, &round_keys, CRYPTO_SW_KEY_SOFTWARE_EXPANDED);

    //Encrypt the data
    BLOCK_CIPHER_SW_CFB8_Encrypt (handle, cipher_text, &numCipherBytes, (void *) plain_text, sizeof(plain_text), BLOCK_CIPHER_SW_OPTION_STREAM_START);
    </code>
*/
BLOCK_CIPHER_SW_ERRORS BLOCK_CIPHER_SW_CFB8_Initialize (BLOCK_CIPHER_SW_HANDLE handle, BLOCK_CIPHER_SW_CFB8_CONTEXT * context, BLOCK_CIPHER_SW_FunctionEncrypt encryptFunction, BLOCK_CIPHER_SW_FunctionDecrypt decryptFunction, uint32_t blockSize, uint8_t * initializationVector, void * key, CRYPTO_SW_KEY_TYPE keyType);

/*********************************************************************************************************************************************************************************************************************
  Function:
    BLOCK_CIPHER_SW_ERRORS BLOCK_CIPHER_SW_CFB8_Encrypt (BLOCK_CIPHER_SW_HANDLE handle,
        uint8_t * cipherText, uint32_t * numCipherBytes, uint8_t * plainText,
        uint32_t numPlainBytes, uint32_t options);
    
  Summary:
    Encrypts plain text using cipher feedback mode.
  Description:
    Encrypts plain text using cipher feedback mode.
  Conditions:
    The CFB context must be initialized with the block cipher
    encrypt/decrypt functions and the block cipher algorithm's block size.
    The block cipher module must be initialized, if necessary.
  Input:
    handle -          A handle that is passed to the block cipher's
                      encrypt/decrypt functions to specify which instance of
                      the block cipher module to use. This parameter can be
                      specified as NULL if the block cipher does not have
                      multiple instances.
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
                      bytes will be cached in the CFB context structure
                      until additional data is provided.
    options -         Block cipher encryption options that the user can
                      specify, or'd together. Valid options for this
                      function are
                      * BLOCK_CIPHER_SW_OPTION_STREAM_START
                      * BLOCK_CIPHER_SW_OPTION_STREAM_CONTINUE
                      * BLOCK_CIPHER_SW_OPTION_STREAM_COMPLETE
                      * BLOCK_CIPHER_SW_OPTION_CIPHER_TEXT_POINTER_ALIGNED
                      * BLOCK_CIPHER_SW_OPTION_PLAIN_TEXT_POINTER_ALIGNED

  Return:
    Returns a member of the BLOCK_CIPHER_SW_ERRORS enumeration:
      * BLOCK_CIPHER_SW_ERROR_NONE - no error.
      * BLOCK_CIPHER_SW_ERROR_INVALID_HANDLE - The specified handle was invalid
    
  Example:
    <code>
    SYS_MODULE_OBJ sysObject;
    BLOCK_CIPHER_SW_HANDLE drvHandle;

    // Initialize the AES module.  
    sysObject = BLOCK_CIPHER_SW_Initialize(BLOCK_CIPHER_SW_INDEX, NULL);
    if (sysObject != SYS_MODULE_OBJ_STATIC)
    {
        return false;
    }

    drvHandle = BLOCK_CIPHER_SW_Open(BLOCK_CIPHER_SW_INDEX, DRV_IO_INTENT_EXCLUSIVE);
    if (drvHandle == BLOCK_CIPHER_SW_HANDLE_INVALID)
    {
        return false;
    }

    //This example is from the KAT CFB8MMT128.rsp file, encrypt count 1
    //  KEY = 0d8f3dc3edee60db658bb97faf46fba3
    //  IV = e481fdc42e606b96a383c0a1a5520ebb
    //  PLAINTEXT = aacd
    //  CIPHERTEXT = 5066

    static uint8_t AESKey128[]      =       { 0x0d, 0x8f, 0x3d, 0xc3,
                                                    0xed, 0xee, 0x60, 0xdb,
                                                    0x65, 0x8b, 0xb9, 0x7f,
                                                    0xaf, 0x46, 0xfb, 0xa3
                                                  };

    static uint8_t initialization_vector[] =
                                                  { 0xe4, 0x81, 0xfd, 0xc4,
                                                    0x2e, 0x60, 0x6b, 0x96,
                                                    0xa3, 0x83, 0xc0, 0xa1,
                                                    0xa5, 0x52, 0x0e, 0xbb
                                                  };

    static uint8_t plain_text[]     =       { 0xaa, 0xcd
                                                  };

    AES_SW_ROUND_KEYS_128_BIT round_keys;
    BLOCK_CIPHER_SW_CFB8_CONTEXT context;
    uint32_t numCipherBytes;

    //We need a buffer to contain the resulting data.
    //  This buffer can be created statically or dynamically and can be
    //  of any size as long as it is larger than or equal to AES_SW_BLOCK_SIZE
    uint8_t cipher_text[sizeof(plain_text)];

    //Create the round keys.  This only needs to be done once for each key.
    //  This example is here for completeness.
    AES_SW_RoundKeysCreate (    &round_keys,
                            (uint8_t*)AESKey128,
                            AES_SW_KEY_SIZE_128_BIT
                       );

    // Initialize the Block Cipher context
    BLOCK_CIPHER_SW_CFB8_Initialize (handle, &context, AES_SW_Encrypt, AES_SW_Decrypt, AES_SW_BLOCK_SIZE, initialization_vector, &round_keys, CRYPTO_SW_KEY_SOFTWARE_EXPANDED);

    //Encrypt the data
    BLOCK_CIPHER_SW_CFB8_Encrypt (handle, cipher_text, &numCipherBytes, (void *) plain_text, sizeof(plain_text), BLOCK_CIPHER_SW_OPTION_STREAM_START);
    </code>
                                                                                                                                                                                                                      
  *********************************************************************************************************************************************************************************************************************/
BLOCK_CIPHER_SW_ERRORS BLOCK_CIPHER_SW_CFB8_Encrypt (BLOCK_CIPHER_SW_HANDLE handle, uint8_t * cipherText, uint32_t * numCipherBytes, uint8_t * plainText, uint32_t numPlainBytes, uint32_t options);

// *****************************************************************************
/* Function:
    BLOCK_CIPHER_SW_ERRORS BLOCK_CIPHER_SW_CFB8_Decrypt (BLOCK_CIPHER_SW_HANDLE handle,
        uint8_t * plainText, uint32_t * numPlainBytes, uint8_t * cipherText,
        uint32_t numCipherBytes, uint32_t options)

    Summary:
    Decrypts cipher text using cipher-block chaining mode.
  Description:
    Decrypts cipher text using cipher-block chaining mode.
  Conditions:
    The CFB context must be initialized with the block cipher
    encrypt/decrypt functions and the block cipher algorithm's block size.
    The block cipher module must be initialized, if necessary.
  Input:
    handle -          A handle that is passed to the block cipher's
                      encrypt/decrypt functions to specify which instance of
                      the block cipher module to use. This parameter can be
                      specified as NULL if the block cipher does not have
                      multiple instances.
    plainText -       The plain test produced by the decryption. This buffer
                      must be a multiple of the block cipher's block size,
                      even if the cipher text passed in is not.
    numPlainBytes -   Pointer to a uint32_t; the number of bytes decrypted
                      will be returned in this parameter.
    cipherText -      The cipher text that will be decrypted. This buffer
                      must be a multiple of the block size, unless this is
                      the end of the stream (the
                      BLOCK_CIPHER_SW_OPTION_STREAM_COMPLETE option must be set
                      in this case).
    numCipherBytes -  The number of cipher text bytes to decrypt.
    options -         Block cipher encryption options that the user can
                      specify, or'd together. Valid options for this
                      function are
                      * BLOCK_CIPHER_SW_OPTION_STREAM_START
                      * BLOCK_CIPHER_SW_OPTION_STREAM_COMPLETE
                      * BLOCK_CIPHER_SW_OPTION_CIPHER_TEXT_POINTER_ALIGNED
                      * BLOCK_CIPHER_SW_OPTION_PLAIN_TEXT_POINTER_ALIGNED

  Return:
    Returns a member of the BLOCK_CIPHER_SW_ERRORS enumeration:
      * BLOCK_CIPHER_SW_ERROR_NONE - no error.
      * BLOCK_CIPHER_SW_ERROR_INVALID_HANDLE - The specified handle was invalid
    
  Example:
    <code>
    SYS_MODULE_OBJ sysObject;
    BLOCK_CIPHER_SW_HANDLE drvHandle;

    // Initialize the AES module.  
    sysObject = BLOCK_CIPHER_SW_Initialize(BLOCK_CIPHER_SW_INDEX, NULL);
    if (sysObject != SYS_MODULE_OBJ_STATIC)
    {
        return false;
    }

    drvHandle = BLOCK_CIPHER_SW_Open(BLOCK_CIPHER_SW_INDEX, DRV_IO_INTENT_EXCLUSIVE);
    if (drvHandle == BLOCK_CIPHER_SW_HANDLE_INVALID)
    {
        return false;
    }

    //This example is from the KAT CFB8MMT128.rsp file, decrypt count 1
    //  KEY = 38cf776750162edc63c3b5dbe311ab9f
    //  IV = 98fbbd288872c40f1926b16ecaec1561
    //  CIPHERTEXT = 4878
    //  PLAINTEXT = eb24
    static uint8_t AESKey128[]      =       { 0x38, 0xcf, 0x77, 0x67,
                                                    0x50, 0x16, 0x2e, 0xdc,
                                                    0x63, 0xc3, 0xb5, 0xdb,
                                                    0xe3, 0x11, 0xab, 0x9f
                                                  };

    static uint8_t initialization_vector[] =
                                                  { 0x98, 0xfb, 0xbd, 0x28,
                                                    0x88, 0x72, 0xc4, 0x0f,
                                                    0x19, 0x26, 0xb1, 0x6e,
                                                    0xca, 0xec, 0x15, 0x61
                                                  };

    static uint8_t cipher_text[]    =       { 0x48, 0x78
                                                  };

    AES_SW_ROUND_KEYS_128_BIT round_keys;
    BLOCK_CIPHER_SW_CFB8_CONTEXT context;
    uint32_t numPlainBytes;

    //We need a buffer to contain the resulting data.
    //  This buffer can be created statically or dynamically and can be
    //  of any size as long as it is larger than or equal to AES_SW_BLOCK_SIZE
    uint8_t plain_text[sizeof(cipher_text)];

    //Create the round keys.  This only needs to be done once for each key.
    //  This example is here for completeness.
    AES_SW_RoundKeysCreate (    &round_keys,
                            (uint8_t*)AESKey128,
                            AES_SW_KEY_SIZE_128_BIT
                       );

    // Initialize the Block Cipher context
    BLOCK_CIPHER_SW_CFB8_Initialize (handle, &context, AES_SW_Encrypt, AES_SW_Decrypt, AES_SW_BLOCK_SIZE, initialization_vector, &round_keys, CRYPTO_SW_KEY_SOFTWARE_EXPANDED);

    //Decrypt the data
    BLOCK_CIPHER_SW_CFB8_Decrypt (handle, plain_text, &numPlainBytes, cipher_text, sizeof(cipher_text), BLOCK_CIPHER_SW_OPTION_STREAM_START);
    </code>
*/
BLOCK_CIPHER_SW_ERRORS BLOCK_CIPHER_SW_CFB8_Decrypt (BLOCK_CIPHER_SW_HANDLE handle, uint8_t * plainText, uint32_t * numPlainBytes, uint8_t * cipherText, uint32_t numCipherBytes, uint32_t options);

#endif      // _BLOCK_CIPHER_SW_CFB8_H

