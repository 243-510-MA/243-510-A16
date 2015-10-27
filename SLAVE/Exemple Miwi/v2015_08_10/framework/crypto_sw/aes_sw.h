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

#if !defined (_AES_SW_H)
#define _AES_SW_H

// *****************************************************************************
// *****************************************************************************
// Section: Includes
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include "crypto_sw/block_cipher_sw.h"

// *****************************************************************************
// *****************************************************************************
// Section: Constants & Data Types
// *****************************************************************************
// *****************************************************************************

// Use an AES key length of 128-bits / 16 bytes.
#define AES_SW_KEY_SIZE_128_BIT    16 
// Use an AES key length of 192-bits / 24 bytes.
#define AES_SW_KEY_SIZE_192_BIT    24 
// Use an AES key length of 256-bits / 32 bytes.
#define AES_SW_KEY_SIZE_256_BIT    32 

// Definition of a 128-bit key to simplify the creation of a round key buffer for the 
// AES_SW_RoundKeysCreate() function.
typedef struct
{
    uint32_t key_length;                    // Length of the key
    uint32_t data[44];                  /*Round keys*/
} AES_SW_ROUND_KEYS_128_BIT;

// Definition of a 192-bit key to simplify the creation of a round key buffer for the 
// AES_SW_RoundKeysCreate() function.
typedef struct
{
    uint32_t key_length;                    // Length of the key
    uint32_t data[52];                  // Round keys
} AES_SW_ROUND_KEYS_192_BIT;

// Definition of a 256-bit key to simplify the creation of a round key buffer for the 
// AES_SW_RoundKeysCreate() function.
typedef struct
{
    uint32_t key_length;                    // Length of the key
    uint32_t data[60];                  // Round keys
} AES_SW_ROUND_KEYS_256_BIT;

#if defined(CRYPTO_CONFIG_SW_AES_KEY_128_ENABLE)
    #define AES_SW_ROUND_KEYS AES_SW_ROUND_KEYS_128_BIT
#elif defined(CRYPTO_CONFIG_SW_AES_KEY_192_ENABLE)
    #define AES_SW_ROUND_KEYS AES_SW_ROUND_KEYS_192_BIT
#else
    /**********************************************************************
      Definition for the AES module's Round Key structure. Depending on the
      configuration of the library, this could be defined as
      AES_SW_ROUND_KEYS_128_BIT, AES_SW_ROUND_KEYS_192_BIT, or
      AES_SW_ROUND_KEYS_256_BIT.                                              
      **********************************************************************/
    #define AES_SW_ROUND_KEYS AES_SW_ROUND_KEYS_256_BIT
#endif

// The AES block size (16 bytes)
#define AES_SW_BLOCK_SIZE  16

// *****************************************************************************
// *****************************************************************************
// Section: AES Interface Routines
// *****************************************************************************
// *****************************************************************************



/*******************************************************************************
  Function:
    void AES_SW_RoundKeysCreate(    void* round_keys,
                                uint8_t* key, 
                                uint8_t key_size
                            )

  Summary:
    Creates a set of round keys from an AES key to be used in AES encryption and decryption of data blocks.

  Description:
    This routine takes an AES key and performs a key schedule to expand the key into a number of separate
    set of round keys.  These keys are commonly know as the Rijindael key schedule or a session key.

  Precondition:
    None.

  Parameters:
    round_keys - Pointer to the output buffer that will contain the expanded short key (Rijindael) schedule/ session key.  This is to be used in the 
                    encryption and decryption routines.  The round_keys buffer must be word aligned for the target processor.
    key        - The input key which can be 128, 192, or 256 bits in length.
    key_size   - Specifies the key length in bytes.  Valid options are\: 
                    * AES_SW_KEY_SIZE_128_BIT
                    * AES_SW_KEY_SIZE_192_BIT
                    * AES_SW_KEY_SIZE_256_BIT
                    The values 16, 24, and 32 may also be used instead of the above definitions.

  Returns:
    None

  Example:
    <code>
    static const uint8_t AESKey128[] = {  0x95, 0xA8, 0xEE, 0x8E, 
                                        0x89, 0x97, 0x9B, 0x9E, 
                                        0xFD, 0xCB, 0xC6, 0xEB, 
                                        0x97, 0x97, 0x52, 0x8D 
                                     };
    AES_SW_ROUND_KEYS_128_BIT round_keys;

    AES_SW_RoundKeysCreate(    &round_keys, 
                            AESKey128, 
                            AES_SW_KEY_SIZE_128_BIT
                       );
    </code>

  *****************************************************************************/
void AES_SW_RoundKeysCreate(void* round_keys, uint8_t* key, uint8_t key_size);

// *****************************************************************************
/* Function:
    void AES_SW_Encrypt (BLOCK_CIPHER_SW_HANDLE handle, void * cipherText, void * plainText, void * key)

  Summary:
    Encrypts a 128-bit block of data using the AES algorithm.

  Description:
    Encrypts a 128-bit block of data using the AES algorithm.
  
  Precondition:
    The AES module must be configured and initialized, if necessary.

  Parameters:
    handle - Pointer to the driver handle for the instance of the AES module you 
        are using to encrypt the plainText.  No function for pure software 
        implementation.
    cipherText - Buffer for the 128-bit output block of cipherText produced by 
        encrypting the plainText.
    plainText - The 128-bit block of plainText to encrypt.
    key - Pointer to a set of round keys created by the AES_SW_RoundKeysCreate function.
  
  Returns:
    None
    
  Remarks:
    AES should be used the a block cipher mode of operation.  See 
    block_cipher_sw.h for more information.
*/
void AES_SW_Encrypt (BLOCK_CIPHER_SW_HANDLE handle, void * cipherText, void * plainText, void * key);

// *****************************************************************************
/* Function:
    void AES_SW_Decrypt (BLOCK_CIPHER_SW_HANDLE handle, void * plainText, void * cipherText, void * key)

  Summary:
    Decrypts a 128-bit block of data using the AES algorithm.

  Description:
    Decrypts a 128-bit block of data using the AES algorithm.
  
  Precondition:
    The AES module must be configured and initialized, if necessary.

  Parameters:
    handle - Pointer to the driver handle for the instance of the AES module you 
        are using to decrypt the cipherText.  No function for pure software 
        implementation.
    plainText - Buffer for the 128-bit output block of plainText produced by 
        decrypting the cipherText.
    cipherText - The 128-bit block of cipherText to decrypt.
    key - Pointer to a set of round keys created by the AES_SW_RoundKeysCreate function.
  
  Returns:
    None.
    
  Remarks:
    AES should be used the a block cipher mode of operation.  See 
    block_cipher_sw.h for more information.
*/
void AES_SW_Decrypt (BLOCK_CIPHER_SW_HANDLE handle, void * plainText, void * cipherText, void * key);

#endif      // _AES_SW_H
