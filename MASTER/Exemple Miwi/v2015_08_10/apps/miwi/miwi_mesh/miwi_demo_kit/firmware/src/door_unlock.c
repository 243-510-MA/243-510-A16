#include "door_unlock.h"
#include "system.h"
#include "system_config.h"
#include "miwi/miwi_api.h"

#define TX_PKT_INTERVAL             4
#define DISPLAY_RSSI_INTERVAL       4
#define EXIT_DEMO_TIMEOUT           6

#define UNLOCK_PKT      1

extern uint8_t ConnectionEntry;

void DoorUnlock(void)
{
    DOOR = 0;
    while(true)
    {
        if(MiApp_MessageAvailable())
        {
            if(rxMessage.Payload[0] == UNLOCK_PKT)
            {
                DOOR = 1;
                LED1 = 1;
                delay_ms(10000);
                LED1 = 0;
                DOOR = 0;
            }
        }
    }
}