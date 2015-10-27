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
    BLOCK_CIPHER_ECB_STATE_IDLE = 0,
    BLOCK_CIPHER_ECB_STATE_INIT,
    BLOCK_CIPHER_ECB_STATE_KEY_DECRYPT,
    BLOCK_CIPHER_ECB_STATE_PROCESS_DATA,
    BLOCK_CIPHER_ECB_STATE_ADD_DATA,
    BLOCK_CIPHER_ECB_STATE_WAIT_FOR_HW,
} BLOCK_CIPHER_ECB_STATE;

void BLOCK_CIPHER_ECB_16BV1_Tasks (BLOCK_CIPHER_HANDLE handle);

BLOCK_CIPHER_ERRORS BLOCK_CIPHER_16BV1_ECB_Initialize (BLOCK_CIPHER_HANDLE handle, BLOCK_CIPHER_16BV1_ECB_CONTEXT * context, BLOCK_CIPHER_FunctionEncrypt encryptFunction, BLOCK_CIPHER_FunctionDecrypt decryptFunction, uint32_t blockSize, void * key, CRYPTO_KEY_TYPE keyType, CRYPTO_KEY_MODE keyMode)
{
    BLOCK_CIPHER_OBJECT *pBlock = &blockHandles[handle];
    uint16_t i = 0;

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
    pBlock->mode = BLOCK_CIPHER_MODE_ECB;
    pBlock->state = BLOCK_CIPHER_STATE_IDLE;
    pBlock->error = BLOCK_CIPHER_ERROR_NONE;
    pBlock->cryconl_context.CPHRMOD = 0;//ECB
    pBlock->cryconh_context.CTRSIZE = 0;//none
    for(i=0;i<CRYPTO_CONFIG_16BV1_BLOCK_MAX_SIZE;i++)
    {
        pBlock->previousData[i] = 0;
    }
    pBlock->previousText = NULL;
    pBlock->blockSize = blockSize;

    pBlock->tasks = BLOCK_CIPHER_ECB_16BV1_Tasks;

    BLOCK_CIPHER_HandleReInitialize (handle);

    return BLOCK_CIPHER_ERROR_NONE;
}

BLOCK_CIPHER_ERRORS BLOCK_CIPHER_16BV1_ECB_Encrypt (BLOCK_CIPHER_HANDLE handle, uint8_t * cipherText, uint32_t * numCipherBytes, uint8_t * plainText, uint32_t numPlainBytes, uint32_t options)
{
    BLOCK_CIPHER_OBJECT *pBlock = &blockHandles[handle];
    BLOCK_CIPHER_16BV1_ECB_CONTEXT * context = (BLOCK_CIPHER_16BV1_ECB_CONTEXT *)BLOCK_CIPHER_16BV1_HandleResolve(handle);

    //Check that Context has been declared properly
    if (context == NULL)
    {
        return BLOCK_CIPHER_ERROR_INVALID_HANDLE;
    }

    //Check that thread is available for an operation
    if (pBlock->state != BLOCK_CIPHER_STATE_IDLE)
    {
        return BLOCK_CIPHER_ERROR_BUSY;
    }

    //Check for start of stream for first encrypt
    if (options & BLOCK_CIPHER_OPTION_STREAM_START)
    {
        pBlock->bytesRemaining = 0;
    }

    if (numPlainBytes == 0)
    {
        return BLOCK_CIPHER_ERROR_NONE;
    }

    pBlock->source = plainText;
    pBlock->dest = cipherText;
    *numCipherBytes = 0;
    pBlock->outCount = numCipherBytes;
    pBlock->inCount = numPlainBytes;
    pBlock->options = options;
    pBlock->error = BLOCK_CIPHER_ERROR_NONE;
    pBlock->inputText = (uint8_t *)&CRYTXTA;
    pBlock->outputText = (uint8_t *)&CRYTXTB;

    if (BLOCK_CIPHER_CurrentHandleIsInitialized() && (pBlock->operation == BLOCK_CIPHER_OPERATION_ENCRYPT))
    {
        context->state = BLOCK_CIPHER_ECB_STATE_ADD_DATA;
    }
    else
    {
        context->state = BLOCK_CIPHER_ECB_STATE_INIT;
        pBlock->operation = BLOCK_CIPHER_OPERATION_ENCRYPT;
    }
    pBlock->state = BLOCK_CIPHER_STATE_BUSY;

    return BLOCK_CIPHER_ERROR_NONE;
}

BLOCK_CIPHER_ERRORS BLOCK_CIPHER_16BV1_ECB_Decrypt (BLOCK_CIPHER_HANDLE handle, uint8_t * plainText, uint32_t * numPlainBytes, uint8_t * cipherText, uint32_t numCipherBytes, uint32_t options)
{
    BLOCK_CIPHER_OBJECT *pBlock = &blockHandles[handle];
    BLOCK_CIPHER_16BV1_ECB_CONTEXT * context = (BLOCK_CIPHER_16BV1_ECB_CONTEXT *)BLOCK_CIPHER_16BV1_HandleResolve(handle);

    //Check that Context has been declared properly
    if (context == NULL)
    {
        return BLOCK_CIPHER_ERROR_INVALID_HANDLE;
    }

    //Check that thread is available for an operation
    if (pBlock->state != BLOCK_CIPHER_STATE_IDLE)
    {
        return BLOCK_CIPHER_ERROR_BUSY;
    }

    if (numCipherBytes == 0)
    {
        return BLOCK_CIPHER_ERROR_NONE;
    }

    //Check for start of stream for first encrypt
    if(options & BLOCK_CIPHER_OPTION_STREAM_START)
    {
        pBlock->bytesRemaining = 0;
    }

    pBlock->source = cipherText;
    pBlock->dest = plainText;
    *numPlainBytes = 0;
    pBlock->outCount = numPlainBytes;
    pBlock->inCount = numCipherBytes;
    pBlock->options = options;
    pBlock->error = BLOCK_CIPHER_ERROR_NONE;
    pBlock->inputText = (uint8_t *)&CRYTXTA;
    pBlock->outputText = (uint8_t *)&CRYTXTB;
    if (BLOCK_CIPHER_CurrentHandleIsInitialized() && (pBlock->operation == BLOCK_CIPHER_OPERATION_DECRYPT))
    {
        context->state = BLOCK_CIPHER_ECB_STATE_ADD_DATA;
    }
    else
    {
        context->state = BLOCK_CIPHER_ECB_STATE_INIT;
        pBlock->operation = BLOCK_CIPHER_OPERATION_DECRYPT;
    }
    pBlock->state = BLOCK_CIPHER_STATE_BUSY;

    return BLOCK_CIPHER_ERROR_NONE;
}

void BLOCK_CIPHER_ECB_16BV1_Tasks (BLOCK_CIPHER_HANDLE handle)
{
    BLOCK_CIPHER_OBJECT *pBlock = &blockHandles[handle];
    uint32_t blockSize = pBlock->blockSize;
    uint32_t dataCount = blockSize - pBlock->bytesRemaining;
    BLOCK_CIPHER_16BV1_ECB_CONTEXT * context = (BLOCK_CIPHER_16BV1_ECB_CONTEXT *)BLOCK_CIPHER_16BV1_HandleResolve(handle);

    if (context == NULL)
    {
        pBlock->error = BLOCK_CIPHER_ERROR_INVALID_HANDLE;
        pBlock->state = BLOCK_CIPHER_STATE_ERROR;
        context->state = BLOCK_CIPHER_ECB_STATE_IDLE;
        return;
    }

    do
    {
        switch (context->state)
        {
            case BLOCK_CIPHER_ECB_STATE_INIT:
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
                    context->state = BLOCK_CIPHER_ECB_STATE_IDLE;
                    return;
                }

                if (pBlock->operation == BLOCK_CIPHER_OPERATION_DECRYPT)
                {
                    // Must perform AES Decryption Key Expansion
                    if (pBlock->cryconl_context.CPHRSEL == 1)
                    {
                        CRYCONHbits.SKEYSEL = 0;
                        CRYCONLbits.OPMOD = 2;
                        context->state = BLOCK_CIPHER_ECB_STATE_KEY_DECRYPT;
                        CRYCONLbits.CRYGO = 1;
                    }
                    else
                    {
                        CRYCONLbits.OPMOD = 1;
                        context->state = BLOCK_CIPHER_ECB_STATE_ADD_DATA;
                    }
                }
                else
                {
                    CRYCONLbits.OPMOD = 0;
                    context->state = BLOCK_CIPHER_ECB_STATE_ADD_DATA;
                }
                
                BLOCK_CIPHER_CurrentHandleInitialize();
                break;
            case BLOCK_CIPHER_ECB_STATE_KEY_DECRYPT:
                // Key decryption complete
                CRYCONLbits.OPMOD = 1;
                context->state = BLOCK_CIPHER_ECB_STATE_ADD_DATA;
                break;
            case BLOCK_CIPHER_ECB_STATE_ADD_DATA:
                if (pBlock->inCount == 0)
                {
                    // Idle state if no more input data is reamining
                    pBlock->state = BLOCK_CIPHER_STATE_IDLE;
                    context->state = BLOCK_CIPHER_ECB_STATE_IDLE;
                    return;
                }
                else
                {
                    if (pBlock->inCount < blockSize - pBlock->bytesRemaining)
                    {
                        // Copy remaining data to the remainingData buffer and set the state to idle if we don't have a full block
                        memcpy (pBlock->remainingData + pBlock->bytesRemaining, pBlock->source, pBlock->inCount);
                        pBlock->bytesRemaining += pBlock->inCount;
                        pBlock->source += pBlock->inCount;
                        pBlock->inCount = 0;
                        dataCount = 0;

                        // Check if a padding option has been specified and the stream is complete
                        if ((pBlock->options & BLOCK_CIPHER_OPTION_STREAM_COMPLETE) == BLOCK_CIPHER_OPTION_STREAM_COMPLETE)
                        {
                            // Pad data as the user has specified and indicate that the main loop should process the data
                            BLOCK_CIPHER_16BV1_PaddingInsert (pBlock->remainingData, pBlock->blockSize, pBlock->blockSize - pBlock->bytesRemaining, pBlock->options);
                            pBlock->bytesRemaining = pBlock->blockSize;
                            context->state = BLOCK_CIPHER_ECB_STATE_PROCESS_DATA;
                        }
                        else
                        {
                            // The stream is not complete; set the state to idle until the user provides more data
                            pBlock->state = BLOCK_CIPHER_STATE_IDLE;
                            context->state = BLOCK_CIPHER_ECB_STATE_IDLE;
                            return;
                        }
                    }
                    else
                    {
                        // One or more blocks of input data are available
                        // Indicate that the hardware should process it when possible
                        context->state = BLOCK_CIPHER_ECB_STATE_PROCESS_DATA;
                    }
                }

                break;
            case BLOCK_CIPHER_ECB_STATE_PROCESS_DATA:
                // Copy the data into the buffer
                memcpy (pBlock->inputText, pBlock->remainingData, pBlock->bytesRemaining);
                memcpy (pBlock->inputText + pBlock->bytesRemaining, pBlock->source, dataCount);
                pBlock->bytesRemaining = 0;

                pBlock->source += dataCount;
                pBlock->inCount -= dataCount;

                CRYCONLbits.CRYGO = 1;
                context->state = BLOCK_CIPHER_ECB_STATE_WAIT_FOR_HW;
                break;
            case BLOCK_CIPHER_ECB_STATE_WAIT_FOR_HW:
                memcpy (pBlock->dest, pBlock->outputText, blockSize);
                pBlock->dest += blockSize;
                if(pBlock->outCount != NULL)
                {
                    *pBlock->outCount += blockSize;
                }
                context->state = BLOCK_CIPHER_ECB_STATE_ADD_DATA;
                break;
            default:
                pBlock->state = BLOCK_CIPHER_STATE_ERROR;
                context->state = BLOCK_CIPHER_ECB_STATE_IDLE;
                break;
        }
    } while ((context->state != BLOCK_CIPHER_ECB_STATE_WAIT_FOR_HW) && (context->state != BLOCK_CIPHER_ECB_STATE_IDLE) && (context->state != BLOCK_CIPHER_ECB_STATE_KEY_DECRYPT));

    return;
}

