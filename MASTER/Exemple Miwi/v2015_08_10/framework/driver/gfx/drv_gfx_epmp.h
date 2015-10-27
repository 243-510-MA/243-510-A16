/*******************************************************************************
 Module for Microchip Graphics Library - Parallel Master Port Driver Layer

  Company:
    Microchip Technology Inc.

  File Name:
    drv_gfx_pmp.h

  Summary:
    Parallel Master Port header file.

  Description:
    This module declares all display driver layer API.
*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*******************************************************************************/
// DOM-IGNORE-END

#ifndef _GFX_EPMP_H_FILE
#define _GFX_EPMP_H_FILE

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "system.h"

#ifdef USE_GFX_EPMP

#ifdef USE_16BIT_PMP
extern volatile __eds__ uint16_t __attribute__((eds,noload)) epmp_data;
#else
extern volatile __eds__ uint8_t __attribute__((eds,noload)) epmp_data;
#endif

extern volatile uint16_t __attribute__((eds,noload)) pixel_data;


// Note:
/*
    All functions here are defined as inline functions for performance.
    When debugging this portion it is best to look at the assembly output 
    and debug from there.
*/

    // error checking
    #if !defined (EPMPCS1_DATA_SETUP_TIME) && !defined (EPMPCS2_DATA_SETUP_TIME)
        #error "Define EPMPCS1_DATA_SETUP_TIME and/or EPMPCS2_DATA_SETUP_TIME in system_config.h for data set up before read/write strobe timing"
    #endif    
    #if !defined (EPMPCS1_DATA_WAIT_TIME) && !defined (EPMPCS2_DATA_WAIT_TIME)
        #error "Define EPMPCS1_DATA_WAIT_TIME and/or EPMPCS2_DATA_WAIT_TIME in system_config.h for read/write strobe wait states"
    #endif    
    #if !defined (EPMPCS1_DATA_HOLD_TIME) && !defined (EPMPCS2_DATA_HOLD_TIME)
        #error "Define EPMPCS1_DATA_HOLD_TIME and/or EPMPCS2_DATA_HOLD_TIME in system_config.h for data hold after read/write strobe"
    #endif    

/*********************************************************************
 Overview: PMP Interface Data Size.

*********************************************************************/
#if defined (USE_8BIT_PMP)
    typedef uint8_t DRV_GFX_PMP_INTERFACE_DATA;
#elif defined (USE_16BIT_PMP)
    typedef  uint16_t DRV_GFX_PMP_INTERFACE_DATA;
#else
    #error "PMP Data Width is not defined!"
#endif

// *****************************************************************************
/*  Function:
    void DRV_GFX_PMPWaitUntilNotBusy(void)

    Summary:
        This function waits until the PMP is not busy.

    Description:
        This function waits until the PMP is not busy. This check
        is usually performed after a transaction is issued on the PMP.
        The function checks the busy bit of the PMP which is set
        while a read or a write is being performed on the port.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
#define DRV_GFX_PMPWaitUntilNotBusy()   while(PMCON2bits.BUSY);

// *****************************************************************************
/*  Function:
    void DRV_GFX_PMPWrite(DRV_GFX_PMP_INTERFACE_DATA data)

    Summary:
        This function writes data to PMP.

    Description:
        This function writes data to PMP. The size of the data written
        is dependent on the mode of the module.
        - use 8-bit wide data when mode is set to use 8-bits
        - use 16-bit wide data when mode is set to use 16-bits

    Precondition:
        None.

    Parameters:
        data - the data written to the port. The width of data is dependent
               on the mode of the interface. (8-bit or 16-bit mode).

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
extern inline void __attribute__((always_inline)) DRV_GFX_PMPWrite(
                                DRV_GFX_PMP_INTERFACE_DATA data)
{ 
    epmp_data = data;
}

// *****************************************************************************
/*  Function:
    void DRV_GFX_PMPWrites(     DRV_GFX_PMP_INTERFACE_DATA data,
                                uint32_t n)

    Summary:
        This function write data to PMP n-times.

    Description:
        This function writes data to PMP n-times. The width of the
        data written is dependent on the mode of the module.
        - use 8-bit wide data when mode is set to use 8-bits
        - use 16-bit wide data when mode is set to use 16-bits

    Precondition:
        None.

    Parameters:
        data - the data written to the port. The width of data is dependent
               on the mode of the interface. (8-bit or 16-bit mode).
        n - the number of times data is written to PMP.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
extern inline void __attribute__((always_inline)) DRV_GFX_PMPWrites(
                                DRV_GFX_PMP_INTERFACE_DATA data,
                                uint32_t n)
{
    unsigned save_DSWPAG = DSWPAG;
    volatile DRV_GFX_PMP_INTERFACE_DATA *pDest;

    // set up page and offset of the destination
    DSWPAG = __builtin_edspage(&epmp_data);
    pDest = (DRV_GFX_PMP_INTERFACE_DATA *) __builtin_edsoffset(&epmp_data);

    while (n--)
    {
        *pDest = data;
    }

    DSWPAG = save_DSWPAG;
}

// *****************************************************************************
/*  Function:
    void DRV_GFX_PMPBufferWrite(DRV_GFX_PMP_INTERFACE_DATA *pData,
                                uint32_t n)

    Summary:
        This function writes an array of data to PMP.

    Description:
        This function writes an array of data to PMP. The width of the
        data written is dependent on the mode of the module.
        - use 8-bit wide data when mode is set to use 8-bits
        - use 16-bit wide data when mode is set to use 16-bits

    Precondition:
        None.

    Parameters:
        pData - the pointer to the array of data to be written to
                the port. The width of data is dependent
                on the mode of the interface. (8-bit or 16-bit mode).
        n - the number of data to be written to PMP.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
extern inline void __attribute__((always_inline)) DRV_GFX_PMPBufferWrite(
                                DRV_GFX_PMP_INTERFACE_DATA *pData,
                                uint32_t n)
{
    unsigned save_DSWPAG = DSWPAG;
    volatile DRV_GFX_PMP_INTERFACE_DATA *pDest;

    // set up page and offset of the destination
    DSWPAG = __builtin_edspage(&epmp_data);
    pDest = (DRV_GFX_PMP_INTERFACE_DATA *)__builtin_edsoffset(&epmp_data);

    while (n--)
    {
        *pDest = *pData++;
    }

    DSWPAG = save_DSWPAG;
}

// *****************************************************************************
/*  Function:
    DRV_GFX_PMP_INTERFACE_DATA DRV_GFX_PMPRead(void)

    Summary:
        This function reads data from PMP.

    Description:
        This function reads data from PMP. The size of the data read
        is dependent on the mode of the module.
        - use 8-bit wide data when mode is set to use 8-bits
        - use 16-bit wide data when mode is set to use 16-bits

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        The data read from the port. The width of data is dependent
        on the mode of the interface. (8-bit or 16-bit mode).

    Example:
        None.

*/
// *****************************************************************************
extern inline DRV_GFX_PMP_INTERFACE_DATA __attribute__((always_inline)) DRV_GFX_PMPRead(void)
{
        DRV_GFX_PMP_INTERFACE_DATA value;
	value = epmp_data;
	DRV_GFX_PMPWaitUntilNotBusy();
	return PMDIN1;
}

// *****************************************************************************
/*  Function:
    void DRV_GFX_PMPWriteTimingSet(bool write)

    Summary:
        This function sets the PMP read/write timing.

    Description:
        This function will set the PMP timing. Often, the timing for
        read is different than that of write. This will allow
        the ability to switch the timing for read or write.

    Precondition:
        When this change is performed, PMP must not be executing any
        transaction.

    Parameters:
    write - set to true if the timing set is for write and false if the
           timing set is for read.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
extern inline void __attribute__((always_inline)) DRV_GFX_PMPWriteTimingSet(bool write)
{
    uint16_t setup, wait, hold;

    // variable for PMP timing calculation
    // GetPeripheralClock() is in Mhz. pClockPeriod will be in nanoseconds.
    uint16_t pClockPeriod = (1000000000ul) / SYS_CLK_FrequencyPeripheralGet();

#if defined (EPMPCS1_DATA_SETUP_TIME)

    if (write == true)
    {
        setup = EPMPCS1_DATA_SETUP_TIME;
        wait  = EPMPCS1_DATA_WAIT_TIME;
        hold  = EPMPCS1_DATA_HOLD_TIME;
    }
    else
    {
#if defined (EPMPCS1_READ_DATA_SETUP_TIME)
        setup = EPMPCS1_READ_DATA_SETUP_TIME;
        wait  = EPMPCS1_READ_DATA_WAIT_TIME;
        hold  = EPMPCS1_READ_DATA_HOLD_TIME;
#else
        // just use the same for read and write
        setup = EPMPCS1_DATA_SETUP_TIME;
        wait  = EPMPCS1_DATA_WAIT_TIME;
        hold  = EPMPCS1_DATA_HOLD_TIME;
#endif
    }

    if (setup < (pClockPeriod / 4))
        PMCS1MDbits.DWAITB = 0;
    else if (setup >= (pClockPeriod / 4))
    {
        PMCS1MDbits.DWAITB = (setup / pClockPeriod);
        if ((setup % pClockPeriod) > 0)
            PMCS1MDbits.DWAITB += 1;
    }

    if (wait < ((pClockPeriod * 3) / 4))
        PMCS1MDbits.DWAITM = 0;
    else if (wait >= ((pClockPeriod * 3) / 4))
    {
        PMCS1MDbits.DWAITM = (wait / pClockPeriod);
        if ((wait % pClockPeriod) > 0)
            PMCS1MDbits.DWAITM += 1;
    }

    if (hold <= (pClockPeriod/4))
        PMCS1MDbits.DWAITE = 0;
    else if (hold >= (pClockPeriod/4))
    {
        PMCS1MDbits.DWAITE = (hold / pClockPeriod);
        if ((hold % pClockPeriod) > 0)
            PMCS1MDbits.DWAITE += 1;
    }

#endif    

#if defined (EPMPCS2_DATA_SETUP_TIME)

    if (write == true)
    {
        setup = EPMPCS2_DATA_SETUP_TIME;
        wait  = EPMPCS2_DATA_WAIT_TIME;
        hold  = EPMPCS2_DATA_HOLD_TIME;
    }
    else
    {
#if defined (EPMPCS1_READ_DATA_SETUP_TIME)
        setup = EPMPCS2_READ_DATA_SETUP_TIME;
        wait  = EPMPCS2_READ_DATA_WAIT_TIME;
        hold  = EPMPCS2_READ_DATA_HOLD_TIME;
#else
        // just use the same for read and write
        setup = EPMPCS2_DATA_SETUP_TIME;
        wait  = EPMPCS2_DATA_WAIT_TIME;
        hold  = EPMPCS2_DATA_HOLD_TIME;
#endif
    }


    if (setup < (pClockPeriod/4))
        PMCS2MDbits.DWAITB = 0;
    else if (setup >= (pClockPeriod/4))
    {
        PMCS2MDbits.DWAITB = (setup / pClockPeriod);
        if ((setup % pClockPeriod) > 0)
            PMCS2MDbits.DWAITB += 1;
    }

    if (wait < ((pClockPeriod*3)/4))
        PMCS2MDbits.DWAITM = 0;
    else if (wait >= ((pClockPeriod*3)/4))
    {
        PMCS2MDbits.DWAITM = (wait / pClockPeriod);
        if ((wait % pClockPeriod) > 0)
            PMCS2MDbits.DWAITM += 1;
    }

    if (hold <= (pClockPeriod/4))
        PMCS2MDbits.DWAITE = 0;
    else if (hold >= (pClockPeriod/4))
    {
        PMCS2MDbits.DWAITE = (hold / pClockPeriod);
        if ((hold % pClockPeriod) > 0)
            PMCS2MDbits.DWAITE += 1;
    }

#endif // #if defined (EPMPCS2_DATA_SETUP_TIME)

}

// *****************************************************************************
/*  Function:
    uint16_t DRV_GFX_PMPInitialize(void)

    Summary:
        This function initializes the PMP for parallel interface to the
        display controller.

    Description:
        This function initializes the PMP for parallel interface to the
        display controller.
        Interface will be dependent on the mode selected for the module.
        When connecting to a display controller with 8-bit parallel interface
        use 8-bit mode and when connecting to a 16-bit display controller
        use 16-bit mode.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        None.

    Example:
        None.

*/
// *****************************************************************************
extern inline void __attribute__ ((always_inline)) DRV_GFX_PMPInitialize(void)
{ 
#if defined (EPMPCS1_ADDRESS_LINE_COUNT) || defined (EPMPCS2_ADDRESS_LINE_COUNT)
    uint16_t i, addrEnable, addrCount;
#endif

    DisplayResetEnable();       // hold in reset by default
    DisplayResetConfig();       // enable RESET line
    DisplayCmdDataConfig();     // enable RS line
    DisplayDisable();           // not selected by default
    DisplayConfig();            // enable chip select line
    DisplayBacklightOff();      // initially set the backlight to off
    DisplayBacklightConfig();   // set the backlight control pin

    // EPMP setup
    PMCON1bits.ADRMUX = 0;	// address is not multiplexed
    PMCON1bits.MODE = 3;        // master mode
    PMCON1bits.CSF = 0;         // PMCS1 pin used for chip select 1, PMCS2 pin used for chip select 2
    PMCON1bits.ALP = 0;         // set address latch strobes to high active level
    PMCON1bits.ALMODE = 0;      // "smart" address strobes are not used
    PMCON1bits.BUSKEEP = 0;     // bus keeper is not used
	
    PMCON2bits.RADDR = 0x00;	// set CS2 end address
	
#if defined (__PIC24FJ256DA210__)	
    PMCON2bits.MSTSEL = 0;		// set CPU as Master
#endif
    
    //enable the address lines if required
#if defined (EPMPCS1_ADDRESS_LINE_COUNT) || defined (EPMPCS2_ADDRESS_LINE_COUNT)
    // decide how many address lines to use
    #if (EPMPCS1_ADDRESS_LINE_COUNT > EPMPCS2_ADDRESS_LINE_COUNT)
        addrCount = EPMPCS1_ADDRESS_LINE_COUNT;
    #else
        addrCount = EPMPCS2_ADDRESS_LINE_COUNT;
    #endif
            
    for(addrEnable = 1, i = 0; i < addrCount; i++)
    {
        if (i <= 15)
            PMCON4 |= addrEnable;
        else
            PMCON3 |= addrEnable;
            
        // change the addrEnable back to 1 when it reaches 16 count
        if (addrEnable == 0x8000)
            addrEnable = 0x0001;
        else
            addrEnable = addrEnable << 1;
    }
    #else    
    
        PMCON4 = 0;                 // PMA0 - PMA15 address lines are disabled
        PMCON3 |= 0;                // PMA16 - PMA17 address line is disabled
        
    #endif    

#if defined (EPMPCS1_DATA_SETUP_TIME)

#if defined (__PIC24FJ256DA210__)	
    PMCS1BS = (GFX_EPMP_CS1_BASE_ADDRESS >> 8);
#else	  
    PMCS1BS = 0x0008; 	        // CS1 start address
    PMCS2BS = 0xff00;		// set CS1 end address and CS2 start address
#endif	    

    //PMCS1CFbits.CSDIS = 0;      // enable CS
    PMCS1CFbits.CSP = 0;        // CS active low
    //PMCS1CFbits.CSPTEN = 1;     // enable CS port
    PMCS1CFbits.BEP = 0;        // uint8_t enable active low
    PMCS1CFbits.WRSP = 0;       // write strobe active low
    PMCS1CFbits.RDSP = 0;       // read strobe active low
    PMCS1CFbits.SM = 0;         // read and write strobes on separate lines 
    
#ifdef USE_16BIT_PMP
    PMCS1CFbits.PTSZ = 2;       // data bus width is 16-bit
#else
    PMCS1CFbits.PTSZ = 0;       // data bus width is 8-bit
#endif
    
    PMCS1MDbits.ACKM = 0; // PMACK is not used

    DRV_GFX_PMPWriteTimingSet(true);

#endif // #if defined (EPMPCS1_DATA_SETUP_TIME)
    
#if defined (EPMPCS2_DATA_SETUP_TIME)

#if defined (__PIC24FJ256DA210__)	
    PMCS2BS = (GFX_EPMP_CS2_BASE_ADDRESS >> 8);
#endif    	

    PMCS2CFbits.CSDIS = 0;      // enable CS
    PMCS2CFbits.CSP = 0;        // CS active low
    PMCS2CFbits.CSPTEN = 1;     // enable CS port
    PMCS2CFbits.BEP = 0;        // uint8_t enable active low
    PMCS2CFbits.WRSP = 0;       // write strobe active low
    PMCS2CFbits.RDSP = 0;       // read strobe active low
    PMCS2CFbits.SM = 0;         // read and write strobes on separate lines 
    
#ifdef USE_16BIT_PMP
    PMCS2CFbits.PTSZ = 2;       // data bus width is 16-bit
#else
    PMCS2CFbits.PTSZ = 0;       // data bus width is 8-bit
#endif
    
    PMCS2MDbits.ACKM = 0;        // PMACK is not used

    DRV_GFX_PMPWriteTimingSet(true);

#endif // #if defined (EPMPCS2_DATA_SETUP_TIME)

    PMCON3bits.PTWREN = 1;      // enable write strobe port
    PMCON3bits.PTRDEN = 1;      // enable read strobe port
    PMCON3bits.PTBE0EN = 0;     // disable uint8_t enable port
    PMCON3bits.PTBE1EN = 0;     // disable uint8_t enable port
    PMCON3bits.AWAITM = 0;      // don't care
    PMCON3bits.AWAITE = 0;      // don't care
	
    PMCON1bits.PMPEN = 1;

    DisplayResetDisable();                         // release from reset

    // hard delay inserted here for devices that needs delays after reset.
    // Value will vary from device to device, please refer to the specific
    // device data sheet for details.
    __delay_us(20);
	
}

#endif // USE_GFX_EPMP
#endif //#ifndef _GFX_EPMP_H_FILE



