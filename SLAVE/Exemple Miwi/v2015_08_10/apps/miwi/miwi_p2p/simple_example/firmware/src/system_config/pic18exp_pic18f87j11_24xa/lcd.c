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

#define __LCDBLOCKING_C

#include <string.h>
#include "system.h"



//#define FOUR_BIT_MODE
#define SAMSUNG_S6A0032		// This LCD driver chip has a different means of entering 4-bit mode.  





// LCDText is a 32 byte shadow of the LCD text.  Write to it and 
// then call LCDUpdate() to copy the string into the LCD module.
uint8_t LCDText[16*2+1];

void LCDInitSPI(void)
{
    TRISCbits.TRISC5 = 0;
    TRISCbits.TRISC3 = 0;
    PIR1bits.SSPIF = 0;
}

void LCDInitPortA(void)
{
    uint8_t preRFIE = RFIE;
    RFIE = 0;
    LCD_CS = 0;
    SPIPut(0x40);
    SPIPut(0x00);
    SPIPut(0x00);
    LCD_CS = 1;
    RFIE = preRFIE;
}

void LCDInitPortB(void)
{
    uint8_t preRFIE = RFIE;
    RFIE = 0;
    LCD_CS = 0;
    SPIPut(0x40);
    SPIPut(0x01);
    SPIPut(0x00);
    LCD_CS = 1;
    RFIE = preRFIE;
}

void LCDWritePortA(uint8_t v)
{
    uint8_t preRFIE = RFIE;
    RFIE = 0;
    LCD_CS = 0;
    SPIPut(0x40);
    SPIPut(0x12);
    SPIPut(v);
    LCD_CS = 1;
    RFIE = preRFIE;
}

void LCDWritePortB(uint8_t v)
{
    uint8_t preRFIE = RFIE;
    RFIE = 0;
    LCD_CS = 0;
    SPIPut(0x40);
    SPIPut(0x13);
    SPIPut(v);
    LCD_CS = 1;
    RFIE = preRFIE;
}

void LCDInitWrite(uint8_t v)
{
    LCDWritePortA(0);
    LCDWritePortB(v);
    Nop();
    Nop();
    Nop();

    LCDWritePortA(0x40);
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    LCDWritePortA(0);
}

void LCD_i_write(uint8_t v)
{
    LCDWritePortA(0);
    DELAY_ms(1);
    LCDWritePortB(v);
    Nop();
    Nop();
    Nop();
    Nop();
    LCDWritePortA(0x40);
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    LCDWritePortA(0x00);
    return;
}

void LCD_d_write(uint8_t v)
{
    DELAY_ms(1);
    LCDWritePortA(0x80);
    LCDWritePortB(v);
    Nop();
    Nop();
    Nop();
    Nop();
    LCDWritePortA(0xC0);
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    Nop();
    LCDWritePortA(0);

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

    LCD_CS_TRIS = 0;
    LCD_CS = 1;
    DELAY_ms(10);

    LCD_RESET_TRIS = 0;
    LCD_RESET = 0;
    DELAY_ms(10);
    LCD_RESET = 1;

    // init SPI
    LCDInitSPI();

    // init MCP923S17 Port
    LCDInitPortA();

    // step B
    LCDInitPortB();

    LCDWritePortA(0);

    // Function Set
    DELAY_ms(10);
    LCDInitWrite(0x3C);

    DELAY_ms(10);
    LCDInitWrite(0x0C);    // display off

    DELAY_ms(10);
    LCDInitWrite(0x01);    // display clear

    DELAY_ms(10);
    LCDInitWrite(0x0C);    // Entry mode

}


/******************************************************************************
 * Function:        void LCDUpdate(void)
 *
 * PreCondition:    LCDInit() must have been called once
 *
 * Input:           LCDText[]
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Copies the contents of the local LCDText[] array into the
 *					LCD's internal display buffer.  Null terminators in
 *					LCDText[] terminate the current line, so strings may be
 *					printed directly to LCDText[].
 *
 * Note:            None
 *****************************************************************************/
void LCD_Update(void)
{
    uint8_t i;

    LCD_i_write(0x80);
    for(i = 0; i < 16; i++)
    {
        if( LCDText[i] == 0 )
        {
            LCD_d_write(' ');
        }
        else
        {
            LCD_d_write(LCDText[i]);
        }
    }
    LCD_i_write(0xC0);
    for(i = 16; i < 32; i++)
    {
        if( LCDText[i] == 0 )
        {
            LCD_d_write(' ');
        }
        else
        {
            LCD_d_write(LCDText[i]);
        }
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
    LCDInitWrite(0x01);
    DELAY_ms(2);

    // Clear local copy
    memset(LCDText, ' ', 32);
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
        sprintf((char *)LCDText, (char*)"TX Messages: %3d", txCount);
        sprintf((char *)&(LCDText[16]), (char*)"RX Messages: %3d", rxCount);
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
     *                  including up to one uint8_t variable, if applicable.
     *
     * Note:            This routine is only effective if Explorer16 or
     *                  PIC18 Explorer demo boards are used
     ********************************************************************/
    void LCDDisplay(char *text, uint8_t value, bool delay)
    {

        LCD_Erase();
        sprintf((char *)LCDText, (char*)text, value);
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

