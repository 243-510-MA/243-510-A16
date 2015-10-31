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
 
#if !defined (_CRYPTO_SW_H)
#define _CRYPTO_SW_H
 

 // Include for the block cipher modes of operation
#include "crypto_sw/block_cipher_sw.h"

 // Include for the ARCFOUR module
 #include "crypto_sw/arcfour_sw.h"
 
 // Include for the AES module
 #include "crypto_sw/aes_sw.h"
 
 // Include for the TDES module
 #include "crypto_sw/tdes_sw.h"
 
 // Include for the RSA module
 #include "crypto_sw/rsa_sw.h"
 
 // Include for the XTEA module
 #include "crypto_sw/xtea_sw.h"
 
#endif      // _CRYPTO_SW_H
 