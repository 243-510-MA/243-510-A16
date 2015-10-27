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

#include "range_demo.h"
#include "system.h"
#include "system_config.h"
#include "miwi/miwi_api.h"

#define TX_PKT_INTERVAL             4
#define DISPLAY_RSSI_INTERVAL       4
#define EXIT_DEMO_TIMEOUT           6

#define EXIT_PKT    1
#define RANGE_PKT   2
#define ACK_PKT     4

/*
#if defined(MRF24J40)

const uint8_t RSSIlookupTable [] =   {90,89,88,88,88,87,87,87,87,86,86,86,86,85,85,85,85,85,84,84,84,
                                84,84,83,83,83,83,82,82,82,82,82,81,81,81,81,81,80,80,80,80,80,
                                80,79,79,79,79,79,78,78,78,78,78,77,77,77,77,77,76,76,76,76,76,
                                75,75,75,75,75,74,74,74,74,74,73,73,73,73,73,72,72,72,72,72,71,
                                71,71,71,71,71,70,70,70,70,70,70,69,69,69,69,69,68,68,68,68,68,
                                68,68,67,67,67,67,66,66,66,66,66,66,65,65,65,65,64,64,64,64,63,
                                63,63,63,62,62,62,62,61,61,61,61,61,60,60,60,60,60,59,59,59,59,
                                59,58,58,58,58,58,57,57,57,57,57,57,56,56,56,56,56,56,55,55,55,
                                55,55,54,54,54,54,54,54,53,53,53,53,53,53,53,52,52,52,52,52,51,
                                51,51,51,51,50,50,50,50,50,49,49,49,49,49,48,48,48,48,47,47,47,
                                47,47,46,46,46,46,45,45,45,45,45,44,44,44,44,43,43,43,42,42,42,
                                42,42,41,41,41,41,41,41,40,40,40,40,40,40,39,39,39,39,39,38,38,
                                38,37,36,35};
#elif defined(MRF89XA)
const uint8_t RSSIlookupTable [] =   {100,89,88,88,88,87,87,87,87,86,86,86,86,85,85,85,85,85,84,84,84,
                                84,84,83,83,83,83,82,82,82,82,82,81,81,81,81,81,80,80,80,80,80,
                                80,79,79,79,79,79,78,78,78,78,78,77,77,77,77,77,76,76,76,76,76,
                                75,75,75,75,75,74,74,74,74,74,73,73,73,73,73,72,72,72,72,72,71,
                                71,71,71,71,71,70,70,70,70,70,70,69,69,69,69,69,68,68,68,68,68,
                                68,68,67,67,67,67,66,66,66,66,66,66,65,65,65,65,64,64,64,64,63,
                                63,63,63,62,62,62,62,61,61,61,61,61,60,60,60,60,60,59,59,59,59,
                                59,58,58,58,58,58,57,57,57,57,57,57,56,56,56,56,56,56,55,55,55,
                                55,55,54,54,54,54,54,54,53,53,53,53,53,53,53,52,52,52,52,52,51,
                                51,51,51,51,50,50,50,50,50,49,49,49,49,49,48,48,48,48,47,47,47,
                                47,47,46,46,46,46,45,45,45,45,45,44,44,44,44,43,43,43,42,42,42,
                                42,42,41,41,41,41,41,41,40,40,40,40,40,40,39,39,39,39,39,38,38,
                                38,37,36,40};
#endif

*/


extern uint8_t ConnectionEntry;
/*********************************************************************
* Function:         void RangeDemo(void)
*
* PreCondition:     none
*
* Input:		    none
*
* Output:		    none
*
* Side Effects:	    none
*
* Overview:		    Following routine 
*                    
*
* Note:			    
**********************************************************************/                                
void RangeDemo(void)
{
    bool Run_Demo = true;
    bool Tx_Packet = true;
    uint8_t rssi = 0;
    uint8_t Pkt_Loss_Cnt = 0;
    MIWI_TICK tick1, tick2;
    uint8_t switch_val;
    uint8_t pktCmd = 0;
    
    /*******************************************************************/
    // Dispaly Range Demo Splach Screen
    /*******************************************************************/	
    LCD_BacklightON();
    LCD_Display((char *)"   Microchip       Door Unlock  ", 0, true);
    LCD_BacklightOFF();
        
    /*******************************************************************/
    // Read Start tickcount
    /*******************************************************************/	
    tick1 = MiWi_TickGet();
    
    while(Run_Demo)
    {	
        /*******************************************************************/
        // Read current tickcount
        /*******************************************************************/
    	tick2 = MiWi_TickGet();
        
    	// Send a Message
        //((MiWi_TickGetDiff(tick2,tick1) > (ONE_SECOND * TX_PKT_INTERVAL)))
        if((MiWi_TickGetDiff(tick2,tick1) > (ONE_MILLI_SECOND * 1000)))
    	{
        	//LCD_Erase();
        	
        	if(Tx_Packet)
        	{	
                    //LCD_Display((char *)"Checking Signal Strength...", 0, true);
            		
        	    MiApp_FlushTx();
        	    
        	    MiApp_WriteData(RANGE_PKT);
        	    MiApp_WriteData(0x4D);
        	    MiApp_WriteData(0x69);
        	    MiApp_WriteData(0x57);
        	    MiApp_WriteData(0x69);
        	    MiApp_WriteData(0x20);
        	    MiApp_WriteData(0x52);
        	    MiApp_WriteData(0x6F);
        	    MiApp_WriteData(0x63);
        	    MiApp_WriteData(0x6B);
        	    MiApp_WriteData(0x73);
        	    MiApp_WriteData(0x21);
        	    	    
                if( MiApp_UnicastConnection(ConnectionEntry, false) == false )
                    Pkt_Loss_Cnt++;
        	    else
        	        Pkt_Loss_Cnt = 0;
        	        
        	    Tx_Packet = false;
    		}
    		else
    		{	      	
            	if(Pkt_Loss_Cnt < 1)
            	{   
                    #if defined(MRF24J40)         		
            		if(rssi > 200)
                    #else
                    if(rssi > 100)
                    #endif    
            		{
                        sprintf((char *)&LCDText, (char*)" Unlock Door ?  SW1: YES  SW2:NO");
                        LED0 = 1;
                        LED1 = 0;
                        LED2 = 0;
                        switch_val = BUTTON_Pressed();
                        if(switch_val == SW1)
                        {
                            LED0 = 1;
                            LED1 = 1;
                            LED2 = 1;
                        }
                        
                        else if(switch_val == SW2)
                        {
                            /*******************************************************************/
                            // Send Exit Demo Request Packet and exit Range Demo
                            /*******************************************************************/
                            MiApp_FlushTx();    
                            MiApp_WriteData(EXIT_PKT);
                            MiApp_UnicastConnection(ConnectionEntry, false);

                            LCD_BacklightON();
                            LCD_Display((char *)"   Exiting....     Range Demo  ", 0, true);
                            LCD_BacklightOFF();

                            tick1 = MiWi_TickGet();
                            // Wait for ACK Packet
                            while(Run_Demo)
                            {
                                if(MiApp_MessageAvailable())
                                {
                                    if(rxMessage.Payload[0] == ACK_PKT)          
                                        Run_Demo = false;

                                    MiApp_DiscardMessage();
                                }
                                if ((MiWi_TickGetDiff(tick2,tick1) > (ONE_SECOND * EXIT_DEMO_TIMEOUT)))
                                    Run_Demo = false;

                                tick2 = MiWi_TickGet();
                            }    
                        }    
                    }
                    #if defined(MRF24J40)
                  	else if(rssi < 201)
                    #else
                    else if(rssi < 61)
                    #endif
            		{
                        sprintf((char *)&LCDText, (char*)" Get Closer to      the Door    ");
                        LED0 = 0;
                        LED1 = 1;
                        LED2 = 0;
                        switch_val = BUTTON_Pressed();
                        if((switch_val == SW1) || (switch_val == SW2))
                        {
                            /*******************************************************************/
                            // Send Exit Demo Request Packet and exit Range Demo
                            /*******************************************************************/
                            MiApp_FlushTx();    
                            MiApp_WriteData(EXIT_PKT);
                            MiApp_UnicastConnection(ConnectionEntry, false);

                            LCD_BacklightON();
                            LCD_Display((char *)"   Exiting....     Range Demo  ", 0, true);
                            LCD_BacklightOFF();

                            tick1 = MiWi_TickGet();
                            // Wait for ACK Packet
                            while(Run_Demo)
                            {
                                if(MiApp_MessageAvailable())
                                {
                                    if(rxMessage.Payload[0] == ACK_PKT)          
                                        Run_Demo = false;

                                    MiApp_DiscardMessage();
                                }
                                if ((MiWi_TickGetDiff(tick2,tick1) > (ONE_SECOND * EXIT_DEMO_TIMEOUT)))
                                    Run_Demo = false;

                                tick2 = MiWi_TickGet();
                            }    
                        }    
                    }
                    
                    // Convert to dB
                    //rssi = RSSIlookupTable[rssi];
                    //sprintf((char *)&(LCDText[16]), (char*)"Rcv RSSI: %03d", rssi);
                }  
                
                else
                {
                        LCD_Display((char *)"No Device Found or Out of Range ", 0, true);
                        LED0 = 0;
                        LED1 = 0;
                        LED2 = 1;
                }

                LCD_Update();
                Tx_Packet = true;
                        		
            } 	     
    	    /*******************************************************************/
            // Read New Start tickcount
            /*******************************************************************/
      		tick1 = MiWi_TickGet(); 
      		  
        }
        
    	// Check if Message Available
    	if(MiApp_MessageAvailable())
    	{
            pktCmd = rxMessage.Payload[0];
            if(pktCmd == EXIT_PKT)
            {
            MiApp_DiscardMessage();
            MiApp_FlushTx();
            MiApp_WriteData(ACK_PKT);
            MiApp_UnicastConnection(ConnectionEntry, false);
            Run_Demo = false;
            LCD_BacklightON();
            LCD_Display((char *)"   Exiting....     Range Demo  ", 0, true);
            LCD_BacklightOFF();


            }
            else if(rxMessage.Payload[0] == RANGE_PKT)
            {
            	// Get RSSI value from Recieved Packet
            	rssi = rxMessage.PacketRSSI;
        	    #if defined(MRF89XA)
                    rssi = rssi<<1;
                #endif
        	    // Disguard the Packet so can recieve next
        	    MiApp_DiscardMessage();
        	}
        	else
            	MiApp_DiscardMessage(); 
    	}
    	
        /*
    	// Check if  Switch Pressed
    	switch_val = BUTTON_Pressed();
    	
        if((switch_val == SW1) || (switch_val == SW2))
        {
            /*******************************************************************
            // Send Exit Demo Request Packet and exit Range Demo
            /*******************************************************************
            MiApp_FlushTx();    
            MiApp_WriteData(EXIT_PKT);
            MiApp_UnicastConnection(ConnectionEntry, false);
   
            LCD_BacklightON();
            LCD_Display((char *)"   Exiting....     Range Demo  ", 0, true);
            LCD_BacklightOFF();
            
            tick1 = MiWi_TickGet();
            // Wait for ACK Packet
            while(Run_Demo)
            {
                if(MiApp_MessageAvailable())
                {
                    if(rxMessage.Payload[0] == ACK_PKT)          
                        Run_Demo = false;
                        
                    MiApp_DiscardMessage();
                }
                if ((MiWi_TickGetDiff(tick2,tick1) > (ONE_SECOND * EXIT_DEMO_TIMEOUT)))
                    Run_Demo = false;
                    
                tick2 = MiWi_TickGet();
            }    
        }*/
    }
}
    	    