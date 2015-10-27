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
#include "block_cipher_sw_private.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void * blockHandles[CRYPTO_CONFIG_SW_BLOCK_HANDLE_MAXIMUM];
bool isInitialized = false;

void BLOCK_CIPHER_SW_PaddingInsert (uint8_t * block, uint8_t blockLen, uint8_t paddingLength, uint32_t options)
{
    uint8_t i;

    i = blockLen - paddingLength;

    if((options & BLOCK_CIPHER_SW_OPTION_PAD_NULLS) == BLOCK_CIPHER_SW_OPTION_PAD_NULLS)
    {
        memset(&block[i], 0, paddingLength);
    }
    else if((options & BLOCK_CIPHER_SW_OPTION_PAD_8000) == BLOCK_CIPHER_SW_OPTION_PAD_8000)
    {
        block[i++] = 0x80;
        memset(&block[i], 0, paddingLength - 1);
    }
    else if((options & BLOCK_CIPHER_SW_OPTION_PAD_NUMBER) == BLOCK_CIPHER_SW_OPTION_PAD_NUMBER)
    {
        memset(&block[i], paddingLength, paddingLength);
    }
}

SYS_MODULE_OBJ BLOCK_CIPHER_SW_Initialize (const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init)
{
    uint8_t i;
    
    if (!isInitialized)
    {
        for (i = 0; i < CRYPTO_CONFIG_SW_BLOCK_HANDLE_MAXIMUM; i++)
        {
            blockHandles[i] = NULL;
        }
        isInitialized = true;
    }

    return SYS_MODULE_OBJ_STATIC;
}

BLOCK_CIPHER_SW_HANDLE BLOCK_CIPHER_SW_Open(const SYS_MODULE_INDEX index, const DRV_IO_INTENT ioIntent)
{
    uint8_t i;

    for (i = 0; i < CRYPTO_CONFIG_SW_BLOCK_HANDLE_MAXIMUM; i++)
    {
        if (blockHandles[i] == NULL)
        {
            blockHandles[i] = BLOCK_CIPHER_SW_HANDLE_IN_USE;
            return i;
        }
    }

    return BLOCK_CIPHER_SW_HANDLE_INVALID;
}

void BLOCK_CIPHER_SW_Close (BLOCK_CIPHER_SW_HANDLE handle)
{
    if (handle >= 0 && handle < CRYPTO_CONFIG_SW_BLOCK_HANDLE_MAXIMUM)
    {
        blockHandles[handle] = NULL;
    }
}

void BLOCK_CIPHER_SW_Deinitialize(SYS_MODULE_OBJ object)
{
    return;
}

void * BLOCK_CIPHER_SW_HandleResolve (BLOCK_CIPHER_SW_HANDLE handle)
{
    if (handle >= 0 && handle < CRYPTO_CONFIG_SW_BLOCK_HANDLE_MAXIMUM)
    {
        if ((blockHandles[handle] != NULL) && (blockHandles[handle] != BLOCK_CIPHER_SW_HANDLE_IN_USE))
        {
            return blockHandles[handle];
        }
    }

    return NULL;
}

BLOCK_CIPHER_SW_STATE BLOCK_CIPHER_SW_GetState (BLOCK_CIPHER_SW_HANDLE handle)
{
    if (blockHandles[handle] == NULL)
    {
        return BLOCK_CIPHER_SW_STATE_CLOSED;
    }
    else
    {
        return BLOCK_CIPHER_SW_STATE_IDLE;
    }
}

void BLOCK_CIPHER_SW_Tasks (void)
{

}
