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

#if !defined (_BLOCK_CIPHER_16BV1_GCM_H)
#define _BLOCK_CIPHER_16BV1_GCM_H

#include <stdint.h>
#include <stdbool.h>
#include "system_config.h"

// Context structure for the Galois counter operation
typedef struct
{
    uint8_t __attribute__((aligned)) initializationVector[CRYPTO_CONFIG_16BV1_BLOCK_MAX_SIZE];  // Buffer containing the initialization vector and initial counter.
    uint8_t __attribute__((aligned)) counter[CRYPTO_CONFIG_16BV1_BLOCK_MAX_SIZE];               // Buffer containing the current counter value.
    uint8_t __attribute__((aligned)) hashSubKey[CRYPTO_CONFIG_16BV1_BLOCK_MAX_SIZE];            // Buffer containing the calculated hash subkey
    uint8_t __attribute__((aligned)) authTag[CRYPTO_CONFIG_16BV1_BLOCK_MAX_SIZE];               // Buffer containing the current authentication tag
    uint8_t __attribute__((aligned)) authBuffer[CRYPTO_CONFIG_16BV1_BLOCK_MAX_SIZE];            // Buffer containing data that has been encrypted but has not been authenticated
    BLOCK_CIPHER_FunctionEncrypt encrypt;                                   // Encrypt function for the algorithm being used with the block cipher mode module
    BLOCK_CIPHER_FunctionDecrypt decrypt;                                   // Decrypt function for the algorithm being used with the block cipher mode module
    void * keyStream;                                                       // Pointer to the key stream.  Must be a multiple of the cipher's block size, but smaller than 2^25 bytes.
    void * keyStreamCurrentPosition;                                        // Pointer to the current position in the key stream.
    uint32_t keyStreamSize;                                                 // Size of the key stream.
    uint32_t bytesRemainingInKeyStream;                                     // Number of bytes remaining in the key stream
    uint32_t blockSize;                                                     // Block size of the cipher algorithm being used with the block cipher mode module
    uint32_t cipherTextLen;                                                 // Current number of ciphertext bytes computed
    uint32_t authDataLen;                                                   // Current number of non-ciphertext bytes authenticated
    void * key;                                                             // Key location
    CRYPTO_KEY_TYPE keyType;                                                // Format of the key
    uint8_t authBufferLen;                                                  // Number of bytes in the auth Buffer
    uint8_t * finalTagPointer;
    uint32_t finalTagLen;
    struct
    {
        uint8_t authCompleted : 1;                                          // Determines if authentication of non-encrypted data has been completed for this device.
        uint8_t filler : 7;
    } flags;
    uint8_t state;                                                          // The GCM mode specific state for the thread
} BLOCK_CIPHER_16BV1_GCM_CONTEXT;


// *****************************************************************************
/* Function:
    BLOCK_CIPHER_ERRORS BLOCK_CIPHER_16BV1_GCM_Initialize (BLOCK_CIPHER_HANDLE handle,
        BLOCK_CIPHER_16BV1_GCM_CONTEXT * context,
        BLOCK_CIPHER_FunctionEncrypt encryptFunction,
        BLOCK_CIPHER_FunctionDecrypt decryptFunction, uint32_t blockSize,
        uint8_t * initializationVector, void * keyStream, uint32_t keyStreamSize,
        void * key, CRYPTO_KEY_TYPE keyType, CRYPTO_KEY_MODE keyMode)

  Summary:
    Initializes a GCM context for encryption/decryption.
  
  Description:
    Initializes a GCM context for encryption/decryption.  The user will specify
    details about the algorithm being used in GCM mode.
  
  Precondition:
    Any required initialization needed by the block cipher algorithm must
    have been performed.

  Parameters:
    handle - A handle that is passed to the block cipher's encrypt/decrypt
        functions to specify which instance of the block cipher module to use.
    context - The GCM context to initialize.
    encryptFunction - Selects the algorithm that will be used
    decryptFunction - only for SW HW crypto API compatibility
    blockSize - The block size of the block cipher algorithm being used in GCM mode.
    initializationVector - A security nonce.  See the GCM specification, section 8.2
        for information about constructing initialization vectors.
    initializationVectorLen - Length of the initialization vector, in bytes
    keyStream - Pointer to a buffer to contain a calculated keyStream.
    keyStreamSize - The size of the keystream buffer, in bytes.
    key - The key to use when encrypting/decrypting the data.  The format of
        this key will depend on the block cipher you are using.  The key is
        used by the Initialize function to calculate the hash subkey.
    keyType - The storage type of the key
    keyMode - The length and algorithm of the key
    // Error type
    BLOCK_CIPHER_ERRORS error;

  Returns:
      * BLOCK_CIPHER_ERROR_NONE - no error.
      * BLOCK_CIPHER_ERROR_UNSUPPORTED_KEY_TYPE - The specified key type is not
        supported by the firmware implementation being used
      * BLOCK_CIPHER_ERROR_INVALID_HANDLE - The specified handle was invalid
    
  Example:
    <code>
    // Initialize the GCM block cipher module for use with AES.
    SYS_MODULE_OBJ sysObject;
    BLOCK_CIPHER_HANDLE handle;
    BLOCK_CIPHER_16BV1_GCM_CONTEXT context;
    // Initialization vector for GCM mode
    static uint8_t ivValue[12] = {0xca,0xfe,0xba,0xbe,0xfa,0xce,0xdb,0xad,0xde,0xca,0xf8,0x88};
    //keyStream could also be allocated memory instead of fixed memory
    uint8_t keyStream[AES_BLOCK_SIZE*4];
    // The encryption key
    static uint8_t AESKey128[] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
    // Error type
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

    // Initialize the Block Cipher context
    error = BLOCK_CIPHER_16BV1_GCM_Initialize (handle, &context, CRYPTO_16BV1_ALGORITHM_AES, CRYPTO_16BV1_ALGORITHM_AES, AES_BLOCK_SIZE, (uint8_t *)ivValue, 12, (void *)&keyStream, sizeof(keyStream), &AESKey128, CRYPTO_KEY_SOFTWARE, CRYPTO_AES_128_KEY);

    if (error != BLOCK_CIPHER_ERROR_NONE)
    {
        // error
    }
    </code>
*/
BLOCK_CIPHER_ERRORS BLOCK_CIPHER_16BV1_GCM_Initialize (BLOCK_CIPHER_HANDLE handle, BLOCK_CIPHER_16BV1_GCM_CONTEXT * context, BLOCK_CIPHER_FunctionEncrypt encryptFunction, BLOCK_CIPHER_FunctionDecrypt decryptFunction, uint32_t blockSize, uint8_t * initializationVector, uint32_t initializationVectorLen, void * keyStream, uint32_t keyStreamSize, void * key, CRYPTO_KEY_TYPE keyType, CRYPTO_KEY_MODE keyMode);

/***********************************************************************************************************************************************************************************************
  Function:
    BLOCK_CIPHER_ERRORS BLOCK_CIPHER_16BV1_GCM_Encrypt (BLOCK_CIPHER_HANDLE handle,
        uint8_t * cipherText, uint32_t * numCipherBytes, uint8_t * plainText,
        uint32_t numPlainBytes, uint8_t * authenticationTag, uint8_t tagLen,
        uint32_t options)
    
  Summary:
    Encrypts/authenticates plain text using Galois/counter mode.

  Description:
    Encrypts/authenticates plain text using Galois/counter mode. This
    function accepts a combination of data that must be authenticated but
    not encrypted, and data that must be authenticated and encrypted. The
    user should initialize a GCM context using BLOCK_CIPHER_16BV1_GCM_Initialize,
    then pass all authenticated-but-not-encrypted data into this function
    with the BLOCK_CIPHER_OPTION_AUTHENTICATE_ONLY option, and then pass
    any authenticated-and-encrypted data in using the
    BLOCK_CIPHER_OPTION_STREAM_CONTINUE option. When calling this function
    for the final time, the user must use the
    BLOCK_CIPHER_OPTION_STREAM_COMPLETE option to generate padding required
    to compute the authentication tag successfully. Note that
    BLOCK_CIPHER_OPTION_STREAM_COMPLETE must always be specified at the end
    of a stream, even if no encryption is being done.
    
    The GMAC (Galois Message Authentication Code) mode can be used by using
    GCM without providing any data to encrypt (e.g. by only using
    BLOCK_CIPHER_OPTION_AUTHENTICATE_ONLY and
    BLOCK_CIPHER_OPTION_STREAM_COMPLETE options).
  Conditions:
    The GCM context must be initialized with the block cipher
    encrypt/decrypt functions and the block cipher algorithm's block size.
    The block cipher module must be initialized, if necessary.
    
    The initializationVector parameter in the BLOCK_CIPHER_16BV1_GCM_CONTEXT
    structure should be initialized. See section 8.2 of the GCM
    specification for more information.
  Input:
    handle -             A handle that is passed to the block cipher's
                         encrypt/decrypt functions to specify which instance of
                         the block cipher module to use.
    cipherText -         The cipher text produced by the encryption. This
                         buffer must be at least numBytes long.
    numCipherBytes -     The number of ciphertext bytes produced or the amount
                         of data authenticated
    plainText -          The plain test to encrypt. Must be at least
                         numBytes long.
    numPlainBytes -      The number of plain text bytes that must be
                         encrypted.
    authenticationTag -  Pointer to a structure to contain the
                         authentication tag generated by a series of
                         authentications. The tag will be written to this
                         buffer when the user specifies the
                         BLOCK_CIPHER_OPTION_STREAM_COMPLETE option.
    tagLen -             The length of the authentication tag, in bytes. 16
                         bytes is standard. Shorter byte lengths can be
                         used, but they provide less reliable
                         authentication.
    options -            Block cipher encryption options that the user can
                         specify, or'd together. If no option is specified
                         then BLOCK_CIPHER_OPTION_STREAM_CONTINUE is
                         assumed. Valid options for this function are
                         * BLOCK_CIPHER_OPTION_AUTHENTICATE_ONLY
                         * BLOCK_CIPHER_OPTION_STREAM_CONTINUE
                         * BLOCK_CIPHER_OPTION_STREAM_COMPLETE
  Return:
    Returns a member of the BLOCK_CIPHER_ERRORS enumeration:
      * BLOCK_CIPHER_ERROR_NONE - no error.
      * BLOCK_CIPHER_ERROR_KEY_STREAM_GEN_OUT_OF_SPACE - There was not
        enough room remaining in the context->keyStream buffer to fit the
        key data requested by the numBlocks parameter.
      * BLOCK_CIPHER_ERROR_GCM_COUNTER_EXPIRED - The requesting call
        has caused the counter number to run out of unique combinations.
      * BLOCK_CIPHER_ERROR_INVALID_HANDLE - The specified handle was invalid

  Example:
    <code>
    // ***************************************************************
    // Encrypt data in GCM mode with the AES algorithm.
    // ***************************************************************
    
    // System module object variable (for initializing AES)
    SYS_MODULE_OBJ sysObject;
    
    // Drive handle variable, to describe which AES module to use
    BLOCK_CIPHER_HANDLE handle;
    
    // GCM mode context
    BLOCK_CIPHER_16BV1_GCM_CONTEXT context;

    // Initialization vector for GCM mode
    static uint8_t ivValue[12] = {0xca,0xfe,0xba,0xbe,0xfa,0xce,0xdb,0xad,0xde,0xca,0xf8,0x88};

    // Data that will be authenticated, but not encrypted.
    uint8_t authData[20] = {0xfe,0xed,0xfa,0xce,0xde,0xad,0xbe,0xef,0xfe,0xed,0xfa,0xce,0xde,0xad,0xbe,0xef,0xab,0xad,0xda,0xd2,};

    // Plain text to encrypt
    static uint8_t plain_text[] = { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a, \
                                    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51, \
                                    0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef, \
                                    0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10};
    // The encryption key
    static uint8_t AESKey128[] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
    
    // Buffer to contain encrypted plaintext
    uint8_t cipher_text[sizeof(plain_text)];
    //keyStream could also be allocated memory instead of fixed memory
    uint8_t keyStream[AES_BLOCK_SIZE*4];
    // Structure to contain the calculated authentication tag
    uint8_t tag[16];
    // Error type
    BLOCK_CIPHER_ERRORS error;
    uint32_t * numCipherBytes;
    
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
    
    // Initialize the Block Cipher context
    error = BLOCK_CIPHER_16BV1_GCM_Initialize (handle, &context, CRYPTO_16BV1_ALGORITHM_AES, CRYPTO_16BV1_ALGORITHM_AES, AES_BLOCK_SIZE, (uint8_t *)ivValue, 12, (void *)&keyStream, sizeof(keyStream), &AESKey128, CRYPTO_KEY_SOFTWARE, CRYPTO_AES_128_KEY);
    
    if (error != BLOCK_CIPHER_ERROR_NONE)
    {
        // error
    }

    //Generate 4 blocks of key stream
    BLOCK_CIPHER_16BV1_GCM_KeyStreamGenerate(handle, 4, &context, 0);

    // Authenticate the non-encrypted data
    if (BLOCK_CIPHER_16BV1_GCM_Encrypt (handle, NULL, &numCipherBytes, (uint8_t *)authData, 20, NULL, 0, BLOCK_CIPHER_OPTION_AUTHENTICATE_ONLY) != BLOCK_CIPHER_ERROR_NONE)
    {
        // An error occured
        while(1);
    }

    // As an example, this data will be encrypted in two blocks, to demonstrate how to use the options.
    // Encrypt the first forty bytes of data.
    // Note that at this point, you don't really need to specify the tag pointer or its length.  This parameter only
    // needs to be specified when the BLOCK_CIPHER_OPTION_STREAM_COMPLETE option is used.
    if (BLOCK_CIPHER_16BV1_GCM_Encrypt (handle, cipherText, &numCipherBytes, (uint8_t *)ptShort, 40, tag, 16, BLOCK_CIPHER_OPTION_STREAM_CONTINUE) != BLOCK_CIPHER_ERROR_NONE)
    {
        // An error occured
        while(1);
    }

    //Encrypt the final twenty bytes of data.
    // Since we are using BLOCK_CIPHER_OPTION_STREAM_COMPLETE, we must specify a pointer to and length of the tag array, to store the auth tag.
    if (BLOCK_CIPHER_16BV1_GCM_Encrypt (handle, cipherText + 40, &numCipherBytes, (uint8_t *)ptShort + 40, 20, tag, 16, BLOCK_CIPHER_OPTION_STREAM_COMPLETE) != BLOCK_CIPHER_ERROR_NONE)
    {
        // An error occured
        while(1);
    }
    </code>
                                                                                                                                                                                                
  ***********************************************************************************************************************************************************************************************/
BLOCK_CIPHER_ERRORS BLOCK_CIPHER_16BV1_GCM_Encrypt (BLOCK_CIPHER_HANDLE handle, uint8_t * cipherText, uint32_t * numCipherBytes, uint8_t * plainText, uint32_t numBytes, uint8_t * authenticationTag, uint8_t tagLen, uint32_t options);

// *****************************************************************************
/* Function:
    BLOCK_CIPHER_ERRORS BLOCK_CIPHER_16BV1_GCM_Decrypt (BLOCK_CIPHER_HANDLE handle,
        uint8_t * plainText, uint32_t * numPlainBytes, uint8_t * cipherText,
        uint32_t numCipherBytes, uint8_t * authenticationTag, uint8_t tagLen,
        uint32_t options)

  Summary:
    Decrypts/authenticates plain text using Galois/counter mode.
  Description:
    Decrypts/authenticates plain text using Galois/counter mode. This
    function accepts a combination of data that must be authenticated but
    not decrypted, and data that must be authenticated and decrypted. The
    user should initialize a GCM context using BLOCK_CIPHER_16BV1_GCM_Initialize,
    then pass all authenticated-but-not-decrypted data into this function
    with the BLOCK_CIPHER_OPTION_AUTHENTICATE_ONLY option, and then pass
    any authenticated-and-decrypted data in using the
    BLOCK_CIPHER_OPTION_STREAM_CONTINUE option. When calling this function
    for the final time, the user must use the
    BLOCK_CIPHER_OPTION_STREAM_COMPLETE option to generate padding required
    to compute the authentication tag successfully. Note that
    BLOCK_CIPHER_OPTION_STREAM_COMPLETE must always be specified at the end
    of a stream, even if no encryption is being done.
    
    The GMAC (Galois Message Authentication Code) mode can be used by using
    GCM without providing any data to decrypt (e.g. by only using
    BLOCK_CIPHER_OPTION_AUTHENTICATE_ONLY and
    BLOCK_CIPHER_OPTION_STREAM_COMPLETE options).
  Conditions:
    The GCM context must be initialized with the block cipher
    encrypt/decrypt functions and the block cipher algorithm's block size.
    The block cipher module must be initialized, if necessary.
    
    The initializationVector parameter in the BLOCK_CIPHER_16BV1_GCM_CONTEXT
    structure should be initialized. See section 8.2 of the GCM
    specification for more information.
  Input:
    handle -             A handle that is passed to the block cipher's
                         encrypt/decrypt functions to specify which instance of
                         the block cipher module to use.
    plainText -          The cipher text produced by the decryption. This
                         buffer must be at least numBytes long.
    numPlainBytes -      The number of plaintext bytes produced or the number
                         of bytes authenticated
    cipherText -         The cipher test to decrypt. Must be at least
                         numBytes long.
    numCipherBytes -     The number of cipher text bytes that must be
                         decrypted.
    authenticationTag -  Pointer to a structure containing the
                         authentication tag generated by an
                         encrypt/authenticate operation. The tag calculated
                         during decryption will be checked against this
                         buffer when the user specifies the
                         BLOCK_CIPHER_OPTION_STREAM_COMPLETE option.
    tagLen -             The length of the authentication tag, in bytes.
    options -            Block cipher decryption options that the user can
                         specify, or'd together. If no option is specified
                         then BLOCK_CIPHER_OPTION_STREAM_CONTINUE is
                         assumed. Valid options for this function are
                         * BLOCK_CIPHER_OPTION_AUTHENTICATE_ONLY
                         * BLOCK_CIPHER_OPTION_STREAM_CONTINUE
                         * BLOCK_CIPHER_OPTION_STREAM_COMPLETE
  Return:
    Returns a member of the BLOCK_CIPHER_ERRORS enumeration:
      * BLOCK_CIPHER_ERROR_NONE - no error.
      * BLOCK_CIPHER_ERROR_KEY_STREAM_GEN_OUT_OF_SPACE - There was not
        enough room remaining in the context->keyStream buffer to fit the
        key data requested by the numBlocks parameter.
      * BLOCK_CIPHER_ERROR_GCM_COUNTER_EXPIRED - The requesting call
        has caused the counter number to run out of unique combinations.
      * BLOCK_CIPHER_ERROR_INVALID_AUTHENTICATION - The calculated
        authentication tag did not match the one provided by the user.
      * BLOCK_CIPHER_ERROR_INVALID_HANDLE - The specified handle was invalid

  Example:
    <code>
    // ***************************************************************
    // Decrypt data in GCM mode with the AES algorithm.
    // ***************************************************************

    // System module object variable (for initializing AES)
    SYS_MODULE_OBJ sysObject;

    // Drive handle variable, to describe which AES module to use
    BLOCK_CIPHER_HANDLE handle;

    // GCM mode context
    BLOCK_CIPHER_16BV1_GCM_CONTEXT context;

    // Initialization vector for GCM mode
    static uint8_t ivValue[12] = {0xca,0xfe,0xba,0xbe,0xfa,0xce,0xdb,0xad,0xde,0xca,0xf8,0x88};

    // Data that will be authenticated, but not decrypted.
    uint8_t authData[20] = {0xfe,0xed,0xfa,0xce,0xde,0xad,0xbe,0xef,0xfe,0xed,0xfa,0xce,0xde,0xad,0xbe,0xef,0xab,0xad,0xda,0xd2,};

   // Cipher text to decrypt
    static uint8_t cipher_text[] = { 0x42, 0x83, 0x1e, 0xc2, 0x21, 0x77, 0x74, 0x24, 0x4b, 0x72, 0x21, 0xb7, 0x84, 0xd0, 0xd4, 0x9c, \
                                    0xe3, 0xaa, 0x21, 0x2f, 0x2c, 0x02, 0xa4, 0xe0, 0x35, 0xc1, 0x7e, 0x23, 0x29, 0xac, 0xa1, 0x2e, \
                                    0x21, 0xd5, 0x14, 0xb2, 0x54, 0x66, 0x93, 0x1c, 0x7d, 0x8f, 0x6a, 0x5a, 0xac, 0x84, 0xaa, 0x05, \
                                    0x1b, 0xa3, 0x0b, 0x39, 0x6a, 0x0a, 0xac, 0x97, 0x3d, 0x58, 0xe0, 0x91,};


    // The decryption key
    static uint8_t AESKey128[] = {0xfe,0xff,0xe9,0x92,0x86,0x65,0x73,0x1c,0x6d,0x6a,0x8f,0x94,0x67,0x30,0x83,0x08};
    // Buffer to contain decrypted ciphertext
    uint8_t plain_text[sizeof(cipher_text)];
    //keyStream could also be allocated memory instead of fixed memory
    uint8_t keyStream[AES_BLOCK_SIZE*4];
    // The authentication tag for our ciphertext and our authData.
    uint8_t tag[]  = {0x5b, 0xc9, 0x4f, 0xbc, 0x32, 0x21, 0xa5, 0xdb, 0x94, 0xfa, 0xe9, 0x5a, 0xe7, 0x12, 0x1a, 0x47,};
    // Error type
    BLOCK_CIPHER_ERRORS error;
    uint32_t * numPlainBytes;
    
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

    // Initialize the Block Cipher context
	error = BLOCK_CIPHER_16BV1_GCM_Initialize (handle, &context, CRYPTO_16BV1_ALGORITHM_AES, CRYPTO_16BV1_ALGORITHM_AES, AES_BLOCK_SIZE, (uint8_t *)ivValue, 12, (void *)&keyStream, sizeof(keyStream), &AESKey128, CRYPTO_KEY_SOFTWARE, CRYPTO_AES_128_KEY);
    
    if (error != BLOCK_CIPHER_ERROR_NONE)
    {
        // error
    }

    //Generate 4 blocks of key stream
    BLOCK_CIPHER_16BV1_GCM_KeyStreamGenerate(handle, 4, 0);

    // Authenticate the non-encrypted data
    if (BLOCK_CIPHER_16BV1_GCM_Decrypt (handle, NULL, &numPlainBytes, (uint8_t *)authData, 20, NULL, 0, BLOCK_CIPHER_OPTION_AUTHENTICATE_ONLY) != BLOCK_CIPHER_ERROR_NONE)
    {
        // An error occured
        while(1);
    }

    // As an example, this data will be decrypted in two blocks, to demonstrate how to use the options.
    // Decrypt the first forty bytes of data.
    // Note that at this point, you don't really need to specify the tag pointer or its length.  This parameter only
    // needs to be specified when the BLOCK_CIPHER_OPTION_STREAM_COMPLETE option is used.
    if (BLOCK_CIPHER_16BV1_GCM_Decrypt (handle, plain_text, &numPlainBytes, (uint8_t *)cipher_text, 40, tag, 16, BLOCK_CIPHER_OPTION_STREAM_CONTINUE) != BLOCK_CIPHER_ERROR_NONE)
    {
        // An error occured
        while(1);
    }

    // Decrypt the final twenty bytes of data.
    // Since we are using BLOCK_CIPHER_OPTION_STREAM_COMPLETE, we must specify the authentication tag and its length.  If it does not match
    // the tag we obtain by decrypting the data, the Decrypt function will return BLOCK_CIPHER_ERROR_INVALID_AUTHENTICATION.
    if (BLOCK_CIPHER_16BV1_GCM_Decrypt (handle, plain_text + 40, &numPlainBytes, (uint8_t *)cipher_text + 40, 20, tag, 16, BLOCK_CIPHER_OPTION_STREAM_COMPLETE) != BLOCK_CIPHER_ERROR_NONE)
    {
        // An error occured
        while(1);
    }
    </code>
*/
BLOCK_CIPHER_ERRORS BLOCK_CIPHER_16BV1_GCM_Decrypt (BLOCK_CIPHER_HANDLE handle, uint8_t * plainText, uint32_t * numPlainBytes, uint8_t * cipherText, uint32_t numCipherBytes, uint8_t * authenticationTag, uint8_t tagLen, uint32_t options);

// *****************************************************************************
/* Function:
    BLOCK_CIPHER_ERRORS BLOCK_CIPHER_16BV1_GCM_KeyStreamGenerate (
        BLOCK_CIPHER_HANDLE handle, uint32_t numBlocks, uint32_t options)

  Summary:
    Generates a key stream for use with the Galois/counter mode.

  Description:
    Generates a key stream for use with the Galois/counter mode.
  
  Precondition:
    The GCM context must be initialized with the block cipher encrypt/decrypt
    functions and the block cipher algorithm's block size.  The block cipher 
    module must be initialized, if necessary.

    The initializationVector parameter in the BLOCK_CIPHER_16BV1_GCM_CONTEXT structure
    should be initialized.  The size of this vector is the same as the block size 
    of the cipher you are using.
    
  Parameters:
    handle - A handle that is passed to the block cipher's encrypt/decrypt 
        functions to specify which instance of the block cipher module to use.
    numBlocks - The number of blocks of key stream that should be created. 
        context->keyStream should have enough space remaining to handle this request.
    options - Block cipher encryption options that the user can specify, or'd together.  This
        function currently does not support any options.
  
  Returns:
    Returns a member of the BLOCK_CIPHER_ERRORS enumeration:
        * BLOCK_CIPHER_ERROR_NONE - no error.
        * BLOCK_CIPHER_ERROR_KEY_STREAM_GEN_OUT_OF_SPACE - There was not enough room 
            remaining in the context->keyStream buffer to fit the key data requested by the 
            numBlocks parameter.
        * BLOCK_CIPHER_ERROR_GCM_COUNTER_EXPIRED - The requesting call has caused the counter
            number to run out of unique combinations.
      * BLOCK_CIPHER_ERROR_INVALID_HANDLE - The specified handle was invalid

  Example:
    <code>
    // ***************************************************************
    // Encrypt data in GCM mode with the AES algorithm.
    // ***************************************************************
    
    // System module object variable (for initializing AES)
    SYS_MODULE_OBJ sysObject;
    
    // Drive handle variable, to describe which AES module to use
    BLOCK_CIPHER_HANDLE handle;
    
    // GCM mode context
    BLOCK_CIPHER_16BV1_GCM_CONTEXT context;

    // Initialization vector for GCM mode
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
    //keyStream could also be allocated memory instead of fixed memory
    uint8_t keyStream[AES_BLOCK_SIZE*4];
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
    error = BLOCK_CIPHER_16BV1_GCM_Initialize (handle, &context, CRYPTO_16BV1_ALGORITHM_AES, CRYPTO_16BV1_ALGORITHM_AES, AES_BLOCK_SIZE, (uint8_t *)ivValue, 12, (void *)&keyStream, sizeof(keyStream), &AESKey128, CRYPTO_KEY_SOFTWARE, CRYPTO_AES_128_KEY);

    if (error != BLOCK_CIPHER_ERROR_NONE)
    {
        // error
    }

    //Generate 4 blocks of key stream
    BLOCK_CIPHER_16BV1_GCM_KeyStreamGenerate(handle, 4, BLOCK_CIPHER_OPTION_STREAM_START);

    //Encrypt the data
    BLOCK_CIPHER_16BV1_GCM_Encrypt (handle, cipher_text,(void *) plain_text, sizeof(plain_text), BLOCK_CIPHER_OPTION_STREAM_CONTINUE);
    </code>
*/
BLOCK_CIPHER_ERRORS BLOCK_CIPHER_16BV1_GCM_KeyStreamGenerate (BLOCK_CIPHER_HANDLE handle, uint32_t numBlocks, uint32_t options);

#endif      // _BLOCK_CIPHER_16BV1_GCM_H
