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

#define _XTAL_FREQ 16000000

#ifndef __SYSTEM_H_
	#define __SYSTEM_H_

#include <xc.h>
#include <stdio.h>
#include <stdbool.h>

#include "lcd.h"
#include "eeprom.h"
#include "serial_flash.h"
#include "symbol.h"
#include "timer.h"
#include "delay.h"
#include "button.h"
#include "spi.h"
#include "console.h"



/************************ DATA TYPE *******************************/

// DOM-IGNORE-BEGIN
/*********************************************************************
 Overview: Data types for drivers. This will facilitate easy
           access smaller chunks of larger data types when sending
           or receiving data (for example byte sized send/receive
           over parallel 8-bit interface.
*********************************************************************/
// DOM-IGNORE-END
typedef union
{

    uint8_t  v[4];
    uint16_t w[2];
    uint32_t Val;

}API_UINT32_UNION;

typedef union
{

    uint8_t  v[2];
    uint16_t Val;

}API_UINT16_UNION;





#define MAIN_RETURN void



/*********************************************************************
* Macro: #define	SYS_CLK_FrequencySystemGet()
*
* Overview: This macro returns the system clock frequency in Hertz.
*
*
********************************************************************/
#define SYS_CLK_FrequencySystemGet()    (16000000)

/*********************************************************************
* Macro: #define	SYS_CLK_FrequencyPeripheralGet()
*
* Overview: This macro returns the peripheral clock frequency
*			used in Hertz.
*
*
********************************************************************/
#define SYS_CLK_FrequencyPeripheralGet()    (SYS_CLK_FrequencySystemGet()/4)

/*********************************************************************
* Macro: #define	SYS_CLK_FrequencyInstructionGet()
*
* Overview: This macro returns instruction clock frequency
*			used in Hertz.
*
*
********************************************************************/
#define SYS_CLK_FrequencyInstructionGet()   (SYS_CLK_FrequencySystemGet()/4)
#define FCY                                 (SYS_CLK_FrequencyInstructionGet())


/*********************************************************************
* Function: void SYSTEM_Initialize( void )
*
* Overview: Initializes the system.
*
* PreCondition: None
*
* Input:  None
*
* Output: None
*
********************************************************************/
void SYSTEM_Initialize(void);


#endif

/*************************************************************************
 * EOF system.h
 */

