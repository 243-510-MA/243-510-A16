//DOOR UNLOCK

#include "door_unlock.h"
#include "codes library.h"
#include "system.h"
#include "system_config.h"
#include "miwi/miwi_api.h"

extern uint8_t ConnectionEntry;

/*VALEUR POUR TIMER*/
uint16_t pwm_value_high_time = 400 ;  
bool door_timer = false;




void DoorUnlock(void)
{
    
     int statusScreen = 0; //etat initialise a 0, l'ecran est roulé
    
    // Commentaires de Timer 3 - 2016 par Samuel Proulx
    // La routine d'interruption pour le timer se trouve dans "drv_mrf_miwi_24j40.c" à la ligne 1925 
     
    // Nomenclature :  (REGISTERNAME . REGISTERBIT)
    PROJECTOR_TRIS = 0;
    RCONbits.IPEN = 1; // Interrupt Priority Level Bit . 1 = Enable Priority Levels
    T3CON = 0b00011111; // Timer 3 Control Register . Voir fin de ce fichier
    TCLKCONbits.T1RUN = 1; 
    TMR3 = 0xE000; //TMR3H + TMR3L  . C'est la valeur du timer (0 à 65536) 0XE000 =  57344
    IPR2bits.TMR3IP = 1; // Overflow Priority bit. 1 = High Priority
    INTCONbits.GIEH = 1; // INTCON = Interrupt Control Access Register . GIEH = Global Interrupt Enable Bit . 1 enables all.
    PIE2bits.TMR3IE  = 0; // Overflow Interrupt Enable Bit . 1 enables
    PIR2bits.TMR3IF = 0;// Timer 3 Overflow Interrupt Flag Bit . 1 = register overflowed(MUST BE CLEARED IN SOFTWARE)
    
    DOOR = 1;
    
    while(true)
    {
        if(MiApp_MessageAvailable())
        {
            if(rxMessage.Payload[0] == UNLOCK_PKT)
            {
                /*DOOR = 0;        // IL FAUDARIT FAIRE UNE INTERRUPTION POUR***
                LED1 = 1;        // POUR POURVOIR AVOIR UN STATUS DOOR OPEN
                delay_ms(1000);
                LED1 = 0;
                DOOR = 1;  */  
                
                door_timer = true;               
                PIE2bits.TMR3IE  = 1;
                
            }
            else if(rxMessage.Payload[0] == STATUS_PORTE)
            {
             
               if(door_timer == true)   //NE SERA JAMAIS 0 DU AU IF DU DESSUS ****
               {               //QUI NEST PAS SUR TIMER MAIS SUR DELAY
                
               
			      LED1 = 1;
                  MiApp_FlushTx();
                  MiApp_WriteData(DOOR_OPEN);
                  MiApp_WriteData(myShortAddress.v[0]);
                  MiApp_WriteData(myShortAddress.v[1]);
                  MiApp_BroadcastPacket(false);
                
                  LED1 = 0;
                }
	    	    else
                { 

                  
       
                  LED1 = 1;
                  MiApp_FlushTx();
                  MiApp_WriteData(DOOR_CLOSED);
                  MiApp_WriteData(myShortAddress.v[0]);
                  MiApp_WriteData(myShortAddress.v[1]);
                  MiApp_BroadcastPacket(false);
                 
                  LED1 = 0;

		        }
            
	        }
            MiApp_DiscardMessage();
        }
    }    
}

/* Timer 3 Control Register T3CON 
 * 
 * bit 7-6  Clock Source Select
 * bit 5-4  Prescale Bits. (clockdiv?) 11 = 1:8 , 10 = 1:4, 01 = 1:2, 00 = 1:1
 * bit 3    Oscillator Source . 1 Power up the timer1 crystal driver
 * bit 2    Timer 3 External Clock Synchronisation Control Bit. 1 = Do not synchronize external clock input
 * bit 1    16 bit Read/Write Mode Enable Bit .  1 = 16 bit operation. 0 = 2 8 bit operations
 * bit 0    Timer 3 On bit . 1 enables timer 3
 * 
 */