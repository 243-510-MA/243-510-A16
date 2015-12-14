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
#include "delay.h"
#include "demo_output.h"
#include "driver/mrf_miwi/drv_mrf_miwi.h"


/*************************************************************************/
// the following two variable arrays are the data to be transmitted
// in this demo. They are bit map of English word "MiWi" and "DE"
// respectively.
/*************************************************************************/
const uint8_t MiWi[6][21] =
{
    {0x20,0xB2,0x20,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0xB2,0x20,0x20,0x20,0x20,0x20,0xB2,0x20,0xB2,0x0D,0x0A},
    {0xB2,0x20,0xB2,0x20,0xB2,0x20,0xB2,0x20,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0x20,0x0D,0x0A},
    {0xB2,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0xB2,0x20,0xB2,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0xB2,0x0D,0x0A},
    {0xB2,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0xB2,0x20,0x20,0xB2,0x20,0xB2,0x20,0xB2,0x20,0x20,0xB2,0x0D,0x0A},
    {0xB2,0x20,0x20,0xB2,0x20,0x20,0xB2,0x20,0xB2,0x20,0x20,0x20,0xB2,0x20,0xB2,0x20,0x20,0x20,0xB2,0x0D,0x0A},
    {0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0D,0x0A}
    
};   

const uint8_t DE[6][11] =
{
    {0xB2,0xB2,0xB2,0x20,0x20,0xB2,0xB2,0xB2,0xB2,0x0D,0x0A},
    {0xB2,0x20,0x20,0xB2,0x20,0xB2,0x20,0x20,0x20,0x0D,0x0A},
    {0xB2,0x20,0x20,0xB2,0x20,0xB2,0xB2,0xB2,0xB2,0x0D,0x0A},
    {0xB2,0x20,0x20,0xB2,0x20,0xB2,0x20,0x20,0x20,0x0D,0x0A},
    {0xB2,0xB2,0xB2,0x20,0x20,0xB2,0xB2,0xB2,0xB2,0x0D,0x0A},
    {0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0D,0x0A}
}; 


#define DEBOUNCE_TIME 0x00003FFF

bool PUSH_BUTTON_pressed;
MIWI_TICK PUSH_BUTTON_press_time;


void DemoOutput_Greeting(void)
{
    #if defined(MRF49XA)
        #if defined(PROTOCOL_P2P)
            LCDDisplay((char *)"Simple P2P Demo  MRF49XA Node 1", 0, true);
        #endif
        #if defined(PROTOCOL_MIWI)
            LCDDisplay((char *)"Simple MiWi Demo MRF49XA Node 1", 0, true);
        #endif
        #if defined(PROTOCOL_MIWI_PRO)
            LCDDisplay((char *)"Simple MiWi PRO  MRF49XA Node 1", 0, true);
        #endif
    #elif defined(MRF24J40)
        #if defined(PROTOCOL_P2P)
            LCDDisplay((char *)"Simple P2P Demo MRF24J40 Node 1", 0, true);
        #endif
        #if defined(PROTOCOL_MIWI)
            LCDDisplay((char *)"Simple MiWi DemoMRF24J40 Node 1", 0, true);
        #endif
        #if defined(PROTOCOL_MIWI_PRO)
            LCDDisplay((char *)"Simple MiWi PRO MRF24J40 Node 1", 0, true);
        #endif
    #elif defined(MRF24XA)
        #if defined(PROTOCOL_P2P)
            LCDDisplay((char *)"Simple P2P Demo MRF24XA Node 1", 0, true);
        #endif
        #if defined(PROTOCOL_MIWI)
            LCDDisplay((char *)"Simple MiWi DemoMRF24XA Node 1", 0, true);
        #endif
        #if defined(PROTOCOL_MIWI_PRO)
            LCDDisplay((char *)"Simple MiWi PRO MRF24XA Node 1", 0, true);
        #endif
    #elif defined(MRF89XA)
        #if defined(PROTOCOL_P2P)
            LCDDisplay((char *)"Simple P2P Demo  MRF89XA Node 1", 0, true);
        #endif
        #if defined(PROTOCOL_MIWI)
            LCDDisplay((char *)"Simple MiWi Demo MRF89XA Node 1", 0, true);
        #endif
        #if defined(PROTOCOL_MIWI_PRO)
            LCDDisplay((char *)"Simple MiWi PRO  MRF89XA Node 1", 0, true);
        #endif
    #endif

    #if defined(PROTOCOL_P2P)
        Printf("\r\nStarting Node 1 of Simple Demo for MiWi(TM) P2P Stack ...");  
    #endif
    #if defined(PROTOCOL_MIWI)
        Printf("\r\nStarting Node 1 of Simple Demo for MiWi(TM) Stack ...");  
    #endif 
    #if defined(PROTOCOL_MIWI_PRO)
        Printf("\r\nStarting Node 1 of Simple Demo for MiWi(TM) PRO Stack ...");  
    #endif 
    
    #if defined(PICDEMZ) 
        Printf("\r\nInput Configuration:");
        Printf("\r\n           Button 1: RB5");
        Printf("\r\n           Button 2: RB4");
        Printf("\r\nOutput Configuration:");
        Printf("\r\n                     RS232 port");
        Printf("\r\n              LED 1: RA0");
        Printf("\r\n              LED 2: RA1");
    #endif
    #if defined(PIC18_EXPLORER) 
        Printf("\r\nInput Configuration:");
        Printf("\r\n           Button 1: RB0");
        Printf("\r\n           Button 2: RA5");
        Printf("\r\nOutput Configuration:");
        Printf("\r\n                     RS232 port");
        Printf("\r\n                     USB port");
        Printf("\r\n              LED 1: D8");
        Printf("\r\n              LED 2: D7");
    #endif
    #if defined(EIGHT_BIT_WIRELESS_BOARD) 
        Printf("\r\nInput Configuration:");
        Printf("\r\n           Button 1: RB0");
        Printf("\r\n           Button 2: RB2");
        Printf("\r\nOutput Configuration:");
        Printf("\r\n              LED 1: RA2");
        Printf("\r\n              LED 2: RA3");
    #endif
    #if defined(EXPLORER16) 
        Printf("\r\nInput Configuration:");
        Printf("\r\n           Button 1: RD6");
        Printf("\r\n           Button 2: RD7");
        Printf("\r\nOutput Configuration:");
        Printf("\r\n                     RS232 port");
        Printf("\r\n              LED 1: D10");
        Printf("\r\n              LED 2: D9");
    #endif
    
    #if defined(MRF24J40)
    Printf("\r\n     RF Transceiver: MRF24J40");
    #elif defined(MRF24XA)
    Printf("\r\n     RF Transceiver: MRF24XA");
    #elif defined(MRF49XA)
    Printf("\r\n     RF Transceiver: MRF49XA");
    #elif defined(MRF89XA)
    Printf("\r\n     RF Transceiver: MRF89XA");
    #endif
    Printf("\r\n   Demo Instruction:");
    Printf("\r\n                     Power on the board until LED 1 lights up");
    Printf("\r\n                     to indicate connecting with peer. Push");
    Printf("\r\n                     Button 1 to broadcast message. Push Button");
    Printf("\r\n                     2 to unicast encrypted message. LED 2 will");
    Printf("\r\n                     be toggled upon receiving messages. ");
    Printf("\r\n\r\n");    
    
}        

void DemoOutput_Channel(uint8_t channel, uint8_t Step)
{
    if( Step == 0 )
    {
        LCDDisplay((char *)"Connecting Peer  on Channel %d ", channel, true);
        Printf("\r\nConnecting Peer on Channel ");
        CONSOLE_PrintDec(channel);
        Printf("\r\n");
    }
    else
    {    
        LCDDisplay((char *)" Connected Peer  on Channel %d", channel, true);
        Printf("\r\nConnected Peer on Channel ");
        CONSOLE_PrintDec(channel);
        Printf("\r\n");
    }
}    

void DemoOutput_Instruction(void)
{
    #if defined(EXPLORER16)
        LCDDisplay((char *)"RD6: Broadcast  RD7: Unicast", 0, false);
    #elif defined(PIC18_EXPLORER)
        LCDDisplay((char *)"RB0: Broadcast  RA5: Unicast", 0, false);
    #elif defined(EIGHT_BIT_WIRELESS_BOARD)
        LCDDisplay((char *)"RB0: Broadcast  RB2: Unicast", 0, false);
    #endif
}    


void DemoOutput_HandleMessage(void)
{
    uint8_t i;

    if( rxMessage.flags.bits.secEn )
    {
        CONSOLE_PutString((char *)"Secured ");
    }

    if( rxMessage.flags.bits.broadcast )
    {
        CONSOLE_PutString((char *)"Broadcast Packet with RSSI ");
    }
    else
    {
        CONSOLE_PutString((char *)"Unicast Packet with RSSI ");
    }
    CONSOLE_PrintHex(rxMessage.PacketRSSI);
    if( rxMessage.flags.bits.srcPrsnt )
    {
        CONSOLE_PutString((char *)" from ");
        if( rxMessage.flags.bits.altSrcAddr )
        {
            CONSOLE_PrintHex( rxMessage.SourceAddress[1]);
            CONSOLE_PrintHex( rxMessage.SourceAddress[0]);
        }
        else
        {    
            for(i = 0; i < MY_ADDRESS_LENGTH; i++)
            {
                CONSOLE_PrintHex(rxMessage.SourceAddress[MY_ADDRESS_LENGTH-1-i]);
            }    
        }
    }
    CONSOLE_PutString((char *)": ");
    
    for(i = 0; i < rxMessage.PayloadSize; i++)
    {
        CONSOLE_Put(rxMessage.Payload[i]);
    }       
}    

void DemoOutput_UpdateTxRx(uint8_t TxNum, uint8_t RxNum)
{
    LCDTRXCount(TxNum, RxNum);  
}    

void DemoOutput_ChannelError(uint8_t channel)
{
    Printf("\r\nSelection of channel ");
    CONSOLE_PrintDec(channel);
    Printf(" is not supported in current configuration.\r\n");
}    

void DemoOutput_UnicastFail(void)
{
    Printf("\r\nUnicast Failed\r\n");                  
    LCDDisplay((char *)" Unicast Failed", 0, true);
}    



/*********************************************************************
 * Function:        uint8_t ButtonPressed(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          Return value to indicate which button has been pressed.
 *                  Return 0 if no button pressed.
 *
 * Side Effects:
 *
 * Overview:        This function check if any button has been pressed
 *
 * Note:
 ********************************************************************/
uint8_t ButtonPressed(void)
{
    MIWI_TICK tickDifference;

    if(PUSH_BUTTON_1 == 0)
    {
        //if the button was previously not pressed
        if(PUSH_BUTTON_pressed == false)
        {
            PUSH_BUTTON_pressed = true;
            PUSH_BUTTON_press_time = MiWi_TickGet();
            return 1;
        }
    }
    else if(PUSH_BUTTON_2 == 0)
    {
        //if the button was previously not pressed
        if(PUSH_BUTTON_pressed == false)
        {
            PUSH_BUTTON_pressed = true;
            PUSH_BUTTON_press_time = MiWi_TickGet();
            return 2;
        }
    }
    else
    {
        //get the current time
        MIWI_TICK t = MiWi_TickGet();

        //if the button has been released long enough
        tickDifference.Val = MiWi_TickGetDiff(t,PUSH_BUTTON_press_time);

        //then we can mark it as not pressed
        if(tickDifference.Val > DEBOUNCE_TIME)
        {
            PUSH_BUTTON_pressed = false;
        }
    }

    return 0;
}





