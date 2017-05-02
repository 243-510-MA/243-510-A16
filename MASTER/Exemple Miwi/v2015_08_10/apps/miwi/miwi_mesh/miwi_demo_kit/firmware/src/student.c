//STUDENT

#include "student.h"
#include "system.h"
#include "codes library.h"
#include "system_config.h"
#include "miwi/miwi_api.h"
#include "string.h"
#include "network.h"

uint8_t questionnaire = 0;
uint8_t switch_val = 0;
extern bool presence;
extern uint8_t myLongAddress[];

void Student(void)
{
 
    uint8_t my_mac_adresse[32], *pos = my_mac_adresse;
    uint8_t IdAdresse[2] = {0x02, 0x01};
    uint16_t digit_adresse;
    uint16_t my_adresse;
    my_adresse =(myShortAddress.v[1] << 8) + (myShortAddress.v[0]); //Adresse courte utilisée pour la messagerie, s'affiche plus a la mise en fonction
    /* Impression de l'adresse MAC du device */
    LCD_Erase();
    pos += sprintf((char *)pos,(char*) " Student Device ");
    for(int i = 0 ; i < 8 ; i++)
        pos += sprintf((char *)pos, (char*) "%02X",myLongAddress[i]);
    memcpy(&LCDText,&my_mac_adresse,32);    
    LCD_Update();

    for (int i = 0; i < 10; i++)
    {
        DELAY_ms(250);
    }
    
    void questionnaire_function(void);
    
    /* Routines d'interruption de timer se trouve dans drv_mrf_miwi_24j40.c à la ligne 1925*/
    T1CON = 0x31;
    PIR1bits.TMR1IF = 0;
    TMR1H = 0x3C;
    TMR1L = 0xB0;
    INTCON = 0xC0;
    PIE1bits.TMR1IE = 1;
    
    
    while (true)
    {
        questionnaire_function();
        sprintf((char *) &LCDText, (char*) "SW1: PROJECTOR  SW2: Suivant   ");
        LCD_Update();

        while (switch_val == 0)
        {
            switch_val = BUTTON_Pressed();
            questionnaire_function();
            sprintf((char *) &LCDText, (char*) "SW1: PROJECTOR  SW2: Suivant   ");
            LCD_Update();
        }

        if (switch_val == SW1)
        {
            switch_val = 0;
            sprintf((char *) &LCDText, (char*) "SW1: ON         SW2: OFF        ");
            LCD_Update();

            while (switch_val == 0)
            {
                switch_val = BUTTON_Pressed();
                questionnaire_function();
                sprintf((char *) &LCDText, (char*) "SW1: ON         SW2: OFF        ");
                LCD_Update();
            }

            if (switch_val == SW1)
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

            if (switch_val == SW2)
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

        if (switch_val == SW2) //menu suivant
        {
            int send = 0;
            switch_val = 0;
            sprintf((char *) &LCDText, (char*) "SW1: Message    SW2: Suivant    ");
            LCD_Update();

            while (switch_val == 0)
            {
                switch_val = BUTTON_Pressed();
                reception_unicast();
                sprintf((char *) &LCDText, (char*) "SW1: Message    SW2: Suivant    ");
                LCD_Update();
            }

            if (switch_val == SW1) //entre dans le menu message
            {
                int digit[3] = {0, 0, 0};
                int select = 0;

                switch_val = 0;
                sprintf((char *) &LCDText, (char*) "A qui souhaitez-vous l'envoyer? ");
                LCD_Update();

                delay_ms(750);
                while (send == 0)
                {
                    sprintf((char *) &LCDText, (char*) "SW1:++  SW2:Suivid=%d%d%d  select=%d", digit[0], digit[1], digit[2], select);
                    LCD_Update();

                    while (switch_val == 0)
                    {
                        switch_val = BUTTON_Pressed();
                        reception_unicast();
                        sprintf((char *) &LCDText, (char*) "SW1:++  SW2:Suivid=%d%d%d  select=%d", digit[0], digit[1], digit[2], select);
                        LCD_Update();
                    }

                    if (switch_val == SW1)
                    {

                        switch_val = 0;
                        delay_ms(150);
                        digit[select] = 1 + digit[select];
                        if (digit[select] == 10)
                        {
                            digit[select] = 0;
                        }
                        sprintf((char *) &LCDText, (char*) "SW1:++  SW2:Suivid=%d%d%d  select=%d", digit[0], digit[1], digit[2], select);
                        LCD_Update();
                        reception_unicast();

                    }

                    if (switch_val == SW2)
                    {
                        switch_val = 0;

                        delay_ms(150);
                        select = select + 1;
                        sprintf((char *) &LCDText, (char*) "SW1:++  SW2:Suivid=%d%d%d  select=%d", digit[0], digit[1], digit[2], select);
                        LCD_Update();

                        if (select == 3)
                        {

                            sprintf((char *) &LCDText, (char*) "SW1:send        SW2:re-cycler   ");
                            LCD_Update();
                            delay_ms(500);
                            while (switch_val == 0)
                            {
                                switch_val = BUTTON_Pressed();
                                reception_unicast();
                                sprintf((char *) &LCDText, (char*) "SW1:send        SW2:re-cycler   ");
                                LCD_Update();
                            }
                            if (switch_val == SW1)
                            {
                                switch_val = 0;

                                send = 1;
                                sprintf((char *) &LCDText, (char*) "adresse choisi                      ");
                                LCD_Update();
                                digit_adresse = ((digit[0] << 8) + (digit[1] << 4) + (digit[2]));

                                /*
                                 *  Exemple : 0x310
                                 *  3 << 8
                                 *  1 << 4
                                 *  0
                                 *  0011 0001 0000
                                 */

                                IdAdresse[1] = digit_adresse >> 8;
                                IdAdresse[0] = digit_adresse;

                                delay_ms(750);
                            }
                            if (switch_val == SW2)
                            {
                                switch_val = 0;
                                select = 0;
                            }
                        }
                    }
                }

                switch_val = 0;
                sprintf((char *) &LCDText, (char*) "SW1: Allo       SW2: Suivant    ");
                LCD_Update();

                while (switch_val == 0)
                {
                    switch_val = BUTTON_Pressed();
                    reception_unicast();
                    sprintf((char *) &LCDText, (char*) "SW1: Allo       SW2: Suivant    ");
                    LCD_Update();
                }

                if (switch_val == SW1) //envoie message ALLO
                {
                    switch_val = 0;
                    LED1 = 1;
                    MiApp_FlushTx();
                    MiApp_WriteData(MSG_ALLO);
                    MiApp_WriteData(myShortAddress.v[0]);
                    MiApp_WriteData(myShortAddress.v[1]);
                    UnicastShortAddress(&IdAdresse);
                    delay_ms(500);
                    LED1 = 0;
                }


                if (switch_val == SW2) //passe au prochain message
                {
                    switch_val = 0;
                    sprintf((char *) &LCDText, (char*) "SW1: Ca va?     SW2: Suivant    ");
                    LCD_Update();

                    while (switch_val == 0)
                    {
                        switch_val = BUTTON_Pressed();
                        reception_unicast();
                        sprintf((char *) &LCDText, (char*) "SW1: Ca va?     SW2: Suivant    ");
                        LCD_Update();
                    }

                    if (switch_val == SW1) //envoie le message Ca va?
                    {
                        switch_val = 0;
                        LED1 = 1;
                        MiApp_FlushTx();
                        MiApp_WriteData(MSG_CA_VA);
                        MiApp_WriteData(myShortAddress.v[0]);
                        MiApp_WriteData(myShortAddress.v[1]);
                        UnicastShortAddress(&IdAdresse);
                        delay_ms(500);
                        LED1 = 0;
                    }
                    if (switch_val == SW2) //passe au prochain message
                    {
                        switch_val = 0;
                        sprintf((char *) &LCDText, (char*) "SW1: Oui        SW2: Suivant    ");
                        LCD_Update();

                        while (switch_val == 0)
                        {
                            switch_val = BUTTON_Pressed();
                            reception_unicast();
                            sprintf((char *) &LCDText, (char*) "SW1: Oui        SW2: Suivant    ");
                            LCD_Update();
                        }

                        if (switch_val == SW1) //envoie le message oui
                        {
                            switch_val = 0;
                            LED1 = 1;
                            MiApp_FlushTx();
                            MiApp_WriteData(MSG_OUI);
                            MiApp_WriteData(myShortAddress.v[0]);
                            MiApp_WriteData(myShortAddress.v[1]);
                            UnicastShortAddress(&IdAdresse);
                            delay_ms(500);
                            LED1 = 0;
                        }

                        if (switch_val == SW2) //passe au prochain message
                        {
                            switch_val = 0;
                            sprintf((char *) &LCDText, (char*) "SW1: Non        SW2: Suivant    ");
                            LCD_Update();

                            while (switch_val == 0)
                            {
                                switch_val = BUTTON_Pressed();
                                reception_unicast();
                                sprintf((char *) &LCDText, (char*) "SW1: Non        SW2: Suivant    ");
                                LCD_Update();
                            }

                            if (switch_val == SW1) //envoie message non
                            {
                                switch_val = 0;
                                LED1 = 1;
                                MiApp_FlushTx();
                                MiApp_WriteData(MSG_NON);
                                MiApp_WriteData(myShortAddress.v[0]);
                                MiApp_WriteData(myShortAddress.v[1]);
                                UnicastShortAddress(&IdAdresse);
                                delay_ms(500);
                                LED1 = 0;
                            }
                        }
                    }
                }
            }

            if (switch_val == SW2)
            {
                switch_val = 0;
                sprintf((char *) &LCDText, (char*) "SW1: Unlock DoorSW2: Suivant  ");
                LCD_Update();

                while (switch_val == 0)
                {
                    switch_val = BUTTON_Pressed();
                    questionnaire_function();
                    sprintf((char *) &LCDText, (char*) "SW1: Unlock DoorSW2: Suivant  ");
                    LCD_Update();
                }

                if (switch_val == SW1)
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

                if (switch_val == SW2)
                {
                    switch_val = 0;
                }
            }
        }
    }
}

void questionnaire_function(void)
{
    if (MiApp_MessageAvailable())
    {
        if (rxMessage.Payload[0] == MSG_ALLO)
        {
            sprintf((char *) &LCDText, (char*) "Allo!                           ");
            LCD_Update();
            delay_ms(1500);
        }
        MiApp_DiscardMessage();
        if (rxMessage.Payload[0] == MSG_CA_VA)
        {
            sprintf((char *) &LCDText, (char*) "Ca va?                          ");
            LCD_Update();
            delay_ms(1500);
        }
        MiApp_DiscardMessage();
        if (rxMessage.Payload[0] == MSG_OUI)
        {
            sprintf((char *) &LCDText, (char*) "Oui                             ");
            LCD_Update();
            delay_ms(1500);
        }
        MiApp_DiscardMessage();
        if (rxMessage.Payload[0] == MSG_NON)
        {
            sprintf((char *) &LCDText, (char*) "Non                             ");
            LCD_Update();
            delay_ms(1500);
        }
        MiApp_DiscardMessage();
        if (rxMessage.Payload[0] == QUEST_ON)
        {
            //MiApp_FlushTx();
            questionnaire = 1;
            LED1 = 1;
            delay_ms(500);
            LED1 = 0;
        }
        MiApp_DiscardMessage();
        if (rxMessage.Payload[0] == QUEST_OFF)
        {
            //MiApp_FlushTx();
            questionnaire = 0;
            LED1 = 1;
            delay_ms(500);
            LED1 = 0;
        }
        MiApp_DiscardMessage();
              
        if (questionnaire == 1)
        {
            sprintf((char *) &LCDText, (char*) "SW1: A          SW2: Suivant    ");
            LCD_Update();
            while (switch_val == 0)
            {
                switch_val = BUTTON_Pressed();
            }
            if (switch_val == SW1)
            {
                switch_val = 0;
                LED1 = 1;
                MiApp_FlushTx();
                MiApp_WriteData(Reponse_A);
                MiApp_WriteData(myShortAddress.v[0]);
                MiApp_WriteData(myShortAddress.v[1]);
                MiApp_BroadcastPacket(false);
                delay_ms(500);
                LED1 = 0;
            }
            if (switch_val == SW2)
            {
                switch_val = 0;
                sprintf((char *) &LCDText, (char*) "SW1: B          SW2: Suivant");
                LCD_Update();
                while (switch_val == 0)
                {
                    switch_val = BUTTON_Pressed();
                }
                if (switch_val == SW1)
                {
                    switch_val = 0;
                    LED1 = 1;
                    MiApp_FlushTx();
                    MiApp_WriteData(Reponse_B);
                    MiApp_WriteData(myShortAddress.v[0]);
                    MiApp_WriteData(myShortAddress.v[1]);
                    MiApp_BroadcastPacket(false);
                    delay_ms(500);
                    LED1 = 0;
                }
                if (switch_val == SW2)
                {
                    switch_val = 0;
                    sprintf((char *) &LCDText, (char*) "SW1: C          SW2: Suivant");
                    LCD_Update();
                    while (switch_val == 0)
                    {
                        switch_val = BUTTON_Pressed();
                    }
                    if (switch_val == SW1)
                    {
                        switch_val = 0;
                        LED1 = 1;
                        MiApp_FlushTx();
                        MiApp_WriteData(Reponse_C);
                        MiApp_WriteData(myShortAddress.v[0]);
                        MiApp_WriteData(myShortAddress.v[1]);
                        MiApp_BroadcastPacket(false);
                        delay_ms(500);
                        LED1 = 0;
                    }
                    if (switch_val == SW2)
                    {
                        switch_val = 0;
                        sprintf((char *) &LCDText, (char*) "SW1: D        ");
                        LCD_Update();
                        while (switch_val == 0)
                        {
                            switch_val = BUTTON_Pressed();
                        }
                        if (switch_val == SW1)
                        {
                            switch_val = 0;
                            LED1 = 1;
                            MiApp_FlushTx();
                            MiApp_WriteData(Reponse_D);
                            MiApp_WriteData(myShortAddress.v[0]);
                            MiApp_WriteData(myShortAddress.v[1]);
                            MiApp_BroadcastPacket(false);
                            delay_ms(500);
                            LED1 = 0;
                        }
                    }
                }
            }
            sprintf((char *) &LCDText, (char*) "Reponse envoyee.      Attente...");
            LCD_Update();
            while (questionnaire == 1)
            {
                if (MiApp_MessageAvailable())
                    if (rxMessage.Payload[0] == QUEST_OFF)
                    {
                        //MiApp_FlushTx();
                        questionnaire = 0;
                        LED1 = 1;
                        delay_ms(500);
                        LED1 = 0;
                    }
                MiApp_DiscardMessage();
            }
        }
        
        
    }
    else if (presence == true)
        {
                MiApp_FlushTx();
                MiApp_WriteData(POLL_PRESENCE);
                for(uint8_t i = 0 ; i < 8 ; i++)
                MiApp_WriteData(myLongAddress[i]);
               
                MiApp_BroadcastPacket(false);
                presence = false;   
        }
    
    
  /*  else if (presence == true)
    {
                MiApp_FlushTx();
                MiApp_WriteData(POLL_PRESENCE);
                for(uint8_t i = 0 ; i < 8 ; i++)
                MiApp_WriteData(myLongAddress[i]);
               
                MiApp_BroadcastPacket(false);
                presence = false;
        
        
        
    }*/
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
                   if(rssi > 50) //200)
                   {
                       //sprintf((char *)&LCDText, (char*)" Unlock Door ?  SW1:YES   SW2:NO");
                       LED0 = 1;
                       LED1 = 0;
                       LED2 = 0;
                       /*
                       switch_val = BUTTON_Pressed();
                       if(switch_val == SW1)
                       {
                           LED0 = 1;
                           LED1 = 1;
                           LED2 = 1;
                       }
 */
/*
                        }

                        else //if(switch_val == SW2)
                        {
                            //*******************************************************************
                            // Send Exit Demo Request Packet and exit Door Unlock
                            //*******************************************************************
                            LED0 = 0;
                            LED1 = 0;
                            LED2 = 1;
                            MiApp_FlushTx();    
                            MiApp_WriteData(EXIT_PKT);
                            MiApp_UnicastConnection(ConnectionEntry, false);


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



                        /*
                        else if(rssi < 201)
                        {
                            sprintf((char *)&LCDText, (char*)" Get Closer to      the Door    ");
                            LED0 = 0;
                            LED1 = 1;
                            LED2 = 0;
                            switch_val = BUTTON_Pressed();
                            if((switch_val == SW1) || (switch_val == SW2))
                            {
                                //*******************************************************************
                                // Send Exit Demo Request Packet and exit Door Unlock
                                //******************************************************************
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
                            }    
                        }

                        // Convert to dB
                        //rssi = RSSIlookupTable[rssi];
                        //sprintf((char *)&(LCDText[16]), (char*)"Rcv RSSI: %03d", rssi);
                    }  

 */
/*
                    LCD_Update();
                    Tx_Packet = true;

                } 	     
                //*******************************************************************
                // Read New Start tickcount
                //*******************************************************************
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
                LCD_Display((char *)"   Exiting....     Door Unlock  ", 0, true);
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