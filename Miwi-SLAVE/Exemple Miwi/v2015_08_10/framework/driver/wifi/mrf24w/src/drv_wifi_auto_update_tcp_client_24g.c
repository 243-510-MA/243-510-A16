/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_wifi_auto_update_tcp_client_24g.c

  Summary:


  Description:
    WiFi module FW auto update via TCP connection
 
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

#if defined(STACK_USE_AUTOUPDATE_TCPCLIENT) && defined(MRF24WG)

/* used for assertions */
#if defined(WF_DEBUG)
    #define WF_MODULE_NUMBER WF_MODULE_WF_AUTO_UPDATE_TCP
#endif

extern void AutoUpdate_Initialize(void);
extern void AutoUpdate_Completed(void);
extern void AutoUpdate_Restore(void);
static void parse_checksum(void);

extern bool wf_update_begin_tcp;
extern uint32_t ImageUpdate_Addr;
extern uint32_t ImageUpdate_Checksum ;
extern uint32_t ImageUpdate_Size ;

static uint8_t ServerName[] = "www.microchip.com";
static uint16_t ServerPort = 80;
static uint8_t PatchName[] = "/mrfupdates/A2Patch_310c.txt";
static uint8_t Key_authorization[] = "bXJmdXBkYXRlczptY2hwMTIzNA==" ;

uint8_t buf_checksum[12]   = {0,};
uint8_t buf_checksum_index = 0;
uint8_t parse_index        = 0;
uint8_t parse_tmp[4]       = {0,};
uint32_t signature_file;
uint32_t length_file;
uint32_t checksum_file;

static void AU_print_string(uint8_t *buf,uint8_t length)
{
    int i;
    
    for (i = 0; i < length; i++) 
        putcUART(buf[i]);

}

static void AutoUpdate_SendToModule(uint8_t c, bool all)
{
    static uint8_t buf_module[36] = {0,};
    static uint8_t index_b = 0;
    
    if (all == true)
    {
        if (index_b > 0)
        {
            buf_module[0] = (ImageUpdate_Addr & 0x00FF0000) >> 16;
            buf_module[1] = (ImageUpdate_Addr & 0x0000FF00) >> 8;
            buf_module[2] = (ImageUpdate_Addr & 0xFF);
            buf_module[3] = index_b;
            putsUART("!");
            SendSetParamMsg(PARAM_FLASH_WRITE, buf_module, index_b+4);
            ImageUpdate_Addr += index_b;
            index_b = 0;
        }
    }
    else
    {
        buf_module[index_b + 4] = c;
        index_b ++;
        if (index_b ==32)
        {
            buf_module[0] = (ImageUpdate_Addr & 0x00FF0000) >> 16;
            buf_module[1] = (ImageUpdate_Addr & 0x0000FF00) >> 8;
            buf_module[2] = (ImageUpdate_Addr & 0xFF);
            buf_module[3] = 32;
            putsUART(".");
            SendSetParamMsg(PARAM_FLASH_WRITE, buf_module, index_b + 4);
            ImageUpdate_Addr += 32;
            index_b = 0;
        }
    }
}

uint8_t char_to_i(char c)
{
    uint8_t tmp = 0;
    
    if ((c >= '0')&&(c <= '9'))       
        tmp = c-'0';
    else if ((c >='a')&&(c <= 'f'))  
        tmp = c-'a'+0x0a;
    else if ((c >='A')&&(c <= 'F'))  
        tmp = c-'A'+0x0a;
    else
    {
        putrsUART("this is not data\r\n");
        WF_ASSERT(false);
    }
    
    return tmp;
}

static void parse_byte(uint8_t c)
{
    uint8_t real_byte;

    if ((c == ',') || (c == 0x0a) || (c == 0x20))
    {
        parse_index = 0;
        return;
    }

    switch (parse_index) {
    case 0:
        if (c == '0')
        {
            parse_tmp[0] = '0';
            parse_index ++;
        }
        break;
    
    case 1:
        if (c == 'x')
        {
            parse_tmp[1] = 'x';
            parse_index ++;
        }
        else
        {    putrsUART("it shold be 'x'");
             WF_ASSERT(false);
        }
        break;
    
    case 2:
        parse_tmp[2] = char_to_i(c);
        parse_index ++;
        break;
        
    case 3:
        parse_tmp[3] = char_to_i(c);
        parse_index ++;

        real_byte = parse_tmp[2]<<4;
        real_byte += parse_tmp[3];

        if (buf_checksum_index < 12)  // it is head
        {
            buf_checksum[buf_checksum_index] = real_byte;
            buf_checksum_index++;
            if (buf_checksum_index == 12)
            {
                parse_checksum();
                putrsUART("----------------------------\r\n");
            }
        }
        else      //it is real bin data
        {
        //Size and CheckSum
            if ((ImageUpdate_Size % 4u) == 0u) ImageUpdate_Checksum += (uint32_t)real_byte << 24;
            if ((ImageUpdate_Size % 4u) == 1u) ImageUpdate_Checksum += (uint32_t)real_byte << 16;
            if ((ImageUpdate_Size % 4u) == 2u) ImageUpdate_Checksum += (uint32_t)real_byte << 8;
            if ((ImageUpdate_Size % 4u) == 3u) ImageUpdate_Checksum += (uint32_t)real_byte;
            ImageUpdate_Size ++;

            AutoUpdate_SendToModule(real_byte,false);
        }
        
        break;
        
    case 4:
        parse_tmp[0] = 0; 
        parse_tmp[1] = 0;
        parse_tmp[2] = 0;
        parse_tmp[3] = 0;

    default:
        break;

    }

}

static void parse_checksum(void)
{
    //int i;

    signature_file  = (uint32_t)buf_checksum[0] << 24;
    signature_file += (uint32_t)buf_checksum[1] << 16;
    signature_file += (uint32_t)buf_checksum[2] <<  8;
    signature_file += (uint32_t)buf_checksum[3] <<  0;

    length_file  = (uint32_t)buf_checksum[4] << 24;
    length_file += (uint32_t)buf_checksum[5] << 16;
    length_file += (uint32_t)buf_checksum[6] <<  8;
    length_file += (uint32_t)buf_checksum[7] <<  0;

    checksum_file  = (uint32_t)buf_checksum[8]  << 24;
    checksum_file += (uint32_t)buf_checksum[9]  << 16 ;
    checksum_file += (uint32_t)buf_checksum[10] <<  8 ;
    checksum_file += (uint32_t)buf_checksum[11] <<  0;

    //for (i = 0; i < 24; i++) 
    //   {SYS_CONSOLE_PRINT("%02x;", buf_checksum[i]); } 
    //SYS_CONSOLE_MESSAGE("\r\n");

    //SYS_CONSOLE_PRINT("signature = 0x%x\r\n", signature_file);
    //SYS_CONSOLE_PRINT("length = 0x%x\r\n",    length_file);
    //SYS_CONSOLE_PRINT("checksum = 0x%x\r\n",  checksum_file);
}

/*****************************************************************************
  Function:
    void AutoUpdate_TCPClient(void)

  Summary:

  Description:

  Precondition:
    TCP is initialized.

  Parameters:
    None

  Returns:
    None
  ***************************************************************************/

void AutoUpdate_TCPClient(void)
{
    uint8_t             i,j;
    uint16_t            w;
    uint8_t             vBuffer[32];
    static uint32_t     Timer;
    static TCP_SOCKET   MySocket = INVALID_SOCKET;
    static int SizeOfPackage     = 0;
    static int SizeOfReceive     = 0;
    uint16_t lenA,lenB;
    static enum _GenericTCPExampleState
    {
        SM_UPDATE_HOME = 0,
        SM_UPDATE_SOCKET_OBTAINED,
        SM_UPDATE_GET_SIZE_1,
        SM_UPDATE_GET_SIZE_2,
        SM_UPDATE_SKIP_TO_DATA,
        SM_UPDATE_READ_DATA,
        SM_UPDATE_DISCONNECT,
        SM_UPDATE_DONE
    } AutoUpdateTCPExampleState = SM_UPDATE_DONE;
    
    switch (AutoUpdateTCPExampleState) {
    case SM_UPDATE_HOME:
        // Connect a socket to the remote TCP server
        MySocket = TCPOpen((uint32_t)((unsigned int)&ServerName[0]), TCP_OPEN_RAM_HOST, ServerPort, TCP_PURPOSE_AUTOUPDATE_TCP_CLIENT);

        // Abort operation if no TCP socket of type TCP_PURPOSE_AUTOUPDATE_TCP_CLIENT is available
        // If this ever happens, you need to go add one to tcpip_config.h
        if (MySocket == INVALID_SOCKET)
            break;

        #if defined(STACK_USE_UART)
        putrsUART((ROM char*)"\r\n\r\nUpdate using Microchip TCP API...\r\n");
        #endif

        for (i = 0; i < 12; i++) 
            buf_checksum[i] = 0;
            
        buf_checksum_index = 0;
        for (i = 0; i < 4; i++) 
            parse_tmp[i] = 0;
        
        parse_index = 0;
        SizeOfReceive = 0;

        AutoUpdateTCPExampleState++;
        Timer = TickGet();
        break;

    case SM_UPDATE_SOCKET_OBTAINED:
        // Wait for the remote server to accept our connection request
        if (!TCPIsConnected(MySocket))
        {
            // Time out if too much time is spent in this state
            if (TickGet() - Timer > 5 * TICK_SECOND)
            {
                // Close the socket so it can be used by other modules
                TCPDisconnect(MySocket);
                MySocket = INVALID_SOCKET;
                AutoUpdateTCPExampleState--;
            }
            break;
        }
        AutoUpdate_Initialize();
        Timer = TickGet();

        // Make certain the socket can be written to
        if (TCPIsPutReady(MySocket) < 125u)
            break;
        
        TCPPutROMString(MySocket, (ROM uint8_t*)"GET ");
        TCPPutROMString(MySocket, (ROM uint8_t*)PatchName);
        TCPPutROMString(MySocket, (ROM uint8_t*)" HTTP/1.1\r\nHost:");
        TCPPutROMString(MySocket, (ROM uint8_t*)ServerName);
        TCPPutROMString(MySocket, (ROM uint8_t*)"\r\nConnection: close\r\n");
        TCPPutROMString(MySocket, (ROM uint8_t*)"Authorization: Basic ");
        TCPPutROMString(MySocket, (ROM uint8_t*)Key_authorization);
        TCPPutROMString(MySocket, (ROM uint8_t*)"\r\n\r\n");

        // Send the packet
        TCPFlush(MySocket);

        AutoUpdateTCPExampleState++;
        break;
    
    case SM_UPDATE_GET_SIZE_1:  //Looking for "Length:"
        if (!TCPIsConnected(MySocket))
        {
            AutoUpdateTCPExampleState = SM_UPDATE_DISCONNECT;
            break;
        }
        // 2. Check ,if there is data?
        w = TCPIsGetReady(MySocket);
        if (w == 0) 
            break;

        lenA = TCPFindROMArray(MySocket, (ROM uint8_t*)"Length:", 7, 0, false);
        if (lenA != 0xffff)
        {// Found it, so remove all data up to and including
            while (lenA > 0)
            {
                lenB = TCPGetArray(MySocket, vBuffer, ((lenA <= sizeof(vBuffer)) ? lenA : sizeof(vBuffer)));
                AU_print_string(vBuffer,lenB);
                lenA -= lenB;
            }
            AutoUpdateTCPExampleState = SM_UPDATE_GET_SIZE_2;

            // No break if we found the "Length:"
        }
        else
        {// Otherwise, remove as much as possible
            w -= 7;
            while (w > 0)
            {
                lenB = TCPGetArray(MySocket, vBuffer, ((w <= sizeof(vBuffer)) ? w : sizeof(vBuffer)));
                AU_print_string(vBuffer,lenB);
                w -= lenB;
            }
            break;
        }
        
    case SM_UPDATE_GET_SIZE_2:  //Looking for "\r\n",if yes, we get the size of package
        if (!TCPIsConnected(MySocket))
        {
            AutoUpdateTCPExampleState = SM_UPDATE_DISCONNECT;
            break;
        }
        // 2. Check ,if there is data?
        w = TCPIsGetReady(MySocket);
        if (w == 0) 
            break;
        
        lenA = TCPFindROMArray(MySocket, (ROM uint8_t*)"\r\n", 2, 0, false);
        if (lenA != 0xffff)
        {// Found it, get the size
            lenA = TCPGetArray(MySocket, vBuffer, lenA + 2);
            AU_print_string(vBuffer, lenA);
            sscanf((const char*)vBuffer, "Length: %d\r\n", &SizeOfPackage);
            //{char buf_t[16];sprintf(buf_t,"\r\n size=%d\r\n",SizeOfPackage);putsUART(buf_t);}
            AutoUpdateTCPExampleState = SM_UPDATE_SKIP_TO_DATA;
            // No break if we found the size
        }
        else
        {// Otherwise, wait to next read
            break;
        }

    case SM_UPDATE_SKIP_TO_DATA: // look for "\r\n\r\n"
        // 1. Check to see if the remote node has disconnected from us or sent us any application data
        if (!TCPIsConnected(MySocket))
        {
            AutoUpdateTCPExampleState = SM_UPDATE_DISCONNECT;
            // Do not break;  We might still have data in the TCP RX FIFO waiting for us
        }

        // 2. Check ,if there is data?
        w = TCPIsGetReady(MySocket);
        if (w == 0) 
            break;

        lenA = TCPFindROMArray(MySocket, (ROM uint8_t*)"\r\n\r\n", 4, 0, false);

        if (lenA != 0xffff)
        {// Found it, so remove all data up to and including
            while (lenA > 0)
            {
                lenB = TCPGetArray(MySocket, vBuffer, ((lenA <= sizeof(vBuffer)) ? lenA : sizeof(vBuffer)));
                AU_print_string(vBuffer, lenB);
                lenA -= lenB;
            }
            lenB = TCPGetArray(MySocket, vBuffer, 4);
            AU_print_string(vBuffer, lenB);
            AutoUpdateTCPExampleState = SM_UPDATE_READ_DATA;

            // No break if we found the header terminator
        }
        else
        {// Otherwise, remove as much as possible

            w -= 4;
            while (w > 0)
            {
                lenB = TCPGetArray(MySocket, vBuffer, ((w <= sizeof(vBuffer)) ? w : sizeof(vBuffer)));
                AU_print_string(vBuffer, lenB);
                w -= lenB;
            }
            //
            break;
        }

    case SM_UPDATE_READ_DATA:
        if (!TCPIsConnected(MySocket))
        {
            AutoUpdateTCPExampleState = SM_UPDATE_DISCONNECT;
            // Do not break;  We might still have data in the TCP RX FIFO waiting for us
        }
        
        // Get count of RX bytes waiting
        w = TCPIsGetReady(MySocket);
        j = sizeof(vBuffer) - 1;
        if (w != 0)
        { 
            char buf_t[30]; 
            
            putsUART("-!\r\n!-Total="); 
            sprintf(buf_t, "%u,Current=%ud, size=%u: ", SizeOfPackage, SizeOfReceive,w);
            putsUART(buf_t);
        }
        
        while (w)
        {
            int i;
            
            if (w < j)
            {
                j = w; 
            }
            
            vBuffer[j] = '\0';
            lenB= TCPGetArray(MySocket, vBuffer, j);
            SizeOfReceive += lenB;
            w -= lenB;

            for (i = 0; i < lenB; i++)
            {
                parse_byte(vBuffer[i]);
            }

        }
        break;
        
    case SM_UPDATE_DISCONNECT:
        putsUART("\r\nClosed---\r\n");
        TCPDisconnect(MySocket);
        MySocket = INVALID_SOCKET;
        AutoUpdateTCPExampleState = SM_UPDATE_DONE;

        //did we receive enough data? Is the size correct?
        if ((length_file == ImageUpdate_Size) && (checksum_file == ImageUpdate_Checksum))
        {
            AutoUpdate_SendToModule(0, true);  //Send all data left in buffer
            AutoUpdate_Completed();
        }
        else   //We lost data, so cancel , and roll back
        {
            putsUART("Revert begin...\r\n");
            AutoUpdate_Restore();
            putsUART("Revert Done\r\n");
        }
        break;

    case SM_UPDATE_DONE:
        // Do nothing unless the user pushes BUTTON1 and wants to restart the whole connection/download process
#if 0  //use button to kick
        if (BUTTON2_IO == 0u)
        {
            AutoUpdateTCPExampleState = SM_UPDATE_HOME;
        }
#else  // use console command to kick
        if (true == wf_update_begin_tcp)
        {
            wf_update_begin_tcp = false;
            AutoUpdateTCPExampleState = SM_UPDATE_HOME;
        }
#endif
        break;
    }
}

#endif
