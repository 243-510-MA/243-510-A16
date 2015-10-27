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

#if !defined (_CRYPTO_SW_CONFIG_H)
#define _CRYPTO_SW_CONFIG_H
 
/****************************************************************************************************************************/
/* Block Cipher Configuration options (AES, TDES, XTEA)                                                                     */
/****************************************************************************************************************************/
// Defines the largest block size used by the ciphers you are using with the block cipher modes of operation
#define CRYPTO_CONFIG_SW_BLOCK_MAX_SIZE      16ul

// Defines the maximum number of block cipher handles
#define CRYPTO_CONFIG_SW_BLOCK_HANDLE_MAXIMUM  10u
 
/****************************************************************************************************************************/
/* AES Configuration options                                                                                                */
/****************************************************************************************************************************/

// Supported key lengths.  Select one of these four key length options for your application.
 
#define CRYPTO_CONFIG_SW_AES_KEY_DYNAMIC_ENABLE         // Dynamically determine key length at runtime
#define CRYPTO_CONFIG_SW_AES_KEY_128_ENABLE           // Use 128-bit key lengths
#define CRYPTO_CONFIG_SW_AES_KEY_192_ENABLE           // Use 192-bit key lengths
#define CRYPTO_CONFIG_SW_AES_KEY_256_ENABLE           // Use 256-bit key lengths.  Enabling this will actually enable CRYPTO_CONFIG_SW_AES_KEY_DYNAMIC_ENABLE

#endif      // _CRYPTO_SW_CONFIG_H
