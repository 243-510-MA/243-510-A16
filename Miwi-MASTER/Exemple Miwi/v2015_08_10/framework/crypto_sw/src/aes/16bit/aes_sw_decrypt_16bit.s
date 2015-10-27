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

.global SiTable
SiTable:    ;Inverse S-Box substitution table. Used by AESDecrypt() only.
.byte    0x52,0x09,0x6A,0xD5,0x30,0x36,0xA5,0x38,0xBF,0x40,0xA3,0x9E,0x81,0xF3,0xD7,0xFB
.byte    0x7C,0xE3,0x39,0x82,0x9B,0x2F,0xFF,0x87,0x34,0x8E,0x43,0x44,0xC4,0xDE,0xE9,0xCB
.byte    0x54,0x7B,0x94,0x32,0xA6,0xC2,0x23,0x3D,0xEE,0x4C,0x95,0x0B,0x42,0xFA,0xC3,0x4E
.byte    0x08,0x2E,0xA1,0x66,0x28,0xD9,0x24,0xB2,0x76,0x5B,0xA2,0x49,0x6D,0x8B,0xD1,0x25
.byte    0x72,0xF8,0xF6,0x64,0x86,0x68,0x98,0x16,0xD4,0xA4,0x5C,0xCC,0x5D,0x65,0xB6,0x92
.byte    0x6C,0x70,0x48,0x50,0xFD,0xED,0xB9,0xDA,0x5E,0x15,0x46,0x57,0xA7,0x8D,0x9D,0x84
.byte    0x90,0xD8,0xAB,0x00,0x8C,0xBC,0xD3,0x0A,0xF7,0xE4,0x58,0x05,0xB8,0xB3,0x45,0x06
.byte    0xD0,0x2C,0x1E,0x8F,0xCA,0x3F,0x0F,0x02,0xC1,0xAF,0xBD,0x03,0x01,0x13,0x8A,0x6B
.byte    0x3A,0x91,0x11,0x41,0x4F,0x67,0xDC,0xEA,0x97,0xF2,0xCF,0xCE,0xF0,0xB4,0xE6,0x73
.byte    0x96,0xAC,0x74,0x22,0xE7,0xAD,0x35,0x85,0xE2,0xF9,0x37,0xE8,0x1C,0x75,0xDF,0x6E
.byte    0x47,0xF1,0x1A,0x71,0x1D,0x29,0xC5,0x89,0x6F,0xB7,0x62,0x0E,0xAA,0x18,0xBE,0x1B
.byte    0xFC,0x56,0x3E,0x4B,0xC6,0xD2,0x79,0x20,0x9A,0xDB,0xC0,0xFE,0x78,0xCD,0x5A,0xF4
.byte    0x1F,0xDD,0xA8,0x33,0x88,0x07,0xC7,0x31,0xB1,0x12,0x10,0x59,0x27,0x80,0xEC,0x5F
.byte    0x60,0x51,0x7F,0xA9,0x19,0xB5,0x4A,0x0D,0x2D,0xE5,0x7A,0x9F,0x93,0xC9,0x9C,0xEF
.byte    0xA0,0xE0,0x3B,0x4D,0xAE,0x2A,0xF5,0xB0,0xC8,0xEB,0xBB,0x3C,0x83,0x53,0x99,0x61
.byte    0x17,0x2B,0x04,0x7E,0xBA,0x77,0xD6,0x26,0xE1,0x69,0x14,0x63,0x55,0x21,0x0C,0x7D
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

;void AES192DecryptBlock(
;    UINT8 * cipher_text, 
;    void * session, 
;    UINT8 * plain_text
;)

;WREG usage in function
;  WREG0 - cipher_text pointer
;  WREG1 - session key pointer
;  WREG2 - plain_text pointer
;  WREG3 - round counter
;  WREG4 - temp var
;  WREG5 - temp var
;  WREG6 - temp var,Si-table pointer,X_time()
;  WREG7 - temp var
;  WREG8 - temp var
;  WREG9 - temp var
;  WREG10- temp var
;  WREG11- temp var

    .global _AES_SW_Decrypt
_AES_SW_Decrypt:
    mov W1, W0
    mov W2, W1
    mov W3, W2
    mov [W2++],W4
    add #2,W2

    ;test bit 5, 0b00100000 = 32, if this bit is set, then it is a 256-bit request
    btsc W4, #5     
    bra _AES256DecryptBlock

    ;test bit 3, 0b00011000 = 24, if this bit is set, then it is a 192-bit request
    btsc W4, #3     
    bra _AES192DecryptBlock

    ;otherwise it was a 128 bit request
    bra _AES128DecryptBlock

    .global _AES128DecryptBlock
_AES128DecryptBlock:
    ;roundCounter = 10
    mov        #10, W3    ; roundCounter

    ;move the key pointer to the last round key
    add     #160,W2

    goto    _AES_SW_Decrypt_Main

    .global _AES192DecryptBlock
_AES192DecryptBlock:
    ;roundCounter = 12
    mov        #12, W3    ; roundCounter

    ;move the key pointer to the last round key
    add     #192, W2

    goto    _AES_SW_Decrypt_Main



    .global _AES256DecryptBlock
_AES256DecryptBlock:
    ;roundCounter = 14
    mov        #14, W3    ; roundCounter

    ;move the key pointer to the last round key
    add     #224,W2

    goto    _AES_SW_Decrypt_Main

_AES_SW_Decrypt_Main:
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
    ;Key addition already takes place at DecodeKeyAddition.  However, there is a 6 
    ;instruction cycle performance penalty total if you jump to DecodeKeyAddition instead 
    ;of wasting 33 bytes of program memory repeating it.
;    bra        DecodeKeyAddition
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
    sub        #16, W0
    sub        #16, W2

    ;The first loop iteration does not do inverse column mixing, so let's jump into the 
    ;middle of the loop
    bra        DecodeSSubstitute

    ;
    ;Enter roundCounter Loop
    ;
DecodeRoundLoop:
    ;
    ;Inverse mix column
    ;
    clr        W4
    mov        #0x011B, W6        ;Set up W6 for doing future Xtime() operations.  Constant 0x011B 
                            ;is used instead of 0x1B because it conveniently causes the high 
                            ;byte to be 0x00

    ;for(i=0;i<16;i+=4)
    ;i=0
    mov.b    [W0++], W4        ;W4=block[i+0x00]
    mov.b    W4, W5            ;temp0=block[i+0x00]
    mov.b    W4, W7            ;temp3=block[i+0x00]
    sl        W4, W4            ;W4=x1[block[i+0x00]]
    btsc    W4, #8            
    xor        W4, W6, W4
    mov        W4, W10            ;W10=x1[block[i+0x00]]
    xor.b    W5, W4, W5        ;temp0=block[i+0x00]^x1[block[i+0x00]]
    sl        W4, W8            ;temp2=x2[block[i+0x00]]
    btsc    W8, #8            
    xor        W8, W6, W8
    xor.b    W5, W8, W5        ;temp0=block[i+0x00]^x1[block[i+0x00]]^x2[block[i+0x00]]
    sl        W8, W4            ;W4=x3[block[i+0x00]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W7, W4, W7        ;temp3=block[i+0x00]^x3[block[i+0x00]]

    mov.b    [W0++], W4        ;W4=block[i+0x01]
    mov.b    W4, W9            ;temp1=block[i+0x01]
    xor.b    W7, W4, W7        ;temp3=block[i+0x00]^x3[block[i+0x00]]^block[i+0x01]
    sl        W4, W4            ;W4=x1[block[i+0x01]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W5, W4, W5        ;temp0=block[i+0x00]^x2[block[i+0x00]]^x1[block[i+0x00]]^x1[block[i+0x01]]
    xor.b    W9, W4, W9        ;temp1=block[i+0x01]^x1[block[i+0x01]]
    sl        W4, W4            ;W4=x2[block[i+0x01]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W10, W4, W10    ;W10=x1[block[i+0x00]]^x2[block[i+0x01]]
    xor.b    W9, W4, W9        ;temp1=block[i+0x01]^x1[block[i+0x01]]^x2[block[i+0x01]]
    sl        W4, W4            ;W4=x3[block[i+0x01]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W7, W4, W7        ;temp3=block[i+0x00]^x3[block[i+0x00]]^block[i+0x01]^x3[block[i+0x01]]

    mov.b    [W0++], W4        ;W4=block[i+0x02]
    xor.b    W7, W4, W7        ;temp3=block[i+0x00]^x3[block[i+0x00]]^block[i+0x01]^x3[block[i+0x01]]^block[i+0x02]
    xor.b    W8, W4, W8        ;temp2=x2[block[i+0x00]]^block[i+0x02]
    sl        W4, W4            ;W4=x1[block[i+0x02]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W9, W4, W9        ;temp1=block[i+0x01]^x1[block[i+0x01]]^x2[block[i+0x01]]^x1[block[i+0x02]]
    xor.b    W8, W4, W8        ;temp2=x2[block[i+0x00]]^block[i+0x02]^x1[block[i+0x02]]
    sl        W4, W4            ;W4=x2[block[i+0x02]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W5, W4, W5        ;temp0=block[i+0x00]^x2[block[i+0x00]]^x1[block[i+0x00]]^x1[block[i+0x01]]^x2[block[i+0x02]]
    xor.b    W8, W4, W8        ;temp2=x2[block[i+0x00]]^block[i+0x02]^x1[block[i+0x02]]^x2[block[i+0x02]]
    sl        W4, W4            ;W4=x3[block[i+0x02]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W7, W4, W7        ;temp3=block[i+0x00]^x3[block[i+0x00]]^block[i+0x01]^x3[block[i+0x01]]^block[i+0x02]^x3[block[i+0x02]]

    mov.b    [W0++], W4        ;W4=block[i+0x03]
    xor.b    W10, W4, W10        ;W10=x1[block[i+0x00]]^x2[block[i+0x01]]^block[i+0x03]
    xor.b    W7, W4, W7        ;temp3=block[i+0x00]^x3[block[i+0x00]]^block[i+0x01]^x3[block[i+0x01]]^block[i+0x02]^x3[block[i+0x02]]^block[i+0x03]
    sl        W4, W4            ;W4=x1[block[i+0x03]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W10, W4, W10        ;W10=x1[block[i+0x00]]^x2[block[i+0x01]]^block[i+0x03]^x1[block[i+0x03]]
    xor.b    W8, W4, W8        ;temp2=x2[block[i+0x00]]^block[i+0x02]^x1[block[i+0x02]]^x2[block[i+0x02]]^x1[block[i+0x03]]
    sl        W4, W4            ;W4=x2[block[i+0x03]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W10, W4, W10        ;W10=x1[block[i+0x00]]^x2[block[i+0x01]]^block[i+0x03]^x2[block[i+0x03]]
    xor.b    W9, W4, W9        ;temp1=block[i+0x01]^x1[block[i+0x01]]^x2[block[i+0x01]]^x1[block[i+0x02]]^x2[block[i+0x03]]
    sl        W4, W4            ;W4=x3[block[i+0x03]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W7, W4, W7        ;temp3=block[i+0x00]^x3[block[i+0x00]]^block[i+0x01]^x3[block[i+0x01]]^block[i+0x02]^x3[block[i+0x02]]^block[i+0x03]^x3[block[i+0x03]]

    xor.b    W5, W7, W5        ;temp0=block[i+0x00]^x2[block[i+0x00]]^x1[block[i+0x00]]^x1[block[i+0x01]]^x2[block[i+0x02]]^temp3
    xor.b    W9, W7, W9        ;temp1=block[i+0x01]^x1[block[i+0x01]]^x2[block[i+0x01]]^x1[block[i+0x02]]^x2[block[i+0x03]]^temp3
    xor.b    W8, W7, W8        ;temp2=block[i+0x00]^x3[block[i+0x00]]^block[i+0x01]^x3[block[i+0x01]]^block[i+0x02]^x3[block[i+0x02]]^block[i+0x03]^x3[block[i+0x03]]^temp3

    xor.b    W7, W10, W7        ;temp3=temp3^x2[block[i+0x03]]^x1[block[i+0x03]]^x1[block[i+0x00]]^x2[block[i+0x01]]^block[i+0x03]

    mov.b    W5, [W0-4]        ;block[i+0]=temp0;
    mov.b    W9, [W0-3]        ;block[i+1]=temp1;
    mov.b    W8, [W0-2]        ;block[i+2]=temp2;
    mov.b    W7, [W0-1]        ;block[i+3]=temp3;

    ;i=4
    mov.b    [W0++], W4        ;W4=block[i+0x00]
    mov.b    W4, W5            ;temp0=block[i+0x00]
    mov.b    W4, W7            ;temp3=block[i+0x00]
    sl        W4, W4            ;W4=x1[block[i+0x00]]
    btsc    W4, #8            
    xor        W4, W6, W4
    mov        W4, W10            ;W10=x1[block[i+0x00]]
    xor.b    W5, W4, W5        ;temp0=block[i+0x00]^x1[block[i+0x00]]
    sl        W4, W8            ;temp2=x2[block[i+0x00]]
    btsc    W8, #8            
    xor        W8, W6, W8
    xor.b    W5, W8, W5        ;temp0=block[i+0x00]^x1[block[i+0x00]]^x2[block[i+0x00]]
    sl        W8, W4            ;W4=x3[block[i+0x00]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W7, W4, W7        ;temp3=block[i+0x00]^x3[block[i+0x00]]

    mov.b    [W0++], W4        ;W4=block[i+0x01]
    mov.b    W4, W9            ;temp1=block[i+0x01]
    xor.b    W7, W4, W7        ;temp3=block[i+0x00]^x3[block[i+0x00]]^block[i+0x01]
    sl        W4, W4            ;W4=x1[block[i+0x01]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W5, W4, W5        ;temp0=block[i+0x00]^x2[block[i+0x00]]^x1[block[i+0x00]]^x1[block[i+0x01]]
    xor.b    W9, W4, W9        ;temp1=block[i+0x01]^x1[block[i+0x01]]
    sl        W4, W4            ;W4=x2[block[i+0x01]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W10, W4, W10    ;W10=x1[block[i+0x00]]^x2[block[i+0x01]]
    xor.b    W9, W4, W9        ;temp1=block[i+0x01]^x1[block[i+0x01]]^x2[block[i+0x01]]
    sl        W4, W4            ;W4=x3[block[i+0x01]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W7, W4, W7        ;temp3=block[i+0x00]^x3[block[i+0x00]]^block[i+0x01]^x3[block[i+0x01]]

    mov.b    [W0++], W4        ;W4=block[i+0x02]
    xor.b    W7, W4, W7        ;temp3=block[i+0x00]^x3[block[i+0x00]]^block[i+0x01]^x3[block[i+0x01]]^block[i+0x02]
    xor.b    W8, W4, W8        ;temp2=x2[block[i+0x00]]^block[i+0x02]
    sl        W4, W4            ;W4=x1[block[i+0x02]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W9, W4, W9        ;temp1=block[i+0x01]^x1[block[i+0x01]]^x2[block[i+0x01]]^x1[block[i+0x02]]
    xor.b    W8, W4, W8        ;temp2=x2[block[i+0x00]]^block[i+0x02]^x1[block[i+0x02]]
    sl        W4, W4            ;W4=x2[block[i+0x02]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W5, W4, W5        ;temp0=block[i+0x00]^x2[block[i+0x00]]^x1[block[i+0x00]]^x1[block[i+0x01]]^x2[block[i+0x02]]
    xor.b    W8, W4, W8        ;temp2=x2[block[i+0x00]]^block[i+0x02]^x1[block[i+0x02]]^x2[block[i+0x02]]
    sl        W4, W4            ;W4=x3[block[i+0x02]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W7, W4, W7        ;temp3=block[i+0x00]^x3[block[i+0x00]]^block[i+0x01]^x3[block[i+0x01]]^block[i+0x02]^x3[block[i+0x02]]

    mov.b    [W0++], W4        ;W4=block[i+0x03]
    xor.b    W10, W4, W10        ;W10=x1[block[i+0x00]]^x2[block[i+0x01]]^block[i+0x03]
    xor.b    W7, W4, W7        ;temp3=block[i+0x00]^x3[block[i+0x00]]^block[i+0x01]^x3[block[i+0x01]]^block[i+0x02]^x3[block[i+0x02]]^block[i+0x03]
    sl        W4, W4            ;W4=x1[block[i+0x03]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W10, W4, W10        ;W10=x1[block[i+0x00]]^x2[block[i+0x01]]^block[i+0x03]^x1[block[i+0x03]]
    xor.b    W8, W4, W8        ;temp2=x2[block[i+0x00]]^block[i+0x02]^x1[block[i+0x02]]^x2[block[i+0x02]]^x1[block[i+0x03]]
    sl        W4, W4            ;W4=x2[block[i+0x03]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W10, W4, W10        ;W10=x1[block[i+0x00]]^x2[block[i+0x01]]^block[i+0x03]^x2[block[i+0x03]]
    xor.b    W9, W4, W9        ;temp1=block[i+0x01]^x1[block[i+0x01]]^x2[block[i+0x01]]^x1[block[i+0x02]]^x2[block[i+0x03]]
    sl        W4, W4            ;W4=x3[block[i+0x03]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W7, W4, W7        ;temp3=block[i+0x00]^x3[block[i+0x00]]^block[i+0x01]^x3[block[i+0x01]]^block[i+0x02]^x3[block[i+0x02]]^block[i+0x03]^x3[block[i+0x03]]

    xor.b    W5, W7, W5        ;temp0=block[i+0x00]^x2[block[i+0x00]]^x1[block[i+0x00]]^x1[block[i+0x01]]^x2[block[i+0x02]]^temp3
    xor.b    W9, W7, W9        ;temp1=block[i+0x01]^x1[block[i+0x01]]^x2[block[i+0x01]]^x1[block[i+0x02]]^x2[block[i+0x03]]^temp3
    xor.b    W8, W7, W8        ;temp2=block[i+0x00]^x3[block[i+0x00]]^block[i+0x01]^x3[block[i+0x01]]^block[i+0x02]^x3[block[i+0x02]]^block[i+0x03]^x3[block[i+0x03]]^temp3

    xor.b    W7, W10, W7        ;temp3=temp3^x2[block[i+0x03]]^x1[block[i+0x03]]^x1[block[i+0x00]]^x2[block[i+0x01]]^block[i+0x03]

    mov.b    W5, [W0-4]        ;block[i+0]=temp0;
    mov.b    W9, [W0-3]        ;block[i+1]=temp1;
    mov.b    W8, [W0-2]        ;block[i+2]=temp2;
    mov.b    W7, [W0-1]        ;block[i+3]=temp3;
    
    ;i=8
    mov.b    [W0++], W4        ;W4=block[i+0x00]
    mov.b    W4, W5            ;temp0=block[i+0x00]
    mov.b    W4, W7            ;temp3=block[i+0x00]
    sl        W4, W4            ;W4=x1[block[i+0x00]]
    btsc    W4, #8            
    xor        W4, W6, W4
    mov        W4, W10            ;W10=x1[block[i+0x00]]
    xor.b    W5, W4, W5        ;temp0=block[i+0x00]^x1[block[i+0x00]]
    sl        W4, W8            ;temp2=x2[block[i+0x00]]
    btsc    W8, #8            
    xor        W8, W6, W8
    xor.b    W5, W8, W5        ;temp0=block[i+0x00]^x1[block[i+0x00]]^x2[block[i+0x00]]
    sl        W8, W4            ;W4=x3[block[i+0x00]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W7, W4, W7        ;temp3=block[i+0x00]^x3[block[i+0x00]]

    mov.b    [W0++], W4        ;W4=block[i+0x01]
    mov.b    W4, W9            ;temp1=block[i+0x01]
    xor.b    W7, W4, W7        ;temp3=block[i+0x00]^x3[block[i+0x00]]^block[i+0x01]
    sl        W4, W4            ;W4=x1[block[i+0x01]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W5, W4, W5        ;temp0=block[i+0x00]^x2[block[i+0x00]]^x1[block[i+0x00]]^x1[block[i+0x01]]
    xor.b    W9, W4, W9        ;temp1=block[i+0x01]^x1[block[i+0x01]]
    sl        W4, W4            ;W4=x2[block[i+0x01]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W10, W4, W10    ;W10=x1[block[i+0x00]]^x2[block[i+0x01]]
    xor.b    W9, W4, W9        ;temp1=block[i+0x01]^x1[block[i+0x01]]^x2[block[i+0x01]]
    sl        W4, W4            ;W4=x3[block[i+0x01]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W7, W4, W7        ;temp3=block[i+0x00]^x3[block[i+0x00]]^block[i+0x01]^x3[block[i+0x01]]

    mov.b    [W0++], W4        ;W4=block[i+0x02]
    xor.b    W7, W4, W7        ;temp3=block[i+0x00]^x3[block[i+0x00]]^block[i+0x01]^x3[block[i+0x01]]^block[i+0x02]
    xor.b    W8, W4, W8        ;temp2=x2[block[i+0x00]]^block[i+0x02]
    sl        W4, W4            ;W4=x1[block[i+0x02]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W9, W4, W9        ;temp1=block[i+0x01]^x1[block[i+0x01]]^x2[block[i+0x01]]^x1[block[i+0x02]]
    xor.b    W8, W4, W8        ;temp2=x2[block[i+0x00]]^block[i+0x02]^x1[block[i+0x02]]
    sl        W4, W4            ;W4=x2[block[i+0x02]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W5, W4, W5        ;temp0=block[i+0x00]^x2[block[i+0x00]]^x1[block[i+0x00]]^x1[block[i+0x01]]^x2[block[i+0x02]]
    xor.b    W8, W4, W8        ;temp2=x2[block[i+0x00]]^block[i+0x02]^x1[block[i+0x02]]^x2[block[i+0x02]]
    sl        W4, W4            ;W4=x3[block[i+0x02]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W7, W4, W7        ;temp3=block[i+0x00]^x3[block[i+0x00]]^block[i+0x01]^x3[block[i+0x01]]^block[i+0x02]^x3[block[i+0x02]]

    mov.b    [W0++], W4        ;W4=block[i+0x03]
    xor.b    W10, W4, W10        ;W10=x1[block[i+0x00]]^x2[block[i+0x01]]^block[i+0x03]
    xor.b    W7, W4, W7        ;temp3=block[i+0x00]^x3[block[i+0x00]]^block[i+0x01]^x3[block[i+0x01]]^block[i+0x02]^x3[block[i+0x02]]^block[i+0x03]
    sl        W4, W4            ;W4=x1[block[i+0x03]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W10, W4, W10        ;W10=x1[block[i+0x00]]^x2[block[i+0x01]]^block[i+0x03]^x1[block[i+0x03]]
    xor.b    W8, W4, W8        ;temp2=x2[block[i+0x00]]^block[i+0x02]^x1[block[i+0x02]]^x2[block[i+0x02]]^x1[block[i+0x03]]
    sl        W4, W4            ;W4=x2[block[i+0x03]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W10, W4, W10        ;W10=x1[block[i+0x00]]^x2[block[i+0x01]]^block[i+0x03]^x2[block[i+0x03]]
    xor.b    W9, W4, W9        ;temp1=block[i+0x01]^x1[block[i+0x01]]^x2[block[i+0x01]]^x1[block[i+0x02]]^x2[block[i+0x03]]
    sl        W4, W4            ;W4=x3[block[i+0x03]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W7, W4, W7        ;temp3=block[i+0x00]^x3[block[i+0x00]]^block[i+0x01]^x3[block[i+0x01]]^block[i+0x02]^x3[block[i+0x02]]^block[i+0x03]^x3[block[i+0x03]]

    xor.b    W5, W7, W5        ;temp0=block[i+0x00]^x2[block[i+0x00]]^x1[block[i+0x00]]^x1[block[i+0x01]]^x2[block[i+0x02]]^temp3
    xor.b    W9, W7, W9        ;temp1=block[i+0x01]^x1[block[i+0x01]]^x2[block[i+0x01]]^x1[block[i+0x02]]^x2[block[i+0x03]]^temp3
    xor.b    W8, W7, W8        ;temp2=block[i+0x00]^x3[block[i+0x00]]^block[i+0x01]^x3[block[i+0x01]]^block[i+0x02]^x3[block[i+0x02]]^block[i+0x03]^x3[block[i+0x03]]^temp3

    xor.b    W7, W10, W7        ;temp3=temp3^x2[block[i+0x03]]^x1[block[i+0x03]]^x1[block[i+0x00]]^x2[block[i+0x01]]^block[i+0x03]

    mov.b    W5, [W0-4]        ;block[i+0]=temp0;
    mov.b    W9, [W0-3]        ;block[i+1]=temp1;
    mov.b    W8, [W0-2]        ;block[i+2]=temp2;
    mov.b    W7, [W0-1]        ;block[i+3]=temp3;

    ;i=12
    mov.b    [W0++], W4        ;W4=block[i+0x00]
    mov.b    W4, W5            ;temp0=block[i+0x00]
    mov.b    W4, W7            ;temp3=block[i+0x00]
    sl        W4, W4            ;W4=x1[block[i+0x00]]
    btsc    W4, #8            
    xor        W4, W6, W4
    mov        W4, W10            ;W10=x1[block[i+0x00]]
    xor.b    W5, W4, W5        ;temp0=block[i+0x00]^x1[block[i+0x00]]
    sl        W4, W8            ;temp2=x2[block[i+0x00]]
    btsc    W8, #8            
    xor        W8, W6, W8
    xor.b    W5, W8, W5        ;temp0=block[i+0x00]^x1[block[i+0x00]]^x2[block[i+0x00]]
    sl        W8, W4            ;W4=x3[block[i+0x00]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W7, W4, W7        ;temp3=block[i+0x00]^x3[block[i+0x00]]

    mov.b    [W0++], W4        ;W4=block[i+0x01]
    mov.b    W4, W9            ;temp1=block[i+0x01]
    xor.b    W7, W4, W7        ;temp3=block[i+0x00]^x3[block[i+0x00]]^block[i+0x01]
    sl        W4, W4            ;W4=x1[block[i+0x01]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W5, W4, W5        ;temp0=block[i+0x00]^x2[block[i+0x00]]^x1[block[i+0x00]]^x1[block[i+0x01]]
    xor.b    W9, W4, W9        ;temp1=block[i+0x01]^x1[block[i+0x01]]
    sl        W4, W4            ;W4=x2[block[i+0x01]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W10, W4, W10    ;W10=x1[block[i+0x00]]^x2[block[i+0x01]]
    xor.b    W9, W4, W9        ;temp1=block[i+0x01]^x1[block[i+0x01]]^x2[block[i+0x01]]
    sl        W4, W4            ;W4=x3[block[i+0x01]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W7, W4, W7        ;temp3=block[i+0x00]^x3[block[i+0x00]]^block[i+0x01]^x3[block[i+0x01]]

    mov.b    [W0++], W4        ;W4=block[i+0x02]
    xor.b    W7, W4, W7        ;temp3=block[i+0x00]^x3[block[i+0x00]]^block[i+0x01]^x3[block[i+0x01]]^block[i+0x02]
    xor.b    W8, W4, W8        ;temp2=x2[block[i+0x00]]^block[i+0x02]
    sl        W4, W4            ;W4=x1[block[i+0x02]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W9, W4, W9        ;temp1=block[i+0x01]^x1[block[i+0x01]]^x2[block[i+0x01]]^x1[block[i+0x02]]
    xor.b    W8, W4, W8        ;temp2=x2[block[i+0x00]]^block[i+0x02]^x1[block[i+0x02]]
    sl        W4, W4            ;W4=x2[block[i+0x02]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W5, W4, W5        ;temp0=block[i+0x00]^x2[block[i+0x00]]^x1[block[i+0x00]]^x1[block[i+0x01]]^x2[block[i+0x02]]
    xor.b    W8, W4, W8        ;temp2=x2[block[i+0x00]]^block[i+0x02]^x1[block[i+0x02]]^x2[block[i+0x02]]
    sl        W4, W4            ;W4=x3[block[i+0x02]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W7, W4, W7        ;temp3=block[i+0x00]^x3[block[i+0x00]]^block[i+0x01]^x3[block[i+0x01]]^block[i+0x02]^x3[block[i+0x02]]

    mov.b    [W0++], W4        ;W4=block[i+0x03]
    xor.b    W10, W4, W10        ;W10=x1[block[i+0x00]]^x2[block[i+0x01]]^block[i+0x03]
    xor.b    W7, W4, W7        ;temp3=block[i+0x00]^x3[block[i+0x00]]^block[i+0x01]^x3[block[i+0x01]]^block[i+0x02]^x3[block[i+0x02]]^block[i+0x03]
    sl        W4, W4            ;W4=x1[block[i+0x03]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W10, W4, W10        ;W10=x1[block[i+0x00]]^x2[block[i+0x01]]^block[i+0x03]^x1[block[i+0x03]]
    xor.b    W8, W4, W8        ;temp2=x2[block[i+0x00]]^block[i+0x02]^x1[block[i+0x02]]^x2[block[i+0x02]]^x1[block[i+0x03]]
    sl        W4, W4            ;W4=x2[block[i+0x03]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W10, W4, W10        ;W10=x1[block[i+0x00]]^x2[block[i+0x01]]^block[i+0x03]^x2[block[i+0x03]]
    xor.b    W9, W4, W9        ;temp1=block[i+0x01]^x1[block[i+0x01]]^x2[block[i+0x01]]^x1[block[i+0x02]]^x2[block[i+0x03]]
    sl        W4, W4            ;W4=x3[block[i+0x03]]
    btsc    W4, #8            
    xor        W4, W6, W4
    xor.b    W7, W4, W7        ;temp3=block[i+0x00]^x3[block[i+0x00]]^block[i+0x01]^x3[block[i+0x01]]^block[i+0x02]^x3[block[i+0x02]]^block[i+0x03]^x3[block[i+0x03]]

    xor.b    W5, W7, W5        ;temp0=block[i+0x00]^x2[block[i+0x00]]^x1[block[i+0x00]]^x1[block[i+0x01]]^x2[block[i+0x02]]^temp3
    xor.b    W9, W7, W9        ;temp1=block[i+0x01]^x1[block[i+0x01]]^x2[block[i+0x01]]^x1[block[i+0x02]]^x2[block[i+0x03]]^temp3
    xor.b    W8, W7, W8        ;temp2=block[i+0x00]^x3[block[i+0x00]]^block[i+0x01]^x3[block[i+0x01]]^block[i+0x02]^x3[block[i+0x02]]^block[i+0x03]^x3[block[i+0x03]]^temp3

    xor.b    W7, W10, W7        ;temp3=temp3^x2[block[i+0x03]]^x1[block[i+0x03]]^x1[block[i+0x00]]^x2[block[i+0x01]]^block[i+0x03]

    mov.b    W5, [W0-4]        ;block[i+0]=temp0;
    mov.b    W9, [W0-3]        ;block[i+1]=temp1;
    mov.b    W8, [W0-2]        ;block[i+2]=temp2;
    mov.b    W7, [W0-1]        ;block[i+3]=temp3;

    ;i is 16
    ;Restore W2 to beginning of data block
    sub        #16, W0

DecodeSSubstitute:
    ;
    ;Inverse S-table subsitution (uses Si-table)
    ;
    mov        #edsoffset(SiTable), W6
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


    ;Data block pointer is forward 16
    ;
    ;Decode shift row
    ;
    ;Given this byte order:     0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15
    ;This byte order is needed: 0  13 10 7  4  1  14 11 8  5  2  15 12 9  6  3
    mov.b    [W0+1-16], W4   ;save the data in 1
    mov.b    [W0+5-16], W5   ;save the data in 5
    mov.b    [W0+9-16], W8   ;save the data in 9
    mov.b    [W0+13-16], W9  ;save the data in 13
    mov.b    W4, [W0+5-16]   ;move old 1 to new 5
    mov.b    W5, [W0+9-16]   ;move old 5 to new 9
    mov.b    W8, [W0+13-16]  ;move old 9 to new 13
    mov.b    W9, [W0+1-16]   ;move old 13 to new 1

    mov.b    [W0+2-16], W4   ;save the data in 2
    mov.b    [W0+10-16], W5  ;save the data in 10
    mov.b    W4, [W0+10-16]  ;move old 2 to new 10
    mov.b    W5, [W0+2-16]   ;move old 10 to new 2

    mov.b    [W0+3-16], W4   ;save the data in 3
    mov.b    [W0+7-16], W5   ;save the data in 7
    mov.b    [W0+11-16], W8  ;save the data in 11
    mov.b    [W0+15-16], W9  ;save the data in 15
    mov.b    W4, [W0+15-16]  ;move old 3 to new 15
    mov.b    W5, [W0+3-16]   ;move old 7 to new 3
    mov.b    W8, [W0+7-16]   ;move old 11 to new 7
    mov.b    W9, [W0+11-16]  ;move old 15 to new 11

    mov.b    [W0+6-16], W4   ;save the data in 6
    mov.b    [W0+14-16], W5  ;save the data in 14
    mov.b    W4, [W0+14-16]  ;move old 6 to new 14
    mov.b    W5, [W0+6-16]   ;move old 14 to new 6

DecodeKeyAddition:
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
    dec        W3, W3    ;roundCounter
    bra        NZ, DecodeRoundLoop

    pop.d    W10
    pop.d    W8
    pop        PSVPAG
.ifndecl __HAS_EDS
    pop  CORCON
.endif
    return
