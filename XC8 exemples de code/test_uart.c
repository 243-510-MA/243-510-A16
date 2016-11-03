/*
 * File:   test_uart.c
 * Programme de test des fonctions de l'UART matériel 
 * 
 * 
 * Auteur: cbarbaud
 * Processeur: PIC 18F2580
 * Compilateur: XC8
 * 
 * Source: https://electrosome.com/uart-pic-microcontroller-mplab-xc8/
 * 
 * Created on 28 septembre 2016, 14:49
 */

#define _XTAL_FREQ 20000000

#include <xc.h>

// BEGIN CONFIG
#pragma config OSC = HS // Oscillator Selection bits (HS oscillator)
#pragma config WDT = ON // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRT = OFF // Power-up Timer Enable bit (PWRT disabled)
#pragma config LVP = OFF // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
//END CONFIG

//----------------------------------------------------------------------
// prototypes des fonctions
//----------------------------------------------------------------------

char UART_Init(const long int baudrate);
void UART_Write(char data);
char UART_Data_Ready();
char UART_Read();
void UART_Write_Text(char *text);

//-----------------------------------------------------------------------
// main()
//------------------------------------------------------------------------
void main(void)
{
  char i, lecture;
  char texte[] = "Programme de test des fonctions de l'UART";
  int ctr;
  ctr = 0;
  UART_Init(9600);    
  for (i = 'A'; i <= 'Z'; i++) UART_Write(i);
  UART_Write(0x0A);       // Line Feed
  UART_Write(0x0D);       // Carriage Return
  UART_Write_Text("Bonjour Claude!");
  UART_Write(0x0A);       // Line Feed
  UART_Write(0x0D);       // Carriage Return
  UART_Write_Text(texte);
  UART_Write(0x0A);       // Line Feed
  UART_Write(0x0D);       // Carriage Return
  while(1)
  {
    UART_Write('-');
    for(i=0;i<50;i++)
    {
        __delay_ms(20); 
        if (UART_Data_Ready())
        {
            lecture = UART_Read();
            UART_Write(lecture);
        }
    
    }
    
  }

}
//----------------------------------------------------------------------------
// char UART_Init(const long int baudrate)
// Initialie UART reception = PIN_C7, transmission = PIN_C6
//----------------------------------------------------------------------------
char UART_Init(const long int baudrate)
{
  unsigned int x;
  x = (_XTAL_FREQ - baudrate*64)/(baudrate*64);   //SPBRG for Low Baud Rate
  
  if(x<256)
  {
    SPBRG = x;                                    //Writing SPBRG Register
    SYNC = 0;                                     //Setting Asynchronous Mode, ie UART
    SPEN = 1;                                     //Enables Serial Port
    TRISC7 = 1;                                   //As Prescribed in Datasheet
    TRISC6 = 0;                                   //As Prescribed in Datasheet
    CREN = 1;                                     //Enables Continuous Reception
    TXEN = 1;                                     //Enables Transmission
    return 1;                                     //Returns 1 to indicate Successful Completion
  }
  return 0;                                       //Returns 0 to indicate UART initialization failed
}

//---------------------------------------------------------------------------
// void UART_Write(char data)
//
// Écrit un caractère sur UART (PIN_C6))
//---------------------------------------------------------------------------

void UART_Write(char data)
{
  while(!TRMT);
  TXREG = data;
}

//---------------------------------------------------------------------------
// char UART_Data_Ready()
//
// Vérifie la réception d'un caractère sur PIN_C7
//---------------------------------------------------------------------------

char UART_Data_Ready()
{
  return RCIF;
}

//---------------------------------------------------------------------------
// char UART_Read()
// Attend la réception (arrêt) puis
// Reçoir un caractère sur PIN_C7
//---------------------------------------------------------------------------
char UART_Read()
{
  while(!RCIF);
  return RCREG;
}


//---------------------------------------------------------------------------
// void UART_Write_Text(char *text)
// Écriture d'un texte
// sur PIN_C6
//---------------------------------------------------------------------------
void UART_Write_Text(char *text)
{
  int i;
  for(i=0;text[i]!='\0';i++)
    UART_Write(text[i]);
}
