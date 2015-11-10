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

;***************************************************************************
; FILE          proc.inc
;
; DESCRIPTION   This file includes the proper SFR and bit definitions
;				depending on the device family.
;               all dsPIC30F devices.
;
; AUTHOR        M. H. Etzel, Security Innovation, Inc.
;
; DATE          09/05/2011
;
; NOTES
;
; CHANGES
;

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

.ifndef PROC_INC_SET
.error "cannot include processor definitions for unknown device family"
.endif
