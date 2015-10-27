/*********************************************************************
 *
 *                    SALSA20 Function Library Headers
 *
 *********************************************************************
 * FileName:        salsa20.h
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

// Salsa20 context
typedef struct
{
    // Key - tracks the key value, nonce, and counter
    union
    {
        uint8_t key8[64];
        uint32_t key32[16];
    }key;
    // Keystream
    union
    {
        uint8_t ks8[64];
        uint32_t ks32[16];
    } keystream;
    // Current offset in the keystream
    uint8_t curOffset;
} SALSA20_SW_CONTEXT;


// *****************************************************************************
/*
  Function:
    bool SALSA20_SW_KeyExpand(SALSA20_SW_CONTEXT * context, uint8_t * nonce, uint8_t * keyIn, uint8_t keyLen);

  Summary:
    Expands a key using a key expansion algorithm and a nonce

  Description:
    This routine performs the SALSA20 key expansion function on an input key

  Precondition:
    None

  Parameters:
    context [IN] - The encryption context
    nonce [IN] - An 8-byte nonce.  Must be unique for each stream
        used with the given key.
    keyIn [IN] - The key
    keyLen [IN] - Length of the key (bytes).  Must be 16 or 32.

  Returns:
    true if the key length was valid and the key was expanded; false otherwise

  Remarks:
    None
*/
bool SALSA20_SW_KeyExpand(SALSA20_SW_CONTEXT * context, uint8_t * nonce, uint8_t * keyIn, uint8_t keyLen);

// *****************************************************************************
/*
  Function:
    void SALSA20_SW_Encrypt (SALSA20_SW_CONTEXT * context, uint8_t * output, uint8_t * message, uint32_t messageLen);

  Summary:
    Encrypts data using SALSA20

  Description:
    This routine encrypts a stream of data using SALSA20.

  Precondition:
    The SALSA20 context must have been initialized with the
    SALSA20_SW_KeyExpand function

  Parameters:
    context [IN] - The encryption context
    output [OUT] - A buffer for the output data (can be the same as message to encrypt in-place).
    message [IN] - The message text
    messageLen [IN] - Length of the message text (bytes)

  Returns:
    None.

  Remarks:
    None
*/
void SALSA20_SW_Encrypt (SALSA20_SW_CONTEXT * context, uint8_t * output, uint8_t * message, uint32_t messageLen);

// *****************************************************************************
/*
  Function:
    void SALSA20_SW_Decrypt (SALSA20_SW_CONTEXT * context, uint8_t * output, uint8_t * cipherText, uint32_t cipherTextLen);

  Summary:
    Decrypts data using SALSA20

  Description:
    This routine decrypts a stream of data using SALSA20.

  Precondition:
    The SALSA20 context must have been initialized with the
    SALSA20_SW_KeyExpand function

  Parameters:
    context [IN] - The context
    output [OUT] - A buffer for the output data (can be the same as message to decrypt in-place).
    cipherText [IN] - The encrypted text
    cipherTextLen [IN] - Length of the cipher text (bytes)

  Returns:
    None.

  Remarks:
    None
*/
#define SALSA20_SW_Decrypt(a,b,c,d)    SALSA20_SW_Encrypt(a,b,c,d)

// *****************************************************************************
/*
  Function:
    void SALSA20_SW_PositionSet (SALSA20_SW_CONTEXT * context, uint8_t upper, uint32_t high, uint32_t low);

  Summary:
    Sets the current position in the encryption/decryption stream

  Description:
    This function sets the current byte offset in the encryption/decryption stream.
    If the user receives data in non-contiguous order, this function can be used to
    manually set the current position in the keystream.  The position to be set is
    specified in bytes, and is equal to the concatenation of:

        ((upper & 0x3F) * 2^64) + (high * 2^32) + low

    This provides addressability to the entire 2^70-1 bytes of the message space.

  Precondition:
    The SALSA20 context must have been initialized with the
    SALSA20_SW_KeyExpand function

  Parameters:
    context [IN] - The context
    upper [IN] - The upper 6 bits (bits 64:69) of the message position
    high [IN] - The middle 32 bits (bits 32:63) of the message position
    low [IN] - The lower 32 bits (bits 0:31) of the message position

  Returns:
    None.

  Remarks:
    None
*/
void SALSA20_SW_PositionSet (SALSA20_SW_CONTEXT * context, uint8_t upper, uint32_t high, uint32_t low);




