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

#ifndef __SECURITY_H
    #define __SECURITY_H

#include "system_config.h"

    #if defined(SOFTWARE_SECURITY)

        //#define XTEA_128
        #define XTEA_64
        
        #define XTEA_ROUND  32

        #define SEC_LEVEL_CTR           0
        #define SEC_LEVEL_CBC_MAC_16    1
        #define SEC_LEVEL_CBC_MAC_32    2
        #define SEC_LEVEL_CBC_MAC_64    3
        #define SEC_LEVEL_CCM_16        4
        #define SEC_LEVEL_CCM_32        5
        #define SEC_LEVEL_CCM_64        6

        #if defined(XTEA_128)
            #define BLOCK_SIZE 8
            #define BLOCK_UNIT uint32_t
            #define KEY_SIZE 16
        #elif defined(XTEA_64)
            #define BLOCK_SIZE 4
            #define BLOCK_UNIT uint16_t
            #define KEY_SIZE 8
        #endif
        
        #if SECURITY_LEVEL == SEC_LEVEL_CTR
            #define SEC_MIC_LEN     0
        #elif SECURITY_LEVEL == SEC_LEVEL_CBC_MAC_16
            #define SEC_MIC_LEN     2
        #elif SECURITY_LEVEL == SEC_LEVEL_CBC_MAC_32
            #define SEC_MIC_LEN     4
        #elif SECURITY_LEVEL == SEC_LEVEL_CBC_MAC_64
            #define SEC_MIC_LEN     8
        #elif SECURITY_LEVEL == SEC_LEVEL_CCM_16
            #define SEC_MIC_LEN     2
        #elif SECURITY_LEVEL == SEC_LEVEL_CCM_32
            #define SEC_MIC_LEN     4
        #elif SECURITY_LEVEL == SEC_LEVEL_CCM_64
            #define SEC_MIC_LEN     8
        #endif
        
        
        extern const unsigned char mySecurityKey[];

        void CTR(uint8_t *text, uint8_t len, uint8_t *key, uint8_t *nounce);
        void CBC_MAC(uint8_t *text, uint8_t len, uint8_t *key, uint8_t *MIC);
        void CCM_Enc(uint8_t *text, uint8_t headerLen, uint8_t payloadLen, uint8_t *key);
        bool CCM_Dec(uint8_t *text, uint8_t headerLen, uint8_t payloadLen, uint8_t *key);
    
    #endif

#endif

