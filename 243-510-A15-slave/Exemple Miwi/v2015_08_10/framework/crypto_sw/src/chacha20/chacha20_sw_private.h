/*********************************************************************
 *
 *                    CHACHA20 Function Library Headers
 *
 *********************************************************************
 * FileName:        chacha20_private.h
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

#define CHACHA20_SW_RotateL32wC(a,d)           ((((uint32_t)a) << d) | (((uint32_t)a) >> (32 - d)))


#define CHACHA20_SW_QuarterRound(a,b,c,d)      (a = a + b); \
                                            (d = d ^ a); \
                                            (d = CHACHA20_SW_RotateL32wC(d,16)); \
                                            (c = c + d); \
                                            (b = b ^ c); \
                                            (b = CHACHA20_SW_RotateL32wC(b,12)); \
                                            (a = a + b); \
                                            (d = d ^ a); \
                                            (d = CHACHA20_SW_RotateL32wC(d,8)); \
                                            (c = c + d); \
                                            (b = b ^ c); \
                                            (b = CHACHA20_SW_RotateL32wC(b,7));



void CHACHA20_SW_Hash (CHACHA20_SW_CONTEXT * context, uint32_t * input);















































