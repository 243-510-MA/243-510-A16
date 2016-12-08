
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
    DIGITAL_ANALOG_DETECT = 1;
    LCD_BKLT = 1;
    void reception(void);
    
    PIE1bits.TMR1IE = 0;
    T1CON = 0x31;
    PIR1bits.TMR1IF = 0;
    TMR1H = 0x3C;
    TMR1L = 0xB0;
    
    INTCON = 0xC0;
    uint8_t derniere_detection = 0;     //0 = etat_début  1: detection  2: plus de mouvement après détection

    reception();
    
    while (true)
    {
        
        reception();
        
        if (DETECT == 0)
        {
            reception();
            if(derniere_detection == 1) derniere_detection = 2;
            
            LED1 = 0;
            delay_ms(150);
        }

        else if (DETECT == 1)
        {
            reception();
            derniere_detection = 1;
            LED1 = 1;
            MiApp_FlushTx();
            MiApp_WriteData(DEMO_ALARM_ON);
            MiApp_WriteData(myShortAddress.v[0]);
            MiApp_WriteData(myShortAddress.v[1]);
            MiApp_BroadcastPacket(false);
            delay_ms(150);
        }
        
        if(derniere_detection == 2)
        {
            reception();
            PIE1bits.TMR1IE = 0;
        }
    }
}

void reception(void)
{
    if (MiApp_MessageAvailable())
    {
        if (rxMessage.Payload[0] == GET_LAST_MOVEMENT)
        {
            sprintf((char *) &LCDText, (char*) "Envoyer le temps"                );
            LCD_Update();
            
            MiApp_FlushTx();
            MiApp_WriteData(SEND_LAST_MOVEMENT);
            MiApp_WriteData(Chrono.Heures);
            MiApp_WriteData(Chrono.Minutes);
            MiApp_WriteData(Chrono.Secondes);
            MiApp_WriteData(myShortAddress.v[0]);
            MiApp_WriteData(myShortAddress.v[1]);
            MiApp_BroadcastPacket(false);
            delay_ms(150);
            
        }
        MiApp_DiscardMessage();
    } 
}

