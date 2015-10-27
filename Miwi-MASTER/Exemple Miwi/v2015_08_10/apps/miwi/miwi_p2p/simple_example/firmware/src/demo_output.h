/********************************************************************
 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the "Company") for its PIC(R) Microcontroller is intended and
 supplied to you, the Company's customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
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
 *******************************************************************/
#ifndef _DEMO_OUPUT_H
#define _DEMO_OUTPUT_H

extern const uint8_t MiWi[6][21];
extern const uint8_t DE[6][11];

/*********************************************************************
* Function: void DemoOutput_Greeting( void )
*
* Overview: Prints demo greeting message
*
* PreCondition: None
*
* Input:  None
*
* Output: None
*
********************************************************************/
void DemoOutput_Greeting(void);

/*********************************************************************
* Function: void DemoOutput_Channel(uint8_t channel, uint8_t step)
*
* Overview: Prints demo channel
*
* PreCondition: None
*
* Input:  None
*
* Output: None
*
********************************************************************/
void DemoOutput_Channel(uint8_t channel, uint8_t step);

/*********************************************************************
* Function: void DemoOutput_Instruction( void )
*
* Overview: Prints demo instructions
*
* PreCondition: None
*
* Input:  None
*
* Output: None
*
********************************************************************/
void DemoOutput_Instruction(void);

/*********************************************************************
* Function: void DemoOutput_HandleMessage( void )
*
* Overview: Prints demo message type
*
* PreCondition: None
*
* Input:  None
*
* Output: None
*
********************************************************************/
void DemoOutput_HandleMessage(void);

/*********************************************************************
* Function: void DemoOutput_UpdateTxRx( uint8_t TxNum, uint8_t RxNum )
*
* Overview: Prints Tx and Rx Packet Count
*
* PreCondition: None
*
* Input:  None
*
* Output: None
*
********************************************************************/
void DemoOutput_UpdateTxRx(uint8_t TxNum, uint8_t RxNum);

/*********************************************************************
* Function: void DemoOutput_ChannelError( uint8_t channel )
*
* Overview: Outputs channel error message
*
* PreCondition: None
*
* Input:  None
*
* Output: None
*
********************************************************************/
void DemoOutput_ChannelError(uint8_t channel);

/*********************************************************************
* Function: void DemoOutput_UnitcastFail( void )
*
* Overview: Outputs Unicast fail status
*
* PreCondition: None
*
* Input:  None
*
* Output: None
*
********************************************************************/
void DemoOutput_UnicastFail(void);


/*********************************************************************
* Function: uint8_t ButtonPressed(void)
*
* Overview: Reads the Button state
*
* PreCondition: None
*
* Input:  None
*
* Output: Buttons State
*
********************************************************************/
uint8_t ButtonPressed(void);



#endif

