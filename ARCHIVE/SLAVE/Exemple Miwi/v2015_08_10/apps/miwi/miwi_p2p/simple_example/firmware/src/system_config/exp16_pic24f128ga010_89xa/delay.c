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
#include "system.h"

/****************************************************************************
  Function:
    void Delay10us( uint32_t tenMicroSecondCounter )

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
void DELAY_10us( uint32_t tenMicroSecondCounter )
{
    volatile int32_t cyclesRequiredForEntireDelay;
        
    
        if(SYS_CLK_FrequencyInstructionGet() <= 500000) //for all FCY speeds under 500KHz (FOSC <= 1MHz)
        {
            //10 cycles burned through this path (includes return to caller).
            //For FOSC == 1MHZ, it takes 5us.
            //For FOSC == 4MHZ, it takes 0.5us
            //For FOSC == 8MHZ, it takes 0.25us.
            //For FOSC == 10MHZ, it takes 0.2us.
        }    
        else
        {
            //7 cycles burned to this point.
            
            //We want to pre-calculate number of cycles required to delay 10us * tenMicroSecondCounter using a 1 cycle granule.
            cyclesRequiredForEntireDelay = (uint32_t)(SYS_CLK_FrequencyInstructionGet()/100000)*tenMicroSecondCounter;
            
            
            //We subtract all the cycles used up until we reach the while loop below, where each loop cycle count is subtracted.
            //Also we subtract the 5 cycle function return.
            cyclesRequiredForEntireDelay -= 44; //(29 + 5) + 10 cycles padding
            
            
            if(cyclesRequiredForEntireDelay <= 0)
            {
                // If we have exceeded the cycle count already, bail!
            }
            else
            {   
                while(cyclesRequiredForEntireDelay>0) //19 cycles used to this point.
                {
                   
                    cyclesRequiredForEntireDelay -= 11; //Subtract cycles burned while doing each delay stage, 12 in this case. Add one cycle as padding.
                    
                }
            }
        }
}

/****************************************************************************
  Function:
    void DELAY_ms( uint16_t ms )

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
void DELAY_ms( uint16_t ms )
{
    
        volatile uint8_t i;
        
        while (ms--)
        {
            i = 4;
            while (i--)
            {
                DELAY_10us( 25 );
            }
        }
  
}

