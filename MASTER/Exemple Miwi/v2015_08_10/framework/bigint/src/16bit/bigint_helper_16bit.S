/**********************************************************************
; *
; *     Big Integer Assembly Helpers
; *  Library for Microchip TCP/IP Stack
; *      - Accelerates processing for BigInt functions
; *
; *********************************************************************
; * FileName:        BigInt_helper.S
; * Dependencies:    None
; * Processor:       PIC24F, PIC24H, dsPIC30F, dsPIC33F
; * Compiler:        Microchip C30 v3.12 or higher
; * Company:         Microchip Technology, Inc.
; *
; * Software License Agreement
; *
; * Copyright (C) 2002-2009 Microchip Technology Inc.  All rights
; * reserved.
; *
; * Microchip licenses to you the right to use, modify, copy, and
; * distribute:
; * (i)  the Software when embedded on a Microchip microcontroller or
; *      digital signal controller product ("Device") which is
; *      integrated into Licensee's product; or
; * (ii) ONLY the Software driver source files ENC28J60.c, ENC28J60.h,
; *        ENCX24J600.c and ENCX24J600.h ported to a non-Microchip device
; *        used in conjunction with a Microchip ethernet controller for
; *        the sole purpose of interfacing with the ethernet controller.
; *
; * You should refer to the license agreement accompanying this
; * Software for additional information regarding your rights and
; * obligations.
; *
; * THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
; * WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
; * LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
; * PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
; * MICROCHIP BE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
; * CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
; * PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
; * BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
; * THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
; * SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
; * (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
; *
; *
; * Author               Date        Comment
; *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
; * Elliott Wood         6/20/07    Original
; * Howard Schlunder     9/12/07    Ported to 16-bit PICs
; ********************************************************************/
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

.equ BIGINT_LENGTH_OFFSET,  2

.text


;***************************************************************************
; Function:     void _addBI()
;
; PreCondition: _iA and _iB are loaded with the address of the LSB of the BigInt
;                _xA and _xB are loaded with the address of the MSB of the BigInt
;                a.size >= b.magnitude
;
; Input:         A and B, the BigInts to add
;
; Output:         A = A + B
;
; Side Effects: None
;
; Overview:     Quickly performs the bulk addition of two BigInts
;
; Note:            Function works
;***************************************************************************
    .global    __addBI
__addBI:
    mov     [W0], W2            ; Move the LSB pointer for arg1 into W2
    mov     [W0+BIGINT_LENGTH_OFFSET], W0            ; Move the length of arg1 into W0
    add.w    W0, W2, W0            ; Store the pointer to the MSB in W0

    mov     [W1], W4            ; Move the LSB pointer for arg2 into W2
    mov     [W1+BIGINT_LENGTH_OFFSET], W3            ; Move the length of arg2 into W0
    add.w    W3, W4, W3            ; Store the pointer to the MSB in W3

    bclr    SR, #C                ; C = 0

; Perform addition of all B words
aDoAdd:
    mov.w    [W4++], W1            ; Read B[i]
    addc.w    W1, [W2], [W2++]    ; A[i] = A[i] + B[i] + C
    xor.w    W3, W4, W5            ; Is ++i past the MSB of B?
    bra        NZ, aDoAdd            ; MSB not finished, go repeat loop

    bra        aFinalCarryCheck

; Add in final carry out and propagate forward as needed
aDoFinalCarry:
    addc.w    W5, [W2], [W2++]    ; W5 == 0, A[i] = A[i] + C
aFinalCarryCheck:
    bra        NC, aDone
    xor.w    W0, W2, W6            ; See if max address reached
    bra        NZ, aDoFinalCarry

aDone:
    btsc    SR, #C                ; Branch to the overflow condition if C is still set
    bra     aOverflow
    mov        #1, W0                ; Set W0, bit 1 (logical true) if there was no overflow
    return    
aOverflow:
    clr W0                        ; Clear W0 (logical false) if there was an unhandled overflow
    return

;***************************************************************************
; Function:     void _subBI()
;
; PreCondition: _iA and _iB are loaded with the address of the LSB of the BigInt
;                _xA and _xB are loaded with the address of the MSB of the BigInt
;
; Input:         A and B, the BigInts to subtract
;
; Output:         A = A - B
;
; Side Effects: None
;
; Overview:     Quickly performs the bulk subtraction of two BigInts
;
; Note:            Function works
;***************************************************************************
    .global    __subBI
__subBI:
    mov     [W0], W2            ; Move the LSB pointer for arg1 into W2
    mov     [W0+BIGINT_LENGTH_OFFSET], W0            ; Move the length of arg1 into W0
    add.w    W0, W2, W0            ; Store the pointer to the MSB in W0

    mov     [W1], W4            ; Move the LSB pointer for arg2 into W2
    mov     [W1+BIGINT_LENGTH_OFFSET], W3            ; Move the length of arg2 into W0
    add.w    W3, W4, W3            ; Store the pointer to the MSB in W3

    bset    SR, #C                ; Borrow = 0

; Perform subtraction of all B words
sDoSub:
    mov.w    [W4++], W1            ; Read B[i]
    subbr.w    W1, [W2], [W2++]    ; A[i] = A[i] - B[i] - Borrow
    xor.w    W3, W4, W5            ; Is ++i past the MSB?
    bra        NZ, sDoSub            ; MSB not finished, go repeat loop

    bra        sFinalBorrowCheck

; Perform final borrow and propagate forward as needed
sDoFinalBorrow:
    subbr.w    W5, [W2], [W2++]    ; W5 == 0, A[i] = A[i] - 0 - Borrow
sFinalBorrowCheck:
    bra        C, sDone
    xor.w    W0, W2, W6            ; See if max address reached
    bra        NZ, sDoFinalBorrow

sDone:
    btsc    SR, #C                ; Branch to the overflow condition if C is still set
    bra     sOverflow
    mov        #1, W0                ; Set W0, bit 1 (logical true) if there was no overflow
    return    
sOverflow:
    clr W0                        ; Clear W0 (logical false) if there was an unhandled overflow

    return

/***************************************************************************
; Function:     void _setBI(BI *A,uint8_t *value)
;
; PreCondition: _iA is loaded with the address of the LSB of the BigInt
;                _xA is loaded with the address of the MSB of the BigInt
;
; Input:         None
;
; Output:         A = 0
;
; Side Effects: None
;
; Overview:     Sets all words from _iA to _xA to zero
;
; Note:            Function works
;***************************************************************************/
    .global    __setBI
__setBI:
    mov        [W0], W2        ; Move the LSB of the bigint into W2
    mov     [W0+BIGINT_LENGTH_OFFSET],W0        ; Move the length of the bigint into W0
    mov        [W1],W1

zDoZero:
    mov        W1, [W2++]
    dec2     W0, W0
    cp0        W0
    bra        NZ, zDoZero

    return
 
;***************************************************************************
; Function:     void _msbBI()
;
; PreCondition: _iA is loaded with the address of the LSB of the BigInt buffer
;                _xA is loaded with the address of the right most byte of the BigInt buffer
;
; Input:         None
;
; Output:         _xA is now pointing to the MSB of the BigInt
;
; Side Effects: None
;
; Overview:     Finds the MSB (first non-zero word) of the BigInt, starting 
;                from the right-most word and testing to the left.  This 
;                function will stop if _iA is reached.
;
; Note:            Function works
;***************************************************************************
    .global    __msbBI
__msbBI:
    mov     [W0], W2            ; Move the LSB pointer for arg1 into W2
    mov     [W0+BIGINT_LENGTH_OFFSET], W0            ; Move the length of arg1 into W0
    add.w    W0, W2, W0            ; Store the pointer to the MSB in W0
    dec2    W0, W0

msbLoop:
    cp        W2,W0            ; Compare the two bigint pointers
    bra        Z, msbDone        ; Branch to done if both pointers are the same
    cp0        [W0--]            ; Compare the LSB of the array to zero and decrement
    bra        Z, msbLoop        ; If it is zero, branch to msbLoop

    inc2    W0, W0

msbDone:
    return


;***************************************************************************
; Function:     void _copyBI()
;
; PreCondition: 
;
; Input:         A and B, the destination and source
;
; Output:         A = B
;
; Side Effects: None
;
; Stack Req:     
;
; Overview:     Copies a value from one BigInt to another
;
; Note:            Function works
;***************************************************************************
    .global    __copyBI
__copyBI:
    mov        [W0+BIGINT_LENGTH_OFFSET],W2        ; Move length of bigint destination into W2
    mov        [W1+BIGINT_LENGTH_OFFSET],W4        ; Move length of bigint source into W4
    mov        [W0], W0        ; Get the address of the destination
    mov        [W1], W1        ; Get the address of the source
    add        W0, W2, W2        ; Add the dest length to the dest start address
    add        W1, W4, W4        ; Add the source length to the source start address

cLoop:
    mov        [W1++], [W0++]
    cp        W4, W1
    bra        Z, cZeroLoopTest
    cp        W2, W0
    bra        NZ, cLoop
    return

cZeroLoop:
    clr        [W0++]
cZeroLoopTest:
    cp        W2, W0
    bra        NZ, cZeroLoop
    return    

;***************************************************************************
; Function:     void _mulBI()
;
; PreCondition: _iA and _iB are loaded with the address of the LSB of each BigInt
;                _xA and _xB are loaded with the address of the MSB of the BigInt
;                _iR is loaded with the LSB address of the destination result memory
;                _iR memory must be zeroed and have enough space (_xB-_iB+_xA-_iA words)
;
; Input:         A and B, the BigInts to multiply
;
; Output:         R = A * B
;
; Side Effects: None
;
; Overview:     Performs the bulk multiplication of two BigInts
;
; Note:            Function works
;***************************************************************************
    .global    __mulBI
__mulBI:
    push    W8
    push    W9
    push    W10

    ; Decement xA, xB (to match termination case)
    ; W0 = xA + 2    (+2 for termination case)
    ; W1 used for iA
    ; W2 = xB + 2    (+2 for termination case)
    ; W3 used for iB
    ; W6:W7 used for multiplication result
    ; W8 used for iR
    ; W9 = 0x0000
    mov        [W2], W8            ; Store the LSB of the result in W8

    mov     [W1],W3                ; Point W3 to the LSB of arg2
    mov        [W1+BIGINT_LENGTH_OFFSET],W2            ; Move the MSB of arg2 to W2
    add     W2, W3, W2

    mov        [W0],W1                ; Move the LSB of arg1 to W1
    mov        [W0+BIGINT_LENGTH_OFFSET],W0            ; Move the MSB for arg1 to W0
    add     W0, W1, W0

    mov        W1,W10                ; Store the address of the LSB of arg1 in W10

    clr        W9

mLoopB:
    cp        W3, W2                ; Compare current iB and xB
    bra        Z, mDone
    
    inc2    W8, W8
    mov        [W3++], W5            ; Get current iB word
;    bra        Z, mLoopB            ; Skip this iB if it is zero
    dec2    W8, W4                ; W4 = iR for A loop
    mov        W10, W1                ; Load the address of the LSB of arg1

mLoopA:
    mul.uu    W5, [W1++], W6        ; W7:W6 = B * A
    add        W6, [W4], [W4++]    ; R = R + (B*A)
    addc    W7, [W4], [W4]
    bra        NC, mFinishedCarrying
    mov        W4, W6
mKeepCarrying:
    addc    W9, [++W6], [W6]    ; Add in residual carry to MSB of R and carry forward if it causes a carry out
    bra        C, mKeepCarrying
mFinishedCarrying:
    
    cp        W1, W0
    bra        NZ, mLoopA
    bra        mLoopB

mDone:
    pop        W10
    pop        W9
    pop        W8
    return


;***************************************************************************
; Function:     void _masBI()
;
; PreCondition: _iB is loaded with the LSB of the modulus BigInt
;                _xB is loaded with the MSB of the modulus BigInt
;                _wC is loaded with the 16 bit integer by which to multiply
;                _iR is the starting LSB of the decumulator BigInt
;
; Input:         B (BigInt) and C (16-bit int) to multiply
;
; Output:         R = R - (B * C)
;
; Side Effects: None
;
; Overview:     Performs a Multiply And Subtract function.  This is used in
;                the modulus calculation to save several steps.  A BigInt (iB/xB)
;                is multiplied by a single word and subtracted rather than
;                accumulated.
;
; Note:            Decumulator is the opposite of an accumulator,
;                if that wasn't obvious
;
; Note:            Function works
;***************************************************************************
    .global    __masBI
__masBI:
    inc2    W0, W0
    clr        W5                    ; Carry word

masLoop:
    subr    W5, [W3], [W3]        ; Subtract carry word from R
    clr        W5                    ; Clear carry word
    btss    SR, #C                ; If a borrow occured
    inc        W5, W5                ;    save 1 to the carry word
    
    mul.uu    W2, [W1++], W6        ; W7:W6 = B * C
    subr    W6, [W3], [W3++]    ; R = R - (B * C)
    btg        SR, #C
    addc    W7, W5, W5

    cpseq    W1, W0                ; Compare current B and xB
    bra        masLoop
    
    subr    W5, [W3], [W3]        ; Finish borrowing
    
masDone:
    return


