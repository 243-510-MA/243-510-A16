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


#include "driver/mrf_miwi/drv_mrf_miwi_24xa.h"
#if defined(ENABLE_NVM)
#include "miwi/miwi_nvm.h"
#endif

#define INPUT
#define OUTPUT
#define IOPUT

/************************ VARIABLES ********************************/
MACINIT_PARAM   MACInitParams;


uint8_t RxBuffer[RX_PACKET_SIZE];



#ifdef ENABLE_SECURITY
    

const char mySecurityKey[16] = {SECURITY_KEY_00, SECURITY_KEY_01, SECURITY_KEY_02, SECURITY_KEY_03, SECURITY_KEY_04,
        SECURITY_KEY_05, SECURITY_KEY_06, SECURITY_KEY_07, SECURITY_KEY_08, SECURITY_KEY_09, SECURITY_KEY_10, SECURITY_KEY_11,
        SECURITY_KEY_12, SECURITY_KEY_13, SECURITY_KEY_14, SECURITY_KEY_15};

    
    

unsigned char myKeySequenceNumber = KEY_SEQUENCE_NUMBER;    // The sequence number of security key. Used to identify
                                                                    // the security key

API_UINT32_UNION OutgoingFrameCounter;
    
#endif
    

//#define Software_ACK

uint8_t IEEESeqNum;
uint8_t MACCurrentChannel = 11;

API_UINT16_UNION MAC_PANID;
API_UINT16_UNION myNetworkAddress;
volatile MRF24XA_IFREG INTflags;
volatile MRF24XA_STATUS MRF24XAStatus;

bool DataEncrypt(uint8_t *Payload, uint8_t *PayloadLen, API_UINT32_UNION FrameCounter,
                uint8_t FrameControl);
bool DataDecrypt(uint8_t *Payload, uint8_t *PayloadLen, uint8_t *SourceIEEEAddress,
                API_UINT32_UNION FrameCounter, uint8_t FrameControl);

/*********************************************************************
 * void PHYSetLongRAMAddr(INPUT uint16_t address, INPUT uint8_t value)
 *
 * Overview:        This function writes a value to a LONG RAM address
 *
 * PreCondition:    Communication port to the MRF24XA initialized
 *
 * Input:           address - the address of the LONG RAM address
 *                      that you want to write to
 *                  value   - the value that you want to write to
 *                      that register
 *
 * Output:          None
 *
 * Side Effects:    The register value is changed
 *                  Interrupt from radio is turned off before accessing
 *                  the SPI and turned back on after accessing the SPI
 *
 ********************************************************************/
void PHYSetLongRAMAddr(uint16_t address, uint8_t value)
{
    volatile uint8_t tmpRFIE = RFIE;
    
    RFIE = 0;
    PHY_CS = 0;
    SPIPut(LONGADDRESSINGMODE_PREFIX | (char)(address >> 3));
    SPIPut(LONGADDRESSINGMODE_WRITEPREFIX | (char)(address<<5));
    SPIPut(value);
    PHY_CS = 1;
    
    RFIE = tmpRFIE;
}

void PHYSetLongRAMAddrBloc(uint16_t address, uint8_t *buffer, uint8_t len)
{
    volatile uint8_t tmpRFIE = RFIE;
    uint8_t i;
    
    RFIE = 0;
    PHY_CS = 0;
    SPIPut(LONGADDRESSINGMODE_PREFIX | (char)(address >> 3));
    SPIPut(LONGADDRESSINGMODE_WRITEPREFIX | (char)(address<<5));
    
    for(i = 0; i < len; i++)
    {
        SPIPut(buffer[i]);
    }  
    PHY_CS = 1;
    RFIE = tmpRFIE;  
}    


void PHYSetShortRAMAddr(uint8_t address, uint8_t value)
{
    volatile uint8_t tmpRFIE = RFIE;
    
    RFIE = 0;
    PHY_CS = 0;
    SPIPut((address << 1) | 0x01);
    SPIPut(value);
    PHY_CS = 1;
    
    RFIE = tmpRFIE;
}    



/*********************************************************************
 * uint8_t PHYGetLongRAMAddr(INPUT uint16_t address)
 *
 * Overview:        This function reads a value from a long RAM address
 *
 * PreCondition:    Communication port to the MRF24XA initialized
 *
 * Input:           address - the address of the long RAM address
 *                      that you want to read from.  
 *
 * Output:          the value read from the specified Long register
 *
 * Side Effects:    Interrupt from radio is turned off before accessing
 *                  the SPI and turned back on after accessing the SPI
 *
 ********************************************************************/
uint8_t PHYGetLongRAMAddr(INPUT uint16_t address)
{
    uint8_t toReturn;
    volatile uint8_t tmpRFIE = RFIE;
    
    RFIE = 0;
    PHY_CS = 0;
    
    SPIPut(LONGADDRESSINGMODE_PREFIX | (char)(address >> 3));
    SPIPut((char)(address<<5));

    toReturn = SPIGet();
    PHY_CS = 1;
    RFIE = tmpRFIE;
    
    return toReturn;
}


uint8_t PHYGetShortRAMAddr(uint8_t address)
{
    uint8_t toReturn;
    volatile uint8_t tmpRFIE = RFIE;
    
    RFIE = 0;
    PHY_CS = 0;
    
    SPIPut(address << 1);
    toReturn = SPIGet();
    
    PHY_CS = 1;
    RFIE = tmpRFIE;
    
    return toReturn;
    
}    


void PHYGetShortRAMAddrBloc(INPUT uint16_t address, uint8_t *buffer, uint8_t len)
{
    volatile uint8_t tempRFIE = RFIE;
    uint8_t i;

    RFIE = 0;
    PHY_CS = 0;

    SPIPut(address << 1);

    for(i = 0; i < len; i++)
    {
        buffer[i] = SPIGet();
    }

    PHY_CS = 1;
    RFIE = tempRFIE;

}

void PHYGetLongRAMAddrBloc(INPUT uint16_t address, uint8_t *buffer, uint8_t len)
{
    volatile uint8_t tempRFIE = RFIE;
    uint8_t i;
    
    RFIE = 0;
    PHY_CS = 0;
    
    SPIPut(LONGADDRESSINGMODE_PREFIX | (char)(address >> 3));
    SPIPut((char)(address<<5));
    
    for(i = 0; i < len; i++)
    {
        buffer[i] = SPIGet();
    }    
    
    PHY_CS = 1;
    RFIE = tempRFIE;
}    


/*********************************************************************
 * void InitMRF24XA(void)
 *
 * Overview:        This function initialize MRF24XA transceiver
 *
 * PreCondition:    Communication port to the MRF24XA initialized
 *
 * Input:           none
 *
 * Output:          none
 *
 * Side Effects:    MRF24XA transceiver is initialized
 *
 ********************************************************************/
void InitMRF24XA(void)
{    
    uint8_t i;

    PHY_RESETn = 0;
    delay_ms(2);
    PHY_RESETn = 1;
    
    
    // delay 10 ms before radio finishing POR.
    delay_ms(20);

    // set interrupt masks
    PHYSetShortRAMAddr(0x08, 0x22);     // enable RDYIE and CALHAIE
    PHYSetShortRAMAddr(0x09, 0xD8);     // enable TXIE, TXACKIF, TXENCIF and TXCSMAIF
    PHYSetShortRAMAddr(0x0A, 0xE0);     // enable RXIE, RXDECIF and RXTAGIF
    PHYSetShortRAMAddr(0x0B, 0x00);
    
    PHYSetLongRAMAddr(0x00FF, 0x5A);
    
    // set to 15.4 mode
    PHYSetShortRAMAddr(0x10, 0x04);
    
    // set channel 11, no security
    PHYSetShortRAMAddr(0x11, 0x00);
    
    // data rate setting and enable CSMA
    PHYSetShortRAMAddr(0x12, 0x03);      // 250Kbps
    
    // ACK wait time / aTurnAround time 12 symbols, enable retransmission
    PHYSetShortRAMAddr(0x13, 0x0C);
    
    // retransmission number
    PHYSetShortRAMAddr(0x14, 0x30);
    
    // receive control, RX not enabled yet
    PHYSetShortRAMAddr(0x15, 0x18);
    
    // enable auto ack and reject identical packet
    #if defined( Software_ACK )
    {
        PHYSetShortRAMAddr(0x16, 0x00);
    }
    #else
    {
        PHYSetShortRAMAddr(0x16, 0x44);
    }
    #endif
    
    // ack timeout 54 symbols
    PHYSetShortRAMAddr(0x17, 0x36);
    
    // filter, CRC reject, not-me unicast reject and non standard frame reject
    PHYSetShortRAMAddr(0x18, 0x45);
    
    // CSMA backoff number = 4, base timer = 16 ???
    PHYSetShortRAMAddr(0x19, 0x90);
    
    // CSMA BE, min 3, max 5
    PHYSetShortRAMAddr(0x1A, 0x53);
    
    // CCA backoff unit BOUNIT
    PHYSetShortRAMAddr(0x1B, 0x14);
    
    // set long address
    for(i=0;i<(uint8_t)8;i++)
    {
        PHYSetShortRAMAddr(0x1F+i,MACInitParams.PAddress[i]);
    }
    
    // set short address
    PHYSetShortRAMAddr(0x27, 0xFF);
    PHYSetShortRAMAddr(0x28, 0xFF);
    
    // set PANID
    PHYSetShortRAMAddr(0x29, 0xFF);
    PHYSetShortRAMAddr(0x2A, 0xFF);
    
    // set CSMA threshold
    PHYSetShortRAMAddr(0x2F, 0x26);

    // CSMA setting
    PHYSetShortRAMAddr(0x30, 0x09);

    // only receive 250Kbps, use legacy preamble, disable psave
    PHYSetShortRAMAddr(0x36, 0xE6);
    
    // Psave threshold
    PHYSetShortRAMAddr(0x37, 0xA8);
    // Set Charge Pump max current
    PHYSetLongRAMAddr(0x00B7, 0xC0);
    
    //Set maximal RF Power
	PHYSetShortRAMAddr(0x3A, 0x1F);   // once
                   
    // set baseband high gain
    PHYSetShortRAMAddr(0x38, 0x20);   // High BB gain

    // Enable automatic PA and LNA switching
    // These lines make no harm on old boards. Could be left in.
    PHYSetShortRAMAddr(0x3D, 0x24);
    PHYSetShortRAMAddr(0x3E, 0x22);

    // Tuning
    PHYSetLongRAMAddr(0x80, 0x87);
    PHYSetLongRAMAddr(0x81, 0x84);
    PHYSetLongRAMAddr(0x82, 0x8A);
    PHYSetLongRAMAddr(0x83, 0x5E);
    PHYSetLongRAMAddr(0x84, 0xA5);
    PHYSetLongRAMAddr(0x8A, 0x2B);
    PHYSetLongRAMAddr(0x8B, 0x30);
    PHYSetLongRAMAddr(0x8C, 0x30);
    PHYSetLongRAMAddr(0x97, 0xD0);
    PHYSetLongRAMAddr(0xA3, 0x3F);
    PHYSetLongRAMAddr(0xA5, 0x1C);
    PHYSetLongRAMAddr(0xAA, 0x54);
    PHYSetLongRAMAddr(0xAB, 0xB0);

    // enable RX
    PHYSetShortRAMAddr(0x15, 0x98);


    // clear POR flag
    PHYSetShortRAMAddr(0x03, 0x00);

    
    // improves PER and range
    //PHYSetLongRAMAddr(0x00F4, 0x57);

}
 
bool MiMAC_ReceivedPacket(void)
{   

    //set the interrupt flag just in case the interrupt was missed
    if(RF_INT_PIN == 0)
    {
        RFIF = 1;
    }


    if( MRF24XAStatus.bits.SLEEP == 0 )
    {
        if( MRF24XAStatus.bits.RADIO_BUFFERED )
        {
            if( MRF24XAStatus.bits.RX_BUFFERED == 0 )
            {
                RxBuffer[0] = PHYGetLongRAMAddr(0x300)+2;
                if( RxBuffer[0] < RX_PACKET_SIZE )
                {
                    MRF24XAStatus.bits.RX_BUFFERED = 1;

                    PHYGetLongRAMAddrBloc(0x0301, &(RxBuffer[1]), RxBuffer[0]);
                }

                #if defined( Software_ACK )
                {
                    PHYSetShortRAMAddr(0x16, 0x00);
                }
                #else
                {
                    PHYSetShortRAMAddr(0x16, 0x44);
                }
                #endif
                //PHYSetShortRAMAddr(0x15, 0x98);
                MRF24XAStatus.bits.RADIO_BUFFERED = 0;
            }
        }
    }

    
    if(MRF24XAStatus.bits.RX_BUFFERED == 1)
    {
        uint8_t addrMode;
        bool bIntraPAN = true;
    
        if( (RxBuffer[1] & 0x40) == 0 )
        {
            bIntraPAN = false;
        }

        MACRxPacket.flags.Val = 0;
        MACRxPacket.altSourceAddress = false;

        //Determine the start of the MAC payload
        addrMode = RxBuffer[2] & 0xCC;
        switch(addrMode)
        {
            case 0xC8: //short dest, long source
                // for P2P only broadcast allows short destination address
                if( RxBuffer[6] == 0xFF && RxBuffer[7] == 0xFF )
                {
                    MACRxPacket.flags.bits.broadcast = 1;
                }
                MACRxPacket.flags.bits.sourcePrsnt = 1;
                
                if( bIntraPAN ) // check if it is intraPAN
                {
                    MACRxPacket.SourcePANID.v[0] = RxBuffer[4];
                    MACRxPacket.SourcePANID.v[1] = RxBuffer[5];
                    MACRxPacket.SourceAddress = &(RxBuffer[8]);
                    
                    MACRxPacket.PayloadLen = RxBuffer[0] - 19;
                    MACRxPacket.Payload = &(RxBuffer[16]);   
                }
                else
                {
                    MACRxPacket.SourcePANID.v[0] = RxBuffer[8];
                    MACRxPacket.SourcePANID.v[1] = RxBuffer[9];
                    MACRxPacket.SourceAddress = &(RxBuffer[10]);
                    MACRxPacket.PayloadLen = RxBuffer[0] - 21;
                    MACRxPacket.Payload = &(RxBuffer[18]);
                }


                break;
            
            case 0xCC: // long dest, long source
                MACRxPacket.flags.bits.sourcePrsnt = 1;
                if( bIntraPAN ) // check if it is intraPAN
                {
                    //rxFrame.flags.bits.intraPAN = 1;
                    MACRxPacket.SourcePANID.v[0] = RxBuffer[4];
                    MACRxPacket.SourcePANID.v[1] = RxBuffer[5];
                    MACRxPacket.SourceAddress = &(RxBuffer[14]);
                    MACRxPacket.PayloadLen = RxBuffer[0] - 25;
                    MACRxPacket.Payload = &(RxBuffer[22]);    
                } 
                else
                {
                    MACRxPacket.SourcePANID.v[0] = RxBuffer[14];
                    MACRxPacket.SourcePANID.v[1] = RxBuffer[15];
                    MACRxPacket.SourceAddress = &(RxBuffer[16]);
                    MACRxPacket.PayloadLen = RxBuffer[0] - 27;
                    MACRxPacket.Payload = &(RxBuffer[24]);
                }
                break;
            
            case 0x80:      // short source only. used in beacon
                {
                    MACRxPacket.flags.bits.broadcast = 1;
                    MACRxPacket.flags.bits.sourcePrsnt = 1;
                    MACRxPacket.altSourceAddress = true;
                    MACRxPacket.SourcePANID.v[0] = RxBuffer[4];
                    MACRxPacket.SourcePANID.v[1] = RxBuffer[5];
                    MACRxPacket.SourceAddress = &(RxBuffer[6]);
                    MACRxPacket.PayloadLen = RxBuffer[0] - 11;
                    MACRxPacket.Payload = &(RxBuffer[8]);
                }
                break;
                    
            case 0x88: // short dest, short source
                {
                    if( RxBuffer[6] == 0xFF && RxBuffer[7] == 0xFF )
                    {
                        MACRxPacket.flags.bits.broadcast = 1;
                    }
                    MACRxPacket.flags.bits.sourcePrsnt = 1;
                    MACRxPacket.altSourceAddress = true;
                    if( bIntraPAN == false )
                    {
                        MACRxPacket.SourcePANID.v[0] = RxBuffer[8];
                        MACRxPacket.SourcePANID.v[1] = RxBuffer[9];
                        MACRxPacket.SourceAddress = &(RxBuffer[10]);
                        MACRxPacket.PayloadLen = RxBuffer[0] - 15;
                        MACRxPacket.Payload = &(RxBuffer[12]);
                    }
                    else
                    {
                        MACRxPacket.SourcePANID.v[0] = RxBuffer[4];
                        MACRxPacket.SourcePANID.v[1] = RxBuffer[5];
                        MACRxPacket.SourceAddress = &(RxBuffer[8]);
                        MACRxPacket.PayloadLen = RxBuffer[0] - 13;
                        MACRxPacket.Payload = &(RxBuffer[10]);
                    }     
                }
                break;
            
            
            case 0x08: //dest-short, source-none
                {
                    if( RxBuffer[6] == 0xFF && RxBuffer[7] == 0xFF )
                    {
                        MACRxPacket.flags.bits.broadcast = 1;
                    }
                    MACRxPacket.PayloadLen = RxBuffer[0] - 11;
                    MACRxPacket.Payload = &(RxBuffer[8]);
                }
                break;

            case 0x0C:  // dest-long, source-none
                {
                    MACRxPacket.PayloadLen = RxBuffer[0] - 17;
                    MACRxPacket.Payload = &(RxBuffer[14]);
                }
                break;
            
            case 0x8C: //long dest, short source
                {
                    if( bIntraPAN ) // check if it is intraPAN
                    {
                        
                        MACRxPacket.SourcePANID.v[0] = RxBuffer[4];
                        MACRxPacket.SourcePANID.v[1] = RxBuffer[5];
                        
                        MACRxPacket.SourceAddress = &(RxBuffer[14]);
                    
                        MACRxPacket.PayloadLen = RxBuffer[0] - 19;
                        MACRxPacket.Payload = &(RxBuffer[16]);   
                    }
                    else
                    {
                        MACRxPacket.SourcePANID.v[0] = RxBuffer[14];
                        MACRxPacket.SourcePANID.v[1] = RxBuffer[15];
                        MACRxPacket.SourceAddress = &(RxBuffer[16]);
                        MACRxPacket.PayloadLen = RxBuffer[0] - 21;
                        MACRxPacket.Payload = &(RxBuffer[18]);
                    }
                    MACRxPacket.altSourceAddress = true;
                }
                break;
                    
            default:
                // not valid addressing mode or no addressing info
                MiMAC_DiscardPacket();
                return false;
        }  
        

        #if defined(ENABLE_SECURITY)

            if( RxBuffer[1] & 0x08 )
            {
                API_UINT32_UNION FrameCounter;
                uint8_t i;

                if( addrMode < 0xC0 )
                {
                    MiMAC_DiscardPacket();
                    return false;
                }

                FrameCounter.v[0] = MACRxPacket.Payload[0];
                FrameCounter.v[1] = MACRxPacket.Payload[1];
                FrameCounter.v[2] = MACRxPacket.Payload[2];
                FrameCounter.v[3] = MACRxPacket.Payload[3];

                for(i = 0; i < CONNECTION_SIZE; i++)
                {
                    if( (ConnectionTable[i].status.bits.isValid) &&
                        isSameAddress(ConnectionTable[i].Address, MACRxPacket.SourceAddress) )
                    {
                        break;
                    }
                }

                if( i < CONNECTION_SIZE )
                {
                    if( IncomingFrameCounter[i].Val > FrameCounter.Val )
                    {
                        MiMAC_DiscardPacket();
                        return false;
                    }
                    else
                    {
                        IncomingFrameCounter[i].Val = FrameCounter.Val;
                    }
                }


                MACRxPacket.PayloadLen -= 5;
                if( false == DataDecrypt(&(MACRxPacket.Payload[5]), &(MACRxPacket.PayloadLen), MACRxPacket.SourceAddress, FrameCounter, RxBuffer[1]) )
                {
                    MiMAC_DiscardPacket();
                    return false;
                }

                // remove the security header from the payload
                MACRxPacket.Payload = &(MACRxPacket.Payload[5]);
                MACRxPacket.flags.bits.secEn = 1;



            }

        #else
            if( RxBuffer[1] & 0x08 )
            {
                MiMAC_DiscardPacket();
                return false;
            }
        #endif


        // check the frame type. Only the data and command frame type
        // are supported. Acknowledgement frame type is handled in 
        // MRF24XA transceiver hardware.
        switch( RxBuffer[1] & 0x07 ) // check frame type
        {
            case 0x01:  // data frame
                MACRxPacket.flags.bits.packetType = PACKET_TYPE_DATA;
                break;
            case 0x03:  // command frame
                MACRxPacket.flags.bits.packetType = PACKET_TYPE_COMMAND;
                break;
            case 0x00:
                // use reserved packet type to represent beacon packet
                MACRxPacket.flags.bits.packetType = PACKET_TYPE_RESERVE;
                break;
            default:    // not support frame type
                MiMAC_DiscardPacket();
                return false;
        }
        MACRxPacket.LQIValue = RxBuffer[RxBuffer[0]-1];
        MACRxPacket.RSSIValue = RxBuffer[RxBuffer[0]];

        return true;
    }
    return false;
}


 
void MiMAC_DiscardPacket(void)
{
    //re-enable the ACKS
    MRF24XAStatus.bits.RX_BUFFERED = 0;
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
 *      A boolean to indicate if a packet has been transmitted by the RF transceiver.
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
    uint8_t regVal;
    uint8_t headerLength;
    uint16_t loc = 0x0200;
    uint8_t i;
    uint8_t frameControl = 0;
    bool IntraPAN;
    MIWI_TICK t1, t2;
    //uint8_t tmpRFIE;

    if( transParam.flags.bits.broadcast )
    {
        transParam.altDestAddr = true;
    }
    
    if( transParam.flags.bits.secEn )
    {
        transParam.flags.bits.sourcePrsnt = 1;
        transParam.altSrcAddr = false;
    }  
    
    // disable receiving
    regVal = PHYGetShortRAMAddr(0x15);
    PHYSetShortRAMAddr(0x15, (0x7F & regVal));

    // set data rate
    //PHYSetShortRAMAddr(0x12, 0x03);
        
    // set the frame control in variable i    
    if( transParam.flags.bits.packetType == PACKET_TYPE_COMMAND )
    {
        frameControl = 0x03;
    }
    else if( transParam.flags.bits.packetType == PACKET_TYPE_DATA)
    {
        frameControl = 0x01;
    }
    

    // decide the header length for different addressing mode
    if( ((transParam.DestPANID.Val == MAC_PANID.Val) /*&& (MAC_PANID.Val != 0xFFFF)*/) || 
        (transParam.flags.bits.sourcePrsnt == 0) ) // this is intraPAN
    {
        headerLength = 5;
        if( transParam.flags.bits.sourcePrsnt )
        {
            frameControl |= 0x40;
        }
        IntraPAN = true;
    }
    else
    {
        headerLength = 7;
        IntraPAN = false;
    }
    
    if( transParam.altDestAddr )
    {
        headerLength += 2;
    }
    else
    {
        headerLength += 8;
    }
    
    if( transParam.flags.bits.sourcePrsnt )
    {
        if( transParam.altSrcAddr )
        {
            headerLength += 2;
        }
        else
        {
            headerLength += 8;
        }
    }
    
    if( transParam.flags.bits.ackReq && transParam.flags.bits.broadcast == false )
    {
        frameControl |= 0x20;
    }
    
    // use PACKET_TYPE_RESERVE to represent beacon. Fixed format for beacon packet
    if( transParam.flags.bits.packetType == PACKET_TYPE_RESERVE)
    {
        frameControl = 0x00;
        headerLength = 7;
        IntraPAN = false;
        transParam.altSrcAddr = true;
        transParam.flags.bits.ackReq = false;
    }

    #ifdef ENABLE_SECURITY
        if( transParam.flags.bits.secEn )
        {
            frameControl |= 0x08;
            DataEncrypt(MACPayload, &MACPayloadLen, OutgoingFrameCounter, frameControl);
            headerLength += 5;
        }
    #endif


    // set packet length
    PHYSetLongRAMAddr(loc++, headerLength + MACPayloadLen);

    // set frame control LSB
    PHYSetLongRAMAddr(loc++, frameControl);
    
    // set frame control MSB
    if( transParam.flags.bits.packetType == PACKET_TYPE_RESERVE )
    {
        PHYSetLongRAMAddr(loc++, 0x80);
        // sequence number
        PHYSetLongRAMAddr(loc++, IEEESeqNum++);
    }
    else 
    {
        if( transParam.altDestAddr && transParam.altSrcAddr )
        {
            if( transParam.flags.bits.sourcePrsnt )
            {
                PHYSetLongRAMAddr(loc++, 0x88);
            }
            else
            {
                PHYSetLongRAMAddr(loc++, 0x08);
            }
        }
        else if( transParam.altDestAddr && transParam.altSrcAddr == 0 )
        {
            if( transParam.flags.bits.sourcePrsnt )
            {
                PHYSetLongRAMAddr(loc++, 0xC8);
            }
            else
            {
                PHYSetLongRAMAddr(loc++, 0x08);
            }
        }
        else if( transParam.altDestAddr == 0 && transParam.altSrcAddr == 1 )
        {
            if( transParam.flags.bits.sourcePrsnt )
            {
                PHYSetLongRAMAddr(loc++, 0x8C);
            }
            else
            {
                PHYSetLongRAMAddr(loc++, 0x0C);
            }
        }
        else
        {
            if( transParam.flags.bits.sourcePrsnt )
            {
                PHYSetLongRAMAddr(loc++, 0xCC);
            }
            else
            {
                PHYSetLongRAMAddr(loc++, 0x0C);
            }
        }
        
        // sequence number
        PHYSetLongRAMAddr(loc++, IEEESeqNum++);
        
        // destination PANID     
        PHYSetLongRAMAddr(loc++, transParam.DestPANID.v[0]);
        PHYSetLongRAMAddr(loc++, transParam.DestPANID.v[1]);
        
        // destination address
        if( transParam.flags.bits.broadcast )
        {
            PHYSetLongRAMAddr(loc++, 0xFF);
            PHYSetLongRAMAddr(loc++, 0xFF);
        }
        else
        {
            if( transParam.altDestAddr )
            {
                PHYSetLongRAMAddr(loc++, transParam.DestAddress[0]);
                PHYSetLongRAMAddr(loc++, transParam.DestAddress[1]);
            }
            else
            {
                for(i = 0; i < 8; i++)
                {
                    PHYSetLongRAMAddr(loc++, transParam.DestAddress[i]);
                }
            }
        }
    }
    
    // source PANID if necessary
    if( IntraPAN == false && transParam.flags.bits.sourcePrsnt )
    {
        PHYSetLongRAMAddr(loc++, MAC_PANID.v[0]);
        PHYSetLongRAMAddr(loc++, MAC_PANID.v[1]);
    }

    
    // source address
    if( transParam.flags.bits.sourcePrsnt )
    {
        if( transParam.altSrcAddr )
        {
            PHYSetLongRAMAddr(loc++, myNetworkAddress.v[0]);
            PHYSetLongRAMAddr(loc++, myNetworkAddress.v[1]);
        }
        else
        {
            for(i = 0; i < 8; i++)
            {
                PHYSetLongRAMAddr(loc++, MACInitParams.PAddress[i]);
            }
        }
    }

    #ifdef ENABLE_SECURITY
        if( transParam.flags.bits.secEn )
        {
            // fill the additional security aux header
            for(i = 0; i < 4; i++)
            {
                PHYSetLongRAMAddr(loc++, OutgoingFrameCounter.v[i]);
            }
            OutgoingFrameCounter.Val++;
            #if defined(ENABLE_NETWORK_FREEZER)
                if( (OutgoingFrameCounter.v[0] == 0) && ((OutgoingFrameCounter.v[1] & 0x03) == 0) )
                {
                    nvmPutOutFrameCounter(OutgoingFrameCounter.v);
                }
            #endif

            PHYSetLongRAMAddr(loc++, myKeySequenceNumber);

        }
    #endif

    PHYSetLongRAMAddrBloc(loc, MACPayload, MACPayloadLen);

    PHYSetShortRAMAddr(0x14, 0x30);
    INTflags.bytes.TXStatus = 0;
    // enable receiving
    regVal = PHYGetShortRAMAddr(0x15);
    PHYSetShortRAMAddr(0x15, (regVal | 0x80));
    // transmit
    regVal = PHYGetShortRAMAddr(0x12);
    PHYSetShortRAMAddr(0x12, (0x88 | regVal));

    t1 = MiWi_TickGet();
    while(  INTflags.bits.TXIF == 0 && INTflags.bits.TXACKIF == 0 &&
            INTflags.bits.TXCSMAIF == 0 )
    {
        if( RF_INT_PIN == 0 )
        {
            RFIF = 1;
        }
        
        t2 = MiWi_TickGet();
        if( MiWi_TickGetDiff(t2, t1) > FIFTY_MILI_SECOND )
        {
            PHYSetShortRAMAddr(0x12, 0x03);
            return false;
        }        
    }    
    

    PHYSetShortRAMAddr(0x12, 0x03);
    if( INTflags.bits.TXACKIF || INTflags.bits.TXCSMAIF )
    {
        return false;
    } 
    
    return true;
    
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
    uint8_t MiMAC_ChannelAssessment(INPUT uint8_t AssessmentMode)
    {
        uint8_t RSSIcheck;
        uint8_t regVal;
        MIWI_TICK t1, t2;

        PHYSetShortRAMAddr(0x31, 0x1E);
        regVal = PHYGetShortRAMAddr(0x31);
        t1 = MiWi_TickGet();
        while( regVal & 0x10 )
        {
            regVal = PHYGetShortRAMAddr(0x31);
            t2 = MiWi_TickGet();
            if( MiWi_TickGetDiff(t2, t1) > TEN_MILI_SECOND )
            {
                break;
            }

        }
        RSSIcheck = PHYGetShortRAMAddr(0x32);
        return RSSIcheck;
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
        uint8_t i;

        switch(PowerState)
        {
            case POWER_STATE_DEEP_SLEEP:
                {
                    if( MRF24XAStatus.bits.SLEEP == 0 )
                    {
                        PHYSetShortRAMAddr(0x03, 0x04);
                        MRF24XAStatus.bits.SLEEP = 1;
                    }
                }
                break;

            case POWER_STATE_OPERATE:
                {
                    uint8_t regVal;

                    if( MRF24XAStatus.bits.SLEEP )
                    {
                        PHYGetShortRAMAddr(0x55);
                        while( MRF24XAStatus.bits.SLEEP ) ;
                    }
                }
                break;

            default:
                return false;
        }

        return true;
    }
#endif


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
    uint8_t regVal;
    
    if( channel < 11 || channel > 26)
    {
        return false;
    }
    
    if( channel != MACCurrentChannel )
    {
        regVal = PHYGetShortRAMAddr(0x11);
        PHYSetShortRAMAddr(0x11, (regVal & 0x0F) | ((channel-11) << 4) );    
        MACCurrentChannel = channel;
    }    
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
    
    if( outputPower == 0 )
    {
        PHYSetShortRAMAddr(0x3A, 0x15);         // maximum power
    }    
    else
    {
        PHYSetShortRAMAddr(0x3A, 0x03);         // -15dB
    }    
    
    return true;
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
    myNetworkAddress.v[0] = Address[0];
    myNetworkAddress.v[1] = Address[1];
    MAC_PANID.v[0] = PANID[0];
    MAC_PANID.v[1] = PANID[1];
    
    
    // set short address
    PHYSetShortRAMAddr(0x27, myNetworkAddress.v[0]);
    PHYSetShortRAMAddr(0x28, myNetworkAddress.v[1]);
    
    // set PANID
    PHYSetShortRAMAddr(0x29, MAC_PANID.v[0]);
    PHYSetShortRAMAddr(0x2A, MAC_PANID.v[1]);
    
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
    MACInitParams = initValue;

    IEEESeqNum = TMRL;
    
    MACCurrentChannel = 11;
       
    InitMRF24XA();
    
    MRF24XAStatus.Val = 0;
    
    return true;
}




/*********************************************************************
 * void HighISR(void)
 *
 * Overview:        This is the interrupt handler for the MRF24XA and
 *                  P2P stack. 
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    Various flags and registers set.
 *
 ********************************************************************/
#if defined( Software_ACK )
    uint8_t AckFrame[4] = {0x03, 0x02, 0x00, 0x00};
#endif

#if defined(__XC8)
    void interrupt high_isr(void)
#elif defined(__XC16)
    void _ISRFAST __attribute__((interrupt, auto_psv)) _INT1Interrupt(void)
#elif defined(__XC32)
    #pragma interrupt _RxInterrupt ipl4 vector 16
    void _RxInterrupt()
#endif
{

    
    if(RFIE && RFIF )
    {  

        
        RFIF = 0;

        
        //PHYGetShortRAMAddrBloc(0x04, INTflags.v, 4);
        #if SYS_CLK_FrequencySystemGet() > 40000000
            INTflags.v[2] = PHYGetLongRAMAddr(0x06);
        #else

            PHY_CS = 0;
            SPIPut(0x80);
            SPIPut(0xC0);
            INTflags.v[2] = SPIGet();
            PHY_CS = 1;
        #endif

        if( INTflags.bits.RXIF )
        {
            if( MRF24XAStatus.bits.RX_BUFFERED == 0 )
            {
                #if SYS_CLK_FrequencySystemGet() > 40000000
                    PHYGetLongRAMAddrBloc(0x0300, &(RxBuffer[0]), 4);
                #else
                    PHY_CS = 0;
                    SPIPut(0xE0);
                    SPIPut(0x00);
                    RxBuffer[0] = SPIGet();
                    RxBuffer[1] = SPIGet();
                    RxBuffer[2] = SPIGet();
                    RxBuffer[3] = SPIGet();
                    PHY_CS = 1;
                #endif


                
                RxBuffer[0] += 2;
                if( RxBuffer[0] < RX_PACKET_SIZE )
                {
                    #if defined( Software_ACK )
                    {
                        if( (RxBuffer[1] & 0x20) > 0 )
                        {
                            if( (((RxBuffer[2] & 0x30)>>4) < 2) && ((RxBuffer[2] & 0x0C) != 0) )
                            {

                                AckFrame[3] = RxBuffer[3];

                                #if SYS_CLK_FrequencySystemGet() == 8000000
                                     #warning ACK time is ~315 us
                                #endif

                                #if SYS_CLK_FrequencySystemGet() == 10000000
                                    #warning ACK time is ~255 us
                                #endif

                                #if SYS_CLK_FrequencySystemGet() == 16000000
                                     Delay10TCYx(11);
                                #endif

                                #if SYS_CLK_FrequencySystemGet() == 32000000
                                     Delay10TCYx(83);
                                #endif

                                #if SYS_CLK_FrequencySystemGet() == 40000000
                                     Delay10TCYx(160);
                                #endif

                                #if SYS_CLK_FrequencySystemGet() > 40000000
                                    PHYSetLongRAMAddrBloc(0x0200, AckFrame, 4);
                                    //Delay10TCYx(10*mMIPS);
                                    //delay_ms(1);
                                #else
                                    PHY_CS = 0;
                                    SPIPut(0xC0);
                                    SPIPut(0x10);
                                    SPIPut(AckFrame[0]);
                                    SPIPut(AckFrame[1]);
                                    SPIPut(AckFrame[2]);
                                    SPIPut(AckFrame[3]);
                                    PHY_CS = 1;

                                #endif

                                
                                // TODO: handle CSMA settings
                                #if SYS_CLK_FrequencySystemGet() > 40000000
                                    PHYSetShortRAMAddr(0x12, 0x83);
                                #else
                                    PHY_CS = 0;
                                    SPIPut(0x25);
                                    SPIPut(0x83);
                                    PHY_CS = 1;
                                #endif

                                // Need to adjust according to MCU MIPS. Delay 40us ( us/10*mMIPS)
                                // Delay10TCYx can only accept input parameter <= 255
                                #if SYS_CLK_FrequencySystemGet() == 8000000
                                     Delay10TCYx(8);
                                #endif

                                #if SYS_CLK_FrequencySystemGet() == 10000000
                                     Delay10TCYx(14);
                                #endif

                                #if SYS_CLK_FrequencySystemGet() == 16000000
                                     Delay10TCYx(16);
                                #endif

                                #if SYS_CLK_FrequencySystemGet() == 32000000
                                     Delay10TCYx(32);
                                #endif

                                #if SYS_CLK_FrequencySystemGet() == 40000000
                                     Delay10TCYx(250);
                                #endif

                                
                                regVal = PHYGetLongRAMAddr(0x05);
                                while( (regVal & 0x80) == 0 )
                                {
                                    regVal = PHYGetLongRAMAddr(0x05);
                                }
                                
                            }
                        }

                        
                        if( (RxBuffer[1] & 0x07) == 0x02 )
                        {
                            #if defined( Software_ACK )
                            {
                                PHYSetShortRAMAddr(0x16, 0x00);
                            }
                            #else
                            {
                                PHYSetShortRAMAddr(0x16, 0x44);
                            }
                            #endif
                            return;
                        }

                        PHYGetLongRAMAddrBloc(0x0304, &(RxBuffer[4]), RxBuffer[0]-3);
                    }
                    #else
                    {
                        PHYGetLongRAMAddrBloc(0x0304, &(RxBuffer[4]), RxBuffer[0]);
                    }
                    #endif

                    MRF24XAStatus.bits.RX_BUFFERED = 1;
                }

                #if defined( Software_ACK )
                {
                    PHYSetShortRAMAddr(0x16, 0x00);
                }
                #else
                {
                    PHYSetShortRAMAddr(0x16, 0x44);
                }
                #endif

            }
            else
            {
                #if !defined( Software_ACK )
                {
                    MRF24XAStatus.bits.RADIO_BUFFERED = 1;
                }
                #else
                {
                    PHYSetShortRAMAddr(0x16, 0x00);
                }
                #endif
            } 
        }


        INTflags.v[0] = PHYGetLongRAMAddr(0x04);
        INTflags.v[1] = PHYGetLongRAMAddr(0x05);
        INTflags.v[3] = PHYGetLongRAMAddr(0x07);

        if( INTflags.bits.RDYIF )
        {
            MRF24XAStatus.bits.SLEEP = 0;
        }
               
    } //end of if(RFIE && RFIF)

       
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

        /*
        if(INTCONbits.RBIE && INTCONbits.RBIF)
        {
            Read = PORTB;
            INTCONbits.RBIF = 0;
        }
        
        if( INTCON3bits.INT2IE && INTCON3bits.INT2IF )
        {
            INTCON3bits.INT2IF = 0;
        }
        
        if( INTCONbits.INT0IE && INTCONbits.INT0IF )
        {
            INTCONbits.INT0IF = 0;
        }       
        
        if( INTCON3bits.INT1IE && INTCON3bits.INT1IF )
        {
            INTCON3bits.INT1IF = 0;
        }
        */
    #endif

    return;
    
} //end of interrupt handler



#if defined(ENABLE_SECURITY)

const uint8_t SecurityLevelConvertTable[8] = { 0, 7, 6, 5, 1, 4, 3, 2};


/************************************************************************************
 * Function:
 *      bool DataDecrypt( uint8_t *key, uint8_t *Payload, uint8_t *PayloadLen,
 *                        uint8_t *SourceIEEEAddress, API_UINT32_UNION FrameCounter,
 *                        uint8_t FrameControl )
 *
 * Summary:
 *      This function decrypt received secured frame
 *
 * Description:        
 *      This is the function to decrypt the secured packet. All parameters are
 *      input information used in the decryption process. After decryption is 
 *      performed successfully, the result will be put into the buffer that is 
 *      pointed by input parameter "Payload" and the parameter "PayloadLen" will
 *      also be updated.
 *
 * PreCondition:    
 *      Transceiver initialization has been done. 
 *
 * Parameters: 
 *      uint8_t * Key          - Security Key used to decrypt packet
 *      uint8_t * Payload      - Pointer to the the input secured payload and output
 *                            decrypted payload
 *      uint8_t * PayloadLen   - Pointer to the length of input secured payload and
 *                            output decrypted payload
 *      uint8_t * SourceIEEEAddress    - The IEEE address of the package originator
 *      API_UINT32_UNION FrameCounter      - Frame counter of the received packet
 *      uint8_t FrameControl   - The frame control byte of the received packet
 *
 * Returns: 
 *      A boolean to indicates if decryption is successful.
 *
 * Example:
 *      <code>
 *      DataDecrypt(key, payload, &payloadLen, SourceIEEEAddr, FrameCounter, FrameControl);
 *      </code>
 *
 * Remarks:    
 *      None
 *
 *****************************************************************************************/ 
bool DataDecrypt( uint8_t *Payload, uint8_t *PayloadLen, uint8_t *SourceIEEEAddress,
                API_UINT32_UNION FrameCounter, uint8_t FrameControl)
{   
    uint8_t i;
    uint8_t regVal;
    uint8_t len;
    uint8_t tmpRFIE;
    MIWI_TICK t1, t2;
    
    // disable receiving 
    regVal = PHYGetShortRAMAddr(0x15);
    PHYSetShortRAMAddr(0x15, (regVal & 0x7F));
        
    // set security suite
    regVal = PHYGetShortRAMAddr(0x11);
    PHYSetShortRAMAddr(0x11, (regVal & 0xF0) | 5 );

    tmpRFIE = RFIE;
    RFIE = 0;
    PHY_CS = 0;
    SPIPut(LONGADDRESSINGMODE_PREFIX | (char)(0x0300 >> 3));
    SPIPut(LONGADDRESSINGMODE_WRITEPREFIX | (char)(0x0300<<5)); 
    SPIPut((*PayloadLen)+15);
    SPIPut(0);
    SPIPut(0);
    
    // fill the header / "a" content
    SPIPut(FrameControl);
    SPIPut(FrameCounter.v[0]);
    SPIPut(FrameCounter.v[1]);
    SPIPut(FrameCounter.v[2]);
    SPIPut(FrameCounter.v[3]);
    for(i = 0; i < 8; i++)
    {
        SPIPut( SourceIEEEAddress[i] );
    }
    
    // fill the payload / "m" contents
    for(i = 0; i < *PayloadLen; i++)
    {
        SPIPut(Payload[i]);
    }        
    PHY_CS = 1;
    RFIE = tmpRFIE;
  
    // set nounce
    tmpRFIE = RFIE;
    RFIE = 0;
    PHY_CS = 0;
    SPIPut(LONGADDRESSINGMODE_PREFIX | (char)(0x0050 >> 3));
    SPIPut(LONGADDRESSINGMODE_WRITEPREFIX | (char)(0x0050<<5));
    SPIPut(SecurityLevelConvertTable[SECURITY_LEVEL]);
    SPIPut(FrameCounter.v[3]);
    SPIPut(FrameCounter.v[2]);
    SPIPut(FrameCounter.v[1]);
    SPIPut(FrameCounter.v[0]);
    SPIPut(SourceIEEEAddress[7]);
    SPIPut(SourceIEEEAddress[6]);
    SPIPut(SourceIEEEAddress[5]);
    SPIPut(SourceIEEEAddress[4]);
    SPIPut(SourceIEEEAddress[3]);
    SPIPut(SourceIEEEAddress[2]);
    SPIPut(SourceIEEEAddress[1]);
    SPIPut(SourceIEEEAddress[0]);
    PHY_CS = 1;
    RFIE = tmpRFIE;

    
    // fill the key
    tmpRFIE = RFIE;
    RFIE = 0;
    PHY_CS = 0;
    SPIPut(LONGADDRESSINGMODE_PREFIX | (char)(0x0040 >> 3));
    SPIPut(LONGADDRESSINGMODE_WRITEPREFIX | (char)(0x0040<<5));
    for(i = 0; i < 16; i++)
    {
        SPIPut((uint8_t)mySecurityKey[15-i]);
    }    
    PHY_CS = 1;
    RFIE = tmpRFIE;

    // set header index
    PHYSetShortRAMAddr(0x2B, 3);
    
    // set payload index
    PHYSetShortRAMAddr(0x2C, 16);    
    
    // set end index
    PHYSetShortRAMAddr(0x2D, 15+(*PayloadLen));
    

    INTflags.bytes.RXStatus = 0;

    regVal = PHYGetShortRAMAddr(0x15);
    PHYSetShortRAMAddr(0x15, (regVal | 0xA0));
    
    // clear filter
    regVal = PHYGetShortRAMAddr(0x18);
    PHYSetShortRAMAddr(0x18, 0x00);

    // RX PROCESS
    regVal = PHYGetShortRAMAddr(0x2E);
    PHYSetShortRAMAddr(0x2E, (regVal | 0x10));

    t1 = MiWi_TickGet();
    while( INTflags.bits.RXDECIF == 0 && INTflags.bits.RXTAGIF == 0)
    {
        if( RF_INT_PIN == 0 )
        {
            RFIF = 1;
        }  
        
        t2 = MiWi_TickGet();
        if( MiWi_TickGetDiff(t2, t1) > TEN_MILI_SECOND )
        {
            // reset mac
            PHYSetShortRAMAddr(0x01, 0x08);
            PHYSetShortRAMAddr(0x01, 0x00);
        }      
    } 
    
    // restore filter
    PHYSetShortRAMAddr(0x18, 0x45);

    len = PHYGetLongRAMAddr(0x0300)+1;
    for(i = 16; i < len; i++)
    {
        Payload[i-16] = PHYGetLongRAMAddr(0x0300+i);  
    } 
    
    // clear nwk security
    regVal = PHYGetShortRAMAddr(0x10);
    PHYSetShortRAMAddr(0x10, regVal & 0xFE);

    // set security suite
    regVal = PHYGetShortRAMAddr(0x11);
    PHYSetShortRAMAddr(0x11, (regVal & 0xF0) );
    
    (*PayloadLen) -= 4;  
    
    if( INTflags.bits.RXTAGIF )
    {
        // reset mac
        PHYSetShortRAMAddr(0x01, 0x08);
        PHYSetShortRAMAddr(0x01, 0x00);
        return false;
    }    
    
    // reset mac
    PHYSetShortRAMAddr(0x01, 0x08);
    PHYSetShortRAMAddr(0x01, 0x00);
 
    return true;
}                    



/************************************************************************************
 * Function:
 *      bool DataEncrypt( uint8_t *key, uint8_t *Payload, uint8_t *PayloadLen,
 *                        API_UINT32_UNION FrameCounter, uint8_t FrameControl )
 *
 * Summary:
 *      This function decrypt received secured frame
 *
 * Description:        
 *      This is the function to encrypt the transmitting packet. All parameters are
 *      input information used in the encryption process. After encryption is 
 *      performed successfully, the result will be put into the buffer that is 
 *      pointed by input parameter "Payload" and the parameter "PayloadLen" will
 *      also be updated.
 *
 * PreCondition:    
 *      Transceiver initialization has been done. 
 *
 * Parameters: 
 *      uint8_t * Key          - Security Key used to decrypt packet
 *      uint8_t * Payload      - Pointer to the the input plain payload and output
 *                            encrypted payload
 *      uint8_t * PayloadLen   - Pointer to the length of input plain payload and
 *                            output encrypted payload
 *      uint8_t * DestIEEEAddress      - The IEEE address of the packet target
 *      API_UINT32_UNION FrameCounter      - Frame counter of the transmitting packet
 *      uint8_t FrameControl   - The frame control byte of the transmitting packet
 *
 * Returns: 
 *      A boolean to indicates if encryption is successful.
 *
 * Example:
 *      <code>
 *      DataEncrypt(key, payload, &payloadLen, FrameCounter, FrameControl);
 *      </code>
 *
 * Remarks:    
 *      None
 *
 *****************************************************************************************/ 
bool DataEncrypt(uint8_t *Payload, uint8_t *PayloadLen, API_UINT32_UNION FrameCounter,
                uint8_t FrameControl)
{
    uint16_t loc;
    uint8_t regVal;
    uint8_t i;
    uint8_t tmpRFIE;
    
    // disable receiving
    regVal = PHYGetShortRAMAddr(0x15);
    PHYSetShortRAMAddr(0x15, (regVal & 0x7F));

    // set security suite
    regVal = PHYGetShortRAMAddr(0x0011);
    PHYSetShortRAMAddr(0x11, (regVal & 0xF0) | 5 );

    tmpRFIE = RFIE;
    RFIE = 0;
    
    PHY_CS = 0;
    SPIPut(LONGADDRESSINGMODE_PREFIX | (char)(0x0200 >> 3));
    SPIPut(LONGADDRESSINGMODE_WRITEPREFIX | (char)(0x0200<<5));
    
    SPIPut((*PayloadLen)+13);
    
    // fill the header / "a" contents
    SPIPut(FrameControl);
    SPIPut(FrameCounter.v[0]);
    SPIPut(FrameCounter.v[1]);
    SPIPut(FrameCounter.v[2]);
    SPIPut(FrameCounter.v[3]);
    for(i = 0; i < 8; i++)
    {
        SPIPut(MACInitParams.PAddress[i]);
    }    
    
    // fill the payload
    for(i = 0; i < *PayloadLen; i++)
    {
        SPIPut(Payload[i]);
    }
    
    PHY_CS = 1;
    RFIE = tmpRFIE;
    
    // set nounce
    tmpRFIE = RFIE;
    RFIE = 0;
    
    PHY_CS = 0;
    SPIPut(LONGADDRESSINGMODE_PREFIX | (char)(0x0050 >> 3));
    SPIPut(LONGADDRESSINGMODE_WRITEPREFIX | (char)(0x0050<<5));
    
    SPIPut(SecurityLevelConvertTable[SECURITY_LEVEL]);
    SPIPut(FrameCounter.v[3]);
    SPIPut(FrameCounter.v[2]);
    SPIPut(FrameCounter.v[1]);
    SPIPut(FrameCounter.v[0]);
    SPIPut(MACInitParams.PAddress[7]);
    SPIPut(MACInitParams.PAddress[6]);
    SPIPut(MACInitParams.PAddress[5]);
    SPIPut(MACInitParams.PAddress[4]);
    SPIPut(MACInitParams.PAddress[3]);
    SPIPut(MACInitParams.PAddress[2]);
    SPIPut(MACInitParams.PAddress[1]);
    SPIPut(MACInitParams.PAddress[0]);
    PHY_CS = 1;
    RFIE = tmpRFIE;

    // fill the key
    tmpRFIE = RFIE;
    RFIE = 0;
    
    PHY_CS = 0;
    SPIPut(LONGADDRESSINGMODE_PREFIX | (char)(0x0040 >> 3));
    SPIPut(LONGADDRESSINGMODE_WRITEPREFIX | (char)(0x0040<<5));
    for(i = 0; i < 16; i++)
    {
        SPIPut((uint8_t)mySecurityKey[15-i]);
    }
    PHY_CS = 1;
    RFIE = tmpRFIE;

    // set header index
    PHYSetShortRAMAddr(0x2B, 1);
    
    // set payload index
    PHYSetShortRAMAddr(0x2C, 14);    
    
    // set end index
    PHYSetShortRAMAddr(0x2D, 13+(*PayloadLen));
    
    INTflags.bytes.TXStatus = 0;

    // trigger encryption
    regVal = PHYGetShortRAMAddr(0x12);
    PHYSetShortRAMAddr(0x12, regVal | 0x20);
  
    {
        MIWI_TICK t1, t2;
        t1 = MiWi_TickGet();
        while( INTflags.bytes.TXStatus == 0 )
        {
            if( RF_INT_PIN == 0 )
            {
                RFIF = 1;
            } 
            t2 = MiWi_TickGet();
            if( MiWi_TickGetDiff(t2, t1) > TWENTY_MILI_SECOND )
            {
                break;
            }    
        }    
    }    
    

        
    // clear nwk security
    regVal = PHYGetShortRAMAddr(0x10);
    PHYSetShortRAMAddr(0x10, regVal & 0xFE);

    // set security suite
    regVal = PHYGetShortRAMAddr(0x11);
    PHYSetShortRAMAddr(0x11, (regVal & 0xF0) );

    tmpRFIE = RFIE;
    RFIE = 0;
    PHY_CS = 0;
    SPIPut(LONGADDRESSINGMODE_PREFIX | (char)(0x020E >> 3));
    SPIPut((char)(0x020E<<5));
    for(i = 0; i < *PayloadLen; i++)
    {
        Payload[i] = SPIGet();
    }
    Payload[(*PayloadLen)] = SPIGet();
    Payload[(*PayloadLen)+1] = SPIGet();
    Payload[(*PayloadLen)+2] = SPIGet();
    Payload[(*PayloadLen)+3] = SPIGet();
    PHY_CS = 1;
    RFIE = tmpRFIE;
    
    *PayloadLen += 4;
    
    // reset mac
    PHYSetShortRAMAddr(0x01, 0x08);
    PHYSetShortRAMAddr(0x01, 0x00);

    
    return true;
}

#endif









