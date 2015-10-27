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

BLOCK_CIPHER_SW_ERRORS BLOCK_CIPHER_SW_CTR_Initialize (BLOCK_CIPHER_SW_HANDLE handle, BLOCK_CIPHER_SW_CTR_CONTEXT * context, BLOCK_CIPHER_SW_FunctionEncrypt encryptFunction, BLOCK_CIPHER_SW_FunctionDecrypt decryptFunction, uint32_t blockSize, uint8_t * noncePlusCounter, void * keyStream, uint32_t keyStreamSize, void * key, CRYPTO_SW_KEY_TYPE keyType)
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
    memcpy (context->noncePlusCounter, noncePlusCounter, blockSize);
    context->keyStream = keyStream;
    context->keyStreamSize = keyStreamSize;
    context->key = key;
    context->keyType = keyType;

    return BLOCK_CIPHER_SW_ERROR_NONE;
}

BLOCK_CIPHER_SW_ERRORS BLOCK_CIPHER_SW_CTR_Encrypt (BLOCK_CIPHER_SW_HANDLE handle, uint8_t * cipherText, uint32_t * numCipherBytes, uint8_t * plainText, uint32_t numPlainBytes, uint32_t options)
{
    BLOCK_CIPHER_SW_ERRORS status;
    BLOCK_CIPHER_SW_CTR_CONTEXT * context = (BLOCK_CIPHER_SW_CTR_CONTEXT *)BLOCK_CIPHER_SW_HandleResolve(handle);

    if (context == NULL)
    {
        return BLOCK_CIPHER_SW_ERROR_INVALID_HANDLE;
    }
    
    //If the user has specified that they want to create a new stream,
    //  then create the stream for them
    if((options & BLOCK_CIPHER_SW_OPTION_STREAM_START) == BLOCK_CIPHER_SW_OPTION_STREAM_START)
    {
        status = BLOCK_CIPHER_SW_CTR_KeyStreamGenerate   (handle, 1, options);
        if(status != BLOCK_CIPHER_SW_ERROR_NONE)
        {
            return status;
        }
    }

    if (numCipherBytes != NULL)
    {
        *numCipherBytes = numPlainBytes;
    }

    while(numPlainBytes--)
    {
        if(context->bytesRemainingInKeyStream == 0)
        {
            status = BLOCK_CIPHER_SW_CTR_KeyStreamGenerate   (handle, 1, BLOCK_CIPHER_SW_OPTION_STREAM_CONTINUE);
            if(status != BLOCK_CIPHER_SW_ERROR_NONE)
            {
                *numCipherBytes -= numPlainBytes;
                return status;
            }
        }

        context->bytesRemainingInKeyStream--;
        *cipherText++ = *plainText++ ^ *(uint8_t*)context->keyStreamCurrentPosition++;

        if(context->keyStreamCurrentPosition == ((uint8_t*)context->keyStream + context->keyStreamSize))
        {
            context->keyStreamCurrentPosition = context->keyStream;
        }
    }

    return BLOCK_CIPHER_SW_ERROR_NONE;
}

BLOCK_CIPHER_SW_ERRORS BLOCK_CIPHER_SW_CTR_KeyStreamGenerate (BLOCK_CIPHER_SW_HANDLE handle, uint32_t numBlocks, uint32_t options)
{
    uint8_t i,ctr_size;
    uint8_t* ptr;
    uint8_t __attribute__((aligned)) buffer[CRYPTO_CONFIG_SW_BLOCK_MAX_SIZE];
    BLOCK_CIPHER_SW_CTR_CONTEXT * context = (BLOCK_CIPHER_SW_CTR_CONTEXT *)BLOCK_CIPHER_SW_HandleResolve(handle);

    if (context == NULL)
    {
        return BLOCK_CIPHER_SW_ERROR_INVALID_HANDLE;
    }

    if((options & BLOCK_CIPHER_SW_OPTION_STREAM_START) == BLOCK_CIPHER_SW_OPTION_STREAM_START)
    {
        context->keyStreamCurrentPosition = context->keyStream;
        context->bytesRemainingInKeyStream = 0;
        memcpy(context->counter,context->noncePlusCounter,context->blockSize);
    }
    else
    {
        if(memcmp(context->counter,context->noncePlusCounter,context->blockSize) == 0)
        {
            return BLOCK_CIPHER_SW_ERROR_CTR_COUNTER_EXPIRED;
        }
    }

    ptr = context->keyStreamCurrentPosition + context->bytesRemainingInKeyStream;

    while(numBlocks--)
    {
        //If there is enough room in the buffer for one more block of key
        //  data, then let's generate another block of key stream
        if((context->keyStreamSize - context->bytesRemainingInKeyStream) >= context->blockSize)
        {
            (*context->encrypt)(handle, buffer, context->counter, context->key);

            for(i=0;i<context->blockSize;i++)
            {
                if(ptr >= ((uint8_t*)context->keyStream + context->keyStreamSize))
                {
                    ptr = context->keyStream;
                }

                *ptr++ = buffer[i];
            }

            context->bytesRemainingInKeyStream += context->blockSize;

            ctr_size = (options - 1) & BLOCK_CIPHER_SW_OPTION_CTR_SIZE_MASK;

            do
            {
                if(++context->counter[ctr_size] != 0)
                {
                    break;
                }
            }while(ctr_size--);
        }
        else
        {
            return BLOCK_CIPHER_SW_ERROR_KEY_STREAM_GEN_OUT_OF_SPACE;
        }
    }

    return BLOCK_CIPHER_SW_ERROR_NONE;
}



