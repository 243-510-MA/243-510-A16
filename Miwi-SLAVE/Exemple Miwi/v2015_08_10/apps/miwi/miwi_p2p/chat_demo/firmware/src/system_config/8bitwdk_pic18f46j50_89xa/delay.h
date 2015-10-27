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
#ifndef __DELAY_H
#define __DELAY_H

#include "system.h"

/****************************************************************************
  Function:
    void DELAY_10us( uint32_t tenMicroSecondCounter )

  Description:
    This routine performs a software delay in intervals of 10 microseconds.

  Precondition:
    None

  Parameters:
    UINT32 tenMicroSecondCounter - number of ten microsecond delays
    to perform at once.

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void DELAY_10us( uint32_t tenMicroSecondCounter );

/****************************************************************************
  Function:
    void DELAY_ms( UINT16 ms )

  Description:
    This routine performs a software delay in intervals of 1 millisecond.

  Precondition:
    None

  Parameters:
    UINT16 ms - number of one millisecond delays to perform at once.

  Returns:
    None

  Remarks:
    None
  ***************************************************************************/
void DELAY_ms( uint16_t ms );


#endif
