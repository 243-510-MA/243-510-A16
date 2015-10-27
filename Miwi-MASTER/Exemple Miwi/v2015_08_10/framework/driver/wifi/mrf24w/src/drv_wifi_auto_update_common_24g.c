/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_wifi_auto_update_common_24g.c

  Summary:


  Description:
    FW updater common part
 
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

#if (defined(STACK_USE_AUTOUPDATE_UART)|| defined(STACK_USE_AUTOUPDATE_TCPCLIENT)) && defined(MRF24WG)

void AutoUpdate_Restore(void);

uint32_t ImageUpdate_Addr     = 0;
uint32_t ImageUpdate_Checksum = 0;
uint32_t ImageUpdate_Size     = 0;

void AutoUpdate_Initialize(void)
{   // Initialze AutoUpdate
    uint8_t buf_command[4];
    
    ImageUpdate_Addr     = 12;
    ImageUpdate_Checksum =  0;
    ImageUpdate_Size     =  0;
    putsUART("Update Image Initialize..begin\r\n");
    buf_command[1] = UPDATE_SECURITY_KEY_1; 
    buf_command[2] = UPDATE_SECURITY_KEY_2; 
    buf_command[3] = UPDATE_SECURITY_KEY_3;
    buf_command[0] = UPDATE_CMD_ERASE1;                   //Erase bank1
    SendSetParamMsg(PARAM_FLASH_update, buf_command, 4);
    
    DelayMs(000);
    buf_command[0] = UPDATE_CMD_CPY_0TO1;                 //Copy bank0 to bank1
    buf_command[1] = UPDATE_SECURITY_KEY_1; 
    buf_command[2] = UPDATE_SECURITY_KEY_2; 
    buf_command[3] = UPDATE_SECURITY_KEY_3;
    SendSetParamMsg(PARAM_FLASH_update, buf_command, 4);
    
    DelayMs(000);
    buf_command[0] = UPDATE_CMD_ERASE0;                  //Erase bank0
    buf_command[1] = UPDATE_SECURITY_KEY_1; 
    buf_command[2] = UPDATE_SECURITY_KEY_2; 
    buf_command[3] = UPDATE_SECURITY_KEY_3;
    SendSetParamMsg(PARAM_FLASH_update, buf_command, 4);
    
    DelayMs(000);
    putsUART("Update Image Initialize..Done\r\n");
}

void AutoUpdate_Completed(void)
{
    #define PATCH_SIGNATURE              (uint32_t)(~(0x00047fbc))
    uint8_t SignatureData[4 + 12];
    uint8_t buf_im[32] = {0,};
    
    ImageUpdate_Addr = 0;
    SignatureData[0] = (ImageUpdate_Addr & 0x00FF0000) >> 16;
    SignatureData[1] = (ImageUpdate_Addr & 0x0000FF00) >> 8;
    SignatureData[2] = (ImageUpdate_Addr & 0xFF);
    SignatureData[3] = 12;

    SignatureData[4 + 0] = (PATCH_SIGNATURE >> 24) & 0xFF;
    SignatureData[4 + 1] = (PATCH_SIGNATURE >> 16) & 0xFF;
    SignatureData[4 + 2] = (PATCH_SIGNATURE >>  8) & 0xFF;
    SignatureData[4 + 3] = (PATCH_SIGNATURE >>  0) & 0xFF;

    SignatureData[4 + 4] = (ImageUpdate_Size >> 24) & 0xFF;
    SignatureData[4 + 5] = (ImageUpdate_Size >> 16) & 0xFF;
    SignatureData[4 + 6] = (ImageUpdate_Size >>  8) & 0xFF;
    SignatureData[4 + 7] = (ImageUpdate_Size >>  0) & 0xFF;

    SignatureData[4 +  8] = (ImageUpdate_Checksum >> 24) & 0xFF;
    SignatureData[4 +  9] = (ImageUpdate_Checksum >> 16) & 0xFF;
    SignatureData[4 + 10] = (ImageUpdate_Checksum >>  8) & 0xFF;
    SignatureData[4 + 11] = (ImageUpdate_Checksum >>  0) & 0xFF;
    SendSetParamMsg(PARAM_FLASH_WRITE, SignatureData, sizeof(SignatureData));
    
    sprintf((char *)buf_im,"Size = 0x%x, sum=%x\r\n", (unsigned int)ImageUpdate_Size, (unsigned int)ImageUpdate_Checksum);
    putsUART((char *)buf_im);
    uint8_t buf_command[4]={UPDATE_CMD_CPY_CALIBRATION, 
                           (uint8_t)(UPDATE_SECURITY_KEY >> 16), 
                           (uint8_t)(UPDATE_SECURITY_KEY >>  8), 
                           (uint8_t)(UPDATE_SECURITY_KEY      )};          //Copy last 128 Bytes from Bank0 to Bank1
    SendSetParamMsg(PARAM_FLASH_update, buf_command, 4);

#if 0 //verify
    uint8_t verify_result = 0;
    
    SendGetParamMsg(PARAM_FLASH_VERIFY, &verify_result, 1);
    if (verify_result == 0x0A)
    {
        putsUART("Verify Checksum success !\r\n");
    }
    else
    {
        putsUART("Verify Checksum Error !. I will roll back\r\n");
        AutoUpdate_Restore();
        putsUART("Roll back done !\r\n");
    }
#endif

    putsUART("Update Image Done!\r\n");
}

void AutoUpdate_Restore(void)
{
    uint8_t buf_command[4];

    buf_command[0] = UPDATE_CMD_ERASE0; //Erase bank0
    buf_command[1] = UPDATE_SECURITY_KEY_1;
    buf_command[2] = UPDATE_SECURITY_KEY_2;
    buf_command[3] = UPDATE_SECURITY_KEY_3;
    SendSetParamMsg(PARAM_FLASH_update, buf_command, 4);
    
    buf_command[0] = UPDATE_CMD_CPY_1TO0; //Copy bank1 to bank0
    buf_command[1] = UPDATE_SECURITY_KEY_1; 
    buf_command[2] = UPDATE_SECURITY_KEY_2; 
    buf_command[3] = UPDATE_SECURITY_KEY_3;
    SendSetParamMsg(PARAM_FLASH_update, buf_command, 4);
}
#endif

