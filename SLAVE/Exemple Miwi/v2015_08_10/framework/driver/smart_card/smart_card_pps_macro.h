/**
  @Company
    Microchip Technology Inc.

  @File Name
    smart_card_pps_macro.h

  @Summary
    This is the header file for Peripheral Pin Select(PPS) configuration.

  @Description
    This file contains the declaration and Macros for Peripheral Pin Select(PPS)
    configuration.
 */

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) <year> released Microchip Technology Inc.  All rights reserved.

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


#ifndef _SMART_CARD_PPS_MACRO_H
#define	_SMART_CARD_PPS_MACRO_H

#define PPSOut(pin,func)    (_##pin##R = func)
#define PPSIn(pin,func)     (func = pin)

/*
 * PPS Pin codes Definitions
 *
 */

#define RP0                0	//RB0
#define RP1                1	//RB1
#define RP2                2	//RD8
#define RP3                3	//RD10
#define RP4                4	//RD9
#define RP5                5	//RD15
#define RP6                6	//RB6
#define RP7                7	//RB7
#define RP8                8	//RB8
#define RP9                9	//RB9
#define RP10               10	//RF4
#define RP11               11	//RD0
#define RP12               12	//RD11
#define RP13               13	//RB2
#define RP14               14	//RB14
#define RP15               15	//RF8
#define RP16               16	//RF3
#define RP17               17	//RF5
#define RP18               18	//RB5
#define RP19               19	//RG8
#define RP20               20	//RD5
#define RP21               21	//RG6
#define RP22               22	//RD3
#define RP23               23	//RD2
#define RP24               24	//RD1
#define RP25               25	//RD4
#define RP26               26	//RG7
#define RP27               27	//RG9
#define RP28               28	//RB4
#define RP29               29	//RB15
#define RP30               30	//RF2
#define RP31               31	//RF13

//The following pin codes 32..43 are for input functions only

#define RPI32              32	//RF12
#define RPI33              33	//RE8
#define RPI34              34	//RE9
#define RPI35              35	//RA15
#define RPI36              36	//RA14
#define RPI37              37
#define RPI38              38	//RC1
#define RPI39              39	//RC2
#define RPI40              40	//RC3
#define RPI41              41	//RC4
#define RPI42              42	//RD12
#define RPI43              43	//RD14


/*
 * PPS Output Function Definitions
 *
 */

#define C1OUT_FUNC          1
#define C2OUT_FUNC          2
#define U1TX_FUNC           3
#define U1RTS_FUNC          4
#define U2TX_FUNC           5
#define U2RTS_FUNC          6
#define SDO1_FUNC           7
#define SCK1OUT_FUNC        8
#define SS1OUT_FUNC         9
#define SDO2_FUNC           10
#define SCK2OUT_FUNC        11
#define SS2OUT_FUNC         12
#define OC1_FUNC            18
#define OC2_FUNC            19
#define OC3_FUNC            20
#define OC4_FUNC            21
#define OC5_FUNC            22
#define OC6_FUNC            23
#define OC7_FUNC            24
#define OC8_FUNC            25
#define OC9_FUNC            35
#define U3TX_FUNC           28
#define U3RTS_FUNC          29
#define U4TX_FUNC           30
#define U4RTS_FUNC          31
#define SDO3_FUNC           32
#define SCK3OUT_FUNC        33
#define SS3OUT_FUNC         34


/*
 * PPS Input Function Definitions
 *
 */

#define INT1_FUNC           _INT1R
#define INT2_FUNC           _INT2R
#define INT3_FUNC           _INT3R
#define INT4_FUNC           _INT4R

#define IC1_FUNC            _IC1R
#define IC2_FUNC            _IC2R
#define IC3_FUNC            _IC3R
#define IC4_FUNC            _IC4R
#define IC5_FUNC            _IC5R
#define IC6_FUNC            _IC6R
#define IC7_FUNC            _IC7R
#define IC8_FUNC            _IC8R
#define IC9_FUNC            _IC9R

#define OCFA_FUNC           _OCFAR
#define OCFB_FUNC           _OCFBR

#define SCK1IN_FUNC         _SCK1R
#define SDI1_FUNC           _SDI1R
#define SS1IN_FUNC          _SS1R
#define SCK2IN_FUNC         _SCK2R
#define SDI2_FUNC           _SDI2R
#define SS2IN_FUNC          _SS2R
#define SCK3IN_FUNC         _SCK3R
#define SDI3_FUNC           _SDI3R
#define SS3IN_FUNC          _SS3R

#define T1CK_FUNC           _T1CKR
#define T2CK_FUNC           _T2CKR
#define T3CK_FUNC           _T3CKR
#define T4CK_FUNC           _T4CKR
#define T5CK_FUNC           _T5CKR

#define T1GATE_FUNC         _T1CKR
#define T2GATE_FUNC         _T2CKR
#define T3GATE_FUNC         _T3CKR
#define T4GATE_FUNC         _T4CKR
#define T5GATE_FUNC         _T5CKR

#define U1CTS_FUNC          _U1CTSR
#define U1RX_FUNC           _U1RXR
#define U2CTS_FUNC          _U2CTSR
#define U2RX_FUNC           _U2RXR
#define U3CTS_FUNC          _U3CTSR
#define U3RX_FUNC           _U3RXR
#define U4CTS_FUNC          _U4CTSR
#define U4RX_FUNC           _U4RXR

#endif


