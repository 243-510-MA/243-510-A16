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
#include "system.h"
#include "system_config.h"


#include "driver/mrf_miwi/drv_mrf_miwi_49xa.h"
#include "driver/mrf_miwi/drv_mrf_miwi_security.h"
#include "driver/mrf_miwi/drv_mrf_miwi_crc.h"
#if defined(ENABLE_NVM)
#include "miwi/miwi_nvm.h"

    //==============================================================
    // Global variables:
    //==============================================================
        
    /**********************************************************************
     * "#pragma udata" is used to specify the starting address of a 
     * global variable. The address may be MCU dependent on RAM available
     * If the size of the global variable is small, such manual assignment
     * may not be necessary. Developer can comment out the address
     * assignment.
     **********************************************************************/
    #if defined(__18CXX) && !defined(__XC8)
        #pragma udata MAC_RX_BUFFER
    #endif
        volatile RX_PACKET          RxPacket[BANK_SIZE];
    #if defined(__18CXX) && !defined(__XC8)
        #pragma udata
    #endif
    volatile TRANSCEIVER_STATUS TransceiverStatus;
    
    MACINIT_PARAM               MACInitParams;
    uint8_t                        TxMACSeq;
    uint8_t                        MACSeq;
    uint8_t                        ReceivedBankIndex;
    
    #if defined(ENABLE_ACK)
        volatile    bool hasAck = false;
        #if defined(ENABLE_RETRANSMISSION)
            ACK_INFO    AckInfo[ACK_INFO_SIZE];
        #endif
    #endif
    
    #if defined(ENABLE_SECURITY)
        API_UINT32_UNION OutgoingFrameCounter;
        uint8_t key[KEY_SIZE];
    #endif
    
    
    volatile uint8_t        MACTxBuffer[TX_PACKET_SIZE];
    

    void SPIPut(uint8_t v);
    uint8_t SPIGet(void);
    
    /*********************************************************************
     * uint16_t getReceiverBW(void)
     *
     * Overview:        
     *              This function get the receiver band width setting
     *              based on RF deviation configuration
     *
     * PreCondition:    
     *              RF deviation configuration has been done in the 
     *              C preprocessor
     *
     * Input:       None
     *
     * Output:      
     *          uint16_t    The configuration setting for receiver band width.
     *                  This output needs to be ORed with receiver 
     *                  configuration command
     *
     * Side Effects:    None
     *
     ********************************************************************/
    uint16_t getReceiverBW(void)
    {
        uint8_t bw_table[16] = {6,6,6,5,5,4,4,4,3,3,2,2,1,1,1,1};
        
        return ( ((uint16_t)(bw_table[(uint8_t)((uint16_t)RF_DEV/15)])) << 5);
    }

    
    /*********************************************************************
     * void RegisterSet(INPUT uint16_t setting)
     *
     * Overview:        
     *              This function access the control register of MRF49XA.
     *              The register address and the register settings are
     *              the input
     *
     * PreCondition:    
     *              None
     *
     * Input:       
     *          uint16_t    setting     The address of the register and its
     *                              corresponding settings
     *
     * Output:  None    
     *
     * Side Effects:    Register settings have been modified
     *
     ********************************************************************/
    void RegisterSet(uint16_t setting)
    {
        uint8_t oldRFIE = RFIE;
        
        RFIE = 0;
        PHY_CS = 0;
        SPIPut((uint8_t)(setting >> 8));
        SPIPut((uint8_t)setting);
        PHY_CS = 1;
        RFIE = oldRFIE;
    }
    
    /*********************************************************************
     * void StatusRead(void)
     *
     * Overview:        
     *              This function read the status register and output the
     *              status in the global variable TransceiverStatus.
     *
     * PreCondition:    
     *              MRF49XA transceiver has been properly initialized
     *
     * Input:       None   
     *
     * Output:      None    
     *
     * Side Effects:    Global variable TransceiverStatus has been modified
     *
     ********************************************************************/
    void StatusRead(void)
    {
        uint8_t preNFSEL = nFSEL;
        uint8_t preNCS = PHY_CS;
        uint8_t oldRFIE = RFIE;
        
        RFIE = 0;
        nFSEL = 1;
        PHY_CS = 0;
        
        #if defined(HARDWARE_SPI)
        
            TransceiverStatus.v[0] = SPIGet();
            TransceiverStatus.v[1] = SPIGet();
            
        #else
            SPI_SDO = 0;

            TransceiverStatus.bits.RG_FF_IT = SPI_SDI;    
            SPI_SCK = 1;
            SPI_SCK = 0;
           
            TransceiverStatus.bits.POR = SPI_SDI;
            SPI_SCK = 1;
            SPI_SCK = 0;
            
            TransceiverStatus.bits.RGUR_FFOV = SPI_SDI;
            SPI_SCK = 1;
            SPI_SCK = 0;
            
            TransceiverStatus.bits.WKUP = SPI_SDI;
            SPI_SCK = 1;
            SPI_SCK = 0;
            
            TransceiverStatus.bits.EXT = SPI_SDI;
            SPI_SCK = 1;
            SPI_SCK = 0;
            
            TransceiverStatus.bits.LBD = SPI_SDI;
            SPI_SCK = 1;
            SPI_SCK = 0;
            
            TransceiverStatus.bits.FFEM = SPI_SDI;
            SPI_SCK = 1;
            SPI_SCK = 0;
            
            TransceiverStatus.bits.RSSI_ATS = SPI_SDI;
            SPI_SCK = 1;
            SPI_SCK = 0;
                
            TransceiverStatus.bits.DQD = SPI_SDI;
            SPI_SCK = 1;
            SPI_SCK = 0;
            
            TransceiverStatus.bits.CRL = SPI_SDI;
            SPI_SCK = 1;
            SPI_SCK = 0;
            
            TransceiverStatus.bits.ATGL = SPI_SDI;
            SPI_SCK = 1;
            SPI_SCK = 0;
        #endif
        
        nFSEL = preNFSEL;
        PHY_CS = preNCS;
        RFIE = oldRFIE;
    }
    
    
    /*********************************************************************
     * bool TxPacket(INPUT uint8_t TxPacketLen, INPUT bool CCA)
     *
     * Overview:        
     *              This function send the packet in the buffer MACTxBuffer
     *
     * PreCondition:    
     *              MRF49XA transceiver has been properly initialized
     *
     * Input:       
     *              uint8_t    TxPacketLen     The length of the packet to be
     *                                      sent.
     *              bool    CCA             The boolean to indicate if a
     *                                      CCA operation needs to be done
     *                                      before sending the packet   
     *
     * Output:      
     *              bool    The boolean to indicate if packet sent successful
     *
     * Side Effects:    
     *              The packet has been sent out
     *
     ********************************************************************/
    bool TxPacket(INPUT uint8_t TxPacketLen, INPUT bool CCA)
    {
        bool status;
        uint8_t TxPacketPtr;
        MIWI_TICK t1, t2;
        uint8_t i;
        uint16_t counter;
        
        #ifdef ENABLE_CCA
            uint8_t CCARetries;
        #endif
    
        #if defined(ENABLE_ACK) && defined(ENABLE_RETRANSMISSION)
            uint8_t reTry = RETRANSMISSION_TIMES;
        #endif
                
        #if defined(ENABLE_ACK) && defined(ENABLE_RETRANSMISSION)
            while( reTry-- )
        #endif
        {
            uint8_t allowedTxFailure;
            uint8_t synCount;
            uint8_t DQDOK;
            
            allowedTxFailure = 0;
Start_Transmitting:
    
            #if defined(ENABLE_ACK)
                hasAck = false;
            #endif
            
            #ifdef ENABLE_CCA
                CCARetries = 0;
        
                if( CCA )
                {
                    RegisterSet(PMCREG | 0x0080);
Start_CCA:    
                    DQDOK = 0;    
                    for(i = 0; i < CCA_TIMES; i++)
                    {
                        StatusRead();
                        if( TransceiverStatus.bits.DQD )
                        {
                            DQDOK++;
                        }
                        DELAY_10us(1);
                    }
                    
                    if( DQDOK > CCA_THRESHOLD )
                    {
                        if(CCARetries++ > CCA_RETRIES )
                        {
                            return false;
                        }
                        goto Start_CCA;
                    }
                }
            #endif
            
            RFIE = 0;
            
            // Turn off receiver, enable the TX register
            RegisterSet(PMCREG);
            RegisterSet(GENCREG | 0x0080);
            
            // enable transmitter
            RegisterSet(PMCREG | 0x0020);
            
            DELAY_ms(1);
             
            TxPacketPtr = 0;
            synCount = 0;

            PHY_CS = 0;

            SPIPut(0xB8);
            SPIPut(0xAA);                 // 3rd preamble
            
            counter = 0;
            while(TxPacketPtr < TxPacketLen + 2 )   // 2 dummy byte
            {
                if( SPI_SDI == 1 ) 
                {
                    if( synCount < 3 )
                    {
                        switch(synCount)
                        {
                            case 0:
                                SPIPut(0x2D); // first syn byte
                                break;
                            case 1:
                                SPIPut(0xD4); // second syn byte
                                break;
                            case 2:
                                SPIPut(TxPacketLen);
                                break;
                            default:
                                break;
                        }
                        synCount++;
                    }
                    else
                    {
                        SPIPut(MACTxBuffer[TxPacketPtr]);
                        TxPacketPtr++;
                    }
                    counter = 0;
                }
                else 
                {
                    if( counter++ > 0xFFFE )
                    {
                        PHY_CS = 1;
                        
                        if( allowedTxFailure++ > MAX_ALLOWED_TX_FAILURE )
                        {
                            break;
                        }
                        
                        RegisterSet(PMCREG | 0x0080);
                        
                        RegisterSet(GENCREG | 0x0040 );
                        RegisterSet(FIFORSTREG | 0x0002);
                        DELAY_ms(5);
                        RFIE = 1;
    
                        goto Start_Transmitting;
                    }
                }  
            }
            PHY_CS = 1;

            // Turn off the transmitter, disable the Tx register
            RegisterSet(PMCREG | 0x0080);
            RegisterSet(GENCREG | 0x0040 );
            
            RegisterSet(FIFORSTREG | 0x0002);
            
            StatusRead();
            
            RFIE = 1;
    
            #if defined(ENABLE_ACK) 
                if( (MACTxBuffer[0] & ACK_MASK) > 0 )        // required acknowledgement
                {
                    TxMACSeq = MACTxBuffer[1];
                    t1 = MiWi_TickGet();
                    while(1)
                    {
                        if( hasAck )
                        {
                            status = true;
                            goto TX_END_HERE;
                        }
                        t2 = MiWi_TickGet();
                        if( MiWi_TickGetDiff(t2, t1) > ONE_SECOND/20 )
                        {
                            break;
                        }
                    }
                }
                else
            #endif
            {
                status = true;
                goto TX_END_HERE;
            }
        }
        
        status = false;
        
TX_END_HERE:    
        return status;
    }
    
    
    /************************************************************************************
     * Function:
     *      bool MiMAC_SetAltAddress(uint8_t *Address, uint8_t *PANID)
     *
     * Summary:
     *      This function set the alternative network address and PAN identifier if
     *      applicable
     *
     * Description:        
     *      This is the primary MiMAC interface for the protocol layer to 
     *      set alternative network address and/or PAN identifier. This function
     *      call applies to only IEEE 802.15.4 compliant RF transceivers. In case
     *      alternative network address is not supported, this function will return
     *      false.
     *
     * PreCondition:    
     *      MiMAC initialization has been done. 
     *
     * Parameters: 
     *      uint8_t * Address -    The alternative network address of the host device.
     *      uint8_t * PANID -      The PAN identifier of the host device
     *
     * Returns: 
     *      A boolean to indicates if setting alternative network address is successful.
     *
     * Example:
     *      <code>
     *      uint16_t NetworkAddress = 0x0000;
     *      uint16_t PANID = 0x1234;
     *      MiMAC_SetAltAddress(&NetworkAddress, &PANID);
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/ 
    bool MiMAC_SetAltAddress(INPUT uint8_t *Address, INPUT uint8_t *PANID)
    {
        return false;
    }
    
    /************************************************************************************
     * Function:
     *      bool MiMAC_SetChannel(uint8_t channel, uint8_t offsetFreq)
     *
     * Summary:
     *      This function set the operating channel for the RF transceiver
     *
     * Description:        
     *      This is the primary MiMAC interface for the protocol layer to 
     *      set the operating frequency of the RF transceiver. Valid channel
     *      number are from 0 to 31. For different frequency band, data rate
     *      and other RF settings, some channels from 0 to 31 might be
     *      unavailable. Paramater offsetFreq is used to fine tune the center
     *      frequency across the frequency band. For transceivers that follow
     *      strict definition of channels, this parameter may be discarded.
     *      The center frequency is calculated as 
     *      (LowestFrequency + Channel * ChannelGap + offsetFreq)
     *
     * PreCondition:    
     *      Hardware initialization on MCU has been done. 
     *
     * Parameters: 
     *      uint8_t channel -  Channel number. Range from 0 to 31. Not all channels
     *                      are available under all conditions.
     *      uint8_t offsetFreq -   Offset frequency used to fine tune the center
     *                          frequency. May not apply to all RF transceivers
     *
     * Returns: 
     *      A boolean to indicates if channel setting is successful.
     *
     * Example:
     *      <code>
     *      // Set center frequency to be exactly channel 12
     *      MiMAC_SetChannel(12, 0);
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/       
    bool MiMAC_SetChannel(INPUT uint8_t channel, INPUT uint8_t offsetFreq)
    {
        if( channel >= CHANNEL_NUM )    
        {
            return false;
        }

        RegisterSet(0xA000 + FREQ_START + ((uint16_t)channel * FREQ_STEP) + offsetFreq);
        DELAY_ms(20);       // delay 20 ms to stablize the transceiver
        return true;
    }
    
    
    /************************************************************************************
     * Function:
     *      bool MiMAC_SetPower(uint8_t outputPower)
     *
     * Summary:
     *      This function set the output power for the RF transceiver
     *
     * Description:        
     *      This is the primary MiMAC interface for the protocol layer to 
     *      set the output power for the RF transceiver. Whether the RF
     *      transceiver can adjust output power depends on the hardware
     *      implementation.
     *
     * PreCondition:    
     *      MiMAC initialization has been done. 
     *
     * Parameters: 
     *      uint8_t outputPower -  RF transceiver output power.
     *
     * Returns: 
     *      A boolean to indicates if setting output power is successful.
     *
     * Example:
     *      <code>
     *      // Set output power to be 0dBm
     *      MiMAC_SetPower(TX_POWER_0_DB);
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/ 
    bool MiMAC_SetPower(INPUT uint8_t outputPower)
    {
        if( outputPower > TX_POWER_N_17_5_DB )
        {
            return false;
        }
        RegisterSet(0x9800 | (((uint16_t)RF_DEV/15 - 1)<<4) | outputPower);
        return true;
    }
    
    
    /************************************************************************************
     * Function:
     *      bool MiMAC_Init(MACINIT_PARAM initValue)
     *
     * Summary:
     *      This function initialize MiMAC layer
     *
     * Description:        
     *      This is the primary MiMAC interface for the protocol layer to 
     *      initialize the MiMAC layer. The initialization parameter is 
     *      assigned in the format of structure MACINIT_PARAM.
     *
     * PreCondition:    
     *      MCU initialization has been done. 
     *
     * Parameters: 
     *      MACINIT_PARAM initValue -   Initialization value for MiMAC layer
     *
     * Returns: 
     *      A boolean to indicates if initialization is successful.
     *
     * Example:
     *      <code>
     *      MiMAC_Init(initParameter);
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/ 
    bool MiMAC_Init(INPUT MACINIT_PARAM initValue)
    {
        uint8_t i;
         
        MACInitParams = initValue;
           
        PHY_CS = 1;           // nSEL inactive
        nFSEL = 1;           // nFFS inactive
        SPI_SDO = 0;        
        SPI_SCK = 0;        
    
        MACSeq = TMRL;
        ReceivedBankIndex = 0xFF;
        
        for(i = 0; i < BANK_SIZE; i++)
        {
            RxPacket[i].flags.Val = 0;
        }
        
        #if defined(ENABLE_ACK) && defined(RETRANSMISSION)
            for(i = 0; i < ACK_INFO_SIZE; i++)
            {
                AckInfo[i].Valid = false;
            }
        #endif
    
        #if defined(ENABLE_SECURITY)
            #if defined(ENABLE_NETWORK_FREEZER)
                if( initValue.actionFlags.bits.NetworkFreezer )
                {
                    nvmGetOutFrameCounter(OutgoingFrameCounter.v);
                    OutgoingFrameCounter.Val += FRAME_COUNTER_UPDATE_INTERVAL;
                    nvmPutOutFrameCounter(OutgoingFrameCounter.v);
                }
                else
                {
                    OutgoingFrameCounter.Val = 0;
                    nvmPutOutFrameCounter(OutgoingFrameCounter.v);
                    OutgoingFrameCounter.Val = 1;
                }        
            #else
        	    OutgoingFrameCounter.uint32Value = 1;
        	#endif
        	
            for(i = 0; i < KEY_SIZE; i++)
            {
                key[i] = mySecurityKey[i];
            }
        #endif
    
    
        //----  configuring the RF link --------------------------------
        RegisterSet(FIFORSTREG);
        RegisterSet(FIFORSTREG | 0x0002);                               // enable synchron latch
        RegisterSet(GENCREG);
        RegisterSet(AFCCREG);
        RegisterSet(0xA7D0);                                            
        RegisterSet(DRVSREG);
        RegisterSet(PMCREG);
        RegisterSet(RXCREG | getReceiverBW());
        RegisterSet(TXCREG);
        RegisterSet(BBFCREG);
        // antenna tuning on startup
        RegisterSet(PMCREG | 0x0020);                                   // turn on the transmitter
        DELAY_ms(5);                                                    // ant.tuning time (~100us) + Xosc starup time (5ms)
        // end of antenna tuning
        RegisterSet(PMCREG | 0x0080);                                   // turn off transmitter, turn on receiver
        RegisterSet(GENCREG | 0x0040);                                  // enable the FIFO
        RegisterSet(FIFORSTREG);
        RegisterSet(FIFORSTREG | 0x0002);                               // enable synchron latch
        RegisterSet(0x0000);										    // read status byte (read ITs)
        
        
        return true;
    }
    
    
    /************************************************************************************
     * Function:
     *      bool MiMAC_SendPacket(  MAC_TRANS_PARAM transParam,
     *                              uint8_t *MACPayload, uint8_t MACPayloadLen)
     *
     * Summary:
     *      This function transmit a packet
     *
     * Description:        
     *      This is the primary MiMAC interface for the protocol layer to 
     *      send a packet. Input parameter transParam configure the way
     *      to transmit the packet.
     *
     * PreCondition:    
     *      MiMAC initialization has been done. 
     *
     * Parameters: 
     *      MAC_TRANS_PARAM transParam -    The struture to configure the transmission way
     *      uint8_t * MACPaylaod -             Pointer to the buffer of MAC payload
     *      uint8_t MACPayloadLen -            The size of the MAC payload
     *
     * Returns: 
     *      A boolean to indicate if a packet has been received by the RF transceiver.
     *
     * Example:
     *      <code>
     *      MiMAC_SendPacket(transParam, MACPayload, MACPayloadLen);
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/   
     bool MiMAC_SendPacket( INPUT MAC_TRANS_PARAM transParam,
                        INPUT uint8_t *MACPayload,
                        INPUT uint8_t MACPayloadLen)
    {
        uint8_t i;
        uint8_t TxIndex;
        uint16_t crc;
      
        if( MACPayloadLen > TX_BUFFER_SIZE )
        {
            return false;
        }

        #if defined(INFER_DEST_ADDRESS)
            transParam.flags.bits.destPrsnt = 0;
        #else
            transParam.flags.bits.destPrsnt = (transParam.flags.bits.broadcast) ? 0:1;
        #endif
        
        #if !defined(SOURCE_ADDRESS_ABSENT)
            transParam.flags.bits.sourcePrsnt = 1;
        #endif
        
        if( transParam.flags.bits.packetType == PACKET_TYPE_COMMAND )
        {
            transParam.flags.bits.sourcePrsnt = 1;
        }
      
        MACTxBuffer[0] = transParam.flags.Val;
        MACTxBuffer[1] = MACSeq++;
        crc = CRC16((uint8_t *)MACTxBuffer, 2, 0);
        
        TxIndex = 2;
        
        if( transParam.flags.bits.destPrsnt )
        {   
            for(i = 0; i < MACInitParams.actionFlags.bits.PAddrLength; i++)
            {
                MACTxBuffer[TxIndex++] = transParam.DestAddress[i];
            }
        }
        if( transParam.flags.bits.broadcast == 0 )
        {
            crc = CRC16(transParam.DestAddress, MACInitParams.actionFlags.bits.PAddrLength, crc);
        }
    
        if( transParam.flags.bits.sourcePrsnt )
        {   
            for(i = 0; i < MACInitParams.actionFlags.bits.PAddrLength; i++)
            {
                MACTxBuffer[TxIndex++] = MACInitParams.PAddress[i];
            }
            crc = CRC16((uint8_t *)&(MACTxBuffer[TxIndex-MACInitParams.actionFlags.bits.PAddrLength]), MACInitParams.actionFlags.bits.PAddrLength, crc);
        }
    
        #if defined(ENABLE_SECURITY)
            if( transParam.flags.bits.secEn )
            {
    
                for(i = 0; i < 4; i++)
                {
                    MACTxBuffer[TxIndex++] = OutgoingFrameCounter.v[i];
                }
                OutgoingFrameCounter.Val++;
                #if defined(ENABLE_NETWORK_FREEZER)
                    if( (OutgoingFrameCounter.v[0] == 0) && ((OutgoingFrameCounter.v[1] & 0x03) == 0) )
                    {
                        nvmPutOutFrameCounter(OutgoingFrameCounter.v);
                    }    
                #endif
                MACTxBuffer[TxIndex++] = KEY_SEQUENCE_NUMBER;
    
                {
                    uint8_t headerLen;
                    
                    headerLen = TxIndex;
                    
                    for(i = 0; i < MACPayloadLen; i++)
                    {
                        MACTxBuffer[TxIndex++] = MACPayload[i];
                    }
                    
                    #if SECURITY_LEVEL == SEC_LEVEL_CTR
                        {
                            uint8_t nounce[BLOCK_SIZE];
                            
                            for(i = 0; i < BLOCK_SIZE; i++)
                            {
                                if( i < TxIndex )
                                {
                                    nounce[i] = MACTxBuffer[i];
                                }
                                else
                                {
                                    nounce[i] = 0;
                                }
                            }
                            CTR(&(MACTxBuffer[headerLen]), MACPayloadLen, key, nounce);
                        }
                    #elif (SECURITY_LEVEL == SEC_LEVEL_CCM_64) || (SECURITY_LEVEL == SEC_LEVEL_CCM_32) || (SECURITY_LEVEL == SEC_LEVEL_CCM_16)
                        CCM_Enc((uint8_t *)MACTxBuffer, headerLen, MACPayloadLen, key);
                        TxIndex += SEC_MIC_LEN;
                    #elif (SECURITY_LEVEL == SEC_LEVEL_CBC_MAC_64) || (SECURITY_LEVEL == SEC_LEVEL_CBC_MAC_32) || (SECURITY_LEVEL == SEC_LEVEL_CBC_MAC_16)
                        CBC_MAC(MACTxBuffer, TxIndex, key, &(MACTxBuffer[TxIndex]));
                        TxIndex += SEC_MIC_LEN;
                    #endif
                    
                    crc = CRC16((uint8_t *)&(MACTxBuffer[headerLen-5]), (MACPayloadLen + SEC_MIC_LEN + 5), crc);
                }
            }
            else
    
        #endif
    
        {
            for(i = 0; i < MACPayloadLen; i++)
            {
                MACTxBuffer[TxIndex++] = MACPayload[i];
            }
            
            crc = CRC16(MACPayload, MACPayloadLen, crc);
        }    
    
    
        MACTxBuffer[TxIndex++] = (uint8_t)(crc>>8);
        MACTxBuffer[TxIndex++] = (uint8_t)crc;
          
        return TxPacket(TxIndex, MACInitParams.actionFlags.bits.CCAEnable);
    }
    

    bool MiMAC_ReceivedPacket(void)
    {
        uint8_t i;
        MIWI_TICK currentTick;
        
        if( RF_INT_PIN == 0 )
        {
            RFIF = 1;
        }
        
        #if defined(ENABLE_ACK) && defined(ENABLE_RETRANSMISSION)
            for(i = 0; i < ACK_INFO_SIZE; i++)
            {
                currentTick = MiWi_TickGet();
                if( AckInfo[i].Valid && (currentTick.Val > AckInfo[i].startTick.Val) && 
                    (MiWi_TickGetDiff(currentTick, AckInfo[i].startTick) > ONE_SECOND) )
                {
                    AckInfo[i].Valid = false;
                }
            }
        #endif
            
        if( ReceivedBankIndex != 0xFF )
        {
            return false;
        }
       
        for(i = 0; i < BANK_SIZE; i++)
        {
            if( RxPacket[i].flags.bits.Valid )
            {
                uint8_t PayloadIndex;
                uint8_t j;

                MACRxPacket.flags.Val = RxPacket[i].Payload[0];
                MACRxPacket.PayloadLen = RxPacket[i].PayloadLen;
                PayloadIndex = 2;
                
                if( MACRxPacket.flags.bits.destPrsnt )
                {
                    PayloadIndex += MACInitParams.actionFlags.bits.PAddrLength;
                }
                
                if( MACRxPacket.flags.bits.sourcePrsnt )
                {
                    MACRxPacket.SourceAddress = (uint8_t *)&(RxPacket[i].Payload[PayloadIndex]);
                    PayloadIndex += MACInitParams.actionFlags.bits.PAddrLength;
                }
                else
                {
                    MACRxPacket.SourceAddress = NULL;
                }
                
                #if defined(ENABLE_SECURITY)
                    if( MACRxPacket.flags.bits.secEn )
                    {
                        // check key sequence number first
                        if( KEY_SEQUENCE_NUMBER != RxPacket[i].Payload[PayloadIndex+4] )    
                        {
                            RxPacket[i].flags.Val = 0;
                            return false;
                        }
                        
                        // check frame counter now
                        if( MACRxPacket.flags.bits.sourcePrsnt )
                        {
    
                            for(j = 0; j < CONNECTION_SIZE; j++)
                            {
                                if( (ConnectionTable[j].status.bits.isValid) && 
                                    isSameAddress(ConnectionTable[j].Address, MACRxPacket.SourceAddress) )
                                {
                                    break;
                                }
                            }
                            if( j < CONNECTION_SIZE )
                            {
                                API_UINT32_UNION FrameCounter;
                                uint8_t k;
                                
                                for(k = 0; k < 4; k++)
                                {
                                    FrameCounter.v[k] = RxPacket[i].Payload[PayloadIndex+k];
                                }
                                
                                if( IncomingFrameCounter[j].Val > FrameCounter.Val )
                                {
                                    RxPacket[i].flags.Val = 0;
                                    return false;
                                }
                                else
                                {
                                    IncomingFrameCounter[j].Val = FrameCounter.Val;
                                }
                            }
                        }
                        
                        // now decrypt the data
                        PayloadIndex += 5;      // bypass the frame counter and key sequence number
                        
                        #if SECURITY_LEVEL == SEC_LEVEL_CTR
                            {
                                uint8_t nounce[BLOCK_SIZE];
                                
                                for(j = 0; j < BLOCK_SIZE; j++)
                                {
                                    if( j < PayloadIndex )
                                    {
                                        nounce[j] = RxPacket[i].Payload[j];
                                    }
                                    else
                                    {
                                        nounce[j] = 0;
                                    }
                                }
                                
                                CTR(&(RxPacket[i].Payload[PayloadIndex]), (RxPacket[i].PayloadLen - PayloadIndex), key, nounce); 
                            }
                        #elif (SECURITY_LEVEL == SEC_LEVEL_CCM_64) || (SECURITY_LEVEL == SEC_LEVEL_CCM_32) || (SECURITY_LEVEL == SEC_LEVEL_CCM_16)

                            if(CCM_Dec((uint8_t *)RxPacket[i].Payload, PayloadIndex, RxPacket[i].PayloadLen-PayloadIndex, key) == false)
                            {
                                RxPacket[i].flags.Val = 0;
                                return false;
                            }

                        #elif (SECURITY_LEVEL == SEC_LEVEL_CBC_MAC_64) || (SECURITY_LEVEL == SEC_LEVEL_CBC_MAC_32) || (SECURITY_LEVEL == SEC_LEVEL_CBC_MAC_16)
                            {
                                uint8_t MIC[BLOCK_SIZE];
                                
                                CBC_MAC(RxPacket[i].Payload, (RxPacket[i].PayloadLen - SEC_MIC_LEN), key, MIC);
                                for(j = 0; j < SEC_MIC_LEN; j++)
                                {
                                    if( MIC[j] != RxPacket[i].Payload[RxPacket[i].PayloadLen-SEC_MIC_LEN+j] )
                                    {
                                        RxPacket[i].flags.Val = 0;
                                        return false;
                                    }    
                                }
                            }
                        #endif
                        MACRxPacket.PayloadLen -= (PayloadIndex + SEC_MIC_LEN);
                    }   
                    else
                    {
                        MACRxPacket.PayloadLen -= PayloadIndex;
                    }
    
                #else
                
                    MACRxPacket.PayloadLen -= PayloadIndex;
                
                #endif
                
                MACRxPacket.Payload = (uint8_t *)&(RxPacket[i].Payload[PayloadIndex]);
                #if !defined(TARGET_SMALL)
                    if( RxPacket[i].flags.bits.RSSI )
                    {
                        #if RSSI_THRESHOLD == RSSI_THRESHOLD_103
                            MACRxPacket.RSSIValue = 1;
                        #elif RSSI_THRESHOLD == RSSI_THRESHOLD_97
                            MACRxPacket.RSSIValue = 7;
                        #elif RSSI_THRESHOLD == RSSI_THRESHOLD_91
                            MACRxPacket.RSSIValue = 13;
                        #elif RSSI_THRESHOLD == RSSI_THRESHOLD_85
                            MACRxPacket.RSSIValue = 19;
                        #elif RSSI_THRESHOLD == RSSI_THRESHOLD_79
                            MACRxPacket.RSSIValue = 25;
                        #elif RSSI_THRESHOLD == RSSI_THRESHOLD_73
                            MACRxPacket.RSSIValue = 31;
                        #endif
                    }
                    else
                    {
                        MACRxPacket.RSSIValue = 0;
                    }
                    MACRxPacket.LQIValue = RxPacket[i].flags.bits.DQD;
                #endif
                ReceivedBankIndex = i;
                
                return true;
            }
        }
    
        return false;
    }
    
    /************************************************************************************
     * Function:
     *      void MiMAC_DiscardPacket(void)
     *
     * Summary:
     *      This function discard the current packet received from the RF transceiver
     *
     * Description:        
     *      This is the primary MiMAC interface for the protocol layer to 
     *      discard the current packet received from the RF transceiver.
     *
     * PreCondition:    
     *      MiMAC initialization has been done. 
     *
     * Parameters: 
     *      None
     *
     * Returns: 
     *      None
     *
     * Example:
     *      <code>
     *      if( true == MiMAC_ReceivedPacket() )
     *      {
     *          // handle the raw data from RF transceiver
     * 
     *          // discard the current packet
     *          MiMAC_DiscardPacket();
     *      }
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/  
    void MiMAC_DiscardPacket(void)
    {
        if( ReceivedBankIndex < BANK_SIZE )
        {
            RxPacket[ReceivedBankIndex].flags.Val = false;
            ReceivedBankIndex = 0xFF;    
        }
    }
    
    
    #if defined(ENABLE_ED_SCAN)
        /************************************************************************************
         * Function:
         *      uint8_t MiMAC_ChannelAssessment(uint8_t AssessmentMode)
         *
         * Summary:
         *      This function perform the noise detection on current operating channel
         *
         * Description:        
         *      This is the primary MiMAC interface for the protocol layer to 
         *      perform the noise detection scan. Not all assessment modes are supported
         *      for all RF transceivers.
         *
         * PreCondition:    
         *      MiMAC initialization has been done.  
         *
         * Parameters: 
         *      uint8_t AssessmentMode -   The mode to perform noise assessment. The possible
         *                              assessment modes are
         *                              * CHANNEL_ASSESSMENT_CARRIER_SENSE Carrier sense detection mode
         *                              * CHANNEL_ASSESSMENT_ENERGY_DETECT Energy detection mode
         *
         * Returns: 
         *      A byte to indicate the noise level at current channel.
         *
         * Example:
         *      <code>
         *      NoiseLevel = MiMAC_ChannelAssessment(CHANNEL_ASSESSMENT_CARRIER_SENSE);
         *      </code>
         *
         * Remarks:    
         *      None
         *
         *****************************************************************************************/          
        uint8_t MiMAC_ChannelAssessment(uint8_t AssessmentMode)
        {
            uint8_t i;
            uint8_t j;
            uint8_t k;
            uint8_t result[6] = {222, 186, 150, 114, 78, 42};
            uint8_t count;
            
            RegisterSet(PMCREG | 0x0080);    // turn on the receiver 
            for(i = 0; i < 6; i++)
            {
                RegisterSet(((RXCREG & 0xFF80) + 5-i) | getReceiverBW());

                count = 0;
                for(j = 0; j < 0xFE; j++)
                {
                    StatusRead();
                    if( AssessmentMode == CHANNEL_ASSESSMENT_CARRIER_SENSE )
                    {
                        count += TransceiverStatus.bits.DQD;
                    }
                    else if( AssessmentMode == CHANNEL_ASSESSMENT_ENERGY_DETECT )
                    {
                        count += TransceiverStatus.bits.RSSI_ATS;
                    }
                    PHY_CS = 1;
                    for(k = 0; k < 0x1F; k++) {}
                }
                
                #if defined(__18CXX)
                    if( OSCTUNEbits.PLLEN )
                    {
                        k = 10;
                    }
                    else
                #endif
                {
                    k = 5;
                }
                
                if( count > k * (7-(getReceiverBW()>>5)) )
                {
                    RegisterSet(RXCREG | getReceiverBW());
                    return result[i];
                }
            }

            RegisterSet(RXCREG | getReceiverBW());
            return 0;
        }
    #endif
    
    
    #if defined(ENABLE_SLEEP)
        /************************************************************************************
         * Function:
         *      bool MiMAC_PowerState(uint8_t PowerState)
         *
         * Summary:
         *      This function puts the RF transceiver into sleep or wake it up
         *
         * Description:        
         *      This is the primary MiMAC interface for the protocol layer to 
         *      set different power state for the RF transceiver. There are minimal 
         *      power states defined as deep sleep and operating mode. Additional
         *      power states can be defined for individual RF transceiver depends
         *      on hardware design.
         *
         * PreCondition:    
         *      MiMAC initialization has been done. 
         *
         * Parameters: 
         *      uint8_t PowerState -   The power state of the RF transceiver to be set to.
         *                          The minimum definitions for all RF transceivers are
         *                          * POWER_STATE_DEEP_SLEEP RF transceiver deep sleep mode.
         *                          * POWER_STATE_OPERATE RF transceiver operating mode.
         * Returns: 
         *      A boolean to indicate if chaning power state of RF transceiver is successful.
         *
         * Example:
         *      <code>
         *      // Put RF transceiver into sleep
         *      MiMAC_PowerState(POWER_STATE_DEEP_SLEEP);
         *      // Put MCU to sleep
         *      Sleep();
         *      // Wake up the MCU by WDT, external interrupt or any other means
         *
         *      // Wake up the RF transceiver
         *      MiMAC_PowerState(POWER_STATE_OPERATE); 
         *      </code>
         *
         * Remarks:    
         *      None
         *
         *****************************************************************************************/ 
        bool MiMAC_PowerState(INPUT uint8_t PowerState)
        {
            switch(PowerState)
            {
                case POWER_STATE_DEEP_SLEEP:
                    {   
                        RegisterSet(FIFORSTREG);                  // turn off FIFO
                        RegisterSet(GENCREG);                   // disable FIFO, TX_latch
                        RegisterSet(PMCREG);                    // turn off both receiver and transmitter
                        StatusRead();                           // reset all non latched interrupts
                        nFSEL = 1;
                    }
                    break;
                
                case POWER_STATE_OPERATE:
                    {
                        uint8_t i;
                        
                        RegisterSet(PMCREG | 0x0008);           // switch on oscillator
                        DELAY_ms(10);                            // oscillator start up time 2~7 ms. Use 10ms here
                        #if defined(ENABLE_ACK)
                            for(i = 0; i < ACK_INFO_SIZE; i++)
                            {
                                AckInfo[i].Valid = false;
                            }
                        #endif
                    }
                    break;
                    
                default:
                    return false;
            }
            return true;
        }
    #endif
    
    
    #if defined(__XC8)
        void interrupt HighISR(void)
    #elif defined(__18CXX)
        #pragma interruptlow HighISR
        void HighISR(void)
    #elif defined(__dsPIC30F__) || defined(__dsPIC33F__) || defined(__PIC24F__) || defined(__PIC24FK__) || defined(__PIC24H__)
        void _ISRFAST __attribute__((interrupt, auto_psv)) _INT1Interrupt(void)
    #elif defined(__PICC__)
        #pragma interrupt_level 0
        void interrupt HighISR(void)
    #elif defined(__PIC32MX__)
        void __ISR(_EXTERNAL_1_VECTOR, ipl4) _INT1Interrupt(void)
    #else
        void _ISRFAST _INT1Interrupt(void)
    #endif
    {   
        if( RFIE && RFIF )
        {
            PHY_CS = 0;
            #if defined(__dsPIC30F__) || defined(__dsPIC33F__) || defined(__PIC24F__) || defined(__PIC24FK__) || defined(__PIC24H__) || defined(__PIC32MX__)
                Nop();          // add Nop here to make sure PIC24 family MCU can respond to the SPI_SDI change
            #endif        
                         
            if( SPI_SDI == 1 )
            {
                uint8_t RxPacketPtr;
                uint8_t PacketLen;
                uint8_t BankIndex;
                uint16_t counter;
                bool bAck;
                uint8_t ackPacket[4];
                
                // There is data in RX FIFO
                PHY_CS = 1;
                nFSEL = 0;                   // FIFO selected
            
                PacketLen = SPIGet();

                for(BankIndex = 0; BankIndex < BANK_SIZE; BankIndex++)
                {
                    if( RxPacket[BankIndex].flags.bits.Valid == false )
                    {
                        break;
                    }
                }
                
                if( PacketLen == 4 )        // may be an acknowledgement
                {
                    bAck = true;
                }
                else
                {
                    bAck = false;
                }

                if( PacketLen >= RX_PACKET_SIZE || PacketLen == 0 || (BankIndex >= BANK_SIZE && (bAck==false)) )
                {
IGNORE_HERE:       
                    nFSEL = 1;                                       // bad packet len received
                    RFIF = 0;
                    RegisterSet(PMCREG);                            // turn off the transmitter and receiver
                    RegisterSet(FIFORSTREG);                          // reset FIFO
                    RegisterSet(GENCREG);                           // disable FIFO, TX_latch
                    RegisterSet(GENCREG | 0x0040);                  // enable FIFO
                    RegisterSet(PMCREG | 0x0080);                   // turn on receiver
                    RegisterSet(FIFORSTREG | 0x0002);                 // FIFO synchron latch re-enabled
                    goto RETURN_HERE;
                }
                
                RxPacketPtr = 0;
                counter = 0;

                while(1)
                {
                    if(FINT == 1)
                    {
                        if( bAck )
                        {
                            ackPacket[RxPacketPtr++] = SPIGet();
                        }
                        else
                        {
                            RxPacket[BankIndex].Payload[RxPacketPtr++] = SPIGet();
                        }
                        
                        if( RxPacketPtr >= PacketLen ) //RxPacket[BankIndex].PayloadLen )
                        {
                            uint16_t received_crc;
                            uint16_t calculated_crc;
                            uint8_t i;

                            StatusRead();

                            if( TransceiverStatus.bits.DQD == 0 )
                            {
                                goto IGNORE_HERE;
                            }
                            
                            nFSEL = 1;
                            RegisterSet(FIFORSTREG);
                           
                            
                            if( bAck )
                            {
                                #if defined(ENABLE_ACK)
                                if( ( ackPacket[0] & PACKET_TYPE_MASK ) == PACKET_TYPE_ACK )
                                {
                                    received_crc  = ((uint16_t)ackPacket[3]) + (((uint16_t)ackPacket[2]) << 8);
                                    calculated_crc = CRC16(ackPacket, 2, 0);
                                    if( received_crc != calculated_crc)
                                    {
                                        RxPacketPtr = 0;
                                        RegisterSet(FIFORSTREG | 0x0002);
                                        goto IGNORE_HERE;
                                    }
                                    if( ackPacket[1] == TxMACSeq )
                                    {
                                        hasAck = true;
                                    }
                                    RxPacketPtr = 0;
                                    RegisterSet(FIFORSTREG | 0x0002);
                                    goto RETURN_HERE;
                                }
                                else
                                #endif
                                if( BankIndex >= BANK_SIZE )
                                {
                                    RxPacketPtr = 0;
                                    RegisterSet(FIFORSTREG | 0x0002);
                                    goto IGNORE_HERE;
                                }
                                RxPacket[BankIndex].Payload[0] = ackPacket[0];
                                RxPacket[BankIndex].Payload[1] = ackPacket[1];
                                RxPacket[BankIndex].Payload[2] = ackPacket[2];
                                RxPacket[BankIndex].Payload[3] = ackPacket[3];
                            }
                            
                            RxPacket[BankIndex].PayloadLen = PacketLen;

                            // checking CRC
                            received_crc = ((uint16_t)(RxPacket[BankIndex].Payload[RxPacket[BankIndex].PayloadLen-1])) + (((uint16_t)(RxPacket[BankIndex].Payload[RxPacket[BankIndex].PayloadLen-2])) << 8);
                            if( (RxPacket[BankIndex].Payload[0] & BROADCAST_MASK) || (RxPacket[BankIndex].Payload[0] & DSTPRSNT_MASK) )
                            {
                                calculated_crc = CRC16((uint8_t *)RxPacket[BankIndex].Payload, RxPacket[BankIndex].PayloadLen-2, 0);
                            }
                            else
                            {
                                calculated_crc = CRC16((uint8_t *)RxPacket[BankIndex].Payload, 2, 0);
                                
                                // try full address first
                                calculated_crc = CRC16(MACInitParams.PAddress, MACInitParams.actionFlags.bits.PAddrLength, calculated_crc);
                                calculated_crc = CRC16((uint8_t *)&(RxPacket[BankIndex].Payload[2]), RxPacket[BankIndex].PayloadLen-4, calculated_crc);
                            }
                            
                            if( received_crc != calculated_crc )
                            {
                                RxPacketPtr = 0;
                                RxPacket[BankIndex].PayloadLen = 0;
                                RegisterSet(FIFORSTREG | 0x0002);            // FIFO synchron latch re-enable 

                                goto IGNORE_HERE;
                            }
                            
                            #if !defined(TARGET_SMALL)
                                RxPacket[BankIndex].flags.bits.DQD = 1;
                                RxPacket[BankIndex].flags.bits.RSSI = TransceiverStatus.bits.RSSI_ATS;
                            #endif
                            
                            // send ack / check ack
                            #if defined(ENABLE_ACK1)
                                if( ( RxPacket[BankIndex].Payload[0] & PACKET_TYPE_MASK ) == PACKET_TYPE_ACK )  // acknowledgement
                                {
                                    if( RxPacket[BankIndex].Payload[1] == TxMACSeq )
                                    {
                                        hasAck = true;
                                    }
        
                                    RxPacketPtr = 0;
                                    RxPacket[BankIndex].PayloadLen = 0;
                                    RegisterSet(FIFORSTREG | 0x0002);
                                }
                                else 
                            #endif
                            {
                                uint8_t ackInfoIndex = 0xFF;
                                
                                if( RxPacket[BankIndex].Payload[0] & DSTPRSNT_MASK )
                                {
                                    for(i = 0; i < MACInitParams.actionFlags.bits.PAddrLength; i++)
                                    {
                                        if( RxPacket[BankIndex].Payload[2+i] != MACInitParams.PAddress[i] )
                                        {
                                            RxPacketPtr = 0;
                                            RxPacket[BankIndex].PayloadLen = 0;
                                            RegisterSet(FIFORSTREG | 0x0002);
                                            goto IGNORE_HERE;
                                        }
                                    }
                                }
    
                                #if defined(ENABLE_ACK)
                                    if( (RxPacket[BankIndex].Payload[0] & ACK_MASK) )  // acknowledgement required
                                    {
                                        RegisterSet(FIFORSTREG | 0x0002);
                                        
                                        for(i = 0; i < 4; i++)
                                        {
                                            ackPacket[i] = MACTxBuffer[i];
                                        }
                                        MACTxBuffer[0] = PACKET_TYPE_ACK | BROADCAST_MASK;   // frame control, ack type + broadcast
                                        MACTxBuffer[1] = RxPacket[BankIndex].Payload[1];     // sequenece number
                                        calculated_crc = CRC16((uint8_t *)MACTxBuffer, 2, 0);
                                        MACTxBuffer[2] = calculated_crc>>8;
                                        MACTxBuffer[3] = calculated_crc;
                                        DELAY_ms(2);
                                        TxPacket(4, false);
                                        RegisterSet(FIFORSTREG);
                                        for(i = 0; i < 4; i++)
                                        {
                                            MACTxBuffer[i] = ackPacket[i];
                                        }
                                    }
                                #endif
                                    
                                #if defined(ENABLE_ACK) && defined(ENABLE_RETRANSMISSION)
                                    for(i = 0; i < ACK_INFO_SIZE; i++)
                                    {
                                        if( AckInfo[i].Valid && (AckInfo[i].Seq == RxPacket[BankIndex].Payload[1]) &&
                                            AckInfo[i].CRC == received_crc )
                                        {
                                            AckInfo[i].startTick = MiWi_TickGet();
                                            break;    
                                        }
                                        if( (ackInfoIndex == 0xFF) && (AckInfo[i].Valid == false) )
                                        {
                                            ackInfoIndex = i;
                                        }
                                    }
                                    if( i >= ACK_INFO_SIZE )
                                    {
                                        if( ackInfoIndex < ACK_INFO_SIZE )
                                        {                                
                                            AckInfo[ackInfoIndex].Valid = true;
                                            AckInfo[ackInfoIndex].Seq = RxPacket[BankIndex].Payload[1];
                                            AckInfo[ackInfoIndex].CRC = received_crc;   
                                            AckInfo[ackInfoIndex].startTick = MiWi_TickGet();
                                        }

                                        RxPacket[BankIndex].PayloadLen -= 2;            // remove CRC
                                        RxPacket[BankIndex].flags.bits.Valid = true;
                                    }
                                #else
                                
                                    RxPacket[BankIndex].PayloadLen -= 2;            // remove CRC
                                    RxPacket[BankIndex].flags.bits.Valid = true;
                                    #if !defined(TARGET_SMALL)
                                        RxPacket[BankIndex].flags.bits.RSSI = TransceiverStatus.bits.RSSI_ATS;
                                        RxPacket[BankIndex].flags.bits.DQD = TransceiverStatus.bits.DQD; 
                                    #endif                      
                                #endif
                                RegisterSet(FIFORSTREG | 0x0002);
    
                            }
                            goto RETURN_HERE;
                        }
                        counter = 0;
                    }
                    else if( counter++ > 0xFFFE )
                    {
                        goto IGNORE_HERE;
                    }
                }
            }
            else            // read out the rest IT bits in case of no FINT
            {
                
                #if defined(HARDWARE_SPI)
                
                    TransceiverStatus.v[0] = SPIGet();
                    
                #else
                
                    TransceiverStatus.bits.RG_FF_IT = SPI_SDI;
                    SPI_SCK = 1;
                    SPI_SCK = 0;
                    
                    TransceiverStatus.bits.POR = SPI_SDI;
                    SPI_SCK = 1;
                    SPI_SCK = 0;
                    
                    TransceiverStatus.bits.RGUR_FFOV = SPI_SDI;
                    SPI_SCK = 1;
                    SPI_SCK = 0;
                    
                    TransceiverStatus.bits.WKUP = SPI_SDI;
                    SPI_SCK = 1;
                    SPI_SCK = 0;
                    
                    TransceiverStatus.bits.EXT = SPI_SDI;
                    SPI_SCK = 1;
                    SPI_SCK = 0;
                    
                    TransceiverStatus.bits.LBD = SPI_SDI;
                    SPI_SCK = 1;
                    SPI_SCK = 0;
                #endif
                
                PHY_CS = 1;
            }
              
RETURN_HERE:
            RFIF = 0;
        }   


        #if defined(__18CXX)
            //check to see if the symbol timer overflowed
            if(TMR_IF)
            {
                if(TMR_IE)
                {
                    /* there was a timer overflow */
                    TMR_IF = 0;
                    timerExtension1++;
                    if(timerExtension1 == 0)
                    {
                        timerExtension2++;
                    }
                }
            }
            
            UserInterruptHandler();
        #endif
    }
    
    
    #if defined(__18CXX) && !defined(__XC8)
        #pragma code highVector=0x08
        void HighVector (void)
        {
            _asm goto HighISR _endasm
        }
        #pragma code /* return to default code section */
    #endif
    
    #if defined(__18CXX) && !defined(__XC8)
        #pragma code lowhVector=0x18
        void LowVector (void)
        {
            _asm goto HighISR _endasm
        }
        #pragma code /* return to default code section */
    #endif

#else
    /*******************************************************************
     * C18 compiler cannot compile an empty C file. define following 
     * bogus variable to bypass the limitation of the C18 compiler if
     * a different transceiver is chosen.
     ******************************************************************/
    extern char bogusVariable;
#endif


