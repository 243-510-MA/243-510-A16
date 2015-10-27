/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    telnet.c

  Summary:
    Module for Microchip TCP/IP Stack
     -Provides Telnet services on TCP port 23
     -Reference: RFC 854

  Description:
    Telnet Server

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

#define __TELNET_C_

#include "tcpip/tcpip.h"

#if defined(STACK_USE_TELNET_SERVER)

// Set up configuration parameter defaults if not overridden in
// tcpip_config.h
#if !defined(TELNET_PORT)
// Unsecured Telnet port
#define TELNET_PORT  23
#endif
#if !defined(TELNETS_PORT)
// SSL Secured Telnet port (ignored if STACK_USE_SSL_SERVER is undefined)
#define TELNETS_PORT  992
#endif
#if !defined(MAX_TELNET_CONNECTIONS)
// Maximum number of Telnet connections
#define MAX_TELNET_CONNECTIONS  (3u)
#endif
#if !defined(TELNET_USERNAME)
// Default Telnet user name
#define TELNET_USERNAME  "admin"
#endif
#if !defined(TELNET_PASSWORD)
// Default Telnet password
#define TELNET_PASSWORD  "microchip"
#endif

// Demo title string
static ROM uint8_t strTitle[] = "\x1b[2J\x1b[31m\x1b[1m" // 2J is clear screen, 31m is red, 1m is bold
        "Microchip Telnet Server 1.1\x1b[0m\r\n" // 0m is clear all attributes
        "(for this demo, type 'admin' for the login and 'microchip' for the password.)\r\n"
        "Login: ";
// Demo password
static ROM uint8_t strPassword[] = "Password: \xff\xfd\x2d"; // DO Suppress Local Echo (stop telnet client from printing typed characters)
// Access denied message
static ROM uint8_t strAccessDenied[] = "\r\nAccess denied\r\n\r\n";
// Successful authentication message
static ROM uint8_t strAuthenticated[] = "\r\nLogged in successfully\r\n\r\n"
        "\r\nPress 'q' to quit\r\n";
// Demo output string
static ROM uint8_t strDisplay[] = "\r\nSNTP Time:    (disabled)"
        "\r\nAnalog:             1023"
        "\r\nButtons:         3 2 1 0"
        "\r\nLEDs:    7 6 5 4 3 2 1 0";
// String with extra spaces, for Demo
static ROM uint8_t strSpaces[] = "          ";
// Demo disconnection message
static ROM uint8_t strGoodBye[] = "\r\n\r\nGoodbye!\r\n";

extern APP_CONFIG AppConfig;

extern uint8_t AN0String[8];

/*********************************************************************
 * Function:        void TelnetTask(void)
 *
 * PreCondition:    Stack is initialized()
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Performs Telnet Server related tasks.  Contains
 *                  the Telnet state machine and state tracking
 *                  variables.
 *
 * Note:            None
 ********************************************************************/
void TelnetTask(void)
{
    uint8_t i;
    uint8_t vTelnetSession;
    uint16_t w, w2;
    TCP_SOCKET MySocket;

    enum {
        SM_HOME = 0,
        SM_PRINT_LOGIN,
        SM_GET_LOGIN,
        SM_GET_PASSWORD,
        SM_GET_PASSWORD_BAD_LOGIN,
        SM_AUTHENTICATED,
        SM_REFRESH_VALUES
    } TelnetState;
    static TCP_SOCKET hTelnetSockets[MAX_TELNET_CONNECTIONS];
    static uint8_t vTelnetStates[MAX_TELNET_CONNECTIONS];
    static bool bInitialized = false;

    // Perform one time initialization on power up
    if ((!bInitialized) || (AppConfig.hibernateFlag)) {
        for (vTelnetSession = 0; vTelnetSession < MAX_TELNET_CONNECTIONS; vTelnetSession++) {
            hTelnetSockets[vTelnetSession] = INVALID_SOCKET;
            vTelnetStates[vTelnetSession] = SM_HOME;
        }
        bInitialized = true;
    }

    // Loop through each telnet session and process state changes and TX/RX data
    for (vTelnetSession = 0; vTelnetSession < MAX_TELNET_CONNECTIONS; vTelnetSession++) {
        // Load up static state information for this session
        MySocket = hTelnetSockets[vTelnetSession];
        TelnetState = vTelnetStates[vTelnetSession];

        // Reset our state if the remote client disconnected from us
        if (MySocket != INVALID_SOCKET) {
            if (TCPWasReset(MySocket))
                TelnetState = SM_PRINT_LOGIN;
        }

        // Handle session state
        switch (TelnetState) {
        case SM_HOME:
            // Connect a socket to the remote TCP server
            MySocket = TCPOpen(0, TCP_OPEN_SERVER, TELNET_PORT, TCP_PURPOSE_TELNET);

            // Abort operation if no TCP socket of type TCP_PURPOSE_TELNET is available
            // If this ever happens, you need to go add one to tcpip_config.h
            if (MySocket == INVALID_SOCKET)
                break;

            // Open an SSL listener if SSL server support is enabled
#if defined(STACK_USE_SSL_SERVER)
            TCPAddSSLListener(MySocket, TELNETS_PORT);
#endif

            TelnetState++;
            break;

        case SM_PRINT_LOGIN:
#if defined(STACK_USE_SSL_SERVER)
            // Reject unsecured connections if TELNET_REJECT_UNSECURED is defined
#if defined(TELNET_REJECT_UNSECURED)
            if (!TCPIsSSL(MySocket)) {
                if (TCPIsConnected(MySocket)) {
                    TCPDisconnect(MySocket);
                    TCPDisconnect(MySocket);
                    break;
                }
            }
#endif

            // Don't attempt to transmit anything if we are still handshaking.
            if (TCPSSLIsHandshaking(MySocket))
                break;
#endif

            // Make certain the socket can be written to
            if (TCPIsPutReady(MySocket) < strlenpgm((ROM char *) strTitle))
                break;

            // Place the application protocol data into the transmit buffer.
            TCPPutROMString(MySocket, strTitle);

            // Send the packet
            TCPFlush(MySocket);
            TelnetState++;

        case SM_GET_LOGIN:
            // Make sure we can put the password prompt
            if (TCPIsPutReady(MySocket) < strlenpgm((ROM char *) strPassword))
                break;

            // See if the user pressed return
            w = TCPFind(MySocket, '\n', 0, false);
            if (w == 0xFFFFu) {
                if (TCPGetRxFIFOFree(MySocket) == 0u) {
                    TCPPutROMString(MySocket, (ROM uint8_t *) "\r\nToo much data.\r\n");
                    TCPDisconnect(MySocket);
                }

                break;
            }

            // Search for the username -- case insensitive
            w2 = TCPFindROMArray(MySocket, (ROM uint8_t *) TELNET_USERNAME, sizeof (TELNET_USERNAME) - 1, 0, true);
            if (!((sizeof (TELNET_USERNAME) - 1 == w - w2) || (sizeof (TELNET_USERNAME) == w - w2))) {
                // Did not find the username, but let's pretend we did so we don't leak the user name validity
                TelnetState = SM_GET_PASSWORD_BAD_LOGIN;
            } else {
                TelnetState = SM_GET_PASSWORD;
            }

            // Username verified, throw this line of data away
            TCPGetArray(MySocket, NULL, w + 1);

            // Print the password prompt
            TCPPutROMString(MySocket, strPassword);
            TCPFlush(MySocket);
            break;

        case SM_GET_PASSWORD:
        case SM_GET_PASSWORD_BAD_LOGIN:
            // Make sure we can put the authenticated prompt
            if (TCPIsPutReady(MySocket) < strlenpgm((ROM char *) strAuthenticated))
                break;

            // See if the user pressed return
            w = TCPFind(MySocket, '\n', 0, false);
            if (w == 0xFFFFu) {
                if (TCPGetRxFIFOFree(MySocket) == 0u) {
                    TCPPutROMString(MySocket, (ROM uint8_t *) "Too much data.\r\n");
                    TCPDisconnect(MySocket);
                }

                break;
            }

            // Search for the password -- case sensitive
            w2 = TCPFindROMArray(MySocket, (ROM uint8_t *) TELNET_PASSWORD, sizeof (TELNET_PASSWORD) - 1, 0, false);
            if (!((sizeof (TELNET_PASSWORD) == w - w2) || (sizeof (TELNET_PASSWORD) - 1 == w - w2))
                    || (TelnetState == SM_GET_PASSWORD_BAD_LOGIN)) {
                // Did not find the password
                TelnetState = SM_PRINT_LOGIN;
                TCPPutROMString(MySocket, strAccessDenied);
                TCPDisconnect(MySocket);
                break;
            }

            // Password verified, throw this line of data away
            TCPGetArray(MySocket, NULL, w + 1);

            // Print the authenticated prompt
            TCPPutROMString(MySocket, strAuthenticated);
            TelnetState = SM_AUTHENTICATED;
            // No break

        case SM_AUTHENTICATED:
            if (TCPIsPutReady(MySocket) < strlenpgm((ROM char *) strDisplay) + 4)
                break;

            TCPPutROMString(MySocket, strDisplay);
            TelnetState++;

            // All future characters will be bold
            TCPPutROMString(MySocket, (ROM uint8_t *) "\x1b[1m");

        case SM_REFRESH_VALUES:
            if (TCPIsPutReady(MySocket) >= 78u) {
                //[10;1]
                //"SNTP Time:    (disabled)\r\n"
                //"Analog:             1023\r\n"
                //"Buttons:         3 2 1 0\r\n"
                //"LEDs:    7 6 5 4 3 2 1 0\r\n"

                // Write current UTC seconds from SNTP module, if it is enable
                // and has changed.  Note that conversion from a uint32_t to an
                // ASCII string can take a lot of CPU power, so we only print
                // this if the value has changed.
#if defined(STACK_USE_SNTP_CLIENT)
                {
                    static uint32_t dwTime;
                    uint8_t vTime[11];

                    if (dwTime != SNTPGetUTCSeconds()) {

                        // Position cursor at Line 10, Col 15
                        TCPPutROMString(MySocket, (ROM uint8_t *) "\x1b[10;15f");
                        dwTime = SNTPGetUTCSeconds();
                        ultoa(dwTime, vTime);
                        TCPPutROMArray(MySocket, (ROM uint8_t *) strSpaces, 10 - strlen((char *) vTime));
                        TCPPutString(MySocket, vTime);
                    }
                }
#endif

                // Position cursor at Line 11, Col 21
                TCPPutROMString(MySocket, (ROM uint8_t *) "\x1b[11;21f");

                // Put analog value with space padding on right side for 4 characters
                TCPPutROMArray(MySocket, (ROM uint8_t *) strSpaces, 4 - strlen((char *) AN0String));
                TCPPutString(MySocket, AN0String);

                // Put Buttons
                TCPPutROMString(MySocket, (ROM uint8_t *) "\x1b[12;18f");
                TCPPut(MySocket, BUTTON3_IO ? '1' : '0');
                TCPPut(MySocket, ' ');
                TCPPut(MySocket, BUTTON2_IO ? '1' : '0');
                TCPPut(MySocket, ' ');
                TCPPut(MySocket, BUTTON1_IO ? '1' : '0');
                TCPPut(MySocket, ' ');
                TCPPut(MySocket, BUTTON0_IO ? '1' : '0');

                // Put LEDs
                TCPPutROMString(MySocket, (ROM uint8_t *) "\x1b[13;10f");
                TCPPut(MySocket, LED7_IO ? '1' : '0');
                TCPPut(MySocket, ' ');
                TCPPut(MySocket, LED6_IO ? '1' : '0');
                TCPPut(MySocket, ' ');
                TCPPut(MySocket, LED5_IO ? '1' : '0');
                TCPPut(MySocket, ' ');
                TCPPut(MySocket, LED4_IO ? '1' : '0');
                TCPPut(MySocket, ' ');
                TCPPut(MySocket, LED3_IO ? '1' : '0');
                TCPPut(MySocket, ' ');
                TCPPut(MySocket, LED2_IO ? '1' : '0');
                TCPPut(MySocket, ' ');
                TCPPut(MySocket, LED1_IO ? '1' : '0');
                TCPPut(MySocket, ' ');
                TCPPut(MySocket, LED0_IO ? '1' : '0');

                // Put cursor at beginning of next line
                TCPPutROMString(MySocket, (ROM uint8_t *) "\x1b[14;1f");

                // Send the data out immediately
                TCPFlush(MySocket);
            }

            if (TCPIsGetReady(MySocket)) {
                TCPGet(MySocket, &i);
                switch (i) {
                case '\r':
                case 'q':
                case 'Q':
                    if (TCPIsPutReady(MySocket) >= strlenpgm((ROM char *) strGoodBye))
                        TCPPutROMString(MySocket, strGoodBye);
                    TCPDisconnect(MySocket);
                    TelnetState = SM_PRINT_LOGIN;
                    break;
                }
            }
            break;
        }

        // Save session state back into the static array
        hTelnetSockets[vTelnetSession] = MySocket;
        vTelnetStates[vTelnetSession] = TelnetState;
    }
}

#endif // defined(STACK_USE_TELNET_SERVER)
