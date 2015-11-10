#include <18F2580.h>

#fuses HS, NOWDT, PUT,NOPROTECT, NOLVP
#use delay(clock=20000000)
#use RS232(Baud=9600, Xmit=PIN_C6,Rcv=PIN_C7, bits=8, STOP=1)

//-----------------------------------------------------//

int i;

void cligne()
{
	for(i=0;i<5;i++)
	{
		output_toggle(PIN_A5);
		delay_ms(100);
	}
	output_low(PIN_A5);
	i=0;
}

void Power_off() //Utiliser un 74LS04 sur TX pour inverser le tout.
{
	putc(0x00);putc(0xBF);putc(0x00);putc(0x00);putc(0x01);putc(0x00);putc(0xC0);
	delay_ms(60);
	putc(0x00);putc(0xBF);putc(0x00);putc(0x00);putc(0x01);putc(0x02);putc(0xC2);
	delay_ms(60);
	putc(0x02);putc(0x01);putc(0x00);putc(0x00);putc(0x00);putc(0x03);
}

void Power_on() //Utiliser un 74LS04 sur TX pour inverser le tout.
{
	putc(0x00);putc(0xBF);putc(0x00);putc(0x00);putc(0x01);putc(0x00);putc(0xC0);
	delay_ms(60);
	putc(0x00);putc(0xBF);putc(0x00);putc(0x00);putc(0x01);putc(0x02);putc(0xC2);
	delay_ms(60);
	putc(0x02);putc(0x00);putc(0x00);putc(0x00);putc(0x00);putc(0x02);
}


//------------------------------------------------------//
void main (void)
{
	cligne();
	while(true)
	{
		if(!input(PIN_B0))
		{
			Power_on();
			delay_ms(1000);
		}

		if(input(PIN_B1))
		{
			Power_off();
			delay_ms(1000);
		}
	}
}
