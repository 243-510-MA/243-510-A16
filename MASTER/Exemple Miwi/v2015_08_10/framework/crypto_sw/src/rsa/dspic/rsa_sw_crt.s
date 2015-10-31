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
; FILE          rsa_crt.s
;
;
; DESCRIPTION   This file contains routines to support the Chinese Remainder
;               Theory operations used in RSA decryption and signing.
;
;
; ROUTINES
;               rsa_crt_exp()  -  reduction and modular exponentiation
;               rsa_crt_lift() -  Chinese remainder theorem lifting
;
; CHANGES
;               - 09/12/11 mhe
;                   - changed rsa_crt_lift()
;

; include files

.include "crypto_sw/src/rsa/dspic/rsa_sw_proc.inc"                     ;common device definitions
.include "crypto_sw/src/rsa/dspic/rsa_sw_ret_codes.inc"                ;return codes

; function declarations

.global rsa_crt_exp
.global rsa_crt_lift

; external function declarations

.extern math_mod_exp
.extern math_mod_red
.extern math_vect_squ
.extern mont_calc_v
.extern mont_multiply
.extern mont_reduce

; code section

.text


;****************************************************************************
; ROUTINE       rsa_crt_exp
;
; DESCRIPTION
;               This routine computes a modular reduction and then a modular
;               exponentiation, ((a mod p)^x) mod p, where a is twice the
;               length of the modulus, p, and the exponent, x, is the same
;               length as p.  This supports the Chinese Remainder Theorem;
;               therefore, this functionality is common to both RSA
;               decryption and RSA signing.
;
; PARAMETERS
;               w0  - pointer to the base, a (network byte order)
;                     this can be in ybuf2 in which case it will be overwritten
;               w1  - pointer to the modulus, p (least significant byte first)
;               w2  - pointer to the exponent, x (network byte order)
;               w8  - k, number of bytes in the modulus, exponent
;               w10 - pointer to the xdata working buffer (length 2k bytes)
;                     thought of as (xbuf1 || xbuf2)
;               w11 - pointer to ydata working buffer (length 3k bytes)
;                     thought of as (ybuf1 || ybuf2 || ybuf3)
;
;               IF (CORCON< 0>) set
;               US (CORCON<12>) set
;
; RETURN VALUES
;
; REGISTERS
;               w0  - temp
;               w1  - temp
;               w2  - temp
;               w3  - temp
;               w4  - temp
;               w5  - temp
;               w6  - temp
;               w7  - temp
;               w8  - number of bytes in modulus
;               w9  - unused
;               w10 - address of xbuf
;               w11 - address of ybuf
;               w12 - unused
;               w13 - unused
;               w14 - unused
;               w15 - stack pointer
;
; SUBROUTINES CALLED
;               math_mod_exp()
;               math_mod_red()
;               math_vect_squ()
;               mont_calc_v()
;               mont_multiply()
;               mont_reduce()
;
; STACK USED
;               32 words
;
; TIMING
;                1024-bit modulus:  2252929 -  2281293 cycles typical
;                2048-bit modulus: 14176912 - 14515572 cycles typical
;
; AUTHOR        N. A. Howgrave-Graham, NTRU Cryptosystems, Inc.
;
; DATE          11/20/2003
;
; NOTES
;               - The result of the modular exponentiation is returned in
;                 xbuf2 (with the least significant word first).
;
;               - Internal functions may use the fact that this function
;                 also returns a ydata copy of p in ybuf1.
;
; CHANGES

rsa_crt_exp:                    

; save parameters

    push    w0                          ;save address of a on stack
    push    w2                          ;save address of x on stack
    push    w1                          ;save address of p on stack

; form RR = (2^8k)^2 mod p and store in ybuf2

    ; form R mod p = R - p = -p mod 2^8k and store in xbuf2

    clr     w4                          ;constant zero
    add     w10, w8, w0                 ;address of xbuf2
    asr     w8, w3                      ;number of words in p
    dec     w3, w3                      ;number of words - 1 in p
    bset    _SR, #C                     ;set no borrow
    repeat  w3                          ;for each word of p
     subb   w4, [w1++], [w0++]          ;negate word of p to xbuf2
    
    ; form R * R and store in xbuf1 || xbuf2

    mov     w10, w0                     ;address of xbuf1
    add     w10, w8, w1                 ;address of R mod p (xbuf2)
    mov     w11, w2                     ;address of ybuf1
    asr     w8, w3                      ;number of words in R mod p
    rcall   math_vect_squ               ;square (R mod p)

    ; copy p to ybuf1

    pop     w0                          ;pointer to p
    mov     w11, w1                     ;pointer to ybuf1
    asr     w8, w2                      ;number of words in p
    dec     w2, w2                      ;number of words - 1 in p
    repeat  w2                          ;for each word of p
     mov    [w0++], [w1++]              ;copy word of p to ybuf1

    ; reduce R * R mod p, and store in ybuf2, unless a is in ybuf2, in which
    ;  case store in xbuf1

    mov     [w15-4], w1                 ;address of a
    add     w11, w8, w0                 ;address for result (ybuf2)
    cpsne   w1, w0                      ;if address of a != address of ybuf2,
                                        ; skip next instruction
     mov    w10, w0                     ;address for result (xbuf1)
    mov     w10, w1                     ;address of R * R (xbuf1)
    mov     w8, w2                      ;number of words in R * R
    mov     w11, w3                     ;address of p (ybuf1)
    asr     w8, w4                      ;number of words in p
    rcall   math_mod_red                ;R * R mod p

; put RR in ybuf2 (if not already) and reverse a into xbuf1 || xbuf2

    ; check if RR or a is in ybuf2

    mov     [w15-4], w1                 ;address of a
    add     w11, w8, w0                 ;address of ybuf2
    cp      w1, w0                      ;address of a = address of ybuf2?
    bra     nz, rsa_crt_exp_rev_a       ;if not, RR is in ybuf2, skip to
                                        ; reverse a into xbuf1 || xbuf2

    ; a is in ybuf2:
    ; copy first half of a in reverse byte order into xbuf2

    add     w10, w8, w1                 ;address of xbuf2
    add     w1, w8, w1                  ;address of byte following xbuf2
    dec     w8, w2                      ;number of bytes - 1 in first half of a
    repeat  w2                          ;for each byte in first half of a
     mov.b  [w0++], [--w1]              ;copy byte of a into xbuf2

    ; copy RR from xbuf1 to ybuf2

    mov     w10, w3                     ;address of RR (xbuf1)
    add     w11, w8, w4                 ;address of ybuf2
    asr     w8, w2                      ;number of words in RR
    dec     w2, w2                      ;number of words - 1 in RR
    repeat  w2                          ;for each word of RR
     mov    [w3++], [w4++]              ;copy word of RR into ybuf2
    
    ; copy second half of a in reverse byte order into xbuf1

    dec     w8, w2                      ;number of bytes - 1 in second half of a
    repeat  w2                          ;for each byte in second half of a
     mov.b  [w0++], [--w1]              ;copy byte of a into xbuf1
    bra     rsa_crt_exp_calc_v

rsa_crt_exp_rev_a:                       
    
    ; a is not in ybuf2
    ; copy a in reverse order into xbuf1 || xbuf2

    add     w8, w8, w2                  ;number of bytes in a
    add     w10, w2, w0                 ;address of byte following xbuf2
    dec     w2, w2                      ;number of bytes - 1 in a
    repeat  w2                          ;for each byte in a
     mov.b  [w1++], [--w0]              ;copy byte of a into xbuf1 || xbuf2
    
; form v and save in reg

rsa_crt_exp_calc_v:                            
    mov     w11, w0                     ;pointer to lsword of p
    rcall   mont_calc_v                 ;calculate v = -p^{-1} mod 2^16
    mov     w0, [w15-4]                 ;save v in place of address of a

; reduce a mod p using Montgomery and store in xbuf2

    ; form a * R^{-1} mod p and store the result in xbuf2

    mov     w10, w0                     ;address of a (xbuf1)
    mov     w11, w1                     ;address of p (ybuf1)
    asr     w8, w2                      ;number of words in p
    mov     [w15-4], w3                 ;v
    rcall   mont_reduce                 ;form a * R^-1 mod p

    ; form a mod p and store in xbuf2

    mov     w10, w0                     ;address of xbuf1
    add     w10, w8, w1                 ;address of a * R^-1 mod p (xbuf2)
    add     w11, w8, w2                 ;address of RR (ybuf2)
    mov     w11, w3                     ;address of p (ybuf1)
    asr     w8, w4                      ;number of words in p
    mov     [w15-4], w5                 ;v
    rcall   mont_multiply               ;form a mod p

; form a^x mod p and store in xbuf2

    mov     w10, w0                     ;address of xbuf
    add     w10, w8, w1                 ;address of a mod p (xbuf2)
    pop     w2                          ;address of x
    mov     w8, w3                      ;number of bytes in x
    mov     w11, w4                     ;address of p (ybuf1)
    asr     w8, w5                      ;number of words in p
    pop     w6                          ;v
    add     w11, w8, w7                 ;address of ybuf2
    push    w7                          ;push address of RR (ybuf2) on stack
    rcall   math_mod_exp                ;form a^x mod p
    pop     w0                          ;restore address of RR from stack

; return

    return                              ;return from rsa_crt_exp()


;****************************************************************************
; ROUTINE       rsa_crt_lift
;
; DESCRIPTION   This routine performs Chinese remainder theorem lifting,
;               a mod pq = q * [(a mod p - a mod q) * q^-1 mod p] + a mod q
;
; PARAMETERS
;               w8  - k, number of bytes in p, q, qinv
;               w10 - address of xbuf (xdata, length 2k bytes)
;                     thought of as xbuf1 || xbuf2
;               w11 - address of ybuf (ydata, length 3k bytes)
;                     thought of as ybuf1 || ybuf2 || ybuf3
;               w12 - address of secret key, holding p || q || dp || dq || qinv
;
;               ap  - a mod p, is in xbuf2
;               aq  - a mod q, is in xbuf3
;               p   - in ybuf1
;
;               IF (CORCON< 0>) set
;               US (CORCON<12>) set
;
; RETURN VALUES
;
; REGISTERS
;               w0  - temp
;               w1  - temp
;               w2  - temp
;               w3  - temp
;               w4  - temp
;               w5  - unused
;               w6  - unused
;               w7  - 2 * k
;               w8  - number of bytes in p, q, qinv
;               w9  - number of words - 1 in p, q, qinv
;               w10 - address of xbuf
;               w11 - address of ybuf
;               w12 - address of sk
;               w13 - unused
;               w14 - unused
;               w15 - stack pointer
;
; SUBROUTINES CALLED
;               math_vect_mul()
;               math_mod_red()
;
; STACK USED
;               8 words
;
; TIMING
;                1024-bit modulus:  8929 -  9013 cycles typical
;                2048-bit modulue: 30245 - 30279 cycles typical
;
; AUTHOR        N. A. Howgrave-Graham, NTRU Cryptosystems, Inc.
;
; DATE          11/20/2003
;
; NOTES
;               - THIS ROUTINE CORRUPTS w9.
;
;               - The result of the lifting is returned in xbuf1 || xbuf2
;                 (with the least significant word first)
;
;               - It is required that p > q.
;
;               - This routine uses 1 level of DO loops.
;
; CHANGES

rsa_crt_lift:

; set up parameters

    asr     w8, w9                      ;number of words in p, q, qinv
    dec     w9, w9                      ;number of words - 1 in p, q, qinv
    add     w8, w8, w7                  ;2k bytes

; copy qinv to ybuf3

;    add     w12, w7, w0                 ;address of dp (sk + 2k bytes)
;    add     w0, w7, w0                  ;address of qinv (sk + 4k bytes)
    mov     [w12+10],w0                    ;address of qInv
    add     w11, w7, w1                 ;address of ybuf3
    repeat  w9                          ;for each word in qinv
     mov    [w0++], [w1++]              ;copy word of qinv to ybuf3

; form h = (ap - aq) * qinv mod p and store in ybuf2

    ; d = ap - aq (staying in xbuf2)

    add     w10, w8, w0                 ;address of ap (xbuf2)
    add     w0, w8, w1                  ;address of aq (xbuf3)
    bset    _SR, #C                     ;set no borrow
    do      w9, rsa_crt_lift_sub_aq_loop_end
                                        ;for each word of ap
     mov    [w1++], w2                  ;get word of aq
rsa_crt_lift_sub_aq_loop_end:                       
     subbr  w2, [w0], [w0++]            ;word of ap -= word of aq

    ; if d < 0, d += p

    bra     c, rsa_crt_lift_mul_qinv    ;if no borrow, skip adding p

    add     w10, w8, w0                 ;address of d (xbuf2)
    mov     w11, w1                     ;address of p (ybuf1)
    bclr    _SR, #C                     ;clear carry
    do      w9, rsa_crt_lift_add_p_loop_end
                                        ;for each word in p
     mov    [w1++], w2                  ;get word of p
rsa_crt_lift_add_p_loop_end:                       
     addc   w2, [w0], [w0++]            ;word of d += word of p

rsa_crt_lift_mul_qinv:                        

;  h = d * qinv and store result in xbuf1 || xbuf2

    mov     w10, w0                     ;address of xbuf1
    add     w10, w8, w1                 ;address of d (xbuf2)
    add     w11, w7, w2                 ;address of qinv (ybuf3)   
    asr     w8, w3                      ;number of words in d
    rcall   math_vect_mul               ;h = d * qinv

;  reduce h mod p and store in ybuf2 (no Montgomery since no RR_p)

    add     w11, w8, w0                 ;address for result (ybuf2)
    mov     w10, w1                     ;address of h (xbuf1)
    mov     w8, w2                      ;number of words in h
    mov     w11, w3                     ;address of p (ybuf1)
    asr     w8, w4                      ;number of words in p
    rcall   math_mod_red                ;h mod p

; copy q to xbuf2

;    add     w12, w8, w0                 ;address of q (sk + k bytes)
    mov        [w12+4],w0                    ;address of q
    add     w10, w8, w1                 ;address of xbuf2
    repeat  w9                          ;for each word in q
     mov    [w0++], [w1++]              ;copy word of q to xbuf2

; form an = (q * h) + aq and store in xbuf1 || xbuf2

    ; r = q * h

    mov     w10, w0                     ;address for result (xbuf1)
    add     w10, w8, w1                 ;address of q (xbuf2)
    add     w11, w8, w2                 ;address of h (ybuf2)
    asr     w8, w3                      ;number of words in q, h
    call    math_vect_mul               ;q * h

    ; an = r + aq

    mov     w10, w0                     ;address of r (xbuf1)
    add     w10, w8, w1                 ;address of xbuf2
    add     w1, w8, w1                  ;address of aq (xbuf3)
    bclr    _SR, #C                     ;clear carry
    do      w9, rsa_crt_lift_add_aq_loop_end
                                        ;for each word of aq
     mov    [w1++], w2                  ;get word of aq
rsa_crt_lift_add_aq_loop_end:                      
     addc   w2, [w0], [w0++]            ;word of r += word of aq

    bra     nc, rsa_crt_lift_done       ;if no carry, skip to end

rsa_crt_lift_add_aq_carry_loop:
    inc     [w0], [w0++]                ;add carry to next word
    bra     c, rsa_crt_lift_add_aq_carry_loop
                                        ;if carry, loop

; return

rsa_crt_lift_done:
    return                              ;return from rsa_crt_lift()


.end
