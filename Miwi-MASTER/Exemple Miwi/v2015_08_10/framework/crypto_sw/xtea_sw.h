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

#if !defined (_XTEA_SW_H)
#define _XTEA_SW_H

#include <stdint.h>
#include "crypto_sw/block_cipher_sw.h"

// *****************************************************************************
// *****************************************************************************
// Section: Constants & Data Types
// *****************************************************************************
// *****************************************************************************

// The XTEA algorithm block size
#define XTEA_SW_BLOCK_SIZE             8ul

// *****************************************************************************
// *****************************************************************************
// Section: XTEA Interface Routines
// *****************************************************************************
// *****************************************************************************

/*********************************************************************
 * Function:        void XTEA_SW_Configure (uint8_t iterations)
 *
 * Description:     Configures the XTEA module.
 *
 * PreCondition:    None
 *
 * Input:           iterations - The number of iterations of the 
 *                      XTEA algorithm that the encrypt/decrypt 
 *                      functions should perform for each block
 *                      encryption/decryption.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Remarks:         This implementation is not thread-safe.  If you 
 *                  are using XTEA for multiple applications in an 
 *                  preemptive operating system you must use the 
 *                  same number of iterations for all applications
 *                  to avoid error.
 ********************************************************************/
void XTEA_SW_Configure (uint8_t iterations);

/*********************************************************************
 * Function:        void XTEA_SW_Encrypt (BLOCK_CIPHER_SW_HANDLE handle, 
 *                      uint32_t * cipherText, uint32_t * plainText, 
 *                      uint32_t * key)
 *
 * Description:     Encrypts a 64-bit block of data using the XTEA algorithm.
 *
 * PreCondition:    None
 *
 * Input:           handle - Provided for compatibility with the block 
 *                      cipher modes of operations module.
 *                  cipherText - Pointer to the 64-bit output buffer for the 
 *                      encrypted plainText.
 *                  plainText - Pointer to one 64-bit block of data to 
 *                      encrypt.
 *                  key - Pointer to the 128-bit key.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Remarks:         None
 ********************************************************************/
void XTEA_SW_Encrypt (BLOCK_CIPHER_SW_HANDLE handle, uint32_t * cipherText, uint32_t * plainText, uint32_t * key);

/*********************************************************************
 * Function:        void XTEA_SW_Decrypt (BLOCK_CIPHER_SW_HANDLE handle, 
 *                      uint32_t * plainText, uint32_t * cipherText, 
 *                      uint32_t * key)
 *
 * Description:     Decrypts a 64-bit block of data using the XTEA algorithm.
 *
 * PreCondition:    None
 *
 * Input:           handle - Provided for compatibility with the block
 *                      cipher modes of operations module.
 *                  plainText - Pointer to the 64-bit output buffer for the 
 *                      decrypted plainText.
 *                  cipherText - Pointer to a 64-bit block of cipherText to 
 *                      decrypt.
 *                  key - Pointer to the 128-bit key.
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        None
 *
 * Note:            None
 ********************************************************************/
void XTEA_SW_Decrypt (BLOCK_CIPHER_SW_HANDLE handle, uint32_t * plainText, uint32_t * cipherText, uint32_t * key);


#endif      // _XTEA_SW_H
