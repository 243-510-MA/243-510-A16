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

/****************************************************************************/
/* FILE         rsa_sign_ver.h
 *
 * DESCRIPTION  This file contains the C function declarations for
 *              RSA signing and verification routines in the dsPIC
 *              Cryptographic Library.
 *
 * AUTHOR       N. A. Howgrave-Graham, NTRU Cryptosystems, Inc.
 *
 * DATE         11/20/2003
 *
 * NOTES
 *
 * CHANGES
 *
 */

#if !defined (_RSA_SW_SIGN_VER_H)
#define _RSA_SW_SIGN_VER_H

/*
 *   the ASN.1 encoding of various hash functions, which are used in
 *   RSA signing and verification.
 */

/* ASN.1 for MD2 */
#define MCL_MD2_ASN_ID { 0x30, 0x20, 0x30, 0x0c, 0x06, 0x08, 0x2a, 0x86, \
                         0x48, 0x86, 0xf7, 0x0d, 0x02, 0x02, 0x05, 0x00, \
                         0x04, 0x10 }

#define MCL_MD2_ASN_ID_BYTE_LEN 18


/* ASN.1 for MD5 */
#define MCL_MD5_ASN_ID { 0x30, 0x20, 0x30, 0x0c, 0x06, 0x08, 0x2a, 0x86, \
                         0x48, 0x86, 0xf7, 0x0d, 0x02, 0x05, 0x05, 0x00, \
                         0x04, 0x10 }

#define MCL_MD5_ASN_ID_BYTE_LEN 18


/* ASN.1 for SHA-1 */
#define MCL_SHA1_ASN_ID { 0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2b, 0x0e, \
                          0x03, 0x02, 0x1a, 0x05, 0x00, 0x04, 0x14 }

#define MCL_SHA1_ASN_ID_BYTE_LEN 15


/* ASN.1 for SHA-256 */
#define MCL_SHA256_ASN_ID { 0x30, 0x31, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, \
                            0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01, 0x05, \
                            0x00, 0x04, 0x20 }

#define MCL_SHA256_ASN_ID_BYTE_LEN 19


/* ASN.1 for SHA-384 */
#define MCL_SHA384_ASN_ID { 0x30, 0x41, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, \
                            0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x02, 0x05, \
                            0x00, 0x04, 0x30 }

#define MCL_SHA384_ASN_ID_BYTE_LEN 19


/* ASN.1 for SHA-512 */
#define MCL_SHA512_ASN_ID { 0x30, 0x51, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, \
                            0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x03, 0x05, \
                            0x00, 0x04, 0x40 }

#define MCL_SHA512_ASN_ID_BYTE_LEN 19


/* function declarations */

/****************************************************************************/
extern unsigned short int               /* return code
                                            MCL_SUCCESS
                                            MCL_ILLEGAL_PARAMETER
                                            MCL_ILLEGAL_SIZE */
rsa_sign(
    unsigned char *s,                   /* address for the signature */
    unsigned short int s_byte_len,      /* number of signature bytes, k,
                                           must be 128 or 256 */
    unsigned char *h,                   /* a pointer to the message hash,
                                           most significant byte first */
    unsigned short int h_byte_len,      /* 0 < no. of hash bytes */
    unsigned char *asn,                 /* a pointer to the hash identification
                                           data */
    unsigned short int asn_byte_len,    /* no. of bytes of hash ID data */
    unsigned char *privkey,             /* a pointer to the private key,
                                           length 5*k/2 aligned(2) bytes */
    unsigned char *xbuf,                /* a pointer to a working buf, (xdata,
                                           length 3*(k/2) aligned(2) bytes */
    unsigned char *ybuf                 /* a pointer to a working buf, (ydata,
                                           length 3*(k/2) aligned(2) bytes */
);

/* NOTE: the s buffer may not overlap the low k bytes of xbuf addresses */ 
  
/* NOTE: THE PRIVATE KEY BUFFER, privkey, MUST BE ALIGNED AS FOLLOWS:
 *       unsigned char __attribute__ ((aligned (2))) sk[5*k/2] */

/* NOTE: THE WORKING BUFFER, xbuf, MUST BE ALIGNED AND LOCATED AS FOLLOWS:
 unsigned char __attribute__ ((aligned (2), space(xmemory))) xbuf[3*k/2];
 */

/* NOTE: THE WORKING BUFFER, ybuf, MUST BE ALIGNED AND LOCATED AS FOLLOWS:
 unsigned char __attribute__ ((aligned (2), space(ymemory), eds)) ybuf[3*k/2];
 */


/****************************************************************************/
extern unsigned short int               /* return code
                                            MCL_SUCCESS
                                            MCL_ILLEGAL_PARAMETER
                                            MCL_ILLEGAL_SIZE
                                            MCL_INVALID_SIGNATURE */
rsa_verify(
    unsigned char *s,                   /* a ponter to the RSA signature */
    unsigned char *h,                   /* a pointer to the message hash,
                                           most significant byte first */
    unsigned short int h_byte_len,      /* 0 < no. of hash bytes */
    unsigned char *asn,                 /* a pointer to the hash identification
                                           data */
    unsigned short int asn_byte_len,    /* no. of bytes of hash ID data */
    unsigned char *n,                   /* a pointer to the RSA public modulus,
                                           most significant byte first */
    unsigned short int n_byte_len,      /* k, the number of bytes in n,
                                           must be 128 or 256 */
    unsigned char *e,                   /* a pointer to the RSA public exponent,
                                           most significant byte first */
    unsigned short int e_byte_len,      /* 0 < number of bytes in e <= k */
    unsigned char *xbuf,                /* a pointer to a working buf, (xdata,
                                           length 2*k aligned(2) bytes */
    unsigned char *ybuf                 /* a pointer to a working buf, (ydata,
                                           length 3*k aligned(2) bytes */
);

/* NOTE: THE WORKING BUFFER, xbuf, MUST BE ALIGNED AND LOCATED AS FOLLOWS:
 unsigned char __attribute__ ((aligned (2), space(xmemory))) xbuf[2*k];
 */

/* NOTE: THE WORKING BUFFER, ybuf, MUST BE ALIGNED AND LOCATED AS FOLLOWS:
 unsigned char __attribute__ ((aligned (2), space(ymemory), eds)) ybuf[3*k];
 */


#endif      // _RSA_SW_SIGN_VER_H
