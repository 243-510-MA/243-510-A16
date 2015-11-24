//PAN
/*
#include "pan.h"
#include "system.h"
#include "system_config.h"
#include "miwi/miwi_api.h"

void Pan(void)
{
    while(true)
    {
        
    }
}
    */	


#include "pan.h"
#include "system.h"
#include "system_config.h"
#include "miwi/miwi_api.h"

#define UNLOCK_PKT      1

extern uint8_t ConnectionEntry;

void Pan(void)
{
    DOOR = 0;
    LCD_Erase();
    sprintf((char *)&(LCDText), (char*)"   PAN   ");
    while(true)
    {
        if(MiApp_MessageAvailable())
        {
            if(rxMessage.Payload[0] == UNLOCK_PKT)
            {
                sprintf((char *)&(LCDText), (char*)"RECU");
                //MiApp_FlushTx();
                DOOR = 1;
                LED1 = 1;
                delay_ms(500);
                LED1 = 0;
                DOOR = 0;
            }
            MiApp_DiscardMessage();
        }
    }
}