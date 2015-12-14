/*********************************************************************
 *
 *              POLY1305-AES Library Public Header File
 *
 *********************************************************************
 * FileName:        poly1305.h
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

#include "stdint.h"
#include <stdbool.h>

// Context for calculating the POLY1305 cipher
typedef struct
{
    uint8_t r[16] __attribute__((__aligned__));             // POLY1305 R parameter
    uint8_t a[34] __attribute__((__aligned__));             // Buffer space
    uint8_t b[18] __attribute__((__aligned__));             // Buffer space
    uint8_t digest[18] __attribute__((__aligned__));        // Current digest
    uint8_t dataCount;                                      // Current count of data bytes in b
} POLY1305_SW_CONTEXT;


// *****************************************************************************
/*
  Function:
    void POLY1305_SW_Initialize (void)

  Summary:
    Performs one-time initialization for the POLY1305 library

  Description:
    Performs one-time initialization for the POLY1305 library

  Precondition:
    None

  Parameters:
    None

  Returns:
    None

  Remarks:
    None
*/
void POLY1305_SW_Initialize (void);


// *****************************************************************************
/*
  Function:
    void POLY1305_SW_ContextInitialize (POLY1305_SW_CONTEXT * context, uint8_t * r);

  Summary:
    Initialize a POLY1305 context

  Description:
    This function will initialize a POLY1305 context and perform some steps of
    the MAC calculation.

  Precondition:
    POLY1305_SW_Initialize must have been called.

  Parameters:
    context [IN] - The POLY1305 context
    r [IN] - The r portion of the POLY1305 key (16 bytes, little-endian)

  Returns:
    void

  Remarks:
    None
*/
void POLY1305_SW_ContextInitialize (POLY1305_SW_CONTEXT * context, uint8_t * r);


// *****************************************************************************
/*
  Function:
    void POLY1305_SW_DataAdd (POLY1305_SW_CONTEXT * context, uint8_t * message, uint32_t messageLen);

  Summary:
    Add data to a MAC being calculated using POLY1305-AES

  Description:
    This function is used to add data to a MAC being calculated using POLY1305-AES.

  Precondition:
    POLY1305_SW_ContextInitialize must have been called on the specified context.

  Parameters:
    context [IN] - The POLY1305 context
    message [IN] - The message data to add to the context
    messageLen [IN] - Length of the message data (bytes)

  Returns:
    None

  Remarks:
    None
*/
void POLY1305_SW_DataAdd (POLY1305_SW_CONTEXT * context, uint8_t * message, uint32_t messageLen);


// *****************************************************************************
/*
  Function:
    bool POLY1305_SW_Calculate (POLY1305_SW_CONTEXT * context, uint8_t * tag, 
      uint8_t * key, uint8_t * nonce);

  Summary:
    Performs the final steps of the POLY1305 MAC calculation

  Description:
    This function performs the final steps of the POLY1305 MAC calculation
    and generates the message authentication code

  Precondition:
    POLY1305_SW_ContextInitialize must have been called on the specified context.

  Parameters:
    context [IN] - The POLT1305 context
    tag [OUT] - The resultant message authentication code tag
    key [IN] - The AES key portion of the POLY1305 key (128-bits, little-endian)
    nonce [IN] - A 16-byte nonce (little-endian).

  Returns:
    bool - true if context was initialized, false if the global resources
        needed by this function are in use by another context.  Should
        never be false unless this function is being called in an interrupt.

  Remarks:
    None
*/
bool POLY1305_SW_Calculate (POLY1305_SW_CONTEXT * context, uint8_t * tag, uint8_t * key, uint8_t * nonce);

