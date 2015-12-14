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

#include "crypto_16bv1/block_cipher_16bv1.h"
#include "crypto_16bv1/src/block_cipher/block_cipher_16bv1_private.h"
#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

BLOCK_CIPHER_OBJECT blockHandles[CRYPTO_CONFIG_16BV1_BLOCK_HANDLE_MAXIMUM];
bool isInitialized = false;
BLOCK_CIPHER_HANDLE CurrentThread = 0;
bool currentThreadIsInitialized;

void BLOCK_CIPHER_16BV1_PaddingInsert (uint8_t * block, uint8_t blockLen, uint8_t paddingLength, uint32_t options)
{
    uint8_t i;

    i = blockLen - paddingLength;

    if((options & BLOCK_CIPHER_OPTION_PAD_NULLS) == BLOCK_CIPHER_OPTION_PAD_NULLS)
    {
        memset(&block[i], 0, paddingLength);
    }
    else if((options & BLOCK_CIPHER_OPTION_PAD_8000) == BLOCK_CIPHER_OPTION_PAD_8000)
    {
        block[i++] = 0x80;
        memset(&block[i], 0, paddingLength - 1);
    }
    else if((options & BLOCK_CIPHER_OPTION_PAD_NUMBER) == BLOCK_CIPHER_OPTION_PAD_NUMBER)
    {
        memset(&block[i], paddingLength, paddingLength);
    }
}

SYS_MODULE_OBJ BLOCK_CIPHER_16BV1_Initialize (const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init)
{
    uint8_t i;
    
    if (!isInitialized)
    {
        for (i = 0; i < CRYPTO_CONFIG_16BV1_BLOCK_HANDLE_MAXIMUM; i++)
        {
            blockHandles[i].state = BLOCK_CIPHER_STATE_CLOSED;
        }
        isInitialized = true;
    }

    if(init->sys.powerState == SYS_MODULE_POWER_SLEEP)
    {
        CRYCONLbits.CRYSIDL = 0;
        CRYCONLbits.CRYON = 1;
    }
    else if(init->sys.powerState == SYS_MODULE_POWER_IDLE_STOP)
    {
        CRYCONLbits.CRYSIDL = 1;
        CRYCONLbits.CRYON = 1;
    }
    else if(init->sys.powerState == SYS_MODULE_POWER_IDLE_RUN)
    {
        CRYCONLbits.CRYSIDL = 0;
        CRYCONLbits.CRYON = 1;
    }
    else if(init->sys.powerState == SYS_MODULE_POWER_RUN_FULL)
    {
        CRYCONLbits.CRYSIDL = 0;
        CRYCONLbits.CRYON = 1;
    }
    else
    {
        CRYCONLbits.CRYSIDL = 0;
        CRYCONLbits.CRYON = 0;
    }

    currentThreadIsInitialized = false;

    return SYS_MODULE_OBJ_STATIC;
}

BLOCK_CIPHER_HANDLE BLOCK_CIPHER_16BV1_Open(const SYS_MODULE_INDEX index, const DRV_IO_INTENT ioIntent)
{
    uint8_t i;

    for (i = 0; i < CRYPTO_CONFIG_16BV1_BLOCK_HANDLE_MAXIMUM; i++)
    {
        if (blockHandles[i].state == BLOCK_CIPHER_STATE_CLOSED)
        {
            blockHandles[i].state = BLOCK_CIPHER_STATE_OPEN;
            blockHandles[i].intent = ioIntent;
            if (i == CurrentThread)
            {
                currentThreadIsInitialized = false;
            }
            return i;
        }
    }

    return BLOCK_CIPHER_HANDLE_INVALID;
}

void BLOCK_CIPHER_16BV1_Close (BLOCK_CIPHER_HANDLE handle)
{
    if (handle >= 0 && handle < CRYPTO_CONFIG_16BV1_BLOCK_HANDLE_MAXIMUM)
    {
        blockHandles[handle].state = BLOCK_CIPHER_STATE_CLOSED;
        if (handle == CurrentThread)
        {
            currentThreadIsInitialized = false;
        }
    }
}

void BLOCK_CIPHER_16BV1_Deinitialize(SYS_MODULE_OBJ object)
{
    return;
}

void * BLOCK_CIPHER_16BV1_HandleResolve (BLOCK_CIPHER_HANDLE handle)
{
    if (handle >= 0 && handle < CRYPTO_CONFIG_16BV1_BLOCK_HANDLE_MAXIMUM)
    {
        if ((blockHandles[handle].state != BLOCK_CIPHER_STATE_CLOSED) && \
           (blockHandles[handle].state != BLOCK_CIPHER_STATE_OPEN))
        {
            return blockHandles[handle].context;
        }
    }

    return NULL;
}

BLOCK_CIPHER_STATE BLOCK_CIPHER_16BV1_GetState (BLOCK_CIPHER_HANDLE handle)
{
    return blockHandles[handle].state;
}

//void * memcpy ( void * destination, const void * source, size_t num );
void BLOCK_CIPHER_16BV1_Tasks (void)
{
    BLOCK_CIPHER_HANDLE PreviousThread = CurrentThread;

    //If module is still busy exit and come back later
    if(CRYSTATbits.CRYBSY || CRYSTATbits.TXTABSY || CRYCONLbits.CRYGO)
    {
        return;
    }
    if(CRYSTATbits.MODFAIL || CRYSTATbits.KEYFAIL)
    {
        blockHandles[CurrentThread].error = BLOCK_CIPHER_ERROR_HW_SETTING;
        blockHandles[CurrentThread].state = BLOCK_CIPHER_STATE_ERROR;
    }
    if(CRYSTATbits.CRYABRT && !CRYCONLbits.CRYGO)
    {
        //User aborted last operation flag thread
        blockHandles[CurrentThread].error = BLOCK_CIPHER_ERROR_ABORT;
        
        CRYSTATbits.CRYABRT = 0;
    }

    if(blockHandles[CurrentThread].state == BLOCK_CIPHER_STATE_BUSY)
    {
        (*blockHandles[CurrentThread].tasks)(CurrentThread);
    }

    if(blockHandles[CurrentThread].state != BLOCK_CIPHER_STATE_BUSY)
    {
        do
        {
            CurrentThread++;
            if(CurrentThread>=CRYPTO_CONFIG_16BV1_BLOCK_HANDLE_MAXIMUM)
            {
                CurrentThread = 0;
            }
        } while((blockHandles[CurrentThread].state != BLOCK_CIPHER_STATE_BUSY) && \
              (CurrentThread!=PreviousThread));

        if (CurrentThread != PreviousThread)
        {
            currentThreadIsInitialized = false;
        }
    }
}

void BLOCK_CIPHER_CurrentHandleInitialize (void)
{
    currentThreadIsInitialized = true;
}

bool BLOCK_CIPHER_CurrentHandleIsInitialized (void)
{
    return currentThreadIsInitialized;
}

void BLOCK_CIPHER_HandleReInitialize (BLOCK_CIPHER_HANDLE handle)
{
    if (handle == CurrentThread)
    {
        currentThreadIsInitialized = false;
    }
}
