
#include <xc.h>
#include "rtcc.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

uint8_t BSP_RTCC_DecToBCD (uint8_t value);
uint8_t BSP_RTCC_BCDToDec (uint8_t value);

void BSP_RTCC_Initialize (BSP_RTCC_DATETIME * value)
{
    unsigned int INTCONSave;

    // Turn on the secondary oscillator
    T1CONbits.SOSCEN = 1;

    // Set the RTCWREN bit.  Requires special unlock sequence.
    INTCONSave = INTCON;        //Save current interrupt settings
    INTCONbits.GIEH = 0;        //Disable interrupts.  Below sequence must be back to back uninterrupted instruction execution to unlock.
    EECON2 = 0x55;
    EECON2 = 0xAA;
    RTCCFGbits.RTCWREN = 1;
    //Restore previous interrupt settings
    if(INTCONSave & 0x80)
    {
        INTCONbits.GIEH = 1;
    }

    //Initially point to Reserved:Year in RTCVALH:RTCVALL
    RTCCFGbits.RTCPTR0 = 1;
    RTCCFGbits.RTCPTR1 = 1;

   // Set it to the correct time
   if(value->bcdFormat)
   {
       RTCVALL = value->year;
       INTCONSave = RTCVALH;      //Dummy access to decrement RTCPTR
       RTCVALL = value->day;
       RTCVALH = value->month;
       RTCVALL = value->hour;
       RTCVALH = value->weekday;
       RTCVALL = value->second;
       RTCVALH = value->minute;
   }
   else
   {
       RTCVALL = BSP_RTCC_DecToBCD(value->year);
       INTCONSave = RTCVALH;      //Dummy access to decrement RTCPTR
       RTCVALL = BSP_RTCC_DecToBCD(value->day);
       RTCVALH = BSP_RTCC_DecToBCD(value->month);
       RTCVALL = BSP_RTCC_DecToBCD(value->hour);
       RTCVALH = BSP_RTCC_DecToBCD(value->weekday);
       RTCVALL = BSP_RTCC_DecToBCD(value->second);
       RTCVALH = BSP_RTCC_DecToBCD(value->minute);
   }

   // Enable RTCC, clear RTCWREN
   RTCCFGbits.RTCEN = 1;
   RTCCFGbits.RTCWREN = 0;
}

void BSP_RTCC_TimeGet (BSP_RTCC_DATETIME * value)
{
    uint8_t registerValue;
    bool moreReadsNeeded;
    
    //Repeatedly read the time registers until we read the same values twice in a row,
    //indicating that the values did not change while we were in the process of reading them.
    moreReadsNeeded = true;
    while(moreReadsNeeded == true)
    {
        moreReadsNeeded = false;

        //Initially point to Reserved:Year in RTCVALH:RTCVALL
        RTCCFGbits.RTCPTR0 = 1;
        RTCCFGbits.RTCPTR1 = 1;

        value->year = RTCVALL;          //Read year value
        registerValue = RTCVALH;        //Dummy read from RTCVALH to cause decrement of RTCPTR bits
        value->day = RTCVALL;           //Read the Day
        value->month = RTCVALH;         //Read the Month and cause decrement of RTCPTR
        value->hour = RTCVALL;          //Read the hour
        value->weekday = RTCVALH;       //Read the Weekday and cause decrement of RTCPTR
        value->second = RTCVALL;        //Read the seconds
        value->minute = RTCVALH;        //Read the minutes        

        //Initially point to Reserved:Year in RTCVALH:RTCVALL
        RTCCFGbits.RTCPTR0 = 1;
        RTCCFGbits.RTCPTR1 = 1;
        if(value->year != RTCVALL)
            moreReadsNeeded = true;
        registerValue = RTCVALH;        //Dummy read from RTCVALH to cause decrement of RTCPTR bits
        if(value->day != RTCVALL)
            moreReadsNeeded = true;
        if(value->month != RTCVALH)
            moreReadsNeeded = true;
        if(value->hour != RTCVALL)
            moreReadsNeeded = true;
        if(value->weekday != RTCVALH)
            moreReadsNeeded = true;
        if(value->second != RTCVALL)
            moreReadsNeeded = true;
        if(value->minute != RTCVALH)
            moreReadsNeeded = true;
    }

    if (!value->bcdFormat)
    {
        value->year = BSP_RTCC_BCDToDec(value->year);
        value->month = BSP_RTCC_BCDToDec(value->month);
        value->day = BSP_RTCC_BCDToDec(value->day);
        value->weekday = BSP_RTCC_BCDToDec(value->weekday);
        value->hour = BSP_RTCC_BCDToDec(value->hour);
        value->minute = BSP_RTCC_BCDToDec(value->minute);
        value->second = BSP_RTCC_BCDToDec(value->second);
    }
}

// Note : value must be < 100
uint8_t BSP_RTCC_DecToBCD (uint8_t value)
{
    return (((value / 10)) << 4) | (value % 10);
}

uint8_t BSP_RTCC_BCDToDec (uint8_t value)
{
    return ((value >> 4) * 10) + (value & 0x0F);
}

