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
; FILE          rsa_sw_enc_dec.s
;
;
; DESCRIPTION   This file contains routines to implement RSA encryption and
;               decryption on Microchip dsPICs.
;
;
; ROUTINES
;               _rsa_encrypt() -  RSA encryption
;               _rsa_decrypt() -  RSA decryption
;
; CHANGES
;               - 05/14/2004 mhe
;                   - changed _rsa_encrypt(), _rsa_decrypt()
;
;               - 09/05/2011 mhe
;                   - changed _rsa_encrypt(), _rsa_decrypt()
;
;               - 09/12/2011 mhe
;                   - changed _rsa_encrypt()
;

; include files

.include "crypto_sw/src/rsa/dspic/rsa_sw_proc.inc"                     ;common device definitions
.include "crypto_sw/src/rsa/dspic/rsa_sw_ret_codes.inc"                ;return codes

; function declarations

.global _rsa_encrypt
.global _rsa_decrypt

.ifdef __dsPIC33F
.equiv LAST_FRAME_OFFSET,24
.endif

.ifdef __dsPIC33E
.equiv LAST_FRAME_OFFSET,26
.endif


; external function declarations

.extern math_mod_exp
.extern math_mod_red
.extern math_vect_squ
.extern mont_calc_v
.extern rsa_crt_exp
.extern rsa_crt_lift

; code section

.text


;****************************************************************************
; ROUTINE       _rsa_encrypt
;
; DESCRIPTION
;               This routine performs RSA encryption of a message.
;               If the RSA public modulus is k bytes, then the message
;               can be at most k-11 bytes.
;
; PARAMETERS
;               w0  - address for the ciphertext, c; must be k bytes
;                     (may not overlap with the bottom k bytes of xbuf)
;               w1  - pointer to the message buffer m
;               w2  - message byte length, at most k-11 bytes
;               w3  - pointer to n, the RSA modulus
;               w4  - RSA modulus byte length (k), must be 128 or 256
;               w5  - pointer to the RSA public exponent e
;               w6  - RSA public exponent byte length; at most k bytes
;               w7  - pointer to xdata working buffer, length 2*k aligned(64)
;                     bytes, thought of as (xbuf1 || xbuf2)
;               [w15-6] - pointer to ydata working buffer, 3*k aligned(2) bytes
;                         thought of as (ybuf1 || ybuf2 || ybuf3)
;
; RETURN VALUES
;               w0 - return code
;                       MCL_SUCCESS
;                       MCL_ILLEGAL_PARAMETER
;                       MCL_ILLEGAL_SIZE
;               w1 - 0
;               w2 - 0
;               w3 - 0
;               w4 - 0
;               w5 - 0
;               w6 - 0
;               w7 - 0
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
;               w8  - address of m, pointer to PKCS-encoded m
;               w9  - number of bytes in m, number of random bytes - 1
;               w10 - address of e
;               w11 - number of bytes in e
;               w12 - address of xbuf
;               w13 - address of ybuf
;               w14 - k, number of bytes in n
;               w15 - stack pointer
;
; SUBROUTINES CALLED
;               _drbg_generate()
;               math_mod_exp()
;               math_mod_red()
;               math_vect_squ()
;               mont_calc_v()
;
; STACK USED
;               40 words
;
; TIMING
;               1024-bit modulus, e = 2^16 + 1: (longest to shortest
;                                                plaintext message)
;                   30F: 194513 - 219031 cycles
;                   33F: 197046 - 222354 cycles
;                   33E: 198715 - 225974 cycles
;
;               2048-bit modulus, e = 2^16 + 1: (longest to shortest
;                                                plaintext message)
;                   30F: 656578 - 715512 cycles
;                   33F: 661543 - 722373 cycles
;                   33E: 663912 - 729380 cycles
;
; AUTHOR        N. A. Howgrave-Graham, NTRU Cryptosystems, Inc.
;
; DATE          11/20/2003
;
; NOTES
;               - All buffers (e.g. ciphertext, message, RSA modulus, RSA
;                 exponent) are given in network byte order.
;
;               - It is assumed that the pseudorandom number generator has
;                 been seeded satisfactorily prior to invoking this routine.
;
; CHANGES
;               - 05/14/2004 mhe
;                   - removed clearing buffers in case output buffer overlapped
;
;               - 09/05/2011 mhe
;                   - set two CORCON bits for unsigned mode
;                   - saved, set, and restored DSRPAG to ensure access to
;                     ydata for devices with EDS
;                   - added nop so bra wasn't penultimate instruction in a
;                     do loop

_rsa_encrypt:

; set processor modes
    
    bset    _CORCON, #IF                ;enable integer arithmetic
    bclr    _CORCON, #SATA              ;disable ACCA saturation
.ifdef __dsPIC33E
    bset    _CORCON, #US0               ;enable unsigned arithmetic
    bclr    _CORCON, #US1
.else
    bset    _CORCON, #US                ;enable unsigned arithmetic
.endif

; save registers

    push.d  w8                          ;save w8, w9 on stack
    push.d  w10                         ;save w10, w11 on stack
    push.d  w12                         ;save w12, w13 on stack
    push    w14                         ;save w14 on stack

; save parameters

    mov     w1, w8                      ;save address of m
    mov     w2, w9                      ;save number of bytes in m
    mov     w5, w10                     ;save address of e
    mov     w6, w11                     ;save number of bytes in e
    mov     w7, w12                     ;save address of xbuf
    mov     [w15-20], w13               ;save address of ybuf
    mov     w4, w14                     ;save k, the number of bytes in n
    
.ifdef __HAS_EDS
    push    DSRPAG                      ;save DSRPAG on stack
    movpag  #1, DSRPAG                  ;set DSRPAG to ydata page
.endif

; check parameters

    ; check that a supported modulus length is being used

    mov     #128, w4                    ;128
    cp      w14, w4                     ;k == 128?
    bra     z, rsa_enc_check_m_len      ;if so, jump to check m size

    mov     #256, w4                    ;256
    cp      w14, w4                     ;k == 256?
    bra     z, rsa_enc_check_m_len      ;if so, jump to check m size

rsa_enc_illegal_size:                       
    mov     #MCL_ILLEGAL_SIZE, w0       ;set error return code
    bra     rsa_enc_done                ;jump to done

rsa_enc_check_m_len:

    ; check m_byte_len is not too large

    sub     w14, #11, w2                ;k - 11
    cp      w9, w2                      ;m_byte_len > k - 11?
    bra     gtu, rsa_enc_illegal_size   ;if so, jump to illegal size

    ; check e_byte_len is not zero and is not too large

    cp0     w11                         ;e_byte_len == 0?
    bra     z, rsa_enc_illegal_size     ;if so, jump to illegal size
    cp      w11, w14                    ;e_byte_len > k?
    bra     gtu, rsa_enc_illegal_size   ;if so, jump to illegal size

    ; check that e is odd

    mov.b    [w5], w6                    ; get lsbyte of e
    btst    w6, #0                        ; lsb of e == 0?
    bra        z, rsa_enc_illegal_param    ; if so, jump to illegal parameter

    ; check that e > 1
    ; find first nonzero byte of e

    add        w5, w11, w1                    ; address of byte following e
    dec     w1, w1                        ; address of msbyte of e
rsa_enc_find_nonzero_e_loop:
    cp0.b    [w1--]                        ; byte of e == 0?
    bra        nz, rsa_enc_check_c            ; if not, jump to check c buffer
    cp        w5, w1                        ; ready to check the lsbyte of e?
    bra        nz, rsa_enc_find_nonzero_e_loop
                                        ; if not, loop

    ; all bytes of e are zero so far: check lsbyte of e

    mov.b   #1, w6                      ;constant 1
    cp.b    w6, [w1]                    ;1 >= lsbyte of e?
    bra     geu, rsa_enc_illegal_param  ;if so, jump to illegal parameter

    ; check that the high k bytes of xbuf can be reversed to c
    ; i.e. the two buffers do not overlap

rsa_enc_check_c:
    add     w12, w14, w7                ;address of xbuf2
    sub     w0, w7, w7                  ;address of c - address of xbuf2
    bra     c, rsa_enc_check_c_cont     ;if no borrow, skip next instruction
    neg     w7, w7                      ;address of xbuf2 - address of c
rsa_enc_check_c_cont:
    cp      w7, w14                     ;difference >= k?
    bra     geu, rsa_enc_check_n        ;if so, jump to check n

rsa_enc_illegal_param:                      
    mov     #MCL_ILLEGAL_PARAMETER, w0  ;set error return code
    bra     rsa_enc_done                ;jump to done

    ; verify that n is odd and 2^((k*8)-1) < n < 2^(k*8)

rsa_enc_check_n:
    mov.b    [w3],w4                        ; get lsbyte of n
    btst    w4, #0                        ; check lsb of n
    bra        z, rsa_enc_illegal_param    ; if not set, jump to illegal parameter
    add        w3, w14, w4                    ; address of byte following n
    mov.b    [w4-1],w1                    ; get msbyte of n
    btst    w1, #7                        ; check msb of n
    bra        z, rsa_enc_illegal_param    ; if not set, jump to illegal parameter

;save address of output ciphertext buffer

    push    w0                          ;save address for ciphertext on stack

; copy n to ybuf1

    mov        w13, w0                        ; pointer to ybuf1
    dec     w14, w1                        ; number of bytes - 1 in n
    repeat    w1                            ; for each of k bytes of n
    mov.b    [w3++], [w0++]                ; copy byte of n into ybuf1

; form RR = (2^8k)^2 mod n and store in ybuf2

    ;  form R mod n = R - n = -n mod 2^8k and store in xbuf2

    mov     w13, w0                     ;address of n (ybuf1)
    add     w12, w14, w1                ;address of xbuf2
    asr     w14, w2                     ;number of words in n
    dec     w2, w2                      ;number of words - 1 in n
    clr     w3                          ;zero
    bset    _SR, #C                     ;set no borrow
    repeat  w2                          ;for each of k/2 words of n
     subb   w3, [w0++], [w1++]          ;negate word of n to xbuf2
    
    ;  form R * R and store in xbuf1 || xbuf2

    mov     w12, w0                     ;address of xbuf1 (for result)
    add     w12, w14, w1                ;address of (R mod n) (xbuf2)
    add     w13, w14, w2                ;address of ybuf2 (temp)
    asr     w14, w3                     ;number of words in R
    rcall   math_vect_squ               ;square (R mod n)

    ;  reduce R * R mod N, and store in ybuf2

    add     w13, w14, w0                ;address of ybuf2 (for result)
    mov     w12, w1                     ;address of R * R (xbuf1)
    mov     w14, w2                     ;number of words in R * R
    mov     w13, w3                     ;address of n (ybuf1)
    asr     w14, w4                     ;number of words in n
    rcall   math_mod_red                ;R * R mod n

; form PKCS-encoded m and store in xbuf2

    ; copy m in reverse byte order into xbuf2

    add     w8, w9, w0                  ;address of byte following m
    add     w12, w14, w8                ;pointer to xbuf2
    dec     w9, w1                      ;number of bytes - 1 in m
    bra     nc, rsa_enc_pad_m           ;if no bytes of m, skip to padding
    repeat  w1                          ;for each byte of m
      mov.b [--w0], [w8++]              ;copy byte of m into xbuf2

rsa_enc_pad_m:

    ; add a zero byte following m in xbuf2

    clr.b   [w8++]                      ;add zero byte to xbuf2

    ; add k - m_byte_len - 3 random bytes following the zero byte in xbuf2

    sub     w14, w9, w9                 ;k - m_byte_len
    sub     #4, w9                      ;number of random bytes - 1 needed
    inc     w9, w2                      ;number of random bytes to generate

    mov     w8, w3                      ;address for random bytes

rsa_enc_pad_m_loop:
    cp        w2, #4
    bra        LT,    rsa_enc_pad_m_final        ; Branch if <= 4 bytes needed
    mov     [w15-LAST_FRAME_OFFSET], w1                ; Move RNG function address to w1
    push.d    w2
    call     w1                            ; Call RNG to generate four bytes
    pop.d    w2
    mov.b    w0, [w3++]                    ; Move random byte into the buffer
    lsr        w0, #8, w0
    mov.b    w0, [w3++]
    mov.b    w1, [w3++]                    ; Move random byte into the buffer
    lsr        w1, #8, w1
    mov.b    w1, [w3++]
    sub        #4, w2
    bra        rsa_enc_pad_m_loop            ; loop

rsa_enc_pad_m_final:
    cp0        w2
    bra        Z, rsa_enc_check_pad_start
    mov        [w15-LAST_FRAME_OFFSET],w1
    push.d    w2
    call    w1
    pop.d    w2
    mov.b    w0, [w3++]
    dec        w2, w2
    cp0        w2
    bra        Z, rsa_enc_check_pad_start
    lsr        w2, #8, w2
    mov.b    w0, [w3++]
    dec        w2, w2
    cp0        w2
    bra        Z, rsa_enc_check_pad_start
    mov.b    w1, [w3++]


rsa_enc_check_pad_start:

    ; check that the random bytes are all non-zero

    do      w9, rsa_enc_check_random_loop_end
                                        ;for each random byte
rsa_enc_check_pad_nonzero:
     cp0.b  [w8++]                      ;is the random byte zero?
     bra    nz, rsa_enc_check_random_loop_cont
                                        ;if not, check next byte
     dec    w8, w8                      ;pointer to zero random byte
     mov [w15-LAST_FRAME_OFFSET], w1
     call     w1
     mov.b    w0, [w8]
;     rcall  _drbg_generate              ;overwrite the zero random byte with a
                                        ; new random byte
     bra    rsa_enc_check_pad_nonzero   ;loop to check that the new random byte
                                        ; isn't zero
rsa_enc_check_random_loop_cont:
    nop                                 ;can't have bra as penultimate
                                        ; instruction in do loop
rsa_enc_check_random_loop_end:
     nop                                ;can't end do loop with bra

    ; xbuf2[k-2] = 0x02, xbuf2[k-1] = 0x00

    mov     #0x0002, w0                 ;0x02, 0x00
    mov     w0, [w8++]                  ;put 0x02, 0x00 into xbuf2 following
                                        ; the random bytes

; calc v

    mov     w13, w0                     ;pointer to lsword of n (ybuf1)
    rcall   mont_calc_v                 ;calculate v
    mov     w0, w6                      ;save v

; form c = m^e mod n and store in xbuf2

    mov     w12, w0                     ;address of xbuf1
    add     w12, w14, w1                ;address of PKCS-encoded m (xbuf2)
    mov     w10, w2                     ;address of e
    mov     w11, w3                     ;number of bytes in e
    mov     w13, w4                     ;address of n
    asr     w14, w5                     ;number of words in n
    add     w13, w14, w7                ;address of working buffer (ybuf2)
    push    w7                          ;push address of RR on stack
    rcall   math_mod_exp                ;form ciphertext
    pop     w0                          ;restore address of RR from stack

; copy c in reverse byte order to output buffer

    add     w12, w14, w0                ;address of c (xbuf2)
    add     w0, w14, w0                 ;address of byte following xbuf2
    pop     w1                          ;address of ciphertext output buffer
    dec     w14, w2                     ;number of bytes - 1 in ciphertext
    repeat  w2                          ;for each byte of c
     mov.b  [--w0], [w1++]              ;copy byte into ciphertext output buffer

    mov     #MCL_SUCCESS, w0            ;set success return code

; clean up registers and stack

    mov     w15, w14                    ;copy stack pointer
    repeat  #32                         ;for each of 33 words
     clr    [w14++]                     ;clear previously used stack space
    clr     A                           ;clear ACCA
    clr     B                           ;clear ACCB
rsa_enc_done:
    mov     #_WREG7, w14                ;address of w7
    repeat  #6                          ;for 7 registers
     clr    [w14--]                     ;clear registers w7 through w1

; restore registers and return

.ifdef __HAS_EDS
    pop     DSRPAG                      ;restore DSRPAG from stack
.endif

    pop     w14                         ;restore w14 from stack
    pop.d   w12                         ;restore w12, w13 from stack
    pop.d   w10                         ;restore w10, w11 from stack
    pop.d   w8                          ;restore w8, w9 from stack

    return                              ;return from _rsa_encrypt()


;****************************************************************************
; ROUTINE       _rsa_decrypt
;
; DESCRIPTION
;               A function to perform RSA decryption of a ciphertext.
;               If the RSA public modulus (and hence ciphertext) is k bytes, 
;               then the message can be at most k-11 bytes.
;
; PARAMETERS
;               w0  - pointer to the plaintext message buffer, m, large
;                     enough to contain the message, which is <= k-11 bytes.
;                     (m may not overlap with the bottom k-11 bytes of xbuf)
;               w1  - pointer to the number of bytes in m returned
;               w2  - pointer to the ciphertext, c
;               w3  - k, the number of bytes in c
;               w4  - pointer to the private key block, sk, which holds
;                     p, q, dp, dp, qinv, each k/2 aligned(2) bytes
;               w5  - pointer to xdata working buffer, length 3k/2 aligned(2)
;                     bytes, thought of as (xbuf1 || xbuf2 || xbuf3)
;               w6  - pointer to ydata working buffer, length 3k/2 aligned(2)
;                     bytes, thought of as (ybuf1 || ybuf2 || ybuf3)
;
; RETURN VALUES
;               w0 - return code
;                       MCL_SUCCESS
;                       MCL_ILLEGAL_PARAMETER
;                       MCL_ILLEGAL_SIZE
;                       MCL_INVALID_CIPHERTEXT
;               w1 - 0
;               w2 - 0
;               w3 - 0
;               w4 - 0
;               w5 - 0
;               w6 - 0
;               w7 - 0
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
;               w8  - k/2, number of bytes in p, q
;               w9  - address of the ciphertext, c
;               w10 - address of xbuf
;               w11 - address of ybuf
;               w12 - address of sk
;               w13 - address for m
;               w14 - address for the number of bytes in m
;               w15 - stack pointer
;
; SUBROUTINES CALLED
;               rsa_crt_exp()
;               rsa_crt_lift()
;
; STACK USED
;               41 words
;
; TIMING
;               1024-bit modulus: (shortest to longest plaintext message)
;                   30F: 4545178 - 4548073 cycles
;                   33F: 4638203 - 4641098 cycles
;                   33E: 4679791 - 4682583 cycles
;
;               2048-bit modulus: (shortest to longest plaintext message)
;                   30F: 28732731 - 28735129 cycles
;                   33F: 29117731 - 29120129 cycles
;                   33E: 29201882 - 29203836 cycles
;
; AUTHOR        N. A. Howgrave-Graham, NTRU Cryptosystems, Inc.
;
; DATE          11/20/2003
;
; NOTES
;               The message and ciphertext buffers are given in network 
;               byte order.
;
;               The private key block sk holds p, q, dp, dq, qinv, and should
;               be obtained from the RSA key geneartion function, in the 
;               accompanying C library.
;
; CHANGES
;               - 05/14/2004 mhe
;                   - removed clearing buffers in case output buffer overlapped
;
;               - 09/05/2011 mhe
;                   - set two CORCON bits for unsigned mode
;                   - saved, set, and restored DSRPAG to ensure access to
;                     ydata for devices with EDS

_rsa_decrypt:

; set the relevant corcon bits
    
    bset    _CORCON, #IF                ;enable integer arithmetic
    bclr    _CORCON, #SATA              ;disable ACCA saturation
.ifdef __dsPIC33E
    bset    _CORCON, #US0               ;enable unsigned arithmetic
    bclr    _CORCON, #US1
.else
    bset    _CORCON, #US                ;enable unsigned arithmetic
.endif

; save registers

    push.d  w8                          ;save w8, w9 on stack
    push.d  w10                         ;save w10, w11 on stack
    push.d  w12                         ;save w12, w13 on stack
    push    w14                         ;save w14 on stack

; store some useful quantities

    asr     w3, w8                      ;save number of bytes in p, q
    mov     w2, w9                      ;save address of ciphertext
    mov     w5, w10                     ;save address of xbuf
    mov     w6, w11                     ;save address of ybuf
    mov     w4, w12                     ;save address of secret key
    mov     w0, w13                     ;save address for plaintext
    mov     w1, w14                     ;save address for plaintext byte length

.ifdef __HAS_EDS
    push    DSRPAG                      ;save DSRPAG on stack
    movpag  #1, DSRPAG                  ;set DSRPAG to ydata page
.endif

; check parameters

    ; check that a supported modulus length is being used

    mov     #128, w7                    ;128
    cp      w3, w7                      ;k == 128?
    bra     z, rsa_dec_check_m_location ;if so, jump to check m location

    mov     #256, w7                    ;256
    cp      w3, w7                      ;k == 256?
    bra     z, rsa_dec_check_m_location ;if so, jump to check m location

rsa_dec_illegal_size:                       
    mov     #MCL_ILLEGAL_SIZE, w0       ;set error return code
    bra     rsa_dec_done                ;jump to done

rsa_dec_check_m_location:

    ; check that the low k-11 bytes of xbuf can be reversed to m
    ; i.e. the two buffers do not overlap

    sub     w3, #11, w7                 ;k - 11
    sub     w0, w10, w1                 ;address of m - address of xbuf
    bra     c, rsa_dec_check_m          ;if no borrow, skip next instruction
    neg     w1, w1                      ;address of xbuf - address of m
rsa_dec_check_m:
    cp      w1, w7                      ;difference >= k-11?
    bra     geu, rsa_dec_check_p        ;if so, jump to check p

rsa_dec_illegal_param:                      
    mov     #MCL_ILLEGAL_PARAMETER, w0  ;set error return code
    bra     rsa_dec_done                ;jump to done

    ; verify that p is odd and 2^((8*k/2)-1) < p < 2^(8*k/2)

rsa_dec_check_p:                           
    mov        [w12+2], w4                    ;move pointer to p to w4
    btst    [w4], #0                    ;check lsb of p
    bra     z, rsa_dec_illegal_param    ;if not set, skip to illegal parameter
    add     w4, w8, w4                  ;address of byte following p
    mov.b   [w4-1], w7                  ;get msbyte of p
    btst    w7, #7                      ;check msb of p
    bra     z, rsa_dec_illegal_param    ;if not set, skip to illegal parameter

    ; verify that q is odd and 2^((8*k/2)-1) < q < 2^(8*k/2)

    mov        [w12+4],w4                    ;move pointer to q to w4
    btst    [w4], #0                    ;check lsb of q
    bra     z, rsa_dec_illegal_param    ;if not set, skip to illegal parameter
    add     w4, w8, w4                  ;address of byte following q
    mov.b   [w4-1], w7                  ;get msbyte of q
    btst    w7, #7                      ;check msb of q
    bra     z, rsa_dec_illegal_param    ;if not set, skip to illegal parameter

; form mq = c^dq mod q and store in xbuf3

    mov     w9, w0                      ;address of ciphertext
;    add     w12, w8, w1                 ;address of q (sk + k/2 bytes)
;    add     w1, w8, w2                  ;address of dp (sk + 2k/2 bytes)
;    add     w2, w8, w2                  ;address of dq (sk + 3k/2 bytes)
    mov        [w12+4],w1                    ;move address of q to w1
    mov        [w12+8],w2                    ;move address of dq to w2
    add     w10, w8, w10                ;address of xbuf2
    rcall   rsa_crt_exp                 ;calculate mq = (c mod q)^dq mod q
    sub     w10, w8, w10                ;restore address of xbuf1

; form mp = c^dp mod p and store in xbuf2
;  and note that a y-data copy of p will be returned in ybuf1

    mov     w9, w0                      ;address of ciphertext
;    mov     w12, w1                     ;address of p (sk)
;    add     w12, w8, w2                 ;address of q (sk + k/2 bytes)
;    add     w2, w8, w2                  ;address of dp (sk + 2k/2 bytes)
    mov        [w12+2],w1                    ;address of p (sk)
    mov        [w12+6],w2                    ;address of dp (sk)
    rcall   rsa_crt_exp                 ;calculate mp = (c mod p)^dp mod p
    
; lift mp and mq to mn and store in xbuf1 || xbuf2

    rcall   rsa_crt_lift                ;form mn = m mod n
                                        ; (corrupts w9)

; check valid padding

    ; check that first two bytes are 0x02, 0x00

    add     w8, w8, w3                  ;k bytes
    add     w10, w3, w0                 ;address of byte following m
                                        ; (following xbuf2)
    sub     w0, #11, w2                 ;highest valid address for end of
                                        ; padding
    mov     #0x0002, w1
    cp      w1, [--w0]                  ;see if the top bytes are 0x02 || 0x00
    bra     nz, rsa_dec_failed          ;if not decryption has failed

    ; find end of padding (first 0x00)

rsa_dec_end_of_padding_loop:
    cp0.b   [--w0]                      ;is next byte 0?
    bra     z, rsa_dec_found_end_of_padding
                                        ;if so, stop looking
    cp      w0, w10                     ;all bytes been looked at?
    bra     gtu, rsa_dec_end_of_padding_loop
                                        ;if not, loop

rsa_dec_failed: 
    mov     #MCL_INVALID_CIPHERTEXT, w0 ;set error return code
    bra     rsa_dec_cleanup             ;jump to cleanup

rsa_dec_found_end_of_padding:                         

    ; zero in valid position (message not >= k-11 bytes)?

    cp      w0, w2                      ;end of padding > highest valid address?
    bra     gtu, rsa_dec_failed         ;if so, jump to failed

; successful decryption:

    ; get message byte length, and check if zero

    sub     w0, w10, [w14]              ;write m_byte_len
    cp0     [w14]                       ;number of bytes in m == 0?
    bra     z, rsa_dec_success          ;if so, skip copying m

    ; nonzero message length: copy mn in reverse byte order to m buffer

    dec     [w14], w1                   ;number of bytes - 1 in m
    repeat  w1                          ;for each byte in m
     mov.b  [--w0], [w13++]             ;copy byte to m buffer

rsa_dec_success:                            
    mov     #MCL_SUCCESS, w0            ;set return code
    
; clean up registers and stack

rsa_dec_cleanup:
    mov     w15, w14                    ;copy stack pointer
    repeat  #33                         ;for each of 34 words
     clr    [w14++]                     ;clear previously used stack space
    clr     A                           ;clear ACCA
    clr     B                           ;clear ACCB
rsa_dec_done:
    mov     #_WREG7, w14                ;address of w7
    repeat  #6                          ;for 7 registers
     clr    [w14--]                     ;clear registers w7 through w1

; restore registers

.ifdef __HAS_EDS
    pop     DSRPAG                      ;restore DSRPAG from stack
.endif

    pop     w14                         ;restore w14 from stack
    pop.d   w12                         ;restore w12, w13 from stack
    pop.d   w10                         ;restore w10, w11 from stack
    pop.d   w8                          ;restore w8, w9 from stack

    return                              ;return from _rsa_decrypt()


.end
