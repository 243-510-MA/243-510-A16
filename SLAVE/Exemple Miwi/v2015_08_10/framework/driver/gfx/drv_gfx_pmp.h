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

#ifndef _DRV_GFX_PMP_H_FILE
#define _DRV_GFX_PMP_H_FILE

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include "system.h"

#ifdef USE_GFX_PMP

// Note:
/*
    All functions here are defined as inline functions for performance.
    When debugging this portion it is best to look at the assembly output 
    and debug from there.
 */

// error checking
#ifndef PMP_DATA_SETUP_TIME
#error "Define PMP_DATA_SETUP_TIME in system_config.h for data set up before read/write strobe timing"
#endif    
#ifndef PMP_DATA_WAIT_TIME
#error "Define PMP_DATA_WAIT_TIME in system_config.h for read/write strobe wait states"
#endif    
#ifndef PMP_DATA_HOLD_TIME
#error "Define PMP_DATA_HOLD_TIME in system_config.h for data hold after read/write strobe"
#endif    

#ifdef __PIC32MX__
#define PMDIN1              PMDIN
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
#define DRV_GFX_PMPWaitUntilNotBusy()   while(PMMODEbits.BUSY); 

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
    PMDIN1 = data;
    DRV_GFX_PMPWaitUntilNotBusy();
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
    value = PMDIN1;

    DRV_GFX_PMPWaitUntilNotBusy();
    PMCONbits.PMPEN = 0; // disable PMP
    value = PMDIN1;

#if defined (USE_16BIT_PMP)
    DRV_GFX_PMPWaitUntilNotBusy();
#endif

    PMCONbits.PMPEN = 1; // enable  PMP
    return value;

}

// *****************************************************************************
/*  Function:
    DRV_GFX_PMP_INTERFACE_DATA DRV_GFX_PMPSingleRead(void)

    Summary:
        This function reads data from PMP.

    Description:
        This function reads data from PMP without the enabling and disabling
        the chip select of the port. This is useful when doing successive
        reads from the port. For example, reading register contents that
        are more than one byte from a given address or index.

        The size of the data read is dependent on the mode of the module.
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
extern inline uint16_t __attribute__((always_inline)) DRV_GFX_PMPSingleRead()
{
    DRV_GFX_PMP_INTERFACE_DATA value;
    value = PMDIN1;
    DRV_GFX_PMPWaitUntilNotBusy();
    return value;
}

// *****************************************************************************
/*  Function:
    uint16_t DRV_GFX_PMP16BitRead(void)

    Summary:
        This function reads 16-bit data from PMP.

    Description:
        This function reads 16-bit data from PMP independent of the size
        of the interface mode used in the port. When the port is configured
        as an 8-bit port, the reading will perform two byte reads. When
        the port is configured as a 16-bit port, then the reading will
        perform only one transaction.

    Precondition:
        None.

    Parameters:
        None.

    Returns:
        The data read from the port. The width of data is always 16-bit.

    Example:
        None.

*/
// *****************************************************************************
extern inline uint16_t __attribute__ ((always_inline)) DRV_GFX_PMP16BitRead()
{

    uint16_t value;
#if defined (USE_8BIT_PMP)
    uint8_t temp;
#endif

    value = PMDIN1;
    DRV_GFX_PMPWaitUntilNotBusy();

#if defined (USE_8BIT_PMP)
    value = value << 8;
#endif
    DRV_GFX_PMPWaitUntilNotBusy();
#if defined (USE_8BIT_PMP)
    temp = PMDIN1;
    value = value & temp;
    DRV_GFX_PMPWaitUntilNotBusy();
#endif
    return value;
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
    // SYS_CLK_FrequencyPeripheralGet() is in Hz. pClockPeriod will be in nanoseconds.
    uint32_t pClockPeriod = (1000000000ul) / SYS_CLK_FrequencyPeripheralGet();

    if (write == true)
    {
        setup = PMP_DATA_SETUP_TIME;
        wait  = PMP_DATA_WAIT_TIME;
        hold  = PMP_DATA_HOLD_TIME;
    }
    else
    {
#ifdef PMP_READ_DATA_SETUP_TIME
        setup = PMP_READ_DATA_SETUP_TIME;
        wait  = PMP_READ_DATA_WAIT_TIME;
        hold  = PMP_READ_DATA_HOLD_TIME;
#else
        setup = PMP_DATA_SETUP_TIME;
        wait  = PMP_DATA_WAIT_TIME;
        hold  = PMP_DATA_HOLD_TIME;
#endif

    }

    if (setup == 0)
        PMMODEbits.WAITB = 0;
    else if (setup <= pClockPeriod)
        PMMODEbits.WAITB = 0;
    else if (setup > pClockPeriod)
            PMMODEbits.WAITB = (setup / pClockPeriod) + 1;

    if (wait == 0)
        PMMODEbits.WAITM = 0;
    else if (wait <= pClockPeriod)
        PMMODEbits.WAITM = 1;
    else if (wait > pClockPeriod)
        PMMODEbits.WAITM = (wait / pClockPeriod) + 1;

    if (hold == 0)
        PMMODEbits.WAITE = 0;
    else if (hold <= pClockPeriod)
        PMMODEbits.WAITE = 0;
    else if (hold > pClockPeriod)
        PMMODEbits.WAITE = (hold / pClockPeriod) + 1;

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

    DisplayResetEnable();       // hold in reset by default
    DisplayResetConfig();       // enable RESET line
    DisplayCmdDataConfig();     // enable RS line
    DisplayDisable();           // not selected by default
    DisplayConfig();            // enable chip select line

    // PMP setup
    PMMODE = 0;
    PMAEN = 0;
    PMCON = 0;
    PMMODEbits.MODE = 2; // Intel 80 master interface

    DRV_GFX_PMPWriteTimingSet(true);

#if defined(USE_16BIT_PMP)
    PMMODEbits.MODE16 = 1;      // 16 bit mode
#elif defined(USE_8BIT_PMP)
    PMMODEbits.MODE16 = 0;      // 8 bit mode
#endif

    PMCONbits.PTRDEN = 1;       // enable RD line
    PMCONbits.PTWREN = 1;       // enable WR line
    PMCONbits.PMPEN = 1;        // enable PMP

    DisplayResetDisable(); // release from reset

    // hard delay inserted here for devices that needs delays after reset.
    // Value will vary from device to device, please refer to the specific
    // device data sheet for details.
    __delay_us(200);
}
#endif //#ifdef USE_GFX_PMP
#endif //#ifndef _DRV_GFX_PMP_H_FILE
