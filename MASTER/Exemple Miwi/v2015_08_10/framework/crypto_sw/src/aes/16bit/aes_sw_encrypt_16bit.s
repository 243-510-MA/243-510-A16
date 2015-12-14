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

.equ VALID_ID,0
    .ifdecl __dsPIC33F
        .include "p33Fxxxx.inc"
    .endif

    .ifdecl __dsPIC33E
        .include "p33Exxxx.inc"
    .endif

    .ifdecl __dsPIC30F
        .include "p30Fxxxx.inc"
    .endif

    .ifdecl __PIC24H
        .include "p24Hxxxx.inc"
    .endif

    .ifdecl __PIC24E
        .include "p24Exxxx.inc"
    .endif

    .ifdecl __PIC24F
        .include "p24Fxxxx.inc"
    .endif
.if VALID_ID <> 1
    .error "Processor ID not specified in generic include files.  New ASM30 assembler needs to be downloaded?"
.endif

; Conditional directives to resolve differences between PIC with PSV vs EDS
.ifdecl DSRPAG
    .equ PSVPAG, DSRPAG
    .macro PSVEnable
    .endm
.else
    .macro PSVEnable
    bset.b    CORCONL, #PSV
    .endm
.endif

.section *,psv, page

.global STable
STable:        ;S-Box substitution table.  Used by AESEncrypt() and AESDecrypt.
.byte    0x63,0x7C,0x77,0x7B,0xF2,0x6B,0x6F,0xC5,0x30,0x01,0x67,0x2B,0xFE,0xD7,0xAB,0x76
.byte    0xCA,0x82,0xC9,0x7D,0xFA,0x59,0x47,0xF0,0xAD,0xD4,0xA2,0xAF,0x9C,0xA4,0x72,0xC0
.byte    0xB7,0xFD,0x93,0x26,0x36,0x3F,0xF7,0xCC,0x34,0xA5,0xE5,0xF1,0x71,0xD8,0x31,0x15
.byte    0x04,0xC7,0x23,0xC3,0x18,0x96,0x05,0x9A,0x07,0x12,0x80,0xE2,0xEB,0x27,0xB2,0x75
.byte    0x09,0x83,0x2C,0x1A,0x1B,0x6E,0x5A,0xA0,0x52,0x3B,0xD6,0xB3,0x29,0xE3,0x2F,0x84
.byte    0x53,0xD1,0x00,0xED,0x20,0xFC,0xB1,0x5B,0x6A,0xCB,0xBE,0x39,0x4A,0x4C,0x58,0xCF
.byte    0xD0,0xEF,0xAA,0xFB,0x43,0x4D,0x33,0x85,0x45,0xF9,0x02,0x7F,0x50,0x3C,0x9F,0xA8
.byte    0x51,0xA3,0x40,0x8F,0x92,0x9D,0x38,0xF5,0xBC,0xB6,0xDA,0x21,0x10,0xFF,0xF3,0xD2
.byte    0xCD,0x0C,0x13,0xEC,0x5F,0x97,0x44,0x17,0xC4,0xA7,0x7E,0x3D,0x64,0x5D,0x19,0x73
.byte    0x60,0x81,0x4F,0xDC,0x22,0x2A,0x90,0x88,0x46,0xEE,0xB8,0x14,0xDE,0x5E,0x0B,0xDB
.byte    0xE0,0x32,0x3A,0x0A,0x49,0x06,0x24,0x5C,0xC2,0xD3,0xAC,0x62,0x91,0x95,0xE4,0x79
.byte    0xE7,0xC8,0x37,0x6D,0x8D,0xD5,0x4E,0xA9,0x6C,0x56,0xF4,0xEA,0x65,0x7A,0xAE,0x08
.byte    0xBA,0x78,0x25,0x2E,0x1C,0xA6,0xB4,0xC6,0xE8,0xDD,0x74,0x1F,0x4B,0xBD,0x8B,0x8A
.byte    0x70,0x3E,0xB5,0x66,0x48,0x03,0xF6,0x0E,0x61,0x35,0x57,0xB9,0x86,0xC1,0x1D,0x9E
.byte    0xE1,0xF8,0x98,0x11,0x69,0xD9,0x8E,0x94,0x9B,0x1E,0x87,0xE9,0xCE,0x55,0x28,0xDF
.byte    0x8C,0xA1,0x89,0x0D,0xBF,0xE6,0x42,0x68,0x41,0x99,0x2D,0x0F,0xB0,0x54,0xBB,0x16
.text

;/*******************************************************************************
;  Function:
;    TODO
;
;  Summary:
;    TODO
;
;  Description:
;    TODO
;
;  Precondition:
;    None.
;
;  Parameters:
;    TODO
;
;  Returns:
;    None
;
;  Example:
;    <code>
;    TODO
;    </code>
;
;  *****************************************************************************/

;WREG usage in function
;  WREG0 - plain_text pointer
;  WREG1 - session key pointer
;  WREG2 - cipher_text pointer

;  WREG0 - cipher_text pointer
;  WREG1 - plain_test pointer
;  WREG2 - session_key pointer
;  WREG3 - round counter
;  WREG4 - temp var
;  WREG5 - temp var
;  WREG6 - temp var,S-table pointer
;  WREG7 - temp var
;  WREG8 - temp var
;  WREG9 - temp var
;  WREG10- temp var
;  WREG11- temp var

    .global _AES_SW_Encrypt
_AES_SW_Encrypt:
    mov W1, W0
    mov W2, W1
    mov W3, W2
    mov [W2++],W4
    add #2,W2

    ;TODO - magic number
    ;test bit 5, 0b00100000 = 32, if this bit is set, then it is a 256-bit request
    btsc W4, #5     
    bra _AES256EncryptBlock

    ;TODO - magic number
    ;test bit 3, 0b00011000 = 24, if this bit is set, then it is a 192-bit request
    btsc W4, #3     
    bra _AES192EncryptBlock

    ;otherwise it was a 128 bit request
    bra _AES128EncryptBlock

    .global    _AES128EncryptBlock
_AES128EncryptBlock:
    ;roundCounter = 10
    mov        #10, W3    ; roundCounter
    goto    _AES_SW_Encrypt_Main

    .global    _AES192EncryptBlock
_AES192EncryptBlock:
    ;roundCounter = 12
    mov        #12, W3    ; roundCounter
    goto    _AES_SW_Encrypt_Main

    .global    _AES256EncryptBlock
_AES256EncryptBlock:
    ;roundCounter = 14
    mov        #14, W3    ; roundCounter
    goto    _AES_SW_Encrypt_Main

_AES_SW_Encrypt_Main:
; Overview: Sixteen data bytes @ W0 are encrypted using 16 byte key @ W1.  
;            The data bytes are updated in place while the key is unmodified.

.ifndecl __HAS_EDS
    push CORCON
.endif
    push    PSVPAG
    push.d    W8
    push.d    W10

    ;Enable Program Space Visibility of S-table
    PSVEnable
    mov        #edspage(STable), W4
    mov        W4, PSVPAG

    ;Key addition
    ;Key addition already takes place at EncodeKeyAddition.  However, there is a 6 
    ;instruction cycle performance penalty total if you jump to EncodeKeyAddition instead 
    ;of wasting 33 bytes of program memory repeating it.
;    bra        EncodeKeyAddition
    mov.d    [W1++], W4
    xor        W4, [W2++], [W0++]
    xor        W5, [W2++], [W0++]
    mov.d    [W1++], W4
    xor        W4, [W2++], [W0++]
    xor        W5, [W2++], [W0++]
    mov.d    [W1++], W4
    xor        W4, [W2++], [W0++]
    xor        W5, [W2++], [W0++]
    mov.d    [W1++], W4
    xor        W4, [W2++], [W0++]
    xor        W5, [W2++], [W0++]

    ; Reset block and key pointers to beginning
    sub     #16, W0
    sub     #16, W2

    ;
    ;Enter roundCounter Loop
    ;
    mov        #edsoffset(STable), W6

EncodeRoundLoop:
    ;Do S-table subsitution
    clr        W4
    
    mov.b    [W0], W4            ;Byte 0
    mov.b    [W4+W6], [W0++]
    mov.b    [W0], W4            ;Byte 1
    mov.b    [W4+W6], [W0++]
    mov.b    [W0], W4            ;Byte 2
    mov.b    [W4+W6], [W0++]
    mov.b    [W0], W4            ;Byte 3
    mov.b    [W4+W6], [W0++]
    mov.b    [W0], W4            ;Byte 4
    mov.b    [W4+W6], [W0++]
    mov.b    [W0], W4            ;Byte 5
    mov.b    [W4+W6], [W0++]
    mov.b    [W0], W4            ;Byte 6
    mov.b    [W4+W6], [W0++]
    mov.b    [W0], W4            ;Byte 7
    mov.b    [W4+W6], [W0++]
    mov.b    [W0], W4            ;Byte 8
    mov.b    [W4+W6], [W0++]
    mov.b    [W0], W4            ;Byte 9
    mov.b    [W4+W6], [W0++]
    mov.b    [W0], W4            ;Byte 10
    mov.b    [W4+W6], [W0++]
    mov.b    [W0], W4            ;Byte 11
    mov.b    [W4+W6], [W0++]
    mov.b    [W0], W4            ;Byte 12
    mov.b    [W4+W6], [W0++]
    mov.b    [W0], W4            ;Byte 13
    mov.b    [W4+W6], [W0++]
    mov.b    [W0], W4            ;Byte 14
    mov.b    [W4+W6], [W0++]
    mov.b    [W0], W4            ;Byte 15
    mov.b    [W4+W6], [W0++]
        

    ;Encode row shift
    ;Given this byte order:     0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15
    ;This byte order is needed: 0  5  10 15 4  9  14 3  8  13 2  7  12 1  6  11

    mov.b    [W0+1-16], W4   ;save the data in 1
    mov.b    [W0+5-16], W5   ;save the data in 5
    mov.b    [W0+9-16], W8   ;save the data in 9
    mov.b    [W0+13-16], W9  ;save the data in 13
    mov.b    W4, [W0+13-16]  ;move old 1 to new 13
    mov.b    W5, [W0+1-16]   ;move old 5 to new 1
    mov.b    W8, [W0+5-16]   ;move old 9 to new 5
    mov.b    W9, [W0+9-16]   ;move old 13 to new 9

    mov.b    [W0+2-16], W4   ;save the data in 2
    mov.b    [W0+10-16], W5  ;save the data in 10
    mov.b    W4, [W0+10-16]  ;move old 2 to new 10
    mov.b    W5, [W0+2-16]   ;move old 10 to new 2

    mov.b    [W0+3-16], W4   ;save the data in 3
    mov.b    [W0+7-16], W5   ;save the data in 7
    mov.b    [W0+11-16], W8  ;save the data in 11
    mov.b    [W0+15-16], W9  ;save the data in 15
    mov.b    W4, [W0+7-16]   ;move old 3 to new 7
    mov.b    W5, [W0+11-16]  ;move old 7 to new 11
    mov.b    W8, [W0+15-16]  ;move old 11 to new 15
    mov.b    W9, [W0+3-16]   ;move old 15 to new 3

    mov.b    [W0+6-16], W4   ;save the data in 6
    mov.b    [W0+14-16], W5  ;save the data in 14
    mov.b    W4, [W0+14-16]  ;move old 6 to new 14
    mov.b    W5, [W0+6-16]   ;move old 14 to new 6

    ;Advance the session key pointer to the end of the next round key
    add     #32, W2

    ;Check to see if we are on the last round.  The last round does not need
    ;to have the columns mixed
    ;Note: We decrement roundCounter here and test for Zero instead of 1. 
    dec        w3, w3
    bra        Z, EncodeKeyAddition

    ;Restore the block pointer to the beginning
    sub        #16, W0

    ;Prepare for future Xtime() operations
    mov        #0x001B, W7        

    ;Mix columns
    ;Address of block is in W2, let's use it
    ;for(i=0;i<16;i+=4)
    ;i=0
    mov.b    [W0++], W4            ;aux1 = block[i+0]^block[i+1];
    xor.b    W4, [W0], W4

    mov.b    [W0++], W5            ;aux2 = block[i+1]^block[i+2];
    xor.b    W5, [W0], W5

    mov.b    [W0++], W8            ;aux3 = block[i+2]^block[i+3];
    xor.b    W8, [W0--], W8

    xor.b    W4, W8, W9            ;aux  = aux1^aux3;

    sl        W4, W4                ;aux1 = xtime(aux1);
    btsc    W4, #8            
    xor.b    W4, W7, W4

    sl        W5, W5                ;aux2 = xtime(aux2);
    btsc    W5, #8            
    xor.b    W5, W7, W5

    sl        W8, W8                ;aux3 = xtime(aux3);
    btsc    W8, #8            
    xor.b    W8, W7, W8
    
    xor.b   W9, W8, W10         ;block[i+2]= aux^aux3^block[i+2];
    xor.b   W10, [W0], [W0--]

    xor.b   W9, W5, W10         ;block[i+1]= aux^aux2^block[i+1];
    xor.b   W10, [W0], [W0--]

    xor.b   W9, W4, W10         ;block[i+0]= aux^aux1^block[i+0];
    xor.b   W10, [W0], [W0]

    xor.b    W9, [W0++], W4        ;block[i+3]= block[i+0]^block[i+1]^block[i+2]^aux;
    xor.b    W4, [W0++], W4
    xor.b    W4, [W0++], [W0++]

    ;i=4
    mov.b    [W0++], W4            ;aux1 = block[i+0]^block[i+1];
    xor.b    W4, [W0], W4

    mov.b    [W0++], W5            ;aux2 = block[i+1]^block[i+2];
    xor.b    W5, [W0], W5

    mov.b    [W0++], W8            ;aux3 = block[i+2]^block[i+3];
    xor.b    W8, [W0--], W8

    xor.b    W4, W8, W9            ;aux  = aux1^aux3;

    sl        W4, W4                ;aux1 = xtime(aux1);
    btsc    W4, #8            
    xor.b    W4, W7, W4

    sl        W5, W5                ;aux2 = xtime(aux2);
    btsc    W5, #8            
    xor.b    W5, W7, W5

    sl        W8, W8                ;aux3 = xtime(aux3);
    btsc    W8, #8            
    xor.b    W8, W7, W8
    
    xor.b   W9, W8, W10         ;block[i+2]= aux^aux3^block[i+2];
    xor.b   W10, [W0], [W0--]

    xor.b   W9, W5, W10         ;block[i+1]= aux^aux2^block[i+1];
    xor.b   W10, [W0], [W0--]

    xor.b   W9, W4, W10         ;block[i+0]= aux^aux1^block[i+0];
    xor.b   W10, [W0], [W0]

    xor.b    W9, [W0++], W4        ;block[i+3]= block[i+0]^block[i+1]^block[i+2]^aux;
    xor.b    W4, [W0++], W4
    xor.b    W4, [W0++], [W0++]

    ;i=8
    mov.b    [W0++], W4            ;aux1 = block[i+0]^block[i+1];
    xor.b    W4, [W0], W4

    mov.b    [W0++], W5            ;aux2 = block[i+1]^block[i+2];
    xor.b    W5, [W0], W5

    mov.b    [W0++], W8            ;aux3 = block[i+2]^block[i+3];
    xor.b    W8, [W0--], W8

    xor.b    W4, W8, W9            ;aux  = aux1^aux3;

    sl        W4, W4                ;aux1 = xtime(aux1);
    btsc    W4, #8            
    xor.b    W4, W7, W4

    sl        W5, W5                ;aux2 = xtime(aux2);
    btsc    W5, #8            
    xor.b    W5, W7, W5

    sl        W8, W8                ;aux3 = xtime(aux3);
    btsc    W8, #8            
    xor.b    W8, W7, W8
    
    xor.b   W9, W8, W10         ;block[i+2]= aux^aux3^block[i+2];
    xor.b   W10, [W0], [W0--]

    xor.b   W9, W5, W10         ;block[i+1]= aux^aux2^block[i+1];
    xor.b   W10, [W0], [W0--]

    xor.b   W9, W4, W10         ;block[i+0]= aux^aux1^block[i+0];
    xor.b   W10, [W0], [W0]

    xor.b    W9, [W0++], W4        ;block[i+3]= block[i+0]^block[i+1]^block[i+2]^aux;
    xor.b    W4, [W0++], W4
    xor.b    W4, [W0++], [W0++]

    ;i=12
    mov.b    [W0++], W4            ;aux1 = block[i+0]^block[i+1];
    xor.b    W4, [W0], W4

    mov.b    [W0++], W5            ;aux2 = block[i+1]^block[i+2];
    xor.b    W5, [W0], W5

    mov.b    [W0++], W8            ;aux3 = block[i+2]^block[i+3];
    xor.b    W8, [W0--], W8

    xor.b    W4, W8, W9            ;aux  = aux1^aux3;

    sl        W4, W4                ;aux1 = xtime(aux1);
    btsc    W4, #8            
    xor.b    W4, W7, W4

    sl        W5, W5                ;aux2 = xtime(aux2);
    btsc    W5, #8            
    xor.b    W5, W7, W5

    sl        W8, W8                ;aux3 = xtime(aux3);
    btsc    W8, #8            
    xor.b    W8, W7, W8
    
    xor.b   W9, W8, W10         ;block[i+2]= aux^aux3^block[i+2];
    xor.b   W10, [W0], [W0--]

    xor.b   W9, W5, W10         ;block[i+1]= aux^aux2^block[i+1];
    xor.b   W10, [W0], [W0--]

    xor.b   W9, W4, W10         ;block[i+0]= aux^aux1^block[i+0];
    xor.b   W10, [W0], [W0]

    xor.b    W9, [W0++], W4        ;block[i+3]= block[i+0]^block[i+1]^block[i+2]^aux;
    xor.b    W4, [W0++], W4
    xor.b    W4, [W0++], [W0++]

    ;i is 16

    ;Add key to block
EncodeKeyAddition:
    mov.d    [--W2], W4
    xor        W5, [--W0], [W0]
    xor        W4, [--W0], [W0]
    mov.d    [--W2], W4
    xor        W5, [--W0], [W0]
    xor        W4, [--W0], [W0]
    mov.d    [--W2], W4
    xor        W5, [--W0], [W0]
    xor        W4, [--W0], [W0]
    mov.d    [--W2], W4
    xor        W5, [--W0], [W0]
    xor        W4, [--W0], [W0]


    ;Process roundCounter loop
    ;Note: roundCounter has already been decremented
    add        #0, W3
    bra        NZ, EncodeRoundLoop

    ;Pop the roundCounter, rcon, and decryption key off the stack
    pop.d    W10
    pop.d    W8
    pop        PSVPAG
.ifndecl __HAS_EDS
    pop CORCON
.endif
    return

