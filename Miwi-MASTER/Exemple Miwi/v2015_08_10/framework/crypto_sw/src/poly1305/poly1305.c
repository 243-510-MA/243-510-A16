/*********************************************************************
 *
 *                 POLY1305-AES Function Library
 *
 *********************************************************************
 * FileName:        poly1305.c
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

#include "crypto_sw/aes_sw.h"
#include "crypto_sw/poly1305_sw.h"
#include "crypto_sw/src/poly1305/poly1305_private.h"
#include <string.h>
#include <stdbool.h>

uint8_t POLY1305_NEGATIVE_MODULUS[18] __attribute__((__aligned__)) = {0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0C,0x00};

AES_SW_ROUND_KEYS_128_BIT aesRk __attribute__((__aligned__));
bool aesMutex = false;

bool POLY1305_SW_TestAndSet (void)
{
    bool returnVal = aesMutex;
    aesMutex = 1;
    return returnVal;
}

void POLY1305_SW_Initialize (void)
{

}

void POLY1305_SW_ContextInitialize (POLY1305_SW_CONTEXT * context, uint8_t * r)
{
    memset (context->digest, 0x00, 18);

    context->b[0] = 0;

    memcpy (context->r, r, 16);
    
    // Ensure that r has the mandated bits cleared
    *(context->r + 3) &= 0x0F;
    *(context->r + 7) &= 0x0F;
    *(context->r + 11) &= 0x0F;
    *(context->r + 15) &= 0x0F;
    *(context->r + 4) &= 0xFC;
    *(context->r + 8) &= 0xFC;
    *(context->r + 12) &= 0xFC;

    context->dataCount = 0;

    return;
}

void POLY1305_SW_DataAdd (POLY1305_SW_CONTEXT * context, uint8_t * message, uint32_t messageLen)
{
    uint8_t count;

    do
    {
        if (messageLen < 16 - context->dataCount)
        {
            count = messageLen;
            memcpy (&context->b[context->dataCount], message, count);
            context->dataCount += messageLen;
            return;
        }
        else
        {
            count = 16 - context->dataCount;
            memcpy (&context->b[context->dataCount], message, count);
            message += count;
            context->dataCount = 16;
        }

        context->b[16] = 1;

        // Add to existing value
        POLY1305_SW_Add18(context->b, context->digest, context->b);

        // Multiply by rbar
        POLY1305_SW_Multiply(context->a, context->r, context->b);

        // Mod by 2^130 - 5
        POLY1305_SW_FastMod (context);
        
        memcpy (&context->digest, &context->a, 17);

        context->dataCount = 0;
        messageLen -= count;
    } while (messageLen != 0);
}

bool POLY1305_SW_Calculate (POLY1305_SW_CONTEXT * context, uint8_t * tag, uint8_t * key, uint8_t * nonce)
{
    uint8_t i;
    uint8_t s[18] __attribute__((__aligned__));             // AESk(n)

    // Try to get the aes round key structure mutex
    if (POLY1305_SW_TestAndSet() == true)
    {
        return false;
    }

    memset (s, 0x00, 18);

    AES_SW_RoundKeysCreate(&aesRk, key, AES_SW_KEY_SIZE_128_BIT);
    AES_SW_Encrypt(0, s, nonce, &aesRk);

    // Free the round key structure
    aesMutex = false;

    if (context->dataCount != 0)
    {
        context->b[context->dataCount++] = 1;
        while(context->dataCount < 17)
        {
            context->b[context->dataCount++] = 0;
        }

        // Add to existing value
        POLY1305_SW_Add18(context->b, context->digest, context->b);

        // Multiply by rbar
        POLY1305_SW_Multiply(context->a, context->r, context->b);

        // Mod by 2^130 - 5
        POLY1305_SW_FastMod (context);

        memcpy (&context->digest, &context->a, 17);
    }

    POLY1305_SW_Add18 (context->b, s, context->digest);
    
    for (i = 0; i < 16; i++)
    {
        *tag++ = context->b[i];
    }

    return true;
}

// Fast algorithm to take the modulo by 2^130 - 5
void POLY1305_SW_FastMod (POLY1305_SW_CONTEXT * context)
{
    uint8_t upperBytes[18] __attribute__((__aligned__));
    uint8_t i, j, negative;

    upperBytes[17] = 0;
    memset (context->b, 0, 18);


    // upperBytes = a >> 130
    // context->b = 4 * upperBytes
    for (i = 0; i < 16; i++)
    {
        upperBytes[i] = context->a[i+16] >> 2;
        upperBytes[i] |= (context->a[i+17] << 6) & 0xC0;
        context->b[i] |= upperBytes[i] << 2;
        context->b[i + 1] |= (upperBytes[i] >> 6) & 0x03;
    }
    upperBytes[16] = context->a[32] >> 2;
    context->b[16] |= context->a[32];

    // Add b to upperBytes (upperBytes == 5 * (a >> 130))
    POLY1305_SW_Add18 (upperBytes, upperBytes, context->b);

    memset (((uint8_t *)&context->a) + 17, 0, 19);
    context->a[16] &= 0x03;

    POLY1305_SW_Add18to34 (context->a, upperBytes, context->a);

    // The result at this point may be between (modulus) and (2*modulus)
    // Reduce it if necessary (in constant time)
    memcpy (upperBytes, context->a, 18);
    POLY1305_SW_Add18to34 (context->a, POLY1305_NEGATIVE_MODULUS, context->a);
    negative = -((context->a[16] >> 2) & 1);
    for (j = 0; j < 18; ++j)
    {
        context->a[j] ^= negative & (upperBytes[j] ^ context->a[j]);
    }
    context->a[16] &= 0x03;

}




