/*
 * File:   soft_uart.c
 * Author: cbarbaud
 *
 * Created on 3 octobre 2016, 15:32
 */

/**/
/* Les prototypes de ces fonctions ont été déplacés dans soft_uart.h*/
/* Code pour UART Matériel et Logiciel par Claude Barbaud */
/* Modifications pour le cours 243-510 , Automne 2016, pour projet MiWi */

/* LISTE DES MODIFICATIONS
 * 2016-11-03 : modification par S.Proulx pour changer la broche du UART Logiciel
 *              des broches B6 B7 aux broches A2 et A1. Fonctionnement de base
 * 
 * 
 *
 * 
 *  */

#include "system_config.h"
#include "soft_uart.h"


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
    TRISC6 = 1;                                   //As Prescribed in Datasheet
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

//---------------------------------------------------------------------------
// void UART_Init_B6_B7(void)
//  PIN_A2 = TXD
//  PIN_A1 = RXD
//---------------------------------------------------------------------------
void UART_Init_A2_A1(void)
{
    UART_TX_TRIS = 0; // Sortie
    UART_RX_TRIS = 1; // Entrée
    UART_TX_V = 1;     // Niveau haut
    UART_RX_V = 1;
}
//---------------------------------------------------------------------------
//  void UART_Write_A2_A1(char data)
//  Écrit le caractère sur le port série software
//  PIN_A2 = TXD (ORANGE))
//  PIN_A1 = RXD (ROUGE)
//  durée d'un bit à 9600 bps
//  t = 1/9600 = 104 us
//  4 us environ pour instructions
//---------------------------------------------------------------------------

void UART_Write_A2_A1(char data)
{
    int i;
    UART_TX_V = 0;        // start bit
    __delay_us(100);
    for(i=0; i<8; i++)
    {
        UART_TX_V = (short) data;
        data = data >> 1;
        __delay_us(100);
    }
    UART_TX_V = 1;    // stop bit
    __delay_us(100);
}

//---------------------------------------------------------------------------
//  char UART_Read_A2_A1(void)
//  Reçoit le caractère sur le port série software
//  PIN_A2 = TXD
//  PIN_A1 = RXD
//  durée d'un bit à 9600 bps
//  t = 1/9600 = 104 us
//  4 us environ pour instructions
//---------------------------------------------------------------------------

char UART_Read_A2_A1(void)
{
    int i;
    char data, lecture;
    data = 0b00000000;
    if (UART_RX_V == 0)           // Détecte le start bit
    {
        __delay_us(50);         // demi-bit
        for(i=0; i<8; i++)
        {
            __delay_us(100);    // Échantillone au milieu des bits
            UART_TX_V = !UART_TX_V;
            data = (data >> 1) + (UART_RX_V << 7);  
        }
        UART_RX_V = 1;                // stop bit (en attente)
    }
    return data;
 }

//---------------------------------------------------------------------------
//  short UART_kbhit_A2_A1(void)
//  Teste la réception d'un caractère su PIN_B7
//---------------------------------------------------------------------------
short UART_kbhit_A2_A1(void)
{   
    return (UART_RX_V == 0);
}

//---------------------------------------------------------------------------
// void UART_Write_Text_A2_A1(char *text)
// Écriture d'un texte
// sur PIN_A2
//---------------------------------------------------------------------------
void UART_Write_Text_A2_A1(char *text)
{
  int i;
  for(i=0;text[i]!='\0';i++)
    UART_Write_A2_A1(text[i]);
}
//---------------------------------------------------------------------------
//  void LCD_Init(void)
//  
//---------------------------------------------------------------------------

void LCD_Init(void)
{
    UART_Write_A2_A1(0xFE);
    UART_Write_A2_A1(0x51);     // clear screen
    UART_Write_A2_A1(0xFE);
    UART_Write_A2_A1(0x41);     // display on
    
}

//---------------------------------------------------------------------------
//  void LCD_Set_Cursor(int pos)
//  
//---------------------------------------------------------------------------
void LCD_Set_Cursor(int pos)
{
    UART_Write_A2_A1(0xFE);
    UART_Write_A2_A1(0x45);     // clear screen
    UART_Write_A2_A1(pos);
        
}

// BEGIN CONFIG
/*#pragma config OSC = HS // Oscillator Selection bits (HS oscillator)
#pragma config WDT = ON // Watchdog Timer Enable bit (WDT enabled)
#pragma config PWRT = OFF // Power-up Timer Enable bit (PWRT disabled)
#pragma config LVP = OFF // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)*/
//END CONFIG


//-----------------------------------------------------------------------
// main()
//------------------------------------------------------------------------
/*void main(void)
{
  char i, lecture;
  char texte[] = "Test UART logiciel";

  UART_Init_B6_B7();
  LCD_Init();
  UART_Write_Text_B6_B7(texte);
  LCD_Set_Cursor(0x40);
  //for(;;) UART_Write_A2_A1('U');                            // Pour mesurer les bps
  //for (i = 'A'; i <= 'Z'; i++) UART_Write_A2_A1(i);
  while(1)
  {
      if (UART_kbhit_B6_B7())
      {
        lecture = UART_Read_B6_B7();
        if (lecture != 0) 
        {
          UART_Write_A2_A1(lecture);
        }
      }
  }
  }*/

//----------------------------------------------------------------------------
// char UART_Init(const long int baudrate)
// Initialie UART reception = PIN_C7, transmission = PIN_C6
//----------------------------------------------------------------------------
