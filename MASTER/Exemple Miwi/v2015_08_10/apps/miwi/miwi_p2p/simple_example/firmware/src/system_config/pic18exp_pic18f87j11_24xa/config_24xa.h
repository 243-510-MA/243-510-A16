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

#ifndef __CONFIG_MRF24XA_H

    #define __CONFIG_MRF24XA_H

    #define DATA_RATE   DR_250KBPS

    /*********************************************************************/
    // SECURITY_KEY_xx defines xxth byte of security key used in the
    // block cipher
    /*********************************************************************/
    #define SECURITY_KEY_00 0x00
    #define SECURITY_KEY_01 0x01
    #define SECURITY_KEY_02 0x02
    #define SECURITY_KEY_03 0x03
    #define SECURITY_KEY_04 0x04
    #define SECURITY_KEY_05 0x05
    #define SECURITY_KEY_06 0x06
    #define SECURITY_KEY_07 0x07
    #define SECURITY_KEY_08 0x08
    #define SECURITY_KEY_09 0x09
    #define SECURITY_KEY_10 0x0a
    #define SECURITY_KEY_11 0x0b
    #define SECURITY_KEY_12 0x0c
    #define SECURITY_KEY_13 0x0d
    #define SECURITY_KEY_14 0x0e
    #define SECURITY_KEY_15 0x0f
    
    
    /*********************************************************************/
    // KEY_SEQUENCE_NUMBER defines the sequence number that is used to
    // identify the key. Different key should have different sequence
    // number, if multiple security keys are used in the application.
    /*********************************************************************/
    #define KEY_SEQUENCE_NUMBER 0x00
    
    
    /*********************************************************************/
    // SECURITY_LEVEL defines the security mode used in the application
    /*********************************************************************/
    #define SECURITY_LEVEL SEC_LEVEL_CCM_32


    /*********************************************************************/
    // INFER_DEST_ADDRESS enables inferred destination address mode, which
    // does not transmit the destination address, but depends on the software
    // CRC to infer the destination address. Infer destination address applies
    // to only transceivers that support MiMAC frame format and the CRC engine
    // that supports this feature.
    /*********************************************************************/
    #define INFER_DEST_ADDRESS
    
    
    /*********************************************************************/
    // SOURCE_ADDRESS_ABSENT disable the stack to transmit the source address
    // in the MAC layer, if the destination does not care where the message
    // comes from. This feature is highly application dependent. This feature
    // is only available for transceivers that support MiMAC frame format.
    /*********************************************************************/
    #define SOURCE_ADDRESS_ABSENT


    /*********************************************************************/
    // FRAME_COUNTER_UPDATE_INTERVAL defines the NVM update interval for
    // frame counter, when security is enabled. The same interval will be
    // added to the frame counter read from NVM when Network Freezer
    // feature is enabled.
    /*********************************************************************/ 
    #define FRAME_COUNTER_UPDATE_INTERVAL 1024

    /*********************************************************************/
    // BANK_SIZE defines the number of packet can be received and stored
    // to wait for handling in MiMAC layer.
    /*********************************************************************/
    #define BANK_SIZE           2


    #if defined(ENABLE_SECURITY)

        #if SECURITY_LEVEL == SEC_LEVEL_CTR
            #define MIC_SIZE 0
        #elif (SECURITY_LEVEL == SEC_LEVEL_CBC_MAC_128) || (SECURITY_LEVEL == SEC_LEVEL_CCM_128)
            #define MIC_SIZE 16
        #elif (SECURITY_LEVEL == SEC_LEVEL_CBC_MAC_64) || (SECURITY_LEVEL == SEC_LEVEL_CCM_64)
            #define MIC_SIZE 8
        #elif (SECURITY_LEVEL == SEC_LEVEL_CBC_MAC_32) || (SECURITY_LEVEL == SEC_LEVEL_CCM_32)
            #define MIC_SIZE 4
        #endif

        #define RX_PACKET_SIZE (RX_BUFFER_SIZE+PROTOCOL_HEADER_SIZE+MY_ADDRESS_LENGTH+MY_ADDRESS_LENGTH+MIC_SIZE+17)

        #if (RX_PACKET_SIZE > 127) && !defined(__18CXX)
            #warning  "Maximum application payload RX BUFFER SIZE is 94"
        #endif

    #else

        #define RX_PACKET_SIZE (RX_BUFFER_SIZE+PROTOCOL_HEADER_SIZE+MY_ADDRESS_LENGTH+MY_ADDRESS_LENGTH+12)

        #if (RX_PACKET_SIZE > 127) && !defined(__18CXX)
            #warning "Maximum application payload RX BUFFER SIZE is 99"
        #endif

    #endif

    #if RX_PACKET_SIZE > 127
        #undef RX_PACKET_SIZE
        #define RX_PACKET_SIZE 127
    #endif
	
#endif
    

