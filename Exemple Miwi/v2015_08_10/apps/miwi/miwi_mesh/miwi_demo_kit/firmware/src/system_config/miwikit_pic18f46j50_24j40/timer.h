/********************************************************************
 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the "Company") for its PIC(R) Microcontroller is intended and
 supplied to you, the Company's customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
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
 *******************************************************************/

#ifndef _MS_TIMER_HEADER_FILE
#define _MS_TIMER_HEADER_FILE

#include "system.h"

    
    
#define delay_us(x)             \
{                               \
    unsigned long _dcnt;        \
    _dcnt=x*((unsigned long)(0.00001/(SYS_CLK_FrequencyInstructionGet())/6));   \
    while(_dcnt--);             \
}

#define delay_ms(x)             \
{                               \
    unsigned long _dcnt;        \
    unsigned long _ms;          \
    _ms = x;                    \
    while(_ms)                  \
    {                           \
        _dcnt=((unsigned long)(0.001/(1.0/SYS_CLK_FrequencyInstructionGet())/6));    \
        while(_dcnt--);         \
        _ms--;                  \
    }                           \
}


#endif
