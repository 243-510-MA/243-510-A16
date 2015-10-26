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

/****************************** Headers *****************************/
#include <stdint.h>
#include "crypto_sw/xtea_sw.h"
#include "crypto_sw/src/xtea/xtea_sw_private.h"
#include <string.h>

/****************************** Constants ***************************/
const uint32_t DELTA = 0x9E3779B9;

/***************************** Config values ************************/
uint8_t gIterations;

/****************************** Functions ***************************/

void XTEA_SW_Configure (uint8_t iterations)
{
    gIterations = iterations;
}

void XTEA_SW_Encrypt (DRV_HANDLE handle, uint32_t * cipherText, uint32_t * plainText, uint32_t * key)
{
    uint8_t i=0;
    uint32_t x1;
    uint32_t x2;
    uint32_t sum;
    uint8_t iterationCount = gIterations;
    
    memcpy (cipherText, plainText, XTEA_SW_BLOCK_SIZE);
    
    sum = 0;
    x1= *cipherText;
    x2= *(cipherText+1);

    while(iterationCount > 0)
    {
        x1 += ((x2<<4 ^ x2>>5) + x2) ^ (sum + *(key+(sum&0x03)));
        sum+=DELTA;
        x2 += ((x1<<4 ^ x1>>5) + x1) ^ (sum + *(key+(sum>>11&0x03)));

        iterationCount--;
    }
    *(cipherText++) = x1;
    *(cipherText++) = x2;
    i++;
}


void XTEA_SW_Decrypt (DRV_HANDLE handle, uint32_t * plainText, uint32_t * cipherText, uint32_t * key)
{
    uint8_t i=0;
    uint32_t x1;
    uint32_t x2;
    uint32_t sum;
    
    memcpy (plainText, cipherText, XTEA_SW_BLOCK_SIZE);

    sum = DELTA * gIterations;
    x1 = *plainText;
    x2 = *(plainText+1);

    while(sum != 0)
    {
        x2 -= ((x1<<4 ^ x1>>5) + x1) ^ (sum + *(key+(sum>>11&0x03)));
        sum -= DELTA;
        x1 -= ((x2<<4 ^ x2>>5) + x2) ^ (sum + *(key+(sum&0x03)));
    }
    *(plainText++) = x1;
    *(plainText++) = x2;
    i++;
}
