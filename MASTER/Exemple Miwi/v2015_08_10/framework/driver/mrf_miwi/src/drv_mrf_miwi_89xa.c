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


#include "driver/mrf_miwi/drv_mrf_miwi_89xa.h"
#include "driver/mrf_miwi/drv_mrf_miwi_security.h"
#if defined(ENABLE_NVM)
    #include "miwi/miwi_nvm.h"
#endif


    //==============================================================
    // Global variables:
    //==============================================================
   


    volatile RX_PACKET          RxPacket[BANK_SIZE];
    volatile bool IRQ1_Received = 0;
    MACINIT_PARAM               MACInitParams;
    uint8_t                        TxMACSeq;
    uint8_t                        MACSeq;
    uint8_t                        ReceivedBankIndex;
    uint8_t RF_Mode;
    uint8_t RSSIVal;
    uint8_t RSSILock;
    const uint8_t PVALUE[]    = {CHANNEL1_PVALUE, CHANNEL2_PVALUE, CHANNEL3_PVALUE, CHANNEL4_PVALUE, CHANNEL5_PVALUE, CHANNEL6_PVALUE,
                            CHANNEL7_PVALUE, CHANNEL8_PVALUE, CHANNEL9_PVALUE, CHANNEL10_PVALUE, CHANNEL11_PVALUE, CHANNEL12_PVALUE,
                            CHANNEL13_PVALUE, CHANNEL14_PVALUE, CHANNEL15_PVALUE, CHANNEL16_PVALUE, CHANNEL17_PVALUE, CHANNEL18_PVALUE,
                            CHANNEL19_PVALUE, CHANNEL20_PVALUE, CHANNEL21_PVALUE, CHANNEL22_PVALUE, CHANNEL23_PVALUE, CHANNEL24_PVALUE,
                            CHANNEL25_PVALUE, CHANNEL26_PVALUE, CHANNEL27_PVALUE, CHANNEL28_PVALUE, CHANNEL29_PVALUE, CHANNEL30_PVALUE,
			    CHANNEL31_PVALUE, CHANNEL32_PVALUE};
    const uint8_t SVALUE[]    = {CHANNEL1_SVALUE, CHANNEL2_SVALUE, CHANNEL3_SVALUE, CHANNEL4_SVALUE, CHANNEL5_SVALUE, CHANNEL6_SVALUE,
                            CHANNEL7_SVALUE, CHANNEL8_SVALUE, CHANNEL9_SVALUE, CHANNEL10_SVALUE, CHANNEL11_SVALUE, CHANNEL12_SVALUE,
                            CHANNEL13_SVALUE, CHANNEL14_SVALUE, CHANNEL15_SVALUE, CHANNEL16_SVALUE, CHANNEL17_SVALUE, CHANNEL18_SVALUE,
                            CHANNEL19_SVALUE, CHANNEL20_SVALUE, CHANNEL21_SVALUE, CHANNEL22_SVALUE, CHANNEL23_SVALUE, CHANNEL24_SVALUE,
                            CHANNEL25_SVALUE, CHANNEL26_SVALUE, CHANNEL27_SVALUE, CHANNEL28_SVALUE, CHANNEL29_SVALUE, CHANNEL30_SVALUE,
			    CHANNEL31_SVALUE, CHANNEL32_SVALUE};
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
    

    //First time configuration settings for MRF89XA
    const uint16_t InitConfigRegs[] = {
        /* 0 */                     GCONREG | GCONREG_SET, 
        /* 1 */                     DMODREG | DMODREG_SET,
        /* 2 */                     FDEVREG | FREQ_DEV,
        /* 3 */                     BRREG | DATARATE,
        /* 4 */                     FLTHREG | FLTHREG_SET,
        /* 5 */                     FIFOCREG | FIFOCREG_SET,
        /* 6 */                     R1CNTREG | R1CNT,
        /* 7 */                     P1CNTREG | P1CNT,
        /* 8 */                     S1CNTREG | S1CNT,
        /* 9 */                     R2CNTREG,
        /* 10 */                    P2CNTREG,
        /* 11 */                    S2CNTREG,
        /* 12 */                    PACONREG | PACONREG_SET,
        /* 13 */                    FTXRXIREG | FTXRXIREG_SET,                     
        /* 14 */                    FTPRIREG | FTPRIREG_SET,
        /* 15 */                    RSTHIREG | RSTHIREG_SET,
        /* 16 */                    FILCONREG | FILCONREG_SET,
        /* 17 */                    PFILCREG | PFILCREG_SET,
        /* 18 */                    SYNCREG | SYNCREG_SET,
        /* 19 */                    RESVREG | RESVREG_SET,
        /* 20 */                    RSTSREG,
        /* 21 */                    OOKCREG,
        /* 22 */                    SYNCV31REG | SYNCV31REG_SET, // 1st byte of Sync word,
        /* 23 */                    SYNCV23REG | SYNCV23REG_SET, // 2nd byte of Sync word,
        /* 24 */                    SYNCV15REG | SYNCV15REG_SET, // 3rd byte of Sync word,
        /* 25 */                    SYNCV07REG | SYNCV07REG_SET, // 4th byte of Sync word,
        /* 26 */                    TXPARAMREG | TXPARAMREG_SET,
        /* 27 */                    CLKOUTREG | CLKOUTREG_SET,
        /* 28 */                    PLOADREG | PLOADREG_SET,
        /* 29 */                    NADDREG | NADDREG_SET,
        /* 30 */                    PCONREG | PCONREG_SET,
         /* 31 */                   FCRCREG | FCRCREG_SET 
                            };    

    void SetRFMode(uint8_t);
    uint8_t RegisterRead(uint8_t);
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
        uint8_t value;
        value = RegisterRead(FILCONREG>>8);
        return (uint16_t)(((value & 0x0F) + 1) * 25);
    }

    
    /*********************************************************************
     * void RegisterSet(INPUT uint16_t setting)
     *
     * Overview:        
     *              This function access the control register of MRF89XA.
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
        uint8_t IRQ1select = PHY_IRQ1_En;
        #if defined USE_IRQ0_AS_INTERRUPT
            bool IRQ0select = PHY_IRQ0_En;
            PHY_IRQ0_En = 0;
        #endif
        
        PHY_IRQ1_En = 0;
        Config_nCS = 0;
        SPIPut((uint8_t)(setting >> 8));
        SPIPut((uint8_t)setting);
        Config_nCS = 1;
        PHY_IRQ1_En = IRQ1select;
        
        #if defined USE_IRQ0_AS_INTERRUPT
            PHY_IRQ0_En = IRQ0select;
        #endif
    }
    
    /*********************************************************************
     * uint8_t RegisterRead(uint8_t)
     *
     * Overview:        
     *              This function reads back the register values
     *
     * PreCondition:    
     *              SPI needs to be intialised
     *
     * Input:       None   
     *
     * Output:      Returns the register readback value to the calling function       
     *
     * Side Effects: None
     *
     ********************************************************************/
    uint8_t RegisterRead(uint8_t address)
    {
        uint8_t value;
        uint8_t IRQ1select = PHY_IRQ1_En;
        #if defined USE_IRQ0_AS_INTERRUPT
            bool IRQ0select = PHY_IRQ0_En;
            
            PHY_IRQ0_En = 0;
        #endif
        
        PHY_IRQ1_En = 0;
        Config_nCS = 0;
        address = (address|0x40);
        SPIPut(address);
        value = SPIGet();
        Config_nCS = 1;
        PHY_IRQ1_En = IRQ1select;
        
        #if defined USE_IRQ0_AS_INTERRUPT
            PHY_IRQ0_En = IRQ0select;
        #endif
        
        return value;
    }
    

    /*********************************************************************
     * void WriteFIFO(uint8_t Data)
     *
     * Overview:        
     *              This function fills the FIFO
     *
     * PreCondition:    
     *              MRF89XA transceiver has to be properly initialized
     *
     * Input:       
     *              uint8_t   Data - Data to be sent to FIFO.
     *
     * Output:      None
     *
     * Side Effects:    
     *              Fills the fifo
     *
     ********************************************************************/
    void WriteFIFO(uint8_t Data)
    {
        uint8_t IRQ1select = PHY_IRQ1_En;
        #if defined USE_IRQ0_AS_INTERRUPT
            bool IRQ0select = PHY_IRQ0_En;
            
            PHY_IRQ0_En = 0;
        #endif
        
        PHY_IRQ1_En = 0;
        Data_nCS = 0;
        SPIPut(Data);
        Data_nCS = 1;
        PHY_IRQ1_En = IRQ1select;
        
        #if defined USE_IRQ0_AS_INTERRUPT
            PHY_IRQ0_En = IRQ0select;
        #endif
    }

    
    /*********************************************************************
     * bool TxPacket(INPUT uint8_t TxPacketLen, INPUT bool CCA)
     *
     * Overview:        
     *              This function send the packet in the buffer MACTxBuffer
     *
     * PreCondition:    
     *              MRF89XA transceiver has been properly initialized
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
        uint8_t i;
        MIWI_TICK t1, t2;
        #ifdef ENABLE_CCA
            uint8_t CCARetries;
            uint8_t rssival;
        #endif
    
        #if defined(ENABLE_ACK) && defined(ENABLE_RETRANSMISSION)
            uint8_t reTry = RETRANSMISSION_TIMES;
        #endif
               
        #if defined(ENABLE_ACK) && defined(ENABLE_RETRANSMISSION)
            while( reTry-- )
        #endif
        {
            uint8_t allowedTxFailure;
            allowedTxFailure = 0;

Start_Transmitting:
    
            #if defined(ENABLE_ACK)
                hasAck = false;
            #endif
            
            #ifdef ENABLE_CCA
                CCARetries = 0;
        
                if( CCA )
                {
                   
Start_CCA:    
                    rssival = RegisterRead(RSTSREG>>8); 
                    rssival = (rssival>>1);            //Rssi value in dB
                
                    if( rssival > CCA_THRESHOLD )
                    {
                        if(CCARetries++ > CCA_RETRIES )
                        {
                            return false;
                            
                        }
 			DELAY_10us(1);
                        goto Start_CCA;
                    }
                }
            #endif
        
            // Turn off receiver, enable the TX register
            #if defined USE_IRQ0_AS_INTERRUPT
                PHY_IRQ0_En = 0;
            #endif            
            PHY_IRQ1_En = 0;
            SetRFMode(RF_STANDBY);
            RegisterSet(FTXRXIREG | FTXRXIREG_SET | 0x01);	//Resets FIFO (If any thing is present or previous FIFO Overrun occurred then this clears it.
            WriteFIFO(TxPacketLen);    //Fill the length information - this is needed if variable length packet format is chosen
            for(i=0; i< TxPacketLen; i++)
            {
                WriteFIFO(MACTxBuffer[i]);
            }
            SetRFMode(RF_TRANSMITTER);
            #if defined USE_IRQ0_AS_INTERRUPT
                PHY_IRQ0_En = 1;
            #endif            
            PHY_IRQ1_En = 1;
            while((IRQ1_Received == 0) && (PHY_IRQ1 == 0) )
            {
        
            };
    
            //Wait until TX Done interrupt and restore the RF state to standby mode
    
            IRQ1_Received = 0;
            SetRFMode(RF_STANDBY);
            SetRFMode(RF_RECEIVER);
                                                            //Set RF to Receive Mode        
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
                    #if defined(ENABLE_RETRANSMISSION)
                        if(allowedTxFailure++ <= RETRANSMISSION_TIMES)
                            goto Start_Transmitting;
                    #endif
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
     *      bool MiMAC_SetAltAddress(byte *Address, byte *PANID)
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
     *      FALSE.
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

        //Program registers R, P, S and Synthesize the RF
        RegisterSet(R1CNTREG | RVALUE);
        RegisterSet(P1CNTREG | PVALUE[channel]);
        RegisterSet(S1CNTREG | SVALUE[channel]);
        SetRFMode(RF_STANDBY);
        RegisterSet(GCONREG | (GCONREG_SET & 0x1F) | RF_SYNTHESIZER);                
        SetRFMode(RF_SYNTHESIZER);
        RegisterSet(FTPRIREG | (FTPRIREG_SET & 0xFD) | 0x02);
        SetRFMode(RF_STANDBY);
        SetRFMode(RF_RECEIVER);

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
        if( outputPower > TX_POWER_N_8_DB )
        {
            return false;
        }
        RegisterSet(TXPARAMREG | (TXPARAMREG_SET & 0xF0) | (outputPower<<1));    
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
         
        DELAY_ms(20);
        Config_nCS = 1;           // Config select inactive
        Data_nCS = 1;             // Data select inactive
                
    
        MACSeq = TMRL;
        ReceivedBankIndex = 0xFF;
        
        for(i = 0; i < BANK_SIZE; i++)
        {
            RxPacket[i].flags.Val = 0;
        }
        
        #if defined(ENABLE_ACK) && defined(RETRANSMISSION)
            for(i = 0; i < ACK_INFO_SIZE; i++)
            {
                AckInfo[i].Valid = FALSE;
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
                OutgoingFrameCounter.Val = 1;
            #endif
            for(i = 0; i < KEY_SIZE; i++)
            {
                key[i] = mySecurityKey[i];
            }
        #endif
    
    
        //----  configuring the RF link --------------------------------
        for (i = 0 ; i <= 31; i++)
        {
            RegisterSet(InitConfigRegs[i]);
        }
        SetRFMode(RF_STANDBY);
        RegisterSet(GCONREG | (GCONREG_SET & 0x1F) | RF_SYNTHESIZER);                
        SetRFMode(RF_SYNTHESIZER);

        
        /* clear PLL_LOCK flag so we can see it restore on the new frequency */
        RegisterSet(FTPRIREG | (FTPRIREG_SET & 0xFD) | 0x02);
    
        SetRFMode(RF_RECEIVER);
        #if defined USE_IRQ0_AS_INTERRUPT
            PHY_IRQ0 = 0;
            PHY_IRQ0_En = 1;
        #endif
        PHY_IRQ1 = 0;
        PHY_IRQ1_En = 1;        
        return true;
    }
    
/*********************************************************************
 * void SetRFMode(byte mode)
 *
 * Overview:        
 *              This functions sets the MRF89XA transceiver operating mode to sleep, transmit, receive or standby
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 ********************************************************************/     
void SetRFMode(uint8_t mode)
{
    switch (mode) {
        case RF_TRANSMITTER:
            RegisterSet(GCONREG | (GCONREG_SET & 0x1F) | RF_TRANSMITTER);
            RF_Mode = RF_TRANSMITTER;                //RF in TX mode
            break;
        case RF_RECEIVER:
            RegisterSet(GCONREG|(GCONREG_SET & 0x1F) | RF_RECEIVER);
            RF_Mode = RF_RECEIVER;                    //RF in RX mode
            break;
        case RF_SYNTHESIZER:
            RegisterSet(GCONREG | (GCONREG_SET & 0x1F) | RF_SYNTHESIZER);
            RF_Mode = RF_SYNTHESIZER;                //RF in Synthesizer mode
            break;
        case RF_STANDBY:
            RegisterSet(GCONREG | (GCONREG_SET & 0x1F) | RF_STANDBY);
            RF_Mode = RF_STANDBY;                    //RF in standby mode
            break;
        case RF_SLEEP:
            RegisterSet(GCONREG | (GCONREG_SET & 0x1F) | RF_SLEEP);
            RF_Mode = RF_SLEEP;                        //RF in sleep mode
            break;
    } /* end switch (mode) */

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
            
        }
    
        if( transParam.flags.bits.sourcePrsnt )
        {   
            for(i = 0; i < MACInitParams.actionFlags.bits.PAddrLength; i++)
            {
                MACTxBuffer[TxIndex++] = MACInitParams.PAddress[i];
            }
            
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
                    
                    
                }
            }
            else
    
        #endif

        for(i = 0; i < MACPayloadLen; i++)
        {
            MACTxBuffer[TxIndex++] = MACPayload[i];
        }
          
        return TxPacket(TxIndex, MACInitParams.actionFlags.bits.CCAEnable);
    }
    
    
     
    bool MiMAC_ReceivedPacket(void)
    {
        uint8_t i;
        MIWI_TICK currentTick;
            
       
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
                    MACRxPacket.RSSIValue = RSSILock;
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
            uint8_t k;
            uint16_t count=0;
            uint8_t result[6] = {42, 78, 114, 150, 186 ,222};
          
            for(i = 0; i < 0xFF; i++)
            {
                    if( AssessmentMode == CHANNEL_ASSESSMENT_CARRIER_SENSE )
                    {
                        count += ((RegisterRead(RSTSREG>>8)/2));
                    }
                    else if( AssessmentMode == CHANNEL_ASSESSMENT_ENERGY_DETECT )
                    {
                        count  += ((RegisterRead(RSTSREG>>8)/2));

                    }
                    for(k = 0; k < 0xFF; k++) {}
             }
                
             count = (count>>8);
             i = (count/10);
             if(i>6) i = 6;
             if(i<1) i = 1;
             return(result[i-1]);

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
                        SetRFMode(RF_SLEEP);
                        #if defined USE_IRQ0_AS_INTERRUPT
                            PHY_IRQ0_En = 0;
                        #endif
                        PHY_IRQ1_En = 0;
                    }
                    break;
                
                case POWER_STATE_OPERATE:
                    {
                        uint8_t i;
                        
                        SetRFMode(RF_STANDBY); 
                        SetRFMode(RF_RECEIVER);
                        #if defined USE_IRQ0_AS_INTERRUPT          
                            PHY_IRQ0_En = 1;
                        #endif
                        PHY_IRQ1_En = 1;
                        DelayMs(10);                        //delay 10ms
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

#if defined USE_IRQ0_AS_INTERRUPT
#if defined(__XC8)
void interrupt low_priority low_isr(void)
#elif defined(__18CXX)
#pragma interruptlow HighISR
void HighISR(void)
#elif defined(__dsPIC30F__) || defined(__dsPIC33F__) || defined(__PIC24F__) || defined(__PIC24FK__) || defined(__PIC24H__)
void _ISRFAST __attribute__((interrupt, auto_psv)) _INT1Interrupt(void)
#elif defined(__PIC32MX__)
void __ISR(_EXTERNAL_1_VECTOR, ipl4) _INT1Interrupt(void)
#else

void _ISRFAST _INT1Interrupt(void)
#endif

{
    if(PHY_IRQ0 && PHY_IRQ0_En)
        PHY_IRQ0 = 0;

    #if !defined(TARGET_SMALL)
        if(RF_Mode == RF_RECEIVER)
        {
            RSSIVal = (RegisterRead(RSTSREG>>8))>>1;        //Capturing the RSSiVal at SYNC/ADRS match
        }
    #endif
    return;
}
#endif



#if defined(__XC8)
    void interrupt high_isr(void)
#elif defined(__18CXX)
    #pragma interruptlow HighISR
    void HighISR(void)
#elif defined(__dsPIC30F__) || defined(__dsPIC33F__) || defined(__PIC24F__) || defined(__PIC24H__)
    void _ISRFAST __attribute__((interrupt, auto_psv)) _INT2Interrupt(void)
#elif defined(__PICC__)
    #pragma interrupt_level 0
    void interrupt HighISR(void)
#elif defined(__PIC32MX__)
    void __ISR(_EXTERNAL_2_VECTOR, ipl4) _INT2Interrupt(void)
#else
    void _ISRFAST _INT2Interrupt(void)
#endif
{
    if( PHY_IRQ1 && PHY_IRQ1_En )
    {
        if(RF_Mode == RF_RECEIVER)
        {
            uint8_t RxPacketPtr;
            uint8_t PacketLen;
            uint8_t BankIndex;
            uint16_t counter;
            bool bAck;
            uint8_t ackPacket[4];
            #if !defined(USE_IRQ0_AS_INTERRUPT)
                #if !defined(TARGET_SMALL)
                    RSSIVal = (RegisterRead(RSTSREG>>8))>>1;        //Capturing the RSSiVal at SYNC/ADRS match
                #endif
            #endif
            RSSILock = RSSIVal;
            //Extract the Packet Length Information
            Data_nCS = 0;
            PacketLen = SPIGet();
            Data_nCS = 1;

            //get an available bank index for the received packet
            for(BankIndex = 0; BankIndex < BANK_SIZE; BankIndex++)
            {
                if( RxPacket[BankIndex].flags.bits.Valid == false )
                {
                    break;
                }
            }

             // may be an acknowledgement (If packetlen = 2, packet information =
            if( PacketLen == 2 )
            {
                bAck = true;
            }
            else
            {
                bAck = false;
            }

            //discard the packet if one of the following conditions is true
            if( PacketLen >= RX_PACKET_SIZE || PacketLen == 0 || (BankIndex >= BANK_SIZE && (bAck==false)) )
            {
IGNORE_HERE:       
                {
                    uint8_t fifo_stat = RegisterRead(FTXRXIREG>>8);
                    while(fifo_stat & 0x02)
                    {
                        Data_nCS = 0;
                        SPIGet();
                        Data_nCS = 1;
                        fifo_stat = RegisterRead(FTXRXIREG>>8);            //read the contents of the fifo (to clear the FIFO)
                    }
                }
                goto RETURN_HERE;
            }

            RxPacketPtr = 0;
            counter = PacketLen;

            while(counter >= 1)
            {
                counter--;
                if(1)
                {
                    //if ack packet store in memory structure for ack
                    if( bAck )
                    {
                        Data_nCS = 0;
                        ackPacket[RxPacketPtr++] = SPIGet();
                        Data_nCS = 1;
                    }
                    //else use the bank
                    else
                    {
                        Data_nCS = 0;
                        RxPacket[BankIndex].Payload[RxPacketPtr++] = SPIGet();
                        Data_nCS = 1;
                    }

                    //after reading the all the fifo contents
                    if( RxPacketPtr >= PacketLen )
                    {
                        uint8_t i;

                        if( bAck )
                        {
                            #if defined(ENABLE_ACK)
                                if( ( ackPacket[0] & PACKET_TYPE_MASK ) == PACKET_TYPE_ACK )        //verify that the packet format is ACK packet
                                {
                                    if( ackPacket[1] == TxMACSeq )                                    //verify the Sequence number in ACK packet
                                    {
                                        hasAck = true;                                                //indicate hasACK (if valid ack)
                                    }
                                    RxPacketPtr = 0;
                                    goto RETURN_HERE;
                                }
                                else
                            #endif
                            if( BankIndex >= BANK_SIZE )                                        //if banks are not available discard the packet
                            {
                                RxPacketPtr = 0;
                                goto IGNORE_HERE;
                            }
                            RxPacket[BankIndex].Payload[0] = ackPacket[0];                        //else copy the 2 byte contents of the packet in the bank
                            RxPacket[BankIndex].Payload[1] = ackPacket[1];

                        }

                        RxPacket[BankIndex].PayloadLen = PacketLen;                                //set the packet length of the packet


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
                            }
                            else
                        #endif
                        {
                            uint8_t ackInfoIndex = 0xFF;

                            if( RxPacket[BankIndex].Payload[0] & DSTPRSNT_MASK )            //discard the packet if the packet is not for us
                            {
                                for(i = 0; i < MACInitParams.actionFlags.bits.PAddrLength; i++)
                                {
                                    if( RxPacket[BankIndex].Payload[2+i] != MACInitParams.PAddress[i] )
                                    {
                                        RxPacketPtr = 0;
                                        RxPacket[BankIndex].PayloadLen = 0;
                                        goto IGNORE_HERE;
                                    }
                                }
                            }

                            #if defined(ENABLE_ACK)
                                if( (RxPacket[BankIndex].Payload[0] & ACK_MASK) )  // acknowledgement required
                                {

                                    for(i = 0; i < 2; i++)
                                    {
                                        ackPacket[i] = MACTxBuffer[i];
                                    }
                                    MACTxBuffer[0] = PACKET_TYPE_ACK | BROADCAST_MASK;   // frame control, ack type + broadcast
                                    MACTxBuffer[1] = RxPacket[BankIndex].Payload[1];     // sequenece number
                                    PHY_IRQ1 = 0;
                                    TxPacket(2, false);


                                    for(i = 0; i < 2; i++)
                                    {
                                        MACTxBuffer[i] = ackPacket[i];
                                    }
                                }
                            #endif

                            #if defined(ENABLE_ACK) && defined(ENABLE_RETRANSMISSION)
                                for(i = 0; i < ACK_INFO_SIZE; i++)
                                {
                                    if( AckInfo[i].Valid && (AckInfo[i].Seq == RxPacket[BankIndex].Payload[1])  )
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
                                        AckInfo[ackInfoIndex].startTick = MiWi_TickGet();
                                    }


                                    RxPacket[BankIndex].flags.bits.Valid = true;
                                }
                            #else

                                RxPacket[BankIndex].flags.bits.Valid = true;

                            #endif

                        }
                        goto RETURN_HERE;
                    }

                }
            }
        }
        else
        {

            IRQ1_Received = 1; //capture interrupt status
        }

RETURN_HERE:     
        PHY_IRQ1 = 0;

        Nop();
    }

    
        #if defined USE_IRQ0_AS_INTERRUPT
            {
                if(PHY_IRQ0 && PHY_IRQ0_En)
                {
                    PHY_IRQ0 = 0;

                #if !defined(TARGET_SMALL)
                    if(RF_Mode == RF_RECEIVER)
                    {
                        RSSIVal = (RegisterRead(RSTSREG>>8))>>1;        //Capturing the RSSiVal at SYNC/ADRS match
                    }
                #endif
                }
            }
        #endif
    #if defined(__XC8)
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
        return;
}
    
    
   


