/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */


#ifndef _SOFT_UART_H
#define	_SOFT_UART_H

#include <xc.h> // include processor files - each processor file is guarded.  

/* Prototypes pour fonctions pour UART Matériel et Logiciel par Claude Barbaud*/

#define _XTAL_FREQ 16000000
//----------------------------------------------------------------------
// prototypes des fonctions
//----------------------------------------------------------------------
// ------------------UART Matériel-------------------------------------
char UART_Init(const long int baudrate);
void UART_Write(char data);
char UART_Data_Ready();
char UART_Read();
void UART_Write_Text(char *text);
// ------------------UART Logiciel-------------------------------------
void UART_Init_A2_A1(void);
void UART_Write_A2_A1(char data);
char UART_Read_A2_A1(void);
short UART_kbhit_A2_A1(void);
void UART_Write_Text_A2_A1(char *text);
//-------------------LCD------------------------------------------------
void LCD_Init(void);
void LCD_Set_Cursor(int pos);


#endif	/* XC_HEADER_TEMPLATE_H */

