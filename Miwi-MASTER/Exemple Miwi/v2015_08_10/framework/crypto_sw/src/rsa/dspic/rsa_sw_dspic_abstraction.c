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

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "crypto_sw/rsa_sw.h"
#include "rsa_sw_ret_codes.h"
#include "rsa_sw_enc_dec.h"

#include "crypto_sw/src/drv_common.h"
#include "crypto_sw/src/sys_common.h"

typedef struct
{
    uint8_t * xBuffer;
    uint8_t * yBuffer;
    uint16_t xLen;
    uint16_t yLen;
    uint32_t length;
    uint32_t * msgSize;
    RSA_SW_RandomGet randFunc;
    RSA_SW_PAD_TYPE padType;
    DRV_IO_INTENT runType;
    RSA_SW_STATUS status;
    RSA_SW_OPERATION_MODES op;
} RSA_SW_DESC;

static RSA_SW_DESC rsaDesc0;

__inline static bool is_aligned (void *p)
{
    return (int)p % 2 == 0;
}

SYS_MODULE_OBJ RSA_SW_Initialize( const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init)
{
    if (index != RSA_SW_INDEX)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    rsaDesc0.xBuffer = NULL;
    rsaDesc0.xLen = 0;
    rsaDesc0.yBuffer = NULL;
    rsaDesc0.yLen = 0;
    rsaDesc0.randFunc = NULL;
    rsaDesc0.padType = RSA_SW_PAD_DEFAULT;
    rsaDesc0.runType = DRV_IO_INTENT_EXCLUSIVE;
    rsaDesc0.status = RSA_SW_STATUS_INIT;
    rsaDesc0.op = RSA_SW_OPERATION_MODE_NONE;

    return SYS_MODULE_OBJ_STATIC;
}

void RSA_SW_Deinitialize( SYS_MODULE_OBJ object)
{
    rsaDesc0.xBuffer = NULL;
    rsaDesc0.xLen = 0;
    rsaDesc0.yBuffer = NULL;
    rsaDesc0.yLen = 0;
    rsaDesc0.randFunc = NULL;
    rsaDesc0.padType = RSA_SW_PAD_DEFAULT;
    rsaDesc0.runType = DRV_IO_INTENT_EXCLUSIVE;
    rsaDesc0.status = RSA_SW_STATUS_INVALID;
    rsaDesc0.op = RSA_SW_OPERATION_MODE_NONE;

    return;
}

DRV_HANDLE RSA_SW_Open( const SYS_MODULE_INDEX index, const DRV_IO_INTENT ioIntent)
{
    if (index != RSA_SW_INDEX)
    {
        return DRV_HANDLE_INVALID;
    }
    if (rsaDesc0.status != RSA_SW_STATUS_INIT)
    {
        return DRV_HANDLE_INVALID;
    }
    
    rsaDesc0.runType = (ioIntent | DRV_IO_INTENT_EXCLUSIVE);

    rsaDesc0.status = RSA_SW_STATUS_OPEN;

    return RSA_SW_HANDLE;
}

void RSA_SW_Close (DRV_HANDLE handle)
{
    rsaDesc0.xBuffer = NULL;
    rsaDesc0.xLen = 0;
    rsaDesc0.yBuffer = NULL;
    rsaDesc0.yLen = 0;
    rsaDesc0.randFunc = NULL;
    rsaDesc0.padType = RSA_SW_PAD_DEFAULT;
    rsaDesc0.runType = DRV_IO_INTENT_EXCLUSIVE;
    rsaDesc0.status = RSA_SW_STATUS_INIT;
    rsaDesc0.op = RSA_SW_OPERATION_MODE_NONE;

    return;
}

RSA_SW_STATUS RSA_SW_ClientStatus( DRV_HANDLE handle )
{
    return rsaDesc0.status;
}

int RSA_SW_Configure(DRV_HANDLE h, uint8_t *xBuffer, uint8_t *yBuffer, uint16_t xLen, uint16_t yLen,  RSA_SW_RandomGet randFunc, RSA_SW_PAD_TYPE padType)
{
    if (h != RSA_SW_HANDLE || (padType != RSA_SW_PAD_DEFAULT && padType != RSA_SW_PAD_PKCS1))
    {
        return -1;
    }

    if (xBuffer == NULL || yBuffer == NULL || randFunc == NULL)
    {
        return -1;
    }

    if (xLen == 0 || yLen == 0)
    {
        return -1;
    }

    if (!is_aligned (xBuffer) || !is_aligned(yBuffer))
    {
        return -1;
    }

    rsaDesc0.xBuffer = xBuffer;
    rsaDesc0.xLen = xLen;
    rsaDesc0.yBuffer = yBuffer;
    rsaDesc0.yLen = yLen;
    rsaDesc0.randFunc = randFunc;
    rsaDesc0.padType = padType;
    rsaDesc0.status = RSA_SW_STATUS_READY;

    return 0;
}

RSA_SW_STATUS RSA_SW_Encrypt (DRV_HANDLE handle, uint8_t *cipherText, uint8_t *plainText, uint16_t msgLen, const RSA_SW_PUBLIC_KEY *publicKey)
{
    uint16_t retVal;

    retVal = rsa_encrypt (cipherText, plainText, msgLen, publicKey->n, publicKey->nLen, publicKey->exp, publicKey->eLen, rsaDesc0.xBuffer, rsaDesc0.yBuffer, rsaDesc0.randFunc);

    switch (retVal)
    {
        case MCL_SUCCESS:
            return RSA_SW_STATUS_READY;
            break;
        case MCL_ILLEGAL_PARAMETER:
            return RSA_SW_STATUS_BAD_PARAM;
            break;
        case MCL_ILLEGAL_SIZE:
        default:
            return RSA_SW_STATUS_ERROR;            
            break;
    }
}

RSA_SW_STATUS RSA_SW_Decrypt (DRV_HANDLE handle, uint8_t *plainText, uint8_t *cipherText, uint16_t * msgLen, const RSA_SW_PRIVATE_KEY_CRT *privateKey)
{
    uint16_t retVal;

    retVal = rsa_decrypt (plainText, (unsigned short int *)msgLen, cipherText, privateKey->nLen, (unsigned char *)privateKey, rsaDesc0.xBuffer, rsaDesc0.yBuffer);

    switch (retVal)
    {
        case MCL_SUCCESS:
            return RSA_SW_STATUS_READY;
            break;
        case MCL_ILLEGAL_PARAMETER:
            return RSA_SW_STATUS_BAD_PARAM;
            break;
        case MCL_INVALID_CIPHERTEXT:
        case MCL_ILLEGAL_SIZE:
        default:
            return RSA_SW_STATUS_ERROR;            
            break;
    }
}

void RSA_SW_Tasks (SYS_MODULE_OBJ object)
{
    
}

//void _RSA_SW_RNGAbstractor (uint8_t * workingBuffer, uint8_t * dest, uint16_t count)
//{
//    uint8_t i = 0;
//    uint32_t random = rsaDesc0.randFunc();
//
//    while (count--)
//    {
//        *dest++ = (uint8_t)((random >> (8 * i)) & 0xFF);
//
//        i++;
//        if (i == 4)
//        {
//            i = 0;
//            random = rsaDesc0.randFunc();
//        }
//    }
//    
//}


