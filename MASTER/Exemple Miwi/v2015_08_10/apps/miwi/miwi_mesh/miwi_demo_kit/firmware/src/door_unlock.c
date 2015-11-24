#include "door_unlock.h"
#include "system.h"
#include "system_config.h"
#include "miwi/miwi_api.h"

#define UNLOCK_PKT      1

extern uint8_t ConnectionEntry;

void DoorUnlock(void)
{
    DOOR = 0;
    LCD_Erase();
    sprintf((char *)&(LCDText), (char*)"   DOORUNLOCK   ");
    while(true)
    {
        sprintf((char *)&(LCDText[16]), (char*)"Address: %02x%02x", myShortAddress.v[1], myShortAddress.v[0]);
        LCD_Update();
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