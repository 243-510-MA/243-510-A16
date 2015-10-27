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

#if !defined (_CRYPTO_16BV1_H)
#define _CRYPTO_16BV1_H
 

 // Include for the block cipher modes of operation
#include "crypto_16bv1/block_cipher_16bv1.h"

// The AES block size (16 bytes)
#define AES_BLOCK_SIZE  16
 
/* Defines the data block size for the TDES algorithm. The TDES algorithm uses
   a fixed 8 byte data block so this is defined as a constant that can be used
   to define or measure against the TDES data block size. */
#define TDES_BLOCK_SIZE  8

#endif      // _CRYPTO_16BV1_H
