/*******************************************************************************
  Company:
    Microchip Technology Inc.

  File Name:
    drv_wifi_tx_power.c

  Summary:
    Module for Microchip TCP/IP Stack
    -Provides access to MRF24W WiFi controller (MRF24WB0MA/B, MRF24WG0MA/B)
    -Reference: MRF24W Data sheet, IEEE 802.11 Standard

  Description:
    MRF24W Driver Tx Power Functions

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
*                                 INCLUDES
********************************************************************************/
#include "tcpip/tcpip.h"

#if defined(WF_CS_TRIS) && defined(WF_USE_TX_POWER_CONTROL_FUNCTIONS)

/*******************************************************************************
*                                  DEFINES
********************************************************************************/

/* used for assertions */
#if defined(WF_DEBUG)
    #define WF_MODULE_NUMBER   WF_MODULE_WF_TX_POWER
#endif

#define TX_THOTTLE_ENABLE_BIT_MASK   ((uint8_t)0x01)

#define WF_ZERO_DB_VALUE        (64)

#define DEFAULT_MIN_TX_POWER        (-10) /* dB */

/*******************************************************************************
*                     LOCAL FUNCTION PROTOTYPES
********************************************************************************/

#if defined (MRF24WG)
/*******************************************************************************
  Function:
    void WF_TxPowerSetMax(int8_t maxTxPower)

  Summary:
    Sets the Tx max power on the MRF24WG0MA/B.

  Description:
    After initialization the  MRF24WG0MA/B max Tx power is determined by a
    factory-set value.  This function can set a different maximum
    Tx power levels.  However, this function can never set a maximum Tx power
    greater than the factory-set value, which can be read via
    WF_TxPowerGetFactoryMax().

  Precondition:
    MACInit must be called first.

  Parameters:
    maxTxPower - valid range (0 to 18 dBm)

  Returns:
     None.

  Remarks:
      No conversion of units needed, input to  MRF24WG0MA/B is in dBm.
  *****************************************************************************/
void WF_TxPowerSetMax(int8_t maxTxPower)
{
    int8_t  factoryMaxPower;
    uint8_t msgData[2];
    int16_t max = (int16_t)maxTxPower;

    WF_TxPowerGetFactoryMax(&factoryMaxPower);
    WF_ASSERT(maxTxPower <= factoryMaxPower); /* cannot set max tx power greater than factor-set max tx power */

    msgData[0] = (int8_t)(max >> 8);      /* msb of max power */
    msgData[1] = (int8_t)(max & 0xff);    /* lsb of max power */

    SendSetParamMsg(PARAM_TX_POWER, msgData, sizeof(msgData));
}

/*******************************************************************************
  Function:
    void WF_TxPowerGetMax(int8_t *p_maxTxPower)

  Summary:
    Gets the Tx max power on the MRF24WG0MA/B.

  Description:

  Precondition:
    MACInit must be called first.

  Parameters:
    p_maxTxPower - Pointer to location to write the maxTxPower

  Returns:
     None.
     
  *****************************************************************************/

void WF_TxPowerGetMax(int8_t *p_maxTxPower)
{
    uint8_t msgData[6];
    int16_t tmp;

    SendGetParamMsg(PARAM_TX_POWER, msgData, sizeof(msgData));

    /* max tx power is a signed 16-bit value stored in the [1:0] msg data */
    tmp = ((int16_t)(msgData[0]) << 8);
    tmp |= (int16_t)msgData[1];
    *p_maxTxPower = (int8_t)tmp;
}

//DOM-IGNORE-BEGIN
#else /* !defined (MRF24WG) */
//DOM-IGNORE-END
/*******************************************************************************
  Function:
    void WF_TxPowerSetMinMax(int8_t minTxPower, int8_t maxTxPower)

  Summary:
    Sets the Tx min and max power on the MRF24WB0MA/B.

  Description:
    After initialization the MRF24WB0MA/B max Tx power is determined by a
    factory-set value.  This function can set a different minimum and maximum
    Tx power levels.  However, this function can never set a maximum Tx power
    greater than the factory-set value, which can be read via
    WF_TxPowerGetFactoryMax().

  Precondition:
    MACInit must be called first.

  Parameters:
    minTxPower - Desired minTxPower (-10 to 10dB)
    maxTxPower - Desired maxTxPower (-10 to 10dB)

  Returns:
     None.

  Remarks:
      No conversion of units needed, input to MRF24WB0MA/B is in dB.
  *****************************************************************************/
void WF_TxPowerSetMinMax(int8_t minTxPower, int8_t maxTxPower)
{
    int8_t  factoryMaxPower;
    uint8_t msgData[4];  /* need to input to chip two signed 16-bit values, max power followed by min power */
    int16_t max;
    int16_t min;

    max = (int16_t)maxTxPower;
    min = (int16_t)minTxPower;

    WF_ASSERT(minTxPower <= maxTxPower);

    WF_TxPowerGetFactoryMax(&factoryMaxPower);
    WF_ASSERT(maxTxPower <= factoryMaxPower); /* cannot set max tx power greater than factor-set max tx power */

    msgData[0] = (int8_t)(max >> 8);      /* msb of max power */
    msgData[1] = (int8_t)(max & 0xff);    /* lsb of max power */

    msgData[2] = (int8_t)(min >> 8);      /* msb of min power */
    msgData[3] = (int8_t)(min & 0xff);    /* lsb of min power */

    SendSetParamMsg(PARAM_TX_POWER, msgData, sizeof(msgData));
}

/*******************************************************************************
  Function:
    void WF_TxPowerGetMinMax(int8_t *p_minTxPower, int8_t *p_maxTxPower)

  Summary:
    Gets the Tx min and max power on the MRF24WB0MA/B.

  Description:

  Precondition:
    MACInit must be called first.

  Parameters:
    p_minTxPower - Pointer to location to write the minTxPower
    p_maxTxPower - Pointer to location to write the maxTxPower

  Returns:
     None.

  Remarks:
      None
  *****************************************************************************/
void WF_TxPowerGetMinMax(int8_t *p_minTxPower, int8_t *p_maxTxPower)
{
    uint8_t msgData[6];
    int16_t tmp;

    SendGetParamMsg(PARAM_TX_POWER, msgData, sizeof(msgData));

    /* max tx power is a signed 16-bit value stored in the [1:0] msg data */
    tmp = ((int16_t)(msgData[0]) << 8);
    tmp |= (int16_t)msgData[1];
    *p_maxTxPower = (int8_t)tmp;

    /* min tx power is a signed 16-bit value stored in the [3:2] msg data */
    tmp = ((int16_t)(msgData[2]) << 8);
    tmp |= (int16_t)msgData[3];
    *p_minTxPower = (int8_t)tmp;

}

/*******************************************************************************
  Function:
    void WF_FixTxRateWithMaxPower(bool oneMegaBps)

  Summary:
    Fix transmission rate with maximum power for MRF24WB0MA/B.

  Description:

  Precondition:
    MACInit must be called first.

  Parameters:
    oneMegaBps - When true, that is 1 mbps. Otherwise 2 mbps

  Returns:
     None.

  Remarks:
     None.
  *****************************************************************************/
void WF_FixTxRateWithMaxPower(bool oneMegaBps)
{
    uint8_t buf[1];

    buf[0] = oneMegaBps ? 0x20 : 0x40;    /* or 2 Mbps */
    SendSetParamMsg(PARAM_TX_THROTTLE_TABLE_ON_OFF, buf, sizeof(buf));
}
#endif /* defined (MRF24WG) */

/*******************************************************************************
  Function:
    void WF_TxPowerGetFactoryMax(int8_t *p_factoryMaxTxPower)

  Summary:
    Retrieves the factory-set max Tx power from the MRF24WB0MA/B and MRF24WG0MA/B.

  Description:

  Precondition:
    MACInit must be called first.

  Parameters:
    p_factoryMaxTxPower -
    for MRF24WB, desired maxTxPower (-10 to 10 dBm), in 1dB steps
    for MRF24WG, desired maxTxPower ( 0 to 18 dBm), in 1dB steps

  Returns:
     None.

  Remarks:
     None.
  *****************************************************************************/
void WF_TxPowerGetFactoryMax(int8_t *p_factoryMaxTxPower)
{
    uint8_t msgData[2];

    /* read max and min factory-set power levels */
    SendGetParamMsg(PARAM_FACTORY_SET_TX_MAX_POWER, msgData, sizeof(msgData));

    /* msgData[0] = max power, msgData[1] = min power */
    *p_factoryMaxTxPower = msgData[0];
}

#endif /* WF_CS_TRIS && WF_USE_TX_POWER_CONTROL_FUNCTIONS */
