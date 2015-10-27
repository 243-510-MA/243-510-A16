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

#ifndef __SYMBOL_TIME_H_
#define __SYMBOL_TIME_H_

#include "system.h"
/************************ HEADERS **********************************/


/************************ DEFINITIONS ******************************/



#define ONE_SECOND              ((uint32_t)SYS_CLK_FrequencySystemGet()/32)
#define SYMBOLS_TO_TICKS(a)     ((uint32_t)SYS_CLK_FrequencySystemGet()/1000*(a))/(uint32_t)2000
#define TICKS_TO_SYMBOLS(a)     ((uint32_t)2000*a)/((uint32_t)SYS_CLK_FrequencySystemGet()/1000)


#define TMR_IF          PIR1bits.TMR1IF
#define TMR_IE          PIE1bits.TMR1IE
#define TMR_ON          T1CONbits.TMR1ON
#define TMR_CON         T1CON
#define TMR_L           TMR1L
#define TMR_H           TMR1H



#define ONE_MILI_SECOND     (ONE_SECOND/1000)
#define HUNDRED_MILI_SECOND (ONE_SECOND/10)
#define FORTY_MILI_SECOND   (ONE_SECOND/25)
#define FIFTY_MILI_SECOND   (ONE_SECOND/20)
#define TWENTY_MILI_SECOND  (ONE_SECOND/50)
#define TEN_MILI_SECOND     (ONE_SECOND/100)
#define FIVE_MILI_SECOND    (ONE_SECOND/200)
#define TWO_MILI_SECOND     (ONE_SECOND/500)
#define ONE_MINUTE          (ONE_SECOND*60)
#define ONE_HOUR            (ONE_MINUTE*60)

#define MiWi_TickGetDiff(a,b) (a.Val - b.Val)

/************************ DATA TYPES *******************************/


/******************************************************************
 // Time unit defined based on IEEE 802.15.4 specification.
 // One tick is equal to one symbol time, or 16us. The Tick structure
 // is four bytes in length and is capable of represent time up to
 // about 19 hours.
 *****************************************************************/
typedef union _MIWI_TICK
{
    uint32_t Val;
    struct _MIWI_TICK_bytes
    {
        uint8_t b0;
        uint8_t b1;
        uint8_t b2;
        uint8_t b3;
    } byte;
    uint8_t v[4];
    struct _MIWI_TICK_words
    {
        uint16_t w0;
        uint16_t w1;
    } word;
} MIWI_TICK;

void InitSymbolTimer(void);
MIWI_TICK MiWi_TickGet(void);

/************************ VARIABLES ********************************/

extern volatile uint8_t timerExtension1,timerExtension2;
#endif

