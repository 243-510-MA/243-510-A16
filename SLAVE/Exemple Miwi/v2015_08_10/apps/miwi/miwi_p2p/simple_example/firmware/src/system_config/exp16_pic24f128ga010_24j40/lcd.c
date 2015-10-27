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
#ifndef __LCDBLCOKING_C
#define __LCDBLOCKING_C

#include "system.h"
#include <string.h>
#include <stdio.h>

//#define FOUR_BIT_MODE
#define SAMSUNG_S6A0032		// This LCD driver chip has a different means of entering 4-bit mode.  



// LCDText is a 32 byte shadow of the LCD text.  Write to it and 
// then call LCDUpdate() to copy the string into the LCD module.
uint8_t LCDText[16*2+1];

/******************************************************************************
 * Function:        static void LCD_Write(uint8_t RS,  uint8_t Data)
 *
 * PreCondition:    None
 *
 * Input:           RS - Register Select - 1:RAM, 0:Config registers
 *					Data - 8 bits of data to write
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Controls the Port I/O pins to cause an LCD write
 *
 * Note:            None
 *****************************************************************************/
void LCD_Write(uint8_t RS, uint8_t Data)
{
    #if defined(LCD_DATA_TRIS)
        LCD_DATA_TRIS = 0x00;
    #else
        LCD_DATA0_TRIS = 0;
        LCD_DATA1_TRIS = 0;
        LCD_DATA2_TRIS = 0;
        LCD_DATA3_TRIS = 0;
        #if !defined(FOUR_BIT_MODE)
            LCD_DATA4_TRIS = 0;
            LCD_DATA5_TRIS = 0;
            LCD_DATA6_TRIS = 0;
            LCD_DATA7_TRIS = 0;
        #endif
    #endif
    LCD_RS_TRIS = 0;
    LCD_RD_WR_TRIS = 0;
    LCD_RD_WR_IO = 0;
    LCD_RS_IO = RS;

#if defined(FOUR_BIT_MODE)
    #if defined(LCD_DATA_IO)
        LCD_DATA_IO = Data>>4;
    #else
        LCD_DATA0_IO = Data & 0x10;
        LCD_DATA1_IO = Data & 0x20;
        LCD_DATA2_IO = Data & 0x40;
        LCD_DATA3_IO = Data & 0x80;
    #endif
    Nop();					// Wait Data setup time (min 40ns)
    Nop();
    LCD_E_IO = 1;
    Nop();					// Wait E Pulse width time (min 230ns)
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    LCD_E_IO = 0;
#endif

    #if defined(LCD_DATA_IO)
        LCD_DATA_IO = Data;
    #else
        LCD_DATA0_IO = ((Data & 0x01) == 0x01);
        LCD_DATA1_IO = ((Data & 0x02) == 0x02);
        LCD_DATA2_IO = ((Data & 0x04) == 0x04);
        LCD_DATA3_IO = ((Data & 0x08) == 0x08);
        #if !defined(FOUR_BIT_MODE)
            LCD_DATA4_IO = ((Data & 0x10) == 0x10);
            LCD_DATA5_IO = ((Data & 0x20) == 0x20);
            LCD_DATA6_IO = ((Data & 0x40) == 0x40);
            LCD_DATA7_IO = ((Data & 0x80) == 0x80);
        #endif
    #endif
    Nop();					// Wait Data setup time (min 40ns)
    Nop();
    LCD_E_IO = 1;
    Nop();					// Wait E Pulse width time (min 230ns)
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    LCD_E_IO = 0;

    #if defined(LCD_DATA_TRIS)
        LCD_DATA_TRIS = 0xFF;
    #else
        LCD_DATA0_TRIS = 1;
        LCD_DATA1_TRIS = 1;
        LCD_DATA2_TRIS = 1;
        LCD_DATA3_TRIS = 1;
        #if !defined(FOUR_BIT_MODE)
            LCD_DATA4_TRIS = 1;
            LCD_DATA5_TRIS = 1;
            LCD_DATA6_TRIS = 1;
            LCD_DATA7_TRIS = 1;
        #endif
    #endif
    LCD_RS_TRIS = 1;
    LCD_RD_WR_TRIS = 1;
}


/******************************************************************************
 * Function:        void LCD_Initialize(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        LCDText[] is blanked, port I/O pin TRIS registers are
 *					configured, and the LCD is placed in the default state
 *
 * Note:            None
 *****************************************************************************/
void LCD_Initialize(void)
{
    uint8_t i;

    memset(LCDText, ' ', sizeof(LCDText)-1);
    LCDText[sizeof(LCDText)-1] = 0;

    // Setup the I/O pins
    LCD_E_IO = 0;
    LCD_RD_WR_IO = 0;


    #if defined(LCD_DATA_TRIS)
        LCD_DATA_TRIS = 0x00;
    #else
        LCD_DATA0_TRIS = 0;
        LCD_DATA1_TRIS = 0;
        LCD_DATA2_TRIS = 0;
        LCD_DATA3_TRIS = 0;
        #if !defined(FOUR_BIT_MODE)
            LCD_DATA4_TRIS = 0;
            LCD_DATA5_TRIS = 0;
            LCD_DATA6_TRIS = 0;
            LCD_DATA7_TRIS = 0;
        #endif
    #endif
    LCD_RD_WR_TRIS = 0;
    LCD_RS_TRIS = 0;
    LCD_E_TRIS = 0;


    // Wait the required time for the LCD to reset
    DELAY_ms(40);

    // Set the default function
    // Go to 8-bit mode first to reset the instruction state machine
    // This is done in a loop 3 times to absolutely ensure that we get
    // to 8-bit mode in case if the device was previously booted into
    // 4-bit mode and our PIC got reset in the middle of the LCD
    // receiving half (4-bits) of an 8-bit instruction
    LCD_RS_IO = 0;
    #if defined(LCD_DATA_IO)
        LCD_DATA_IO = 0x03;
    #else
        LCD_DATA0_IO = 1;
        LCD_DATA1_IO = 1;
        LCD_DATA2_IO = 0;
        LCD_DATA3_IO = 0;
        #if !defined(FOUR_BIT_MODE)
            LCD_DATA4_IO = 0;
            LCD_DATA5_IO = 0;
            LCD_DATA6_IO = 0;
            LCD_DATA7_IO = 0;
        #endif
    #endif
    Nop();					// Wait Data setup time (min 40ns)
    Nop();
    for(i = 0; i < 3; i++)
    {
        LCD_E_IO = 1;
                DELAY_10us(1);			// Wait E Pulse width time (min 230ns)
        LCD_E_IO = 0;
                DELAY_ms(2);
    }

    #if defined(FOUR_BIT_MODE)
        #if defined(SAMSUNG_S6A0032)
                // Enter 4-bit mode (requires only 4-bits on the S6A0032)
            #if defined(LCD_DATA_IO)
                    LCD_DATA_IO = 0x02;
            #else
                    LCD_DATA0_IO = 0;
                    LCD_DATA1_IO = 1;
                    LCD_DATA2_IO = 0;
                    LCD_DATA3_IO = 0;
            #endif
            Nop();					// Wait Data setup time (min 40ns)
            Nop();
            LCD_E_IO = 1;
                    Delay10us(1);			// Wait E Pulse width time (min 230ns)
            LCD_E_IO = 0;
        #else
            // Enter 4-bit mode with two lines (requires 8-bits on most LCD controllers)
            LCDWrite(0, 0x28);
        #endif
    #else
        // Use 8-bit mode with two lines
        LCD_Write(0, 0x38);
    #endif
    DELAY_10us(5);

    // Set the entry mode
    LCD_Write(0, 0x06);	// Increment after each write, do not shift
    DELAY_10us(5);

    // Set the display control
    LCD_Write(0, 0x0C);		// Turn display on, no cusor, no cursor blink
    DELAY_10us(5);

    // Clear the display
    LCD_Write(0, 0x01);
    DELAY_ms(2);

}


/******************************************************************************
 * Function:        void LCDUpdate(void)
 *
 * PreCondition:    LCD_Initialize() must have been called once
 *
 * Input:           LCDText[]
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Copies the contents of the local LCDText[] array into the
 *                  LCD's internal display buffer.  Null terminators in
 *                  LCDText[] terminate the current line, so strings may be
 *                  printed directly to LCDText[].
 *
 * Note:            None
 *****************************************************************************/
void LCD_Update(void)
{
    uint8_t i, j;

    // Go home
    LCD_Write(0, 0x02);
    DELAY_ms(2);

    // Output first line
    for(i = 0; i < 16u; i++)
    {
        // Erase the rest of the line if a null char is
        // encountered (good for printing strings directly)
        if(LCDText[i] == 0u)
        {
                for(j=i; j < 16u; j++)
                {
                        LCDText[j] = ' ';
                }
        }
        LCD_Write(1, LCDText[i]);
        DELAY_10us(5);
    }

    // Set the address to the second line
    LCD_Write(0, 0xC0);
    DELAY_10us(5);

    // Output second line
    for(i = 16; i < 32u; i++)
    {
        // Erase the rest of the line if a null char is
        // encountered (good for printing strings directly)
        if(LCDText[i] == 0u)
        {
                for(j=i; j < 32u; j++)
                {
                        LCDText[j] = ' ';
                }
        }
        LCD_Write(1, LCDText[i]);
        DELAY_10us(5);
    }
}

/******************************************************************************
 * Function:        void LCD_Erase(void)
 *
 * PreCondition:    LCD_Initialize() must have been called once
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Clears LCDText[] and the LCD's internal display buffer
 *
 * Note:            None
 *****************************************************************************/
void LCD_Erase(void)
{
    // Clear display
    LCD_Write(0, 0x01);
    DELAY_ms(2);

    // Clear local copy
    memset(LCDText, ' ', 32);
}


/******************************************************************************
 * Function:        void LCD_WriteLine(void)
 *
 * PreCondition:    LCDInit() must have been called once
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Clears LCDText[] and the LCD's internal display buffer
 *
 * Note:            None
 *****************************************************************************/
void LCD_WriteLine(uint16_t number, char *line)
{
    uint8_t i = 0;
    uint8_t j = 0;

    if(number == 2)
    {
        while ((LCDText[j] != 0) && (j < 16))
        {
            j++;
        }
    }

    do
    {
        LCDText[j++] = line[i++];
    }
    while ((LCDText[j-1] != 0) && (j < 31));

    LCD_Update();
}


/*********************************************************************
 * Function:        void LCDTRXCount(uint8_t txCount, uint8_t rxCount)
 *
 * PreCondition:    LCD has been initialized
 *
 * Input:           txCount - the total number of transmitted messages
 *                  rxCount - the total number of received messages
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function display the total numbers of TX and
 *                  RX messages on the LCD, if applicable.
 *
 * Note:            This routine is only effective if Explorer16 or
 *                  PIC18 Explorer demo boards are used
 ********************************************************************/
void LCDTRXCount(uint8_t txCount, uint8_t rxCount)
{
    LCD_Erase();
    sprintf((char *)LCDText, (const char*)"TX Messages: %d", txCount);
    sprintf((char *)&(LCDText[16]), (const char*)"RX Messages: %d", rxCount);
    LCD_Update();
    
}



/*********************************************************************
 * Function:        void LCDDisplay(char *text, uint8_t value, bool delay)
 *
 * PreCondition:    LCD has been initialized
 *
 * Input:           text - text message to be displayed on LCD
 *                  value - the text message allows up to one byte
 *                          of variable in the message
 *                  delay - whether need to display the message for
 *                          2 second without change
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function display the text message on the LCD,
 *                  including up to one BYTE variable, if applicable.
 *
 * Note:            This routine is only effective if Explorer16 or
 *                  PIC18 Explorer demo boards are used
 ********************************************************************/
void LCDDisplay(char *text, uint8_t value, bool delay)
{

    LCD_Erase();
    sprintf((char *)LCDText, (const char*)text, value);
    LCD_Update();

    // display the message for 2 seconds
    if( delay )
    {
        uint8_t i;
        for(i = 0; i < 8; i++)
        {

            DELAY_ms(250);

        }
    }
   
}


#endif
