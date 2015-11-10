/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_wifi_auto_update_uart_24g.c

  Summary:


  Description:
    WiFi module FW auto update via UART connection
 
 *******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) <2014> released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
//DOM-IGNORE-END

#include "tcpip/tcpip.h"
#include "main.h"

#if defined(STACK_USE_AUTOUPDATE_UART) && defined(MRF24WG)

/* used for assertions */
#if defined(WF_DEBUG)
    #define WF_MODULE_NUMBER WF_MODULE_WF_AUTO_UPDATE_UART
#endif

#define MAX_USER_RESPONSE_LEN   (20u)

#define XMODEM_SOH      0x01u
#define XMODEM_EOT      0x04u
#define XMODEM_ACK      0x06u
#define XMODEM_NAK      0x15u
#define XMODEM_CAN      0x18u
#define XMODEM_BLOCK_LEN 128u

extern void AutoUpdate_Initialize(void);
extern void AutoUpdate_Completed(void);
extern void AutoUpdate_Restore(void);

extern uint32_t ImageUpdate_Addr;
extern uint32_t ImageUpdate_Checksum;
extern uint32_t ImageUpdate_Size;
extern bool wf_update_begin_uart;

static void XMODEM_SendToModule_subAPI(uint8_t *buf)
{
    int i;
    uint8_t buf_module[36];

    buf_module[0] = (ImageUpdate_Addr & 0x00FF0000) >> 16;
    buf_module[1] = (ImageUpdate_Addr & 0x0000FF00) >>  8;
    buf_module[2] = (ImageUpdate_Addr & 0xFF);
    buf_module[3] = 32;
    for (i = 0; i < 32; i++) 
        buf_module[i+4] = buf[i];
   
    SendSetParamMsg(PARAM_FLASH_WRITE, buf_module, 36);
    ImageUpdate_Addr += 32;
}

static void XMODEM_SendToModule(uint8_t *xmodm_buf)
{
    int i;

    //  1. Calculate checksum
    for (i = 0; i < 128; i++)
    {
        if ((ImageUpdate_Size % 4) == 0) ImageUpdate_Checksum += (uint32_t)xmodm_buf[i] << 24;
        if ((ImageUpdate_Size % 4) == 1) ImageUpdate_Checksum += (uint32_t)xmodm_buf[i] << 16;
        if ((ImageUpdate_Size % 4) == 2) ImageUpdate_Checksum += (uint32_t)xmodm_buf[i] <<  8;
        if ((ImageUpdate_Size % 4) == 3) ImageUpdate_Checksum += (uint32_t)xmodm_buf[i];
        ImageUpdate_Size ++;
    }
    
    // 2. send 128 bytes
    XMODEM_SendToModule_subAPI(&xmodm_buf[0]);
    XMODEM_SendToModule_subAPI(&xmodm_buf[32]);
    XMODEM_SendToModule_subAPI(&xmodm_buf[64]);
    XMODEM_SendToModule_subAPI(&xmodm_buf[96]);
}
static uint8_t tempData[XMODEM_BLOCK_LEN];

bool    AutoUpdate_UartXMODEM_24G(void)
{
    enum SM_FIRMWARE_UPDATE
    {
        SM_FIRMWARE_UPDATE_SOH,
        SM_FIRMWARE_UPDATE_BLOCK,
        SM_FIRMWARE_UPDATE_BLOCK_CMP,
        SM_FIRMWARE_UPDATE_DATA,
        SM_FIRMWARE_UPDATE_CHECKSUM,
        SM_FIRMWARE_UPDATE_FINISH,
    } state;

    uint8_t c;
   // MPFS_HANDLE handle;
    bool lbDone;
    uint8_t blockLen    = 0;
    bool lResult        = false;
    uint8_t BlockNumber = 0;
    uint8_t preBlockNum = 0;
    uint8_t checksum    = 0;

    uint32_t lastTick;
    uint32_t currentTick;
    state = SM_FIRMWARE_UPDATE_SOH;
    lbDone = false;

#if 0 //use button to begin update
    if (BUTTON3_IO == 1u) 
        return false;

    putsUART("\n\rPress S2 (on Explorer16) to start the update.\n\r");
    while (BUTTON2_IO == 1u)
        ;
    
#else //use console command to begin update
    if (false == wf_update_begin_uart) 
        return false;
            
    wf_update_begin_uart = false;
#endif

    MACInit();
    DelayMs(100);
    AutoUpdate_Initialize();
    putsUART("Please initiate file transfer of firmware patch by XMODEM.\r\n");
    putsUART("If S3 pressed (on Explorer16), update will stop and previous image will be restored.\r\n");
    lastTick = TickGet();
    do
    {
        currentTick = TickGet();
        if (currentTick - lastTick >= (TICK_SECOND * 2))
        {
            lastTick = TickGet();
            while (BusyUART())
                ;
            
            WriteUART(XMODEM_NAK);
        }

    } while (!DataRdyUART());

    while (!lbDone)
    {
        if (BUTTON3_IO == 0u)   // If you want to cancel AutoUpdate, please press S3
        {
            putsUART("You press S3 button, revert begin...\r\n");
            AutoUpdate_Restore();
            putsUART("revert done\r\n");
            return false;
        }
        
        if (DataRdyUART())
        {
            c = ReadUART();
            lastTick = TickGet();
        }
        else
        {
            // put some timeout to make sure  that we do not wait forever.
            currentTick = TickGet();
            if (currentTick - lastTick >= (TICK_SECOND * 10))
            {
                //if time out, copy old patch image from bank2 to bank1
                putsUART("timeout, revert begin...\r\n");
                AutoUpdate_Restore();
                putsUART("revert done\r\n");
                return false;
            }
            continue;
        }
        
        switch (state) {
        case SM_FIRMWARE_UPDATE_SOH:
            if (c == XMODEM_SOH)
            {
                state = SM_FIRMWARE_UPDATE_BLOCK;
                checksum = c;
                lResult = true;
            }
            else if (c == XMODEM_EOT)
            {
                state = SM_FIRMWARE_UPDATE_FINISH;

                while (BusyUART())
                    ;
                    
                WriteUART(XMODEM_ACK);
                lbDone = true;
            }
            else
            {
                WF_ASSERT(false);
            }
            
            break;
        
        case SM_FIRMWARE_UPDATE_BLOCK:
            BlockNumber = c;
            checksum += c;
            state = SM_FIRMWARE_UPDATE_BLOCK_CMP;
            break;

        case SM_FIRMWARE_UPDATE_BLOCK_CMP:
            //Judge: Is it correct ?
            if (c != (BlockNumber ^ 0xFF))
            {
                lResult = false;
            }
            else
            {
                if ((uint8_t)(preBlockNum + 1) != BlockNumber)
                {
                    lResult = false;
                }
            }
            
            checksum += c;
            blockLen = 0;
            state = SM_FIRMWARE_UPDATE_DATA;
            break;
        
        case SM_FIRMWARE_UPDATE_DATA:
            // Buffer block data until it is over.
            tempData[blockLen++] = c;
            if (blockLen == XMODEM_BLOCK_LEN)
            {
                state = SM_FIRMWARE_UPDATE_CHECKSUM;
            }
            
            checksum += c;
            break;
            
        case SM_FIRMWARE_UPDATE_CHECKSUM:
            if (checksum != c)
            {
                lResult = false;
            }
            
            XMODEM_SendToModule(tempData);
            while (BusyUART())
                ;
            
            if (lResult == true)
            {
                WriteUART(XMODEM_ACK);
                preBlockNum++;
            }
            else
            {
                WriteUART(XMODEM_NAK);
            }
            
            state = SM_FIRMWARE_UPDATE_SOH;
            break;

        default:
            WF_ASSERT(false);
                
            break;
        }

    }

    AutoUpdate_Completed();

    return true;
}

#endif
