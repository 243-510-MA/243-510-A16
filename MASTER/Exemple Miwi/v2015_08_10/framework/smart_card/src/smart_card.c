/********************************************************************
 FileName:		smart_card_lib.c
 Dependencies:	See INCLUDES section
 Processor:		PIC18,PIC24,PIC32 & dsPIC33F Microcontrollers
 Hardware:		This demo is natively intended to be used on Exp 16, LPC
 				& HPC Exp board. This demo can be modified for use on other hardware
 				platforms.
 Complier:  	Microchip C18 (for PIC18), C30 (for PIC24 & dsPIC) & C32 (for PIC32) 
 Company:		Microchip Technology, Inc.

 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the “Company”) for its PIC® Microcontroller is intended and
 supplied to you, the Company’s customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN “AS IS” CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.

********************************************************************/
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <xc.h>
#include "smart_card_layer3.h"
#include "system_config.h"
#include "drv_smart_card_sw.h"


#define MAX_ATR_LEN                     ( uint8_t ) 33
#define GET_RESPONSE_INS                ( uint8_t ) 0xC0
#define SMARTCARD_ATR_INTERVAL_10080    ( uint16_t ) 10080


#ifdef EMV_SUPPORT
extern volatile uint8_t emvDelayLapsedFlag;
extern volatile uint8_t delayLapsedFlag;
#endif
// *****************************************************************************
// *****************************************************************************
// Section: Global Declarations of variables.
// *****************************************************************************
// *****************************************************************************
/* Smart Card General definitions:

  Summary:
    Definition of various general definitions used under smart card

  Description:
    During the protocol transaction  operations,communication bytes would be exchanged.
 These are aggregated in this enumeration data type

  Remarks:
    None
 */
typedef enum
{
    SMARTCARD_LRC_TYPE_EDC              = 0,    // Longitudinal Redundancy Check(LRC) type is used for EDC in Epilogue Field
    SMARTCARD_CRC_TYPE_EDC              = 1,    // Cyclic Redundancy Check(CRC) type is used for EDC in Epilogue Field
    SMARTCARD_RESYNC_REQ                = 0xC0, // PCB byte for Resync Request of T1 Protocol
    SMARTCARD_RESYNC_RESPONSE           = 0xE0, // PCB byte for Resync Response of T1 Protocol
    SMARTCARD_ABORT_REQUEST             = 0xC2, // PCB byte for Abort Request of T1 Protocol
    SMARTCARD_ABORT_RESPONSE            = 0xE2, // PCB byte for Abort Response of T1 Protocol
    SMARTCARD_IFS_REQUEST               = 0xC1, // PCB byte for IFS Request of T1 Protocol
    SMARTCARD_IFS_RESPONSE              = 0xE1, // PCB byte for IFS Response of T1 Protocol
    SMARTCARD_VPP_ERROR_RESPONSE        = 0x04,
    SMARTCARD_WAIT_TIME_EXT_REQUEST     = 0xC3,
    SMARTCARD_WAIT_TIME_EXT_RESPONSE    = 0xE3,
    SMARTCARD_INVERSE_CONVENTION        = 0x03,
    SMARTCARD_DIRECT_CONVENTION         = 0x3B,
    SMARTCARD_CHECK_TA1_PRESENCE        = 0x10,
    SMARTCARD_MIN_DLY_OF_12_ETU_T1      = 0xFF,
    SMARTCARD_MIN_CHAR_TO_CHAR_DURATN_T1= 0x0B,
    SMARTCARD_NULL_NAD_ADDRESS          = 0x00,
    SMARTCARD_STATUS_BYTE_T0_60         = 0x60,
    SMARTCARD_STATUS_BYTE_T0_90         = 0x90,
    SMARTCARD_SECOND_PROC_BYTE_6C       = 0x6C,
    SMARTCARD_SECOND_PROC_BYTE_61       = 0x61,
    SMARTCARD_WI                        = 0x0A, // DEFAULT Value of WI Indicator used in calculation of WWT for T=0 protocol
    SMARTCARD_CWI                       = 0x0C, // DEFAULT Value of CWI Indicator used in calculation of CWT for T=1 protocol
    SMARTCARD_BWI                       = 0x04, // DEFAULT Value of BWI Indicator used in calculation of BWT for T=1 protocol
    SMARTCARD_CLK_CONV_INTEGER_372      = 0x11, // Smart card Frequency selection
    SMARTCARD_TERMINAL_ACCEPT_ATR       = 0x0A,
    SMARTCARD_EMV_MAX_IFS_LENGTH        = 0xFE,
    SMARTCARD_EMV_MIN_IFS_LENGTH        = 0x10,
    SMARTCARD_R_BLOCK_BIT_SIX           = 0x20,
    SMARTCARD_I_BLOCK_MASK              = 0x80,
    SMARTCARD_S_BLOCK_MASK              = 0xC0,
    SMARTCARD_R_BLOCK_MASK              = 0xC0,
    SMARTCARD_I_BLOCK_IDENTIFIER        = 0x00,
    SMARTCARD_R_BLOCK_IDENTIFIER        = 0x80,
    SMARTCARD_S_BLOCK_IDENTIFIER        = 0xC0,
    SMARTCARD_R_BLOCK_S_BIT_SET         = 0x10,
    SMARTCARD_S_BLOCK_REQ_RESP_BIT      = 0x20,
    SMARTCARD_M_BIT_SET                 = 0x20,
    SMARTCARD_M_BIT_CLR                 = 0xDF,
    SMARTCARD_S_BIT_SET                 = 0x40,
    SMARTCARD_S_BIT_CLR                 = 0xBF,
    SMARTCARD_ATR_MAX_CLK_CYCLES        = 0xB3B0,
    SMARTCARD_ATR_INIT_CLK_CYCLES       = 0x4F10,
    SMARTCARD_ATR_INT_CLK_CYCLE_ERROR   = 0x780,
    SMARTCARD_MIN_BITRATE_ADJ_FACTOR    = 0x10,
    SMARTCARD_MAX_BITRATE_ADJ_FACTOR    = 0x14,
    SMARTCARD_IFD_BITRATE_ADJ_FACTOR    = 0xD6,
    SMARTCARD_MAX_PPS_LENGTH            = 0x07,
    SMARTCARD_ETU_11                    = 0x0B,
    SMARTCARD_ETU_12                    = 0x0C,
    SMARTCARD_CLK_DATE_CONV_FACTOR_372  = 0x174,
} SMARTCARD_PROTOCOL_RELATED_DEFINITIONS;

typedef enum
{
    SMARTCARD_BITMASK_1             = 0x01,
    SMARTCARD_BITMASK_2             = 0x02,
    SMARTCARD_BITMASK_3             = 0x04,
    SMARTCARD_BITMASK_4             = 0x08,
    SMARTCARD_BITMASK_5             = 0x10,
    SMARTCARD_BITMASK_6             = 0x20,
    SMARTCARD_BITMASK_7             = 0x40,
    SMARTCARD_BITMASK_8             = 0x80,
    SMARTCARD_BITMASK_LOWER_NIBBLE  = 0x0F,
    SMARTCARD_BITMASK_HIGHER_NIBBLE = 0xF0,
    SMARTCARD_BITMASK_BYTE          = 0xFF,
}SMARTCARD_BITMASKS;

typedef enum
{
    NO_ERROR,
    PARITY_LRC_ERROR,
    OTHER_ERROR
}SMARTCARD_ERROR_TYPES;

SMARTCARD_ERROR_TYPES               errType = NO_ERROR;

uint8_t                             factorDNumerator = 1;
uint8_t                             factorDdenominator = 1;
uint8_t                             scCardATR[MAX_ATR_LEN];
uint8_t                             scATRLength = 0;
uint8_t                             scTA1PresentFlag, scTB1PresentFlag;
uint8_t                             scTA2PresentFlag, scTB2PresentFlag, scTC2PresentFlag, scTD2PresentFlag;
uint8_t                             scTA3PresentFlag, scTB3PresentFlag, scTC3PresentFlag;
bool                                inverseConventionFlag;
uint8_t                             scTS, scT0, scTCK;
uint8_t                             scTA1 = 0x11, scTA2, scTA3 = 0x20;
uint8_t                             scTB1, scTB2, scTB3;
uint8_t                             scTC1, scTC2 = 0x0A, scTC3;
uint8_t                             scTD1, scTD2, scTD3;

uint8_t                             *scATR_HistoryBuffer = NULL;
uint8_t                             scATR_HistoryLength = 0;
uint8_t                             scPPSResponseLength;
uint8_t                             scPPSResponse[7];


bool                             deactivationEnabled = false;
bool                             deactivateOnIFSLenErrorEnabled = false;
bool                             bitRateAdjuFactor6Supported = true;
bool                             resyncEnabled = false;
bool                             abortResponseBeforeDeactivation = false;

uint16_t                            atrDelayETUs;
uint16_t                            factorF = SMARTCARD_CLK_DATE_CONV_FACTOR_372;

SMARTCARD_STATUS                    gCardState = SMARTCARD_UNKNOWN;

// Work Wait time for T=0 Protocol in units of etu's
uint32_t                            t0WWTetu;

static void                         SMARTCARD_FindFDvalues( uint8_t tA1Type );
static void                         SMARTCARD_CalculateWaitTime( void );
static void                         SMARTCARD_ColdReset( void );
static void                         SMARTCARD_WarmReset( void );
static void                         SMARTCARD_InitATR_Variables( void );
static bool                         ValidateATRData(SMARTCARD_RESET_TYPES resetRequest);

#ifdef SMARTCARD_SUPPORT_IN_HARDWARE
DRV_SMARTCARD_ATR_CONFIG            atrConfig;
#endif
static SMARTCARD_TRANSACTION_STATUS scTransactionStatus = SMARTCARD_TRANSACTION_SUCCESSFUL;

#ifdef SMARTCARD_PROTO_T1
uint32_t                            t1BWTetu;
uint16_t                            t1CWTetu;
uint32_t                            currT1BWTetu;
uint8_t                             t1BGTetu = 22;
uint8_t                             edcType = ( uint8_t ) SMARTCARD_LRC_TYPE_EDC;
uint8_t                             maxSegmentLength = 0x20;
bool                                txSbit = true;
SMARTCARD_T1BLOCK_TYPES             currentT1RxBlockType;

#if !defined( EMV_SUPPORT )
static uint16_t                     SMARTCARD_UpdateCRC( uint8_t data, uint16_t crc );
static void                         SMARTCARD_UpdateEDC( uint8_t data, uint16_t *edc );
#endif
static void                         SMARTCARD_SendT1Block( uint8_t nad, uint8_t pcb, uint8_t length, uint8_t *buffer );
static bool                         SMARTCARD_ReceiveT1Block( uint8_t *rxNAD, uint8_t *rxPCB, uint8_t *rxLength,
                                                              uint8_t *buffer );
#endif

// Character Guard Time for T=0 & T=1 Protocol
uint16_t                            cgtETU;
uint16_t                            cgtInMicroSeconds;
uint16_t                            oppTimeInMicroSeconds;

static uint8_t                      prevIFS = 0;
static bool                         rxSbitPrevIBlk = true;

extern uint8_t                      countfunc;
extern uint32_t                     baudRate;
extern uint32_t                     scReferenceClock;
extern bool                         parityErrorFlag;
extern bool                         cwtExceeded;
extern uint16_t                     oneETUtimeinMicroSeconds;
extern uint16_t                     point3ETUtimeinMicroSeconds;
extern uint16_t                     point05ETUtimeinMicroSeconds;


// CLA set to '00' = no command chaining,
//                   no secure messaging,
//					 basic logical channel.

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
void SMARTCARD_Initialize( void )
{
    //Initialize the low level driver
    DRV_SMARTCARD_Initialize();
    #ifdef SMARTCARD_PROTO_T1

    // Initialize smart card library variables
    txSbit = true;
    #endif
}

#if 0

// ISO standard based function
/*******************************************************************************
  Function:
	uint8_t SMARTCARD_PowerOnATR(SMARTCARD_RESET_TYPE resetRequest)
  
  Description:
    This function performs the power on sequence of the SmartCard and 
	interprets the Answer-to-Reset data received from the card.

  Precondition:
    SMARTCARD_Initialize() is called, and card is present

  Parameters:
    resetRequest: type of reset requested by the card

  Return Values:
    1 if Answer to Reset (ATR) was successfuly received and processed
	
  Remarks:
    None
  *****************************************************************************/
SMARTCARD_TRANSACTION_STATUS SMARTCARD_PowerOnATR( SMARTCARD_RESET_TYPES resetRequest )
{
    SMARTCARD_InitATR_Variables();

    //check whether card is present before doing power on reset to the card
    if( !SMARTCARD_Drv_CardPresent() )
    {
        scTransactionStatus = SMARTCARD_ERROR_CARD_NOT_PRESENT;
        return ( scTransactionStatus );
    }

    scTransactionStatus = SMARTCARD_TRANSACTION_SUCCESSFUL;

        #ifdef EMV_SUPPORT

    // Wait for maximum of 42,000 smard card clock cycles
    // to get an ATR from card.The number of clock cycles is converted
    // into etu's for easier usage in the code.
    atrDelayETUs = ( uint16_t ) ( (42000UL * baudRate) / scReferenceClock );

        #else

    // Wait for maximum of 40,000 smard card clock cycles
    // to get an ATR from card.The number of clock cycles is converted
    // into etu's for easier usage in the code.
    atrDelayETUs = ( uint16_t ) ( (40000UL * baudRate) / scReferenceClock );
        #endif
    if( resetRequest == SMARTCARD_WARM_RESET )
    {
        SMARTCARD_WarmReset();
    }
    else
    {
        SMARTCARD_ColdReset();
    }

        #ifdef EMV_SUPPORT

    //The terminal shall be able to receive an ATR having a duration of less than or equal to 20160 initial etus.
    SMARTCARD_Drv_EnableDelayTimerIntr45();

    SMARTCARD_Drv_SetDelayTimerCnt45( ((FCY / baudRate) * 20160UL) );

    SMARTCARD_Drv_EnableDelayTimer45();
        #endif
    if( DRV_SMARTCARD_DataGet(&scCardATR[scATRLength], atrDelayETUs) )
    {
        scATRLength++;

            #ifdef EMV_SUPPORT
        atrDelayETUs = SMARTCARD_ATR_INTERVAL_10080;
            #else
        atrDelayETUs = ( uint16_t ) 9600;
            #endif

        // Read Answer to RESET
            #ifdef EMV_SUPPORT
        while( emvDelayLapsedFlag == 0 )
                #else
            while( 1 )
                    #endif
            {
                //wait for data byte from CARD
                if( DRV_SMARTCARD_DataGet(&scCardATR[scATRLength], atrDelayETUs) )
                {
                    scATRLength++;

                    if( scATRLength == MAX_ATR_LEN )
                    {
                        break;
                    }
                }
                else
                {
                    break;      //no data
                }
            }
    }

        #ifdef EMV_SUPPORT
    SMARTCARD_Drv_DisableDelayTimer45();
    emvDelayLapsedFlag = 0;
        #endif

    //decode the ATR values
        #ifdef EMV_SUPPORT
    if( (scATRLength >= 3) && !parityErrorFlag )
            #else
        if( scATRLength >= 3 )  //min TS, T0 and setup byte
                #endif
        {
            uint8_t T0 = scCardATR[1];
            uint8_t atrIdx = 2;

            //Extract Interface bytes TAx TBx TCx and TDx from ATR
            scTA1 = 0x11;
            scTB1 = scTC1 = scTD1 = 0;
            scTA2 = scTB2 = scTD2 = 0;
            scTA3 = 0x20;
            scTC2 = 0x0A;
            scTB3 = scTC3 = scTD3 = 0;

            // Read the global interface bytes
            if( T0 & 0x10 )
            {
                scTA1 = scCardATR[atrIdx++];
            }

            if( T0 & 0x20 )
            {
                scTB1 = scCardATR[atrIdx++];
            }

            if( T0 & 0x40 )
            {
                scTC1 = scCardATR[atrIdx++];
            }

            if( T0 & 0x80 )
            {
                scTD1 = scCardATR[atrIdx++];
            }

            //read the next set of interface bytes if present
            if( scTD1 & 0xF0 )
            {
                if( scTD1 & 0x10 )
                {
                    scTA2 = scCardATR[atrIdx++];
                    scTA2PresentFlag = 1;
                }

                if( scTD1 & 0x20 )
                {
                    scTB2 = scCardATR[atrIdx++];
                }

                if( scTD1 & 0x40 )
                {
                    scTC2 = scCardATR[atrIdx++];
                }

                if( scTD1 & 0x80 )
                {
                    scTD2 = scCardATR[atrIdx++];
                }

                if( scTD2 & 0xF0 )
                {
                    if( scTD2 & 0x10 )
                    {
                        scTA3 = scCardATR[atrIdx++];

                        if( (scTA3 < 0x10) || (scTA3 == 0xFF) )
                        {
                            SMARTCARD_Shutdown();
                            scTransactionStatus = SMARTCARD_ERROR_ATR_DATA;
                            return ( scTransactionStatus );
                        }

                            #ifdef SC_PROTO_T1
                        maxSegmentLength = scTA3;
                            #endif
                    }

                    if( scTD2 & 0x20 )
                    {
                        scTB3 = scCardATR[atrIdx++];
                    }

                    if( scTD2 & 0x40 )
                    {
                        scTC3 = scCardATR[atrIdx++];

                            #ifdef SC_PROTO_T1
                        edcType = ( scTC3 & 0x01 ) ? ( uint8_t ) SMARTCARD_CRC_TYPE_EDC : ( uint8_t ) SMARTCARD_LRC_TYPE_EDC;
                            #endif
                    }

                    if( scTD2 & 0x80 )
                    {
                        scTD3 = scCardATR[atrIdx++];
                    }
                }
            }

            scATR_HistoryLength = T0 & 0x0F;
            scATR_HistoryBuffer = ( scATR_HistoryLength ) ? ( &scCardATR[atrIdx] ) : NULL;

            // Calculate the protocol wait times for default values
            SMARTCARD_CalculateWaitTime();
            gCardState = SMARTCARD_ATR_ON;
            atrConfig.cardState = SMARTCARD_ATR_ON;

            return ( scTransactionStatus );
        }
        else
        {
            // Not a Valid ATR Reponse
            scTransactionStatus = SMARTCARD_ERROR_NO_ATR_RESPONSE;
            SMARTCARD_Shutdown();
            return ( scTransactionStatus );
        }
}

#endif

/*******************************************************************************
  Function:
	static void SMARTCARD_InitATR_Variables(void)
  
  Description:
    This function performs the power on sequence of the SmartCard and 
	interprets the Answer-to-Reset data received from the card.

  Precondition:
    SMARTCARD_Initialize() is called, and card is present

  Parameters:
    resetRequest: type of reset requested by the card

  Return Values:
    1 if Answer to Reset (ATR) was successfuly received and processed
	
  Remarks:
    None
  *****************************************************************************/
static void SMARTCARD_InitATR_Variables( void )
{
    gCardState = SMARTCARD_UNKNOWN; // intializing to Zero
    scTA2PresentFlag = 0;
    parityErrorFlag = false;
    inverseConventionFlag = false;

    memset( scCardATR, 0x00, sizeof(scCardATR) );

    scATR_HistoryLength = 0;
    scATR_HistoryBuffer = NULL;
    scATRLength = 0;

    scTA1PresentFlag = scTB1PresentFlag = 0;
    scTA2PresentFlag = scTB2PresentFlag = scTC2PresentFlag = scTD2PresentFlag = 0;
    scTA3PresentFlag = scTB3PresentFlag = scTC3PresentFlag = 0;

    scTA1 = ( uint8_t ) SMARTCARD_CLK_CONV_INTEGER_372;
    scTB1 = scTC1 = scTD1 = 0;
    scTA2 = scTB2 = scTD2 = 0;
    scTC2 = ( uint8_t ) SMARTCARD_TERMINAL_ACCEPT_ATR;      // Terminal accepts an ATR TC2=0x0A
    scTA3 = 0x20;
    scTB3 = scTC3 = scTD3 = 0;
}

/*******************************************************************************
  Function:
	SMARTCARD_TRANSACTION_STATUS SMARTCARD_EMV_ATRProcess(SMARTCARD_RESET_TYPE resetRequest)
  
  Description:
    This function checks and validates the Answer-To-Reset(ATR) command
    bytes  and accordingly calls the functions.

  Precondition:
    SMARTCARD_Initialize() is called, and card is present

  Parameters:
    resetRequest: type of reset requested by the card

  Return Values:
    1 if Answer to Reset (ATR) was successfuly received and processed
	
  Remarks:
    None
  *****************************************************************************/
#ifdef EMV_SUPPORT
SMARTCARD_TRANSACTION_STATUS SMARTCARD_EMV_ATRProcess( SMARTCARD_RESET_TYPES resetRequest )
{
    bool rejectATR = false;
    uint16_t    brg = 0;
    uint64_t    tempAtrDelay = 0;
    uint8_t     expectedMinATRLength = 0;
    uint8_t     atrIdx = 0;
    uint8_t     calculationDummyByte1 = 0;
    uint8_t     index = 0;
    uint8_t     continueFlag = 0;

        #ifdef SMARTCARD_SUPPORT_IN_HARDWARE

    //Enabling SMART card and related interrupts
    DRV_SMARTCARD_UART1_IntrEnable();
        #endif

    // Default FD Values
    SMARTCARD_FindFDvalues( (uint8_t) SMARTCARD_CLK_CONV_INTEGER_372 );

    // Calculate the new baud rate
    baudRate = ( uint64_t )
        (
            (uint64_t) ((uint64_t) scReferenceClock * factorDNumerator) / (uint32_t)
                (factorF * (uint32_t) factorDdenominator)
        );

    // Configure UART for new baud rate
    brg = ( uint16_t ) ( (uint32_t) ((uint32_t) (FCY / 4) / baudRate) - 1 );
    DRV_SMARTCARD_BRGSet( brg );

    while( 1 )
    {
        if( resetRequest == SMARTCARD_WARM_RESET )
        {
            // Default FD Values
            SMARTCARD_FindFDvalues( (uint8_t) SMARTCARD_CLK_CONV_INTEGER_372 );

            // Calculate the new baud rate
            baudRate = ( uint64_t )
                (
                    (uint64_t) ((uint64_t) scReferenceClock * factorDNumerator) / (uint32_t)
                        (factorF * (uint32_t) factorDdenominator)
                );

            // Configure UART for new baud rate
            brg = ( uint16_t ) ( (uint32_t) ((uint32_t) (FCY / 4) / baudRate) - 1 );
            DRV_SMARTCARD_BRGSet( brg );
        }

        continueFlag = 0;
        expectedMinATRLength = 2;
        atrIdx = 2;
        SMARTCARD_Drv_SetParity( 3 );
            #ifdef SMARTCARD_SUPPORT_IN_HARDWARE
            atrConfig.logicConvention = 0;
        U1SCCONbits.CONV = atrConfig.logicConvention;
            #endif
        SMARTCARD_InitATR_Variables();

        scTransactionStatus = SMARTCARD_TRANSACTION_SUCCESSFUL;

        //The terminal shall be able to receive an ATR having a duration of less than or equal to 20160
        //initial etus.
        SMARTCARD_Drv_EnableDelayTimerIntr45();

        // Wait for maximum of 42,000 smard card clock cycles
        // to get an ATR from card.The number of clock cycles is converted
        // into etu's for easier usage in the code.
        tempAtrDelay = ( (FCY / baudRate) * SMARTCARD_ATR_INIT_CLK_CYCLES ) + SMARTCARD_ATR_INT_CLK_CYCLE_ERROR;
        atrDelayETUs = ( unsigned short int ) ( (SMARTCARD_ATR_MAX_CLK_CYCLES * baudRate) / scReferenceClock ) + 1;

        //atrDelayETUs = 134;
        if( resetRequest == SMARTCARD_WARM_RESET )
        {
            SMARTCARD_WarmReset();
        }
        else
        {
            SMARTCARD_ColdReset();
        }

        SMARTCARD_Drv_SetDelayTimerCnt45( tempAtrDelay );
        SMARTCARD_Drv_EnableDelayTimer45();

            #ifdef SMARTCARD_SUPPORT_IN_HARDWARE
        SMARTCARD_Drv_SetDelayTimerCnt( tempAtrDelay );

        delayLapsedFlag = 0;
        SMARTCARD_Drv_EnableDelayTimer();
            #endif
        if( DRV_SMARTCARD_DataGet(&scCardATR[scATRLength], atrDelayETUs) )
        {
            #ifdef SMARTCARD_SUPPORT_IN_HARDWARE
            SMARTCARD_Drv_DisableDelayTimer();
            delayLapsedFlag = 0;
                #endif
            scATRLength++;

            atrDelayETUs = SMARTCARD_ATR_INTERVAL_10080;

            scTS = scCardATR[0];

            // InDirect Convention
            if( scTS == (uint8_t) SMARTCARD_INVERSE_CONVENTION )
            {
                    #ifdef SMARTCARD_SUPPORT_IN_HARDWARE
                SMARTCARD_Drv_SetParity( 1 );
                SMARTCARD_Drv_SetDataConvention( 1 );
                    #else
                SMARTCARD_Drv_SetParity( 2 );
                inverseConventionFlag = true;
                    #endif
            }
            else if( scTS == (uint8_t) SMARTCARD_DIRECT_CONVENTION )
            {
                SMARTCARD_Drv_SetParity( 1 );
                    #ifdef SMARTCARD_SUPPORT_IN_HARDWARE
                SMARTCARD_Drv_SetDataConvention( 0 );
                    #endif
            }
            else
            {
                break;
            }

            // Read Answer to RESET
            while( emvDelayLapsedFlag == 0 )
            {
                //wait for data byte from CARD
                if( DRV_SMARTCARD_DataGet(&scCardATR[scATRLength], atrDelayETUs) )
                {
                    scATRLength++;

                    if( scATRLength == MAX_ATR_LEN )
                    {
                        break;
                    }
                }
                else
                {
                    break;  //no data
                }
            }
        }

        SMARTCARD_Drv_DisableDelayTimer45();
        emvDelayLapsedFlag = 0;

        if( (scATRLength < 2) || parityErrorFlag )
        {
            break;
        }

        scT0 = scCardATR[1];
        calculationDummyByte1 = scCardATR[1];
        index = 0;
        while( 1 )
        {
            // Check for the present of TA1 byte
            if( calculationDummyByte1 & (uint8_t) SMARTCARD_CHECK_TA1_PRESENCE )
            {
                expectedMinATRLength++;

                if( index == 0 )
                {
                    scTA1PresentFlag = 1;
                    scTA1 = scCardATR[atrIdx++];
                }
                else if( index == 1 )
                {
                    scTA2PresentFlag = 1;
                    scTA2 = scCardATR[atrIdx++];

                    if( (scTA2 & SMARTCARD_BITMASK_5) == 0x00 )
                    {
                        if( ((scTA1 > SMARTCARD_MIN_BITRATE_ADJ_FACTOR) && (scTA1 < SMARTCARD_MAX_BITRATE_ADJ_FACTOR)) || (scTA1 == SMARTCARD_IFD_BITRATE_ADJ_FACTOR && bitRateAdjuFactor6Supported) )
                        {
                            SMARTCARD_FindFDvalues( scTA1 );

                            // Calculate the new baud rate
                            baudRate = ( uint64_t )
                                (
                                    (uint64_t) ((uint64_t) scReferenceClock * factorDNumerator) / (uint32_t)
                                        (factorF * (uint32_t) factorDdenominator)
                                );

                            // Configure UART for new baud rate
                            DRV_SMARTCARD_BRGSet( (uint16_t) ((uint32_t) ((uint32_t) (FCY / 4) / baudRate) - 1) );
                            SMARTCARD_CalculateWaitTime();
                                #ifdef SMARTCARD_SUPPORT_IN_HARDWARE
                            DRV_SMARTCARD_ATR_ConfigSet( atrConfig );
                                #endif
                        }
                        else if( resetRequest == SMARTCARD_WARM_RESET )
                        {
                            // Not a Valid ATR Reponse
                            SMARTCARD_Shutdown();
                            scTransactionStatus = SMARTCARD_ERROR_NO_ATR_RESPONSE;
                            return ( scTransactionStatus );
                        }
                        else
                        {
                            resetRequest = SMARTCARD_WARM_RESET;
                            continueFlag = 1;
                            break;
                        }
                    }
                }
                else if( index == 2 )
                {
                    scTA3PresentFlag = 1;
                    scTA3 = scCardATR[atrIdx++];
                        #ifdef SMARTCARD_PROTO_T1
                    maxSegmentLength = scTA3;
                        #endif
                }
            }

            // Check for the presence of TB1
            if( calculationDummyByte1 & SMARTCARD_BITMASK_6 )
            {
                expectedMinATRLength++;

                if( index == 0 )
                {
                    scTB1PresentFlag = 1;
                    scTB1 = scCardATR[atrIdx++];
                }
                else if( index == 1 )
                {
                    scTB2PresentFlag = 1;
                    scTB2 = scCardATR[atrIdx++];
                }
                else if( index == 2 )
                {
                    scTB3PresentFlag = 1;
                    scTB3 = scCardATR[atrIdx++];
                }
            }

            // Check for the present of TC1
            if( calculationDummyByte1 & SMARTCARD_BITMASK_7 )
            {
                expectedMinATRLength++;

                if( index == 0 )
                {
                    scTC1 = scCardATR[atrIdx++];
                }
                else if( index == 1 )
                {
                    scTC2PresentFlag = 1;
                    scTC2 = scCardATR[atrIdx++];
                }
                else if( index == 2 )
                {
                    scTC3PresentFlag = 1;
                    scTC3 = scCardATR[atrIdx++];
                        #ifdef SMARTCARD_PROTO_T1
                    edcType = ( scTC3 & 0x01 ) ? SMARTCARD_CRC_TYPE_EDC : ( uint8_t ) SMARTCARD_LRC_TYPE_EDC;
                        #endif
                }
            }

            // Check for the presence of TD1
            if( calculationDummyByte1 & SMARTCARD_BITMASK_8 )
            {
                calculationDummyByte1 = scCardATR[expectedMinATRLength];
                expectedMinATRLength++;

                if( index == 0 )
                {
                    scTD1 = scCardATR[atrIdx++];
                        #ifndef SMARTCARD_SUPPORT_IN_HARDWARE
                          DRV_SMARTCARD_SetProtocolSelected( scTD1 );
                        #endif
                }
                else if( index == 1 )
                {
                    scTD2PresentFlag = 1;
                    scTD2 = scCardATR[atrIdx++];
                }
                else if( index == 2 )
                {
                    scTD3 = scCardATR[atrIdx++];
                }
            }
            else
            {
                break;
            }

            index++;
        }

        if( continueFlag )
        {
            continue;
        }

        expectedMinATRLength = expectedMinATRLength + ( scT0 & SMARTCARD_BITMASK_LOWER_NIBBLE );

        if( scATRLength == expectedMinATRLength )
        {
            /*If indicated protocol is T1 and TCK byte is absent then reject the ATR*/
            if( ((scTD1 & SMARTCARD_BITMASK_LOWER_NIBBLE) == 0x01) || ((scTD2 & SMARTCARD_BITMASK_LOWER_NIBBLE) == 0x01) )
            {
                break;
            }
        }
        else if( scATRLength == (expectedMinATRLength + 1) )
        {
            calculationDummyByte1 = scT0;
            for( atrIdx = 2; atrIdx < scATRLength; atrIdx++ )
            {
                calculationDummyByte1 = calculationDummyByte1 ^ scCardATR[atrIdx];
            }
            /*if EXCLUSIVE OR of all the ATR bytes is not equal to zero then error*/
            if( calculationDummyByte1 )
            {
                break;
            }
        }
        else
        {
            if( resetRequest == SMARTCARD_WARM_RESET )
            {
                break;
            }
            if( SMARTCARD_ProtocolTypeGet() == SMARTCARD_T1_TYPE )
            {
                resetRequest = SMARTCARD_WARM_RESET;
            }
            continue;
        }

       rejectATR = ValidateATRData(resetRequest);
       if(rejectATR)
        {
            if( resetRequest == SMARTCARD_WARM_RESET )
            {
                resetRequest = SMARTCARD_COLD_RESET;
                break;     
            }
            else
            {
                resetRequest = SMARTCARD_WARM_RESET;
                continue;
            }
        }

        scATR_HistoryLength = scT0 & SMARTCARD_BITMASK_LOWER_NIBBLE;
        scATR_HistoryBuffer = ( scATR_HistoryLength ) ? ( &scCardATR[atrIdx] ) : NULL;

        // Calculate the protocol wait times for default values
        SMARTCARD_CalculateWaitTime();
        gCardState = SMARTCARD_ATR_ON;
            #ifdef SMARTCARD_SUPPORT_IN_HARDWARE
        atrConfig.cardState = SMARTCARD_ATR_ON;
        DRV_SMARTCARD_ATR_ConfigSet( atrConfig );
            #endif
        return ( scTransactionStatus );
    }

    // Not a Valid ATR Reponse
    SMARTCARD_Shutdown();
    scTransactionStatus = SMARTCARD_ERROR_NO_ATR_RESPONSE;
    return ( scTransactionStatus );
}

#endif

bool ValidateATRData(SMARTCARD_RESET_TYPES resetRequest)
{
        uint8_t     atrIdx = 0;
        bool rejectATR = false;
        uint8_t     calculationDummyByte = 0;
        /*If TB1 is present then its value should be equal to zero if the reset was cold reset*/
        if(scTB1PresentFlag)
        {
          if(((scTB1 != 0x00)) && (resetRequest == SMARTCARD_COLD_RESET))
          {
             rejectATR = true;
          }
        }
        if(scTB1PresentFlag == false && (resetRequest == SMARTCARD_COLD_RESET))
        {
            rejectATR = true;
        }
        /*If the protocol type to be used for the subsequent exchanges is not T0 or T1 then reject the ATR */
        if(((scTD1 & SMARTCARD_BITMASK_LOWER_NIBBLE) != 0x00) && ((scTD1 & SMARTCARD_BITMASK_LOWER_NIBBLE) != 0x01))
        {
            rejectATR = true;
        }

        if(scTA2PresentFlag)
        {
            /*The protocol indicated in the l.s. nibble is also the first indicated protocol in
              the ATR else reject the ATR*/
            if((scTA2 & SMARTCARD_BITMASK_LOWER_NIBBLE) != (scTD1 & SMARTCARD_BITMASK_LOWER_NIBBLE))
            {
                rejectATR = true;
            }
            /*If TA2 is present the the fifth bit should be zero else reject the ATR*/
            if((scTA2 & SMARTCARD_BITMASK_5) != 0x00)
            {
               rejectATR = true;
            }
        }
        /*If TB2 is present in the ATR then reject the ATR*/
        if(scTB2PresentFlag)
        {
          rejectATR = true;
        }
        /*If TC2 is present and Its value is zero then reject the ATR*/
        if(scTC2PresentFlag)
        {
            if(scTC2 == 0x00)
            {
                rejectATR = true;
            }
        }

        if(scTD2PresentFlag)
        {
          /*If l.s niblle of TD1 is 1 and l.s nibble of TD2 is not equal to 1 then reject the ATR*/
          if(((scTD1 & SMARTCARD_BITMASK_LOWER_NIBBLE) == 0x01) && ((scTD2 & SMARTCARD_BITMASK_LOWER_NIBBLE) != 0x01))
          {
            rejectATR = true;
          }
          /*If l.s nibble of TD1 is 0 and l.s nibble of TD2 is not equal to 0x0E or not equal to 0x01 then reject ATR*/
          if(((scTD1 & SMARTCARD_BITMASK_LOWER_NIBBLE) == 0x00) && ((scTD2 & SMARTCARD_BITMASK_LOWER_NIBBLE) != 0x0E) && ((scTD2 & SMARTCARD_BITMASK_LOWER_NIBBLE) != 0x01))
          {
             rejectATR = true;
          }
        }
        /*If TA3 is present , if its value is less than 0x10 or equal to oxFF then reject the ATR*/
        if(scTA3PresentFlag)
        {
            if((scTA3 < SMARTCARD_EMV_MIN_IFS_LENGTH) || (scTA3 == (SMARTCARD_EMV_MAX_IFS_LENGTH+1)))
            {
                rejectATR = true;
            }
        }

        if( ((scTD1 & SMARTCARD_BITMASK_LOWER_NIBBLE) == 0x01) || ((scTD2 & SMARTCARD_BITMASK_LOWER_NIBBLE) == 0x01) )
        {

            atrIdx = scTB3 & SMARTCARD_BITMASK_LOWER_NIBBLE;
            calculationDummyByte = 1;
            while( atrIdx-- )
            {
                calculationDummyByte = calculationDummyByte * 2;
            }

            if( scTC1 == 0xFF )
            {
                atrIdx = 0;
            }
            else
            {
                atrIdx = scTC1 + 1;
            }

            /* 1. Reject ATR if does not contain TB3
             * 2. BWI should be less than or equal to 4
             * 3. CWI should be less than or equal to 5 or pow(2,CWI)should be less than or equal to N+1 where N = TC1*/
            if( !scTB3PresentFlag || ((scTB3 & SMARTCARD_BITMASK_HIGHER_NIBBLE) > 0x40) || ((scTB3 & SMARTCARD_BITMASK_LOWER_NIBBLE) > 0x05) ||
                (calculationDummyByte <= atrIdx) )
            {
                rejectATR = true;
            }
        }
        /*If TC3 is present and its value is not equal to zero then reject the ATR*/
        if(scTC3PresentFlag)
        {
            if(scTC3 != 0x00)
            {
               rejectATR = true;
            }
        }
        

    return rejectATR;
}
/*******************************************************************************
  Function:
	void SMARTCARD_ColdReset(void)
  
  Description:
    The function does the cold reset of smart card module.

  Precondition:
    None

  Parameters:
 None

  Return Values:
 None
	
  Remarks:
    None
  *****************************************************************************/
static void SMARTCARD_ColdReset( void )
{
    #ifdef SMARTCARD_ENABLE_POWER_THROUGH_PORT_PIN
    SMARTCARD_Drv_SetSwitchCardPower( 1 );  //Turn off power to smart card
    #endif
    WaitMicroSec( 100 );

    SMARTCARD_Drv_EnableUART();
    SMARTCARD_Drv_EnableTxPinPullUp();

    WaitMicroSec( 100 );

    //Start the clock
    SMARTCARD_Drv_EnableClock();

    // Wait for atleast 400 Clock Cycles after applying reference clock to card.
    WaitMilliSec( 10 );

    //Release card reset line. set to high state
    SMARTCARD_Drv_SetSwitchCardReset( 1 );
}

/*******************************************************************************
  Function:
    void SMARTCARD_WarmReset(void)
  
  Description:
    This function does the warm Reset.

  Precondition:
    None

  Parameters:
    None

  Return Values:
    None
	
  Remarks:
    None
  *****************************************************************************/
static void SMARTCARD_WarmReset( void )
{
    //make sure card is in reset
    SMARTCARD_Drv_SetSwitchCardReset( 0 );

    // Wait for atleast 400 Clock Cycles after applying reference clock to card.
    WaitMilliSec( (int32_t) 10 );

    //Release card reset line. set to high state
    SMARTCARD_Drv_SetSwitchCardReset( 1 );
}

/*******************************************************************************
  Function:
    SMARTCARD_PPS_SUPPORT_STATUS SMARTCARD_Is_PPS_Supported(void)
  
  Description:
    This function tells whether PPS exchange is supported or not.

  Precondition:
    SMARTCARD_PowerOnATR was success

  Parameters:
    None

  Return Values:
    Returns SMARTCARD_PPS_SUPPORT_STATUS
	
  Remarks:
    This function is called when SMARTCARD_PowerOnATR() returns 1.
  *****************************************************************************/
SMARTCARD_PPS_SUPPORT_STATUS SMARTCARD_IsPPSSupported( void )
{
    SMARTCARD_PPS_SUPPORT_STATUS    ppsSupportStatus;

    if( scTA2PresentFlag )
    {
        if( scTA2 & SMARTCARD_BITMASK_8 )
        {   // Specific Mode Only
            ppsSupportStatus = SMARTCARD_PPS_NOT_ALLOWED;
        }
        else
        {   // Specifc Mode, But is capable to change the mode
            ppsSupportStatus = SMARTCARD_PPS_ALLOWED_AFTER_WARM_RESET;
        }
    }
    else
    {       // Negotiable Mode
        ppsSupportStatus = SMARTCARD_PPS_ALLOWED;
    }

    return ( ppsSupportStatus );
}

/*******************************************************************************
  Function:
	SMARTCARD_TRANSACTION_STATUS SMARTCARD_PPS(uint8_t *ppsPtr)
  
  Description:
    This function does the PPS exchange with the smart card & configures the baud 
    rate of the PIC UART module as per the PPS response from the smart card.

  Precondition:
    SMARTCARD_PowerOnATR was success

  Parameters:
    Input is pointer to PPS string

  Return Values:
    Returns SMARTCARD_TRANSACTION_STATUS
	
  Remarks:
    This function is called when SMARTCARD_PowerOnATR() returns 1.
  *****************************************************************************/
SMARTCARD_TRANSACTION_STATUS SMARTCARD_PPSExchange( uint8_t *ppsPtr )
{
    uint8_t     isLastByte = 0;
    uint8_t     ppsn[3] = { 0, 0, 0 };
    uint8_t     index1;
    uint8_t     index2 = 0x10;
    uint8_t     index3 = 2;
    uint8_t     ppsStrLength = 3;
    uint8_t     pckByte = 0x00;
    uint16_t    ppsCGTinMicroSeconds;

    // For PPS exchange GT = 12 ETUs
    ppsCGTinMicroSeconds = ( SMARTCARD_ETU_12 * 1000000UL ) / baudRate;

    scTransactionStatus = SMARTCARD_ERROR_PPS;

    // Calculate the length of PPS request and store PPS1, PPS2 and PPS3
    // in local variables for future calculations
    for( index1 = 0; index1 < 3; index1++ )
    {
        if( ppsPtr[1] & index2 )
        {
            ppsStrLength++;
            ppsn[index1] = ppsPtr[index3];
            index3++;
        }

        index2 = index2 << 1;
    }

    // Check for the conditions whether PPS can be done to the card...
    scPPSResponseLength = 0;
    if( !SMARTCARD_Drv_CardPresent() || (gCardState != SMARTCARD_ATR_ON) || (scTD1 & SMARTCARD_BITMASK_5) )
    {
        SMARTCARD_Shutdown();
        return ( scTransactionStatus );
    }

    // Send PPS request to the card
    index1 = 0;
    while( ppsStrLength-- )
    {
        #ifdef SMARTCARD_SUPPORT_IN_HARDWARE
        if( ppsStrLength == 1 )
        {
            isLastByte = 1;
        }

        #else
        WaitMicroSec( ppsCGTinMicroSeconds );
        #endif
        DRV_SMARTCARD_DataSend( ppsPtr[index1++], isLastByte );
    }

    // Recieve PPS response from the smart card
    index1 = 0;
    while( 1 )
    {
        #ifdef SMARTCARD_SUPPORT_IN_HARDWARE
        SMARTCARD_Drv_SetDelayTimerCnt45( ((FCY / baudRate) * 9600UL) );
        SMARTCARD_Drv_EnableDelayTimer45();
        #endif

        //wait for data byte from CARD
        if( DRV_SMARTCARD_DataGet(&scPPSResponse[index1], 9600) )
        {
            #ifdef SMARTCARD_SUPPORT_IN_HARDWARE
            SMARTCARD_Drv_DisableDelayTimer45();
            emvDelayLapsedFlag = 0;
            #endif
            if( ++index1 == SMARTCARD_MAX_PPS_LENGTH )
            {
                SMARTCARD_Shutdown();
                return ( scTransactionStatus );
            }
        }
        else
        {
            break;
        }
    }

    // If PPS response length is greater or equal to 3 bytes and PPSS byte is 0xFF and lower
    // nibble of PPS0 byte of PPS response matches with the PPS request then proceed for
    // further calculation or else exit...
    if( (index1 > 2) && (scPPSResponse[0] == SMARTCARD_BITMASK_BYTE) && ((ppsPtr[1] & SMARTCARD_BITMASK_LOWER_NIBBLE) == (scPPSResponse[1] & SMARTCARD_BITMASK_LOWER_NIBBLE)) )
    {
        index2 = 2;
        if( scPPSResponse[1] & SMARTCARD_BITMASK_5 )
        {
            // Check PPS1 Response is identical to PPS1 Request.
            if( scPPSResponse[index2] != ppsn[0] )
            {
                SMARTCARD_Shutdown();
                return ( scTransactionStatus );
            }

            index2++;
        }

        if( scPPSResponse[1] & SMARTCARD_BITMASK_6 )
        {
            // Check PPS1 Response is identical to PPS1 Request.
            if( scPPSResponse[index2] != ppsn[1] )
            {
                SMARTCARD_Shutdown();
                return ( scTransactionStatus );
            }

            index2++;
        }

        if( scPPSResponse[1] & SMARTCARD_BITMASK_7 )
        {
            // Check PPS1 Response is identical to PPS1 Request.
            if( scPPSResponse[index2] != ppsn[2] )
            {
                SMARTCARD_Shutdown();
                return ( scTransactionStatus );
            }
        }

        // Calculate PCK for the PPS response and ex-or it with recieved PCK byte
        pckByte = 0x00;
        for( index2 = 0; index2 < index1; index2++ )
        {
            pckByte = pckByte ^ scPPSResponse[index2];
        }

        // If the final vaue is non-zero then exit...
        if( pckByte )
        {
            SMARTCARD_Shutdown();
            return ( scTransactionStatus );
        }

        // If baud rate modification request has been accepted by the smart card,
        // change the UART baud rate and other wait time constants
        if( scPPSResponse[1] & SMARTCARD_BITMASK_5 )
        {
            SMARTCARD_FindFDvalues( scPPSResponse[2] );

            // Calculate the new baud rate
            baudRate = ( uint64_t )
                (
                    (uint64_t) ((uint64_t) scReferenceClock * factorDNumerator) / (uint32_t)
                        (factorF * (uint32_t) factorDdenominator)
                );

            // Configure UART for new baud rate
            DRV_SMARTCARD_BRGSet( (uint16_t) ((uint32_t) ((uint32_t) (FCY / 4) / baudRate) - 1) );
            SMARTCARD_CalculateWaitTime();
            #ifdef SMARTCARD_SUPPORT_IN_HARDWARE
            DRV_SMARTCARD_ATR_ConfigSet( atrConfig );
            #endif
        }

        // Store the PPS response length in global variable
        scPPSResponseLength = index1;
        scTransactionStatus = SMARTCARD_TRANSACTION_SUCCESSFUL;
    }

    return ( scTransactionStatus );
}

/*******************************************************************************
 Function:
    bool SMARTCARD_IsPresent(void)

  Description:
    This function returns the true or false depending on  card is present/not present.

  Precondition:
    SMARTCARD_Initialize is called.

  Parameters:
    None

  Return Values:
    True of False

  Remarks:
    None
  *****************************************************************************/
bool SMARTCARD_IsPresent( void )
{
    return ( SMARTCARD_Drv_CardPresent() );
}

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
  *****************************************************************************/
SMARTCARD_STATUS SMARTCARD_StateGet( void )
{
    return ( gCardState );
}

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
  *****************************************************************************/
void SMARTCARD_Shutdown( void )
{
    //Bring reset line low
    SMARTCARD_Drv_SetSwitchCardReset( 0 );

    WaitMilliSec( 1 );

    // Turn off external Clock given to the card
    SMARTCARD_Drv_DisableClock();

    // Shut down UART and remove any pullups
    DRV_SMARTCARD_DeInitialize();

    WaitMicroSec( 100 );

    SMARTCARD_Drv_SetSwitchCardPower( 0 );
    SMARTCARD_Drv_SetSwitchCardPower( 0 );

    // Turn Off Card Power
    //#ifdef SMARTCARD_ENABLE_POWER_THROUGH_PORT_PIN
    SMARTCARD_Drv_SetSwitchCardPower( 0 );

    //#endif
    gCardState = SMARTCARD_UNKNOWN;
    rxSbitPrevIBlk = true;
    txSbit = true;
}

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
SMARTCARD_TRANSACTION_TYPES SMARTCARD_ProtocolTypeGet( void )
{
    SMARTCARD_TRANSACTION_TYPES scProtocolType = SMARTCARD_INVALID_TYPE;
    if( (scTD1 & 0x1) == SMARTCARD_T1_TYPE )
    {
        scProtocolType = SMARTCARD_T1_TYPE;
    }
    else if( (scTD1 & 0x1) == SMARTCARD_T0_TYPE )
    {
        scProtocolType = SMARTCARD_T0_TYPE;
    }

    return ( scProtocolType );
}

/*******************************************************************************
  Function:
    void SMARTCARD_FindFDvalues(uint8_t tA1Type)
	
  Description:
    This function finds the clock rate and baud rate adjustment integers(F,D)

  Precondition:
    SMARTCARD_PowerOnATR is called.

  Parameters:
    Input byte as encoded in TA1 character

  Return Values:
    None
	
  Remarks:
  *****************************************************************************/
static void SMARTCARD_FindFDvalues( uint8_t tA1Type )
{
    uint8_t ta1Code = 0;

    ta1Code = tA1Type & SMARTCARD_BITMASK_LOWER_NIBBLE;

    factorDNumerator = 1;
    factorDdenominator = 1;

    //TA1 encodes the indicated value of the clock rate conversion integer Fi, the indicated value of the baud rate
    //adjustment integer Di and the maximum value of the frequency supported by the card. The default
    //values are Fi = 372, Di = 1 and f (max.) = 5 MHz.
    // Calculate Factor 'D' from TA1 value as per ISO 7816-3 specifications
    switch( ta1Code )
    {
        // Case 0,1 and 7 F(i)= 372  and f(Max)=4Mhz
        case 0x00:
        case 0x07:
        case 0x01:
            break;

        // Case 2: F(i)= 558  and f(Max)=6Mhz
        case 0x02:
            factorDNumerator = 2;
            break;

        // Case 3: F(i)= 744  and f(Max)=8Mhz
        case 0x03:
            factorDNumerator = 4;
            break;

        // Case 4: F(i)= 558  and f(Max)=6Mhz
        case 0x04:
            factorDNumerator = 8;
            break;

        // Case 5: F(i)= 1488  and f(Max)=16Mhz
        case 0x05:
            factorDNumerator = 16;
            break;

        // Case 6: F(i)= 1860  and f(Max)=20Mhz
        case 0x06:
            factorDNumerator = 32;
            break;

        // Case 8: Reserved Value
        case 0x08:
            factorDNumerator = 12;
            break;

        // Case 9: F(i)= 512  and f(Max)=5Mhz
        case 0x09:
            factorDNumerator = 20;
            break;

        // Case 0x0A: F(i)= 768  and f(Max)=7Mhz/6Mhz
        case 0x0A:
            factorDdenominator = 2;
            break;

        // Case 0x0B: F(i)= 1024  and f(Max)=10Mhz
        case 0x0B:
            factorDdenominator = 4;
            break;

        // Case 0x0C: F(i)= 1536  and f(Max)=15Mhz
        case 0x0C:
            factorDdenominator = 8;
            break;

        // Case 0x0D: F(i)= 2048  and f(Max)=20Mhz
        case 0x0D:
            factorDdenominator = 16;
            break;

        // Case 0x0E/0xoF: Reserved Frequency, could be any value
        case 0x0E:
            factorDdenominator = 32;
            break;

        case 0x0F:
            factorDdenominator = 64;
            break;
    }

    ta1Code = ( tA1Type & SMARTCARD_BITMASK_HIGHER_NIBBLE ) >> 4;

    factorF = SMARTCARD_CLK_DATE_CONV_FACTOR_372;

    // Calculate Factor 'F' from TA1 value as per ISO 7816-3 specifications
    // Baudrate value of Adjustment integer is choosen in the following switch case
    // statements.
    switch( ta1Code )
    {
        case 0x00:
        case 0x07:
        case 0x08:
        case 0x0E:
        case 0x0F:
        case 0x01:
            break;

        case 0x02:
            factorF = 558;
            break;

        case 0x03:
            factorF = 744;
            break;

        case 0x04:
            factorF = 1116;
            break;

        case 0x05:
            factorF = 1488;
            break;

        case 0x06:
            factorF = 1860;
            break;

        case 0x09:
            factorF = 512;
            break;

        case 0x0A:
            factorF = 768;
            break;

        case 0x0B:
            factorF = 1024;
            break;

        case 0x0C:
            factorF = 1536;
            break;

        case 0x0D:
            factorF = 2048;
            break;
    }
}

/*******************************************************************************
  Function:
    void SMARTCARD_CalculateWaitTime(void)
	
  Description:
    This function calculates the wait time values for T=0 and T=1 Protocol

  Precondition:
    SMARTCARD_PowerOnATR is called.

  Parameters:
    None

  Return Values:
    None
	
  Remarks:
  *****************************************************************************/
static void SMARTCARD_CalculateWaitTime( void) 
{
    uint8_t tempVariable1;
#ifdef SMARTCARD_PROTO_T1
    uint8_t tb2Code;
    uint8_t index;
    uint16_t tempVariable2 = 1;
    uint8_t tempVarCGT = SMARTCARD_ETU_12;
#endif

    // Calculate Character Guard Time ETU
    if (scTC1 != (uint8_t) SMARTCARD_MIN_DLY_OF_12_ETU_T1) 
    {
        cgtETU = SMARTCARD_ETU_12 + scTC1;
    }

    // Check whether T=0 or T=1 protocol ?
    switch (scTD1 & SMARTCARD_BITMASK_LOWER_NIBBLE) 
    {
            // For T = 1 protocol
        case 1:

            // Calculate Character Guard Time in ETU's for T=1 Protocol
            if (scTC1 == (uint8_t) SMARTCARD_MIN_DLY_OF_12_ETU_T1) 
            {
                // For T=1, cgtETU=0x0B
                cgtETU = (uint8_t) SMARTCARD_MIN_CHAR_TO_CHAR_DURATN_T1;
                tempVarCGT = (uint8_t) SMARTCARD_MIN_CHAR_TO_CHAR_DURATN_T1;
            }

#ifdef SMARTCARD_PROTO_T1
            if (scTB2PresentFlag) 
            {
                tb2Code = scTB2 & SMARTCARD_BITMASK_LOWER_NIBBLE;

                tempVariable1 = (scTB2 & SMARTCARD_BITMASK_HIGHER_NIBBLE) >> 4;
            } 
            else 
            {
                if (scTB3PresentFlag) 
                {
                    if (scTB3PresentFlag) 
                    {
                        tb2Code = scTB3 & SMARTCARD_BITMASK_LOWER_NIBBLE;
                        tempVariable1 = (scTB3 & SMARTCARD_BITMASK_HIGHER_NIBBLE) >> 4;
                    } 
                    else 
                    {
                        tb2Code = (uint8_t) SMARTCARD_CWI;
                        tempVariable1 = (uint8_t) SMARTCARD_BWI;
                    }
                } 
                else 
                {
                    tb2Code = (uint8_t) SMARTCARD_CWI;
                    tempVariable1 = (uint8_t) SMARTCARD_BWI;
                }
            }

            for (index = 0; index < tb2Code; index++) 
            {
                tempVariable2 = tempVariable2 * 2;
            }

            // Calculate Character Wait Time in ETU's for T=1 Protocol as set in the card
            t1CWTetu = SMARTCARD_ETU_11 + tempVariable2;
#ifdef SMARTCARD_SUPPORT_IN_HARDWARE
            atrConfig.characterWaitingTime = t1CWTetu;
#endif
            tempVariable2 = 1;

            for (index = 0; index < tempVariable1; index++) 
            {
                tempVariable2 = tempVariable2 * 2;
            }

            // Calculate Block Wait Time in ETU's for T=1 Protocol as set in the card
            //t1BWTetu = SMARTCARD_ETU_11 + (uint16_t)((uint32_t)tempVariable2 * (((357120UL * factorDNumerator)/factorF)/factorDdenominator));
            t1BWTetu = SMARTCARD_ETU_11 + ((uint32_t) (tempVariable2 * (960UL * factorDNumerator)) + (factorDNumerator * (960UL)));
#ifdef SMARTCARD_SUPPORT_IN_HARDWARE
            atrConfig.blockWaitingTime = t1BWTetu;
#endif
#endif
#ifndef SMARTCARD_SUPPORT_IN_HARDWARE

            // Convert CGT etu value in terms of microseconds
            cgtInMicroSeconds = (cgtETU * 1000000UL) / baudRate;
            oppTimeInMicroSeconds = cgtInMicroSeconds;
            /*Add wait only if CGT is more than 12 , UART DataSend function has the necessary wait which comes upto 12 ETUs*/
            DRV_SMARTCARD_CGT_Adjustment(&cgtInMicroSeconds ,tempVarCGT , scTC1);
            if (scTC1 == 0xFF) 
            {
                point3ETUtimeinMicroSeconds = (oneETUtimeinMicroSeconds * 2) / 100;
                point05ETUtimeinMicroSeconds = point3ETUtimeinMicroSeconds;
            }

#endif
            break;

            // For T = 0 Protocol
        case 0:
        default:
            // Calculate Character Guard Time in ETU's for T=0 Protocol
            if (scTC1 == 0xFF) 
            {
                cgtETU = (uint8_t) SMARTCARD_ETU_12;
            }

#ifdef SMARTCARD_SUPPORT_IN_HARDWARE
            atrConfig.characterGuardTime = cgtETU;
#endif

            // If scTC2 is transmitted by the card then calculate work wait time
            // or else use default value
            if (scTC2PresentFlag) 
            {
                tempVariable1 = scTC2;
            } 
            else 
            {
                tempVariable1 = (uint8_t) SMARTCARD_WI;
            }

            // Calculate Wait Time used for T = 0 protocol
            t0WWTetu = (uint32_t) ((uint32_t) (tempVariable1 * 960UL * factorDNumerator) / factorDdenominator);
#ifdef SMARTCARD_SUPPORT_IN_HARDWARE
            atrConfig.workWaitingTime = t0WWTetu; // Convert CGT etu value in terms of microseconds
#else

            // Convert CGT etu value in terms of microseconds
            cgtInMicroSeconds = (cgtETU * 1000000UL) / baudRate;
#endif
            break;
    }
}

#if 0

// Function is not used, In Future of ISO implementation could be used.
// Hence commented and retained the code

/*******************************************************************************
  Function:
	uint8_t SMARTCARD_ISO_TransactT0(SMARTCARD_APDU_COMMAND* apduCommand, SMARTCARD_APDU_RESPONSE* apduResponse, uint8_t* apduDataBuffer)
  
  Description:
    This function Sends/recieves the ISO 7816-4 compaliant APDU commands to the card.

  Precondition:
    SMARTCARD_PPS was success or SMARTCARD_PPS functionality not called

  Parameters:
    SMARTCARD_APDU_COMMAND* apduCommand	- Pointer to APDU Command Structure
	SMARTCARD_APDU_RESPONSE* pResp - Pointer to APDU Response structure
			uint8_t* pResp - Pointer to the Command/Response Data buffer

  Return Values:
    1 if transaction was success, and followed the ISO 7816-4 protocol.
	
  Remarks:
    In the APDU command structure, the LC field defines the number of data bytes to 
	be transmitted to the card. This array can hold max of 256 bytes, which 
	can be redefined by the user.  The LE field in APDU command defines the number
	of bytes expected to be received from the card.  This array can hold max 256 bytes,
	which can be redefined by the user.	
  *****************************************************************************/
uint8_t SMARTCARD_ISO_TransactT0( SMARTCARD_APDU_COMMAND *apduCommand, SMARTCARD_APDU_RESPONSE *apduResponse,
                                  uint8_t *apduDataBuffer) 
{
    uint8_t *apduCommandBuffer;
    uint8_t index;
    uint8_t lc = apduCommand->LC;
    uint8_t le = apduCommand->LE;
    uint8_t ins = apduCommand->INS;
    uint8_t rx_char;
    uint8_t lcLength = 0;
    uint8_t leLength = 0;

    // Return False if there is no Card inserted in the Slot
    if (!SMARTCARD_Drv_CardPresent() || gCardState != SMARTCARD_ATR_ON) 
    {
        scTransactionStatus = SMARTCARD_ERROR_CARD_NOT_PRESENT;
        return ( 0);
    }

    // Clear APDU Response data if present before getting the new one's
    memset(apduResponse, 0, sizeof (SMARTCARD_APDU_RESPONSE));

    apduCommandBuffer = (uint8_t *) apduCommand;

    //Send the Command Bytes: CLA INS P1 P2
    for (index = 0; index < 4; index++) 
    {
        DRV_SMARTCARD_DataSend(apduCommandBuffer[index]);
        WaitMicroSec(cgtInMicroSeconds);
    }

    //Now transmit LE or LC field if non zero
    if (lc) 
    {
        DRV_SMARTCARD_DataSend(lc);
    } 
    else if (le) 
    {
        DRV_SMARTCARD_DataSend(le);
    } 
    else 
    {
        DRV_SMARTCARD_DataSend(0x00);
    }

    while (1) 
    {
        // Get Procedure byte
        if (!DRV_SMARTCARD_DataGet(&rx_char, t0WWTetu)) //wait for data byte from CARD
        {
            scTransactionStatus = SMARTCARD_ERROR_CARD_NO_RESPONSE;
            return ( 0); //no response received
        }

        // Process Procedure Byte
        if (rx_char == 0x60) 
        {
            // Do Nothing
        } 
        else if (((rx_char & 0xF0) == 0x60) || ((rx_char & 0xF0) == 0x90)) 
        {
            // SW1, get SW2
            apduResponse->SW1 = rx_char; //save SW1

            //now receive SW2
            if (DRV_SMARTCARD_DataGet(&rx_char, t0WWTetu)) //wait for data byte from CARD
            {
                apduResponse->SW2 = rx_char;
            } 
            else 
            {
                scTransactionStatus = SMARTCARD_ERROR_CARD_NO_RESPONSE;
                return ( 0); //no response received
            }

            break;
        } 
        else if (rx_char == ins) 
        {
            // Send all remaining bytes
            if (lcLength < lc) //transmit app data if any
            {
                WaitMicroSec(cgtInMicroSeconds);

                for (; lcLength < lc; lcLength++) 
                {
                    DRV_SMARTCARD_DataSend(apduDataBuffer[lcLength]);
                    WaitMicroSec(cgtInMicroSeconds);
                }
            } 
            else 
            {
                // Recive all remaining bytes
                for (; leLength < le; leLength++) 
                {
                    if (DRV_SMARTCARD_DataGet(&rx_char, t0WWTetu)) 
                    { //wait for data byte from CARD
                        apduDataBuffer[leLength] = rx_char;
                    } 
                    else 
                    {
                        scTransactionStatus = SMARTCARD_ERROR_CARD_NO_RESPONSE;
                        return ( 0); //no response received
                    }
                }
            }
        } 
        else if (rx_char == ~ins) 
        {
            // ACK, send one byte if remaining
            if (lcLength < lc) 
            {
                WaitMicroSec(cgtInMicroSeconds);

                DRV_SMARTCARD_DataSend(apduDataBuffer[lcLength++]);
            } 
            else 
            {
                //wait for data byte from CARD or timeout
                if (DRV_SMARTCARD_DataGet(&rx_char, t0WWTetu)) 
                {
                    apduDataBuffer[leLength++] = rx_char;
                } 
                else 
                {
                    scTransactionStatus = SMARTCARD_ERROR_CARD_NO_RESPONSE;
                    return ( 0); //no response received
                }
            }
        } 
        else 
        {
            // Do Nothing
        }
    }

    // Store the number of recieved data bytes including the
    // status codes to make the life of Smart Card Reader easier
    apduResponse->rxDataLen = leLength;

    return ( 1);
}

#endif
#ifdef SMARTCARD_PROTO_T1

/*******************************************************************************
  Function:
    static uint16_t SMARTCARD_UpdateCRC(uint8_t data,uint16_t crc)
	
  Description:
    This function calculates 16 bit CRC for T=1 Protocol

  Precondition:
    Initial value of crc should be 0xFFFF.

  Parameters:
    uint8_t data - Data that has to be used to update CRC.
	uint16_t crc - current crc value

  Return Values:
    uint16_t - updated CRC
	
  Remarks:
    CRC 16 - X^16 + X^12 + X^5 + 1

  *****************************************************************************/
#if !defined( EMV_SUPPORT )
static uint16_t SMARTCARD_UpdateCRC( uint8_t data, uint16_t crc )
{
    uint8_t     index;
    uint16_t    tempData = ( uint16_t ) data << 8;

    // Update the CRC & return it Back
    for( index = 0; index < 8; index++ )
    {
        if( (crc ^ tempData) & 0x8000 )
        {
            crc <<= 1;
            crc ^= ( uint16_t ) 0x1021; // X^12 + X^5 + 1
        }
        else
        {
            crc <<= 1;
        }

        tempData <<= 1;
    }

    return ( crc );
}
#endif

/*******************************************************************************
  Function:
    static void SMARTCARD_UpdateEDC(uint8_t data,uint16_t *edc)
	
  Description:
    This function updates Error Data Check value depending on the EDC type
    for T=1 Protocol

  Precondition:
    None.

  Parameters:
    uint8_t data - Data that has to be used to update EDC.
	uint16_t *edc - Pointer to EDC

  Return Values:
    None
	
  Remarks:
    None

*****************************************************************************/
#if !defined( EMV_SUPPORT )
static void SMARTCARD_UpdateEDC( uint8_t data, uint16_t *edc )
{
    // Store the updated LRC/CRC in the EDC
    if( edcType == (uint8_t) SMARTCARD_CRC_TYPE_EDC )   // type = CRC
    {
        *edc = SMARTCARD_UpdateCRC( data, *edc );
    }
    else    // type = LRC
    {
        *edc = *edc ^ data;
    }
}
#endif

/*******************************************************************************
  Function:
    static void SMARTCARD_SendT1Block(uint8_t nad,uint8_t pcb,uint8_t length,uint8_t *buffer)
	
  Description:
    This function transmits a T=1 formatted block

  Precondition:
    Complete ATR...

  Parameters:
    uint8_t nad - NAD to be transmitted to the card
    uint8_t pcb - PCB to be transmitted to the card
    uint8_t length - Length of I-Field transmitted to the card
    uint8_t *buffer - Pointer to data that is to be transmitted to the card

  Return Values:
    None
	
  Remarks:
    None

*****************************************************************************/
static void SMARTCARD_SendT1Block( uint8_t nad, uint8_t pcb, uint8_t length, uint8_t *buffer) 
{
    uint8_t index = 0;
    uint16_t edc = 0; // Error Detection and Correction Word

    // Choose the initial value of edc depending upon LRC or CRC
#if defined( EMV_SUPPORT )
    edc = pcb ^ length;

    // Update the edc for the data to be transmitted
    for (index = 0; index < length; index++) 
    {
        edc = edc ^ buffer[index];
    }

    // Transmit Node Address, by default is 00. Generally the EMV standard does not
    // support any other node address.
#ifndef SMARTCARD_SUPPORT_IN_HARDWARE
    WaitMicroSec(oppTimeInMicroSeconds + 93);
#endif
    DRV_SMARTCARD_DataSend(nad, 0);

#else
    if (edcType == (uint8_t) SMARTCARD_CRC_TYPE_EDC) 
    {
        edc = 0xFFFF;
    } 
    else 
    {
        edc = 0x0000;
    }

    // Update the edc for Node Address Data Byte
    SMARTCARD_UpdateEDC(nad, &edc);

    // Update the edc for Protocol Control Byte
    SMARTCARD_UpdateEDC(pcb, &edc);

    // Update the edc for length of tx Bytes
    SMARTCARD_UpdateEDC(length, &edc);

    // Update the edc for the data to be transmitted
    for (index = 0; index < length; index++) 
    {
        SMARTCARD_UpdateEDC(buffer[index], &edc);
    }

    // Transmit Node Address
    DRV_SMARTCARD_DataSend(nad);
#endif
#ifndef SMARTCARD_SUPPORT_IN_HARDWARE
    WaitMicroSec(cgtInMicroSeconds);
#endif

    // Transmit Protocol Control Byte	
    DRV_SMARTCARD_DataSend(pcb, 0);

    // Transmit length of Data Byte
#ifndef SMARTCARD_SUPPORT_IN_HARDWARE
    WaitMicroSec(cgtInMicroSeconds);
#endif
    DRV_SMARTCARD_DataSend(length, 0);

#ifndef SMARTCARD_SUPPORT_IN_HARDWARE
    WaitMicroSec(cgtInMicroSeconds);
#endif

    // Transmit Data Bytes
    for (index = 0; index < length; index++) 
    {
        DRV_SMARTCARD_DataSend(buffer[index], 0);
#ifndef SMARTCARD_SUPPORT_IN_HARDWARE
        WaitMicroSec(cgtInMicroSeconds);
#endif
    }

    // Transmit EDC
    DRV_SMARTCARD_DataSend((uint8_t) edc, 1);

#if !defined( EMV_SUPPORT )
    if (edcType == (uint8_t) SMARTCARD_CRC_TYPE_EDC) 
    {
        WaitMicroSec(cgtInMicroSeconds);
        DRV_SMARTCARD_DataSend(edc >> 8);
    }

#endif
}

/*******************************************************************************
  Function:
    void SMARTCARD_ReceiveT1Block(void)
	
  Description:
    This function receives a T=1 formatted block

  Precondition:
    Transmit a block before expecting the response...

  Parameters:
    uint8_t *rxNAD - Pointer to NAD recieved from the card
    uint8_t *rxPCB - Pointer to PCB recieved from the card
    uint8_t *rxLength - Pointer to Length of I-Field recieved from the card
    uint8_t *buffer - Pointer to data recieved from the card
	uint32_t blockWaitTime - value of Block Wait Time

  Return Values:
    1 if block recieve is successful, and follows the ISO 7816-4 protocol.
	
  Remarks:
    None
*****************************************************************************/
static bool SMARTCARD_ReceiveT1Block(uint8_t *rxNAD,uint8_t *rxPCB,uint8_t *rxLength,uint8_t *buffer) 
{
    bool dataReceiveError = false;
    unsigned short int edc;
    uint16_t index;
    uint16_t expectedLength;
    parityErrorFlag = false;
    errType = NO_ERROR;
    cwtExceeded = false;

    currentT1RxBlockType = SMARTCARD_INVALID_BLOCK;

    // Get NAD
    if (!DRV_SMARTCARD_DataGet(rxNAD, (currT1BWTetu + 4))) 
    {
        scTransactionStatus = SMARTCARD_ERROR_CARD_NO_RESPONSE;
        currentT1RxBlockType = SMARTCARD_NO_BLOCK;
        return false;
    }

    // Get PCB
    if (!DRV_SMARTCARD_DataGet(rxPCB, (t1CWTetu + 4))) 
    {
        scTransactionStatus = SMARTCARD_ERROR_CARD_NO_RESPONSE;
        dataReceiveError = true;
    }

    // Get Length
    if (!DRV_SMARTCARD_DataGet(rxLength, (t1CWTetu + 4))) 
    {
        scTransactionStatus = SMARTCARD_ERROR_CARD_NO_RESPONSE;
        dataReceiveError = true;
    }

    // Add one to the expected length for LRC
    if (*rxLength <= SMARTCARD_EMV_MAX_IFS_LENGTH) 
    {
        //Add one to the expected length for LRC
        expectedLength = *rxLength + 1;
    } 
    else 
    {
        //if size is more than 254 bytes its an error , no need to add LRC
        expectedLength = *rxLength;
        errType = OTHER_ERROR;
    }
    // Add additional byte to the length if using CRC
#if !defined(EMV_SUPPORT)

    if (edcType == SC_CRC_TYPE_EDC)
        expectedLength++;

#endif
    // Get all the data bytes plus EDC (1 or 2 bytes at end)
    for (index = 0; index < expectedLength;) 
    {
        if (!DRV_SMARTCARD_DataGet(buffer + index, (t1CWTetu + 4))) 
        {
            scTransactionStatus = SMARTCARD_ERROR_CARD_NO_RESPONSE;
            dataReceiveError = true;
        }

        ++index;
    }
    if (dataReceiveError) 
    {
        return false;
    }
    // Check for the LRC Error
    if (edcType == SMARTCARD_LRC_TYPE_EDC) 
    {
        edc = 0;
        edc = edc ^ *rxNAD;
        edc = edc ^ *rxPCB;
        edc = edc ^ *rxLength;

        for (index = 0; index < expectedLength;) 
        {
            edc = edc ^ buffer[index];
            ++index;
        }

        if (edc != 0) 
        {
            scTransactionStatus = SMARTCARD_ERROR_RECEIVE_LRC;
            errType = PARITY_LRC_ERROR;
            return false;
        }
    }
    #if !defined(EMV_SUPPORT)
    else // // Check for the CRC Error
    {
        edc = 0xFFFF;
        edc = SC_UpdateCRC(*rxNAD, edc);
        edc = SC_UpdateCRC(*rxPCB, edc);
        edc = SC_UpdateCRC(*rxLength, edc);

        for (index = 0; index < (expectedLength - 2);) 
        {
            edc = SC_UpdateCRC(buffer[index], edc);
            ++index;
        }

        if (((edc >> 8) != buffer[expectedLength - 2]) || ((edc & 0xFF) != buffer[expectedLength - 1])) 
        {
            scTransactionStatus = SC_ERR_RECEIVE_CRC;
            return false;
        }
    }
#endif

    if (parityErrorFlag == true) 
    {
        errType = PARITY_LRC_ERROR;
        return false;
    }
    if (*rxNAD != 0x00) 
    {
        errType = OTHER_ERROR;
        return false;
    }
    //For Rblock value of rxLength should be zero else its an error.
    //For Rblock the value of the bit 6 (b8-b1) should be zero else its an error
    //Incase of len error data block will be considered as invalid block
    //Previous data block sent by the IFD needs to be resent
    if (((*rxPCB & SMARTCARD_R_BLOCK_MASK) == SMARTCARD_R_BLOCK_IDENTIFIER)) 
    {
        if ((*rxLength) != 0x00) 
        {
            errType = OTHER_ERROR;
            return true;
        }
        if ((*rxPCB & SMARTCARD_R_BLOCK_BIT_SIX) == SMARTCARD_R_BLOCK_BIT_SIX) 
        {
            errType = OTHER_ERROR;
            return true;
        }
    }
    if (((*rxPCB & SMARTCARD_S_BLOCK_MASK) == SMARTCARD_S_BLOCK_IDENTIFIER)) 
    {
        //If IFS or rxLength is more than one for Sblock then its an error
        //If IFS request or response data length is less than 1
        //Previous data block sent by the IFD needs to be resent
        if (((*rxPCB == SMARTCARD_IFS_REQUEST) || (*rxPCB == SMARTCARD_IFS_RESPONSE) || (*rxPCB == SMARTCARD_WAIT_TIME_EXT_REQUEST)) && (((*rxLength) > 0x01) || ((*rxLength) < 1))) 
        {
            errType = OTHER_ERROR;
            return true;
        }
        //EMV standard supports IFS value in the range 0x10 to 0xFE , else invalid block
        //Previous data block sent by the IFD needs to be resent
        if (((*rxPCB == SMARTCARD_IFS_REQUEST) || (*rxPCB == SMARTCARD_IFS_RESPONSE)) && ((buffer[0] < SMARTCARD_EMV_MIN_IFS_LENGTH) || (buffer[0] >= (SMARTCARD_EMV_MAX_IFS_LENGTH + 1)))) 
        {
            errType = OTHER_ERROR;
            return true;
        }
        //Abort response lenth is more than zero then error
        if (((*rxPCB == SMARTCARD_ABORT_RESPONSE) && ((*rxLength) > 0x00))) 
        {
            errType = OTHER_ERROR;
            return true;
        }
        //If the value of last 5 bits is greater than 4 then its an error
        if ((*rxPCB & 0x1F) >= 0x04) 
        {
            errType = OTHER_ERROR;
            return true;
        }
    }
    // Determine the type of Block recieved from the card
    if ((*rxPCB & SMARTCARD_I_BLOCK_MASK) == SMARTCARD_I_BLOCK_IDENTIFIER) 
    {
        // I-Block
        currentT1RxBlockType = SMARTCARD_I_BLOCK;
    } 
    else if ((*rxPCB & SMARTCARD_S_BLOCK_MASK) == SMARTCARD_S_BLOCK_IDENTIFIER) 
    {
        // S-Block
        currentT1RxBlockType = SMARTCARD_S_BLOCK;
    } 
    else if ((*rxPCB & SMARTCARD_R_BLOCK_MASK) == SMARTCARD_R_BLOCK_IDENTIFIER) 
    {
        // R-Block
        currentT1RxBlockType = SMARTCARD_R_BLOCK;
    }


    // Return true if there is no LRC or CRC error & data bytes are recieved sucessfully
    return true;
}
#endif

/*******************************************************************************
  Function:
    SMARTCARD_TRANSACTION_STATUS SMARTCARD_EMV_TransactT0(uint8_t* apduCommand, uint32_t apduCommandLength, SMARTCARD_APDU_RESPONSE* apduResponse)
  
  Description:
    This function Sends/recieves the  T = 0 commands to the card.

  Precondition:
    SMARTCARD_PPS was success

  Parameters:
    SMARTCARD_T1_PROLOGUE_FIELD* pfield - Pointer to Prologue Field
    uint8_t* iField - Pointer to the Information Field of Tx/Rx Data
    SMARTCARD_APDU_RESPONSE* apduResponse - Pointer to APDU Response structure

  Return Values:
    1 if transaction was success, and followed the ISO 7816-4 protocol.
	
  Remarks:    	
 *****************************************************************************/
SMARTCARD_TRANSACTION_STATUS SMARTCARD_EMV_DataExchangeT0(uint8_t *apduCommand, uint32_t apduCommandLength,
        SMARTCARD_APDU_RESPONSE *apduResponse) 
{
    uint8_t isLastByte = 0;
    uint32_t emvWWTetu = 0;
    SMARTCARD_T0CASE_TYPES CURRENT_T0_CASE = SMARTCARD_UNKNOWN_CASE;
    uint32_t dataLength;
    uint32_t remainingBytesNum;
    uint32_t case4ResponseLen;
    uint16_t compareIndex = 0;
    uint16_t rxCardDataLength = 0;
    uint16_t txDataLength = 0;
    uint8_t tpduCommand[5];
    uint8_t index;
    uint8_t txIndex = 0;
    uint8_t rxIndex = 0;
    uint8_t offset;
    uint8_t rx_char = 0;
    uint8_t ins;
    uint8_t complimentedIns;
    uint8_t LoopContinue = 1;

    // Return False if there is no card inserted in the Slot or ATR of the card is unsucessful
    if (!SMARTCARD_Drv_CardPresent() || (gCardState != SMARTCARD_ATR_ON)) 
    {
        SMARTCARD_Shutdown();
        return ( SMARTCARD_ERROR_CARD_NOT_PRESENT);
    }

    // Calculate Work Waiting Time requirement for EMV Cards
    emvWWTetu = t0WWTetu + (factorDNumerator * 480UL) / factorDdenominator;

    // Copy APDU command in temporary TPDU buffer
    tpduCommand[0] = apduCommand[0];
    tpduCommand[1] = apduCommand[1];
    tpduCommand[2] = apduCommand[2];
    tpduCommand[3] = apduCommand[3];
    tpduCommand[4] = apduCommand[4];

    // Determine the case type of command transfer as per ISO 7816-3 specifications, Also
    // modify TPDU buffer accordingly
    if (apduCommandLength == 0x04) 
    {
        tpduCommand[4] = 0x00;
        CURRENT_T0_CASE = SMARTCARD_CASE_1;
    } 
    else if (apduCommandLength == 0x05) 
    {
        dataLength = tpduCommand[4];
        CURRENT_T0_CASE = SMARTCARD_CASE_2S;
        if (dataLength == 0x00) 
        {
            dataLength = 256;
        }
    } 
    else 
    {
        dataLength = tpduCommand[4];
        offset = 5;
        if (apduCommandLength == (5 + apduCommand[4])) 
        {
            CURRENT_T0_CASE = SMARTCARD_CASE_3S;
        } 
        else if (apduCommandLength == (6 + apduCommand[4])) 
        {
            CURRENT_T0_CASE = SMARTCARD_CASE_4S;
            case4ResponseLen = apduCommand[apduCommandLength - 1];
            if (case4ResponseLen == 0x00) 
            {
                case4ResponseLen = 256;
            }
        }
    }

    // If the APDU command is none of the case type of ISO 7816 standard then return 0
    if (CURRENT_T0_CASE == SMARTCARD_UNKNOWN_CASE) 
    {
        SMARTCARD_Shutdown();
        return ( SMARTCARD_ERROR_CMD_APDU_T0);
    }

    // Store Case 2, Case 3 or Case 4 bytes expected or bytes to be transmitted in tracking variable
    remainingBytesNum = dataLength;

    //Send the Command Bytes: CLA INS P1 P2 P3
    for (index = 0; index < 5; index++) 
    {
#ifdef SMARTCARD_SUPPORT_IN_HARDWARE
        if (index == 4) 
        {
            isLastByte = 1;
        }

#else
        WaitMicroSec(cgtInMicroSeconds);
#endif
        if (!DRV_SMARTCARD_DataSend(tpduCommand[index], isLastByte)) //wait for data byte from CARD
        {
            SMARTCARD_Shutdown();
            return ( SMARTCARD_ERROR_TRANSMIT);
        }
    }

    // Continue untill you decide something !
    while (LoopContinue) 
    {
        isLastByte = 0;

        // Copy Instruction Byte in Local Variable
        ins = tpduCommand[1];
        complimentedIns = ~ins;

        // Get Procedure byte
        if (!DRV_SMARTCARD_DataGet(&rx_char, emvWWTetu)) //wait for data byte from CARD
        {
            SMARTCARD_Shutdown();
            return ( SMARTCARD_ERROR_CARD_NO_RESPONSE); //no response received
        }

        // Process Procedure Byte
        if (rx_char == 0x60) 
        {
            // Do Nothing
        } 
        else if (((rx_char & SMARTCARD_BITMASK_HIGHER_NIBBLE) == (uint8_t) SMARTCARD_STATUS_BYTE_T0_60) ||
                ((rx_char & SMARTCARD_BITMASK_HIGHER_NIBBLE) == (uint8_t) SMARTCARD_STATUS_BYTE_T0_90)) 
        {
            // SW1, get SW2
            apduResponse->SW1 = rx_char; //save SW1
            txIndex = 0;
            rxIndex = 0;

            //now receive SW2
            if (DRV_SMARTCARD_DataGet(&rx_char, emvWWTetu)) 
            { //wait for data byte from CARD
                apduResponse->SW2 = rx_char;
            } 
            else 
            {
                SMARTCARD_Shutdown();
                return ( SMARTCARD_ERROR_CARD_NO_RESPONSE); //no response received
            }

            // Execute Based on APDU case command
            switch (CURRENT_T0_CASE) 
            {
                case SMARTCARD_CASE_1:
                    // Stop for any of the status codes. You are Done !
                    LoopContinue = 0;
                    break;

                case SMARTCARD_CASE_2S: // Second procedure byte
                    if (apduResponse->SW1 == (uint8_t) SMARTCARD_SECOND_PROC_BYTE_6C) 
                    {
                        tpduCommand[4] = apduResponse->SW2;

                        //Send the Command Bytes: CLA INS P1 P2 P3
                        for (index = 0; index < 5; index++) 
                        {
#ifdef SMARTCARD_SUPPORT_IN_HARDWARE
                            if (index == 4) 
                            {
                                isLastByte = 1;
                            }

#else
                            WaitMicroSec(cgtInMicroSeconds);
#endif
                            if (!DRV_SMARTCARD_DataSend(tpduCommand[index], isLastByte)) //wait for data byte from CARD
                            {
                                SMARTCARD_Shutdown();
                                return ( SMARTCARD_ERROR_TRANSMIT); //no response received
                            }
                        }
                    } 
                    else if (apduResponse->SW1 == (uint8_t) SMARTCARD_SECOND_PROC_BYTE_61) 
                    {
                        tpduCommand[0] = 0x00;
                        tpduCommand[1] = GET_RESPONSE_INS;
                        tpduCommand[2] = 0x00;
                        tpduCommand[3] = 0x00;
                        tpduCommand[4] = apduResponse->SW2;

                        //Send the Command Bytes: CLA INS P1 P2 P3
                        for (index = 0; index < 5; index++) 
                        {
#ifdef SMARTCARD_SUPPORT_IN_HARDWARE
                            if (index == 4) 
                            {
                                isLastByte = 1;
                            }

#else
                            WaitMicroSec(cgtInMicroSeconds);
#endif
                            if (!DRV_SMARTCARD_DataSend(tpduCommand[index], isLastByte)) //wait for data byte from CARD
                            {
                                SMARTCARD_Shutdown();
                                return ( SMARTCARD_ERROR_TRANSMIT); //no response received
                            }
                        }
                    } 
                    else 
                    {
                        LoopContinue = 0;
                    }

                    break;

                case SMARTCARD_CASE_3S:
                    if (((apduResponse->SW1 == 0x90) && (apduResponse->SW2 == 0x00)) ||
                            ((apduResponse->SW1 == 0x62) && (apduResponse->SW2 == 0x81)) ||
                            ((apduResponse->SW1 == 0x63) && (apduResponse->SW2 != 0x35))) 
                    {
                        LoopContinue = 0;
                    } 
                    else if ((apduResponse->SW1 == (uint8_t) SMARTCARD_SECOND_PROC_BYTE_61) ||
                            ((apduResponse->SW1 == 0x62) && (ins != 0x82)) || ((apduResponse->SW1 == 0x63) && (ins != 0x82)) ||
                            (((apduResponse->SW1 == 0x9F) && (ins != 0x82)) && (txDataLength == dataLength))) 
                    {
                        txDataLength = dataLength;
                        tpduCommand[0] = 0x00;
                        tpduCommand[1] = GET_RESPONSE_INS;
                        tpduCommand[2] = 0x00;
                        tpduCommand[3] = 0x00;

                        if (apduResponse->SW1 == (uint8_t) SMARTCARD_SECOND_PROC_BYTE_61) 
                        {
                            tpduCommand[4] = apduResponse->SW2;
                        } 
                        else 
                        {
                            tpduCommand[4] = 0x00;
                        }

                        //Send the Command Bytes: CLA INS P1 P2 P3
                        for (index = 0; index < 5; index++) 
                        {
#ifdef SMARTCARD_SUPPORT_IN_HARDWARE
                            if (index == 4) 
                            {
                                isLastByte = 1;
                            }

#else
                            WaitMicroSec(cgtInMicroSeconds);
#endif
                            if (!DRV_SMARTCARD_DataSend(tpduCommand[index], isLastByte)) //wait for data byte from CARD
                            {
                                SMARTCARD_Shutdown();
                                return ( SMARTCARD_ERROR_TRANSMIT); //no response received
                            }
                        }
                    } 
                    else if (apduResponse->SW1 == (uint8_t) SMARTCARD_SECOND_PROC_BYTE_6C) 
                    {
                        tpduCommand[4] = apduResponse->SW2;
                        txDataLength = dataLength;

                        //Send the Command Bytes: CLA INS P1 P2 P3
                        for (index = 0; index < 5; index++) 
                        {
#ifdef SMARTCARD_SUPPORT_IN_HARDWARE
                            if (index == 4) 
                            {
                                isLastByte = 1;
                            }

#else
                            WaitMicroSec(cgtInMicroSeconds);
#endif
                            if (!DRV_SMARTCARD_DataSend(tpduCommand[index], isLastByte)) //wait for data byte from CARD
                            {
                                SMARTCARD_Shutdown();
                                return ( SMARTCARD_ERROR_TRANSMIT); //no response received
                            }
                        }
                    } 
                    else 
                    {
                        LoopContinue = 0;
                    }

                    break;

                case SMARTCARD_CASE_4S:
                    if ((apduResponse->SW1 == (uint8_t) SMARTCARD_SECOND_PROC_BYTE_61) || (apduResponse->SW1 == 0x62) ||
                            (apduResponse->SW1 == 0x63) || (apduResponse->SW1 == (uint8_t) SMARTCARD_SECOND_PROC_BYTE_6C)) 
                    {
                        if ((apduResponse->SW2) != 0x84) 
                        {
                            if ((apduResponse->SW1 == (uint8_t) SMARTCARD_SECOND_PROC_BYTE_61) ||
                                    (apduResponse->SW1 == (uint8_t) SMARTCARD_SECOND_PROC_BYTE_6C)) 
                            {
                                tpduCommand[4] = apduResponse->SW2;
                            } 
                            else if (((apduResponse->SW1 == 0x62) && (apduResponse->SW2 == 0x81)) ||
                                    ((apduResponse->SW1 == 0x63) && (apduResponse->SW2 != 0x35))) 
                            {
                                LoopContinue = 0;
                                break;
                            } 
                            else 
                            {
                                tpduCommand[4] = 0x00;
                            }

                            tpduCommand[0] = 0x00;
                            tpduCommand[1] = GET_RESPONSE_INS;
                            tpduCommand[2] = 0x00;
                            tpduCommand[3] = 0x00;

                            dataLength = tpduCommand[4];
                            txDataLength = dataLength;
                        } 
                        else 
                        {
                            tpduCommand[0] = apduResponse->apduData[0];
                            tpduCommand[1] = apduResponse->apduData[1];
                            tpduCommand[2] = apduResponse->apduData[2];
                            tpduCommand[3] = apduResponse->apduData[3];
                            tpduCommand[4] = apduResponse->apduData[4];
                            rxCardDataLength = 0;
                        }

                        //Send the Command Bytes: CLA INS P1 P2 P3
                        for (index = 0; index < 5; index++) 
                        {
#ifdef SMARTCARD_SUPPORT_IN_HARDWARE
                            if (index == 4) 
                            {
                                isLastByte = 1;
                            }

#else
                            WaitMicroSec(cgtInMicroSeconds);
#endif
                            if (!DRV_SMARTCARD_DataSend(tpduCommand[index], isLastByte)) //wait for data byte from CARD
                            {
                                SMARTCARD_Shutdown();
                                return ( SMARTCARD_ERROR_TRANSMIT); //no response received
                            }
                        }
                    } 
                    else 
                    {
                        LoopContinue = 0;
                    }

                default:
                    break;
            }
        } 
        else if (rx_char == ins) 
        {
            switch (CURRENT_T0_CASE) 
            {
                case SMARTCARD_CASE_2S:
                    // Recive all remaining bytes
                    if (tpduCommand[4] == 0) 
                    {
                        compareIndex = 256;
                    } 
                    else 
                    {
                        compareIndex = tpduCommand[4];
                    }

                    for (; rxIndex < compareIndex; rxIndex++) 
                    {
                        if (DRV_SMARTCARD_DataGet(&rx_char, emvWWTetu)) 
                        { //wait for data byte from CARD
                            apduResponse->apduData[rxCardDataLength++] = rx_char;
                        } 
                        else 
                        {
                            SMARTCARD_Shutdown();
                            return ( SMARTCARD_ERROR_CARD_NO_RESPONSE); //no response received
                        }
                    }

                    rxIndex = 0;
                    break;

                case SMARTCARD_CASE_3S:
                    if (txDataLength < dataLength) //transmit app data if any
                    {
                        for (; txIndex < tpduCommand[4]; txIndex++) 
                        {
#ifdef SMARTCARD_SUPPORT_IN_HARDWARE
                            if (index == 4) 
                            {
                                isLastByte = 1;
                            }

#else
                            WaitMicroSec(cgtInMicroSeconds);
#endif
                            if (!DRV_SMARTCARD_DataSend(apduCommand[offset + txDataLength], isLastByte)) //wait for data byte from CARD
                            {
                                SMARTCARD_Shutdown();
                                return ( SMARTCARD_ERROR_TRANSMIT); //no response received
                            }

                            txDataLength++;
                        }

                        txIndex = 0;
                    } 
                    else if (tpduCommand[1] == GET_RESPONSE_INS) 
                    {
                        if (tpduCommand[4] == 0) 
                        {
                            compareIndex = 256;
                        } 
                        else 
                        {
                            compareIndex = tpduCommand[4];
                        }

                        for (; rxIndex < compareIndex; rxIndex++) 
                        {
                            if (DRV_SMARTCARD_DataGet(&rx_char, emvWWTetu)) 
                            { //wait for data byte from CARD
                                apduResponse->apduData[rxCardDataLength++] = rx_char;
                            } 
                            else 
                            {
                                SMARTCARD_Shutdown();
                                return ( SMARTCARD_ERROR_CARD_NO_RESPONSE); //no response received
                            }
                        }

                        rxIndex = 0;
                    }

                    break;

                case SMARTCARD_CASE_4S:
                    if (txDataLength < dataLength) //transmit app data if any
                    {
                        for (; txIndex < tpduCommand[4]; txIndex++) 
                        {
#ifdef SMARTCARD_SUPPORT_IN_HARDWARE
                            if (index == 4) 
                            {
                                isLastByte = 1;
                            }

#else
                            WaitMicroSec(cgtInMicroSeconds);
#endif
                            if (!DRV_SMARTCARD_DataSend(apduCommand[offset + txDataLength], isLastByte)) //wait for data byte from CARD
                            {
                                SMARTCARD_Shutdown();
                                return ( SMARTCARD_ERROR_TRANSMIT); //no response received
                            }

                            txDataLength++;
                        }

                        txDataLength = txDataLength + offset;
                        txIndex = 0;
                    } 
                    else 
                    {
                        if (tpduCommand[4] == 0) 
                        {
                            compareIndex = 256;
                        } 
                        else 
                        {
                            compareIndex = tpduCommand[4];
                        }

                        for (; rxIndex < compareIndex; rxIndex++) 
                        {
                            if (DRV_SMARTCARD_DataGet(&rx_char, emvWWTetu)) 
                            { //wait for data byte from CARD
                                apduResponse->apduData[rxCardDataLength++] = rx_char;
                            } 
                            else 
                            {
                                SMARTCARD_Shutdown();
                                return ( SMARTCARD_ERROR_CARD_NO_RESPONSE); //no response received
                            }
                        }

                        rxIndex = 0;
                    }

                default:
                    break;
            }
        } else if (rx_char == complimentedIns) 
        {
            switch (CURRENT_T0_CASE) 
            {
                case SMARTCARD_CASE_3S:
                    if (txDataLength < dataLength) //transmit app data if any
                    {
                        if (txIndex < tpduCommand[4]) 
                        {
#ifndef SMARTCARD_SUPPORT_IN_HARDWARE
                            WaitMicroSec(cgtInMicroSeconds);
#endif
                            if (!DRV_SMARTCARD_DataSend(apduCommand[offset + txDataLength], 1)) //wait for data byte from CARD
                            {
                                SMARTCARD_Shutdown();
                                return ( SMARTCARD_ERROR_TRANSMIT); //no response received
                            }

                            txDataLength++;
                            txIndex++;
                        }
                    } 
                    else if (tpduCommand[1] == GET_RESPONSE_INS) 
                    {
                        if (tpduCommand[4] == 0) 
                        {
                            compareIndex = 256;
                        } 
                        else 
                        {
                            compareIndex = tpduCommand[4];
                        }

                        if (rxIndex < compareIndex) 
                        {
                            if (DRV_SMARTCARD_DataGet(&rx_char, emvWWTetu)) //wait for data byte from CARD
                            {
                                apduResponse->apduData[rxCardDataLength++] = rx_char;
                                rxIndex++;
                            } 
                            else 
                            {
                                SMARTCARD_Shutdown();
                                return ( SMARTCARD_ERROR_CARD_NO_RESPONSE); //no response received
                            }
                        }
                    }

                    break;

                case SMARTCARD_CASE_4S:
                    if (txDataLength < dataLength) //transmit app data if any
                    {
                        if (txIndex < tpduCommand[4]) 
                        {
#ifndef SMARTCARD_SUPPORT_IN_HARDWARE
                            WaitMicroSec(cgtInMicroSeconds);
#endif
                            if (!DRV_SMARTCARD_DataSend(apduCommand[offset + txDataLength], 1)) //wait for data byte from CARD
                            {
                                SMARTCARD_Shutdown();
                                return ( SMARTCARD_ERROR_TRANSMIT); //no response received
                            }

                            txDataLength++;
                            txIndex++;
                        }
                    } 
                    else 
                    {
                        case SMARTCARD_CASE_2S:
                        {
                            if (tpduCommand[4] == 0) 
                            {
                                compareIndex = 256;
                            } 
                            else 
                            {
                                compareIndex = tpduCommand[4];
                            }

                            if (rxIndex < compareIndex) 
                            {
                                if (DRV_SMARTCARD_DataGet(&rx_char, emvWWTetu)) //wait for data byte from CARD
                                {
                                    apduResponse->apduData[rxCardDataLength++] = rx_char;
                                    rxIndex++;
                                } 
                                else 
                                {
                                    SMARTCARD_Shutdown();
                                    return ( SMARTCARD_ERROR_CARD_NO_RESPONSE); //no response received
                                }
                            }
                        }
                    }

                default:
                    break;
            }
        } 
        else 
        {
            SMARTCARD_Shutdown();
            return ( SMARTCARD_ERROR_PROCEDURE_BYTE); //no response received
        }
    }

    // Store the number of recieved data bytes including the
    // status codes to make the life of Smart Card Reader easier
    apduResponse->rxDataLen = rxCardDataLength;

    return ( SMARTCARD_TRANSACTION_SUCCESSFUL);
}
/*******************************************************************************
  Function:
    SMARTCARD_TRANSACTION_STATUS SMARTCARD_EMV_TransactT1(SMARTCARD_T1_PROLOGUE_FIELD* pfield,uint8_t* iField,SMARTCARD_APDU_RESPONSE* apduResponse)

  Description:
    This function Sends/recieves  T = 1 commands to the card.

  Precondition:
    SMARTCARD_PPS was success

  Parameters:
    SMARTCARD_T1_PROLOGUE_FIELD* pfield - Pointer to Prologue Field
    uint8_t* iField - Pointer to the Information Field of Tx/Rx Data
    SMARTCARD_APDU_RESPONSE* apduResponse - Pointer to APDU Response structure

  Return Values:
    1 if transaction was success, and followed the ISO 7816-4 protocol.

  Remarks:
 *****************************************************************************/
SMARTCARD_TRANSACTION_STATUS SMARTCARD_EMV_DataExchangeT1(SMARTCARD_T1PROLOGUE_FIELD* pfield, uint8_t* iField, SMARTCARD_APDU_RESPONSE* apduResponse) 
{
    SMARTCARD_T1BLOCK_TYPES t1TxBlockType;
    uint8_t txPCB, rxNAD, rxPCB, txLength, rxLEN, retryR, startCount;
    int iFieldYetToBeTransmitted;
    bool txMbit, rxMbit, rxSbit;
    bool rxSbitRBlock = 0;
    bool tempSbitIBlock = false;
    uint8_t *rxField, *txField;
    unsigned int rxLength;
    prevIFS = 0;
start:
    cwtExceeded = false;
    iFieldYetToBeTransmitted = pfield->length;
    txLength = iFieldYetToBeTransmitted;
    txPCB = pfield->PCB;
    txField = iField;
    rxLength = 0;
    retryR = 0, startCount = 0;
    rxField = apduResponse->apduData;
    txMbit = false;
    rxMbit = false;
    rxSbit = false;


    // Determine which type of block is to be transmitted to the card
    if ((txPCB & SMARTCARD_I_BLOCK_MASK) == SMARTCARD_I_BLOCK_IDENTIFIER) 
    {
        // I-Block
        t1TxBlockType = SMARTCARD_I_BLOCK;

        if (txSbit) 
        {
            txPCB = txPCB & SMARTCARD_S_BIT_CLR;
            txSbit = false;
        } 
        else 
        {
            txPCB = txPCB | SMARTCARD_S_BIT_SET;
            txSbit = true;
        }

        // Set/Reset the M bit appropriately
        if (iFieldYetToBeTransmitted > maxSegmentLength) 
        {
            txLength = maxSegmentLength;
            txMbit = true;
            txPCB = txPCB | SMARTCARD_M_BIT_SET;
        } 
        else 
        {
            txMbit = false;
            txPCB = txPCB & SMARTCARD_M_BIT_CLR;
        }
    } 
    else if ((txPCB & SMARTCARD_S_BLOCK_MASK) == SMARTCARD_S_BLOCK_IDENTIFIER) 
    {
        // S-Block
        t1TxBlockType = SMARTCARD_S_BLOCK;
        if (iFieldYetToBeTransmitted > 1)
            return SMARTCARD_ERROR_CMD_APDU_T1;
    } 
    else 
    {
        // INVALID BLOCK
        return SMARTCARD_ERROR_CMD_APDU_T1;
    }

    // Initialize the recieved data length packet to zero
    apduResponse->rxDataLen = 0;

    currT1BWTetu = t1BWTetu;

    // Go to appropriate case depending upon the type of block
    switch (t1TxBlockType) 
    {
        case SMARTCARD_I_BLOCK:

            // Continue Untill Transaction is Passed or Failed...
            while (1) 
            {
                if ((txPCB & SMARTCARD_I_BLOCK_MASK) == SMARTCARD_I_BLOCK_IDENTIFIER) 
                {
                    // I-Block
                    t1TxBlockType = SMARTCARD_I_BLOCK;
                } 
                else if ((txPCB & SMARTCARD_R_BLOCK_MASK) == SMARTCARD_R_BLOCK_IDENTIFIER) 
                {
                    t1TxBlockType = SMARTCARD_R_BLOCK;
                } 
                else if ((txPCB & SMARTCARD_S_BLOCK_MASK) == SMARTCARD_S_BLOCK_IDENTIFIER) 
                {
                    t1TxBlockType = SMARTCARD_S_BLOCK;
                } 
                else 
                {
                    t1TxBlockType = SMARTCARD_INVALID_BLOCK;
                }

                // Send block with chaining mode, current sequence number, and maximum length.
                SMARTCARD_SendT1Block(0x00, txPCB, txLength, txField);

                // Recieve the Block
                if (SMARTCARD_ReceiveT1Block(&rxNAD, &rxPCB, &rxLEN, rxField)) 
                {
                    if (currentT1RxBlockType != SMARTCARD_INVALID_BLOCK) 
                    {
                        // Determine the type of Block recieved from the card
                        if (currentT1RxBlockType == SMARTCARD_I_BLOCK) 
                        {
                            if (txMbit == true) 
                            {
                                errType = OTHER_ERROR;
                                currentT1RxBlockType = SMARTCARD_INVALID_BLOCK;
                            } 
                            else 
                            {
                                if ((rxPCB & SMARTCARD_S_BIT_SET) == SMARTCARD_S_BIT_SET)
                                    tempSbitIBlock = true;
                                else
                                    tempSbitIBlock = false;


                                if (tempSbitIBlock == rxSbitPrevIBlk) 
                                {
                                    errType = OTHER_ERROR;
                                    currentT1RxBlockType = SMARTCARD_INVALID_BLOCK;
                                } 
                                else if (t1TxBlockType == SMARTCARD_I_BLOCK && ((txPCB & SMARTCARD_M_BIT_SET) == SMARTCARD_M_BIT_SET) && ((rxPCB & SMARTCARD_M_BIT_SET) == SMARTCARD_M_BIT_SET)) 
                                {
                                    errType = OTHER_ERROR;
                                    currentT1RxBlockType = SMARTCARD_INVALID_BLOCK;
                                } 
                                else 
                                {
                                    if ((rxPCB & SMARTCARD_M_BIT_SET) == SMARTCARD_M_BIT_SET)
                                        rxMbit = true;
                                    else
                                        rxMbit = false;

                                    if ((rxPCB & SMARTCARD_S_BIT_SET) == SMARTCARD_S_BIT_SET)
                                        rxSbit = true;
                                    else
                                        rxSbit = false;
                                    rxSbitPrevIBlk = rxSbit;
                                }
                            }
                        }
                        if (currentT1RxBlockType != SMARTCARD_INVALID_BLOCK) 
                        {
                            if (currentT1RxBlockType == SMARTCARD_I_BLOCK) 
                            {
                                retryR = 0;
                            }
                        }

                    }
                }

                if (cwtExceeded && (deactivationEnabled || deactivateOnIFSLenErrorEnabled)) 
                {
                    SMARTCARD_Shutdown();
                    cwtExceeded = false;
                    return ( SMARTCARD_ERROR_CARD_NO_RESPONSE);
                } 
                else 
                {
                    if (cwtExceeded) 
                    {
                        errType = OTHER_ERROR;
                    }
                }

                if (startCount < 6)
                    startCount++;

                currT1BWTetu = t1BWTetu;


                switch (currentT1RxBlockType) 
                {
                    case SMARTCARD_I_BLOCK:

                        rxField = rxField + (uint8_t) rxLEN;
                        rxLength = rxLength + rxLEN;

                        // If More Bit is set by the card, send the apprpriate R Block
                        if (rxMbit) 
                        {
                            // Transmit R(N) - Expected Seq
                            txLength = 0x00;

                            if (rxSbit)
                                txPCB = SMARTCARD_R_BLOCK_IDENTIFIER;
                            else
                                txPCB = SMARTCARD_R_BLOCK_IDENTIFIER | SMARTCARD_R_BLOCK_S_BIT_SET;

                            // R-Block
                            t1TxBlockType = SMARTCARD_R_BLOCK;
                            continue;
                        } 
                        else 
                        {
                            if (rxLength >= 2) 
                            {
                                apduResponse->rxDataLen = rxLength - 2;
                                apduResponse->SW1 = *(apduResponse->apduData + rxLength - 2);
                                apduResponse->SW2 = *(apduResponse->apduData + rxLength - 1);
                            }
                            return SMARTCARD_TRANSACTION_SUCCESSFUL;
                        }

                    case SMARTCARD_S_BLOCK:

                        if (rxPCB == SMARTCARD_IFS_REQUEST) // Request IFS Change
                        {
                            txPCB = SMARTCARD_IFS_RESPONSE;
                            txLength = 1;
                            txField = rxField;
                            prevIFS = maxSegmentLength;
                            maxSegmentLength = *rxField;
                            // R-Block
                            t1TxBlockType = SMARTCARD_S_BLOCK;
                            continue;
                        } 
                        else if (rxPCB == SMARTCARD_WAIT_TIME_EXT_REQUEST) // Request Wait time Extension
                        {
                            currT1BWTetu = t1BWTetu * *rxField;
                            txPCB = SMARTCARD_WAIT_TIME_EXT_RESPONSE;
                            txLength = 1;
                            txField = rxField;
                            // R-Block
                            t1TxBlockType = SMARTCARD_S_BLOCK;
                            continue;
                        } 
                        else if (rxPCB == SMARTCARD_ABORT_REQUEST)// Abort Request
                        {
                            if (abortResponseBeforeDeactivation) 
                            {
                                txPCB = SMARTCARD_ABORT_RESPONSE;
                                txLength = 0;
                                SMARTCARD_SendT1Block(0x00, txPCB, txLength, txField);
                            }
                            // Deactivate the device
                            txSbit = false;
                            SMARTCARD_Shutdown();
                            return SMARTCARD_TRANSACTION_SUCCESSFUL;
                        } 
                        else if (rxPCB == SMARTCARD_WAIT_TIME_EXT_RESPONSE) 
                        {
                            if (txPCB == SMARTCARD_WAIT_TIME_EXT_REQUEST) {
                                txSbit = true;
                                goto start;
                            } 
                            else 
                            {
                                currentT1RxBlockType = SMARTCARD_INVALID_BLOCK;
                                errType = OTHER_ERROR;
                                goto LabelInvalidBlock;
                            }
                        } 
                        else if (rxPCB == SMARTCARD_RESYNC_RESPONSE) // Abort Response from the card
                        {
                            if (txPCB == SMARTCARD_RESYNC_REQ) 
                            {
                                txSbit = true;
                                goto start;
                            } 
                            else 
                            {
                                currentT1RxBlockType = SMARTCARD_INVALID_BLOCK;
                                errType = OTHER_ERROR;
                                goto LabelInvalidBlock;
                            }
                        } 
                        else if (rxPCB == SMARTCARD_ABORT_RESPONSE) // Resync Response from the card
                        {
                            if (txPCB == SMARTCARD_ABORT_REQUEST) 
                            {
                                txSbit = true;
                                goto start;
                            } 
                            else 
                            {
                                currentT1RxBlockType = SMARTCARD_INVALID_BLOCK;
                                errType = OTHER_ERROR;
                                goto LabelInvalidBlock;
                            }
                        } 
                        else if (rxPCB == SMARTCARD_IFS_RESPONSE) 
                        {
                            currentT1RxBlockType = SMARTCARD_INVALID_BLOCK;
                            errType = OTHER_ERROR;
                            goto LabelInvalidBlock;
                        }

                        break;

                    case SMARTCARD_R_BLOCK:
                        if ((rxPCB & SMARTCARD_R_BLOCK_S_BIT_SET) == SMARTCARD_R_BLOCK_S_BIT_SET) 
                        {
                            rxSbitRBlock = true;
                        } 
                        else 
                        {
                            rxSbitRBlock = false;
                        }
                        // If More Bit is set by the reader
                        if (txMbit) 
                        {
                            if (rxSbitRBlock != txSbit) 
                            {
                                // Transmit next segment.

                                if (prevIFS != 0) 
                                {
                                    iFieldYetToBeTransmitted = iFieldYetToBeTransmitted - prevIFS;
                                    txField = txField /*changed  from iField to txField*/ + prevIFS;
                                    prevIFS = 0;
                                } 
                                else 
                                {
                                    iFieldYetToBeTransmitted = iFieldYetToBeTransmitted - maxSegmentLength;
                                    txField = txField /*changed  from iField to txField*/ + maxSegmentLength;
                                }
                                // I-Block
                                t1TxBlockType = SMARTCARD_I_BLOCK;
                                txPCB = pfield->PCB;


                                if (txSbit) 
                                {
                                    txPCB = txPCB & SMARTCARD_S_BIT_CLR;
                                    txSbit = false;
                                } 
                                else 
                                {
                                    txPCB = txPCB | SMARTCARD_S_BIT_SET;
                                    txSbit = true;
                                }
                                // Set/Reset the M bit appropriately
                                if (iFieldYetToBeTransmitted > maxSegmentLength) 
                                {
                                    txLength = maxSegmentLength;
                                    txMbit = true;
                                    txPCB = txPCB | SMARTCARD_M_BIT_SET;
                                } 
                                else 
                                {
                                    txLength = iFieldYetToBeTransmitted;
                                    txMbit = false;
                                    txPCB = txPCB & SMARTCARD_M_BIT_CLR;
                                }
                            } 
                            else 
                            {
                                retryR++;
                                if (retryR > 2) 
                                {
                                    if (resyncEnabled == true) 
                                    {
                                        txPCB = SMARTCARD_RESYNC_REQ;
                                        txLength = 0x00;
                                    } 
                                    else 
                                    {
                                        SMARTCARD_Shutdown();
                                        return ( SMARTCARD_ERROR_T1_RETRY);
                                    }
                                }
                                // Deactivate
                            }
                        } 
                        else 
                        {
                            if (rxMbit) 
                            {
                                txPCB = txPCB;
                                txLength = 0x00;
                            } 
                            else 
                            {
                                if (rxSbitRBlock == txSbit) 
                                {

                                    if (rxSbit) 
                                    {
                                        txPCB = SMARTCARD_R_BLOCK_IDENTIFIER;
                                        txLength = 0x00;
                                    } 
                                    else 
                                    {
                                        if (retryR < 2) 
                                        {
                                            // I-Block
                                            t1TxBlockType = SMARTCARD_I_BLOCK;
                                            txPCB = pfield->PCB;

                                            if (txSbit)
                                                txPCB = txPCB | SMARTCARD_S_BIT_SET;
                                            else
                                                txPCB = txPCB & SMARTCARD_S_BIT_CLR;

                                            // Set/Reset the M bit appropriately
                                            if (iFieldYetToBeTransmitted > maxSegmentLength) 
                                            {
                                                txLength = maxSegmentLength;
                                                txMbit = true;
                                                txPCB = txPCB | SMARTCARD_M_BIT_SET;
                                            } 
                                            else 
                                            {
                                                txLength = iFieldYetToBeTransmitted;
                                                txMbit = false;
                                                txPCB = txPCB & SMARTCARD_M_BIT_CLR;
                                            }
                                        } 
                                        else 
                                        {
                                            if (resyncEnabled == true) 
                                            {
                                                txPCB = SMARTCARD_RESYNC_REQ;
                                                txLength = 0x00;
                                            } 
                                            else 
                                            {
                                                SMARTCARD_Shutdown();
                                                return SMARTCARD_ERROR_T1_RETRY;
                                            }
                                        }
                                        retryR++;
                                    }
                                }
                                else 
                                {
                                    if ((txPCB & SMARTCARD_R_BLOCK_MASK) == SMARTCARD_R_BLOCK_IDENTIFIER) 
                                    {
                                        txPCB = txPCB;
                                        t1TxBlockType = SMARTCARD_R_BLOCK;
                                    } 
                                    else 
                                    {
                                        errType = OTHER_ERROR;
                                        if (rxSbitPrevIBlk) 
                                        {
                                            txPCB = SMARTCARD_R_BLOCK_IDENTIFIER | errType;
                                        } 
                                        else 
                                        {
                                            txPCB = SMARTCARD_R_BLOCK_IDENTIFIER | SMARTCARD_R_BLOCK_S_BIT_SET | errType;
                                        }
                                        t1TxBlockType = SMARTCARD_R_BLOCK;
                                    }
                                    txLength = 0x00;
                                }
                            }
                            continue;
                        }
                        break;
LabelInvalidBlock:
                    case SMARTCARD_INVALID_BLOCK:
                    case SMARTCARD_NO_BLOCK:

                        if (rxMbit == false) 
                        {
                            if (retryR < 2) 
                            {

                                if (((txPCB & SMARTCARD_I_BLOCK_MASK) == SMARTCARD_I_BLOCK_IDENTIFIER) || t1TxBlockType == SMARTCARD_S_BLOCK) 
                                {
                                    if (rxSbitPrevIBlk) 
                                    {
                                        txPCB = SMARTCARD_R_BLOCK_IDENTIFIER | errType;
                                    } 
                                    else 
                                    {
                                        txPCB = SMARTCARD_R_BLOCK_IDENTIFIER | SMARTCARD_R_BLOCK_S_BIT_SET | errType;
                                    }
                                } 
                                else 
                                {

                                }

                            } 
                            else 
                            {
                                if (resyncEnabled == true) 
                                {
                                    txPCB = SMARTCARD_RESYNC_REQ;
                                } 
                                else 
                                {
                                    SMARTCARD_Shutdown();
                                    return SMARTCARD_ERROR_T1_RETRY;
                                }
                            }

                            txLength = 0x00;
                            retryR++;
                        }

                        if ((retryR == 6) || (startCount == 10)) 
                        {
                            // Deactivate the Device
                            txSbit = false;
                            SMARTCARD_Shutdown();
                            return SMARTCARD_ERROR_T1_RETRY;
                        }

                    default:
                        break;
                }//I Block Switch end
            } // I Block While end

            break;

        case SMARTCARD_S_BLOCK:
            // Continue Untill Transaction is Passed or Failed...
            while (1) 
            {
                // Send block with chaining mode, current sequence number, and maximum length.
                SMARTCARD_SendT1Block(0x00, txPCB, txLength, txField);

                // Recieve the Block
                if (SMARTCARD_ReceiveT1Block(&rxNAD, &rxPCB, &rxLEN, rxField)) 
                {
                    if (currentT1RxBlockType == SMARTCARD_S_BLOCK) {

                        if (rxPCB == SMARTCARD_IFS_RESPONSE && (*txField != *rxField)) 
                        {
                            retryR++;
                            continue;
                        }
                        retryR = 0;
                    }
                }

                if (cwtExceeded && deactivationEnabled) 
                {
                    SMARTCARD_Shutdown();
                    cwtExceeded = false;
                    return ( SMARTCARD_ERROR_CARD_NO_RESPONSE);
                }

                switch (currentT1RxBlockType) 
                {
                    case SMARTCARD_I_BLOCK:
                    case SMARTCARD_R_BLOCK:
                    case SMARTCARD_INVALID_BLOCK:
                    case SMARTCARD_NO_BLOCK:

                        if (retryR >= 2) 
                        {
                            if ((resyncEnabled == false) && deactivationEnabled) 
                            {
                                SMARTCARD_Shutdown();
                                return ( SMARTCARD_ERROR_T1_RETRY);
                            } 
                            else 
                            {
                                if (resyncEnabled == true) 
                                {
                                    retryR = 0;
                                    txPCB = SMARTCARD_RESYNC_REQ;
                                    txLength = 0;
                                } 
                                else 
                                {
                                    SMARTCARD_Shutdown();
                                    return ( SMARTCARD_ERROR_T1_RETRY);
                                }
                            }

                        }
                        retryR++;
                        continue;

                    case SMARTCARD_S_BLOCK:

                        if (rxPCB & SMARTCARD_S_BLOCK_REQ_RESP_BIT) // Response
                        {
                            if (*txField == *rxField) 
                            {
                                apduResponse->rxDataLen = rxLEN;
                                return ( SMARTCARD_TRANSACTION_SUCCESSFUL);
                            } 
                            else 
                            {
                                if (txPCB == SMARTCARD_IFS_REQUEST && rxPCB == SMARTCARD_IFS_RESPONSE) 
                                {
                                    return ( SMARTCARD_TRANSACTION_SUCCESSFUL);
                                } 
                                else if (txPCB == SMARTCARD_RESYNC_REQ && rxPCB == SMARTCARD_RESYNC_RESPONSE) 
                                {
                                    return ( SMARTCARD_TRANSACTION_SUCCESSFUL);
                                } 
                                else 
                                {
                                    continue;
                                }
                            }
                        } 
                        else 
                        {
                            if (*rxField >= SMARTCARD_EMV_MIN_IFS_LENGTH && *rxField <= SMARTCARD_EMV_MAX_IFS_LENGTH) 
                            {
                                txPCB = SMARTCARD_IFS_REQUEST;
                                *txField = 0xFE;
                            } 
                            else 
                            {
                                // Deactivate the device
                                txSbit = false;
                                SMARTCARD_Shutdown();
                                return SMARTCARD_ERROR_T1_S_BLOCK_RESPONSE;
                            }
                        }
                }// S Block switch end
            }// S Block while end
        case SMARTCARD_NO_BLOCK:
        case SMARTCARD_R_BLOCK:
        case SMARTCARD_INVALID_BLOCK: break;
            break;
    }// switch end
    return SMARTCARD_ERROR_T1_INVALID_BLOCK;
}// function ed
