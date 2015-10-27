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
; FILE          rsa_sign_ver.inc
; 
; DESCRIPTION   This file contains the ASM function declarations and
;               definitions for RSA signing and verification routines in
;               the dsPIC Cryptographic Library.
;
; AUTHOR        M. H. Etzel, NTRU Cryptosystems, Inc.
;
; DATE          12/16/2003
;
; NOTES
;
; CHANGES
;


; function declarations

;****************************************************************************
.extern _rsa_sign                       ;w0 - return code
                                        ;   MCL_SUCCESS
                                        ;   MCL_ILLEGAL_PARAMETER
                                        ;   MCL_ILLEGAL_SIZE
    ;w0 - unsigned char *s              ;address for the signature
    ;w1 - unsigned short int *s_byte_len
                                        ;number of signature bytes, must be
                                        ; k=128 or k=256
    ;w2 - unsigned char *h              ;a pointer to the message hash,
                                        ; most significant byte first
    ;w3 - unsigned short int *h_byte_len
                                        ;0 < no. of hash bytes
    ;w4 - unsigned char *asn            ;a pointer to the hash identification
                                        ; data
    ;w5 - unsigned short int *asn_byte_len
                                        ;no. of bytes of hash ID data
    ;w6 - unsigned char *privkey        ;a pointer to the RSA private key,
                                        ; length 5*k/2 aligned(2) bytes
    ;w7 - unsigned char *xbuf           ;a pointer to a working buffer
                                        ; (xdata, length 3*k/2 aligned(2) bytes)
    ;[w15-2] - unsigned char *ybuf      ;a pointer to a working buffer
                                        ; (ydata, length 3*k/2 aligned(2) bytes)

; NOTE: the s buffer may not overlap the low k bytes of xbuf addresses

; NOTE: THE PRIVATE KEY BUFFER, privkey, MUST BE ALIGNED AS FOLLOWS:
;       .xbss
;       .align 2
;       xbuf: .space 5*k/2

; NOTE: THE WORKING BUFFER, xbuf, MUST BE ALIGNED AND LOCATED AS FOLLOWS:
;       .xbss
;       .align 2
;       xbuf: .space 3*k/2
 
; NOTE: THE WORKING BUFFER, ybuf, MUST BE ALIGNED AND LOCATED AS FOLLOWS:
;       .ybss
;       .align 2
;       ybuf: .space 3*k/2
 

;****************************************************************************
.extern _rsa_verify                     ;w0 - return code
                                        ;   MCL_SUCCESS
                                        ;   MCL_ILLEGAL_PARAMETER
                                        ;   MCL_ILLEGAL_SIZE
                                        ;   MCL_INVALID_SIGNATURE
    ;w0 - unsigned char *s              ;pointer to the RSA signature
    ;w1 - unsigned char *h              ;a pointer to the message hash,
                                        ; most significant byte first
    ;w2 - unsigned short int *h_byte_len
                                        ;0 < no. of hash bytes
    ;w4 - unsigned char *asn            ;a pointer to the hash identification
                                        ; data
    ;w4 - unsigned short int asn_byte_len
                                        ;no. of bytes of hash ID data
    ;w5 - unsigned char *n              ;a pointer to the RSA public modulus,
                                        ; most significant byte first
    ;w6 - unsigned short int n_byte_len ;the number of bytes in n, must be
                                        ; k=128 or k=256
    ;w7 - unsigned char *e              ;a pointr to the RSA public exponent,
                                        ; most significant byte first
    ;[w15-2] - unsigned short int e_byte_len
                                        ;0 < number of byts in e <= k
    ;[w15-4] - unsigned char *xbuf      ;a pointer to a working buffer
                                        ; (xdata, length 2*k aligned(2) bytes)
    ;[w15-6] - unsigned char *ybuf      ;a pointer to a working buffer
                                        ; (ydata, length 3*k aligned(2) bytes)

; NOTE: THE WORKING BUFFER, xbuf, MUST BE ALIGNED AND LOCATED AS FOLLOWS:
;       .xbss
;       .align 2
;       xbuf: .space 2*k
 
; NOTE: THE WORKING BUFFER, ybuf, MUST BE ALIGNED AND LOCATED AS FOLLOWS:
;       .ybss
;       .align 2
;       ybuf: .space 3*k
 

