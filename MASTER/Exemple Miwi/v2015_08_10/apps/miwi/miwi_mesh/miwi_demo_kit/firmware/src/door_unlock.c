//DOOR UNLOCK

#include "door_unlock.h"
#include "codes library.h"
#include "system.h"
#include "system_config.h"
#include "miwi/miwi_api.h"

extern uint8_t ConnectionEntry;

void DoorUnlock(void)
{
    DOOR = 1;
    while(true)
    {
        if(MiApp_MessageAvailable())
        {
            if(rxMessage.Payload[0] == UNLOCK_PKT)
            {
                DOOR = 0;
                LED1 = 1;
                delay_ms(1000);
                LED1 = 0;
                DOOR = 1;
            }
            MiApp_DiscardMessage();
        }
    }
}