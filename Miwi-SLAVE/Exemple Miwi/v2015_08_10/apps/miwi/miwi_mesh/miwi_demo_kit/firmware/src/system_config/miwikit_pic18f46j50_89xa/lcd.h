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
#ifndef __LCD_ST7032_H_
#define __LCD_ST7032_H_

#include <stdint.h>
    
    #define	LCD_LINELENGTH                  16
    #define	LCD_FIRSTLINE_ADDRESS	        0x00
    #define	LCD_SECONDLINE_ADDRESS	        0x40
        
    #define LCD_WAKEUP                      0x30
    #define LCD_FUNCTION_SET                0x39    // 0x39	-	8 bit interface; 2 lines; Instruction Table 1
    #define LCD_INT_OSC                     0x14    // 0x14	-	Set Internal OSC and frame frequency -> 183 Hz
    #define LCD_PWR_CTRL                    0x56    // 0x56	-	Power Control; Booster ON; Contrast C5=1
    #define LCD_FOLLOWER_CTRL               0x6D    // 0x6D	-	Follower Control; Follower On; Amplification = 0b101
    #define LCD_ON                          0x0E    // 0x0C	-	Display ON
    #define LCD_ENTRY_MODE                  0x06    // 0x06	-	Entry Mode: Left to right
    #define LCDCMD_CLEARDISPLAY             0x01    // 0x01	-	Clear Display
    #define LCDCMD_CONTRASTSET_HIGHBYTE	    0x54
    #define LCDCMD_CONTRASTSET_LOWBYTE	    0x70    // 0x70	-	Set Contrast low byte				 -> 0x00
    #define LCDCMD_SET_DDRAM_ADDRESS	    0x80	

    extern uint8_t LCDText[16*2+1];
    
    void LCD_Initialize(void);
    void LCD_Update(void);
    void LCD_Erase(void);
    void LCD_BacklightON(void);
    void LCD_BacklightOFF(void);
    void LCD_Display(char *, uint8_t, bool);
    void LCDWriteLine(uint16_t number, char *line);
    void UserInterruptHandler(void);
#endif
