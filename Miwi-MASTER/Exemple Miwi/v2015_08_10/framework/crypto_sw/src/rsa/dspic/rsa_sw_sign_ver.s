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
; FILE          rsa_sign_ver.s
;
;
; DESCRIPTION   This file contains routines to implement RSA signing and
;               verification on Microchip dsPICs.
;
;
; ROUTINES
;               _rsa_sign()    -  RSA signing
;               _rsa_verify()  -  RSA verification
;
; CHANGES
;               - 05/14/2004 mhe
;                   - changed _rsa_sign()
;
;               - 09/05/2011 mhe
;                   - changed _rsa_sign(), _rsa_verify()
;

; include files

.include "proc.inc"                     ;common device definitions
.include "ret_codes.inc"                ;return codes
.include "drbg.inc"                     ;pseudorandom number generation fns

; function declarations

.global _rsa_sign
.global _rsa_verify

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
; ROUTINE       _rsa_sign
;
; DESCRIPTION
;               A function to perform RSA signing of a message hash.
;
; PARAMETERS
;               w0  - pointer to the signature buffer, s
;                   - (may not overlap with the bottom k bytes of xbuf)
;               w1  - s_byte_len (or k), no. of bytes in s, must be 128 or 256
;               w2  - pointer to the message hash, h
;               w3  - h_byte_len, number of bytes in h
;               w4  - pointer to the ASN.1 hash identification string, asn
;               w5  - asn_byte_len, number of bytes in asn
;               w6  - pointer to the private key block, sk, which holds
;                     p, q, dp, dp, qinv, each k/2 aligned(2) bytes
;               w7  - xbuf (xdata working buffer, length 3k/2 aligned(2) bytes)
;                   - thought of as (xbuf1 || xbuf2 || xbuf3)
;               [w15-6] - ybuf (ydata working buffer, length 3k/2 aligned(2)
;                         bytes)
;                       - thought of as (ybuf1 || ybuf2 || ybuf3)
;
; RETURN VALUES
;               w0 - return code
;                       MCL_SUCCESS
;                       MCL_ILLEGAL_SIZE
;                       MCL_ILLEGAL_PARAMETER
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
;               w9  - address of h
;               w10 - address of xbuf
;               w11 - address of ybuf
;               w12 - address of sk
;               w13 - number of bytes in h
;               w14 - number of bytes in asn
;               w15 - stack pointer
;
; SUBROUTINES CALLED
;               rsa_crt_exp()
;               rsa_crt_lift()
;
; STACK USED
;               43 words
;
; TIMING
;               1024-bit modulus:
;                   30F:  4547494 cycles typical
;                   33F:  4640519 cycles typical
;                   33E:  4681981 cycles typical
;
;               2048-bit modulus:
;                   30F: 28704890 cycles typical
;                   33F: 29089890 cycles typical
;                   33E: 29173127 cycles typical
;
; AUTHOR        N. A. Howgrave-Graham, NTRU Cryptosystems, Inc.
;
; DATE          11/20/2003
;
; NOTES
;               - All buffers (e.g. message, signature, hash, asn ID) are 
;                 given in network byte order.
;
;               - The private key block sk holds p, q, dp, dq, qinv, and should
;                 be obtained from the RSA key geneartion function in the 
;                 accompanying C library.
;
; CHANGES
;               - 05/14/2004 mhe
;                   - removed clearing buffers in case output buffer overlapped
;
;               - 09/05/2011 mhe
;                   - set two CORCON bits for unsigned mode
;                   - saved, set, and restored DSRPAG to ensure access to
;                     ydata for devices with EDS

_rsa_sign:

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

.ifdef __HAS_EDS
    push    DSRPAG                      ;save DSRPAG on stack
    movpag  #1, DSRPAG                  ;set DSRPAG to ydata page
.else
	push	w14							;dummy push for non-EDS processors
.endif

; store some useful quantities

    asr     w1, w8                      ;save number of bytes in p, q
    mov     w2, w9                      ;save address  of h
    mov     w7, w10                     ;save address of xbuf
    mov     [w15-22], w11               ;save address of ybuf
    mov     w6, w12                     ;save address of secret key
    mov     w3, w13                     ;save number of bytes in h
    mov     w5, w14                     ;save number of bytes in asn
    mov     w0, [w15-22]                ;save address for signature output

; check parameters

    ; check that a supported modulus length is being used

    mov     #128, w2                    ;128
    cp      w1, w2                      ;k == 128?
    bra     z, rsa_sign_check_h_len     ;if so, jump to check h size

    mov     #256, w2                    ;256
    cp      w1, w2                      ;k == 256?
    bra     z, rsa_sign_check_h_len     ;if so, jump to check h size

rsa_sign_illegal_size:                       
    mov     #MCL_ILLEGAL_SIZE, w0       ;set error return code
    bra     rsa_sign_done               ;jump to done

rsa_sign_check_h_len:

    ; check h_byte_len is not zero and is not too large

    cp0     w3                          ;h_byte_len == 0?
    bra     z, rsa_sign_illegal_size    ;if so, jump to illegal size
    mov     #64, w2                     ;the largest supported hash (SHA-512)
    cp      w3, w2                      ;h_byte_len > 64?
    bra     gtu, rsa_sign_illegal_size  ;if so, jump to illegal size

    ; check asn_byte_len is not zero and is not too large

    cp0     w5                          ;asn_byte_len == 0?
    bra     z, rsa_sign_illegal_size    ;if so, jump to illegal size
    cp      w5, #19                     ;asn_byte_len > 19 (max ASN.1 hash ID)?
    bra     gtu, rsa_sign_illegal_size  ;if so, jump to illegal size

    ; check that the low k bytes of xbuf can be reversed to s
    ; i.e. the two buffers do not overlap

    sub     w0, w10, w7                 ;address of c - address of xbuf
    bra     c, rsa_sign_check_s         ;if no borrow, skip next instruction
    neg     w7, w7                      ;address of xbuf - address of c
rsa_sign_check_s:
    cp      w7, w1                      ;difference >= k?
    bra     geu, rsa_sign_check_p       ;if so, jump to check p

rsa_sign_illegal_param:                      
    mov     #MCL_ILLEGAL_PARAMETER, w0  ;set error return code
    bra     rsa_sign_done               ;jump to done

    ; verify that p is odd and 2^((8*k/2)-1) < p < 2^(8*k/2)

rsa_sign_check_p:                           
    btst    [w6], #0                    ;check lsb of p
    bra     z, rsa_sign_illegal_param   ;if not set, skip to illegal parameter
    add     w6, w8, w6                  ;address of byte following p
    mov.b   [w6-1], w2                  ;get msbyte of p
    btst    w2, #7                      ;check msb of p
    bra     z, rsa_sign_illegal_param   ;if not set, skip to illegal parameter

    ; verify that q is odd and 2^((8*k/2)-1) < q < 2^(8*k/2)

    btst    [w6], #0                    ;check lsb of q
    bra     z, rsa_sign_illegal_param   ;if not set, skip to illegal parameter
    add     w6, w8, w6                  ;address of byte following q
    mov.b   [w6-1], w2                  ;get msbyte of q
    btst    w2, #7                      ;check msb of q
    bra     z, rsa_sign_illegal_param   ;if not set, skip to illegal parameter

; form m (in network byte order) in ybuf2 || ybuf3

    push    w4                          ;save address of asn ID string on stack
    rcall   rsa_sign_form_m             ;form m

; form sq = m^dq mod q and store in xbuf3
    
    add     w11, w8, w0                 ;address of m (ybuf2)
    add     w12, w8, w1                 ;address of q (sk + k/2 bytes)
    add     w1, w8, w2                  ;address of dp (sk + 2k/2 bytes)
    add     w2, w8, w2                  ;address of dq (sk + 3k/2 bytes)
    add     w10, w8, w10                ;address of xbuf2
    rcall   rsa_crt_exp                 ;calculate sq = (m mod q)^dq mod q
    sub     w10, w8, w10                ;restore address of xbuf1

; form m (in network byte order) in ybuf2 || ybuf3

    pop     w4                          ;retrieve address of asn ID string from
                                        ; stack
    rcall   rsa_sign_form_m             ;form m

; form sp = m^dp mod p and store in xbuf2
; and note that a y-data copy of p will be returned in ybuf1

    add     w11, w8, w0                 ;address of m (ybuf2)
    mov     w12, w1                     ;address of p (sk)
    add     w12, w8, w2                 ;address of q (sk + k/2 bytes)
    add     w2, w8, w2                  ;address of dp (sk + 2k/2 bytes)
    rcall   rsa_crt_exp                 ;calculate sp = (m mod p)^dp mod p
    
; lift sp and sq to sn and store in xbuf1 || xbuf2

    rcall   rsa_crt_lift                ;form sn = s mod n
                                        ; (corrupts w9)

; copy sn in reverse byte order to s output buffer

    mov     [w15-22], w3                ;get address of s buffer from stack
    add     w8, w8, w2                  ;number of bytes in sn
    add     w10, w2, w0                 ;pointer to byte following xbuf2
    dec     w2, w1                      ;number of bytes - 1 in sn
    repeat  w1                          ;for each byte of sn
     mov.b  [--w0], [w3++]              ;copy byte of sn into s buffer

    mov     #MCL_SUCCESS, w0            ;return with MCL_SUCCESS

; clean up registers and stack

    mov     w15, w14                    ;copy stack pointer
    repeat  #34                         ;for each of 35 words
     clr    [w14++]                     ;clear previously used stack space
    clr     A                           ;clear ACCA
    clr     B                           ;clear ACCB
rsa_sign_done:
    mov     #_WREG7, w14                ;address of w7
    repeat  #6                          ;for 7 registers
     clr    [w14--]                     ;clear registers w7 through w1

; restore registers

.ifdef __HAS_EDS
    pop     DSRPAG                      ;restore DSRPAG from stack
.else
	pop		w14							;pop dummy push for non-EDS processors
.endif

    pop     w14                         ;restore w14 from stack
    pop.d   w12                         ;restore w12, w13 from stack
    pop.d   w10                         ;restore w10, w11 from stack
    pop.d   w8                          ;restore w8, w9 from stack

    return                              ;return from _rsa_sign()


;**************************************************************************
; form m (in network byte order) in ybuf2 || ybuf3
;
; pointer to asn ID string is in w4

rsa_sign_form_m:

    ; set the top two bytes to 0x01, 0x00

    add     w11, w8, w0                 ;address of ybuf2
    mov     #0x0100, w1                 ;0x00, 0x01
    mov     w1, [w0++]                  ;set top two bytes

    ; set the proper number of subsequent bytes to 0xff

    sl      w8, w1                      ;number of bytes in m
    sub     w1, w13, w1                 ;number of bytes not h in m
    sub     w1, w14, w1                 ;number of bytes not h or asn in m
    sub     #4, w1                      ;number of bytes - 1 of 0xff in m
    repeat  w1                          ;for each 0xff byte
     setm.b [w0++]                      ;set byte to 0xff

    ; add the 0x00 byte

    clr.b   [w0++]                      ;next byte = 0x00

    ; copy asn into m buffer
    
    dec     w14, w1                     ;number of bytes - 1 in asn
    repeat  w1                          ;for each byte of asn
     mov.b  [w4++], [w0++]              ;copy asn byte into m buffer
    
    ; copy h into m buffer

    mov     w9, w2                      ;pointer to h
    dec     w13, w1                     ;number of bytes - 1 in h
    repeat  w1                          ;for each byte of h
     mov.b  [w2++], [w0++]              ;copy h byte into m buffer

    return                              ;return from rsa_sign_form_m()
    

;****************************************************************************
; ROUTINE       _rsa_verify
;
; DESCRIPTION
;               This routine performs RSA verification of a 
;               (message, signature) pair.
;
; PARAMETERS
;               w0  - pointer to the signature buffer, s
;               w1  - pointer to the message hash, h
;               w2  - h_byte_len, number of bytes in h
;               w3  - pointer to the ASN.1 hash identification string, asn
;               w4  - asn_byte_len, number of bytes in asn
;               w5  - pointer to the RSA modulus buffer, n
;               w6  - n_byte_len (or k), no. of bytes in n, must be 128 or 256
;               w7  - pointer to the RSA public exponent buffer, e
;               [w15-6] - e_byte_len, number of bytes in e (at most k bytes)
;               [w15-8] - pointer to xdata working buffer, 2*k aligned(2) bytes
;                         thought of as (xbuf1 || xbuf2)
;               [w15-10] - pointer to ydata working buffer, 3*k aligned(2) bytes
;                          thought of as (ybuf1 || ybuf2 || ybuf3)
;
; RETURN VALUES
;               w0 - return code
;                       MCL_SUCCESS
;                       MCL_ILLEGAL_PARAMETER
;                       MCL_ILLEGAL_SIZE
;                       MCL_INVALID_SIGNATURE
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
;               w8  - address of s
;               w9  - number of bytes in h
;               w10 - number of bytes in asn
;               w11 - address of e
;               w12 - address of xbuf
;               w13 - address of ybuf
;               w14 - k, number of bytes in n
;               w15 - stack pointer
;
; SUBROUTINES CALLED
;               math_mod_exp()
;               math_mod_red()
;               math_vect_squ()
;               mont_calc_v()
;
; STACK USED
;               41 words
;
; TIMING
;               1024-bit modulus, e = 2^16 + 1:
;                   30F: 190443 cycles typical
;                   33F: 192818 cycles typical
;                   33E: 194346 cycles typical
;
;               2048-bit modulus, e = 2^16 + 1:
;                   30F: 653855 cycles typical
;                   33F: 658662 cycles typical
;                   33E: 661150 cycles typical
;
; AUTHOR        N. A. Howgrave-Graham, NTRU Cryptosystems, Inc.
;
; DATE          11/20/2003
;
; NOTES
;               All buffers (e.g. ciphertext, message, RSA modulus, RSA
;               exponent) are given in network byte order.
;
; CHANGES
;               - 09/05/2011 mhe
;                   - set two CORCON bits for unsigned mode
;                   - saved, set, and restored DSRPAG to ensure access to
;                     ydata for devices with EDS
;

_rsa_verify:

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

.ifdef __HAS_EDS
    push    DSRPAG                      ;save DSRPAG on stack
    movpag  #1, DSRPAG                  ;set DSRPAG to ydata page
.else
	push	w14							;dummy push for no-EDS processors
.endif

; save parameters

    mov     w0, w8                      ;save address of s
    mov     w2, w9                      ;save number of bytes in h
    mov     w4, w10                     ;save number of bytes in asn
    mov     w7, w11                     ;save address of e
    mov     [w15-24], w12               ;save address of xbuf
    mov     [w15-26], w13               ;save address of ybuf
    mov     w6, w14                     ;save k, the number of bytes in n
    mov     w1, [w15-24]                ;save address of h
    mov     w3, [w15-26]                ;save address of asn
    
; check parameters

    ; check that a supported modulus length is being used

    mov     #128, w6                    ;128
    cp      w14, w6                     ;k == 128?
    bra     z, rsa_ver_check_h_len      ;if so, jump to check m size

    mov     #256, w6                    ;256
    cp      w14, w6                     ;k == 256?
    bra     z, rsa_ver_check_h_len      ;if so, jump to check m size

rsa_ver_illegal_size:                       
    mov     #MCL_ILLEGAL_SIZE, w0       ;set error return code
    bra     rsa_ver_done                ;jump to done


    ; check h_byte_len is not zero and is not too large

rsa_ver_check_h_len:
    cp0     w2                          ;h_byte_len == 0?
    bra     z, rsa_ver_illegal_size     ;if so, jump to illegal size
    mov     #64, w0                     ;the largest supported hash (SHA-512)
    cp      w2, w0                      ;h_byte_len > 64?
    bra     gtu, rsa_ver_illegal_size   ;if so, jump to illegal size

    ; check asn_byte_len is not zero and is not too large

    cp0     w4                          ;asn_byte_len == 0?
    bra     z, rsa_ver_illegal_size     ;if so, jump to illegal size
    cp      w4, #19                     ;asn_byte_len > 19 (max ASN.1 hash ID)?
    bra     gtu, rsa_ver_illegal_size   ;if so, jump to illegal size

    ; check e_byte_len is not zero and is not too large

    mov     [w15-22], w0                ;get e_byte_len from stack
    cp0     w0                          ;e_byte_len == 0?
    bra     z, rsa_ver_illegal_size     ;if so, jump to illegal size
    cp      w0, w14                     ;e_byte_len > k?
    bra     gtu, rsa_ver_illegal_size   ;if so, jump to illegal size

    ; check that e is odd

    add     w7, w0, w1                  ;address of byte following e
    dec     w1, w1                      ;address of lsbyte of e
    mov.b   [w1], w0                    ;get lsbyte of e
    btst    w0, #0                      ;lsb of e == 0?
    bra     z, rsa_ver_illegal_param    ;if so, jump to illegal parameter

    ; check that e > 1
    ; find first nonzero byte of e

rsa_ver_find_nonzero_e_loop:
    cp0.b   [w7++]                      ;byte of e == 0?
    bra     nz, rsa_ver_check_n         ;if not, jump to check n
    cp      w7, w1                      ;ready to check the lsbyte of e?
    bra     nz, rsa_ver_find_nonzero_e_loop
                                        ;if not, loop

    ; all bytes of e are zero so far: check lsbyte of e

    mov.b   #1, w0                      ;constant 1
    cp.b    w0, [w7]                    ;1 >= lsbyte of e?
    bra     geu, rsa_ver_illegal_param  ;if so, jump to illegal parameter

    ; verify that n is odd and 2^((k*8)-1) < n < 2^(k*8)

rsa_ver_check_n:
    mov.b   [w5], w1                    ;get msbyte of n
    btst    w1, #7                      ;check msb of n
    bra     z, rsa_ver_illegal_param    ;if not set, skip to illegal parameter
    add     w5, w14, w1                 ;address of byte following n
    mov.b   [w1-1], w0                  ;get lsbyte of n
    btst    w0, #0                      ;check lsb of n
    bra     nz, rsa_ver_reverse_n       ;if set, skip to reverse n

rsa_ver_illegal_param:                      
    mov     #MCL_ILLEGAL_PARAMETER, w0  ;set error return code
    bra     rsa_ver_done                ;jump to done

rsa_ver_reverse_n:           

; copy n in reverse byte order to ybuf1

    add     w13, w14, w0                ;address of byte following ybuf1
    dec     w14, w1                     ;number of bytes - 1 in n
    repeat  w1                          ;for each of the k bytes in n
     mov.b  [w5++], [--w0]              ;copy byte of n into ybuf1

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
    rcall   math_vect_squ               ;form R * R

    ;  reduce R * R mod N, and store in ybuf2

    add     w13, w14, w0                ;address of ybuf2 (for result)
    mov     w12, w1                     ;address of R * R (xbuf1)
    mov     w14, w2                     ;number of words in R * R
    mov     w13, w3                     ;address of n (ybuf1)
    asr     w14, w4                     ;number of words in n
    rcall   math_mod_red                ;R * R mod n

; copy s in reverse byte order into xbuf2

    add     w12, w14, w0                ;address of xbuf2
    add     w0, w14, w0                 ;address of byte following xbuf2
    dec     w14, w1                     ;number of bytes - 1 in s
    repeat  w1                          ;for each byte in s
     mov.b  [w8++], [--w0]              ;copy byte of s into xbuf2

; calc v

    mov     w13, w0                     ;pointer to lsword of n (ybuf1)
    rcall   mont_calc_v                 ;calculate v
    mov     w0, w6                      ;save v

; form m = s^e mod n and store in xbuf2

    mov     w12, w0                     ;address of xbuf1
    add     w12, w14, w1                ;address of s (xbuf2)
    mov     w11, w2                     ;address of e
    mov     [w15-22], w3                ;number of bytes in e
    mov     w13, w4                     ;address of n
    asr     w14, w5                     ;number of words in n
    add     w13, w14, w7                ;address of working buffer (ybuf2)
    push    w7                          ;push address of RR on stack
    rcall   math_mod_exp                ;form message that was signed
    pop     w0                          ;restore address of RR from stack

; check value and format of m

    add     w12, w14, w0                ;address of xbuf2
    add     w0, w14, w0                 ;address of byte following xbuf2

    ; check that the top two bytes are 0x01, 0x00

    mov     #0x0001, w1                 ;0x01, 0x00
    cp      w1, [--w0]                  ;first two bytes of m == 0x01, 0x00?
    bra     z, rsa_ver_check_padding    ;if so, jump to check padding

rsa_ver_invalid_sig:                        
    mov     #MCL_INVALID_SIGNATURE, w0  ;set error return code
    bra     rsa_ver_cleanup             ;jump to cleanup

rsa_ver_check_padding:

    ; check that the proper number of subsequent bytes are 0xff

    add     w9, w10, w8                 ;number of bytes in h + asn
    sub     w14, w8, w8                 ;number of bytes not h or asn
    sub     #3, w8                      ;number of bytes of 0xff
    mov.b   #0xff, w1                   ;constant 0xff
rsa_ver_check_ff_loop:
    cp.b    w1, [--w0]                  ;byte of m == 0xff?
    bra     nz, rsa_ver_invalid_sig     ;if not, jump to invalid sig
    dec     w8, w8                      ;byte counter--
    bra     nz, rsa_ver_check_ff_loop   ;if more bytes to check, loop

    ; check for the 0x00 byte

    cp0.b   [--w0]                      ;next byte of m == 0?
    bra     nz, rsa_ver_invalid_sig     ;if not, jump to invalid sig

    ; check that the next asn_byte_len bytes match the ASN.1 hash ID buffer

    mov     [w15-26], w1                ;pointer to asn
rsa_ver_check_asn_loop:
    mov.b   [--w0], w2                  ;next byte of m
    cp.b    w2, [w1++]                  ;compare to byte of asn hash ID
    bra     nz, rsa_ver_invalid_sig     ;if not the same, jump to invalid sig
    dec     w10, w10                    ;byte counter--
    bra     nz, rsa_ver_check_asn_loop  ;if more bytes to check, loop

    ; check that the next h_byte_len bytes match the hash buffer

    mov     [w15-24], w1                ;pointer to h
rsa_ver_check_h_loop:
    mov.b   [--w0], w2                  ;next byte of m
    cp.b    w2, [w1++]                  ;compare to byte of h
    bra     nz, rsa_ver_invalid_sig     ;if not the same, jump to invalid sig
    dec     w9, w9                      ;byte counter--
    bra     nz, rsa_ver_check_h_loop    ;if more bytes to check, loop

    mov     #MCL_SUCCESS, w0            ;set success return code

; clean up buffers and stack

rsa_ver_cleanup:
    dec     w14, w1                     ;number of words - 1 in xbuf
    repeat  w1                          ;for each word in xbuf
     clr    [w12++]                     ;zero word of xbuf
    lsr     w14, w14                    ;number of words in n
    add     w1, w14, w1                 ;number of words - 1 in ybuf
    repeat  w1                          ;for each word in ybuf
     clr    [w13++]                     ;zero word of ybuf
    mov     w15, w14                    ;copy stack pointer
    repeat  #31                         ;for each of 32 words
     clr    [w14++]                     ;clear previously used stack space
    clr     A                           ;clear ACCA
    clr     B                           ;clear ACCB
rsa_ver_done:
    mov     #_WREG7, w14                ;address of w7
    repeat  #6                          ;for 7 registers
     clr    [w14--]                     ;clear registers w7 through w1

; restore registers and return

.ifdef __HAS_EDS
    pop     DSRPAG                      ;restore DSRPAG from stack
.else
	pop		w14							;pop dummy push for non-EDS processors
.endif

    pop     w14                         ;restore w14 from stack
    pop.d   w12                         ;restore w12, w13 from stack
    pop.d   w10                         ;restore w10, w11 from stack
    pop.d   w8                          ;restore w8, w9 from stack

    return                              ;return from _rsa_verify()


.end
