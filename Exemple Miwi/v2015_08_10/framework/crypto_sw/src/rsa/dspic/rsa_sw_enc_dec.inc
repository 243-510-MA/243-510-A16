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
; FILE          rsa_enc_dec.inc
; 
; DESCRIPTION   This file contains the ASM function declarations and
;               definitions for RSA  encryption and decryption routines in
;               the dsPIC Cryptographic Library.
;
; AUTHOR        M. H. Etzel, NTRU Cryptosystems, Inc.
;
; DATE          12/16/2003
;
; NOTES
;
; CHANGES
;				- 08/25/2009 mhe
;					- added _rsa_encrypt_raw(), _rsa_decrypt_raw()
;


; function declarations

;****************************************************************************
.extern _rsa_encrypt                    ;w0 - return code
                                        ;   MCL_SUCCESS
                                        ;   MCL_ILLEGAL_PARAMETER
                                        ;   MCL_ILLEGAL_SIZE
    ;w0 - unsigned char *c              ;address for the ciphertext, k bytes
    ;w1 - unsigned char *m              ;a pointer to the plaintext message
    ;w2 - unsigned short int *m_byte_len
                                        ;0 <= no. of m bytes <= k-11
    ;w3 - unsigned char *n              ;a pointer to the RSA public modulus,
                                        ; most significant bytes first
    ;w4 - unsigned short int *n_byte_len
                                        ;k, the number of bytes in n, must be
                                        ; 128 or 256
    ;w5 - unsigned char *e              ;a pointer to the RSA public exponent,
                                        ; most significant byte first
    ;w6 - unsigned short int *e_byte_len
                                        ;0 < number of bytes in e <= k
    ;w7 - unsigned char *xbuf           ;a pointer to a working buffer
                                        ; (xdata, length 2*k aligned(64) bytes)
    ;[w15-2] - unsigned char *ybuf      ;a pointer to a working buffer
                                        ; (ydata, length 3*k aligned(2) bytes)

; NOTE: the c buffer may not overlap the high half of xbuf addresses

; NOTE: THE WORKING BUFFER, xbuf, MUST BE ALIGNED AND LOCATED AS FOLLOWS:
;       .xbss
;       .align 64
;       xbuf: .space 2*k
 
; NOTE: THE WORKING BUFFER, ybuf, MUST BE ALIGNED AND LOCATED AS FOLLOWS:
;       .ybss
;       .align 2
;       ybuf: .space 3*k
 

;****************************************************************************
.extern _rsa_decrypt                    ;w0 - return code
                                        ;   MCL_SUCCESS
                                        ;   MCL_ILLEGAL_PARAMETER
                                        ;   MCL_ILLEGAL_SIZE
                                        ;   MCL_INVALID_CIPHERTEXT
    ;w0 - unsigned char *m              ;address for the plaintext message
    ;w1 - unsigned short int *m_byte_len
                                        ;address for the length of the
                                        ; plaintext message,
                                        ; 0 <= no. of m bytes <= k-11
    ;w2 - unsigned char *c              ;a pointer to the ciphertext buffer
    ;w3 - unsigned short int *c_byte_len
                                        ;k, the number of bytes in c, must be
                                        ; 128 or 256
    ;w4 - unsigned char *privkey        ;a pointer to the RSA private key,
                                        ; length 5*k/2 aligned(2) bytes
    ;w5 - unsigned char *xbuf           ;a pointer to a working buffer
                                        ; (xdata, length 3*k/2 aligned(2) bytes)
    ;w6 - unsigned char *ybuf           ;a pointer to a working buffer
                                        ; (ydata, length 3*k/2 aligned(2) bytes)

; NOTE: the m buffer may not overlap the low k-11 bytes of xbuf addresses

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
.extern _rsa_encrypt_raw                ;w0 - return code
                                        ;   MCL_SUCCESS
                                        ;   MCL_ILLEGAL_PARAMETER
                                        ;   MCL_ILLEGAL_SIZE
    ;w0 - unsigned char *c              ;address for the ciphertext, k bytes
    ;w1 - unsigned char *m              ;a pointer to the plaintext message,
										; m < k
    ;w2 - unsigned short int *m_byte_len
                                        ;0 <= no. of m bytes <= k
    ;w3 - unsigned char *n              ;a pointer to the RSA public modulus,
                                        ; most significant bytes first
    ;w4 - unsigned short int *n_byte_len
                                        ;k, the number of bytes in n, must be
                                        ; 128 or 144
    ;w5 - unsigned char *e              ;a pointer to the RSA public exponent,
                                        ; most significant byte first
    ;w6 - unsigned short int *e_byte_len
                                        ;0 < number of bytes in e <= k
    ;w7 - unsigned char *xbuf           ;a pointer to a working buffer
                                        ; (xdata, length 2*k aligned(64) bytes)
    ;[w15-2] - unsigned char *ybuf      ;a pointer to a working buffer
                                        ; (ydata, length 3*k aligned(2) bytes)

; NOTE: the c buffer may not overlap the high half of xbuf addresses

; NOTE: THE WORKING BUFFER, xbuf, MUST BE ALIGNED AND LOCATED AS FOLLOWS:
;       .xbss
;       .align 64
;       xbuf: .space 2*k
 
; NOTE: THE WORKING BUFFER, ybuf, MUST BE ALIGNED AND LOCATED AS FOLLOWS:
;       .ybss
;       .align 2
;       ybuf: .space 3*k
 

;****************************************************************************
.extern _rsa_decrypt_raw                ;w0 - return code
                                        ;   MCL_SUCCESS
                                        ;   MCL_ILLEGAL_PARAMETER
                                        ;   MCL_ILLEGAL_SIZE
    ;w0 - unsigned char *m              ;address for the plaintext message
    ;w1 - unsigned short int *m_byte_len
                                        ;address for the length of the
                                        ; plaintext message,
                                        ; 0 <= no. of m bytes <= k
    ;w2 - unsigned char *c              ;a pointer to the ciphertext buffer
    ;w3 - unsigned short int *c_byte_len
                                        ;k, the number of bytes in c, must be
                                        ; 128 or 144
    ;w4 - unsigned char *privkey        ;a pointer to the RSA private key,
                                        ; length 5*k/2 aligned(2) bytes
    ;w5 - unsigned char *xbuf           ;a pointer to a working buffer
                                        ; (xdata, length 3*k/2 aligned(2) bytes)
    ;w6 - unsigned char *ybuf           ;a pointer to a working buffer
                                        ; (ydata, length 3*k/2 aligned(2) bytes)

; NOTE: the m buffer may not overlap the low k bytes of xbuf addresses

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
 

