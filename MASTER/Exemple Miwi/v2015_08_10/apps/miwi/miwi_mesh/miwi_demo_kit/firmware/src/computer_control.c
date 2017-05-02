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

void ComputerControl(void)
{

    /*  ATTENTIION
     * 
     *  LIRE LE FICHIER "Problème de UART" avant de continuer
     * 
     * 
     */

    RX_ANALOG_DIGITAL = 1;
    TX_ANALOG_DIGITAL = 1;
    LCD_BKLT = 1;
    char Title[] = "Hello World!";
    uint8_t CMD = 1;
    char buffer;
    uint8_t k = 0;
    uint8_t heures, minutes, secondes = 0;
    unsigned char uart_tableau[100] = {0};

    uint16_t i = 0;
    uint8_t Dizaine_Heures = 0;
    uint8_t Unite_Heures = 0;
    uint8_t Dizaine_Minutes = 0;
    uint8_t Unite_Minutes = 0;
    uint8_t Dizaine_Secondes = 0;
    uint8_t Unite_Secondes = 0;
    char tableau_temps[9] = {0, 0, 'h', 0, 0, 'm', 0, 0, 's'};
    uint8_t MacTemp[9];
    UART_Init_A2_A1();
    MiApp_DiscardMessage();

    while (true)
    {
        /* LED1 = 0;
         LED2 = 0;
         LED0 = 0;*/
        // Reception des messages de status MIWI //
        if(MiApp_MessageAvailable())
        {
            if(rxMessage.Payload[0] == POLL_PRESENCE) // Reception de l'adresse MAC d'un des modules
            {
                for(uint8_t i = 0; i < 8 ; i++)
                    MacTemp[i] = rxMessage.Payload[i+1];
                
                MacTemp[8] = 0;            
                UART_Write_Text_A2_A1(&MacTemp);            
                
            }
            
            
            
        }
        // Reception par le terminal et envoie des commandes MIWI //
        if (UART_kbhit_A2_A1())
        {

            buffer = UART_Read_A2_A1();         
            uart_tableau[k] = buffer;
            k++;
                        
            delay_ms(35); // Delay nécessaire pour prévenir du re-entrancy dans UART_kbhit. 
            if (uart_tableau[k - 1] == '\r')
            {
                uart_tableau[k - 1] = 0x00;
                k = 0;

                // Messages de commande//

                if (!strcmp("PROJECTOR OFF", &uart_tableau)) // Comparaison du msg dans le uart_tableau
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
                else if (!strcmp("PROJECTOR ON", &uart_tableau)) // Comparaison du msg dans le uart_tableau
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
                else if (!strcmp("PROJECTOR MOTOR DOWN", &uart_tableau)) // Comparaison du msg dans le uart_tableau
                {
                    MiApp_FlushTx();
                    MiApp_WriteData(PROJECTOR_MOTOR_DOWN);
                    MiApp_WriteData(myShortAddress.v[0]);
                    MiApp_WriteData(myShortAddress.v[1]);
                    MiApp_BroadcastPacket(false);
                    LED1 = 1;
                    delay_ms(50);
                    LED1 = 0;

                }

                else if (!strcmp("PROJECTOR MOTOR UP", &uart_tableau)) // Comparaison du msg dans le uart_tableau
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

                else if (!strcmp("ALARM ON", &uart_tableau)) // Comparaison du msg dans le uart_tableau
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

                else if (!strcmp("ALARM OFF", &uart_tableau)) // Comparaison du msg dans le uart_tableau
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
                else if (!strcmp("UNLOCK DOOR", &uart_tableau)) // Comparaison du msg dans le uart_tableau
                {
                    MiApp_FlushTx();
                    MiApp_WriteData(UNLOCK_PKT);
                    MiApp_WriteData(myShortAddress.v[0]);
                    MiApp_WriteData(myShortAddress.v[1]);
                    MiApp_BroadcastPacket(false);
                    LED1 = 1;
                    delay_ms(50);
                    LED1 = 0;

                } // Messages de status //

                else if (!strcmp("GET STATUS DOOR", &uart_tableau)) // Comparaison du msg dans le uart_tableau
                {
                    
                    
                    MiApp_FlushTx();
                    MiApp_WriteData(STATUS_PORTE); // STATUS_PORTE
                    MiApp_WriteData(myShortAddress.v[0]);
                    MiApp_WriteData(myShortAddress.v[1]);
                    MiApp_BroadcastPacket(false);
                    LED1 = 1;
                    LED1 = 0;
                     
                    i = 0;
                    while(!MiApp_MessageAvailable()){
                        delay_ms(1);
                        i++;
                        if(i == 2000){
                            UART_Write_Text_A2_A1("Erreur de communications");                          
                            break;
                        }
                            
                    };
                    if(i == 2000) break;
                    
                    
                     if (rxMessage.Payload[0] == DOOR_OPEN)
                    {
                        UART_Write_Text_A2_A1("Door is unlocked");

                    }

                    else if (rxMessage.Payload[0] == DOOR_CLOSED)
                    {
                        UART_Write_Text_A2_A1("Door is locked");

                    }

                    MiApp_DiscardMessage();

                }

                else if (!strcmp("GET STATUS SCREEN", &uart_tableau)) // Comparaison du msg dans le uart_tableau
                {
                    MiApp_FlushTx();
                    MiApp_WriteData(STATUS_SCREEN); //STATUS_SCREEN
                    MiApp_WriteData(myShortAddress.v[0]);
                    MiApp_WriteData(myShortAddress.v[1]);
                    MiApp_BroadcastPacket(false);
                    LED1 = 1;
                    LED1 = 0;
                   
                    

                    if (rxMessage.Payload[0] == SCREEN_UP)
                    {
                        UART_Write_Text_A2_A1("Projector screen is UP !");

                    }

                    if (rxMessage.Payload[0] == SCREEN_DOWN)
                    {
                        UART_Write_Text_A2_A1("Projector screen is DOWN !");

                    }

                    MiApp_DiscardMessage();


                }

                else if (!strcmp("GET STATUS PROJECTOR", &uart_tableau)) // Comparaison du msg dans le uart_tableau
                {
                    MiApp_FlushTx();
                    MiApp_WriteData(STATUS_PROJECTOR); //STATUS_PROJECTOR
                    MiApp_WriteData(myShortAddress.v[0]);
                    MiApp_WriteData(myShortAddress.v[1]);
                    MiApp_BroadcastPacket(false);
                    LED1 = 1;
                    LED1 = 0;

                    while(!MiApp_MessageAvailable());
                    
                    if (rxMessage.Payload[0] == SCREEN_UP)
                    {
                        UART_Write_Text_A2_A1("Projector screen is UP !");

                    }

                    if (rxMessage.Payload[0] == SCREEN_DOWN)
                    {
                        UART_Write_Text_A2_A1("Projector screen is DOWN !");

                    }

                    MiApp_DiscardMessage();

                }

                else if (!strcmp("GET LAST MOVEMENT", &uart_tableau)) // Comparaison du msg dans le uart_tableau
                {
                    MiApp_FlushTx();
                    MiApp_WriteData(GET_LAST_MOVEMENT); // STATUS_PORTE
                    MiApp_WriteData(myShortAddress.v[0]);
                    MiApp_WriteData(myShortAddress.v[1]);
                    MiApp_BroadcastPacket(false);                 
                    LED1 = 1;
                    LED1 = 0;
                    

                      while(!MiApp_MessageAvailable());
                    
                        if (rxMessage.Payload[0] == SEND_LAST_MOVEMENT)
                        {
                            heures = rxMessage.Payload[1];
                            minutes = rxMessage.Payload[2];
                            secondes = rxMessage.Payload[3];

                            Dizaine_Heures = ((heures) / 10) + 0x30; //calcul pour avoir les dizaines d'heure on /10
                            Unite_Heures = ((heures) % 10) + 0x30; //calcul pour avoir les unites d'heure on prend le reste de la /10
                            Dizaine_Minutes = ((minutes) / 10) + 0x30; //calcul pour avoir les dizaines de minutes on /10
                            Unite_Minutes = ((minutes) % 10) + 0x30; //calcul pour avoir les unites de minutes on prend le reste de la /10
                            Dizaine_Secondes = ((secondes) / 10) + 0x30; //calcul pour avoir les dizaines de secondes on /10
                            Unite_Secondes = ((secondes) % 10) + 0x30; //calcul pour avoir les unites de secondes on prend le reste de la /10

                            tableau_temps[0] = Dizaine_Heures;
                            tableau_temps[1] = Unite_Heures;
                            tableau_temps[3] = Dizaine_Minutes;
                            tableau_temps[4] = Unite_Minutes;
                            tableau_temps[6] = Dizaine_Secondes;
                            tableau_temps[7] = Unite_Secondes;

                            UART_Write_Text_A2_A1("Last movement :  ");
                            UART_Write_Text_A2_A1(&tableau_temps);


                        }
                        MiApp_DiscardMessage();
                  }
                    
                 memset(uart_tableau,'0',100);

                }
               
                

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




