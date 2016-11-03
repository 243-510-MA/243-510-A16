/*
 * File:   test_int_rb0.c
 * Author: cbarbaud
 * Programme de test des interruptions
 * venant de la broche RB0
 * 
 * Created on 6 octobre 2016, 11:34
 */

#define _XTAL_FREQ 20000000

#include <xc.h>

// BEGIN CONFIG
#pragma config OSC = HS // Oscillator Selection bits (HS oscillator)
#pragma config WDT = ON // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRT = OFF // Power-up Timer Enable bit (PWRT disabled)
//#pragma config BORE = ON // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
//END CONFIG

//----------------------------------------------------------------------
// Prototype des fonctions
//----------------------------------------------------------------------
void cligne (void);

//----------------------------------------------------------------------
// main())
//----------------------------------------------------------------------
void main(void)
{
  int;
  
  TRISAbits.RA5 = 0;        // RA5 = sortie
  TRISBbits.RB0 = 1;        // RB0 = entrée
  ADCON1 = 0x0E;            // RB<4:0> entrées numériques
                            // Voir page 254 du Data Sheet
  INTCONbits.INT0E = 1;     // Autorise interruption RB0
  INTCON2bits.INTEDG0 = 0;  // Sur un front descendant
  INTCONbits.INT0F = 0;     // reset interrupt flag
  INTCONbits.GIEH = 1;      // Autorise interruptions Global
  RCONbits.IPEN = 1;        // Interrupt Priority Enable
  //ei();
  cligne();
  while(1)
  {
                            //  fait rien
   }

}

//----------------------------------------------------------------
// void interrupt rb0_int (void)
// Les fonctions d'interruptions sont toujours void ---(void))
//----------------------------------------------------------------
void interrupt high_priority rb0_int (void)
{
    if((INTCONbits.INT0F == 1) && (INTCONbits.INT0E == 1)) // vérifie que ça vient de RB0
    {
        cligne();
        cligne();
        INTCONbits.INT0F = 0;     // reset interrupt flag
    }
}


//----------------------------------------------------------------
// void cligne (void)
// cligne une fois
//----------------------------------------------------------------
void cligne (void)
{
    int i;
    PORTAbits.RA5 = 1;  // LED ON
    for(i=0;i<25;i++)__delay_ms(20); // 1/2 Second Delay
    PORTAbits.RA5 = 0;  // LED OFF
    for(i=0;i<25;i++)__delay_ms(20); // 1/2 Second Delay
}