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

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "crypto_sw/src/rsa/other/rsa_sw_private.h"
#include "bigint/bigint.h"

#include "crypto_sw/src/drv_common.h"
#include "crypto_sw/src/sys_common.h"

// State of the current decryption operation
typedef enum
{
    DECRYPTION_STATE_START = 0,
    DECRYPTION_STATE_FIND_M1,
    DECRYPTION_STATE_FIND_M2,
    DECRYPTION_STATE_FINISH
} STATE_DECRYPTION;


// This struct is for internal use by the DRV_RSA library. It is not exposed to the client. 
typedef struct {
    uint8_t *xBuffer;
    uint8_t *yBuffer;
    uint8_t * pe;
    uint8_t * pend;
    uint32_t length;
    uint16_t * msgSize;
    uint16_t xLen;
    uint16_t yLen;
    uint8_t startBit;
    STATE_DECRYPTION decryptionState;
    RSA_SW_RandomGet randFunc;
    RSA_SW_PAD_TYPE padType;
    DRV_IO_INTENT runType;
    RSA_SW_STATUS status;
    RSA_SW_OPERATION_MODES op;
    BIGINT_DATA pT;
    BIGINT_DATA cT;
    BIGINT_DATA pkExp;
    BIGINT_DATA pkN;
    BIGINT_DATA pkdP;
    BIGINT_DATA pkP;
    BIGINT_DATA pkdQ;
    BIGINT_DATA pkQ;
    BIGINT_DATA pkqInv;
} RSA_SW_DESC;

static RSA_SW_DESC rsaDesc0;

__inline static bool is_aligned(void *p)
{
    return (int)p % sizeof (int) == 0;
}

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
SYS_MODULE_OBJ RSA_SW_Initialize(const SYS_MODULE_INDEX index, const SYS_MODULE_INIT * const init)
{
    if (index != RSA_SW_INDEX)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    rsaDesc0.xBuffer = NULL;
    rsaDesc0.yBuffer = NULL;
    rsaDesc0.randFunc = NULL;
    rsaDesc0.padType = RSA_SW_PAD_DEFAULT;
    rsaDesc0.runType = DRV_IO_INTENT_EXCLUSIVE;
    rsaDesc0.status = RSA_SW_STATUS_INIT;
    rsaDesc0.op = RSA_SW_OPERATION_MODE_NONE;
    rsaDesc0.pe = 0x00;
    rsaDesc0.pend = 0x00;

    return SYS_MODULE_OBJ_STATIC;
}

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
void RSA_SW_Deinitialize(SYS_MODULE_OBJ object) 
{
    rsaDesc0.xBuffer = NULL;
    rsaDesc0.yBuffer = NULL;
    rsaDesc0.randFunc = NULL;
    rsaDesc0.padType = RSA_SW_PAD_DEFAULT;
    rsaDesc0.runType = DRV_IO_INTENT_EXCLUSIVE;
    rsaDesc0.status = RSA_SW_STATUS_INVALID;
    rsaDesc0.op = RSA_SW_OPERATION_MODE_NONE;

    return;
}

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
    ioIntent        - Possible values from the enumeration DRV_IO_INTENT
                      Used to specify blocking or non-blocking mode
  
  Returns:
    None
*/
DRV_HANDLE RSA_SW_Open(const SYS_MODULE_INDEX index, const DRV_IO_INTENT ioIntent)
{
    if (index != RSA_SW_INDEX)
    {
        return DRV_HANDLE_INVALID;
    }

    if (rsaDesc0.status != RSA_SW_STATUS_INIT) 
    {
        return DRV_HANDLE_INVALID;
    }

    rsaDesc0.runType = (ioIntent | DRV_IO_INTENT_EXCLUSIVE);

    rsaDesc0.status = RSA_SW_STATUS_OPEN;

    return RSA_SW_HANDLE;

}

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
void RSA_SW_Close (DRV_HANDLE handle)
{
    rsaDesc0.xBuffer = NULL;
    rsaDesc0.yBuffer = NULL;
    rsaDesc0.xLen = 0;
    rsaDesc0.yLen = 0;
    rsaDesc0.randFunc = NULL;
    rsaDesc0.padType = RSA_SW_PAD_DEFAULT;
    rsaDesc0.runType = DRV_IO_INTENT_EXCLUSIVE;
    rsaDesc0.status = RSA_SW_STATUS_INIT;
    rsaDesc0.op = RSA_SW_OPERATION_MODE_NONE;

    return;
}

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
    xBuffer            - A pointer to a working buffer needed by the encrypt/decrypt
                      routines
    yBuffer            - A pointer to a working buffer needed by the encrypt/decrypt
                      routines
    xLen            - The size (in bytes) of xBuffer
    yLen            - The size (in bytes) of yBuffer
    randFunc        - A pointer to a function used to generate random numbers for
                      message padding.
    padType            - The type of padding requested.
  
  Returns:
    0 if successful; 1 if not successful

  Remarks:
    NOTE: xBuffer and yBuffer must each be at least 1.5 times the key size
    NOTE: RSA_SW_PAD_DEFAULT is currently the only supported type of padding
*/
int RSA_SW_Configure(DRV_HANDLE handle, uint8_t *xBuffer, uint8_t *yBuffer, uint16_t xLen, uint16_t yLen, RSA_SW_RandomGet randFunc, RSA_SW_PAD_TYPE padType)
{
    if (handle != RSA_SW_HANDLE || padType != RSA_SW_PAD_DEFAULT)
    {
        return -1;
    }

    if (xBuffer == NULL || yBuffer == NULL || randFunc == NULL) 
    {
        return -1;
    }

    if (xLen == 0 || yLen == 0)
    {
        return -1;
    }

    if (!is_aligned(xBuffer) || !is_aligned(yBuffer))
    {
        return -1;
    }
    
    rsaDesc0.xBuffer = xBuffer;
    rsaDesc0.yBuffer = yBuffer;
    rsaDesc0.xLen = xLen;
    rsaDesc0.yLen = yLen;
    rsaDesc0.randFunc = randFunc;
    rsaDesc0.padType = padType;
    rsaDesc0.status = RSA_SW_STATUS_READY;

    return 0;
}

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
    cipherText        - A pointer to a buffer that will hold the encrypted message
    plainText        - A pointer to the buffer containing the message to be encrypted
    msgLen            - The length of the message to be encrypted
    publicKey        - A pointer to a structure containing the public key
  
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
RSA_SW_STATUS RSA_SW_Encrypt (DRV_HANDLE handle, uint8_t *cipherText, uint8_t *plainText, uint16_t msgLen, const RSA_SW_PUBLIC_KEY *publicKey)
{
    bool Chk_done = false;
    uint8_t * ePtr;

    if(rsaDesc0.op != RSA_SW_OPERATION_MODE_NONE)
    {
        return RSA_SW_STATUS_ERROR;
    }    

    if (handle != RSA_SW_HANDLE)
    {
        return RSA_SW_STATUS_BAD_PARAM;
    }

    if (!is_aligned(cipherText) || !is_aligned(plainText))
    {
        return RSA_SW_STATUS_BAD_PARAM;
    }

    if ((rsaDesc0.xLen < (msgLen * 2)) || (rsaDesc0.yLen < (msgLen * 2)))
    {
        return RSA_SW_STATUS_BAD_PARAM;
    }

    if (cipherText == NULL || plainText == NULL || publicKey == NULL)
    {
        return RSA_SW_STATUS_BAD_PARAM;
    }
    
    if (msgLen > publicKey->nLen - 11)
    {
        return RSA_SW_STATUS_ERROR;
    }

    if ((publicKey->nLen != 0x40) && (publicKey->nLen != 0x80) && (publicKey->nLen != 0x100))
    {
        return RSA_SW_STATUS_ERROR;
    }

    if (((publicKey->n[0] & 0x01) != 0x01) || ((publicKey->n[publicKey->nLen - 1] & 0x80) != 0x80))
    {
        return RSA_SW_STATUS_BAD_PARAM;
    }

    if ((publicKey->eLen <= 0) || (publicKey->eLen > publicKey->nLen))
    {
        return RSA_SW_STATUS_ERROR;
    }
    
    // Ensure e is odd
    if ((publicKey->exp[0] & 0x01) != 0x01)
    {
        return RSA_SW_STATUS_BAD_PARAM;
    }

    // Ensure e doesn't equal 1
    ePtr = publicKey->exp + publicKey->eLen - 1;
    while ((*ePtr == 0x00) && (ePtr != publicKey->exp))
    {
        ePtr--;
    }

    if ((ePtr == publicKey->exp) && (*ePtr == 0x01))
    {
        return RSA_SW_STATUS_BAD_PARAM;
    }

    rsaDesc0.op = RSA_SW_OPERATION_MODE_ENCRYPT;
    rsaDesc0.status = RSA_SW_STATUS_BUSY;
    rsaDesc0.length = (uint16_t)publicKey->nLen;
    
    // set parameters
    rsaDesc0.cT.startAddress = cipherText;
    rsaDesc0.cT.bLength = publicKey->nLen;

    RSAPadData (rsaDesc0.yBuffer, plainText, msgLen, rsaDesc0.length, rsaDesc0.randFunc, rsaDesc0.padType);
    rsaDesc0.pT.startAddress = rsaDesc0.yBuffer;
    rsaDesc0.pT.bLength = rsaDesc0.length;

    rsaDesc0.pkExp.startAddress = publicKey->exp;
    rsaDesc0.pkExp.bLength = publicKey->eLen;
    rsaDesc0.pkN.startAddress = publicKey->n;
    rsaDesc0.pkN.bLength = publicKey->nLen;

    if (!(rsaDesc0.runType & DRV_IO_INTENT_NONBLOCKING))
    {
        Chk_done = RSAModExp(&rsaDesc0.cT, &rsaDesc0.pT, &rsaDesc0.pkExp, &rsaDesc0.pkN);
        while(!(Chk_done))
        {
            Chk_done = RSAModExp(&rsaDesc0.cT, &rsaDesc0.pT, &rsaDesc0.pkExp, &rsaDesc0.pkN);
        }

        RSASwapEndianness (&rsaDesc0.cT);

        rsaDesc0.op = RSA_SW_OPERATION_MODE_NONE;
        rsaDesc0.status = RSA_SW_STATUS_READY;
    }
    
    return rsaDesc0.status;
}

// *****************************************************************************
/* Function:
    RSA_SW_STATUS RSA_SW_Decrypt (DRV_HANDLE handle, uint8_t *plainText, 
                                    uint8_t *cipherText, int msgLen, 
                                    const RSA_SW_PRIVATE_KEY_CRT *privateKey)

  Summary:
    Decrypts a message using a private RSA key

  Description:
    This routine decrypts a message using a private RSA key.

  Precondition:
    Driver must be opened by a client.

  Parameters:
    handle          - The handle of the opened client instance
    plainText        - A pointer to a buffer that will hold the decrypted message
    cipherText        - A pointer to a buffer containing the message to be decrypted
    msgLen            - The length of the message to be decrypted
    privateKey        - A pointer to a structure containing the public key
  
  Returns:
    Driver status. If running in blocking mode, the function will return 
    RSA_SW_STATUS_READY after    a successful decryption operation.
    If running in non-blocking mode, the driver will start the decryption
    operation and return immediately with RSA_SW_STATUS_BUSY.

  Remarks:
    NOTE: The plainText and cipherText buffers must be at least as large as the 
          key size
    NOTE: The message length must not be greater than the key size
*/
RSA_SW_STATUS RSA_SW_Decrypt (DRV_HANDLE handle, uint8_t *plainText, uint8_t *cipherText, uint16_t * msgLen, const RSA_SW_PRIVATE_KEY_CRT *privateKey)
{
    BIGINT_DATA m1,*m1Ptr;
    BIGINT_DATA m2,*m2Ptr;
    BIGINT_DATA tmp, *tmpPtr;
    bool Chk_done = false;
    uint8_t * endPtr;

    if (handle != RSA_SW_HANDLE)
    {
        return RSA_SW_STATUS_BAD_PARAM;
    }

    if (!is_aligned(cipherText) || !is_aligned(plainText))
      {
           return RSA_SW_STATUS_BAD_PARAM;
       }

    if ((privateKey->nLen != 0x40) && (privateKey->nLen != 0x80) && (privateKey->nLen != 0x100))
    {
        return RSA_SW_STATUS_ERROR;
    }

    if (((privateKey->P[0] & 0x01) != 0x01) || ((privateKey->P[(privateKey->nLen >> 1) - 1] & 0x80) != 0x80))
    {
        return RSA_SW_STATUS_BAD_PARAM;
    }

    if (((privateKey->Q[0] & 0x01) != 0x01) || ((privateKey->Q[(privateKey->nLen >> 1) - 1] & 0x80) != 0x80))
    {
        return RSA_SW_STATUS_BAD_PARAM;
    }

    rsaDesc0.status = RSA_SW_STATUS_BUSY;
    rsaDesc0.op = RSA_SW_OPERATION_MODE_DECRYPT;
    rsaDesc0.length = privateKey->nLen;

    memcpy (rsaDesc0.yBuffer, cipherText, rsaDesc0.length);
    rsaDesc0.cT.startAddress = rsaDesc0.yBuffer;
    rsaDesc0.cT.bLength = rsaDesc0.length;
    rsaDesc0.pT.startAddress = plainText;
    rsaDesc0.pT.bLength = rsaDesc0.length;

    rsaDesc0.msgSize = msgLen;

    tmpPtr = &tmp;
    tmpPtr->startAddress = rsaDesc0.xBuffer;
    tmpPtr->bLength = rsaDesc0.length * 2;

    m1Ptr = &m1;
    m1Ptr->startAddress = rsaDesc0.yBuffer + rsaDesc0.length;
    m1Ptr->bLength = rsaDesc0.length / 2;

    m2Ptr = &m2;
    m2Ptr->startAddress = rsaDesc0.yBuffer + ((rsaDesc0.length * 3) / 2);
    m2Ptr->bLength = rsaDesc0.length / 2;

    rsaDesc0.pkdP.startAddress = (uint32_t*)privateKey->dP;
    rsaDesc0.pkdP.bLength = (uint32_t)privateKey->nLen/2; 

    rsaDesc0.pkP.startAddress = (uint32_t*)privateKey->P;
    rsaDesc0.pkP.bLength = (uint32_t)privateKey->nLen/2; 

    rsaDesc0.pkdQ.startAddress = (uint32_t*)privateKey->dQ;
    rsaDesc0.pkdQ.bLength = (uint32_t)privateKey->nLen/2;

    rsaDesc0.pkQ.startAddress = (uint32_t*)privateKey->Q;
    rsaDesc0.pkQ.bLength = (uint32_t)privateKey->nLen/2;

    rsaDesc0.pkqInv.startAddress = (uint32_t*)privateKey->qInv;
    rsaDesc0.pkqInv.bLength = (uint32_t)privateKey->nLen/2;

    RSASwapEndianness (&rsaDesc0.cT);

    if (!(rsaDesc0.runType & DRV_IO_INTENT_NONBLOCKING))
    {
        while(!Chk_done)
        {
            Chk_done = RSAModExp(m1Ptr, &rsaDesc0.cT, &rsaDesc0.pkdP, &rsaDesc0.pkP);
        }
        Chk_done = false;
        while(!Chk_done)
        {
            Chk_done = RSAModExp(m2Ptr, &rsaDesc0.cT, &rsaDesc0.pkdQ, &rsaDesc0.pkQ);
        }
    }
    else
    {
        rsaDesc0.decryptionState = DECRYPTION_STATE_START;
        return rsaDesc0.status;
    }

    if(BIGINT_Compare(m1Ptr, m2Ptr) < 0)
    {
        BIGINT_Add(m1Ptr, m1Ptr, &rsaDesc0.pkP);
    }

    // m1Ptr = m1Ptr - m2Ptr
    BIGINT_Subtract(m1Ptr, m1Ptr, m2Ptr);

    // tmpPtr = m1Ptr * qInv
    BIGINT_Multiply(tmpPtr, m1Ptr, &rsaDesc0.pkqInv);

    // m1Ptr = tmpPtr % p
    BIGINT_Mod(tmpPtr, tmpPtr, &rsaDesc0.pkP);
    memcpy (m1Ptr->startAddress, tmpPtr->startAddress, m1Ptr->bLength);

    // tmp = h = qInv * (m1-m2) mod P
    BIGINT_Multiply(tmpPtr, m1Ptr, &rsaDesc0.pkQ);

    // rmp = m = m2 + h*q
    BIGINT_Add(tmpPtr, tmpPtr, m2Ptr);

    rsaDesc0.status = RSA_SW_STATUS_READY;

    // Find the message length
    endPtr = (uint8_t *)tmpPtr->startAddress + rsaDesc0.cT.bLength - 1;

    if ((*endPtr-- == 0x00) && (*endPtr-- == 0x02))
    {
        while ((endPtr >= (uint8_t *)tmpPtr->startAddress) && (*endPtr-- != 0x00));

        if ((endPtr - (uint8_t *)tmpPtr->startAddress + 1) > rsaDesc0.cT.bLength - 11)
        {
            rsaDesc0.status = RSA_SW_STATUS_ERROR;
        }
        else
        {
            *msgLen = endPtr - (uint8_t *)tmpPtr->startAddress + 1;

            if (endPtr >= (uint8_t *)tmpPtr->startAddress)
            {
                uint8_t i = 0;

                while (endPtr >= (uint8_t *)tmpPtr->startAddress)
                {
                    *(plainText + i++) = *endPtr--;
                }
            }
        }
    }
    else
    {
        rsaDesc0.status = RSA_SW_STATUS_ERROR;
    }

    rsaDesc0.op = RSA_SW_OPERATION_MODE_NONE;

    return rsaDesc0.status;
}

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
RSA_SW_STATUS RSA_SW_ClientStatus(DRV_HANDLE handle)
{
    if (handle != RSA_SW_HANDLE)
    {
        return RSA_SW_STATUS_BAD_PARAM;
    }

    return rsaDesc0.status;
}

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
void RSA_SW_Tasks(SYS_MODULE_OBJ object)
{
    bool chk_done = false;
    BIGINT_DATA m1, *m1Ptr;
    BIGINT_DATA m2, *m2Ptr;
    BIGINT_DATA tmp, *tmpPtr;
    uint8_t * endPtr;

    if (object != SYS_MODULE_OBJ_STATIC)
    {
        return;
    }

    if (rsaDesc0.status == RSA_SW_STATUS_BUSY)
    {
        if (rsaDesc0.op == RSA_SW_OPERATION_MODE_ENCRYPT)
        {
            chk_done = RSAModExp(&rsaDesc0.cT, &rsaDesc0.pT, &rsaDesc0.pkExp, &rsaDesc0.pkN);
            if (chk_done)
            {
                RSASwapEndianness (&rsaDesc0.cT);
                rsaDesc0.status = RSA_SW_STATUS_READY;
                rsaDesc0.op = RSA_SW_OPERATION_MODE_NONE;
            }
        }
        else if (rsaDesc0.op == RSA_SW_OPERATION_MODE_DECRYPT)
        {
            tmpPtr = &tmp;
            tmpPtr->startAddress = rsaDesc0.xBuffer;
            tmpPtr->bLength = rsaDesc0.length * 2;

            m1Ptr = &m1;
            m1Ptr->startAddress = rsaDesc0.yBuffer + rsaDesc0.length;
            m1Ptr->bLength = rsaDesc0.length / 2;

            m2Ptr = &m2;
            m2Ptr->startAddress = rsaDesc0.yBuffer + ((rsaDesc0.length * 3) / 2);
            m2Ptr->bLength = rsaDesc0.length / 2;

            switch (rsaDesc0.decryptionState)
            {
                case DECRYPTION_STATE_START:
                    rsaDesc0.status = RSA_SW_STATUS_BUSY;
                    rsaDesc0.decryptionState = DECRYPTION_STATE_FIND_M1;
                case DECRYPTION_STATE_FIND_M1:
                    chk_done = RSAModExp(m1Ptr, &rsaDesc0.cT, &rsaDesc0.pkdP, &rsaDesc0.pkP);
                    if (chk_done)
                    {
                        rsaDesc0.decryptionState = DECRYPTION_STATE_FIND_M2;
                    }
                    break;
                case DECRYPTION_STATE_FIND_M2:
                    chk_done = RSAModExp(m2Ptr, &rsaDesc0.cT, &rsaDesc0.pkdQ, &rsaDesc0.pkQ);
                    if (chk_done)
                    {
                        rsaDesc0.decryptionState = DECRYPTION_STATE_FINISH;
                    }
                    break;
                case DECRYPTION_STATE_FINISH:
                    if(BIGINT_Compare(m1Ptr, m2Ptr) < 0)
                    {
                        BIGINT_Add(m1Ptr, m1Ptr, &rsaDesc0.pkP);
                    }

                    // m1Ptr = m1Ptr - m2Ptr
                    BIGINT_Subtract(m1Ptr, m1Ptr, m2Ptr);

                    // tmpPtr = m1Ptr * qInv
                    BIGINT_Multiply(tmpPtr, m1Ptr, &rsaDesc0.pkqInv);

                    // m1Ptr = tmpPtr % p
                    BIGINT_Mod(tmpPtr, tmpPtr, &rsaDesc0.pkP);
                    memcpy (m1Ptr->startAddress, tmpPtr->startAddress, m1Ptr->bLength);

                    // tmp = h = qInv * (m1-m2) mod P
                    BIGINT_Multiply(tmpPtr, m1Ptr, &rsaDesc0.pkQ);

                    // rmp = m = m2 + h*q
                    BIGINT_Add(tmpPtr, tmpPtr, m2Ptr);

                    endPtr = (uint8_t *)tmpPtr->startAddress + rsaDesc0.cT.bLength - 1;

                    if ((*endPtr-- == 0x00) && (*endPtr-- == 0x02))
                    {
                        while ((endPtr >= (uint8_t *)tmpPtr->startAddress) && (*endPtr-- != 0x00));

                        if ((endPtr - (uint8_t *)tmpPtr->startAddress + 1) > rsaDesc0.cT.bLength - 11)
                        {
                            rsaDesc0.status = RSA_SW_STATUS_ERROR;
                        }
                        else
                        {
                            *rsaDesc0.msgSize = endPtr - (uint8_t *)tmpPtr->startAddress + 1;

                            if (endPtr >= (uint8_t *)tmpPtr->startAddress)
                            {
                                uint8_t i = 0;

                                while (endPtr >= (uint8_t *)tmpPtr->startAddress)
                                {
                                    *((uint8_t *)rsaDesc0.pT.startAddress + i++) = *endPtr--;
                                }
                            }
                            rsaDesc0.status = RSA_SW_STATUS_READY;
                        }
                    }
                    else
                    {
                        rsaDesc0.status = RSA_SW_STATUS_ERROR;
                    }

                    rsaDesc0.op = RSA_SW_OPERATION_MODE_NONE;
                    break;

            }
        }
    }

    return;
}

bool RSAModExp(BIGINT_DATA * result, BIGINT_DATA * data, BIGINT_DATA * e, BIGINT_DATA *n )
{
    BIGINT_DATA *tmp,t;
    tmp=&t;
    tmp->startAddress = rsaDesc0.xBuffer;
    tmp->bLength = rsaDesc0.length * 2;

    // This macro processes a single bit, either with or without debug output.
    // The C preprocessor will optimize this without the overhead of a function call.
      #define doBit(a)      BIGINT_Multiply(tmp,result,result); \
                            BIGINT_Mod(tmp,tmp, n); \
                            memcpy (result->startAddress, tmp->startAddress, result->bLength); \
                            if(*rsaDesc0.pe & a) \
                            { \
                                BIGINT_Multiply(tmp,result, data); \
                                BIGINT_Mod(tmp,tmp, n); \
                                memcpy (result->startAddress, tmp->startAddress, result->bLength); \
                            }

    // Determine if this is a new computation
    if(rsaDesc0.pe == rsaDesc0.pend)
    {// Yes, so set up the calculation

        // Set up *pe to point to the MSB in e, *pend to stop byte
        rsaDesc0.pe = (uint8_t*)e->startAddress+ e->bLength-1;
        rsaDesc0.pend = ((uint8_t*)e->startAddress) -1;
        while(*rsaDesc0.pe == 0x00u)
        {
            rsaDesc0.pe--;

            // Handle special case where e is zero and n >= 2 (result y should be 1).
            // If n = 1, then y should be zero, but this really special case isn't
            // normally useful, so we shall not implement it and will return 1
            // instead.
            if(rsaDesc0.pe == rsaDesc0.pend)
            {
                BIGINT_Set(result,0x00);
                *(uint8_t *)(result->startAddress) = 0x01;
                return true;
            }
        }

        // Start at the bit following the MSbit
        rsaDesc0.startBit = *rsaDesc0.pe;
        if(rsaDesc0.startBit & 0x80)
            rsaDesc0.startBit = 0x40;
        else if(rsaDesc0.startBit & 0x40)
            rsaDesc0.startBit = 0x20;
        else if(rsaDesc0.startBit & 0x20)
            rsaDesc0.startBit = 0x10;
        else if(rsaDesc0.startBit & 0x10)
            rsaDesc0.startBit = 0x08;
        else if(rsaDesc0.startBit & 0x08)
            rsaDesc0.startBit = 0x04;
        else if(rsaDesc0.startBit & 0x04)
            rsaDesc0.startBit = 0x02;
        else if(rsaDesc0.startBit & 0x02)
            rsaDesc0.startBit = 0x01;
        else
        {
            rsaDesc0.pe--;
            rsaDesc0.startBit = 0x80;
        }

        // Process that second bit now to save memory in Y
        // (first round squares the message (X).  Subsequent rounds square Y,
        // which is at most half that size when running the CRT.)
        BIGINT_Multiply(tmp,data,data);
        BIGINT_Mod(tmp,tmp, n);
        memcpy (result->startAddress, tmp->startAddress, result->bLength);

        if(*rsaDesc0.pe & rsaDesc0.startBit)
        {// If bit is '1'
            BIGINT_Multiply(tmp,result, data);
            BIGINT_Mod(tmp, tmp, n);
            memcpy (result->startAddress, tmp->startAddress, result->bLength);
        }

        // Move to the next bit as the startBit
        rsaDesc0.startBit >>= 1;
        if(!rsaDesc0.startBit)
        {
            rsaDesc0.pe--;
            rsaDesc0.startBit = 0x80;
        }

        // We are finished if e has only one or two set bits total, ex: e = 3
        if(rsaDesc0.pe == rsaDesc0.pend)
          return true;

    }

    // Process remaining bits in current byte
    switch(rsaDesc0.startBit)
    {
        case 0x80:
            doBit(0x80);
            rsaDesc0.startBit >>= 1;
        case 0x40:
            doBit(0x40);
            rsaDesc0.startBit >>= 1;
            break;
        case 0x20:
            doBit(0x20);
            rsaDesc0.startBit >>= 1;
        case 0x10:
            doBit(0x10);
            rsaDesc0.startBit >>= 1;
            break;
        case 0x08:
            doBit(0x08);
            rsaDesc0.startBit >>= 1;
        case 0x04:
            doBit(0x04);
            rsaDesc0.startBit >>= 1;
            break;
        case 0x02:
            doBit(0x02);
            rsaDesc0.startBit >>= 1;
        case 0x01:
            doBit(0x01);
            rsaDesc0.startBit = 0x80;
            rsaDesc0.pe--;
            if(rsaDesc0.pe == rsaDesc0.pend)
                return true;
    }

    return false;

}


void RSAPadData (uint8_t * result, uint8_t * source, uint16_t msgLen, uint16_t keyLen, RSA_RandomGet randFunction, RSA_SW_PAD_TYPE padType)
{
    uint32_t paddingLength;
    uint16_t i;

    switch (padType)
    {
        case RSA_SW_PAD_DEFAULT:
        case RSA_SW_PAD_PKCS1:
            paddingLength = keyLen - msgLen - 3;

            // Copy the message into the destination buffer backwards
            source += msgLen;
            for (i = 0; i < msgLen; i++)
            {
                *(result + i) = *(--source);
            }
            *(result + i++) = 0x00;
            // Pad the message using random numbers obtained from the user's RNG function
            while (paddingLength != 0)
            {
                do
                {
                    *(result + i) = (*randFunction)();
                } while (*(result + i) == 0x00u);
                i++;
                paddingLength--;
            }
            *(result + i++) = 0x02;
            *(result + i) = 0x00;
            break;
        default:
            memcpy (result, source, msgLen);
            memset (result + msgLen, 0x00, keyLen - msgLen);
            break;
    }
}

void RSASwapEndianness (BIGINT_DATA * X)
{
    uint8_t swap;
    uint8_t * ptr = (uint8_t *)X->startAddress;
    uint8_t * endPtr = (uint8_t *)X->startAddress + X->bLength - 1;

    while (ptr < endPtr)
    {
        swap = *ptr;
        *ptr++ = *endPtr;
        *endPtr-- = swap;
    }
}




