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

#if !defined (_BLOCK_CIPHER_SW_PRIVATE_H)
#define _BLOCK_CIPHER_SW_PRIVATE_H

#include "crypto_sw/block_cipher_sw.h"

extern void * blockHandles[];

#define BLOCK_CIPHER_SW_HANDLE_IN_USE          (void *)(-1)

void BLOCK_CIPHER_SW_PaddingInsert (uint8_t * block, uint8_t blockLen, uint8_t paddingLength, uint32_t options);
void * BLOCK_CIPHER_SW_HandleResolve (BLOCK_CIPHER_SW_HANDLE handle);

#endif      // _BLOCK_CIPHER_SW_PRIVATE_H
