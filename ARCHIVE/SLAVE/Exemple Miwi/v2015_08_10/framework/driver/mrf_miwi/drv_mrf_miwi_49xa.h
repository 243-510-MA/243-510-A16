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
#include "driver/mrf_miwi/drv_mrf_miwi.h"
#include "driver/mrf_miwi/drv_mrf_miwi_security.h"
#include "driver/mrf_miwi/drv_mrf_miwi_crc.h"




// following should be in the def file
#define XTAL_LD_CAP_85					0x0000
#define XTAL_LD_CAP_9					0x0001
#define XTAL_LD_CAP_95					0x0002
#define XTAL_LD_CAP_10					0x0003
#define XTAL_LD_CAP_105					0x0004
#define XTAL_LD_CAP_11					0x0005
#define XTAL_LD_CAP_115					0x0006
#define XTAL_LD_CAP_12					0x0007
#define XTAL_LD_CAP_125					0x0008
#define XTAL_LD_CAP_13					0x0009
#define XTAL_LD_CAP_135					0x000A
#define XTAL_LD_CAP_14					0x000B
#define XTAL_LD_CAP_145					0x000C
#define XTAL_LD_CAP_15					0x000D
#define XTAL_LD_CAP_155					0x000E
#define XTAL_LD_CAP_16					0x000F


#define LNA_GAIN_0_DB                   0x0000
#define LNA_GAIN_N_6_DB                 0x0008
#define LNA_GAIN_N_14_DB                0x0010
#define LNA_GAIN_N_20_DB                0x0018

#define TX_POWER_0_DB                   0x0000
#define TX_POWER_N_2_5_DB               0x0001
#define TX_POWER_N_5_DB                 0x0002
#define TX_POWER_N_7_5_DB               0x0003
#define TX_POWER_N_10_DB                0x0004
#define TX_POWER_N_12_5_DB              0x0005
#define TX_POWER_N_15_DB                0x0006
#define TX_POWER_N_17_5_DB              0x0007


#define RSSI_THRESHOLD_103              0x0000
#define RSSI_THRESHOLD_97               0x0001
#define RSSI_THRESHOLD_91               0x0002
#define RSSI_THRESHOLD_85               0x0003
#define RSSI_THRESHOLD_79               0x0004
#define RSSI_THRESHOLD_73               0x0005


#if defined(BAND_434)

   #define              FREQ_BAND               0x0010              // 434MHz
   #if defined(DATA_RATE_1200)
        #define         DRVSREG                 0xC6A3              // 1200bps

        #define         RAW_RF_DEV              ((uint16_t)11+2*((uint16_t)CRYSTAL_PPM*434/1000))
        #define         RF_DEV                  ( ((RAW_RF_DEV % 15) < 8 ) ? (RAW_RF_DEV - (RAW_RF_DEV % 15)) : (RAW_RF_DEV - (RAW_RF_DEV % 15) + 15) )
        #define         TXCREG                  (0x9800 | (((uint16_t)RF_DEV/15 - 1)<<4) | TX_POWER)
        #define         FREQ_START              1332
        #define         FREQ_STEP               132
        #define         CHANNEL_NUM             5
        #define         FULL_CHANNEL_MAP        0x0000001F

   #elif defined(DATA_RATE_9600)

        #define         DRVSREG                 0xC623              // 9600bps

        #define         RAW_RF_DEV              ((uint16_t)19+2*((uint16_t)CRYSTAL_PPM*434/1000))
        #define         RF_DEV                  ( ((RAW_RF_DEV % 15) < 8 ) ? (RAW_RF_DEV - (RAW_RF_DEV % 15)) : (RAW_RF_DEV - (RAW_RF_DEV % 15) + 15) )
        #define         TXCREG                  (0x9800 | (((uint16_t)RF_DEV/15 - 1)<<4) | TX_POWER)
        #define         FREQ_START              1332
        #define         FREQ_STEP               132
        #define         CHANNEL_NUM             5
        #define         FULL_CHANNEL_MAP        0x0000001F

   #elif defined(DATA_RATE_19200)

        #define         DRVSREG                 0xC611              // 19200bps

        #define         RAW_RF_DEV              ((uint16_t)29+2*((uint16_t)CRYSTAL_PPM*434/1000))
        #define         RF_DEV                  ( ((RAW_RF_DEV % 15) < 8 ) ? (RAW_RF_DEV - (RAW_RF_DEV % 15)) : (RAW_RF_DEV - (RAW_RF_DEV % 15) + 15) )
        #define         TXCREG                  (0x9800 | (((uint16_t)RF_DEV/15 - 1)<<4) | TX_POWER)
        #define         FREQ_START              1412
        #define         FREQ_STEP               154
        #define         CHANNEL_NUM             4
        #define         FULL_CHANNEL_MAP        0x0000000F

   #elif defined(DATA_RATE_38400)

        #define         DRVSREG                 0xC608              // 38400

        #define         RAW_RF_DEV              ((uint16_t)48+2*((uint16_t)CRYSTAL_PPM*434/1000))
        #define         RF_DEV                  ( ((RAW_RF_DEV % 15) < 8 ) ? (RAW_RF_DEV - (RAW_RF_DEV % 15)) : (RAW_RF_DEV - (RAW_RF_DEV % 15) + 15) )
        #define         TXCREG                  (0x9800 | (((uint16_t)RF_DEV/15 - 1)<<4) | TX_POWER)
        #define         FREQ_START              1388
        #define         FREQ_STEP               180
        #define         CHANNEL_NUM             4
        #define         FULL_CHANNEL_MAP        0x0000000F

   #elif defined(DATA_RATE_57600)

        #define         DRVSREG                 0xC605

        #define         RAW_RF_DEV              ((uint16_t)67+2*((uint16_t)CRYSTAL_PPM*434/1000))
        #define         RF_DEV                  ( ((RAW_RF_DEV % 15) < 8 ) ? (RAW_RF_DEV - (RAW_RF_DEV % 15)) : (RAW_RF_DEV - (RAW_RF_DEV % 15) + 15) )
        #define         TXCREG                  (0x9800 | (((uint16_t)RF_DEV/15 - 1)<<4) | TX_POWER)
        #define         FREQ_START              1444
        #define         FREQ_STEP               256
        #define         CHANNEL_NUM             3
        #define         FULL_CHANNEL_MAP        0x00000007

   #elif defined(DATA_RATE_115200)

        #define         DRVSREG                 0xC602

        #define         RAW_RF_DEV              ((uint16_t)125+2*((uint16_t)CRYSTAL_PPM*434/1000))
        #define         RF_DEV                  ( ((RAW_RF_DEV % 15) < 8 ) ? (RAW_RF_DEV - (RAW_RF_DEV % 15)) : (RAW_RF_DEV - (RAW_RF_DEV % 15) + 15) )
        #define         TXCREG                  (0x9800 | (((uint16_t)RF_DEV/15 - 1)<<4) | TX_POWER)
        #define         FREQ_START              1564
        #define         FREQ_STEP               345
        #define         CHANNEL_NUM             2
        #define         FULL_CHANNEL_MAP        0x00000003
   #else
        #error          "No valid data rate defined"
   #endif

#elif defined(BAND_868)

    #define              FREQ_BAND               0x0020              // 868MHz
    #if defined(DATA_RATE_1200)
        #define         DRVSREG                 0xC6A3

        #define         RAW_RF_DEV              ((uint16_t)11+2*((uint16_t)CRYSTAL_PPM*868/1000))
        #define         RF_DEV                  ( ((RAW_RF_DEV % 15) < 8 ) ? (RAW_RF_DEV - (RAW_RF_DEV % 15)) : (RAW_RF_DEV - (RAW_RF_DEV % 15) + 15) )
        #define         TXCREG                  (0x9800 | (((uint16_t)RF_DEV/15 - 1)<<4) | TX_POWER)
        #define         FREQ_START              710
        #define         FREQ_STEP               90
        #define         CHANNEL_NUM             15
        #define         FULL_CHANNEL_MAP        0x00007FFF
    #elif defined(DATA_RATE_9600)

        #define         DRVSREG                 0xC623

        #define         RAW_RF_DEV              ((uint16_t)19+2*((uint16_t)CRYSTAL_PPM*868/1000))
        #define         RF_DEV                  ( ((RAW_RF_DEV % 15) < 8 ) ? (RAW_RF_DEV - (RAW_RF_DEV % 15)) : (RAW_RF_DEV - (RAW_RF_DEV % 15) + 15) )
        #define         TXCREG                  (0x9800 | (((uint16_t)RF_DEV/15 - 1)<<4) | TX_POWER)
        #define         FREQ_START              710
        #define         FREQ_STEP               90
        #define         CHANNEL_NUM             15
        #define         FULL_CHANNEL_MAP        0x00007FFF

    #elif defined(DATA_RATE_19200)

        #define         DRVSREG                 0xC611

        #define         RAW_RF_DEV              ((uint16_t)29+2*((uint16_t)CRYSTAL_PPM*868/1000))
        #define         RF_DEV                  ( ((RAW_RF_DEV % 15) < 8 ) ? (RAW_RF_DEV - (RAW_RF_DEV % 15)) : (RAW_RF_DEV - (RAW_RF_DEV % 15) + 15) )
        #define         TXCREG                  (0x9800 | (((uint16_t)RF_DEV/15 - 1)<<4) | TX_POWER)
        #define         FREQ_START              740
        #define         FREQ_STEP               110
        #define         CHANNEL_NUM             12
        #define         FULL_CHANNEL_MAP        0x00000FFF

    #elif defined(DATA_RATE_38400)

        #define         DRVSREG                 0xC608

        #define         RAW_RF_DEV              ((uint16_t)48+2*((uint16_t)CRYSTAL_PPM*868/1000))
        #define         RF_DEV                  ( ((RAW_RF_DEV % 15) < 8 ) ? (RAW_RF_DEV - (RAW_RF_DEV % 15)) : (RAW_RF_DEV - (RAW_RF_DEV % 15) + 15) )
        #define         TXCREG                  (0x9800 | (((uint16_t)RF_DEV/15 - 1)<<4) | TX_POWER)
        #define         FREQ_START              724
        #define         FREQ_STEP               128
        #define         CHANNEL_NUM             11
        #define         FULL_CHANNEL_MAP        0x000007FF

    #elif defined(DATA_RATE_57600)

        #define         DRVSREG                 0xC605

        #define         RAW_RF_DEV              ((uint16_t)67+2*((uint16_t)CRYSTAL_PPM*868/1000))
        #define         RF_DEV                  ( ((RAW_RF_DEV % 15) < 8 ) ? (RAW_RF_DEV - (RAW_RF_DEV % 15)) : (RAW_RF_DEV - (RAW_RF_DEV % 15) + 15) )
        #define         TXCREG                  (0x9800 | (((uint16_t)RF_DEV/15 - 1)<<4) | TX_POWER)
        #define         FREQ_START              750
        #define         FREQ_STEP               157
        #define         CHANNEL_NUM             9
        #define         FULL_CHANNEL_MAP        0x000001FF

    #elif defined(DATA_RATE_115200)

        #define         DRVSREG                 0xC602

        #define         RAW_RF_DEV              ((uint16_t)125+2*((uint16_t)CRYSTAL_PPM*868/1000))
        #define         RF_DEV                  ( ((RAW_RF_DEV % 15) < 8 ) ? (RAW_RF_DEV - (RAW_RF_DEV % 15)) : (RAW_RF_DEV - (RAW_RF_DEV % 15) + 15) )
        #define         TXCREG                  (0x9800 | (((uint16_t)RF_DEV/15 - 1)<<4) | TX_POWER)
        #define         FREQ_START              800
        #define         FREQ_STEP               200
        #define         CHANNEL_NUM             7
        #define         FULL_CHANNEL_MAP        0x0000007F

    #else
        #error          "No valid data rate defined"
    #endif

#elif defined(BAND_915)

   #define              FREQ_BAND               0x0030              // 915MHz
   #if defined(DATA_RATE_1200)
        #define         DRVSREG                 0xC6A3

        #define         RAW_RF_DEV              ((uint16_t)11+2*((uint16_t)CRYSTAL_PPM*915/1000))
        #define         RF_DEV                  ( ((RAW_RF_DEV % 15) < 8 ) ? (RAW_RF_DEV - (RAW_RF_DEV % 15)) : (RAW_RF_DEV - (RAW_RF_DEV % 15) + 15) )
        #define         TXCREG                  (0x9800 | (((uint16_t)RF_DEV/15 - 1)<<4) | TX_POWER)
        #define         FREQ_START              320
        #define         FREQ_STEP               105
        #define         CHANNEL_NUM             32
        #define         FULL_CHANNEL_MAP        0xFFFFFFFF
   #elif defined(DATA_RATE_9600)

        #define         DRVSREG                 0xC623

        #define         RAW_RF_DEV              ((uint16_t)19+2*((uint16_t)CRYSTAL_PPM*915/1000))
        #define         RF_DEV                  ( ((RAW_RF_DEV % 15) < 8 ) ? (RAW_RF_DEV - (RAW_RF_DEV % 15)) : (RAW_RF_DEV - (RAW_RF_DEV % 15) + 15) )
        #define         TXCREG                  (0x9800 | (((uint16_t)RF_DEV/15 - 1)<<4) | TX_POWER)
        #define         FREQ_START              320
        #define         FREQ_STEP               105
        #define         CHANNEL_NUM             32
        #define         FULL_CHANNEL_MAP        0xFFFFFFFF

    #elif defined(DATA_RATE_19200)

        #define         DRVSREG                 0xC611

        #define         RAW_RF_DEV              ((uint16_t)29+2*((uint16_t)CRYSTAL_PPM*915/1000))
        #define         RF_DEV                  ( ((RAW_RF_DEV % 15) < 8 ) ? (RAW_RF_DEV - (RAW_RF_DEV % 15)) : (RAW_RF_DEV - (RAW_RF_DEV % 15) + 15) )
        #define         TXCREG                  (0x9800 | (((uint16_t)RF_DEV/15 - 1)<<4) | TX_POWER)
        #define         FREQ_START              320
        #define         FREQ_STEP               105
        #define         CHANNEL_NUM             32
        #define         FULL_CHANNEL_MAP        0xFFFFFFFF

    #elif defined(DATA_RATE_38400)

        #define         DRVSREG                 0xC608

        #define         RAW_RF_DEV              ((uint16_t)48+2*((uint16_t)CRYSTAL_PPM*915/1000))
        #define         RF_DEV                  ( ((RAW_RF_DEV % 15) < 8 ) ? (RAW_RF_DEV - (RAW_RF_DEV % 15)) : (RAW_RF_DEV - (RAW_RF_DEV % 15) + 15) )
        #define         TXCREG                  (0x9800 | (((uint16_t)RF_DEV/15 - 1)<<4) | TX_POWER)
        #define         FREQ_START              320
        #define         FREQ_STEP               105
        #define         CHANNEL_NUM             32
        #define         FULL_CHANNEL_MAP        0xFFFFFFFF

    #elif defined(DATA_RATE_57600)

        #define         DRVSREG                 0xC605

        #define         RAW_RF_DEV              ((uint16_t)68+2*((uint16_t)CRYSTAL_PPM*915/1000))
        #define         RF_DEV                  ( ((RAW_RF_DEV % 15) < 8 ) ? (RAW_RF_DEV - (RAW_RF_DEV % 15)) : (RAW_RF_DEV - (RAW_RF_DEV % 15) + 15) )
        #define         TXCREG                  (0x9800 | (((uint16_t)RF_DEV/15 - 1)<<4) | TX_POWER)
        #define         FREQ_START              320
        #define         FREQ_STEP               105
        #define         CHANNEL_NUM             32
        #define         FULL_CHANNEL_MAP        0xFFFFFFFF

   #elif defined(DATA_RATE_115200)

        #define         DRVSREG                 0xC602

        #define         RAW_RF_DEV              ((uint16_t)125+2*((uint16_t)CRYSTAL_PPM*915/1000))
        #define         RF_DEV                  ( ((RAW_RF_DEV % 15) < 8 ) ? (RAW_RF_DEV - (RAW_RF_DEV % 15)) : (RAW_RF_DEV - (RAW_RF_DEV % 15) + 15) )
        #define         TXCREG                  (0x9800 | (((uint16_t)RF_DEV/15 - 1)<<4) | TX_POWER)
        #define         FREQ_START              333
        #define         FREQ_STEP               134
        #define         CHANNEL_NUM             26
        #define         FULL_CHANNEL_MAP        0x03FFFFFF

   #else
        #error          "No valid data rate defined"
   #endif
#else
//             #error      "No valid frequency band selected"
#endif


#define         RXCREG                  (0x9400 | LNA_GAIN | RSSI_THRESHOLD)
#define         GENCREG                 (0x8000|FREQ_BAND|XTAL_LD_CAP)
#define         PMCREG                  0x8201
#define         FIFORSTREG              0xCA81
#define         AFCCREG                 0xC4B7
#define         BBFCREG                 0xC2AC

#if defined(PROTOCOL_MIWI)
    #define PROTOCOL_HEADER_SIZE 11
#endif

#if defined(PROTOCOL_MIWI_PRO)
    #define PROTOCOL_HEADER_SIZE 11
#endif

#if defined(PROTOCOL_P2P)
    #define PROTOCOL_HEADER_SIZE 0
#endif

#if defined(ENABLE_SECURITY)
    #define TX_PACKET_SIZE (TX_BUFFER_SIZE+PROTOCOL_HEADER_SIZE+BLOCK_SIZE+MY_ADDRESS_LENGTH+MY_ADDRESS_LENGTH+10)
    #define RX_PACKET_SIZE (RX_BUFFER_SIZE+PROTOCOL_HEADER_SIZE+MY_ADDRESS_LENGTH+MY_ADDRESS_LENGTH+BLOCK_SIZE+10)
#else
    #define TX_PACKET_SIZE  (TX_BUFFER_SIZE+PROTOCOL_HEADER_SIZE+MY_ADDRESS_LENGTH+MY_ADDRESS_LENGTH+5)
    #define RX_PACKET_SIZE  (RX_BUFFER_SIZE+PROTOCOL_HEADER_SIZE+MY_ADDRESS_LENGTH+MY_ADDRESS_LENGTH+5)
#endif


#if RX_PACKET_SIZE > 126
    #warning "RX\_BUFFER\_SIZE RX_BUFFER_SIZE may be defined too big"
    #undef RX_PACKET_SIZE
    #define RX_PACKET_SIZE 126
#endif

#if TX_PACKET_SIZE > 126
    #warning "TX\_BUFFER\_SIZE TX_BUFFER_SIZE may be defined too big"
    #undef TX_PACKET_SIZE
    #define TX_PACKET_SIZE 126
#endif


typedef struct
{
    union
    {
        uint8_t    Val;
        struct
        {
            uint8_t    Valid       :1;
            uint8_t    RSSI        :1;
            uint8_t    DQD         :1;
        } bits;
    } flags;
    uint8_t        Payload[RX_PACKET_SIZE];
    uint8_t        PayloadLen;
} RX_PACKET;

typedef struct
{
    bool        Valid;
    uint8_t     Seq;
    uint16_t        CRC;
    MIWI_TICK   startTick;
} ACK_INFO;

typedef union
{
    uint16_t    Val;
    uint8_t    v[2];
    struct
    {
        uint8_t    RSSI_ATS    :1;
        uint8_t    FFEM        :1;
        uint8_t    LBD         :1;
        uint8_t    EXT         :1;
        uint8_t    WKUP        :1;
        uint8_t    RGUR_FFOV   :1;
        uint8_t    POR         :1;
        uint8_t    RG_FF_IT    :1;

        uint8_t    filler      :5;
        uint8_t    ATGL        :1;
        uint8_t    CRL         :1;
        uint8_t    DQD         :1;
    } bits;
} TRANSCEIVER_STATUS;


extern volatile TRANSCEIVER_STATUS   TransceiverStatus;

