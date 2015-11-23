//-----------------------------------------------------------
//  CCS C                        Name  Francois     Date     //
//                                                           //
/*                      clignote.C                           */
//
//                  LED blinks 8 times                       //
//                                                           //
//   27/08/2014                                              //
//-----------------------------------------------------------//
#include <18F2580.H>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use RS232(baud=9600,Xmit=pin_c6,Rcv=pin_c7,bits=8,errors)

#BYTE port_b = 0xF81
#bit txPin = port_b.0
#define bouton PIN_A0

long BAUD = 9600;
short parity = 0;
short pairImpair = 0;  //0=Pair 1=Impair


short parityBit = 0;
int parityResult = 0;
int data = 0;

void sendData(data){
	int i;
	for(i=0;i<8;i++){
	txPin = data;
	delay_us(104);
	parityResult += data;
	data = data>>1;
	}
}

void startBit(void){
txPin = 0;
delay_us(BAUD);
}

void stopBit(void){
txPin = 1;
delay_us(BAUD);
}

void parityBit_f(void){
txPin = parityBit;
delay_us(BAUD);
}

void calculParity(void){
parityResult %= 2;
if(parity == 1){
	if(pairImpair == 0){      
		if(parityResult == 0){
		parityBit = 0;
		}
		else{
		parityBit = 1;
		}
	}
	else if(pairImpair == 1){
		if(parityResult == 0){
		parityBit = 1;
		}
		else{
		parityBit = 0;
		}

	}
}
}

void main(void)
{
	while(true)
	{
		set_tris_b(0);
		txPin = 1;
		BAUD = (1000000/BAUD);
		int e = 0;

		do{}while(!kbhit());
		{
			while(1)
			{

				startBit();
				sendData((0x30+e));
				if(parity == 1)
				{
					calculParity();
					parityBit_f();
				}
				stopBit();
				e++;
				if(e==10)
				{
					e = 0;
				}
				txPin = 1;
				delay_ms(100);

			}
		}
	}
}