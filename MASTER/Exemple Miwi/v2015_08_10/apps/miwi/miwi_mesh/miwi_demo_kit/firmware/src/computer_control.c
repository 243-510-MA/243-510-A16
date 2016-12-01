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
#include "string.h"

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
    
    RX_ANALOG_DIGITAL = 1 ;
    TX_ANALOG_DIGITAL = 1 ; 
    LCD_BKLT = 1;
    char Title[]= "Hello World!";
    uint8_t CMD = 1 ;
    char buffer;
    uint8_t k=0;
    unsigned char uart_tableau[100]={0};
    unsigned char projector_off[14] ="PROJECTOR OFF";
    
    UART_Init_A2_A1();
    
    while(true)
    {
       /* LED1 = 0;
        LED2 = 0;
        LED0 = 0;*/
      
     
        
        if(UART_kbhit_A2_A1())
        {
            
            buffer = UART_Read_A2_A1();
            uart_tableau[k] = buffer;
            k++;
            delay_ms(10); // Delay nécessaire pour prévenir du re-entrancy dans UART_kbhit.
            if(uart_tableau[k-1] == '\r')
            {
                uart_tableau[k-1] = 0x00;
                k = 0;
                
                                // Messages de commande//
                
             if(!strcmp("PROJECTOR OFF",&uart_tableau)) // Comparaison du msg dans le uart_tableau
             {     
                MiApp_FlushTx();
                MiApp_WriteData(PROJECTOR_OFF);
                MiApp_WriteData(myShortAddress.v[0]);
                MiApp_WriteData(myShortAddress.v[1]);
                MiApp_BroadcastPacket(false);
                LED1 = 1;
                delay_ms(50);
                LED1 = 0;
                    
             }
                
             else if(!strcmp("PROJECTOR ON",&uart_tableau)) // Comparaison du msg dans le uart_tableau
             {     
                MiApp_FlushTx();
                MiApp_WriteData(PROJECTOR_ON);
                MiApp_WriteData(myShortAddress.v[0]);
                MiApp_WriteData(myShortAddress.v[1]);
                MiApp_BroadcastPacket(false);
                LED1 = 1;
                delay_ms(50);
                LED1 = 0;
                    
             }
              else if(!strcmp("PROJECTOR MOTOR DOWN",&uart_tableau)) // Comparaison du msg dans le uart_tableau
             {     
                MiApp_FlushTx();
                MiApp_WriteData(PROJECTOR_MOTOR_DOWN);
                MiApp_WriteData(myShortAddress.v[0]);
                MiApp_WriteData(myShortAddress.v[1]);
                MiApp_BroadcastPacket(false);
                LED1 = 1;
                delay_ms(200);
                LED1 = 0;
                    
             }   
                
               else if(!strcmp("PROJECTOR MOTOR UP",&uart_tableau)) // Comparaison du msg dans le uart_tableau
             {     
                MiApp_FlushTx();
                MiApp_WriteData(PROJECTOR_MOTOR_UP);
                MiApp_WriteData(myShortAddress.v[0]);
                MiApp_WriteData(myShortAddress.v[1]);
                MiApp_BroadcastPacket(false);
                LED1 = 1;
                delay_ms(50);
                LED1 = 0;
                    
             }  
                
               else if(!strcmp("ALARM ON",&uart_tableau)) // Comparaison du msg dans le uart_tableau
             {     
                MiApp_FlushTx();
                MiApp_WriteData(ALARM_ON);
                MiApp_WriteData(myShortAddress.v[0]);
                MiApp_WriteData(myShortAddress.v[1]);
                MiApp_BroadcastPacket(false);
                LED1 = 1;
                delay_ms(50);
                LED1 = 0;
                    
             }   
                
               else if(!strcmp("ALARM OFF",&uart_tableau)) // Comparaison du msg dans le uart_tableau
             {     
                MiApp_FlushTx();
                MiApp_WriteData(ALARM_OFF);
                MiApp_WriteData(myShortAddress.v[0]);
                MiApp_WriteData(myShortAddress.v[1]);
                MiApp_BroadcastPacket(false);
                LED1 = 1;
                delay_ms(50);
                LED1 = 0;
                    
             }
                
             
                
                                // Messages de status //
                
               else if(!strcmp("GET STATUS DOOR",&uart_tableau)) // Comparaison du msg dans le uart_tableau
             {     
                MiApp_FlushTx();
                MiApp_WriteData(0x02); // STATUS_PORTE
                MiApp_WriteData(myShortAddress.v[0]);
                MiApp_WriteData(myShortAddress.v[1]);
                MiApp_BroadcastPacket(false);
                LED1 = 1;
                delay_ms(50);
                LED1 = 0;
                    
             }   
                
                else if(!strcmp("GET STATUS SCREEN",&uart_tableau)) // Comparaison du msg dans le uart_tableau
             {     
                MiApp_FlushTx();
                MiApp_WriteData(0x07); //STATUS_SCREEN
                MiApp_WriteData(myShortAddress.v[0]);
                MiApp_WriteData(myShortAddress.v[1]);
                MiApp_BroadcastPacket(false);
                LED1 = 1;
                delay_ms(50);
                LED1 = 0;
                    
             }   
                
                else if(!strcmp("GET STATUS PROJECTOR",&uart_tableau)) // Comparaison du msg dans le uart_tableau
             {     
                MiApp_FlushTx();
                MiApp_WriteData(0x0A);//STATUS_PROJECTOR
                MiApp_WriteData(myShortAddress.v[0]);
                MiApp_WriteData(myShortAddress.v[1]);
                MiApp_BroadcastPacket(false);
                LED1 = 1;
                delay_ms(50);
                LED1 = 0;
                    
             }   
             
            }  
            
        }
           
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
            
            
        

        
 
