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

#if !defined (_ARCFOUR_SW_H)
#define _ARCFOUR_SW_H

#include <stdint.h>

// Encryption Context for ARCFOUR module.
// The program need not access any of these values directly, but rather
// only store the structure and use ARCFOUR_SW_CreateSBox to set it up.
typedef struct
{
	uint8_t *sBox;              // A pointer to a 256 byte S-box array
	uint8_t iterator;           // The iterator variable
	uint8_t coiterator;         // The co-iterator
} ARCFOUR_SW_CONTEXT;

/*****************************************************************************
  Function:
	void ARCFOUR_SW_CreateSBox(ARCFOUR_SW_CONTEXT* context, uint8_t * sBox, 
    uint8_t* key, uint16_t key_length)

  Summary:
	Initializes an ARCFOUR encryption stream.

  Description:
	This function initializes an ARCFOUR encryption stream.  Call this 
	function to set up the initial state of the encryption context and the
	S-box.  The S-box will be initialized to its zero state with the 
	supplied key.
	
	This function can be used to initialize for encryption and decryption.

  Precondition:
	None.

  Parameters:
	context - A pointer to the allocated encryption context structure
	sBox - A pointer to a 256-byte buffer that will be used for the S-box.
    key - A pointer to the key to be used
	key_length - The length of the key, in bytes.

  Returns:
	None

  Remarks:
	For security, the key should be destroyed after this call.
  ***************************************************************************/
void ARCFOUR_SW_CreateSBox(ARCFOUR_SW_CONTEXT* context, uint8_t * sBox, uint8_t* key, uint16_t key_length);

/*****************************************************************************
  Function:
	void ARCFOUR_SW_Encrypt(uint8_t* data, uint32_t data_length, 
        ARCFOUR_SW_CONTEXT* context)

  Summary:
	Encrypts an array of data with the ARCFOUR algorithm.

  Description:
	This function uses the current ARCFOUR context to encrypt data in place.

  Precondition:
	The encryption context has been initialized with ARCFOUR_SW_CreateSBox.

  Parameters:
	data - The data to be encrypted (in place)
	data_length - The length of data
	context - A pointer to the initialized encryption context structure

  Returns:
	None
  ***************************************************************************/
void ARCFOUR_SW_Encrypt(uint8_t* data, uint32_t data_length, ARCFOUR_SW_CONTEXT* context);


/*****************************************************************************
  Function:
	void ARCFOUR_SW_Decrypt(uint8_t* data, uint32_t data_length, 
        ARCFOUR_SW_CONTEXT* context

  Summary:
	Decrypts an array of data with the ARCFOUR algorithm.

  Description:
	This function uses the current ARCFOUR context to decrypt data in place.

  Precondition:
	The encryption context has been initialized with ARCFOUR_SW_CreateSBox.

  Parameters:
	data - The data to be encrypted (in place)
	data_length - The length of data
	context - A pointer to the initialized encryption context structure

  Returns:
	None
  ***************************************************************************/
#define ARCFOUR_SW_Decrypt     ARCFOUR_SW_Encrypt

#endif      // _ARCFOUR_SW_H

