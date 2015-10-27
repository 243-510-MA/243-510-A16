/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    big_int.h

  Summary:
    

  Description:
     Big Integer Class Headers

 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) <2014> released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
//DOM-IGNORE-END

#ifndef __BIG_INT_H_
#define __BIG_INT_H_

#define BIGINT_DEBUG          0
#define BIGINT_DEBUG_COMPARE  0
#define RSAEXP_DEBUG          0

#if defined(__XC8)
#define BIGINT_DATA_SIZE    8ul // bits
#define BIGINT_DATA_TYPE    uint8_t
#define BIGINT_DATA_MAX     0xFFu
#define BIGINT_DATA_TYPE_2  uint16_t
#elif defined(__XC16)
#define BIGINT_DATA_SIZE    16ul // bits
#define BIGINT_DATA_TYPE    uint16_t
#define BIGINT_DATA_MAX     0xFFFFu
#define BIGINT_DATA_TYPE_2  uint32_t
#elif defined(__XC32)
#define BIGINT_DATA_SIZE    32ul // bits
#define BIGINT_DATA_TYPE    uint32_t
#define BIGINT_DATA_MAX     0xFFFFFFFFu
#define BIGINT_DATA_TYPE_2  uint64_t
#endif

typedef struct {
    BIGINT_DATA_TYPE *ptrLSB; // Pointer to the least significant byte/word (lowest memory address)
    BIGINT_DATA_TYPE *ptrMSB; // Pointer to the first non-zero most significant byte/word (higher memory address) if bMSBValid set
    BIGINT_DATA_TYPE *ptrMSBMax; // Pointer to the maximum memory address that ptrMSB could ever be (highest memory address)
    bool bMSBValid;
} BIGINT;

#if defined(__XC8)
typedef struct _BIGINT_ROM {
    ROM BIGINT_DATA_TYPE *ptrLSB;
    ROM BIGINT_DATA_TYPE *ptrMSB;
} BIGINT_ROM;
#else
#define BIGINT_ROM  BIGINT
#endif

void BigInt(BIGINT *theInt, BIGINT_DATA_TYPE *data, uint16_t wWordLength);
void BigIntMod(BIGINT*, BIGINT*);
void BigIntMultiply(BIGINT*, BIGINT*, BIGINT*);

void BigIntAdd(BIGINT*, BIGINT*);
void BigIntSubtract(BIGINT*, BIGINT*);
void BigIntSubtractROM(BIGINT*, BIGINT_ROM*);
void BigIntCopy(BIGINT*, BIGINT*);
void BigIntSquare(BIGINT *a, BIGINT *res);
void BigIntZero(BIGINT *theInt);

int BigIntMagnitudeDifference(BIGINT *a, BIGINT *b);
int BigIntMagnitudeDifferenceROM(BIGINT *a, BIGINT_ROM *b);
int8_t BigIntCompare(BIGINT*, BIGINT*);
uint16_t BigIntMagnitude(BIGINT *n);

void BigIntSwapEndianness(BIGINT *a);

void BigIntPrint(const BIGINT *a);

#if defined(__XC8)
void BigIntROM(BIGINT_ROM *theInt, ROM BIGINT_DATA_TYPE *data, uint16_t wWordLength);
void BigIntModROM(BIGINT*, BIGINT_ROM*);
void BigIntMultiplyROM(BIGINT*, BIGINT_ROM*, BIGINT*);
void BigIntAddROM(BIGINT*, BIGINT_ROM*);
void BigIntCopyROM(BIGINT*, BIGINT_ROM*);
int8_t BigIntCompareROM(BIGINT*, BIGINT_ROM*);
uint16_t BigIntMagnitudeROM(BIGINT_ROM *n);

extern ROM BIGINT_DATA_TYPE *_iBr, *_xBr;

void BigIntPrintROM(BIGINT_ROM*);
#else
#define BigIntROM(a,b,c)          BigInt(a,((BIGINT_DATA_TYPE*)(b)),c)
#define BigIntModROM(a,b)         BigIntMod(a,b)
#define BigIntMultiplyROM(a,b,c)  BigIntMultiply(a,b,c)
#define BigIntAddROM(a,b)         BigIntAdd(a,b)
#define BigIntCopyROM(a,b)        BigIntCopy(a,b)
#define BigIntCompareROM(a,b)     BigIntCompare(a,b)
#define BigIntMagnitudeROM(a)     BigIntMagnitude(a)
#endif

#endif
