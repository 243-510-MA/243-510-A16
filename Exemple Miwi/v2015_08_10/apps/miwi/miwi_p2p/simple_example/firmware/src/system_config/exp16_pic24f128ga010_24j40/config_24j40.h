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

#ifndef __CONFIG_MRF24J40_H

    #define __CONFIG_MRF24J40_H
    
    /*********************************************************************/
    // TURBO_MODE enables MRF24J40 transceiver to perform the communication
    // in proprietary modulation, which is not compliant to IEEE 802.15.4
    // specification. The data rate at turbo mode is up to 625Kbps.
    /*********************************************************************/
    //#define TURBO_MODE
    
    
    /*********************************************************************/
    // VERIFY_TRANSMIT configures the MRF24J40 transceiver to transmit 
    // data in a block procedure, which ensures finish transmission before
    // continue other task. This block procedure ensures the delivery state
    // of transmitting known to the upper protocol layer, thus may be 
    // necessary to detect transmission failure. However, this block procedure
    // slightly lowers the throughput
    /*********************************************************************/
    #define VERIFY_TRANSMIT
    
    
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
	
    /*********************************************************************/
    // If MRF24J40MB module with external power amplifier and low noise
    // amplifier has been used, the stack needs to do output power adjustment
    // according to MRF24J40MB data sheet.
    // Comment this part if used other design of MRF24J40 with external PA
    // and/or LNA. This definition cannot be used with definition of
    // MRF24J40MC
    /*********************************************************************/
    #define MRF24J40MB

    /*********************************************************************/
    // If MRF24J40MC module with external power amplifier, low noise
    // amplifier and external antenna has been used, the stack needs to 
    // do output power adjustment according to MRF24J40MC data sheet.
    // Comment this part if used other design of MRF24J40 with external PA
    // and/or LNA. This definition cannot be used with definition of 
    // MRF24J40MB
    /*********************************************************************/
    //#define MRF24J40MC


    #if defined(MRF24J40MB) && defined(MRF24J40MC)
        #error "MRF24J40MB and MRF24J40MC cannot be defined at the same time"
    #endif

    #if defined(MRF24J40MB) || defined(MRF24J40MC)
        #define UNDEFINED_LOCATION  0x00
        #define UNITED_STATES       0x01
        #define CANADA              0x02
        #define EUROPE              0x03
        /*********************************************************************/
        // If MRF24J40MB/C module is used, the output power setting depends on
        // the country where the application is used. Define one of the 
        // locations that this appliation will be applied. If none of the location
        // is set, US FCC setting for MRF24J40MB/C module will be used in the stack.
        // Check MRF24J40MB/C data sheet for details.
        /*********************************************************************/
        #define APPLICATION_SITE    UNITED_STATES
    #endif
	
    
#endif

