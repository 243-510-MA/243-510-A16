/********************************************************************
 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the "Company") for its PIC(R) Microcontroller is intended and
 supplied to you, the Company's customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *******************************************************************/

#include "system.h"
#include "system_config.h"
#include "self_test.h"

#include "miwi/miwi_api.h"

#define EEPROM_MAC_ADDR 0xFA

uint8_t switch_val;
uint8_t EEPROM_Val[3];
uint8_t SST_Val;
uint8_t init_case = 0x00;


void SelfTest(uint8_t myChannel)
{

    LCD_Display((char *)"   MiWi  Demo    Self Test Mode ", 0, true);
    LCD_Display((char *)"SW1: Start Test SW2: Exit Test", 0, true);
    init_case = 0;
    while(1)
    {
    switch_val = BUTTON_Pressed();
    if(switch_val == SW1)
        {
            //SW1 and SW2 Push Buttons verified
            //LCD Display and Backlight verified (MiWi Card Demo Display)
            while(1)
            {
                LCD_Display((char *)"Testing ...", 0, true);
                LED0 = LED1 = LED2 = 0;
                switch(init_case)
                {
                    case 0: //Test  MAC EEPROM 
                          
                            EEPROMRead(&(EEPROM_Val[0]), EEPROM_MAC_ADDR, 3);
                            if((EEPROM_Val[0] == 0x00) && (EEPROM_Val[1] == 0x00) && (EEPROM_Val[2] == 0x00))
                                init_case = 255;
                            else if((EEPROM_Val[0] == 0xFF) && (EEPROM_Val[1] == 0xFF) && (EEPROM_Val[2] == 0xFF))
                                init_case = 255;
                            else init_case++;
                            break;
                    case 1: //Test SST Flash
                         
                            SSTGetID(&SST_Val);
                            if(SST_Val == 0xBF)
                                init_case++;
                            else init_case = 255;
                            break;
                    case 2: //Test Radio
                         
                            if (MiApp_SetChannel(myChannel) == false)
                            {
                                init_case = 254;
                                break;
                            }
                            //Scan for networks (Testing TX and RX functionality
                            {
                               
                                bool Tx_Success = false;
                                uint8_t Tx_Trials = 0, scanresult = 0;
                                
                                while(!Tx_Success)
                                {
                                    if(myChannel < 8)
                                        scanresult = MiApp_SearchConnection(10, (0x00000001 << myChannel));
                                    else if(myChannel < 16)
                    				    scanresult = MiApp_SearchConnection(10, (0x00000100 << (myChannel-8)));
                                    else if(myChannel < 24)
                    				    scanresult = MiApp_SearchConnection(10, (0x00010000 << (myChannel-16)));
                                    else 
                    				    scanresult = MiApp_SearchConnection(10, (0x01000000 << (myChannel-24)));
                                    if(scanresult == 0)
                                    {
                                        Tx_Trials++;
                                        if(Tx_Trials > 2) break;
  
                                    }
                                    else Tx_Success = true;
                                   
                                }
                                if(Tx_Success) init_case = 254;
                                else init_case = 255;
                            }
                            break;
                   
                    case 254:   LCD_Display((char *)"Self Test Mode  Test Pass", 0, true);
                                LED0 = LED1= LED2 = 1;
                                init_case = 0xF0;
                                break;
                       case 255:   LCD_Display((char *)"Self Test Mode  Test Fail", 0, true);
                                LED0 = LED1 = LED2 = 1;
                                init_case = 0xF0;
                                break;
                    default:    break;
    
                }
                
            if(init_case == 0xF0) 
            {
                while(1)
                {
                    switch_val = BUTTON_Pressed();
                    if((switch_val == SW1) || (switch_val == SW2))
                        break;
                }
                break;
            }                       
            }
            break;

        }
    else if(switch_val == SW2)
        {
            break;
        }
    }
}
