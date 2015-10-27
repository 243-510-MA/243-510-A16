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

;*************************************************************************
; FILE          math.s
;
; DESCRIPTION   This file contains miscellaneous math routines.
;
; ROUTINES
;               math_mod_exp()  - modular exponentiation
;               math_vect_squ() - squares two vectors
;               math_vect_mul() - multiplies two vectors
;               math_mod_red()  - modular reduction, x mod y
;
; CHANGES
;                - 09/08/09 mhe
;                    - fixed bug in math_mod_red()
;    
;               - 09/12/11 mhe
;                   - changed math_vect_squ(), math_vect_mul(), math_mod_red()
;

; include files

;.ifdef __dsPIC30F
;.include "p30Fxxxx.inc"
;.equiv PROC_INC_SET,1
;.endif

.ifdef __dsPIC33F
.include "p33Fxxxx.inc"
.equiv PROC_INC_SET,1
.endif

.ifdef __dsPIC33E
.include "p33Exxxx.inc"
.equiv PROC_INC_SET,1
.endif


; function declarations

.global math_mod_exp
.global math_vect_squ
.global math_vect_mul
.global math_mod_red

; external function declarations

.extern mont_decode
.extern mont_mod_exp
.extern mont_multiply
.extern mont_square

; code section

.text


;****************************************************************************
; ROUTINE        math_mod_exp
;
; DESCRIPTION
;               A multi-precision exponentiation function c = a^e mod N.
;               The result is input and returned in the top half of xbuf.
;
; PARAMETERS
;               w0  - pointer to xbuf, a working buffer returning the result
;                     in the top half (xdata, length 2m words)
;               w1  - pointer to a, the base (length m words)
;               w2  - pointer to e, the exponent
;               w3  - number of 8-bit bytes in the exponent
;               w4  - pointer to N, the modulus (ydata, length m words)
;               w5  - m, the number of 16-bit words in N
;               w6  - v = -N^{-1} mod 2^16
;               w7  - pointer to ybuf, a working buffer
;                     (ydata, length 2m words)
;               [w15-6] - pointer to RR = 2^{32m} mod N (length m words)
;
;               IF (CORCON< 0>) set
;               US (CORCON<12>) set
;
; RETURN VALUES
;
; REGISTERS
;               w0  - arg, param
;               w1  - arg, param
;               w2  - arg, param
;               w3  - arg, e pointer, param
;               w4  - arg, param
;               w5  - arg, param
;               w6  - arg, temp, param
;               w7  - arg, param
;               w8  - pointer to xbuf
;               w9  - pointer to top half of xbuf
;               w10 - pointer to ybuf
;               w11 - pointer to top half of ybuf
;               w12 - address of N
;               w13 - number of words in N
;               w14 - number of bytes in N, v
;               w15 - stack pointer
;               ACCAL - v, used by subroutine
;               ACCAH - address of ybuf, used by subroutine
;               ACCB - used by subroutine
;
; SUBROUTINES CALLED
;               mont_multiply()
;               mont_square()
;               mont_mod_exp()
;               mont_decode()
;
; STACK USED
;               29 words
;
; TIMING
;               ? cycles
;
; AUTHOR        N. A. Howgrave-Graham, NTRU Cryptosystems, Inc.
;               M. H. Etzel, Security Innovation, Inc.
;
; DATE          09/09/2003
;
; NOTES
;               - One could halve the size of the working buffer 
;                 ybuf, at the cost of doubling the time for 
;                 squaring (i.e. ending up roughly equivalent to 
;                 the time for a general multiplication). 
;
;               - This routine assumes the least significant word of
;                 a, N, RR, is in the lowest memory address.
;
;               - This routine assumes the MOST significant BYTE of
;                 e is in the lowest memory address.
;
;               - The vector, a, may be located in the top half of xbuf.
;                 If it is, it will be overwritten.
;
;               - The vector, RR, may be located in the bottom half of ybuf.
;                 If it is, it will be overwritten.
;
;               - This routine uses 2 level of (nested) DO loops.
; CHANGES

math_mod_exp:

; check if RR is in bottom of ybuf

    mov     w6, _ACCAL                  ;save v in ACCAL
    mov     [w15-6], w6                 ;get pointer to RR from stack
    dec     w5, w5                      ;number of words - 1 in RR
    cp      w6, w7                      ;address of RR == address of bottom
                                        ; half of ybuf?
    bra     z, math_mod_exp_find_nonzero_ebyte
                                        ;if so, skip copying RR

; copy RR into bottom of ybuf

    mov     w7, _ACCAH                  ;save address of ybuf
    repeat  w5                          ;for each word in RR
     mov    [w6++], [w7++]              ;copy word of RR into ybuf
    mov     _ACCAH, w7                  ;restore address of ybuf

; find most-significant nonzero exponent word

math_mod_exp_find_nonzero_ebyte:
    inc        w5, w6                        ; number of words in N
    add        w2, w3, w3                    ; pointer to byte after msbyte of e
math_mod_exp_find_nonzero_ebyte_loop:
    cp0.b    [--w3]                        ; nonzero e byte?
    bra        nz, math_mod_exp_nonzero_e    ; if nonzero, jump to start
    cp        w2, w3                        ; least-significant e byte?
    bra        nz, math_mod_exp_find_nonzero_ebyte_loop
                                        ; if not, loop

; all exponent words == 0: copy 1 into top half of c and return

    sl      w6, #2, w6                  ;number of bytes in xbuf
    add     w0, w6, w0                  ;pointer to address above xbuf
    repeat  w5                          ;for each remaining word of answer
    clr    [--w0]                      ;put 0 in word of answer
    mov     #1, w6                      ;constant 1
    mov     w6, [w0]                    ;put 1 in lsword of answer
    return                              ;return from math_mod_exp()

math_mod_exp_nonzero_e:

; save registers

    push.d  w8                          ;save w8, w9 on stack
    push.d  w10                         ;save w10, w11 on stack
    push.d  w12                         ;save w12, w13 on stack
    push    w14                         ;save w14 on stack

; save parameters

    sl      w6, w14                     ;number of bytes in N
    mov     w0, w8                      ;save address of xbuf
    add     w8, w14, w9                 ;save address of top half of xbuf
    mov     w7, w10                     ;save address of ybuf
    add     w10, w14, w11               ;save address of top half of ybuf
    mov     w4, w12                     ;save address of N
    mov     w6, w13                     ;save number of words in N

; check if base is in top of xbuf

    cp      w1, w9                      ;address of a == address of top half of
                                        ; xbuf?
    bra     z, math_mod_exp_check_exp   ;if so, skip copying a

; copy base to top of xbuf

    repeat  w5                          ;for each word in a
     mov    [w1++], [w9++]              ;copy a to top half of xbuf
    sub     w9, w14, w9                 ;pointer to top half of xbuf
    mov     w9, w1                      ;address of a in top half of xbuf

; check if exponent > 3

math_mod_exp_check_exp:
    mov        _ACCAL, w14                    ; save v
    sub        w3, w2, w3                    ; number of bytes - 1 in exponent
    bra        nz, math_mod_exp_e_gt_3        ; if more than one byte in exponent,
                                        ; jump since exponent > 3
    add        w2, w3, w6                    ; msbyte of e
    mov.b    [w6], w6                    ; e[0]
    cp        w6, #3                        ; exponent > 3?
    bra     gtu, math_mod_exp_e_gt_3    ; if so, jump
    bra        z, math_mod_exp_e_eq_3        ; is exponent == 3, jump
    cp         w6, #1                        ; exponent == 1?
    bra        z, math_mod_exp_done        ; if so, answer (a) is already in
                                        ; top of xbuf

; exponent = 2

; square base using Montgomery squaring

    mov     w11, w2                     ;address of top half of ybuf for temp
    mov     w12, w3                     ;address of N
    mov     w13, w4                     ;number of words in N
    mov     w14, w5                     ;v
    rcall   mont_square                 ;a * a * R^-1

; multiply by RR using Montgomery multiplication

    mov     w8, w0                      ;address of xbuf
    mov     w9, w1                      ;address of a^2 in top half of xbuf
    mov     w10, w2                     ;address of RR in bottom of ybuf
    mov     w12, w3                     ;address of N
    mov     w13, w4                     ;number of words in N
    mov     w14, w5                     ;v
    rcall   mont_multiply               ;a^2 = (a^2 * R^-1) * RR * R^-1
    bra     math_mod_exp_done           ;jump to done

; exponent = 3

math_mod_exp_e_eq_3:

; copy a to top half of ybuf

    mov     w11, w2                     ;address of top half of ybuf
    repeat  w5                          ;for each word in a
     mov    [w1++], [w2++]              ;copy word of a to top half of ybuf

; encode base into Montgomery's form

    mov     w9, w1                      ;address of base in top half of xbuf
    mov     w10, w2                     ;address of RR in bottom of ybuf
    mov     w12, w3                     ;address of N
    mov     w13, w4                     ;number of words in N
    mov     w14, w5                     ;v
    rcall   mont_multiply               ;aR = a * RR * R^-1

; square encoded base using Montgomery squaring

    mov     w8, w0                      ;address of xbuf
    mov     w9, w1                      ;address of aR in top half of xbuf
    mov     w10, w2                     ;address of bottom of ybuf for temp
    mov     w12, w3                     ;address of N
    mov     w13, w4                     ;number of words in N
    mov     w14, w5                     ;v
    rcall   mont_square                 ;a^2 * R = aR * aR * R^-1

; multiply by unencoded base using Montgomery multiplication

    mov     w8, w0                      ;address of xbuf
    mov     w9, w1                      ;address of a^2 * R in top half of xbuf
    mov     w11, w2                     ;address of a in top half of ybuf
    mov     w12, w3                     ;address of N
    mov     w13, w4                     ;number of words in N
    mov     w14, w5                     ;v
    rcall   mont_multiply               ;a^3 = (a^2 * R) * a * R^-1
    bra     math_mod_exp_done           ;jump to done

; exponent > 3

math_mod_exp_e_gt_3:

; save pointer to e, number of bytes in e

    inc     w3, w3                      ;number of bytes of exponent
    push.d  w2                          ;save address of e and number of words
                                        ; in e on stack

; encode base into Montgomery's form

    mov     w10, w2                     ;address of RR in bottom of ybuf
    mov     w12, w3                     ;address of N
    mov     w13, w4                     ;number of words in N
    mov     w14, w5                     ;v
    rcall   mont_multiply               ;aR = a * RR * R^-1

; exponentiate using Montgomery exponentiation

    mov     w8, w0                      ;address of xbuf
    mov     w9, w1                      ;address of aR in top half of xbuf
    pop.d   w2                          ;restore address of e and number of
                                        ; words in e from stack
    mov     w12, w4                     ;address of N
    mov     w13, w5                     ;number of words in N
    mov     w14, w6                     ;v
    mov     w10, w7                     ;address of ybuf
    rcall   mont_mod_exp                ;exponentiate in Montgomery's form

; decode result out of Montgomery's form

    mov     w8, w0                      ;address of xbuf
    mov     w9, w1                      ;address of result in top half of xbuf
    mov     w12, w2                     ;address of N
    mov     w13, w3                     ;number of words in N
    mov     w14, w4                     ;v
    rcall   mont_decode                 ;decode answer (into top half of xbuf)

math_mod_exp_done:
    pop     w14                         ;restore w14 from stack
    pop.d   w12                         ;restore w12, w13 from stack
    pop.d   w10                         ;restore w10, w11 from stack
    pop.d   w8                          ;restore w8, w9 from stack
    return                              ;return from math_mod_exp()


;****************************************************************************
; ROUTINE       math_vect_squ
;
; DESCRIPTION   This routine performs a multi-precision squaring function
;               c = a * a. To hold the result, the buffer corresponding to c 
;               should be 2m words long.  The integer corresponding to a 
;               should be m bits long.
;
; PARAMETERS
;               w0  -  pointer to the c buffer (length 2m words)
;               w1  -  pointer to a (xdata, length m words)
;               w2  -  pointer to temp b buffer (ydata, length m words)
;               w3  -  non-zero number of 16-bit words in a
;
;               IF (CORCON< 0>) set
;               US (CORCON<12>) set
;
; RETURN VALUES
;
; REGISTERS USED
;               w0  - points through the c buffer
;               w1  - points through the a buffer
;               w2  - points through the b buffer
;               w3  - number of words in a, b, column counter for do loops
;               w4  - double-term counter for the repeat loop
;               w5  - points to _ACCAL, then _ACCAH
;               w6  - word of a
;               w7  - word of b
;               w8  - xdata mac prefetch
;               w9  - unused
;               w10 - ydata mac prefetch
;               w11 - unused
;               w12 - unused
;               w13 - unused
;               w14 - unused
;               w15 - stack pointer
;               ACCA - accumulator for column results
;               ACCB - accumulator for double-term results
;
; SUBROUTINES CALLED
;
; STACK USED
;               2 words
;
; TIMING
;               ? cycles
;
; AUTHOR        N. A. Howgrave-Graham, NTRU Cryptosystems, Inc.
;               M. H. Etzel, Security Innovation, Inc.
;
; DATE          09/09/2003
;
; NOTES
;               - This routine does not check for zero-length vectors.
;
;               - This routine assumes the least significant word of
;                 a, c, is in the lowest memory address.
;
;               - The vector a may be located in the top half of the c buffer.
;                 If it is, it will be overwritten.
;
;               - This routine uses 1 level of DO loops.
;
; CHANGES
;               - 09/12/11 mhe
;                   - added conditional skip of do instruction only if
;                     one pass for errata in 33E

math_vect_squ:

; save registers

    push    w8                          ;save w8 on stack
    push    w10                         ;save w10 on stack

; init pointers

    mov     #_ACCAL, w5                 ;pointer to _ACCAL 

; copy a to b buffer

    mov     w1, w8                      ;pointer to a
    mov     w2, w10                     ;pointer to b buffer
    dec     w3, w6                      ;number of words - 1 in a
    repeat  w6                          ;for each word in a
     mov    [w8++], [w10++]             ;copy word of a to b buffer

; do first column separately to avoid mac prefetching out of bounds

    mov     [w2++], w6                  ;lsword of b
    mpy     w6*w6, A                    ;square lsword
    cp      w3, #1                      ;one word in a, b?
    bra     z, math_vect_squ_finish     ;if so, jump to finish
    mov     [w5], [w0++]                ;store the low 16 bits of A as output
    sftac   A, #16                      ;shift A to keep the high 16 bits 

; init for loop through the (m-1)th column: 1st half

    lsr     w3, w3                      ;number of double columns
    sub     #2, w3                      ;number of double columns left - 1
    bra     n, math_vect_squ_mth_column
                                        ;if no more double columns,
                                        ; skip to mth column
    clr     w4                          ;init double-term counter

.ifdef NO_DO_ONCE
    mov     #0, w8                      ;constant zero
    cpseq   w3, w8                      ;skip do instruction if do count = 0
.endif

    do      w3, math_vect_squ_loop1_end ;for number of words - 2 in b times 

     ; for a column without a square term

     mov    w1, w8                      ;set pointer to a[0]
     mov    w2, w10                     ;set pointer to b[j]
     clr    B, [w8]+=2, w6, [w10]-=2, w7
                                        ;clear B, prefetch a[i], b[j] for mac
     repeat w4                          ;for each double term in the column
      mac   w6*w7, B, [w8]+=2, w6, [w10]-=2, w7 
                                        ;a[i] * b[j], prefetch a[++i], b[--j]
     add    A                           ;add double-term sum to column results
     add    A                           ;add double-term sum to column results
     mov    [w5], [w0++]                ;store the low 16 bits of A as output
     sftac  A, #16                      ;shift A to keep high 16 bits

     ; for a column with a square term

     mov    w1, w8                      ;set pointer to a[0]
     add    w2, #2, w10                 ;set pointer to b[j]
     clr    B, [w8]+=2, w6, [w10]-=2, w7
                                        ;clear B, prefetch a[i], b[j] for mac
     repeat w4                          ;for each double term in the column
      mac   w6*w7, B, [w8]+=2, w6, [w10]-=2, w7 
                                        ;a[i] * b[j], prefetch a[++i], b[--j]
     add    A                           ;add double-term sum to column results
     add    A                           ;add double-term sum to column results
     mac    w6*w6, A                    ;add in square term
     mov    [w5], [w0++]                ;store the low 16 bits of A as output
     sftac  A, #16                      ;shift A to keep high 16 bits
     add    #4, w2                      ;increment starting point in b
math_vect_squ_loop1_end:
     inc    w4, w4                      ;double-term counter++

; process the mth column (no square term)

math_vect_squ_mth_column:
    mov    w1, w8                       ;set pointer to a[0]
    mov    w2, w10                      ;set pointer to b[j]
    clr    B, [w8]+=2, w6, [w10]-=2, w7
                                        ;clear B, prefetch a[i], b[j] for mac
    repeat w4                           ;for each double term in the column
     mac   w6*w7, B, [w8]+=2, w6, [w10]-=2, w7 
                                        ;a[i] * b[j], prefetch a[++i], b[--j]
    add    A                            ;add double-term sum to column results
    add    A                            ;add double-term sum to column results
    mov    [w5], [w0++]                 ;store the low 16 bits of A as output
    sftac  A, #16                       ;shift A to keep high 16 bits

; start loop through (2m-2)th column: 2nd half

.ifdef NO_DO_ONCE
    mov     #0, w8                      ;constant zero
    cpseq   w3, w8                      ;skip do instruction if do count = 0
.endif

    do      w3, math_vect_squ_loop2_end ;for number of words - 1 in b times 

     ; for a column with a square term

     dec    w4, w4                      ;double-term counter--
     add    w1, #2, w8                  ;set pointer to a[0]
     mov    w2, w10                     ;set pointer to b[m-1]
     clr    B, [w8]+=2, w6, [w10]-=2, w7
                                        ;clear B, prefetch a[i], b[j] for mac
     repeat w4                          ;for each term in the column
      mac   w6*w7, B, [w8]+=2, w6, [w10]-=2, w7 
                                        ;a[i] * b[j], prefetch a[++i], b[--j]
     add    A                           ;add double-term sum to column results
     add    A                           ;add double-term sum to column results
     mac    w6*w6, A                    ;add in square term
     mov    [w5], [w0++]                ;store the low 16 bits of A as output
     sftac  A, #16                      ;shift A to keep high 16 bits

     ; for a column without a square term

     add    w1, #4, w1                  ;increment starting point in a
     mov    w1, w8                      ;set pointer to a[0]
     mov    w2, w10                     ;set pointer to b[m-1]
     clr    B, [w8]+=2, w6, [w10]-=2, w7
                                        ;clear B, prefetch a[i], b[j] for mac
     repeat w4                          ;for each term in the column
      mac   w6*w7, B, [w8]+=2, w6, [w10]-=2, w7 
                                        ;a[i] * b[j], prefetch a[++i], b[--j]
     add    A                           ;add double-term sum to column results
     add    A                           ;add double-term sum to column results
     mov    [w5], [w0++]                ;store the low 16 bits of A as output
math_vect_squ_loop2_end:
     sftac  A, #16                      ;shift A to keep high 16 bits

; do last two columns separately to avoid mac prefetching out of bounds

    mov     [++w1], w6                  ;msword of a
    mac     w6*w6, A                    ;add last column to partial result
math_vect_squ_finish:
    mov     [w5++], [w0++]              ;store the low 16 bits of A as output
    mov     [w5], [w0]                  ;store the high 16 bits of A as output

; restore registers and return

    pop     w10                         ;restore w10 from stack
    pop     w8                          ;restore w8 from stack
    return                              ;return from math_vect_squ() 


;****************************************************************************
; ROUTINE       math_vect_mul
;
; DESCRIPTION
;               A multi-precision multiplication function c = a * b. 
;               To hold the result the buffer corresponding to c 
;               should be 2m words long.  The integer corresponding to a 
;               should be m bits long.
;
; PARAMETERS
;               w0  - pointer to the c buffer (length 2m words)
;               w1  - pointer to a (xdata, length m words)
;               w2  - pointer to b (ydata, length m words)
;               w3  - non-zero number of 16-bit words in a, b
;
;               IF (CORCON< 0>) set
;               US (CORCON<12>) set
;
; RETURN VALUES
;
; REGISTERS USED
;               w0  - points through the c buffer
;               w1  - points through the a buffer
;               w2  - points through the b buffer
;               w3  - number of words in a, b, column counter for do loops
;               w4  - term counter for the repeat loop
;               w5  - points to _ACCAL, then _ACCAH
;               w6  - word of a
;               w7  - word of b
;               w8  - xdata mac prefetch
;               w9  - unused
;               w10 - ydata mac prefetch
;               w11 - unused
;               w12 - unused
;               w13 - unused
;               w14 - unused
;               w15 - stack pointer
;               ACCA - accumulator for partial results
;
; SUBROUTINES CALLED
;
; STACK USED
;               2 words
;
; TIMING
;               ? cycles
;
; AUTHOR        N. A. Howgrave-Graham, NTRU Cryptosystems, Inc.
;               M. H. Etzel, Security Innovation, Inc.
;
; DATE          09/09/2003
;
; NOTES
;               - This routine does not check for zero-length vectors.
;
;               - This routine assumes the least significant word of
;                 a, b, c, is in the lowest memory address.
;
;               - The vector a may be located in the top half of the c buffer.
;                 If it is, it will be overwritten.
;
;               - This routine uses 1 level of DO loops.
;
; CHANGES
;               - 09/12/11 mhe
;                   - added conditional skip of do instruction only if
;                     one pass for errata in 33E

math_vect_mul:

; save registers

    push    w8                          ; save w8 on stack
    push    w10                         ; save w10 on stack

; init pointers

    mov     #_ACCAL, w5                 ; pointer to _ACCAL 

; do first column separately to avoid mac prefetching out of bounds

    mov     [w1], w6                    ; lsword of a
    mov     [w2++], w7                  ; lsword of b
    mpy     w6*w7, A                    ; product of lswords
    cp      w3, #1                      ; one word in a, b?
    bra     z, math_vect_mul_finish     ; if so, jump to finish
    mov     [w5], [w0++]                ; store the low 16 bits of A as output
    sftac   A, #16                      ; shift A to keep the high 16 bits 

; init for loop through the (m-1)th column: 1st half

    clr     w4                          ; init number of terms - 2 in column
    sub     #3, w3                      ; number of columns left - 1
    bra     n, math_vect_mul_second_half; if no more columns, skip to 2nd half

.ifdef NO_DO_ONCE
    mov     #0, w8                      ;constant zero
    cpseq   w3, w8                      ;skip do instruction if do count = 0
.endif

    do      w3, math_vect_mul_loop1_end ; for number of words - 2 in b times 
     mov    w1, w8                      ; set pointer to a[0]
     mov    w2, w10                     ; set pointer to b[j]
     movsac A, [w8]+=2, w6, [w10]-=2, w7
                                        ; prefetch a[i], b[j] for mac
     repeat w4                          ; for each term in the column
      mac   w6*w7, A, [w8]+=2, w6, [w10]-=2, w7 
                                        ; a[i] * b[j], prefetch a[++i], b[--j]
     mac    w6*w7, A                    ; mac final term in column
                                        ; (avoids mac prefetching out of bounds)
     mov    [w5], [w0++]                ; store the low 16 bits of A as output
     sftac  A, #16                      ; shift A to keep high 16 bits
     inc    w4, w4                      ; (number of terms - 2 in column)++
math_vect_mul_loop1_end:
     inc2   w2, w2                      ; increment starting point in b

; init for loop through the next m-1 columns: 2nd half

math_vect_mul_second_half:
    inc     w3, w3                      ; number of columns left - 1

.ifdef NO_DO_ONCE
    mov     #0, w8                      ;constant zero
    cpseq   w3, w8                      ;skip do instruction if do count = 0
.endif

    do      w3, math_vect_mul_loop2_end ; for number of words - 1 in b times 
     mov    w1, w8                      ; set pointer to a[0]
     mov    w2, w10                     ; set pointer to b[m-1]
     movsac A, [w8]+=2, w6, [w10]-=2, w7
                                        ; prefetch a[i], b[j] for mac
     repeat w4                          ; for each term in the column
      mac   w6*w7, A, [w8]+=2, w6, [w10]-=2, w7 
                                        ; a[i] * b[j], prefetch a[++i], b[--j]
     mac    w6*w7, A                    ; mac final term in column
                                        ; (avoids mac prefetching out of bounds)
     mov    [w5], [w0++]                ; store the low 16 bits of A as output
     sftac  A, #16                      ; shift A to keep high 16 bits
     dec    w4, w4                      ; (number of terms - 1 in column)--
math_vect_mul_loop2_end:
     inc2   w1, w1                      ; increment starting point in a

; do last two columns separately to avoid mac prefetching out of bounds

    mov     [w1], w6                    ; msword of a
    mov     [w2], w7                    ; msword of b
    mac     w6*w7, A                    ; add last column to partial result
math_vect_mul_finish:
    mov     [w5++], [w0++]              ; store the low 16 bits of A as output
    mov     [w5], [w0]                  ; store the high 16 bits of A as output

; restore registers and return

    pop     w10                         ; restore w10 from stack
    pop     w8                          ; restore w8 from stack
    return                              ; return from math_vect_mul()


;*************************************************************************
; ROUTINE       math_mod_red()
;
; DESCRIPTION   This routine performs a modular reduction, u mod v.
;
;               This routine performs division, without saving the
;               quotient.
;
; PARAMETERS
;               w0 - address of working buffer (length = no. of words in u),
;                    returning the result (length = no. of words in v)
;               w1 - address of lsword of u
;               w2 - number of words in u
;               w3 - address of v, lsword first (ydata)
;               w4 - number of words in v
;
; RETURN VALUES
;
; REGISTERS USED
;               w0  - pointer to working buffer, quotient word
;               w1  - pointer to u, no. words - 1 in quotient, remainder word
;               w2  - no. words in u, u[i-2]
;               w3  - address of v
;               w4  - no. words in v, no. words - 1 in v
;               w5  - address of low word of remainder in working buffer
;               w6  - temp, (q * v[n-2])L, v[j]
;               w7  - (q * v[n-2])H, q
;               w8  - v[n-2]
;               w9  - v[n-1]
;               w10 - u[i-1], pointer to v (prefetch for MAC)
;               w11 - u[i]
;               w12 - no. words - 2 in v
;               w13 - address of high word of remainder in working buffer
;               w14 - unused
;               w15 - sp
;
; SUBROUTINES CALLED
;
; STACK DEPTH
;               6 or 7 words
;
; TIMING
;               ? cycles
;
; AUTHOR        M. H. Etzel, Security Innovation, Inc.
;
; DATE          11/12/2003
;
; NOTES
;               - v must have a 1 in the most significant bit.
;
;               - The number of words in v must be > 1.
;
;               - The number of words in u must be >= the number of words
;                 in v.
;
;               - This routine assumes the least significant word of u, v
;                 is in the lowest memory address.
;
;               - This routine uses 2 levels of DO loops.
;
; CHANGES
;                - mhe 09/08/09
;                    - q must be tested after it is set to its max
;
;               - 09/12/11 mhe
;                   - added conditional skip of do instruction only if
;                     one pass for errata in 33E
;

math_mod_red:                           ;math_mod_red() subroutine

; save registers

    push.d  w8                          ;save w8, w9 on stack
    push.d  w10                         ;save w10, w11 on stack
    push.d  w12                         ;save w12, w13 on stack

; copy u into the working buffer if it is not already there

    sl      w2, w6                      ;no. of bytes in u
    add     w0, w6, w13                 ;address of word after last word in
                                        ; working buffer

    cp      w0, w1                      ;address of u == address of working buf?
    bra     z, math_mod_red_init        ;if so, skip copying u to working buffer

    dec     w2, w6                      ;number of words - 1 in u
    repeat  w6                          ;for each word in u
     mov    [w1++], [w0++]              ;copy word of u to working buffer

; save parameters and init registers

math_mod_red_init:
    sub     w2, w4, w1                  ;no. of quotient words - 1 = 
                                        ; no. words in u - no. words in v
    sl      w4, w6                      ;number of bytes in v
    sub     w13, w6, w5                 ;address of low word of remainder in
                                        ; working buffer
    add     w3, w6, w6                  ;address of word after last word of v
    mov     [--w6], w9                  ;v[n-1]
    mov     [--w6], w8                  ;v[n-2]
    dec     w4, w4                      ;number of words - 1 in v
    dec     w4, w12                     ;number of words - 2 in v
    clr     w11                         ;init u[j] = 0 since not in working
                                        ; buffer

; for each quotient digit, update the remainder

.ifdef NO_DO_ONCE
    mov     #0, w10                     ;constant zero
    cpseq   w1, w10                     ;skip do instruction if do count = 0
.endif

    do      w1, math_mod_red_q_loop_end ;for each quotient word

     mov    [--w13], w10                ;u[j-1]
     mov    [--w13], w2                 ;u[j-2]
     add    #2*2, w13                   ;reset pointer

     ; if u[j] == v[n-1], set q[j] = 0xffff
     ; else set q[j] = (u[j](2^16) + u[j-1]) / v[n-1]

     cp     w11, w9                     ;u[j] == v[n-1]?
     bra    z, math_mod_red_set_q_max   ;if so, jump to set q to its max

     ; divide two digits of u by one digit of v for estimate of q

     repeat #17                         ;for division
      div.ud w10, w9                    ;(u[i]*2^16 + u[i-1]) / v[n-1]
     cp0    w0                          ;q == 0?
     bra    z, math_mod_red_update_r_done
                                        ;skip updating remainder
     bra    math_mod_red_test_q

math_mod_red_set_q_max:
     setm   w0                          ;set q = 0xffff
     mul.uu w0, w9, w6                  ;q * v[n-1]
     sub    w10, w6, w1                 ;u[j-1] - low product (low remainder)
     subb   w11, w7, w7                 ;u[j] - high product (high remainder)
     bra    nz, math_mod_red_update_r   ;if high remainder != 0 then
                                        ; q[j] * v[n-2] cannot be larger than
                                        ; (2^16) * remainder + u[j-2] so skip
                                        ; test of estimate of q

     ; test if the estimate of the quotient is good
     ; while q[j]v[n-2] > ((u[j](2^16) + u[j-1] - q[j]v[n-1])(2^16) + u[j-2])
     ;   q[j]--
     ; Note: This algorithm guarantees that q will never be more than two too
     ;       high, and will never be too low.

math_mod_red_test_q:
     mul.uu w0, w8, w6                  ;q * v[n-2]
     cp     w7, w1                      ;compare high product with remainder
     bra    nz, math_mod_red_test2_q    ;if not equal, skip comparing low
                                        ; product and u[j-2]
     cp     w6, w2                      ;compare low product with u[j-2]
math_mod_red_test2_q:
     bra    leu, math_mod_red_update_r  ;if q is ok, skip to update remainder

     ; estimate of q is too large: reduce q and increase r

     dec    w0, w0                      ;q -= 1
     add    w1, w9, w1                  ;r += v[n-1] (since q--)
     bra    nc, math_mod_red_test_q     ;if no carry, loop to check new q
                                        ; (if carry, then the remainder
                                        ;  is >= 2^16 so q[j] * v[n-2]
                                        ;  cannot be larger than
                                        ;  (2^16) * remainder + u[j-2],
                                        ;  so skip retest of estimate of q)

     ; u -= q[j]v

math_mod_red_update_r:
     mov    w3, w10                     ;pointer to v
     mov    w5, w1                      ;pointer to remainder in working buffer
     mov    w0, w7                      ;q
     clr    A, [w10]+=2, w6             ;clear ACCA, prefetch u[i-n+1]
     bset   _SR, #C                     ;clear borrow

.ifdef NO_DO_ONCE
     mov    #0, w0                      ;constant zero
     cpseq  w12, w0                     ;skip do instruction if do count = 0
.endif

     do     w12, math_mod_red_update_r_loop_end
                                        ;for each word in v except the last
      mac   w6*w7, A, [w10]+=2, w6      ;add u[i-n+1] * q to partial results
      mov   _ACCAL, w0                  ;low word of product
      sftac A, #16                      ;keep high word of product
math_mod_red_update_r_loop_end:
      subbr w0, [w1], [w1++]            ;subtract product word from remainder
                                        ; word
     mac    w6*w7, A                    ;q * v[n-1]
     mov    _ACCAL, w0                  ;low word of product
     subbr  w0, [w1], [w1++]            ;subtract product word from remainder
                                        ; word
     mov    _ACCAH, w0                  ;high word of product
     subbr  w0, w11, w11                ;subtract product word from last
                                        ; remainder word
     bra    c, math_mod_red_update_r_done
                                        ;if no borrow, skip correction of
                                        ; remainder

    ; the result is negative, so add back v

     mov    w3, w10                     ;pointer to v
     mov    w5, w1                      ;pointer to remainder in working buffer
     bclr   _SR, #C                     ;clear carry
     
.ifdef NO_DO_ONCE
     mov    #0, w0                      ;constant zero
     cpseq  w4, w0                      ;skip do instruction if do count = 0
.endif

     do     w4, math_mod_red_correct_r_loop_end
                                        ;for each word in v
      mov   [w10++], w0                 ;v[j]
math_mod_red_correct_r_loop_end:
      addc  w0, [w1], [w1++]            ;add word of v to word of remainder

    ; get ready for next quotient digit

math_mod_red_update_r_done:
     mov    [--w13], w11                ;get new u[i]
math_mod_red_q_loop_end:
     dec2   w5, w5                      ;(address of low word of remainder in
                                        ; working buffer)--

math_mod_red_done:
    pop.d   w12                         ;restore w12, w13 from stack
    pop.d   w10                         ;restore w10, w11 from stack
    pop.d   w8                          ;restore w8, w9 from stack
    return                              ;return from math_mod_red()


.end
