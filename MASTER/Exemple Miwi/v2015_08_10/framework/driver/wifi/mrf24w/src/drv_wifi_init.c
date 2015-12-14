/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_wifi_init.c

  Summary:
    Module for Microchip TCP/IP Stack
    -Provides access to MRF24W WiFi controller
    -Reference: MRF24W Data sheet, IEEE 802.11 Standard

  Description:
    MRF24W Driver Initialization

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

/*******************************************************************************
*                          INCLUDES
********************************************************************************/

#include "tcpip/tcpip.h"

#if defined(WF_CS_TRIS)

#if !defined(MRF24WG)
    bool gRFModuleVer1209orLater = false;
#endif

#if defined(WF_CONSOLE)
    #include "drv_wifi_console.h"
    #include "iperf_app.h"
#endif

/*******************************************************************************
*                                           DEFINES
********************************************************************************/

/* used for assertions */
#if defined(WF_DEBUG)
    #define WF_MODULE_NUMBER    WF_MODULE_WF_INIT
#endif

/* This MAC address is the default MAC address used in tcpip_config.h.  If the */
/* user leaves this MAC address unchanged then the WiFi Driver will get the   */
/* unique MAC address from the MRF24W and have the stack use it.           */
#define MCHP_DEFAULT_MAC_ADDRESS_BYTE_1     (0x00)
#define MCHP_DEFAULT_MAC_ADDRESS_BYTE_2     (0x04)
#define MCHP_DEFAULT_MAC_ADDRESS_BYTE_3     (0xa3)
#define MCHP_DEFAULT_MAC_ADDRESS_BYTE_4     (0x00)
#define MCHP_DEFAULT_MAC_ADDRESS_BYTE_5     (0x00)
#define MCHP_DEFAULT_MAC_ADDRESS_BYTE_6     (0x00)

/*******************************************************************************
*                      LOCAL GLOBAL VARIABLES
********************************************************************************/

/* This MAC address is the default MAC address used in tcpip_config.h.  If the */
/* user leaves this MAC address unchanged then the WiFi Driver will get the  */
/* unique MAC address from the MRF24W and have the stack use it.              */
static const uint8_t MchpDefaultMacAddress[WF_MAC_ADDRESS_LENGTH] = {0x00u, 0x04u, 0xA3u, 0x00u, 0x00u, 0x00u};

/*******************************************************************************
*                           LOCAL FUNCTION PROTOTYPES
********************************************************************************/

static void WF_LibInitialize(void);

extern void SetAppPowerSaveMode(bool state);

#if defined(CONSOLE)
extern void IperfAppInit(void);
#endif

void SetDhcpProgressState(void);

/*****************************************************************************
 * FUNCTION: WF_Init
 *
 * RETURNS: None
 *
 * PARAMS:
 *      N/A.
 *
 *
 *  NOTES: This function must be called once prior to calling any other WF...
 *          functions.  This function initializes the Wifi Driver internal State.
 *          It also verifies functionality of the lower level SPI driver and
 *          connected hardware.
 *****************************************************************************/
void WF_Init(void)
{
    tWFDeviceInfo deviceInfo;

    WFHardwareInit();
    RawInit();

    WFEnableMRF24WB0MMode();
    WF_GetDeviceInfo(&deviceInfo);

    // if MRF24WB
    #if !defined(MRF24WG)
        WF_ASSERT(deviceInfo.romVersion == 0x12);
        WF_ASSERT(deviceInfo.patchVersion >= 0x02);
        if (deviceInfo.romVersion == 0x12 && deviceInfo.patchVersion >= 0x09)
        {
            gRFModuleVer1209orLater = true;
        }
    #else // must be a MRF24WG
        WF_ASSERT(deviceInfo.romVersion == 0x30 || deviceInfo.romVersion == 0x31 || deviceInfo.romVersion == 0x32);
    #endif

    /* send init messages to MRF24W */
    WF_LibInitialize();

    #if defined(WF_CONSOLE)
        WFConsoleInit();
        #if defined(WF_CONSOLE_DEMO) || (defined(WF_EASY_CONFIG_DEMO) && defined(__XC32))
            IperfAppInit();
        #endif
    #endif

    if (AppConfig.Flags.bIsDHCPEnabled)
    {
        SetDhcpProgressState();
    }
}

extern void WFMgmtMessageTest();
/*****************************************************************************
 * FUNCTION: WF_LibInitialize
 *
 * RETURNS:  None
 *
 * PARAMS:   None
 *
 *  NOTES:   Performs initialization which is specific to the Microchip Demo code.
 *****************************************************************************/
static void WF_LibInitialize()
{
    /* Disable Tx Data confirms (from the MRF24W) */
    WF_SetTxDataConfirm(WF_DISABLED);

    /* if the user has left the default MAC address in tcpip_config.h unchanged then use */
    /* the unique MRF24W MAC address so prevent multiple devices from having the same   */
    /* MAC address.                                                                     */
    if ( memcmp((void *)AppConfig.MyMACAddr.v, (void *)MchpDefaultMacAddress, WF_MAC_ADDRESS_LENGTH) == 0)
    {
        /* get the MRF24W MAC address and overwrite the MAC in AppConfig */
        WF_GetMacAddress((uint8_t *)AppConfig.MyMACAddr.v);
    }
    /* else presume the user has a unique MAC address of their own that they wish to use */
    else
    {
        // set MAC address with user-supplied MAC */
        WF_SetMacAddress((uint8_t *)AppConfig.MyMACAddr.v);
    }

    #ifdef WF_CONFIG_DHCP
    WF_SET_DHCP_STATE(DHCP_ENABLED);
    #endif

}

#endif /* WF_CS_TRIS */
