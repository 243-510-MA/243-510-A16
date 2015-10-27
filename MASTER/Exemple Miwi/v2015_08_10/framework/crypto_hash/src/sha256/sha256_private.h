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

#if !defined (_SHA256_PRIVATE_H)
#define _SHA256_PRIVATE_H

#define SHA2_ROTL(x,n)      ((x << n) | (x >> (32-n)))   
#define SHA2_ROTR(x,n)      ((x >> n) | (x << (32-n)))
#define SHA2_SHR(x,n)       (x >> n)

#define SHA2_SIG_U0(x)      (SHA2_ROTR(x,2) ^ SHA2_ROTR(x,13) ^ SHA2_ROTR(x,22))
#define SHA2_SIG_U1(x)      (SHA2_ROTR(x,6) ^ SHA2_ROTR(x,11) ^ SHA2_ROTR(x,25))
#define SHA2_SIG_L0(x)      (SHA2_ROTR(x,7) ^ SHA2_ROTR(x,18) ^ SHA2_SHR(x,3))
#define SHA2_SIG_L1(x)      (SHA2_ROTR(x,17) ^ SHA2_ROTR(x,19) ^ SHA2_SHR(x,10))

#define SHA2_CH(x,y,z)      ((x & y) ^ ((~x) & z))
#define SHA2_MAJ(x,y,z)     ((x & y) ^ (x & z) ^ (y & z)) 
 
 void SHA256_OutputData (uint8_t * output, uint32_t value);
 void SHA256_HashBlock (SHA256_CONTEXT * context);

#endif      // _SHA256_PRIVATE_H
 