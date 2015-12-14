//PROJECTOR_SCREEN

#include "projector_screen.h"
#include "codes library.h"
#include "system.h"
#include "system_config.h"
#include "miwi/miwi_api.h"

void ProjectorScreen(void)
{
    
        RCONbits.IPEN = 1;
    T3CON = 0b00011111;
    T1RUN = 1;
    TMR3 = 0xE000;
    TMR3IP = 1;//hihg prioritu
    INTCONbits.GIEH = 1;
    TMR3IE =1;
    TMR3IF = 0;//flagbit
    LCD_BKLT = 1;
    while(true)
    {

    }
}
