/**
  @Company
    Microchip Technology Inc.

  @File Name
    drv_smart_card_sw.h

  @Summary
    This is the header file for the software based smart card module.

  @Description
    This file contains the driver API's which implements smart card functionality using UART and Timer.
 */

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) <year> released Microchip Technology Inc.  All rights reserved.

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

#ifndef _DRV_SMART_CARD_SW_H
#define _DRV_SMART_CARD_SW_H

#include <stdint.h>
#include <stdbool.h>
#include "smart_card_pps_macro.h"
#include <smart_card.h>


#define T0_PROTOCOL 0
#define T1_PROTOCOL 1

/*******************************************************************************
  Function:
    void DRV_SMARTCARD_Initialize(void)

  Summary:
    Initializes the smart card module.

  Description:
    This function initializes supported UART module.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
 *****************************************************************************/
void DRV_SMARTCARD_Initialize(void);
/*******************************************************************************
  Function:
    void DRV_SMARTCARD_DeInitialize(void)

  Summary:
    De-initializes the smart card module.

  Description:
    This function de-initializes the UART module.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
 *****************************************************************************/
void DRV_SMARTCARD_DeInitialize(void);

/*******************************************************************************
  Function:
    void DRV_SMARTCARD_BRGSet(uint16_t brg)

  Summary:
    Sets baud rate.

  Description:
    This function sets the UART baud rate as per the speed code used in ISO 7816
    standard communication.

  Precondition:
    None

  Parameters:
    brg : baud rate to be set

  Return Values:
    None

  Remarks:
    None
 *****************************************************************************/
void DRV_SMARTCARD_BRGSet(uint16_t brg);


/*******************************************************************************
  Function:
    bool DRV_SMARTCARD_DataGet( uint8_t* pData, uint32_t waitingTimeInETU)

  Summary:
    Receives a byte of data.

  Description:
    This function receives one byte of data as per ISO-7816 standard

  Precondition:
    None.

  Parameters:
    pData - pointer to Data byte to be received
    timeOutInETU - waiting timei ETU

  Return Values:
    Returns TRUE on success else returs FALSE

  Example:
        <code>
        {
            //during ATR receive the TS byte;
            uint8_t ts;
            uint32_t atrDelay = 10080;
            DRV_SMARTCARD_DataGet( &ts , atrDelay );
            // Do something else...
        }
        </code>

  Remarks:
    None.
 *****************************************************************************/
bool DRV_SMARTCARD_DataGet(uint8_t* pDat, uint32_t timeOutETUs);

/*******************************************************************************
  Function:
    bool DRV_SMARTCARD_UART1_DataSend(uint8_t data)

  Summary:
    Sends a byte of data.

  Description:
    This function transmits one byte of data as per ISO-7816 standard

  Precondition:
    None.

  Parameters:
    data - Data byte to be transmitted

  Return Values:
    Returns TRUE on success else returs FALSE

   Example:
        <code>
        {
          //Send the Command Bytes: CLA INS P1 P2
          apduCommandBuffer[0] = CLA;
          apduCommandBuffer[1] = INS;
          apduCommandBuffer[2] = P1;
          apduCommandBuffer[3] = P2;
          uint8_t isLastByte = 0;
          for( index = 0; index < 4; index++ )
          {
                if(index == 3)
                {
                  isLastByte = 1;
                }
                DRV_SMARTCARD_UART1_DataSend( apduCommandBuffer[index] );
          }
          // Do something else...
        }
        </code>

  Remarks:
    None.
 *****************************************************************************/
bool DRV_SMARTCARD_DataSend(uint8_t data, uint8_t isLastByte);

void DRV_SMARTCARD_CGT_Adjustment(uint16_t *cgtInMicroSeconds ,uint8_t tempVarCGT, uint8_t scTC1);
void DRV_SMARTCARD_SetProtocolSelected(uint8_t data);
void WaitMicroSec(int32_t microSec);
void WaitMilliSec(int32_t ms);

#endif

