/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    zero_conf_link_multicast_dns.c

  Summary:
    

  Description:
    Zero Configuration (Zeroconf) Multicast DNS and
    Service Discovery Module for Microchip TCP/IP Stack

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

#define __ZERO_CONF_MULTICAST_DNS_C_

#include "tcpip/tcpip.h"
#include "tcpip/zero_conf_link_multicast_dns.h"

#define TICK uint32_t

//MDNS_STATIC should be static in production code
//For code size tuning, defined to be (blank) - so C30 can give us the size of each function.
//#define MDNS_STATIC static
#define MDNS_STATIC static

#if defined(STACK_USE_ZEROCONF_MDNS_SD)

extern void DisplayIPValue(IP_ADDR IPVal);

#define MDNS_PORT         5353
#define MAX_HOST_NAME_SIZE  32 // 31+'\0'  Max Host name size
#define MAX_LABEL_SIZE      64 // 63+'\0'  Maximum size allowed for a label. RFC 1035 (2.3.4) == 63

#define MAX_SRV_TYPE_SIZE   32 // 31+'\0'  eg. "_http._tcp.local". Max could be 255, but is an overkill.

#if defined(__XC32)
#define MAX_RR_NAME_SIZE   256 // 255+'\0' Max Resource Recd Name size. RFC 1035 (2.3.4) == 255
#define MAX_SRV_NAME_SIZE   64 // 63+'\0'  eg. "My Web server". Max could be 255, but is an overkill.
#define MAX_TXT_DATA_SIZE  128 // 127+'\0' eg. "path=/index.htm"
#else
#define MAX_RR_NAME_SIZE    64 //256 // 255+'\0' Max Resource Recd Name size. RFC 1035 (2.3.4) == 255
#define MAX_SRV_NAME_SIZE   32 //64 // 63+'\0'  eg. "My Web server". Max could be 255, but is an overkill.
#define MAX_TXT_DATA_SIZE   64 //128 // 127+'\0' eg. "path=/index.htm"
#endif

#define RESOURCE_RECORD_TTL_VAL  3600 // Time-To-Live for a Resource-Record in seconds.

#define MAX_RR_NUM  4 // for A, PTR, SRV, and TXT  Max No.of Resource-Records/Service

/* Constants from mdns.txt (IETF Draft) */
#define MDNS_PROBE_WAIT             750 // msecs (initial random delay)
#define MDNS_PROBE_INTERVAL         250 // msecs (maximum delay till repeated probe)
#define MDNS_PROBE_NUM                6 // (number of probe packets)
#define MDNS_MAX_PROBE_CONFLICT_NUM  30 // max num of conflicts before we insist and move on to announce ...
#define MDNS_ANNOUNCE_NUM             3 // (number of announcement packets)
#define MDNS_ANNOUNCE_INTERVAL      250 // msecs (time between announcement packets)
#define MDNS_ANNOUNCE_WAIT          250 // msecs (delay before announcing)

// SOFTAP_ZEROCONF_SUPPORT
enum {
    MDNS_RESPONDER_INIT = 0,
    MDNS_RESPONDER_LISTEN,
} mDNS_responder_state = MDNS_RESPONDER_INIT;

/* Resource-Record Types from RFC-1035 */
typedef enum {
    QTYPE_A = 1,
    QTYPE_NS = 2,
    QTYPE_CNAME = 5,
    QTYPE_PTR = 12,
    QTYPE_TXT = 16,
    QTYPE_SRV = 33,
    QTYPE_ANY = 255,
} MDNS_QTYPE;

/* Indexes in Resource-record list */
#define QTYPE_A_INDEX   0
#define QTYPE_PTR_INDEX 1
#define QTYPE_SRV_INDEX 2
#define QTYPE_TXT_INDEX 3

/* MDNS Message Fomrat, which is common
 * for Queries and Resource-Records. Taken
 * from RFC 1035
 */

/* MDNS Message Header Flags */
typedef union _MDNS_MSG_HEADER_FLAGS {
    struct {
        uint8_t rcode : 4;
        uint8_t z : 3;
        uint8_t ra : 1;
        uint8_t rd : 1;
        uint8_t tc : 1;
        uint8_t aa : 1;
        uint8_t opcode : 4;
        uint8_t qr : 1;
    } bits;
    uint16_t Val;
    uint8_t v[2];
} MDNS_MSG_HEADER_FLAGS;

/* MDNS Message-Header Format */
typedef struct _MDNS_MSG_HEADER {
    TCPIP_UINT16_VAL query_id;
    MDNS_MSG_HEADER_FLAGS flags;
    TCPIP_UINT16_VAL nQuestions;
    TCPIP_UINT16_VAL nAnswers;
    TCPIP_UINT16_VAL nAuthoritativeRecords;
    TCPIP_UINT16_VAL nAdditionalRecords;
} MDNS_MSG_HEADER;

/* DNS-Query Format, which is prepended by
 * DNS-MESSAGE Header defined above */
struct question {
    unsigned char *name;
    unsigned short int type, class;
};

/* DNS-Resource Record Format, which is
 * prepended by DNS-MESSAGE Header
 * defined above. This definition includes
 * all resource-record data formats, to have
 * small-memory foot print */

struct _mDNSProcessCtx_sd; // mdnsd_struct
struct _mDNSProcessCtx_common;

typedef struct _mDNSResourceRecord {
    uint8_t *name;
    TCPIP_UINT16_VAL type;
    TCPIP_UINT16_VAL class;
    TCPIP_UINT32_VAL ttl;
    TCPIP_UINT16_VAL rdlength;

    union {
        IP_ADDR ip; // for A record

        struct {
            TCPIP_UINT16_VAL priority;
            TCPIP_UINT16_VAL weight;
            TCPIP_UINT16_VAL port;
        } srv; // for SRV record
    };

    // DO NOT merge this into the union.
    uint8_t *rdata; // for PTR, SRV and TXT records.

    /* House-Keeping Stuff */

    // pointer to the header Ctx of the process that "owns" this resource record.
    struct _mDNSProcessCtx_common *pOwnerCtx;

    uint8_t valid; /* indicates whether rr is valid */
    bool bNameAndTypeMatched;
    bool bResponseRequested;
    bool bResponseSuppressed;
} mDNSResourceRecord;

/* DNS-SD Specific Data-Structures */

typedef enum _MDNS_STATE {
    MDNS_STATE_HOME = 0,
    MDNS_STATE_INTF_NOT_CONNECTED,
    MDNS_STATE_IPADDR_NOT_CONFIGURED,
    MDNS_STATE_NOT_READY,
    MDNS_STATE_INIT,
    MDNS_STATE_PROBE,
    MDNS_STATE_ANNOUNCE,
    MDNS_STATE_DEFEND,
} MDNS_STATE;

typedef enum _MDNS_RR_GROUP {
    MDNS_RR_GROUP_QD,
    MDNS_RR_GROUP_AN,
    MDNS_RR_GROUP_NS,
    MDNS_RR_GROUP_AR
} MDNS_RR_GROUP;

typedef struct __attribute__((__packed__)) _mDNSResponderCtx
{
    mDNSResourceRecord rr_list[MAX_RR_NUM]; // Our resource records.

    bool bLastMsgIsIncomplete; // Last DNS msg was truncated
    TCPIP_UINT16_VAL query_id; // mDNS Query transaction ID
    IP_ADDR prev_ipaddr; // To keep track of changes in IP-addr
}
mDNSResponderCtx;

typedef enum _MDNS_CTX_TYPE {
    MDNS_CTX_TYPE_HOST = 0,
    MDNS_CTX_TYPE_SD
} MDNS_CTX_TYPE;

typedef struct _mDNSProcessCtx_common {
    MDNS_CTX_TYPE type; // Is owner mDNS ("HOST") or mDNS-SD ("SD")?
    MDNS_STATE state; // PROBE, ANNOUNCE, DEFEND, ...

    uint8_t nProbeCount;
    uint8_t nProbeConflictCount;
    uint8_t nClaimCount;
    bool bProbeConflictSeen;
    bool bLateConflictSeen;

    bool bConflictSeenInLastProbe;
    uint8_t nInstanceId;

    TICK event_time; // Internal Timer, to keep track of events
    uint8_t time_recorded; // Flag to indicate event_time is loaded
    TICK random_delay;

} mDNSProcessCtx_common;

typedef struct __attribute__((__packed__)) _mDNSProcessCtx_host
{
    mDNSProcessCtx_common common;

    mDNSResponderCtx *pResponderCtx;

    // other host name related info

    uint8_t szUserChosenHostName[MAX_HOST_NAME_SIZE]; // user chosen host name
    uint8_t szHostName[MAX_HOST_NAME_SIZE]; // mDNS chosen Host-Name

}
mDNSProcessCtx_host;

typedef struct __attribute__((__packed__)) _mDNSProcessCtx_sd
{
    mDNSProcessCtx_common common;

    mDNSResponderCtx *pResponderCtx;

    // info specific to SD
    uint8_t srv_name[MAX_SRV_NAME_SIZE];
    uint8_t srv_type[MAX_SRV_TYPE_SIZE];
    uint8_t sd_qualified_name[MAX_RR_NAME_SIZE];
    uint8_t used; /* Spinlock to protect Race-cond. */

    uint8_t sd_auto_rename : 1, /* Flag to show auto-Rename is enabled */
            sd_service_advertised : 1, /* Flag to show whether service is advertised */
            service_registered : 1; /* Flag to indicate that user has registered this service */

    uint16_t sd_port; /* Port number in Local-sys where Service is being offered */
    uint8_t sd_txt_rec[MAX_TXT_DATA_SIZE];
    uint8_t sd_txt_rec_len;

    void (*sd_call_back)(char *, MDNSD_ERR_CODE, void *);
    void *sd_context;

}
mDNSProcessCtx_sd;

static mDNSProcessCtx_host gHostCtx;
static mDNSProcessCtx_sd gSDCtx;
static mDNSResponderCtx gResponderCtx;

/* DNS-SD State-Machine */

const uint8_t *CONST_STR_local = (uint8_t *) "local";

/* Multicast-DNS States defintion */

/************** Global Declarations ***************/
/* Remote Node info, which is Multicast-Node
 * whose IP-address is 224.0.0.251 & MAC-Address
 * is 01:00:5E:00:00:FB. Multicast-IP address for
 * mDNS is specified by mdns.txt (IETF). IP is
 * translated into Multicast-MAC address according
 * rules specified in Std.
 */
static NODE_INFO mDNSRemote;
UDP_SOCKET mDNS_socket = INVALID_UDP_SOCKET; // Multicast-Socket Opened up for
// mDNS Server/Client (Responder/Qurier)

/* Global declaration to support Message-Compression
 * defined in RFC-1035, Section 4.1.4 */

/* Forward declarations */
MDNS_STATIC void mDNSResponder(void);
MDNS_STATIC uint16_t mDNSDeCompress(uint16_t wPos, uint8_t *pcString, bool bFollowPtr, uint8_t cElement, uint8_t cDepth);
MDNS_STATIC size_t mDNSSDFormatServiceInstance(uint8_t *string, size_t strSize);
static uint16_t g_mDNS_offset;

// Redirect all UDPPUT_LOCAL() calls to here.
// This seems to make all those 'call + nop' (2 instructions) into 'rcall' (1 instruction)

MDNS_STATIC void
UDPPUT_LOCAL(uint8_t c)
{
    UDPPut(c);
}

/***************************************************************
  Function:
    void DisplayHostName(uint8_t *HostName)

  Summary:
    Writes an Host-Name to the LCD display and the UART
    as available

  Description:
    This function takes a string and then displays it on LCD's
    2nd half.

  Parameters:
    HostName - the Hos-name to be displayed.

  Returns:
    None
 ***************************************************************/
MDNS_STATIC void DisplayHostName(uint8_t *HostName)
{
    uint8_t SingleChar;
    uint8_t i;
    uint8_t len;
#ifdef USE_LCD
    uint8_t LCDPos = 0;
#endif

#if defined(STACK_USE_UART)
    putrsUART((ROM char *) "\r\nZeroConf: Host = ");
    putrsUART((ROM char *) HostName);
    putrsUART((ROM char *) "\r\n");
#endif

    len = strlen((char *) HostName);

    for (i = 0; (i < len) && (i < 16u); i++) {
        SingleChar = *HostName++;
        if (SingleChar == '\0')
            break;

#ifdef USE_LCD
        LCDText[LCDPos++] = SingleChar;
#endif
    }

#ifdef USE_LCD
    if (LCDPos < 32u)
        LCDText[LCDPos] = 0;
    LCDUpdate();
#endif
}

/************* Local String Functions ******************/

/***************************************************************
  Function:
    static uint8_t strcmp_local_ignore_case(uint8_t *string_1, uint8_t *string_2)

  Summary:
    Compares two strings by ignoring the case.

  Parameters:
    string_1 & string_2 - Two strings

  Returns:
    Zero: If two strings are equal.
    Non-Zero: If both strings are not equal or on error case
 **************************************************************/
MDNS_STATIC uint8_t strcmp_local_ignore_case(uint8_t *str_1, uint8_t *str_2)
{
    if (str_1 == NULL || str_2 == NULL) {
        WARN_MDNS_PRINT("strmcmp_local_ignore_case: String is NULL \r\n");
        return -1;
    }

    while (*str_1 && *str_2) {
        if (*str_1 == *str_2 || (*str_1 - 32) == *str_2 ||
                *str_1 == (*str_2 - 32)) {
            str_1++;
            str_2++;
            continue;
        } else
            return 1;

    }
    if (*str_1 == '\0' && *str_2 == '\0')
        return 0;
    else
        return 1;

}

MDNS_STATIC void
mDNSResetCounters(mDNSProcessCtx_common *pHeader, bool bResetProbeConflictCount)
{
    if (bResetProbeConflictCount) {

        pHeader->nProbeConflictCount = 0;
    }

    pHeader->nProbeCount = 0;
    pHeader->nClaimCount = 0;
    pHeader->bLateConflictSeen = false;
    pHeader->bProbeConflictSeen = false;
}

/***************************************************************
  Function:
    static void mDNSRename(uint8_t *str, uint8_t max_len)

  Summary:
    Renames a string with a numerical-extension.

  Description:
    This function is to rename host-name/Resource-Record Name,
    when a name-conflict is detected on local-network.
    For-Ex: "myhost" is chosen name and a conflict is detected
    this function renames as "myhost-2". Also ensures that string
    is properly formatted.

  Precondition:
    None

  Parameters:
    String - the string to be Renamed with Numerical-Extenstion.
    max_len - Maximum Length allowed for String

  Returns:
    None
 **************************************************************/

// strLabel:  the user registered name.
//            E.g., "Web Server", for service name (srv_name), or
//                 "My Host", for host name (taken from MY_DEFAULT_HOST_NAME)
// nLabelId:  instance number, to avoid conflict in the name space.
// strBase:   the base name for the appropriate name space.
//            E.g., "_http._tcp.local" for service name, or
//                 "local" for host name.
// strTarget: where the newly constructed fully-qualified-name will be stored.
// nMaxLen:   max length for the newly constructed label, which is the first portion of the
//            fully-qualified-name
//
// ("Web Server", 3, "_http._tcp.local", strTarget, 63) =>
//     stores "Web Server-3._http._tcp.local" to *strTarget.
// ("MyHost", 2, "local", strTarget, 63) =>
//     stores "MyHost-2.local" to *strTarget
//
#define mDNSRename_ID_LEN 6
MDNS_STATIC void mDNSRename(uint8_t *strLabel, uint8_t nLabelId, uint8_t *strBase, uint8_t *strTarget, uint8_t nMaxLen)
{
    uint8_t n = nLabelId;
    uint8_t i = mDNSRename_ID_LEN - 1;
    uint8_t str_n[mDNSRename_ID_LEN]; //enough for "-255." + '\0'.

    str_n[i--] = 0;
    str_n[i--] = '.';

    // construct str_n from n
    while (i != 0) {
        str_n[i--] = '0' + n % 10;
        if (n < 10) break;
        n = n / 10;
    }
    str_n[i] = '-';

#ifdef MDNS_WARN
    size_t targetLen = strncpy_m((char *) strTarget, nMaxLen, 3, strLabel, &(str_n[i]), strBase);
    if (targetLen == nMaxLen) {
        MDNS_WARN("mDNSRename: label too long - truncated\r\n");
    }
#else
    strncpy_m((char *) strTarget, nMaxLen, 3, strLabel, &(str_n[i]), strBase);
#endif
}

/***************************************************************
  Function:
    static void mDNSPutString(uint8_t * String)

  Summary:
    Writes a string to the Multicast-DNS socket.

  Description:
    This function writes a string to the Multicast-DNS socket,
    ensuring that it is properly formatted.

  Precondition:
    UDP socket is obtained and ready for writing.

  Parameters:
    String - the string to write to the UDP socket.

  Returns:
    None
 **************************************************************/

MDNS_STATIC void mDNSPutString(uint8_t * string)
{
    uint8_t *right_ptr, *label_ptr;
    uint8_t label[MAX_LABEL_SIZE];
    uint8_t i;
    uint8_t len;

    right_ptr = string;

    while (1) {
        label_ptr = label;
        len = 0;
        while (*right_ptr) {
            i = *right_ptr;

            if (i == '.' || i == '/' ||
                    i == ',' || i == '>' || i == '\\') {
                /* Formatted Serv-Instance will have '\.'
                 * instead of just '.' */
                if (i == '\\') {
                    right_ptr++;
                } else
                    break;
            }
            *label_ptr++ = *right_ptr;
            len++;
            right_ptr++;
        }
        i = *right_ptr++;

        // Put the length and data
        // Also, skip over the '.' in the input string
        UDPPUT_LOCAL(len);
        UDPPutArray(label, len);
        string = right_ptr;

        if (i == 0x00u || i == '/' || i == ',' || i == '>')
            break;
    }

    // Put the string null terminator character
    UDPPUT_LOCAL(0x00);
}

/***************************************************************
  Function:
    static bool mDNSSendQuery(uint8_t * name, uint8_t record_type)

  Summary:
    Sends out a Multicast-DNS-Query to Multicast-Address
    through mDNS_socket.

  Description:
    This function is used in Probing-phase to check the
    uniqueness of chosen Host-Name/ Resoruce-Record-Name.
    Selected Name and Type of Query are put into
    Multicast UDP socket.

  Precondition:
    UDP socket (mDNS_socket) is obtained and ready for writing.

  Parameters:
    name - Chosen Host-Name/Resource-Record-Name, checking
           for uniqueness.
    type - Type of Query

  Returns:
    true - On Success
    false - On Failure (If UDP-Socket is invalid)
 **************************************************************/
//MDNS_STATIC bool mDNSSendQuery(uint8_t * name, uint8_t record_type, uint8_t cFlush, uint8_t probe_type)

MDNS_STATIC bool mDNSProbe(mDNSProcessCtx_common *pCtx)
{
    MDNS_MSG_HEADER mDNS_header;

    // Abort operation if no UDP sockets are available
    // If this ever happens, incrementing MAX_UDP_SOCKETS in
    // stack_task.h may help (at the expense of more global memory
    // resources).

    if (mDNS_socket == INVALID_UDP_SOCKET) {
        WARN_MDNS_PRINT("mDNSSendQuery: Opening UDP Socket Failed \r\n");
        return false;
    }

    // Make certain the socket can be written to
    while (!UDPIsPutReady(mDNS_socket));

    // Put DNS query here
    gResponderCtx.query_id.Val++;

    /*
    UDPPUT_LOCAL(gResponderCtx.query_id.v[1]); // User chosen transaction ID
    UDPPUT_LOCAL(gResponderCtx.query_id.v[0]);

    UDPPUT_LOCAL(0x00); // Standard query with recursion
    UDPPUT_LOCAL(0x00);

    UDPPUT_LOCAL(0x00); // 1 entry in the question section
    UDPPUT_LOCAL(0x01);

    UDPPUT_LOCAL(0x00); // 0 entry in the answer section
    UDPPUT_LOCAL(0x00);

    UDPPUT_LOCAL(0x00); // 1 entry in name server section
    UDPPUT_LOCAL(0x01);

    UDPPUT_LOCAL(0x00); // 0 entry in additional records section
    UDPPUT_LOCAL(0x00);
    */

    mDNS_header.query_id.Val = swaps(gResponderCtx.query_id.Val); // User chosen transaction ID
    mDNS_header.flags.Val = 0; // Standard query with recursion
    mDNS_header.nQuestions.Val = swaps(((uint16_t) 1u)); // 1 entry in the question section
    mDNS_header.nAnswers.Val = 0; // 0 entry in the answer section
    mDNS_header.nAuthoritativeRecords.Val = swaps(((uint16_t) 1u)); // 1 entry in name server section
    mDNS_header.nAdditionalRecords.Val = 0; // 0 entry in additional records section

    // Put out the mDNS message header
    UDPPutArray((uint8_t *) & mDNS_header, sizeof (MDNS_MSG_HEADER));

    // Start of the QD section
    switch (pCtx->type) {
    case MDNS_CTX_TYPE_HOST:

        mDNSPutString(gResponderCtx.rr_list[QTYPE_A_INDEX].name);
        break;

    case MDNS_CTX_TYPE_SD:

        mDNSPutString(gResponderCtx.rr_list[QTYPE_SRV_INDEX].name);
        break;
    }

    UDPPUT_LOCAL(0x00); // Type: Always QTYPE_ANY
    UDPPUT_LOCAL(QTYPE_ANY);

    UDPPUT_LOCAL(0x80); // Class: Cache-Flush
    UDPPUT_LOCAL(0x01); //        IN (Internet)

    // Start of the NS section

    switch (pCtx->type) {
    case MDNS_CTX_TYPE_HOST:
        mDNSPutString(gResponderCtx.rr_list[QTYPE_A_INDEX].name);
        break;

    case MDNS_CTX_TYPE_SD:
        mDNSPutString(gResponderCtx.rr_list[QTYPE_SRV_INDEX].name);
        break;
    }

    UDPPUT_LOCAL(0x00); // Type: A or SRV

    switch (pCtx->type) {
    case MDNS_CTX_TYPE_HOST:
        UDPPUT_LOCAL(QTYPE_A);
        break;

    case MDNS_CTX_TYPE_SD:
        UDPPUT_LOCAL(QTYPE_SRV);
        break;
    }

    UDPPUT_LOCAL(0x00); // Class: Cache-Flush bit MUST NOT be set
    UDPPUT_LOCAL(0x01); //        IN (Internet)

    UDPPUT_LOCAL(0x00); // 0x00000078 Time To Live, 2 minutes
    UDPPUT_LOCAL(0x00);
    UDPPUT_LOCAL(0x00);
    UDPPUT_LOCAL(0x78);

    switch (pCtx->type) {
    case MDNS_CTX_TYPE_HOST:
    {
        UDPPUT_LOCAL(gResponderCtx.rr_list[QTYPE_A_INDEX].rdlength.v[1]);
        UDPPUT_LOCAL(gResponderCtx.rr_list[QTYPE_A_INDEX].rdlength.v[0]);

        UDPPUT_LOCAL(gResponderCtx.rr_list[QTYPE_A_INDEX].ip.v[0]);
        UDPPUT_LOCAL(gResponderCtx.rr_list[QTYPE_A_INDEX].ip.v[1]);
        UDPPUT_LOCAL(gResponderCtx.rr_list[QTYPE_A_INDEX].ip.v[2]);
        UDPPUT_LOCAL(gResponderCtx.rr_list[QTYPE_A_INDEX].ip.v[3]);

        break;
    }

    case MDNS_CTX_TYPE_SD:
    {
        UDPPUT_LOCAL(gResponderCtx.rr_list[QTYPE_SRV_INDEX].rdlength.v[1]);
        UDPPUT_LOCAL(gResponderCtx.rr_list[QTYPE_SRV_INDEX].rdlength.v[0]);

        UDPPUT_LOCAL(gResponderCtx.rr_list[QTYPE_SRV_INDEX].srv.priority.v[1]);
        UDPPUT_LOCAL(gResponderCtx.rr_list[QTYPE_SRV_INDEX].srv.priority.v[0]);
        UDPPUT_LOCAL(gResponderCtx.rr_list[QTYPE_SRV_INDEX].srv.weight.v[1]);
        UDPPUT_LOCAL(gResponderCtx.rr_list[QTYPE_SRV_INDEX].srv.weight.v[0]);
        UDPPUT_LOCAL(gResponderCtx.rr_list[QTYPE_SRV_INDEX].srv.port.v[1]);
        UDPPUT_LOCAL(gResponderCtx.rr_list[QTYPE_SRV_INDEX].srv.port.v[0]);

        mDNSPutString(gResponderCtx.rr_list[QTYPE_SRV_INDEX].rdata);

        break;
    }
    }

    UDPFlush();

    return true;
}

/***************************************************************
  Function:
    static bool mDNSSendRR(struct mDNSResourceRecord *record,
                    uint8_t record_type, uint32_t ttl_val,uint16_t query_id)

  Summary:
    Sends out a Multicast-DNS-Answer (Resource-Record) to
    Multicast-Address through mDNS_socket (UDP Socket).

  Description:
    This function is used in Announce-phase & Defend-Phase.

    In announce-phase the Host-Name or Resource-Record (Service)
    will be announced in local-network, so that neighbors can
    detect new-service or update their caches with new host-name
    to IP-Address mapping.

    In Defend-Phase, when mDNSResponder receives a query for
    Host-name or Resounce-record for which this holds authority.

  Precondition:
    UDP socket (mDNS_socket) is obtained and ready for writing.

  Parameters:
    record - Resource-Record filled up with required info
    type   - Type of Res-Rec
    ttl_val - Time-To-Live value for Res-Record
    query_id - Query-ID for which this mDNS-answer (Res-Rec)
               corresponds to

  Returns:
    true - On Success
    false - On Failure (If UDP-Socket is invalid)
 **************************************************************/

MDNS_STATIC bool
mDNSSendRR(mDNSResourceRecord *pRecord,
        uint16_t query_id,
        uint8_t cFlush,
        uint16_t nAnswersInMsg,
        bool bIsFirstRR,
        bool bIsLastRR)
{
    MDNS_MSG_HEADER mDNS_header;
    TCPIP_UINT32_VAL ttl;
    uint8_t rec_length;
    uint8_t record_type;

    record_type = pRecord->type.Val;

    DEBUG0_MDNS_MESG(zeroconf_dbg_msg, "tx RR: (%d)\r\n", record_type);
    DEBUG0_MDNS_PRINT(zeroconf_dbg_msg);

    if (mDNS_socket == INVALID_UDP_SOCKET) {
        WARN_MDNS_PRINT("mDNSSendRR: Opening UDP Socket Failed \r\n");
        return false;
    }

    while (!UDPIsPutReady(mDNS_socket));

    if (bIsFirstRR) {
        memset(&mDNS_header, 0, sizeof (MDNS_MSG_HEADER));

        mDNS_header.query_id.Val = swaps(query_id);

        mDNS_header.flags.bits.qr = 1; // this is a Response,
        mDNS_header.flags.bits.aa = 1; // and we are authoritative
        mDNS_header.flags.Val = swaps(mDNS_header.flags.Val);

        mDNS_header.nAnswers.Val = swaps(nAnswersInMsg);

        // Put out the mDNS message header
        UDPPutArray((uint8_t *) & mDNS_header, sizeof (MDNS_MSG_HEADER));
    }

    ttl.Val = pRecord->ttl.Val;

    //rex#if 0
    //rex    if ( name != NULL)
    //rex    {
    //rex        mDNSPutString(name);
    //rex    }
    //rex    else
    //rex#endif
    //rex    {
    mDNSPutString(pRecord->name);
    //rex    }

    UDPPUT_LOCAL(0x00);
    UDPPUT_LOCAL(record_type); // Resource Record Type

    /* MSB of Upper-byte in Class filed acts as
     * Cache-Flush bit to notify all Neighbors to
     * flush their caches and fill with this new
     * information */
    if (UDPSocketInfo[mDNS_socket].remotePort == MDNS_PORT) {
        UDPPUT_LOCAL(cFlush);
        UDPPUT_LOCAL(0x01); // Class
    } else {
        // Legacy/Unicast DNS response should not set the Cache-Flush bit.
        UDPPUT_LOCAL(0x00);
        UDPPUT_LOCAL(0x01); // Class
    }

    UDPPUT_LOCAL(ttl.v[3]); // Time To Live
    UDPPUT_LOCAL(ttl.v[2]);
    UDPPUT_LOCAL(ttl.v[1]);
    UDPPUT_LOCAL(ttl.v[0]);

    switch (record_type) {
    case QTYPE_A:

        UDPPUT_LOCAL(0x00); // 0x0004 Data length
        UDPPUT_LOCAL(0x04);
        UDPPUT_LOCAL(pRecord->ip.v[0]); // Put out IP address
        UDPPUT_LOCAL(pRecord->ip.v[1]); // AppConfig.MyIPAddr.v[1]
        UDPPUT_LOCAL(pRecord->ip.v[2]);
        UDPPUT_LOCAL(pRecord->ip.v[3]);

        break;

    case QTYPE_PTR:

        /* 2 bytes extra. One for Prefix Length for first-label.
         * Other one for NULL terminator */
        pRecord->rdlength.Val = strlen((char *) pRecord->rdata) + 2;

        UDPPUT_LOCAL(pRecord->rdlength.v[1]);
        UDPPUT_LOCAL(pRecord->rdlength.v[0]); // Res-Data Length

        mDNSPutString(((mDNSProcessCtx_sd *) (pRecord->pOwnerCtx))->sd_qualified_name);

        break;

    case QTYPE_SRV:

        /* 2 bytes extra. One for Prefix Length for first-label.
         * Other one for NULL terminator */

        pRecord->rdlength.Val = strlen((char *) pRecord->rdata) + 2;
        pRecord->rdlength.Val += 6; // for priority, weight, and port

        UDPPUT_LOCAL(pRecord->rdlength.v[1]);
        UDPPUT_LOCAL(pRecord->rdlength.v[0]); // Res-Data Length

        UDPPUT_LOCAL(pRecord->srv.priority.v[1]); // Put Priority
        UDPPUT_LOCAL(pRecord->srv.priority.v[0]);
        UDPPUT_LOCAL(pRecord->srv.weight.v[1]);
        UDPPUT_LOCAL(pRecord->srv.weight.v[0]);
        UDPPUT_LOCAL(pRecord->srv.port.v[1]);
        UDPPUT_LOCAL(pRecord->srv.port.v[0]);

        mDNSPutString(pRecord->rdata);

        break;

    case QTYPE_TXT:

        rec_length = strlen((char *) pRecord->rdata);
        pRecord->rdlength.Val = rec_length + 1;

        UDPPUT_LOCAL(pRecord->rdlength.v[1]);
        UDPPUT_LOCAL(pRecord->rdlength.v[0]); // Res-Data Length

        UDPPUT_LOCAL(pRecord->rdlength.Val - 1); // As of now only single TXT string supported!!

        UDPPutArray(pRecord->rdata, rec_length);

        break;

    default:

        WARN_MDNS_PRINT("RR Type not supported \n");
    }

    if (bIsLastRR) {
        UDPFlush();
    }

    return true;
}

/***************************************************************
  Function:
    size_t mDNSSDFormatServiceInstance(uint8_t *string, size_t strSize )

  Summary:
    Formats the Service-Instance name according to DNS-SD standard
    specification.

  Description:
    This function is used to format the Service-Instance name, if
    it contains 'dots' and 'backslashes'

    As the service-instance name will be merged with service-type &
    to distinguish the 'dots' seperating the service-type words and
    'dots' within service-instance name, the 'dots' within service-
    instance name will be replaced with '\.' in place of '.' Even the
    '\' are replaced with '\\'.

    When the resource-record containing service-instance name is
    pushed out, the formatted dots '\.' are sentout as '.' and the
    'dots' sperating the service-type & service-instances are replaced
    with length bytes, as specified in RFC 1035.

  Precondition:
    None

  Parameters:
    String - Service-Instance name to be formatted
    strSize - available size for the formatted string, not to be exceeded

  Returns:
    size of the formatted string
 **************************************************************/
MDNS_STATIC size_t mDNSSDFormatServiceInstance(uint8_t *string, size_t strSize)
{
    uint8_t *temp;
    uint8_t output[MAX_LABEL_SIZE];
    uint8_t i;
    uint8_t *right_ptr, *str_token;
    uint8_t len;

    temp = output;
    right_ptr = string;
    str_token = string;
    while (1) {
        do {
            i = *right_ptr++;
        } while ((i != 0x00u) && (i != '\\') && (i != '.'));

        /* Prefix '\' for every occurance of '.' & '\' */
        len = (uint8_t) (right_ptr - str_token - 1);

        memcpy(temp, str_token, len);
        temp += len;
        str_token += len;
        if (i == '.' || i == '\\') {
            *temp = '\\';
            temp++;
            *temp++ = i;
            str_token += 1;

        } else if (i == 0x00u || i == '/' || i == ',' || i == '>')
            break;

    }
    *temp++ = '\0';
    return strncpy_m((char *) string, strSize, 1, output);
}

/***************************************************************
  Function:
    void mDNSSDFillResRecords(mdnsd_struct *sd)

  Summary:
    Fills the resource-records with the information received from
    sd structure-instance, in which the information is filled from
    user input.

  Description:
    This function is used to fill the resource-records according to
    format specified in RFC 1035.

    In this context Service-Instance + Service-Type is called fully
    qualified name. For ex: Dummy HTTP Web-Server._http._tcp.local
    where Dummy HTTP Web-Server is Service-instance name
     and  _http._tcp.local is Service-Type

    Each service-instance that needs to be advertised contains three
    resource-reocrds.
    1) PTR Resource-Record: This is a shared record, with service-type
                           as rr-name and fully-qualified name as
                           rr-data.
    2) SRV Resource-Record: This is a unique record, with fully-
                            qualified name as rr-name and Host-name,
                            port-num as rr-data.
    3) TXT Resource-Record: This is a unique record, with fully-
                            qualified name as rr-name and additional
                            information as rr-data like default-page
                            name (For ex: "/index.htm")

  Precondition:
    None

  Parameters:
    sd - Service-Discovery structure instance for which Resource-
         records to be filled.

  Returns:
    None
 **************************************************************/
void mDNSSDFillResRecords(mDNSProcessCtx_sd *sd)
{
    size_t srv_name_len, srv_type_len, qual_len;
    mDNSResourceRecord *rr_list = &(gResponderCtx.rr_list[QTYPE_PTR_INDEX]);

    srv_name_len = strlen((char *) sd->srv_name);
    srv_type_len = strlen((char *) sd->srv_type);

    memset(&(gResponderCtx.rr_list[QTYPE_PTR_INDEX]), 0, (sizeof (mDNSResourceRecord)));
    memset(&(gResponderCtx.rr_list[QTYPE_SRV_INDEX]), 0, (sizeof (mDNSResourceRecord)));
    memset(&(gResponderCtx.rr_list[QTYPE_TXT_INDEX]), 0, (sizeof (mDNSResourceRecord)));

    /* Formatting Service-Instance name.
     * And preparing a fully qualified
     * Service-instance name. */
    strncpy((char *) sd->sd_qualified_name, (char *) sd->srv_name, sizeof (sd->sd_qualified_name));
    qual_len = mDNSSDFormatServiceInstance(sd->sd_qualified_name, sizeof (sd->sd_qualified_name));

    // SOFTAP_ZEROCONF_SUPPORT
    // Overwritten with zeros inside the gSDCtx and the mDNS will start advertizing on port 0 instead of the normal port
    //strncpy_m((char *)sd->sd_qualified_name + qual_len, sizeof(sd->sd_qualified_name), 2, ".", sd->srv_type);
    strncpy_m((char *) sd->sd_qualified_name + qual_len, sizeof (sd->sd_qualified_name) - qual_len, 2, ".", sd->srv_type);

    DEBUG_MDNS_MESG(zeroconf_dbg_msg, "Fully Qualified Name: %s \r\n", sd->sd_qualified_name);
    DEBUG_MDNS_PRINT(zeroconf_dbg_msg);

    /* Fill-up PTR Record */
    rr_list->type.Val = QTYPE_PTR;
    rr_list->name = (uint8_t *) (sd->srv_type);

    /* Res Record Name is
     * Service_Instance_name._srv-type._proto.domain */
    rr_list->rdata = (uint8_t *) (sd->sd_qualified_name);

    strncpy_m((char *) rr_list->rdata + srv_name_len, sizeof (sd->sd_qualified_name) - srv_name_len, 2, ".", sd->srv_type);

    /* 3 bytes extra. One for dot added between
     * Serv-Name and Serv-Type. One for length byte.
     * added for first-label in fully qualified name
     * Other one for NULL terminator */
    rr_list->rdlength.Val = srv_name_len + srv_type_len + 3;
    rr_list->ttl.Val = RESOURCE_RECORD_TTL_VAL;
    rr_list->pOwnerCtx = (mDNSProcessCtx_common *) sd; /* Save back ptr */
    rr_list->valid = 1; /* Mark as valid */

    rr_list = &gResponderCtx.rr_list[QTYPE_SRV_INDEX]; /* Move onto next entry */

    /* Fill-up SRV Record */
    rr_list->name = (uint8_t *) (sd->sd_qualified_name);
    rr_list->type.Val = QTYPE_SRV;
    rr_list->ttl.Val = RESOURCE_RECORD_TTL_VAL;

    // rdlength is calculated/assigned last
    rr_list->srv.priority.Val = 0;
    rr_list->srv.weight.Val = 0;
    rr_list->srv.port.Val = sd->sd_port;

    /* Res Record Name is
     * Service_Instance_name._srv-type._proto.domain */
    rr_list->rdata = (uint8_t *) gHostCtx.szHostName;

    /* 2 bytes extra. One for Prefix Length for first-label.
     * Other one for NULL terminator */
    // then, add 6-byte extra: for priority, weight, and port

    rr_list->rdlength.Val = strlen((char *) rr_list->rdata) + 2 + 6;

    rr_list->pOwnerCtx = (mDNSProcessCtx_common *) sd; /* Save back ptr */
    rr_list->valid = 1; /* Mark as valid */

    rr_list = &gResponderCtx.rr_list[QTYPE_TXT_INDEX]; /* Move onto next entry */

    /* Fill-up TXT Record with NULL data */
    rr_list->type.Val = QTYPE_TXT;
    rr_list->name = (uint8_t *) (sd->sd_qualified_name);

    /* Res Record data is what defined by the user */
    rr_list->rdata = (uint8_t *) (sd->sd_txt_rec);

    /* Extra byte for Length-Byte of TXT string */
    rr_list->rdlength.Val = gSDCtx.sd_txt_rec_len + 1;
    rr_list->ttl.Val = RESOURCE_RECORD_TTL_VAL;
    rr_list->pOwnerCtx = (mDNSProcessCtx_common *) sd; /* Save back ptr */
    rr_list->valid = 1; /* Mark as valid */
}

/***************************************************************
  Function:
     MDNSD_ERR_CODE mDNSServiceUpdate(
        uint16_t port,
        uint16_t txt_len,
        const uint8_t *txt_record)

  Summary:
    DNS-Service Discovery API for end-user to update the service
    -advertisement, which was previously registered with
    mDNSServiceRegister

  Description:
    This API is used by end-user application to update its service
    which was previously registered. With this API end-user app
    update the port number on which the service is running. It can
    update the additional information of service. For example: the
    default page can be updated to new page and corresponding page
    name can be input to this API to update all peer machines. The
    modified service will be announced with new contents on local
    network.

    This is an optional API and should be invoked only if it is
    necessary.

  Precondition:
    mDNSServiceRegister must be invoked before this call.

  Parameters:
    port      - Port number on which service is running
    txt_len   - For additional information about service like
                default page (eg "index.htm") for HTTP-service.
                Length of such additional information
   txt_record - String of additional information (eg "index.htm")
                for HTTP-service.

  Returns:
    MDNSD_ERR_CODE - Returns Error-code to indicate registration is
                     success or not.
      1) MDNSD_SUCCESS  - returns on success of call
      2) MDNSD_ERR_INVAL - When the input parameters are invalid or
                           if the API is invoked in invalid state
 **************************************************************/
MDNSD_ERR_CODE
mDNSServiceUpdate(uint16_t port,
        const uint8_t *txt_record)
{
    mDNSProcessCtx_sd *sd = &gSDCtx;

    if (sd->used) {
        sd->service_registered = 0;
        sd->sd_port = port;
        /* Update Port Value in SRV Resource-record */
        gResponderCtx.rr_list[QTYPE_SRV_INDEX].srv.port.Val = port;

        if (txt_record != NULL) {
            sd->sd_txt_rec_len = strncpy_m((char *) sd->sd_txt_rec, sizeof (sd->sd_txt_rec), 1, (uint8_t *) txt_record);

            /* Update Resource-records for this
             * Service-instance, in MDNS-SD state-
             * -machine */
            mDNSSDFillResRecords(sd);
            sd->common.state = MDNS_STATE_NOT_READY;
        }

        /* Notify MDNS Stack about Service-Registration
         * to get a time-slot for its own processing */
        sd->service_registered = 1;
        return MDNSD_SUCCESS;
    } else
        return MDNSD_ERR_INVAL;
}

/***************************************************************
  Function:
     MDNSD_ERR_CODE mDNSServiceDeRegister()

  Summary:
    DNS-Service Discovery API for end-user to De-register a
    service-advertisement, which was previously registered with
    mDNSServiceRegister API.

  Description:
    This API is used by end-user application to de-register its
    service-advertisement on local network. When this gets invoked
    by end-user DNS-SD stack sends out Good-Bye packets to update
    all peer machines that service will no longer be present. All
    peer machines remove the corresponding entry from Browser list.

    This is the last API that needs to be invoked by end-user
    application to free-up the DNS-SD stack for some other app.

  Precondition:
    mDNSServiceRegister must be invoked before this call.

  Parameters:
    None

  Returns:
    MDNSD_ERR_CODE - Returns Error-code to indicate registration is
                     success or not.
      1) MDNSD_SUCCESS  - returns on success of call
      2) MDNSD_ERR_INVAL - When the input parameters are invalid or
                           if the API is invoked in invalid state
 **************************************************************/
MDNSD_ERR_CODE mDNSServiceDeRegister()
{
    mDNSProcessCtx_sd *sd = &gSDCtx;

    if (sd->used) {
        if (sd->sd_service_advertised == 1) {
            /* Send GoodBye Packet */
            gResponderCtx.rr_list[QTYPE_PTR_INDEX].ttl.Val = 0;
            gResponderCtx.rr_list[QTYPE_SRV_INDEX].ttl.Val = 0;
            gResponderCtx.rr_list[QTYPE_SRV_INDEX].ttl.Val = 0;

            mDNSSendRR(&gResponderCtx.rr_list[QTYPE_PTR_INDEX], 0, 0x00, 3, true, false);
            mDNSSendRR(&gResponderCtx.rr_list[QTYPE_SRV_INDEX], 0, 0x80, 3, false, false);
            mDNSSendRR(&gResponderCtx.rr_list[QTYPE_SRV_INDEX], 0, 0x80, 3, false, true);
        }
        /* Clear gSDCtx struct */
        sd->service_registered = 0;
        memset(sd, 0, sizeof (mDNSProcessCtx_sd));
        mDNS_responder_state = MDNS_RESPONDER_INIT;
        return MDNSD_SUCCESS;
    } else
        return MDNSD_ERR_INVAL; /* Invalid Parameter */
}

/***************************************************************
  Function:
     MDNSD_ERR_CODE mDNSServiceRegister( ...)

  Summary:
    DNS-Service Discovery API for end-user to register for a
    service-advertisement.

  Description:
    This API is used by end-user application to announce its
    service on local network. All peer machines that are compliant
    with Multicast-DNS & DNS-Service Discovery protocol can detect
    the announcement and lists out an entry in Service-Browser list.
    End-User selects an entry to connect to this service. So
    ultimately this is an aid to end-user to discover any services
    that he is interested in, on a local network.

    This is the first API that needs to be invoked by end-user
    application. Presently only Multicast-DNS & Service-discovery
    stack supports only single service-advertisement. Once the
    application wants to terminate the service it has to invoke
    mDNSServiceDeRegister() API to free-up the DNS-SD stack for
    some other application.

  Precondition:
    None

  Parameters:
    srv_name  - Service Name, which is being advertised
    srv_type  - For a HTTP-Service its "_http._tcp.local"
                _http: is application protocol preceeded with
                     under-score
                _tcp: is lower-layer protocol on which service runs
                local: is to represent service is on local-network

                For a iTunes Music Sharing "_daap._tcp.local"
                For a Priniting Service "_ipp._tcp.local"
                Refer to  http://www.dns-sd.org/ServiceTypes.html
                for more service types

    port      - Port number on which service is running
    txt_len   - For additional information about service like
                default page (eg "index.htm") for HTTP-service.
                Length of such additional information
   txt_record - String of additional information (eg "index.htm")
                for HTTP-service.

   auto_rename- A flag to indicate DNS-SD stack, whether to rename
                the service automatically or not.
                If this is set to '0' Callback parameter will be used
                to indicate the conflict error and user has to select
                different name and re-register with this API.
                If this is set to '1' service-name will be automatically
                renamed with numerical suffix.

    callback  - Callback function, which is user-application defined.
                This callback gets invoked on completion of service-
                advertisement. If an service name-conflit error is
                detected and auto_rename is set to '0' callback gets
                invoked with MDNSD_ERR_CONFLICT as error-code.

    context   - Opaque context (pointer to opaque data), which needs
                to be used in callback function.

  Returns:
    MDNSD_ERR_CODE - Returns Error-code to indicate registration is
                     success or not.
      1) MDNSD_SUCCESS  - returns on success of call
      2) MDNSD_ERR_BUSY - When already some other service is being
                          advertised using this DNS-SD stack
 **************************************************************/
MDNSD_ERR_CODE
mDNSServiceRegister(const char *srv_name,
        const char *srv_type,
        uint16_t port,
        const uint8_t *txt_record,
        uint8_t auto_rename,
        void (*call_back)(char *name, MDNSD_ERR_CODE err, void *context),
        void *context)
{
    if (gSDCtx.used) {
        WARN_MDNS_PRINT("mDNSServiceRegister: Some Other Service is registered" \
                "Currently only One Serv-Reg is allowed \r\n");
        return MDNSD_ERR_BUSY;
    }
    if ((srv_name == NULL) || (srv_type == NULL) || (txt_record == NULL)) {
        return MDNSD_ERR_INVAL; // Invalid Parameter
    }

    /* Clear the State-Machine */
    memset(&gSDCtx, 0, sizeof (mDNSProcessCtx_sd));
    gSDCtx.used = 1; /* Mark it as used */
    gSDCtx.sd_auto_rename = auto_rename;
    gSDCtx.sd_port = port;
    gSDCtx.sd_service_advertised = 0;
    strncpy((char *) gSDCtx.srv_name, (char *) srv_name, sizeof (gSDCtx.srv_name));

    strncpy((char *) gSDCtx.srv_type, (char *) srv_type, sizeof (gSDCtx.srv_type));
    gSDCtx.sd_call_back = call_back;
    gSDCtx.sd_context = context;

    gSDCtx.sd_txt_rec_len = strncpy_m((char *) gSDCtx.sd_txt_rec, sizeof (gSDCtx.sd_txt_rec), 1, (uint8_t *) txt_record);

    /* Fill up Resource-records for this
     * Service-instance, in MDNS-SD state-
     * -machine */
    mDNSSDFillResRecords(&gSDCtx);

    gSDCtx.common.type = MDNS_CTX_TYPE_SD;
    gSDCtx.common.state = MDNS_STATE_NOT_READY;
    gSDCtx.common.nInstanceId = 0;

    /* Notify MDNS Stack about Service-Registration
     * to get a time-slot for its own processing */
    gSDCtx.service_registered = 1;

    return MDNSD_SUCCESS;
}

/***************************************************************
  Function:
    static void mDNSSDStateMachineReset(mdnsd_struct *sd)

  Summary:
    Resets DNS-SD state-machine

  Description:
    This function is used to reset all state-variables related
    to DNS-SD state-machine.

  Precondition:
    None

  Parameters:
    sd - Service Discovery structure instance

  Returns:
    None
 **************************************************************/

/***************************************************************
  Function:
    void mDNSSDProbe(mdnsd_struct *sd)

  Summary:
    Sends out Multicast-DNS SD probe-packet with chosen service-name
    The Query-type is of SRV.

  Description:
    This function is used to send out mDNS-probe packet for
    checking uniqueness of selected service-instance-name. This
    function makes use of mDNSSendQuery to send out DNS-Query with
    chosen service-name to Multicast-Address. The type of such query
    is QTYPE_SRV.

    If any other machine is using same service-name, it responds with
    a reply and this host has to select different service-name.

  Precondition:
    None

  Parameters:
    sd - Service Discovery structure instance

  Returns:
    None
 **************************************************************/

/***************************************************************
  Function:
    void mDNSSDAnnounce(mdnsd_struct *sd)

  Summary:
    Sends out Multicast-DNS SD packet with SRV Resource-Record.

  Description:
    This function is used to send out DNS-SD SRV resource-record
    Announce packet for announcing the service-name on local network.
    This function makes use of mDNSSendRR to send out DNS-Resource-
    Record with chosen service-name+service-type as rr-name and the
    host-name, port-number as rr-data.

    This announcement updates DNS-caches of neighbor machines on
    the local network.

  Precondition:
    None

  Parameters:
    sd - Service Discovery structure instance

  Returns:
    None
 **************************************************************/
void mDNSAnnounce(mDNSResourceRecord *pRR)
{
    if (false ==
            mDNSSendRR(
            pRR,
            0,
            0x80,
            1,
            true,
            true
            )
            ) {
        WARN_MDNS_PRINT("mDNSAnnounce: Error in sending out Announce pkt \r\n");
    }
}

/***************************************************************
  Function:
    void mDNSSDTask(mdnsd_struct *sd, struct mDNSResourceRecord *rr,
                    MDNS_MSG_HEADER *mDNS_header)

  Summary:
    Processes the received Resource-Record, which can either be
    a query or response.

  Description:
    This function is an extension function mDNSResponder. If incoming
    query/response's resource-record doesn't match with Host-Name then
    the corresponding resource-record will be forwarded to DNS-SD state
    machine for further processing. This is invoked from mDNSResponder
    if the service is marked as registered.

    This routine compares it with existing resource-records and checks
    for any conflict. If it is a query and there's match with the
    publihed resource-records then response will be sent back with
    that resource-reocrd.

  Precondition:
    None

  Parameters:
    sd          - Service Discovery structure instance
    rr          -  Resource-Record filled with either DNS-query/
                   response
    mDNS_header - Multicast-DNS Message header filled with incoming
                  query/response information

  Returns:
    None
 **************************************************************/

/***************************************************************
  Function:
    void mDNSSDProcess(mdnsd_struct *sd)

  Summary:
    Main routine for DNS-SD state-machine.

  Description:
    This function is main routine of DNS-SD state-machine and invoked
    from Multicast-DNS state-machine routine. Based on current state
    and the additional flag-bits the coresponding actions will be taken

    Once the user selected service-name is finalized the user-callback
    registered with DNS-SD will be invoked to notify that the service
    is advertised successfully. Two gracious announcement packets with
    service-information will be sent out to update Neighbor caches and
    existing service-browsers.

  Precondition:
    None

  Parameters:
    sd          - Service Discovery structure instance

  Returns:
    None
 **************************************************************/
#ifdef DEBUG_MDNS
MDNS_MSG_HEADER *dbg_p_mDNS_header;
mDNSResourceRecord *dbg_p_res_rec;
#endif

MDNS_STATIC uint16_t mDNSFetch(uint16_t wOffset, uint16_t wLen, uint8_t *pcString)
{
    uint16_t rc;

    UDPSetRxBuffer(wOffset);

    rc = UDPGetArray(pcString, wLen);

    return rc;
}

/***************************************************************
  Function:
    static uint16_t mDNSDeCompress(uint16_t wPos,
                               uint8_t *pcString,
                               bool bFollowPtr,
                               uint8_t cElement,
                               uint8_t cDepth)

  Summary:
    Read a string from a resource record, from the Multicast-DNS socket buffer.

  Description:
    This function reads a string to the Multicast-DNS socket,
    ensuring that it is properly formatted.

    String may be reconstructed through decompression if necessary.
    Decompression pointer traversal is done in place, recursively, in UDP's RxBuffer.

    cDepth represents the recursion depth, for debugging purpose.

    cElement represents the number of elements in the string. For example,
    "ezconfig._http._tcp.local" has 4 elements.

    bFollowPtr indicates if DNS compression offset needs to be followed. That is, if
    we should reconstruct a compressed string.

    The reconstructed string is placed in pcString, if it is not NULL.

    For DNS message compression format, see RFC 1035, section 4.1.4.

  Precondition:
    UDP socket is obtained and ready for writing.
    wPos correctly reflect the current position in the UDP RxBuffer.

  Parameters:
    String - the string to write to the UDP socket.

  Returns:
    Number of bytes in THIS resource record field (in RFC 1035's term, NAME or RDATA).
    UDP RxBuffer pointer is repositioned to the place right after THIS resource record field.

 **************************************************************/
MDNS_STATIC uint16_t mDNSDeCompress(uint16_t wPos, uint8_t *pcString, bool bFollowPtr, uint8_t cElement, uint8_t cDepth)
{
    uint16_t rr_len = 0; // As is in the packet. Could be in compressed format.
    uint16_t startOffset, endOffset;
    uint8_t i, tmp;
    uint16_t offset_in_ptr;
    uint8_t substr_len;

    startOffset = wPos;

    while (1) {
        rr_len++;
        if (!UDPGet(&substr_len))
            goto mDNSDeCompress_done;

        if (substr_len == 0u) {
            if (pcString) {
                *pcString++ = '\0';
            }
            goto mDNSDeCompress_done;
        }

        if ((substr_len & 0xC0) == 0xC0) // b'11 at MSb indicates compression ptr
        {
            offset_in_ptr = substr_len & 0x3F; // the rest of 6 bits is part of offset_in_ptr.
            offset_in_ptr = offset_in_ptr << 8;

            /* Remove label-ptr byte */
            rr_len++;
            UDPGet(&i);
            offset_in_ptr += i;

            if (bFollowPtr) {
                cDepth++;

                DEBUG_MDNS_MESG(zeroconf_dbg_msg, "follow ptr: h'%X, ", offset_in_ptr);
                DEBUG_MDNS_PRINT(zeroconf_dbg_msg);
                DEBUG_MDNS_MESG(zeroconf_dbg_msg, "%d\r\n", cDepth);
                DEBUG_MDNS_PRINT(zeroconf_dbg_msg);

                UDPSetRxBuffer(offset_in_ptr);
                mDNSDeCompress(offset_in_ptr, pcString, bFollowPtr, cElement, cDepth);

                // compressed ptr is always the last element
                goto mDNSDeCompress_done;
            }

            goto mDNSDeCompress_done;
        } else {
            if (pcString) {
                if (cElement > 0) {
                    // not the first element in name
                    *pcString++ = '.';
                }

                UDPGetArray(pcString, substr_len);
                pcString += substr_len;
            } else {
                i = substr_len;
                while (i--) {
                    UDPGet(&tmp);
                }
            }

            cElement++;
            rr_len += substr_len;
        }
    }

mDNSDeCompress_done:

    endOffset = startOffset + rr_len;
    UDPSetRxBuffer(endOffset);

    return rr_len;
}

/***************************************************************
  Function:
    void mDNSResponder(void)

  Summary:
    Acts as Multicast-DNS respoder & replies when it receives
    a query.

  Description:
    This function is used as mDNS-Responder. On initialization of
    Multicast-DNS stack, this function Opens up mDNS_socket
    (UDP-Socket) for Mulitcast-Address (224.0.0.251).

    This function gets polled from mDNSProcess for every iteration.
    mDNSResponder constantly monitors the packets being sent to
    Multicast-Address, to check whether it is a conflict with
    its own host-name/resource-record names. It also verifies
    whether incoming query is for its own Host-name/Resource-
    Record, in which case it sends back a reply with corresponding
    Resource-Record.

  Precondition:
    UDP socket (mDNS_socket) is obtained and ready for writing.
    A UDP socket must be available before this function is called.
    MAX_UDP_SOCKETS may need to be increased if other modules use
    UDP sockets.

  Parameters:
    None

  Returns:
    None
 **************************************************************/
MDNS_STATIC bool
mDNSTieBreaker(mDNSResourceRecord *their, mDNSResourceRecord *our)
{
    bool WeWonTheTieBreaker = true;
    uint8_t i;

    if (their->type.Val == QTYPE_A) {
        for (i = 0; i <= 3; i++) {
            if (their->ip.v[i] < our->ip.v[i]) {
                WeWonTheTieBreaker = true;
                break;
            } else if (their->ip.v[i] > our->ip.v[i]) {
                WeWonTheTieBreaker = false;
                break;
            }
        }
    } else if (their->type.Val == QTYPE_SRV) {
        if (their->srv.port.Val >= our->srv.port.Val) {
            WeWonTheTieBreaker = false;
        }
    }

    DEBUG0_MDNS_PRINT((char *) (WeWonTheTieBreaker ? "   tie-breaker won\r\n" : "   tie-breaker lost\r\n"));

    return WeWonTheTieBreaker;
}

MDNS_STATIC uint8_t
mDNSProcessIncomingRR(MDNS_RR_GROUP tag,
        MDNS_MSG_HEADER * pmDNSMsgHeader,
        uint16_t idxGroup,
        uint16_t idxRR)
{
    mDNSResourceRecord res_rec;
    uint8_t name[MAX_RR_NAME_SIZE];
    uint16_t i, j;
    uint16_t len;
    mDNSProcessCtx_common *pOwnerCtx;
    mDNSResourceRecord *pMyRR;
    bool WeWonTheTieBreaker = false;
    bool bMsgIsAQuery; // QUERY or RESPNSE ?
    bool bSenderHasAuthority; // Sender has the authority ?

    bMsgIsAQuery = (pmDNSMsgHeader->flags.bits.qr == 0);
    bSenderHasAuthority = (pmDNSMsgHeader->flags.bits.qr == 1);

    res_rec.name = name; // for temporary name storage.

#ifdef DEBUG_MDNS
    dbg_p_mDNS_header = &mDNS_header;
    dbg_p_res_rec = &res_rec;
#endif

    DEBUG0_MDNS_MESG(
            zeroconf_dbg_msg,
            "   rec [%d:%d]\t",
            idxGroup, idxRR);
    DEBUG0_MDNS_PRINT(zeroconf_dbg_msg);

    // NAME
    memset(name, 0, MAX_RR_NAME_SIZE);
    len = mDNSDeCompress(g_mDNS_offset, name, true, 0, 0);
    g_mDNS_offset += len;

    // TYPE & CLASS
    UDPGet(&res_rec.type.v[1]);
    UDPGet(&res_rec.type.v[0]);
    UDPGet(&res_rec.class.v[1]);
    UDPGet(&res_rec.class.v[0]);
    g_mDNS_offset += 4;

    DEBUG0_MDNS_PRINT("Name: ");
    DEBUG0_MDNS_PRINT((char *) name);
    DEBUG0_MDNS_MESG(zeroconf_dbg_msg, " Type: %d", res_rec.type.Val);
    DEBUG0_MDNS_PRINT((char *) zeroconf_dbg_msg);
    DEBUG0_MDNS_PRINT("\r\n");

    // Do the first round name check
    for (i = 0; i < MAX_RR_NUM; i++) {
        gResponderCtx.rr_list[i].bNameAndTypeMatched = false;

        if (
                !strcmp_local_ignore_case((void *) name, gResponderCtx.rr_list[i].name)
                &&
                ((res_rec.type.Val == QTYPE_ANY) ||
                (res_rec.type.Val == gResponderCtx.rr_list[i].type.Val))
                ) {
            gResponderCtx.rr_list[i].bNameAndTypeMatched = true;
        } else if (
                (tag == MDNS_RR_GROUP_QD)
                &&
                !strcmp_local_ignore_case(name, (uint8_t *) "_services._dns-sd._udp.local")
                &&
                (res_rec.type.Val == QTYPE_PTR)
                ) {
            gResponderCtx.rr_list[i].bNameAndTypeMatched = true;
        }
    }

    // Only AN, NS, AR records have extra fields
    if (tag == MDNS_RR_GROUP_QD) {
        goto ReviewStage;
    }

    // Now retrieve those extra fields

    UDPGet(&res_rec.ttl.v[3]); // Time to live
    UDPGet(&res_rec.ttl.v[2]);
    UDPGet(&res_rec.ttl.v[1]);
    UDPGet(&res_rec.ttl.v[0]);
    UDPGet(&res_rec.rdlength.v[1]); // Response length
    UDPGet(&res_rec.rdlength.v[0]);
    g_mDNS_offset += 6;

    // The rest is record type dependent

    switch (res_rec.type.Val) {
    case QTYPE_A:
        UDPGet(&res_rec.ip.v[0]); // Read out IP address
        UDPGet(&res_rec.ip.v[1]);
        UDPGet(&res_rec.ip.v[2]);
        UDPGet(&res_rec.ip.v[3]);

        g_mDNS_offset += 4;

        DEBUG_MDNS_MESG(zeroconf_dbg_msg, "     [A]: TTL=%ld\r\n", res_rec.ttl.Val);
        DEBUG_MDNS_PRINT((char *) zeroconf_dbg_msg);

        break;

    case QTYPE_PTR:
        memset(name, 0, MAX_RR_NAME_SIZE);
        len = mDNSDeCompress(g_mDNS_offset, name, true, 0, 0);
        g_mDNS_offset += len;

        DEBUG_MDNS_MESG(zeroconf_dbg_msg, "     [PTR]: TTL=%ld RDATA=%s\r\n", res_rec.ttl.Val, name);
        DEBUG_MDNS_PRINT((char *) zeroconf_dbg_msg);

        break;

    case QTYPE_SRV:
        UDPGet(&res_rec.srv.priority.v[1]); // Put Priority, weight, port
        UDPGet(&res_rec.srv.priority.v[0]);
        UDPGet(&res_rec.srv.weight.v[1]);
        UDPGet(&res_rec.srv.weight.v[0]);
        UDPGet(&res_rec.srv.port.v[1]);
        UDPGet(&res_rec.srv.port.v[0]);

        g_mDNS_offset += 6;

        memset(name, 0, MAX_RR_NAME_SIZE);
        len = mDNSDeCompress(g_mDNS_offset, name, true, 0, 0);
        g_mDNS_offset += len;

        DEBUG_MDNS_MESG(zeroconf_dbg_msg, "     [SRV]: TTL=%ld RDATA=%s\r\n", res_rec.ttl.Val, name);
        DEBUG_MDNS_PRINT((char *) zeroconf_dbg_msg);

        break;

    case QTYPE_TXT:
        UDPGetArray(NULL, res_rec.rdlength.Val);

        g_mDNS_offset += res_rec.rdlength.Val;

        DEBUG_MDNS_MESG(zeroconf_dbg_msg, "     [TXT]: (%d bytes)\r\n", res_rec.rdlength.Val);
        DEBUG_MDNS_PRINT((char *) zeroconf_dbg_msg);

        break;

    default:
        // Still needs to read it off
        UDPGetArray(NULL, res_rec.rdlength.Val);

        g_mDNS_offset += res_rec.rdlength.Val;

        DEBUG_MDNS_MESG(zeroconf_dbg_msg, "     [*]: (%d bytes)\r\n", res_rec.rdlength.Val);
        DEBUG_MDNS_PRINT((char *) zeroconf_dbg_msg);

        break;
    }

    // We now have all info about this received RR.

ReviewStage:

    // Do the second round
    for (i = 0; i < MAX_RR_NUM; i++) {
        pMyRR = &(gResponderCtx.rr_list[i]);
        pOwnerCtx = gResponderCtx.rr_list[i].pOwnerCtx;

        if ((!pMyRR->bNameAndTypeMatched) || (pOwnerCtx == NULL)) {
            // do nothing
        } else if (
                bMsgIsAQuery &&
                (tag == MDNS_RR_GROUP_QD) &&
                (pOwnerCtx->state == MDNS_STATE_DEFEND)
                ) {
            // Simple reply to an incoming DNS query.
            // Mark all of our RRs for reply.

            for (j = 0; j < MAX_RR_NUM; j++) {
                gResponderCtx.rr_list[j].bResponseRequested = true;
            }
        } else if (
                bMsgIsAQuery &&
                (tag == MDNS_RR_GROUP_AN) &&
                (pOwnerCtx->state == MDNS_STATE_DEFEND)
                ) {
            // An answer in the incoming DNS query.
            // Look for possible duplicate (known) answers suppression.

            if ((((res_rec.type.Val == QTYPE_PTR) && (res_rec.ip.Val == gResponderCtx.rr_list[i].ip.Val))
                    ||
                    (!strcmp_local_ignore_case(name, gResponderCtx.rr_list[i].rdata)))
                    &&
                    (res_rec.ttl.Val > (gResponderCtx.rr_list[i].ttl.Val / 2))
                    ) {
                gResponderCtx.rr_list[i].bResponseSuppressed = true;
                DEBUG_MDNS_PRINT("     rr suppressed\r\n");
            }
        } else if (
                bMsgIsAQuery &&
                (tag == MDNS_RR_GROUP_NS) &&
                ((pOwnerCtx->state == MDNS_STATE_PROBE) ||
                (pOwnerCtx->state == MDNS_STATE_ANNOUNCE))
                ) {
            // Simultaneous probes by us and sender of this DNS query.
            // Mark as a conflict ONLY IF we lose the Tie-Breaker.

            WeWonTheTieBreaker = mDNSTieBreaker(&res_rec,
                    &(gResponderCtx.rr_list[i]));

            if (!WeWonTheTieBreaker) {
                pOwnerCtx->bProbeConflictSeen = true;
                pOwnerCtx->nProbeConflictCount++;
            }

            UDPDiscard();

            return 0;
        } else if (
                !bMsgIsAQuery &&
                bSenderHasAuthority &&
                (tag == MDNS_RR_GROUP_AN) &&
                ((pOwnerCtx->state == MDNS_STATE_PROBE) ||
                (pOwnerCtx->state == MDNS_STATE_ANNOUNCE))
                ) {
            // An authoritative DNS response to our probe/announcement.
            // Mark as a conflict. Effect a re-name, followed by a
            // re-probe.

            pOwnerCtx->bProbeConflictSeen = true;
            pOwnerCtx->nProbeConflictCount++;

            UDPDiscard();

            return 0;
        } else if (
                bMsgIsAQuery &&
                (tag == MDNS_RR_GROUP_NS) &&
                (pOwnerCtx->state == MDNS_STATE_DEFEND)
                ) {
            // A probe by the sender conflicts with our established record.
            // Need to defend our record. Effect a DNS response.

            INFO_MDNS_PRINT("Defending RR: \r\n");

            pMyRR->bResponseRequested = true;

            UDPDiscard();

            return 0;
        } else if (
                !bMsgIsAQuery &&
                bSenderHasAuthority &&
                (tag == MDNS_RR_GROUP_AN) &&
                (pMyRR->type.Val != QTYPE_PTR) && // No one can claim authority on shared RR
                (pOwnerCtx->state == MDNS_STATE_DEFEND)
                ) {
            // Sender claims that it also has the authority on
            // a unique (non-shared) record that we have already established authority.
            // Effect a re-probe.

            pOwnerCtx->bLateConflictSeen = true;

            UDPDiscard();

            return 0;
        }
    }
    return 0;
}

MDNS_STATIC void mDNSResponder(void)
{
    MDNS_MSG_HEADER mDNS_header;

#if 0 // SOFTAP_ZEROCONF_SUPPORT
    static enum {
        MDNS_RESPONDER_INIT = 0,
        MDNS_RESPONDER_LISTEN,
    } mDNS_responder_state = MDNS_RESPONDER_INIT;
#endif

    uint16_t len;
    uint16_t i, j, count;

    uint16_t rr_count[4];
    MDNS_RR_GROUP rr_group[4];

    bool bMsgIsComplete;

    g_mDNS_offset = 0;

    if (mDNS_socket == INVALID_UDP_SOCKET) {
        mDNS_responder_state = MDNS_RESPONDER_INIT;
    }
    switch (mDNS_responder_state) {
    case MDNS_RESPONDER_INIT:

        /* Open a UDP socket for inbound and outbound transmission
         * Since we expect to only receive multicast packets and
         * only send multicast packets the remote NodeInfo
         * parameter is initialized to Multicast-IP (224.0.0.251)
         * corresponding Multicast MAC-Address (01:00:5E:00:00:FB) */

        mDNSRemote.IPAddr.v[0] = 0xE0;
        mDNSRemote.IPAddr.v[1] = 0;
        mDNSRemote.IPAddr.v[2] = 0;
        mDNSRemote.IPAddr.v[3] = 0xFB;

        mDNSRemote.MACAddr.v[0] = 0x01;
        mDNSRemote.MACAddr.v[1] = 0x00;
        mDNSRemote.MACAddr.v[2] = 0x5E;
        mDNSRemote.MACAddr.v[3] = 0x00;
        mDNSRemote.MACAddr.v[4] = 0x00;
        mDNSRemote.MACAddr.v[5] = 0xFB;

        INFO_MDNS_PRINT("mDNSResponder: MDNS_RESPONDER_INIT: Opening mDNS socket \r\n");
        mDNS_socket = UDPOpenEx((uint32_t) (PTR_BASE) & mDNSRemote, UDP_OPEN_NODE_INFO, MDNS_PORT, MDNS_PORT);

        if (mDNS_socket == INVALID_UDP_SOCKET) {
            WARN_MDNS_PRINT("mDNSResponder: Can't open Multicast-DNS UDP-Socket \r\n");
            return;
        } else
            mDNS_responder_state = MDNS_RESPONDER_LISTEN;
        /* Called from mDNSInitialize. So return immediately */
        break;

    case MDNS_RESPONDER_LISTEN:

        // Do nothing if no data is waiting
        if (!UDPIsGetReady(mDNS_socket))
            return;

        INFO_MDNS_PRINT("mDNSResponder: MDNS_RESPONDER_LISTEN \r\n");

        if (UDPSocketInfo[mDNS_socket].remotePort != MDNS_PORT) {
            // If the remote port (sender's src port)
            // is not MDNS_PORT (5353), then it is a multicast query
            // reqeusting a unicast response (even though the packet
            // was sent to the multicast group IP:MDNS_PORT ).
            // The response needs to be unicast, and sent
            // to sender:port, NOT to the multicast group IP:MDSN_PORT
            // (i.e., 224.0.0.251:5353).

            // See section 8.5, draft-cheshire-dnsext-multicastdns-08.txt.
        } else {
            /* Reset the Remote-node information in UDP-socket */
            memcpy((void *) &UDPSocketInfo[mDNS_socket].remote.remoteNode,
                    (const void *) &mDNSRemote, sizeof (mDNSRemote));
            UDPSocketInfo[mDNS_socket].remotePort = MDNS_PORT;
            UDPSocketInfo[mDNS_socket].localPort = MDNS_PORT;
        }

        // Retrieve the mDNS header
        len = mDNSFetch(0, sizeof (mDNS_header), (uint8_t *) & mDNS_header);
        mDNS_header.query_id.Val = swaps(mDNS_header.query_id.Val);
        mDNS_header.flags.Val = swaps(mDNS_header.flags.Val);
        mDNS_header.nQuestions.Val = swaps(mDNS_header.nQuestions.Val);
        mDNS_header.nAnswers.Val = swaps(mDNS_header.nAnswers.Val);
        mDNS_header.nAuthoritativeRecords.Val = swaps(mDNS_header.nAuthoritativeRecords.Val);
        mDNS_header.nAdditionalRecords.Val = swaps(mDNS_header.nAdditionalRecords.Val);

        g_mDNS_offset += len; // MUST BE 12

        if ((mDNS_header.flags.bits.qr == 0)) {
            DEBUG0_MDNS_PRINT("rx QUERY \r\n");
        } else {
            DEBUG0_MDNS_PRINT("rx RESPONSE \r\n");
        }

        bMsgIsComplete = (mDNS_header.flags.bits.tc == 0); // Message is not truncated.

        rr_count[0] = mDNS_header.nQuestions.Val;
        rr_group[0] = MDNS_RR_GROUP_QD;

        rr_count[1] = mDNS_header.nAnswers.Val;
        rr_group[1] = MDNS_RR_GROUP_AN;

        rr_count[2] = mDNS_header.nAuthoritativeRecords.Val;
        rr_group[2] = MDNS_RR_GROUP_NS;

        rr_count[3] = mDNS_header.nAdditionalRecords.Val;
        rr_group[3] = MDNS_RR_GROUP_AR;

        for (i = 0; i < MAX_RR_NUM; i++) {
            // Reset flags
            gResponderCtx.rr_list[i].bNameAndTypeMatched = false;

            if (gResponderCtx.bLastMsgIsIncomplete) {
                // Do nothing.
                // Whether a reply is needed is determined only when all parts
                // of the message are received.

                // Ideally, we want to verify that the current message is the
                // continuation of the previous message.
                // Don't have a cost-effective way to do this yet.
            } else {
                // Start of a new message

                gResponderCtx.rr_list[i].bResponseRequested = false;
                gResponderCtx.rr_list[i].bResponseSuppressed = false;
            }
        }

        for (i = 0; i < 4; i++) // for all 4 groups: QD, AN, NS, AR
        {
            for (j = 0; j < rr_count[i]; j++) // RR_count = {#QD, #AN, #NS, #AR}
            {
                mDNSProcessIncomingRR
                        (
                        rr_group[i],
                        &mDNS_header,
                        i,
                        j
                        );

            }
        }

        // Record the fact, for the next incoming message.
        gResponderCtx.bLastMsgIsIncomplete = (bMsgIsComplete == false);

        // Do not reply any answer if the current message is not the last part of
        // the complete message.
        // Future parts of the message may request some answers be suppressed.

        if (!bMsgIsComplete) {
            DEBUG0_MDNS_PRINT("   truncated msg.\r\n");
            return;
        }

        // Count all RRs marked as "reply needed".
        count = 0;
        for (i = 0; i < MAX_RR_NUM; i++) {
            if ((gResponderCtx.rr_list[i].pOwnerCtx != NULL) &&
                    (gResponderCtx.rr_list[i].pOwnerCtx->state == MDNS_STATE_DEFEND) &&
                    (gResponderCtx.rr_list[i].bResponseRequested == true) &&
                    (gResponderCtx.rr_list[i].bResponseSuppressed == false)
                    ) {
                count++;
            }
        }

        // Send all RRs marked as "reply needed".
        j = 1;
        for (i = 0; (count > 0) && (i < MAX_RR_NUM); i++) {
            if ((gResponderCtx.rr_list[i].pOwnerCtx != NULL) &&
                    (gResponderCtx.rr_list[i].pOwnerCtx->state == MDNS_STATE_DEFEND) &&
                    (gResponderCtx.rr_list[i].bResponseRequested == true) &&
                    (gResponderCtx.rr_list[i].bResponseSuppressed == false)) {
                mDNSSendRR(
                        &gResponderCtx.rr_list[i],
                        mDNS_header.query_id.Val,
                        (gResponderCtx.rr_list[i].type.Val == QTYPE_PTR) ?
                        (0x00) : (0x80), // flush, except for PTR; for Conformance Test.
                        count, // MAX_RR_NUM answers;
                        (j == 1) ? true : false, // Is this the first RR?
                        (j == count) ? true : false // Is this the last RR?
                        );
                j++;
            }
        }

        // end of MDNS_RESPONDER_LISTEN
        break;

    default:
        break;
    }

    return;
}

/***************************************************************
  Function:
    static void mDNSStateMachineReset(bool bResetProbeCount)

  Summary:
    Resets Multicast-DNS state-machine

  Description:
    This function is used to reset all global-variables related
    to Multicast-DNS state-machine.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None
 **************************************************************/

/***************************************************************
  Function:
    static void mDNSProbe(uint8_t *name, MDNS_QTYPE q_type)

  Summary:
    Sends out Multicast-DNS probe packet with Host-name

  Description:
    This function is used to send out mDNS-probe packet for
    checking uniqueness of selected host-name. This function makes
    use of mDNSSendQuery to send out DNS-Query with chosen host-name
    to Multicast-Address.

    If any other machine is using same host-name, it responds with
    a reply and this host has to select different name.

  Precondition:
    None

  Parameters:
    name - Selected Host-Name
    Type - Query Type, which is of TYPE_A

  Returns:
    None
 **************************************************************/

/***************************************************************
  Function:
    static void mDNSAnnounce(uint8_t *name, MDNS_QTYPE q_type)

  Summary:
    Sends out Multicast-DNS Announce/Claim packet with Host-name

  Description:
    This function is used to send out mDNS-Announce packet for
    announcing the selected host-name. This function makes
    use of mDNSSendRR to send out DNS-Resource-Rec with chosen
    host-name to Multicast-Address.

    This announcement updates DNS-caches of neighbor machines on
    the local network.

  Precondition:
    None

  Parameters:
    name - Selected Host-Name
    Type - Query Type, which is of TYPE_A

  Returns:
    None
 **************************************************************/

/***************************************************************
  Function:
    void mDNSInitialize(void)

  Summary:
    Initialization routine for Multicast-DNS (mDNS) state-machine.

  Description:
    This is initialization function for mDNS and invoked from init
    portion of Main-function.

    This initalizes the Multicast-DNS Responder (mDNSResponder) by
    opening up Multicast UDP socket on which mDNSResponder keeps on
    listening for incoming queries/responses from neighbor machines.

    The host-name chosen is initially seeded from DEFUALT_HOST_NAME
    defined in tcpip_config.h.

  Precondition:
    None

  Parameters:
    None

  Returns:
    None
 **************************************************************/
void mDNSFillHostRecord(void)
{
    uint8_t i;

    // Fill the type A resource record
    gResponderCtx.rr_list[QTYPE_A_INDEX].name = gHostCtx.szHostName;
    gResponderCtx.rr_list[QTYPE_A_INDEX].type.Val = QTYPE_A;
    gResponderCtx.rr_list[QTYPE_A_INDEX].ttl.Val = RESOURCE_RECORD_TTL_VAL;

    gResponderCtx.rr_list[QTYPE_A_INDEX].rdlength.Val = 4u; // 4-byte for IP address

    for (i = 0; i <= 3; i++)
        gResponderCtx.rr_list[QTYPE_A_INDEX].ip.v[i] = AppConfig.MyIPAddr.v[i];

    gResponderCtx.rr_list[QTYPE_A_INDEX].valid = 1;
    gResponderCtx.rr_list[QTYPE_A_INDEX].pOwnerCtx = (mDNSProcessCtx_common *) & gHostCtx;
}

MDNSD_ERR_CODE mDNSHostRegister(const char *host_name)
{
    if (host_name == 0) {
#ifdef MY_DEFAULT_HOST_NAME
        host_name = MY_DEFAULT_HOST_NAME;
#else
        host_name = "MyHost";
#endif
    }

    strncpy((char *) gHostCtx.szUserChosenHostName, host_name, sizeof (gHostCtx.szUserChosenHostName));
    strncpy_m((char *) gHostCtx.szHostName, sizeof (gHostCtx.szHostName), 3, gHostCtx.szUserChosenHostName, ".", CONST_STR_local);

    mDNSResetCounters((mDNSProcessCtx_common *) & gHostCtx, true);
    gHostCtx.common.type = MDNS_CTX_TYPE_HOST;
    gHostCtx.common.state = MDNS_STATE_INIT;
    gHostCtx.common.nInstanceId = 0;

    mDNSFillHostRecord();
    gResponderCtx.bLastMsgIsIncomplete = false;

    gHostCtx.common.state = MDNS_STATE_INIT;

    return MDNSD_SUCCESS;
}

void mDNSInitialize(const char *szHostName)
{
    gResponderCtx.query_id.Val = 0;
    gResponderCtx.prev_ipaddr.Val = AppConfig.MyIPAddr.Val;

    /* Initial Host-Name is seeded from DEFUALT_HOST_NAME
     * configured in tcpip_config.h. Later on if a name-conflict
     * is detected it'll be automatically renamed with a
     * numerical-label extenstion */

    if (szHostName != NULL) {
        mDNSHostRegister(szHostName);
    } else {
#ifdef MY_DEFAULT_HOST_NAME
        mDNSHostRegister((const char *) MY_DEFAULT_HOST_NAME);
#else
        mDNSHostRegister((const char *) "ZCHOST"); // Hardcoded default.
#endif
    }

    if (!MACIsLinked()) {
        gHostCtx.common.state = MDNS_STATE_INTF_NOT_CONNECTED;
        return;
    }

    /* Initialize MDNS-Responder by opening up
     * Multicast-UDP-Socket */
    mDNSResponder();

    gHostCtx.common.state = MDNS_STATE_INIT;

    return;
}

/***************************************************************
  Function:
    void mDNSProcess(void)

  Summary:
    Main routine for Multicast-DNS (mDNS) state-machine.

  Description:
    This function is polled from Main-Routine & Designed to support
    co-operative multi-tasking. This has to retrun to Main, if we
    have to wait for longer durations.

    This is the main routine of mDNS state-machine. Based on current
    state and additional flag-bits the coresponding actions will be
    taken.

    Once the chosen host-name is finalized two gracious announcement
    packets with host-name will be sent out to update Neighbor DNS-
    -caches

  Precondition:
    mDNSInitialize should have been called before.

  Parameters:
    None

  Returns:
    None
 **************************************************************/

void mDNSProcessInternal(mDNSProcessCtx_common *pCtx)
{
    bool bIsHost = (((void *) pCtx) == ((void *) &gHostCtx));

    switch (pCtx->state) {
    case MDNS_STATE_HOME:

        DEBUG_MDNS_PRINT("MDNS_STATE_HOME: Wrong state \r\n");
        break;

    case MDNS_STATE_NOT_READY: // SD starts from here. SD only.

        if (gHostCtx.common.state != MDNS_STATE_DEFEND) {
            /* Multicast DNS is not ready */
            return;
        } else {
            /* Multicast DNS is ready now */
            pCtx->state = MDNS_STATE_INIT;
            pCtx->time_recorded = 0;
        }

        INFO_MDNS_PRINT("MDNS_STATE_NOT_READY --> MDNS_STATE_INIT \r\n");
        break;

    case MDNS_STATE_INTF_NOT_CONNECTED: // HOST starts from here. HOST only.

        if (!MACIsLinked())
            return;

        else {
            /* Interface is connected now */
            pCtx->state = MDNS_STATE_IPADDR_NOT_CONFIGURED;
            pCtx->time_recorded = 0;
        }

        // No break. Fall through

    case MDNS_STATE_IPADDR_NOT_CONFIGURED: // HOST only.
    {
        // Wait until IP addr is configured ...
        if (AppConfig.MyIPAddr.Val == 0)
            break;

        pCtx->state = MDNS_STATE_INIT;
        pCtx->time_recorded = 0;

        INFO_MDNS_PRINT("MDNS_STATE_IPADDR_NOT_CONFIGURED --> MDNS_STATE_INIT \r\n");

        // No break. Fall through
    }

    case MDNS_STATE_INIT:
    {
        //DEBUG_MDNS_MESG(zeroconf_dbg_msg,"MDNS_STATE_INIT \r\n");
        //DEBUG_MDNS_PRINT((char *)zeroconf_dbg_msg);

        pCtx->bConflictSeenInLastProbe = false;

        switch (zgzc_wait_for(&(pCtx->random_delay), &(pCtx->event_time), &(pCtx->time_recorded))) {
        case ZGZC_STARTED_WAITING:

            // Need to choose Random time between 0-MDNS_PROBE_WAIT msec

            pCtx->random_delay = (TICK) ((LFSRRand() % (MDNS_PROBE_WAIT) * (TICK_SECOND / 1000)));
            DEBUG_MDNS_MESG(zeroconf_dbg_msg, "MDNS_PROBE_WAIT Random Delay: %ld ticks\r\n",
                    pCtx->random_delay);
            DEBUG_MDNS_PRINT((char *) zeroconf_dbg_msg);

            // Intentional fall-through

        case ZGZC_KEEP_WAITING:

            // Not Completed the delay proposed
            return;
        }

        // Completed the delay required

        DEBUG_MDNS_MESG(zeroconf_dbg_msg, "MDNS_PROBE_WAIT Random Delay: %ld ticks" \
                " Completed \r\n",
                pCtx->random_delay);
        DEBUG_MDNS_PRINT((char *) zeroconf_dbg_msg);

        // Clear all counters
        mDNSResetCounters(pCtx, true);

        pCtx->state = MDNS_STATE_PROBE;
        INFO_MDNS_PRINT("MDNS_STATE_INIT --> MDNS_STATE_PROBE \r\n");

        // No break. Fall through
    }

    case MDNS_STATE_PROBE:
    case MDNS_STATE_ANNOUNCE:
    {
        //DEBUG_MDNS_PRINT("MDNS_STATE_PROBE \n");
        // or
        //DEBUG_MDNS_PRINT("MDNS_CLAIM \n");

        if (pCtx->bProbeConflictSeen) {
            pCtx->bConflictSeenInLastProbe = true;

            INFO_MDNS_PRINT("Conflict detected. Will rename\r\n");

            /* Conflict with selected name */
            pCtx->state = MDNS_STATE_PROBE;

            // Do not reset nProbeConflictCount if in PROBE state
            mDNSResetCounters(
                    pCtx,
                    (pCtx->state == MDNS_STATE_PROBE) ? false : true
                    );

            if (bIsHost) {
                // Rename host name
                mDNSRename(
                        gHostCtx.szUserChosenHostName,
                        ++gHostCtx.common.nInstanceId,
                        (uint8_t *) CONST_STR_local,
                        gHostCtx.szHostName,
                        MAX_HOST_NAME_SIZE);

                INFO_MDNS_MESG(zeroconf_dbg_msg, "New host name : %s \r\n",
                        gHostCtx.szHostName);
                INFO_MDNS_PRINT(zeroconf_dbg_msg);
            } else {
                // Rename service instance name
                if (gSDCtx.sd_auto_rename) {
                    mDNSRename(
                            gSDCtx.srv_name,
                            ++gSDCtx.common.nInstanceId,
                            gSDCtx.srv_type,
                            gSDCtx.sd_qualified_name,
                            MAX_LABEL_SIZE);

                    /* Reset Multicast-UDP socket */

                    INFO_MDNS_PRINT("mDNSProcessInternal: MDNS_STATE_PROBE/ANNOUNCE: closing mDNS socket  \r\n");

                    UDPClose(mDNS_socket);
                    mDNS_socket = INVALID_UDP_SOCKET;
                    mDNSResponder();
                } else {
                    gSDCtx.service_registered = 0;
                    gSDCtx.used = 0;

                    INFO_MDNS_PRINT("mDNSProcessInternal: service_registered = used = 0 \r\n");

                    if (gSDCtx.sd_call_back != NULL) {
                        gSDCtx.sd_call_back((char *) gSDCtx.srv_name,
                                MDNSD_ERR_CONFLICT,
                                gSDCtx.sd_context);
                    }
                }
            }
            break;
        }

    SET_PROBE_ANNOUNCE_TIMER:

        switch (zgzc_wait_for(&(pCtx->random_delay), &(pCtx->event_time), &(pCtx->time_recorded))) {
        case ZGZC_STARTED_WAITING:

            if (pCtx->state == MDNS_STATE_PROBE) {
                if (((pCtx->nProbeCount >= MDNS_PROBE_NUM) && !pCtx->bConflictSeenInLastProbe) ||
                        (pCtx->nProbeConflictCount >= MDNS_MAX_PROBE_CONFLICT_NUM)) {
                    /* Move onto Announce Step */
                    pCtx->state = MDNS_STATE_ANNOUNCE;
                    pCtx->bConflictSeenInLastProbe = false;

                    INFO_MDNS_PRINT("MDNS_STATE_PROBE --> MDNS_STATE_ANNOUNCE \r\n");

                    //Shall we mDNSResetCounters(pCtx, true)?
                    return;
                }
            } else {
                // We are in MDNS_STATE_ANNOUNCE
                if (pCtx->nClaimCount >= MDNS_ANNOUNCE_NUM) {
                    /* Finalize mDNS Host-name, Announced */
                    pCtx->state = MDNS_STATE_DEFEND;

                    if (bIsHost) {
                        // The rest of the MCHP system knows its name through
                        // AppConfig.NetBIOSName, so update it here.

                        size_t i = 0;
                        // in the following while loop, all lower case letters have been changed to upper case ones
                        // because browser automatically translates lower case letters to upper case
                        while (i < sizeof (AppConfig.NetBIOSName)) {
                            if ((*((char *) gHostCtx.szHostName + i) >= 'a') & (*((char *) gHostCtx.szHostName + i) <= 'z'))
                                *((char *) AppConfig.NetBIOSName + i) = *((char *) gHostCtx.szHostName + i) - 32;
                            else
                                *((char *) AppConfig.NetBIOSName + i) = *((char *) gHostCtx.szHostName + i);

                            if (*((char *) AppConfig.NetBIOSName + i) == '\0')
                                break;
                            i++;
                        }
                        FormatNetBIOSName(AppConfig.NetBIOSName);

                        INFO_MDNS_MESG(zeroconf_dbg_msg, "\r\n********* Taken Host-Name: %s ********* \r\n",
                                gHostCtx.szHostName);

                        INFO_MDNS_PRINT((char *) zeroconf_dbg_msg);
                        INFO_MDNS_PRINT("MDNS_STATE_ANNOUNCE --> MDNS_STATE_DEFEND \r\n");

                        DisplayHostName(gHostCtx.szHostName);
                        //DisplayIPValue(AppConfig.MyIPAddr);

#if defined(STACK_USE_UART)
                        //putrsUART((ROM char *)"\r\n");
#endif
                    } else {
#if defined(STACK_USE_UART)
                        putrsUART((ROM char *) "\r\nZeroConf: Service = ");
                        putrsUART((ROM char *) gSDCtx.sd_qualified_name);
                        putrsUART((ROM char *) "\r\n");
#endif

                        INFO_MDNS_MESG(zeroconf_dbg_msg, "\r\n******** Taken Service-Name: %s ********\r\n",
                                gSDCtx.sd_qualified_name);

                        INFO_MDNS_PRINT((char *) zeroconf_dbg_msg);
                        INFO_MDNS_PRINT("MDNS_STATE_ANNOUNCE --> MDNS_STATE_DEFEND \r\n");

                        mDNSSendRR(&gResponderCtx.rr_list[QTYPE_PTR_INDEX],
                                0,
                                0x00,
                                1,
                                true, true);

                        gSDCtx.sd_service_advertised = 1;
                        if (gSDCtx.sd_call_back != NULL) {
                            gSDCtx.sd_call_back((char *) gSDCtx.srv_name, MDNSD_SUCCESS, gSDCtx.sd_context);
                        }
                    }

                    mDNSResetCounters(pCtx, true);

                    return;
                }
            }

            if (pCtx->state == MDNS_STATE_PROBE) {
                // Send out Probe packet
                mDNSProbe(pCtx);

                pCtx->nProbeCount++;
                pCtx->bConflictSeenInLastProbe = false;

                /* Need to set timeout for MDNS_PROBE_INTERVAL msec */
                if (pCtx->nProbeConflictCount < 9) // less-than-10 is required to pass Bonjour Conformance test.
                {
                    pCtx->random_delay = (TICK) (MDNS_PROBE_INTERVAL * (TICK_SECOND / 1000));
                } else {
                    pCtx->random_delay = (TICK) (TICKS_PER_SECOND);
                }

                DEBUG_MDNS_MESG(
                        zeroconf_dbg_msg, "MDNS_PROBE_INTERVAL Delay: %ld ticks (%d)\r\n",
                        pCtx->random_delay, pCtx->nProbeCount);
                DEBUG_MDNS_PRINT((char *) zeroconf_dbg_msg);

                return;
            }

            // We are in MDNS_STATE_ANNOUNCE

            /* Announce Name chosen on Local Network */

            mDNSAnnounce(&gResponderCtx.rr_list[(bIsHost ? QTYPE_A_INDEX : QTYPE_SRV_INDEX)]);

            pCtx->nClaimCount++;

            // Need to set timeout: ANNOUNCE_WAIT or INTERVAL

            if (pCtx->nClaimCount == 1) {
                /* Setup a delay of MDNS_ANNOUNCE_WAIT before announcing */

                /* Need to wait for time MDNS_ANNOUNCE_WAIT msec */
                pCtx->random_delay = (TICK) (MDNS_ANNOUNCE_WAIT * (TICK_SECOND / 1000));
            } else {
                pCtx->random_delay = (TICK) (MDNS_ANNOUNCE_INTERVAL * (TICK_SECOND / 1000));
            }

            // Intenional fall-through

        case ZGZC_KEEP_WAITING:

            // Not Completed the delay proposed
            return;
        }

        // Completed the delay required

        DEBUG_MDNS_MESG(zeroconf_dbg_msg, "Probe/Announce delay completed : %ld ticks\r\n",
                random_delay);
        DEBUG_MDNS_PRINT((char *) zeroconf_dbg_msg);

        /* Set the timer for next announce */
        goto SET_PROBE_ANNOUNCE_TIMER;
    }

    case MDNS_STATE_DEFEND:
    {
        //DEBUG_MDNS_PRINT("MDNS_STATE_DEFEND \n");
        /* On detection of Conflict Move back to PROBE step */

        if (pCtx->bLateConflictSeen) {
            /* Clear the Flag */
            pCtx->bLateConflictSeen = false;
            INFO_MDNS_PRINT("CONFLICT DETECTED !!! \r\n");
            INFO_MDNS_PRINT("Re-probing the Host-Name because of Conflict \r\n");
            pCtx->state = MDNS_STATE_INIT;
            pCtx->time_recorded = 0;

            INFO_MDNS_PRINT("MDNS_STATE_DEFEND --> MDNS_STATE_INIT \r\n");
        } else
            return;
    }

    default:
        break;
    }
}

void mDNSProcess(void)
{
    if (!MACIsLinked()) {
        gHostCtx.common.state = MDNS_STATE_INTF_NOT_CONNECTED;
        return;
    }

    if (AppConfig.MyIPAddr.Val == 0x00) {
        return;
    } else if (AppConfig.MyIPAddr.Val != gResponderCtx.prev_ipaddr.Val) {
        // IP address has been changed outside of Zeroconf.
        // Such change could be due to static IP assignment, or
        // a new dynamic IP lease.
        // Need to restart state-machine

        if (gResponderCtx.prev_ipaddr.v[0] != 169)
            mDNS_responder_state = MDNS_RESPONDER_INIT; // SOFTAP_ZEROCONF_SUPPORT

        INFO_MDNS_PRINT("IP-Address change is detected \r\n");
        gResponderCtx.prev_ipaddr.Val = AppConfig.MyIPAddr.Val;
        gHostCtx.common.state = MDNS_STATE_IPADDR_NOT_CONFIGURED;

        // Do not change the nInstanceId.
        // Change of IP does not imply prior name conflicts can be avoided.
        // Change of host name does.

        mDNSFillHostRecord();
    }

    /* Poll mDNSResponder to allow it to check for
     * incoming mDNS Quries/Responses */

    mDNSResponder();

    if (gSDCtx.service_registered) {
        // Application has registered some services.
        // We now need to start the service probe/announce/defend process.

        if (gHostCtx.common.state != MDNS_STATE_DEFEND) {
            gSDCtx.common.state = MDNS_STATE_NOT_READY;
        } else {
            mDNSProcessInternal((mDNSProcessCtx_common *) & gSDCtx);
        }
    }

    mDNSProcessInternal((mDNSProcessCtx_common *) & gHostCtx);
}

/**
 * Zeroconf uses 224.0.0.251
 *            => E0.00.00.FB
 *            => 1110_0000.0000_0000.0000_0000.1111_1011
 * Lower 23 bits are mapped to the multicast MAC address, prepended by 01:00:5E.
 * This becomes 0000_0001:0000_0000:0101_1110:0000_0000:0000_0000:1111_1011
 *           => 01:00:5E:00:00:FB
 */
MDNSD_ERR_CODE
mDNSMulticastFilterRegister(void)
{
    // Register an RX MAC fitler for the IP multicast group 224.0.0.251,
    // which is mapped to 01:00:5E:00:00:FB

    uint8_t mcast_addr[6] = {0x01, 0x00, 0x5E, 0x00, 0x00, 0xFB};

#if /* PIC32MX6XX/7XX Internal Ethernet controller */ (defined(__XC32) && defined(_ETH) && !defined(ENC100_INTERFACE_MODE) && !defined(ENC_CS_TRIS) && !defined(WF_CS_TRIS)) || \
    /* ENC424J600/624J600 */ defined(ENC100_INTERFACE_MODE) || \
    /* ENC28J60 */ defined(ENC_CS_TRIS) || \
    /* PIC18F97J60 family internal Ethernet controller with C18 compiler */ (defined(__18F97J60) || defined(__18F96J65) || defined(__18F96J60) || defined(__18F87J60) || defined(__18F86J65) || defined(__18F86J60) || defined(__18F67J60) || defined(__18F66J65) || defined(__18F66J60) || \
    /* PIC18F97J60 family internal Ethernet controller HI-TECH PICC-18 compiler */ defined(_18F97J60) || defined(_18F96J65) || defined(_18F96J60) || defined(_18F87J60) || defined(_18F86J65) || defined(_18F86J60) || defined(_18F67J60) || defined(_18F66J65) || defined(_18F66J60))
    SetRXHashTableEntry(*((MAC_ADDR*) mcast_addr));
#elif defined(WF_CS_TRIS)
#if defined(MRF24WG)
#if WF_SOFTWARE_MULTICAST_FILTER == WF_ENABLED
    WF_EnableSWMultiCastFilter();
    do {
        tWFMultiCastConfig p_config;

        p_config.filterId = WF_MULTICAST_FILTER_1;
        p_config.action = WF_MULTICAST_USE_FILTERS;
        memcpy((void *) p_config.macBytes, (void *) mcast_addr, WF_MAC_ADDRESS_LENGTH);
        p_config.macBitMask = 0x3F; /* each bit corresponds to 6 mac address bytes.
                                     * 1 means to force to compare the byte.
                                     * Conversely 0 means not to compare the byte, and
                                     * accept the byte unconditionally.
                                     */
        WF_MulticastSetConfig(&p_config);
    } while (0);
#else /* #if WF_SOFTWARE_MULTICAST_FILTER != WF_ENABLED */
    WF_SetMultiCastFilter(WF_MULTICAST_FILTER_1, mcast_addr);
#endif /* #if WF_SOFTWARE_MULTICAST_FILTER == WF_ENABLED */
#else /* #if !defined(MRF24WG */
    WF_SetMultiCastFilter(WF_MULTICAST_FILTER_1, mcast_addr);
#endif /* #if defined(MRF24WG */
#else
#error Must call appropraite API to enable multicast packet reception in the network controller.
#endif

    return MDNSD_SUCCESS;
}

// #if defined(DEBUG_MDNS) || defined(INFO_MDNS)

void mDNSDumpInfo(void)
{
    uint8_t tmp[8];

    putsUART("   Host registered: ");
    putsUART((char *) gHostCtx.szUserChosenHostName);
    putsUART("\r\n");
    putsUART("         qualified: ");
    putsUART((char *) gHostCtx.szHostName);
    putsUART("\r\n");
    putsUART("Service registered: ");
    putsUART((char *) gSDCtx.srv_name);
    putsUART("\r\n");
    putsUART("         qualified: ");
    putsUART((char *) gSDCtx.sd_qualified_name);
    putsUART("\r\n");
    sprintf((char *) tmp, "%d", gSDCtx.sd_port);
    putsUART("              port: ");
    putsUART((char *) tmp);
    putsUART("\r\n");
    putsUART("    TXT registered: ");
    putsUART((char *) gSDCtx.sd_txt_rec);
    putsUART("\r\n");
}

//#endif

#endif // #if defined(STACK_USE_ZEROCONF_MDNS_SD)
