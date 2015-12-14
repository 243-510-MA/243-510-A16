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

#include <xc.h>
#include "crypto_16bv1/block_cipher_16bv1.h"
#include "crypto_16bv1/src/block_cipher/block_cipher_16bv1_private.h"
#include "system_config.h"
#include <string.h>

typedef enum
{
    BLOCK_CIPHER_OFB_STATE_IDLE = 0,
    BLOCK_CIPHER_OFB_STATE_INIT,
    BLOCK_CIPHER_OFB_STATE_PROCESS_DATA,
    BLOCK_CIPHER_OFB_STATE_ADD_DATA,
    BLOCK_CIPHER_OFB_STATE_WAIT_FOR_HW,
    BLOCK_CIPHER_OFB_STATE_GENERATE_KEYSTREAM,
    BLOCK_CIPHER_OFB_STATE_WAIT_FOR_KEYSTREAM
} BLOCK_CIPHER_OFB_STATE;

void BLOCK_CIPHER_OFB_16BV1_Tasks(BLOCK_CIPHER_HANDLE handle);

BLOCK_CIPHER_ERRORS BLOCK_CIPHER_16BV1_OFB_Initialize (BLOCK_CIPHER_HANDLE handle, BLOCK_CIPHER_16BV1_OFB_CONTEXT * context, BLOCK_CIPHER_FunctionEncrypt encryptFunction, BLOCK_CIPHER_FunctionDecrypt decryptFunction, uint32_t blockSize, uint8_t * initializationVector, void * keyStream, uint32_t keyStreamSize, void * key, CRYPTO_KEY_TYPE keyType, CRYPTO_KEY_MODE keyMode)
{
    BLOCK_CIPHER_OBJECT *pBlock = &blockHandles[handle];
    
    if (handle >= 0 && handle < CRYPTO_CONFIG_16BV1_BLOCK_HANDLE_MAXIMUM)
    {
        pBlock->context = context;
    }
    else
    {
        return BLOCK_CIPHER_ERROR_INVALID_HANDLE;
    }

    if (encryptFunction == CRYPTO_16BV1_ALGORITHM_AES)
    {
        pBlock->cryconl_context.CPHRSEL = 1;
        switch(keyMode)
        {
            case CRYPTO_AES_128_KEY:
                pBlock->keyMode = CRYPTO_AES_128_KEY;
                pBlock->cryconh_context.KEYMOD = 0;
                pBlock->keyLength = 16;
                break;
            case CRYPTO_AES_192_KEY:
                pBlock->keyMode = CRYPTO_AES_192_KEY;
                pBlock->cryconh_context.KEYMOD = 1;
                pBlock->keyLength = 24;
                break;
            case CRYPTO_AES_256_KEY:
                pBlock->keyMode = CRYPTO_AES_256_KEY;
                pBlock->cryconh_context.KEYMOD = 2;
                pBlock->keyLength = 32;
                break;
            default:
                pBlock->keyMode = CRYPTO_MODE_NONE;
                pBlock->error = BLOCK_CIPHER_ERROR_UNSUPPORTED_KEY_TYPE;
                return BLOCK_CIPHER_ERROR_UNSUPPORTED_KEY_TYPE;
                break;
        }
    }
    else if (encryptFunction == CRYPTO_16BV1_ALGORITHM_TDES)
    {
        pBlock->cryconl_context.CPHRSEL = 0;
        switch(keyMode)
        {
            case CRYPTO_64DES_1_KEY:
                pBlock->keyMode = CRYPTO_64DES_1_KEY;
                pBlock->cryconh_context.KEYMOD = 0;
                pBlock->keyLength = 8;
                break;
            case CRYPTO_64DES_2_KEY:
                pBlock->keyMode = CRYPTO_64DES_2_KEY;
                pBlock->cryconh_context.KEYMOD = 1;
                pBlock->keyLength = 16;
                break;
            case CRYPTO_64DES_3_KEY:
                pBlock->keyMode = CRYPTO_64DES_3_KEY;
                pBlock->cryconh_context.KEYMOD = 3;
                pBlock->keyLength = 24;
                break;
            default:
                pBlock->keyMode = CRYPTO_MODE_NONE;
                pBlock->error = BLOCK_CIPHER_ERROR_UNSUPPORTED_KEY_TYPE;
                return BLOCK_CIPHER_ERROR_UNSUPPORTED_KEY_TYPE;
                break;
        }
    }
    else
    {
        pBlock->error = BLOCK_CIPHER_ERROR_INVALID_FUNCTION;
        return BLOCK_CIPHER_ERROR_INVALID_FUNCTION;
    }

    if (keyType == CRYPTO_KEY_SOFTWARE)
    {
        pBlock->keyType = CRYPTO_KEY_SOFTWARE;
        //select CRYKEY
        pBlock->cryconh_context.KEYSRC = 0;
        pBlock->key = key;
    }
    else if (keyType == CRYPTO_KEY_HARDWARE_KEK)
    {
        pBlock->keyType = CRYPTO_KEY_HARDWARE_KEK;
        //this mode needs lots of special attention.
        pBlock->cryconh_context.KEYSRC = 0;
        pBlock->error = BLOCK_CIPHER_ERROR_UNSUPPORTED_KEY_TYPE;
        return BLOCK_CIPHER_ERROR_UNSUPPORTED_KEY_TYPE;
    }
    else if ((keyType >= CRYPTO_KEY_HARDWARE_OTP_1) && (keyType <= CRYPTO_KEY_HARDWARE_OTP_7))
    {
        pBlock->keyType = keyType;
        pBlock->cryconh_context.KEYSRC = keyType - CRYPTO_KEY_HARDWARE_OTP_OFFSET;
    }
    else
    {
        pBlock->keyType = CRYPTO_KEY_NONE;
        pBlock->error = BLOCK_CIPHER_ERROR_UNSUPPORTED_KEY_TYPE;
        return BLOCK_CIPHER_ERROR_UNSUPPORTED_KEY_TYPE;
    }

    pBlock->options = BLOCK_CIPHER_OPTION_STREAM_START;
    pBlock->blockSize = blockSize;
    pBlock->mode = BLOCK_CIPHER_MODE_OFB;
    pBlock->state = BLOCK_CIPHER_STATE_IDLE;
    context->state = BLOCK_CIPHER_OFB_STATE_IDLE;
    pBlock->error = BLOCK_CIPHER_ERROR_NONE;
    pBlock->cryconl_context.CPHRMOD = 3;   // OFB
    pBlock->cryconh_context.CTRSIZE = 0;
    memcpy (context->initializationVector, initializationVector, blockSize);
    context->keyStream = keyStream;
    context->keyStreamCurrentPosition = keyStream;
    context->keyStreamSize = keyStreamSize;
    context->bytesRemainingInKeyStream = 0;
    context->key = key;
    context->keyType = keyType;

    pBlock->tasks = BLOCK_CIPHER_OFB_16BV1_Tasks;
    
    pBlock->previousText = (uint8_t *)&CRYTXTB;
    pBlock->inputText = (uint8_t *)&CRYTXTA;
    pBlock->outputText = (uint8_t *)&CRYTXTC;

    BLOCK_CIPHER_HandleReInitialize (handle);

    return BLOCK_CIPHER_ERROR_NONE;
}

BLOCK_CIPHER_ERRORS BLOCK_CIPHER_16BV1_OFB_Encrypt (BLOCK_CIPHER_HANDLE handle, uint8_t * cipherText, uint32_t * numCipherBytes, uint8_t * plainText, uint32_t numPlainBytes, uint32_t options)
{
    BLOCK_CIPHER_OBJECT *pBlock = &blockHandles[handle];
    BLOCK_CIPHER_16BV1_OFB_CONTEXT * context = (BLOCK_CIPHER_16BV1_OFB_CONTEXT *)BLOCK_CIPHER_16BV1_HandleResolve(handle);

    if (context == NULL)
    {
        return BLOCK_CIPHER_ERROR_INVALID_HANDLE;
    }
    
    if (pBlock->state != BLOCK_CIPHER_STATE_IDLE)
    {
        return BLOCK_CIPHER_ERROR_BUSY;
    }

    pBlock->dest = cipherText;
    pBlock->source = plainText;
    pBlock->inCount = numPlainBytes;
    pBlock->options = options;
    pBlock->outCount = numCipherBytes;
    if(pBlock->outCount != NULL)
    {
        *pBlock->outCount = 0;
    }

    if (BLOCK_CIPHER_CurrentHandleIsInitialized())
    {
        context->state = BLOCK_CIPHER_OFB_STATE_PROCESS_DATA;
    }
    else
    {
        context->state = BLOCK_CIPHER_OFB_STATE_INIT;
        pBlock->operation = BLOCK_CIPHER_OPERATION_ENCRYPT;
    }
    pBlock->error = BLOCK_CIPHER_ERROR_NONE;

    pBlock->state = BLOCK_CIPHER_STATE_BUSY;

    return BLOCK_CIPHER_ERROR_NONE;
}

void BLOCK_CIPHER_OFB_16BV1_Tasks(BLOCK_CIPHER_HANDLE handle)
{
    BLOCK_CIPHER_OBJECT *pBlock = &blockHandles[handle];
    uint32_t blockSize = pBlock->blockSize;
    uint32_t dataCount;
    BLOCK_CIPHER_16BV1_OFB_CONTEXT * context = (BLOCK_CIPHER_16BV1_OFB_CONTEXT *)BLOCK_CIPHER_16BV1_HandleResolve(handle);

    if (context == NULL)
    {
        pBlock->error = BLOCK_CIPHER_ERROR_INVALID_HANDLE;
        pBlock->state = BLOCK_CIPHER_STATE_ERROR;
        context->state = BLOCK_CIPHER_OFB_STATE_IDLE;
        return;
    }

    do
    {
        switch (context->state)
        {
            case BLOCK_CIPHER_OFB_STATE_INIT:
                // Load Context into SFRs
                CRYCONHbits.CTRSIZE = pBlock->cryconh_context.CTRSIZE;
                CRYCONHbits.KEYMOD = pBlock->cryconh_context.KEYMOD;
                CRYCONHbits.KEYSRC = pBlock->cryconh_context.KEYSRC;
                CRYCONLbits.CPHRSEL = pBlock->cryconl_context.CPHRSEL;
                CRYCONLbits.CPHRMOD = pBlock->cryconl_context.CPHRMOD;

                if(pBlock->keyType == CRYPTO_KEY_SOFTWARE)
                {
                    memcpy((void *)&CRYKEY, pBlock->key, pBlock->keyLength);
                }
                else if((pBlock->keyType == CRYPTO_KEY_NONE)||(pBlock->keyType == CRYPTO_KEY_SOFTWARE_EXPANDED)||(pBlock->keyType == CRYPTO_KEY_HARDWARE_KEK))
                {
                    pBlock->state = BLOCK_CIPHER_STATE_ERROR;
                    context->state = BLOCK_CIPHER_OFB_STATE_IDLE;
                    return;
                }
                CRYCONLbits.OPMOD = 0;
                if (pBlock->operation == BLOCK_CIPHER_OPERATION_ENCRYPT)
                {
                    context->state = BLOCK_CIPHER_OFB_STATE_PROCESS_DATA;
                }
                else if (pBlock->operation == BLOCK_CIPHER_OPERATION_GENERATE_KEYSTREAM)
                {
                    context->state = BLOCK_CIPHER_OFB_STATE_GENERATE_KEYSTREAM;
                }

                BLOCK_CIPHER_CurrentHandleInitialize();
                memcpy (pBlock->previousText, context->initializationVector, pBlock->blockSize);//check
                break;
            case BLOCK_CIPHER_OFB_STATE_PROCESS_DATA:
                {
                    uint8_t * output;
                    uint8_t * input;

                    dataCount = context->bytesRemainingInKeyStream;
                    if (pBlock->inCount < dataCount)
                    {
                        dataCount = pBlock->inCount;
                    }

                    pBlock->inCount -= dataCount;
                    if(pBlock->outCount!=NULL)
                    {
                        *pBlock->outCount += dataCount;
                    }
                    context->bytesRemainingInKeyStream -= dataCount;
                    input = pBlock->source;
                    output = pBlock->dest;
                    pBlock->source += dataCount;
                    pBlock->dest += dataCount;

                    while (dataCount-- != 0)
                    {
                        *output++ = *input++ ^ *(uint8_t*)context->keyStreamCurrentPosition++;

                        if(context->keyStreamCurrentPosition == ((uint8_t*)context->keyStream + context->keyStreamSize))
                        {
                            context->keyStreamCurrentPosition = context->keyStream;
                        }
                    }

                    if (pBlock->inCount != 0)
                    {
                        context->state = BLOCK_CIPHER_OFB_STATE_ADD_DATA;
                    }
                    else
                    {
                        memcpy (context->initializationVector, pBlock->previousText, pBlock->blockSize);//check
                        pBlock->state = BLOCK_CIPHER_STATE_IDLE;
                        context->state = BLOCK_CIPHER_OFB_STATE_IDLE;
                    }
                }
                break;
            case BLOCK_CIPHER_OFB_STATE_ADD_DATA:
                dataCount = blockSize;
                if (pBlock->inCount < blockSize)
                {
                    dataCount = pBlock->inCount;
                }
                memcpy (pBlock->inputText, pBlock->source, dataCount);
                memset (pBlock->inputText + dataCount, 0x00, blockSize - dataCount);
                pBlock->source += dataCount;
                CRYCONLbits.CRYGO = 1;
                context->state = BLOCK_CIPHER_OFB_STATE_WAIT_FOR_HW;
                break;
            case BLOCK_CIPHER_OFB_STATE_WAIT_FOR_HW:
                dataCount = blockSize;
                if (pBlock->inCount < blockSize)
                {
                    dataCount = pBlock->inCount;
                }
                memcpy (pBlock->dest, pBlock->outputText, dataCount);
                pBlock->dest += dataCount;
                if(pBlock->outCount!=NULL)
                {
                    *pBlock->outCount += dataCount;
                }
                pBlock->inCount -= dataCount;

                if (pBlock->inCount != 0)
                {
                    context->state = BLOCK_CIPHER_OFB_STATE_ADD_DATA;
                }
                else
                {
                    if (dataCount != pBlock->blockSize)
                    {
                        // Note: If we're in this state, all of the data that used to be in the key stream must have been used
                        memcpy (context->keyStream, pBlock->outputText + dataCount, pBlock->blockSize - dataCount);
                        context->keyStreamCurrentPosition = context->keyStream;
                        context->bytesRemainingInKeyStream = pBlock->blockSize - dataCount;
                    }
                    memcpy (context->initializationVector, pBlock->previousText, blockSize);// check
                    pBlock->state = BLOCK_CIPHER_STATE_IDLE;
                    context->state = BLOCK_CIPHER_OFB_STATE_IDLE;
                }
                break;
            case BLOCK_CIPHER_OFB_STATE_GENERATE_KEYSTREAM:
                if ((context->keyStreamSize - context->bytesRemainingInKeyStream) < (blockSize * pBlock->inCount))
                {
                    pBlock->inCount = (context->keyStreamSize - context->bytesRemainingInKeyStream) / blockSize;
                }
                memset (pBlock->inputText, 0x00, blockSize);
                CRYCONLbits.CRYGO = 1;
                context->state = BLOCK_CIPHER_OFB_STATE_WAIT_FOR_KEYSTREAM;
                break;
            case BLOCK_CIPHER_OFB_STATE_WAIT_FOR_KEYSTREAM:
                dataCount = ((uint8_t *)context->keyStream + context->keyStreamSize) - ((uint8_t *)context->keyStreamCurrentPosition + context->bytesRemainingInKeyStream);
                if (dataCount > blockSize)
                {
                    dataCount = blockSize;
                }
                memcpy (context->keyStreamCurrentPosition + context->bytesRemainingInKeyStream, pBlock->outputText, dataCount);
                memcpy (context->keyStream, pBlock->outputText + dataCount, pBlock->blockSize - dataCount);
                context->bytesRemainingInKeyStream += blockSize;
                pBlock->inCount -= 1;
                if (pBlock->inCount == 0)
                {
                    memcpy (context->initializationVector, pBlock->previousText, pBlock->blockSize);
                    pBlock->state = BLOCK_CIPHER_STATE_IDLE;
                    context->state = BLOCK_CIPHER_OFB_STATE_IDLE;
                }
                else
                {
                    context->state = BLOCK_CIPHER_OFB_STATE_GENERATE_KEYSTREAM;
                }
                break;
            default:
                pBlock->state = BLOCK_CIPHER_STATE_ERROR;
                context->state = BLOCK_CIPHER_OFB_STATE_IDLE;
                break;
        }
    } while ((context->state != BLOCK_CIPHER_OFB_STATE_WAIT_FOR_HW) && (context->state != BLOCK_CIPHER_OFB_STATE_IDLE) && (context->state != BLOCK_CIPHER_OFB_STATE_WAIT_FOR_KEYSTREAM));

    return;
}

BLOCK_CIPHER_ERRORS BLOCK_CIPHER_16BV1_OFB_KeyStreamGenerate (BLOCK_CIPHER_HANDLE handle, uint32_t numBlocks, uint32_t options)
{
    BLOCK_CIPHER_OBJECT *pBlock = &blockHandles[handle];
    BLOCK_CIPHER_16BV1_OFB_CONTEXT * context = (BLOCK_CIPHER_16BV1_OFB_CONTEXT *)BLOCK_CIPHER_16BV1_HandleResolve(handle);

    if (context == NULL)
    {
        return BLOCK_CIPHER_ERROR_INVALID_HANDLE;
    }

    if (pBlock->state != BLOCK_CIPHER_STATE_IDLE)
    {
        return BLOCK_CIPHER_ERROR_BUSY;
    }

    pBlock->dest = NULL;
    pBlock->source = NULL;
    pBlock->inCount = numBlocks;
    pBlock->options = options;
    pBlock->outCount = NULL;
    pBlock->state = BLOCK_CIPHER_STATE_BUSY;
    if (BLOCK_CIPHER_CurrentHandleIsInitialized())
    {
        context->state = BLOCK_CIPHER_OFB_STATE_GENERATE_KEYSTREAM;
    }
    else
    {
        context->state = BLOCK_CIPHER_OFB_STATE_INIT;
        pBlock->operation = BLOCK_CIPHER_OPERATION_GENERATE_KEYSTREAM;
    }

    return BLOCK_CIPHER_ERROR_NONE;
}



