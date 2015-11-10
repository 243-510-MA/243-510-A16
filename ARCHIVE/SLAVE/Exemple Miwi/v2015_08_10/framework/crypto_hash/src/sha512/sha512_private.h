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

#if !defined (_SHA512_PRIVATE_H)
#define _SHA512_PRIVATE_H

#define SHA2_ROTL(x,n)      ((x << n) | (x >> (64-n)))   
#define SHA2_ROTR(x,n)      ((x >> n) | (x << (64-n)))
#define SHA2_SHR(x,n)       (x >> n)

#define SHA2_SIG_U0(x)      (SHA2_ROTR(x,28) ^ SHA2_ROTR(x,34) ^ SHA2_ROTR(x,39))
#define SHA2_SIG_U1(x)      (SHA2_ROTR(x,14) ^ SHA2_ROTR(x,18) ^ SHA2_ROTR(x,41))
#define SHA2_SIG_L0(x)      (SHA2_ROTR(x,1) ^ SHA2_ROTR(x,8) ^ SHA2_SHR(x,7))
#define SHA2_SIG_L1(x)      (SHA2_ROTR(x,19) ^ SHA2_ROTR(x,61) ^ SHA2_SHR(x,6))

#define SHA2_CH(x,y,z)      ((x & y) ^ ((~x) & z))
#define SHA2_MAJ(x,y,z)     ((x & y) ^ (x & z) ^ (y & z)) 
 
void SHA512_OutputData (uint8_t * output, uint64_t value);
 void SHA512_HashBlock (SHA512_CONTEXT * context);

#endif      // _SHA512_PRIVATE_H
 