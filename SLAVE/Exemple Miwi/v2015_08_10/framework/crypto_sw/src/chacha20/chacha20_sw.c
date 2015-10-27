/*********************************************************************
 *
 *                    CHACHA20 Function Library Headers
 *
 *********************************************************************
 * FileName:        chacha20.c
 * Dependencies:    None
 * Processor:       PIC24F
 * Company:         Microchip Technology, Inc.
 *
 * Software License Agreement
 *
 * Copyright (C) 2002-2014 Microchip Technology Inc.  All rights
 * reserved.
 *
 * Microchip licenses to you the right to use, modify, copy, and
 * distribute:
 * (i)  the Software when embedded on a Microchip microcontroller or
 *      digital signal controller product ("Device") which is
 *      integrated into Licensee's product; or
 * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
 *        ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
 *        used in conjunction with a Microchip ethernet controller for
 *        the sole purpose of interfacing with the ethernet controller.
 *
 * You should refer to the license agreement accompanying this
 * Software for additional information regarding your rights and
 * obligations.
 *
 * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
 * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
 * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
 * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
 * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
 * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
 * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
 * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
 *
 * IMPORTANT:  The implementation and use of third party algorithms,
 * specifications and/or other technology may require a license from
 * various third parties.  It is your responsibility to obtain
 * information regarding any applicable licensing obligations.
 *
 ********************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "string.h"
#include "crypto_sw/chacha20_sw.h"
#include "crypto_sw/src/chacha20/chacha20_sw_private.h"

const uint32_t sigmaTau0 = 0x61707865;      // "apxe"
const uint32_t sigmaTau3 = 0x6B206574;      // "k et"
const uint32_t sigma1 = 0x3320646E;         // "3 dn"
const uint32_t sigma2 = 0x79622D32;         // "yb-2"
const uint32_t tau1 = 0x3120646E;           // "1 dn"
const uint32_t tau2 = 0x79622D36;           // "yb-6"


bool CHACHA20_SW_KeyExpand(CHACHA20_SW_CONTEXT * context, uint8_t * nonce, uint8_t * keyIn, uint8_t keyLen)
{
    uint8_t * keyIn2;

    if (keyLen == 16)
    {
        keyIn2 = keyIn;
        context->key.key32[1] = tau1;
        context->key.key32[2] = tau2;
    }
    else if (keyLen == 32)
    {
        keyIn2 = keyIn + 16;
        context->key.key32[1] = sigma1;
        context->key.key32[2] = sigma2;
    }
    else
    {
        // Invalid key length
        return false;
    }

    context->key.key32[0] = sigmaTau0;
    context->key.key32[3] = sigmaTau3;

    // Using memcpy to copy the data into the key arrays will
    // automatically convert the 32-bit key elements to little-endian form
    memcpy (&context->key.key32[4], keyIn, 16);
    memcpy (&context->key.key32[8], keyIn2, 16);
    memset (&context->key.key32[12], 0x00, 8);
    memcpy (&context->key.key32[14], nonce, 8);

    CHACHA20_SW_Hash (context, context->key.key32);
    context->curOffset = 0;

    return true;
}


void CHACHA20_SW_Encrypt (CHACHA20_SW_CONTEXT * context, uint8_t * output, uint8_t * message, uint32_t messageLen)
{
    uint32_t offset = 0;
    uint8_t count, i;
    uint8_t * valuePtr;

    while (messageLen > 0)
    {
        if (context->curOffset >= 64)
        {
            context->key.key32[12] += 1;
            if (context->key.key32[12] == 0)
            {
                context->key.key32[13] += 1;
            }
            CHACHA20_SW_Hash (context, context->key.key32);
            context->curOffset = 0;
        }

        if (messageLen > (64 - context->curOffset))
        {
            count = 64 - context->curOffset;
        }
        else
        {
            count = messageLen;
        }

        valuePtr = &context->keystream.ks8[context->curOffset];
        for (i = 0; i < count; i++)
        {
            *(output + offset) = *(message + offset) ^ *(valuePtr + i);
            offset++;
        }

        messageLen -= count;
        context->curOffset += count;

    }
}

void CHACHA20_SW_Hash (CHACHA20_SW_CONTEXT * context, uint32_t * input)
{
    uint8_t i;
    uint32_t * temp = context->keystream.ks32;

    for (i = 0; i < 16; i++)
    {
        temp[i] = input[i];
    }

    for (i = 0; i < 10; i++)
    {
        CHACHA20_SW_QuarterRound(temp[0],temp[4],temp[8],temp[12])
        CHACHA20_SW_QuarterRound(temp[1],temp[5],temp[9],temp[13])
        CHACHA20_SW_QuarterRound(temp[2],temp[6],temp[10],temp[14])
        CHACHA20_SW_QuarterRound(temp[3],temp[7],temp[11],temp[15])
        CHACHA20_SW_QuarterRound(temp[0],temp[5],temp[10],temp[15])
        CHACHA20_SW_QuarterRound(temp[1],temp[6],temp[11],temp[12])
        CHACHA20_SW_QuarterRound(temp[2],temp[7],temp[8],temp[13])
        CHACHA20_SW_QuarterRound(temp[3],temp[4],temp[9],temp[14])
    }

    for (i = 0; i < 16; i++)
    {
        temp[i] += input[i];
    }
}


void CHACHA20_SW_PositionSet (CHACHA20_SW_CONTEXT * context, uint8_t upper, uint32_t high, uint32_t low)
{
    context->key.key32[9] = upper & 0x3f;
    context->key.key32[9] <<= 26;
    context->key.key32[9] |= (high >> 6) & 0x03ffffff;
    context->key.key32[8] = (high << 26) & 0xfc000000;
    context->key.key32[8] |= (low >> 6) & 0x03ffffff;

    CHACHA20_SW_Hash (context, context->key.key32);

    context->curOffset = low & 0x3F;
}



