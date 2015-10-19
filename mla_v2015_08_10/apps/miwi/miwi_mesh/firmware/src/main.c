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
#include "system.h"
#include "system_config.h"
#include "miwi/miwi_api.h"
#include "self_test.h"
#include "range_demo.h"
#include "temp_demo.h"



// Demo Version
#define MAJOR_REV       1
#define MINOR_REV       3

/*************************************************************************/
// The variable myChannel defines the channel that the device
// is operate on. This variable will be only effective if energy scan
// (ENABLE_ED_SCAN) is not turned on. Once the energy scan is turned
// on, the operating channel will be one of the channels available with
// least amount of energy (or noise).
/*************************************************************************/

// Possible channel numbers are from 0 to 31
uint8_t myChannel = 26;

#define MiWi_CHANNEL        0x04000000                          //Channel 26 bitmap

#define EXIT_DEMO           1
#define RANGE_DEMO          2
#define TEMP_DEMO           3
#define IDENTIFY_MODE       4
#define EXIT_IDENTIFY_MODE  5

#define NODE_INFO_INTERVAL  5

uint8_t ConnectionEntry = 0;
			
bool NetFreezerEnable = false;

extern uint8_t myLongAddress[];

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
    uint8_t AdditionalNodeID[ADDITIONAL_NODE_ID_SIZE] = {0x00};
#endif

                                                                                               
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
* Overview:		    This is the main function that runs the simple 
*                   example demo. The purpose of this example is to
*                   demonstrate the simple application programming
*                   interface for the MiWi(TM) Development 
*                   Environment. By virtually total of less than 30 
*                   lines of code, we can develop a complete 
*                   application using MiApp interface. The 
*                   application will first try to establish a P2P 
*                   link with another device and then process the 
*                   received information as well as transmit its own 
*                   information.
*                   MiWi(TM) DE also support a set of rich 
*                   features. Example code FeatureExample will
*                   demonstrate how to implement the rich features 
*                   through MiApp programming interfaces.
*
* Note:			    
**********************************************************************/
void main(void)
{   
    uint8_t i, j;
    bool result = true;
    uint8_t switch_val;

    NetFreezerEnable = false;

    /*******************************************************************/
    // Initialize Hardware
    /*******************************************************************/
    SYSTEM_Initialize();
 
 	
    LED0 = LED1 = LED2 = 1;
 	
    Read_MAC_Address();
    
    /*******************************************************************/
    // If Network Freezer Is Not Enabled User Must Configure the Network
    /*******************************************************************/
    if( !NetFreezerEnable )
	{
    	/*******************************************************************/
    	// Display Start-up Splash Screen
    	/*******************************************************************/
        LCD_BacklightON();
        
        LCD_Erase();
        sprintf((char *)LCDText, (char*)"    Microchip   "  );
        sprintf((char *)&(LCDText[16]), (char*)" MiWi Demo Board");
        LCD_Update();

        /*******************************************************************/
        // Initialize the MiWi Protocol Stack. The only input parameter indicates
        // if previous network configuration should be restored.
        /*******************************************************************/
        MiApp_ProtocolInit(false);
      
        
        {
            MIWI_TICK tick1, tick2;
            tick1 = MiWi_TickGet();
            while(1)
            {
                tick2 = MiWi_TickGet();
                if(MiWi_TickGetDiff(tick2, tick1) > (ONE_SECOND * 4))
                    break;
               switch_val = BUTTON_Pressed();
               if(switch_val == SW2)
                    SelfTest(myChannel);
               
            }
        }
        LCD_BacklightOFF();

        /*******************************************************************/
        // Set-up PAN_ID
        /*******************************************************************/
CreateorJoin:
		
	    
 
        /*******************************************************************/
        //Ask Use to select channel
        /*******************************************************************/
        LCD_Erase();
        sprintf((char *)LCDText, (char*)"SW1:<Sel Ch:%02d>", myChannel);
        sprintf((char *)&(LCDText[16]), (char*)"SW2: Chnge Chnl");
        LCD_Update();
        DELAY_ms(1000);

        while(1)
		{
    		switch_val = BUTTON_Pressed();
                if(switch_val == SW1)
			{
                //User selected Default Channel
                break;
            }
            
            else if(switch_val == SW2)
			{
                uint8_t select_channel = 0;
                bool update_channel = true;

                #if defined (MRF24J40)
                    select_channel = 11;
                #else
                    select_channel = 27;
                #endif
                while(update_channel == true)
                {
                    //User Selected Change Channel
                    LCD_Erase();
    		    sprintf((char *)LCDText, (char*)"SW1:<Sel Ch:%02d>", select_channel);
                    sprintf((char *)&(LCDText[16]), (char*)"SW2: Chnge Chnl");
                    LCD_Update();
    
                    while(1)
                    {
                        switch_val = BUTTON_Pressed();
                        if(switch_val == SW1)
                            {
                                myChannel = select_channel;
                                update_channel = false;
                                break;
                            }
                        else if(switch_val == SW2)
                            {
                                select_channel = select_channel+1;
                                #if defined(MRF24J40)
                                    if(select_channel == 27) select_channel = 11;
                                #elif defined(MRF89XA)
                                    if(select_channel == 32) select_channel = 0;                                
                                #else
                                     #error "MiWi Demo is not supported for this transceiver"
                                #endif
                                break;
                            }
                    
                    } //End of while(1)
                } //End of while(result==TRUE)
                break;
            }   //End of Change Channel (switch_val == SW2)

        }           

        

        /*******************************************************************/
        // Set Device Communication Channel
        /*******************************************************************/

	    if( MiApp_SetChannel(myChannel) == false )
	    {
	        LCD_Display((char *)"ERROR: Unable toSet Channel..", 0, true);
	        return;
	    }
	    
	    /*******************************************************************/
	    //  Set the connection mode. The possible connection modes are:
	    //      ENABLE_ALL_CONN:    Enable all kinds of connection
	    //      ENABLE_PREV_CONN:   Only allow connection already exists in 
	    //                          connection table
	    //      ENABL_ACTIVE_SCAN_RSP:  Allow response to Active scan
	    //      DISABLE_ALL_CONN:   Disable all connections. 
	    /*******************************************************************/
	    MiApp_ConnectionMode(ENABLE_ALL_CONN);
	    
	    /*******************************************************************/
	    // Ask User to Create or Join a Network
	    /*******************************************************************/
	    LCD_Display((char *)"SW1: Create NtwkSW2: Join Ntwk  ", 0, false);
	    DELAY_ms(1000);
	    	
            while(result == true)
            {
            switch_val = BUTTON_Pressed();
    		
            /*******************************************************************/
            // Create a New Network
            /*******************************************************************/
            if(switch_val == SW1)
            {
                    LCD_Display((char *)"Creating Network", 0, true);

                    MiApp_ProtocolInit(false);
                    MiApp_StartConnection(START_CONN_DIRECT, 0, 0);

                    LCD_Display((char *)"Created Network Successfully", 0, true);

                    LCD_Erase();
                    sprintf((char *)&(LCDText), (char*)"PANID:%02x%02x Ch:%02d",myPANID.v[1],myPANID.v[0],myChannel);
                    sprintf((char *)&(LCDText[16]), (char*)"Address: %02x%02x", myShortAddress.v[1], myShortAddress.v[0]);
                    LCD_Update();

                    /*******************************************************************/
                    // Wait for a Node to Join Network then proceed to Demo's
                    /*******************************************************************/
                    while(!ConnectionTable[0].status.bits.isValid)
                    {
                            if(MiApp_MessageAvailable())
                                MiApp_DiscardMessage();
                    }
                    result = false;

             }
			
            /*******************************************************************/
            // Join a Network
            /*******************************************************************/
            if(switch_val == SW2)
            {
                    volatile uint8_t scanresult;

                    LCD_Display((char *)"  Scanning for    Networks....", 0, true);
                    LCD_Display((char *)"Please Select   Network to Join ", 0, true);

                    MiApp_ProtocolInit(false);

                /*******************************************************************/
                // Perform an active scan
                /*******************************************************************/

                if(myChannel < 8)
                    scanresult = MiApp_SearchConnection(10, (0x00000001 << myChannel));
                else if(myChannel < 16)
                    scanresult = MiApp_SearchConnection(10, (0x00000100 << (myChannel-8)));
                else if(myChannel < 24)
                    scanresult = MiApp_SearchConnection(10, (0x00010000 << (myChannel-16)));
                else
                    scanresult = MiApp_SearchConnection(10, (0x01000000 << (myChannel-24)));



                /*******************************************************************/
                // Display Scan Results
                /*******************************************************************/
                if(scanresult > 0)
                {
                  uint8_t k;
                    
	                for(j = 0; j < scanresult; j++)
	                {
                        uint8_t skip_print = false;
                        if(j > 0)
                        {
                            
                            skip_print = false;
                            for(k = 0; k < j; k++)
                            {
                                if((ActiveScanResults[j].PANID.v[1] == ActiveScanResults[k].PANID.v[1]) &
                                    (ActiveScanResults[j].PANID.v[0] == ActiveScanResults[k].PANID.v[0]))
                                    {
                                        skip_print = true;
                                        break;
                                    }
                                
                            }
                            if(skip_print == true)
                            { 
                                if(j == (scanresult-1)) j = -1;
                                continue;
                            }
                        }
                        
                    // Display the index on LCD
                    LCD_Erase();

                    // Display if Network is Cordinator or PAN Cordinator

                    sprintf((char *)LCDText, (char*)"SW1:<PANID:%02x%02x>",ActiveScanResults[j].PANID.v[1], ActiveScanResults[j].PANID.v[0]);


                    sprintf((char *)&(LCDText[16]), (char*)"SW2: Additional");

                    LCD_Update();

                    while(1)
                    {
                    switch_val = BUTTON_Pressed();

                    /*******************************************************************/
                    // Connect to Display Network
                    /*******************************************************************/
                        if(switch_val == SW1)
                        {
                                uint8_t Status;
                                uint8_t CoordCount = 0;
                                MiApp_FlushTx();
    	                        
                                for(k = 0 ; k < scanresult ; k++)
                                {
                                    if((ActiveScanResults[j].PANID.v[1] == ActiveScanResults[k].PANID.v[1]) &
                                        (ActiveScanResults[j].PANID.v[1] == ActiveScanResults[k].PANID.v[1]))
                                    {
                                        CoordCount++;
                                    }
                                }
                                if(CoordCount > 1)
                                {
                                    uint8_t nodeIndex = 0;
                                    uint8_t count = 0;
                                    MiApp_FlushTx();
                                    MiApp_WriteData(IDENTIFY_MODE);
                                    MiApp_WriteData(ActiveScanResults[j].PANID.v[1]);
                                    MiApp_WriteData(ActiveScanResults[j].PANID.v[0]);
    	                            MiApp_BroadcastPacket(false);
                                    for(k = 0 ; k < scanresult ; k++)
                                    {
                                        if((ActiveScanResults[j].PANID.v[1] == ActiveScanResults[k].PANID.v[1]) &
                                        (ActiveScanResults[j].PANID.v[0] == ActiveScanResults[k].PANID.v[0]))
                                        {
                                                count++;
                                            	LCD_Erase();
						
                                                // Display Network information

                                                sprintf((char *)LCDText, (char*)"SW1:<Addr:%02x%02x>",ActiveScanResults[k].Address[1], ActiveScanResults[k].Address[0]);


                                                sprintf((char *)&(LCDText[16]), (char*)"SW2: Additional");

                                                LCD_Update();
                                                nodeIndex = k;

                                                while(1)
                                                {
                                                    switch_val = BUTTON_Pressed();
                                                    if(switch_val == SW1)
                                                    {
                                                       //Establish connection with the node
                                                       j = nodeIndex;
                                                       k = scanresult-1;
                                                       break;
                                                    }
                                                    else if(switch_val == SW2)
                                                    {
                                                        //Display Additional node information
                                                        if((k == (scanresult - 1)) || (count == CoordCount))
                                                          {
                                                             k = -1;
                                                             count = 0;
                                                          }  
                                                       break;
                                                    }
                                                } //End of while(1) loop
                                        }
                                         
                                          
                                    } //End of for(k = 0; ....)
                                } //End of if (CoordCount > ...)							
                        /*******************************************************************/
                        // Establish Connection and Display results of connection
                        /*******************************************************************/
                                Status = MiApp_EstablishConnection(j, CONN_MODE_DIRECT);
                                if(Status == 0xFF)
                                {
                                LCD_Display((char *)"Join Failed!!!", 0, true);
                                        goto CreateorJoin;
                                }
                                else
                                {
                                        LCD_Display((char *)"Joined  Network Successfully..", 0, true);
                                        result = false;
                                }
                                MiApp_FlushTx();
                                MiApp_WriteData(EXIT_IDENTIFY_MODE);
                                MiApp_WriteData(myPANID.v[1]);
                                MiApp_WriteData(myPANID.v[0]);
    	                        MiApp_BroadcastPacket(false);
                                break;

                        }
							
                        /*******************************************************************/
                        // Display Next Network in Scan List
                        /*******************************************************************/
                                else if(switch_val == SW2)
                                {
                                        if((scanresult-j-1) == 0)
                                            j = -1;

                                        break;
                                }
                        }
                    /*******************************************************************/
                        // If Connected to a Network Successfully bail out
                        /*******************************************************************/
                        if(result == false)
                            break;
                    }
							
					result = false;
            	}
				else
				{
					LCD_Display((char *)"No Network FoundSW2: Re-Scan", 0, false);
				}
			}
		}
	}
	
	while(1)
	{
    	uint8_t pktCMD = 0;
    	uint8_t switch_val, menu_choice = 0;
    	result = false;
    	
    	
    	LED0 = LED1 = LED2 = 0;
    	
        /*******************************************************************/
        // Ask User which Demo to Run
        /*******************************************************************/
        LCD_Display((char *)"SW1: Range Demo SW2: Other Apps  ", 0, false);
	   	
        /*******************************************************************/
        // Wait for User to Select Demo to Run 
        /*******************************************************************/
        while(result == false)
        {  	
            // Read Switches
            switch_val = BUTTON_Pressed();
            
        	// Check if MiMAC has any pkt's to processes
        	if(MiApp_MessageAvailable())
        	{
            	pktCMD = rxMessage.Payload[0];
                if(pktCMD == IDENTIFY_MODE)
                    {
                        if((rxMessage.Payload[1] != myPANID.v[1]) || (rxMessage.Payload[2] != myPANID.v[0]))
                            pktCMD = 0;
                            
                    }
                if(pktCMD == RANGE_DEMO)
                    {
                        for(i = 0 ; i< CONNECTION_SIZE ; i++)
                        {
                            if(ConnectionTable[i].status.bits.isValid)
                            {
                                if((ConnectionTable[i].AltAddress.v[1] == rxMessage.SourceAddress[1]) && (ConnectionTable[i].AltAddress.v[0] == rxMessage.SourceAddress[0]))
                                {
                                    ConnectionEntry = i;
                                    
                                    break;                           
                                }
                            }
                        }
                    }
            	MiApp_DiscardMessage();
            }
            if(pktCMD == RANGE_DEMO)
            {
                MiApp_FlushTx();
    	        MiApp_WriteData(RANGE_DEMO);
    	        MiApp_UnicastConnection(ConnectionEntry, false);
    	        // Run Range Test Demo
                RangeDemo();
                result = true;
            }
        	
        	if((switch_val == SW1) && (menu_choice == 0))
    		{	
                uint8_t NumberOfConnections = 0;
                
                //Reset ConnectionEntry
                ConnectionEntry = 0;
                
                for(i = 0; i < CONNECTION_SIZE; i++)
                {
                    if((ConnectionTable[i].status.bits.isValid) && (ConnectionTable[i].status.bits.isFamily))
                        NumberOfConnections++;
                }
                if(NumberOfConnections > 1)
                {
                    bool result = false;
                    uint8_t ConnectionsCount = 0;
                    //Select Peer for Range Demo

                    LCD_Display((char *)"Select Peer Node for Range Test", 0, false);

                    for(i = 0; i < CONNECTION_SIZE; i++)
    			    {
    				    if(ConnectionTable[i].status.bits.isValid)
                        {
                            // Display Peer information
                            LCD_Erase();
                            sprintf((char *)LCDText, (char*)"SW1:<Addr:%02x%02x>",ConnectionTable[i].AltAddress.v[1], ConnectionTable[i].AltAddress.v[0]);				
                            sprintf((char *)&(LCDText[16]), (char*)"SW2: Additional");
                            LCD_Update();
                            ConnectionsCount++;
                            while(1)
                            {
                                switch_val = BUTTON_Pressed();
                                if(switch_val == SW1)
                                    {
                                        ConnectionEntry = i;
                                        result = true;
                                        break;
                                    }
                                else if(switch_val == SW2)
                                    {
                                        if(ConnectionsCount == NumberOfConnections) 
                                        {
                                            i = -1;
                                            ConnectionsCount = 0;
                                        }
                                        break;
                                    }
                            }
                        }
                        if(result == true) break;
                    }  
                }       
                MiApp_FlushTx();
    	        MiApp_WriteData(RANGE_DEMO);
    	        MiApp_UnicastConnection(ConnectionEntry, false);
    	        
                // Run Range Test Demo
                RangeDemo();
                result = true;
    		}
            if((switch_val == SW2))
            {
                menu_choice++;
                if(menu_choice == 3) menu_choice = 0;
                if(menu_choice == 0)
                {
                    LCD_Display((char *)"SW1: Range Demo SW2: Other Apps  ", 0, false);
                }
                else if(menu_choice == 1)
                {
                    LCD_Display((char *)"SW1: Temp Demo  SW2: Other Apps  ", 0, false);
                }    
                else if(menu_choice == 2)
                {
                    LCD_Display((char *)"SW1: Node Info  SW2: Other Apps  ", 0, false);
                }
                
            }
            if((pktCMD == TEMP_DEMO) || ((switch_val == SW1) && (menu_choice == 1)))
            {
                MiApp_FlushTx();
                MiApp_WriteData(TEMP_DEMO);
                MiApp_BroadcastPacket(false);

                    // Run Temp Sensor Demo
                TempDemo();
                result = true;
            }
            if((pktCMD == IDENTIFY_MODE) || ((switch_val == SW1) && (menu_choice == 2)))
            {
                
                LCD_Erase();
                sprintf((char *)&(LCDText), (char*)"PANID:%02x%02x Ch:%02d",myPANID.v[1],myPANID.v[0],myChannel);
                #if defined(IEEE_802_15_4)
                    sprintf((char *)&(LCDText[16]), (char*)"Address: %02x%02x", myShortAddress.v[1], myShortAddress.v[0]);
                #else
                    sprintf((char *)&(LCDText[16]), (char*)"Addr:%02x%02x/%02x%02x", myShortAddress.v[1], myShortAddress.v[0], myLongAddress[1],myLongAddress[0]);
                #endif
                LCD_Update();
                pktCMD = 0;
            }
            
            if(pktCMD == EXIT_IDENTIFY_MODE)
            {
               
                result = true;
               
            }
    	}
	}
}


