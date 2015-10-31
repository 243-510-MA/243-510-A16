/**************************************************************************
  Company:
    Microchip Technology Inc.
  File Name:
    drv_uart_config_template.h
  Summary:
    UART Driver Configuration Template.
  Description:
    UART Driver Configuration Template
    
    These file provides the list of all the configurations that can be used
    with the driver. This file should not be included in the driver.       
  **************************************************************************/
//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2012 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
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

#ifndef _DRV_UART_CONFIG_TEMPLATE_H
#define _DRV_UART_CONFIG_TEMPLATE_H

//#error "This is a configuration template file.  Do not include it directly."

// *****************************************************************************
// *****************************************************************************
// Section: Core Functionality Configuration Options
// *****************************************************************************
// *****************************************************************************

/*************************************************************************
  Summary:
    Macro controls operation of the driver for defining the size of the TX
    buffer
  Description:
    UART Byte mode internal buffer TX size configuration
    
    This macro controls the operation of the driver for defining the size
    of the TX buffer                                                      
*/

#define DRV_UART1_CONFIG_TX_BYTEQ_LENGTH        4
#define DRV_UART2_CONFIG_TX_BYTEQ_LENGTH        4
#define DRV_UART3_CONFIG_TX_BYTEQ_LENGTH        4
#define DRV_UART4_CONFIG_TX_BYTEQ_LENGTH        4


/*************************************************************************
  Summary:
    Macro controls operation of the driver for defining the size of the RX
    buffer
  Description:
    UART Byte mode internal buffer RX size configuration
    
    This macro controls the operation of the driver for defining the size
    of the RX buffer                                                      
*/

#define DRV_UART1_CONFIG_RX_BYTEQ_LENGTH        4
#define DRV_UART2_CONFIG_RX_BYTEQ_LENGTH        4
#define DRV_UART3_CONFIG_RX_BYTEQ_LENGTH        4
#define DRV_UART4_CONFIG_RX_BYTEQ_LENGTH        4


// *****************************************************************************
/*  Summary:
    Macro controls operation of the driver for Baud rate configuration

  Description:
    UART Baud Rate configuration

    This macro controls the operation of the driver for Baud rate.
*/

#define DRV_UART1_CONFIG_BAUD_RATE
#define DRV_UART2_CONFIG_BAUD_RATE
#define DRV_UART3_CONFIG_BAUD_RATE
#define DRV_UART4_CONFIG_BAUD_RATE


// *****************************************************************************
/* Sets the UART for 8-N-1 configuration

  Summary:
    Macro defines the line control mode to 8-N-1 configuration

  Description:
    This macro defines the line control mode as 8 data bits, none parity and 1
    stop bit. 
*/

#define DRV_UART1_CONFIG_8N1
#define DRV_UART2_CONFIG_8N1
#define DRV_UART3_CONFIG_8N1
#define DRV_UART4_CONFIG_8N1


// *****************************************************************************
/* Sets the UART for 8-N-2 configuration

  Summary:
    Macro defines the line control mode to 8-N-2 configuration

  Description:
    This macro defines the line control mode as 8 data bits, none parity and 2
    stop bit.
*/

#define DRV_UART1_CONFIG_8N2
#define DRV_UART2_CONFIG_8N2
#define DRV_UART3_CONFIG_8N2
#define DRV_UART4_CONFIG_8N2


// *****************************************************************************
/* Sets the UART for 8-O-1 configuration

  Summary:
    Macro defines the line control mode to 8-O-1 configuration

  Description:
    This macro defines the line control mode as 8 data bits, odd parity and 1
    stop bit.
*/

#define DRV_UART1_CONFIG_8O1
#define DRV_UART2_CONFIG_8O1
#define DRV_UART3_CONFIG_8O1
#define DRV_UART4_CONFIG_8O1


// *****************************************************************************
/* Sets the UART for 8-O-2 configuration

  Summary:
    Macro defines the line control mode to 8-O-2 configuration

  Description:
    This macro defines the line control mode as 8 data bits, odd parity and 2
    stop bit.
*/

#define DRV_UART1_CONFIG_8O2
#define DRV_UART2_CONFIG_8O2
#define DRV_UART3_CONFIG_8O2
#define DRV_UART4_CONFIG_8O2


// *****************************************************************************
/* Sets the UART for 8-E-1 configuration

  Summary:
    Macro defines the line control mode to 8-E-1 configuration

  Description:
    This macro defines the line control mode as 8 data bits, even parity and 1
    stop bit.
*/

#define DRV_UART1_CONFIG_8E1
#define DRV_UART2_CONFIG_8E1
#define DRV_UART3_CONFIG_8E1
#define DRV_UART4_CONFIG_8E1


// *****************************************************************************
/* Sets the UART for 8-E-2 configuration

  Summary:
    Macro defines the line control mode to 8-E-2 configuration

  Description:
    This macro defines the line control mode as 8 data bits, even parity and 2
    stop bit.
*/

#define DRV_UART1_CONFIG_8E2
#define DRV_UART2_CONFIG_8E2
#define DRV_UART3_CONFIG_8E2
#define DRV_UART4_CONFIG_8E2


// *****************************************************************************
/* Sets the UART for 9-N-1 configuration

  Summary:
    Macro defines the line control mode to 9-N-1 configuration

  Description:
    This macro defines the line control mode as 9 data bits, none parity and 1
    stop bit.
*/

#define DRV_UART1_CONFIG_9N1
#define DRV_UART2_CONFIG_9N1
#define DRV_UART3_CONFIG_9N1
#define DRV_UART4_CONFIG_9N1


// *****************************************************************************
/* Sets the UART for 9-N-2 configuration

  Summary:
    Macro defines the line control mode to 9-N-2 configuration

  Description:
    This macro defines the line control mode as 9 data bits, none parity and 2
    stop bit.
*/

#define DRV_UART1_CONFIG_9N2
#define DRV_UART2_CONFIG_9N2
#define DRV_UART3_CONFIG_9N2
#define DRV_UART4_CONFIG_9N2


#endif // #ifndef _DRV_UART_CONFIG_TEMPLATE_H

/*******************************************************************************
 End of File
*/
