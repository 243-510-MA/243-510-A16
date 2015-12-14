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

/************************ HEADERS **********************************/


#include "system.h"


/************************ FUNCTIONS ********************************/

/*********************************************************************
* Function:         void SPIPut(uint8_t v)
*
* PreCondition:     SPI has been configured 
*
* Input:		    v - is the byte that needs to be transfered
*
* Output:		    none
*
* Side Effects:	    SPI transmits the byte
*
* Overview:		    This function will send a byte over the SPI
*
* Note:			    None
********************************************************************/
void SPIPut(uint8_t v)
{
    uint8_t i;
    
    #if !defined(HARDWARE_SPI)
        
        SPI_SDO = 0;
        SPI_SCK = 0;
        
        for(i = 0; i < 8; i++)
        {
            SPI_SDO = (v >> (7-i));      
            SPI_SCK = 1;
            SPI_SCK = 0;  
        }  
        SPI_SDO = 0; 
    #else
    
        
        IFS0bits.SPI1IF = 0;
        i = SPI1BUF;
        SPI1BUF = v;
        while(IFS0bits.SPI1IF == 0){}
        
    #endif
}


/*********************************************************************
* Function:         uint8_t SPIGet(void)
*
* PreCondition:     SPI has been configured 
*
* Input:		    none
*
* Output:		    uint8_t - the byte that was last received by the SPI
*
* Side Effects:	    none
*
* Overview:		    This function will read a byte over the SPI
*
* Note:			    None
********************************************************************/
uint8_t SPIGet(void)
{
    #if !defined(HARDWARE_SPI)
        uint8_t i;
        uint8_t spidata = 0;

        
        SPI_SDO = 0;
        SPI_SCK = 0;
        
        for(i = 0; i < 8; i++)
        {
            spidata = (spidata << 1) | SPI_SDI;  
            SPI_SCK = 1;
            SPI_SCK = 0; 
        }
        
        return spidata;
    #else 
        
        SPIPut(0x00);
        return SPI1BUF;

    #endif
}

#if defined(SUPPORT_TWO_SPI)
    /*********************************************************************
    * Function:         void SPIPut2(uint8_t v)
    *
    * PreCondition:     SPI has been configured 
    *
    * Input:		    v - is the byte that needs to be transfered
    *
    * Output:		    none
    *
    * Side Effects:	    SPI transmits the byte
    *
    * Overview:		    This function will send a byte over the SPI
    *
    * Note:			    None
    ********************************************************************/
    void SPIPut2(uint8_t v)
    {
        uint8_t i;
        
        #if !defined(HARDWARE_SPI)
            
            SPI_SDO2 = 0;
            SPI_SCK2 = 0;
            
            for(i = 0; i < 8; i++)
            {
                SPI_SDO2 = (v >> (7-i));      
                SPI_SCK2 = 1;
                SPI_SCK2 = 0;  
            }  
            SPI_SDO2 = 0; 
        #else
            IFS2bits.SPI2IF = 0;
            i = SPI2BUF;
            SPI2BUF = v;
            while(IFS2bits.SPI2IF == 0){}
           
        #endif
    }
    
    
    /*********************************************************************
    * Function:         uint8_t SPIGet2(void)
    *
    * PreCondition:     SPI has been configured 
    *
    * Input:		    none
    *
    * Output:		    uint8_t - the byte that was last received by the SPI
    *
    * Side Effects:	    none
    *
    * Overview:		    This function will read a byte over the SPI
    *
    * Note:			    None
    ********************************************************************/
    uint8_t SPIGet2(void)
    {
        #if !defined(HARDWARE_SPI)
            uint8_t i;
            uint8_t spidata = 0;
    
            SPI_SDO2 = 0;
            SPI_SCK2 = 0;
            
            for(i = 0; i < 8; i++)
            {
                spidata = (spidata << 1) | SPI_SDI2;  
                SPI_SCK2 = 1;
                SPI_SCK2 = 0; 
            }
            
            return spidata;
        #else 
            
            SPIPut2(0x00);
            return SPI2BUF;
            
        #endif
    }
#endif    
    







