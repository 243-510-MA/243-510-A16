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

#include "system.h"
#include "lcd.h"
#include "delay.h"


#if defined(SENSOR_PORT_LCD)
    #if defined(SENSOR_PORT_UART)
        #define LCD_SLAVEADDRESS            0x3E
        #define LCD_WRITE_CMD               0x32
        #define LCD_INIT_CMD                0x81
        #define LCDCMD_FIRSTLINE_ADDRESS    0x00
        
        uint8_t LCDCommand[4];
        uint8_t CommandLen = 0;
        uint8_t TextLen = 0;
        uint8_t CommandPtr = 0;
        uint8_t TextPtr = 0;
    #else
        #include "spi.h"
    #endif
    
    // LCDText is a 32 byte shadow of the LCD text.  Write to it and 
    // then call LCDUpdate() to copy the string into the LCD module.
    uint8_t LCDText[16*2+1];

    #if defined(SENSOR_PORT_UART)
    /*********************************************************************
    * Function:         void I2CTask(void)
    *
    * PreCondition:     none
    *
    * Input:            none
    *
    * Output:           none
    *
    * Side Effects:	none
    *
    * Overview:         Used when I2C is used for writing to LCD
    *
    * Note:             None
    ********************************************************************/
        void I2CTask(void)
        {
            
            if( CommandPtr < CommandLen || TextPtr < TextLen )
            {
                while (SSP2CON2bits.SEN == 1);
                if( CommandPtr < CommandLen )
                {
                    SSP2BUF = LCDCommand[CommandPtr];
                    //if (SSP2CON2bits.ACKSTAT == 0)
                    {
                        CommandPtr++;
                    }    
                    if( CommandPtr == CommandLen && TextLen == 0 )
                    {
                        //DELAY_ms(300);
                        SSP2CON2bits.PEN = 1;
                        DELAY_ms(300);
                    }    
                } 
                else if( TextPtr < TextLen )
                {
                    if( LCDText[TextPtr] == 0 )
                    {
                        LCDText[TextPtr] = ' ';
                    }
                    SSP2BUF = LCDText[TextPtr];
    
                    //if (SSP2CON2bits.ACKSTAT == 0)
                    {
                        TextPtr++;
                    }  
                    if( TextPtr == TextLen )
                    {
                        //DELAY_ms(300);
                        SSP2CON2bits.PEN = 1;
                        DELAY_ms(300);
                    }     
                }
            } 
        }    


    /*********************************************************************
    * Function:         void I2CWrite(uint8_t Commandlen, uint8_t BufferLen)
    *
    * PreCondition:     none
    *
    * Input:            none
    *
    * Output:           none
    *
    * Side Effects:	none
    *
    * Overview:         Writes to I2C
    *
    * Note:             None
    ********************************************************************/
        void I2CWrite(uint8_t Commandlen, uint8_t BufferLen)
        {
            while(SSP2CON2bits.PEN == 1 );
            CommandPtr = 0;
            TextPtr = 0;
            CommandLen = Commandlen;
            TextLen = BufferLen;
            SSP2CON2bits.SEN = 1;
        }
    #endif
        
/*********************************************************************
* Function:         void LCD_Initialize(void)
*
* PreCondition:     none
*
* Input:	    none
*
* Output:	    none
*
* Side Effects:	    LCD is configured
*
* Overview:         Configure the LCD
*
* Note:             None
********************************************************************/
    void LCD_Initialize(void)
    {
        #if defined(SENSOR_PORT_UART)   
            TRISD |= 0x53;
            //PORTEbits.RDPU = 1;
            
            PIR3bits.SSP2IF = 0;	                    //Clear SSP IF
            SSP2STAT = 0x00;                //Enable Slewrate Control, Clearing flags
            //SSP2ADD = 0x19;					//Fclk = 100kHz
            SSP2ADD = 0xF0;
            SSP2CON1 = 0x28;                //Enable Master Mode
            SSP2CON2 = 0x00;
            PIE3bits.SSP2IE = 1;

            INTCONbits.GIEH = 1;
            INTCONbits.GIEL = 1;
    	
            DELAY_ms(300);
            
            LCDCommand[0] = LCD_SLAVEADDRESS << 1;
            LCDCommand[1] = LCD_INIT_CMD;
            LCDCommand[2] = 0x00;
            
            I2CWrite(3, 0);
            
            DELAY_ms(200);

            #if defined(ENABLE_MANUALBACKLIGHT)
                LCDCommand[0] = LCD_SLAVEADDRESS << 1;
                LCDCommand[1] = 0x51;
                LCDCommand[2] = 0x08;

                I2CWrite(3, 0);
                DELAY_ms(200);
            #endif
        #else
            TRISDbits.TRISD0 = 1;
            TRISDbits.TRISD1 = 1;
    
            TRISDbits.TRISD6 = 0;
            TRISDbits.TRISD2 = 1;
            TRISDbits.TRISD4 = 0;
            TRISCbits.TRISC6 = 0;

            SPISSPSTAT = 0x80;//0xC0;
            SPISSPCON = 0x22;//0x21;

            SPInCS_LAT = 0;
        #endif
    }    
/*********************************************************************
* Function:         void LCD_Erase(void)
*
* PreCondition:     none
*
* Input:	    none
*
* Output:	    none
*
* Side Effects:	    LCD display cleared
*
* Overview:         Clears LCD display
*
* Note:             None
********************************************************************/
    void LCD_Erase(void)
    {
        uint8_t i;
        
        for(i = 0; i < 32; i++)
        {
            LCDText[i] = ' ';
        }
    }
/*********************************************************************
* Function:         void LCD_Update(void)
*
* PreCondition:     none
*
* Input:	    none
*
* Output:	    none
*
* Side Effects:	    LCD is updated with message
*
* Overview:         LCD displays message
*
* Note:             None
********************************************************************/
    void LCD_Update(void)
    {
        #if defined(SENSOR_PORT_UART)   
            LCDCommand[0] = LCD_SLAVEADDRESS<<1;
            LCDCommand[1] = LCD_WRITE_CMD;
            LCDCommand[2] = LCDCMD_FIRSTLINE_ADDRESS;
            LCDCommand[3] = 32;
            
            I2CWrite(4, 32);
        #else
            uint8_t i;
        
            DELAY_ms(10);
            SPIPut2(0x32);                  //LCD Subsystem Write Data Command
            DELAY_ms(5);
            SPIPut2(0);                     //Screen Address
            DELAY_ms(5);
            SPIPut2(32);                    //Data length (16 + 3)
            for(i = 0; i < 32; i++)
            {
                DELAY_ms(5);
                if( LCDText[i] == 0 )
                {
                    SPIPut2(' ');
                }
                else
                {    
                    SPIPut2(LCDText[i]);
                }    
            }
            
            DELAY_ms(300);
        #endif
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
    #if defined(SENSOR_PORT_LCD)
        LCD_Erase();
        sprintf((char *)LCDText, (char*)"TX Messages: %3d", txCount);
        sprintf((char *)&(LCDText[16]), (char*)"RX Messages: %3d", rxCount);
        LCD_Update();
    #endif
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
    #if defined(SENSOR_PORT_LCD)
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
    #endif
}


void UserInterruptHandler(void)
{
    #if defined(SENSOR_PORT_UART) && defined(SENSOR_PORT_LCD)
        if( PIR3bits.SSP2IF )
        {
            PIR3bits.SSP2IF = 0;
            I2CTask();
            DELAY_ms(5);
        }
    #endif
}

#if defined(ENABLE_MANUALBACKLIGHT)

/*********************************************************************
* Function:         void LCDBacklightON(void)
*
* PreCondition:     LCD Initialization
*
* Input:            none
*
* Output:           none
*
* Side Effects:	LCD backlight is turned ON
*
* Overview:         LCD backlight is turned ON to highlight the dislpay
*                   message
*
* Note:             None
********************************************************************/
void LCDBacklightON(void)
{
 #if defined(SENSOR_PORT_UART)
    LCDCommand[0] = LCD_SLAVEADDRESS << 1;
    LCDCommand[1] = 0x44;
    LCDCommand[2] = 0x00;

    I2CWrite(3, 0);
    DELAY_ms(200);

    LCDCommand[0] = LCD_SLAVEADDRESS << 1;
    LCDCommand[1] = 0x42;
    LCDCommand[2] = 0x00;

    I2CWrite(3, 0);
    DELAY_ms(200);
#else
#endif
}
/*********************************************************************
* Function:         void LCDBacklightOFF(void)
*
* PreCondition:     LCD Initialization
*
* Input:            none
*
* Output:           none
*
* Side Effects:	LCD backlight is turned off
*
* Overview:         LCD backight is turned off to save power
*
* Note:             None
********************************************************************/
void LCDBacklightOFF(void)
{
#if defined(SENSOR_PORT_UART)
    LCDCommand[0] = LCD_SLAVEADDRESS << 1;
    LCDCommand[1] = 0x44;
    LCDCommand[2] = 0x00;

    I2CWrite(3, 0);
    DELAY_ms(200);
#else
#endif
}

#endif

#endif
