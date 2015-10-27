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
#define __ARCFOUR_SW_C

#include "crypto_sw/arcfour_sw.h"

void ARCFOUR_SW_CreateSBox(ARCFOUR_SW_CONTEXT* context, uint8_t * sBox, uint8_t* key, uint16_t key_length)
{
	uint8_t temp, i, j, *Sbox;

	// Initialize the context indicies
	i = 0;
	j = 0;
    context->sBox = sBox;
	Sbox = sBox;
	
	// Initialize each S-box element with its index
	do
	{
		Sbox[i] = i;
		i++;
	} while(i != 0u);

	// Fill in the S-box
	do
	{
		j = j + Sbox[i] + key[i % key_length];
		temp = Sbox[i];
		Sbox[i] = Sbox[j];
		Sbox[j] = temp;
		i++;
	} while(i != 0u);

	// Reset the context indicies
	context->iterator = 0;
	context->coiterator = 0;

}

void ARCFOUR_SW_Encrypt(uint8_t* data, uint32_t data_length, ARCFOUR_SW_CONTEXT* context)
{
	uint8_t temp, temp2, i, j, *Sbox;

	// Buffer context variables in local RAM for faster access
	i = context->iterator;
	j = context->coiterator;
	Sbox = context->sBox;

	// Loop over each byte.  Extract its key and XOR
	while(data_length--)
	{		
		i++;
		temp = Sbox[i];		
		j += temp;
		temp2 = Sbox[j];
		Sbox[i] = temp2;
		Sbox[j] = temp;
		temp += temp2;
		temp2 = Sbox[temp];

		*data++ ^= temp2;
	}
	
	// Save the new context
	context->iterator = i;
	context->coiterator = j;

}

