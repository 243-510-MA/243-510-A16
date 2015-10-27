/****************************************************************************
* FileName:         Demo.c
* Dependencies:     none   
* Processor:        PIC18F46J50	
* Complier:         Microchip C18 v3.04 or higher
* Company:          Microchip Technology, Inc.
*
* Copyright and Disclaimer Notice for MiWi DE Software:
*
* Copyright © 2007-2010 Microchip Technology Inc.  All rights reserved.
*
* Microchip licenses to you the right to use, modify, copy and distribute 
* Software only when embedded on a Microchip microcontroller or digital 
* signal controller and used with a Microchip radio frequency transceiver, 
* which are integrated into your product or third party product (pursuant 
* to the terms in the accompanying license agreement).   
*
* You should refer to the license agreement accompanying this Software for 
* additional information regarding your rights and obligations.
*
* SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY 
* KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY 
* WARRANTY OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A 
* PARTICULAR PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE 
* LIABLE OR OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, 
* CONTRIBUTION, BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY 
* DIRECT OR INDIRECT DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO 
* ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, 
* LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS, 
* TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES (INCLUDING BUT 
* NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*
****************************************************************************
* File Description:

*   This simple demo for the 8-bit Wireless Development Kit 
*   setups a application for chatting wirelessly. 
*   
**************************************************************************/

/************************ HEADERS ****************************************/
#include "system.h"
#include "system_config.h"
#include "miwi/miwi_api.h"
/************************** VARIABLES ************************************/

/*************************************************************************/

// The variable myChannel defines the channel that the device
// is operate on. This variable will be only effective if energy scan
// (ENABLE_ED_SCAN) is not turned on. Once the energy scan is turned
// on, the operating channel will be one of the channels available with
// least amount of energy (or noise).

/*************************************************************************/
uint8_t        myChannel = 11;

//Chat Window Application Variables
#define MAX_MESSAGE_LEN 35
uint8_t        TxMessage[MAX_MESSAGE_LEN];
uint8_t        TxMessageSize = 0;

//Uses ReadMacAddress routine to give unique ID to each node
extern uint8_t myLongAddress[];
extern CONNECTION_ENTRY    	ConnectionTable[CONNECTION_SIZE];
//Chat Application status variables
bool    messagePending = false;
bool    transmitPending = false;

//Timers used in the application to determine 'no activity'
MIWI_TICK    tickCurrent;
MIWI_TICK    tickPrevious;




/*************************************************************************/

// AdditionalNodeID variable array defines the additional
// information to identify a device on a PAN. This array
// will be transmitted when initiate the connection between
// the two devices. This  variable array will be stored in
// the Connection Entry structure of the partner device. The
// size of this array is ADDITIONAL_NODE_ID_SIZE, defined in
// ConfigApp.h.
// In this demo, this variable array is set to be empty.

/*************************************************************************/
#if ADDITIONAL_NODE_ID_SIZE > 0
uint8_t        AdditionalNodeID[ADDITIONAL_NODE_ID_SIZE] = {0x00};
#endif

void ReadMacAddress(void);
void ProcessRxMessage(void);
void FormatTxMessage(void);
void TransmitMessage(void);

/*********************************************************************
* Function:         void main(void)
*
* PreCondition:     none
*
* Input:		    none
*
* Output:		    none
*
* Side Effects:	    none
*
* Overview:		    This is the main function that runs the Chat application
*
* Note:			    
**********************************************************************/

void main (void)
{
    uint8_t    i;
    
    
    SYSTEM_Initialize();
    CONSOLE_Initialize();

    /*******************************************************************/

    // Initialize the system

    /*******************************************************************/

    /*******************************************************************/

    // Following block display demo information on LCD of Explore 16 or
    // PIC18 Explorer demo board.

    /*******************************************************************/
    LCDDisplay((char *)"Chat Demo", 0, true);

    // Clear the screen (VT100)
    Printf("\x1b[2J");
    // Send the cursor home (VT100)
    Printf("\x1b[H");
    Printf("\r\nChat Demo");
            #if defined(MRF24J40)
    Printf("\r\nRF Transceiver: MRF24J40");
            #elif defined(MRF49XA)
    Printf("\r\nRF Transceiver: MRF49XA");
            #elif defined(MRF89XA)
    Printf("\r\nRF Transceiver: MRF89XA");
            #endif
    Printf("\r\n\r\nDemo Instruction:");
    Printf("\r\nUse Console to Chat with the Peer Device");
    Printf("\r\n");
    LED_1 = 0;
    LED_2 = 0;

   /******************************************************************/

    // Read the MAC address from the MAC EEPROM on the PICTail Card

    ReadMacAddress();

    // ..and display on terminal

	Printf("\r\n\r\nMy MAC Address: 0x");
    for(i = 0; i < MY_ADDRESS_LENGTH; i++)
    {
        CONSOLE_PrintHex(myLongAddress[MY_ADDRESS_LENGTH-1-i]);
    }
    /*******************************************************************/

    // Initialize Microchip proprietary protocol. Which protocol to use
    // depends on the configuration in ConfigApp.h

    /*******************************************************************/

    /*******************************************************************/

    // Function MiApp_ProtocolInit initialize the protocol stack. The
    // only input parameter indicates if previous network configuration
    // should be restored. In this simple example, we assume that the
    // network starts from scratch.

    /*******************************************************************/
    MiApp_ProtocolInit(false);

    // Set default channel
    if(MiApp_SetChannel(myChannel) == false)
    {
        Printf("\r\nERROR: Unable to program the channel\r\n");
        Printf("\r\nPress MCLR to start again\r\n");
        LCDDisplay((char *)"Error: Unable to Program Channel ", 0, true);
        while(1);
        //Display error message on LCD and Console
    }

    /*******************************************************************/

    // Function MiApp_ConnectionMode defines the connection mode. The
    // possible connection modes are:
    //  ENABLE_ALL_CONN:    Enable all kinds of connection
    //  ENABLE_PREV_CONN:   Only allow connection already exists in
    //                      connection table
    //  ENABL_ACTIVE_SCAN_RSP:  Allow response to Active scan
    //  DISABLE_ALL_CONN:   Disable all connections.

    /*******************************************************************/
    MiApp_ConnectionMode(ENABLE_ALL_CONN);

    /*******************************************************************/

    // Display current opertion on LCD of demo board, if applicable

    /*******************************************************************/
    LCDDisplay((char *)"Connecting Peer  on Channel %d ", myChannel, true);
    Printf("\r\n\r\nConnecting to Peer...\r\n");

    /*******************************************************************/

    // Function MiApp_StartConnection will enable a node to start operating
    // in a variety of ways. Usually, this fucntion is called by the
    // PAN Coordinator who is the first in the PAN.
    //
    // The first parameter defines the mode to start the PAN in
    //
    // The second parameter defines the scan duration (if energy/carrier
    // sense scan is enabled). 0 if START_CONN_DIRECT used.
    //
    // The third parameter is a bit map of of the channels to perform the
    // noise scan on. 0 if START_CONN_DIRECT used.

    /*******************************************************************/

    MiApp_StartConnection(START_CONN_DIRECT, 0, 0) ;
    /*******************************************************************/

    // Function MiApp_EstablishConnection try to establish a new
    // connection with peer device.
    // The first parameter is the index to the active scan result,
    //      which is acquired by discovery process (active scan). If
    //      the value of the index is 0xFF, try to establish a
    //      connection with any peer.
    // The second parameter is the mode to establish connection,
    //      either direct or indirect. Direct mode means connection
    //      within the radio range; indirect mode means connection
    //      may or may not in the radio range.

    /*******************************************************************/
    #ifdef ENABLE_HAND_SHAKE
    i = 0xFF;
    while(i == 0xFF)
    {
        i = MiApp_EstablishConnection(0xFF, CONN_MODE_DIRECT);
    }
    #endif

    /*******************************************************************/

    // Display current opertion on LCD of demo board, if applicable

    /*******************************************************************/

    LCDDisplay((char *)"Joined  Network Successfully..", 0, true);

    /*******************************************************************/

    // Function DumpConnection is used to print out the content of the
    //  Connection Entry on the hyperterminal. It may be useful in
    //  the debugging phase.
    // The only parameter of this function is the index of the
    //  Connection Entry. The value of 0xFF means to print out all
    //  valid Connection Entry; otherwise, the Connection Entry
    //  of the input index will be printed out.

    /*******************************************************************/
    #ifdef ENABLE_DUMP
    DumpConnection(0xFF);
    #endif

    #ifndef ENABLE_POWERSAVE
    // Turn on LED 1 to indicate P2P connection established
    LED_1 = 1;
    #endif
    DELAY_ms(100);

    LCDBacklightON();
    LCD_Erase();
    sprintf((char *)LCDText, (char *)"MyAddr:   %02x%02x%02x", myLongAddress[2],
             myLongAddress[1], myLongAddress[0]);
    sprintf((char *) &(LCDText[16]), (char *)"PeerAddr: %02x%02x%02x", ConnectionTable[i].Address[2],
             ConnectionTable[i].Address[1], ConnectionTable[i].Address[0]);
    LCD_Update();

    /*******************************************************************/

    // Following block display demo instructions on LCD based on the
    // demo board used.

    /*******************************************************************/
    Printf("-------------------------------------------------------\r\n");
    Printf("Chat Window: \r\n");
    Printf("-------------------------------------------------------\r\n");
    Printf("$$");

    while(1)
    {
        if(MiApp_MessageAvailable())
        {  
            ProcessRxMessage();      
        }

        if(CONSOLE_IsGetReady())
        {
            FormatTxMessage();
            
        }

        if(messagePending)
        {
            tickCurrent = MiWi_TickGet();
            if
            (
                (MiWi_TickGetDiff(tickCurrent, tickPrevious) > (ONE_SECOND * 30)) ||
                (TxMessageSize >= MAX_MESSAGE_LEN) ||
                (transmitPending == true)
            )
            {
                
                TransmitMessage();


            }
        }

        // Display connection table if RB0 is pressed
        if(PUSH_BUTTON_1 == 0)
    	{
        	while(PUSH_BUTTON_1 == 0);
        	Printf("\r\n\r\nDumping Connection Table...\r\n");
        	DumpConnection(0xFF);
        	Printf("-------------------------------------------------------\r\n");
    		Printf("Chat Window: \r\n");
    		Printf("-------------------------------------------------------\r\n");
    		Printf("$$ ");
    	}


    }

    //Enable device to foward the received packet information to the console
}                           //end of main

void ProcessRxMessage()
{
    uint8_t index;
    if(rxMessage.flags.bits.broadcast)
    {
        CONSOLE_PutString((char *) "\n -------- From group:");
    }
    else
    {
        CONSOLE_PutString((char *) "\n -------- From [");
    }

    if(rxMessage.flags.bits.srcPrsnt)
    
    {
        if(rxMessage.flags.bits.altSrcAddr)
        {
            CONSOLE_PrintHex(rxMessage.SourceAddress[1]);
            CONSOLE_PrintHex(rxMessage.SourceAddress[0]);
        }
        else
        {
            for(index = 0; index < MY_ADDRESS_LENGTH; index++)
            {
                CONSOLE_PrintHex(rxMessage.SourceAddress[MY_ADDRESS_LENGTH - 1 - index]);
            }
        }

        CONSOLE_PutString((char *) "] : ");
    }

    for(index = 1; index < rxMessage.PayloadSize; index++)
    {
        CONSOLE_Put(rxMessage.Payload[index]);
    }

    CONSOLE_PutString((char *) "\r\n$$");

    // Toggle LED2 to indicate receiving a packet.
    LED_2 ^= 1;

    /*******************************************************************/

    // Function MiApp_DiscardMessage is used to release the current
    //  received packet.
    // After calling this function, the stack can start to process the
    //  next received frame

    /*******************************************************************/
    MiApp_DiscardMessage();
}           

void FormatTxMessage()
{
    uint8_t    inputChar;
    
    inputChar = CONSOLE_Get();
    CONSOLE_Put(inputChar);
    if(inputChar == 0x0D)
    {
        CONSOLE_PutString((char *) "\r\n$$");
        messagePending = true;
        transmitPending = true;
    }
    else if(inputChar == 0x08)
    {
        TxMessageSize--;
    }
    else
    {
        if(TxMessageSize < MAX_MESSAGE_LEN)
        {
            TxMessage[TxMessageSize] = inputChar;
            TxMessageSize++;
            tickPrevious = MiWi_TickGet();
            messagePending = true;
        }
    }
}
     
     
void TransmitMessage()
{
    uint8_t index;
    //Send message

    /******************************************************************/

    // First call function MiApp_FlushTx to reset the Transmit buffer.
    //  Then fill the buffer one byte by one byte by calling function
    //  MiApp_WriteData

    /*******************************************************************/
    MiApp_FlushTx();

    MiApp_WriteData(TxMessageSize);
    for(index = 0; index < TxMessageSize; index++)
    {
        MiApp_WriteData(TxMessage[index]);
    }

    //Unicast Message

    /*******************************************************************/

    // Function MiApp_UnicastConnection is one of the functions to
    // unicast a message.
    //    The first parameter is the index of connection table for
    //       the peer device. In this lab, the chat is always sent to the
    //       first P2P Connection Entry of the connection table. If that
    //		 node is down (eg. student is programming new firmare into it),
    //		 the user is prompted to reset the node to re-establish the
    //       connection table with a new peer.
    //
    //    The second parameter is the boolean to indicate if we need
    //       to secure the frame. If encryption is applied, the
    //       security level and security key are defined in the
    //       configuration file for the transceiver
    //
    // Another way to unicast a message is by calling function
    // MiApp_UnicastAddress. Instead of supplying the index of the
    // connection table of the peer device, this function requires the
    // input parameter of destination address directly.

    /*******************************************************************/
    if(MiApp_UnicastConnection(0, true) == false)
    {
        //Message TX Failed (peer node 00 was likely being re-programmed by student)
        //Should reset the node to establish new peer connection to send chat to

        CONSOLE_PutString((char *) "Transmit to Peer 00 Failed. Press MCLR to establish new connections");
    }


	//Reset Chat Application state variables
    messagePending = false;
    transmitPending = false;
    TxMessageSize = 0;


}


/*********************************************************************
* Function:         void ReadMacAddress()
*
* PreCondition:     none
*
* Input:		    none
*
* Output:		    Reads MAC Address from MAC Address EEPROM
*
* Side Effects:	    none
*
* Overview:		    Uses the MAC Address from the EEPROM for addressing
*
* Note:			    
**********************************************************************/
void ReadMacAddress(void)
{
    RF_EEnCS_TRIS = 0;
    {
        uint8_t    Address0, Address1, Address2;
        RF_EEnCS = 0;
        SPIPut(0x03);   //Read Sequence to EEPROM
        SPIPut(0xFA);   //MAC address Start byte
        Address0 = SPIGet();
        Address1 = SPIGet();
        Address2 = SPIGet();
        RF_EEnCS = 1;
        if((Address0 == 0x00) && (Address1 == 0x04) && (Address2 == 0xA3))  //Compare the value against Microchip's OUI
        {
            RF_EEnCS = 0;
            SPIPut(0x03);
            SPIPut(0xFD);
            switch(MY_ADDRESS_LENGTH)
            {
            	case 8: myLongAddress[7] = 0x00;
                case 7: myLongAddress[6] = 0x04;
                case 6: myLongAddress[5] = 0xA3;
                case 5: myLongAddress[4] = 0xFF;
                case 4: myLongAddress[3] = 0xFE;
                case 3: myLongAddress[2] = SPIGet();
                case 2: myLongAddress[1] = SPIGet();
                case 1: myLongAddress[0] = SPIGet();
                        break;
                default: break;
            }
            RF_EEnCS = 1;
        }
    }
}
 
