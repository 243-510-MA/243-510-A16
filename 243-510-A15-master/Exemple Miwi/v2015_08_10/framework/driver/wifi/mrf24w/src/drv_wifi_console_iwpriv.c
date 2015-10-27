/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_wifi_console_iwpriv.c

  Summary:
    Module for Microchip TCP/IP Stack
    -Provides access to MRF24W10C WiFi controller
    -Reference: MRF24W10C Data sheet, IEEE 802.11 Standard

  Description:
    MRF24W10C Driver Iwpriv

 ******************************************************************************/

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
 *                               INCLUDES
 *******************************************************************************/
#include <ctype.h>

#include "tcpip/tcpip.h"

#include "drv_wifi_console.h"

#if defined( WF_CONSOLE_IFCFGUTIL )

#include "drv_wifi_console_msg.h"
#include "drv_wifi_console_msg_handler.h"
#include "drv_wifi_console_iwconfig.h" // for using iwconfigCb
#include "drv_wifi_console_iwpriv.h"

/*******************************************************************************
 *                              CONSTANTS
 *******************************************************************************/

#define IWPRIV_WEB_KEY_NUM          (4u)
#define IWPRIV_WEB_LONG_KEY_LEN     (13u) // in byte
#define IWPRIV_WEB_SHORT_KEY_LEN    (5u)  // in byte

/*******************************************************************************
 *                              GLOBALS
 *******************************************************************************/

static bool iwprivCbInitialized = false;
static struct
{
    // NOTE: cpId, ssid, and wepDefaultKeyId
    // are refreshed at each iwpriv

    uint8_t cpId; // conn. profile ID
    uint8_t ssid[WF_MAX_SSID_LENGTH + 1];
    uint8_t wepDefaultKeyId;

    // NOTE: securityType, securityKey and securityKeyLength are
    // not refreshed at each iwpriv

    uint8_t securityType;
    uint8_t securityKey[WF_MAX_SECURITY_KEY_LENGTH + 1];
    uint8_t securityKeyLength;
} iwprivCb;

extern tWFHibernate WF_hibernate;

/*******************************************************************************
 *                     LOCAL FUNCTION PROTOTYPES
 *******************************************************************************/

/*****************************************************************************
 * FUNCTION: iwprivSetCb
 *
 * RETURNS: true or false
 *
 * PARAMS: none
 *
 * NOTES: Set the iwprivCb structure
 *****************************************************************************/
static bool iwprivSetCb(void)
{
    tWFCPElements cprof;
    bool cpIdChanged = false;

    if ( !iwprivCbInitialized ) // first time call of iwprivSetCb
    {
        memset(&iwprivCb, 0, sizeof(iwprivCb));
        iwprivCbInitialized = true;
    }

    if (!WF_hibernate.state && !iwconfigSetCb() ) // first set iwconfigCb
        return false;

    if ( iwprivCb.cpId != iwconfigCb.cpId)
    {
        iwprivCb.cpId = iwconfigCb.cpId;
        cpIdChanged = true;
    }

    WF_CPGetElements(iwprivCb.cpId, &cprof);

    // set refreshable part of iwprivCb
    {
        memcpy((void*)iwprivCb.ssid, (const void*)cprof.ssid, cprof.ssidLength);
        iwprivCb.ssid[cprof.ssidLength] = '\0';

        iwprivCb.wepDefaultKeyId = cprof.wepDefaultKeyId;
    }

    // set non-refreshable part of iwprivCb only when cpId has changed
    if (cpIdChanged)
    {
        iwprivCb.securityType = cprof.securityType;
        iwprivCb.securityKeyLength = 0;
    }

    return true;
}

/*****************************************************************************
 * FUNCTION: iwprivDisplayStatus
 *
 * RETURNS: none
 *
 * PARAMS: none
 *
 * NOTES: Display security mode status
 *         Valid security mode:
 *          Open
 *          WEP40
 *          WEP104
 *          WPA-PSK/WPA2-PSK Auto with Key
 *          WPA-PSK/WPA2-PSK Auto with Passphrase
 *****************************************************************************/
static void iwprivDisplayStatus(void)
{
    uint8_t i, j;
    uint8_t * p;

    // security type
    {
        WFConsolePrintRomStr("Encryption: ", false);

        switch (iwprivCb.securityType)
        {
        case WF_SECURITY_OPEN:
            WFConsolePrintRomStr("Open", true);
            break;
        case WF_SECURITY_WEP_40:
            WFConsolePrintRomStr("WEP40", true);
            break;
        case WF_SECURITY_WEP_104:
            WFConsolePrintRomStr("WEP104", true);
            break;
        case WF_SECURITY_WPA_AUTO_WITH_KEY:
            WFConsolePrintRomStr("WPA-PSK/WPA2-PSK Auto with Key", true);
            break;
        case WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE:
            WFConsolePrintRomStr("WPA-PSK/WPA2-PSK Auto with Passphrase", true);
            break;
        default:
            WFConsolePrintRomStr("Unknown", true);
            return;
        }
    }

    if ( iwprivCb.securityType == WF_SECURITY_WEP_40 || iwprivCb.securityType == WF_SECURITY_WEP_104 )
    {
        uint8_t webKeyLen;

        if ( iwprivCb.securityKeyLength == (IWPRIV_WEB_KEY_NUM * IWPRIV_WEB_LONG_KEY_LEN) )
        {
            webKeyLen = IWPRIV_WEB_LONG_KEY_LEN;
        }
        else if ( iwprivCb.securityKeyLength == (IWPRIV_WEB_KEY_NUM * IWPRIV_WEB_SHORT_KEY_LEN) )
        {
            webKeyLen = IWPRIV_WEB_SHORT_KEY_LEN;
        }
        else
        {
            WFConsolePrintRomStr("  WEP Key: not yet set or unknown", true);
            return;
        }

        p = iwprivCb.securityKey;
        for (j = 0; j < IWPRIV_WEB_KEY_NUM; j++)
        {
            if (j == iwprivCb.wepDefaultKeyId )
                WFConsolePrintRomStr(" *", false);
            else
                WFConsolePrintRomStr("  ", false);

            WFConsolePrintRomStr("WEP Key[", false);
            WFConsolePrintInteger(j + 1, false);
            WFConsolePrintRomStr("]:  0x", false);

            for (i = 0; i < webKeyLen ; i++)
            {
                sprintf( (char *) g_ConsoleContext.txBuf,
                    "%.2x", *p++);
                WFConsolePrintRamStr((char *) g_ConsoleContext.txBuf, false);
            }

            WFConsolePrintRomStr("", true);
        }
    }
    else if ( iwprivCb.securityType == WF_SECURITY_WPA_AUTO_WITH_KEY )
    {
        if ( iwprivCb.securityKeyLength != WF_WPA_KEY_LENGTH )
        {
            WFConsolePrintRomStr("  PSK: not yet set or unknown", true);
            return;
        }

        putrsUART("  PSK: \"");

        p = iwprivCb.securityKey;
        for (j = 0; j < WF_WPA_KEY_LENGTH; j++)
        {
            sprintf( (char *) g_ConsoleContext.txBuf, "%.2x", *p++);
            WFConsolePrintRamStr( (char *) g_ConsoleContext.txBuf , false );
        }

        WFConsolePrintRomStr("", true);
    }
    else if ( iwprivCb.securityType == WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE )
    {
        if ( iwprivCb.securityKeyLength == 0 )
        {
            WFConsolePrintRomStr("  Passphrase: not yet set or unknown", true);
            return;
        }

        WFConsolePrintRomStr("  Passphrase: \"", false);

        p = iwprivCb.securityKey;
        for (j = 0; j < iwprivCb.securityKeyLength; j++ )
        {
            sprintf( (char *) g_ConsoleContext.txBuf, "%c", *p++);
            WFConsolePrintRamStr( (char *) g_ConsoleContext.txBuf, false );
        }

        WFConsolePrintRomStr("\"", true);

        WFConsolePrintRomStr("  SSID: ", false);
        WFConsolePrintRamStr(iwprivCb.ssid, true);
    }
}

/*****************************************************************************
 * FUNCTION: iwprivSetEnc
 *
 * RETURNS: true or false
 *
 * PARAMS: none
 *
 * NOTES: Set security mode
 *         Valid security mode:
 *          Open
 *          WEP40
 *          WEP104
 *          WPA-PSK/WPA2-PSK Auto with Key
 *          WPA-PSK/WPA2-PSK Auto with Passphrase
 *****************************************************************************/
static bool iwprivSetEnc(void)
{
    uint8_t securityType;

    if (ARGC < 3u)
    {
        WFConsolePrintRomStr("Missing value for last parameter", true);
        return false;
    }

    if ( (3u <= ARGC) && (strcmppgm2ram((char *)ARGV[2], "OPEN") == 0) )
    {
        securityType = WF_SECURITY_OPEN;
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char *)ARGV[2], "WEP40") == 0) )
    {
        securityType = WF_SECURITY_WEP_40;
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char *)ARGV[2], "WEP104") == 0) )
    {
        securityType = WF_SECURITY_WEP_104;
    }    
    else if ( (3u <= ARGC) && (strcmppgm2ram((char *)ARGV[2], "WPA-PSK/WPA2-PSK AUTO with KEY") == 0) )
    {
        securityType = WF_SECURITY_WPA_AUTO_WITH_KEY;
    }
    else if ( (3u <= ARGC) && (strcmppgm2ram((char *)ARGV[2], "WPA-PSK/WPA2-PSK Auto with Passphrase") == 0) )
    {
        securityType = WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE;
    }
    else
    {
        WFConsolePrintRomStr("Unknown parameter", true);
        return false;
    }

    if ( iwprivCb.securityType != securityType ) // security type changed
    {   // reset the security context
        memset(iwprivCb.securityKey, 0, sizeof(iwprivCb.securityKey));
        iwprivCb.securityKeyLength = 0;
    }

    iwprivCb.securityType = securityType; // save the security type

    if (iwprivCb.securityType == WF_SECURITY_OPEN)
    {
        WF_CPSetSecurity(iwprivCb.cpId, iwprivCb.securityType, 0, NULL, 0);
    }

    return true;
}

/*****************************************************************************
 * FUNCTION: iwprivSetKey
 *
 * RETURNS: true or false
 *
 * PARAMS: none
 *
 * NOTES: Set WEP key
 *         Valid key:
 *          WEP40 Key - 5 bytes Hex numbers 
 *          WEP104 Key - 13 bytes Hex numbers
 *****************************************************************************/
static bool iwprivSetKey(void)
{
    uint8_t webKey;

    if (iwprivCb.securityType != WF_SECURITY_WEP_40 && iwprivCb.securityType != WF_SECURITY_WEP_104)
    {
        WFConsolePrintRomStr("WEP encryption mode is not selected", true);
        return false;
    }

    if (ARGC < 3u)
    {
        WFConsolePrintRomStr("Missing value for last parameter", true);
        return false;
    }

    if ( (3u <= ARGC) && (strcmppgm2ram((char *)ARGV[2], "[1]") == 0) )
    {
        webKey = 0u;
    }
    else
    {
        WFConsolePrintRomStr("WEP key index not supported.", true);
        return false;
    }

    if (4u <= ARGC)
    {
        if ( convertAsciiToHexInPlace(ARGV[3], IWPRIV_WEB_LONG_KEY_LEN) ) // for long web key
        {
            iwprivCb.securityType = WF_SECURITY_WEP_104;

            memcpy((void*)(iwprivCb.securityKey + webKey * IWPRIV_WEB_LONG_KEY_LEN), (const void*)ARGV[3], IWPRIV_WEB_LONG_KEY_LEN);
            iwprivCb.securityKeyLength = IWPRIV_WEB_KEY_NUM * IWPRIV_WEB_LONG_KEY_LEN;

            WF_CPSetSecurity(iwprivCb.cpId, iwprivCb.securityType, webKey,
                iwprivCb.securityKey + webKey * IWPRIV_WEB_LONG_KEY_LEN, IWPRIV_WEB_LONG_KEY_LEN);
        }
        else if ( convertAsciiToHexInPlace(ARGV[3], IWPRIV_WEB_SHORT_KEY_LEN) ) // for short web key
        {
            iwprivCb.securityType = WF_SECURITY_WEP_40;

            memcpy((void*)(iwprivCb.securityKey + webKey * IWPRIV_WEB_SHORT_KEY_LEN), (const void*)ARGV[3], IWPRIV_WEB_SHORT_KEY_LEN);
            iwprivCb.securityKeyLength = IWPRIV_WEB_KEY_NUM * IWPRIV_WEB_SHORT_KEY_LEN;

            WF_CPSetSecurity(iwprivCb.cpId, iwprivCb.securityType, webKey,
                iwprivCb.securityKey + webKey * IWPRIV_WEB_SHORT_KEY_LEN, IWPRIV_WEB_SHORT_KEY_LEN);
        }
        else
        {
            WFConsolePrintRomStr("64/128bit WEP key format not valid", true);
            return false;
        }
    }
    else // ARGC == 3u
    {
        WF_CPSetDefaultWepKeyIndex(iwprivCb.cpId, webKey);
    }

    return true;
}

/*****************************************************************************
 * FUNCTION: iwprivSetPsk
 *
 * RETURNS: true or false
 *
 * PARAMS: none
 *
 * NOTES: Set PSK key
 *         Valid key length:
 *          The PSK is 32 bytes (256 bits),
 * 			often displayed as 64 hexadecimal characters.
 *****************************************************************************/
static bool iwprivSetPsk(void)
{
    if ( iwprivCb.securityType != WF_SECURITY_WPA_AUTO_WITH_KEY )
    {
        WFConsolePrintRomStr("WPA-PSK encryption mode is not selected", true);
        return false;
    }

    if (ARGC < 3u)
    {
        WFConsolePrintRomStr("Missing value for last parameter", true);
        return false;
    }

    if ( convertAsciiToHexInPlace(ARGV[2], WF_WPA_KEY_LENGTH) )
    {
        memcpy((void*)iwprivCb.securityKey, (const void *)ARGV[2], WF_WPA_KEY_LENGTH);
        iwprivCb.securityKeyLength = WF_WPA_KEY_LENGTH;
    }
    else
    {
        WFConsolePrintRomStr("WPA-PSK Key length must be exactly 32 bytes long. It is often displayed as 64 hex characters.", true);
        return false;
    }

    WF_CPSetSecurity(iwprivCb.cpId, iwprivCb.securityType, 0, iwprivCb.securityKey, iwprivCb.securityKeyLength);

    return true;
}

/*****************************************************************************
 * FUNCTION: iwprivSetPhrase
 *
 * RETURNS: true or false
 *
 * PARAMS: none
 *
 * NOTES: Set PSK passphrase
 *         Valid passphrase length:
 *          Phrase string must be at least 8 characters and no greater than 64
 *****************************************************************************/
static bool iwprivSetPhrase(void)
{
    uint8_t j;
    uint8_t securityType;
    uint8_t * phraseStart;
    uint8_t * phraseEnd;
    uint8_t phraseLen;

    if ( iwprivCb.securityType == WF_SECURITY_WPA_AUTO_WITH_KEY || iwprivCb.securityType == WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE )
    {
        securityType = WF_SECURITY_WPA_AUTO_WITH_PASS_PHRASE;
    }
    else
    {
        WFConsolePrintRomStr("WPA-PSK/WPA2-PSK encryption mode is not selected", true);
        return false;
    }

    if (ARGC < 3u)
    {
        WFConsolePrintRomStr("Missing value for last parameter", true);
        return false;
    }

    phraseStart = (uint8_t *) ARGV[2];
    if (*phraseStart == '\"') // concatenate remaining arguments into one string
    {
        for (j = 2; j < (ARGC - 1); j++)
        {
            uint8_t argLen = strlen((char *)ARGV[j]);
            ARGV[j][argLen] = ' '; // replace '\0' with ' '
        }

        // searching for an ending quote
        phraseEnd = phraseStart + strlen((char *)phraseStart) - 1;
        while (*phraseEnd != '\"')
            phraseEnd--;

        // remove the double quotes
        phraseStart++;
        phraseEnd--;
    }
    else // a single word
    {
        phraseEnd = phraseStart + strlen((char *)phraseStart) - 1;
    }

    phraseLen = phraseEnd - phraseStart + 1;
    if (phraseLen < WF_MIN_WPA_PASS_PHRASE_LENGTH || WF_MAX_WPA_PASS_PHRASE_LENGTH < phraseLen)
    {
        WFConsolePrintRomStr("Passphrase string must be at least 8 chars and no greater than 64", true);
        return false;
    }

    iwprivCb.securityType = securityType;

    memcpy((void*)iwprivCb.securityKey, (const void*)phraseStart, phraseLen);
    iwprivCb.securityKey[phraseLen] = '\0'; // just for easy printing on the console
    iwprivCb.securityKeyLength = phraseLen;

    WF_CPSetSecurity(iwprivCb.cpId, iwprivCb.securityType, 0,
        iwprivCb.securityKey, iwprivCb.securityKeyLength);

    return true;
}

/*****************************************************************************
* FUNCTION: do_iwpriv_cmd
*
* RETURNS: None
*
* PARAMS:    None
*
* NOTES:   Responds to the user invoking ifconfig
*****************************************************************************/
void do_iwpriv_cmd(void)
{
    if (WF_hibernate.state)
    {
        WFConsolePrintRomStr("The Wi-Fi module is in hibernate mode - command failed.", true);
        return;
    }

    if ( !iwprivSetCb() )
        return;

    // if user only typed in iwpriv with no other parameters
    if (ARGC == 1u)
    {
        iwprivDisplayStatus();
        return;
    }

    if ( !iwconfigCb.isIdle )
    {
        WFConsolePrintRomStr("Security context modification can be only done in the idle state", true);
        return;
    }

    if ( (2u <= ARGC) && (strcmppgm2ram((char *)ARGV[1], "enc") == 0) )
    {
        if ( !iwprivSetEnc() )
            return;
    }
    else if ( (2u <= ARGC) && (strcmppgm2ram((char *)ARGV[1], "key") == 0) )
    {
        if ( !iwprivSetKey() )
            return;
    }
    else if ( (2u <= ARGC) && (strcmppgm2ram((char *)ARGV[1], "psk") == 0) )
    {
        if ( !iwprivSetPsk() )
            return;
    }
    else if ( (2u <= ARGC) && (strcmppgm2ram((char *)ARGV[1], "phrase") == 0) )
    {
        if ( !iwprivSetPhrase() )
            return;
    }
    else
    {
        WFConsolePrintRomStr("Unknown parameter", true);
        return;
    }
}

#endif /* WF_CONSOLE_IFCFGUTIL */
