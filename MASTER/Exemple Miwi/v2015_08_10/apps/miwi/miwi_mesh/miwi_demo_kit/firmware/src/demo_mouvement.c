
/*
 * File:   computer_control.c
 * Author: e1434245
 *
 * Created on 3 novembre 2016, 14:47
 */

#include "computer_control.h"
#include "codes library.h"
#include "system.h"
#include "system_config.h"
#include "miwi/miwi_api.h"
#include "soft_uart.h"



void MovementDetector(void)
{
    
    DIGITAL_ANALOG_DETECT = 1 ; 
    LCD_BKLT = 1;
   
    
    while(true)
    {
       

        if(DETECT == 0)
        {
            LED1 = 0;
            MiApp_FlushTx();
            MiApp_WriteData(ALARM_OFF);
            MiApp_WriteData(myShortAddress.v[0]);
            MiApp_WriteData(myShortAddress.v[1]);
            MiApp_BroadcastPacket(false);
            delay_ms(150);
        }
        
        else if(DETECT == 1)
        {
            LED1 = 1;
            MiApp_FlushTx();
            MiApp_WriteData(ALARM_ON);
            MiApp_WriteData(myShortAddress.v[0]);
            MiApp_WriteData(myShortAddress.v[1]);
            MiApp_BroadcastPacket(false);
            delay_ms(150);
        } 
    }
}