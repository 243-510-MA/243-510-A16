/*
 
 
 
 
 
 */
#include "demo_pan.h"
#include "system.h"
#include "codes library.h"
#include "system_config.h"
#include "miwi/miwi_api.h"

void Demo_Pan(void)
{
    TRISB = 0x0;
    int status = 0;
    GYRO = 0;
    Buzzer = 0;
   
    
    LCD_BKLT = 1;
    while(true)
    {
        if(MiApp_MessageAvailable())
        {
            if(rxMessage.Payload[0] == DEMO_ALARM_ON )
            {
                demo_gyro(1);
            }
            
            else if(rxMessage.Payload[0] == DEMO_ALARM_OFF )
            {
                
                demo_gyro(0);
            }
           
        }
        MiApp_DiscardMessage();
    }
    
    
    
}

void demo_gyro(int status)
{
    GYRO = status;
    
}