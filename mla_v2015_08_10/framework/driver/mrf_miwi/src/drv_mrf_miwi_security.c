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

#include "system_config.h"

#if defined(SOFTWARE_SECURITY) && defined(ENABLE_SECURITY)

    #include "driver/mrf_miwi/drv_mrf_miwi_security.h"

    #include "console.h"
#if defined(MRF89XA)
    #include "config_89xa.h"
#elif defined (MRF49XA)
    #include "config_49xa.h"
#endif
    
    uint8_t tmpBlock[BLOCK_SIZE];
        
    #if defined(XTEA_128)
		 /**************************************************************************
		  * To follow US export control regulation, this free download version 
		  * implements XTEA with 32bit block size and 64bit key length. To acquire 
		  * the XTEA engine with 64bit block size and 128bit key length, customer
		  * needs to authorize through MicrochipDirect web site or any other Microchip 
		  * agent.
		  ***************************************************************************/
    
    #elif defined(XTEA_64)
        
            const unsigned char mySecurityKey[8] = {SECURITY_KEY_00, SECURITY_KEY_01, SECURITY_KEY_02,    // The 16-byte security key used in the
                SECURITY_KEY_03, SECURITY_KEY_04, SECURITY_KEY_05, SECURITY_KEY_06, SECURITY_KEY_07};   // security module.                                                              
        
        
        /*********************************************************************
         * void encode(INPUT uint16_t *text, INPUT uint16_t *key)
         *
         * Overview:        This function apply XTEA-64 security engine to
         *                  the input data buffer with input security key. 
         *                  The encoded data will replace the input data
         *
         * PreCondition:    None
         *
         * Input:       
         *          uint16_t *      data        The input buffer to the XTEA engine. The
         *                                  encoded data will replace the original 
         *                                  content after the function call
         *          uint16_t *      key         The security key for the XTEA engine
         * Output:          
         *          None
         *
         * Side Effects:    None
         * 
         ********************************************************************/
        void encode(uint16_t *text, uint16_t *key)
        {
            uint16_t part1=text[0], part2=text[1];
            uint16_t sum=0, delta=0x9E37;
            uint8_t i;
            
            for(i=0; i<XTEA_ROUND; i++) 
            {
                part1 += (((part2 << 4) ^ (part2 >> 5)) + part2) ^ (sum + key[sum & 3]);
                sum += delta;
                part2 += (((part1 << 4) ^ (part1 >> 5)) + part1) ^ (sum + key[(sum>>11) & 3]);
            }
            text[0]=part1; text[1]=part2;
        }
    #endif
    
    /*********************************************************************
     * void CTR(uint8_t *text,
     *          uint8_t len,
     *          uint8_t *key,
     *          uint8_t *nounce)
     *
     * Overview:        This function implement Counter (CTR) mode of security 
     *                  engine to the input text. Counter mode is used to 
     *                  encrypt the data.
     *
     * PreCondition:    None
     *
     * Input:       
     *          uint8_t *      text        The text to be encrypted. The encrypted
     *                                  data will replace the original content
     *                                  after this function call.
     *          uint8_t        len         The length of the text to be encrypted
     *          uint8_t *      key         The security key for the XTEA engine
     *          uint8_t *      nounce      The nounce to be used in the CTR mode
     * Output:          
     *          None
     *
     * Side Effects:    None
     * 
     ********************************************************************/
    void CTR(uint8_t *text, uint8_t len, uint8_t *key, uint8_t *nounce)
    {
        uint8_t block = len/BLOCK_SIZE+1;
        uint8_t i, j;
        #if defined(__18CXX)
            uint8_t ITStatus = INTCONbits.GIEH;
        
            INTCONbits.GIEH = 0;
        #endif

        for(i = 0; i < block; i++)
        {
            for(j = 0; j < BLOCK_SIZE-1; j++)
            {
                tmpBlock[j] = nounce[j];
            }
            tmpBlock[BLOCK_SIZE-1] = i;
            encode((BLOCK_UNIT *)tmpBlock, (BLOCK_UNIT *)key);
            for(j = 0; j < BLOCK_SIZE; j++)
            {
                if( (i * BLOCK_SIZE + j) >= len )
                {
                    break;
                }
                text[i * BLOCK_SIZE + j] ^= tmpBlock[j];
            }
        }

        #if defined(__18CXX)
            INTCONbits.GIEH = ITStatus;
        #endif
    }
        
    
    /*********************************************************************
     * void CBC_MAC(   uint8_t *text,
     *                 uint8_t len,
     *                 uint8_t *key,
     *                 uint8_t *MIC)
     *
     * Overview:        This function implements CBC-MAC mode of security 
     *                  engine to the input text. CBC-MAC mode generates
     *                  Message Interity Code (MIC) to authenticate the 
     *                  input text.
     *
     * PreCondition:    None
     *
     * Input:       
     *          uint8_t *      text        The text to be authenticated
     *          uint8_t        len         The length of the input text
     *          uint8_t *      key         The security key for the XTEA engine
     * Output:          
     *          uint8_t *      MIC         The generated MIC from the CBC-MAC mode
     *
     * Side Effects:    None
     * 
     ********************************************************************/    
    void CBC_MAC(   uint8_t *text,
                    uint8_t len,
                    uint8_t *key,
                    uint8_t *MIC)
    {
        uint8_t block = len / BLOCK_SIZE + 1;
        uint8_t i, j;
        #if defined(__18CXX)
            uint8_t ITStatus = INTCONbits.GIEH;
        
            INTCONbits.GIEH = 0;
        #endif
        
        for(i = 0; i < BLOCK_SIZE; i++)
        {
            MIC[i] = 0;
        }
        
        for(i = 0; i < block; i++)
        {
            for(j = 0; j < BLOCK_SIZE; j++)
            {
                if( (i * BLOCK_SIZE + j) >= len )
                {
                    break;
                }
                MIC[j] ^= text[i * BLOCK_SIZE + j];    
            }
            encode((BLOCK_UNIT *)MIC, (BLOCK_UNIT *)key);
        }  
        #if defined(__18CXX)
            INTCONbits.GIEH = ITStatus;
        #endif  
    }
    


    /*********************************************************************
     * void CCM_Enc(    uint8_t *text,
     *                  uint8_t headerLen,
     *                  uint8_t payloadLen,
     *                  uint8_t *key)
     *
     * Overview:        This function implements CCM mode of security 
     *                  engine to the input text. CCM mode ensures data
     *                  interity as well as secrecy. This function is used
     *                  to encode the data
     *
     * PreCondition:    None
     *
     * Input:       
     *          uint8_t *      text        The text to be encrypted. The encrypted
     *                                  data will replace the original content
     *                                  after this function call.
     *          uint8_t *      headerLen   The header length, used to authenticate, but
     *                                  not encrypted
     *          uint8_t        payloadLen  The length of the text to be authenticated
     *                                  and encrypted
     *          uint8_t *      key         The security key for the XTEA engine
     * Output:          
     *          None
     *
     * Side Effects:    None
     * 
     ********************************************************************/ 
    void CCM_Enc(   uint8_t *text,
                    uint8_t headerLen,
                    uint8_t payloadLen,
                    uint8_t *key)
    {
        uint8_t i;
        #if defined(__18CXX)
            uint8_t ITStatus = INTCONbits.GIEH;
        
            INTCONbits.GIEH = 0;
        #endif
        
        CBC_MAC(text, (headerLen + payloadLen), key, tmpBlock);   
        
        for(i = 0; i < BLOCK_SIZE; i++)
        {
            text[headerLen + payloadLen + i] = tmpBlock[i];
        }
        
        for(i = 0; i < BLOCK_SIZE-1; i++)
        {
            tmpBlock[i] = (i < headerLen) ? text[i] : 0;
        }
    
        CTR(&(text[headerLen]), (payloadLen + BLOCK_SIZE), key, tmpBlock);    
        #if defined(__18CXX)
            INTCONbits.GIEH = ITStatus;
        #endif  
    }
    
    
    /*********************************************************************
     * bool CCM_Dec(    uint8_t *text,
     *                  uint8_t headerLen,
     *                  uint8_t payloadLen,
     *                  uint8_t *key)
     *
     * Overview:        This function implements CCM mode of security 
     *                  engine to the input text. CCM mode ensures data
     *                  interity as well as secrecy. This function is used
     *                  to decode the data
     *
     * PreCondition:    None
     *
     * Input:       
     *          uint8_t *      text        The text to be encrypted. The decrypted
     *                                  data will replace the original content
     *                                  after this function call.
     *          uint8_t *      headerLen   The header length, used to authenticate, but
     *                                  not decrypted
     *          uint8_t        payloadLen  The length of the text to be authenticated
     *                                  and decrypted
     *          uint8_t *      key         The security key for the XTEA engine
     * Output:          
     *          None
     *
     * Side Effects:    None
     * 
     ********************************************************************/ 
    bool CCM_Dec(uint8_t *text, uint8_t headerLen, uint8_t payloadLen, uint8_t *key)
    {
        uint8_t i;
        #if defined(__18CXX)
            uint8_t ITStatus = INTCONbits.GIEH;
        
            INTCONbits.GIEH = 0;
        #endif

        for(i = 0; i < BLOCK_SIZE-1; i++)
        {
            tmpBlock[i] = (i < headerLen) ? text[i] : 0;
        }
        CTR(&(text[headerLen]), payloadLen, key, tmpBlock);

        CBC_MAC(text, (headerLen + payloadLen - SEC_MIC_LEN), key, tmpBlock);
        for(i = 0; i < SEC_MIC_LEN; i++)
        {
            if( tmpBlock[i] != text[headerLen + payloadLen - SEC_MIC_LEN + i] )
            {
                #if defined(__18CXX)
                    INTCONbits.GIEH = ITStatus;
                #endif
                return false;
            }       
        }
        #if defined(__18CXX)
            INTCONbits.GIEH = ITStatus;
        #endif  
        return true;
    }

#endif

extern char bogus;

