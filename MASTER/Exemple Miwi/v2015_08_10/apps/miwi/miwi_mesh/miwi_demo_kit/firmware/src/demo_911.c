/*
 * File:   demo_911.c
 * Author: e1430397
 *
 * Created on 10 novembre 2016, 15:02
 */
#include "demo_911.h"
#include "system.h"
#include "codes library.h"
#include "system_config.h"
#include "miwi/miwi_api.h"
#include "string.h"

void Demo_911(void)
{
    uint8_t switch_val = 0;
    extern uint8_t ConnectionEntry;
    uint8_t alarm_status = 0;
    uint8_t alarm_on_off = 1;
    const char msg_arme[] = "arme";
    const char msg_desarme[] = "desarme";
    char msg_affiche[7];
    bool Tx_Packet = true;
    
    LCD_Erase();
    LCD_Display((char *)"      DEMO           ALARME     ", 0, true);
    
    LCD_Erase();
    LCD_BKLT = 0;
    
    
    LED1 = 1;
    {sprintf((char *)&LCDText, (char*)"Sys: armee      SW2:ON/OFF");} // Système : armé SW2:ON/OFF
    LCD_Update();
    
    while(true)
        {

        
        
        while(switch_val == 0)
        {
            switch_val = BUTTON_Pressed();
           
            if(MiApp_MessageAvailable())
                break;
        }
        
        
        if(MiApp_MessageAvailable())
        {
            if(rxMessage.Payload[0] == DEMO_ALARM_ON )
            {
                if(alarm_on_off == 1)
                {
                sprintf((char *)&LCDText, (char*)"     ALARME      SW1:Cancel Alarme");
                LCD_Update();                
                alarm_status = 1;  
                LCD_BKLT = 1;
                }
            }      
            MiApp_DiscardMessage();
        }
        
       
            
            if(switch_val == SW1 )
            {
                
             switch_val = 0;  
             
                if(alarm_status == 1)
               {
                   alarm_status = 0;
                   LED1 = 1;
                   LCD_BKLT = 0;
                   MiApp_FlushTx();
                   MiApp_WriteData(DEMO_ALARM_OFF);
                   MiApp_WriteData(myShortAddress.v[0]);
                   MiApp_WriteData(myShortAddress.v[1]);
                   MiApp_BroadcastPacket(false);
                   delay_ms(100);
                   sprintf((char *)&LCDText, (char*)"     Alarme         Cancele!    ");
                   LCD_Update();  

                   delay_ms(400);
                   
                   
                   sprintf((char *)&LCDText, (char*)"Sys: armee      SW2:ON/OFF"); // Système : armé SW2:ON/OFF
                   LCD_Update();
                   
                   
                   
                }
                
            }
        
            else if (switch_val == SW2)
            {
                
                switch_val = 0;
                
                if(alarm_status == 0)
                    {
                    
                    alarm_on_off = !alarm_on_off;
                    
                        if(alarm_on_off == 1)
                        {
                             MiApp_FlushTx();
                            MiApp_WriteData(DEMO_ALARM_ARMED);
                            MiApp_WriteData(myShortAddress.v[0]);
                            MiApp_WriteData(myShortAddress.v[1]);
                            MiApp_BroadcastPacket(false);
                             delay_ms(100);
                            
                            LED1 = 1;    
                            sprintf((char *)&LCDText, (char*)"Sys: armee      SW2:ON/OFF   "); // Système : armé SW2:ON/OFF
                            LCD_Update();
                            delay_ms(100);
                                     
                        }
                    
                        else if (alarm_on_off == 0)
                        {
                            MiApp_FlushTx();
                            MiApp_WriteData(DEMO_ALARM_DISARMED);
                            MiApp_WriteData(myShortAddress.v[0]);
                            MiApp_WriteData(myShortAddress.v[1]);
                            MiApp_BroadcastPacket(false);
                             delay_ms(100);
                            LED1 = 0;     
                            sprintf((char *)&LCDText, (char*)"Sys: desarmee   SW2:ON/OFF   ");
                            LCD_Update();
                            delay_ms(100);
                        }
                        
                    }    
                
            }
        
            
            
        }
      
    }
