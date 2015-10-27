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

#if !defined _SYS_MODULE_H
#define _SYS_MODULE_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include "assert.h"
#include "crypto_sw/src/sys_common.h"


#ifdef __cplusplus
    extern "C" {
#endif


// *****************************************************************************
/* System Module Index

  Summary:
   Identifies which instance of a system module should be initialized or opened

  Description:
    This data type identifies to which instance of a system module a call to
    that module's "Initialize" and "Open" routines refers.

  Remarks:
    Each individual module will usually define macro names for the index values
    it supports (ex. DRV_TMR_INDEX_1, DRV_TMR_INDEX_2, ...).
*/

typedef unsigned short int SYS_MODULE_INDEX;


// *****************************************************************************
/* System Module Object

  Summary:
   Handle to an instance of a system module

  Description:
    This data type is a handle to a specific instance of a system module (such
    as a device driver).

  Remarks:
    Code outside of a specific module should consider this as an opaque type
    (much like a void *).  Do not make any assumptions about base type as it
    may change in the future or about the value stored in a variable of this
    type.
*/

typedef unsigned short int SYS_MODULE_OBJ;


// *****************************************************************************
/* System Module Object Invalid

  Summary:
    Object handle value returned if unable to initialize the requested instance
    of a system module

  Description:
    This is the object handle value returned if unable to initialize the
    requested instance of a system module.

  Remarks:
    Do not rely on the actual value of this constant.  It may change in future
    implementations.
*/

#define SYS_MODULE_OBJ_INVALID      ((SYS_MODULE_OBJ) -1 )


// *****************************************************************************
/* System Module Object Static

  Summary:
    Object handle value returned by static modules

  Description:
    This is the object handle value returned by static system modules.

  Remarks:
    Do not rely on the actual value of this constant.  It may change in future
    implementations.
*/

#define SYS_MODULE_OBJ_STATIC       ((SYS_MODULE_OBJ) 0 )


// *****************************************************************************
/* System Module Status

  Summary:
    Identifies the current status/state of a system module (including device
    drivers)

  Description:
    This enumeration identifies the current status/state of a system module
    (including device drivers).

  Remarks:
    This enumeration is the return type for the system-level status routine
    defined by each device driver or system module (for example:
    DRV_I2C_Status).
*/

typedef enum
{
    // Indicates that a non-system defined error has occurred.  The caller
    // must call the extended status routine for the module in question to
    // identify the error.
    SYS_STATUS_ERROR_EXTENDED   = -10,

    // An un-specified error has occurred.
    SYS_STATUS_ERROR            = -1,

    // The module has not yet been initialized
    SYS_STATUS_UNINITIALIZED    = 0,

    // An operation is currently in progress
    SYS_STATUS_BUSY             = 1,

    // Any previous operations have succeeded and the module is ready for
    // additional operations
    SYS_STATUS_READY            = 2,

    // Indicates that the module is in a non-system defined ready/run state.
    // The caller must call the extended status routine for the module in
    // question to identify the state.
    SYS_STATUS_READY_EXTENDED   = 10

} SYS_STATUS;


// *****************************************************************************
/* System Module Power Off State

  Summary:
    Module power-state power off state code

  Description:
    This value identifies the current power status/state of a system module
    (including device drivers).  It is used to indicate that the module should
    prepare to enter a full power-off state.

  Remarks:
    A power off state indicates that power may be completely removed (0 Volts).

    This value is passed in the powerState field of the SYS_MODULE_INIT
    structure that takes part in all modules initialization and re-
    initialization.

    The power state codes between SYS_MODULE_POWER_IDLE_RUN (with a value of 3)
    and SYS_MODULE_POWER_RUN_FULL (with a value of 15) are available for module-
    specific definition and usage.
*/

#define SYS_MODULE_POWER_OFF        0


// *****************************************************************************
/* System Module Power Sleep State

  Summary:
    Module power-state sleep state code

  Description:
    This value identifies the current power status/state of a system module
    (including device drivers).  It is used to indicate that the module should
    prepare to enter a sleep state.

  Remarks:
    A sleep state indicates that the core CPU and peripheral clocks may be
    stopped and no code will execute and any module hardware will be stopped.

    This value is passed in the powerState field of the SYS_MODULE_INIT
    structure that takes part in all modules initialization and re-
    initialization.

    The power state codes between SYS_MODULE_POWER_IDLE_RUN (with a value of 3)
    and SYS_MODULE_POWER_RUN_FULL (with a value of 15) are available for module-
    specific definition and usage.
*/

#define SYS_MODULE_POWER_SLEEP      1


// *****************************************************************************
/* System Module Power Idle-Stop State

  Summary:
    Module power-state idle-stop state code

  Description:
    This value identifies the current power status/state of a system module
    (including device drivers).  It is used to indicate that the module should
    prepare to enter an idle-stop state.

  Remarks:
    An idle-stop state indicates that the core CPU clock may be stopped, but the
    module's peripheral clock may continue running.  However, the peripheral
    should prepare to stop operations when the idle state is entered.

    This value is passed in the powerState field of the SYS_MODULE_INIT
    structure that takes part in all modules initialization and re-
    initialization.

    The power state codes between SYS_MODULE_POWER_IDLE_RUN (with a value of 3)
    and SYS_MODULE_POWER_RUN_FULL (with a value of 15) are available for module-
    specific definition and usage.
*/

#define SYS_MODULE_POWER_IDLE_STOP  2


// *****************************************************************************
/* System Module Power Idle-Run State

  Summary:
    Module power-state idle-run state code

  Description:
    This value identifies the current power status/state of a system module
    (including device drivers).  It is used to indicate that the module should
    prepare to enter an idle-run state.

  Remarks:
    An idle-run state indicates that the core CPU clock may be stopped, but the
    module's peripheral clock may continue running and peripheral operations may
    continue as long as no code needs to be executed.  If code needs to execute,
    the module must cause an interrupt.

    This value is passed in the powerState field of the SYS_MODULE_INIT
    structure that takes part in all modules initialization and re-
    initialization.

    The power state codes between SYS_MODULE_POWER_IDLE_RUN (with a value of 3)
    and SYS_MODULE_POWER_RUN_FULL (with a value of 15) are available for module-
    specific definition and usage.
*/

#define SYS_MODULE_POWER_IDLE_RUN   3


// *****************************************************************************
/* System Module Power Run-Full State

  Summary:
    Module power-state run-full state code

  Description:
    This value identifies the current power status/state of a system module
    (including device drivers).  It is used to indicate that the module should
    prepare to enter an run-full state.

  Remarks:
    An run-full state indicates that the core CPU and pereipheral clocks are
    operational at their normal configured speed and the module should be
    ready for normal operation.

    This value is passed in the powerState field of the SYS_MODULE_INIT
    structure that takes part in all modules initialization and re-
    initialization.

    The power state codes between SYS_MODULE_POWER_IDLE_RUN (with a value of 3)
    and SYS_MODULE_POWER_RUN_FULL (with a value of 15) are available for module-
    specific definition and usage.
*/

#define SYS_MODULE_POWER_RUN_FULL   15


// *****************************************************************************
/* System Module Init

  Summary:
    Initializes a module (including device drivers) in a current power status as
    requested by the system or power manager.

  Description:
    This structure provides the necessary data to initialize or re-initialize
    a module (including device drivers) into a requested power state.
    The structure can be extended in a module specific way as to carry
    module specific initialization data.

  Remarks:
    This structure is used in the device driver routines DRV_<module>_Initialize
    and DRV_<module>_Reinitialize that are defined by each device driver.

    The "powerState" member has several pre-defined values (shown below).  All
    other available values (within the 4-bit field) are available for module-
    specific meaning.

    Pre-defined powerState Values:

    0,  SYS_MODULE_POWER_OFF        - Module power-state power off state code

    1.  SYS_MODULE_POWER_SLEEP      - Module power-state sleep state code

    2.  SYS_MODULE_POWER_IDLE_STOP  - Module power-state idle-stop state code

    3.  SYS_MODULE_POWER_IDLE_RUN   - Module power-state idle-run state code

    4-14.                           - Module-specific meaning

    15. SYS_MODULE_POWER_RUN_FULL   - Module power-state run-full state code
*/

typedef union
{
    uint8_t         value;

    struct
    {
        // Requested power state
        uint8_t     powerState  : 4;

        // Module-definable field, module-specific usage
        uint8_t     reserved    : 4;
    }sys;

} SYS_MODULE_INIT;


// *****************************************************************************
// *****************************************************************************
// Section:  Pointers to System Module Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* System Module Initialization Routine Pointer

  Function:
    SYS_MODULE_OBJ (* SYS_MODULE_INITIALIZE_ROUTINE) (
                        const SYS_MODULE_INDEX index,
                        const SYS_MODULE_INIT * const init )

  Summary:
    Pointer to a routine that initializes a system module (driver, library, or
    system-maintained application)

  Description:
    This data type is a pointer to a routine that initializes a system module
    (driver, library, or system-maintained application).

  Precondition:
    The low-level board initialization must have (and will be) completed before
    the system will call the initialization routine for any modules.

  Parameters:
    index           - Identifier for the module instance to be initialized

    init            - Pointer to the data structure containing any data
                      necessary to initialize the module. This pointer may
                      be null if no data is required and default initialization
                      is to be used.

  Returns:
    A handle to the instance of the system module that was initialized.  This
    handle is a necessary parameter to all of the other system-module level
    routines for that module.

  Remarks:
    This routine will only be called once during system initialization.
*/

typedef SYS_MODULE_OBJ (* SYS_MODULE_INITIALIZE_ROUTINE) ( const SYS_MODULE_INDEX index,
                                                           const SYS_MODULE_INIT * const init );



// *****************************************************************************
/* System Module Re-initialization Routine Pointer

  Function:
    void (* SYS_MODULE_REINITIALIZE_ROUTINE) ( SYS_MODULE_OBJ object,
                const SYS_MODULE_INIT * const init)

  Summary:
    Pointer to a routine that re-initializes a system module (driver, library,
    or system-maintained application)

  Description:
    This data type is a pointer to a routine that re-initializes a system
    module (driver, library, or system-maintained application).

  Precondition:
    The low-level board initialization must have (and will be) completed and the
    module's initialization routine will have been called before the system will
    call the re-initialization routine for any modules.

  Parameters:
    object          - Handle to the module instance

    init            - Pointer to the data structure containing any data
                      necessary to initialize the module. This pointer may
                      be null if no data is required and default initialization
                      is to be used.

  Returns:
    None.

  Example:
    <code>
    TBD
    </code>

  Remarks:
    This operation uses the same initialization data structure as the Initialize
    operation.

    This operation can be used to change the power state of a module.

    This operation can also be used to refresh the hardware state as defined
    by the initialization data, thus it must guarantee that all hardware
    state has been refreshed.

    This function can be called multiple times to reinitialize the module.
*/

typedef void (* SYS_MODULE_REINITIALIZE_ROUTINE) ( SYS_MODULE_OBJ object,
                                                   const SYS_MODULE_INIT * const init );


// *****************************************************************************
/* System Module De-initialization Routine Pointer

  Function:
    void (* SYS_MODULE_DEINITIALIZE_ROUTINE) (  SYS_MODULE_OBJ object )

  Summary:
    Pointer to a routine that de-initializes a system module (driver, library,
    or system-maintained application)

  Description:
    This data type is a pointer to a routine that de-initializes a system
    module (driver, library, or system-maintained application).

  Precondition:
    The low-level board initialization must have (and will be) completed and the
    module's initialization routine will have been called before the system will
    call the de-initialization routine for any modules.

  Parameters:
    object          - Handle to the module instance

  Returns:
    None.

  Example:
    <code>
    TBD
    </code>

  Remarks:
    If the module instance has to be used again, the module's "initalize" must
    first be called.
 */

typedef void (* SYS_MODULE_DEINITIALIZE_ROUTINE) (  SYS_MODULE_OBJ object );


// *****************************************************************************
/* System Module Status Routine Pointer

  Function:
    SYS_STATUS (* SYS_MODULE_STATUS_ROUTINE) (  SYS_MODULE_OBJ object )

  Summary:
    Pointer to a routine that gets the current status of a system module
    (driver, library, or system-maintained application)

  Description:
    This data type is a pointer to a routine that gets the current status of a
    system module (driver, library, or system-maintained application).

  Precondition:
    The low-level board initialization must have (and will be) completed and the
    module's initialization routine will have been called before the system will
    call the status routine for any modules.

  Parameters:
    object          - Handle to the module instance

  Returns:
    One of the possible status codes from SYS_STATUS

  Example:
    <code>
    TBD
    </code>

  Remarks:
    A module's status operation can be used to determine when any of the
    other module level operations has completed as well as to obtain general
    status of the module.  The value returned by the status routine will be
    checked after calling any of the module operations to find out when they
    have completed.

    If the status operation returns SYS_STATUS_BUSY, the previous operation
    has not yet completed. Once the status operation returns SYS_STATUS_READY,
    any previous operations have completed.

    The value of SYS_STATUS_ERROR is negative (-1). A module may define
    module-specific error values of less or equal SYS_STATUS_ERROR_EXTENDED
    (-10).

    The status function must NEVER block.

    If the status operation returns an error value, the error may be cleared by
    calling the re-initialize operation. If that fails, the de-initialize
    operation will need to be called, followed by the initialize operation to
    return to normal operations.
*/

typedef SYS_STATUS (* SYS_MODULE_STATUS_ROUTINE) (  SYS_MODULE_OBJ object );


// *****************************************************************************
/* System Module Tasks Routine Pointer

  Function:
    void (* SYS_MODULE_TASKS_ROUTINE) ( SYS_MODULE_OBJ object )

  Summary:
    Pointer to a routine that performs the tasks necessary to maintain a state
    machine in a module system module (driver, library, or system-maintained
    application)

  Description:
    This data type is a pointer to a routine that performs the tasks necessary
    to maintain a state machine in a module system module (driver, library, or
    system-maintained application).

  Precondition:
    The low-level board initialization must have (and will be) completed and the
    module's initialization routine will have been called before the system will
    call the de-initialization routine for any modules.

  Parameters:
    object          - Handle to the module instance

  Returns:
    None.

  Example:
    <code>
    TBD
    </code>

  Remarks:
    If the module is interrupt driven, the system will call this routine from
    an interrupt context.
*/

typedef void (* SYS_MODULE_TASKS_ROUTINE) ( SYS_MODULE_OBJ object );


// *****************************************************************************
/* System Module Tasks data

  Summary:
    Structure contains pointers to module's tasks routine.

  Description:
    This structure contains pointers to module's tasks routine.
    A module can have an array of such structures depending on number of
    tasks routines it supports.
    The system will either directly/indirectly call the tasks routine depending on
    the value of intModule.

  Example:
    <code>
    TBD
    </code>

  Remarks:
    A pointer to this structure is passed to the SYS_ModuleRegister routine.
*/

typedef struct
{
    // Pointer to the module's "Tasks" routine
    SYS_MODULE_TASKS_ROUTINE            pTasksFunction;

    // Tasks type (polled/interrupt)
    bool                                intModule;

}SYS_MODULE_TASKS_DATA;


// *****************************************************************************
/* System Module Interface

  Summary:
    Structure contains pointers to module-interface routines for a single system
    module.

  Description:
    This structure contains pointers to module-interface routines for a single
    system module.  The system will initialize these modules (using their
    respective initialization routines) and (potentially) monitor and
    maintain their states using the re-initialize, de-initialize, and status
    routines.

  Example:
    <code>
    TBD
    </code>

  Remarks:
    A pointer to this structure is passed to the SYS_ModuleRegister routine.
*/

typedef struct
{
    // Pointer to the module's initialization routine
    SYS_MODULE_INITIALIZE_ROUTINE       pModuleInitialize;

    // Pointer to the module's re-initializtion routine
    SYS_MODULE_REINITIALIZE_ROUTINE     pModuleReinitialize;

    // Pointer to the module's de-initialization routine
    SYS_MODULE_DEINITIALIZE_ROUTINE     pModuleDeinitialize;

    // Pointer to the module's status routine
    SYS_MODULE_STATUS_ROUTINE           pModuleStatus;

    // Pointer to the module's "Tasks" routine
    SYS_MODULE_TASKS_DATA               *pTasksData;

    // Pointer to the module's init data table
    SYS_MODULE_INIT                    *pModuleInitData;

    // Index to the module instance
    SYS_MODULE_INDEX                    index;

    // Number of tasks supported by this module
    unsigned int                        tasksNum;

} SYS_MODULE_INTERFACE;


// *****************************************************************************
/* System Module Data

  Summary:
    Contains module-interface info used to define a table of system modules

  Description:
    This structure contains module-interface info used to define a table of
    system modules.

  Remarks:
    Used internally by the system services.  Not to be accessed by user code.
*/

typedef struct
{
    // Module interface (function pointers, index, etc)
    SYS_MODULE_INTERFACE    interface;

    // Object handle obtained from initialization call
    SYS_MODULE_OBJ          object;

    // Requested module priority (identifies "Tasks" interval)
    SYS_TASKS_PRIORITY      priority;

    // Number of intervals (of the module priority) before calling this
    // module's "Tasks" routine.
    unsigned int            count;

} SYS_MODULE_DATA;


#ifdef __cplusplus
    }
#endif


#endif      // _SYS_MODULE_H
