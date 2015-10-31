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

#if !defined (_RSA_PRIVATE_H)
#define _RSA_PRIVATE_H

#include <stdlib.h>
#include <stdbool.h>

#include "crypto_sw/rsa_sw.h"

#include "bigint/bigint.h"

bool RSAModExp(BIGINT_DATA  * result, BIGINT_DATA  * data, BIGINT_DATA  * n, BIGINT_DATA  * e);

void RSAPadData (uint8_t * result, uint8_t * source, uint16_t msgLen, uint16_t keyLen, RSA_RandomGet randFunction, RSA_SW_PAD_TYPE padType);

void RSASwapEndianness (BIGINT_DATA * X);

#endif      // _RSA_PRIVATE_H
