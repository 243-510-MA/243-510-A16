/*******************************************************************************
 Basic SPI Driver

  Company:
    Microchip Technology Inc.

  File Name:
    drv_spi_16bit_v2.c

  Summary:
    The is the SPI driver file for PIC24 and dsPIC.
    Use this implementation for SPI peripheral version with Audio Codec support.

  Description:
    The is the SPI driver file for PIC24 and dsPIC.
    Use this implementation for SPI peripheral version with Audio Codec support.

*******************************************************************************/

// DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2015 released Microchip Technology Inc.  All rights reserved.

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
// DOM-IGNORE-END

#include <xc.h>
#include "driver/spi/drv_spi.h"
#include "system_config.h"
#include "system.h"
#include <stdint.h>
#include <stddef.h>

/*
 NOTE: This driver supports 8-bit SPI mode only.
 */

// error checks
#if !defined(DRV_SPI_CONFIG_CHANNEL_1_ENABLE) && \
    !defined(DRV_SPI_CONFIG_CHANNEL_2_ENABLE) && \
    !defined(DRV_SPI_CONFIG_CHANNEL_3_ENABLE) && \
    !defined(DRV_SPI_CONFIG_CHANNEL_4_ENABLE)
    #warning "No SPI Channel defined! Please define in system_config.h or system.h"
#endif

/**
  DRV_SPIx_CONFIG_DUMMY_DATA

  @Summary
    Dummy data to be sent.

  @Description
    Dummy data to be sent, when no input buffer is specified in the buffer APIs.
 */
#ifndef DRV_SPI1_CONFIG_DUMMY_DATA
    #define DRV_SPI1_CONFIG_DUMMY_DATA 0xFF
#endif
#ifndef DRV_SPI2_CONFIG_DUMMY_DATA
    #define DRV_SPI2_CONFIG_DUMMY_DATA 0xFF
#endif
#ifndef DRV_SPI3_CONFIG_DUMMY_DATA
    #define DRV_SPI3_CONFIG_DUMMY_DATA 0xFF
#endif
#ifndef DRV_SPI4_CONFIG_DUMMY_DATA
    #define DRV_SPI4_CONFIG_DUMMY_DATA 0xFF
#endif

/**
  SPI_FIFO_FILL_LIMIT

  @Summary
    FIFO fill limit for data transmission.

  @Description
    The amount of data to be filled in the FIFO during transmission. The maximum limit allowed is 8.
 */
#define SPI_FIFO_FILL_LIMIT 0x8

//Check to make sure that the FIFO limit does not exceed the maximum allowed limit of 8
#if (SPI_FIFO_FILL_LIMIT > 8)

    #define SPI_FIFO_FILL_LIMIT 8

#endif


/* SPI SFR definitions. i represents the SPI
   channel number.
   valid i values are: 1, 2, 3, 4
*/
#define DRV_SPI_STATL(i)      SPI##i##STATL
#define DRV_SPI_STATLbits(i)  SPI##i##STATLbits
#define DRV_SPI_STATH(i)      SPI##i##STATH
#define DRV_SPI_STATHbits(i)  SPI##i##STATHbits
#define DRV_SPI_CON1L(i)      SPI##i##CON1L
#define DRV_SPI_CON1Lbits(i)  SPI##i##CON1Lbits
#define DRV_SPI_CON2L(i)      SPI##i##CON2L
#define DRV_SPI_CON2Lbits(i)  SPI##i##CON2Lbits
#define DRV_SPI_CON1H(i)      SPI##i##CON1H
#define DRV_SPI_CON1Hbits(i)  SPI##i##CON1Hbits
#define DRV_SPI_CON2H(i)      SPI##i##CON2H
#define DRV_SPI_CON2Hbits(i)  SPI##i##CON2Hbits
#define DRV_SPI_BRGL(i)       SPI##i##BRGL
#define DRV_SPI_BRGLbits(i)   SPI##i##BRGLbits

static int spiMutex[4] = { 0, 0, 0, 0 };

#ifdef DRV_SPI_CONFIG_CHANNEL_1_ENABLE
static uint8_t spi1DummyData = DRV_SPI1_CONFIG_DUMMY_DATA;
#endif

#ifdef DRV_SPI_CONFIG_CHANNEL_2_ENABLE
static uint8_t spi2DummyData = DRV_SPI2_CONFIG_DUMMY_DATA;
#endif

#ifdef DRV_SPI_CONFIG_CHANNEL_3_ENABLE
static uint8_t spi3DummyData = DRV_SPI3_CONFIG_DUMMY_DATA;
#endif

#ifdef DRV_SPI_CONFIG_CHANNEL_4_ENABLE
static uint8_t spi4DummyData = DRV_SPI4_CONFIG_DUMMY_DATA;
#endif

/**
  SPIx Exchange Function

  @Summary
    Exchanges one byte of data from SPIx

  @Description
    This routine exchanges one byte of data from the SPIx.
    This is a blocking routine.

  @Preconditions
    The initialize routine must have been called for the specified
    SPI1 driver instance.

  @Returns
    Data read from SPIx

  @Param
    data         - Data to be written onto SPIx.

  @Example
    <code>
    uint16_t   myWriteBuffer[MY_BUFFER_SIZE];
    uint16_t   myReadBuffer[MY_BUFFER_SIZE];
    uint16_t writeData;
    uint16_t readData;
    SPI1_STATUS status;
    unsigned int    total;
    SPI1_Initialize;

    total = 0;
    do
    {
        total  = SPI1_ExchangeBuffer( &myWriteBuffer[total], MY_BUFFER_SIZE - total, &myWriteBuffer[total]);

        // Do something else...

    } while( total < MY_BUFFER_SIZE );

    readData = SPI1_Exchange( writeData);

    status = SPI1_StatusGet();

    </code>
*/
void SPI1_Exchange(void *pTransmitData, void *pReceiveData);
void SPI2_Exchange(void *pTransmitData, void *pReceiveData);
void SPI3_Exchange(void *pTransmitData, void *pReceiveData);
void SPI4_Exchange(void *pTransmitData, void *pReceiveData);

/**
  SPIx Exchange Buffer Function

  @Summary
    Exchanges data from a buffer of size specified by len from SPIx

  @Description
    This routine exchanges data from a buffer of size len from the SPIx.
    This is a blocking routine.

  @Preconditions
    The initialize routine must have been called for the specified
    SPIx driver instance.

  @Returns
    Number of bytes written/read.

  @Param
    pTransmitData - Location of buffer of data to be written onto SPIx.

  @Param
    len - Number of data to be read. 

  @Param
    pReceiveData - Location of buffer of data to be read from SPI1.

  @Example
    Refer to SPIx_Exchange() for an example

*/
uint16_t SPI1_ExchangeBuffer(void *pTransmitData, uint16_t len, void* pReceiveData);
uint16_t SPI2_ExchangeBuffer(void *pTransmitData, uint16_t len, void* pReceiveData);
uint16_t SPI3_ExchangeBuffer(void *pTransmitData, uint16_t len, void* pReceiveData);
uint16_t SPI4_ExchangeBuffer(void *pTransmitData, uint16_t len, void* pReceiveData);

/**
  SPIx Dummy Data Set Function

  @Summary:
    Sets the dummy data when calling exchange functions

  @Description:
    This function sets the dummy data used when performing a an SPI
    get call. When get is used, the exchange functions will still need
    to send data for proper SPI operation.

  @Precondition:
    The DRV_SPI_Initialize routine must have been called.

  @Returns:
    None.

  @Parameters:
    channel      - SPI instance through which the communication needs to happen

    dummyData    - Dummy data to be used.

  @Remarks:
    This is blocking routine.
*/
void SPI1_DummyDataSet(uint8_t dummyData);
void SPI2_DummyDataSet(uint8_t dummyData);
void SPI3_DummyDataSet(uint8_t dummyData);
void SPI4_DummyDataSet(uint8_t dummyData);

/*****************************************************************************
 * void DRV_SPI_Put(uint8_t channel, uint8_t data)
 *****************************************************************************/
void DRV_SPI_Put(uint8_t channel, uint8_t data)
{
#ifdef DRV_SPI_CONFIG_CHANNEL_1_ENABLE
    if (channel == 1)      
        SPI1_Exchange(&data, NULL);
#endif //#ifdef DRV_SPI_CONFIG_CHANNEL_1_ENABLE
#ifdef DRV_SPI_CONFIG_CHANNEL_2_ENABLE
    if (channel == 2)
        SPI2_Exchange(&data, NULL);
#endif //#ifdef DRV_SPI_CONFIG_CHANNEL_2_ENABLE
#ifdef DRV_SPI_CONFIG_CHANNEL_3_ENABLE
    if (channel == 3)
        SPI3_Exchange(&data, NULL);
#endif //#ifdef DRV_SPI_CONFIG_CHANNEL_3_ENABLE
#ifdef DRV_SPI_CONFIG_CHANNEL_4_ENABLE
    if (channel == 4)
        SPI4_Exchange(&data, NULL);
#endif //#ifdef DRV_SPI_CONFIG_CHANNEL_4_ENABLE
}

/*****************************************************************************
 * uint8_t DRV_SPI_Get (uint8_t channel)
 *****************************************************************************/
uint8_t DRV_SPI_Get (uint8_t channel)
{
    uint8_t ReceiveData;

#ifdef DRV_SPI_CONFIG_CHANNEL_1_ENABLE
    if (channel == 1)
        SPI1_Exchange(NULL, &ReceiveData);
#endif //#ifdef DRV_SPI_CONFIG_CHANNEL_1_ENABLE
#ifdef DRV_SPI_CONFIG_CHANNEL_2_ENABLE
    if (channel == 2)
        SPI2_Exchange(NULL, &ReceiveData);
#endif //#ifdef DRV_SPI_CONFIG_CHANNEL_2_ENABLE
#ifdef DRV_SPI_CONFIG_CHANNEL_3_ENABLE
    if (channel == 3)
        SPI3_Exchange(NULL, &ReceiveData);
#endif //#ifdef DRV_SPI_CONFIG_CHANNEL_3_ENABLE
#ifdef DRV_SPI_CONFIG_CHANNEL_4_ENABLE
    if (channel == 4)
        SPI4_Exchange(NULL, &ReceiveData);
#endif //#ifdef DRV_SPI_CONFIG_CHANNEL_4_ENABLE

    return ReceiveData;
}

/*****************************************************************************
 void DRV_SPI_PutBuffer( uint8_t channel,
                         uint8_t *pData,
                         uint16_t count)
 *****************************************************************************/
void DRV_SPI_PutBuffer( uint8_t channel,
                        uint8_t *pData,
                        uint16_t count)
{

#ifdef DRV_SPI_CONFIG_CHANNEL_1_ENABLE
    if (channel == 1)
        SPI1_ExchangeBuffer(pData, count, NULL);
#endif //#ifdef DRV_SPI_CONFIG_CHANNEL_1_ENABLE
#ifdef DRV_SPI_CONFIG_CHANNEL_2_ENABLE
    if (channel == 2)
        SPI2_ExchangeBuffer(pData, count, NULL);
#endif //#ifdef DRV_SPI_CONFIG_CHANNEL_2_ENABLE
#ifdef DRV_SPI_CONFIG_CHANNEL_3_ENABLE
    if (channel == 3)       
        SPI3_ExchangeBuffer(pData, count, NULL);
#endif //#ifdef DRV_SPI_CONFIG_CHANNEL_3_ENABLE
#ifdef DRV_SPI_CONFIG_CHANNEL_4_ENABLE
    if (channel == 4)
        SPI4_ExchangeBuffer(pData, count, NULL);
#endif //#ifdef DRV_SPI_CONFIG_CHANNEL_4_ENABLE

}

/*****************************************************************************
 void DRV_SPI_GetBuffer( uint8_t channel,
                         uint8_t *pData,
                         uint16_t count)
 *****************************************************************************/
void DRV_SPI_GetBuffer( uint8_t channel,
                        uint8_t *pData,
                        uint16_t count)
{
#ifdef DRV_SPI_CONFIG_CHANNEL_1_ENABLE
    if (channel == 1)
        SPI1_ExchangeBuffer(NULL, count, pData);
#endif //#ifdef DRV_SPI_CONFIG_CHANNEL_1_ENABLE
#ifdef DRV_SPI_CONFIG_CHANNEL_2_ENABLE
    if (channel == 2)
        SPI2_ExchangeBuffer(NULL, count, pData);
#endif //#ifdef DRV_SPI_CONFIG_CHANNEL_2_ENABLE
#ifdef DRV_SPI_CONFIG_CHANNEL_3_ENABLE
    if (channel == 3)
        SPI3_ExchangeBuffer(NULL, count, pData);
#endif //#ifdef DRV_SPI_CONFIG_CHANNEL_3_ENABLE
#ifdef DRV_SPI_CONFIG_CHANNEL_4_ENABLE
    if (channel == 4)
        SPI4_ExchangeBuffer(NULL, count, pData);
#endif //#ifdef DRV_SPI_CONFIG_CHANNEL_4_ENABLE

}

/*****************************************************************************
void SPI_DummyDataSet(
                        uint8_t channel,
                        uint8_t dummyData)
 *****************************************************************************/
void SPI_DummyDataSet(
                        uint8_t channel,
                        uint8_t dummyData)
{
#ifdef DRV_SPI_CONFIG_CHANNEL_1_ENABLE
    if (channel == 1)
        SPI1_DummyDataSet(dummyData);
#endif //#ifdef DRV_SPI_CONFIG_CHANNEL_1_ENABLE
#ifdef DRV_SPI_CONFIG_CHANNEL_2_ENABLE
    if (channel == 2)
        SPI2_DummyDataSet(dummyData);
#endif //#ifdef DRV_SPI_CONFIG_CHANNEL_2_ENABLE
#ifdef DRV_SPI_CONFIG_CHANNEL_3_ENABLE
    if (channel == 3)
        SPI3_DummyDataSet(dummyData);
#endif //#ifdef DRV_SPI_CONFIG_CHANNEL_3_ENABLE
#ifdef DRV_SPI_CONFIG_CHANNEL_4_ENABLE
    if (channel == 4)
        SPI4_DummyDataSet(dummyData);
#endif //#ifdef DRV_SPI_CONFIG_CHANNEL_4_ENABLE

}

/*****************************************************************************
 * void DRV_SPI_Initialize(const unsigned int channel, DRV_SPI_INIT_DATA *pData)
 *****************************************************************************/
void DRV_SPI_Initialize(DRV_SPI_INIT_DATA *pData)
{
#ifdef DRV_SPI_CONFIG_CHANNEL_1_ENABLE        
    if (pData->channel == 1)
    {

        DRV_SPI_CON1L(1) = 0;
        DRV_SPI_CON1H(1) = 0;
        DRV_SPI_CON1H(1) = 0;
        DRV_SPI_CON2L(1) = 0;

        DRV_SPI_CON1Lbits(1).MSTEN = SPI_MST_MODE_ENABLE;

        // set to 8-bit mode
        DRV_SPI_CON1Lbits(1).MODE16 = 0;
        DRV_SPI_CON1Lbits(1).MODE32 = 0;

        DRV_SPI_CON1Lbits(1).CKE = pData->cke;
        switch (pData->spibus_mode)
        {
            case SPI_BUS_MODE_0:
                //smp = 0, ckp = 0
                DRV_SPI_CON1Lbits(1).CKP = 0;
                DRV_SPI_CON1Lbits(1).SMP = 0;
                break;
            case SPI_BUS_MODE_1:
                //smp = 1, ckp = 0
                DRV_SPI_CON1Lbits(1).CKP = 0;
                DRV_SPI_CON1Lbits(1).SMP = 1;
                break;
            case SPI_BUS_MODE_2:
                //smp = 0, ckp = 1
                DRV_SPI_CON1Lbits(1).CKP = 1;
                DRV_SPI_CON1Lbits(1).SMP = 0;
                break;
            case SPI_BUS_MODE_3:
                //smp = 1, ckp = 1
                DRV_SPI_CON1Lbits(1).CKP = 1;
                DRV_SPI_CON1Lbits(1).SMP = 1;
                break;
            default:
                // should not happen
                break;
        }

        DRV_SPI_CON2L(1) = 0;
#ifndef DRV_SPI_CONFIG_ENHANCED_BUFFER_DISABLE
        DRV_SPI_CON1Lbits(1).ENHBUF = 1;
#else
        DRV_SPI_CON1Lbits(1).ENHBUF = 0;
#endif
        DRV_SPI_BRGLbits(1).BRG = pData->primaryPrescale;
        DRV_SPI_CON1Lbits(1).SPIEN = SPI_MODULE_ENABLE;
        return;
    }
#endif // #ifdef DRV_SPI_CONFIG_CHANNEL_1_ENABLE
#ifdef DRV_SPI_CONFIG_CHANNEL_2_ENABLE
    if (pData->channel == 2)
    {

        DRV_SPI_CON1L(2) = 0;
        DRV_SPI_CON1H(2) = 0;
        DRV_SPI_CON1H(2) = 0;
        DRV_SPI_CON2L(2) = 0;

        DRV_SPI_CON1Lbits(2).MSTEN = SPI_MST_MODE_ENABLE;

        // set to 8-bit mode
        DRV_SPI_CON1Lbits(2).MODE16 = 0;
        DRV_SPI_CON1Lbits(2).MODE32 = 0;

        DRV_SPI_CON1Lbits(2).CKE = pData->cke;
        switch (pData->spibus_mode)
        {
            case SPI_BUS_MODE_0:
                //smp = 0, ckp = 0
                DRV_SPI_CON1Lbits(2).CKP = 0;
                DRV_SPI_CON1Lbits(2).SMP = 0;
                break;
            case SPI_BUS_MODE_1:
                //smp = 1, ckp = 0
                DRV_SPI_CON1Lbits(2).CKP = 0;
                DRV_SPI_CON1Lbits(2).SMP = 1;
                break;
            case SPI_BUS_MODE_2:
                //smp = 0, ckp = 1
                DRV_SPI_CON1Lbits(2).CKP = 1;
                DRV_SPI_CON1Lbits(2).SMP = 0;
                break;
            case SPI_BUS_MODE_3:
                //smp = 1, ckp = 1
                DRV_SPI_CON1Lbits(2).CKP = 1;
                DRV_SPI_CON1Lbits(2).SMP = 1;
                break;
            default:
                // should not happen
                break;
        }           

        DRV_SPI_CON2L(2) = 0;
#ifndef DRV_SPI_CONFIG_ENHANCED_BUFFER_DISABLE
        DRV_SPI_CON1Lbits(2).ENHBUF = 1;
#else
        DRV_SPI_CON1Lbits(2).ENHBUF = 0;
#endif
        DRV_SPI_BRGLbits(2).BRG = pData->primaryPrescale;
        DRV_SPI_CON1Lbits(2).SPIEN = SPI_MODULE_ENABLE;
        return;
    }
#endif // #ifdef DRV_SPI_CONFIG_CHANNEL_2_ENABLE
#ifdef DRV_SPI_CONFIG_CHANNEL_3_ENABLE
    if (pData->channel == 3)
    {

        DRV_SPI_CON1L(3) = 0;
        DRV_SPI_CON1H(3) = 0;
        DRV_SPI_CON1H(3) = 0;
        DRV_SPI_CON2L(3) = 0;

        DRV_SPI_CON1Lbits(3).MSTEN = SPI_MST_MODE_ENABLE;

        DRV_SPI_CON1Lbits(3).MODE16 = 0;
        DRV_SPI_CON1Lbits(3).MODE32 = 0;

        DRV_SPI_CON1Lbits(3).CKE = pData->cke;
        switch (pData->spibus_mode)
        {
            case SPI_BUS_MODE_0:
                //smp = 0, ckp = 0
                DRV_SPI_CON1Lbits(3).CKP = 0;
                DRV_SPI_CON1Lbits(3).SMP = 0;
                break;
            case SPI_BUS_MODE_1:
                //smp = 1, ckp = 0
                DRV_SPI_CON1Lbits(3).CKP = 0;
                DRV_SPI_CON1Lbits(3).SMP = 1;
                break;
            case SPI_BUS_MODE_2:
                //smp = 0, ckp = 1
                DRV_SPI_CON1Lbits(3).CKP = 1;
                DRV_SPI_CON1Lbits(3).SMP = 0;
                break;
            case SPI_BUS_MODE_3:
                //smp = 1, ckp = 1
                DRV_SPI_CON1Lbits(3).CKP = 1;
                DRV_SPI_CON1Lbits(3).SMP = 1;
                break;
            default:
                // should not happen
                break;
        }

        DRV_SPI_CON2L(3) = 0;
#ifndef DRV_SPI_CONFIG_ENHANCED_BUFFER_DISABLE
        DRV_SPI_CON1Lbits(3).ENHBUF = 1;
#else
        DRV_SPI_CON1Lbits(3).ENHBUF = 0;
#endif
        DRV_SPI_BRGLbits(3).BRG = pData->primaryPrescale;
        DRV_SPI_CON1Lbits(3).SPIEN = SPI_MODULE_ENABLE;
        return;
    }
#endif // #ifdef DRV_SPI_CONFIG_CHANNEL_3_ENABLE
#ifdef DRV_SPI_CONFIG_CHANNEL_4_ENABLE
    if (pData->channel == 4)
    {

        DRV_SPI_CON1L(4) = 0;
        DRV_SPI_CON1H(4) = 0;
        DRV_SPI_CON1H(4) = 0;
        DRV_SPI_CON2L(4) = 0;

        DRV_SPI_CON1Lbits(4).MSTEN = SPI_MST_MODE_ENABLE;

        DRV_SPI_CON1Lbits(4).MODE16 = 0;
        DRV_SPI_CON1Lbits(4).MODE32 = 0;

        DRV_SPI_CON1Lbits(4).CKE = pData->cke;
        switch (pData->spibus_mode)
        {
            case SPI_BUS_MODE_0:
                //smp = 0, ckp = 0
                DRV_SPI_CON1Lbits(4).CKP = 0;
                DRV_SPI_CON1Lbits(4).SMP = 0;
                break;
            case SPI_BUS_MODE_1:
                //smp = 1, ckp = 0
                DRV_SPI_CON1Lbits(4).CKP = 0;
                DRV_SPI_CON1Lbits(4).SMP = 1;
                break;
            case SPI_BUS_MODE_2:
                //smp = 0, ckp = 1
                DRV_SPI_CON1Lbits(4).CKP = 1;
                DRV_SPI_CON1Lbits(4).SMP = 0;
                break;
            case SPI_BUS_MODE_3:
                //smp = 1, ckp = 1
                DRV_SPI_CON1Lbits(4).CKP = 1;
                DRV_SPI_CON1Lbits(4).SMP = 1;
                break;
            default:
                // should not happen
                break;
        }

        DRV_SPI_CON2L(4) = 0;
#ifndef DRV_SPI_CONFIG_ENHANCED_BUFFER_DISABLE
        DRV_SPI_CON1Lbits(4).ENHBUF = 1;
#else
        DRV_SPI_CON1Lbits(4).ENHBUF = 0;
#endif
        DRV_SPI_BRGLbits(4).BRG = pData->primaryPrescale;
        DRV_SPI_CON1Lbits(4).SPIEN = SPI_MODULE_ENABLE;
        return;
    }
#endif // #ifdef DRV_SPI_CONFIG_CHANNEL_4_ENABLE
}

/*****************************************************************************
 * void DRV_SPI_Deinitialize(uint8_t channel)
 *****************************************************************************/
void DRV_SPI_Deinitialize (uint8_t channel)
{
#ifdef DRV_SPI_CONFIG_CHANNEL_1_ENABLE
    if (channel == 1)
    {
        DRV_SPI_CON1Lbits(1).SPIEN = SPI_MODULE_DISABLE;
    }
#endif // #ifdef DRV_SPI_CONFIG_CHANNEL_1_ENABLE
#ifdef DRV_SPI_CONFIG_CHANNEL_2_ENABLE
    if (channel == 2)
    {
        DRV_SPI_CON1Lbits(2).SPIEN = SPI_MODULE_DISABLE;
    }
#endif // #ifdef DRV_SPI_CONFIG_CHANNEL_2_ENABLE
#ifdef DRV_SPI_CONFIG_CHANNEL_3_ENABLE
    if (channel == 3)
    {
        DRV_SPI_CON1Lbits(3).SPIEN = SPI_MODULE_DISABLE;
    }
#endif // #ifdef DRV_SPI_CONFIG_CHANNEL_3_ENABLE
#ifdef DRV_SPI_CONFIG_CHANNEL_4_ENABLE
    if (channel == 4)
    {
        DRV_SPI_CON1Lbits(4).SPIEN = SPI_MODULE_DISABLE;
    }
#endif // #ifdef DRV_SPI_CONFIG_CHANNEL_4_ENABLE
}

/* ********************************************************** */
/* SPI Channel 1 Functions                                    */
/* ********************************************************** */

#ifdef DRV_SPI_CONFIG_CHANNEL_1_ENABLE
/*****************************************************************************
 void SPI1_Exchange(
                        void *pTransmitData,
                        void *pReceiveData )
 *****************************************************************************/
void SPI1_Exchange( void *pTransmitData, void *pReceiveData )
{
    uint8_t dummyRead = 0;
    uint8_t dummyWrite = spi1DummyData;

    if(pTransmitData == NULL)
    {
        pTransmitData = &dummyWrite;
    }

    if(pReceiveData == NULL)
    {
        pReceiveData = &dummyRead;
    }

    while( SPI1STATLbits.SPITBF == true )
    { }

    // ----------------
    // sending data
    // ----------------
    SPI1BUFL = *((uint8_t*)pTransmitData);

    while ( SPI1STATLbits.SPIRBE == true)
    { }

    // ----------------
    // extracting data
    // ----------------
    *((uint8_t*)pReceiveData) = (uint8_t)(SPI1BUFL);

}

/*****************************************************************************
 uint16_t SPI1_ExchangeBuffer(
                        void *pTransmitData,
                        uint16_t len,
                        void *pReceiveData)
 *****************************************************************************/
uint16_t SPI1_ExchangeBuffer(
                        void *pTransmitData,
                        uint16_t len,
                        void *pReceiveData)
{

    uint16_t dataSentCount = 0;
    uint16_t count = 0;
    uint8_t dummyDataReceived = 0;
    uint8_t dummyDataTransmit = (uint8_t)spi1DummyData;

    uint8_t fifoMultiplier = 1;

    uint8_t  *pSend, *pReceived;
    uint16_t addressIncrement;
    uint16_t receiveAddressIncrement, sendAddressIncrement;

    addressIncrement = 1;
    fifoMultiplier = 4;

    // set the pointers and increment delta
    // for transmit and receive operations
    if (pTransmitData == NULL)
    {
        sendAddressIncrement = 0;
        pSend = (uint8_t*)&dummyDataTransmit;
    }
    else
    {
        sendAddressIncrement = addressIncrement;
        pSend = (uint8_t*)pTransmitData;
    }

    if (pReceiveData == NULL)
    {
       receiveAddressIncrement = 0;
       pReceived = (uint8_t*)&dummyDataReceived;
    }
    else
    {
       receiveAddressIncrement = addressIncrement;
       pReceived = (uint8_t*)pReceiveData;
    }


    while( SPI1STATLbits.SPITBF == true )
    { }

    while (dataSentCount < len)
    {
        if ((count < ((SPI_FIFO_FILL_LIMIT)*fifoMultiplier)))
        {
            SPI1BUFL = *pSend;
            pSend += sendAddressIncrement;
            dataSentCount++;
            count++;
        }

        if (SPI1STATLbits.SPIRBE == false)
        {
            *pReceived = SPI1BUFL;
            pReceived += receiveAddressIncrement;
            count--;
        }

    }
    while (count)
    {
        if (SPI1STATLbits.SPIRBE == false)
        {
            *pReceived = SPI1BUFL;
            pReceived += receiveAddressIncrement;
            count--;
        }
    }

    return dataSentCount;
}

/*****************************************************************************
 void SPI1_DummyDataSet(uint8_t dummyData)
 *****************************************************************************/
void SPI1_DummyDataSet(uint8_t dummyData)
{
    spi1DummyData = dummyData;
}

#endif //#ifdef DRV_SPI_CONFIG_CHANNEL_1_ENABLE

/* ********************************************************** */
/* SPI Channel 2 Functions                                    */
/* ********************************************************** */
#ifdef DRV_SPI_CONFIG_CHANNEL_2_ENABLE

/*****************************************************************************
 void SPI2_Exchange(
                        void *pTransmitData,
                        void *pReceiveData )
 *****************************************************************************/
void SPI2_Exchange( void *pTransmitData, void *pReceiveData )
{

    uint8_t dummyRead = 0;
    uint8_t dummyWrite = spi2DummyData;

    if(pTransmitData == NULL)
    {
        pTransmitData = &dummyWrite;
    }

    if(pReceiveData == NULL)
    {
        pReceiveData = &dummyRead;
    }

    while( SPI2STATLbits.SPITBF == true )
    {

    }

    // ----------------
    // sending data
    // ----------------
    SPI2BUFL = *((uint8_t*)pTransmitData);

    while ( SPI2STATLbits.SPIRBE == true)
    { }

    // ----------------
    // extracting data
    // ----------------
    *((uint8_t*)pReceiveData) = SPI2BUFL;

}

/*****************************************************************************
 uint16_t SPI2_ExchangeBuffer(
                        void *pTransmitData,
                        uint16_t len,
                        void *pReceiveData)
 *****************************************************************************/
uint16_t SPI2_ExchangeBuffer(
                        void *pTransmitData,
                        uint16_t len,
                        void *pReceiveData)
{

    uint16_t dataSentCount = 0;
    uint16_t count = 0;
    uint8_t dummyDataReceived = 0;
    uint8_t dummyDataTransmit = (uint8_t)spi2DummyData;

    uint8_t fifoMultiplier = 1;

    uint8_t  *pSend, *pReceived;
    uint16_t addressIncrement;
    uint16_t receiveAddressIncrement, sendAddressIncrement;

    addressIncrement = 1;
    fifoMultiplier = 4;

    // set the pointers and increment delta
    // for transmit and receive operations
    if (pTransmitData == NULL)
    {
        sendAddressIncrement = 0;
        pSend = (uint8_t*)&dummyDataTransmit;
    }
    else
    {
        sendAddressIncrement = addressIncrement;
        pSend = (uint8_t*)pTransmitData;
    }

    if (pReceiveData == NULL)
    {
       receiveAddressIncrement = 0;
       pReceived = (uint8_t*)&dummyDataReceived;
    }
    else
    {
       receiveAddressIncrement = addressIncrement;
       pReceived = (uint8_t*)pReceiveData;
    }


    while( SPI2STATLbits.SPITBF == true )
    { }

    while (dataSentCount < len)
    {
        if ((count < ((SPI_FIFO_FILL_LIMIT)*fifoMultiplier)))
        {
            SPI2BUFL = *pSend;
            pSend += sendAddressIncrement;
            dataSentCount++;
            count++;
        }

        if (SPI2STATLbits.SPIRBE == false)
        {
            *pReceived = SPI2BUFL;
            pReceived += receiveAddressIncrement;
            count--;
        }

    }
    while (count)
    {
        if (SPI2STATLbits.SPIRBE == false)
        {
            *pReceived = SPI2BUFL;
            pReceived += receiveAddressIncrement;
            count--;
        }
    }

    return dataSentCount;
}

/*****************************************************************************
 void SPI2_DummyDataSet(uint8_t dummyData)
 *****************************************************************************/
void SPI2_DummyDataSet(uint8_t dummyData)
{
    spi2DummyData = dummyData;
}

#endif //#ifdef DRV_SPI_CONFIG_CHANNEL_2_ENABLE

/* ********************************************************** */
/* SPI Channel 3 Functions                                    */
/* ********************************************************** */

#ifdef DRV_SPI_CONFIG_CHANNEL_3_ENABLE

/*****************************************************************************
 void SPI3_Exchange(
                        void *pTransmitData,
                        void *pReceiveData )
 *****************************************************************************/
void SPI3_Exchange( void *pTransmitData, void *pReceiveData )
{
    uint8_t dummyRead = 0;
    uint8_t dummyWrite = spi3DummyData;

    if(pTransmitData == NULL)
    {
        pTransmitData = &dummyWrite;
    }

    if(pReceiveData == NULL)
    {
        pReceiveData = &dummyRead;
    }

    while( SPI3STATLbits.SPITBF == true )
    {

    }

    // ----------------
    // sending data
    // ----------------
    SPI3BUFL = *((uint8_t*)pTransmitData);

    while ( SPI3STATLbits.SPIRBE == true)
    { }

    // ----------------
    // extracting data
    // ----------------
    *((uint8_t*)pReceiveData) = SPI3BUFL;
    
}

/*****************************************************************************
 uint16_t SPI3_ExchangeBuffer(
                        void *pTransmitData,
                        uint16_t len,
                        void *pReceiveData)
 *****************************************************************************/
uint16_t SPI3_ExchangeBuffer(
                        void *pTransmitData,
                        uint16_t len,
                        void *pReceiveData)
{

    uint16_t dataSentCount = 0;
    uint16_t count = 0;
    uint8_t dummyDataReceived = 0;
    uint8_t dummyDataTransmit = (uint8_t)spi3DummyData;

    uint8_t fifoMultiplier = 1;

    uint8_t  *pSend, *pReceived;
    uint16_t addressIncrement;
    uint16_t receiveAddressIncrement, sendAddressIncrement;

    addressIncrement = 1;
    fifoMultiplier = 4;
    
    // set the pointers and increment delta
    // for transmit and receive operations
    if (pTransmitData == NULL)
    {
        sendAddressIncrement = 0;
        pSend = (uint8_t*)&dummyDataTransmit;
    }
    else
    {
        sendAddressIncrement = addressIncrement;
        pSend = (uint8_t*)pTransmitData;
    }

    if (pReceiveData == NULL)
    {
       receiveAddressIncrement = 0;
       pReceived = (uint8_t*)&dummyDataReceived;
    }
    else
    {
       receiveAddressIncrement = addressIncrement;
       pReceived = (uint8_t*)pReceiveData;
    }


    while( SPI3STATLbits.SPITBF == true )
    { }

    while (dataSentCount < len)
    {
        if ((count < ((SPI_FIFO_FILL_LIMIT)*fifoMultiplier)))
        {
            SPI3BUFL = *pSend;
            pSend += sendAddressIncrement;
            dataSentCount++;
            count++;
        }

        if (SPI3STATLbits.SPIRBE == false)
        {
            *pReceived = SPI3BUFL;
            pReceived += receiveAddressIncrement;
            count--;
        }

    }
    while (count)
    {
        if (SPI3STATLbits.SPIRBE == false)
        {
            *pReceived = SPI3BUFL;
            pReceived += receiveAddressIncrement;
            count--;
        }
    }

    return dataSentCount;
}

/*****************************************************************************
 void SPI3_DummyDataSet(uint8_t dummyData)
 *****************************************************************************/
void SPI3_DummyDataSet(uint8_t dummyData)
{
    spi3DummyData = dummyData;
}

#endif //#ifdef DRV_SPI_CONFIG_CHANNEL_3_ENABLE

/* ********************************************************** */
/* SPI Channel 4 Functions                                    */
/* ********************************************************** */

#ifdef DRV_SPI_CONFIG_CHANNEL_4_ENABLE

/*****************************************************************************
 void SPI4_Exchange(
                        void *pTransmitData,
                        void *pReceiveData )
 *****************************************************************************/
void SPI4_Exchange( void *pTransmitData, void *pReceiveData )
{

    uint8_t dummyRead = 0;
    uint8_t dummyWrite = spi4DummyData;

    if(pTransmitData == NULL)
    {
        pTransmitData = &dummyWrite;
    }

    if(pReceiveData == NULL)
    {
        pReceiveData = &dummyRead;
    }

    while( SPI4STATLbits.SPITBF == true )
    {

    }

    // ----------------
    // sending data
    // ----------------
    SPI4BUFL = *((uint8_t*)pTransmitData);

    while ( SPI4STATLbits.SPIRBE == true)
    { }

    // ----------------
    // extracting data
    // ----------------
    *((uint8_t*)pReceiveData) = SPI4BUFL;
    
}

/*****************************************************************************
 uint16_t SPI4_ExchangeBuffer(
                        void *pTransmitData,
                        uint16_t len,
                        void *pReceiveData)
 *****************************************************************************/
uint16_t SPI4_ExchangeBuffer(
                        void *pTransmitData,
                        uint16_t len,
                        void *pReceiveData)
{

    uint16_t dataSentCount = 0;
    uint16_t count = 0;
    uint8_t dummyDataReceived = 0;
    uint8_t dummyDataTransmit = (uint8_t)spi4DummyData;

    uint8_t fifoMultiplier = 1;

    uint8_t  *pSend, *pReceived;
    uint16_t addressIncrement;
    uint16_t receiveAddressIncrement, sendAddressIncrement;

    addressIncrement = 1;
    fifoMultiplier = 4;
    
    // set the pointers and increment delta
    // for transmit and receive operations
    if (pTransmitData == NULL)
    {
        sendAddressIncrement = 0;
        pSend = (uint8_t*)&dummyDataTransmit;
    }
    else
    {
        sendAddressIncrement = addressIncrement;
        pSend = (uint8_t*)pTransmitData;
    }

    if (pReceiveData == NULL)
    {
       receiveAddressIncrement = 0;
       pReceived = (uint8_t*)&dummyDataReceived;
    }
    else
    {
       receiveAddressIncrement = addressIncrement;
       pReceived = (uint8_t*)pReceiveData;
    }


    while( SPI4STATLbits.SPITBF == true )
    { }

    while (dataSentCount < len)
    {
        if ((count < ((SPI_FIFO_FILL_LIMIT)*fifoMultiplier)))
        {
            SPI4BUFL = *pSend;
            pSend += sendAddressIncrement;
            dataSentCount++;
            count++;
        }

        if (SPI4STATLbits.SPIRBE == false)
        {
            *pReceived = SPI4BUFL;
            pReceived += receiveAddressIncrement;
            count--;
        }

    }
    while (count)
    {
        if (SPI4STATLbits.SPIRBE == false)
        {
            *pReceived = SPI4BUFL;
            pReceived += receiveAddressIncrement;
            count--;
        }
    }


    return dataSentCount;
}

/*****************************************************************************
 void SPI4_DummyDataSet(uint8_t dummyData)
 *****************************************************************************/
void SPI4_DummyDataSet(uint8_t dummyData)
{
    spi4DummyData = dummyData;
}

#endif //#ifdef DRV_SPI_CONFIG_CHANNEL_4_ENABLE

/*****************************************************************************
 * int SPILock(unsigned int channel)
 *****************************************************************************/
int DRV_SPI_Lock(uint8_t channel)
{
#ifdef DRV_SPI_CONFIG_CHANNEL_1_ENABLE
    if (channel == 1)
    {
        if(!spiMutex[0])
        {
            spiMutex[0] = 1;
            return 1;
        }

        return 0;
    }
#endif //#ifdef DRV_SPI_CONFIG_CHANNEL_1_ENABLE
#ifdef DRV_SPI_CONFIG_CHANNEL_2_ENABLE
    if (channel == 2)
    {
        if(!spiMutex[1])
        {
            spiMutex[1] = 1;
            return 1;
        }

        return 0;
    }
#endif // #ifdef DRV_SPI_CONFIG_CHANNEL_2_ENABLE
#ifdef DRV_SPI_CONFIG_CHANNEL_3_ENABLE
    if (channel == 3)
    {
        if(!spiMutex[2])
        {
            spiMutex[2] = 1;
            return 1;
        }

        return 0;
    }
#endif // #ifdef DRV_SPI_CONFIG_CHANNEL_3_ENABLE
#ifdef DRV_SPI_CONFIG_CHANNEL_4_ENABLE
    if (channel == 4)
    {
        if(!spiMutex[3])
        {
            spiMutex[3] = 1;
            return 1;
        }

        return 0;
    }
#endif // #ifdef DRV_SPI_CONFIG_CHANNEL_4_ENABLE
    return -1;
}

/*****************************************************************************
 * void SPIUnLock(unsigned int channel)
 *****************************************************************************/
void DRV_SPI_Unlock(uint8_t channel)
{
#ifdef DRV_SPI_CONFIG_CHANNEL_1_ENABLE
    if (channel == 1)
    {
        spiMutex[0] = 0;
    }
#endif //#ifdef DRV_SPI_CONFIG_CHANNEL_1_ENABLE
#ifdef DRV_SPI_CONFIG_CHANNEL_2_ENABLE
    if (channel == 2)
    {
        spiMutex[1] = 0;
    }
#endif // #ifdef DRV_SPI_CONFIG_CHANNEL_2_ENABLE
#ifdef DRV_SPI_CONFIG_CHANNEL_3_ENABLE
    if (channel == 3)
    {
        spiMutex[2] = 0;
    }
#endif // #ifdef DRV_SPI_CONFIG_CHANNEL_3_ENABLE
#ifdef DRV_SPI_CONFIG_CHANNEL_4_ENABLE
    if (channel == 4)
    {
        spiMutex[3] = 0;
    }
#endif // #ifdef DRV_SPI_CONFIG_CHANNEL_4_ENABLE
}


/**
 End of File
*/
