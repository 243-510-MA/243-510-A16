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

#ifndef __P2P_H_
#define __P2P_H_

#define INPUT
#define OUTPUT
#define IOPUT


#include "system.h"
#include "system_config.h"

#if defined(PROTOCOL_P2P)
/************************ HEADERS **********************************/
#include "symbol.h"
#include "miwi/miwi_api.h"
#include "driver/mrf_miwi/drv_mrf_miwi.h"

/************************ DEFINITIONS ******************************/

#define PAYLOAD_START                           0

#define STATUS_SUCCESS                          0x00
#define STATUS_EXISTS                           0x01
#define STATUS_ACTIVE_SCAN                      0x02
#define STATUS_ENTRY_NOT_EXIST                  0xF0
#define STATUS_NOT_ENOUGH_SPACE                 0xF1
#define STATUS_NOT_SAME_PAN                     0xF2
#define STATUS_NOT_PERMITTED                    0xF3

#define CMD_P2P_CONNECTION_REQUEST              0x81
#define CMD_P2P_CONNECTION_REMOVAL_REQUEST      0x82
#define CMD_DATA_REQUEST                        0x83
#define CMD_CHANNEL_HOPPING                     0x84
#define CMD_TIME_SYNCHRONIZATION_REQUEST        0x85
#define CMD_TIME_SYNCHRONIZATION_NOTIFICATION   0x86
#define CMD_P2P_ACTIVE_SCAN_REQUEST             0x87

#define CMD_TIME_SYNC_DATA_PACKET               0x8A
#define CMD_TIME_SYNC_COMMAND_PACKET            0x8B

#define CMD_P2P_CONNECTION_RESPONSE             0x91
#define CMD_P2P_CONNECTION_REMOVAL_RESPONSE     0x92
#define CMD_P2P_ACTIVE_SCAN_RESPONSE            0x97

#define CMD_MAC_DATA_REQUEST                    0x04

#if defined(ENABLE_ED_SCAN) && defined(ENABLE_FREQUENCY_AGILITY)
    #define FREQUENCY_AGILITY_STARTER
#endif

#define MICRO_SECOND_PER_COUNTER_TICK   (1000000ul / ( COUNTER_CRYSTAL_FREQ / 8 ))


/******************************************************************
 * Overview: The capacity information for a MiWi(TM) P2P device. It is the 
 * definition of the first byte of PeerInfo defined in 
 * P2P_CONNECTION_ENTRY. The highest bit also be used to indicate 
 * if the P2P connection entry is a valid entry
 *****************************************************************/
typedef union 
{
    uint8_t    Val;                            // the value of the P2P capacitity
    struct _P2P_CAPACITY_BITS
    {
        uint8_t    RXOnWhileIdel       : 1;    // if device turns on radio when idle
        uint8_t    DataRequestNeeded   : 1;    // if data request is required when device turns off radio when
                                            // idle. It is used to decide if an indirect message is necessary
                                            // to be stored.
        uint8_t    TimeSynchronization : 1;    // reserved bit for future development
        uint8_t    SecurityCapacity    : 1;    // if the device is capable of handling encrypted information
        uint8_t    ConnectionMode      : 2;
        uint8_t    filler              : 1;
        uint8_t    isValid             : 1;    // use this bit to indicate that this entry is a valid entry
    } bits;
} P2P_CAPACITY;         


/******************************************************************
 * Overview: The structure to store indirect messages for devices turn off 
 * radio when idle
 *****************************************************************/
typedef struct 
{
    MIWI_TICK       TickStart;      // start time of the indirect message. Used for checking 
                                // indirect message time out
    #if defined(IEEE_802_15_4)                                
        API_UINT16_UNION    DestPANID;      // the PAN identifier for the destination node
    #endif
    union 
    {
        uint8_t        DestLongAddress[MY_ADDRESS_LENGTH];             // unicast destination long address
        uint8_t        DestIndex[CONNECTION_SIZE];                     // broadcast index of the P2P Connection Entries
                                                                    // for destination RFD devices
    } DestAddress;  // destination address for the indirect message. Can either for unicast or broadcast
    union 
    {
        uint8_t    Val;                        // value for the flags
        struct 
        {
            uint8_t    isValid         : 1;    // if this indirect message is valid
            uint8_t    isBroadcast     : 1;    // if this indirect message is for broadcasting
            uint8_t    isCommand       : 1;    // if this indirect message a command
            uint8_t    isSecured       : 1;    // if this indirect message requires encryption
        } bits;                             // bit map of the flags
    } flags;                                // flags for indirect message
    uint8_t        PayLoadSize;                // the indirect message pay load size
    #if (TX_BUFFER_SIZE) > 110
        uint8_t    PayLoad[110];
    #else
        uint8_t    PayLoad[TX_BUFFER_SIZE];    // the indirect message pay load
    #endif
} INDIRECT_MESSAGE; 


/******************************************************************
 * Overview: structure to indicate the status of P2P stack
 *****************************************************************/
typedef union
{
    uint8_t Val;                               // The value of the P2P status flags
    struct 
    {
        uint8_t Sleeping               :1;     // indicate if the device in sleeping state
        uint8_t SaveConnection         :1;     // indicate if it is time for Network Freezer module to
                                            // save valid connections. Important because there may be
                                            // more than one response coming back and NVM operation is 
                                            // blocking
        uint8_t DataRequesting         :1;     // indicate that device is in the process of data request
                                            // from its parent. Only effective if device enables sleeping
        uint8_t RxHasUserData          :1;     // indicate if the received frame needs processing from
                                            // the application layer   
        uint8_t SearchConnection       :1;     // indicate if the stack is currently in the process of
                                            // looking for new connection
        uint8_t Resync                 :1;     // indicate if the stack is currently in the process of
                                            // resynchronizing connection with the peer device
        uint8_t Enhanced_DR_SecEn      :1;
    }bits;                                  // bit map of the P2P status
} P2P_STATUS;                               


/************************ EXTERNAL VARIABLES **********************/
extern uint8_t TxBuffer[];
extern uint8_t TxData;
extern volatile P2P_STATUS P2PStatus;
#if defined(IEEE_802_15_4)
    extern API_UINT16_UNION myPANID;
#endif
extern API_UINT16_UNION CounterVal;

/************************ FUNCTION PROTOTYPES **********************/
void    DumpConnection(INPUT uint8_t index);
bool    isSameAddress(INPUT uint8_t *Address1, INPUT uint8_t *Address2);

#endif

#endif
    
    
