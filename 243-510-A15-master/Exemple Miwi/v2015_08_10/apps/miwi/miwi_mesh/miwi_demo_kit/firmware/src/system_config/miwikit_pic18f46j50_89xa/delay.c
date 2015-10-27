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
void DELAY_10us( uint32_t tenMicroSecondCounter )
{
    volatile int32_t cyclesRequiredForEntireDelay;
   
    
    if (SYS_CLK_FrequencyInstructionGet() <= 2500000) //for all FCY speeds under 2MHz (FOSC <= 10MHz)
    {
        //26 cycles burned through this path (includes return to caller).
        //For FOSC == 1MHZ, it takes 104us.
        //For FOSC == 4MHZ, it takes 26us
        //For FOSC == 8MHZ, it takes 13us.
        //For FOSC == 10MHZ, it takes 10.5us.
    }
    else
    {
        //14 cycles burned to this point.

        //We want to pre-calculate number of cycles required to delay 10us * tenMicroSecondCounter using a 1 cycle granule.
        cyclesRequiredForEntireDelay = (int32_t)(SYS_CLK_FrequencyInstructionGet()/100000) * tenMicroSecondCounter;

        //We subtract all the cycles used up until we reach the while loop below, where each loop cycle count is subtracted.
        //Also we subtract the 22 cycle function return.
        cyclesRequiredForEntireDelay -= (153 + 22);

        if (cyclesRequiredForEntireDelay <= 45)
        {
            // If we have exceeded the cycle count already, bail! Best compromise between FOSC == 12MHz and FOSC == 24MHz.
        }
        else
        {
            //Try as you may, you can't get out of this heavier-duty case under 30us. ;]

            while (cyclesRequiredForEntireDelay>0) //153 cycles used to this point.
            {
                Nop(); //Delay one instruction cycle at a time, not absolutely necessary.
                cyclesRequiredForEntireDelay -= 42; //Subtract cycles burned while doing each delay stage, 42 in this case.
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

    
    int32_t cyclesRequiredForEntireDelay = 0;

    // We want to pre-calculate number of cycles required to delay 1ms, using a 1 cycle granule.
    cyclesRequiredForEntireDelay = (int32_t)(SYS_CLK_FrequencyInstructionGet()/1000) * ms;

    // We subtract all the cycles used up until we reach the while loop below, where each loop cycle count is subtracted.
    // Also we subtract the 22 cycle function return.
    cyclesRequiredForEntireDelay -= (148 + 22);

    if (cyclesRequiredForEntireDelay <= (170+25))
    {
        return;     // If we have exceeded the cycle count already, bail!
    }
    else
    {
        while (cyclesRequiredForEntireDelay > 0) //148 cycles used to this point.
        {
            Nop();                              // Delay one instruction cycle at a time, not absolutely necessary.
            cyclesRequiredForEntireDelay -= 39; // Subtract cycles burned while doing each delay stage, 39 in this case.
        }
    }
   
}

