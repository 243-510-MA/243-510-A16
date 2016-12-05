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
                DOOR = 0;        // IL FAUDARIT FAIRE UNE INTERRUPTION POUR***
                LED1 = 1;        // POUR POURVOIR AVOIR UN STATUS DOOR OPEN
                delay_ms(1000);
                LED1 = 0;
                DOOR = 1;
            }
            else if(rxMessage.Payload[0] == STATUS_PORTE)
            {
               if(DOOR == 0)   //NE SERA JAMAIS 0 DU AU IF DU DESSUS ****
               {               //QUI NEST PAS SUR TIMER MAIS SUR DELAY
                
			      LED1 = 1;
                  MiApp_FlushTx();
                  MiApp_WriteData(DOOR_OPEN);
                  MiApp_WriteData(myShortAddress.v[0]);
                  MiApp_WriteData(myShortAddress.v[1]);
                  MiApp_BroadcastPacket(false);
                  delay_ms(500);
                  LED1 = 0;
                }
	    	    else
                { LED1 = 1;
                  MiApp_FlushTx();
                  MiApp_WriteData(DOOR_CLOSED);
                  MiApp_WriteData(myShortAddress.v[0]);
                  MiApp_WriteData(myShortAddress.v[1]);
                  MiApp_BroadcastPacket(false);
                  delay_ms(500);
                  LED1 = 0;

		        }
	        }
            MiApp_DiscardMessage();
        }
    }    
}
