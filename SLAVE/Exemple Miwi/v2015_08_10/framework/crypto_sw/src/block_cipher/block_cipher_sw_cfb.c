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

#include "crypto_sw/block_cipher_sw.h"
#include "crypto_sw/src/block_cipher/block_cipher_sw_private.h"
#include "system_config.h"
#include <string.h>

BLOCK_CIPHER_SW_ERRORS BLOCK_CIPHER_SW_CFB_Initialize (BLOCK_CIPHER_SW_HANDLE handle, BLOCK_CIPHER_SW_CFB_CONTEXT * context, BLOCK_CIPHER_SW_FunctionEncrypt encryptFunction, BLOCK_CIPHER_SW_FunctionDecrypt decryptFunction, uint32_t blockSize, uint8_t * initializationVector, void * key, CRYPTO_SW_KEY_TYPE keyType)
{
    if ((keyType != CRYPTO_SW_KEY_SOFTWARE_EXPANDED) && (keyType != CRYPTO_SW_KEY_SOFTWARE))
    {
        return BLOCK_CIPHER_SW_ERROR_UNSUPPORTED_KEY_TYPE;
    }

    if (handle >= 0 && handle < CRYPTO_CONFIG_SW_BLOCK_HANDLE_MAXIMUM)
    {
        blockHandles[handle] = context;
    }
    else
    {
        return BLOCK_CIPHER_SW_ERROR_INVALID_HANDLE;
    }

    context->encrypt = encryptFunction;
    context->decrypt = decryptFunction;
    context->blockSize = blockSize;
    memcpy (context->initializationVector, initializationVector, blockSize);
    context->key = key;
    context->keyType = keyType;

    return BLOCK_CIPHER_SW_ERROR_NONE;
}

BLOCK_CIPHER_SW_ERRORS BLOCK_CIPHER_SW_CFB_Encrypt (BLOCK_CIPHER_SW_HANDLE handle, uint8_t * cipherText, uint32_t * numCipherBytes, uint8_t * plainText, uint32_t numPlainBytes, uint32_t options)
{
    BLOCK_CIPHER_SW_CFB_CONTEXT * context = (BLOCK_CIPHER_SW_CFB_CONTEXT *)BLOCK_CIPHER_SW_HandleResolve(handle);

    if (context == NULL)
    {
        return BLOCK_CIPHER_SW_ERROR_INVALID_HANDLE;
    }

    if (numCipherBytes != NULL)
    {
        *numCipherBytes = numPlainBytes;
    }

    if((options & BLOCK_CIPHER_SW_OPTION_STREAM_START) == BLOCK_CIPHER_SW_OPTION_STREAM_START)
    {
        context->bytesRemaining = context->blockSize;
    }

    while(numPlainBytes--)
    {
        if(context->bytesRemaining == context->blockSize)
        {
            (*context->encrypt)(handle, context->initializationVector, context->initializationVector, context->key);
            context->bytesRemaining = 0;
        }

        *cipherText = *plainText++ ^ context->initializationVector[context->bytesRemaining];
        context->initializationVector[context->bytesRemaining++] = *cipherText++;
    }

    return BLOCK_CIPHER_SW_ERROR_NONE;
}

BLOCK_CIPHER_SW_ERRORS BLOCK_CIPHER_SW_CFB_Decrypt (BLOCK_CIPHER_SW_HANDLE handle, uint8_t * plainText, uint32_t * numPlainBytes, uint8_t * cipherText, uint32_t numCipherBytes, uint32_t options)
{
    BLOCK_CIPHER_SW_CFB_CONTEXT * context = (BLOCK_CIPHER_SW_CFB_CONTEXT *)BLOCK_CIPHER_SW_HandleResolve(handle);

    if (context == NULL)
    {
        return BLOCK_CIPHER_SW_ERROR_INVALID_HANDLE;
    }

    if (numPlainBytes != NULL)
    {
        *numPlainBytes = numCipherBytes;
    }

    if((options & BLOCK_CIPHER_SW_OPTION_STREAM_START) == BLOCK_CIPHER_SW_OPTION_STREAM_START)
    {
        context->bytesRemaining = context->blockSize;
    }

    while(numCipherBytes--)
    {
        if(context->bytesRemaining == context->blockSize)
        {
            (*context->encrypt)(handle, context->initializationVector, context->initializationVector, context->key);
            context->bytesRemaining = 0;
        }

        *plainText++ = *cipherText ^ context->initializationVector[context->bytesRemaining];
        context->initializationVector[context->bytesRemaining++] = *cipherText++;
    }

    return BLOCK_CIPHER_SW_ERROR_NONE;
}


