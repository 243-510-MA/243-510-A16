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

;****************************************************************************
; FILE          mont.s
;
; DESCRIPTION   This file contains routines for Montgomery operations in
;               support of modular exponentiation and modular reduction.
;
; ROUTINES
;               mont_mod_exp()  - modular exponentiation in Montgomery's form
;               mont_square()   - squaring in Montgomery's form
;               mont_multiply() - multiplying in Montgomery's form
;               mont_decode()   - decodes vector out of Montgomery's form
;               mont_reduce()   - Montgomery reduction
;               mont_calc_v()   - calculates v = -N^-1 mod 2 ^16
;
; CHANGES
;               - 05/13/2004 mhe
;                   - changed mont_mod_exp()
;
;               - 09/12/11 mhe
;                   - changed mont_mod_exp(), mont_reduce()
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

.global mont_mod_exp
.global mont_square
.global mont_multiply
.global mont_decode
.global mont_reduce
.global mont_calc_v

;extern funtion declarations

.extern math_vect_mul
.extern math_vect_squ

; code section

.text


;****************************************************************************
; ROUTINE        mont_mod_exp
;
; DESCRIPTION
;               A multi-precision exponentiation function c = a^e 
;               mod N, where a and c are encoded in Montgomery's form.
;               The result is input and returned in the top half of xbuf.
;
; PARAMETERS
;               w0 - pointer to xbuf, a working buffer returning the result
;                    in the top half (x-data, length 2m words)
;               w1 - pointer to a, the encoded base
;               w2 - pointer to e, the exponent
;               w3 - the number of 8-bit bytes in exponent
;               w4 - pointer to N, the modulus (y-data, length m words)
;               w5 - m, the number of 16-bit words in the modulus
;               w6 - v = -N^{-1} mod 2^16
;               w7 - pointer to ybuf, a working buffer
;                    (y-data, length 2m words)
;
;               IF (CORCON< 0>) set
;               US (CORCON<12>) set
;
; RETURN VALUES
;
; REGISTERS USED
;               w0  - arg, param
;               w1  - arg, param
;               w2  - arg, param
;               w3  - arg, e pointer, param
;               w4  - arg, param
;               w5  - arg, param
;               w6  - arg, leftmost one position
;               w7  - arg
;               w8  - pointer to xbuf
;               w9  - pointer to top half of xbuf
;               w10 - pointer to ybuf
;               w11 - pointer to top half of ybuf
;               w12 - address of last byte of e
;               w13 - tmp, pointer to current e byte
;               w14 - current bit position in current e byte
;               w15 - stack pointer
;               ACCA - tmp, used by subroutine
;               ACCB - used by subroutine
;
; SUBROUTINES CALLED
;               mont_multiply()
;               mont_square()
;
; STACK USED
;               20 words
;
; TIMING
;               ? cycles
;
; AUTHOR        N. A. Howgrave-Graham, NTRU Cryptosystems, Inc.
;               M. H. Etzel, Security Innovation, Inc.
;
; DATE          11/03/2003
;
; NOTES
;               - One could halve the size of the working buffer 
;                 ybuf, at the cost of doubling the time for 
;                 squaring (i.e. ending up roughly equivalent to 
;                 the time for a general multiplication). 
;
;               - This routine assumes the least significant word of
;                 c, a, N, RR, is in the lowest memory address.
;
;               - This routine assumes the MOST significant BYTE of
;                 e is in the lowest memory address.
;
;               - The vector, a, may be located in the top half of xbuf.
;                 If it is, it will be overwritten there, but preserved in
;                 the top half of ybuf.
;
;               - The vector, a, may be located in the top half of ybuf.
;
;               - This routine does not check for the number of e words == 0.
;
;               - This routine uses 2 levels of (nested) DO loops.
;
; CHANGES
;               - 05/13/2004 mhe
;                   - added nop to mont_mod_exp_bit_loop_end loop so that
;                     rcall was not penultimate instruction in loop
;                   - ensured bra didn't branch to last instruction in
;                     mont_mod_exp_bit_loop
;
;               - 09/12/11 mhe
;                   - added conditional skip of do instruction only if
;                     one pass for errata in 33E
;

mont_mod_exp:
    mov     w6, _ACCAL                  ;save v temporarily in ACCAL

; find most-significant nonzero exponent byte

    clr        w6                            ; zero high byte of temp
    add        w2, w3, w3                    ; pointer to byte after msbyte of e
mont_mod_exp_find_nonzero_ebyte_loop:
    mov.b    [--w3], w6                    ; get next byte of e
    ff1l    w6, w6                        ; find leftmost 1 in a byte
    bra        nc, mont_mod_exp_nonzero_e    ; if nonzero, jump to start
    cp        w2, w3                        ; checked least significant e byte?
    bra        nz, mont_mod_exp_find_nonzero_ebyte_loop
                                        ; if not, loop

; all exponent words == 0:
;  form (1 * R) mod N = -N in top half of c, and return

    add     w0, w5, w0                  ;pointer to quarter through xbuf
    add     w0, w5, w0                  ;pointer to halfway through xbuf
    clr     w3                          ;constant 0
    dec     w5, w5                      ;number of words - 1 in N
    bset    _SR, #C                     ;set no borrow
    repeat  w5                          ;for each word in N
     subb   w3, [w4++], [w0++]          ;store -N in top half of xbuf
    return                              ;return from mont_mod_exp()

; save registers

mont_mod_exp_nonzero_e:
    push.d  w8                          ;save w8, w9 on stack
    push.d  w10                         ;save w10, w11 on stack
    push.d  w12                         ;save w12, w13 on stack
    push    w14                         ;save w14 on stack

; save parameters

    mov     w0, w8                      ;save address of xbuf
    sl      w5, w13                     ;number of bytes in N
    add     w8, w13, w9                 ;save address of top half of xbuf
    mov     w7, w10                     ;save address of ybuf
    add     w10, w13, w11               ;save address of top half of ybuf
    mov        w2, w12                        ; save address of last byte of e
;    dec     w3, w12                     ;save address of last byte of e
    push    w4                          ;save address of N on stack
    push    w5                          ;save number of words in N on stack
    push    _ACCAL                      ;save v on stack

; check if a is in top half of xbuf

    cp      w1, w9                      ;address of a == halfway through xbuf?
    bra     z, mont_mod_exp_a_to_ybuf   ;if so, jump to copy a to top half
                                        ; of ybuf (since it can't be there)

; copy a to top half of xbuf

    mov     w1, w4                      ;pointer to a
    mov     w9, w13                     ;pointer to top half of xbuf
    dec     w5, w14                     ;number of words - 1 in a
    repeat  w14                         ;for each word of a
     mov    [w4++], [w13++]             ; copy word of a to top half of xbuf

; check if a is in top half of ybuf

    cp      w1, w11                     ;address of a == halfway through ybuf?
    bra     z, mont_mod_exp_process_exp ;if so, jump to process exponent

; copy a to top half of ybuf

mont_mod_exp_a_to_ybuf:
    mov     w11, w13                    ;pointer to top half of ybuf
    dec     w5, w14                     ;number of words - 1 in a
    repeat  w14                         ;for each word of a
     mov    [w1++], [w13++]             ; copy word of a to top half of ybuf

; set bit position for processing in exponent byte

mont_mod_exp_process_exp:
    mov        w3, w13                        ; set pointer to current e byte

;    dec     w2, w13                     ;set pointer to current e byte
    subr    w6, #15, w14                ;next bit position to examine in e byte
    bra     n, mont_mod_exp_next_ebyte  ;if processed all bits, jump to next
                                        ; e byte

mont_mod_exp_process_ebyte_loop:

.ifdef NO_DO_ONCE
    mov     #0, w0                      ;constant zero
    cpseq   w14, w0                     ;skip do instruction if do count = 0
.endif

    do      w14, mont_mod_exp_bit_loop_end
                                        ;for each remaining bit in e byte
    
    ; Montgomery squaring

     mov    w8, w0                      ;address of xbuf
     mov    w9, w1                      ;address of partial (top half of xbuf)
     mov    w10, w2                     ;address of temp b (bottom half of ybuf)
     mov    [w15-6], w3                 ;address of N
     mov    [w15-4], w4                 ;number of words in N
     mov    [w15-2], w5                 ;v
     rcall  mont_square                 ;square and reduce

    ; check for multiplication

     mov.b  [w13], w0                   ;get current e byte
     btst   w0, w14                     ;is current bit in current e byte == 1?
     bra    z, mont_mod_exp_mult_done   ;if not, skip multiplication

    ; Montgomery multiplication

     mov    w8, w0                      ;address of xbuf
     mov    w9, w1                      ;address of partial (top half of xbuf)
     mov    w11, w2                     ;address of encoded a (top half of ybuf)
     mov    [w15-6], w3                 ;address of N
     mov    [w15-4], w4                 ;number of words in N
     mov    [w15-2], w5                 ;v
     rcall  mont_multiply               ;multiply and reduce
mont_mod_exp_mult_done:
     nop                                ;can't have rcall as penultimate
mont_mod_exp_bit_loop_end:
     dec    w14, w14                    ;bit position--
                                        ; instruction in a do loop

; check for next e byte

mont_mod_exp_next_ebyte:
    cp        w13, w12                    ; address of lsbyte of e?
    bra        z, mont_mod_exp_done        ; if so, jump to done
    dec        w13, w13                    ; pointer to next e byte
    mov        #7, w14                        ; reset next bit position
    bra        mont_mod_exp_process_ebyte_loop
                                        ; loop

; restore registers and return

mont_mod_exp_done:
    sub     #3*2, w15                   ;adjust stack pointer
    pop     w14                         ;restore w14 from stack
    pop.d   w12                         ;restore w12, w13 from stack
    pop.d   w10                         ;restore w10, w11 from stack
    pop.d   w8                          ;restore w8, w9 from stack
    return                              ;return from mont_mod_exp()


;****************************************************************************
; ROUTINE       mont_square
;
; DESCRIPTION   This routine squares a vector (multi-word integer) in
;               Montgomery's form by squaring the integer and performing
;               Montgomery reduction.  c = a * a * R^-1 mod N
;
; PARAMETERS
;               w0 - pointer to xbuf, a working buffer returning the result
;                     in the top half (x-data, length 2m words)
;               w1 - pointer to a (x-data, length n words)
;               w2 - pointer to temp b buffer (y-data, length n words)
;               w3 - pointer to N (y-data, length n words)
;               w4 - n, number of words in N
;               w5 - v=-N^{-1} mod 2^16
;
;               IF (CORCON< 0>) set
;               US (CORCON<12>) set
;
; RETURN VALUES
;
; REGISTERS USED    
;               w0  - pointer to xbuf
;               w1  - pointer to a, pointer to N
;               w2  - pointer to b, number of words in N
;               w3  - pointer to N, v
;               w4  - number of words in N
;               w5  - v
;               w6  - used by subroutine
;               w7  - used by subroutine
;               w8  - unused
;               w9  - unused
;               w10 - unused
;               w11 - unused
;               w12 - unused
;               w13 - unused
;               w14 - unused
;               w15 - stack pointer
;               ACCA - used by subroutine
;               ACCB - used by subroutine
;
; SUBROUTINES CALLED
;               math_vect_squ()
;               mont_reduce()
;
; STACK USED
;               9 words
;
; TIMING
;               ? cycles
;
; AUTHOR        M. H. Etzel, Security Innovation, Inc.
;
; DATE          11/03/2003
;
; NOTES
;               - This routine assumes the least significant word of
;                 c, a, N, is in the lowest memory address.
;
;               - The vector, a, may be located in the top half of xbuf.
;                 If it is, it will be overwritten.
;
;               - A called subroutine uses 1 level of DO loops.
;
; CHANGES

mont_square:

; save parameters

    push    w0                          ;save pointer to xbuf on stack
    push    w3                          ;save pointer to N on stack
    push.d  w4                          ;save number of words in N, v on stack

; square a

    mov     w4, w3                      ;number of words in N
    rcall   math_vect_squ               ;square a, result returned in top
                                        ; half of xbuf

; restore parameters and Montgomery-reduce result

    pop.d   w2                          ;restore number of words in N, v from
                                        ; stack
    pop.d   w0                          ;restore pointer to xbuf, pointer to N
                                        ; from stack
    rcall   mont_reduce                 ;Montgomery reduction, result
                                        ; returned in top half of xbuf

; return

    return                              ;return from mont_square()


;****************************************************************************
; ROUTINE       mont_multiply
;
; DESCRIPTION   This routine multiplies two vectors (multi-word integers) in
;               Montgomery's form by multiplying the integers and performing
;               Montgomery reduction.  c = a * b * R^-1 mod N
;
; PARAMETERS
;               w0 - pointer to xbuf, a working buffer returning the result
;                     in the top half (x-data, length 2m words)
;               w1 - pointer to a (x-data, length n words)
;               w2 - pointer to b (y-data, length n words)
;               w3 - pointer to N (y-data, length n words)
;               w4 - n, number of words in N
;               w5 - v=-N^{-1} mod 2^16
;
;               IF (CORCON< 0>) set
;               US (CORCON<12>) set
;
; RETURN VALUES
;
; REGISTERS USED    
;               w0  - pointer to xbuf
;               w1  - pointer to a, pointer to N
;               w2  - pointer to b, number of words in N
;               w3  - pointer to N, v
;               w4  - number of words in N
;               w5  - v
;               w6  - used by subroutine
;               w7  - used by subroutine
;               w8  - unused
;               w9  - unused
;               w10 - unused
;               w11 - unused
;               w12 - unused
;               w13 - unused
;               w14 - unused
;               w15 - stack pointer
;               ACCA - used by subroutine
;
; SUBROUTINES CALLED
;               math_vect_mul()
;               mont_reduce()
;
; STACK USED
;               8 words
;
; TIMING
;               ? cycles
;
; AUTHOR        M. H. Etzel, Security Innovation, Inc.
;
; DATE          11/03/2003
;
; NOTES
;               - This routine assumes the least significant word of
;                 c, a, b, N, is in the lowest memory address.
;
;               - The vector, a, may be located in the top half of xbuf.
;                 If it is, it will be overwritten.
;
;               - A called subroutine uses 1 level of DO loops.
;
; CHANGES

mont_multiply:

; save parameters

    push    w0                          ;save pointer to xbuf on stack
    push    w3                          ;save pointer to N on stack
    push.d  w4                          ;save number of words in N, v on stack

; multiply a and b

    mov     w4, w3                      ;number of words in N
    rcall   math_vect_mul               ;multiply a and b, result returned
                                        ; in top half of xbuf

; restore parameters and Montgomery-reduce result

    pop.d   w2                          ;restore number of words in N, v from
                                        ; stack
    pop.d   w0                          ;restore pointer to xbuf, pointer to N
                                        ; from stack
    rcall   mont_reduce                 ;Montgomery reduction, result
                                        ; returned in top half of xbuf

; return

    return                              ;return from mont_multiply()


;****************************************************************************
; ROUTINE       mont_decode
;
; DESCRIPTION   This routine decodes a vector, a, out of Montgomery's form:
;               c = a * R^-1.
;
; PARAMETERS
;               w0 - pointer to xbuf, a working buffer returning the result
;                     in the top half (x-data, length 2n words)
;               w1 - pointer to a (length n words) 
;               w2 - pointer to N (y-data, length n words)
;               w3 - n, number of words in N, a, c
;               w4 - v=-N^{-1} mod 2^16
;
;               IF (CORCON< 0>) set
;               US (CORCON<12>) set
;
; RETURN VALUES
;
; REGISTERS USED    
;               w0  - address of xbuf
;               w1  - pointer to a
;               w2  - pointer to N
;               w3  - number of words in N
;               w4  - v
;               w5  - pointer to xbuf
;               w6  - number of words - 1 in N
;               w7  - unused
;               w8  - unused
;               w9  - unused
;               w10 - unused
;               w11 - unused
;               w12 - unused
;               w13 - unused
;               w14 - unused
;               w15 - stack pointer
;
; SUBROUTINES CALLED
;               mont_reduce()
;
; STACK USED
;               8 words
;
; TIMING
;               ? cycles
;
; AUTHOR        M. H. Etzel, Security Innovation, Inc.
;
; DATE          11/03/2003
;
; NOTES
;               - This routine assumes the least significant word of
;                 c, a, N, is in the lowest memory address.
;
;               - A called subroutine uses 1 level of DO loops.
;
;
; CHANGES

mont_decode:

; copy vector to be decoded to bottom of xbuf and zero top of xbuf

    mov     w0, w5                      ;pointer to xbuf
    dec     w3, w6                      ;number of words - 1 in a
    repeat  w6                          ;for each word in a
     mov    [w1++], [w5++]              ;mov word from a to bottom half of xbuf
    repeat  w6                          ;for each word in top half of xbuf
     clr    [w5++]                      ;zero word in top half of xbuf

; decode

    mov     w2, w1                      ;pointer to N
    mov     w3, w2                      ;number of words in N
    mov     w4, w3                      ;v
    rcall   mont_reduce                 ;Montgomery reduction, result
                                        ; returned in top half of xbuf

; return

    return                              ;return from mont_decode()


;****************************************************************************
; ROUTINE       mont_reduce
;
; DESCRIPTION   Using Montgomery reduction, this routine reduces mod p an
;               integer t < N^2, where N is n words long, to r, defined
;               to be an integer 0 <= r < N such that r = u * t mod N,
;               where u is the multiplicative inverse of 2^{16m} mod N. 
;               In practice this means a multiple of N is added to t
;               to ensure that all the bottom n words are zero.  The
;               result is then returned in the top n words of t.
;
; PARAMETERS
;               w0  - t, a pointer to the t buffer (x-data, length 2n words)
;               w1  - N, a pointer to the modulus N (y-data, length n words)
;               w2  - n, the number of 16-bit words of N
;               w3  - v = -N^{-1} mod 2^16
;
;               IF (CORCON< 0>) set
;               US (CORCON<12>) set
;
; RETURN VALUES
;
; REGISTERS USED
;               w0  - pointer to t (m), temp
;               w1  - pointer to starting point in N
;               w2  - n, n-2, n-1
;               w3  - the integer v=-p^{-1} mod 2^16
;               w4  - N[0], pointer to starting point in m
;               w5  - constant 1 for mac
;               w6  - temp, m[i]
;               w7  - temp, N[j]
;               w8  - pointer to current t (next m)
;               w9  - xdata mac prefetch
;               w10 - ydata mac prefetch
;               w11 - term counter for each column
;               w12 - pointer to N
;               w13 - pointer to _ACCAL
;               w14 - unused
;               w15 - unused
;               ACCA - partial results
;
; SUBROUTINES CALLED
;
; STACK USED
;               6 words
;
; TIMING
;               ? cycles
;
; AUTHOR        N. A. Howgrave-Graham, NTRU Cryptosystems, Inc.
;
; DATE          09/09/2003
;
; NOTES
;               - This routine assumes the least significant word of
;                 the result, t, N, is in the lowest memory address.
;
;               - This routine uses 1 level of DO loops.
;
; CHANGES
;               - 09/12/11 mhe
;                   - added conditional skip of do instruction only if
;                     one pass for errata in 33E

mont_reduce:

; save registers

    push.d  w8                          ;save w8, w9 on stack
    push.d  w10                         ;save w10, w11 on stack
    push.d  w12                         ;save w12, w13 on stack

; set pointers and variables

    mov     w1, w12                     ;save pointer to N
    mov     #_ACCAL, w13                ;pointer to ACCAL 
    mov     #1, w5                      ;constant 1 for mac
    mov     w0, w8                      ;init pointer for current t word
                                        ; (and next m word)

; do first column separately to avoid mac prefetching out of bounds

    clr     A                           ;clear ACCA
    mov     [w0], [w13]                 ;load t[0] into ACCAL
    mul.uu  w3, [w0], w6                ;m[0] = v * t[0] mod 2^16 
                                        ;(w7 is also affected, but unused)
    mov     w6, [w8++]                  ;store m[0]
    mov     [w1], w4                    ;N[0] for adding last term in column
    mac     w4*w6, A                    ;add m[0] * N[0]
    sftac   A, #16                      ;shift out zeros and keep the carry

; init for loop through the nth column: 1st half of t

    clr     w11                         ;init number of terms - 2 in column
    sub     w2, #2, w2                  ;number of columns left - 1

.ifdef NO_DO_ONCE
    mov     #0, w9                      ;constant zero
    cpseq   w2, w9                      ;skip do instruction if do count = 0
.endif

    do      w2, mont_reduce_loop1_end   ;for each column in m * N                  
     mov    w0, w9                      ;set pointer to m[0]
     inc2   w1, w1                      ;increment starting point in N
     mov    w1, w10                     ;set pointer to N[j]
     mov    [w8], w6                    ;get t[j] 
     mac    w5*w6, A, [w9]+=2, w6, [w10]-=2, w7 
                                        ;add t[j] to the accumulator and
                                        ; prefetch m[0], N[j] for mac
     repeat w11                         ;for each term in the column
      mac   w6*w7, A, [w9]+=2, w6, [w10]-=2, w7 
                                        ;m[i] * N[j], prefetch m[++i], N[--j]
     mul.uu w3, [w13], w6               ;m[j] = v * t[j] mod 2^16
     mov    w6, [w8++]                  ;store m[j] 
     mac    w4*w6, A                    ;add m[j] * N[0]
     sftac  A, #16                      ;shift out zeros and keep the carry
mont_reduce_loop1_end:
     inc    w11, w11                    ;(number of terms - 2 in column)++

; init for loop through the last n-1 columns: 2nd half of t

    mov     w0, w4                      ;pointer to m[0]

.ifdef NO_DO_ONCE
    mov     #0, w9                      ;constant zero
    cpseq   w2, w9                      ;skip do instruction if do count = 0
.endif

    do      w2, mont_reduce_loop2_end   ;for each column in m * N
     dec    w11, w11                    ;(number of terms - 1 in column)--
     inc2   w4, w4                      ;increment starting point in m
     mov    w4, w9                      ;set pointer to m[i]
     mov    w1, w10                     ;set pointer to N[n-1]
     mov    [w8], w6                    ;get t[n+i-1] 
     mac    w5*w6, A, [w9]+=2, w6, [w10]-=2, w7 
                                        ;add t[n+i-1] to the accumulator and 
                                        ;prefetch m[i], N[j] for mac
     repeat w11                         ;for each term in the column
      mac   w6*w7, A, [w9]+=2, w6, [w10]-=2, w7 
                                        ;m[i] * N[j], prefetch m[++i], N[--j]
     mov    [w13], [w8++]               ;store new t[n+i-1]
mont_reduce_loop2_end:
     sftac  A, #16                      ;shift the accumulator to keep carry 

; do last column separately

    mov     [w8], w6                    ;get t[2n-1]
    mac     w5*w6, A                    ;add t[2n-1] to the accumulator
    mov     [w13], [w8]                 ;store new t[2n-1] 
    inc     w2, w2                      ;no. words - 1 in N

; check if t > 2^R, where R = 2^16n

    cp0     _ACCAH                      ;ACCAH == 0?
    bra     nz, mont_reduce_sub_N       ;if not, jump to subtract N

; check if t < p, by examining the words from the most sig 

    mov     w2, w6                      ;init word counter
mont_reduce_cp_t_N_loop:
    mov     [w8--], w0                  ;get t[i]
    cp      w0, [w1--]                  ;compare t[i] to N[i]
    bra     ltu, mont_reduce_done       ;if t < N, jump to done
    bra     gtu, mont_reduce_sub_N      ;if t > N, jump to subtract N
    dec     w6, w6                      ;word counter--
    bra     nz, mont_reduce_cp_t_N_loop ;if more words, loop

mont_reduce_sub_N:
    bset    _SR, #C                     ;set no borrow

.ifdef NO_DO_ONCE
    mov     #0, w0                      ;constant zero
    cpseq   w2, w0                      ;skip do instruction if do count = 0
.endif

    do      w2, mont_reduce_sub_N_loop_end
                                        ;for each word in N
     mov    [++w4], w0                  ;t[i]
mont_reduce_sub_N_loop_end:
     subb   w0, [w12++], [w4]           ;t[i] -= N[i]

mont_reduce_done:
    pop.d   w12                         ;restore w12, w13 from stack
    pop.d   w10                         ;restore w10, w11 from stack
    pop.d   w8                          ;restore w8, w9 from stack
    return                              ;return from mont_reduce()


;****************************************************************************
; ROUTINE       mont_calc_v
;
; DESCRIPTION   This routine calculates an inverse needed for Montgomery
;               operations, viz. the negative of the inverse of a modulus
;               mod 2^16.
;
; PARAMETERS
;               w0  - pointer to the modulus (least significant 16-bit word)
;
; RETURN VALUES
;               w0  - -N^-1 mod 2^16
;
; REGISTERS USED
;               w0  - -N^-1 mod 2^16
;               w1  - working register
;               w2  - (N[0] + 1) >> 1
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
;               w15 - unused
;
; SUBROUTINES CALLED
;
; STACK USED
;               0 words
;
; TIMING
;               ? cycles
;
; AUTHOR        M. H. Etzel, Security Innovation, Inc.
;
; DATE          11/18/2003
;
; NOTES
;               - This routine uses 1 level of DO loops.
;
; CHANGES

mont_calc_v:


    mov     #1, w1                      ;set working reg with first inverse bit
    add     w1, [w0], w2                ;N[0] + 1
    lsr     w2, #1, w2                  ;(N[0] + 1) >> 1

    do      #14, mont_calc_v_loop_end   ;for each of the first 15 inverse bits
     lsr    w1, w1                      ;put inverse bit in carry
     rrc    w0, w0                      ;get inverse bit from carry
     btsc   w0, #15                     ;don't add N if inverse bit != 1
      add   w1, w2, w1                  ;add modulus to working register
mont_calc_v_loop_end:
     nop

    lsr    w1, w1                       ;put last inverse bit in carry
    rrc    w0, w0                       ;get last inverse bit from carry
    return                              ;return from mont_calc_v()


.end
