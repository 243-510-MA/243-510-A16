/**
  @Company
    Microchip Technology Inc.

  @File Name
    smart_card.h

  @Summary
    SMart card library generic function definitions.

  @Description
    This File contains all the smart card library generic API definitions which is
    common for both contact and contact-less based smart cards.
 */

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2014-2015 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip micro controller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sub license terms in the accompanying license agreement).

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
#ifndef _SMART_CARD_H
#define _SMART_CARD_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************

/* Reset Type:

  Summary:
    Reset Types

  Description:
    This Enum holds the type of Reset provided

  Remarks:
    The enum type signifies the type of reset given by smart card operation
 */

typedef enum {
    SMARTCARD_COLD_RESET, // Cold Reset (Done only during Power ON)
    SMARTCARD_WARM_RESET // Warm Reset
}SMARTCARD_RESET_TYPES;




// *****************************************************************************

/* PPS support Status:

  Summary:
    Protocol and Parameter Selections (PPS) status are defined

  Description:
    PPS support status states are defined

  Remarks:
    provides various states for PPS support to track during protocol transaction
 */
typedef enum {
    SMARTCARD_PPS_NOT_ALLOWED, // Protocol Parameter Selection(PPS) not allowed (Specific Mode)
    SMARTCARD_PPS_ALLOWED, // Supports Parameter Parameter Selection (Negotiable Mode)
    SMARTCARD_PPS_ALLOWED_AFTER_WARM_RESET // Supports Parameter Parameter Selection only after warm reset
} SMARTCARD_PPS_SUPPORT_STATUS;


// *****************************************************************************

/* Smart card ATR Status:

  Summary:
    This  Enum defines Answer To Reset(ATR) status

  Description:
    It shows the whether ATR is ON or unknown byte
  Remarks:
    None
 */
typedef enum {
    SMARTCARD_UNKNOWN, // Indicates the state before the reset of smartcard protocol
    SMARTCARD_ATR_ON // Indicates the state after reset of smartcard protocol
} SMARTCARD_STATUS;


// *****************************************************************************
// *****************************************************************************
// Section: SmartCard APDU Response structure 7816-4
// *****************************************************************************
// *****************************************************************************


// *****************************************************************************

/* SmartCard APDU Response structure 7816-4

  Summary:
    This  Structure defines APDU response packet

  Description:
    The structure defines the response packet definition of various data like
    CLA, INS, P1, P2,LC and LE
  Remarks:
    None
 */
typedef struct {
    uint8_t CLA; // CLA Field :Command class
    uint8_t INS; // INS Field :Instruction Operation code
    uint8_t P1; // P1 Field : Selection Mode
    uint8_t P2; // P2 Field : Selection Option
    uint8_t LC; // LC Field : Data length
    uint8_t LE; // LE Field : Expected length of data to be returned
} SMARTCARD_APDU_COMMAND;



// *****************************************************************************

/* SmartCard APDU Response structure 7816-4

  Summary:
    This  Structure defines  response packet with status bytes

  Description:
     The APDU response byte which can hold 512 bytes of data and two status byte
  Remarks:
    None
 */
typedef struct {
    uint16_t rxDataLen; // Received Data length from smart card(excluding SW1 and SW2 bytes)
    uint8_t apduData[512]; // Application Protocol Data unit (APDU) max size in bytes is 512
    uint8_t SW1; // Status byte 1
    uint8_t SW2; // Status byte 2
} SMARTCARD_APDU_RESPONSE;


// *****************************************************************************

/* Smart Card Error Types:

  Summary:
    Definition of various Error Types

  Description:
    During the protocol transaction various Errors could be encountered.
    This Enum defines the various errors states and it's interpretations.

  Remarks:
    None
 */

typedef enum {
    SMARTCARD_TRANSACTION_SUCCESSFUL = 1, // No Error
    SMARTCARD_ERROR_CARD_NOT_SUPPORTED = -16, // Card Not Supported
    SMARTCARD_ERROR_ATR_DATA, // ERROR in Answer-To-Reset (ATR) data received from the card
    SMARTCARD_ERROR_NO_ATR_RESPONSE, // No ATR Response from the card
    SMARTCARD_ERROR_CMD_APDU_T0, // Wrong T0 Command Application Protocol Data Unit (APDU)
    SMARTCARD_ERROR_CMD_APDU_T1, // Wrong T1 Command Application Protocol Data Unit (APDU)
    SMARTCARD_ERROR_CARD_NOT_PRESENT, // Card Not present in the slot
    SMARTCARD_ERROR_CARD_NO_RESPONSE, // No response from the card
    SMARTCARD_ERROR_CARD_VPP, // VPP Error received from the card (Voltage Mismatch/Programming Voltage not supported)
    SMARTCARD_ERROR_PROCEDURE_BYTE, // Incorrect Procedure Byte from the card
    SMARTCARD_ERROR_PPS, // Unsuccessful Protocol and Parameter Select (PPS) Exchange
    SMARTCARD_ERROR_RECEIVE_CRC, // CRC Error in the block received from the card
    SMARTCARD_ERROR_RECEIVE_LRC, // Longitudinal Redundancy check (LRC) Error in the block received from the card
    SMARTCARD_ERROR_TRANSMIT, // Transmission of byte to smart card failed
    SMARTCARD_ERROR_T1_RETRY, // Retry for T1 also Unsuccessful
    SMARTCARD_ERROR_T1_S_BLOCK_RESPONSE, // ERROR in T1 'S' Block Response
    SMARTCARD_ERROR_T1_INVALID_BLOCK,  //Invalid block

} SMARTCARD_TRANSACTION_STATUS;


// *****************************************************************************
/* Smart Card General definitions:

  Summary:
    Definition of various general definitions used under smart card

  Description:
    During the protocol transaction various Errors could be encountered.
    This Enum defines the various errors states and it's interpretations.

  Remarks:
    None
 */

// *****************************************************************************
// *****************************************************************************
// Section: Smart Card Commands during Protocol Transaction
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************




// *****************************************************************************
// *****************************************************************************
// Section: Smart Card Lib API
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
        void SMARTCARD_Initialize(void)
	
  Description:
        This function initializes the smart card library

  Precondition:
        None

  Parameters:
        None

  Return Values:
        None
	
  Remarks:
        None
 *****************************************************************************/
void SMARTCARD_Initialize(void);



/*******************************************************************************
  Function:
        bool SMARTCARD_IsPresent(void)
  
  Description:
        This macro checks if card is inserted in the socket

  Precondition:
        SMARTCARD_Initialize() is called

  Parameters:
       None

  Return Values:
       1 if Card is inserted, otherwise return 0
	
  Remarks:
       None

  Example:
    <code>
    main())
    {
        SMARTCARD_Initialize();
        while(!SMARTCARD_IsPresent());
        // Do other tasks only when the card is detected
    }
    </code>
 ******************************************************************************/
bool SMARTCARD_IsPresent(void);



/*******************************************************************************
  Function:
    SMARTCARD_STATUS SMARTCARD_StateGet(void)
	
  Description:
    This function returns the current state of SmartCard

  Precondition:
    SMARTCARD_Initialize is called.

  Parameters:
        None

  Return Values:
    SMARTCARD_UNKNOWN:  No Card Detected
    SMARTCARD_ATR_ON:       Card is powered and ATR received
	
  Remarks:
    None
  Example:
    <code>
    main())
    {
		SMARTCARD_Initialize();
        if(!SMARTCARD_StateGet());
        // Checks for Card ATR_ON state or Unknown state
        // If in ATR_ON state, then card can behave in normal manner, can communicate.
        // If in Unknown state the communication would time out and reset.
    }
    </code>
 *****************************************************************************/
SMARTCARD_STATUS SMARTCARD_StateGet(void);

/*******************************************************************************
  Function:
    SMARTCARD_TRANSACTION_STATUS SMARTCARD_PowerOnATR(SMARTCARD_RESET_TYPE resetRequest)
  
  Description:
    This function performs the power on sequence of the SmartCard and
    interprets the Answer-to-Reset data received from the card.

  Precondition:
    SMARTCARD_Initialize() is called, and card is present

  Parameters:
    None

  Return Values:
    1 if Answer to Reset (ATR) was successfully received and processed
	
  Remarks:
    None

  Example:
    <code>
     {

        while(!SMARTCARD_IsPresent());
                ...
                ...
                contact = TypeOfCard();
        if(contact)
                {
                        return(SMARTCARD_PowerOnATR); //Contact based Smart card
                }
                else
                {
                        return(return(SCL_PowerOnATR););  //Contact-less based Smart Card
                }

    }
  </code>
 *****************************************************************************/
SMARTCARD_TRANSACTION_STATUS SMARTCARD_PowerOnATR(SMARTCARD_RESET_TYPES resetRequest);

/*******************************************************************************
  Function:
    uint8_t SMARTCARD_DataExchange(SMARTCARD_APDU_COMMAND* apduCommand)
  
  Description:
    This function performs the  data transaction, by calling the appropriate routine 
    based upon card type.

  Precondition:
    SMARTCARD_Initialize() is called, and card is present

  Parameters:
    None

  Return Values:
    1 if Answer to Reset (ATR) was successfully received and processed
	
  Remarks:
    None

  Example:
    <code>
     {

        while(!SMARTCARD_IsPresent());
                ...
                ...
                contact = TypeOfCard();
        if(contact)
                {
                        SMARTCARD_EMV_T0();
                }
                else
                {
                        ...
                }

        }
  </code>
 *****************************************************************************/
uint8_t SMARTCARD_DataExchange(SMARTCARD_APDU_COMMAND* apduCommand);
/*******************************************************************************
  Function:
    void SMARTCARD_Shutdown(void)
	
  Description:
    This function Performs the Power Down sequence of the SmartCard

  Precondition:
    SMARTCARD_Initialize is called.

  Parameters:
    None

  Return Values:
    None
	
  Remarks:
    None
  Example:

   <code>
   statement1;
    if (resetRequest == WARM_RESET)
    {
        SMARTCARD_Shutdown();
    }
    ...
    ...
    ...

    // Not a Valid ATR Response
    scTransactionStatus = SC_ERR_BAR_OR_NO_ATR_RESPONSE;
    SMARTCARD_Shutdown();
    ...
    ...
    // Return False if there is no card inserted in the Slot or ATR of the card is unsuccessful
    if( !SCdrv_CardPresent() || (gCardState != ATR_ON ))
    {
        SMARTCARD_Shutdown();
        return SC_ERR_CARD_NOT_PRESENT;
    }
    </code>
 *****************************************************************************/
void SMARTCARD_Shutdown(void);

/*******************************************************************************
  Function:
    void SMARTCARD_IsPPSSupported(void)

  Description:
    This function gets whether PPS(Protocol & Parameter Selection) is supported or not

  Precondition:
    SMARTCARD_Initialize is called and CARD is in ATR on state.

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
  Example:

   <code>
   statement1;
    if (resetRequest == WARM_RESET)
    {
        SMARTCARD_Shutdown();
    }
    ...
    ...
    ...

    SMARTCARD_PowerOnATR();
    ...
    ...
    // Return False if there is no card inserted in the Slot or ATR of the card is unsuccessful
    if( !SCdrv_CardPresent() || (gCardState != ATR_ON ))
    {
        SMARTCARD_Shutdown();
        return SC_ERR_CARD_NOT_PRESENT;
    }
    </code>
 *****************************************************************************/
SMARTCARD_PPS_SUPPORT_STATUS SMARTCARD_IsPPSSupported(void);
#endif


/*******************************************************************************
 End of File
 */
