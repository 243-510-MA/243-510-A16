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

#ifndef _SPI_H_
#define _SPI_H_

#include <stdint.h>

/************************ FUNCTION PROTOTYPES **********************/

/*********************************************************************
* Function:         void SPIPut(uint8_t v)
*
* PreCondition:     SPI has been configured
*
* Input:            v - is the byte that needs to be transfered
*
* Output:           none
*
* Side Effects:	    SPI transmits the byte
*
* Overview:         This function will send a byte over the SPI
*
* Note:             None
********************************************************************/
void SPIPut(uint8_t v);

/*********************************************************************
* Function:         uint8_t SPIGet(void)
*
* PreCondition:     SPI has been configured
*
* Input:            none
*
* Output:           uint8_t - the byte that was last received by the SPI
*
* Side Effects:	    none
*
* Overview:         This function will read a byte over the SPI
*
* Note:             None
********************************************************************/
uint8_t SPIGet(void);

#endif

