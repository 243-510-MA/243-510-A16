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

;void AES_SW_RoundKeysCreate(
;    UINT8 * key, 
;    void * session, 
;)
; key is in WREG0
; session is in WREG1
;
;WREG usage in function
;  WREG0 - session key pointer
;  WREG1 - key pointer, loop interation counter
;  WREG2 - temp variable
;  WREG3 - PSVPAG copy
;  WREG4 - xor constant for rcon
;  WREG5 - rcon
;  WREG6 - w[i-Nk] pointer
;  WREG7 - psv table offset
    .global    _AES_SW_RoundKeysCreate256
_AES_SW_RoundKeysCreate256:
.ifndecl __HAS_EDS
    push CORCON
.endif
    mov        PSVPAG, W3

    ;Need to clear the upper half of W2 since key_size is a UINT8
    and.w #0x00FF, W2
    ;Copy key_length to round_keys variable
    mov     W2,[W0++]
    clr     [W0++]

    ;Enable Program Space Visibility of S-table
    PSVEnable
    mov        #edspage(STable), W7
    mov        W7, PSVPAG
    mov        #edsoffset(STable), W7
    clr     W2

    ;Initialize variables
    mov        #1, W5        ; rcon
    mov        #0x011B, W4    ; xor constant for xtime operations

    ;point WREG6 to w[i-Nk]    
    mov     W0,W6

    repeat #15
    mov     [W1++],[W0++]   ;w0-w7 of the key to the session key 

    ;WREG1 points to w[i] (w8 in this case)    
    ;w8 = SubWord(RotWord(w7))^rcon^w[i-Nk];
    mov.b    [W0-3], W2            ;RotWord(w7) for byte 3
    mov.b    [W7+W2], W2         ;SubWord() for byte 3
    xor.b    W2, W5, W2            ;^rcon
    xor.b    W2, [W6++], [W0++]  ;^w[i-Nk]
    mov.b    [W0-3], W2            ;RotWord(w7) for byte 2
    mov.b    [W7+W2], W2         ;SubWord() for byte 2
    xor.b    W2, [W6++], [W0++]  ;^w[i-Nk]
    mov.b    [W0-3], W2            ;RotWord(w7) for byte 1
    mov.b    [W7+W2], W2         ;SubWord() for byte 1
    xor.b    W2, [W6++], [W0++]  ;^w[i-Nk]
    mov.b    [W0-7], W2            ;RotWord(w7) for byte 0
    mov.b    [W7+W2], W2         ;SubWord() for byte 0
    xor.b    W2, [W6++], [W0++]  ;^w[i-Nk]

    sl        W5, W5                ;rcon = xtime(rcon);
    btsc    W5, #8
    xor        W4, W5, W5
    
    ;w9 = w[i-1] ^ w[i-Nk]
    mov     [W0-4],W2
    xor     W2,[W6++],[W0++]
    mov     [W0-4],W2
    xor     W2,[W6++],[W0++]

    ;W10 = w[i-1] ^ w[i-Nk]
    mov     [W0-4],W2
    xor     W2,[W6++],[W0++]
    mov     [W0-4],W2
    xor     W2,[W6++],[W0++]

    ;W11 = w[i-1] ^ w[i-Nk]
    mov     [W0-4],W2
    xor     W2,[W6++],[W0++]
    mov     [W0-4],W2
    xor     W2,[W6++],[W0++]

    mov     #6,W1

_AES_SW_RoundKeysCreate256_Loop:
    ;loop

    clr     W2                  ;clear W2 so that upper byte doesn't affect offset

    ;w[i] = SubWord(w[i-1])^w[i-Nk];
    mov.b    [W0-4], W2            ;RotWord(w[i-1]) for byte 3
    mov.b    [W7+W2], W2         ;SubWord() for byte 3
    xor.b    W2, [W6++], [W0++]  ;^w[i-Nk]
    mov.b    [W0-4], W2            ;RotWord(w[i-1]) for byte 2
    mov.b    [W7+W2], W2         ;SubWord() for byte 2
    xor.b    W2, [W6++], [W0++]  ;^w[i-Nk]
    mov.b    [W0-4], W2            ;RotWord(w[i-1]) for byte 1
    mov.b    [W7+W2], W2         ;SubWord() for byte 1
    xor.b    W2, [W6++], [W0++]  ;^w[i-Nk]
    mov.b    [W0-4], W2            ;RotWord(w[i-1]) for byte 0
    mov.b    [W7+W2], W2         ;SubWord() for byte 0
    xor.b    W2, [W6++], [W0++]  ;^w[i-Nk]

    ;wi = w[i-1] ^ w[i-Nk]
    mov     [W0-4],W2
    xor     W2,[W6++],[W0++]
    mov     [W0-4],W2
    xor     W2,[W6++],[W0++]

    ;wi = w[i-1] ^ w[i-Nk]
    mov     [W0-4],W2
    xor     W2,[W6++],[W0++]
    mov     [W0-4],W2
    xor     W2,[W6++],[W0++]

    ;wi = w[i-1] ^ w[i-Nk]
    mov     [W0-4],W2
    xor     W2,[W6++],[W0++]
    mov     [W0-4],W2
    xor     W2,[W6++],[W0++]

    ;WREG1 points to w[i] (w6 in this case)    
    ;w6 = SubWord(RotWord(w5))^rcon^w[i-Nk];
    clr     W2                  ;clear W2 so that upper byte doesn't affect offset
    mov.b    [W0-3], W2            ;RotWord(w5) for byte 3
    mov.b    [W7+W2], W2         ;SubWord() for byte 3
    xor.b    W2, W5, W2            ;^rcon
    xor.b    W2, [W6++], [W0++]  ;^w[i-Nk]
    mov.b    [W0-3], W2            ;RotWord(w5) for byte 2
    mov.b    [W7+W2], W2         ;SubWord() for byte 2
    xor.b    W2, [W6++], [W0++]  ;^w[i-Nk]
    mov.b    [W0-3], W2            ;RotWord(w5) for byte 1
    mov.b    [W7+W2], W2         ;SubWord() for byte 1
    xor.b    W2, [W6++], [W0++]  ;^w[i-Nk]
    mov.b    [W0-7], W2            ;RotWord(w5) for byte 0
    mov.b    [W7+W2], W2         ;SubWord() for byte 0
    xor.b    W2, [W6++], [W0++]  ;^w[i-Nk]

    sl        W5, W5                ;rcon = xtime(rcon);
    btsc    W5, #8
    xor        W4, W5, W5

    ;wi = w[i-1] ^ w[i-Nk]
    mov     [W0-4],W2
    xor     W2,[W6++],[W0++]
    mov     [W0-4],W2
    xor     W2,[W6++],[W0++]

    ;wi = w[i-1] ^ w[i-Nk]
    mov     [W0-4],W2
    xor     W2,[W6++],[W0++]
    mov     [W0-4],W2
    xor     W2,[W6++],[W0++]

    ;wi = w[i-1] ^ w[i-Nk]
    mov     [W0-4],W2
    xor     W2,[W6++],[W0++]
    mov     [W0-4],W2
    xor     W2,[W6++],[W0++]

    dec        W1, W1
    bra        NZ, _AES_SW_RoundKeysCreate256_Loop

    mov        W3, PSVPAG
.ifndecl __HAS_EDS
    pop CORCON
.endif

    return
    