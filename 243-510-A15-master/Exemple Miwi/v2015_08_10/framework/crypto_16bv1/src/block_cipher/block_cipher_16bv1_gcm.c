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
    BLOCK_CIPHER_GCM_STATE_IDLE = 0,
    BLOCK_CIPHER_GCM_STATE_INIT_SUBKEY,
    BLOCK_CIPHER_GCM_STATE_INIT_SUBKEY_FINISH,
    BLOCK_CIPHER_GCM_STATE_INIT,
    BLOCK_CIPHER_GCM_STATE_PROCESS_DATA,
    BLOCK_CIPHER_GCM_STATE_ADD_DATA,
    BLOCK_CIPHER_GCM_STATE_WAIT_FOR_HW,
    BLOCK_CIPHER_GCM_STATE_GENERATE_KEYSTREAM,
    BLOCK_CIPHER_GCM_STATE_WAIT_FOR_KEYSTREAM,
    BLOCK_CIPHER_GCM_STATE_GENERATE_TAG,
    BLOCK_CIPHER_GCM_STATE_GENERATE_TAG_FINISH
} BLOCK_CIPHER_GCM_STATE;

void BLOCK_CIPHER_GCM_GaloisMultiply (uint8_t * result, uint8_t * a, uint8_t * b);
void BLOCK_CIPHER_GCM_GHash (BLOCK_CIPHER_16BV1_GCM_CONTEXT * context);
void BLOCK_CIPHER_16BV1_GCM_Tasks (BLOCK_CIPHER_HANDLE handle);

typedef union {
  uint32_t ui32;
  uint8_t  ui8_arr[4];
} ff_int32_t;

BLOCK_CIPHER_ERRORS BLOCK_CIPHER_16BV1_GCM_Initialize (BLOCK_CIPHER_HANDLE handle, BLOCK_CIPHER_16BV1_GCM_CONTEXT * context, BLOCK_CIPHER_FunctionEncrypt encryptFunction, BLOCK_CIPHER_FunctionDecrypt decryptFunction, uint32_t blockSize, uint8_t * initializationVector, uint32_t initializationVectorLen, void * keyStream, uint32_t keyStreamSize, void * key, CRYPTO_KEY_TYPE keyType, CRYPTO_KEY_MODE keyMode)
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
    pBlock->error = BLOCK_CIPHER_ERROR_NONE;
    pBlock->cryconl_context.CPHRMOD = 4;       // ECB
    pBlock->cryconh_context.CTRSIZE = 31;
    context->keyStream = keyStream;
    context->keyStreamSize = keyStreamSize;
    context->flags.authCompleted = false;
    context->authDataLen = 0;
    context->cipherTextLen = 0;
    context->key = key;
    context->keyType = keyType;
    context->blockSize = blockSize;

    // Zero the hash subkey field
    memset (context->hashSubKey, 0x00, blockSize);
    // Zero the initial authentication tag
    memset (context->authTag, 0x00, blockSize);

    // We can't calculate the initialization vector until the hash subkey has been calculated
    // Store a pointer to it in the finalTag pointer until it's possible to calculate it
    context->finalTagPointer = initializationVector;
    context->finalTagLen = initializationVectorLen;
    
    // Reset the key stream pointer
    context->keyStreamCurrentPosition = context->keyStream;
    context->bytesRemainingInKeyStream = 0;

    pBlock->mode = BLOCK_CIPHER_MODE_GCM;
    pBlock->state = BLOCK_CIPHER_STATE_BUSY;
    context->state = BLOCK_CIPHER_GCM_STATE_INIT_SUBKEY;

    pBlock->previousText = NULL;
    pBlock->inputText = (uint8_t *)&CRYTXTA;
    pBlock->outputText = (uint8_t *)&CRYTXTB;
    
    pBlock->tasks = BLOCK_CIPHER_16BV1_GCM_Tasks;

    BLOCK_CIPHER_HandleReInitialize (handle);

    return BLOCK_CIPHER_ERROR_NONE;
}

void BLOCK_CIPHER_16BV1_GCM_Tasks (BLOCK_CIPHER_HANDLE handle)
{
    BLOCK_CIPHER_OBJECT *pBlock = &blockHandles[handle];
    uint32_t blockSize = pBlock->blockSize;
    uint32_t dataCount, i;
    BLOCK_CIPHER_16BV1_GCM_CONTEXT * context = (BLOCK_CIPHER_16BV1_GCM_CONTEXT *)BLOCK_CIPHER_16BV1_HandleResolve(handle);

    if (context == NULL)
    {
        pBlock->error = BLOCK_CIPHER_ERROR_INVALID_HANDLE;
        pBlock->state = BLOCK_CIPHER_STATE_ERROR;
        context->state = BLOCK_CIPHER_GCM_STATE_IDLE;
        return;
    }

    do
    {
        switch (context->state)
        {
            case BLOCK_CIPHER_GCM_STATE_INIT_SUBKEY:
                // Load Context into SFRs
                CRYCONHbits.CTRSIZE = 0;
                CRYCONHbits.KEYMOD = pBlock->cryconh_context.KEYMOD;
                CRYCONHbits.KEYSRC = pBlock->cryconh_context.KEYSRC;
                CRYCONLbits.CPHRSEL = pBlock->cryconl_context.CPHRSEL;
                CRYCONLbits.CPHRMOD = 0;

                if(pBlock->keyType == CRYPTO_KEY_SOFTWARE)
                {
                    memcpy((void *)&CRYKEY, pBlock->key, pBlock->keyLength);
                }
                else if((pBlock->keyType == CRYPTO_KEY_NONE)||(pBlock->keyType == CRYPTO_KEY_SOFTWARE_EXPANDED)||(pBlock->keyType == CRYPTO_KEY_HARDWARE_KEK))
                {
                    pBlock->state = BLOCK_CIPHER_STATE_ERROR;
                    context->state = BLOCK_CIPHER_GCM_STATE_IDLE;
                    return;
                }
                CRYCONLbits.OPMOD = 0;
                memset (pBlock->inputText, 0x00, pBlock->blockSize);
                CRYCONLbits.CRYGO = 1;
                context->state = BLOCK_CIPHER_GCM_STATE_INIT_SUBKEY_FINISH;
                break;
            case BLOCK_CIPHER_GCM_STATE_INIT_SUBKEY_FINISH:
                // Get hash subkey from ECB
                memcpy (context->hashSubKey, pBlock->outputText, pBlock->blockSize);
                pBlock->previousText = (uint8_t *)&CRYTXTB;
                pBlock->inputText = (uint8_t *)&CRYTXTA;
                pBlock->outputText = (uint8_t *)&CRYTXTC;

                // Now that the hash subkey exists, calculate the initialization vector
                if (context->finalTagLen == 12)
                {
                    // Set up the initialization vector from the vector passed in by the user
                    memcpy (context->initializationVector, context->finalTagPointer, blockSize - 4);
                    memset (context->initializationVector + blockSize - 4, 0x00, 3);
                    *(context->initializationVector + blockSize - 1) = 0x01;
                }
                else
                {
                    // Calculate an initialization vector from the user-specified value
                    uint32_t tempIVLen = context->finalTagLen;
                    while (tempIVLen > blockSize)
                    {
                        memcpy (context->authBuffer, context->finalTagPointer, blockSize);
                        context->authBufferLen = blockSize;
                        context->finalTagPointer += blockSize;
                        tempIVLen -= blockSize;
                        BLOCK_CIPHER_GCM_GHash (context);
                    }
                    context->authBufferLen = blockSize;
                    // If the IV isn't an even multiple of the block size, pad it with zeros and hash the rest of it
                    if (tempIVLen != 0)
                    {
                        memcpy (context->authBuffer, context->finalTagPointer, tempIVLen);
                        memset (context->authBuffer + tempIVLen, 0x00, 16 - tempIVLen);
                        BLOCK_CIPHER_GCM_GHash(context);
                        context->authBufferLen = blockSize;
                        tempIVLen = 0;
                    }
                    // Hash the length of the IV to create the final initialization vector
                    while (tempIVLen < 11)
                    {
                        context->authBuffer[tempIVLen++] = 0;
                    }
                    context->authBuffer[15] = (uint8_t)((context->finalTagLen << 3) & 0xFF);
                    context->authBuffer[14] = (uint8_t)((context->finalTagLen >> 5) & 0xFF);
                    context->authBuffer[13] = (uint8_t)((context->finalTagLen >> 13) & 0xFF);
                    context->authBuffer[12] = (uint8_t)((context->finalTagLen >> 21) & 0xFF);
                    context->authBuffer[11] = (uint8_t)((context->finalTagLen >> 29) & 0xFF);

                    BLOCK_CIPHER_GCM_GHash (context);

                    memcpy (context->initializationVector, context->authTag, blockSize);
                }
                
                memcpy (context->counter, context->initializationVector, blockSize);
                // Increment block size.  The initial counter vector will be used for authentication at the end of
                // this encryption; any actual encryption will start with counter value 0x00000002.
                *(context->counter + blockSize - 1) = *(context->counter + blockSize - 1) + 1;

                // Zero the initial authentication tag
                memset (context->authTag, 0x00, blockSize);

                context->authBufferLen = 0;

                context->finalTagPointer = NULL;
                context->finalTagLen = 0;
                
                pBlock->state = BLOCK_CIPHER_STATE_IDLE;
                context->state = BLOCK_CIPHER_GCM_STATE_IDLE;
                break;
            case BLOCK_CIPHER_GCM_STATE_INIT:
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
                    context->state = BLOCK_CIPHER_GCM_STATE_IDLE;
                    return;
                }
                CRYCONLbits.OPMOD = 0;
                if ((pBlock->operation == BLOCK_CIPHER_OPERATION_ENCRYPT) || (pBlock->operation == BLOCK_CIPHER_OPERATION_DECRYPT))
                {
                    context->state = BLOCK_CIPHER_GCM_STATE_PROCESS_DATA;
                }
                else if (pBlock->operation == BLOCK_CIPHER_OPERATION_GENERATE_KEYSTREAM)
                {
                    context->state = BLOCK_CIPHER_GCM_STATE_GENERATE_KEYSTREAM;
                }
                BLOCK_CIPHER_CurrentHandleInitialize();
                memcpy (pBlock->previousText, context->counter, pBlock->blockSize);
                break;
            case BLOCK_CIPHER_GCM_STATE_PROCESS_DATA:
                {
                    dataCount = context->bytesRemainingInKeyStream;
                    if (pBlock->inCount < dataCount)
                    {
                        dataCount = pBlock->inCount;
                    }

                    pBlock->inCount -= dataCount;
                    context->bytesRemainingInKeyStream -= dataCount;
                    if(pBlock->outCount != NULL)
                    {
                        *pBlock->outCount += dataCount;
                    }

                    while (dataCount-- != 0)
                    {
                        *pBlock->dest = *pBlock->source++ ^ *(uint8_t *)context->keyStreamCurrentPosition++;
                        if (pBlock->operation == BLOCK_CIPHER_OPERATION_ENCRYPT)
                        {
                            // Store the calculated ciphertext in the auth buffer
                            *(context->authBuffer + context->authBufferLen) = *pBlock->dest++;
                            context->authBufferLen++;
                            // If we have a whole block of ciphertext stored, hash it.
                            if (context->authBufferLen == context->blockSize)
                            {
                                BLOCK_CIPHER_GCM_GHash (context);
                            }
                        }
                        else
                        {
                            pBlock->dest++;
                        }

                        if(context->keyStreamCurrentPosition == ((uint8_t*)context->keyStream + context->keyStreamSize))
                        {
                            context->keyStreamCurrentPosition = context->keyStream;
                        }
                    }

                    if (pBlock->inCount != 0)
                    {
                        context->state = BLOCK_CIPHER_GCM_STATE_ADD_DATA;
                    }
                    else
                    {
                        memcpy (context->counter, pBlock->previousText, pBlock->blockSize);
                        if (pBlock->options & BLOCK_CIPHER_OPTION_STREAM_COMPLETE)
                        {
                            context->state = BLOCK_CIPHER_GCM_STATE_GENERATE_TAG;
                        }
                        else
                        {
                            pBlock->state = BLOCK_CIPHER_STATE_IDLE;
                            context->state = BLOCK_CIPHER_GCM_STATE_IDLE;
                        }
                    }
                }
                break;
            case BLOCK_CIPHER_GCM_STATE_ADD_DATA:
                dataCount = blockSize;
                if (pBlock->inCount < blockSize)
                {
                    dataCount = pBlock->inCount;
                }
                memcpy (pBlock->inputText, pBlock->source, dataCount);
                memset (pBlock->inputText + dataCount, 0x00, blockSize - dataCount);
                pBlock->source += dataCount;
                CRYCONLbits.CRYGO = 1;
                context->state = BLOCK_CIPHER_GCM_STATE_WAIT_FOR_HW;
                break;
            case BLOCK_CIPHER_GCM_STATE_WAIT_FOR_HW:
                dataCount = blockSize;
                if (pBlock->inCount < blockSize)
                {
                    dataCount = pBlock->inCount;
                }

                if(pBlock->outCount != NULL)
                {
                    *pBlock->outCount += dataCount;
                }

                memcpy (pBlock->dest, pBlock->outputText, dataCount);
                if (pBlock->operation == BLOCK_CIPHER_OPERATION_ENCRYPT)
                {
                    for (i = 0; i < dataCount; i++)
                    {
                        // Store the calculated ciphertext in the auth buffer
                        *(context->authBuffer + context->authBufferLen) = *pBlock->dest++;
                        context->authBufferLen++;
                        // If we have a whole block of ciphertext stored, hash it.
                        if (context->authBufferLen == context->blockSize)
                        {
                            BLOCK_CIPHER_GCM_GHash (context);
                        }
                    }
                }
                else
                {
                    pBlock->dest += dataCount;
                }
                pBlock->inCount -= dataCount;

                if (pBlock->inCount != 0)
                {
                    context->state = BLOCK_CIPHER_GCM_STATE_ADD_DATA;
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
                    memcpy (context->counter, pBlock->previousText, blockSize);
                    if (pBlock->options & BLOCK_CIPHER_OPTION_STREAM_COMPLETE)
                    {
                        context->state = BLOCK_CIPHER_GCM_STATE_GENERATE_TAG;
                    }
                    else
                    {
                        pBlock->state = BLOCK_CIPHER_STATE_IDLE;
                        context->state = BLOCK_CIPHER_GCM_STATE_IDLE;
                    }
                }
                break;
            case BLOCK_CIPHER_GCM_STATE_GENERATE_KEYSTREAM:
                if ((context->keyStreamSize - context->bytesRemainingInKeyStream) < (blockSize * pBlock->inCount))
                {
                    pBlock->inCount = (context->keyStreamSize - context->bytesRemainingInKeyStream) / blockSize;
                }
                memset (pBlock->inputText, 0x00, blockSize);
                CRYCONLbits.CRYGO = 1;
                context->state = BLOCK_CIPHER_GCM_STATE_WAIT_FOR_KEYSTREAM;
                break;
            case BLOCK_CIPHER_GCM_STATE_WAIT_FOR_KEYSTREAM:
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
                    memcpy (context->counter, pBlock->previousText, pBlock->blockSize);
                    pBlock->state = BLOCK_CIPHER_STATE_IDLE;
                    context->state = BLOCK_CIPHER_GCM_STATE_IDLE;
                }
                else
                {
                    context->state = BLOCK_CIPHER_GCM_STATE_GENERATE_KEYSTREAM;
                }
                break;
            case BLOCK_CIPHER_GCM_STATE_GENERATE_TAG:
                // Pad the existing data with zeros and hash.
                BLOCK_CIPHER_GCM_GHash (context);
                // Copy the 64-bit lengths on the authenticated-but-not-encrypted data and the cipherText into
                // the authBuffer and hash it
                context->authBuffer[15] = (uint8_t)((context->cipherTextLen << 3) & 0xFF);
                context->authBuffer[14] = (uint8_t)((context->cipherTextLen >> 5) & 0xFF);
                context->authBuffer[13] = (uint8_t)((context->cipherTextLen >> 13) & 0xFF);
                context->authBuffer[12] = (uint8_t)((context->cipherTextLen >> 21) & 0xFF);
                context->authBuffer[11] = (uint8_t)((context->cipherTextLen >> 29) & 0xFF);
                context->authBuffer[10] = 0;
                context->authBuffer[9] = 0;
                context->authBuffer[8] = 0;
                context->authBuffer[7] = (uint8_t)((context->authDataLen << 3) & 0xFF);
                context->authBuffer[6] = (uint8_t)((context->authDataLen >> 5) & 0xFF);
                context->authBuffer[5] = (uint8_t)((context->authDataLen >> 13) & 0xFF);
                context->authBuffer[4] = (uint8_t)((context->authDataLen >> 21) & 0xFF);
                context->authBuffer[3] = (uint8_t)((context->authDataLen >> 29) & 0xFF);
                context->authBuffer[2] = 0;
                context->authBuffer[1] = 0;
                context->authBuffer[0] = 0;
                context->authBufferLen = 16;
                BLOCK_CIPHER_GCM_GHash (context);

                CRYCONLbits.CPHRMOD = 0;        // ECB
                memcpy ((uint8_t *)&CRYTXTA, context->initializationVector, blockSize);

                CRYCONLbits.CRYGO = 1;
                context->state = BLOCK_CIPHER_GCM_STATE_GENERATE_TAG_FINISH;
                break;
            case BLOCK_CIPHER_GCM_STATE_GENERATE_TAG_FINISH:
                memcpy (context->authBuffer, (uint8_t *)&CRYTXTB, blockSize);
                for (dataCount = 0; dataCount < context->blockSize; dataCount++)
                {
                    context->authTag[dataCount] ^= context->authBuffer[dataCount];
                }

                pBlock->state = BLOCK_CIPHER_STATE_IDLE;
                context->state = BLOCK_CIPHER_GCM_STATE_IDLE;

                if (pBlock->operation == BLOCK_CIPHER_OPERATION_ENCRYPT)
                {
                    memcpy (context->finalTagPointer, context->authTag, context->finalTagLen);
                }
                else if (pBlock->operation == BLOCK_CIPHER_OPERATION_DECRYPT)
                {
                    if (memcmp (context->finalTagPointer, context->authTag, context->finalTagLen) != 0)
                    {
                        pBlock->error = BLOCK_CIPHER_ERROR_INVALID_AUTHENTICATION;
                        pBlock->state = BLOCK_CIPHER_STATE_ERROR;
                        context->state = BLOCK_CIPHER_GCM_STATE_IDLE;
                        return;
                    }
                }
                break;
            default:
                pBlock->state = BLOCK_CIPHER_STATE_ERROR;
                context->state = BLOCK_CIPHER_GCM_STATE_IDLE;
                break;
        }
    } while ((context->state != BLOCK_CIPHER_GCM_STATE_GENERATE_TAG_FINISH) && \
            (context->state != BLOCK_CIPHER_GCM_STATE_INIT_SUBKEY_FINISH) && (context->state != BLOCK_CIPHER_GCM_STATE_WAIT_FOR_HW) && \
            (context->state != BLOCK_CIPHER_GCM_STATE_IDLE) && (context->state != BLOCK_CIPHER_GCM_STATE_WAIT_FOR_KEYSTREAM));
}


BLOCK_CIPHER_ERRORS BLOCK_CIPHER_16BV1_GCM_Encrypt (BLOCK_CIPHER_HANDLE handle, uint8_t * cipherText, uint32_t * numCipherBytes, uint8_t * plainText, uint32_t numPlainBytes, uint8_t * authenticationTag, uint8_t tagLen, uint32_t options)
{
    BLOCK_CIPHER_OBJECT *pBlock = &blockHandles[handle];
    uint8_t i;
    BLOCK_CIPHER_16BV1_GCM_CONTEXT * context = (BLOCK_CIPHER_16BV1_GCM_CONTEXT *)BLOCK_CIPHER_16BV1_HandleResolve(handle);

    if (context == NULL)
    {
        return BLOCK_CIPHER_ERROR_INVALID_HANDLE;
    }
    
    if (pBlock->state != BLOCK_CIPHER_STATE_IDLE)
    {
        return BLOCK_CIPHER_ERROR_BUSY;
    }

    // If authenticate of data that has not been encrypted has not been completed, continue
    if (!context->flags.authCompleted)
    {
        if ((options & BLOCK_CIPHER_OPTION_AUTHENTICATE_ONLY) != BLOCK_CIPHER_OPTION_AUTHENTICATE_ONLY)
        {
            // Note: The GHash function will zero-pad the authentication data if authBufferLen is not at the buffer size
            BLOCK_CIPHER_GCM_GHash (context);
            context->flags.authCompleted = true;
        }
    }

    pBlock->error = BLOCK_CIPHER_ERROR_NONE;
    pBlock->operation = BLOCK_CIPHER_OPERATION_ENCRYPT;
    context->finalTagPointer = authenticationTag;
    context->finalTagLen = tagLen;

    if ((options & BLOCK_CIPHER_OPTION_AUTHENTICATE_ONLY) == BLOCK_CIPHER_OPTION_AUTHENTICATE_ONLY)
    {
        // Add the number of bytes being authenticated but not encrypted to the authDataLen parameter.
        context->authDataLen += numPlainBytes;
        if(numCipherBytes != NULL)
        {
            *numCipherBytes += numPlainBytes;
        }

        // Authenticate this data
        while (numPlainBytes)
        {
            if (context->blockSize - context->authBufferLen < numPlainBytes)
            {
                i = context->blockSize - context->authBufferLen;
            }
            else
            {
                i = numPlainBytes;
            }
            memcpy (context->authBuffer + context->authBufferLen, plainText, i);
            context->authBufferLen += i;
            if (context->authBufferLen == context->blockSize)
            {
                BLOCK_CIPHER_GCM_GHash (context);
                // Note: The GHash function will clear authBufferLen
            }
            numPlainBytes -= i;
            plainText += i;
        }
        if ((options & BLOCK_CIPHER_OPTION_STREAM_COMPLETE) == BLOCK_CIPHER_OPTION_STREAM_COMPLETE)
        {
            context->state = BLOCK_CIPHER_GCM_STATE_GENERATE_TAG;
            pBlock->state = BLOCK_CIPHER_STATE_BUSY;
        }
    }
    else
    {
        context->cipherTextLen += numPlainBytes;
        pBlock->dest = cipherText;
        pBlock->source = plainText;
        pBlock->inCount = numPlainBytes;
        pBlock->options = options;
        pBlock->outCount = NULL;
        if(pBlock->outCount != NULL)
        {
            *pBlock->outCount = 0;
        }
        pBlock->state = BLOCK_CIPHER_STATE_BUSY;
        if (BLOCK_CIPHER_CurrentHandleIsInitialized())
        {
            context->state = BLOCK_CIPHER_GCM_STATE_PROCESS_DATA;
        }
        else
        {
            context->state = BLOCK_CIPHER_GCM_STATE_INIT;
        }
        context->state = BLOCK_CIPHER_GCM_STATE_INIT;
    }
    
    return BLOCK_CIPHER_ERROR_NONE;
}

BLOCK_CIPHER_ERRORS BLOCK_CIPHER_16BV1_GCM_Decrypt (BLOCK_CIPHER_HANDLE handle, uint8_t * plainText, uint32_t * numPlainBytes, uint8_t * cipherText, uint32_t numCipherBytes, uint8_t * authenticationTag, uint8_t tagLen, uint32_t options)
{
    BLOCK_CIPHER_OBJECT *pBlock = &blockHandles[handle];
    uint8_t i;
    BLOCK_CIPHER_16BV1_GCM_CONTEXT * context = (BLOCK_CIPHER_16BV1_GCM_CONTEXT *)BLOCK_CIPHER_16BV1_HandleResolve(handle);

    if (context == NULL)
    {
        return BLOCK_CIPHER_ERROR_INVALID_HANDLE;
    }

    if (pBlock->state != BLOCK_CIPHER_STATE_IDLE)
    {
        return BLOCK_CIPHER_ERROR_BUSY;
    }
    // If authenticate of data that has not been encrypted has not been completed, continue
    if (!context->flags.authCompleted)
    {
        if ((options & BLOCK_CIPHER_OPTION_AUTHENTICATE_ONLY) != BLOCK_CIPHER_OPTION_AUTHENTICATE_ONLY)
        {
            // Note: The GHash function will zero-pad the authentication data if authBufferLen is not at the buffer size
            BLOCK_CIPHER_GCM_GHash (context);
            context->flags.authCompleted = true;
        }
    }

    pBlock->operation = BLOCK_CIPHER_OPERATION_DECRYPT;
    pBlock->error = BLOCK_CIPHER_ERROR_NONE;
    context->finalTagPointer = authenticationTag;
    context->finalTagLen = tagLen;

    if ((options & BLOCK_CIPHER_OPTION_AUTHENTICATE_ONLY) == BLOCK_CIPHER_OPTION_AUTHENTICATE_ONLY)
    {
        // Add the number of bytes being authenticated but not encrypted to the authDataLen parameter.
        context->authDataLen += numCipherBytes;
        if(numPlainBytes != NULL)
        {
            *numPlainBytes += numCipherBytes;
        }
        if ((options & BLOCK_CIPHER_OPTION_STREAM_COMPLETE) == BLOCK_CIPHER_OPTION_STREAM_COMPLETE)
        {
            pBlock->state = BLOCK_CIPHER_STATE_BUSY;
            context->state = BLOCK_CIPHER_GCM_STATE_GENERATE_TAG;
        }
    }
    else
    {
        context->cipherTextLen += numCipherBytes;
        pBlock->dest = plainText;
        pBlock->source = cipherText;
        pBlock->inCount = numCipherBytes;
        pBlock->options = options;
        pBlock->outCount = numPlainBytes;
        if(pBlock->outCount != NULL)
        {
            *pBlock->outCount = 0;
        }

        pBlock->state = BLOCK_CIPHER_STATE_BUSY;
        if (BLOCK_CIPHER_CurrentHandleIsInitialized())
        {
            context->state = BLOCK_CIPHER_GCM_STATE_PROCESS_DATA;
        }
        else
        {
            context->state = BLOCK_CIPHER_GCM_STATE_INIT;
        }
    }

    // Only authenticate this data
    while (numCipherBytes)
    {
        if (context->blockSize - context->authBufferLen < numCipherBytes)
        {
            i = context->blockSize - context->authBufferLen;
        }
        else
        {
            i = numCipherBytes;
        }
        memcpy (context->authBuffer + context->authBufferLen, cipherText, i);
        context->authBufferLen += i;
        if (context->authBufferLen == context->blockSize)
        {
            BLOCK_CIPHER_GCM_GHash (context);
            // Note: The GHash function will clear authBufferLen
        }
        numCipherBytes -= i;
        cipherText += i;
    }
    
    return BLOCK_CIPHER_ERROR_NONE;
}

BLOCK_CIPHER_ERRORS BLOCK_CIPHER_16BV1_GCM_KeyStreamGenerate (BLOCK_CIPHER_HANDLE handle, uint32_t numBlocks, uint32_t options)
{
    BLOCK_CIPHER_OBJECT *pBlock = &blockHandles[handle];
    BLOCK_CIPHER_16BV1_GCM_CONTEXT * context = (BLOCK_CIPHER_16BV1_GCM_CONTEXT *)BLOCK_CIPHER_16BV1_HandleResolve(handle);

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
    pBlock->operation = BLOCK_CIPHER_OPERATION_GENERATE_KEYSTREAM;

    pBlock->state = BLOCK_CIPHER_STATE_BUSY;
    if (BLOCK_CIPHER_CurrentHandleIsInitialized())
    {
        context->state = BLOCK_CIPHER_GCM_STATE_GENERATE_KEYSTREAM;
    }
    else
    {
        context->state = BLOCK_CIPHER_GCM_STATE_INIT;
    }

    return BLOCK_CIPHER_ERROR_NONE;
}

void BLOCK_CIPHER_GCM_GHash (BLOCK_CIPHER_16BV1_GCM_CONTEXT * context)
{
    uint8_t * buffer = context->authBuffer;
    uint8_t * result = context->authTag;
    uint8_t i, j;

    j = context->blockSize;

    if (context->authBufferLen == 0)
    {
        return;
    }
    else if (context->authBufferLen < j)
    {
        // Pad the data with zeros
        memset (buffer + context->authBufferLen, 0x00, j - context->authBufferLen);
    }

    // XOR the current auth tag with the data in the auth buffer
    for (i = 0; i < j; i++)
    {
        *(result + i) ^= *(buffer + i);
    }

    // Compute the finite field multiplication of that XOR and the hash subkey
    BLOCK_CIPHER_GCM_GaloisMultiply (context->authTag, context->authTag, context->hashSubKey);

    context->authBufferLen = 0;
}

void BLOCK_CIPHER_GCM_GaloisMultiply (uint8_t * result, uint8_t * a, uint8_t * b)
{
    uint8_t bitCount;
    ff_int32_t v[4];
    ff_int32_t product[4];
    uint8_t mask = 0x80;
    bool msbSet;

    // Initialize product to zero
    memset (product, 0x00, 16);

    // Copy b to v (taking endianness into account)
    v[0].ui8_arr[3] = *b++;
    v[0].ui8_arr[2] = *b++;
    v[0].ui8_arr[1] = *b++;
    v[0].ui8_arr[0] = *b++;
    v[1].ui8_arr[3] = *b++;
    v[1].ui8_arr[2] = *b++;
    v[1].ui8_arr[1] = *b++;
    v[1].ui8_arr[0] = *b++;
    v[2].ui8_arr[3] = *b++;
    v[2].ui8_arr[2] = *b++;
    v[2].ui8_arr[1] = *b++;
    v[2].ui8_arr[0] = *b++;
    v[3].ui8_arr[3] = *b++;
    v[3].ui8_arr[2] = *b++;
    v[3].ui8_arr[1] = *b++;
    v[3].ui8_arr[0] = *b++;

    for (bitCount = 0; bitCount < 128; bitCount++)
    {
        if ((*a & mask) != 0)
        {
            product[0].ui32 ^= v[0].ui32;
            product[1].ui32 ^= v[1].ui32;
            product[2].ui32 ^= v[2].ui32;
            product[3].ui32 ^= v[3].ui32;
        }

        // Store the MSb of the 128-bit var
        msbSet = (v[3].ui32 & 0x01) != 0;
        // Shift the other bytes in the 128-bit var
        v[3].ui32 = (v[3].ui32 >> 1) | (v[2].ui32 << 31);
        v[2].ui32 = (v[2].ui32 >> 1) | (v[1].ui32 << 31);
        v[1].ui32 = (v[1].ui32 >> 1) | (v[0].ui32 << 31);
        v[0].ui32 = (v[0].ui32 >> 1);

        // If the MSb of the 128-bit var was set, xor the var with the reducing polynomial
        if (msbSet)
        {
            v[0].ui32 ^= 0xE1000000;
        }
        // Adjust the mask value and move a if necessary
        mask >>= 1;
        if (mask == 0)
        {
            a++;
            mask = 0x80;
        }
    }

    // Copy the product to the output variable
    for (mask = 0; mask < 4; mask++)
    {
        *result++ = product[mask].ui8_arr[3];
        *result++ = product[mask].ui8_arr[2];
        *result++ = product[mask].ui8_arr[1];
        *result++ = product[mask].ui8_arr[0];
    }
}




