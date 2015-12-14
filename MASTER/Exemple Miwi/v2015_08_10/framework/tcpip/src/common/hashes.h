/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    hashes.h

  Summary:
    

  Description:
    Hash Function Library Headers

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

#ifndef __HASHES_H_
#define __HASHES_H_

/****************************************************************************
  Section:
    Data Types
 ***************************************************************************/

// Type of hash being calculated
typedef enum {
    HASH_MD5 = 0u, // MD5 is being calculated
    HASH_SHA1 // SHA-1 is being calculated
} HASH_TYPE;

// Context storage for a hash operation
typedef struct {
    uint32_t h0; // Hash state h0
    uint32_t h1; // Hash state h1
    uint32_t h2; // Hash state h2
    uint32_t h3; // Hash state h3
    uint32_t h4; // Hash state h4
    uint32_t bytesSoFar; // Total number of bytes hashed so far
    uint8_t partialBlock[64]; // Beginning of next 64 byte block
    HASH_TYPE hashType; // Type of hash being calculated
} HASH_SUM;

/****************************************************************************
  Section:
    Function Prototypes
 ***************************************************************************/

#if defined(STACK_USE_SHA1)
void SHA1Initialize(HASH_SUM * theSum);
void SHA1AddData(HASH_SUM * theSum, uint8_t * data, uint16_t len);
void SHA1Calculate(HASH_SUM * theSum, uint8_t * result);
#if defined(__XC8)
void SHA1AddROMData(HASH_SUM * theSum, ROM uint8_t * data, uint16_t len);
#else
// Non-ROM variant for C30 / C32
#define SHA1AddROMData(a,b,c)  SHA1AddData(a,(uint8_t *)b,c)
#endif
#endif

#if defined(STACK_USE_MD5)
void MD5Initialize(HASH_SUM * theSum);
void MD5AddData(HASH_SUM * theSum, uint8_t * data, uint16_t len);
void MD5Calculate(HASH_SUM * theSum, uint8_t * result);
#if defined(__XC8)
void MD5AddROMData(HASH_SUM * theSum, ROM uint8_t * data, uint16_t len);
#else
// Non-ROM variant for C30 / C32
#define MD5AddROMData(a,b,c)  MD5AddData(a,(uint8_t *)b,c)
#endif
#endif

void HashAddData(HASH_SUM * theSum, uint8_t * data, uint16_t len);
#if defined(__XC8)
void HashAddROMData(HASH_SUM * theSum, ROM uint8_t * data, uint16_t len);
#else
// Non-ROM variant for C30 / C32
#define HashAddROMData(a,b,c)  HashAddData(a,(uint8_t *)b,c)
#endif

#endif
