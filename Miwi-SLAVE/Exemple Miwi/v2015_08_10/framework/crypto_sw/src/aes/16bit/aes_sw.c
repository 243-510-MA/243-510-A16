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

#include <system_config.h>
#include "crypto_sw/aes_sw.h"

#include <stdio.h>
#include <string.h>


#if defined(__C30__)
void AES_SW_RoundKeysCreate128(void* round_keys, uint8_t* key);
void AES_SW_RoundKeysCreate192(void* round_keys, uint8_t* key);
void AES_SW_RoundKeysCreate256(void* round_keys, uint8_t* key);

void AES_SW_RoundKeysCreate(void* round_keys, uint8_t* key, uint8_t key_size)
{
    #if defined(CRYPTO_CONFIG_SW_AES_KEY_128_ENABLE)
        AES_SW_RoundKeysCreate128(round_keys,key);
    #elif defined(CRYPTO_CONFIG_SW_AES_KEY_192_ENABLE)
        AES_SW_RoundKeysCreate192(round_keys,key);
    #elif defined(CRYPTO_CONFIG_SW_AES_KEY_256_ENABLE)
        AES_SW_RoundKeysCreate256(round_keys,key);
    #else
        //Dynamic
        switch(key_size)
        {
            case AES_SW_KEY_SIZE_128_BIT:
                AES_SW_RoundKeysCreate128(round_keys,key);
                break;
            case AES_SW_KEY_SIZE_192_BIT:
                AES_SW_RoundKeysCreate192(round_keys,key);
                break;
            case AES_SW_KEY_SIZE_256_BIT:
                AES_SW_RoundKeysCreate256(round_keys,key);
                break;
        }
    #endif        
}
#endif

