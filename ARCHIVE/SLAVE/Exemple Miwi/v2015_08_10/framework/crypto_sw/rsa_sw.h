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

#if !defined (_RSA_SW_H)
#define _RSA_SW_H

#include "crypto_sw/src/drv_common.h"
#include "crypto_sw/src/sys_common.h"
#include "crypto_sw/src/sys_module.h"

// Type to identify which instance of an RSA hardware module to use.  Unused in the software implementation
typedef uint8_t RSA_MODULE_ID;

// Definition for a single drive index for the software-only RSA module
#define RSA_SW_INDEX_0 0
// Map of the default drive index to drive index 0
#define RSA_SW_INDEX RSA_SW_INDEX_0
// Number of drive indicies for this module
#define RSA_SW_INDEX_COUNT 1
// Definition for a single drive handle for the software-only RSA module
#define RSA_SW_HANDLE ((DRV_HANDLE) 0)

// Function pointer for the rand function type.
typedef uint32_t(*RSA_SW_RandomGet) (void);
// Global variable for the rand function used with this library.
typedef RSA_SW_RandomGet RSA_RandomGet;

/*** Data Types for Keys ***/
// complete description of a RSA public and private key

// Structure describing the format of an RSA public key (used for encryption or verification)
typedef struct
{
    int      nLen;      // key length, in bytes
    uint8_t* n;         // public modulus
    int      eLen;      // exponent length, in bytes
    uint8_t* exp;       // public exponent
} RSA_SW_PUBLIC_KEY;

// Structure describing the format of an RSA private key, in CRT format (used for decryption or signing)
typedef struct
{
    int      nLen;      // key length, in bytes
    uint8_t* P;         // CRT "P" parameter
    uint8_t* Q;         // CRT "Q" parameter
    uint8_t* dP;        // CRT "dP" parameter
    uint8_t* dQ;        // CRT "dQ" parameter
    uint8_t* qInv;      // CRT "qInv" parameter
} RSA_SW_PRIVATE_KEY_CRT;

// Enumeration describing the padding type that should be used with a message being encrypted
typedef enum
{
    RSA_SW_PAD_DEFAULT,        // Use default padding
    RSA_SW_PAD_PKCS1,          // Use the PKCS1 padding format
} RSA_SW_PAD_TYPE;

// Enumeration describing statuses that could apply to an RSA operation
typedef enum
{
    /* Driver open, but not configured by client */
    RSA_SW_STATUS_OPEN = DRV_CLIENT_STATUS_READY + 3,

    /* Driver initialized, but not opened by client */
    RSA_SW_STATUS_INIT = DRV_CLIENT_STATUS_READY + 2,

    /* Open and configured, ready to start new operation */
    RSA_SW_STATUS_READY = DRV_CLIENT_STATUS_READY + 0,

    /* Operation in progress, unable to start a new one */
    RSA_SW_STATUS_BUSY = DRV_CLIENT_STATUS_BUSY,

    /* Error Occured */
    RSA_SW_STATUS_ERROR = DRV_CLIENT_STATUS_ERROR - 0,

    /* client Invalid or driver not initialized */
    RSA_SW_STATUS_INVALID = DRV_CLIENT_STATUS_ERROR - 1,

    /* client Invalid or driver not initialized */
    RSA_SW_STATUS_BAD_PARAM = DRV_CLIENT_STATUS_ERROR - 2

} RSA_SW_STATUS;

// Enumeration describing modes of operation used with RSA
typedef enum
{
    /* RS232 Mode (Asynchronous Mode of Operation) */
    RSA_SW_OPERATION_MODE_NONE  = (1 << 0),

    /* RS232 Mode (Asynchronous Mode of Operation) */
    RSA_SW_OPERATION_MODE_ENCRYPT = (1 << 1),

    /* RS485 Mode (Asynchronous Mode of Operation) */
    RSA_SW_OPERATION_MODE_DECRYPT = (1 << 2)

} RSA_SW_OPERATION_MODES;

// Initialization structure used for RSA.
typedef struct
{
    /* System module initialization */
    SYS_MODULE_INIT                 moduleInit;

    /* Identifies RSA hardware module (PLIB-level) ID */
    RSA_MODULE_ID                 rsaID;

    /* Operation Modes of the driver */
    uint8_t                         operationMode;

    /* Flags for the rsa initialization */
    uint8_t                         initFlags;

} RSA_SW_INIT;

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ RSA_SW_Initialize(const SYS_MODULE_INDEX index, 
										const SYS_MODULE_INIT * const init)

  Summary:
    Initializes the data for the instance of the RSA module

  Description:
    This routine initializes data for the instance of the RSA module.

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
*/
SYS_MODULE_OBJ RSA_SW_Initialize( const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init);

// *****************************************************************************
/* Function:
    void RSA_SW_Deinitialize(SYS_MODULE_OBJ object)

  Summary:
    Deinitializes the instance of the RSA module

  Description:
    Deinitializes the specific module instance disabling its operation.  Resets 
	all the internal data structures and fields for the specified instance to 
	the default settings.

  Precondition:
    None

  Parameters:
    object           - Identifier for the instance to be de-initialized
  
  Returns:
    None
*/
void RSA_SW_Deinitialize( SYS_MODULE_OBJ object);

// *****************************************************************************
/* Function:
    DRV_HANDLE RSA_SW_Open(const SYS_MODULE_INDEX index, 
							const DRV_IO_INTENT ioIntent)

  Summary:
    Opens a new client for the device instance

  Description:
    Returns a handle of the opened client instance. All client operation APIs
    will require this handle as an argument

  Precondition:
    The driver must have been previously initialized and in the 
	initialized state.

  Parameters:
    index           - Identifier for the instance to opened
	ioIntent		- Possible values from the enumeration DRV_IO_INTENT
					  Used to specify blocking or non-blocking mode
  
  Returns:
    None
*/
DRV_HANDLE RSA_SW_Open( const SYS_MODULE_INDEX index, const DRV_IO_INTENT ioIntent);

// *****************************************************************************
/* Function:
    void RSA_SW_Close (DRV_HANDLE handle)

  Summary:
    Closes an opened client

  Description:
    Closes an opened client, resets the data structure and removes the client
	from the driver.

  Precondition:
    None.

  Parameters:
    handle          - The handle of the opened client instance returned by 
					  RSA_SW_Open().
  
  Returns:
    None
*/
void RSA_SW_Close (DRV_HANDLE handle);

// *****************************************************************************
/* Function:
    RSA_SW_STATUS RSA_SW_ClientStatus(DRV_HANDLE handle)

  Summary:
    Returns the current state of the encryption/decryption operation

  Description:
    This routine returns the current state of the encryption/decryption operation.

  Precondition:
    Driver must be opened by a client.

  Parameters:
    handle          - The handle of the opened client instance
  
  Returns:
    Driver status (the current status of the encryption/decryption operation). 
*/
RSA_SW_STATUS RSA_SW_ClientStatus( DRV_HANDLE handle );

// *****************************************************************************
/* Function:
    int RSA_SW_Configure(DRV_HANDLE handle, uint8_t *xBuffer, uint8_t *yBuffer, 
						  int xLen, int yLen, RSA_SW_RandomGet randFunc, 
						  RSA_SW_PAD_TYPE padType)

  Summary:
    Configures the client instance

  Description:
    Configures the client instance data structure with information needed by the 
	encrypt/decrypt routines

  Precondition:
    Driver must be opened by a client.

  Parameters:
    handle          - The handle of the opened client instance.
	xBuffer			- A pointer to a working buffer needed by the encrypt/decrypt
					  routines.
	yBuffer			- A pointer to a working buffer needed by the encrypt/decrypt
					  routines
	xLen			- The size (in bytes) of xBuffer
	yLen			- The size (in bytes) of yBuffer
	randFunc		- A pointer to a function used to generate random numbers for
					  message padding.
	padType			- The type of padding requested.
  
  Returns:
    0 if successful; 1 if not successful

  Remarks:
	In the dsPIC implementation the xBuffer should be twice as large as the key 
    length, located in x-memory, and be 64-byte aligned.  The yBuffer should be 
    three times as large as the key length, located in y-memory, and be 2-byte 
    aligned.
    In the other implementations, xBuffer and yBuffer should both be 4-byte aligned 
    and should both be twice the size of the key length.
	NOTE: RSA_SW_PAD_DEFAULT is currently the only supported type of padding
*/
int RSA_SW_Configure(DRV_HANDLE h, uint8_t *xBuffer, uint8_t *yBuffer, uint16_t xLen, uint16_t yLen,  RSA_SW_RandomGet randFunc, RSA_SW_PAD_TYPE padType);

// *****************************************************************************
/* Function:
    RSA_SW_STATUS RSA_SW_Encrypt (DRV_HANDLE handle, uint8_t *cipherText, 
									uint8_t *plainText, int msgLen, 
									const RSA_SW_PUBLIC_KEY *publicKey)

  Summary:
    Encrypts a message using a public RSA key

  Description:
    This routine encrypts a message using a public RSA key.

  Precondition:
    Driver must be opened by a client.

  Parameters:
    handle          - The handle of the opened client instance
	cipherText		- A pointer to a buffer that will hold the encrypted message
	plainText		- A pointer to the buffer containing the message to be encrypted
	msgLen			- The length of the message to be encrypted
	publicKey		- A pointer to a structure containing the public key
  
  Returns:
    Driver status. If running in blocking mode, the function will return 
	RSA_SW_STATUS_READY aftera successful encryption operation.
	If running in non-blocking mode, the driver will start the encryption
	operation and return immediately with RSA_SW_STATUS_BUSY.

  Remarks:
	NOTE: The plainText and cipherText buffers must be at least as large as the 
		  key size
	NOTE: The message length must not be greater than the key size
*/
RSA_SW_STATUS RSA_SW_Encrypt (DRV_HANDLE handle, uint8_t *cipherText, uint8_t *plainText, uint16_t msgLen, const RSA_SW_PUBLIC_KEY *publicKey);

// *****************************************************************************
/* Function:
    RSA_SW_STATUS RSA_SW_Decrypt (DRV_HANDLE handle, uint8_t *plainText, 
									uint8_t *cipherText, int * msgLen, 
									const RSA_SW_PRIVATE_KEY_CRT *privateKey)

  Summary:
    Decrypts a message using a private RSA key

  Description:
    This routine decrypts a message using a private RSA key.

  Precondition:
    Driver must be opened by a client.

  Parameters:
    handle          - The handle of the opened client instance
	plainText		- A pointer to a buffer that will hold the decrypted message
	cipherText		- A pointer to a buffer containing the message to be decrypted
	msgLen			- The length of the message that was decrypted
	privateKey		- A pointer to a structure containing the public key
  
  Returns:
    Driver status. If running in blocking mode, the function will return 
	RSA_SW_STATUS_READY after	a successful decryption operation.
	If running in non-blocking mode, the driver will start the decryption
	operation and return immediately with RSA_SW_STATUS_BUSY.

  Remarks:
	NOTE: The plainText and cipherText buffers must be at least as large as the 
		  key size
	NOTE: The message length must not be greater than the key size
*/
RSA_SW_STATUS RSA_SW_Decrypt (DRV_HANDLE handle, uint8_t *plainText, uint8_t *cipherText, uint16_t * msgLen, const RSA_SW_PRIVATE_KEY_CRT *privateKey);

// *****************************************************************************
/* Function:
    void RSA_SW_Tasks(SYS_MODULE_OBJ object)

  Summary:
    Maintains the driver's state machine by advancing a non-blocking encryption
	or decryption operation.

  Description:
    This routine maintains the driver's state machine by advancing a non-blocking 
	encryptiom or decryption operation.

  Precondition:
    Driver must be opened by a client.

  Parameters:
    object      - Object handle for the specified driver instance
  
  Returns:
    None.
*/
void RSA_SW_Tasks (SYS_MODULE_OBJ object);

#endif      // _RSA_SW_H
