/**
  @Company
    Microchip Technology Inc.

  @File Name
    smart_card_layer3.h

  @Summary
    Smart card library function definitions for contact type cards.

  @Description
    This File contains all the smart card library API definitions pertaining to contact based
    smart card type.
 */

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2014-2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
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

// Smart Card Library
#ifndef _SMART_CARD_LAYER3_H
#define _SMART_CARD_LAYER3_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include "smart_card.h"

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************

/* Protocol Type Supported:

  Summary:
    Transaction Protocol Types

  Description:
    Two types of protocols supported

  Remarks:
    The T=0/T=1 protocols are supported,this enum used in assigning the protocol
    type used in the smart card operation.
 */

typedef enum {
    SMARTCARD_T0_TYPE, //T=0, Protocol is supported
    SMARTCARD_T1_TYPE, //T=1, Protocol is supported
    SMARTCARD_INVALID_TYPE //Other than 0 or 1 its invalid
} SMARTCARD_TRANSACTION_TYPES;

// *****************************************************************************

/* T0 Case types:

  Summary:
    Various cases handled under T=0 are defined

  Description:
    The various cases like short mode, extended mode  are defined

  Remarks:
    None
 */

typedef enum {
    SMARTCARD_UNKNOWN_CASE, //An unknown mode under T=0
    SMARTCARD_CASE_1, // Case 1
    SMARTCARD_CASE_2S, // Case 2 Short Mode
    SMARTCARD_CASE_2E1, // Case 2 Extended Mode(1)
    SMARTCARD_CASE_2E2, // Case 2 Extended Mode(2)
    SMARTCARD_CASE_3S, // Case 3 Short Mode
    SMARTCARD_CASE_3E1, // Case 3 Extended Mode(1)
    SMARTCARD_CASE_3E2, // Case 3 Extended Mode(2)
    SMARTCARD_CASE_4S, // Case 4 Short Mode
    SMARTCARD_CASE_4E1, // Case 4 Extended Mode(1)
    SMARTCARD_CASE_4E2 // Case 4 Extended Mode(2)
} SMARTCARD_T0CASE_TYPES;




// *****************************************************************************

/* Block Types in T=1:

  Summary:
    This  Enum defines the various blocks present in the T=1, Protocol
    Data  Unit (PDU)

  Description:
 In T=1 protocol, the PDU contains various blocks like
    I-block
    R-Block
    S-Block
 The above said blocks are defined.

  Remarks:
    None
 */
typedef enum {
    SMARTCARD_NO_BLOCK,
    SMARTCARD_I_BLOCK, // I Block
    SMARTCARD_R_BLOCK, // R Block
    SMARTCARD_S_BLOCK, // S Block
    SMARTCARD_INVALID_BLOCK // INVALID BLOCK
} SMARTCARD_T1BLOCK_TYPES;



// *****************************************************************************

/* Prologue Field for T=1 Protocol

  Summary:
    This  Structure defines  Prologue field of T=1 protocol

  Description:
    None
  Remarks:
    None
 */
typedef struct {
    uint8_t NAD; // Node Address
    uint8_t PCB; // Protocol Control Byte
    uint16_t length; // LENGTH of I-Field
} SMARTCARD_T1PROLOGUE_FIELD;

// *****************************************************************************
// Section: Smart Card Lib API


/*******************************************************************************
  Function:
    SMARTCARD_TRANSACTION_STATUS SMARTCARD_PPSExchange(uint8_t *ppsPtr)
  
  Description:
    This function does the PPS exchange with the smart card & configures the baud 
    rate of the PIC UART module as per the PPS response from the smart card.

  Precondition:
    SMARTCARD_PowerOnATR was success

  Parameters:
    Input is pointer to PPS string

  Return Values:
    1 if PPS exchange is successful
	
  Remarks:
    This function is called when SMARTCARD_EMV_ATRProcess() returns 1.
 *****************************************************************************/
SMARTCARD_TRANSACTION_STATUS SMARTCARD_PPSExchange(uint8_t *ppsPtr);

/*******************************************************************************
  Function:
    SMARTCARD_TRANSACTION_TYPES SMARTCARD_ProtocolTypeGet(void)

  Description:
    This function gets the type of the protocol supported by the card.

  Precondition:
    SMARTCARD_PowerOnATR was success

  Parameters:
    None

  Return Values:
    SMARTCARD_T0_TYPE, //T=0, Protocol is supported
    SMARTCARD_T1_TYPE  //T=1, Protocol is supported

  Remarks:
    None
 *****************************************************************************/
SMARTCARD_TRANSACTION_TYPES SMARTCARD_ProtocolTypeGet(void);


#ifdef	EMV_SUPPORT
/*******************************************************************************
  Function:
    SMARTCARD_TRANSACTION_STATUS SMARTCARD_EMV_ATRProcess(SMARTCARD_RESET_TYPE resetRequest)

  Description:
    This function performs the power on sequence of the SmartCard and
    interprets the Answer-to-Reset data received from the card.

  Precondition:
    SMARTCARD_Initialize() is called, and card is present

  Parameters:
    resetRequest: type of reset requested by the card

  Return Values:
    1 if Answer to Reset (ATR) was successfully received and processed

  Remarks:
    None
 *****************************************************************************/

SMARTCARD_TRANSACTION_STATUS SMARTCARD_EMV_ATRProcess(SMARTCARD_RESET_TYPES resetRequest);

/*******************************************************************************
  Function:
    SMARTCARD_TRANSACTION_STATUS SMARTCARD_EMV_DataExchangeT0(uint8_t* apduCommand, uint32_t apduCommandLength, SMARTCARD_APDU_RESPONSE* apduResponse)

  Description:
    This function Sends/receives the ISO 7816-4 compliant T = 0 commands to the card.

  Precondition:
    SMARTCARD_PPS was success

  Parameters:
    uint8_t* apduCommand - Pointer to application protocol data unit Field
    uint32_t apduCommandLength - Variable holds the value(in bytes) of command length
    SMARTCARD_APDU_RESPONSE* apduResponse - Pointer to APDU Response structure

  Return Values:
    1 if transaction was success, and followed the ISO 7816-4 protocol.

  Remarks: None
 Example:
    <code>
    main()
    {

        while(!SMARTCARD_IsPresent());
        scError = LoopBackMode(transType);
        ....
        ....
        // Send Command APDU and get Response APDU
         EMV_APDU (transactionType); // trascationType=T0/T1
         ....
         if(transactionType == T0_TYPE)
        {
                return(SMARTCARD_EMV_DataExchangeT0(&apduCmd[0], apduCmdLength, &cardResponse));
        }
        else
        {
                return(SMARTCARD_EMV_DataExchangeT1(&pField,&apduCmd[0],&cardResponse));  //SMARTCARD_EMV_TransactT1
        }

    }
  </code>
 *****************************************************************************/
SMARTCARD_TRANSACTION_STATUS SMARTCARD_EMV_DataExchangeT0(uint8_t* apduCommand, uint32_t apduCommandLength, SMARTCARD_APDU_RESPONSE* apduResponse);

/*******************************************************************************
  Function:
    SMARTCARD_TRANSACTION_STATUS SMARTCARD_EMV_DataExchangeT1(SMARTCARD_T1_PROLOGUE_FIELD* pfield,uint8_t* iField,SMARTCARD_APDU_RESPONSE* apduResponse)

  Description:
    This function Sends/receives the ISO 7816-4 compliant T = 1 commands to the card.

  Precondition:
    SC_PPS was success

  Parameters:
    SMARTCARD_T1_PROLOGUE_FIELD* pfield - Pointer to Prologue Field
    uint8_t* iField - Pointer to the Information Field of Tx/Rx Data
    SMARTCARD_APDU_RESPONSE* apduResponse - Pointer to APDU Response structure

  Return Values:
    1 if transaction was success, and followed the ISO 7816-4 protocol.

  Remarks:None

  Example :
    
    Refer to SMARTCARD_EMV_DataExchangeT0() function
 *****************************************************************************/

SMARTCARD_TRANSACTION_STATUS SMARTCARD_EMV_DataExchangeT1(SMARTCARD_T1PROLOGUE_FIELD* pfield, uint8_t* iField, SMARTCARD_APDU_RESPONSE* apduResponse);

#endif

#endif


/*******************************************************************************
 End of File
 */
