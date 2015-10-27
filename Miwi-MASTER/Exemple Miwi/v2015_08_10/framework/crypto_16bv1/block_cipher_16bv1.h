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

#if !defined (_BLOCK_CIPHER_16BV1_H)
#define _BLOCK_CIPHER_16BV1_H

#include <stdint.h>
#include <stdbool.h>
#include "crypto_16bv1/src/drv_common.h"
#include "crypto_16bv1/src/sys_common.h"
#include "crypto_16bv1/src/sys_module.h"
#include "system_config.h"

#ifndef CRYPTO_CONFIG_16BV1_BLOCK_MAX_SIZE
    #define CRYPTO_CONFIG_16BV1_BLOCK_MAX_SIZE        16ul
#endif

// Definition for a single drive index for the hardware AES/TDES module
#define BLOCK_CIPHER_INDEX_0 0
// Map of the default drive index to drive index 0
#define BLOCK_CIPHER_INDEX BLOCK_CIPHER_INDEX_0
// Number of drive indicies for this module
#define BLOCK_CIPHER_INDEX_COUNT 1

// Enumeration defining different key types
typedef enum
{
    // Key is unspecified
    CRYPTO_KEY_NONE = 0,

    // Key is specified in software
    CRYPTO_KEY_SOFTWARE,

    // Expanded key is specified in software
    CRYPTO_KEY_SOFTWARE_EXPANDED,

    // Key-encrypted key
    CRYPTO_KEY_HARDWARE_KEK,

    // Key is stored in OTP memory
    CRYPTO_KEY_HARDWARE_OTP_OFFSET,
    CRYPTO_KEY_HARDWARE_OTP_1, 
    CRYPTO_KEY_HARDWARE_OTP_2,
    CRYPTO_KEY_HARDWARE_OTP_3,
    CRYPTO_KEY_HARDWARE_OTP_4,
    CRYPTO_KEY_HARDWARE_OTP_5,
    CRYPTO_KEY_HARDWARE_OTP_6,
    CRYPTO_KEY_HARDWARE_OTP_7
} CRYPTO_KEY_TYPE;

typedef enum
{
    CRYPTO_MODE_NONE = 0,                    // Key mode is unspecified
    CRYPTO_64DES_1_KEY,
    CRYPTO_64DES_2_KEY,
    CRYPTO_64DES_3_KEY,
    CRYPTO_AES_128_KEY,
    CRYPTO_AES_192_KEY,
    CRYPTO_AES_256_KEY
} CRYPTO_KEY_MODE;

typedef enum
{
    BLOCK_CIPHER_STATE_CLOSED = 0,
    BLOCK_CIPHER_STATE_OPEN,
    BLOCK_CIPHER_STATE_IDLE,
    BLOCK_CIPHER_STATE_ERROR,
    BLOCK_CIPHER_STATE_BUSY,
} BLOCK_CIPHER_STATE;

// Block cipher handle type
typedef unsigned short int BLOCK_CIPHER_HANDLE;

#define BLOCK_CIPHER_HANDLE_INVALID     (-1)

#define CRYPTO_16BV1_ALGORITHM_AES      ((BLOCK_CIPHER_FunctionEncrypt) 0)
#define CRYPTO_16BV1_ALGORITHM_TDES     ((BLOCK_CIPHER_FunctionEncrypt) 1)

// Enumeration defining potential errors the can occur when using a block cipher mode 
// of operation.  Modes that do not use keystreams will not generate errors.
typedef enum
{
    /* No errors. */
    BLOCK_CIPHER_ERROR_NONE = (0x00000000u),

    /* The calling function has requested that more bits be added to the
       key stream then are available in the buffer allotted for the key stream.
       Since there was not enough room to complete the request, the request
       was not processed. */
    BLOCK_CIPHER_ERROR_KEY_STREAM_GEN_OUT_OF_SPACE,

    /* The requesting call has caused the counter number to run out of unique
       combinations.  In CTR mode it is not safe to use the same counter
       value for a given key.  */
    BLOCK_CIPHER_ERROR_CTR_COUNTER_EXPIRED,

    /* Authentication of the specified data failed.  */
    BLOCK_CIPHER_ERROR_INVALID_AUTHENTICATION,

    /* The specified key type (format) is unsupported by the crypto implementation
       that is being used.  */
    BLOCK_CIPHER_ERROR_UNSUPPORTED_KEY_TYPE,

    /* The user specified an invalid handle */
    BLOCK_CIPHER_ERROR_INVALID_HANDLE,

    /* The user specified an invalid function pointer */
    BLOCK_CIPHER_ERROR_INVALID_FUNCTION,

    /* The specified block cipher module is busy and cannot start a new operation */
    BLOCK_CIPHER_ERROR_BUSY,

    /* The specified block cipher module is has already started a stream on this handle */
    BLOCK_CIPHER_ERROR_STREAM_START,

    /* The User has aborted the last operation on this handle */
    BLOCK_CIPHER_ERROR_ABORT,

    /* The Hardware settings either key, mode, or otp program configuration is invalid */
    BLOCK_CIPHER_ERROR_HW_SETTING
} BLOCK_CIPHER_ERRORS;


// This option is used to pass data that will be authenticated but not encrypted into an authenticating block cipher mode function.
#define BLOCK_CIPHER_OPTION_AUTHENTICATE_ONLY           /*DOM-IGNORE-BEGIN*/ (0x00002000u)    /*DOM-IGNORE-END*/

// The stream is still in progress.
#define BLOCK_CIPHER_OPTION_STREAM_CONTINUE             /*DOM-IGNORE-BEGIN*/ (0x00000000u)    /*DOM-IGNORE-END*/

// The stream is complete.  Padding will be applied if required.
#define BLOCK_CIPHER_OPTION_STREAM_COMPLETE             /*DOM-IGNORE-BEGIN*/ (0x00004000u)    /*DOM-IGNORE-END*/

// This should be passed when a new stream is starting
#define BLOCK_CIPHER_OPTION_STREAM_START                /*DOM-IGNORE-BEGIN*/ (0x00008000u)    /*DOM-IGNORE-END*/

// The cipher text pointer is pointing to data that is aligned to the target machine's word size (16-bit aligned for PIC24/dsPIC30/dsPIC33, and 8-bit aligned for PIC18).  Enabling this feature may improve throughput.
#define BLOCK_CIPHER_OPTION_CIPHER_TEXT_POINTER_ALIGNED /*DOM-IGNORE-BEGIN*/ (0x00000040u)    /*DOM-IGNORE-END*/

// The plain text pointer is pointing to data that is aligned to the target machine's word size (16-bit aligned for PIC24/dsPIC30/dsPIC33, and 8-bit aligned for PIC18).  Enabling this feature may improve throughput.
#define BLOCK_CIPHER_OPTION_PLAIN_TEXT_POINTER_ALIGNED  /*DOM-IGNORE-BEGIN*/ (0x00000080u)    /*DOM-IGNORE-END*/

// Pad with whatever data is already in the RAM.  This flag is normally set only for the last block of data.
#define BLOCK_CIPHER_OPTION_PAD_NONE                    /*DOM-IGNORE-BEGIN*/ (0x00000000u)    /*DOM-IGNORE-END*/

// Pad with 0x00 bytes if the current and previous data lengths do not end on a block boundary (multiple of 16 bytes).  This flag is normally set only for the last block of data.
#define BLOCK_CIPHER_OPTION_PAD_NULLS                   /*DOM-IGNORE-BEGIN*/ (0x00000100u)    /*DOM-IGNORE-END*/

// Pad with 0x80 followed by 0x00 bytes (a 1 bit followed by several 0 bits) if the current and previous data lengths do not end on a block boundary (multiple of 16 bytes).  This flag is normally set only for the last block of data.
#define BLOCK_CIPHER_OPTION_PAD_8000                    /*DOM-IGNORE-BEGIN*/ (0x00000200u)    /*DOM-IGNORE-END*/

// Pad with three 0x03's, four 0x04's, five 0x05's, six 0x06's, etc. set by the number of padding bytes needed if the current and previous data lengths do not end on a block boundary (multiple of 16 bytes).  This flag is normally set only for the last block of data.
#define BLOCK_CIPHER_OPTION_PAD_NUMBER                  /*DOM-IGNORE-BEGIN*/ (0x00000400u)    /*DOM-IGNORE-END*/

// Mask to determine the padding option that is selected.
#define BLOCK_CIPHER_OPTION_PAD_MASK                    /*DOM-IGNORE-BEGIN*/ (0x00000700u)    /*DOM-IGNORE-END*/

// Mask to determine the size of the counter in bytes.
#define BLOCK_CIPHER_OPTION_CTR_SIZE_MASK               /*DOM-IGNORE-BEGIN*/ (0x0000000Fu)    /*DOM-IGNORE-END*/

// Treat the counter as a 32-bit counter.  Leave the remaining section of the counter unchanged
#define BLOCK_CIPHER_OPTION_CTR_32BIT                   /*DOM-IGNORE-BEGIN*/ (0x00000004u)    /*DOM-IGNORE-END*/

// Treat the counter as a 64-bit counter.  Leave the remaining section of the counter unchanged
#define BLOCK_CIPHER_OPTION_CTR_64BIT                   /*DOM-IGNORE-BEGIN*/ (0x00000008u)    /*DOM-IGNORE-END*/

// Treat the counter as a full 128-bit counter.  This is the default option.
#define BLOCK_CIPHER_OPTION_CTR_128BIT                  /*DOM-IGNORE-BEGIN*/ (0x00000000u)    /*DOM-IGNORE-END*/

// A definition to specify the default set of options.
#define BLOCK_CIPHER_OPTION_OPTIONS_DEFAULT             /*DOM-IGNORE-BEGIN*/ (0x00000000u)    /*DOM-IGNORE-END*/

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ BLOCK_CIPHER_16BV1_Initialize(const SYS_MODULE_INDEX index,
            const SYS_MODULE_INIT * const init)

  Summary:
    Initializes the data for the instance of the block cipher module.

  Description:
    This routine initializes data for the instance of the block cipher module.

  Precondition:
    None

  Parameters:
    index       - Identifier for the instance to be initialized

    init        - Pointer to the data structure containing any data
                  necessary to initialize the hardware. This pointer may
                  be null if no data is required and default
                  initialization is to be used

  Returns:
    If successful, returns a valid handle to a driver instance object.
    Otherwise, it returns SYS_MODULE_OBJ_INVALID

  Example:
    <code>
    SYS_MODULE_OBJ sysObject;

    sysObject = BLOCK_CIPHER_16BV1_Initialize (BLOCK_CIPHER_INDEX, NULL);
    if (sysObject != SYS_MODULE_OBJ_STATIC)
    {
        // error
    }
    </code>
*/
SYS_MODULE_OBJ BLOCK_CIPHER_16BV1_Initialize (const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);

// *****************************************************************************
/* Function:
    BLOCK_CIPHER_HANDLE BLOCK_CIPHER_16BV1_Open(const SYS_MODULE_INDEX index,
        const DRV_IO_INTENT ioIntent)

  Summary:
    Opens a new client for the device instance.

  Description:
    Returns a handle of the opened client instance. All client operation APIs
    will require this handle as an argument.

  Precondition:
    The driver must have been previously initialized and in the
    initialized state.

  Parameters:
    index           - Identifier for the instance to opened
    ioIntent        - Possible values from the enumeration DRV_IO_INTENT
                      There are currently no applicable values for this module.

  Returns:
    Returns a handle of the opened client instance. Otherwise, it returns
	BLOCK_CIPHER_HANDLE_INVALID

  Example:
    <code>
    SYS_MODULE_OBJ sysObject;
    BLOCK_CIPHER_HANDLE handle;

    sysObject = BLOCK_CIPHER_16BV1_Initialize (BLOCK_CIPHER_INDEX, NULL);
    if (sysObject != SYS_MODULE_OBJ_STATIC)
    {
        // error
    }

    handle = BLOCK_CIPHER_16BV1_Open (BLOCK_CIPHER_INDEX, 0);
    if (handle == BLOCK_CIPHER_HANDLE_INVALID)
    {
        // error
    }
    </code>
*/
BLOCK_CIPHER_HANDLE BLOCK_CIPHER_16BV1_Open(const SYS_MODULE_INDEX index, const DRV_IO_INTENT ioIntent);

// *****************************************************************************
/* Function:
    void BLOCK_CIPHER_16BV1_Close (BLOCK_CIPHER_HANDLE handle)

  Summary:
    Closes an opened client

  Description:
    Closes an opened client, resets the data structure and removes the client
    from the driver.

  Precondition:
    None.

  Parameters:
    handle          - The handle of the opened client instance returned by
                      BLOCK_CIPHER_16BV1_Open().

  Returns:
    None

  Example:
    <code>
    handle = BLOCK_CIPHER_16BV1_Open (BLOCK_CIPHER_INDEX, 0);
    if (handle == BLOCK_CIPHER_HANDLE_INVALID)
    {
        // error
    }

    BLOCK_CIPHER_16BV1_Close (handle);
    </code>
*/
void BLOCK_CIPHER_16BV1_Close (BLOCK_CIPHER_HANDLE handle);

// *****************************************************************************
/* Function:
    void BLOCK_CIPHER_16BV1_Deinitialize(SYS_MODULE_OBJ object)

  Summary:
    Deinitializes the instance of the block cipher module

  Description:
    Deinitializes the specific module instance disabling its operation.

  Precondition:
    None

  Parameters:
    object           - Identifier for the instance to be de-initialized

  Returns:
    None

  Example:
    <code>
    SYS_MODULE_OBJ sysObject;

    sysObject = BLOCK_CIPHER_16BV1_Initialize (BLOCK_CIPHER_INDEX, NULL);
    if (sysObject != SYS_MODULE_OBJ_STATIC)
    {
        // error
    }

    BLOCK_CIPHER_16BV1_Deinitialize (sysObject);
    </code>
*/
void BLOCK_CIPHER_16BV1_Deinitialize(SYS_MODULE_OBJ object);

/***************************************************************************
  Function:
            void BLOCK_CIPHER_FunctionEncrypt (
                         BLOCK_CIPHER_HANDLE handle, void * cipherText,
                         void * plainText, void * key)
    
  Conditions:
    None
  Input:
    handle -      A driver handle. If the encryption module you are using
                  has multiple instances, this handle will be used to
                  differentiate them. For single instance encryption modules
                  (software\-only modules) this parameter can be specified
                  as NULL.
    cipherText -  The resultant cipherText produced by the encryption. The
                  type of pointer used for this parameter will be dependent
                  on the block cipher module you are using.
    plainText -   The plainText that will be encrypted. The type of pointer
                  used for this parameter will be dependent on the block
                  cipher module you are using.
    key -         Pointer to the key. The format and length of the key
                  depends on the block cipher module you are using.
  Return:
    None
  Side Effects:
    None
  Description:
    Function pointer for a block cipher's encryption function. When using
    the block cipher modes of operation module, you will configure it to
    use the encrypt function of the block cipher module that you are using
    with a pointer to that block cipher's encrypt function.
    
    None
  Remarks:
    None                                                                    
  ***************************************************************************/
typedef void (*BLOCK_CIPHER_FunctionEncrypt)(BLOCK_CIPHER_HANDLE handle, void * cipherText, void * plainText, void * key);

/***************************************************************************
  Function:
            void BLOCK_CIPHER_FunctionDecrypt (
                         BLOCK_CIPHER_HANDLE handle, void * cipherText,
                         void * plainText, void * key)
    
  Conditions:
    None
  Input:
    handle -      A driver handle. If the decryption module you are using
                  has multiple instances, this handle will be used to
                  differentiate them. For single instance decryption modules
                  (software\-only modules) this parameter can be specified
                  as NULL.
    plainText -   The resultant plainText that was decrypted. The type of
                  pointer used for this parameter will be dependent on the
                  block cipher module you are using.
    cipherText -  The cipherText that will be decrypted. The type of pointer
                  used for this parameter will be dependent on the block
                  cipher module you are using.
    key -         Pointer to the key. The format and length of the key
                  depends on the block cipher module you are using.
  Return:
    None
  Side Effects:
    None
  Description:
    Function pointer for a block cipher's decryption function. When using
    the block cipher modes of operation module, you will configure it to
    use the decrypt function of the block cipher module that you are using
    with a pointer to that block cipher's encrypt function.
    
    None
  Remarks:
    None                                                                    
  ***************************************************************************/
typedef void (*BLOCK_CIPHER_FunctionDecrypt)(BLOCK_CIPHER_HANDLE handle, void * plainText, void * cipherText, void * key);

/***************************************************************************
  Function:
			BLOCK_CIPHER_STATE BLOCK_CIPHER_16BV1_GetState (
						BLOCK_CIPHER_HANDLE handle)
  Summary:
    Returns the current state of the client handle

  Description:
    Returns the current state of the client handle. This can be used by the
    user to determine what state the thread is in durring an operation.

  Precondition:
    None.

  Parameters:
    handle          - The handle of the opened client instance returned by
                      BLOCK_CIPHER_16BV1_Open().

  Returns:
    Returns the state of the selected client handle. If the handle is in use
    this will be applicable to the operation being performed
    BLOCK_CIPHER_STATE_IDLE, BLOCK_CIPHER_STATE_ERROR, BLOCK_CIPHER_STATE_BUSY.
    otherwise the handle state will be BLOCK_CIPHER_STATE_CLOSED or
    BLOCK_CIPHER_STATE_OPEN.

  Example:
    <code>
    while (((state = BLOCK_CIPHER_16BV1_GetState(handle)) != BLOCK_CIPHER_STATE_IDLE) && (state != BLOCK_CIPHER_STATE_ERROR))
    {
        BLOCK_CIPHER_16BV1_Tasks();
    }

    if (state == BLOCK_CIPHER_STATE_ERROR)
    {
        while(1);
    }
    </code>
 */
BLOCK_CIPHER_STATE BLOCK_CIPHER_16BV1_GetState (BLOCK_CIPHER_HANDLE handle);

/***************************************************************************
  Function:
			void BLOCK_CIPHER_16BV1_Tasks (void)
  Summary:
    Runs all tasks necessary for processing a request

  Description:
    This function initiates and runs all tasks necessary for processing
    the users request for all handles. It manages all threads and allows
    access to the HW to process requests. The user should call this function
    to keep all threads moving through the hardware. It also updates the
    state of each thread as requests are moved through hardware.

  Precondition:
    None.

  Parameters:
    None.
 
  Returns:
    None

  Example:
    <code>
    while (((state = BLOCK_CIPHER_16BV1_GetState(handle)) != BLOCK_CIPHER_STATE_IDLE) && (state != BLOCK_CIPHER_STATE_ERROR))
    {
        BLOCK_CIPHER_16BV1_Tasks();
    }
    </code>
 */
void BLOCK_CIPHER_16BV1_Tasks (void);

// Includes for each mode's header file
#include "crypto_16bv1/block_cipher_16bv1_ecb.h"
#include "crypto_16bv1/block_cipher_16bv1_cbc.h"
#include "crypto_16bv1/block_cipher_16bv1_ofb.h"
#include "crypto_16bv1/block_cipher_16bv1_cfb.h"
#include "crypto_16bv1/block_cipher_16bv1_cfb1.h"
#include "crypto_16bv1/block_cipher_16bv1_cfb8.h"
#include "crypto_16bv1/block_cipher_16bv1_ctr.h"
#include "crypto_16bv1/block_cipher_16bv1_gcm.h"

#endif      // _BLOCK_CIPHER_16BV1_H
