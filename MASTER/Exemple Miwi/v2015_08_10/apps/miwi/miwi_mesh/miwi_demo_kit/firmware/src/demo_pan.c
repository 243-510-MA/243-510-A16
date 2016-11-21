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
    UART_TX_TRIS = 0;
    int alarm_status = 1;
    
    while(true)
    {
        if(MiApp_MessageAvailable())
        {
            if (rxMessage.Payload[0] == DEMO_ALARM_ARMED)
            {alarm_status = 1;}
            
            if (rxMessage.Payload[0] == DEMO_ALARM_DISARMED)
            {alarm_status = 0;}
            
            if (alarm_status == 1)
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
        }
        MiApp_DiscardMessage();
    }
    
    
    
}

void demo_gyro(int status)
{
    LED0 = status;
    LED1 = status;
    LED2 = status;
    UART_TX_V = status;
    
}