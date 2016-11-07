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

void output_control(int on_off)
{
    if(on_off == 1)
    {
        UART_TX_V = 1;
    }
    
    else if(on_off == 0)
    {
        UART_TX_V = 0;
    }
}

void ComputerControl(void)
{
    
    ANCON0bits.PCFG1 = 1 ;
    ANCON0bits.PCFG2 = 1 ; 
    LCD_BKLT = 1;
    char Title[]= "Hello World!";
    uint8_t CMD = 1 ;
    UART_Init_A2_A1();
    
    while(true)
    {
        LED1 = 0;
        LED2 = 0;
        LED0 = 0;
      
        if(UART_kbhit_A2_A1())
        {
           
            
            CMD = UART_Read_A2_A1();
            
             if(CMD == 'P')
             {     
                MiApp_FlushTx();
                MiApp_WriteData(PROJECTOR_OFF);
                MiApp_WriteData(myShortAddress.v[0]);
                MiApp_WriteData(myShortAddress.v[1]);
                MiApp_BroadcastPacket(false);
                LED1 = 1;
                delay_ms(200);
             }
            
        }
           
            
           /* if(CMD == 'P')
             {     
                MiApp_FlushTx();
                MiApp_WriteData(PROJECTOR_OFF);
                MiApp_WriteData(myShortAddress.v[0]);
                MiApp_WriteData(myShortAddress.v[1]);
                MiApp_BroadcastPacket(false);
                LED1 = 1;
             }*/
            
            
        

        
    }
}