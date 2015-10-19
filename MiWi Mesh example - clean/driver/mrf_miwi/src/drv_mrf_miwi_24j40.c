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


#include "driver/mrf_miwi/drv_mrf_miwi_24j40.h"
#if defined(ENABLE_NVM)
#include "miwi/miwi_nvm.h"
#endif

/************************ VARIABLES ********************************/
MACINIT_PARAM MACInitParams;

#define MIPS    (SYS_CLK_FrequencyInstructionGet()/1000000)
#define FAILURE_COUNTER ((uint16_t)0x20 * MIPS)

#ifdef ENABLE_SECURITY
const char mySecurityKey[16] = {SECURITY_KEY_00, SECURITY_KEY_01, SECURITY_KEY_02, SECURITY_KEY_03, SECURITY_KEY_04,
    SECURITY_KEY_05, SECURITY_KEY_06, SECURITY_KEY_07, SECURITY_KEY_08, SECURITY_KEY_09, SECURITY_KEY_10, SECURITY_KEY_11,
    SECURITY_KEY_12, SECURITY_KEY_13, SECURITY_KEY_14, SECURITY_KEY_15};

const uint8_t myKeySequenceNumber = KEY_SEQUENCE_NUMBER; // The sequence number of security key. Used to identify the security key

API_UINT32_UNION OutgoingFrameCounter;

#include "miwi/miwi_api.h"
#endif	

struct
{
    uint8_t PayloadLen;
    uint8_t Payload[RX_PACKET_SIZE];
} RxBuffer[BANK_SIZE];



uint8_t BankIndex = 0xFF;

uint8_t IEEESeqNum;
volatile uint16_t failureCounter = 0;
uint8_t MACCurrentChannel;

API_UINT16_UNION MAC_PANID;
API_UINT16_UNION myNetworkAddress;

volatile MRF24J40_STATUS MRF24J40Status;

bool DataEncrypt(uint8_t *Payload, uint8_t *PayloadLen, API_UINT32_UNION FrameCounter, uint8_t FrameControl);
bool DataDecrypt(uint8_t *Payload, uint8_t *PayloadLen, uint8_t *SourceIEEEAddress, API_UINT32_UNION FrameCounter, uint8_t FrameControl);

/*********************************************************************
 * void PHYSetLongRAMAddr(INPUT uint16_t address, INPUT uint8_t value)
 *
 * Overview:        This function writes a value to a LONG RAM address
 *
 * PreCondition:    Communication port to the MRF24J40 initialized
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
void PHYSetLongRAMAddr(INPUT uint16_t address, INPUT uint8_t value)
{
    volatile uint8_t tmpRFIE = RFIE;

    RFIE = 0;
    PHY_CS = 0;
    SPIPut((((uint8_t) (address >> 3))&0x7F) | 0x80);
    SPIPut((((uint8_t) (address << 5))&0xE0) | 0x10);
    SPIPut(value);
    PHY_CS = 1;
    RFIE = tmpRFIE;
}

/*********************************************************************
 * void PHYSetShortRAMAddr(INPUT uint8_t address, INPUT uint8_t value)
 *
 * Overview:        This function writes a value to a short RAM address
 *
 * PreCondition:    Communication port to the MRF24J40 initialized
 *
 * Input:           address - the address of the short RAM address
 *                      that you want to write to.  Should use the
 *                      WRITE_ThisAddress definition in the MRF24J40
 *                      include file.
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
void PHYSetShortRAMAddr(INPUT uint8_t address, INPUT uint8_t value)
{
    volatile uint8_t tmpRFIE = RFIE;

    RFIE = 0;
    PHY_CS = 0;
    SPIPut(address);
    SPIPut(value);
    PHY_CS = 1;
    RFIE = tmpRFIE;
}

/*********************************************************************
 * uint8_t PHYGetShortRAMAddr(INPUT uint8_t address)
 *
 * Overview:        This function reads a value from a short RAM address
 *
 * PreCondition:    Communication port to the MRF24J40 initialized
 *
 * Input:           address - the address of the short RAM address
 *                      that you want to read from.  Should use the
 *                      READ_ThisAddress definition in the MRF24J40
 *                      include file.
 *
 * Output:          None
 *
 * Side Effects:    Interrupt from radio is turned off before accessing
 *                  the SPI and turned back on after accessing the SPI
 *
 ********************************************************************/
uint8_t PHYGetShortRAMAddr(INPUT uint8_t address)
{
    uint8_t toReturn;
    volatile uint8_t tmpRFIE = RFIE;

    RFIE = 0;
    PHY_CS = 0;
    SPIPut(address);
    toReturn = SPIGet();
    PHY_CS = 1;
    RFIE = tmpRFIE;

    return toReturn;
}

/*********************************************************************
 * uint8_t PHYGetLongRAMAddr(INPUT uint16_t address)
 *
 * Overview:        This function reads a value from a long RAM address
 *
 * PreCondition:    Communication port to the MRF24J40 initialized
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
    SPIPut(((address >> 3)&0x7F) | 0x80);
    SPIPut(((address << 5)&0xE0));
    toReturn = SPIGet();
    PHY_CS = 1;
    RFIE = tmpRFIE;

    return toReturn;
}

void InitMRF24J40(void)
{
    uint8_t i;
    uint16_t j;

    // first perform a hardware reset
    PHY_RESETn = 0;
    delay_ms(3);

    PHY_RESETn = 1;
    delay_ms(10);

    /* do a soft reset */
    PHYSetShortRAMAddr(WRITE_SOFTRST, 0x07);
    do
    {
        i = PHYGetShortRAMAddr(READ_SOFTRST);
    } while ((i & 0x07) != (uint8_t) 0x00);

    for (j = 0; j < (uint16_t) 1000; j++)
    {
    }

    /* flush the RX fifo */
    PHYSetShortRAMAddr(WRITE_RXFLUSH, 0x01);

    /* Program the short MAC Address, 0xffff */
    PHYSetShortRAMAddr(WRITE_SADRL, 0xFF);
    PHYSetShortRAMAddr(WRITE_SADRH, 0xFF);
    PHYSetShortRAMAddr(WRITE_PANIDL, 0xFF);
    PHYSetShortRAMAddr(WRITE_PANIDH, 0xFF);

    /* Program Long MAC Address*/
    for (i = 0; i < (uint8_t) 8; i++)
    {
        PHYSetShortRAMAddr(WRITE_EADR0 + i * 2, MACInitParams.PAddress[i]);
    }

    /* setup */
    PHYSetLongRAMAddr(RFCTRL2, 0x80);

#if defined(ENABLE_PA_LNA)
#if defined(MRF24J40MB)
    // There are special MRF24J40 transceiver output power
    // setting for Microchip MRF24J40MB module.
#if APPLICATION_SITE == EUROPE
    // MRF24J40 output power set to be -14.9dB
    PHYSetLongRAMAddr(RFCTRL3, 0x70);
#else
    // MRF24J40 output power set to be -1.9dB
    PHYSetLongRAMAddr(RFCTRL3, 0x18);
#endif
#elif defined(MRF24J40MC)
    // MRF24J40 output power set to be -3.7dB for MRF24J40MB
    PHYSetLongRAMAddr(RFCTRL3, 0x28);
#else
    // power level set to be 0dBm, must adjust according to
    // FCC/IC/ETSI requirement
    PHYSetLongRAMAddr(RFCTRL3, 0x00);
#endif
#else
    // power level to be 0dBm
    PHYSetLongRAMAddr(RFCTRL3, 0x00);
#endif

    /* program RSSI ADC with 2.5 MHz clock */
    PHYSetLongRAMAddr(RFCTRL6, 0x90);

    PHYSetLongRAMAddr(RFCTRL7, 0x80);

    PHYSetLongRAMAddr(RFCTRL8, 0x10);

    PHYSetLongRAMAddr(SCLKDIV, 0x21);

    /* Program CCA mode using RSSI */
    PHYSetShortRAMAddr(WRITE_BBREG2, 0x80);
    /* Enable the packet RSSI */
    PHYSetShortRAMAddr(WRITE_BBREG6, 0x40);
    /* Program CCA, RSSI threshold values */
    PHYSetShortRAMAddr(WRITE_RSSITHCCA, 0x60);

#if defined(ENABLE_PA_LNA)

#if defined(MRF24J40MC)
    PHYSetShortRAMAddr(WRITE_GPIODIR, 0x08);
    PHYSetShortRAMAddr(WRITE_GPIO, 0x08);
#endif
    PHYSetLongRAMAddr(TESTMODE, 0x0F);

#endif
    PHYSetShortRAMAddr(WRITE_FFOEN, 0x98);
    PHYSetShortRAMAddr(WRITE_TXPEMISP, 0x95);

    // wait until the MRF24J40 in receive mode
    do
    {
        i = PHYGetLongRAMAddr(RFSTATE);
    }
    while ((i&0xA0) != 0xA0);

    PHYSetShortRAMAddr(WRITE_INTMSK, 0xE6);

#ifdef ENABLE_INDIRECT_MESSAGE
    PHYSetShortRAMAddr(WRITE_ACKTMOUT, 0xB9);
#endif

    // Make RF communication stable under extreme temperatures
    PHYSetLongRAMAddr(RFCTRL0, 0x03);
    PHYSetLongRAMAddr(RFCTRL1, 0x02);

    MiMAC_SetChannel(MACCurrentChannel, 0);

    // Define TURBO_MODE if more bandwidth is required
    // to enable radio to operate to TX/RX maximum
    // 625Kbps
#ifdef TURBO_MODE

    PHYSetShortRAMAddr(WRITE_BBREG0, 0x01);
    PHYSetShortRAMAddr(WRITE_BBREG3, 0x38);
    PHYSetShortRAMAddr(WRITE_BBREG4, 0x5C);

    PHYSetShortRAMAddr(WRITE_RFCTL, 0x04);
    PHYSetShortRAMAddr(WRITE_RFCTL, 0x00);

#endif          

}

/************************************************************************************
 * Function:
 *      bool MiMAC_ReceivedPacket(void)
 *
 * Summary:
 *      This function check if a new packet has been received by the RF transceiver
 *
 * Description:
 *      This is the primary MiMAC interface for the protocol layer to
 *      check if a packet has been received by the RF transceiver. When a packet has
 *      been received, all information will be stored in the global variable
 *      MACRxPacket in the format of MAC_RECEIVED_PACKET;
 *
 * PreCondition:
 *      MiMAC initialization has been done.
 *
 * Parameters:
 *      None
 *
 * Returns:
 *      A boolean to indicate if a packet has been received by the RF transceiver.
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
bool MiMAC_ReceivedPacket(void)
{
    uint8_t i;

    //set the interrupt flag just in case the interrupt was missed
    if (RF_INT_PIN == 0)
    {
        RFIF = 1;
    }

    //If the stack TX has been busy for a long time then
    //time out the TX because we may have missed the interrupt
    //and don't want to lock up the stack forever
    if (MRF24J40Status.bits.TX_BUSY)
    {
        if (failureCounter >= FAILURE_COUNTER)
        {
            failureCounter = 0;
            MRF24J40Status.bits.TX_BUSY = 0;
        } else
        {
            failureCounter++;
        }
    }

    BankIndex = 0xFF;
    for (i = 0; i < BANK_SIZE; i++)
    {
        if (RxBuffer[i].PayloadLen > 0)
        {
            BankIndex = i;
            break;
        }
    }

    //if(MRF24J40Status.bits.RX_BUFFERED == 1)
    if (BankIndex < BANK_SIZE)
    {
        uint8_t addrMode;
#ifndef TARGET_SMALL
        bool bIntraPAN = true;

        if ((RxBuffer[BankIndex].Payload[0] & 0x40) == 0)
        {
            bIntraPAN = false;
        }
#endif
        MACRxPacket.flags.Val = 0;
        MACRxPacket.altSourceAddress = false;

        //Determine the start of the MAC payload
        addrMode = RxBuffer[BankIndex].Payload[1] & 0xCC;
        switch (addrMode)
        {
            case 0xC8: //short dest, long source
                // for P2P only broadcast allows short destination address
                if (RxBuffer[BankIndex].Payload[5] == 0xFF && RxBuffer[BankIndex].Payload[6] == 0xFF)
                {
                    MACRxPacket.flags.bits.broadcast = 1;
                }
                MACRxPacket.flags.bits.sourcePrsnt = 1;

#ifndef TARGET_SMALL
                if (bIntraPAN) // check if it is intraPAN
#endif
                {
#ifndef TARGET_SMALL
                    MACRxPacket.SourcePANID.v[0] = RxBuffer[BankIndex].Payload[3];
                    MACRxPacket.SourcePANID.v[1] = RxBuffer[BankIndex].Payload[4];
#endif
                    MACRxPacket.SourceAddress = &(RxBuffer[BankIndex].Payload[7]);

                    MACRxPacket.PayloadLen = RxBuffer[BankIndex].PayloadLen - 19;
                    MACRxPacket.Payload = &(RxBuffer[BankIndex].Payload[15]);
                }
#ifndef TARGET_SMALL
                else
                {
                    MACRxPacket.SourcePANID.v[0] = RxBuffer[BankIndex].Payload[7];
                    MACRxPacket.SourcePANID.v[1] = RxBuffer[BankIndex].Payload[8];
                    MACRxPacket.SourceAddress = &(RxBuffer[BankIndex].Payload[9]);
                    MACRxPacket.PayloadLen = RxBuffer[BankIndex].PayloadLen - 21;
                    MACRxPacket.Payload = &(RxBuffer[BankIndex].Payload[17]);
                }
#endif

                break;

            case 0xCC: // long dest, long source
                MACRxPacket.flags.bits.sourcePrsnt = 1;
#ifndef TARGET_SMALL
                if (bIntraPAN) // check if it is intraPAN
#endif
                {
                    //rxFrame.flags.bits.intraPAN = 1;
#ifndef TARGET_SMALL
                    MACRxPacket.SourcePANID.v[0] = RxBuffer[BankIndex].Payload[3];
                    MACRxPacket.SourcePANID.v[1] = RxBuffer[BankIndex].Payload[4];
#endif
                    MACRxPacket.SourceAddress = &(RxBuffer[BankIndex].Payload[13]);
                    MACRxPacket.PayloadLen = RxBuffer[BankIndex].PayloadLen - 25;
                    MACRxPacket.Payload = &(RxBuffer[BankIndex].Payload[21]);
                }
#ifndef TARGET_SMALL
                else
                {
                    MACRxPacket.SourcePANID.v[0] = RxBuffer[BankIndex].Payload[13];
                    MACRxPacket.SourcePANID.v[1] = RxBuffer[BankIndex].Payload[14];
                    MACRxPacket.SourceAddress = &(RxBuffer[BankIndex].Payload[15]);
                    MACRxPacket.PayloadLen = RxBuffer[BankIndex].PayloadLen - 27;
                    MACRxPacket.Payload = &(RxBuffer[BankIndex].Payload[23]);
                }
#endif
                break;

            case 0x80: // short source only. used in beacon
            {
                MACRxPacket.flags.bits.broadcast = 1;
                MACRxPacket.flags.bits.sourcePrsnt = 1;
                MACRxPacket.altSourceAddress = true;
                MACRxPacket.SourcePANID.v[0] = RxBuffer[BankIndex].Payload[3];
                MACRxPacket.SourcePANID.v[1] = RxBuffer[BankIndex].Payload[4];
                MACRxPacket.SourceAddress = &(RxBuffer[BankIndex].Payload[5]);
                MACRxPacket.PayloadLen = RxBuffer[BankIndex].PayloadLen - 11;
                MACRxPacket.Payload = &(RxBuffer[BankIndex].Payload[7]);
            }
                break;

            case 0x88: // short dest, short source
            {
                if (RxBuffer[BankIndex].Payload[5] == 0xFF && RxBuffer[BankIndex].Payload[6] == 0xFF)
                {
                    MACRxPacket.flags.bits.broadcast = 1;
                }
                MACRxPacket.flags.bits.sourcePrsnt = 1;
                MACRxPacket.altSourceAddress = true;
#ifndef TARGET_SMALL
                if (bIntraPAN == false)
                {
                    MACRxPacket.SourcePANID.v[0] = RxBuffer[BankIndex].Payload[7];
                    MACRxPacket.SourcePANID.v[1] = RxBuffer[BankIndex].Payload[8];
                    MACRxPacket.SourceAddress = &(RxBuffer[BankIndex].Payload[9]);
                    MACRxPacket.PayloadLen = RxBuffer[BankIndex].PayloadLen - 15;
                    MACRxPacket.Payload = &(RxBuffer[BankIndex].Payload[11]);
                } else
#endif
                {
                    MACRxPacket.SourcePANID.v[0] = RxBuffer[BankIndex].Payload[3];
                    MACRxPacket.SourcePANID.v[1] = RxBuffer[BankIndex].Payload[4];
                    MACRxPacket.SourceAddress = &(RxBuffer[BankIndex].Payload[7]);
                    MACRxPacket.PayloadLen = RxBuffer[BankIndex].PayloadLen - 13;
                    MACRxPacket.Payload = &(RxBuffer[BankIndex].Payload[9]);
                }
            }
                break;

            case 0x8C: // long dest, short source
            {
                MACRxPacket.flags.bits.sourcePrsnt = 1;
                MACRxPacket.altSourceAddress = true;
#ifndef TARGET_SMALL
                if (bIntraPAN) // check if it is intraPAN
#endif
                {
#ifndef TARGET_SMALL
                    MACRxPacket.SourcePANID.v[0] = RxBuffer[BankIndex].Payload[3];
                    MACRxPacket.SourcePANID.v[1] = RxBuffer[BankIndex].Payload[4];
#endif
                    MACRxPacket.SourceAddress = &(RxBuffer[BankIndex].Payload[12]);

                    MACRxPacket.PayloadLen = RxBuffer[BankIndex].PayloadLen - 19;
                    MACRxPacket.Payload = &(RxBuffer[BankIndex].Payload[15]);
                }
#ifndef TARGET_SMALL
                else
                {
                    MACRxPacket.SourcePANID.v[0] = RxBuffer[BankIndex].Payload[12];
                    MACRxPacket.SourcePANID.v[1] = RxBuffer[BankIndex].Payload[13];
                    MACRxPacket.SourceAddress = &(RxBuffer[BankIndex].Payload[14]);
                    MACRxPacket.PayloadLen = RxBuffer[BankIndex].PayloadLen - 21;
                    MACRxPacket.Payload = &(RxBuffer[BankIndex].Payload[17]);
                }
#endif
            }
                break;


            case 0x08: //dest-short, source-none
            {
                if (RxBuffer[BankIndex].Payload[5] == 0xFF && RxBuffer[BankIndex].Payload[6] == 0xFF)
                {
                    MACRxPacket.flags.bits.broadcast = 1;
                }
                MACRxPacket.PayloadLen = RxBuffer[BankIndex].PayloadLen - 10;
                MACRxPacket.Payload = &(RxBuffer[BankIndex].Payload[7]);
            }
                break;

                // all other addressing mode will not be supported in P2P
            default:
                // not valid addressing mode or no addressing info
                MiMAC_DiscardPacket();
                return false;
        }

#ifdef ENABLE_SECURITY
        if (RxBuffer[BankIndex].Payload[0] & 0x08)
        {
            API_UINT32_UNION FrameCounter;

            if (addrMode < 0xC0)
            {
                MiMAC_DiscardPacket();
                return false;
            }

            FrameCounter.v[0] = MACRxPacket.Payload[0];
            FrameCounter.v[1] = MACRxPacket.Payload[1];
            FrameCounter.v[2] = MACRxPacket.Payload[2];
            FrameCounter.v[3] = MACRxPacket.Payload[3];

            for (i = 0; i < CONNECTION_SIZE; i++)
            {
                if ((ConnectionTable[i].status.bits.isValid) &&
                        isSameAddress(ConnectionTable[i].Address, MACRxPacket.SourceAddress))
                {
                    break;
                }
            }

            if (i < CONNECTION_SIZE)
            {
                if (IncomingFrameCounter[i].Val > FrameCounter.Val)
                {
                    MiMAC_DiscardPacket();
                    return false;
                } else
                {
                    IncomingFrameCounter[i].Val = FrameCounter.Val;
                }
            }


            MACRxPacket.PayloadLen -= 5;

            if (false == DataDecrypt(&(MACRxPacket.Payload[5]), &(MACRxPacket.PayloadLen), MACRxPacket.SourceAddress, FrameCounter, RxBuffer[BankIndex].Payload[0]))
            {
                MiMAC_DiscardPacket();
                return false;
            }

            // remove the security header from the payload
            MACRxPacket.Payload = &(MACRxPacket.Payload[5]);
            MACRxPacket.flags.bits.secEn = 1;
        }
#else
        if (RxBuffer[BankIndex].Payload[0] & 0x08)
        {
            MiMAC_DiscardPacket();
            return false;
        }
#endif

        // check the frame type. Only the data and command frame type
        // are supported. Acknowledgement frame type is handled in
        // MRF24J40 transceiver hardware.
        switch (RxBuffer[BankIndex].Payload[0] & 0x07) // check frame type
        {
            case 0x01: // data frame
                MACRxPacket.flags.bits.packetType = PACKET_TYPE_DATA;
                break;
            case 0x03: // command frame
                MACRxPacket.flags.bits.packetType = PACKET_TYPE_COMMAND;
                break;
            case 0x00:
                // use reserved packet type to represent beacon packet
                MACRxPacket.flags.bits.packetType = PACKET_TYPE_RESERVE;
                break;
            default: // not support frame type
                MiMAC_DiscardPacket();
                return false;
        }
#ifndef TARGET_SMALL
        MACRxPacket.LQIValue = RxBuffer[BankIndex].Payload[RxBuffer[BankIndex].PayloadLen - 2];
        MACRxPacket.RSSIValue = RxBuffer[BankIndex].Payload[RxBuffer[BankIndex].PayloadLen - 1];
#endif

        return true;
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
    //re-enable the ACKS

    if (BankIndex < BANK_SIZE)
    {
        RxBuffer[BankIndex].PayloadLen = 0;
    }
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
bool MiMAC_SendPacket(INPUT MAC_TRANS_PARAM transParam,
        INPUT uint8_t *MACPayload,
        INPUT uint8_t MACPayloadLen)
{
    uint8_t headerLength;
    uint8_t loc = 0;
    uint8_t i = 0;
#ifndef TARGET_SMALL
    bool IntraPAN;
#endif
    MIWI_TICK t1, t2;
    uint8_t frameControl;

    if (transParam.flags.bits.broadcast)
    {
        transParam.altDestAddr = true;
    }

    if (transParam.flags.bits.secEn)
    {
        transParam.altSrcAddr = false;
    }


    // wait for the previous transmission finish
#if !defined(VERIFY_TRANSMIT)
    t1 = MiWi_TickGet();
    while (MRF24J40Status.bits.TX_BUSY)
    {
        if (RF_INT_PIN == 0)
        {
            RFIF = 1;
        }

        t2 = MiWi_TickGet();
        t2.Val = MiWi_TickGetDiff(t2, t1);
        if (t2.Val > TWENTY_MILI_SECOND) // 20 ms
        {
            InitMRF24J40();
            //MiMAC_SetChannel(MACCurrentChannel, 0);
            MiMAC_SetAltAddress(myNetworkAddress.v, MAC_PANID.v);
            MRF24J40Status.bits.TX_BUSY = 0;
        }
    }
#endif

    // set the frame control in variable i
    if (transParam.flags.bits.packetType == PACKET_TYPE_COMMAND)
    {
        frameControl = 0x03;
    } else if (transParam.flags.bits.packetType == PACKET_TYPE_DATA)
    {
        frameControl = 0x01;
    }


    // decide the header length for different addressing mode
#ifndef TARGET_SMALL
    if ((transParam.DestPANID.Val == MAC_PANID.Val) && (MAC_PANID.Val != 0xFFFF)) // this is intraPAN
#endif
    {
        headerLength = 5;
        frameControl |= 0x40;
#ifndef TARGET_SMALL
        IntraPAN = true;
#endif
    }
#ifndef TARGET_SMALL
    else
    {
        headerLength = 7;
        IntraPAN = false;
    }
#endif

    if (transParam.altDestAddr)
    {
        headerLength += 2;
    } else
    {
        headerLength += 8;
    }

    if (transParam.altSrcAddr)
    {
        headerLength += 2;
    } else
    {
        headerLength += 8;
    }

    if (transParam.flags.bits.ackReq && transParam.flags.bits.broadcast == false)
    {
        frameControl |= 0x20;
    }

    // use PACKET_TYPE_RESERVE to represent beacon. Fixed format for beacon packet
    if (transParam.flags.bits.packetType == PACKET_TYPE_RESERVE)
    {
        frameControl = 0x00;
        headerLength = 7;
#if !defined(TARGET_SMALL)
        IntraPAN = false;
#endif
        transParam.altSrcAddr = true;
        transParam.flags.bits.ackReq = false;
    }

#ifdef ENABLE_SECURITY
    if (transParam.flags.bits.secEn)
    {
        frameControl |= 0x08;
        DataEncrypt(MACPayload, &MACPayloadLen, OutgoingFrameCounter, frameControl);
    }
#endif

    // set header length
    PHYSetLongRAMAddr(loc++, headerLength);
    // set packet length
#ifdef ENABLE_SECURITY
    if (transParam.flags.bits.secEn)
    {
        PHYSetLongRAMAddr(loc++, headerLength + MACPayloadLen + 5);
    } else
#endif
    {
        PHYSetLongRAMAddr(loc++, headerLength + MACPayloadLen);
    }

    // set frame control LSB
    PHYSetLongRAMAddr(loc++, frameControl);

    // set frame control MSB
    if (transParam.flags.bits.packetType == PACKET_TYPE_RESERVE)
    {
        PHYSetLongRAMAddr(loc++, 0x80);
        // sequence number
        PHYSetLongRAMAddr(loc++, IEEESeqNum++);
    } else
    {
        if (transParam.altDestAddr && transParam.altSrcAddr)
        {
            PHYSetLongRAMAddr(loc++, 0x88);
        } else if (transParam.altDestAddr && transParam.altSrcAddr == 0)
        {
            PHYSetLongRAMAddr(loc++, 0xC8);
        } else if (transParam.altDestAddr == 0 && transParam.altSrcAddr == 1)
        {
            PHYSetLongRAMAddr(loc++, 0x8C);
        } else
        {
            PHYSetLongRAMAddr(loc++, 0xCC);
        }

        // sequence number
        PHYSetLongRAMAddr(loc++, IEEESeqNum++);

        // destination PANID
        PHYSetLongRAMAddr(loc++, transParam.DestPANID.v[0]);
        PHYSetLongRAMAddr(loc++, transParam.DestPANID.v[1]);

        // destination address
        if (transParam.flags.bits.broadcast)
        {
            PHYSetLongRAMAddr(loc++, 0xFF);
            PHYSetLongRAMAddr(loc++, 0xFF);
        } else
        {
            if (transParam.altDestAddr)
            {
                PHYSetLongRAMAddr(loc++, transParam.DestAddress[0]);
                PHYSetLongRAMAddr(loc++, transParam.DestAddress[1]);
            } else
            {
                for (i = 0; i < 8; i++)
                {
                    PHYSetLongRAMAddr(loc++, transParam.DestAddress[i]);
                }
            }
        }
    }

#ifndef TARGET_SMALL
    // source PANID if necessary
    if (IntraPAN == false)
    {
        PHYSetLongRAMAddr(loc++, MAC_PANID.v[0]);
        PHYSetLongRAMAddr(loc++, MAC_PANID.v[1]);
    }
#endif

    // source address
    if (transParam.altSrcAddr)
    {
        PHYSetLongRAMAddr(loc++, myNetworkAddress.v[0]);
        PHYSetLongRAMAddr(loc++, myNetworkAddress.v[1]);
    } else
    {
        for (i = 0; i < 8; i++)
        {
            PHYSetLongRAMAddr(loc++, MACInitParams.PAddress[i]);
        }
    }

#ifdef ENABLE_SECURITY
    if (transParam.flags.bits.secEn)
    {
        // fill the additional security aux header
        for (i = 0; i < 4; i++)
        {
            PHYSetLongRAMAddr(loc++, OutgoingFrameCounter.v[i]);
        }
        OutgoingFrameCounter.Val++;

#if defined(ENABLE_NETWORK_FREEZER)
        if ((OutgoingFrameCounter.v[0] == 0) && ((OutgoingFrameCounter.v[1] & 0x03) == 0))
        {
            nvmPutOutFrameCounter(OutgoingFrameCounter.v);
        }
#endif
        //copy myKeySequenceNumber
        PHYSetLongRAMAddr(loc++, myKeySequenceNumber);

    }
#endif


    // write the payload
    for (i = 0; i < MACPayloadLen; i++)
    {
        PHYSetLongRAMAddr(loc++, MACPayload[i]);
    }

    MRF24J40Status.bits.TX_BUSY = 1;

    // set the trigger value
    if (transParam.flags.bits.ackReq && transParam.flags.bits.broadcast == false)
    {
        i = 0x05;
#ifndef TARGET_SMALL
        MRF24J40Status.bits.TX_PENDING_ACK = 1;
#endif
    } else
    {
        i = 0x01;
#ifndef TARGET_SMALL
        MRF24J40Status.bits.TX_PENDING_ACK = 0;
#endif
    }


    // now trigger the transmission
    PHYSetShortRAMAddr(WRITE_TXNMTRIG, i);

#ifdef VERIFY_TRANSMIT
    t1 = MiWi_TickGet();
    while (1)
    {
        if (RF_INT_PIN == 0)
        {
            RFIF = 1;
        }
        if (MRF24J40Status.bits.TX_BUSY == 0)
        {
            if (MRF24J40Status.bits.TX_FAIL)
            {
                MRF24J40Status.bits.TX_FAIL = 0;
                return false;
            }
            break;
        }
        t2 = MiWi_TickGet();
        if (MiWi_TickGetDiff(t2, t1) > FORTY_MILI_SECOND)
        {
            InitMRF24J40();
            MiMAC_SetAltAddress(myNetworkAddress.v, MAC_PANID.v);
            MRF24J40Status.bits.TX_BUSY = 0;
            return false;
        }
    }
#endif
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

    #if defined(ENABLE_PA_LNA)
        PHYSetLongRAMAddr(TESTMODE, 0x08); // Disable automatic switch on PA/LNA
    #if defined(MRF24J40MC)
        PHYSetShortRAMAddr(WRITE_GPIODIR, 0x0F); // Set GPIO direction
        PHYSetShortRAMAddr(WRITE_GPIO, 0x0C); // Enable LNA
    #else
        PHYSetShortRAMAddr(WRITE_GPIODIR, 0x0F); // Set GPIO direction
        PHYSetShortRAMAddr(WRITE_GPIO, 0x04); // Enable LNA
    #endif
    #endif

    // calculate RSSI for firmware request
    PHYSetShortRAMAddr(WRITE_BBREG6, 0x80);

    // Firmware Request the RSSI
    RSSIcheck = PHYGetShortRAMAddr(READ_BBREG6);
    while ((RSSIcheck & 0x01) != 0x01)
    {
        RSSIcheck = PHYGetShortRAMAddr(READ_BBREG6);
    }

    // read the RSSI
    RSSIcheck = PHYGetLongRAMAddr(0x210);

    // enable RSSI attached to received packet again after
    // the energy scan is finished
    PHYSetShortRAMAddr(WRITE_BBREG6, 0x40);

    #if defined(ENABLE_PA_LNA)
    #if defined(MRF24J40MC)
        PHYSetShortRAMAddr(WRITE_GPIODIR, 0x08);
        PHYSetShortRAMAddr(WRITE_GPIO, 0x08);
    #else
        PHYSetShortRAMAddr(WRITE_GPIODIR, 0x00);
        PHYSetShortRAMAddr(WRITE_GPIO, 0);
    #endif
        PHYSetLongRAMAddr(TESTMODE, 0x0F);
    #endif

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
    switch (PowerState)
    {
        case POWER_STATE_DEEP_SLEEP:
        {
            //;clear the WAKE pin in order to allow the device to go to sleep
            PHY_WAKE = 0;

#if defined(ENABLE_PA_LNA)
            PHYSetLongRAMAddr(TESTMODE, 0x08); // Disable automatic switch on PA/LNA
            PHYSetShortRAMAddr(WRITE_GPIODIR, 0x0F); // Set GPIO direction
            PHYSetShortRAMAddr(WRITE_GPIO, 0x00); // Disable PA and LNA
#endif

            // make a power management reset to ensure device goes to sleep
            PHYSetShortRAMAddr(WRITE_SOFTRST, 0x04);

            //;write the registers required to place the device in sleep
            PHYSetShortRAMAddr(WRITE_TXBCNINTL, 0x80);
            PHYSetShortRAMAddr(WRITE_RXFLUSH, 0x60);
            PHYSetShortRAMAddr(WRITE_SLPACK, 0x80);
        }
            break;

        case POWER_STATE_OPERATE:
        {
#if 1
            // use wake pin to wake up the radio
            // enable the radio to wake up quicker

            uint8_t results;
            MIWI_TICK t1, t2;

            //wake up the device
            PHY_WAKE = 1;

            t1 = MiWi_TickGet();

            while (1)
            {
                t2 = MiWi_TickGet();
                t2.Val = MiWi_TickGetDiff(t2, t1);

                // if timeout, assume the device has waken up
                if (t2.Val > HUNDRED_MILI_SECOND)
                {
                    InitMRF24J40();
                    //MiMAC_SetChannel(MACCurrentChannel);
                    MiMAC_SetAltAddress(myNetworkAddress.v, MAC_PANID.v);
                    break;
                }

                results = PHYGetShortRAMAddr(READ_ISRSTS);
                if ((results & 0x40) != 0x00)
                {
                    break;
                }
            }

            while (1)
            {
                t2 = MiWi_TickGet();
                t2.Val = MiWi_TickGetDiff(t2, t1);

                // if timeout, assume the device has waken up
                if (t2.Val > HUNDRED_MILI_SECOND)
                {
                    InitMRF24J40();
                    //MiMAC_SetChannel(MACCurrentChannel);
                    MiMAC_SetAltAddress(myNetworkAddress.v, MAC_PANID.v);
                    break;
                }

                results = PHYGetLongRAMAddr(RFSTATE);
                if ((results & 0xE0) == 0xA0)
                {
                    break;
                }

            }

            PHYSetShortRAMAddr(WRITE_RFCTL, 0x04);
            PHYSetShortRAMAddr(WRITE_RFCTL, 0x00);

#else

            // use reset to wake up the radio is more
            // reliable
            InitMRF24J40();
            MiMAC_SetAltAddress(myNetworkAddress.v, MAC_PANID.v);

#endif

#if defined(ENABLE_PA_LNA)
#if defined(MRF24J40MC)
            PHYSetShortRAMAddr(WRITE_GPIODIR, 0x08);
            PHYSetShortRAMAddr(WRITE_GPIO, 0x08);
#else
            PHYSetShortRAMAddr(WRITE_GPIODIR, 0x00);
            PHYSetShortRAMAddr(WRITE_GPIO, 0);
#endif
            PHYSetLongRAMAddr(TESTMODE, 0x0F);
#endif             
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
    if (channel < 11 || channel > 26)
    {
        return false;
    }

#if defined(ENABLE_PA_LNA) && (defined(MRF24J40MB) || defined(MRF24J40MC))
    if (channel == 26)
    {
        return false;
    }
#endif

    MACCurrentChannel = channel;
    PHYSetLongRAMAddr(RFCTRL0, ((channel - 11) << 4) | 0x03);
    PHYSetShortRAMAddr(WRITE_RFCTL, 0x04);
    PHYSetShortRAMAddr(WRITE_RFCTL, 0x00);
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
    uint8_t reg;
    uint8_t tmp = outputPower % 10;

    reg = (outputPower / 10) << 6;
    if (tmp > 5)
    {
        reg += 0x38;
    } else if (tmp > 4)
    {
        reg += 0x30;
    } else if (tmp > 3)
    {
        reg += 0x28;
    } else if (tmp > 2)
    {
        reg += 0x20;
    } else if (tmp > 1)
    {
        reg += 0x18;
    } else if (tmp > 0)
    {
        reg += 0x10;
    }

    PHYSetLongRAMAddr(RFCTRL3, reg);
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
    myNetworkAddress.v[0] = Address[0];
    myNetworkAddress.v[1] = Address[1];
    MAC_PANID.v[0] = PANID[0];
    MAC_PANID.v[1] = PANID[1];

    PHYSetShortRAMAddr(WRITE_SADRL, myNetworkAddress.v[0]);
    PHYSetShortRAMAddr(WRITE_SADRH, myNetworkAddress.v[1]);
    PHYSetShortRAMAddr(WRITE_PANIDL, MAC_PANID.v[0]);
    PHYSetShortRAMAddr(WRITE_PANIDH, MAC_PANID.v[1]);
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

    IEEESeqNum = TMRL;

    MACCurrentChannel = 11;

    InitMRF24J40();

    MRF24J40Status.Val = 0;

    for (i = 0; i < BANK_SIZE; i++)
    {
        RxBuffer[i].PayloadLen = 0;
    }
#ifdef ENABLE_SECURITY
#if defined(ENABLE_NETWORK_FREEZER)
    if (initValue.actionFlags.bits.NetworkFreezer)
    {
        nvmGetOutFrameCounter(OutgoingFrameCounter.v);
        OutgoingFrameCounter.Val += FRAME_COUNTER_UPDATE_INTERVAL;
        nvmPutOutFrameCounter(OutgoingFrameCounter.v);
    } else
    {
        OutgoingFrameCounter.Val = 0;
        nvmPutOutFrameCounter(OutgoingFrameCounter.v);
        OutgoingFrameCounter.Val = 1;
    }
#else
    OutgoingFrameCounter.Val = 1;
#endif
#endif 

    return true;
}








#if defined(ENABLE_SECURITY)

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
bool DataDecrypt(uint8_t *Payload, uint8_t *PayloadLen, uint8_t *SourceIEEEAddress,
        API_UINT32_UNION FrameCounter, uint8_t FrameControl)
{
    uint8_t i;
    uint16_t loc;
    MIWI_TICK t1, t2;

    // disable receiving further message
    PHYSetShortRAMAddr(WRITE_RXFLUSH, 0x01);
    PHYSetShortRAMAddr(WRITE_BBREG1, 0x04);

    // fill the header length
    PHYSetLongRAMAddr(0x000, 13);

    // fill the length of packet
    PHYSetLongRAMAddr(0x001, (*PayloadLen + 15)); // two additional bytes FCS

    loc = 2;
    // fill the header
    PHYSetLongRAMAddr(loc++, FrameControl);
    PHYSetLongRAMAddr(loc++, FrameCounter.v[0]);
    PHYSetLongRAMAddr(loc++, FrameCounter.v[1]);
    PHYSetLongRAMAddr(loc++, FrameCounter.v[2]);
    PHYSetLongRAMAddr(loc++, FrameCounter.v[3]);

    for (i = 0; i < 8; i++)
    {
        PHYSetLongRAMAddr(loc++, SourceIEEEAddress[i]);
    }

    // fill the payload
    for (i = 0; i < *PayloadLen; i++)
    {
        PHYSetLongRAMAddr(loc++, Payload[i]);
    }

    // set nounce
    loc = 0x24C;
    for (i = 0; i < 8; i++)
    {
        PHYSetLongRAMAddr(loc--, SourceIEEEAddress[i]);
    }
    for (i = 0; i < 4; i++)
    {
        PHYSetLongRAMAddr(loc--, FrameCounter.v[i]);
    }
    PHYSetLongRAMAddr(loc--, SECURITY_LEVEL);

    // fill the key
    loc = 0x280;
    for (i = 0; i < 16; i++)
    {
        PHYSetLongRAMAddr(loc++, mySecurityKey[i]);
    }

    // set the decryption mode
    PHYSetShortRAMAddr(WRITE_SECCR2, 0x80);

    // step 5, fill the security mode
    PHYSetShortRAMAddr(WRITE_SECCR0, SECURITY_LEVEL);

    MRF24J40Status.bits.SEC_IF = 1;
    MRF24J40Status.bits.TX_BUSY = 1;

    // trigger the hardware cipher to operate
    PHYSetShortRAMAddr(WRITE_TXNMTRIG, 0x03);

    t1 = MiWi_TickGet();
    while (MRF24J40Status.bits.SEC_IF)
    {
        if (RF_INT_PIN == 0)
        {
            RFIF = 1;
        }

        t2 = MiWi_TickGet();
        if (MiWi_TickGetDiff(t2, t1) > ONE_SECOND)
        {
            InitMRF24J40();
            MiMAC_SetChannel(MACCurrentChannel, 0);
            MiMAC_SetAltAddress(myNetworkAddress.v, MAC_PANID.v);
            return false;
        }
    }

    if (SECURITY_LEVEL != 0x01) // not CTR mode, need to check MIC
    {
        uint8_t MIC_check = PHYGetShortRAMAddr(READ_RXSR);
        if (MIC_check & 0x40)
        {
            PHYSetShortRAMAddr(WRITE_RXSR, 0x40);
            /*InitMRF24J40();
            MiMAC_SetAltAddress(myNetworkAddress.v, MAC_PANID.v);
             */
            PHYSetShortRAMAddr(WRITE_BBREG1, 0x00);
            PHYSetShortRAMAddr(WRITE_RXFLUSH, 0x01);
            return false;
        }
    }

    *PayloadLen = PHYGetLongRAMAddr(0x001) - 13;
    i = SECURITY_LEVEL;
    switch (i)
    {
        case 0x02:
        case 0x05:
            *PayloadLen -= 18;
            break;

        case 0x03:
        case 0x06:
            *PayloadLen -= 10;
            break;

        case 0x04:
        case 0x07:
            *PayloadLen -= 6;
            break;

        case 0x01:
            *PayloadLen -= 2;
            break;
    }

    // copy the output data
    loc = 15;
    for (i = 0; i < *PayloadLen; i++)
    {
        Payload[i] = PHYGetLongRAMAddr(loc++);
    }

    // renable receiving further message
    PHYSetShortRAMAddr(WRITE_BBREG1, 0x00);
    PHYSetShortRAMAddr(WRITE_RXFLUSH, 0x01);
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
    uint8_t i;
    uint16_t loc;
    MIWI_TICK t1, t2;

    // disable receiving further message
    PHYSetShortRAMAddr(WRITE_RXFLUSH, 0x01);
    PHYSetShortRAMAddr(WRITE_BBREG1, 0x04);

    // fill the header length
    PHYSetLongRAMAddr(0x000, 13);

    // fill the packet length
    PHYSetLongRAMAddr(0x001, (*PayloadLen + 13));

    loc = 2;
    // fill the header
    PHYSetLongRAMAddr(loc++, FrameControl);
    PHYSetLongRAMAddr(loc++, FrameCounter.v[0]);
    PHYSetLongRAMAddr(loc++, FrameCounter.v[1]);
    PHYSetLongRAMAddr(loc++, FrameCounter.v[2]);
    PHYSetLongRAMAddr(loc++, FrameCounter.v[3]);

    for (i = 0; i < 8; i++)
    {
        PHYSetLongRAMAddr(loc++, MACInitParams.PAddress[i]);
    }

    // fill the payload
    for (i = 0; i < *PayloadLen; i++)
    {
        PHYSetLongRAMAddr(loc++, Payload[i]);
    }

    // set nounce
    loc = 0x24C;
    for (i = 0; i < 8; i++)
    {
        PHYSetLongRAMAddr(loc--, MACInitParams.PAddress[i]);
    }
    for (i = 0; i < 4; i++)
    {
        PHYSetLongRAMAddr(loc--, FrameCounter.v[i]);
    }
    PHYSetLongRAMAddr(loc--, SECURITY_LEVEL);

    // fill the key
    loc = 0x280;
    for (i = 0; i < 16; i++)
    {
        PHYSetLongRAMAddr(loc++, (uint8_t) mySecurityKey[i]);
    }

    // set the encryption
    PHYSetShortRAMAddr(WRITE_SECCR2, 0x40);

    // fill security model
    PHYSetShortRAMAddr(WRITE_SECCR0, SECURITY_LEVEL);

    MRF24J40Status.bits.SEC_IF = 1;

    // trigger the hardware cipher to operate
    PHYSetShortRAMAddr(WRITE_TXNMTRIG, 0x03);

    t1 = MiWi_TickGet();
    while (MRF24J40Status.bits.SEC_IF)
    {
        if (RF_INT_PIN == 0)
        {
            RFIF = 1;
        }

        t2 = MiWi_TickGet();
        if (MiWi_TickGetDiff(t2, t1) > ONE_SECOND)
        {
            InitMRF24J40();
            MiMAC_SetAltAddress(myNetworkAddress.v, MAC_PANID.v);
            return false;
        }
    }

    *PayloadLen = PHYGetLongRAMAddr(0x001) - 13;

    loc = 0x002 + 13;
    for (i = 0; i < *PayloadLen; i++)
    {
        Payload[i] = PHYGetLongRAMAddr(loc++);
    }

    // renable receiving further message
    PHYSetShortRAMAddr(WRITE_BBREG1, 0x00);
    PHYSetShortRAMAddr(WRITE_RXFLUSH, 0x01);

    return true;
}



#endif




/*********************************************************************
 * void HighISR(void)
 *
 * Overview:        This is the interrupt handler for the MRF24J40 and
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
#if defined(__XC8)
void interrupt  high_isr (void)
#elif defined(__18CXX)
#pragma interruptlow HighISR
void HighISR(void)
#elif defined(__dsPIC30F__) || defined(__dsPIC33F__) || defined(__PIC24F__) || defined(__PIC24FK__) || defined(__PIC24H__)
void _ISRFAST __attribute__((interrupt, auto_psv)) _INT1Interrupt(void)
#elif defined(__XC32)
void __attribute((interrupt(ipl4), vector(_EXTERNAL_1_VECTOR), nomips16))  _RxInterrupt()
//#pragma interrupt _RxInterrupt ipl4 vector 16
#else

void _ISRFAST _INT1Interrupt(void)
#endif
{

    if (RFIE && RFIF)
    {
        uint8_t i;

        //clear the interrupt flag as soon as possible such that another interrupt can
        //occur quickly.
        RFIF = 0;

        //create a new scope for the MRF24J40 interrupts so that we can clear the interrupt
        //flag quickly and then handle the interrupt that we have already received
        {
            MRF24J40_IFREG flags;

            //read the interrupt status register to see what caused the interrupt
            flags.Val = PHYGetShortRAMAddr(READ_ISRSTS);

            if (flags.bits.RF_TXIF)
            {
                //if the TX interrupt was triggered
                //clear the busy flag indicating the transmission was complete
                MRF24J40Status.bits.TX_BUSY = 0;

                if (MRF24J40Status.bits.SEC_IF)
                {
                    MRF24J40Status.bits.SEC_IF = 0;
                }

                failureCounter = 0;

#ifndef TARGET_SMALL
                //if we were waiting for an ACK
                if (MRF24J40Status.bits.TX_PENDING_ACK)
                {
                    DRIVER_UINT8_UNION results;

                    //read out the results of the transmission
                    results.Val = PHYGetShortRAMAddr(READ_TXSR);

                    if (results.bits.b0 == 1)
                    {
                        //the transmission wasn't successful and the number
                        //of retries is located in bits 7-6 of TXSR
                        MRF24J40Status.bits.TX_FAIL = 1;
                    }

                    //transmission finished
                    //clear that I am pending an ACK, already got it
                    MRF24J40Status.bits.TX_PENDING_ACK = 0;

                }
#endif
            }

            if (flags.bits.RF_RXIF)
            {
                uint8_t RxBank = 0xFF;

                for (i = 0; i < BANK_SIZE; i++)
                {
                    if (RxBuffer[i].PayloadLen == 0)
                    {
                        RxBank = i;
                        break;
                    }
                }

                //if the RX interrupt was triggered
                if (RxBank < BANK_SIZE)
                {

                    //If the part is enabled for receiving packets right now
                    //(not pending an ACK)
                    //indicate that we have a packet in the buffer pending to
                    //be read into the buffer from the FIFO
                    PHYSetShortRAMAddr(WRITE_BBREG1, 0x04);

                    //get the size of the packet
                    //2 more bytes for RSSI and LQI reading
                    RxBuffer[RxBank].PayloadLen = PHYGetLongRAMAddr(0x300) + 2;
                    if (RxBuffer[RxBank].PayloadLen < RX_PACKET_SIZE)
                    {
                        //indicate that data is now stored in the buffer
                        MRF24J40Status.bits.RX_BUFFERED = 1;

                        //copy all of the data from the FIFO into the TxBuffer, plus RSSI and LQI
                        for (i = 1; i <= RxBuffer[RxBank].PayloadLen + 2; i++)
                        {
                            RxBuffer[RxBank].Payload[i - 1] = PHYGetLongRAMAddr(0x300 + i);
                        }
                        PHYSetShortRAMAddr(WRITE_RXFLUSH, 0x01);
                    } else
                    {
                        //else it was a larger packet than we can receive
                        //flush it
                        PHYSetShortRAMAddr(WRITE_RXFLUSH, 0x01);
                    }

                    // enable radio to receive next packet
                    PHYSetShortRAMAddr(WRITE_BBREG1, 0x00);
                } else
                {
                    //else if the RX is not enabled then we need to flush this packet
                    //flush the buffer
                    PHYSetShortRAMAddr(WRITE_RXFLUSH, 0x01);
#if defined(ENABLE_SECURITY)
                    MRF24J40Status.bits.RX_SECURITY = 0;
#if !defined(TARGET_SMALL)
                    MRF24J40Status.bits.RX_IGNORE_SECURITY = 0;
#endif
#endif
                }//end of RX_BUFFERED check

            } //end of RXIF check


            if (flags.bits.SECIF)
            {
                PHYSetShortRAMAddr(WRITE_SECCR0, 0x80); // ignore the packet
            }
        } //end of scope of RF interrupt handler
    } //end of if(RFIE && RFIF)


    #if defined(__XC8)
    //check to see if the symbol timer overflowed
    if (TMR_IF)
    {
        if (TMR_IE)
        {
            /* there was a timer overflow */
            TMR_IF = 0;
            timerExtension1++;
            if (timerExtension1 == 0)
            {
                timerExtension2++;
            }
        }
    }

    UserInterruptHandler();
    #endif

    return;

} //end of interrupt handler




