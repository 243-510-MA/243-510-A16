//PAN

#include "pan.h"
#include "system.h"
#include "codes library.h"
#include "system_config.h"
#include "miwi/miwi_api.h"

void Pan(void)
{
    TRISB = 0x0;
    int status = 0;
    uint8_t rssi = 0;
    GYRO = 0;
    Buzzer = 0;
    
 /*  
    RCONbits.IPEN = 1;
    T4CON = 0b00111111;
    PR4 = 156;
    IPR3bits.TMR4IP = 0;
    INTCONbits.GIEH = 0;
    PIR3bits.TMR4IF = 0;
    
*/
    
    LCD_BKLT = 1;
    while(true)
    {
        if(MiApp_MessageAvailable())
        {
            if(rxMessage.Payload[0] == PROJECTOR_ON)
            {
                Power_on();
            }
            
            else if(rxMessage.Payload[0] == PROJECTOR_OFF)
            {
                Power_off();
            }
            
            else if(rxMessage.Payload[0] == ALARM_OFF)
            {
                alarm(0);
            }
            
            else if(rxMessage.Payload[0] == ALARM_ON)
            {
                alarm(1);
            }
        }
        MiApp_DiscardMessage();
    }
}

void startBit(void)
{
	LCD_BKLT = 0;
	__delay_us(102);
}

void stopBit(void)
{
	LCD_BKLT = 1;
	__delay_us(102);
}

void putcv(int data)	//UART VIRTUEL
{
	int i,x;
	startBit();
	int data2 = data;      // = ~data
	for(i=0;i<8;i++)
    {
		LCD_BKLT = data2;
		data2 = data2>>1;
		__delay_us(102);	
	}
	stopBit();
}

void Power_off() //POWER OFF PROJECTEUR
{
    LCD_BKLT = 1;
	putcv(0x00);putcv(0xBF);putcv(0x00);putcv(0x00);putcv(0x01);putcv(0x00);putcv(0xC0);
	LCD_BKLT = 1;
    delay_ms(19);
	putcv(0x00);putcv(0xBF);putcv(0x00);putcv(0x00);putcv(0x01);putcv(0x02);putcv(0xC2);
	LCD_BKLT = 1;
    delay_ms(19);
	putcv(0x02);putcv(0x01);putcv(0x00);putcv(0x00);putcv(0x00);putcv(0x03);
	LCD_BKLT = 1;
    delay_ms(19);
}

void Power_on() //POWER ON PROJECTEUR
{	
	LCD_BKLT = 1;
	putcv(0x00);putcv(0xBF);putcv(0x00);putcv(0x00);putcv(0x01);putcv(0x00);putcv(0xC0);
	LCD_BKLT = 1;
    delay_ms(35);
    putcv(0x00);putcv(0xBF);putcv(0x00);putcv(0x00);putcv(0x01);putcv(0x02);putcv(0xC2);
	LCD_BKLT = 1;
    delay_ms(19);
	putcv(0x02);putcv(0x00);putcv(0x00);putcv(0x00);putcv(0x00);putcv(0x02);
    LCD_BKLT = 1;
    delay_ms(19);
}

void alarm(int status) //Alarm is activated/desactivated     status=1=on    status=0=off
{
    GYRO = status; //status;  //Activate/desactive le Gyrophare
    Buzzer = status; //status;
}