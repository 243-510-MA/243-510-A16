//NETWORK

#include "network.h"
#include "system.h"
#include "system_config.h"
#include "miwi/miwi_api.h"

//*************************************************************************
// The variable myChannel defines the channel that the device
// is operate on. This variable will be only effective if energy scan
// (ENABLE_ED_SCAN) is not turned on. Once the energy scan is turned
// on, the operating channel will be one of the channels available with
// least amount of energy (or noise).
//*************************************************************************

uint8_t myChannel = 20; 

#define MiWi_CHANNEL        0x04000000        //Channel 26 bitmap

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

    
void Network(int DEVICEMODE)
{       
    uint8_t i, j;
    bool result = true;
    uint8_t switch_val;

    //*******************************************************************
    // Initialize Hardware
    //*******************************************************************
    
    SYSTEM_Initialize();
 	
    LED0 = LED1 = LED2 = 1;
 	
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
    
    
    
    
    else
    {
        /*******************************************************************/
        // JOIN A NETWORK
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
}


