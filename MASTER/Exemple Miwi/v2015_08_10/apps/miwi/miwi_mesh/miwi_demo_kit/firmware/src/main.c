//MASTER

#define PAN_PROJECTOR 0
#define DOOR_LOCK 1
#define PROJECTOR_SCREEN 2
#define STUDENT 3
#define TEACHER 4

//*************************************************************************

#define DEVICEMODE PAN_COORDINATOR // Choisir ici 


#include "system.h"
#include "system_config.h"
#include "miwi/miwi_api.h"
#include "self_test.h"
#include "door_unlock.h"
#include "temp_demo.h"


// Demo Version
#define MAJOR_REV       1
#define MINOR_REV       3

//*************************************************************************
// The variable myChannel defines the channel that the device
// is operate on. This variable will be only effective if energy scan
// (ENABLE_ED_SCAN) is not turned on. Once the energy scan is turned
// on, the operating channel will be one of the channels available with
// least amount of energy (or noise).
//*************************************************************************

uint8_t myChannel = 20; 

#define MiWi_CHANNEL        0x04000000                          //Channel 26 bitmap

#define EXIT_DEMO           1
#define DOOR_UNLOCK         2
#define TEMP_DEMO           3
#define IDENTIFY_MODE       4
#define EXIT_IDENTIFY_MODE  5

#define NODE_INFO_INTERVAL  5

uint8_t ConnectionEntry = 0;

extern uint8_t myLongAddress[];

//*************************************************************************
// AdditionalNodeID variable array defines the additional 
// information to identify a device on a PAN. This array
// will be transmitted when initiate the connection between 
// the two devices. This  variable array will be stored in 
// the Connection Entry structure of the partner device. The 
// size of this array is ADDITIONAL_NODE_ID_SIZE, defined in 
// ConfigApp.h.
// In this demo, this variable array is set to be empty.
//*************************************************************************

#if ADDITIONAL_NODE_ID_SIZE > 0
    uint8_t AdditionalNodeID[ADDITIONAL_NODE_ID_SIZE] = {0x00};
#endif

    
void main(void)
{   
    uint8_t i, j;
    bool result = true;
    uint8_t switch_val;

    //*******************************************************************
    // Initialize Hardware
    //*******************************************************************
    
    SYSTEM_Initialize();
 	
    LED0 = LED1 = LED2 = 1;
    
    if(DEVICEMODE == 1)
    {
        LCD_Display((char *)"Welcome, Mathieu", 0, true); //Do not remove, useful to hide delay in program
    }
 	
    Read_MAC_Address();

    MiApp_ProtocolInit(false);

    //*******************************************************************
    // Set-up PAN_ID
    //*******************************************************************
    
CreateorJoin:

    //*******************************************************************
    // Set Device Communication Channel
    //*******************************************************************

    if( MiApp_SetChannel(myChannel) == false ) //do not remove, needed to connect
        return;

    //*******************************************************************
    //  Set the connection mode. The possible connection modes are:
    //      ENABLE_ALL_CONN:    Enable all kinds of connection
    //      ENABLE_PREV_CONN:   Only allow connection already exists in 
    //                          connection table
    //      ENABL_ACTIVE_SCAN_RSP:  Allow response to Active scan
    //      DISABLE_ALL_CONN:   Disable all connections. 
    //*******************************************************************

    MiApp_ConnectionMode(ENABLE_ALL_CONN);

    if(DEVICEMODE == 0)
    {
        while(result == true)
        {

            //*******************************************************************
            // Create a New Network
            //*******************************************************************

            MiApp_ProtocolInit(false);
            MiApp_StartConnection(START_CONN_DIRECT, 0, 0);

            LCD_Erase();
            sprintf((char *)&(LCDText), (char*)"PANID:%02x%02x Ch:%02d",myPANID.v[1],myPANID.v[0],myChannel);
            sprintf((char *)&(LCDText[16]), (char*)"Address: %02x%02x", myShortAddress.v[1], myShortAddress.v[0]);
            LCD_Update();

            //*******************************************************************
            // Wait for a Node to Join Network then proceed to Demo's
            //*******************************************************************
            while(!ConnectionTable[0].status.bits.isValid)
            {
                    if(MiApp_MessageAvailable())
                        MiApp_DiscardMessage();
            }
            result = false;

        }   
    }
    
    
    if(DEVICEMODE != 0)
    {
        /*******************************************************************/
        // Join a Network
        /*******************************************************************/

        while(result == true)
        {
            volatile uint8_t scanresult;

            MiApp_ProtocolInit(false);

            /*******************************************************************/
            // Perform an active scan
            /*******************************************************************/

            scanresult = MiApp_SearchConnection(10, (0x00010000 << (myChannel-16)));

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

                    while(1)
                    {

                    /*******************************************************************/
                    // Connect to Network
                    /*******************************************************************/

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
                                    nodeIndex = k;
                                }
                            } //End of for(k = 0; ....)
                        } //End of if (CoordCount > ...)	
						
                    /*******************************************************************/
                    // Establish Connection and Display results of connection
                    /*******************************************************************/
                        
                        Status = MiApp_EstablishConnection(j, CONN_MODE_DIRECT);
                        if(Status == 0xFF)
                        {
                            //LCD_Display((char *)"Join Failed!!!", 0, true);
                            goto CreateorJoin;
                        }
                        else
                        {
                            //LCD_Display((char *)"Joined  Network Successfully..", 0, true);
                            result = false;
                        }
                        MiApp_FlushTx();
                        MiApp_WriteData(EXIT_IDENTIFY_MODE);
                        MiApp_WriteData(myPANID.v[1]);
                        MiApp_WriteData(myPANID.v[0]);
                        MiApp_BroadcastPacket(false);
                        break;

                    }
                    
                    //*******************************************************************
                    // If Connected to a Network Successfully bail out
                    //*******************************************************************
                    
                    if(result == false)
                        break;
                }

                result = false;
            }
            else
            {
                LCD_Display((char *)"Waiting for PAN Rescanning......", 0, false);
            }
        }//end of (while (result == true)
    }
    
    
    
    
    
    
    
    
	while(1)
	{
    	uint8_t pktCMD = 0;
    	uint8_t switch_val, menu_choice = 0;
    	result = false;
    	
    	LED0 = LED1 = LED2 = 0;
    	
        //*******************************************************************
        // Ask User which Demo to Run
        //*******************************************************************
        
        LCD_Display((char *)"SW1: Door UnlockSW2: Other Apps  ", 0, false);
	   	
        //*******************************************************************
        // Wait for User to Select Demo to Run 
        //*******************************************************************
        
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
                if(pktCMD == DOOR_UNLOCK)
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
            if(pktCMD == DOOR_UNLOCK)
            {
                MiApp_FlushTx();
    	        MiApp_WriteData(DOOR_UNLOCK);
    	        MiApp_UnicastConnection(ConnectionEntry, false);
    	        // Run Unlock Door Demo
                DoorUnlock();
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
                    //Select Peer for Door Unlock

                    LCD_Display((char *)"Select Peer Node for Unlock Door", 0, false);

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
    	        MiApp_WriteData(DOOR_UNLOCK);
    	        MiApp_UnicastConnection(ConnectionEntry, false);
    	        
                // Run Unlock Door Demo
                DoorUnlock();
                result = true;
    		}
            if((switch_val == SW2))
            {
                menu_choice++;
                if(menu_choice == 3) menu_choice = 0;
                if(menu_choice == 0)
                {
                    LCD_Display((char *)"SW1: Door Unlock SW2: Other Apps  ", 0, false);
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


