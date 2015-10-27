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
#include <stdint.h>
#include "system.h"


#if defined(ENABLE_EEPROM)

    #include "EEPROM.h"


    void EEPROMRead(uint8_t *dest, uint8_t addr, uint8_t count)
    {
        #if defined(__18CXX)
            uint8_t oldGIEH = INTCONbits.GIEH;

            INTCONbits.GIEH = 0;
        #endif

        EE_nCS = 0;
        SPIPut(SPI_READ);
        SPIPut(addr);
        while( count )
        {
            *dest++ = SPIGet();
            count--;
        }
        EE_nCS = 1;

        #if defined(__18CXX)
            INTCONbits.GIEH = oldGIEH;
        #endif
    }


#endif
