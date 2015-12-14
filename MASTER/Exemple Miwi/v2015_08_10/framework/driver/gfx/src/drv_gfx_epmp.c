/*******************************************************************************
 Module for Microchip Graphics Library - Enhanced Parallel Master Port (EPMP) Driver 

  Company:
    Microchip Technology Inc.

  File Name:
    gfxempm.c

  Summary:
    Driver for use with the Microchip Graphics Library.

  Description:
    This module maps the memory space located in Extended Data Space (EDS).       
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

#include "system.h"
#include <stdint.h>

#ifdef USE_GFX_EPMP

// this is just a dummy address to set up EDS access on epmp
#if defined (GFX_EPMP_CS1_BASE_ADDRESS)
    // we are adding a -2 here so we do not overlap the base address of CS1
    // if CS1 base address is used. Since this address is dummy we do not 
    // care the value. We just want the EDS access to be set up.
    #define EPMP_REGION_START_ADDRESS (GFX_EPMP_CS1_BASE_ADDRESS-2)
#else    
    #define EPMP_REGION_START_ADDRESS (0x00020000ul)
#endif

#ifdef USE_16BIT_PMP
volatile __eds__ uint16_t __attribute__((eds,address(EPMP_REGION_START_ADDRESS),noload,section("epmp_data_for_CPU_access"))) epmp_data;

#else
volatile __eds__ uint8_t __attribute__((eds,address(EPMP_REGION_START_ADDRESS),noload,section("epmp_data_for_CPU_access"))) epmp_data;
#endif

#endif // USE_GFX_EPMP
