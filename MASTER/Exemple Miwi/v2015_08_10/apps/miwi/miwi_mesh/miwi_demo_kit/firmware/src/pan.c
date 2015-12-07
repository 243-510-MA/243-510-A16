//PAN

#include "pan.h"
#include "system.h"
#include "codes library.h"
#include "system_config.h"
#include "miwi/miwi_api.h"

void Pan(void)
{
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
	putcv(0x02);putcv(0x01);putcv(0x00);putcv(0x00);putcv(0x00);putcv(0x03);
	LCD_BKLT = 1;
}

void Power_on() //POWER ON PROJECTEUR
{
	LCD_BKLT = 1;
	putcv(0x02);putcv(0x00);putcv(0x00);putcv(0x00);putcv(0x00);putcv(0x02);
	LCD_BKLT = 1;
}

void alarm(status) //Alarm is activated/desactivated     status=1=on    status=0=off
{
    GYRO = status;//Activate/desactive le Gyrophare
    PIE3bits.TMR4IE = status;//Activate/desactive le Buzzer(Interrupt in drv_mrf_miwi_24j40.c)
}
