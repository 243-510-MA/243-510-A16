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

#if !defined (_BLOCK_CIPHER_16BV1_PRIVATE_H)
#define _BLOCK_CIPHER_16BV1_PRIVATE_H

#include "crypto_16bv1/block_cipher_16bv1.h"
#include <stdbool.h>

#define BLOCK_CIPHER_HANDLE_IN_USE          (void *)(-1)

void BLOCK_CIPHER_16BV1_PaddingInsert (uint8_t * block, uint8_t blockLen, uint8_t paddingLength, uint32_t options);
void * BLOCK_CIPHER_16BV1_HandleResolve (BLOCK_CIPHER_HANDLE handle);
void BLOCK_CIPHER_CurrentHandleInitialize (void);
bool BLOCK_CIPHER_CurrentHandleIsInitialized (void);
void BLOCK_CIPHER_HandleReInitialize (BLOCK_CIPHER_HANDLE handle);

typedef enum
{
    CRYPTO_KEY_STATE_UNLOADED = 0,
    CRYPTO_KEY_STATE_LOADED
} CRYPTO_KEY_STATE;

typedef enum
{
    BLOCK_CIPHER_ALGORITHM_AES = 0,
    BLOCK_CIPHER_ALGORITHM_TDES,
} BLOCK_CIPHER_ALGORITHM;

typedef enum
{
    BLOCK_CIPHER_OPERATION_ENCRYPT = 0,
    BLOCK_CIPHER_OPERATION_DECRYPT,
    BLOCK_CIPHER_OPERATION_GENERATE_KEYSTREAM
} BLOCK_CIPHER_OPERATION;

// Enumeration defining available block cipher modes of operation
typedef enum
{
    BLOCK_CIPHER_MODE_ECB = 0,      // Electronic Codebook mode
    BLOCK_CIPHER_MODE_CBC,          // Cipher-block Chaining mode
    BLOCK_CIPHER_MODE_CFB,          // Cipher Feedback mode
    BLOCK_CIPHER_MODE_OFB,          // Output Feedback mode
    BLOCK_CIPHER_MODE_CTR,          // Counter mode
    BLOCK_CIPHER_MODE_GCM           // Galois-Counter mode
} BLOCK_CIPHER_MODES;

typedef struct{
    unsigned CPHRMOD:3;
    unsigned CPHRSEL:1;
    unsigned :12;
} CRYCONL_CONTEXT;

typedef struct{
  unsigned KEYSRC:4;
  unsigned :1;
  unsigned KEYMOD:2;
  unsigned :1;
  unsigned CTRSIZE:7;
} CRYCONH_CONTEXT;

typedef void (*BLOCK_CIPHER_Tasks)(BLOCK_CIPHER_HANDLE handle);

typedef struct
{
    uint8_t * source;
    uint8_t * dest;
    uint32_t * outCount;
    uint32_t inCount;
    uint32_t options;
    BLOCK_CIPHER_MODES mode;
    BLOCK_CIPHER_STATE state;
    BLOCK_CIPHER_ERRORS error;
    CRYCONL_CONTEXT cryconl_context;
    CRYCONH_CONTEXT cryconh_context;
    uint8_t previousData[CRYPTO_CONFIG_16BV1_BLOCK_MAX_SIZE];
    uint8_t * previousText;
    uint8_t * inputText;
    uint8_t * outputText;
    uint32_t blockSize;
    uint8_t bytesRemaining;
    uint8_t __attribute__((aligned)) remainingData[CRYPTO_CONFIG_16BV1_BLOCK_MAX_SIZE];
    void * context;
    void * key;
    uint8_t keyLength;
    CRYPTO_KEY_TYPE keyType;
    CRYPTO_KEY_MODE keyMode;
    BLOCK_CIPHER_OPERATION operation;
    DRV_IO_INTENT intent;
    BLOCK_CIPHER_Tasks tasks;
    bool dataGenerated;
} BLOCK_CIPHER_OBJECT;

extern BLOCK_CIPHER_OBJECT blockHandles[];

#endif      // _BLOCK_CIPHER_16BV1_PRIVATE_H
