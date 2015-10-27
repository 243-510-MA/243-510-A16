/******************************************************************************
 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the "Company") for its PICmicro(r) Microcontroller is intended and
 supplied to you, the Company's customer, for use solely and
 exclusively on Microchip PICmicro Microcontroller products. The
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
********************************************************************/

#if !defined (_SYS_COMMON_H)
#define _SYS_COMMON_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files 
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include "assert.h"


#ifdef __cplusplus
    extern "C" {
#endif


/*******************************************************************************
  Function:
    void SYS_ASSERT ( bool test, char *message )

  Summary:
    Implements default system assert routine, asserts that "test" is true

  Description:
    This macro implements the default system assert routine that asserts that a 
    test is true.

  Precondition:
    None.
  
  Parameters:
    test     - This is an expression that resolves to a boolean value 
               (zero=false, non-zero=true)
               
    message  - This is a NULL-terminated character string that can be displayed
               on a debug output terminal if "test" is false (if supported)
  
  Returns:
    None     - Normaly hangs in a loop

  Example:
    <code>
    void MyFunc ( int *pointer )
    {
        SYS_ASSERT(NULL != pointer, "NULL Pointer passed ty MyFunc");
        
        //...
    }
    </code>

  Remarks:
    Can be overridden as desired by defining your own SYS_ASSERT macro before 
    including sys.h.
*/

#ifndef SYS_ASSERT

    #define SYS_ASSERT(test,message)    assert(test)

#endif


// *****************************************************************************
/* Main Routine Return Type

  Summary:
    Defines the correct return type for the "main" routine

  Description:
    This macro defines the correct return type for the "main" routine for the 
    selected Microchip microcontroller family.

  Example:
    <code>
    MAIN_RETURN main ( void )
    {
        // Initialize the system
        SYS_Initialize(...);
    
        // Main Loop
        while(true)
        {
            SYS_Tasks();
        }
    
        return MAIN_RETURN_CODE(MAIN_RETURN_SUCCESS);
    }
    </code>
    
  Remarks:
    This type changes depending upon which family of Microchip microcontrollers 
    is chosen. Most Microchip microcontrollers do not return any value from 
    "main".
*/

#if defined(__PIC32MX__) || defined(__C30__)

    #define MAIN_RETURN int

#else

    #define MAIN_RETURN
    
#endif


// *****************************************************************************
/* Main Routine Code Macro

  Summary:
    Provides the correct value for the return code from "main"

  Description:
    This macro provides the correct value for the return code from "main".

  Example:
    <code>
    MAIN_RETURN main ( void )
    {
        // Initialize the system
        SYS_Initialize(...);
    
        // Main Loop
        while(true)
        {
            SYS_Tasks();
        }
    
        return MAIN_RETURN_CODE(MAIN_RETURN_SUCCESS);
    }
    </code>
    
  Remarks:
    Most Microchip microcontrollers do not provide a return value from "main".  
    So, this macro "throws away" the code it is given unless it is needed.
*/

#if defined(__PIC32MX__) || defined(__C30__)

    #define MAIN_RETURN_CODE(c)     ((MAIN_RETURN)(c))

#else

    #define MAIN_RETURN_CODE(c)
    
#endif


// *****************************************************************************
/* Main Routine Codes

  Summary:
    Pre-defined list of return codes for "main"
    
  Description:
    This enumeration provides a pre-defined list of return codes for "main".

  Example:
    <code>
    MAIN_RETURN main ( void )
    {
        // Initialize the system
        SYS_Initialize(...);
    
        // Main Loop
        while(true)
        {
            SYS_Tasks();
        }
    
        return MAIN_RETURN_CODE(MAIN_RETURN_SUCCESS);
    }
    </code>
    
  Remarks:
    These codes can be passed into the MAIN_RETURN_CODE() macro to convert them
    to the appropriate type (or "throw them away" if not needed) for the 
    Microchip microcontroller in use.
*/

typedef enum
{
    MAIN_RETURN_FAILURE     = -1,

    MAIN_RETURN_SUCCESS     = 0

} MAIN_RETURN_CODES;


// *****************************************************************************
/* System Tasks Priority

  Summary:
    Defines system tasks priorities

  Description:
    This enumeration defines the available system tasks priorities
    
  Remarks
    To use medium priority tasks, a medium priority interval must be defined
    by defining SYS_TASKS_CONFIG_MEDIUM_INTERVAL to the desired interval (in 
    milliseconds).
    
    To use low priority tasks, a low priority interval must be defined
    by defining SYS_TASKS_CONFIG_LOW_INTERVAL to the desired interval (in 
    milliseconds).
*/

typedef enum _SYS_TASKS_PRIORITIES
{
    // Invalid priority (can be used as a sentinal value)
    SYS_TASKS_PRIORITY_INVALID = 0,

    // High priority tasks are called every time through the loop
    SYS_TASKS_PRIORITY_HIGH,

    // Called at the medium priority interval.
    SYS_TASKS_PRIORITY_MEDIUM,

    // Called at the low priority interval. 
    SYS_TASKS_PRIORITY_LOW

} SYS_TASKS_PRIORITY;


// *****************************************************************************
/* uintptr_t

  Summary:
    Temporary definition of uintptr_t

  Description:
    This is a temporary definition of uintptr_t to work around compilers that
    don't support it.  The uintptr_t type is the smallest integer that will 
    hold a pointer value.

  Remarks:
    This definition will be removed once all compilers support uintptr_t in 
    stdint.h.
*/

#ifndef uintptr_t
    typedef void * uintptr_t;
#endif


#ifdef __cplusplus
    }
#endif

#endif      // _SYS_COMMON_H

