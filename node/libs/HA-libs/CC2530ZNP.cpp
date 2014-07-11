/**
 * @file CC2530ZNP.h
 * @author  Pham Huu Dang Nhat  <phamhuudangnhat@gmail.com>, HLib MBoard team.
 * @version 1.3
 * @date 18-11-2013
 * @brief This is source file for CC2530ZNP APIs.
 */

/* Includes */
#include "CC2530ZNP.h"

using namespace CC_ns;

/* Definitions of Len (1 byte, len = FF means vary) and Cmd (2 bytes) for CC2530ZNP API */
/* System Interface */
#define SYS_RESET_REQ               0x014100
#define SYS_RESET_IND               0x064180
#define SYS_VERSION                 0x002102
#define SYS_VERSION_SRSP            0x056102
#define SYS_OSAL_NV_READ            0x032108
#define SYS_OSAL_NV_SRSP            0xFF6108
#define SYS_OSAL_NV_WRITE           0xFF2109
#define SYS_OSAL_NV_WRITE_SRSP      0x016109
#define SYS_OSAL_NV_ITEM_INIT       0xFF2107
#define SYS_OSAL_NV_ITEM_INIT_SRSP  0x016107
#define SYS_OSAL_NV_DELETE          0x042112
#define SYS_OSAL_NV_DELETE_SRSP     0x016112
#define SYS_OSAL_NV_LENGTH          0x022113
#define SYS_OSAL_NV_LENGTH_SRSP     0x026113
#define SYS_ADC_READ                0x02210D
#define SYS_ADC_READ_SRSP           0x02610D
#define SYS_GPIO                    0x02210E
#define SYS_GPIO_SRSP               0x01610E
#define SYS_RANDOM                  0x00210C
#define SYS_RANDOM_SRSP             0x02610C
#define SYS_SET_TIME                0x0B2110
#define SYS_SET_TIME_SRSP           0x016110
#define SYS_GET_TIME                0x002111
#define SYS_GET_TIME_SRSP           0x0B6111
#define SYS_SET_TX_POWER            0x012114
#define SYS_SET_TX_POWER_SRSP       0x016114

/* Configuration Interface */
#define ZB_READ_CONFIGURATION       0x012604
#define ZB_READ_CONFIGURATION_SRSP  0xFF6604
#define ZB_WRITE_CONFIGURATION      0xFF2605
#define ZB_WRITE_CONFIGURATION_SRSP 0x016605

/* Simple API interface */
#define ZB_APP_REGISTER_REQUEST     0xFF260A
#define ZB_APP_REGISTER_REQUEST_SRSP 0x01660A
#define ZB_START_REQUEST            0x002600
#define ZB_START_REQUEST_SRSP       0x006600
#define ZB_START_CONFIRM            0x014680
#define ZB_PERMIT_JOINING_REQUEST   0x032608
#define ZB_PERMIT_JOINING_REQUEST_SRSP 0x016608
#define ZB_BIND_DEVICE              0x0B2601
#define ZB_BIND_DEVICE_SRSP         0x006601
#define ZB_BIND_CONFIRM             0x034681
#define ZB_ALLOW_BIND               0x012602
#define ZB_ALLOW_BIND_SRSP          0x006602
#define ZB_ALLOW_BIND_CONFIRM       0x024682
#define ZB_SEND_DATA_REQUEST        0xFF2603
#define ZB_SEND_DATA_REQUEST_SRSP   0x006603
#define ZB_SEND_DATA_CONFIRM        0x024683
#define ZB_RECEIVE_DATA_INDICATION  0xFF4687
#define ZB_GET_DEVICE_INFO          0x012606
#define ZB_GET_DEVICE_INFO_SRSP     0x096606
#define ZB_FIND_DEVICE_REQUEST      0x082607
#define ZB_FIND_DEVICE_REQUEST_SRSP 0x006607
#define ZB_FIND_DEVICE_CONFIRM      0x0B4685


/**
  * @brief Constructor of CC2530ZNP class
  * @return None
  * - Allocate mem for a new CC2530 object.
  * - Init value for data members. Numerical vars set to 0, pointer vars set to NULL.
  */
CC2530ZNP::CC2530ZNP (void){

    /**< conf (run-time) interface */
    baudRatePrescaler = 0;
    SPI_p = NULL;

    resetGPIO_port = NULL;
    resetGPIO_pin = 0;
    resetGPIO_clk = 0;

    srdyGPIO_port = NULL;
    srdyGPIO_pin = 0;
    srdyGPIO_clk = 0;

    mrdyGPIO_isUsed = false; // if it's false, no need to config other params for mrdy.
    mrdyGPIO_port = NULL;
    mrdyGPIO_pin = 0;
    mrdyGPIO_clk = 0;

    /**< end conf (run-time) interface */

    /**< command interface */
    SPI_isAttached = false;

    /**< attributes */
    transportRev = 0;
    productID = 0;
    majorRel = 0;
    minorRel = 0;
    maintRel = 0;

    errno = successful;
    /**< end attributes */

    return;
}

/**
  * @brief init, Init cc2530 for the first time.
  * @param CC_types::conf_SPIParams_s *SPIParams_s.
  * @param CC_types::conf_GPIOParams_s *GPIOParams_s.
  * @return CC_types::status_t.
  */
status_t CC2530ZNP::init (conf_SPIParams_s *SPIParams_s, conf_GPIOParams_s *GPIOParams_s){

    /* check parameters */
    if (SPIParams_s->SPI_p == NULL)
        return failed;
    if ( (GPIOParams_s->resetGPIO_port == NULL) || (GPIOParams_s->srdyGPIO_port == NULL) )
        return failed;
    if ( (GPIOParams_s->mrdyGPIO_port == NULL) && (GPIOParams_s->mrdyGPIO_isUsed) )
        return failed;

    /* update data members of CC2530 */
    baudRatePrescaler = SPIParams_s->baudRatePrescaler;
    SPI_p = SPIParams_s->SPI_p;
    deviceType = SPIParams_s->deviceType;

    resetGPIO_port = GPIOParams_s->resetGPIO_port;
    resetGPIO_pin = GPIOParams_s->resetGPIO_pin;
    resetGPIO_clk = GPIOParams_s->resetGPIO_clk;

    srdyGPIO_port = GPIOParams_s->srdyGPIO_port;
    srdyGPIO_pin = GPIOParams_s->srdyGPIO_pin;
    srdyGPIO_clk = GPIOParams_s->srdyGPIO_clk;

    mrdyGPIO_isUsed = GPIOParams_s->mrdyGPIO_isUsed; // if it's false, no need to config other params for mrdy.
    if (mrdyGPIO_isUsed){
        mrdyGPIO_port = GPIOParams_s->mrdyGPIO_port;
        mrdyGPIO_pin = GPIOParams_s->mrdyGPIO_pin;
        mrdyGPIO_clk = GPIOParams_s->mrdyGPIO_clk;
    }

    /* Init GPIOs and SPI */
    SPI_reInit ();
    GPIOs_reInit ();

    return successful;
}

/**
  * @brief SPI_reInit, Init SPI with parameters from data members of object.
  * @return CC_types::status_t.
  */
status_t CC2530ZNP::SPI_reInit (void){
    if (SPI_p == NULL)
        return failed;

    SPI_ns::SPI_params_t SPI_paramsStruct;
    SPI_paramsStruct.baudRatePrescaler = baudRatePrescaler;
    SPI_paramsStruct.CPHA = SPI_CPHA_2Edge;
    SPI_paramsStruct.CPOL = SPI_CPOL_Low;
    SPI_paramsStruct.dataSize = SPI_DataSize_8b;
    SPI_paramsStruct.firstBit = SPI_FirstBit_MSB;
    SPI_paramsStruct.direction = SPI_Direction_2Lines_FullDuplex;
    SPI_paramsStruct.mode = SPI_Mode_Master;
    SPI_paramsStruct.nss = SPI_NSS_Soft;
    SPI_paramsStruct.crcPoly = 0x01;
    SPI_p->init (&SPI_paramsStruct);

    return successful;
}

/**
  * @brief GPIOs_reInit, Init GPIOs with parameters from data members of object.
  * @return CC_types::status_t.
  */
status_t CC2530ZNP::GPIOs_reInit (void){
    /* check parameters */
    if ( (resetGPIO_port == NULL) || (srdyGPIO_port == NULL) )
        return failed;
    if ( (mrdyGPIO_port == NULL) && (mrdyGPIO_isUsed) )
        return failed;

    /* Init GPIOs */
    RCC_APB2PeriphClockCmd (srdyGPIO_clk | resetGPIO_clk, ENABLE);
    if (mrdyGPIO_isUsed)
        RCC_APB2PeriphClockCmd (mrdyGPIO_clk, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStruct.GPIO_Pin = resetGPIO_pin;
    GPIO_Init (resetGPIO_port, &GPIO_InitStruct);

    if (mrdyGPIO_isUsed){
        GPIO_InitStruct.GPIO_Pin = mrdyGPIO_pin;
        GPIO_Init (mrdyGPIO_port, &GPIO_InitStruct);
        GPIO_SetBits (mrdyGPIO_port, mrdyGPIO_pin);
    }

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Pin = srdyGPIO_pin;
    GPIO_Init (srdyGPIO_port, &GPIO_InitStruct);

    return successful;
}

/**
  * @brief cmd_SPI_attach, attach SPI object to this CC2530ZNP object.
  * @return CC_types::status_t: CC_types::SPI_busy if SPI is being used by other device. Otherwise, CC_types::successful.
  */
status_t CC2530ZNP::cmd_SPI_attach (void){
    SPI_ns::status_t retVal;
    retVal = SPI_p->SM_device_attach (deviceType);

    if (retVal == SPI_ns::busy)
        return SPI_busy;
    else if (retVal == SPI_ns::successful){
        SPI_isAttached = true;
        return successful;
    }
    else
        return failed;
}

/**
  * @brief cmd_SPI_release, release SPI object from this object so other device can use.
  * @return CC_types::status_t, CC_types::failed if this object is not using SPI object.
  */
status_t CC2530ZNP::cmd_SPI_release (void){
    SPI_ns::status_t retVal;
    retVal = SPI_p->SM_device_release (deviceType);

    if (retVal == SPI_ns::successful){
        SPI_isAttached = false;
        return successful;
    }
    else
        return failed;
}

/**
  * @brief send POLL cmd, place respond message in dataBuffer, dataLen and cmdBuffer.
  * @return CC_types::status_t.
  * @attention
  * + GPIO and SPI have been setup before call this functions.
  * + NSS must be high before call this function.
  * + attach a SPI before call this function, otherwise, there will be an infinite loop.
  */
status_t CC2530ZNP::cmd_POLL (void){
    /* some vars */
    uint16_t aCount;

    /* check SPI_isAttached */
    if (!SPI_isAttached)
        return failed;

    dataLen = 0;

    /* wait for SRDY to go low */
    while (GPIO_ReadInputDataBit (srdyGPIO_port, srdyGPIO_pin) == 1);

    /* SRDY = 0, set NSS, MRDY low */
    SPI_p->SM_device_select(deviceType);

    if (mrdyGPIO_isUsed)
        GPIO_ResetBits (mrdyGPIO_port, mrdyGPIO_pin);

    /* Send POLL cmd */
    SPI_p->M2F_sendAndGet_blocking (deviceType, 0); //Length
    SPI_p->M2F_sendAndGet_blocking (deviceType, 0); //cmd0
    SPI_p->M2F_sendAndGet_blocking (deviceType, 0); //cmd1

    /* wait for SRDY to go high */
    while (GPIO_ReadInputDataBit (srdyGPIO_port, srdyGPIO_pin) == 0);

    /* SRDY high, get AREQ message */
    dataLen = SPI_p->M2F_sendAndGet_blocking (deviceType, 0xFF); //dummy byte
    cmdBuffer[0] = SPI_p->M2F_sendAndGet_blocking (deviceType, 0xFF);
    cmdBuffer[1] = SPI_p->M2F_sendAndGet_blocking (deviceType, 0xFF);
    for (aCount = 0; aCount < dataLen; aCount++){
        dataBuffer[aCount] = SPI_p->M2F_sendAndGet_blocking (deviceType, 0xFF);
    }

    /* Set NSS = 1, end transaction */
    SPI_p->SM_device_deselect (deviceType);

    if (mrdyGPIO_isUsed)
        GPIO_SetBits (mrdyGPIO_port, mrdyGPIO_pin);

    return successful;
}

/**
  * @brief send SREQ cmd, place respond message in dataBuffer, dataLen and cmdBuffer.
  * @param uint16_t cmd : cmd to send, format (MSB->LSB) : cmd1 cmd0.
  * @param uint8_t len : length of data to send in array pointed by data_p. len should be less than or equal to 250.
  * @param uint8_t* data_p : pointer to array of data to send.
  * @return CC_types::status_t.
  * @attention
  * + GPIO and SPI have been setup before call this functions.
  * + NSS must be high before call this function.
  * + SPI must be attached successfully, otherwise, there will be an infinite loop.
  */
status_t CC2530ZNP::cmd_SREQ (uint16_t cmd, uint8_t len, uint8_t* data_p){
    uint8_t aCount;

    /* check SPI_isAttached */
    if (!SPI_isAttached)
        return failed;

    /* check len, data_p */
    if ((len > 250) || ((len != 0) && (data_p == NULL)) )
        return failed;
    /* len, DataPtr ok */

    dataLen = 0;

    /* Set NSS, MRDY low, prepare SREQ transaction */
    SPI_p->SM_device_select (deviceType);

    if (mrdyGPIO_isUsed)
        GPIO_ResetBits (mrdyGPIO_port, mrdyGPIO_pin);

    /* wait for SRDY to go low */
    while (GPIO_ReadInputDataBit (srdyGPIO_port, srdyGPIO_pin) == 1);

    /* SRDY = 0, send frame to CC2530ZNP */
    SPI_p->M2F_sendAndGet_blocking (deviceType, len); // send length.
    SPI_p->M2F_sendAndGet_blocking (deviceType, (uint8_t) cmd); // send cmd0
    SPI_p->M2F_sendAndGet_blocking (deviceType, (uint8_t) (cmd >> 8)); // send cmd1;

    for (aCount = 0; aCount < len; aCount++){ // send data.
        SPI_p->M2F_sendAndGet_blocking (deviceType, data_p[aCount]);
    }

    /* finish sending frame, wait SRDY to go high */
    while (GPIO_ReadInputDataBit (srdyGPIO_port, srdyGPIO_pin) == 0);

    /* SRDY = 1, receive SRSP from CC2530ZNP */
    dataLen = SPI_p->M2F_sendAndGet_blocking  (deviceType, 0xFF); // get data len.
    cmdBuffer[0] = SPI_p->M2F_sendAndGet_blocking (deviceType, 0xFF); // get cmd0
    cmdBuffer[1] = SPI_p->M2F_sendAndGet_blocking  (deviceType, 0xFF); // get cmd1
    for (aCount = 0; aCount < dataLen; aCount++){ // get data
        dataBuffer[aCount] = SPI_p->M2F_sendAndGet_blocking  (deviceType, 0xFF);
    }

    /* Set NSS = 1, end transaction */
    SPI_p->SM_device_deselect(deviceType);

    if (mrdyGPIO_isUsed)
        GPIO_SetBits (mrdyGPIO_port, mrdyGPIO_pin);

    return successful;
}

/**
  * @brief cmd_isNewMessage, check if there is a new message from CC2530,
  * and POLL to get message to cmdBuffer, dataLen, dataBuffer.
  * @return bool, true if there is new message. Otherwise, false.
  * @attention
  * + GPIO and SPI have been setup before call this functions.
  * + NSS must be high before call this function.
  * + SPI must be attached successfully, otherwise, there will be an infinite loop.
  * + application should get data from cmdBuffer, dataLen, dataBuffer before call other CC2350 function.
  */
bool CC2530ZNP::cmd_isNewMessage (void){
    /**< check SRDY */
    if ( GPIO_ReadInputDataBit (srdyGPIO_port, srdyGPIO_pin) != 0x00 )
        return false;

    /**< new message */
    cmd_POLL ();

    return true;
}

/**
  * @brief CC2530 hard reset, reset and send POLL command to get SYS_RESET_IND message
  * and update versions info in data members.
  * @return CC_types::status_t.
  * @attention
  * + GPIOs have been setup before call this functions.
  */
status_t CC2530ZNP::sys_reset_hard (void){
    GPIO_ResetBits (resetGPIO_port, resetGPIO_pin);
    delay_us (1000);
    GPIO_SetBits (resetGPIO_port, resetGPIO_pin);
    delay_us (3000);

    cmd_POLL ();

    if (cmdBuffer[0] != 0x41 || cmdBuffer[1] != 0x80)
        return failed;

    transportRev = dataBuffer[1];
    productID = dataBuffer[2];
    majorRel = dataBuffer[3];
    minorRel = dataBuffer[4];
    maintRel = dataBuffer[5];

    return successful;
}
