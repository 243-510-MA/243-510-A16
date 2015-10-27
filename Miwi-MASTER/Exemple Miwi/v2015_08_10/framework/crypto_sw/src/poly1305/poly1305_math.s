/*********************************************************************
 *
 *              POLY1305-AES Library Math Assembly Source File
 *
 *********************************************************************
 * FileName:        poly1305_math.s
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

.equ VALID_ID,0
.ifdecl __dsPIC30F
    .include "p30Fxxxx.inc"
.endif
.ifdecl __dsPIC33F
    .include "p33Fxxxx.inc"
.endif
.ifdecl __PIC24H
    .include "p24Hxxxx.inc"
.endif
.ifdecl __PIC24F
    .include "p24Fxxxx.inc"
.endif
.if VALID_ID <> 1
    //.error "Processor ID not specified in generic include files.  New ASM30 assembler needs to be downloaded?"
    .equ C,0
.endif


.text

    .global _POLY1305_SW_Add18
_POLY1305_SW_Add18:
    bclr    SR, #C              ; Clear the carry bit
POLY1305_SW_ADD18Loop:
    mov     [W1++], W4
    addc.w  W4, [W2++], [W0++]
    mov     [W1++], W4
    addc.w  W4, [W2++], [W0++]
    mov     [W1++], W4
    addc.w  W4, [W2++], [W0++]
    mov     [W1++], W4
    addc.w  W4, [W2++], [W0++]
    mov     [W1++], W4
    addc.w  W4, [W2++], [W0++]
    mov     [W1++], W4
    addc.w  W4, [W2++], [W0++]
    mov     [W1++], W4
    addc.w  W4, [W2++], [W0++]
    mov     [W1++], W4
    addc.w  W4, [W2++], [W0++]
    mov     [W1++], W4
    addc.w  W4, [W2++], [W0++]
    return

    .global _POLY1305_SW_Add18to34
_POLY1305_SW_Add18to34:
    bclr    SR, #C              ; Clear the carry bit
    clr     W6
    mov     [W1++], W4
    addc.w  W4, [W2++], [W0++]
    mov     [W1++], W4
    addc.w  W4, [W2++], [W0++]
    mov     [W1++], W4
    addc.w  W4, [W2++], [W0++]
    mov     [W1++], W4
    addc.w  W4, [W2++], [W0++]
    mov     [W1++], W4
    addc.w  W4, [W2++], [W0++]
    mov     [W1++], W4
    addc.w  W4, [W2++], [W0++]
    mov     [W1++], W4
    addc.w  W4, [W2++], [W0++]
    mov     [W1++], W4
    addc.w  W4, [W2++], [W0++]
    mov     [W1++], W4
    addc.w  W4, [W2++], [W0++]

    ; Add carries
    addc.w  W6, [W2++], [W0++]
    addc.w  W6, [W2++], [W0++]
    addc.w  W6, [W2++], [W0++]
    addc.w  W6, [W2++], [W0++]
    addc.w  W6, [W2++], [W0++]
    addc.w  W6, [W2++], [W0++]
    addc.w  W6, [W2++], [W0++]
    addc.w  W6, [W2++], [W0++]

    return




    .global _POLY1305_SW_Multiply
_POLY1305_SW_Multiply:
    push    W8
    push    W9
    push    W10
    push    W11

    ; Clear the result buffer
    mov         W0, W6        ; Move the LSB of result into W6
    mov         #34,W11        ; Move the length of result into W11

cmDoZero:
    clr         [W6++]
    dec2        W11, W11
    cp0         W11
    bra         NZ, cmDoZero

    ; Decrement xA, xB (to match termination case)
    ; W0 = LSB of result
    ; W1 = LSB of arg1
    ; W2 = LSB of arg2
    ; W3 = MSB of result
    ; W4 = MSB of arg1
    ; W5 = MSB of arg2
    ; W6:W7 used for multiplication result
    ; W9 = 0x0000

    add         W0, #16, W3        ; Set W3 to the MSB of the result
    add         W3, #18, W3
    add         W1, #16, W4          ; Set W4 to the MSB of the arg1
    add         W2, #18, W5          ; Set W5 to the MSB of the arg2

    mov         W1,W11                ; Store the address of the LSB of arg1 in W10
    clr         W9                  ; Set W9 to 0 (for carry addition)

cmLoopB:
    cp          W2, W5                ; Compare current iB and xB
    bra         Z, cmDone

    inc2        W0, W0
    mov         [W2++], W10            ; Get current iB word
    dec2        W0, W8                ; W4 = iR for A loop
    mov         W11, W1                ; Load the address of the LSB of arg1

cmLoopA:
    mul.uu      W10, [W1++], W6        ; W7:W6 = B * A
    add         W6, [W8], [W8++]    ; R = R + (B*A)
    addc        W7, [W8], [W8]

    ; Propagate carries through all bytes (constant time)
    mov         W8, W6
cmCarry:
    addc        W9, [++W6], [W6]    ; Add in residual carry to MSB of R and carry forward if it causes a carry out
    xor.w       W6, W3, W7            ; See if max address reached
    bra         NZ, cmCarry

cmCarryDone:
    cp          W1, W4
    bra         NZ, cmLoopA
    bra         cmLoopB

cmDone:
    pop         W11
    pop         W10
    pop         W9
    pop         W8
    return


