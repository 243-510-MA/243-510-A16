//TEACHER

#include "teacher.h"
#include "system.h"
#include "codes library.h"
#include "system_config.h"
#include "miwi/miwi_api.h"
#include "string.h"

void Teacher(void)
{
    uint8_t i = 0;
    uint8_t switch_val = 0;
    extern uint8_t ConnectionEntry;
    extern uint8_t myLongAddress[];
    bool Tx_Packet = true;
    uint8_t rssi = 0;
    uint8_t memoire = 0;
    uint8_t Pkt_Loss_Cnt = 0;
    uint8_t questionnaire = 0;
    uint8_t reponsea = 0;
    uint8_t reponseb = 0;
    uint8_t reponsec = 0;
    uint8_t reponsed = 0;
    uint8_t choix_envoi = 0;
    uint16_t my_adresse;
    uint8_t IdAdresse[2] = {0x02, 0x01};
    uint16_t digit_adresse;
    uint8_t my_mac_adresse[32], *pos = my_mac_adresse;
            
    my_adresse = (myShortAddress.v[1] << 8) + (myShortAddress.v[0]); //Adresse courte utilisée pour la messagerie, s'affiche plus a la mise en fonction

    
    /* Impression de l'adresse MAC du device */
    LCD_Erase();
    pos += sprintf((char *)pos,(char*) " Teacher Device ");
    for(int i = 0 ; i < 8 ; i++)
        pos += sprintf((char *)pos, (char*) "%02X",myLongAddress[i]);
    memcpy(&LCDText,&my_mac_adresse,32);
    LCD_Update();

    for (i = 0; i < 10; i++)
    {
        DELAY_ms(250);
    }
    //   unicast adresse test pour 0x0102
    uint8_t IdAdresse[2] = {0x02, 0x01};



    void reception_unicast(void);

    while (true)
    {

        /*
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
                if(rssi > 120)
                {
                    MiApp_FlushTx();
                    MiApp_WriteData(PROJECTOR_ON);
                    MiApp_WriteData(myShortAddress.v[0]);
                    MiApp_WriteData(myShortAddress.v[1]);
                    MiApp_FlushTx();
                    MiApp_WriteData(PROJECTOR_MOTOR_DOWN);
                    MiApp_WriteData(myShortAddress.v[0]);
                    MiApp_WriteData(myShortAddress.v[1]);
                    memoire=1;*
                    MiApp_BroadcastPacket(false);
                    LCD_Display((char *)" ALLO ", 0, false);
                    Tx_Packet = false;
                }
                
                else if(rssi < 50)
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
        
        if(MiApp_MessageAvailable())
        {
            if(rxMessage.Payload[0] == RANGE_PKT)
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
         */
        reception_unicast();
        switch_val = 0;
        sprintf((char *) &LCDText, (char*) "SW1: Projector  SW2: Suivant  ");
        LCD_Update();

        while (switch_val == 0)
        {
            switch_val = BUTTON_Pressed();
            reception_unicast();
            sprintf((char *) &LCDText, (char*) "SW1: Projector  SW2: Suivant  ");
            LCD_Update();
        }

        if (switch_val == SW1) //entre dans le menu PROJECTOR
        {
            switch_val = 0;
            sprintf((char *) &LCDText, (char*) "SW1: ON         SW2: OFF        ");
            LCD_Update();

            while (switch_val == 0)
            {
                switch_val = BUTTON_Pressed();
                reception_unicast();
                sprintf((char *) &LCDText, (char*) "SW1: ON         SW2: OFF        ");
                LCD_Update();
            }

            if (switch_val == SW1) //Allume le projecteur
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

            if (switch_val == SW2) //Eteind le projecteur
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
                sprintf((char *) &LCDText, (char*) "SW1: Unlock DoorSW2: Suivant  "); //blou blou
                LCD_Update();

                while (switch_val == 0)
                {
                    switch_val = BUTTON_Pressed();
                    reception_unicast();
                    sprintf((char *) &LCDText, (char*) "SW1: Unlock DoorSW2: Suivant  "); //blou blou
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
                    sprintf((char *) &LCDText, (char*) "SW1: Proj Motor SW2: Suivant    ");
                    LCD_Update();

                    while (switch_val == 0)
                    {
                        switch_val = BUTTON_Pressed();
                        reception_unicast();
                        sprintf((char *) &LCDText, (char*) "SW1: Proj Motor SW2: Suivant    ");
                        LCD_Update();
                    }
                    if (switch_val == SW1)
                    {
                        switch_val = 0;
                        sprintf((char *) &LCDText, (char*) "SW1: DOWN       SW2: UP         ");
                        LCD_Update();

                        while (switch_val == 0)
                        {
                            switch_val = BUTTON_Pressed();
                            reception_unicast();
                            sprintf((char *) &LCDText, (char*) "SW1: DOWN       SW2: UP         ");
                            LCD_Update();
                        }

                        if (switch_val == SW1)
                        {
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

                        if (switch_val == SW2)
                        {
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
                    if (switch_val == SW2)
                    {
                        switch_val = 0;
                        sprintf((char *) &LCDText, (char*) "SW1: Alarm      SW2: Suivant    ");
                        LCD_Update();

                        while (switch_val == 0)
                        {
                            switch_val = BUTTON_Pressed();
                            reception_unicast();
                            sprintf((char *) &LCDText, (char*) "SW1: Alarm      SW2: Suivant    ");
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
                                reception_unicast();
                                sprintf((char *) &LCDText, (char*) "SW1: ON         SW2: OFF        ");
                                LCD_Update();
                            }

                            if (switch_val == SW1)
                            {
                                switch_val = 0;
                                LED1 = 1;
                                MiApp_FlushTx();
                                MiApp_WriteData(ALARM_ON);
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
                                MiApp_WriteData(ALARM_OFF);
                                MiApp_WriteData(myShortAddress.v[0]);
                                MiApp_WriteData(myShortAddress.v[1]);
                                MiApp_BroadcastPacket(false);
                                delay_ms(500);
                                LED1 = 0;
                            }
                        }
                        if (switch_val == SW2)
                        {
                            switch_val = 0;
                            sprintf((char *) &LCDText, (char*) "SW1: Questions  SW2: Suivant    ");
                            LCD_Update();

                            while (switch_val == 0)
                            {
                                switch_val = BUTTON_Pressed();
                                reception_unicast();
                                sprintf((char *) &LCDText, (char*) "SW1: Questions  SW2: Suivant    ");
                                LCD_Update();
                            }
                            if (switch_val == SW1)
                            {
                                switch_val = 0;
                                LED1 = 1;
                                sprintf((char *) &LCDText, (char*) "SW1: Question envoye.    ");
                                LCD_Update();
                                delay_ms(500);
                                questionnaire = 1;
                                MiApp_FlushTx();
                                MiApp_WriteData(QUEST_ON);
                                MiApp_WriteData(myShortAddress.v[0]);
                                MiApp_WriteData(myShortAddress.v[1]);
                                MiApp_BroadcastPacket(false);
                                LED1 = 0;
                                delay_ms(100);
                                while (questionnaire == 1 && switch_val == 0)
                                {
                                    switch_val = BUTTON_Pressed();
                                    sprintf((char *) &LCDText, (char*) "A: %u B: %u C: %u D: %u      ", reponsea, reponseb, reponsec, reponsed);
                                    LCD_Update();
                                    delay_ms(100);
                                    if (MiApp_MessageAvailable())
                                    {
                                        if (rxMessage.Payload[0] == Reponse_A)
                                        {
                                            reponsea++;
                                        }
                                        if (rxMessage.Payload[0] == Reponse_B)
                                        {
                                            reponseb++;
                                        }
                                        if (rxMessage.Payload[0] == Reponse_C)
                                        {
                                            reponsec++;
                                        }
                                        if (rxMessage.Payload[0] == Reponse_D)
                                        {
                                            reponsed++;
                                        }
                                        MiApp_DiscardMessage();
                                    }
                                    if (switch_val == SW1)
                                    {
                                        switch_val = 0;
                                        questionnaire = 0;
                                        reponsea = 0;
                                        reponseb = 0;
                                        reponsec = 0;
                                        reponsed = 0;
                                        sprintf((char *) &LCDText, (char*) "Quest. Fini");
                                        LCD_Update();
                                        MiApp_FlushTx();
                                        MiApp_WriteData(QUEST_OFF);
                                        MiApp_WriteData(myShortAddress.v[0]);
                                        MiApp_WriteData(myShortAddress.v[1]);
                                        MiApp_BroadcastPacket(false);
                                        delay_ms(500);
                                    }
                                }
                            }

                        }
                    }
                }
            }//blooup bloup
        }
    }
}

void reception_unicast(void)
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
            sprintf((char *) &LCDText, (char*) "Oui                             ", rxMessage.SourceAddress);
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


