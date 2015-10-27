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

#if defined(PROTOCOL_MIWI_PRO)
  

    /************************ VARIABLES ********************************/

    // Scan Duration formula 
    //  60 * (2 ^ n + 1) symbols, where one symbol equals 16us
    #define SCAN_DURATION_0 SYMBOLS_TO_TICKS(120)
    #define SCAN_DURATION_1 SYMBOLS_TO_TICKS(180)
    #define SCAN_DURATION_2 SYMBOLS_TO_TICKS(300)
    #define SCAN_DURATION_3 SYMBOLS_TO_TICKS(540)
    #define SCAN_DURATION_4 SYMBOLS_TO_TICKS(1020)
    #define SCAN_DURATION_5 SYMBOLS_TO_TICKS(1980)
    #define SCAN_DURATION_6 SYMBOLS_TO_TICKS(3900)
    #define SCAN_DURATION_7 SYMBOLS_TO_TICKS(7740)
    #define SCAN_DURATION_8 SYMBOLS_TO_TICKS(15420)
    #define SCAN_DURATION_9 SYMBOLS_TO_TICKS(30780)
    #define SCAN_DURATION_10 SYMBOLS_TO_TICKS(61500)
    #define SCAN_DURATION_11 SYMBOLS_TO_TICKS(122940)
    #define SCAN_DURATION_12 SYMBOLS_TO_TICKS(245820)
    #define SCAN_DURATION_13 SYMBOLS_TO_TICKS(491580)
    #define SCAN_DURATION_14 SYMBOLS_TO_TICKS(983100)
    const ROM uint32_t ScanTime[15] = {SCAN_DURATION_0,SCAN_DURATION_1,SCAN_DURATION_2,SCAN_DURATION_3,
        SCAN_DURATION_4,SCAN_DURATION_5,SCAN_DURATION_6,SCAN_DURATION_7,SCAN_DURATION_8,SCAN_DURATION_9,
        SCAN_DURATION_10, SCAN_DURATION_11, SCAN_DURATION_12, SCAN_DURATION_13,SCAN_DURATION_14
    };
    
    // permanent address definition
    #if MY_ADDRESS_LENGTH == 8
        uint8_t myLongAddress[MY_ADDRESS_LENGTH] = {EUI_0,EUI_1,EUI_2,EUI_3, EUI_4, EUI_5,EUI_6,EUI_7};
    #elif MY_ADDRESS_LENGTH == 7
        uint8_t myLongAddress[MY_ADDRESS_LENGTH] = {EUI_0,EUI_1,EUI_2,EUI_3, EUI_4, EUI_5,EUI_6};
    #elif MY_ADDRESS_LENGTH == 6
        uint8_t myLongAddress[MY_ADDRESS_LENGTH] = {EUI_0,EUI_1,EUI_2,EUI_3, EUI_4, EUI_5};
    #elif MY_ADDRESS_LENGTH == 5
        uint8_t myLongAddress[MY_ADDRESS_LENGTH] = {EUI_0,EUI_1,EUI_2,EUI_3, EUI_4};
    #elif MY_ADDRESS_LENGTH == 4
        uint8_t myLongAddress[MY_ADDRESS_LENGTH] = {EUI_0,EUI_1,EUI_2,EUI_3};
    #elif MY_ADDRESS_LENGTH == 3
        uint8_t myLongAddress[MY_ADDRESS_LENGTH] = {EUI_0,EUI_1,EUI_2};
    #elif MY_ADDRESS_LENGTH == 2
        uint8_t myLongAddress[MY_ADDRESS_LENGTH] = {EUI_0,EUI_1};
    #endif

    uint8_t            currentChannel = 0;             // current operating channel for the device
    uint8_t            ConnMode = 0;
    typedef union
    {
        uint8_t        Val;
        struct
        {
            uint8_t    Sleep           :1;
            uint8_t    Role            :2;
            uint8_t    Security        :1;
            uint8_t    ConnMode        :2;
            uint8_t    CoordCap        :1;
        }bits;
    } MIWI_PRO_CAPACITY_INFO;    
    MIWI_PRO_CAPACITY_INFO MiWiPROCapacityInfo;        
    RECEIVED_MESSAGE  rxMessage;                    // structure to store information for the received packet

    uint8_t    ActiveScanResultIndex;
    ACTIVE_SCAN_RESULT ActiveScanResults[ACTIVE_SCAN_RESULT_SIZE];  // The results for active scan, including
                                                                    // the PAN identifier, signal strength and 
                                                                    // operating channel
                                                               

    #ifdef ENABLE_SLEEP
        MIWI_TICK DataRequestTimer;
    #endif
    
    uint8_t            TxData;
    #if defined(__18CXX) && !defined(__XC8)
        #pragma udata TRX_BUFFER
    #endif
        #if (TX_BUFFER_SIZE + MIWI_PRO_HEADER_LEN) > 112
            uint8_t TxBuffer[112];
        #else
            uint8_t TxBuffer[TX_BUFFER_SIZE+MIWI_PRO_HEADER_LEN];
        #endif
        
        #if defined(ENABLE_SECURITY)
            API_UINT32_UNION IncomingFrameCounter[CONNECTION_SIZE];
        #endif
    #if defined(__18CXX) && !defined(__XC8)
        #pragma udata
    #endif
    
    API_UINT16_UNION myShortAddress;
    API_UINT16_UNION myPANID;
    uint8_t myParent;
    
    // temporary global variables
    uint8_t tempLongAddress[MY_ADDRESS_LENGTH];
    API_UINT16_UNION tempShortAddress;
    API_UINT16_UNION tempPANID;

    // MiWiPRO acknowledgement related
    uint8_t AcknowledgementSeq = 0xFF;
    API_UINT16_UNION AcknowledgementAddr;
    uint8_t MiWiPROSeqNum;
    #if defined(ENABLE_MIWI_PRO_ACKNOWLEDGEMENT)
        BOOL MiWiPROAckRequired = true;
    #else
        BOOL MiWiPROAckRequired = false;
    #endif
    
    
    uint8_t defaultHops = MAX_HOPS;

    MIWI_PRO_STATE_MACHINE MiWiPROStateMachine;
    CONNECTION_STATUS tempNodeStatus;
    
    // state machine to track saving of particluar
    // network related info into NVM
    #if defined(NWK_ROLE_COORDINATOR)
        union _MIWI_PRO_NVM_STATUS
        {
            uint8_t Val;
            struct
            {
                uint8_t    saveFamilyTree          :1;
                uint8_t    saveRoutingTable        :1;
                uint8_t    saveNeighborTable       :1;
                uint8_t    saveConnectionTable     :1;
            } bits;
        } MiWiPRONVMStatus;
    #endif
    
    #ifdef NWK_ROLE_COORDINATOR
        MIWI_PRO_STATUS MiWiPROStatus; 
    
        MIWI_TICK backgroundTaskTick;
        
        #if defined(ENABLE_INDIRECT_MESSAGE)
            #if defined(__18CXX) && !defined(__XC8)
                #pragma udata INDIRECT_BUFFER
            #endif
            INDIRECT_MESSAGE indirectMessages[INDIRECT_MESSAGE_SIZE];
            #if defined(__18CXX) && !defined(__XC8)
                #pragma udata
            #endif
        #endif

        // Random send buffer for broadcast
        #if defined(__18CXX) && !defined(__XC8)
            #pragma udata RANDOM_SEND_BUFFER
        #endif
        struct
        {
            MIWI_TICK StartTick;
            MAC_TRANS_PARAM MTP;
            uint8_t RandomDelayInMilliSecond;
            uint8_t RandomSendTxData;
            #if (TX_BUFFER_SIZE + MIWI_PRO_HEADER_LEN) > 112
                uint8_t RandomSendTxBuffer[112];
            #else
                uint8_t RandomSendTxBuffer[TX_BUFFER_SIZE+MIWI_PRO_HEADER_LEN];
            #endif
        } RandomSendInfo;        
        
        #if defined(ENABLE_FREQUENCY_AGILITY)
            uint8_t EnergyScanResults[32];
        #endif
        
        // Routing related information
        uint8_t RoutingTable[NUM_COORDINATOR/8];
        uint8_t RouterFailures[NUM_COORDINATOR];
        uint8_t FamilyTree[NUM_COORDINATOR];
        uint8_t NeighborRoutingTable[NUM_COORDINATOR][NUM_COORDINATOR/8];
        #if defined(__18CXX) && !defined(__XC8)
            #pragma udata
        #endif
        
        #if defined(ENABLE_ROUTING_UPDATE)
            MIWI_TICK RoutingUpdateTick;
            uint8_t RoutingUpdateCounter[NUM_COORDINATOR];
        #endif
        
        uint8_t role;
        
    #endif
    
    
    #if defined(ENABLE_FREQUENCY_AGILITY) 
        struct
        {
            union
            {
                uint8_t Val;
                struct
                {
                    uint8_t EnergyScan         :1;
                    uint8_t ChangeChannel      :1;
                    uint8_t DisputeChannel     :1;
                    uint8_t SuggestChannel     :5;
                } bits;    
            } status;    
            API_UINT32_UNION   ChannelMap;
            MIWI_TICK FA_StatTick;
        } FrequencyAgilityInfo;    
    #endif
    
    
    OPEN_SOCKET openSocketInfo;

    MAC_TRANS_PARAM MTP;
    MAC_RECEIVED_PACKET MACRxPacket;
 
    #if defined(__18CXX) && !defined(__XC8)
        #pragma udata __CONNECTION_TABLE
    #endif
    CONNECTION_ENTRY    ConnectionTable[CONNECTION_SIZE]; 
    #if defined(__18CXX) && !defined(__XC8)
        #pragma udata
    #endif

    struct _PACKET_RECORD
    {
        API_UINT16_UNION    AltSourceAddr;
        uint8_t        MiWiPROSeq;
        uint8_t        RxCounter;
        MIWI_TICK   StartTick;
    } PacketRecords[PACKET_RECORD_SIZE];
    
    #if defined(ENABLE_TIME_SYNC)
        #if defined(ENABLE_SLEEP)
            API_UINT16_UNION WakeupTimes;
            API_UINT16_UNION CounterValue;
        #elif defined(ENABLE_INDIRECT_MESSAGE)
            uint8_t TimeSyncSlot = 0;
            MIWI_TICK TimeSyncTick;
            MIWI_TICK TimeSlotTick;
        #endif
    #endif
    
    extern uint8_t     AdditionalNodeID[];             // the additional information regarding the device
                                                    // that would like to share with the peer on the 
                                                    // other side of P2P connection. This information 
                                                    // is applicaiton specific. 

    
    /************************ FUNCTIONS ********************************/
    
    #if defined(IEEE_802_15_4)
        BOOL SendMACPacket(uint8_t *PANID, uint8_t *Address, uint8_t PacketType, uint8_t ModeMask);
    #else
        BOOL SendMACPacket(uint8_t *Address, uint8_t PacketType);
    #endif
    
    BOOL RouteMessage(API_UINT16_UNION PANID, API_UINT16_UNION ShortAddress, BOOL SecEn);
    void StartChannelHopping(INPUT uint8_t OptimalChannel);
    void SendBeacon(void);
    BOOL MultiCast( INPUT uint8_t MultiCastMode, INPUT BOOL SecEn );
    
    #if defined(IEEE_802_15_4)
        BOOL SaveIndirectMessage(INPUT BOOL Broadcast, 
                                 INPUT API_UINT16_UNION DestinationPANID,
                                 INPUT uint8_t *DestinationAddress,
                                 INPUT BOOL isAltAddr, 
                                 INPUT BOOL SecurityEnabled);
    #else
        BOOL SaveIndirectMessage(INPUT BOOL Broadcast, 
                                 INPUT uint8_t *DestinationAddress,
                                 INPUT BOOL isAltAddr, 
                                 INPUT BOOL SecurityEnabled);
    #endif  
    BOOL UnicastShortAddress(uint8_t *DestAddress);

    
    /******************************************************************/
    // C18 compiler cannot optimize the code with a macro. Instead of 
    // calling macro Nop in a big function, we define a wrapper to call
    // Nop in order to be able to optimize the code efficiently.
    /******************************************************************/
    void MacroNop(void)
    {
        Nop();
    }   

    #if !defined(TARGET_SMALL)
        /******************************************************************/
        // Function:        void BroadcastJitter(uint8_t range)
        //
        // PreCondition:    None
        //  
        // Input:           range   - The range of random delay in milli 
        //                            second.
        // 
        // Output:          None
        //
        // Side Effect:     A random delay between 0 to range in milli
        //                  second
        //
        // Overview:        This function introduce a random delay between
        //                  0 to range in millisecond. This delay helps
        //                  multiple devices to rebroadcast the message 
        //                  at different time, thus lower the possibility
        //                  of packet collision in rebroadcast.
        /******************************************************************/
        void BroadcastJitter(uint8_t range)
        {
            uint8_t jitter = TMRL % range;
            MIWI_TICK t1, t2;

            while( jitter > 0 )
            {
                jitter--;
                t1 = MiWi_TickGet();
                while(1)
                {
                    t2 = MiWi_TickGet();
                    if( MiWi_TickGetDiff(t2, t1) > ONE_MILI_SECOND )
                    {
                        break;
                    }
                }        
            }  
        }
    #endif
    
    /*********************************************************************
     * Function:        void MiWiPROTasks( void )
     *
     * PreCondition:    None
     *
     * Input:           None
     *
     * Output:          None
     *
     * Side Effects:    The stack receives, handles, buffers, and transmits 
     *                  packets.  It also handles all of the joining 
     *
     * Overview:        This function maintains the operation of the stack
     *                  It should be called as often as possible.  
     ********************************************************************/
    void MiWiPROTasks(void)
    {
        uint8_t i;
        MIWI_TICK t1, t2;
        
        if( MiMAC_ReceivedPacket() )
        {
            if( MiWiPROStateMachine.bits.RxHasUserData )
            {
                return;
            }
            
            rxMessage.flags.Val = 0;
            rxMessage.flags.bits.broadcast = MACRxPacket.flags.bits.broadcast;
            rxMessage.flags.bits.secEn = MACRxPacket.flags.bits.secEn;
            rxMessage.flags.bits.command = (MACRxPacket.flags.bits.packetType == PACKET_TYPE_COMMAND) ? 1:0;
            rxMessage.flags.bits.srcPrsnt = MACRxPacket.flags.bits.sourcePrsnt;
            if( MACRxPacket.flags.bits.sourcePrsnt )
            {
                #if defined(IEEE_802_15_4)
                    rxMessage.flags.bits.altSrcAddr = MACRxPacket.altSourceAddress;
                #else
                    rxMessage.flags.bits.altSrcAddr = 1;
                #endif
                rxMessage.SourceAddress = MACRxPacket.SourceAddress;
            }
            #if defined(IEEE_802_15_4) 
                rxMessage.SourcePANID.Val = MACRxPacket.SourcePANID.Val;
            #endif
            rxMessage.PacketLQI = MACRxPacket.LQIValue;
            rxMessage.PacketRSSI = MACRxPacket.RSSIValue;
            
            MiWiPROStateMachine.bits.RxHasUserData = 0;
    
            //determine what type of packet it is.
            switch(MACRxPacket.flags.bits.packetType)
            {
                //if it is a data packet
                case PACKET_TYPE_DATA:
                    //The guts of MiWi PRO ...
                    {                      
                        API_UINT16_UNION destPANID;
                        API_UINT16_UNION sourcePANID;
                        API_UINT16_UNION sourceShortAddress;
                        API_UINT16_UNION destShortAddress;
HANDLE_DATA_PACKET:  
                        #if defined(ENABLE_SLEEP)
                            #if defined(ENABLE_BROADCAST_TO_SLEEP_DEVICE)
                                for(i = 0; i < PACKET_RECORD_SIZE; i++)
                                {
                                    if( PacketRecords[i].RxCounter > 0)
                                    {
                                        PacketRecords[i].RxCounter--;
                                    }
                                }
                            #endif
                            
                            // If it is just an empty packet, ignore here.
                            if( MACRxPacket.PayloadLen == 0 )
                            {
                                MiWiPROStateMachine.bits.DataRequesting = 0;
                                break;
                            }
                        #endif
                        
                        if( MACRxPacket.PayloadLen < 10 )
                        {
                            break;
                        }
                        
                        // Load the source and destination address information.
                        destPANID.v[0] = MACRxPacket.Payload[2];
                        destPANID.v[1] = MACRxPacket.Payload[3];
                        destShortAddress.v[0] = MACRxPacket.Payload[4];
                        destShortAddress.v[1] = MACRxPacket.Payload[5];
                        sourcePANID.v[0] = MACRxPacket.Payload[6];
                        sourcePANID.v[1] = MACRxPacket.Payload[7];
                        sourceShortAddress.v[0] = MACRxPacket.Payload[8];
                        sourceShortAddress.v[1] = MACRxPacket.Payload[9];                 
                        
                        //rxMessage.flags.Val = 0;
                        rxMessage.flags.bits.secEn = MACRxPacket.flags.bits.secEn;
                        // if this is a broadcast
                        if(rxMessage.flags.bits.broadcast || destShortAddress.Val > 0xFFFC)
                        {
                            // if this broadcast is from myself
                            if( sourceShortAddress.Val == myShortAddress.Val && 
                                sourcePANID.Val == myPANID.Val )
                            {
                                break;   
                            }
   
                            #if defined(NWK_ROLE_END_DEVICE)
                                #if defined(ENABLE_SLEEP)
                                    if( destShortAddress.Val != 0xFFFF )
                                    {
                                        break;
                                    }
                                #else
                                    if( destShortAddress.Val < 0xFFFE )
                                    {
                                        break;
                                    }
                                #endif
                            #endif    
   
                            //check if this broadcast has been received before
                            for(i = 0; i < PACKET_RECORD_SIZE; i++)
                            {
                                if( PacketRecords[i].RxCounter && 
                                    PacketRecords[i].AltSourceAddr.Val == sourceShortAddress.Val &&
                                    PacketRecords[i].MiWiPROSeq == MACRxPacket.Payload[10] )
                                {
                                    break;
                                }
                            }
                            
                            // if the broadcast is already in the broadcast record
                            if( i < PACKET_RECORD_SIZE )
                            {
                                #if defined(ENABLE_SLEEP)
                                    MiWiPROStateMachine.bits.DataRequesting = 0;
                                #endif
                                break;
                            }
                            
                            
                            // save the broadcast information into the broadcast record.
                            for(i = 0; i < PACKET_RECORD_SIZE; i++)
                            {
                                if( PacketRecords[i].RxCounter == 0 )
                                {
                                    break;
                                }
                            }
                            
                            if( i < PACKET_RECORD_SIZE )
                            {
                                PacketRecords[i].AltSourceAddr.Val = sourceShortAddress.Val;
                                PacketRecords[i].MiWiPROSeq = MACRxPacket.Payload[10];
                                PacketRecords[i].RxCounter = INDIRECT_MESSAGE_TIMEOUT_CYCLE + 1;
                                #if !defined(ENABLE_SLEEP)
                                    PacketRecords[i].StartTick = MiWi_TickGet();
                                #endif
                            }
                            
                            if( destShortAddress.Val == 0xFFFF )
                            {
                                rxMessage.flags.bits.broadcast = BROADCAST_TO_ALL;
                            }
                            else if( destShortAddress.Val == 0xFFFE )
                            {
                                rxMessage.flags.bits.broadcast = MULTICAST_TO_FFDS;
                            }    
                            else if( destShortAddress.Val == 0xFFFD )    
                            {
                                rxMessage.flags.bits.broadcast = MULTICAST_TO_COORDINATORS;
                            } 

                            #ifdef NWK_ROLE_COORDINATOR
                                // Consider to rebroadcast the message
                                if(MACRxPacket.Payload[0]>1)
                                {
                                    MACRxPacket.Payload[0]--;
                                    
                                    if( MiWiPROStatus.bits.COMM_RANDOM_SEND )
                                    {
                                        MiMAC_SendPacket(RandomSendInfo.MTP, RandomSendInfo.RandomSendTxBuffer, RandomSendInfo.RandomSendTxData);
                                    }    
                                    
                                    RandomSendInfo.RandomSendTxData = 0;
                                    for(i = 0; i < MACRxPacket.PayloadLen; i++)
                                    {
                                        RandomSendInfo.RandomSendTxBuffer[RandomSendInfo.RandomSendTxData++] = MACRxPacket.Payload[i];
                                    }    
                                    RandomSendInfo.MTP.flags.Val = MACRxPacket.flags.Val;
                                    #if defined(IEEE_802_15_4)
                                        RandomSendInfo.MTP.altSrcAddr = MACRxPacket.altSourceAddress;
                                        RandomSendInfo.MTP.DestPANID.Val = MACRxPacket.SourcePANID.Val;
                                    #endif
                                    RandomSendInfo.StartTick = MiWi_TickGet();
                                    RandomSendInfo.RandomDelayInMilliSecond = (TMRL % RANDOM_DELAY_RANGE);
                                    MiWiPROStatus.bits.COMM_RANDOM_SEND = 1;
                                    
                                    #if defined(ENABLE_INDIRECT_MESSAGE)
                                        if( (destShortAddress.Val == 0xFFFF) && (MACRxPacket.Payload[0]>1))
                                        {
                                            for(i = 0; i < CONNECTION_SIZE; i++)
                                            {
                                                if( ConnectionTable[i].status.bits.isValid && (ConnectionTable[i].status.bits.RXOnWhenIdle == 0) &&
                                                    ConnectionTable[i].status.bits.isFamily )    
                                                {
                                                    break;
                                                }
                                            }
                                            if( i < CONNECTION_SIZE )
                                            {
                                                uint8_t j;
                                                
                                                MACRxPacket.Payload[0]--;
                                                
                                                TxData = MACRxPacket.PayloadLen;
                                                for(j = 0; j < TxData; j++)
                                                {
                                                    TxBuffer[j] = MACRxPacket.Payload[j];
                                                }
                                                #if defined(IEEE_802_15_4)
                                                    SaveIndirectMessage(true, destPANID, NULL, false, MACRxPacket.flags.bits.secEn);
                                                #else
                                                    SaveIndirectMessage(true, NULL, false, MACRxPacket.flags.bits.secEn);
                                                #endif
                                            }
                                        }    
                                    #endif
                                }

                            #endif
                            
                            
                            goto ThisPacketIsForMe;
                        }
    
                        // this is unicast
                        if((destPANID.Val == myPANID.Val) && (destShortAddress.Val == myShortAddress.Val))
                        {                        
                            //need to check to see if the MiWi PRO Ack is set
                            if(MACRxPacket.Payload[1] & MIWI_PRO_ACK_REQ)
                            { 
                                TxBuffer[0] = defaultHops;		    //number of hops
                                TxBuffer[1] = 0x02;		            //Frame Control
                                TxBuffer[2] = sourcePANID.v[0];		
                                TxBuffer[3] = sourcePANID.v[1];
                                TxBuffer[4] = sourceShortAddress.v[0]; 
                                TxBuffer[5] = sourceShortAddress.v[1];
                                TxBuffer[6] = myPANID.v[0];			//source PANID LSB
                                TxBuffer[7] = myPANID.v[1];			//source PANID MSB
                                TxBuffer[8] = myShortAddress.v[0];	//source address LSB
                                TxBuffer[9] = myShortAddress.v[1];	//source address MSB
                                TxBuffer[10] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN-1];   //sequence number
                                TxBuffer[11] = MIWI_PRO_STACK_REPORT_TYPE;
                                TxBuffer[12] = ACK_REPORT_TYPE;
                                TxData = 13;
                                
                                #if defined(NWK_ROLE_COORDINATOR)
                                    if( sourceShortAddress.v[1] != myShortAddress.v[1] )
                                    {
                                        RouteMessage(sourcePANID, sourceShortAddress, false);
                                    }
                                    else 
                                    {
                                        MTP.flags.Val = 0;
                                        MTP.flags.bits.ackReq = 1;
                                            
                                        #if defined(IEEE_802_15_4)
                                            MTP.altDestAddr = true;
                                            MTP.altSrcAddr = true;
                                            MTP.DestAddress = sourceShortAddress.v;
                                            MTP.DestPANID.Val = sourcePANID.Val;
                                        #else
                                            tempShortAddress.Val = sourceShortAddress.Val;
                                            tempPANID.Val = sourcePANID.Val;
                                            if( (i = SearchForShortAddress()) != 0xFF )
                                            {
                                                MTP.DestAddress = ConnectionTable[i].Address;
                                            }    
                                        #endif
                                       
                                        MiMAC_SendPacket(MTP, TxBuffer, TxData);
                                    }   
                                #else
                                
                                    // for end device, always send the message to its parent
                                    MTP.flags.Val = 0;
                                    MTP.flags.bits.ackReq = 1;
                                        
                                    #if defined(IEEE_802_15_4)
                                        MTP.altDestAddr = true;
                                        MTP.altSrcAddr = true;
                                        MTP.DestAddress = ConnectionTable[myParent].AltAddress.v;
                                        MTP.DestPANID.Val = myPANID.Val;
                                    #else
                                        MTP.DestAddress = ConnectionTable[myParent].Address;
                                    #endif
                                   
                                    MiMAC_SendPacket(MTP, TxBuffer, TxData);
                                #endif

                                rxMessage.flags.bits.ackReq = 1;
                            }
                            
ThisPacketIsForMe:
    						//if the packet is a MiWi PRO stack packet 
                            if(MACRxPacket.Payload[MIWI_PRO_HEADER_LEN] == MIWI_PRO_STACK_REPORT_TYPE)
                            {
                                if( MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+1] != ACK_REPORT_TYPE)
                                {
                                    //determine which stack report it was
                                    switch(MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+1])
                                    {                                
                                        #if defined(SUPPORT_EUI_ADDRESS_SEARCH)
                                        #if defined(NWK_ROLE_COORDINATOR)       
                                        case EUI_ADDRESS_SEARCH_REQUEST:
                                            {
                                                uint8_t handle,j;
                                                BOOL ItsMe;
            
                                                if(sourceShortAddress.Val == myShortAddress.Val)
                                                {
                                                    if(sourcePANID.Val == myPANID.Val)
                                                    {
                                                        //I started this message.  I don't need to re-broadcast it.
                                                        break;
                                                    }
                                                }
                                                
                                                //copy the address into tempLongAddress
                                                ConsolePutROMString((ROM char*)"\r\nEUI address search request\r\n");
                                                for(j = 0; j < MY_ADDRESS_LENGTH; j++)
                                                {
                                                    tempLongAddress[j] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+2+j];
                                                }
            
                                                
                                                ItsMe = true;
                                                for(j=0;j<MY_ADDRESS_LENGTH;j++)
                                                {
                                                    //see if the address in question is my own
                                                    if(tempLongAddress[j] != myLongAddress[j])
                                                    {
                                                        ItsMe = false;
                                                        break;
                                                    }
                                                }
                                                
                                                //load up the TX Buffer with the results
                                                MiApp_FlushTx();
                                                MiApp_WriteData(MIWI_PRO_STACK_REPORT_TYPE);      //Report Type
                                                MiApp_WriteData(EUI_ADDRESS_SEARCH_RESPONSE);      //Report ID
                                                for(j = 0; j < MY_ADDRESS_LENGTH; j++)
                                                {
                                                    MiApp_WriteData(tempLongAddress[j]);      //Data byte 0
                                                }
                                                
                                                //if it wasn't me
                                                if(ItsMe == false)
                                                {
                                                    //search the network table for the device
                                                    handle = SearchForLongAddress();
                                                    
                                                    //if we found the device
                                                    if(handle!=0xFF)
                                                    {
                                                        //check to see if the short address is valid for that device
                                                        if(networkStatus[handle].bits.shortAddressValid)
                                                        {
                                                            //I know the device in question so I will send back a response
                                                            MiApp_WriteData(networkTable[handle].PANID.v[0]);
                                                            MiApp_WriteData(networkTable[handle].PANID.v[1]);
                                                            MiApp_WriteData(networkTable[handle].AltAddress.v[0]);
                                                            MiApp_WriteData(networkTable[handle].AltAddress.v[1]);
                                                             
                                                            //if everything is cool then send the response back
                                                            RouteMessage(sourcePANID, sourceShortAddress, false);
                                                            //SendReportByShortAddress(sourcePANID,sourceShortAddress,false);
                                                        }
                                                        else
                                                        {
                                                            //if we didn't have the short address then we can't
                                                            //respond to this packet even though we know the long address
                                                            ConsolePutROMString((ROM char*)"found long address but short address not valid\r\n");
                                                            //clear out the data I already wrote
                                                            TxData = 0;     
                                                        }
                
                                                    }
                                                    else
                                                    {
                                                        //if we couldn't find the long address of the device then forget it
                                                        ConsolePutROMString((ROM char*)"couldn't find the device");
                                                        TxData = 0;     
                                                    }
                                                }
                                                else
                                                {
                                                    //The node in question is me
                                                    TxBuffer[TxData++] = myPANID.v[0];
                                                    TxBuffer[TxData++] = myPANID.v[1];
                                                    TxBuffer[TxData++] = myShortAddress.v[0];
                                                    TxBuffer[TxData++] = myShortAddress.v[1];
                                                       
                                                    //send a response back with my own information
                                                    RouteMessage(sourcePANID, sourceShortAddress, false);
                                                }
                                            }
                                            break;
                                        #endif //COORDINATOR_CAPABLE
                                        #endif //SUPPORT_EUI_ADDRESS_SEARCH
                                        
                                        
                                        #if defined(SUPPORT_EUI_ADDRESS_SEARCH)
                                        case EUI_ADDRESS_SEARCH_RESPONSE:
                                            {
                                                //got a response back from the IEEE address search
                                                //record all of the information in the packet
                                                //long address
                                                
                                                MiWiPROStateMachine.bits.EUISearching = 0;
                                                for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                                                {
                                                    tempLongAddress[i] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+2+i];
                                                }
                                                //PANID and short address
                                                tempPANID.v[0] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+2+MY_ADDRESS_LENGTH];
                                                tempPANID.v[1] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+3+MY_ADDRESS_LENGTH];
                                                tempShortAddress.v[0] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+4+MY_ADDRESS_LENGTH];
                                                tempShortAddress.v[1] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+5+MY_ADDRESS_LENGTH];
                                                
                                                if( (SearchForLongAddress() == 0xFF) && (SearchForShortAddress() == 0xFF ) )
                                                {
                                                    //create a new Status entry for the network table
                                                    tempNodeStatus.Val = 0x8D;
                                                    if( tempShortAddress.v[0] & 0x80 )
                                                    {
                                                        tempNodeStatus.bits.RXOnWhenIdle = 0;
                                                    }
                                                    if((tempShortAddress.Val == sourceShortAddress.Val) 
                                                        #if defined(IEEE_802_15_4)
                                                            && (tempPANID.Val == sourcePANID.Val)
                                                        #endif
                                                       )
                                                    {
                                                        tempNodeStatus.bits.directConnection = 1;
                                                    }
                                                    //add the node to the network table
                                                    i = AddNodeToNetworkTable();
                                                    #if defined(ENABLE_NETWORK_FREEZER)
                                                        if( i < 0xFF )
                                                        {
                                                            nvmPutConnectionTableIndex(&(ConnectionTable[i]), i);
                                                        }    
                                                    #endif
                                                }    
                                            }    
                                            break;
                                        #endif
        
                                        #ifdef NWK_ROLE_COORDINATOR
                                            case OPEN_SOCKET_REQUEST:
                                                {
                                                    uint8_t j;
                                                    
                                                    //if we have a socket request open
                                                    if(openSocketInfo.status.bits.requestIsOpen == 0)
                                                    {
                                                        //if there isn't a request already open
                                                        //get the current time and the source address
                                                        openSocketInfo.socketStart = MiWi_TickGet();
                                                        openSocketInfo.ShortAddress1.v[0] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN-3];
                                                        openSocketInfo.ShortAddress1.v[1] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN-2];
            
                                                        //copy the long address
                                                        for(j=0;j<MY_ADDRESS_LENGTH;j++)
                                                        {
                                                            openSocketInfo.LongAddress1[j] = MACRxPacket.Payload[j+2+MIWI_PRO_HEADER_LEN];
                                                        }
                                                        //mark a request as open
                                                        openSocketInfo.status.bits.requestIsOpen = 1;
                                                        openSocketInfo.status.bits.matchFound = 0;
                                                        
                                                        #if ADDITIONAL_NODE_ID_SIZE > 0
                                                            for(j = 0; j < ADDITIONAL_NODE_ID_SIZE; j++)
                                                            {
                                                                openSocketInfo.AdditionalNodeID1[j] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+2+MY_ADDRESS_LENGTH+j];
                                                            }
                                                        #endif
                                                    }
                                                    else
                                                    {
                                                        //if there was already a request open
    
                                                        if( (MACRxPacket.Payload[MIWI_PRO_HEADER_LEN-3] == openSocketInfo.ShortAddress1.v[0]) && 
                                                            (MACRxPacket.Payload[MIWI_PRO_HEADER_LEN-2] == openSocketInfo.ShortAddress1.v[1]))
                                                        {
                                                            //if we get a request from the same guy twice,
                                                            //just throw away the second request
                                                            //ConsolePutROMString((ROM char*)"got request from same guy twice\r\n");
                                                        }
                                                        else if(openSocketInfo.ShortAddress1.Val == 0x0000)
                                                        {                                                
                                                            //I am the PAN coordinator and I initiated the request so don't send back a response to myself
                                                            //copy the long and short address from the Rx Buffer
                                                            for(j=0;j<MY_ADDRESS_LENGTH;j++)
                                                            {
                                                                openSocketInfo.LongAddress2[j] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+2+j];      //Data byte
                                                                tempLongAddress[j] = openSocketInfo.LongAddress2[j];
                                                            }
                                                            
                                                            //save the important information about the node sending 
                                                            //the request
                                                            openSocketInfo.ShortAddress2.v[0] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN-3];
                                                            openSocketInfo.ShortAddress2.v[1] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN-2];
    
                                                            tempShortAddress.v[0] = openSocketInfo.ShortAddress2.v[0];
                                                            tempShortAddress.v[1] = openSocketInfo.ShortAddress2.v[1];
                                                            tempPANID.Val = myPANID.Val;
                                                            
                                                            //look to see if that device exists in our network table
                                                            openSocketInfo.socketHandle = SearchForShortAddress();
                                                            
                                                            if(openSocketInfo.socketHandle == 0xFF)
                                                            {
                                                                //otherwise create it
                                                                //Family, RxOnWHenIdle, Neighbor/Network, P2P, ShortVal, LongVal, Direct, Valid
                                                                tempNodeStatus.Val = 0x8D;
                                                                if(tempShortAddress.v[0] & 0x80)
                                                                {
                                                                    tempNodeStatus.bits.RXOnWhenIdle = 0;
                                                                }
                                                                  
                                                                openSocketInfo.socketHandle = AddNodeToNetworkTable();
                                                                if( openSocketInfo.socketHandle < 0xFF )
                                                                {
                                                                    #if ADDITIONAL_NODE_ID_SIZE > 0 
                                                                        for(j = 0; j < ADDITIONAL_NODE_ID_SIZE; j++)
                                                                        {
                                                                            ConnectionTable[openSocketInfo.socketHandle].PeerInfo[j] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+2+MY_ADDRESS_LENGTH+j];
                                                                        }    
                                                                    #endif
                                                                    #if defined(ENABLE_NETWORK_FREEZER)
                                                                        nvmPutConnectionTableIndex(&(ConnectionTable[openSocketInfo.socketHandle]), openSocketInfo.socketHandle);
                                                                    #endif
                                                                }    
                                                            }
    
                                                                                              
                                                            openSocketInfo.status.bits.requestIsOpen = 1;
                                                            openSocketInfo.status.bits.matchFound = 1; 
                                                        }
                                                        else
                                                        {                                            
                                                            ConsolePutROMString((ROM char*)"sending out response\r\n");
                                                            
                                                            //we got a match so let's send out the response
                                                            
                                                            MiApp_FlushTx();
                                                            MiApp_WriteData(MIWI_PRO_STACK_REPORT_TYPE);          //Report Type
                                                            MiApp_WriteData(OPEN_SOCKET_RESPONSE);    //Report ID
            
                                                            //copy the long and short address from the Rx Buffer
                                                            for(j=0;j<MY_ADDRESS_LENGTH;j++)
                                                            {
                                                                openSocketInfo.LongAddress2[j] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+2+j];      //Data byte
                                                                MiApp_WriteData(openSocketInfo.LongAddress2[j]); 
                                                            }
    
                                                            openSocketInfo.ShortAddress2.v[0] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN-3];
                                                            openSocketInfo.ShortAddress2.v[1] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN-2];
                                                            /*#if ADDITIONAL_NODE_ID_SIZE > 0
                                                                for(j = 0; j < ADDITIONAL_NODE_ID_SIZE; j++)
                                                                {
                                                                    openSocketInfo.AdditionalNodeID2[j] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+10+j];
                                                                }    
                                                            #endif
                                                            */
                                                            
                                                            MiApp_WriteData(openSocketInfo.ShortAddress2.v[0]);
                                                            MiApp_WriteData(openSocketInfo.ShortAddress2.v[1]);
                                                            #if ADDITIONAL_NODE_ID_SIZE > 0
                                                                for(j = 0; j < ADDITIONAL_NODE_ID_SIZE; j++)
                                                                {
                                                                    MiApp_WriteData(MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+2+MY_ADDRESS_LENGTH+j]);
                                                                }
                                                            #endif
                                                            
                                                            
                                                            //MiApp_UnicastAddress(openSocketInfo.ShortAddress1.v, false, false);
                                                            UnicastShortAddress(openSocketInfo.ShortAddress1.v);
                                                            openSocketInfo.status.bits.requestIsOpen = 1;
                                                            openSocketInfo.status.bits.matchFound = 1;    
                                                        }
    
                                                    }
                                                }
                                                break;
                                                
                                                
                                            case COORDINATOR_REPORT:
                                                {
                                                    if( role == ROLE_PAN_COORDINATOR )
                                                    {          
                                                        uint8_t entry;
                                                        
                                                        if(MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+2] == COORDINATOR_REPORT_STATUS_JOIN )
                                                        {                                          
                                                            //load up the TX Buffer with the results
                                                            MiApp_FlushTx();
                                                            MiApp_WriteData(MIWI_PRO_STACK_REPORT_TYPE);        //Report Type
                                                            MiApp_WriteData(COORDINATOR_RESPONSE);          //Report ID
                                                            for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                                                            {
                                                                tempLongAddress[i] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+3+i];
                                                                MiApp_WriteData(MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+3+i]);
                                                            }
                                                            if( (entry = SearchForLongAddress()) == 0xFF )
                                                            {
ASSIGN_COORDINATOR_SHORT_ADDRESS:                                                            
                                                                tempShortAddress.v[0] = 0;
                                                                for(i = 1; i < NUM_COORDINATOR; i++)
                                                                {
                                                                    tempShortAddress.v[1] = i;
                                                                    if( (entry = SearchForShortAddress()) == 0xFF )
                                                                    {
                                                                        break;
                                                                    }    
                                                                }    
                                                                
                                                                if( i < NUM_COORDINATOR )
                                                                {
                                                                    // update family tree
                                                                    FamilyTree[i] = sourceShortAddress.v[1];
                                                                    MiApp_WriteData(i);
                                                                    
                                                                    tempNodeStatus.Val = 0x9D;
                                                                    tempPANID.Val = myPANID.Val;
                                                                    entry = AddNodeToNetworkTable();
                                                                    
                                                                    if( entry < CONNECTION_SIZE )
                                                                    {
                                                                        #if ADDITIONAL_NODE_ID_SIZE > 0
                                                                            for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
                                                                            {
                                                                                ConnectionTable[entry].PeerInfo[i] = MACRxPacket.Payload[14+MY_ADDRESS_LENGTH+i];
                                                                            }
                                                                        #endif
                                                                        #if defined(ENABLE_NETWORK_FREEZER)
                                                                            if( entry < CONNECTION_SIZE )
                                                                            {
                                                                                nvmPutConnectionTableIndex(&(ConnectionTable[entry]), entry);
                                                                            }    
                                                                        #endif
                                                                    }
                                                                    else
                                                                    {
                                                                        FamilyTree[i] = 0xFF;
                                                                        MiApp_WriteData(0xFF);
                                                                    }

                                                                }
                                                                else
                                                                {
                                                                    // no more Coordinator available. Use 0xFF
                                                                    // to indicate that the joining device must
                                                                    // be an end device
                                                                    MiApp_WriteData(0xFF);
                                                                }
                                                            }
                                                            else
                                                            {
                                                                if( ConnectionTable[entry].AltAddress.v[0] == 0 )
                                                                {
                                                                    // check the loop in family tree
                                                                    uint8_t loopCheckTarget = sourceShortAddress.v[1];
                                                                    
                                                                    while( loopCheckTarget )
                                                                    {
                                                                        if( FamilyTree[loopCheckTarget] == ConnectionTable[entry].AltAddress.v[1] )
                                                                        {
                                                                            // potential parent is the children of joining coordinator 
                                                                            // under current topology 
                                                                            break;
                                                                        }
                                                                        loopCheckTarget = FamilyTree[loopCheckTarget];
                                                                    }
                                                                    
                                                                    if( loopCheckTarget == 0 )
                                                                    {
                                                                        FamilyTree[ConnectionTable[entry].AltAddress.v[1]] = sourceShortAddress.v[1];
                                                                    }    
                                                                    MiApp_WriteData( ConnectionTable[entry].AltAddress.v[1] );
                                                                }
                                                                else
                                                                {
                                                                    ConnectionTable[entry].status.Val = 0;
                                                                    goto ASSIGN_COORDINATOR_SHORT_ADDRESS;
                                                                }        
                                                            }        
                                                            
                                                            //MiApp_UnicastAddress(sourceShortAddress.v, false, false);
                                                            UnicastShortAddress(sourceShortAddress.v);

                                                            MiWiPROStatus.bits.COMM_FAMILY_TREE = FAMILY_TREE_BROADCAST;
                                                            backgroundTaskTick = MiWi_TickGet();
                                                            backgroundTaskTick.Val -= (ONE_SECOND/2);
                                                            
                                                            MiWiPRONVMStatus.bits.saveFamilyTree = 1;
                                                        }
                                                        else
                                                        {
                                                            // Coordinator report status leave
                                                            for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                                                            {
                                                                tempLongAddress[i] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+3+i];
                                                            }
                                                            if( (entry = SearchForLongAddress()) != 0xFF )
                                                            {
                                                                // remove all its children coordinators
                                                                for(i = (NUM_COORDINATOR-1); i > 0; i--)
                                                                {
                                                                    uint8_t index;
                                                                    uint8_t loopCheck;
                                                                    
                                                                    index = i;
                                                                    loopCheck = 0;
                                                                    while(index > 0)
                                                                    {
                                                                        if( FamilyTree[index] == 0xFF )
                                                                        {
                                                                            break;
                                                                        }
                                                                        
                                                                        if( FamilyTree[index] == ConnectionTable[entry].AltAddress.v[1] )
                                                                        {
                                                                            uint8_t entry2;
                                                                            
                                                                            FamilyTree[i] = 0xFF;
                                                                            tempShortAddress.v[0] = 0;
                                                                            tempShortAddress.v[1] = i;
                                                                            if( (entry2 = SearchForShortAddress()) != 0xFF )
                                                                            {
                                                                                ConnectionTable[entry2].status.Val = 0;
                                                                            }
                                                                            break;
                                                                        }
                                                                        
                                                                        index = FamilyTree[index];
                                                                        if( loopCheck++ > NUM_COORDINATOR )
                                                                        {
                                                                            break;
                                                                        }
                                                                    }
                                                                }

                                                                FamilyTree[ConnectionTable[entry].AltAddress.v[1]] = 0xFF;
                                                                ConnectionTable[entry].status.Val = 0;
                                                                #if defined(ENABLE_NETWORK_FREEZER)
                                                                    nvmPutConnectionTableIndex(&(ConnectionTable[entry]), entry);  
                                                                #endif
                                                                
                                                                MiWiPROStatus.bits.COMM_FAMILY_TREE = FAMILY_TREE_BROADCAST;
                                                                backgroundTaskTick = MiWi_TickGet();
                                                                backgroundTaskTick.Val -= (ONE_SECOND/2);
                                                            }
                                                                  
                                                        }        
                                                    }
                                                }
                                                break;
                                                
                                                
                                            case COORDINATOR_RESPONSE:
                                                {
                                                    uint8_t handle;
                                                    uint8_t entry;
                                                    uint8_t associateStatus = ASSOCIATION_SUCCESSFUL;
                                                    
                                                    for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                                                    {
                                                        tempLongAddress[i] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+2+i];
                                                    }
                                                    tempNodeStatus.Val = 0xBF;
                                                    
                                                    if( MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+2+MY_ADDRESS_LENGTH] == 0xFF )
                                                    {
                                                        // PAN Coord has run out of Coordinators, assign
                                                        // this device as end device
                                                        handle = SearchForLongAddress();
                                                        if( handle < CONNECTION_SIZE )
                                                        {
                                                            tempShortAddress.v[1] = myShortAddress.v[1];
                                                            entry = 1;
                                                            for(i = 0; i < CONNECTION_SIZE; i++)
                                                            {
                                                                tempShortAddress.v[0] = entry;
                                                                if( SearchForShortAddress() == 0xFF )
                                                                {
                                                                    tempShortAddress.v[0] = entry + 0x80;
                                                                    if( SearchForShortAddress() == 0xFF )
                                                                    {
                                                                        break;
                                                                    }
                                                                } 
                                                                entry++;       
                                                            }
                                                            if( i < CONNECTION_SIZE )
                                                            {
                                                                if( entry < 128 )
                                                                {
                                                                    tempShortAddress.v[0] = entry;
                                                                }
                                                                else
                                                                {
                                                                    associateStatus = ASSOCIATION_PAN_FULL;
                                                                }        
                                                            } 
                                                        } 
                                                        else
                                                        {
                                                            break;
                                                        }       
                                                    }
                                                    else
                                                    {
                                                        tempShortAddress.v[0] = 0;
                                                        tempShortAddress.v[1] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+2+MY_ADDRESS_LENGTH];
                                                    }    
                                                    
                                                    tempPANID.Val = myPANID.Val;
                                                    handle = AddNodeToNetworkTable();
                                                    
                                                    // send back the association response
                                                    TxBuffer[0] = MAC_COMMAND_ASSOCIATION_RESPONSE;
                                                    TxBuffer[1] = tempShortAddress.v[0];
                                                    TxBuffer[2] = tempShortAddress.v[1];
                                                    TxBuffer[3] = associateStatus;
                                                    #if ADDITIONAL_NODE_ID_SIZE > 0
                                                        for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
                                                        {
                                                            TxBuffer[4+i] = AdditionalNodeID[i];
                                                        }
                                                    #endif
                                                    #if defined(IEEE_802_15_4)
                                                        TxData = 4+ADDITIONAL_NODE_ID_SIZE;
                                                    #else
                                                        TxBuffer[4+ADDITIONAL_NODE_ID_SIZE] = myShortAddress.v[1];
                                                        TxData = 5 + ADDITIONAL_NODE_ID_SIZE;
                                                    #endif
                                                    
                                                    #if defined(ENABLE_SECURITY)
                                                        IncomingFrameCounter[handle].Val = 0;
                        					        #endif        
                                                    
                                                    #if defined(IEEE_802_15_4)
                                                        SendMACPacket(myPANID.v, ConnectionTable[handle].Address, PACKET_TYPE_COMMAND, 0);
                                                    #else
                                                        SendMACPacket(ConnectionTable[handle].Address, PACKET_TYPE_COMMAND);
                                                    #endif
                                                    
                                                    #if defined(ENABLE_NETWORK_FREEZER)
                                                        if( handle < CONNECTION_SIZE )
                                                        {
                                                            nvmPutConnectionTableIndex(&(ConnectionTable[handle]), handle);
                                                        }    
                                                    #endif
                                                }
                                                break;
                                                
                                                        
                                            case FAMILY_TREE_REQUEST:
                                                {
                                                    MiApp_FlushTx();
                                                    MiApp_WriteData(MIWI_PRO_STACK_REPORT_TYPE);
                                                    MiApp_WriteData(FAMILY_TREE_REPORT);
                                                    for(i = 0; i < NUM_COORDINATOR; i++)
                                                    {
                                                        MiApp_WriteData(FamilyTree[i]);
                                                    }    
                                                    //MiApp_UnicastAddress(sourceShortAddress.v, false, false);
                                                    UnicastShortAddress(sourceShortAddress.v);
                                                }
                                                break;
                                                
                                                
                                            case FAMILY_TREE_REPORT:
                                                {
                                                    for(i = 0; i < NUM_COORDINATOR; i++)
                                                    {
                                                        FamilyTree[i] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+2+i];
                                                    } 
                                                    
                                                    backgroundTaskTick = MiWi_TickGet();
                                                    #if defined(ENABLE_NETWORK_FREEZER)
                                                        MiWiPRONVMStatus.bits.saveFamilyTree = 1;
                                                    #endif   
                                                }
                                                break;
                                                
                                                
                                            case ROUTING_TABLE_REPORT:
                                                {
                                                    if( MACRxPacket.RSSIValue > COMM_RSSI_THRESHOLD )
                                                    {
                                                        #if defined(IEEE_802_15_4)
                                                            uint8_t index = MIWI_PRO_HEADER_LEN+2;
                                                        #else
                                                            uint8_t index = MIWI_PRO_HEADER_LEN+3;
                                                        #endif
                                                        uint8_t initiator = MACRxPacket.Payload[index];
                                                        
                                                        for(i = 0; i < NUM_COORDINATOR/8; i++)
                                                        {
                                                            #if defined(ENABLE_ROUTING_UPDATE)
                                                                if( NeighborRoutingTable[sourceShortAddress.v[1]][i] != MACRxPacket.Payload[index+1+i] )
                                                                {
                                                                    NeighborRoutingTable[sourceShortAddress.v[1]][i] = MACRxPacket.Payload[index+1+i];
                                                                    MiWiPRONVMStatus.bits.saveNeighborTable = 1; 
                                                                } 
                                                            #else
                                                                NeighborRoutingTable[sourceShortAddress.v[1]][i] = MACRxPacket.Payload[index+1+i];
                                                            #endif   
                                                        }    
                                                        #if defined(IEEE_802_15_4)
                                                            #if defined(ENABLE_ROUTING_UPDATE)
                                                                if( (RoutingTable[ MACRxPacket.SourceAddress[1]/8] & (0x01 << (MACRxPacket.SourceAddress[1]%8))) == 0 )
                                                                {
                                                                    RoutingTable[ MACRxPacket.SourceAddress[1]/8] |= (0x01 << (MACRxPacket.SourceAddress[1]%8));
                                                                    MiWiPRONVMStatus.bits.saveRoutingTable = 1;
                                                                }   
                                                            #else
                                                                RoutingTable[ MACRxPacket.SourceAddress[1]/8] |= (0x01 << (MACRxPacket.SourceAddress[1]%8));
                                                            #endif 
                                                            
                                                            if( initiator == MACRxPacket.SourceAddress[1] ) 
                                                        #else
                                                            #if defined(ENABLE_ROUTING_UPDATE)
                                                                if( (RoutingTable[sourceShortAddress.v[1]/8] & (0x01 << (MACRxPacket.Payload[index-1]%8))) == 0 )
                                                                {
                                                                    RoutingTable[sourceShortAddress.v[1]/8] |= (0x01 << (MACRxPacket.Payload[index-1]%8));
                                                                    MiWiPRONVMStatus.bits.saveRoutingTable = 1;
                                                                }
                                                            #else
                                                                RoutingTable[sourceShortAddress.v[1]/8] |= (0x01 << (MACRxPacket.Payload[index-1]%8));
                                                            #endif
                                                            
                                                            for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                                                            {
                                                                tempLongAddress[i] = MACRxPacket.SourceAddress[i];
                                                            }    
                                                            tempShortAddress.v[0] = 0;
                                                            tempShortAddress.v[1] = MACRxPacket.Payload[index-1];
                                                            tempNodeStatus.Val = 0x8F;
                                                            tempPANID.Val = myPANID.Val;
                                                            
                                                            if( (i = SearchForLongAddress()) < CONNECTION_SIZE )
                                                            {
                                                                if( ConnectionTable[i].AltAddress.Val != tempShortAddress.Val )
                                                                {
                                                                    ConnectionTable[i].AltAddress.Val = tempShortAddress.Val;
                                                                    ConnectionTable[i].status.bits.shortAddressValid = 1;
                                                                    MiWiPRONVMStatus.bits.saveConnectionTable = 1;
                                                                }
                                                                ConnectionTable[i].status.bits.directConnection = 1;
                                                            }
                                                            else
                                                            {
                                                                if( AddNodeToNetworkTable() < CONNECTION_SIZE )
                                                                {
                                                                    MiWiPRONVMStatus.bits.saveConnectionTable = 1;
                                                                }    
                                                            }
  
                                                            if( initiator == MACRxPacket.Payload[index-1] )
                                                        #endif
                                                        {
                                                            // broadcast my routing table
                                                            if( MiWiPROStatus.bits.COMM_RANDOM_SEND )
                                                            {
                                                                MiMAC_SendPacket(RandomSendInfo.MTP, RandomSendInfo.RandomSendTxBuffer, RandomSendInfo.RandomSendTxData);
                                                                MiWiPROStatus.bits.COMM_RANDOM_SEND = 0;    
                                                            }   
                                                            
                                                            RandomSendInfo.RandomSendTxBuffer[0] = 1;
                                                            RandomSendInfo.RandomSendTxBuffer[1] = 0x02;
                                                            RandomSendInfo.RandomSendTxBuffer[2] = myPANID.v[0];
                                                            RandomSendInfo.RandomSendTxBuffer[3] = myPANID.v[1];
                                                            RandomSendInfo.RandomSendTxBuffer[4] = 0xFD; 
                                                            RandomSendInfo.RandomSendTxBuffer[5] = 0xFF;
                                                            RandomSendInfo.RandomSendTxBuffer[6] = myPANID.v[0];
                                                            RandomSendInfo.RandomSendTxBuffer[7] = myPANID.v[1];
                                                            RandomSendInfo.RandomSendTxBuffer[8] = myShortAddress.v[0];
                                                            RandomSendInfo.RandomSendTxBuffer[9] = myShortAddress.v[1];
                                                            RandomSendInfo.RandomSendTxBuffer[10] = MiWiPROSeqNum++;
                                                            RandomSendInfo.RandomSendTxBuffer[11] = MIWI_PRO_STACK_REPORT_TYPE;
                                                            RandomSendInfo.RandomSendTxBuffer[12] = ROUTING_TABLE_REPORT;
                                                            RandomSendInfo.RandomSendTxData = 13;
                                                            #if !defined(IEEE_802_15_4)
                                                                RandomSendInfo.RandomSendTxBuffer[RandomSendInfo.RandomSendTxData++] = myShortAddress.v[1];
                                                            #endif
                                                            RandomSendInfo.RandomSendTxBuffer[RandomSendInfo.RandomSendTxData++] = initiator;
                                                            for(i = 0; i < NUM_COORDINATOR/8; i++)
                                                            {
                                                                RandomSendInfo.RandomSendTxBuffer[RandomSendInfo.RandomSendTxData++] = RoutingTable[i];
                                                            }
                                                            RandomSendInfo.MTP.flags.Val = 0;
                                                            RandomSendInfo.MTP.flags.bits.broadcast = 1;
                                                            #if defined(IEEE_802_15_4)
                                                                RandomSendInfo.MTP.altSrcAddr = true;
                                                                RandomSendInfo.MTP.DestPANID.Val = myPANID.Val;
                                                            #else
                                                                RandomSendInfo.MTP.flags.bits.sourcePrsnt = 1;
                                                            #endif    
                                                            RandomSendInfo.RandomDelayInMilliSecond = (TMRL % RANDOM_DELAY_RANGE);
                                                            RandomSendInfo.StartTick = MiWi_TickGet();
                                                            MiWiPROStatus.bits.COMM_RANDOM_SEND = 1;
                                                        } 
                                                        
                                                        #if defined(ENABLE_ROUTING_UPDATE)
                                                            if( RoutingUpdateCounter[sourceShortAddress.v[1]] == 0 )
                                                            {
                                                        #endif
                                                                MiWiPRONVMStatus.bits.saveRoutingTable = 1;
                                                                MiWiPRONVMStatus.bits.saveNeighborTable = 1; 
                                                        
                                                        #if defined(ENABLE_ROUTING_UPDATE)
                                                            }
                                                            RoutingUpdateCounter[sourceShortAddress.v[1]] = ROUTING_UPDATE_EXPIRATION;
                                                        #endif
                                                        backgroundTaskTick = MiWi_TickGet();
                                                    }    
                                                }
                                                break;                
                                                
                                                
                                                
                                        #endif
                                        
                                        case OPEN_SOCKET_RESPONSE:
                                            {
                                                uint8_t j;
                                                
                                                //save the long address of the device
                                                for(j=0;j<MY_ADDRESS_LENGTH;j++)
                                                {
                                                    tempLongAddress[j] = MACRxPacket.Payload[j+MIWI_PRO_HEADER_LEN+2];
                                                }
                                                
                                                //create a new status for the device
                                                tempNodeStatus.Val = 0x8D;
                                                //check the RxOnWhenIdle bit
                                                if(MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+2+MY_ADDRESS_LENGTH] & 0x80)
                                                {
                                                    tempNodeStatus.bits.RXOnWhenIdle = 0;
                                                }
                                                
                                                //copy the short address
                                                tempShortAddress.v[0] = MACRxPacket.Payload[MY_ADDRESS_LENGTH+MIWI_PRO_HEADER_LEN+2];
                                                tempShortAddress.v[1] = MACRxPacket.Payload[MY_ADDRESS_LENGTH+MIWI_PRO_HEADER_LEN+3];
                                                
                                                //use my PANID since this version of MiWi PRO only supports single PANIDs
                                                tempPANID.Val = myPANID.Val;
                                                
                                                //see if this node already exists in my table
                                                openSocketInfo.socketHandle = SearchForShortAddress();
                                               
                                                if(openSocketInfo.socketHandle != 0xFF)
                                                {
                                                    //if it does then get the status of the node already in the table
                                                    tempNodeStatus.Val = ConnectionTable[openSocketInfo.socketHandle].status.Val;
                                                    tempNodeStatus.bits.longAddressValid = 1;
                                                    tempNodeStatus.bits.shortAddressValid = 1;
                                                }
                                                else
                                                {
                                                    //Family, RxOnWHenIdle, Neighbor/Network, P2P, ShortVal, LongVal, Direct, Valid
                                                    tempNodeStatus.Val = 0x8D;
                                                    if(tempShortAddress.v[0] & 0x80)
                                                    {
                                                        tempNodeStatus.bits.RXOnWhenIdle = 0;
                                                    }
                                                }
                                                //update the network information
                                                openSocketInfo.socketHandle = AddNodeToNetworkTable();
                                                if( openSocketInfo.socketHandle < 0xFF )
                                                {
                                                    #if ADDITIONAL_NODE_ID_SIZE > 0
                                                        for(j = 0; j < ADDITIONAL_NODE_ID_SIZE; j++)
                                                        {
                                                            ConnectionTable[openSocketInfo.socketHandle].PeerInfo[j] = MACRxPacket.Payload[MY_ADDRESS_LENGTH+4+MIWI_PRO_HEADER_LEN+j];
                                                        }
                                                    #endif
                                                    #if defined(ENABLE_NETWORK_FREEZER)
                                                        nvmPutConnectionTableIndex(&(ConnectionTable[openSocketInfo.socketHandle]), openSocketInfo.socketHandle);
                                                    #endif
                                                    openSocketInfo.status.bits.matchFound = 1;
                                                }  
                                                else
                                                {
                                                    openSocketInfo.status.bits.matchFound = 0;
                                                }      
                                                
                                                //clear out out request
                                                openSocketInfo.status.bits.requestIsOpen = 0;
                                                
                                            }
                                            break;


                                        #if defined(ENABLE_FREQUENCY_AGILITY)
                                        
                                            #if defined(NWK_ROLE_COORDINATOR)
                                            
                                                case FA_COORDINATOR_REQUEST:
                                                    {
                                                        API_UINT32_UNION ChannelMap;
                                                        
                                                        ChannelMap.v[0] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+2];
                                                        ChannelMap.v[1] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+3];
                                                        ChannelMap.v[2] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+4];
                                                        ChannelMap.v[3] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+5];
                                                        MiApp_DiscardMessage();
                                                        // Due to XC8 Compiler limitation, channel hopping initialization
                                                        // could not start from here. We could only set a flag and depends
                                                        // on application layer of a PAN Coordinator to check such flag
                                                        // and invoke channel hopping
                                                        #if defined(__XC8)

                                                            if(MiWiPROStatus.bits.COMM_FREQ_HOPPING == 0)
                                                            {
                                                                MiWiPROStatus.bits.COMM_FREQ_HOPPING = 1;
                                                                //MiApp_InitChannelHopping(ChannelMap.Val);
                                                                //MiWiPROStatus.bits.COMM_FREQ_HOPPING = 0;
                                                            }



                                                        #else

                                                            if(MiWiPROStatus.bits.COMM_FREQ_HOPPING == 0)
                                                            {
                                                                MiWiPROStatus.bits.COMM_FREQ_HOPPING = 1;
                                                                MiApp_InitChannelHopping(ChannelMap.Val);
                                                                MiWiPROStatus.bits.COMM_FREQ_HOPPING = 0;
                                                            }

                                                        #endif
                                                    }
                                                    break;    
                                            
                                            
                                                case FA_ENERGY_SCAN_REQUEST:
                                                    {  
                                                        FrequencyAgilityInfo.ChannelMap.v[0] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+2];
                                                        FrequencyAgilityInfo.ChannelMap.v[1] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+3];
                                                        FrequencyAgilityInfo.ChannelMap.v[2] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+4];
                                                        FrequencyAgilityInfo.ChannelMap.v[3] = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+5];
                                                        FrequencyAgilityInfo.status.Val = 0;
                                                        FrequencyAgilityInfo.status.bits.EnergyScan = 1;
                                                        FrequencyAgilityInfo.FA_StatTick = MiWi_TickGet();
                                                    }
                                                    break;
                                                    
                                                    
                                                case FA_SUGGEST_CHANNEL:
                                                    {
                                                        if( FA_MAX_NOISE_THRESHOLD < EnergyScanResults[MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+2]] )
                                                        {
                                                            MiApp_FlushTx();
                                                            MiApp_WriteData(MIWI_PRO_STACK_REPORT_TYPE);
                                                            MiApp_WriteData(FA_DISPUTE_CHANNEL);
                                                            MiApp_WriteData(MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+2]);
                                                            tempShortAddress.Val = 0;
                                                            //MiApp_UnicastAddress(tempShortAddress.v, false, false);
                                                            UnicastShortAddress(tempShortAddress.v);
                                                        } 
                                                    }
                                                    break;
                                                    
                                                    
                                                case FA_DISPUTE_CHANNEL:
                                                    {
                                                        if( MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+2] == FrequencyAgilityInfo.status.bits.SuggestChannel )
                                                        {
                                                            FrequencyAgilityInfo.status.bits.DisputeChannel = 1;
                                                        }    
                                                    }
                                                    break;            
                                                
                                                case RESYNCHRONIZATION_REQUEST:
                                                    {
                                                        if( MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+2] != currentChannel )
                                                        {
                                                            break;
                                                        }
                                                        MAC_FlushTx();
                                                        TxBuffer[0] = defaultHops;
                                                        TxBuffer[1] = 0x02;
                                                        TxBuffer[2] = myPANID.v[0];
                                                        TxBuffer[3] = myPANID.v[1];
                                                        TxBuffer[4] = MACRxPacket.Payload[8];
                                                        TxBuffer[5] = MACRxPacket.Payload[9];
                                                        TxBuffer[6] = myPANID.v[0];
                                                        TxBuffer[7] = myPANID.v[1];
                                                        TxBuffer[8] = myShortAddress.v[0];
                                                        TxBuffer[9] = myShortAddress.v[1];
                                                        TxBuffer[10] = MiWiPROSeqNum++;
                                                        TxBuffer[11] = MIWI_PRO_STACK_REPORT_TYPE;
                                                        TxBuffer[12] = RESYNCHRONIZATION_RESPONSE;
                                                        
                                                        MTP.flags.Val = 0;
                                                        MTP.flags.bits.ackReq = 1;
                                                        #if defined(IEEE_802_15_4)
                                                            MTP.altDestAddr = true;
                                                            MTP.altSrcAddr = true;
                                                            MTP.DestPANID.Val = myPANID.Val;
                                                            MTP.DestAddress = &(MACRxPacket.Payload[8]);
                                                        #else
                                                            tempShortAddress.v[0] = MACRxPacket.Payload[8];
                                                            tempShortAddress.v[1] = MACRxPacket.Payload[9];
                                                            i = SearchForShortAddress();
                                                            if( i == 0xFF )
                                                            {
                                                                break;
                                                            }
                                                            MTP.DestAddress = ConnectionTable[i].Address;
                                                        #endif
                                                        
                                                        MiMAC_SendPacket(MTP, TxBuffer, 13);
                                                    }
                                                    break;
                                            #endif
                                        
                                            case CHANNEL_HOPPING_REQUEST:
                                                {
                                                    if( MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+2] == currentChannel )
                                                    {
                                                        FrequencyAgilityInfo.FA_StatTick = MiWi_TickGet();
                                                        FrequencyAgilityInfo.status.bits.ChangeChannel = 1;
                                                        FrequencyAgilityInfo.status.bits.SuggestChannel = MACRxPacket.Payload[MIWI_PRO_HEADER_LEN+3];
                                                    }    
                                                }
                                                break;
                                                    
                                            case RESYNCHRONIZATION_RESPONSE:
                                                {
                                                    MiWiPROStateMachine.bits.Resynning = 0;
                                                }
                                                break;
                                        
                                        
                                        #endif

                                        default:
                                            //unknown stack request.  don't do anything
                                            //just let this packet die
                                            break;
                                    }
                                }
                                else
                                {
                                    // in case receive acknowledgement 
                                    if( (AcknowledgementSeq == MACRxPacket.Payload[MIWI_PRO_HEADER_LEN-1]) &&
                                        (sourceShortAddress.Val == AcknowledgementAddr.Val) )
                                    {
                                        MiWiPROStateMachine.bits.MiWiPROAckInProgress = 0;
                                    }    
                                    if( sourceShortAddress.v[0] > 0x80 )
                                    {
                                        MiApp_CB_RFDAcknowledgement(sourceShortAddress.Val, MACRxPacket.Payload[MIWI_PRO_HEADER_LEN-1]);
                                    }
                                    break;
                                }
                            }
                            else
                            {
                                //This data is for the user, pass it up to them
                                #if defined(ENABLE_SLEEP)
                                    MiWiPROStateMachine.bits.DataRequesting = 0;
                                #endif
                                rxMessage.PayloadSize = MACRxPacket.PayloadLen - 11;
                                rxMessage.Payload = &MACRxPacket.Payload[11];
                                rxMessage.SourcePANID.Val = sourcePANID.Val;
                                if( MACRxPacket.Payload[8] == 0xFF && MACRxPacket.Payload[9] == 0xFF )
                                {
                                    #if defined(IEEE_802_15_4)
                                        rxMessage.flags.bits.altSrcAddr = MACRxPacket.altSourceAddress;
                                        rxMessage.SourceAddress = MACRxPacket.SourceAddress; 
                                    #else
                                        if( MACRxPacket.flags.bits.sourcePrsnt )
                                        {
                                            rxMessage.SourceAddress = MACRxPacket.SourceAddress; 
                                        }
                                        else
                                        {
                                            rxMessage.flags.bits.altSrcAddr = 1;
                                            rxMessage.SourceAddress = &(MACRxPacket.Payload[8]);
                                        }
                                    #endif  
                                }
                                else
                                {
                                    rxMessage.flags.bits.altSrcAddr = 1;
                                    rxMessage.SourceAddress = &(MACRxPacket.Payload[8]);
                                }
                                rxMessage.flags.bits.srcPrsnt = 1;

                                if( rxMessage.PayloadSize > 0 )
                                {
                                    MiWiPROStateMachine.bits.RxHasUserData = 1;
                                }
                                
                            }
                        }
                        else
                        {
                            //if this packet wasn't for me
                            #ifdef NWK_ROLE_COORDINATOR
                                //then if I am a coordinator pass it along to the
                                //next hop, decrementing the number of hops available
                                if( MACRxPacket.Payload[0] > 0 )
                                {
                                    MACRxPacket.Payload[0]--;      //decrement the hops counter
                                    MAC_FlushTx();
                                    for(i = 0; i < MACRxPacket.PayloadLen; i++)
                                    {
                                        MiApp_WriteData(MACRxPacket.Payload[i]);
                                    }
                                    
                                    if( (destShortAddress.v[1] == myShortAddress.v[1]) &&
                                        (TxBuffer[11] == MIWI_PRO_STACK_REPORT_TYPE) &&
                                        (TxBuffer[12] == ACK_REPORT_TYPE) )
                                    {
                                        MTP.flags.Val = 0;
                                        MTP.flags.bits.ackReq = 1;
                                            
                                        #if defined(IEEE_802_15_4)
                                            MTP.altDestAddr = true;
                                            MTP.altSrcAddr = true;
                                            MTP.DestAddress = destShortAddress.v;
                                            MTP.DestPANID.Val = destPANID.Val;
                                            MiMAC_SendPacket(MTP, TxBuffer, TxData);
                                        #else
                                            tempShortAddress.Val = destShortAddress.Val;
                                            if( (i = SearchForShortAddress()) != 0xFF )
                                            {
                                                MTP.DestAddress = ConnectionTable[i].Address;
                                                MiMAC_SendPacket(MTP, TxBuffer, TxData);
                                            }    
                                        #endif
                                    }
                                    else
                                    {
                                        RouteMessage(destPANID, destShortAddress, MACRxPacket.flags.bits.secEn);
                                    }   
                                }
                            #endif    
                        }
    
                    }
                    break;
    
                //if it is a beacon packet
                case PACKET_TYPE_BEACON:
                    {
                        uint8_t rxIndex;
                        
                        #if defined(IEEE_802_15_4)
                            rxIndex = 0;
                        #else
                            rxIndex = 4;
                        #endif


                        if(MACRxPacket.Payload[rxIndex+4] != MIWI_PRO_PROTOCOL_ID)
                        {
                            #if defined(NWK_ROLE_END_DEVICE)
                                if(MACRxPacket.Payload[rxIndex+4] != MIWI_PROTOCOL_ID)
                            #endif
                            {
                                break;
                            }    
                        } 
                     
                        //if we are looking for a network
                        if(MiWiPROStateMachine.bits.searchingForNetwork)
                        {
                            #if defined(IEEE_802_15_4)
                                tempPANID.Val = MACRxPacket.SourcePANID.Val;
                                tempShortAddress.v[0] = MACRxPacket.SourceAddress[0];
                                tempShortAddress.v[1] = MACRxPacket.SourceAddress[1];
                            #else
                                tempPANID.v[0] = MACRxPacket.Payload[0];
                                tempPANID.v[1] = MACRxPacket.Payload[1];
                                tempShortAddress.v[0] = MACRxPacket.Payload[2];
                                tempShortAddress.v[1] = MACRxPacket.Payload[3]; 
                            #endif                   
                   
                            //ignore all beacon networks
                            if(MACRxPacket.Payload[rxIndex] == 0xFF)
                            {
                                MIWI_PRO_CAPACITY_INFO CapacityByte;
                                CapacityByte.Val = MACRxPacket.Payload[rxIndex+1];
                                if( (ActiveScanResultIndex < ACTIVE_SCAN_RESULT_SIZE) && (CapacityByte.bits.ConnMode <= ENABLE_PREV_CONN) )
                                {
                                    
                                    ActiveScanResults[ActiveScanResultIndex].Channel = currentChannel;
                                    ActiveScanResults[ActiveScanResultIndex].RSSIValue = rxMessage.PacketRSSI;
                                    ActiveScanResults[ActiveScanResultIndex].LQIValue = rxMessage.PacketLQI;
                                    ActiveScanResults[ActiveScanResultIndex].PANID.Val = tempPANID.Val;
                                    
                                    ActiveScanResults[ActiveScanResultIndex].Capability.Val = 0;
                                    ActiveScanResults[ActiveScanResultIndex].Capability.bits.Direct = 1;
                                    ActiveScanResults[ActiveScanResultIndex].Capability.bits.Role = CapacityByte.bits.Role;
                                    if( CapacityByte.bits.Security )
                                    {
                                        ActiveScanResults[ActiveScanResultIndex].Capability.bits.SecurityEn = 1;
                                    }
                                    if( CapacityByte.bits.ConnMode <= ENABLE_PREV_CONN )
                                    {
                                        ActiveScanResults[ActiveScanResultIndex].Capability.bits.AllowJoin = 1;
                                    }    

                                    #if defined(IEEE_802_15_4)
                                        ActiveScanResults[ActiveScanResultIndex].Capability.bits.altSrcAddr = 1;
                                        ActiveScanResults[ActiveScanResultIndex].Address[0] = rxMessage.SourceAddress[0];
                                        ActiveScanResults[ActiveScanResultIndex].Address[1] = rxMessage.SourceAddress[1];
                                    #else
                                        for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                                        {
                                            ActiveScanResults[ActiveScanResultIndex].Address[i] = rxMessage.SourceAddress[i];
                                        }
                                    #endif
                                    #if ADDITIONAL_NODE_ID_SIZE > 0
                                        for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
                                        {
                                            ActiveScanResults[ActiveScanResultIndex].PeerInfo[i] = MACRxPacket.Payload[rxIndex+6+i];
                                        }
                                    #endif
                                    ActiveScanResultIndex++;
                                }
                            }
                        }
                    }
                    break;
  
                
                //if it is a command packet
                case PACKET_TYPE_COMMAND:
HANDLE_COMMAND_PACKET:
                    //figure out which command packet it is
                    switch(MACRxPacket.Payload[0])
                    {
                        #ifdef NWK_ROLE_COORDINATOR
                        case MAC_COMMAND_ASSOCIATION_REQUEST:
                            {
                                uint8_t handle;
                                uint8_t associateStatus = ASSOCIATION_SUCCESSFUL;
                                uint8_t entry;
                                
                                if( ConnMode > ENABLE_PREV_CONN )
                                {
                                    break;
                                }
                                
                                if( MiWiPROStateMachine.bits.memberOfNetwork == 0 )
                                {
                                    break;
                                }
                    
                                //get the long address of the device
                                for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                                {
                                    tempLongAddress[i] = MACRxPacket.SourceAddress[i];
                                }
    
                                handle = SearchForLongAddress();
                                if( handle != 0xFF && ConnectionTable[handle].status.bits.isFamily && handle != myParent && 
                                    ConnectionTable[handle].status.bits.shortAddressValid )
                                {
                                    tempShortAddress.Val = ConnectionTable[handle].AltAddress.Val;
                                    if( ConnMode > ENABLE_PREV_CONN )
                                    {
                                        break;
                                    } 
                                    #if ADDITIONAL_NODE_ID_SIZE > 0
                                        for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
                                        {
                                            ConnectionTable[handle].PeerInfo[i] = MACRxPacket.Payload[2+i];
                                        }    
                                    #endif
                                    goto START_ASSOCIATION_RESPONSE;
                                }  
                                

                                if( ConnMode > ENABLE_ALL_CONN )
                                {
                                    break;
                                }
                                tempNodeStatus.Val = 0xA7;
                                tempPANID.Val = myPANID.Val;
                                
                                //add this node to the network table
                                handle = AddNodeToNetworkTable();
                                if( handle == 0xFF )
                                {
                                    associateStatus = ASSOCIATION_PAN_FULL;
                                    tempShortAddress.Val = 0xFFFE;
                                    goto START_ASSOCIATION_RESPONSE;
                                }    
                                
                                #if ADDITIONAL_NODE_ID_SIZE > 0
                                    for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
                                    {
                                        ConnectionTable[handle].PeerInfo[i] = MACRxPacket.Payload[2+i];
                                    }    
                                #endif
                                
                                                  
                                //if entry was successful then assign the device a short address
                                if( MACRxPacket.Payload[1] & 0x40 )
                                {
                                    // if this device is a potential coordinator                                        
                                    if( role != ROLE_PAN_COORDINATOR )
                                    {
                                        MiApp_FlushTx();
                                        MiApp_WriteData(MIWI_PRO_STACK_REPORT_TYPE);    //Report Type
                                        MiApp_WriteData(COORDINATOR_REPORT);        //Report ID
                                        MiApp_WriteData(COORDINATOR_REPORT_STATUS_JOIN);
                                        for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                                        {
                                            MiApp_WriteData(tempLongAddress[i]);        
                                        }    
                                        #if ADDITIONAL_NODE_ID_SIZE > 0
                                            for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
                                            {
                                                MiApp_WriteData(AdditionalNodeID[i]);
                                            }
                                        #endif
                                        
                                        tempShortAddress.Val = 0;
                                        //MiApp_UnicastAddress(tempShortAddress.v, false, false);
                                        UnicastShortAddress(tempShortAddress.v);
                                        break;
                                    } 
                                    else
                                    {
                                        uint8_t j;
                                        tempShortAddress.v[0] = 0;
                                        
                                        //search to see if there is a coordinator address available
                                        for(j=1;j<NUM_COORDINATOR;j++)
                                        {
                                            tempShortAddress.v[1] = j;                                        
                                            entry = SearchForShortAddress();
                                            
                                            if(entry == 0xFF)
                                            {
                                                tempShortAddress.v[0] = 0x00;
                                                tempShortAddress.v[1] = j;
                                                FamilyTree[j] = 0;
                                                MiWiPROStatus.bits.COMM_FAMILY_TREE = FAMILY_TREE_BROADCAST;
                                                backgroundTaskTick = MiWi_TickGet();
                                                backgroundTaskTick.Val -= (ONE_SECOND/2);
                                                
                                                MiWiPRONVMStatus.bits.saveFamilyTree = 1;
                                                break;
                                            }
                                        }

                                        if(j==NUM_COORDINATOR)
                                        {
                                            // no more coordinator available
                                            tempShortAddress.v[1] = 0;
                                            for(i = 1; i < 128; i++)
                                            {
                                                tempShortAddress.v[0] = i;
                                                if( SearchForShortAddress() == 0xFF )
                                                {
                                                    tempShortAddress.v[0] = i + 0x80;
                                                    if( SearchForShortAddress() == 0xFF )
                                                    {
                                                        tempShortAddress.v[0] = i;
                                                        break;
                                                    }    
                                                }    
                                            }
                                            
                                            if( i > 127 )
                                            {
                                                associateStatus = ASSOCIATION_PAN_FULL;
                                                tempShortAddress.Val = 0xFFFE;
                                                goto START_ASSOCIATION_RESPONSE;
                                            }    
                                        }
                                    }       
                                    
                                } 
                                else
                                {
                                    tempShortAddress.v[1] = myShortAddress.v[1];
                                    
                                    for(i = 1; i < 128; i++)
                                    {
                                        tempShortAddress.v[0] = i;
                                        if( SearchForShortAddress() == 0xFF )
                                        {
                                            tempShortAddress.v[0] = i + 0x80;
                                            if( SearchForShortAddress() == 0xFF )
                                            {
                                                tempShortAddress.v[0] = i;  
                                                break;
                                            }    
                                        }       
                                    }
                                    
                                    if( i > 127 )
                                    {
                                        associateStatus = ASSOCIATION_PAN_FULL;
                                        tempShortAddress.Val = 0xFFFE;
                                        goto START_ASSOCIATION_RESPONSE;
                                    }    
                                }       

                                //create a new status entry for the device
                                tempNodeStatus.Val = 0xBF;
                                
                                //mark if the Rx is on when idle
                                if((MACRxPacket.Payload[1] & 0x01) == 0)
                                {
                                    tempNodeStatus.bits.RXOnWhenIdle = 0;
                                    tempShortAddress.v[0] |= 0x80;                                   
                                }

                                if( MiApp_CB_AllowConnection(handle) == false )
                                {
                                    ConnectionTable[handle].status.Val = 0;
                                    associateStatus = ASSOCIATION_ACCESS_DENIED;
                                }     
                                
                                //update the information in the network table
                                if( associateStatus == ASSOCIATION_SUCCESSFUL )
                                {
                                    tempPANID.Val = myPANID.Val;
                                    handle = AddNodeToNetworkTable();
                                    
                                    if( handle < CONNECTION_SIZE )
                                    {
                                        ConnectionTable[handle].status.bits.FinishJoin = 1;    
        
                                        #if ADDITIONAL_NODE_ID_SIZE > 0
                                            for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
                                            {
                                                ConnectionTable[handle].PeerInfo[i] = MACRxPacket.Payload[2+i];
                                            }
                                        #endif
                                        
                                        #if defined(ENABLE_NETWORK_FREEZER)
                                            nvmPutConnectionTableIndex(&(ConnectionTable[handle]), handle);  
                                        #endif
                                    }     
                                }    
 
    
START_ASSOCIATION_RESPONSE:                       
                                //send back the asociation response
                                TxBuffer[0] = MAC_COMMAND_ASSOCIATION_RESPONSE;
                                TxBuffer[1] = tempShortAddress.v[0];
                                TxBuffer[2] = tempShortAddress.v[1];
                                TxBuffer[3] = associateStatus;
                                #if ADDITIONAL_NODE_ID_SIZE > 0
                                    for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
                                    {
                                        TxBuffer[4+i] = AdditionalNodeID[i];
                                    }
                                #endif
                                #if defined(IEEE_802_15_4)
                                    TxData = 4+ADDITIONAL_NODE_ID_SIZE;
                                #else
                                    TxBuffer[4+ADDITIONAL_NODE_ID_SIZE] = myShortAddress.v[1];
                                    TxData = 5 + ADDITIONAL_NODE_ID_SIZE;
                                #endif
                                
                                #if defined(ENABLE_SECURITY)
                                    IncomingFrameCounter[handle].Val = 0;
    					        #endif
                                
                                #if defined(IEEE_802_15_4)
                                    SendMACPacket(myPANID.v, ConnectionTable[handle].Address, PACKET_TYPE_COMMAND, 0);
                                #else
                                    SendMACPacket(ConnectionTable[handle].Address, PACKET_TYPE_COMMAND);
                                #endif
                            }
                            break;
                        #endif
                        
                        case MAC_COMMAND_ASSOCIATION_RESPONSE:
                            {
                                if(MACRxPacket.Payload[3] == ASSOCIATION_SUCCESSFUL)
                                {
                                    //The join was successful
                                    //assign myself the correct short address
                                    myShortAddress.v[0] = MACRxPacket.Payload[1];
                                    myShortAddress.v[1] = MACRxPacket.Payload[2];
                                                                
                                    #ifdef NWK_ROLE_COORDINATOR
                                        if(myShortAddress.v[0] == 0x00)
                                        {
                                            ConsolePutROMString((ROM char*)"I am a coordinator\r\n");
                                            role = ROLE_COORDINATOR;
                                            MiWiPROCapacityInfo.bits.Role = role;
                                        }
                                        else
                                        {
                                            ConsolePutROMString((ROM char*)"I am an end device\r\n");
                                            role = ROLE_FFD_END_DEVICE;
                                            MiWiPROCapacityInfo.bits.Role = role;
                                        }
                                        nvmPutRole(&role);
                                    #endif
                                    
                                    //set the short address of the device
                                    MiMAC_SetAltAddress(myShortAddress.v, myPANID.v);
        
                                    //mark as having joined a network
                                    MiWiPROStateMachine.bits.memberOfNetwork = 1;
                                    
                                    //record the parents information
                                    tempNodeStatus.Val = 0xBF;
                                    for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                                    {
                                        tempLongAddress[i] = MACRxPacket.SourceAddress[i];
                                    }
                                    
                                    
                                    #if defined(IEEE_802_15_4)
                                        tempShortAddress.Val = ConnectionTable[myParent].AltAddress.Val;
                                    #else
                                        tempShortAddress.v[0] = 0;
                                        tempShortAddress.v[1] = MACRxPacket.Payload[4+ADDITIONAL_NODE_ID_SIZE];
                                    #endif
                                    tempPANID.Val = myPANID.Val;
                                    
                                    //and add the parents information to the network table
                                    myParent = AddNodeToNetworkTable();
                                    #if defined(ENABLE_SECURITY)
        						        IncomingFrameCounter[myParent].Val = 0;
        					        #endif                            
        					        
        					        #if ADDITIONAL_NODE_ID_SIZE > 0
        					            for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
        					            {
            					            ConnectionTable[myParent].PeerInfo[i] = MACRxPacket.Payload[4+i];
            					        }
        					        #endif
        					        
        					        #if defined(NWK_ROLE_COORDINATOR)
                                        MiWiPROStatus.bits.COMM_ROUTING_TABLE = ROUTING_TABLE_BROADCAST;
                                        backgroundTaskTick = MiWi_TickGet();
                                    #endif
        					        
        					        #if defined(ENABLE_NETWORK_FREEZER)
        					            nvmPutMyPANID(myPANID.v);
        					            nvmPutMyShortAddress(myShortAddress.v);
        					            nvmPutMyParent(&myParent);
                                        nvmPutConnectionTableIndex(&(ConnectionTable[myParent]), myParent);
                                    #endif    					        
    
                                    ConsolePutROMString((ROM char *)"\r\nJoin the network successfully\r\n");                            
                                }
                                else
                                {
                                    //Access denied, remove this as my parent
                                    myParent = 0xFF;
                                    myPANID.Val = 0xFFFF;
                                    tempShortAddress.Val = 0xFFFF;
                                    MiMAC_SetAltAddress(tempShortAddress.v, myPANID.v);
                                    #if defined(ENABLE_NETWORK_FREEZER)
                                        nvmPutMyParent(&myParent);
                                        nvmPutMyPANID(myPANID.v);
                                    #endif
                                }
                            }    
                            break;
                            
                        #if !defined(TARGET_SMALL)
                            case MAC_COMMAND_DISASSOCIATION_NOTIFICATION:
                                {
                                    uint8_t cIndex;
                                    
                                    if( MiWiPROStateMachine.bits.memberOfNetwork == 0 )
                                    {
                                        break;
                                    }
                                    
                                    #if defined(IEEE_802_15_4)
                                        if( MACRxPacket.altSourceAddress )
                                        {
                                            tempShortAddress.v[0] = MACRxPacket.SourceAddress[0];
                                            tempShortAddress.v[1] = MACRxPacket.SourceAddress[1];
                                            
                                            if( (cIndex = SearchForShortAddress()) == 0xFF )
                                            {
                                                break;
                                            }
                                        }
                                        else
                                    #endif
                                    {
                                        for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                                        {
                                            tempLongAddress[i] = MACRxPacket.SourceAddress[i];
                                        }
                                        if( (cIndex = SearchForLongAddress()) == 0xFF )
                                        {
                                            break;
                                        }
                                    }
                                    
                                    #if defined(NWK_ROLE_COORDINATOR)
                                        if( ConnectionTable[cIndex].status.bits.isFamily )
                                        {
                                            if( cIndex != myParent ) // it is from my child
                                            {
                                                if( ConnectionTable[cIndex].AltAddress.v[0] == 0 )
                                                {
                                                    if( role == ROLE_PAN_COORDINATOR )
                                                    {
                                                        // remove all its children coordinators
                                                        for(i = (NUM_COORDINATOR-1); i > 0; i--)
                                                        {
                                                            uint8_t index;
                                                            uint8_t loopCheck;
                                                            
                                                            index = i;
                                                            loopCheck = 0;
                                                            while(index > 0)
                                                            {
                                                                if( FamilyTree[index] == 0xFF )
                                                                {
                                                                    break;
                                                                }
                                                                
                                                                if( FamilyTree[index] == ConnectionTable[cIndex].AltAddress.v[1] )
                                                                {
                                                                    uint8_t entry2;
                                                                    
                                                                    FamilyTree[i] = 0xFF;
                                                                    tempShortAddress.v[0] = 0;
                                                                    tempShortAddress.v[1] = i;
                                                                    if( (entry2 = SearchForShortAddress()) != 0xFF )
                                                                    {
                                                                        ConnectionTable[entry2].status.Val = 0;
                                                                    }
                                                                    break;
                                                                }
                                                                
                                                                index = FamilyTree[index];
                                                                if( loopCheck++ > NUM_COORDINATOR )
                                                                {
                                                                    break;
                                                                }
                                                            }
                                                        }
                                                        FamilyTree[ConnectionTable[cIndex].AltAddress.v[1]] = 0xFF;
                                                        ConnectionTable[cIndex].status.Val = 0;
                                                        #if defined(ENABLE_NETWORK_FREEZER)
                                                            nvmPutConnectionTableIndex(&(ConnectionTable[cIndex]), cIndex);  
                                                        #endif
                                                        
                                                        MiWiPROStatus.bits.COMM_FAMILY_TREE = FAMILY_TREE_BROADCAST;
                                                        backgroundTaskTick = MiWi_TickGet();
                                                        backgroundTaskTick.Val -= (ONE_SECOND/2);                                                        
                                                    }
                                                    else
                                                    {
                                                        MiApp_FlushTx();
                                                        MiApp_WriteData(MIWI_PRO_STACK_REPORT_TYPE);    //Report Type
                                                        MiApp_WriteData(COORDINATOR_REPORT);        //Report ID
                                                        MiApp_WriteData(COORDINATOR_REPORT_STATUS_LEAVE);
                                                        for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                                                        {
                                                            MiApp_WriteData(ConnectionTable[cIndex].Address[i]);
                                                        }
                                                        
                                                        tempShortAddress.Val = 0;
                                                        //MiApp_UnicastAddress(tempShortAddress.v, false, false);
                                                        UnicastShortAddress(tempShortAddress.v);
                                                    }
                                                }    
                                                ConnectionTable[cIndex].status.Val = 0;
                                                #if defined(ENABLE_NETWORK_FREEZER)
                                                    nvmPutConnectionTableIndex(&(ConnectionTable[cIndex]), cIndex);
                                                #endif
                                            }
                                            else // it is from my parent
                                            {
                                                // notify my children
                                                for(i = 0; i < CONNECTION_SIZE; i++)
                                                {
                                                    if( ConnectionTable[i].status.bits.isValid &&
                                                        ConnectionTable[i].status.bits.isFamily && (i != myParent ))
                                                    {
                                                        MAC_FlushTx();
                                                        MiApp_WriteData(MAC_COMMAND_DISASSOCIATION_NOTIFICATION);
                                                        MTP.flags.Val = 0;
                                                        MTP.flags.bits.ackReq = 1;
                                                        MTP.flags.bits.packetType = PACKET_TYPE_COMMAND;                                                
                                                        #if defined(IEEE_802_15_4)
                                                            MTP.altDestAddr = true;
                                                            MTP.altSrcAddr = true;
                                                            MTP.DestPANID.Val = myPANID.Val;
                                                            MTP.DestAddress = ConnectionTable[i].AltAddress.v;
                                                        #else
                                                            MTP.flags.bits.sourcePrsnt = 1;
                                                            MTP.DestAddress = ConnectionTable[i].Address;
                                                        #endif
                                                        MiMAC_SendPacket(MTP, TxBuffer, TxData);
                                                        DelayMs(100);
                                                    }
                                                    ConnectionTable[i].status.Val = 0;
                                                }
                                                MiApp_FlushTx();
                                                MiApp_WriteData(MIWI_PRO_STACK_REPORT_TYPE);    //Report Type
                                                MiApp_WriteData(COORDINATOR_REPORT);        //Report ID
                                                MiApp_WriteData(COORDINATOR_REPORT_STATUS_LEAVE);
                                                for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                                                {
                                                    MiApp_WriteData(ConnectionTable[cIndex].Address[i]);
                                                }
                                                
                                                tempShortAddress.Val = 0;
                                                //MiApp_UnicastAddress(tempShortAddress.v, false, false);
                                                UnicastShortAddress(tempShortAddress.v);
                                                
                                                myParent = 0xFF;
                                                myShortAddress.Val = 0xFFFF;
                                                myPANID.Val = 0xFFFF;
                                                MiMAC_SetAltAddress(myShortAddress.v, myPANID.v);
                                                MiWiPROStateMachine.bits.memberOfNetwork = 0;
                                                for(i = 0; i < NUM_COORDINATOR; i++)
                                                {
                                                    FamilyTree[i] = 0xFF;
                                                    for(cIndex = 0; cIndex < NUM_COORDINATOR/8; cIndex++)
                                                    {
                                                        NeighborRoutingTable[i][cIndex] = 0;
                                                    }    
                                                }
                                                for(i = 0; i < NUM_COORDINATOR/8; i++)
                                                {
                                                    RoutingTable[i] = 0;
                                                }    
                                                #if defined(ENABLE_NETWORK_FREEZER)
                                                    nvmPutMyParent(&myParent);
                                                    nvmPutMyShortAddress(myShortAddress.v);
                                                    nvmPutMyPANID(myPANID.v);
                                                    nvmPutFamilyTree(FamilyTree);
                                                    nvmPutRoutingTable(RoutingTable);
                                                    nvmPutNeighborRoutingTable(NeighborRoutingTable);
                                                    nvmPutConnectionTable(ConnectionTable);
                                                #endif
                                            }
                                        }
                                    #else
                                        if( ConnectionTable[cIndex].status.bits.isFamily ) // it is from my parent
                                        {
                                            for(i = 0; i < CONNECTION_SIZE; i++)
                                            {
                                                ConnectionTable[i].status.Val = 0;
                                            }
                                            myParent = 0xFF;
                                            myShortAddress.Val = 0xFFFF;
                                            myPANID.Val = 0xFFFF;
                                            MiMAC_SetAltAddress(myShortAddress.v, myPANID.v);
                                            MiWiPROStateMachine.bits.memberOfNetwork = 0;
                                            #if defined(ENABLE_NETWORK_FREEZER)
                                                nvmPutMyParent(&myParent);
                                                nvmPutMyShortAddress(myShortAddress.v);
                                                nvmPutMyPANID(myPANID.v);
                                                nvmPutConnectionTable(ConnectionTable);
                                            #endif
                                        }
                                    #endif
    
                                }
                                break;
                        #endif

                        
                        
                        #if defined(NWK_ROLE_COORDINATOR) && defined(ENABLE_INDIRECT_MESSAGE)
                        case MAC_COMMAND_DATA_REQUEST:
                        {
                            //uint8_t handle;
                            
                            #if defined(IEEE_802_15_4)                              
                                
                                if( MACRxPacket.altSourceAddress )
                                {
                                    SendIndirectPacket(NULL, MACRxPacket.SourceAddress, true);
                                }
                                else
                                {
                                    SendIndirectPacket(MACRxPacket.SourceAddress, NULL, false);
                                }         
                             
                            #else

                                SendIndirectPacket(MACRxPacket.SourceAddress, &(MACRxPacket.Payload[1]), true);

                            #endif

                            #if defined(ENABLE_ENHANCED_DATA_REQUEST)
                                #if defined(IEEE_802_15_4)
                                    if( MACRxPacket.PayloadLen > 1 )
                                    {
                                        goto HANDLE_DATA_PACKET;
                                    }    
                                #else

                                    if( MACRxPacket.PayloadLen > 3 )
                                    {
                                        for(i = 1; i < MACRxPacket.PayloadLen-2; i++)
                                        {
                                            MACRxPacket.Payload[i] = MACRxPacket.Payload[i+2];
                                        }   
                                        MACRxPacket.PayloadLen -= 2;
                                        goto HANDLE_DATA_PACKET; 
                                    }    
                                #endif
                            #endif

                            
                            break;
                        }
                        #endif
                        
                        #ifdef NWK_ROLE_COORDINATOR
                            case MAC_COMMAND_BEACON_REQUEST:
                                {
                                    if( ConnMode > ENABLE_ACTIVE_SCAN_RSP )
                                    {
                                        break;
                                    }
                                                    
                                    //if we are a member of a network
                                    if(MiWiPROStateMachine.bits.memberOfNetwork)
                                    {
                                        if( MACRxPacket.Payload[1] != currentChannel )
                                        {
                                            break;
                                        }
                                        
                                        //send out a beacon as long as we are not
                                        //currently acting as an FFD end device
                                        if(role != ROLE_FFD_END_DEVICE)
                                        {
                                            #if !defined(TARGET_SMALL)
                                                BroadcastJitter(RANDOM_DELAY_RANGE);
                                            #endif
                                            SendBeacon();
                                        }
                                    }
                                }    
                                break;
                        #endif
                        
                        
                        #if defined(ENABLE_TIME_SYNC) && defined(ENABLE_SLEEP)
                            case MAC_COMMAND_TIME_SYNC_DATA_PACKET:
                            case MAC_COMMAND_TIME_SYNC_COMMAND_PACKET:
                                {                    
                                    WakeupTimes.v[0] = MACRxPacket.Payload[1];
                                    WakeupTimes.v[1] = MACRxPacket.Payload[2];
                                    CounterValue.v[0] = MACRxPacket.Payload[3];
                                    CounterValue.v[1] = MACRxPacket.Payload[4];

                                    if( MACRxPacket.PayloadLen > 5 )
                                    {
                                        if( MACRxPacket.Payload[0] == MAC_COMMAND_TIME_SYNC_DATA_PACKET )
                                        {
                                            for(i = 0; i < MACRxPacket.PayloadLen-5; i++)
                                            {
                                                MACRxPacket.Payload[i] = MACRxPacket.Payload[5+i];
                                            }  
                                            MACRxPacket.PayloadLen -= 5;
                                           
                                            goto HANDLE_DATA_PACKET;
                                        }
                                        else
                                        {
                                            for(i = 0; i < MACRxPacket.PayloadLen-5; i++)
                                            {
                                                MACRxPacket.Payload[i] = MACRxPacket.Payload[5+i];
                                            }  
                                            MACRxPacket.PayloadLen -= 5;
                                           
                                            goto HANDLE_COMMAND_PACKET;
                                        }    
                                    }    
                                    else
                                    {
                                        MiWiPROStateMachine.bits.DataRequesting = 0;
                                    }    
                                }
                                break;
                        #endif    
                        
                        default:
                            break;
                      }
                    break;
                    
                default:
                    break;

            }
            
            if ( MiWiPROStateMachine.bits.RxHasUserData == 0 )
            {
                MiMAC_DiscardPacket();
            }   
        }   

        t1 = MiWi_TickGet();
        
        //if there really isn't anything going on 
        #if defined(NWK_ROLE_COORDINATOR) && defined(ENABLE_INDIRECT_MESSAGE)
            // check indirect message periodically. If an indirect message is not acquired within
            // time of INDIRECT_MESSAGE_TIMEOUT
            for(i = 0; i < INDIRECT_MESSAGE_SIZE; i++)
            {
                if( indirectMessages[i].flags.bits.isValid )
                {
                    if( MiWi_TickGetDiff(t1, indirectMessages[i].TickStart) > INDIRECT_MESSAGE_TIMEOUT )
                    {
                        indirectMessages[i].flags.Val = 0x00;   
                    }    
                }    
            }
        #endif //COORDINATOR_CAPABLE

        #if !defined(ENABLE_SLEEP)
            for(i = 0; i < PACKET_RECORD_SIZE; i++)
            {
                if( PacketRecords[i].RxCounter > 0 )
                {
                    if( MiWi_TickGetDiff(t1, PacketRecords[i].StartTick) > PACKET_RECORD_TIMEOUT )
                    {
                        PacketRecords[i].RxCounter = 0;
                    }    
                }
            }
            
        #else
        
            //if we are an RFD and a member of the network
            if(MiWiPROStateMachine.bits.memberOfNetwork)
            {
                if(MiWiPROStateMachine.bits.DataRequesting)
                {
                    t2.Val = MiWi_TickGetDiff(t1, DataRequestTimer);
                    if(t2.Val > RFD_DATA_WAIT)
                    {
                        MiWiPROStateMachine.bits.DataRequesting = 0;
                        #if defined(ENABLE_TIME_SYNC)
                            WakeupTimes.Val = RFD_WAKEUP_INTERVAL / 16;
                            CounterValue.Val = 0xFFFF - ((uint16_t)4000*(RFD_WAKEUP_INTERVAL % 16));
                        #endif
                    }
                }
            }
        #endif
        
        #if defined(ENABLE_TIME_SYNC) && !defined(ENABLE_SLEEP) && defined(ENABLE_INDIRECT_MESSAGE)
            if( MiWi_TickGetDiff(t1, TimeSyncTick) > ((ONE_SECOND) * RFD_WAKEUP_INTERVAL) )
            {
                TimeSyncTick.Val += ((uint32_t)(ONE_SECOND) * RFD_WAKEUP_INTERVAL);
                if( TimeSyncTick.Val > t1.Val )
                {
                    //Printf("   avoid waterfall");
                    TimeSyncTick.Val = t1.Val;
                }    
                TimeSyncSlot = 0;
            }    
        #endif
        
        
        #if defined(NWK_ROLE_COORDINATOR)
            t2.Val = t1.Val - backgroundTaskTick.Val;
 
            if( MiWiPROStatus.bits.COMM_FAMILY_TREE > 0 )
            {

                if( t2.Val > COMM_INTERVAL )
                {
                    MiWiPROStatus.bits.COMM_FAMILY_TREE--;
                    MiApp_FlushTx();
                    MiApp_WriteData(MIWI_PRO_STACK_REPORT_TYPE);
                    MiApp_WriteData(FAMILY_TREE_REPORT);
                    for(i = 0; i < NUM_COORDINATOR; i++)
                    {
                        MiApp_WriteData(FamilyTree[i]);
                    }
                    MultiCast(MULTICAST_TO_COORDINATORS, false);
                    backgroundTaskTick.Val = t1.Val;
                }  
            }       
            else if( MiWiPROStatus.bits.COMM_ROUTING_TABLE > 0 )
            {
                if( t2.Val > COMM_INTERVAL )
                {
                    MiWiPROStatus.bits.COMM_ROUTING_TABLE--;
                    
                    MiApp_FlushTx();
                    MiApp_WriteData(MIWI_PRO_STACK_REPORT_TYPE);
                    MiApp_WriteData(ROUTING_TABLE_REPORT);
                    #if !defined(IEEE_802_15_4)
                        MiApp_WriteData(myShortAddress.v[1]);
                    #endif
                    MiApp_WriteData(myShortAddress.v[1]);
                    for(i = 0; i < NUM_COORDINATOR/8; i++)
                    {
                        MiApp_WriteData(RoutingTable[i]);
                    }
                    defaultHops = 1;
                    MultiCast(MULTICAST_TO_COORDINATORS, false);
                    defaultHops = MAX_HOPS;
                    backgroundTaskTick.Val = t1.Val;
                }      
            }
            else if( (FamilyTree[0] == 0xFF) && (role == ROLE_COORDINATOR) && 
                    MiWiPROStateMachine.bits.memberOfNetwork )
            {
                if( t2.Val > COMM_INTERVAL )
                {
                    MiApp_FlushTx();
                    MiApp_WriteData(MIWI_PRO_STACK_REPORT_TYPE);
                    MiApp_WriteData(FAMILY_TREE_REQUEST);
                    //MiApp_UnicastConnection(myParent, false);
                    UnicastShortAddress(ConnectionTable[myParent].AltAddress.v);
                    backgroundTaskTick.Val = t1.Val;
                }    
            }      
   
             
            #if defined(ENABLE_NETWORK_FREEZER)

                else if( MiWiPRONVMStatus.bits.saveFamilyTree )
                {
                    if( t2.Val > COMM_INTERVAL * (FAMILY_TREE_BROADCAST+1))
                    {
                        MiWiPRONVMStatus.bits.saveFamilyTree = 0;
                        
                        // verify the family relationship
                        if( role != ROLE_PAN_COORDINATOR && FamilyTree[ myShortAddress.v[1] ] != ConnectionTable[myParent].AltAddress.v[1] )
                        {
                            tempShortAddress.v[0] = 0;
                            tempShortAddress.v[1] = FamilyTree[ myShortAddress.v[1] ];
                            if( (i = SearchForShortAddress()) < CONNECTION_SIZE )
                            {
                                myParent = i;
                            }
                            else
                            {
                                tempNodeStatus.Val = 0xBB;
                                tempPANID.Val = myPANID.Val;
                                myParent = AddNodeToNetworkTable();
                            }   
                        }
                        
                        nvmPutFamilyTree(FamilyTree); 
                    }    
                }
                else if( MiWiPRONVMStatus.bits.saveRoutingTable )
                {
                    if( t2.Val > COMM_INTERVAL * (FAMILY_TREE_BROADCAST+1))
                    {
                        MiWiPRONVMStatus.bits.saveRoutingTable = 0;
                        nvmPutRoutingTable(RoutingTable);
                    }    
                }
                else if( MiWiPRONVMStatus.bits.saveNeighborTable )
                {
                    if( t2.Val > COMM_INTERVAL * (FAMILY_TREE_BROADCAST+1))
                    {
                        MiWiPRONVMStatus.bits.saveNeighborTable = 0;
                        nvmPutNeighborRoutingTable(NeighborRoutingTable);
                    }    
                } 
                else if( MiWiPRONVMStatus.bits.saveConnectionTable )
                {
                    if( t2.Val > COMM_INTERVAL * (FAMILY_TREE_BROADCAST+1))
                    {
                        MiWiPRONVMStatus.bits.saveConnectionTable = 0;
                        nvmPutConnectionTable(ConnectionTable);
                    }    
                }           
            #endif
            
            
            if( MiWiPROStatus.bits.COMM_RANDOM_SEND )
            {
                if( MiWi_TickGetDiff(t1, RandomSendInfo.StartTick) > ((ONE_MILI_SECOND) * RandomSendInfo.RandomDelayInMilliSecond) )
                {
                    MiWiPROStatus.bits.COMM_RANDOM_SEND = 0;
                    MiMAC_SendPacket(RandomSendInfo.MTP, RandomSendInfo.RandomSendTxBuffer, RandomSendInfo.RandomSendTxData);
                }    
            }    
            

            #if defined(ENABLE_ROUTING_UPDATE)

                if( role != ROLE_FFD_END_DEVICE )
                {
                    if( MiWi_TickGetDiff(t1, RoutingUpdateTick) > ROUTING_UPDATE_INTERVAL )
                    {
                        MiWiPROStatus.bits.COMM_ROUTING_TABLE = 1;
                        backgroundTaskTick = MiWi_TickGet();
                        RoutingUpdateTick.Val = backgroundTaskTick.Val;
                        for(i = 0; i < NUM_COORDINATOR; i++)
                        {
                            if( RoutingUpdateCounter[i] > 0 )
                            {
                                RoutingUpdateCounter[i]--;
                                if( RoutingUpdateCounter[i] == 0 )
                                {
                                    uint8_t j;
                                    RoutingTable[i/8] = RoutingTable[i/8] & ((0x01 << (i%8))^0xFF);
                                    for(j = 0; j < NUM_COORDINATOR/8; j++)
                                    {
                                        NeighborRoutingTable[i][j] = 0;
                                    } 
                                    MiWiPRONVMStatus.bits.saveRoutingTable = 1;
                                    MiWiPRONVMStatus.bits.saveNeighborTable = 1;   
                                }
                            }
                        }
                    }
                }
                
            #endif
            
        #endif
        
 
        
        #if defined(ENABLE_FREQUENCY_AGILITY)      
            if( FrequencyAgilityInfo.status.bits.ChangeChannel )
            {
                if( MiWi_TickGetDiff(t1, FrequencyAgilityInfo.FA_StatTick) > FA_WAIT_TIMEOUT )
                {
                    MiApp_SetChannel( FrequencyAgilityInfo.status.bits.SuggestChannel );
                    FrequencyAgilityInfo.status.Val = 0;
                }    
            }
            #if defined(ENABLE_ED_SCAN) && defined(NWK_ROLE_COORDINATOR)   
                else if( FrequencyAgilityInfo.status.bits.EnergyScan )
                {
                    if( MiWi_TickGetDiff(t1, FrequencyAgilityInfo.FA_StatTick) > FA_WAIT_TIMEOUT )
                    {
                        uint8_t backupChannel = currentChannel;
                        MiApp_NoiseDetection(FrequencyAgilityInfo.ChannelMap.Val, 10, NOISE_DETECT_ENERGY, &i);
                        MiApp_SetChannel(backupChannel);
                        FrequencyAgilityInfo.status.bits.EnergyScan = 0;
                    }    
                } 
            #endif 
        #endif
        
        //clean up an old socket if one still exists
        if(openSocketInfo.status.bits.requestIsOpen)
        {
            if(openSocketInfo.status.bits.matchFound)
            {  
                #ifdef NWK_ROLE_COORDINATOR
                    uint8_t j;
        
                    ConsolePutROMString((ROM char*)"sending out second response\r\n");

                    MiApp_FlushTx();
                    TxBuffer[TxData++] = MIWI_PRO_STACK_REPORT_TYPE;      //Report Type
                    TxBuffer[TxData++] = OPEN_SOCKET_RESPONSE;      //Report ID

                
                    //copy the long and short address from the Rx Buffer
                    for(j=0;j<MY_ADDRESS_LENGTH;j++)
                    {
                        TxBuffer[TxData++] = openSocketInfo.LongAddress1[j]; 
                    }
                
                    TxBuffer[TxData++] = openSocketInfo.ShortAddress1.v[0];
                    TxBuffer[TxData++] = openSocketInfo.ShortAddress1.v[1];
                    
                    #if ADDITIONAL_NODE_ID_SIZE > 0
                        for(j = 0; j < ADDITIONAL_NODE_ID_SIZE; j++)
                        {
                            TxBuffer[TxData++] = openSocketInfo.AdditionalNodeID1[j];    
                        }
                    #endif
                    
                    //MiApp_UnicastAddress(openSocketInfo.ShortAddress2.v, false, false);
                    UnicastShortAddress(openSocketInfo.ShortAddress2.v);
                    openSocketInfo.status.bits.requestIsOpen = 0;
                #endif

                //openSocketInfo.status.bits.requestIsOpen = 0;
            }
            else
            {
                t2.Val = MiWi_TickGetDiff(t1, openSocketInfo.socketStart);
                
                if(t2.Val > OPEN_SOCKET_TIMEOUT)
                {
                    //invalidate the expired socket request
                    openSocketInfo.status.bits.requestIsOpen = 0;
                }
            }
        }
    }


    #ifdef NWK_ROLE_COORDINATOR
        uint8_t FindDepth(uint8_t nodeMSB)
        {
            uint8_t depth;
            uint8_t target;
            uint8_t loopCheck;
            
            if( nodeMSB == 0 )
            {
                return 0;
            }
            if( FamilyTree[nodeMSB] >= NUM_COORDINATOR )
            {
                return 0;
            }

            depth = 1;
            target = nodeMSB;
            loopCheck = 0;
            while( FamilyTree[target] )
            {
                target = FamilyTree[target];
                depth++;
                if( loopCheck++ > NUM_COORDINATOR )
                {
                    break;
                }
            }
            return depth;   
        }
    
        #define DEST_NULL   0        
        #define DEST_PARENT 1
        #define DEST_CHILD  2
        #define DEST_PEER   3
        
        uint8_t FindRoutingPath(uint8_t destMSB)
        {
            uint8_t i;
            uint8_t j;
            uint8_t target;
            uint8_t treeSteps;
            uint8_t loopCheck;
            uint8_t destRole = DEST_NULL;
            
            // house keeping check
            if( (destMSB > NUM_COORDINATOR) || (FamilyTree[0] == 0 && FamilyTree[destMSB] > NUM_COORDINATOR) )
            {
                return 0xFF;
            }
            
            // first check if I know this guy directly
            if( RoutingTable[destMSB/8] & (0x01 << (destMSB % 8)) )
            {
                // yes, I know this guy directly
                return destMSB;
            }  
            
            // second, check if my neighbor know this guy
            for(i = 0; i < NUM_COORDINATOR; i++)
            {
                if( NeighborRoutingTable[i][destMSB/8] & (0x01 << (destMSB % 8)) )
                {
                    return i;
                }    
            }

            // check the maximum steps I need to rout insted of tree routing
            target = destMSB;
            loopCheck = 0;
            while(target)
            {
                if( FamilyTree[target] == myShortAddress.v[1] )
                {
                    destRole = DEST_CHILD;
                    treeSteps = FindDepth(destMSB) - FindDepth(myShortAddress.v[1]);
                    break;
                }
                target = FamilyTree[target];   
                if( loopCheck++ > NUM_COORDINATOR )
                {
                    break;
                } 
            }
                      
            if ( destRole == DEST_NULL )
            {
                target = myShortAddress.v[1];
                loopCheck = 0;
                while(target)
                {
                    if( FamilyTree[target] == destMSB )
                    {
                        destRole = DEST_PARENT;
                        treeSteps = FindDepth(myShortAddress.v[1]) - FindDepth(destMSB);
                        break;
                    }  
                    target = FamilyTree[target];  
                    if( loopCheck++ > NUM_COORDINATOR )
                    {
                        break;
                    }  
                } 
            } 
                
            if( destRole == DEST_NULL )
            {
                destRole = DEST_PEER;
                treeSteps = FindDepth(destMSB);
            }
                      
            for(i = 1; i < treeSteps; i++)
            {
                uint8_t k;
                
                // check if I know this guy along with the parent path
                target = destMSB;
                for(j = 0; j < i; j++)
                {
                    target = FamilyTree[target];
                }    
                if( RoutingTable[target/8] & (0x01 << (target % 8)) )
                {
                    return target;
                }    
                
                // check if I know this guy along with the child path
                for(k = 0; k < NUM_COORDINATOR; k++)
                {
                    if( FamilyTree[k] < NUM_COORDINATOR )
                    {
                        target = k;
                        for(j = 0; j < i; j++)
                        {
                            target = FamilyTree[target];
                        }    
                        if( (target == destMSB) && (RoutingTable[k/8] & (0x01 << (k%8))) )
                        {
                            return k;
                        }    
                    }    
                }
                
                // check if my neighbor know this guy along the parent path
                target = destMSB;
                for(j = 0; j < i; j++)
                {
                    target = FamilyTree[target];
                }
                for(k = 0; k < NUM_COORDINATOR; k++)
                {
                    if( NeighborRoutingTable[k][target/8] & (0x01 << (target%8)) )
                    {
                        return k;
                    }    
                }    
                
                // check if my neighbor know this guy along the child path
                for(k = 0; k < NUM_COORDINATOR; k++)
                {
                    if( FamilyTree[k] < NUM_COORDINATOR )
                    {
                        target = k;
                        for(j = 0; j < i; j++)
                        {
                            target = FamilyTree[target];
                        }    
                        if( target == destMSB )
                        {
                            for(j = 0; j < NUM_COORDINATOR; j++)
                            {
                                if( NeighborRoutingTable[j][k/8] & (0x01 << (k%8)) )
                                {
                                    return j;
                                }    
                            }    
                        }    
                    }    
                }        
                
            } 
                      
            // I have looked forward for three steps, now I need to go tree routing
            // check if the destination is my child
            if( destRole == DEST_CHILD )
            {
                i = destMSB;
                loopCheck = 0;
                while( (FamilyTree[i] != myShortAddress.v[1]) ||
                    (RoutingTable[i/8] & (0x01 << (i % 8))) )
                {
                    i = FamilyTree[i];
                    if( loopCheck++ > NUM_COORDINATOR )
                    {
                        break;
                    }
                }
                return i;  
            }

            i = FamilyTree[myShortAddress.v[1]];
            target = i;
            loopCheck = 0;
            while( (FamilyTree[i] != i) && (i != destMSB) )
            {
                if( RoutingTable[i/8] & (0x01 << (i%8)) )
                {
                    target = i;
                }
                i = FamilyTree[i];
                if( loopCheck++ > NUM_COORDINATOR )
                {
                    break;
                }
            }
            return target;

        }    
    
    
        /*********************************************************************
         * Function:        BOOL RouteMessage(API_UINT16_UNION PANID,
         *                                    API_UINT16_UNION ShortAddress, BOOL SecEn)
         *
         * PreCondition:    Coordinator has joined the network
         *
         * Input:           PANID           - PAN identifier of the destination 
         *                                    node
         *                  ShortAddress    - Network short address of the 
         *                                    destination node
         *                  SecEn           - Boolean to indicate if the message
         *                                    payload needs to be secured
         *
         * Output:          A boolean to indicate if routing successful
         *
         * Side Effects:    None
         *
         * Overview:        This function is the backbone of MiWi PRO routing
         *                  mechanism. MiWi PRO uses this function to route
         *                  the message across multiple hops to reach the
         *                  destination node.
         ********************************************************************/
        BOOL RouteMessage(API_UINT16_UNION PANID, API_UINT16_UNION ShortAddress, BOOL SecEn)
        {
            uint8_t i;
            uint8_t parentNode = ShortAddress.v[1];
            
            if( parentNode > 0x7F )
            {
                return false;
            }
            
            if( FamilyTree[0] == 0 && FamilyTree[parentNode] > NUM_COORDINATOR )
            {
                // if the destination node has not join the network
                return false;
            } 

            for(i = 0; i < PACKET_RECORD_SIZE; i++)
            {
                if( (PacketRecords[i].RxCounter > 0 ) &&
                    (PacketRecords[i].AltSourceAddr.Val == ShortAddress.Val) &&
                    (PacketRecords[i].MiWiPROSeq == TxBuffer[10]) 
                  )
                {
                    MiWiPROStatus.bits.COMM_ROUTING_TABLE = ROUTING_TABLE_BROADCAST;
                    backgroundTaskTick = MiWi_TickGet();
                    return false;
                }
            }
            
            for(i = 0; i < PACKET_RECORD_SIZE; i++)
            {
                if( PacketRecords[i].RxCounter == 0 )
                {
                    PacketRecords[i].RxCounter = INDIRECT_MESSAGE_TIMEOUT_CYCLE + 1;
                    PacketRecords[i].MiWiPROSeq = TxBuffer[10];
                    PacketRecords[i].AltSourceAddr.Val = ShortAddress.Val;
                    PacketRecords[i].StartTick = MiWi_TickGet();
                    break;
                }    
            }        
            
            // first check if it is one of my end device children
            if( parentNode == myShortAddress.v[1] )
            {
                // destination is my child
                if( ShortAddress.v[0] > 0x80 )
                {
                    #if defined(ENABLE_INDIRECT_MESSAGE)
                        // this is a sleeping device, need indirect message
                        #if defined(IEEE_802_15_4)
                            return SaveIndirectMessage(false, PANID, ShortAddress.v, true, SecEn);
                        #else
                            tempShortAddress.Val = ShortAddress.Val;
                            if( (i = SearchForShortAddress()) == 0xFF )
                            {
                                return SaveIndirectMessage(false, ShortAddress.v, true, SecEn);
                            }

                            return SaveIndirectMessage(false, ConnectionTable[i].Address, false, SecEn);
                        #endif
                    #else
                        return false;
                    #endif
                }
                else
                {
                    MTP.flags.Val = 0;
                    MTP.flags.bits.ackReq = 1;
                    MTP.flags.bits.secEn = SecEn;
                    
                    tempShortAddress.Val = ShortAddress.Val;
                    
                    #if defined(IEEE_802_15_4)
                        MTP.altDestAddr = true;
                        MTP.altSrcAddr = true;
                        MTP.DestAddress = tempShortAddress.v;
                        MTP.DestPANID.Val = myPANID.Val;
                    #else
                        if( (i = SearchForShortAddress()) != 0xFF )
                        {
                            MTP.DestAddress = ConnectionTable[i].Address;
                        }
                        else
                        {
                            // the children's info is not in the connection table
                            // should not happen
                            return false;
                        }
                    #endif
                    return MiMAC_SendPacket(MTP, TxBuffer, TxData);
                }
            }
                    
            // now routing
            parentNode = FindRoutingPath(ShortAddress.v[1]);
            if( parentNode == 0xFF )
            {
                return false;
            }
            
            if( RouterFailures[parentNode] >= MAX_ROUTING_FAILURE )
            {
                uint8_t i;

                RouterFailures[parentNode] = 0;
                RoutingTable[parentNode/8] &= (0x01 << (parentNode % 8) ) ^ 0xFF;
                for(i = 0; i < NUM_COORDINATOR/8; i++)
                {
                    NeighborRoutingTable[parentNode][i] = 0;
                }
                /*
                if (parentNode == myParent)
                {
                    MiWiPROStateMachine.bits.memberOfNetwork = 0;
                    myParent = 0xFF;
                }  
                */
                #if defined(ENABLE_NETWORK_FREEZER)
                    nvmPutRoutingTable(RoutingTable);
                    nvmPutNeighborRoutingTable(NeighborRoutingTable);
                #endif
                
                MiWiPROStatus.bits.COMM_ROUTING_TABLE = ROUTING_TABLE_BROADCAST;
                backgroundTaskTick = MiWi_TickGet();
                return false;
            }

            MTP.flags.Val = 0;
            MTP.flags.bits.ackReq = 1;
            MTP.flags.bits.secEn = SecEn;

            tempShortAddress.v[0] = 0;
            tempShortAddress.v[1] = parentNode;
            
            #if defined(IEEE_802_15_4)
                MTP.altDestAddr = true;
                MTP.altSrcAddr = true;
                MTP.DestAddress = tempShortAddress.v;
                MTP.DestPANID.Val = myPANID.Val;
            #else
                if( (i = SearchForShortAddress()) != 0xFF )
                {
                    MTP.DestAddress = ConnectionTable[i].Address;
                }
                else
                {
                    // highly impossible for none 15.4, where the
                    // access of Coordinators not through beacon
                    RouterFailures[parentNode]++;
                    return false;
                }
            #endif
            if( MiMAC_SendPacket(MTP, TxBuffer, TxData) == false )
            {
                RouterFailures[parentNode]++;
                return false;
            }

            RouterFailures[parentNode] = 0;
            return true;

        }
    #endif




    /*********************************************************************
     * Function:        BOOL SendMACPacket(uint8_t *PANID, uint8_t *Address,
     *                                     uint8_t PacketType, uint8_t ModeMask)
     *
     * PreCondition:    Transceiver has been initialized properly
     *
     * Input:           PANID           - PAN identifier of the destination 
     *                                    node. (IEEE 802.15.4 only)
     *                  Address         - Address of the destination node. 
     *                                    Always long address for non-IEEE
     *                                    802.15.4, can be short or long
     *                                    address for IEEE 802.15.4.
     *                  PacketType      - The packet type, passed to
     *                                    transceiver driver directly
     *                  ModeMask        - The source and destination address
     *                                    mode (short or long) maks. 
     *                                    (IEEE 802.15.4 only)
     *
     * Output:          A boolean to indicate if packet sent successfully
     *
     * Side Effects:    None
     *
     * Overview:        This function is a bridge from MiWi PRO layer to 
     *                  transceiver MAC layer. It organizes the MiMAC
     *                  layer parameters and return  the status of
     *                  MiMAC transmission status.
     ********************************************************************/
    #if defined(IEEE_802_15_4)
        BOOL SendMACPacket(uint8_t *PANID, uint8_t *Address, uint8_t PacketType, uint8_t ModeMask)
    #else
        BOOL SendMACPacket(uint8_t *Address, uint8_t PacketType)
    #endif
    {   
        MTP.flags.Val = 0;
        
        MTP.flags.bits.packetType = PacketType;
        if( Address == NULL )
        {
            MTP.flags.bits.broadcast = 1;
        }
        MTP.flags.bits.ackReq = (MTP.flags.bits.broadcast) ? 0:1;
        MTP.flags.bits.sourcePrsnt = 1;
        
        MTP.DestAddress = Address;
        
        #if defined(IEEE_802_15_4)
            if( (ModeMask & MSK_ALT_DST_ADDR) > 0)
            {
                MTP.altDestAddr = true;
            }
            else
            {
                MTP.altDestAddr = false;
            }
            if( (ModeMask & MSK_ALT_SRC_ADDR) > 0)
            {
                MTP.altSrcAddr = true;
            }
            else
            {
                MTP.altSrcAddr = false;
            }
            MTP.DestPANID.v[0] = PANID[0];
            MTP.DestPANID.v[1] = PANID[1];
        #endif
        
        return MiMAC_SendPacket(MTP, TxBuffer, TxData);
    }

    /*********************************************************************
     * Function:        void SendIndirectPacket(uint8_t *Address,
     *                                          uint8_t *AltAddress,
     *                                          BOOL isAltAddress)
     *
     * PreCondition:    Node has joined the network
     *
     * Input:           Address         - Pointer to the long address
     *                  AltAddress      - Pointer to the short address
     *                  isAltAddress    - Boolean to indicate if use long
     *                                    or short address
     *
     * Output:          None
     *
     * Side Effects:    None
     *
     * Overview:        This function is used to send an indirect message
     *                  to a sleeping device
     ********************************************************************/
    #if defined(NWK_ROLE_COORDINATOR) && defined(ENABLE_INDIRECT_MESSAGE)
        #if defined(ENABLE_TIME_SYNC) && !defined(ENABLE_SLEEP)
            MIWI_TICK tmpTick;
        #endif
        void SendIndirectPacket(uint8_t *Address, uint8_t *AltAddress, BOOL isAltAddress)
        {
            uint8_t i,j;
            uint8_t index;
            uint8_t packetType = PACKET_TYPE_DATA;
            #if defined(ENABLE_TIME_SYNC) && !defined(ENABLE_SLEEP)
                API_UINT16_UNION tmpW;
            #endif
            
            MAC_FlushTx();
            #if defined(ENABLE_TIME_SYNC) && !defined(ENABLE_SLEEP)
                MiApp_WriteData(MAC_COMMAND_TIME_SYNC_DATA_PACKET);
                packetType = PACKET_TYPE_COMMAND;
                tmpTick = MiWi_TickGet();
                if( (tmpTick.Val - TimeSyncTick.Val) < ((ONE_SECOND) * RFD_WAKEUP_INTERVAL) )
                {
                    //tmpW.Val = (((ONE_SECOND) * RFD_WAKEUP_INTERVAL) - (tmpTick.Val - TimeSyncTick.Val) + ( TimeSlotTick.Val * TimeSyncSlot ) ) / (ONE_SECOND * 16);
                    tmpW.Val = (((ONE_SECOND) * RFD_WAKEUP_INTERVAL) - (tmpTick.Val - TimeSyncTick.Val) + ( TimeSlotTick.Val * TimeSyncSlot ) ) / SYMBOLS_TO_TICKS((uint32_t)0xFFFF * MICRO_SECOND_PER_COUNTER_TICK / 16);
                    MiApp_WriteData(tmpW.v[0]);
                    MiApp_WriteData(tmpW.v[1]);
                    //tmpW.Val = 0xFFFF - (uint16_t)((TICKS_TO_SYMBOLS((((ONE_SECOND) * RFD_WAKEUP_INTERVAL) - (tmpTick.Val - TimeSyncTick.Val)  + ( TimeSlotTick.Val * TimeSyncSlot ) + TimeSlotTick.Val/2 - (ONE_SECOND * tmpW.Val * 16) )) * 16 / 250));
                    //tmpW2.Val = 0xFFFF - (uint16_t)((TICKS_TO_SYMBOLS((((ONE_SECOND) * RFD_WAKEUP_INTERVAL) - (tmpTick.Val - TimeSyncTick.Val)  + ( TimeSlotTick.Val * TimeSyncSlot ) + TimeSlotTick.Val/2 - ((uint32_t)0xFFFF * tmpW.Val) )) * 16 / MICRO_SECOND_PER_COUNTER_TICK));
                    //tmpW.Val = 0xFFFF - (uint16_t)((TICKS_TO_SYMBOLS((((ONE_SECOND) * RFD_WAKEUP_INTERVAL) - (tmpTick.Val - TimeSyncTick.Val)  + ( TimeSlotTick.Val * TimeSyncSlot ) + TimeSlotTick.Val/2 - SYMBOLS_TO_TICKS((uint32_t)0xFFFF * MICRO_SECOND_PER_COUNTER_TICK / 16 * tmpW.Val) )) * 16 / MICRO_SECOND_PER_COUNTER_TICK));
                    tmpW.Val = 0xFFFF - (uint16_t)((TICKS_TO_SYMBOLS(( ((ONE_SECOND) * RFD_WAKEUP_INTERVAL) - (tmpTick.Val - TimeSyncTick.Val)  + ( TimeSlotTick.Val * TimeSyncSlot ) + TimeSlotTick.Val/2)) - ((uint32_t)0xFFFF * MICRO_SECOND_PER_COUNTER_TICK / 16 * tmpW.Val) ) * 16 / MICRO_SECOND_PER_COUNTER_TICK);
                    if( TimeSyncSlot < TIME_SYNC_SLOTS )
                    {
                        TimeSyncSlot++;
                    }    
                    MiApp_WriteData(tmpW.v[0]);
                    MiApp_WriteData(tmpW.v[1]);
               }
                else
                {
                    MiApp_WriteData(0);
                    MiApp_WriteData(0);
                    MiApp_WriteData(0x5F);
                    MiApp_WriteData(0xF0);
                }        

            #endif
            
            
            if( isAltAddress )
            {
                tempShortAddress.v[0] = AltAddress[0];
                tempShortAddress.v[1] = AltAddress[1];
                if( (index = SearchForShortAddress()) == 0xFF )
                {
                    goto NO_INDIRECT_MESSAGE;
                }       
            }
            else
            {
                for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                {
                    tempLongAddress[i] = Address[i];
                }
                if( (index = SearchForLongAddress()) == 0xFF )
                {
                    goto NO_INDIRECT_MESSAGE;
                }        
            }        
    
            for(i = 0; i < INDIRECT_MESSAGE_SIZE; i++)
            {
                if( indirectMessages[i].flags.bits.isValid )
                {
                    if( (indirectMessages[i].flags.bits.isBroadcast == 0) && 
                        indirectMessages[i].flags.bits.isAltAddr &&
                        (ConnectionTable[index].AltAddress.v[0] == indirectMessages[i].DestAddress[0]) && 
                        (ConnectionTable[index].AltAddress.v[1] == indirectMessages[i].DestAddress[1]) )
                    {
                        for(j = 0; j < indirectMessages[i].PayLoadSize; j++)
                        {
                            MiApp_WriteData(indirectMessages[i].PayLoad[j]);
                        }
                        
                        #if defined(IEEE_802_15_4)
                            if( indirectMessages[i].flags.bits.isCommand )
                            {
                                #if defined(ENABLE_TIME_SYNC) && !defined(ENABLE_SLEEP)
                                    TxBuffer[0] = MAC_COMMAND_TIME_SYNC_COMMAND_PACKET;
                                #endif
                                SendMACPacket(myPANID.v, ConnectionTable[index].AltAddress.v, PACKET_TYPE_COMMAND, MSK_ALT_DST_ADDR | MSK_ALT_SRC_ADDR);
                            }
                            else
                            { 
                                MTP.flags.Val = 0;
                                MTP.flags.bits.packetType = packetType;
                                MTP.flags.bits.ackReq = 1;
                                MTP.flags.bits.secEn = indirectMessages[i].flags.bits.isSecured;
                                MTP.DestAddress = ConnectionTable[index].AltAddress.v;
                                MTP.altDestAddr = true;
                                MTP.altSrcAddr = true;
                                MTP.DestPANID.Val = indirectMessages[i].DestPANID.Val;
                                MiMAC_SendPacket(MTP, TxBuffer, TxData);
                                //SendMACPacket(myPANID.v, AltAddress, PACKET_TYPE_DATA, MSK_ALT_DST_ADDR | MSK_ALT_SRC_ADDR);
                            }
                        #else
                            MTP.flags.Val = 0;
                            MTP.flags.bits.packetType = packetType;
                            MTP.flags.bits.ackReq = 1;
                            MTP.flags.bits.secEn = indirectMessages[i].flags.bits.isSecured;
                            MTP.DestAddress = ConnectionTable[index].Address;
                            if( indirectMessages[i].flags.bits.isCommand )
                            {
                                MTP.flags.bits.packetType = PACKET_TYPE_COMMAND;
                                MTP.flags.bits.sourcePrsnt = 1;
                                #if defined(ENABLE_TIME_SYNC) && !defined(ENABLE_SLEEP)
                                    TxBuffer[0] = MAC_COMMAND_TIME_SYNC_COMMAND_PACKET;
                                #endif
                            }

                            MiMAC_SendPacket(MTP, TxBuffer, TxData);
                        #endif
                        
                        indirectMessages[i].flags.bits.isValid = 0;
                        return;
                    }
    
                    if( (indirectMessages[i].flags.bits.isBroadcast == 0) &&
                        (indirectMessages[i].flags.bits.isAltAddr == 0) &&
                        isSameAddress( ConnectionTable[index].Address, indirectMessages[i].DestAddress) )
                    {
                        for(j = 0; j < indirectMessages[i].PayLoadSize; j++)
                        {
                            MiApp_WriteData(indirectMessages[i].PayLoad[j]);
                        }
                        #if defined(IEEE_802_15_4)
                            if( indirectMessages[i].flags.bits.isCommand )
                            {
                                #if defined(ENABLE_TIME_SYNC) && !defined(ENABLE_SLEEP)
                                    TxBuffer[0] = MAC_COMMAND_TIME_SYNC_COMMAND_PACKET;
                                #endif
                                SendMACPacket(myPANID.v, ConnectionTable[index].Address, PACKET_TYPE_COMMAND, 0);
                            }
                            else
                            {
                                MTP.flags.Val = 0;
                                MTP.flags.bits.packetType = packetType;
                                MTP.flags.bits.ackReq = 1;
                                MTP.flags.bits.secEn = indirectMessages[i].flags.bits.isSecured;
                                MTP.DestAddress = ConnectionTable[index].Address;
                                MTP.altDestAddr = false;
                                MTP.altSrcAddr = true;
                                MTP.DestPANID.Val = indirectMessages[i].DestPANID.Val;

                                MiMAC_SendPacket(MTP, TxBuffer, TxData);
                                //SendMACPacket(myPANID.v, Address, PACKET_TYPE_DATA, 0);
                            }
                        #else
                            MTP.flags.Val = 0;
                            MTP.flags.bits.packetType = packetType;
                            MTP.flags.bits.ackReq = 1;
                            MTP.flags.bits.secEn = indirectMessages[i].flags.bits.isSecured;
                            MTP.DestAddress = ConnectionTable[index].Address;
                            if( indirectMessages[i].flags.bits.isCommand )
                            {
                                MTP.flags.bits.packetType = PACKET_TYPE_COMMAND;
                                #if defined(ENABLE_TIME_SYNC) && !defined(ENABLE_SLEEP)
                                    TxBuffer[0] = MAC_COMMAND_TIME_SYNC_COMMAND_PACKET;
                                #endif
                                MTP.flags.bits.sourcePrsnt = 1;
                            }

                            MiMAC_SendPacket(MTP, TxBuffer, TxData);
                        #endif
                        
                        indirectMessages[i].flags.bits.isValid = 0;
                        return;
                    }
                }
            }
            
            
            for(i = 0; i < INDIRECT_MESSAGE_SIZE; i++)
            {
                if( indirectMessages[i].flags.bits.isValid )
                {        
                    if( indirectMessages[i].flags.bits.isBroadcast )
                    {
                        for(j = 0; j < indirectMessages[i].PayLoadSize; j++)
                        {
                            MiApp_WriteData(indirectMessages[i].PayLoad[j]);
                        }
                        #if defined(IEEE_802_15_4)
                            MTP.flags.Val = 0;
                            MTP.flags.bits.packetType = packetType;
                            if( indirectMessages[i].flags.bits.isCommand )
                            {
                                MTP.flags.bits.packetType = PACKET_TYPE_COMMAND;
                                #if defined(ENABLE_TIME_SYNC) && !defined(ENABLE_SLEEP)
                                    TxBuffer[0] = MAC_COMMAND_TIME_SYNC_COMMAND_PACKET;
                                #endif
                            }
                            MTP.flags.bits.ackReq = 1;
                            MTP.flags.bits.sourcePrsnt = 1;
                            MTP.flags.bits.secEn = indirectMessages[i].flags.bits.isSecured;
                            MTP.altSrcAddr = true;
                            if( isAltAddress )
                            {
                                MTP.altDestAddr = true;
                                MTP.DestAddress = ConnectionTable[index].AltAddress.v;
                            }
                            else
                            {
                                MTP.altDestAddr = false;
                                MTP.DestAddress = ConnectionTable[index].Address;
                            }
                            MTP.DestPANID.Val = indirectMessages[i].DestPANID.Val;
                            
                            MiMAC_SendPacket(MTP, TxBuffer, TxData);
                        #else
                            MTP.flags.Val = 0;
                            MTP.flags.bits.packetType = packetType;
                            MTP.flags.bits.ackReq = 1;
                            MTP.flags.bits.secEn = indirectMessages[i].flags.bits.isSecured;
                            if( indirectMessages[i].flags.bits.isCommand )
                            {
                                MTP.flags.bits.packetType = PACKET_TYPE_COMMAND;
                                #if defined(ENABLE_TIME_SYNC) && !defined(ENABLE_SLEEP)
                                    TxBuffer[0] = MAC_COMMAND_TIME_SYNC_COMMAND_PACKET;
                                #endif
                            }
                            MTP.DestAddress = ConnectionTable[index].Address;

                            MiMAC_SendPacket(MTP, TxBuffer, TxData);
                        #endif 
                        return;   
                    }
                }
            }

NO_INDIRECT_MESSAGE:            
            // no indirect message found
            #if defined(IEEE_802_15_4)
                if( isAltAddress )
                {
                    SendMACPacket(myPANID.v, AltAddress, packetType, MSK_ALT_DST_ADDR | MSK_ALT_SRC_ADDR );
                }
                else
                {
                    SendMACPacket(myPANID.v, Address, packetType, MSK_ALT_SRC_ADDR);
                }
            #else
                MTP.flags.Val = 0;
                MTP.flags.bits.packetType = packetType;
                MTP.flags.bits.ackReq = 1;
                MTP.DestAddress = Address;
                MiMAC_SendPacket(MTP, TxBuffer, TxData);
            #endif
        }
    #endif
    
    
    /*********************************************************************
     * Function:        uint8_t findNextNetworkEntry(void)
     *
     * PreCondition:    None
     *
     * Input:           None
     *
     * Output:          uint8_t 0xFF if there is no room in the network table
     *                  otherwise this function returns the index of the 
     *                  first blank entry in the table
     *
     * Side Effects:    None
     *
     * Overview:        This function is used to determine the next available
     *                  slot in the network table.  This function can be used
     *                  to manually create entries in the networkTable
     ********************************************************************/
    uint8_t findNextNetworkEntry(void)
    {
        uint8_t i;
        
        for(i=0;i<CONNECTION_SIZE;i++)
        {
            if(ConnectionTable[i].status.bits.isValid == 0)
            {
                return i;
            }
        }
        return 0xFF;
    }
    
    
    
    /*********************************************************************
     * Function:        void DumpNetworkTable(void)
     *
     * PreCondition:    None
     *
     * Input:           None
     *
     * Output:          None
     *
     * Side Effects:    None
     *
     * Overview:        This function dumps the contents of many tables to
     *                  the console.  This is a debugging\helper function only
     *                  and is not used by the stack
     ********************************************************************/
    #if defined(ENABLE_DUMP)
    void DumpConnection(uint8_t index)
    {
        uint8_t i, j;
        
        if( index == 0xFF )
        {
            Printf("\r\n\r\nAddress: 0x");
            for(i = 0; i < MY_ADDRESS_LENGTH; i++)
            {
                PrintChar(myLongAddress[MY_ADDRESS_LENGTH-1-i]);
            }
            Printf("  Short Addr: ");
            PrintChar(myShortAddress.v[1]);
            PrintChar(myShortAddress.v[0]);
            Printf("  PANID: ");
            PrintChar(myPANID.v[1]);
            PrintChar(myPANID.v[0]);
            Printf("  Channel: ");
            PrintDec(currentChannel);
        }    
        
        ConsolePutROMString((ROM char*)"\r\nAvailable nodes\r\nHandle RX DC PANID ADDR LONG_ADDR        PEER_INFO\r\n");
        if( index == 0xFF )
        {
            for(i=0;i<CONNECTION_SIZE;i++)
            {
                if(ConnectionTable[i].status.bits.isValid)
                {
                    PrintChar(i);
                    ConsolePut(' ');
                    ConsolePut(' ');
                    ConsolePut(' ');
                    ConsolePut(' ');
                    ConsolePut(' ');
                    
                    if(ConnectionTable[i].status.bits.RXOnWhenIdle == 1)
                    {
                        ConsolePut('Y');
                    }
                    else
                    {
                        ConsolePut('N');
                    }
                    ConsolePut(' ');
                    ConsolePut(' ');
                    
                    if(ConnectionTable[i].status.bits.directConnection == 1)
                    {
                        ConsolePut('Y');
                    }
                    else
                    {
                        ConsolePut('N');
                    }
                    ConsolePut(' ');
                    ConsolePut(' ');
                                  
                    if(ConnectionTable[i].status.bits.shortAddressValid)
                    {
                        //this is a neighbor
                        PrintChar(ConnectionTable[i].PANID.v[1]);
                        PrintChar(ConnectionTable[i].PANID.v[0]);
                        ConsolePut(' ');
                        ConsolePut(' ');
                        PrintChar(ConnectionTable[i].AltAddress.v[1]);
                        PrintChar(ConnectionTable[i].AltAddress.v[0]);
                        ConsolePut(' ');
                    }
                    else
                    {
                        ConsolePut('?');
                        ConsolePut('?');
                        ConsolePut('?');
                        ConsolePut('?');
                        ConsolePut(' ');
                        ConsolePut(' ');
                        ConsolePut('?');
                        ConsolePut('?');
                        ConsolePut('?');
                        ConsolePut('?');
                        ConsolePut(' ');
                    }
                    
                    if(ConnectionTable[i].status.bits.longAddressValid)
                    {
                        
                        for(j = 0; j < MY_ADDRESS_LENGTH; j++)
                        {
                            PrintChar(ConnectionTable[i].Address[MY_ADDRESS_LENGTH-1-j]);
                        }
                    }
                    
                    #if ADDITIONAL_NODE_ID_SIZE > 0
                        ConsolePut(' ');
                        #if MY_ADDRESS_LENGTH < 8
                            for(j = 0; j < (8-MY_ADDRESS_LENGTH); j++)
                            {
                                Printf("  ");
                            }
                        #endif
                        for(j = 0; j < ADDITIONAL_NODE_ID_SIZE; j++)
                        {
                            PrintChar(ConnectionTable[i].PeerInfo[j]);
                        }
                    #endif
                    
                    ConsolePut('\r');
                    ConsolePut('\n');
                }
            }
        }
        else
        {
            if(ConnectionTable[index].status.bits.isValid)
            {
                PrintChar(index);
                ConsolePut(' ');
                ConsolePut(' ');
                ConsolePut(' ');
                ConsolePut(' ');
                ConsolePut(' ');
                
                if(ConnectionTable[index].status.bits.RXOnWhenIdle == 1)
                {
                    ConsolePut('Y');
                }
                else
                {
                    ConsolePut('N');
                }
                ConsolePut(' ');
                ConsolePut(' ');
                
                if(ConnectionTable[index].status.bits.directConnection == 1)
                {
                    ConsolePut('Y');
                }
                else
                {
                    ConsolePut('N');
                }
                ConsolePut(' ');
                ConsolePut(' ');
                              
                if(ConnectionTable[index].status.bits.shortAddressValid)
                {
                    //this is a neighbor
                    PrintChar(ConnectionTable[index].PANID.v[1]);
                    PrintChar(ConnectionTable[index].PANID.v[0]);
                    ConsolePut(' ');
                    ConsolePut(' ');
                    PrintChar(ConnectionTable[index].AltAddress.v[1]);
                    PrintChar(ConnectionTable[index].AltAddress.v[0]);
                    ConsolePut(' ');
                }
                else
                {
                    ConsolePut('?');
                    ConsolePut('?');
                    ConsolePut('?');
                    ConsolePut('?');
                    ConsolePut(' ');
                    ConsolePut(' ');
                    ConsolePut('?');
                    ConsolePut('?');
                    ConsolePut('?');
                    ConsolePut('?');
                    ConsolePut(' ');
                }
                
                if(ConnectionTable[index].status.bits.longAddressValid)
                {
                    
                    for(j = 0; j < MY_ADDRESS_LENGTH; j++)
                    {
                        PrintChar(ConnectionTable[index].Address[MY_ADDRESS_LENGTH-1-j]);
                    }
                }
                
                #if ADDITIONAL_NODE_ID_SIZE > 0
                    ConsolePut(' ');
                    #if MY_ADDRESS_LENGTH < 8
                        for(j = 0; j < (8-MY_ADDRESS_LENGTH); j++)
                        {
                            Printf("  ");
                        }
                    #endif
                    for(j = 0; j < ADDITIONAL_NODE_ID_SIZE; j++)
                    {
                        PrintChar(ConnectionTable[index].PeerInfo[j]);
                    }
                #endif
                
                ConsolePut('\r');
                ConsolePut('\n');
            }
        }
    }
    #else
    void DumpNetworkTable(void){}
    #endif
    
    
    /*********************************************************************
     * Function:        void DiscoverNodeByEUI(void)
     *
     * PreCondition:    tempLongAddress[0-7] need to be set to the address
     *                  that needs to be discovered
     *
     * Input:           None
     *
     * Output:          None
     *
     * Side Effects:    None
     *
     * Overview:        This function constructs and sends out the from to
     *                  discover a device by their EUI address
     ********************************************************************/

    #if defined(SUPPORT_EUI_ADDRESS_SEARCH)
    void DiscoverNodeByEUI(void)
    {
        uint8_t i;
        
        MiWiPROStateMachine.bits.EUISearching = 1;
        #ifndef NWK_ROLE_COORDINATOR
            //if we a member of the network
            if(MiWiPROStateMachine.bits.memberOfNetwork == 1)
            {
                MAC_FlushTx();
    
                TxBuffer[TxData++] = defaultHops;
                TxBuffer[TxData++] = 0x06;                          //Frame Control
                TxBuffer[TxData++] = 0xFF;                          //dest PANID LSB
                TxBuffer[TxData++] = 0xFF;                          //dest PANID MSB
                TxBuffer[TxData++] = 0xFF;                          //dest address LSB
                TxBuffer[TxData++] = 0xFF;                          //dest address MSB
                TxBuffer[TxData++] = myPANID.v[0];                  //source PANID LSB
                TxBuffer[TxData++] = myPANID.v[1];                  //source PANID MSB
                TxBuffer[TxData++] = myShortAddress.v[0];           //source address LSB
                TxBuffer[TxData++] = myShortAddress.v[1];           //source address MSB
                TxBuffer[TxData++] = MiWiPROSeqNum++;               //seq num
                TxBuffer[TxData++] = MIWI_PRO_STACK_REPORT_TYPE;        //Report Type
                TxBuffer[TxData++] = EUI_ADDRESS_SEARCH_REQUEST;    //Report ID
                for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                {
                    TxBuffer[TxData++] = tempLongAddress[i];            //Data byte 0
                }
                
                MTP.flags.Val = 0;
                MTP.flags.bits.ackReq = 1;
                MTP.flags.bits.sourcePrsnt = 1;
                
                #if defined(IEEE_802_15_4)
                    MTP.DestAddress = ConnectionTable[myParent].AltAddress.v;
                    MTP.DestPANID.Val = ConnectionTable[myParent].PANID.Val;
                    MTP.altDestAddr = true;
                    MTP.altSrcAddr = true;
                #else
                    MTP.DestAddress = ConnectionTable[myParent].LongAddress;
                #endif
                
                MiMAC_SendPacket(MTP, TxBuffer, TxData);
            }
        #else
            //if I am a coordinator capable device
            if(MiWiPROStateMachine.bits.memberOfNetwork == 1)
            {
                MAC_FlushTx();
                TxBuffer[TxData++] = defaultHops;      
                TxBuffer[TxData++] = 0x06;      //Frame Control
                TxBuffer[TxData++] = 0xFF;      //dest PANID LSB
                TxBuffer[TxData++] = 0xFF;      //dest PANID MSB
                TxBuffer[TxData++] = 0xFF;      //dest address LSB
                TxBuffer[TxData++] = 0xFF;      //dest address MSB
                TxBuffer[TxData++] = myPANID.v[0];      //source PANID LSB
                TxBuffer[TxData++] = myPANID.v[1];      //source PANID MSB
                TxBuffer[TxData++] = myShortAddress.v[0];      //source address LSB
                TxBuffer[TxData++] = myShortAddress.v[1];      //source address MSB
                TxBuffer[TxData++] = MiWiPROSeqNum++;      //seq num
                TxBuffer[TxData++] = MIWI_PRO_STACK_REPORT_TYPE;      //Report Type
                TxBuffer[TxData++] = EUI_ADDRESS_SEARCH_REQUEST;      //Report ID
                for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                {
                    TxBuffer[TxData++] = tempLongAddress[i];      //Data byte 0
                }
                
                MTP.flags.Val = 0;
                MTP.flags.bits.broadcast = 1;
                
                #if defined(IEEE_802_15_4)
                    MTP.DestAddress = NULL;
                    MTP.altDestAddr = true;
                    MTP.altSrcAddr = true;
                    MTP.DestPANID.Val = myPANID.Val;
                #else
                    MTP.DestAddress = NULL;
                #endif
                
                MiMAC_SendPacket(MTP, TxBuffer, TxData);

            }
        #endif
    }
    #endif

    /*********************************************************************
     * Function:        void OpenSocket(void)
     *
     * PreCondition:    None
     *
     * Input:           uint8_t socketType - either CLUSTER_SOCKET for a cluster
     *                  socket or P2P_SOCKET for a P2P socket.
     *
     * Output:          None
     *
     * Side Effects:    This will send out a packet trying to create a socket
     *
     * Overview:        This will send out a packet to the PAN coordinator 
     *                  that is trying to link this device to another device.
     *                  the socket operation is complete when OpenSocketComplete()
     *                  returns true.  The status of the operation is retreived through
     *                  the OpenSocketSuccessful() function.  If it returns true
     *                  then OpenSocketHandle() returns the handle of the created
     *                  socket.  This value is valid until openSocket() is called
     *                  again.  If OpenSocketComplete() returns false then the
     *                  scan in still in progress.
     ********************************************************************/
    void OpenSocket(void)
    {
        uint8_t i;
        
        openSocketInfo.status.bits.matchFound = 0;
        
        #ifdef NWK_ROLE_COORDINATOR
        
        //If I am the PAN coordinator
        if(role == ROLE_PAN_COORDINATOR)
        {
            //if there isn't a request currently open
            if(openSocketInfo.status.bits.requestIsOpen == 0)
            {
                //I am the PAN coordinator, there is no reason to send a request out
                openSocketInfo.socketStart = MiWi_TickGet();
                openSocketInfo.ShortAddress1.Val = 0x0000;
                for(i=0;i<MY_ADDRESS_LENGTH;i++)
                {
                    openSocketInfo.LongAddress1[i] = myLongAddress[i];
                }
                openSocketInfo.status.bits.requestIsOpen = 1;
                openSocketInfo.status.bits.matchFound = 0;
                #if ADDITIONAL_NODE_ID_SIZE > 0 
                    for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
                    {
                        openSocketInfo.AdditionalNodeID1[i] = AdditionalNodeID[i];
                    }
                #endif
            }
            else
            {
                if(openSocketInfo.ShortAddress1.Val == 0x0000)
                {
                    //I am the PAN coordinator and I don't want to talk to myself so lets forget we even tried
                    return;
                }

                if(openSocketInfo.ShortAddress1.Val != myShortAddress.Val)
                {
	                MiApp_FlushTx();
                    TxBuffer[TxData++] = MIWI_PRO_STACK_REPORT_TYPE;      //Report Type
                    TxBuffer[TxData++] = OPEN_SOCKET_RESPONSE;      //Report ID
                    for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                    {
                        TxBuffer[TxData++] = myLongAddress[i];     
                    }
                    TxBuffer[TxData++] = myShortAddress.v[0];
                    TxBuffer[TxData++] = myShortAddress.v[1];
                    #if ADDITIONAL_NODE_ID_SIZE > 0
                        for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
                        {
                            TxBuffer[TxData++] = AdditionalNodeID[i];    
                        }
                    #endif
                    
                    for(i=0;i<MY_ADDRESS_LENGTH;i++)
                    {
                        tempLongAddress[i] = openSocketInfo.LongAddress1[i];
                    }
                    tempShortAddress.Val = openSocketInfo.ShortAddress1.Val;

                    openSocketInfo.socketHandle = SearchForShortAddress();
                
                    if(openSocketInfo.socketHandle != 0xFF)
                    {
                        tempNodeStatus.Val = ConnectionTable[openSocketInfo.socketHandle].status.Val;
                    }
                    else
                    {
                        //Family, RxOnWHenIdle, Neighbor/Network, P2P, ShortVal, LongVal, Direct, Valid
                        //tempNodeStatus.Val = 0x2D;
                        tempNodeStatus.Val = 0x8C;
                        if((tempShortAddress.v[0] & 0x80) == 0)
                        {
                            tempNodeStatus.bits.RXOnWhenIdle = 1;
                        }
                    }
                    tempPANID.Val = myPANID.Val;
                                                    
                    openSocketInfo.socketHandle = AddNodeToNetworkTable();
                    
                    if( openSocketInfo.socketHandle < 0xFF )
                    {
                        #if ADDITIONAL_NODE_ID_SIZE > 0
                            for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
                            {
                                ConnectionTable[openSocketInfo.socketHandle].PeerInfo[i] = openSocketInfo.AdditionalNodeID1[i];
                            }    
                        #endif
                    }    
                    
                    //RouteMessage(myPANID,openSocketInfo.ShortAddress1,false);
                    MiApp_UnicastAddress(openSocketInfo.ShortAddress1.v, false, false);
                    openSocketInfo.status.bits.requestIsOpen = 0;
                    openSocketInfo.status.bits.matchFound = 1;    
                    
                    if( openSocketInfo.socketHandle < 0xFF )
                    {
                        #if defined(ENABLE_NETWORK_FREEZER)
                            nvmPutConnectionTableIndex(&(ConnectionTable[openSocketInfo.socketHandle]), openSocketInfo.socketHandle);
                        #endif
                    }
                    
                }            
            }
            return;
        }
        else
        {
            //take a record of when you started to send the socket request
            //and send it to the PAN coordinator
            openSocketInfo.socketStart = MiWi_TickGet();
        }
        
        #else
            openSocketInfo.socketStart = MiWi_TickGet();
        #endif
           
        tempShortAddress.Val = 0x0000;

		MiApp_FlushTx();
        TxBuffer[TxData++] = MIWI_PRO_STACK_REPORT_TYPE;      //Report Type
        TxBuffer[TxData++] = OPEN_SOCKET_REQUEST;      //Report ID
        for(i = 0; i < MY_ADDRESS_LENGTH; i++)
        {
            TxBuffer[TxData++] = myLongAddress[i];      
        }
        #if ADDITIONAL_NODE_ID_SIZE > 0
            for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
            {
                TxBuffer[TxData++] = AdditionalNodeID[i];    
            }
        #endif
        
        MiApp_UnicastAddress(tempShortAddress.v, false, false);
        
        openSocketInfo.status.bits.requestIsOpen = 1;
    

    }

    #if defined(NWK_ROLE_COORDINATOR) && defined(ENABLE_INDIRECT_MESSAGE)
    /*********************************************************************
     * BOOL IndirectPacket(BOOL Broadcast, 
     *                     API_UINT16_UNION DestinationPANID,
     *                     uint8_t *DestinationAddress,
     *                     BOOL isCommand, 
     *                     BOOL SecurityEnabled)
     *
     * Overview:        This function store the indirect message for node
     *                  that turns off radio when idle     
     *
     * PreCondition:    None
     *
     * Input:           Broadcast           - Boolean to indicate if the indirect
     *                                        message a broadcast message
     *                  DestinationPANID    - The PAN Identifier of the 
     *                                        destination node
     *                  DestinationAddress  - The pointer to the destination
     *                                        long address
     *                  isCommand           - The boolean to indicate if the packet
     *                                        is command
     *                  SecurityEnabled     - The boolean to indicate if the 
     *                                        packet needs encryption
     *
     * Output:          boolean to indicate if operation successful
     *
     * Side Effects:    An indirect message stored and waiting to deliever
     *                  to sleeping device. An indirect message timer
     *                  has started to expire the indirect message in case
     *                  RFD does not acquire data in predefined interval
     *
     ********************************************************************/
    #if defined(IEEE_802_15_4)
        BOOL SaveIndirectMessage(INPUT BOOL Broadcast, 
                                 INPUT API_UINT16_UNION DestinationPANID,
                                 INPUT uint8_t *DestinationAddress,
                                 INPUT BOOL isAltAddress, 
                                 INPUT BOOL SecurityEnabled)
    #else
        BOOL SaveIndirectMessage(INPUT BOOL Broadcast, 
                                 INPUT uint8_t *DestinationAddress,
                                 INPUT BOOL isAltAddress, 
                                 INPUT BOOL SecurityEnabled)
    #endif    
    {
        uint8_t i;
        uint8_t j;
        
        for(i = 0; i < INDIRECT_MESSAGE_SIZE; i++)
        {
            if( indirectMessages[i].flags.bits.isValid == 0)
            {
                indirectMessages[i].flags.Val = 0;
                indirectMessages[i].flags.bits.isBroadcast = Broadcast;
                indirectMessages[i].flags.bits.isSecured = SecurityEnabled;
                indirectMessages[i].flags.bits.isValid = 1;
                indirectMessages[i].flags.bits.isAltAddr = isAltAddress;
                #if defined(IEEE_802_15_4)
                    if( isAltAddress == false )
                    {
                        if( Broadcast == false )
                        {
                            for(j = 0; j < MY_ADDRESS_LENGTH; j++)
                            {
                                indirectMessages[i].DestAddress[j] = DestinationAddress[j];
                            }
                        }
                    }
                    else
                    {
                        if( Broadcast == false )
                        {
                            indirectMessages[i].DestAddress[0] = DestinationAddress[0];
                            indirectMessages[i].DestAddress[1] = DestinationAddress[1];
                        }
                    }
                    indirectMessages[i].DestPANID.Val = DestinationPANID.Val;
                #else
                    if( Broadcast == false )
                    {
                        if( isAltAddress )
                        {
                            indirectMessages[i].DestAddress[0] = DestinationAddress[0];
                            indirectMessages[i].DestAddress[1] = DestinationAddress[1]; 
                        }
                        else
                        {
                            for(j = 0; j < MY_ADDRESS_LENGTH; j++)
                            {
                                indirectMessages[i].DestAddress[j] = DestinationAddress[j];
                            }
                        }     
                    }
                #endif
                
                #if defined(ENABLE_SECURITY) && defined(ENABLE_NETWORK_FREEZER) && !defined(TARGET_SMALL)
                    if( SecurityEnabled )
                    {  
                        if( isAltAddress )
                        {
                            tempShortAddress.v[0] = DestinationAddress[0];
                            tempShortAddress.v[1] = DestinationAddress[1];
                            j = SearchForShortAddress();
                        }
                        else
                        {
                            for(j = 0; j < MY_ADDRESS_LENGTH; j++)
                            {
                                tempLongAddress[j] = DestinationAddress[j];
                            }
                            j = SearchForLongAddress();
                        }
                    }
                #endif
                indirectMessages[i].PayLoadSize = TxData;
                for(j = 0; j < TxData; j++)
                {
                    indirectMessages[i].PayLoad[j] = TxBuffer[j];
                }
                indirectMessages[i].TickStart = MiWi_TickGet();
                return true;
            }
        }
        
        return false;
    }
    #endif
    

    
    /*********************************************************************
     * Function:        uint8_t SearchForShortAddress(void)
     *
     * PreCondition:    tempShortAddress and tempPANID are set to the device
     *                  that you are looking for
     *
     * Input:           uint8_t entryType - this is set to NEIGHBOR if you
     *                  want to find a Node on the network.  This value
     *                  is set to NETWORK if you are looking for a specific network
     *                  and not a node on the current operating network
     *
     * Output:          uint8_t - the index of the network table entry of the
     *                  requested device.  0xFF indicates that the requested
     *                  device doesn't exist in the network table
     *
     * Side Effects:    None
     *
     * Overview:        This function looks up the index of a node or network
     *                  in the network table by short address.
     ********************************************************************/
    uint8_t SearchForShortAddress(void)
    {
        uint8_t i;
        
        for(i=0;i<CONNECTION_SIZE;i++)
        {
            if(ConnectionTable[i].status.bits.isValid && ConnectionTable[i].status.bits.shortAddressValid)
            {
                if(ConnectionTable[i].AltAddress.Val == tempShortAddress.Val)
                {
                    return i;
                }
            }
        }
        return 0xFF;
    }

    #if defined(NWK_ROLE_COORDINATOR)
        /*********************************************************************
         * Function:        void SendBeacon(void)
         *
         * PreCondition:    Coordinator has joined the network
         *
         * Input:           None
         *
         * Output:          None
         *
         * Side Effects:    None
         *
         * Overview:        This function sends a beacon frame.
         ********************************************************************/
        void SendBeacon(void)
        {
            uint8_t i;
            
            MAC_FlushTx();
            #if !defined(IEEE_802_15_4)
                MiApp_WriteData(myPANID.v[0]);
                MiApp_WriteData(myPANID.v[1]);
                MiApp_WriteData(myShortAddress.v[0]);
                MiApp_WriteData(myShortAddress.v[1]);
            #endif
            MiApp_WriteData(0xFF);    //superframe specification (BO = 0xF, SO = 0xF)
    		MiApp_WriteData(MiWiPROCapacityInfo.Val);
            MiApp_WriteData(0x00);    // GTS
            MiApp_WriteData(0x00);    // Pending addresses
            MiApp_WriteData(MIWI_PRO_PROTOCOL_ID);    // Protocol ID
            MiApp_WriteData(MIWI_PRO_VERSION_NUM);    // Version Number
            #if ADDITIONAL_NODE_ID_SIZE > 0 
                for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
                {
                    MiApp_WriteData(AdditionalNodeID[i]);
                }
            #endif
           
            #if defined(IEEE_802_15_4)
                SendMACPacket(myPANID.v, NULL, PACKET_TYPE_RESERVE, MSK_ALT_SRC_ADDR);
            #else
                SendMACPacket(NULL, PACKET_TYPE_RESERVE);
            #endif   
        }    
    #endif
    
    /*********************************************************************
     * Function:        uint8_t SearchForLongAddress(void)
     *
     * PreCondition:    tempLongAddress is set to the device
     *                  that you are looking for
     *
     * Input:           None
     *
     * Output:          uint8_t - the index of the network table entry of the
     *                  requested device.  0xFF indicates that the requested
     *                  device doesn't exist in the network table
     *
     * Side Effects:    None
     *
     * Overview:        This function looks up the index of a node or network
     *                  in the network table by long address.
     ********************************************************************/
    uint8_t SearchForLongAddress(void)
    {
        uint8_t i,j;
        
        for(i=0;i<CONNECTION_SIZE;i++)
        {
            if(ConnectionTable[i].status.bits.isValid && ConnectionTable[i].status.bits.longAddressValid)
            {
                for(j=0;j<MY_ADDRESS_LENGTH;j++)
                {
                    if(ConnectionTable[i].Address[j] != tempLongAddress[j])
                    {
                        goto EndOfSearchLoop;
                    }
                }
                return i;
            }
EndOfSearchLoop:
            ;
        }
        return 0xFF;
    }

    
    
    /*********************************************************************
     * Function:        uint8_t AddNodeToNetworkTable(void)
     *
     * PreCondition:    tempLongAddress, tempShortAddress, tempPANID, and 
     *                  tempNodeStatus are set to the correct values.  If 
     *                  tempNodeStatus.bits.longAddressValid is set then
     *                  tempLongAddress needs to be set.  If 
     *                  tempNodeStatus.bits.shortAddressValid is set then
     *                  tempShortAddress and tempPANID need to be set.
     *
     * Input:           None
     *
     * Output:          uint8_t - the index of the network table entry
     *                  where the device was inserted.  0xFF indicates that
     *                  the requested device couldn't be inserted into the table
     *
     * Side Effects:    Network table is updated with the devices info
     *
     * Overview:        This function is used to insert new device into the 
     *                  network table (or update already existing entries)
     ********************************************************************/
    uint8_t AddNodeToNetworkTable(void)
    {
        uint8_t handle = 0xFF;
        
        if(tempNodeStatus.bits.longAddressValid)
        {
            handle = SearchForLongAddress();
        }
        else
        {
            if(tempNodeStatus.bits.shortAddressValid)
            {
                handle = SearchForShortAddress();
            }
        }
                                       
        if(handle==0xFF)
        {
            handle = findNextNetworkEntry();
        }
        
        if(handle != 0xFF)
        { 
            //isFamily = 1, RxOnWhenIdle = 1, Is a neighbor and not a network,
            //not a P2P connection, short and long addresses valid as well 
            //as the entire entry
            
            //If it is not family then update the node information
            ConnectionTable[handle].status.Val = tempNodeStatus.Val;
            if(tempNodeStatus.bits.longAddressValid)
            {
                uint8_t i;
                for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                {
                    ConnectionTable[handle].Address[i] = tempLongAddress[i];
                }
            }
                   
            if(tempNodeStatus.bits.shortAddressValid)
            {        
                ConnectionTable[handle].AltAddress.Val = tempShortAddress.Val;
            }
            
            ConnectionTable[handle].PANID.Val = tempPANID.Val;
            #if defined(ENABLE_SECURITY)
                IncomingFrameCounter[handle].Val = 0;
            #endif
        }

        return handle;
    }

    /*********************************************************************
     * BOOL    isSameAddress(uint8_t *Address1, uint8_t *Address2)
     *
     * Overview:        This function compares two long addresses and returns
     *                  the boolean to indicate if they are the same
     *
     * PreCondition:    
     *
     * Input:  
     *          Address1    - Pointer to the first long address to be compared
     *          Address2    - Pointer to the second long address to be compared
     *                  
     * Output: 
     *          If the two address are the same
     *
     * Side Effects:    
     *
     ********************************************************************/
    BOOL    isSameAddress(INPUT uint8_t *Address1, INPUT uint8_t *Address2)
    {
        uint8_t i;
        
        for(i = 0; i < MY_ADDRESS_LENGTH; i++)
        {
            if( Address1[i] != Address2[i] )
            {
                return false;
            }
        }
        return true;
    }
    

    #if defined(ENABLE_SLEEP)
     /*********************************************************************
     * BOOL CheckForData(void)
     *
     * Overview:        This function sends out a Data Request to the peer
     *                  device of the first P2P connection. 
     *
     * PreCondition:    Transceiver is initialized and fully waken up
     *
     * Input:           None
     *
     * Output:          None
     *
     * Side Effects:    The P2P stack is waiting for the response from
     *                  the peer device. A data request timer has been
     *                  started. In case there is no response from the
     *                  peer device, the data request will time-out itself
     *
     ********************************************************************/
     BOOL CheckForData(void)
     {
        uint8_t tmpTxData = TxData;

        MAC_FlushTx();
        MiApp_WriteData(MAC_COMMAND_DATA_REQUEST);
        
        #if defined(ENABLE_ENHANCED_DATA_REQUEST)
            if( tmpTxData > PAYLOAD_START )
            {
                TxData = tmpTxData;
                
                MTP.flags.Val = 0;
                MTP.flags.bits.packetType = PACKET_TYPE_COMMAND;
                MTP.flags.bits.ackReq = 1;
                MTP.flags.bits.sourcePrsnt = 1;
                if( (TxBuffer[1] & 0x01) > 0 )
                {
                    MTP.flags.bits.secEn = 1;
                    TxBuffer[1] &= 0xFE;
                }
                    
                #if defined(IEEE_802_15_4)
                    MTP.altDestAddr = true;
                    MTP.altSrcAddr = true;

                    MTP.DestPANID.Val = ConnectionTable[myParent].PANID.Val;
                    MTP.DestAddress = ConnectionTable[myParent].AltAddress.v;
                #else
                
                    {
                        uint8_t i;
                        for(i = tmpTxData; i > 1; i--)
                        {
                            TxBuffer[i+1] = TxBuffer[i-1];
                        }  
                        TxData += 2;  
                    }    
                    TxBuffer[1] = myShortAddress.v[0];
                    TxBuffer[2] = myShortAddress.v[1];
                    MTP.DestAddress = ConnectionTable[myParent].Address;
                #endif
                if( MiMAC_SendPacket(MTP, TxBuffer, TxData) )
                {
                    MiWiPROStateMachine.bits.DataRequesting = 1;
                    DataRequestTimer = MiWi_TickGet();
                    return true;
                }
                return false;
            }    
        #endif
        
        #if defined(IEEE_802_15_4)
            if( SendMACPacket(ConnectionTable[0].PANID.v, ConnectionTable[0].AltAddress.v, PACKET_TYPE_COMMAND, MSK_ALT_DST_ADDR|MSK_ALT_SRC_ADDR) )
        #else
            MiApp_WriteData(myShortAddress.v[0]);
            MiApp_WriteData(myShortAddress.v[1]);
            
            if( SendMACPacket(ConnectionTable[0].Address, PACKET_TYPE_COMMAND) )
        #endif           
        {
            MiWiPROStateMachine.bits.DataRequesting = 1;
            DataRequestTimer = MiWi_TickGet();
            TxData = tmpTxData;
            #if defined(ENABLE_TIME_SYNC)
                #if defined(__18CXX)
                    TMR3H = 0;
                    TMR3L = 0;
                #elif defined(__dsPIC33F__) || defined(__PIC24F__) || defined(__PIC24FK__) || defined(__PIC24H__)
                    PR1 = 0xFFFF;
                    TMR1 = 0;
                #elif defined(__PIC32MX__)
                    PR1 = 0xFFFF;
                    while(T1CONbits.TWIP) ;
                    TMR1 = 0;
                #endif
            #endif
            return true;
        }
        TxData = tmpTxData;
        return false;
    }
    #endif


    
    
    BOOL MiApp_ProtocolInit(BOOL bNetworkFreezer)
    {
        MACINIT_PARAM MIP;
        uint8_t i;

        #if defined(ENABLE_NVM_MAC)
            if( MY_ADDRESS_LENGTH > 6 )
            {
                for(i = 0; i < 3; i++)
                {
                    EEPROM_Read(&(myLongAddress[MY_ADDRESS_LENGTH-1-i]), EEPROM_MAC_ADDR+i, 1);
                }
                myLongAddress[4] = 0xFF;
                if( MY_ADDRESS_LENGTH > 7 )
                {
                    myLongAddress[3] = 0xFE;
                }
                for(i = 0; i < 3; i++)
                {
                    EEPROM_Read(&(myLongAddress[2-i]), EEPROM_MAC_ADDR+3+i, 1);
                }
            }
            else
            {
                for(i = 0; i < MY_ADDRESS_LENGTH; i++)
                {
                    EEPROM_Read(&(myLongAddress[MY_ADDRESS_LENGTH-1-i]), EEPROM_MAC_ADDR+i, 1);
                }   
            }
        
        #endif
        
        #if defined(ENABLE_NETWORK_FREEZER)
            NVMInit();
        #endif

        myShortAddress.Val = 0xFFFF;
        myPANID.Val = MY_PAN_ID;
    
        //clear the network table
        for(i = 0; i < CONNECTION_SIZE; i++)
        {
            ConnectionTable[i].status.Val = 0;
        }
        
        #ifdef NWK_ROLE_COORDINATOR
            for(i=0;i< NUM_COORDINATOR/8;i++)
            {
                RoutingTable[i] = 0;
            }
            for(i=0; i < NUM_COORDINATOR; i++)
            {
                RouterFailures[i] = 0;
            }
            role = ROLE_FFD_END_DEVICE;
            MiWiPROStatus.Val = 0;
        #endif
        MiWiPROStateMachine.Val = 0;
        #if defined(MiWiPRONVMStatus)
            MiWiPRONVMStatus.Val = 0;
        #endif
        
        openSocketInfo.status.Val = 0;
        MiWiPROCapacityInfo.Val = 0;
        
        InitSymbolTimer();
        
        TxData = 0;
        #ifdef ENABLE_INDIRECT_MESSAGE
            for(i = 0; i < INDIRECT_MESSAGE_SIZE; i++)
            {
                indirectMessages[i].flags.Val = 0;
            }
        #endif 
        
        #if defined(ENABLE_SLEEP) && defined(ENABLE_BROADCAST_TO_SLEEP_DEVICE)
            for(i = 0; i < PACKET_RECORD_SIZE; i++)
            {
                PacketRecords[i].RxCounter = 0;
            }
        #endif
        
        #if defined(ENABLE_SECURITY)
            for(i = 0; i < CONNECTION_SIZE; i++)
            {
                IncomingFrameCounter[i].Val = 0;
            }
        #endif
                
        #if defined(ENABLE_NETWORK_FREEZER)

            if( bNetworkFreezer )
            {
                nvmGetCurrentChannel(&currentChannel);
                if( currentChannel >= 32 )
                {
                    return false;
                }
                
                nvmGetMyShortAddress(myShortAddress.v);
                if( myShortAddress.Val == 0xFFFF )
                {
                    return false;
                }
                
                nvmGetMyPANID(myPANID.v);
                nvmGetConnMode(&ConnMode);
                MiWiPROCapacityInfo.bits.ConnMode = ConnMode;
                nvmGetConnectionTable(ConnectionTable);
                nvmGetMyShortAddress(myShortAddress.v);
                nvmGetMyParent(&myParent);
                #if defined(NWK_ROLE_COORDINATOR)
                    nvmGetRoutingTable(RoutingTable);
                    nvmGetNeighborRoutingTable(NeighborRoutingTable);
                    nvmGetFamilyTree(FamilyTree);
                    nvmGetRole(&role);
                #endif
                
                Printf("Channel:");
                PrintDec(currentChannel);
                Printf(" PANID:");
                PrintChar(myPANID.v[1]);
                PrintChar(myPANID.v[0]);
                Printf("ShortAddr:");
                PrintChar(myShortAddress.v[1]);
                PrintChar(myShortAddress.v[0]);
            
                MiApp_SetChannel(currentChannel);
                MiMAC_SetAltAddress(myShortAddress.v, myPANID.v);
                MiWiPROStateMachine.bits.memberOfNetwork = 1;
            }
            else
            {
                nvmPutMyPANID(myPANID.v);
                nvmPutCurrentChannel(&currentChannel);
                nvmPutConnMode(&ConnMode);
                nvmPutConnectionTable(ConnectionTable);
                
                nvmPutMyShortAddress(myShortAddress.v);
                nvmPutMyParent(&myParent);
                #if defined(NWK_ROLE_COORDINATOR)
                    for(i = 0; i < NUM_COORDINATOR/8; i++)
                    {
                        RoutingTable[i] = 0;
                    }    
                    for(i = 0; i < NUM_COORDINATOR; i++)
                    {
                        uint8_t j;
                        for(j = 0; j < NUM_COORDINATOR/8; j++)
                        {
                            NeighborRoutingTable[i][j] = 0;
                        }    
                        FamilyTree[i] = 0xFF;
                    }    
                #endif
            }
        #endif
        

        MIP.actionFlags.Val = 0;
        MIP.actionFlags.bits.PAddrLength = MY_ADDRESS_LENGTH;
        MIP.actionFlags.bits.CCAEnable = 1;
        MIP.actionFlags.bits.NetworkFreezer = bNetworkFreezer;
        MIP.PAddress = myLongAddress;
        MiMAC_Init(MIP);

        MiMAC_SetAltAddress(myShortAddress.v, myPANID.v);
        MiApp_SetChannel(currentChannel);
        
        #if !defined(ENABLE_SLEEP)
            MiWiPROCapacityInfo.bits.Sleep = 1;
        #endif
        #if defined(ENABLE_SECURITY)
            MiWiPROCapacityInfo.bits.Security = 1;
        #endif
        MiWiPROCapacityInfo.bits.ConnMode = ConnMode;
        #ifdef NWK_ROLE_COORDINATOR
            MiWiPROCapacityInfo.bits.CoordCap = 1;
            MiWiPROCapacityInfo.bits.Role = role;
            MiWiPRONVMStatus.Val = 0;
            backgroundTaskTick = MiWi_TickGet();
            #if defined(ENABLE_ROUTING_UPDATE)
                RoutingUpdateTick.Val = backgroundTaskTick.Val;
                for(i = 0; i < NUM_COORDINATOR; i++)
                {
                    RoutingUpdateCounter[i] = 0;
                }
            #endif
        #endif
        
        #if defined(ENABLE_TIME_SYNC)
            #if defined(ENABLE_SLEEP)
                WakeupTimes.Val = 0;
                CounterValue.Val = 61535;   // (0xFFFF - 4000) one second
            #elif defined(ENABLE_INDIRECT_MESSAGE)
                TimeSlotTick.Val = ((ONE_SECOND) * RFD_WAKEUP_INTERVAL) / TIME_SYNC_SLOTS;
            #endif
        #endif
        
        #if defined(ENABLE_FREQUENCY_AGILITY) && defined(NWK_ROLE_COORDINATOR)
            FrequencyAgilityInfo.status.Val = 0;
        #endif
        
        MiWiPROSeqNum = TMRL;

        RFIF = 0;
        RFIE = 1;
        return true;
    
    }
    
    #if defined(ENABLE_SLEEP)
    /************************************************************************************
     * Function:
     *      uint8_t    MiApp_TransceiverPowerState(uint8_t Mode)
     *
     * Summary:
     *      This function put the RF transceiver into different power state. i.e. Put the 
     *      RF transceiver into sleep or wake it up.
     *
     * Description:        
     *      This is the primary user interface functions for the application layer to 
     *      put RF transceiver into sleep or wake it up. This function is only available
     *      to those wireless nodes that may have to disable the transceiver to save 
     *      battery power.
     *
     * PreCondition:    
     *      Protocol initialization has been done. 
     *
     * Parameters: 
     *      uint8_t Mode - The mode of power state for the RF transceiver to be set. The possible
     *                  power states are following
     *                  * POWER_STATE_SLEEP     The deep sleep mode for RF transceiver
     *                  * POWER_STATE_WAKEUP    Wake up state, or operating state for RF transceiver
     *                  * POWER_STATE_WAKEUP_DR Put device into wakeup mode and then transmit a 
     *                                          data request to the device's associated device
     *
     * Returns: 
     *      The status of the operation. The following are the possible status
     *      * SUCCESS           Operation successful
     *      * ERR_TRX_FAIL      Transceiver fails to go to sleep or wake up
     *      * ERR_TX_FAIL       Transmission of Data Request command failed. Only available if the
     *                          input mode is POWER_STATE_WAKEUP_DR.
     *      * ERR_RX_FAIL       Failed to receive any response to Data Request command. Only available
     *                          if input mode is POWER_STATE_WAKEUP_DR.
     *      * ERR_INVLAID_INPUT Invalid input mode. 
     *
     * Example:
     *      <code>
     *      // put RF transceiver into sleep
     *      MiApp_TransceiverPowerState(POWER_STATE_SLEEP;
     *
     *      // Put the MCU into sleep
     *      Sleep();    
     *
     *      // wakes up the MCU by WDT, external interrupt or any other means
     *      
     *      // make sure that RF transceiver to wake up and send out Data Request
     *      MiApp_TransceiverPowerState(POWER_STATE_WAKEUP_DR);
     *      </code>
     *
     * Remarks:    
     *      None
     *
     *****************************************************************************************/
    uint8_t MiApp_TransceiverPowerState(uint8_t Mode)
    {
        
        switch(Mode)
        {
            case POWER_STATE_SLEEP:
                {
                    #if defined(ENABLE_NETWORK_FREEZER) && defined(NWK_ROLE_COORDINATOR)
                        if( MiWiPRONVMStatus.bits.saveConnectionTable )
                        {
                            nvmPutConnectionTable(ConnectionTable);
                            MiWiPRONVMStatus.bits.saveConnectionTable = 0;
                        }
                    #endif
                    if( MiMAC_PowerState(POWER_STATE_DEEP_SLEEP) )
                    {
                        MiWiPROStateMachine.bits.Sleeping = 1;
                        return SUCCESS;
                    }
                    return ERR_TRX_FAIL;
                }
                
            case POWER_STATE_WAKEUP:
                {
                    if( MiMAC_PowerState(POWER_STATE_OPERATE) )
                    {
                        MiWiPROStateMachine.bits.Sleeping = 0;
                        return SUCCESS;
                    }
                    return ERR_TRX_FAIL;
                }
               
            case POWER_STATE_WAKEUP_DR:
                {
                    if( false == MiMAC_PowerState(POWER_STATE_OPERATE) )
                    {
                        return ERR_TRX_FAIL;
                    } 
                    
                    MiWiPROStateMachine.bits.Sleeping = 0;
                    if( CheckForData() == false )
                    {
                        return ERR_TX_FAIL;
                    }
                    while( MiWiPROStateMachine.bits.DataRequesting ) 
                    {
                        MiWiPROTasks();
                    }
                    return SUCCESS;
                }
                
             default:
                break;
        }
        
        return ERR_INVALID_INPUT;    
    }    
    #endif
    
    
BOOL MiApp_SetChannel(uint8_t channel)
{
    if( MiMAC_SetChannel(channel, 0) )
    {
        currentChannel = channel;
        #if defined(ENABLE_NETWORK_FREEZER)
            nvmPutCurrentChannel(&currentChannel);
        #endif
        return true;
    }
    return false;
}    
    
    
/************************************************************************************
 * Function:
 *      void    MiApp_ConnectionMode(uint8_t Mode)
 *
 * Summary:
 *      This function set the current connection mode.
 *
 * Description:        
 *      This is the primary user interface function for the application layer to 
 *      configure the way that the host device accept connection request.
 *
 * PreCondition:    
 *      Protocol initialization has been done. 
 *
 * Parameters:           
 *      uint8_t Mode -     The mode to accept connection request. The privilege for those modes
 *                      decreases gradually as defined. The higher privilege mode has all the 
 *                      rights of the lower privilege modes.
 *                      The possible modes are
 *                      * ENABLE_ALL_CONN       Enable response to all connection request
 *                      * ENABLE_PREV_CONN      Enable response to connection request
 *                                              from device already in the connection
 *                                              table.
 *                      * ENABLE_ACTIVE_SCAN_RSP    Enable response to active scan only
 *                      * DISABLE_ALL_CONN      Disable response to connection request, including
 *                                              an acitve scan request.
 *
 * Returns: 
 *      None
 *
 * Example:
 *      <code>
 *      // Enable all connection request
 *      MiApp_ConnectionMode(ENABLE_ALL_CONN);
 *      </code>
 *
 * Remarks:    
 *      None
 *
 *****************************************************************************************/ 
void MiApp_ConnectionMode(INPUT uint8_t Mode)
{
    if( Mode > 3 )
    {
        return;
    }
    ConnMode = Mode;
    MiWiPROCapacityInfo.bits.ConnMode = ConnMode;
    
    #if defined(ENABLE_NETWORK_FREEZER)
        nvmPutConnMode(&ConnMode);
    #endif
}    
    


/************************************************************************************
 * Function:
 *      uint8_t    MiApp_SearchConnection(uint8_t ScanDuartion, uint32_t ChannelMap)
 *
 * Summary:
 *      This function perform an active scan to locate operating PANs in the
 *      neighborhood.
 *
 * Description:        
 *      This is the primary user interface function for the application layer to 
 *      perform an active scan. After this function call, all active scan response
 *      will be stored in the global variable ActiveScanResults in the format of 
 *      structure ACTIVE_SCAN_RESULT. The return value indicates the total number
 *      of valid active scan response in the active scan result array.
 *
 * PreCondition:    
 *      Protocol initialization has been done.
 *
 * Parameters:           
 *      uint8_t ScanDuration - The maximum time to perform scan on single channel. The
 *                          value is from 5 to 14. The real time to perform scan can
 *                          be calculated in following formula from IEEE 802.15.4 
 *                          specification 
 *                              960 * (2^ScanDuration + 1) * 10^(-6) second
 *      uint32_t ChannelMap -  The bit map of channels to perform noise scan. The 32-bit
 *                          double word parameter use one bit to represent corresponding
 *                          channels from 0 to 31. For instance, 0x00000003 represent to 
 *                          scan channel 0 and channel 1. 
 *                  
 * Returns: 
 *      The number of valid active scan response stored in the global variable ActiveScanResults.
 *
 * Example:
 *      <code>
 *      // Perform an active scan on all possible channels
 *      NumOfActiveScanResponse = MiApp_SearchConnection(10, 0xFFFFFFFF);
 *      </code>
 *
 * Remarks:    
 *      None
 *
 *****************************************************************************************/
uint8_t MiApp_SearchConnection(INPUT uint8_t ScanDuration, INPUT uint32_t ChannelMap)
{
    uint8_t i;
    uint32_t channelMask = 0x00000001;
    uint8_t backupChannel = currentChannel;
    MIWI_TICK t1, t2;
        
    for(i = 0; i < ACTIVE_SCAN_RESULT_SIZE; i++)
    {
        ActiveScanResults[i].Channel = 0xFF;
    }
    ActiveScanResultIndex = 0;
    MiWiPROStateMachine.bits.searchingForNetwork = 1;
    
    i = 0;
    while(i < 32 )
    {
        if( ChannelMap & FULL_CHANNEL_MAP & (channelMask << i) )
        {
            Printf("\r\nScan Channel ");
            PrintDec(i);
            /* choose appropriate channel */
            MiApp_SetChannel( i );
 
            MAC_FlushTx();
            MiApp_WriteData(MAC_COMMAND_BEACON_REQUEST);
            MiApp_WriteData(currentChannel);
            #if defined(IEEE_802_15_4)
                tempPANID.Val = 0xFFFF;
                SendMACPacket(tempPANID.v, NULL, PACKET_TYPE_COMMAND, 0);
            #else
                SendMACPacket(NULL, PACKET_TYPE_COMMAND);
            #endif
            
            t1 = MiWi_TickGet();
            while(1)
            {
                if( MiApp_MessageAvailable())
                {
                    MiApp_DiscardMessage();
                }                
                //MiWiPROTasks();
                t2 = MiWi_TickGet();
                if( MiWi_TickGetDiff(t2, t1) > ((uint32_t)(ScanTime[ScanDuration])) )
                {
                    // if scan time exceed scan duration, prepare to scan the next channel
                    break;
                }
            } 
        
        }  
        i++;
    }
    
    MiApp_SetChannel(backupChannel);
    MiWiPROStateMachine.bits.searchingForNetwork = 0;

    return ActiveScanResultIndex;
}







    
/************************************************************************************
 * Function:
 *      uint8_t    MiApp_EstablishConnection(uint8_t ActiveScanIndex, uint8_t Mode)
 *
 * Summary:
 *      This function establish a connection with one or more nodes in an existing
 *      PAN.
 *
 * Description:        
 *      This is the primary user interface function for the application layer to 
 *      start communication with an existing PAN. For P2P protocol, this function
 *      call can establish one or more connections. For network protocol, this 
 *      function can be used to join the network, or establish a virtual socket
 *      connection with a node out of the radio range. There are multiple ways to
 *      establish connection(s), all depends on the input parameters.
 *
 * PreCondition:    
 *      Protocol initialization has been done. If only to establish connection with
 *      a predefined device, an active scan must be performed before and valid active
 *      scan result has been saved.
 *
 * Parameters:           
 *      uint8_t ActiveScanIndex -  The index of the target device in the ActiveScanResults
 *                              array, if a predefined device is targeted. If the 
 *                              value of ActiveScanIndex is 0xFF, the protocol stack will
 *                              try to establish a connection with any device.
 *      uint8_t Mode - The mode to establish a connection. This parameter is generally valid in
 *                  a network protocol. The possible modes are:
 *                  * CONN_MODE_DIRECT      Establish a connection without radio range.
 *                  * CONN_MODE_INDIRECT    Establish a virtual connection with a device 
 *                                          that may be in or out of the radio range. This
 *                                          mode sometimes is called cluster socket, which
 *                                          is only valid for network protocol. The PAN 
 *                                          Coordinator will be involved to establish a 
 *                                          virtual indirect socket connection.
 *                  
 * Returns: 
 *      The index of the peer device on the connection table.
 *
 * Example:
 *      <code>
 *      // Establish one or more connections with any device
 *      PeerIndex = MiApp_EstablishConnection(0xFF, CONN_MODE_DIRECT);
 *      </code>
 *
 * Remarks:    
 *      If more than one connections have been established through this function call, the
 *      return value points to the index of one of the peer devices.
 *
 *****************************************************************************************/  
uint8_t    MiApp_EstablishConnection(INPUT uint8_t ActiveScanIndex, INPUT uint8_t Mode)
{
    uint8_t retry = CONNECTION_RETRY_TIMES;
    uint8_t i;
    MIWI_TICK t1, t2;
    
    if( Mode == CONN_MODE_INDIRECT )
    {
        #if defined(ENABLE_SLEEP)
            t1 = MiWi_TickGet();;
        #endif
        OpenSocket();
        while(openSocketInfo.status.bits.requestIsOpen)
        {
            if( MiApp_MessageAvailable())
            {
                MiApp_DiscardMessage();
            }            
            //MiWiPROTasks();
            #if defined(ENABLE_SLEEP) && defined(NWK_ROLE_END_DEVICE)
                t2 = MiWi_TickGet();
                if( MiWi_TickGetDiff(t2, t1) > OPEN_SOCKET_POLL_INTERVAL )
                {
                    CheckForData();
                    t1.Val = t2.Val;
                }
            #endif
        }
        if( openSocketInfo.status.bits.matchFound )
        {
            return openSocketInfo.socketHandle;
        }
        return 0xFF;
    }
    else if( Mode == CONN_MODE_DIRECT)
    {
        if( ActiveScanIndex == 0xFF )
        {
            while( MiApp_SearchConnection(10, ((uint32_t)0x00000001)<<currentChannel) == 0 )
            {
                if( --retry == 0 )
                {
                    return 0xFF;
                }
            }
            ActiveScanIndex = 0;
        }
        
        #if defined(IEEE_802_15_4)
            tempPANID.Val = ActiveScanResults[ActiveScanIndex].PANID.Val;
            tempShortAddress.v[0] = ActiveScanResults[ActiveScanIndex].Address[0];
            tempShortAddress.v[1] = ActiveScanResults[ActiveScanIndex].Address[1];
            myParent = SearchForShortAddress();
        #else
            tempPANID.Val = ActiveScanResults[ActiveScanIndex].PANID.Val;
            for(i = 0; i < MY_ADDRESS_LENGTH; i++)
            {
                tempLongAddress[i] = ActiveScanResults[ActiveScanIndex].Address[i];
            }    
            myParent = SearchForLongAddress();
        #endif
        
        if( myParent == 0xFF )
        {
            if( (myParent = findNextNetworkEntry()) == 0xFF )
            {
                return 0xFF;
            }
        }        
        
        ConnectionTable[myParent].status.Val = 0;
        ConnectionTable[myParent].PANID.Val = ActiveScanResults[ActiveScanIndex].PANID.Val;
        #if defined(IEEE_802_15_4)
            ConnectionTable[myParent].AltAddress.v[0] = ActiveScanResults[ActiveScanIndex].Address[0];
            ConnectionTable[myParent].AltAddress.v[1] = ActiveScanResults[ActiveScanIndex].Address[1];
            ConnectionTable[myParent].status.bits.shortAddressValid = 1;
        #else
            for(i = 0; i < MY_ADDRESS_LENGTH; i++)
            {
                ConnectionTable[myParent].Address[i] = ActiveScanResults[ActiveScanIndex].Address[i];
            }
            ConnectionTable[myParent].status.bits.longAddressValid = 1;
        #endif
        #if ADDITIONAL_NODE_ID_SIZE > 0
            for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
            {
                ConnectionTable[myParent].PeerInfo[i] = ActiveScanResults[ActiveScanIndex].PeerInfo[i];
            }
        #endif
        
        ConnectionTable[myParent].status.bits.directConnection = 1;
        ConnectionTable[myParent].status.bits.isFamily = 1;
        ConnectionTable[myParent].status.bits.RXOnWhenIdle = 1;
        
        MiApp_SetChannel(ActiveScanResults[ActiveScanIndex].Channel);
        
        /* Program the PANID to the attempted network */
        myPANID.Val = ConnectionTable[myParent].PANID.Val;
        tempShortAddress.Val = 0xFFFF;
        MiMAC_SetAltAddress(tempShortAddress.v, myPANID.v);
        
    
        MAC_FlushTx();
        MiApp_WriteData(MAC_COMMAND_ASSOCIATION_REQUEST);   
        MiApp_WriteData(MiWiPROCapacityInfo.Val);
        #if ADDITIONAL_NODE_ID_SIZE > 0
            for(i = 0; i < ADDITIONAL_NODE_ID_SIZE; i++)
            {
                MiApp_WriteData(AdditionalNodeID[i]);
            }
        #endif
        
        #if defined(IEEE_802_15_4)
            SendMACPacket(ConnectionTable[myParent].PANID.v, ConnectionTable[myParent].AltAddress.v, PACKET_TYPE_COMMAND, MSK_ALT_DST_ADDR);
        #else
            SendMACPacket(ConnectionTable[myParent].Address, PACKET_TYPE_COMMAND);
        #endif
        
        t1 = MiWi_TickGet();
        while(ConnectionTable[myParent].status.bits.FinishJoin == 0 )
        {
            if( MiApp_MessageAvailable())
            {
                MiApp_DiscardMessage();
            }            
            //MiWiPROTasks();
            t2 = MiWi_TickGet();
            if( MiWi_TickGetDiff(t2, t1) > ONE_SECOND )
            {
                return 0xFF;
            }
        }
        
        #if defined(ENABLE_TIME_SYNC) && !defined(ENABLE_SLEEP) && defined(ENABLE_INDIRECT_MESSAGE)
            TimeSyncTick = MiWi_TickGet();
        #endif
        return myParent;
    }
    return 0xFF;
}

BOOL MiApp_MessageAvailable(void)
{
    MiWiPROTasks();

    return MiWiPROStateMachine.bits.RxHasUserData;
}
    
void MiApp_DiscardMessage(void)
{
    MiWiPROStateMachine.bits.RxHasUserData = 0;
    MiMAC_DiscardPacket();    
}    
    


/************************************************************************************
 * Function:
 *      BOOL MiApp_BroadcastPacket(BOOL SecEn )
 *
 * Summary:
 *      This function broadcast a message in the TxBuffer.
 *
 * Description:        
 *      This is the primary user interface function for the application layer to 
 *      broadcast a message. The application payload is filled in the global char
 *      array TxBuffer.
 *
 * PreCondition:    
 *      Protocol initialization has been done. 
 *
 * Parameters:           
 *      BOOL SecEn -    The boolean indicates if the application payload needs to be
 *                      secured before transmission.
 *
 * Returns: 
 *      A boolean to indicates if the broadcast procedure is succcessful.
 *
 * Example:
 *      <code>
 *      // Secure and then broadcast the message stored in TxBuffer
 *      MiApp_BroadcastPacket(true);
 *      </code>
 *
 * Remarks:    
 *      None
 *
 *****************************************************************************************/ 
BOOL MiApp_BroadcastPacket( INPUT BOOL SecEn )   
{
    return MultiCast(BROADCAST_TO_ALL, SecEn);
}


BOOL MultiCast( INPUT uint8_t MultiCastMode, INPUT BOOL SecEn )
{
    TxBuffer[0] = defaultHops;
    TxBuffer[1] = 0x02;
    TxBuffer[2] = myPANID.v[0];
    TxBuffer[3] = myPANID.v[1];
    switch( MultiCastMode )
    {
        case BROADCAST_TO_ALL:
            TxBuffer[4] = 0xFF;
            break;
        
        case MULTICAST_TO_FFDS:
            TxBuffer[4] = 0xFE;
            break;
            
        case MULTICAST_TO_COORDINATORS:
            TxBuffer[4] = 0xFD;
            break;
            
        default:
            return false;
    }    
    TxBuffer[5] = 0xFF;
    TxBuffer[6] = myPANID.v[0];
    TxBuffer[7] = myPANID.v[1];
    TxBuffer[8] = myShortAddress.v[0];
    TxBuffer[9] = myShortAddress.v[1];
    TxBuffer[10] = MiWiPROSeqNum++;
    
    #if defined(ENABLE_INDIRECT_MESSAGE) && defined(NWK_ROLE_COORDINATOR)
        if( MultiCastMode == BROADCAST_TO_ALL )
        {
            uint8_t i;
            
            for(i = 0; i < CONNECTION_SIZE; i++)
            {
                if( ConnectionTable[i].status.bits.isValid && ConnectionTable[i].status.bits.RXOnWhenIdle == 0 )
                {
                    break;
                }     
            }
            
            if( i < CONNECTION_SIZE )
            {
                #if defined(IEEE_802_15_4)
                    SaveIndirectMessage(true, myPANID, NULL, false, SecEn);
                #else
                    SaveIndirectMessage(true, NULL, false, SecEn);
                #endif
            }
        }    
    #endif
        
    #if defined(ENABLE_ENHANCED_DATA_REQUEST) && defined(ENABLE_SLEEP)
        if( MiWiPROStateMachine.bits.Sleeping )
        {
            if( SecEn )
            {
                TxBuffer[1] |= 1;
            }    
            return true;
        }    
    #endif
    
    MTP.flags.Val = 0;
    MTP.flags.bits.broadcast = 1;
    MTP.flags.bits.secEn = SecEn;

        
    #if defined(IEEE_802_15_4)
        MTP.altSrcAddr = true;
        MTP.DestPANID.Val = myPANID.Val;
    #else
        if(MultiCastMode == MULTICAST_TO_COORDINATORS)
        {
            MTP.flags.bits.sourcePrsnt = 1;
        }    
    #endif    

    return MiMAC_SendPacket(MTP, TxBuffer, TxData);    
}    


/************************************************************************************
 * Function:
 *      BOOL MiApp_UnicastConnection(uint8_t ConnectionIndex, BOOL SecEn)
 *
 * Summary:
 *      This function unicast a message in the TxBuffer to the device with the input 
 *      ConnectionIndex in the connection table. 
 *
 * Description:        
 *      This is one of the primary user interface functions for the application layer to 
 *      unicast a message. The destination device is in the connection table specified by 
 *      the input parameter ConnectionIndex. The application payload is filled in the 
 *      global char array TxBuffer.
 *
 * PreCondition:    
 *      Protocol initialization has been done. The input parameter ConnectionIndex points to
 *      a valid peer device in the connection table.
 *
 * Parameters: 
 *      uint8_t ConnectionIndex -  The index of the destination device in the connection table.
 *      BOOL SecEn -    The boolean indicates if the application payload needs to be
 *                      secured before transmission.
 *
 * Returns: 
 *      A boolean to indicates if the unicast procedure is succcessful.
 *
 * Example:
 *      <code>
 *      // Secure and then unicast the message stored in TxBuffer to the first device in 
 *      // the connection table
 *      MiApp_UnicastConnection(0, true);
 *      </code>
 *
 * Remarks:    
 *      None
 *
 *****************************************************************************************/  
BOOL MiApp_UnicastConnection( INPUT uint8_t ConnectionIndex,
                        INPUT BOOL SecEn)
{
    uint8_t MiWiPROFrameControl;
    
    if( ConnectionTable[ConnectionIndex].status.bits.isValid == 0 )
    {
        return false;
    }
    if( MiWiPROAckRequired && TxBuffer[MIWI_PRO_HEADER_LEN])
    {
        MiWiPROStateMachine.bits.MiWiPROAckInProgress = 1;
        AcknowledgementSeq = MiWiPROSeqNum;
        AcknowledgementAddr.Val = ConnectionTable[ConnectionIndex].AltAddress.Val;
        MiWiPROFrameControl = 0x06;
    }    
    else
    {
        MiWiPROStateMachine.bits.MiWiPROAckInProgress = 0;
        MiWiPROFrameControl = 0x02;
    }
        
    TxBuffer[0] = defaultHops;				//number of hops
    TxBuffer[1] = MiWiPROFrameControl;		//Frame Control
    TxBuffer[2] = ConnectionTable[ConnectionIndex].PANID.v[0];			
    TxBuffer[3] = ConnectionTable[ConnectionIndex].PANID.v[1];
    TxBuffer[4] = ConnectionTable[ConnectionIndex].AltAddress.v[0];
    TxBuffer[5] = ConnectionTable[ConnectionIndex].AltAddress.v[1];
    TxBuffer[6] = myPANID.v[0];			//source PANID LSB
    TxBuffer[7] = myPANID.v[1];			//source PANID MSB
    TxBuffer[8] = myShortAddress.v[0];	//source address LSB
    TxBuffer[9] = myShortAddress.v[1];	//source address MSB
    TxBuffer[10] = MiWiPROSeqNum++;			    //sequence number
    
    #if defined(NWK_ROLE_COORDINATOR)
        if( false == RouteMessage(ConnectionTable[ConnectionIndex].PANID, ConnectionTable[ConnectionIndex].AltAddress, SecEn) )
        {
            MiWiPROStateMachine.bits.MiWiPROAckInProgress = 0;
            return false;
        }
        else if( MiWiPROStateMachine.bits.MiWiPROAckInProgress &&
                 (ConnectionTable[ConnectionIndex].AltAddress.v[0] < 0x80))
        {
            MIWI_TICK t1, t2;
            t1 = MiWi_TickGet();
            while(1)
            {
                if( MiApp_MessageAvailable())
                {
                    MiApp_DiscardMessage();
                }                
                //MiWiPROTasks();
                if( MiWiPROStateMachine.bits.MiWiPROAckInProgress == 0 )
                {
                    return true;
                }    
                t2 = MiWi_TickGet();
                if( MiWi_TickGetDiff(t2, t1) > MIWI_PRO_ACK_TIMEOUT )
                {
                    MiWiPROStateMachine.bits.MiWiPROAckInProgress = 0;
                    return false;
                }    
            }    
        }
        return true;
    #else
        // for end device, always send the message to its parent
        #if defined(ENABLE_ENHANCED_DATA_REQUEST) && defined(ENABLE_SLEEP)
            if( MiWiPROStateMachine.bits.Sleeping )
            {
                if( SecEn )
                {
                    TxBuffer[1] |= 1;
                }
                return true;
            }    
        #endif
        
        MTP.flags.Val = 0;
        MTP.flags.bits.ackReq = 1;
        MTP.flags.bits.secEn = SecEn;
            
        #if defined(IEEE_802_15_4)
            MTP.altDestAddr = true;
            MTP.altSrcAddr = true;
            MTP.DestAddress = ConnectionTable[myParent].AltAddress.v;
            MTP.DestPANID.Val = ConnectionTable[myParent].PANID.Val;
        #else
            MTP.DestAddress = ConnectionTable[myParent].Address;
        #endif
        
        if( MiMAC_SendPacket(MTP, TxBuffer, TxData) == false )
        {
            MiWiPROStateMachine.bits.MiWiPROAckInProgress = 0;
            return false;
        }  
        
        if( MiWiPROStateMachine.bits.MiWiPROAckInProgress &&
            (ConnectionTable[ConnectionIndex].AltAddress.v[0] < 0x80) )
        {
            MIWI_TICK t1, t2;
            t1 = MiWi_TickGet();
            while(1)
            {
                if( MiApp_MessageAvailable())
                {
                    MiApp_DiscardMessage();
                }                
                //MiWiPROTasks();
                if( MiWiPROStateMachine.bits.MiWiPROAckInProgress == 0 )
                {
                    return true;
                }    
                t2 = MiWi_TickGet();
                if( MiWi_TickGetDiff(t2, t1) > MIWI_PRO_ACK_TIMEOUT )
                {
                    MiWiPROStateMachine.bits.MiWiPROAckInProgress = 0;
                    return false;
                }    
            }    
        }
        return true;
    #endif
}                        




BOOL UnicastShortAddress(uint8_t *DestAddress)
{
    MiWiPROStateMachine.bits.MiWiPROAckInProgress = 0;

    TxBuffer[0] = defaultHops;          //number of hops
    TxBuffer[1] = 0X02;                 //Frame Control
    TxBuffer[2] = myPANID.v[0];
    TxBuffer[3] = myPANID.v[1];
    TxBuffer[4] = DestAddress[0];
    TxBuffer[5] = DestAddress[1];
    TxBuffer[6] = myPANID.v[0];			//source PANID LSB
    TxBuffer[7] = myPANID.v[1];			//source PANID MSB
    TxBuffer[8] = myShortAddress.v[0];	//source address LSB
    TxBuffer[9] = myShortAddress.v[1];	//source address MSB
    TxBuffer[10] = MiWiPROSeqNum++;	    //sequence number

    #if defined(NWK_ROLE_COORDINATOR)
        tempShortAddress.v[0] = DestAddress[0];
        tempShortAddress.v[1] = DestAddress[1];
        if( false == RouteMessage(myPANID, tempShortAddress, false) )
        {
            return false;
        }
        return true;
    #else
        // for end device, always send data to its parent

        #if defined(ENABLE_ENHANCED_DATA_REQUEST) && defined(ENABLE_SLEEP)
            if( MiWiPROStateMachine.bits.Sleeping )
            {
                return true;
            }
        #endif


        MTP.flags.Val = 0;
        MTP.flags.bits.ackReq = 1;

        #if defined(IEEE_802_15_4)
            MTP.DestPANID.Val = ConnectionTable[myParent].PANID.Val;
            MTP.altDestAddr = true;
            MTP.altSrcAddr = true;
            MTP.DestAddress = ConnectionTable[myParent].AltAddress.v;
        #else
            MTP.DestAddress = ConnectionTable[myParent].Address;
        #endif


        if( MiMAC_SendPacket(MTP, TxBuffer, TxData) == false )
        {
            MiWiPROStateMachine.bits.MiWiPROAckInProgress = 0;
            return false;
        }
        return true;
    #endif
}








BOOL MiApp_UnicastAddress(uint8_t *DestAddress, BOOL PermanentAddr, BOOL SecEn)
{
    uint8_t handle;
    uint8_t i;
    uint8_t MiWiPROFrameControl;
    
    if(PermanentAddr == false)
    {
        tempShortAddress.v[0] = DestAddress[0];
        tempShortAddress.v[1] = DestAddress[1];
        if( tempShortAddress.Val > 0xFFFC )
        {
            switch(tempShortAddress.Val)
            {
                case 0xFFFF:
                    return MultiCast(BROADCAST_TO_ALL, SecEn);
                   
                case 0xFFFE:
                    return MultiCast(MULTICAST_TO_FFDS, SecEn);
                    
                case 0xFFFD:
                    return MultiCast(MULTICAST_TO_COORDINATORS, SecEn);
                    
                default:
                    return false;
            }
        }
    }
    
    
    if( MiWiPROAckRequired && TxBuffer[MIWI_PRO_HEADER_LEN])
    {
        MiWiPROStateMachine.bits.MiWiPROAckInProgress = 1;
        AcknowledgementSeq = MiWiPROSeqNum;
        MiWiPROFrameControl = 0x06;
    } 
    else
    {
        MiWiPROStateMachine.bits.MiWiPROAckInProgress = 0;
        MiWiPROFrameControl = 0x02;
    }
           
    if( PermanentAddr )
    {
        for(i = 0; i < MY_ADDRESS_LENGTH; i++)
        {
            tempLongAddress[i] = DestAddress[i];
        }
        if( (handle = SearchForLongAddress()) == 0xFF )
        {
DIRECT_LONG_ADDRESS:

            TxBuffer[0] = defaultHops;          //number of hops
            TxBuffer[1] = 0x02;		            //Frame Control
            TxBuffer[2] = 0xFF;			
            TxBuffer[3] = 0xFF;
            TxBuffer[4] = 0xFF;
            TxBuffer[5] = 0xFF;
            TxBuffer[6] = myPANID.v[0];			//source PANID LSB
            TxBuffer[7] = myPANID.v[1];			//source PANID MSB
            TxBuffer[8] = myShortAddress.v[0];	//source address LSB
            TxBuffer[9] = myShortAddress.v[1];	//source address MSB
            TxBuffer[10] = MiWiPROSeqNum++;			    //sequence number

            #if defined(ENABLE_ENHANCED_DATA_REQUEST) && defined(ENABLE_SLEEP)
                if( MiWiPROStateMachine.bits.Sleeping )
                {
                    if( SecEn )
                    {
                        TxBuffer[1] |= 1;
                    }  
                    return true;
                }    
            #endif
            
            MTP.flags.Val = 0;
            MTP.flags.bits.ackReq = 1;
            MTP.flags.bits.secEn = SecEn;   
            MTP.DestAddress = DestAddress;
            
            #if defined(IEEE_802_15_4)
                MTP.altDestAddr = false;
                MTP.altSrcAddr = true;
                MTP.DestPANID.Val = myPANID.Val;
            #endif
            MiWiPROStateMachine.bits.MiWiPROAckInProgress = 0;
            return MiMAC_SendPacket(MTP, TxBuffer, TxData);
        }
        else
        {
            if( ConnectionTable[handle].status.bits.shortAddressValid == 0 )
            {
                goto DIRECT_LONG_ADDRESS;
            }
            
            if( MiWiPROAckRequired )
            {
                AcknowledgementAddr.Val = ConnectionTable[handle].AltAddress.Val;
            }    
            
            TxBuffer[0] = defaultHops;          //number of hops
            TxBuffer[1] = MiWiPROFrameControl;	    //Frame Control
            TxBuffer[2] = ConnectionTable[handle].PANID.v[0];			
            TxBuffer[3] = ConnectionTable[handle].PANID.v[1];
            TxBuffer[4] = ConnectionTable[handle].AltAddress.v[0];
            TxBuffer[5] = ConnectionTable[handle].AltAddress.v[1];
            TxBuffer[6] = myPANID.v[0];			//source PANID LSB
            TxBuffer[7] = myPANID.v[1];			//source PANID MSB
            TxBuffer[8] = myShortAddress.v[0];	//source address LSB
            TxBuffer[9] = myShortAddress.v[1];	//source address MSB
            TxBuffer[10] = MiWiPROSeqNum++;	    //sequence number
            #if defined(NWK_ROLE_COORDINATOR)
                if( false == RouteMessage(ConnectionTable[handle].PANID, ConnectionTable[handle].AltAddress, SecEn) )
                {
                    MiWiPROStateMachine.bits.MiWiPROAckInProgress = 0;
                    return false;
                }
                
                if( MiWiPROStateMachine.bits.MiWiPROAckInProgress )
                {
                    MIWI_TICK t1, t2;
                    t1 = MiWi_TickGet();
                    while(1)
                    {
                        if( MiApp_MessageAvailable())
                        {
                            MiApp_DiscardMessage();
                        }                        
                        //MiWiPROTasks();
                        if( MiWiPROStateMachine.bits.MiWiPROAckInProgress == 0 )
                        {
                            return true;
                        }    
                        t2 = MiWi_TickGet();
                        if( MiWi_TickGetDiff(t2, t1) > MIWI_PRO_ACK_TIMEOUT )
                        {
                            MiWiPROStateMachine.bits.MiWiPROAckInProgress = 0;
                            return false;
                        }    
                    }    
                }
                return true;
            #else
                // for end device, always send data to its parent
                
                #if defined(ENABLE_ENHANCED_DATA_REQUEST) && defined(ENABLE_SLEEP)
                    if( MiWiPROStateMachine.bits.Sleeping )
                    {
                        if( SecEn )
                        {
                            TxBuffer[1] |= 1;
                        }  
                        return true;
                    }    
                #endif
                
                MTP.flags.Val = 0;
                MTP.flags.bits.ackReq = 1;
                MTP.flags.bits.secEn = SecEn;   
                
                #if defined(IEEE_802_15_4)
                    MTP.DestPANID.Val = ConnectionTable[myParent].PANID.Val;
                    MTP.altDestAddr = true;
                    MTP.altSrcAddr = true;
                    MTP.DestAddress = ConnectionTable[myParent].AltAddress.v;
                #else
                    MTP.DestAddress = ConnectionTable[myParent].Address;
                #endif


                if( MiMAC_SendPacket(MTP, TxBuffer, TxData) == false )
                {
                    MiWiPROStateMachine.bits.MiWiPROAckInProgress = 0;
                    return false;
                }
                    
                if( MiWiPROStateMachine.bits.MiWiPROAckInProgress )
                {
                    MIWI_TICK t1, t2;
                    t1 = MiWi_TickGet();
                    while(1)
                    {
                        if( MiApp_MessageAvailable())
                        {
                            MiApp_DiscardMessage();
                        }                        
                        //MiWiPROTasks();
                        if( MiWiPROStateMachine.bits.MiWiPROAckInProgress == 0 )
                        {
                            return true;
                        }    
                        t2 = MiWi_TickGet();
                        if( MiWi_TickGetDiff(t2, t1) > MIWI_PRO_ACK_TIMEOUT )
                        {
                            MiWiPROStateMachine.bits.MiWiPROAckInProgress = 0;
                            return false;
                        }    
                    }    
                }
                return true;
            #endif
        }
    }

    TxBuffer[0] = defaultHops;          //number of hops
    TxBuffer[1] = MiWiPROFrameControl;  //Frame Control
    TxBuffer[2] = myPANID.v[0];			
    TxBuffer[3] = myPANID.v[1];
    TxBuffer[4] = DestAddress[0];
    TxBuffer[5] = DestAddress[1];
    TxBuffer[6] = myPANID.v[0];			//source PANID LSB
    TxBuffer[7] = myPANID.v[1];			//source PANID MSB
    TxBuffer[8] = myShortAddress.v[0];	//source address LSB
    TxBuffer[9] = myShortAddress.v[1];	//source address MSB
    TxBuffer[10] = MiWiPROSeqNum++;	    //sequence number
    
    AcknowledgementAddr.v[0] = DestAddress[0];
    AcknowledgementAddr.v[1] = DestAddress[1];
    
    #if defined(NWK_ROLE_COORDINATOR)
        tempShortAddress.v[0] = DestAddress[0];
        tempShortAddress.v[1] = DestAddress[1];
        if( false == RouteMessage(myPANID, tempShortAddress, SecEn) )
        {
            MiWiPROStateMachine.bits.MiWiPROAckInProgress = 0;
            return false;
        }
        else if( MiWiPROStateMachine.bits.MiWiPROAckInProgress && 
                    (DestAddress[0] < 0x80) )
        {
            MIWI_TICK t1, t2;

            t1 = MiWi_TickGet();
            while(1)
            {
                if( MiApp_MessageAvailable())
                {
                    MiApp_DiscardMessage();
                }                
                //MiWiPROTasks();
                if( MiWiPROStateMachine.bits.MiWiPROAckInProgress == 0 )
                {
                    return true;
                }    
                t2 = MiWi_TickGet();
                if( MiWi_TickGetDiff(t2, t1) > MIWI_PRO_ACK_TIMEOUT )
                {
                    MiWiPROStateMachine.bits.MiWiPROAckInProgress = 0;
                    return false;
                }    
            }    
        }
        return true;
    #else
        // for end device, always send data to its parent
        
        #if defined(ENABLE_ENHANCED_DATA_REQUEST) && defined(ENABLE_SLEEP)
            if( MiWiPROStateMachine.bits.Sleeping )
            {
                if( SecEn )
                {
                    TxBuffer[1] |= 1;
                }  
                return true;
            }    
        #endif
        
        
        MTP.flags.Val = 0;
        MTP.flags.bits.ackReq = 1;
        MTP.flags.bits.secEn = SecEn;   
        
        #if defined(IEEE_802_15_4)
            MTP.DestPANID.Val = ConnectionTable[myParent].PANID.Val;
            MTP.altDestAddr = true;
            MTP.altSrcAddr = true;
            MTP.DestAddress = ConnectionTable[myParent].AltAddress.v;
        #else
            MTP.DestAddress = ConnectionTable[myParent].Address;
        #endif
        
        
        if( MiMAC_SendPacket(MTP, TxBuffer, TxData) == false )
        {
            MiWiPROStateMachine.bits.MiWiPROAckInProgress = 0;
            return false;
        }    
        else if( MiWiPROStateMachine.bits.MiWiPROAckInProgress )
        {
            MIWI_TICK t1, t2;
            t1 = MiWi_TickGet();
            while(1)
            {
                if( MiApp_MessageAvailable())
                {
                    MiApp_DiscardMessage();
                }                
                //MiWiPROTasks();
                if( MiWiPROStateMachine.bits.MiWiPROAckInProgress == 0 )
                {
                    return true;
                }    
                t2 = MiWi_TickGet();
                if( MiWi_TickGetDiff(t2, t1) > MIWI_PRO_ACK_TIMEOUT )
                {
                    MiWiPROStateMachine.bits.MiWiPROAckInProgress = 0;
                    return false;
                }    
            }    
        }
        return true;
    #endif
}



BOOL MiApp_StartConnection(uint8_t Mode, uint8_t ScanDuration, uint32_t ChannelMap)
{
    switch(Mode)
    {
        #if defined(NWK_ROLE_COORDINATOR)
            case START_CONN_DIRECT:
                myShortAddress.Val = 0;
                myParent = 0xFF;
                #if MY_PAN_ID == 0xFFFF
                    myPANID.v[0] = TMRL;
                    myPANID.v[1] = TMRL+0x51;
                #else
                    myPANID.Val = MY_PAN_ID;
                #endif
                MiMAC_SetAltAddress(myShortAddress.v, myPANID.v);
                MiWiPROStateMachine.bits.memberOfNetwork = 1;
                role = ROLE_PAN_COORDINATOR;
                MiWiPROCapacityInfo.bits.Role = role;
                FamilyTree[0] = 0;
                RoutingTable[0] = 0;
                NeighborRoutingTable[0][0] = 0;
                #if defined(ENABLE_NETWORK_FREEZER)
                    nvmPutMyShortAddress(myShortAddress.v);
                    nvmPutMyPANID(myPANID.v);
                    nvmPutRole(&role);
                    nvmPutFamilyTree(FamilyTree);
                    nvmPutRoutingTable(RoutingTable);
                    nvmPutNeighborRoutingTable(NeighborRoutingTable);
                #endif
                #if defined(ENABLE_TIME_SYNC) && !defined(ENABLE_SLEEP) && defined(ENABLE_INDIRECT_MESSAGE)
                    TimeSyncTick = MiWi_TickGet();
                #endif
                return true;
        #endif
            
        case START_CONN_ENERGY_SCN:
            #if defined(ENABLE_ED_SCAN) && defined(NWK_ROLE_COORDINATOR)
            {
                uint8_t channel;
                uint8_t RSSIValue;
                
                channel = MiApp_NoiseDetection(ChannelMap, ScanDuration, NOISE_DETECT_ENERGY, &RSSIValue);
                MiApp_SetChannel(channel);
                Printf("\r\nStart Wireless Communication on Channel ");
                PrintDec(channel);
                Printf("\r\n");
                MiWiPROStateMachine.bits.memberOfNetwork = 1;
                myShortAddress.Val = 0;
                myParent = 0xFF;
                #if MY_PAN_ID == 0xFFFF
                    myPANID.v[0] = TMRL;
                    myPANID.v[1] = TMRL+0x51;
                #else
                    myPANID.Val = MY_PAN_ID;
                #endif
                MiMAC_SetAltAddress(myShortAddress.v, myPANID.v);
                role = ROLE_PAN_COORDINATOR;
                MiWiPROCapacityInfo.bits.Role = role;

                FamilyTree[0] = 0;
                RoutingTable[0] = 0;
                NeighborRoutingTable[0][0] = 0;
                #if defined(ENABLE_NETWORK_FREEZER)
                    nvmPutMyShortAddress(myShortAddress.v);
                    nvmPutMyPANID(myPANID.v);
                    nvmPutRole(&role);
                    nvmPutFamilyTree(FamilyTree);
                    nvmPutRoutingTable(RoutingTable);
                    nvmPutNeighborRoutingTable(NeighborRoutingTable);
                #endif
                #if defined(ENABLE_TIME_SYNC) && !defined(ENABLE_SLEEP) && defined(ENABLE_INDIRECT_MESSAGE)
                    TimeSyncTick = MiWi_TickGet();
                #endif
                return true;
            }
            #else
                return false;
            #endif
            
        case START_CONN_CS_SCN:
            // Carrier sense scan is not supported for current available transceivers
            return false;
        
        default:
            break;
    }    
    return false;
}



    #ifdef ENABLE_ED_SCAN
        /************************************************************************************
         * Function:
         *      uint8_t MiApp_NoiseDetection(  uint32_t ChannelMap, uint8_t ScanDuration,
         *                                  uint8_t DetectionMode, uint8_t *NoiseLevel)
         *
         * Summary:
         *      This function perform a noise scan and returns the channel with least noise
         *
         * Description:        
         *      This is the primary user interface functions for the application layer to 
         *      perform noise detection on multiple channels.
         *
         * PreCondition:    
         *      Protocol initialization has been done. 
         *
         * Parameters: 
         *      uint32_t ChannelMap -  The bit map of channels to perform noise scan. The 32-bit
         *                          double word parameter use one bit to represent corresponding
         *                          channels from 0 to 31. For instance, 0x00000003 represent to 
         *                          scan channel 0 and channel 1. 
         *      uint8_t ScanDuration - The maximum time to perform scan on single channel. The
         *                          value is from 5 to 14. The real time to perform scan can
         *                          be calculated in following formula from IEEE 802.15.4 
         *                          specification 
         *                              960 * (2^ScanDuration + 1) * 10^(-6) second
         *      uint8_t DetectionMode -    The noise detection mode to perform the scan. The two possible
         *                              scan modes are
         *                              * NOISE_DETECT_ENERGY   Energy detection scan mode
         *                              * NOISE_DETECT_CS       Carrier sense detection scan mode
         *      uint8_t *NoiseLevel -  The noise level at the channel with least noise level
         *
         * Returns: 
         *      The channel that has the lowest noise level
         *
         * Example:
         *      <code>
         *      uint8_t NoiseLevel;
         *      OptimalChannel = MiApp_NoiseDetection(0xFFFFFFFF, 10, NOISE_DETECT_ENERGY, &NoiseLevel);
         *      </code>
         *
         * Remarks:    
         *      None
         *
         *****************************************************************************************/
        uint8_t MiApp_NoiseDetection(INPUT uint32_t ChannelMap, INPUT uint8_t ScanDuration, INPUT uint8_t DetectionMode, OUTPUT uint8_t *RSSIValue)
        {
            uint8_t i;
            uint8_t OptimalChannel = 0xFF;
            uint8_t minRSSI = 0xFF;
            uint32_t channelMask = 0x00000001;
            MIWI_TICK t1, t2;
            
            if( DetectionMode != NOISE_DETECT_ENERGY )
            {
                return 0xFF;
            }
            
            ConsolePutROMString((ROM char*)"\r\nEnergy Scan Results:");
            i = 0;
            while( i < 32 )
            {
                #if defined(ENABLE_FREQUENCY_AGILITY)
                    EnergyScanResults[i] = 0xFF;
                #endif
                if( ChannelMap & FULL_CHANNEL_MAP & (channelMask << i) )
                {
                    uint8_t RSSIcheck;
                    uint8_t maxRSSI = 0;
                    uint8_t j, k;
    
                    /* choose appropriate channel */
                    MiApp_SetChannel(i);
                    
                    t2 = MiWi_TickGet();
                    
                    while(1)
                    {
                        RSSIcheck = MiMAC_ChannelAssessment(CHANNEL_ASSESSMENT_ENERGY_DETECT);
                        if( RSSIcheck > maxRSSI )
                        {
                            maxRSSI = RSSIcheck;
                        }
                        
                        t1 = MiWi_TickGet();
                        if( MiWi_TickGetDiff(t1, t2) > ((uint32_t)(ScanTime[ScanDuration])) )
                        {
                            // if scan time exceed scan duration, prepare to scan the next channel
                            break;
                        }
                    } 
                    
                    Printf("\r\nChannel ");
                    PrintDec(i);
                    Printf(": ");
                    j = maxRSSI/5;
                    for(k = 0; k < j; k++)
                    {
                        ConsolePut('-');
                    }
                    Printf(" ");
                    PrintChar(maxRSSI);
                    
                    #if defined(ENABLE_FREQUENCY_AGILITY)
                        EnergyScanResults[i] = maxRSSI;
                    #endif
                    
                    if( maxRSSI < minRSSI )
                    {
                        minRSSI = maxRSSI;
                        OptimalChannel = i;
                        if( RSSIValue )
                        {
                            *RSSIValue = minRSSI;
                        }   
                    }              
                }  
                i++;
            }        
           
            return OptimalChannel;
        }
       
    #endif


    #if defined(ENABLE_FREQUENCY_AGILITY)
        /********************************************************************************************
         * Function:
         *      BOOL MiApp_ResyncConnection(uint8_t ConnectionIndex, uint32_t ChannelMap)
         *
         * Summary:
         *      This function tries to resynchronize the lost connection with 
         *      peers, probably due to channel hopping
         *
         * Description:        
         *      This is the primary user interface function for the application to resynchronize a 
         *      lost connection. For a RFD device that goes to sleep periodically, it may not 
         *      receive the channel hopping command that is sent when it is sleep. The sleeping 
         *      RFD device depends on this function to hop to the channel that the rest of
         *      the PAN has jumped to. This function call is usually triggered by continously 
         *      communication failure with the peers.
         *
         * PreCondition:    
         *      Transceiver has been initialized
         *
         * Parameters:      
         *      uint32_t ChannelMap -  The bit map of channels to perform noise scan. The 32-bit
         *                          double word parameter use one bit to represent corresponding
         *                          channels from 0 to 31. For instance, 0x00000003 represent to 
         *                          scan channel 0 and channel 1. 
         *                  
         * Returns: 
         *                  a boolean to indicate if resynchronization of connection is successful
         *
         * Example:
         *      <code>
         *      // Sleeping RFD device resync with its associated device, usually the first peer
         *      // in the connection table
         *      MiApp_ResyncConnection(0, 0xFFFFFFFF);
         *      </code>
         *
         * Remark:          
         *      If operation is successful, the wireless node will be hopped to the channel that 
         *      the rest of the PAN is operating on.
         *
         *********************************************************************************************/ 
        BOOL MiApp_ResyncConnection(INPUT uint8_t ConnectionIndex, INPUT uint32_t ChannelMap)
        {
            uint8_t i;
            uint8_t j;
            uint8_t backupChannel = currentChannel;
            MIWI_TICK t1, t2;
            
            t1 = MiWi_TickGet();
            MiWiPROStateMachine.bits.Resynning = 1;
            for(i = 0; i < RESYNC_TIMES; i++)
            {
                uint32_t ChannelMask = 0x00000001;
                
                j = 0;
                while(MiWiPROStateMachine.bits.Resynning)
                {
                    t2 = MiWi_TickGet();
                    
                    if( MiWi_TickGetDiff(t2, t1) > SCAN_DURATION_9 )
                    {
                        t1.Val = t2.Val;
                        
                        if( j > 31 )
                        {
                            break;
                        }
                        
                        while( (ChannelMap & FULL_CHANNEL_MAP & (ChannelMask << j)) == 0 )
                        {
                            if( ++j > 31 )
                            {
                                goto GetOutOfLoop;
                            }
                        }
                        
                        Printf("\r\nChecking Channel ");
                        PrintDec(j);
                        MiApp_SetChannel(j);
                        j++;
    
                    
                        MiApp_FlushTx();
                        MiApp_WriteData(MIWI_PRO_STACK_REPORT_TYPE);
                        MiApp_WriteData(RESYNCHRONIZATION_REQUEST);
                        MiApp_WriteData(currentChannel);
                        MiApp_UnicastConnection(ConnectionIndex, false);
                    }
                    if( MiApp_MessageAvailable())
                    {
                        MiApp_DiscardMessage();
                    }                    
                    //MiWiPROTasks();
                }
                if( MiWiPROStateMachine.bits.Resynning == 0 )
                {
                    Printf("\r\nResynchronized Connection to Channel ");
                    PrintDec(currentChannel);
                    Printf("\r\n");
                    return true;
                }
GetOutOfLoop:
                MacroNop();         
            }
            
            MiApp_SetChannel(backupChannel);
            MiWiPROStateMachine.bits.Resynning = 0;
            return false;
        }

        #if defined(NWK_ROLE_COORDINATOR)
            /*********************************************************************
             * void StartChannelHopping(uint8_t OptimalChannel)
             *
             * Overview:        This function broadcast the channel hopping command
             *                  and after that, change operating channel to the 
             *                  input optimal channel     
             *
             * PreCondition:    Transceiver has been initialized
             *
             * Input:           OptimalChannel  - The channel to hop to
             *                  
             * Output: 
             *          None
             *
             * Side Effects:    The operating channel for current device will change
             *                  to the specified channel
             *
             ********************************************************************/
            void StartChannelHopping(INPUT uint8_t OptimalChannel)
            {
                uint8_t i;
                MIWI_TICK t1, t2;
                
                for( i = 0; i < FA_BROADCAST_TIME; i++)
                {
                    t1 = MiWi_TickGet();
                    while(1)
                    {
                        t2 = MiWi_TickGet();
                        if( MiWi_TickGetDiff(t2, t1) > FA_COMM_INTERVAL )
                        {
                            MiApp_FlushTx();
                            MiApp_WriteData(MIWI_PRO_STACK_REPORT_TYPE);
                            MiApp_WriteData(CHANNEL_HOPPING_REQUEST);
                            MiApp_WriteData(currentChannel);
                            MiApp_WriteData(OptimalChannel);
                            MultiCast(MULTICAST_TO_FFDS, false);
                            break;
                        }
                    }
                }
                FrequencyAgilityInfo.status.bits.ChangeChannel = 1;
                FrequencyAgilityInfo.FA_StatTick = MiWi_TickGet();
            }


            BOOL MiApp_InitChannelHopping( INPUT uint32_t ChannelMap)
            {
                uint8_t RSSIValue;
                uint8_t backupChannel = currentChannel;
                uint8_t backupConnMode = ConnMode;
                uint8_t i;
                uint8_t optimalChannel;
                MIWI_TICK t1, t2;
                
                if( role != ROLE_PAN_COORDINATOR )
                {
                    return false;
                }
                
                for( i = 0; i < FA_BROADCAST_TIME; i++)
                {
                    t1 = MiWi_TickGet();
                    while(1)
                    {
                        t2 = MiWi_TickGet();
                        if( MiWi_TickGetDiff(t2, t1) > FA_COMM_INTERVAL )
                        {
                            MiApp_FlushTx();
                            MiApp_WriteData(MIWI_PRO_STACK_REPORT_TYPE);
                            MiApp_WriteData(FA_ENERGY_SCAN_REQUEST);
                            MiApp_WriteData(ChannelMap);
                            MiApp_WriteData(ChannelMap>>8);
                            MiApp_WriteData(ChannelMap>>16);
                            MiApp_WriteData(ChannelMap>>24);
                            MultiCast(MULTICAST_TO_COORDINATORS, false);
                            break;
                        }
                    }
                }
                
                FrequencyAgilityInfo.status.Val = 0;
                MiApp_ConnectionMode(DISABLE_ALL_CONN);
                MiApp_NoiseDetection(ChannelMap, 10, NOISE_DETECT_ENERGY, &RSSIValue);
                MiApp_SetChannel(backupChannel);
                MiApp_ConnectionMode(backupConnMode);
                
                FrequencyAgilityInfo.FA_StatTick = MiWi_TickGet();
                while(1)
                {
                    t1 = MiWi_TickGet();
                    if( MiWi_TickGetDiff(t1, FrequencyAgilityInfo.FA_StatTick) > (FA_WAIT_TIMEOUT * 3) )
                    {
                        break;
                    }
                    if( MiApp_MessageAvailable())
                    {
                        MiApp_DiscardMessage();
                    }                    
                    //MiWiPROTasks();
                }
                
                while(1)
                {
                    RSSIValue = 0xFF;
                    optimalChannel = 0xFF;
                    for(i = 0; i < 32; i++)
                    {
                        if( EnergyScanResults[i] < RSSIValue )
                        {
                            RSSIValue = EnergyScanResults[i];
                            optimalChannel = i;
                        }    
                    }    
                                      
                    if( (optimalChannel < 0xFF) && (RSSIValue < FA_MAX_NOISE_THRESHOLD) )
                    {
                        EnergyScanResults[optimalChannel] = 0xFF;
                    } 
                    else
                    {
                        MiApp_SetChannel(backupChannel);
                        return false;
                    }    
                    
                    // send out FA_SUGGEST_CHANNEL command
                    MiApp_FlushTx();
                    MiApp_WriteData(MIWI_PRO_STACK_REPORT_TYPE);
                    MiApp_WriteData(FA_SUGGEST_CHANNEL);
                    MiApp_WriteData(optimalChannel);
                    MultiCast(MULTICAST_TO_COORDINATORS, false);
                    FrequencyAgilityInfo.status.bits.DisputeChannel = 0;
                    FrequencyAgilityInfo.status.bits.SuggestChannel = optimalChannel;
                    
                    FrequencyAgilityInfo.FA_StatTick = MiWi_TickGet();
                    while(1)
                    {
                        t1 = MiWi_TickGet();
                        if( MiWi_TickGetDiff(t1, FrequencyAgilityInfo.FA_StatTick) > FA_WAIT_TIMEOUT )
                        {
                            break;
                        } 
                        if( FrequencyAgilityInfo.status.bits.DisputeChannel )
                        {
                            break;
                        }
                        if( MiApp_MessageAvailable())
                        {
                            MiApp_DiscardMessage();
                        }       
                        //MiWiPROTasks();
                    }
                    
                    if( FrequencyAgilityInfo.status.bits.DisputeChannel == 0 )
                    {
                        break;
                    }     
                } 
                
                // send out FA_CHANGE_CHANNEL command
                StartChannelHopping(FrequencyAgilityInfo.status.bits.SuggestChannel);
                
                while(FrequencyAgilityInfo.status.bits.ChangeChannel)
                {
                    if( MiApp_MessageAvailable())
                    {
                        MiApp_DiscardMessage();
                    }
                    //MiWiPROTasks();
                }
                return true;
            }
        #endif
    
    #endif
    
    
    #if !defined(TARGET_SMALL)
        /*********************************************************************
         * Function:
         *      void MiApp_RemoveConnection(uint8_t ConnectionIndex)
         *
         * Summary:
         *      This function remove connection(s) in connection table
         *
         * Description:        
         *      This is the primary user interface function to disconnect connection(s).
         *      For a P2P protocol, it simply remove the connection. For a network protocol,
         *      if the device referred by the input parameter is the parent of the device
         *      calling this function, the calling device will get out of network along with
         *      its children. If the device referred by the input parameter is children of
         *      the device calling this function, the target device will get out of network.
         * 
         * PreCondition:    
         *      Transceiver has been initialized. Node has establish
         *      one or more connections
         *
         * Parameters:           
         *      uint8_t ConnectionIndex -  The index of the connection in the
         *                              connection table to be removed
         *                  
         * Returns: 
         *      None
         *
         * Example:
         *      <code>
         *      MiApp_RemoveConnection(0x00);
         *      </code>
         *
         * Remarks:    
         *      None
         *
         ********************************************************************/
        void MiApp_RemoveConnection(INPUT uint8_t ConnectionIndex)
        {
            uint8_t i;
                                
            if( ConnectionIndex == 0xFF )
            {
                for(i = 0; i < CONNECTION_SIZE; i++)
                {
                    if( ConnectionTable[i].status.bits.isValid )
                    {
                        if( ConnectionTable[i].status.bits.isFamily )
                        {
                            MAC_FlushTx();
                            MiApp_WriteData(MAC_COMMAND_DISASSOCIATION_NOTIFICATION);
                            MTP.flags.Val = 0;
                            MTP.flags.bits.ackReq = 1;
                            MTP.flags.bits.packetType = PACKET_TYPE_COMMAND;
                            #if defined(IEEE_802_15_4)
                                MTP.altDestAddr = true;
                                MTP.altSrcAddr = true;
                                MTP.DestPANID.Val = ConnectionTable[i].PANID.Val;
                                MTP.DestAddress = ConnectionTable[i].AltAddress.v;
                            #else
                                MTP.flags.bits.sourcePrsnt = 1;
                                MTP.DestAddress = ConnectionTable[i].Address;
                            #endif
                            MiMAC_SendPacket(MTP, TxBuffer, TxData);
                            DelayMs(100);
                        }
                    }
                    ConnectionTable[i].status.Val = 0;
                } 
                
                myParent = 0xFF;
                myShortAddress.Val = 0xFFFF;
                myPANID.Val = 0xFFFF;
                MiMAC_SetAltAddress(myShortAddress.v, myPANID.v);
                MiWiPROStateMachine.bits.memberOfNetwork = 0;
                #if defined(ENABLE_NETWORK_FREEZER)
                    nvmPutMyShortAddress(myShortAddress.v);
                    nvmPutMyPANID(myPANID.v);
                    nvmPutMyParent(&myParent);
                    nvmPutConnectionTable(ConnectionTable);
                #endif
            }
            else if( ConnectionTable[ConnectionIndex].status.bits.isValid )
            {    
                if( ConnectionTable[ConnectionIndex].status.bits.isFamily && (ConnectionIndex == myParent) ) // parent
                {
                    // first notify my parent
                    MAC_FlushTx();
                    MiApp_WriteData(MAC_COMMAND_DISASSOCIATION_NOTIFICATION);
                    MTP.flags.Val = 0;
                    MTP.flags.bits.ackReq = 1;
                    MTP.flags.bits.packetType = PACKET_TYPE_COMMAND;
                    #if defined(IEEE_802_15_4)
                        MTP.altDestAddr = true;
                        MTP.altSrcAddr = true;
                        MTP.DestPANID.Val = ConnectionTable[ConnectionIndex].PANID.Val;
                        MTP.DestAddress = ConnectionTable[ConnectionIndex].AltAddress.v;
                    #else
                        MTP.flags.bits.sourcePrsnt = 1;
                        MTP.DestAddress = ConnectionTable[ConnectionIndex].Address;
                    #endif
                    MiMAC_SendPacket(MTP, TxBuffer, TxData);
                    ConnectionTable[ConnectionIndex].status.Val = 0;
                    DelayMs(100);
                    

                    for(i = 0; i < CONNECTION_SIZE; i++)
                    {
                        #if defined(NWK_ROLE_COORDINATOR)
                            if( ConnectionTable[i].status.bits.isValid && 
                                ConnectionTable[i].status.bits.isFamily && (myParent != i) ) // children
                            {
                                #if defined(IEEE_802_15_4)
                                    MTP.DestPANID.Val = ConnectionTable[i].PANID.Val;
                                    MTP.DestAddress = ConnectionTable[i].AltAddress.v;
                                #else
                                    MTP.DestAddress = ConnectionTable[i].Address;
                                #endif 
                                MiMAC_SendPacket(MTP, TxBuffer, TxData);
                                DelayMs(100);     
                            }
                        #endif
                        ConnectionTable[i].status.Val = 0;
                    }

                    // get myself out of network
                    myParent = 0xFF;
                    myShortAddress.Val = 0xFFFF;
                    myPANID.Val = 0xFFFF;
                    MiMAC_SetAltAddress(myShortAddress.v, myPANID.v);
                    MiWiPROStateMachine.bits.memberOfNetwork = 0;
                    #if defined(ENABLE_NETWORK_FREEZER)
                        nvmPutMyShortAddress(myShortAddress.v);
                        nvmPutMyPANID(myPANID.v);
                        nvmPutMyParent(&myParent);
                        nvmPutConnectionTable(ConnectionTable);
                    #endif
                }
                #if defined(NWK_ROLE_COORDINATOR)                
                    else if( ConnectionTable[ConnectionIndex].status.bits.isFamily && (ConnectionIndex != myParent) ) // child
                    {
                        MAC_FlushTx();
                        MiApp_WriteData(MAC_COMMAND_DISASSOCIATION_NOTIFICATION);
                        MTP.flags.Val = 0;
                        MTP.flags.bits.ackReq = 1;
                        MTP.flags.bits.packetType = PACKET_TYPE_COMMAND;
                        #if defined(IEEE_802_15_4)
                            MTP.altDestAddr = true;
                            MTP.altSrcAddr = true;
                            MTP.DestPANID.Val = ConnectionTable[ConnectionIndex].PANID.Val;
                            MTP.DestAddress = ConnectionTable[ConnectionIndex].AltAddress.v;
                        #else
                            MTP.flags.bits.sourcePrsnt = 1;
                            MTP.DestAddress = ConnectionTable[ConnectionIndex].Address;
                        #endif
                        MiMAC_SendPacket(MTP, TxBuffer, TxData);
                        ConnectionTable[ConnectionIndex].status.Val = 0;
                        #if defined(ENABLE_NETWORK_FREEZER)
                            nvmPutConnectionTableIndex(&(ConnectionTable[ConnectionIndex]), ConnectionIndex);
                        #endif
                    }
                #endif
            }
        }
    #endif

    
#else
    // define a bogus variable to bypass limitation of C18 compiler not able to compile an empty file
    extern char bogusVar;
#endif

