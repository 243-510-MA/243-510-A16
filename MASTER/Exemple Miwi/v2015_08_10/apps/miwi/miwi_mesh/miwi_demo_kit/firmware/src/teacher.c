//TEACHER

#include "teacher.h"
#include "system.h"
#include "codes library.h"
#include "system_config.h"
#include "miwi/miwi_api.h"

void Teacher(void)
{
    uint8_t switch_val = 0;
    extern uint8_t ConnectionEntry;
    bool Tx_Packet = true;
    uint8_t rssi = 0;
    uint8_t memoire = 0;
    uint8_t Pkt_Loss_Cnt = 0;
    LCD_Erase();
    LCD_Display((char *)" O'shit wuddup", 0, true);
   
    while(true)
    {
        if(Tx_Packet)
        {
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

            if(MiApp_UnicastConnection(ConnectionEntry, false) == false)
                Pkt_Loss_Cnt++;
            else
                Pkt_Loss_Cnt = 0;
      	
            if(Pkt_Loss_Cnt < 1)
            {           		
                if(rssi < 50)
                {
                    MiApp_FlushTx();
                    MiApp_WriteData(PROJECTOR_ON);
                    MiApp_WriteData(myShortAddress.v[0]);
                    MiApp_WriteData(myShortAddress.v[1]);
                    MiApp_FlushTx();
                    MiApp_WriteData(PROJECTOR_MOTOR_DOWN);
                    MiApp_WriteData(myShortAddress.v[0]);
                    MiApp_WriteData(myShortAddress.v[1]);
                    memoire=1;
                    MiApp_BroadcastPacket(false);
                    Tx_Packet = false;
                }
                else if(rssi > 201)
                {
                    MiApp_FlushTx();
                    MiApp_WriteData(PROJECTOR_OFF);
                    MiApp_WriteData(myShortAddress.v[0]);
                    MiApp_WriteData(myShortAddress.v[1]);
                    MiApp_FlushTx();
                    MiApp_WriteData(PROJECTOR_MOTOR_UP);
                    MiApp_WriteData(myShortAddress.v[0]);
                    MiApp_WriteData(myShortAddress.v[1]);
                    memoire=0;
                    MiApp_BroadcastPacket(false);
                    Tx_Packet = false;
                }    
            }
        } 	  
        
        sprintf((char *)&LCDText, (char*)"SW1: PROJECTOR  SW2: Suivant  ");
        LCD_Update();
        
        while(switch_val == 0)
        {
            switch_val = BUTTON_Pressed();
        }
        
        if(switch_val == SW1)
        {
            switch_val = 0;
            sprintf((char *)&LCDText, (char*)"SW1: ON         SW2: OFF        ");
            LCD_Update();
            
            while(switch_val == 0)
            {
                switch_val = BUTTON_Pressed();
            }
            
            if(switch_val == SW1)
            {
                switch_val = 0;
                LED1 = 1;
                MiApp_FlushTx();
                MiApp_WriteData(PROJECTOR_ON);
                MiApp_WriteData(myShortAddress.v[0]);
                MiApp_WriteData(myShortAddress.v[1]);
                MiApp_BroadcastPacket(false);
                delay_ms(500);
                LED1 = 0;
            }
            
            if(switch_val == SW2)
            {
                switch_val = 0;
                LED1 = 1;
                MiApp_FlushTx();
                MiApp_WriteData(PROJECTOR_OFF);
                MiApp_WriteData(myShortAddress.v[0]);
                MiApp_WriteData(myShortAddress.v[1]);
                MiApp_BroadcastPacket(false);
                delay_ms(500);
                LED1 = 0;
            }
        }
        
        if(switch_val == SW2)
        {
            switch_val = 0;
            sprintf((char *)&LCDText, (char*)"SW1: Unlock DoorSW2: Suivant  ");
            LCD_Update();
            
            while(switch_val == 0)
            {
                switch_val = BUTTON_Pressed();
            }
            
            if(switch_val == SW1)
            {
                switch_val = 0;
                LED1 = 1;
                MiApp_FlushTx();
                MiApp_WriteData(UNLOCK_PKT);
                MiApp_WriteData(myShortAddress.v[0]);
                MiApp_WriteData(myShortAddress.v[1]);
                MiApp_BroadcastPacket(false);
                delay_ms(500);
                LED1 = 0;
                
            }
            
            if(switch_val == SW2)
            {
                switch_val = 0;
                sprintf((char *)&LCDText, (char*)"SW1: PROJ MOTOR SW2: Suivant    ");
                LCD_Update();
            
                while(switch_val == 0)
                {
                    switch_val = BUTTON_Pressed();
                }
                if(switch_val == SW1)
                {
                    if(memoire==1)//projector is up
                    {
                        memoire=0;
                        switch_val = 0;
                        LED1 = 1;
                        MiApp_FlushTx();
                        MiApp_WriteData(PROJECTOR_MOTOR_DOWN);
                        MiApp_WriteData(myShortAddress.v[0]);
                        MiApp_WriteData(myShortAddress.v[1]);
                        MiApp_BroadcastPacket(false);
                        delay_ms(500);
                        LED1 = 0;
                    }
                    else
                    {
                        memoire=1;
                        switch_val = 0;
                        LED1 = 1;
                        MiApp_FlushTx();
                        MiApp_WriteData(PROJECTOR_MOTOR_UP);
                        MiApp_WriteData(myShortAddress.v[0]);
                        MiApp_WriteData(myShortAddress.v[1]);
                        MiApp_BroadcastPacket(false);
                        delay_ms(500);
                        LED1 = 0;
                    }
                }
                if(switch_val == SW2)
                {
                    switch_val = 0;
                }
            }    
        }        
    }
}
/*     
            
            
            
    while(true)
    {
        
        bool Run_Demo = true;
        bool Tx_Packet = true;
        uint8_t rssi = 0;
        uint8_t Pkt_Loss_Cnt = 0;
        MIWI_TICK tick1, tick2;
        uint8_t switch_val;
        uint8_t pktCmd = 0;

        //*******************************************************************
        // Dispaly Door Unlock Splach Screen
        //*******************************************************************
        LCD_Display((char *)"   Microchip       Door Unlock  ", 0, true);

        //*******************************************************************
        // Read Start tickcount
        //*******************************************************************	
        tick1 = MiWi_TickGet();

        while(Run_Demo)
        {	
            //*******************************************************************
            // Read current tickcount
            //*******************************************************************
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
                        if(rssi < 50) //200)
                        {
               
                        }    
                    }

                        else
                        {
                        }
                    Tx_Packet = true;

                } 	     
                //*******************************************************************
                // Read New Start tickcount
                //*******************************************************************
                tick1 = MiWi_TickGet(); 

            }



            // Check if  Switch Pressed
            switch_val = BUTTON_Pressed();

            if((switch_val == SW1) || (switch_val == SW2))
            {
                /*******************************************************************
                // Send Exit Demo Request Packet and exit Door Unlock
                /*******************************************************************
                MiApp_FlushTx();    
                MiApp_WriteData(EXIT_PKT);
                MiApp_UnicastConnection(ConnectionEntry, false);

                LCD_BacklightON();
                LCD_Display((char *)"   Exiting....     Door Unlock  ", 0, true);
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
            }*//*
        }
    }
}
    	  
*/
    

    	    