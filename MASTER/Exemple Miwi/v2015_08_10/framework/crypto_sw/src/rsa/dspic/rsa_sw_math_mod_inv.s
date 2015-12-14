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
; FILE          math_mod_inv.s
;
; DESCRIPTION   This file contains math routines to compute a modular
;                inverse.
;
; ROUTINES
;               math_mod_inv()  - calculates a modular inverse, x^-1 mod y.
;               math_vect_cmp() - compares magnitude of two vectors
;               math_vect_0_1() - checks for vector equal to 0 or 1
;
; CHANGES
;                - 04/14/2006 mhe
;                    - changed math_mod_inv()
;
;               - 09/12/2011 mhe
;                   - changed math_mod_inv()
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

.global math_mod_inv

; code section

.text


;*************************************************************************
; ROUTINE       math_mod_inv()
;
; DESCRIPTION   This routine calculates a modular inverse, x^-1 mod y.
;
;               The overall binary Euclidean algorithm used starts with
;               two numbers, x and y, not both even, and for each iteration:
;               1) removes all multiples of two from the larger number, and
;               2) replaces the larger with the larger - smaller.
;               When the result is 1 or 0, the algorithm stops with the
;               numbers being relatively prime or not, respectively.
;
;               To modify this to an extended binary Euclidean algorithm
;               to determine modular inverses, the numbers described above
;               become x3 and y3, and the following equations and relations
;               hold at the beginning of each iteration (inv_loop):
;
;                       x1(x) + x2(y) = x3
;                       y1(x) + y2(y) = y3
;
;                       where 0 <= x1, y1 <= y
;                                -x <= x2, y2 <= 0
;                             0 <= x3, y3 <= max(x, y)
;
;               Notation: X = (x1, x2, x3)  Y = (y1, y2, y3)
;
;               Steps 1) and 2) above now operate on X and Y.
;
;               The detailed steps for implementation are:
;
;               0) Init X = (1, 0, x), Y = (y, 1-x, y).
;
;               1a) If x3 < y3, X <-> Y
;               1b) While x3 is even:
;                   if x1, x2 even then X <- (x1/2, x2/2, x3/2)
;                   else X <- ((x1+y)/2, (x2-x)/2, x3/2)
;                   Note that: if y is odd,  x1, x2 even iff x1 even
;                          if y is even, x1, x2 even iff x2 even
;               1c) If x3 == 1, then x and y are relatively prime and
;                   x1 = x^-1 mod y.
;
;               2a) If x3 < y3, X <-> Y
;               2b) X <- X-Y
;               2c) If x1 < 0 (and therefore x2 > 0):
;                   x1 += y, x2 -= x
;               2d) If x3 == 1, then x and y are relatively prime and
;                   x1 = x^-1 mod y.  If x3 == 0, then x and y are not
;                   relatively prime.  If x3 == any other value, go to 1a).
;
; PARAMETERS
;               w0 - address of lsword of x
;               w1 - address of lsword of y
;               w2 - number of words in x, y
;               w3 - address of working buffers
;
; RETURN VALUES
;               w0 - 1: x, y relatively prime
;                    0: x, y not relatively prime
;               w1 - address of answer
;
; REGISTERS USED
;               w0  - address of x, param
;               w1  - address of y, param
;               w2  - no. of words in x, y; no. of significant words in x3, y3
;               w3  - pointer to working buffers, temp
;               w4  - constant 1, constant 0
;               w5  - unused
;               w6  - unused
;               w7  - no. of words in x3, y3
;               w8  - address of x
;               w9  - address of y
;               w10 - number of words in x, y
;               w11 - address of x1
;               w12 - address of y1
;               w13 - lsword of y
;               w14 - no. of bytes in buffers for x1, x2, x2, y1, y2, y3
;               w15 - sp
;
; SUBROUTINES CALLED
;               math_vect_cmp()
;               math_vect_0_1()
;
; STACK DEPTH
;               9 words
;
; TIMING
;               ? cycles
;
; AUTHOR        M. H. Etzel, Security Innovation, Inc.
;
; DATE          10/27/2003
;
; NOTES
;               - It is assumed that x, y are not both even.
;
;               - x, y must include an msword = 0 to allow the routine
;                 to handle negative numbers correctly, not included in
;                 the word count passed in w2.
;
;               - This routine assumes the least significant word of x, y
;                 is in the lowest memory address.
;
;               - The working buffers must be 6(n+1) 16-bit words long,
;                 where n is the number of 16-bit words in x or y.
;                   - x1 is n+1 words
;                   - x2 is n+1 words
;                   - x3 is n+1 words
;                   - y1 is n+1 words
;                   - y2 is n+1 words
;                   - y3 is n+1 words
;
;               - This routine uses 1 level of DO loops.
;
; CHANGES
;                - 04/14/2006 mhe
;                    - added workaround for 33F exch bug and made it
;                      conditionally assemble using NO_EXCH
;
;               - 09/12/11 mhe
;                   - added conditional skip of do instruction only if
;                     one pass for errata in 33E
;

math_mod_inv:                           ;math_mod_inv() subroutine

; save parameters and init registers

    push.d  w8                          ;save w8, w9 on stack
    push.d  w10                         ;save w10, w11 on stack
    push.d  w12                         ;save w12, w13 on stack
    push    w14                         ;save w14 on stack
    mov     w2, w7                      ;init no. of words in x3, y3
    mov     w0, w8                      ;address of x
    mov     w1, w9                      ;address of y
    mov     w2, w10                     ;no. of words - 1 in buffers for x1,
                                        ; x2, x3, y1, y2, y3
    mov     w3, w11                     ;address of x1
    mov     [w1], w13                   ;lsword of y
    inc     w2, w14                     ;no. of words in buffers for x1, x2,
                                        ; x3, y1, y2, y3
    sl      w14, #1, w14                ;no. of bytes in buffers for x1, x2,
                                        ; x3, y1, y2, y3
    dec     w2, w2                      ;no. of words - 2 in x1, x2, x3, y1,
                                        ; y2, y3

; initialize x1, x2, x3, y1, y2, y3 working vectors

; x1 = 1

    repeat  w10                         ;for each word in x1
     clr    [w3++]                      ;init x1 word = 0
    inc     [w11], [w11]                ;set lsword of x1 = 1

; x2 = 0

    repeat  w10                         ;for each word of x2
     clr    [w3++]                      ;init x2 word = 0

; x3 = x

    repeat  w10                         ;for each word of x3
     mov    [w0++], [w3++]              ;init x3 word = x word

; y1 = y

    mov     w3, w12                     ;save address of y1
    repeat  w10                         ;for each word of y1
     mov    [w1++], [w3++]              ;init y1 word = y word

; y2 = 1-x

    mov     w8, w0                      ;address of x
    mov     #1, w4                      ;constant 1
    sub     w4, [w0++], [w3++]          ;1 - lsword of x
    clr     w4                          ;constant 0
    repeat  w2                          ;for each remaining word of y2
     subb   w4, [w0++], [w3++]          ;0 - word of x

; y3 = y

    mov     w9, w1                      ;address of y
    repeat  w10                         ;for each word of y3
     mov    [w1++], [w3++]              ;init y3 word = y word

math_mod_inv_loop:

    ; if x3 < y3, swap x1 and y1 addresses

    sl      w14, #1, w1                 ;2 * no. bytes in buffers for x1, x2,
                                        ; x3, y1, y2, y3
    add     w11, w1, w0                 ;address of x3
    add     w12, w1, w1                 ;address of y3
    mov     w7, w2                      ;no. of words in x3, y3
    rcall   math_vect_cmp               ;x3 < y3 ?
    bra     geu, math_mod_inv_x3_ge_y3_1
                                        ;if not, skip
.ifdef NO_EXCH

    xor        w11, w12, w12                ;swap x1 and y1 addresses
    xor        w11, w12, w11
    xor        w11, w12, w12

.else

    exch    w11, w12                    ;swap x1 and y1 addresses

.endif

math_mod_inv_x3_ge_y3_1:
    mov     w2, w7                      ;save number of words in x3, y3

    ; if x3 is odd, skip halving x1, x2, x3

    add     w11, w14, w1                ;address of x2
    add     w1, w14, w0                 ;address of x3
    btsc    [w0], #0                    ;if x3 is even, skip next instruction
     bra    math_mod_inv_x3_odd         ;skip halving

    ; if y is odd then if x1 is odd, adjustment to x1, x2 needed
    ; if y is even then if x2 is odd, adjustment to x1, x2 needed

math_mod_inv_halve_loop:
     mov    [w11], w0                   ;lsword of x1
     btss   w13, #0                     ;if y is odd, skip next instruction
      mov   [w1], w0                    ;lsword of x2
     btss   w0, #0                      ;if lsword is odd, skip next instruction
      bra   math_mod_inv_halve          ;jump to halve, skipping adjustments

     ; make x1 mod y even (x1 += y)

     mov    w11, w0                     ;address of x1
     mov    w9, w1                      ;address of y
     bclr   _SR, #C                     ;clear carry
     do     w10, math_mod_inv_x1_plus_y_loop1_end
                                        ;for each word in x1
      mov   [w1++], w2                  ;get word of y
math_mod_inv_x1_plus_y_loop1_end:
      addc  w2, [w0], [w0++]            ;word of x1 += word of y

     ; make x2 mod x even (x2 -= x)

     mov    w8, w1                      ;address of x
     bset   _SR, #C                     ;set no borrow
     do     w10, math_mod_inv_x2_minus_x_loop1_end
                                        ;for each word in x1
      mov   [w1++], w2                  ;get word of x
math_mod_inv_x2_minus_x_loop1_end:
      subbr w2, [w0], [w0++]            ;word of x2 -= word of x

     ; halve x1, x2, x3

math_mod_inv_halve:
     add    w11, w14, w0                ;address of x2
     dec    w10, w1                     ;number of words left in x1, x2 buffers
     asr    [--w0], [w0]                ;halve word with sign extension
     repeat w1                          ;for each remaining word of x1 buffer
      rrc   [--w0], [w0]                ;halve word
     add    w0, w14, w0                 ;address of x2
     add    w0, w14, w0                 ;address of x3
     asr    [--w0], [w0]                ;halve word with sign extension
     repeat w1                          ;for each remaining word of x2 buffer
      rrc   [--w0], [w0]                ;halve word
     add    w0, w14, w0                 ;address of x3
     sl     w7, w1                      ;number of bytes in x3
     add    w0, w1, w0                  ;address of most significant word of x3
     dec    w7, w1                      ;number of words left in x3 
     asr    [w0], [w0]                  ;halve word with sign extension
     repeat w1                          ;for each remaining word of x3
      rrc   [--w0], [w0]                ;halve word

     btst   [w0], #0                    ;check lsb of x3
     bra    z, math_mod_inv_halve_loop  ;loop if x3 is even

math_mod_inv_x3_odd:

; if x3 == 1 (x3 cannot be 0 at this step), algorithm done

    mov     w7, w1                      ;number of words in x3
    rcall   math_vect_0_1               ;compare x3 to 1
    cp      w0, #1                      ;is x3 == 1?
    bra     z, math_mod_inv_done        ;if so, jump to end

; if x3 < y3, swap x1 and y1 addresses

    sl      w14, #1, w1                 ;2 * no. bytes in buffers for x1, x2,
                                        ; x3, y1, y2, y3
    add     w11, w1, w0                 ;address of x3
    add     w12, w1, w1                 ;address of y3
    mov     w7, w2                      ;no. of words in x3, y3
    rcall   math_vect_cmp               ;x3 < y3 ?
    bra     geu, math_mod_inv_x3_ge_y3_2
                                        ;if not, skip
.ifdef NO_EXCH

    xor        w11, w12, w12                ;swap x1 and y1 addresses
    xor        w11, w12, w11
    xor        w11, w12, w12

.else

    exch    w11, w12                    ;swap x1 and y1 addresses

.endif

math_mod_inv_x3_ge_y3_2:
    mov     w2, w7                      ;save number of words in x3, y3

; x2 -= y2, x1 -= y1

    add     w11, w14, w0                ;address of x2
    add     w12, w14, w1                ;address of y2
    bset    _SR, #C                     ;set no borrow
    do      w10, math_mod_inv_x2_minus_y2_loop_end
                                        ;for each word in x2
     mov    [w1++], w2                  ;get word of y2
math_mod_inv_x2_minus_y2_loop_end:
     subbr  w2, [w0], [w0++]            ;word of x2 -= word of y2

    mov     w11, w0                     ;address of x1
    mov     w12, w1                     ;address of y1
    bset    _SR, #C                     ;set no borrow
    do      w10, math_mod_inv_x1_minus_y1_loop_end
                                        ;for each word in x1
     mov    [w1++], w2                  ;get word of y1
math_mod_inv_x1_minus_y1_loop_end:
     subbr  w2, [w0], [w0++]            ;word of x1 -= word of y1

; if x1 < 0, adjust x1, x2

    bra     c, math_mod_inv_sub_x3      ;jump if x1 >= 0 (no borrow)

     ; x1 += y

    mov     w11, w0                     ;address of x1
    mov     w9, w1                      ;address of y
    bclr    _SR, #C                     ;clear carry
    do      w10, math_mod_inv_x1_plus_y_loop2_end
                                        ;for each word in x1
     mov    [w1++], w2                  ;get word of y
math_mod_inv_x1_plus_y_loop2_end:
     addc   w2, [w0], [w0++]            ;word of x1 += word of y

     ; x2 -= x

    add     w11, w14, w0                ;address of x2
    mov     w8, w1                      ;address of x
    bset    _SR, #C                     ;set no borrow
    do      w10, math_mod_inv_x2_minus_x_loop2_end
                                        ;for each word in x1
     mov    [w1++], w2                  ;get word of x
math_mod_inv_x2_minus_x_loop2_end:
     subbr  w2, [w0], [w0++]            ;word of x2 -= word of x

math_mod_inv_sub_x3:

; x3 -= y3

    sl      w14, #1, w3                 ;2 * no. bytes in buffers for x1, x2,
                                        ; x3, y1, y2, y3
    add     w11, w3, w0                 ;address of x3
    add     w12, w3, w1                 ;address of y3
    bset    _SR, #C                     ;set no borrow
    do      w7, math_mod_inv_x3_minus_y3_loop_end
                                        ;for each word in x3
     mov    [w1++], w2                  ;get word of y3
math_mod_inv_x3_minus_y3_loop_end:
     subbr  w2, [w0], [w0++]            ;word of x3 -= word of y3

; if x3 != 1 and x3 != 0, continue

    add     w11, w3, w0                 ;address of x3
    mov     w7, w1                      ;number of words in x3
    rcall   math_vect_0_1               ;compare x3 to 1
    cp      w0, #1                      ;compare return code to 1
    bra     gtu, math_mod_inv_loop      ;loop if x3 != 0 or 1

math_mod_inv_done:
    mov     w11, w1                     ;return address of answer
    pop     w14                         ;restore w14 from stack
    pop.d   w12                         ;restore w12, w13 from stack
    pop.d   w10                         ;restore w10, w11 from stack
    pop.d   w8                          ;restore w8, w9 from stack
    return                              ;return from math_mod_inv()


;*************************************************************************
; ROUTINE       math_vect_cmp()
;
; DESCRIPTION   This routine compares two vectors of the same length and
;               returns whether the first is smaller than, equal to, or
;               larger than the second.
;
; PARAMETERS
;               w0 - address of lsword of a
;               w1 - address of lsword of b
;               w2 - number of words in a, b
;
; RETURN VALUES
;               status flags indicate relation of vector a to vector b
;               w2 - number of significant words in largest vector
;                    (does not include mswords of zeros)
;
; REGISTERS USED
;               w0  - address of a
;               w1  - pointer to b
;               w2  - max no. of nonzero words in a, b
;               w3  - constant 0
;               w4  - pointer to a
;               w5  - temp
;               w6  - decrement value of max. no. of nonzero word counter
;               w7  - CANNOT BE USED
;               w8  - unused
;               w9  - unused
;               w10 - unused
;               w11 - unused
;               w12 - unused
;               w13 - unused
;               w14 - unused
;               w15 - sp
;
; SUBROUTINES CALLED
;
; STACK DEPTH
;               0 words
;
; TIMING
;               ? cycles
;
; AUTHOR        M. H. Etzel, Security Innovation, Inc.
;
; DATE          10/28/2003
;
; NOTES
;               - This routine assumes the least significant word of a
;                 vector is in the lowest memory address.
;
;               - This routine does not check for zero length vectors.
;
;               - This routine MUST PRESERVE w7 (for math_mod_inv()).
;
; CHANGES
;

math_vect_cmp:                          ;math_vect_cmp() subroutine
    sl      w2, w3                      ;no. of bytes in a, b
    add     w0, w3, w4                  ;address above a
    add     w1, w3, w1                  ;address above b
    clr     w3                          ;constant 0
    mov     #1, w6                      ;significant word counter decrement
math_vect_cmp_loop:
    mov     [--w4], w5                  ;word of a
    cp      w5, [--w1]                  ;compare to word of b
    bra     nz, math_vect_cmp_done      ;if not equal, jump to done
    cpseq   w5, w3                      ;if the identical words == 0, skip
                                        ; next instruction
     clr    w6                          ;no more reduction of number of nonzero
                                        ; words
    sub     w2, w6, w2                  ;update significant word counter
    cp      w4, w0                      ;checked all words?
    bra     nz, math_vect_cmp_loop      ;if not, loop
math_vect_cmp_done:
    return                              ;return from math_vect_cmp()
    

;*************************************************************************
; ROUTINE       math_vect_0_1()
;
; DESCRIPTION   This routine determines if vector is equal to 0 or 1.
;
; PARAMETERS
;               w0 - address of lsword of a
;               w1 - number of words in a
;
; RETURN VALUES
;               w0 - 0 = match (vector == 0)
;                    1 = match (vector == 1)
;                    other = no match (vector != 0 or 1)
;               w2 - number of significant words in largest vector
;                    (does not include mswords of zeros)
;
; REGISTERS USED
;               w0  - address of a, return code
;               w1  - word counter
;               w2  - pointer to a
;               w3  - unused
;               w4  - unused
;               w5  - unused
;               w6  - unused
;               w7  - unused
;               w8  - unused
;               w9  - unused
;               w10 - unused
;               w11 - unused
;               w12 - unused
;               w13 - unused
;               w14 - unused
;               w15 - sp
;
; SUBROUTINES CALLED
;
; STACK DEPTH
;               0 words
;
; TIMING
;               ? cycles
;
; AUTHOR        M. H. Etzel, Security Innovation, Inc.
;
; DATE          10/28/2003
;
; NOTES
;               - This routine assumes the least significant word of the
;                 vector is in the lowest memory address.
;
;               - This routine does not check for zero-length vectors.
;
;               - This routine MUST PRESERVE w7 (for math_mod_inv()).
;
; CHANGES
;

math_vect_0_1:                          ;math_vect_0_1() subroutine
    mov     w0, w2                      ;init pointer to vector

; check if least significant word is 0 or 1, putting into return code

    mov     [w2++], w0                  ;lsword of test vector
    cp      w0, #1                      ;compare lsword to 1
    bra     gtu, math_vect_0_1_done     ;if not, jump to done

; check if remaining words are 0

math_vect_0_1_zero_test_loop:
    dec     w1, w1                      ;word counter--
    bra     z, math_vect_0_1_done       ;if no more words, jump to done
    cp0     [w2++]                      ;word of vector == 0?
    bra     z, math_vect_0_1_zero_test_loop
                                        ;if so, loop
    mov     #2, w0                      ;set no match return code
math_vect_0_1_done:
    return                              ;return from math_vect_0_1()
    

.end
