/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    rsa.h

  Summary:
    

  Description:
    RSA Public Key Encryption Library Header

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

#ifndef __RSA_H_
#define __RSA_H_

#define RSA_KEY_WORDS  (SSL_RSA_KEY_SIZE/BIGINT_DATA_SIZE) // Represents the number of words in a key
#define RSA_PRIME_WORDS  (SSL_RSA_KEY_SIZE/BIGINT_DATA_SIZE/2) // Represents the number of words in an RSA prime

/****************************************************************************
  Section:
    State Machines and Status Codes
 ***************************************************************************/

// State machine for RSA processes
typedef enum {
    SM_RSA_IDLE = 0u, // Data is being initialized by the application
    SM_RSA_ENCRYPT_START, // Initial state for encryption processes; encryption is ready to begin
    SM_RSA_ENCRYPT, // RSA encryption is proceeding
    SM_RSA_DECRYPT_START, // Initial state for decryption processes; decryption is ready to begin
    SM_RSA_DECRYPT_FIND_M1, // First stage in the CRT decryption algorithm
    SM_RSA_DECRYPT_FIND_M2, // Second stage in the CRT decryption algorithm
    SM_RSA_DECRYPT_FINISH, // CRT values have been calculated, so finalize the operation
    SM_RSA_DONE // RSA process is complete
} SM_RSA;

// Status response from RSA procedures
typedef enum {
    RSA_WORKING = 0u, // RSA is working through a calculation
    RSA_FINISHED_M1, // RSA decryption has just completed calculation of the M1 CRT value
    RSA_FINISHED_M2, // RSA decryption has just completed calculation of the M2 CRT value
    RSA_DONE // The RSA calculation is complete
} RSA_STATUS;

// Indicates the data format for any RSA integer
typedef enum {
    RSA_BIG_ENDIAN = 0u, // Data expressed with the most significant byte first
    RSA_LITTLE_ENDIAN // Data expressed with the least significant byte first
} RSA_DATA_FORMAT;

// Indicates the RSA operation to be completed
typedef enum {
    RSA_OP_ENCRYPT = 0u, // This is an encryption procedure
    RSA_OP_DECRYPT // This is a decryption procedure
} RSA_OP;

/****************************************************************************
  Section:
    Function Prototypes
 ***************************************************************************/
void RSAInit(void);
bool RSABeginUsage(RSA_OP op, uint16_t vKeyByteLen);
void RSAEndUsage(void);
void RSASetData(uint8_t * data, uint16_t len, RSA_DATA_FORMAT format);
void RSASetResult(uint8_t * data, RSA_DATA_FORMAT format);
RSA_STATUS RSAStep(void);

#if defined(STACK_USE_RSA_ENCRYPT)
#define RSABeginEncrypt(a)  RSABeginUsage(RSA_OP_ENCRYPT, a)
#define RSAEndEncrypt()     RSAEndUsage()
void RSASetE(uint8_t * data, uint8_t len, RSA_DATA_FORMAT format);
void RSASetN(uint8_t * data, RSA_DATA_FORMAT format);
#endif

#if defined(STACK_USE_RSA_DECRYPT)
#define RSABeginDecrypt()   RSABeginUsage(RSA_OP_DECRYPT, SSL_RSA_KEY_SIZE/8)
#define RSAEndDecrypt()     RSAEndUsage()
#endif

/****************************************************************************
  Section:
    BigInt Function Selection
    Determines which BigInt functions to compile.
 ***************************************************************************/
#if defined(STACK_USE_RSA_ENCRYPT)
#define BI_USE_CONSTRUCTOR
#define BI_USE_ZERO
#define BI_USE_MOD
#define BI_USE_COMPARE
#define BI_USE_MAG_DIFF
#define BI_USE_MAG
#define BI_USE_MSB
#define BI_USE_MULTIPLY
#define BI_USE_SQUARE
#define BI_USE_COPY
#endif

#if defined(STACK_USE_RSA_DECRYPT)
#define BI_USE_CONSTRUCTOR
#define BI_USE_ZERO
#define BI_USE_COMPARE
#define BI_USE_MAG
#define BI_USE_MSB
#define BI_USE_MULTIPLY
#define BI_USE_SQUARE
#define BI_USE_ADD
#define BI_USE_SUBTRACT
#define BI_USE_COPY

#if defined(__XC8)
#define BI_USE_CONSTRUCTOR_ROM
#define BI_USE_COPY_ROM
#define BI_USE_MOD_ROM
#define BI_USE_COMPARE_ROM
#define BI_USE_MAG_DIFF_ROM
#define BI_USE_MAG_ROM
#define BI_USE_MULTIPLY_ROM
#else
#define BI_USE_MAG_DIFF
#define BI_USE_MOD
#endif
#endif

#endif
