/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    tftp.h

  Summary:
    

  Description:
    TFTP Client module for Microchip TCP/IP Stack

 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) <2014> released Microchip Technology Inc.  All rights reserved.

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
//DOM-IGNORE-END

#ifndef __TFTPC_H_
#define __TFTPC_H_

#if defined(STACK_USE_TFTP_CLIENT)

// Number of seconds to wait before declaring TIMEOUT error on Get.
#define TFTP_GET_TIMEOUT_VAL  (3u * TICKS_PER_SECOND)

// Number of seconds to wait before declaring TIMEOUT error on Put
#define TFTP_ARP_TIMEOUT_VAL  (3u * TICKS_PER_SECOND)

// Number of attempts before declaring TIMEOUT error.
#define TFTP_MAX_RETRIES  (3u)

// Retry count must be 1 or more.
#if TFTP_MAX_RETRIES <= 0u
#error Retry count must at least be 1
#endif

// Enum. of results returned by most of the TFTP functions.
typedef enum _TFTP_RESULT {
    TFTP_OK = 0,
    TFTP_NOT_READY,
    TFTP_END_OF_FILE,
    TFTP_ERROR,
    TFTP_RETRY,
    TFTP_TIMEOUT
} TFTP_RESULT;

// File open mode as used by TFTPFileOpen().
typedef enum _TFTP_FILE_MODE {
    TFTP_FILE_MODE_READ = 1,
    TFTP_FILE_MODE_WRITE = 2
} TFTP_FILE_MODE;

// Standard error codes as defined by TFTP spec.
// Use to decode value retuned by TFTPGetError().
typedef enum _TFTP_ACCESS_ERROR {
    TFTP_ERROR_NOT_DEFINED = 0,
    TFTP_ERROR_FILE_NOT_FOUND,
    TFTP_ERROR_ACCESS_VIOLATION,
    TFTP_ERROR_DISK_FULL,
    TFTP_ERROR_INVALID_OPERATION,
    TFTP_ERROR_UNKNOWN_TID,
    TFTP_ERROR_FILE_EXISTS,
    TFTP_ERROR_NO_SUCH_USE
} TFTP_ACCESS_ERROR;

// Status codes for TFTPGetUploadStatus() function.  Zero means upload success, >0 means working and <0 means fatal error.
#define TFTP_UPLOAD_COMPLETE               0
#define TFTP_UPLOAD_GET_DNS                1
#define TFTP_UPLOAD_RESOLVE_HOST           2
#define TFTP_UPLOAD_CONNECT                3
#define TFTP_UPLOAD_SEND_FILENAME          4
#define TFTP_UPLOAD_SEND_DATA              5
#define TFTP_UPLOAD_WAIT_FOR_CLOSURE       6
#define TFTP_UPLOAD_HOST_RESOLVE_TIMEOUT  -1
#define TFTP_UPLOAD_CONNECT_TIMEOUT       -2
#define TFTP_UPLOAD_SERVER_ERROR          -3

typedef struct {
    uint8_t *vDataPointer;
    uint16_t wDataLength;
} TFTP_CHUNK_DESCRIPTOR;

void TFTPOpen(IP_ADDR *host);
TFTP_RESULT TFTPIsOpened(void);
void TFTPOpenFile(uint8_t *fileName, TFTP_FILE_MODE mode);

#if defined(__XC8)
// PIC18 ROM argument implementation of TFTPOpenFile
void TFTPOpenROMFile(ROM uint8_t *fileName, TFTP_FILE_MODE mode);
#else
#define TFTPOpenROMFile(a,b)    TFTPOpenFile((uint8_t *)(a),b)
#endif

TFTP_RESULT TFTPIsFileOpened(void);
void TFTPCloseFile(void);
TFTP_RESULT TFTPIsFileClosed(void);
TFTP_RESULT TFTPIsGetReady(void);
uint8_t TFTPGet(void);
TFTP_RESULT TFTPIsPutReady(void);
void TFTPPut(uint8_t c);

void TFTPUploadRAMFileToHost(ROM uint8_t *vRemoteHost, ROM uint8_t *vFilename, uint8_t *vData, uint16_t wDataLength);
void TFTPUploadFragmentedRAMFileToHost(ROM uint8_t *vRemoteHost, ROM uint8_t *vFilename, TFTP_CHUNK_DESCRIPTOR *vFirstChunkDescriptor);
int8_t TFTPGetUploadStatus(void);

/*********************************************************************
 * Macro:           void TFTPClose(void)
 *
 * PreCondition:    TFTPOpen is already called and TFTPIsOpened()
 *                  returned TFTP_OK.
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Closes TFTP client socket.
 *
 * Note:            Once closed, application must do TFTPOpen to
 *                  perform any new TFTP operations.
 *
 *                  If TFTP server does not change during application
 *                  life-time, one may not need to call TFTPClose
 *                  and keep TFTP socket open.
 ********************************************************************/
#define TFTPClose(void)  UDPClose(_tftpSocket)
extern UDP_SOCKET _tftpSocket;

/*********************************************************************
 * Macro:           bool TFTPIsFileOpenReady(void)
 *
 * PreCondition:    TFTPOpen is already called and TFTPIsOpened()
 *                  returned TFTP_OK.
 *
 * Input:           None
 *
 * Output:          true, if it is ok to call TFTPOpenFile()
 *                  false, if otherwise.
 *
 * Side Effects:    None
 *
 * Overview:        Checks to see if it is okay to send TFTP file
 *                  open request to remote server.
 *
 * Note:            None
 ********************************************************************/
#define TFTPIsFileOpenReady()  UDPIsPutReady(_tftpSocket)

/*********************************************************************
 * Macro:           uint16_t TFTPGetError(void)
 *
 * PreCondition:    One of the TFTP function returned with
 *                  TFTP_ERROR result.
 *
 * Input:           None
 *
 * Output:          Error code as returned by remote server.
 *                  Application may use TFTP_ACCESS_ERROR enum. to
 *                  decode standard error code.
 *
 * Side Effects:    None
 *
 * Overview:        Returns previously saved error code.
 *
 * Note:            None
 ********************************************************************/
#define TFTPGetError()  (_tftpError)

extern uint16_t _tftpError;

#endif // #if defined(STACK_USE_TFTP_CLIENT)

#endif
