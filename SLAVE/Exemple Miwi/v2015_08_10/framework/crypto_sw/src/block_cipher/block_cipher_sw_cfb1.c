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

BLOCK_CIPHER_SW_ERRORS BLOCK_CIPHER_SW_CFB1_Initialize (BLOCK_CIPHER_SW_HANDLE handle, BLOCK_CIPHER_SW_CFB1_CONTEXT * context, BLOCK_CIPHER_SW_FunctionEncrypt encryptFunction, BLOCK_CIPHER_SW_FunctionDecrypt decryptFunction, uint32_t blockSize, uint8_t * initializationVector, void * key, CRYPTO_SW_KEY_TYPE keyType)
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

BLOCK_CIPHER_SW_ERRORS BLOCK_CIPHER_SW_CFB1_Encrypt (BLOCK_CIPHER_SW_HANDLE handle, uint8_t * cipherText, uint32_t * numCipherBytes, uint8_t * plainText, uint32_t numPlainBytes, uint32_t options)
{
    BLOCK_CIPHER_SW_CFB1_CONTEXT * context = (BLOCK_CIPHER_SW_CFB1_CONTEXT *)BLOCK_CIPHER_SW_HandleResolve(handle);
    uint8_t i, plainText_temp = 0;
    uint8_t __attribute__((aligned)) buffer[CRYPTO_CONFIG_SW_BLOCK_MAX_SIZE];
    uint32_t j;

    if (context == NULL)
    {
        return BLOCK_CIPHER_SW_ERROR_INVALID_HANDLE;
    }

    if (numCipherBytes != NULL)
    {
        *numCipherBytes = numPlainBytes;
    }

    j=0;

    while(numPlainBytes--)
    {
        if(j==8)
        {
            cipherText++;
            j = 0;
        }

        if(j++==0)
        {
            plainText_temp = *plainText++;
            *cipherText = 0;
        }

        (*context->encrypt)(handle, buffer, context->initializationVector, context->key);

        *cipherText >>= 1;

        if((plainText_temp & 0x01) != 0)
        {
            if((buffer[0] & 0x80) == 0)
            {
                *cipherText |= 0x80;
            }
        }
        else
        {
            if((buffer[0] & 0x80) != 0)
            {
                *cipherText |= 0x80;
            }
        }

        plainText_temp >>= 1;

        for(i=0;i<(context->blockSize - 1);i++)
        {
            context->initializationVector[i] <<= 1;
            if((context->initializationVector[i+1] & 0x80) == 0x80)
            {
                context->initializationVector[i] |= 0x01;
            }
        }

        context->initializationVector[context->blockSize - 1] <<=1;

        if((*cipherText & (0x80)) == 0x80)
        {
            context->initializationVector[context->blockSize - 1] |= 0x01;
        }
    }
    while(j++ != 8)
    {
        *cipherText >>= 1;
    }

    return BLOCK_CIPHER_SW_ERROR_NONE;
}

BLOCK_CIPHER_SW_ERRORS BLOCK_CIPHER_SW_CFB1_Decrypt (BLOCK_CIPHER_SW_HANDLE handle, uint8_t * plainText, uint32_t * numPlainBytes, uint8_t * cipherText, uint32_t numCipherBytes, uint32_t options)
{
    BLOCK_CIPHER_SW_CFB1_CONTEXT * context = (BLOCK_CIPHER_SW_CFB1_CONTEXT *)BLOCK_CIPHER_SW_HandleResolve(handle);
    uint8_t i,cipherText_temp = 0;
    uint8_t __attribute__((aligned)) buffer[CRYPTO_CONFIG_SW_BLOCK_MAX_SIZE];
    uint32_t j;

    if (context == NULL)
    {
        return BLOCK_CIPHER_SW_ERROR_INVALID_HANDLE;
    }

    if (numPlainBytes != NULL)
    {
        *numPlainBytes = numCipherBytes;
    }

    j=0;

    while(numCipherBytes--)
    {
        if(j==8)
        {
            plainText++;
            j = 0;
        }

        if(j++==0)
        {
            *plainText = 0;
            cipherText_temp = *cipherText++;
        }

        (*context->encrypt)(handle, buffer, context->initializationVector, context->key);

        *plainText >>= 1;

        if((cipherText_temp & 0x01) != 0)
        {
            if((buffer[0] & 0x80) == 0)
            {
                *plainText |= 0x80;
            }
        }
        else
        {
            if((buffer[0] & 0x80) != 0)
            {
                *plainText |= 0x80;
            }
        }

        for(i=0;i<(context->blockSize - 1);i++)
        {
            context->initializationVector[i] <<= 1;
            if((context->initializationVector[i+1] & 0x80) == 0x80)
            {
                context->initializationVector[i] |= 0x01;
            }
        }

        context->initializationVector[context->blockSize-1] <<=1;
        if((cipherText_temp & (0x01)) == 0x01)
        {
            context->initializationVector[context->blockSize-1] |= 0x01;
        }

        cipherText_temp >>= 1;
    }
    while(j++ != 8)
    {
        *plainText >>= 1;
    }

    return BLOCK_CIPHER_SW_ERROR_NONE;
}


