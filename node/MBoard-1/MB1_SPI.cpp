/**
 * @file MB1_SPI.h
 * @author  Pham Huu Dang Nhat  <phamhuudangnhat@gmail.com>, HLib MBoard team.
 * @version 1.2
 * @date 15-11-2013
 * @brief This is source file for SPIs on MBoard-1. MBoard-1 only supports SP1 and
 * SP2 as AF. SPI1 on APB2 clock brigde with Fmax = SystemClock, SP2 on APB1 with
 * Fmax = SystemClock/2.
 */

#include "MB1_SPI.h"

using namespace SPI_ns;

/**< sys_config, we should config at compile time. */
/* Private vars and definitions */
SPI_TypeDef* SPIs [numOfSPIs] = {SPI1, SPI2};
uint32_t RCCSPIs [numOfSPIs] = {RCC_APB2Periph_SPI1, RCC_APB1Periph_SPI2};
void (* RCCSPI_FPtrs[numOfSPIs]) (uint32_t, FunctionalState) = {RCC_APB2PeriphClockCmd, RCC_APB1PeriphClockCmd} ;

/*  MOSI, MISO, CLK, NSS AF configuration depend on SPI mode and direction
    Speed is fixed at 50MHz
 */
const bool GPIO_isRemap [numOfSPIs] = {0, 0};

GPIO_TypeDef* MOSI_ports [numOfSPIs][2] = {   {GPIOA, GPIOB},     //SPI1
                                                    {GPIOB, NULL} };    //SPI2
const uint16_t MOSI_pins [numOfSPIs][2] = {   {GPIO_Pin_7, GPIO_Pin_5},   //SPI1
                                                    {GPIO_Pin_15, 0} };         //SPI2
uint32_t MOSI_RCCs [numOfSPIs][2] ={          {RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOB}, //SPI1
                                                    {RCC_APB2Periph_GPIOB, 0} }; //SPI2

GPIO_TypeDef* MISO_ports [numOfSPIs][2] = {   {GPIOA, GPIOB}, //SPI1
                                                    {GPIOB, NULL} }; //SPI2
const uint16_t MISO_pins [numOfSPIs][2] = {   {GPIO_Pin_6, GPIO_Pin_4}, //SPI1
                                                    {GPIO_Pin_14, 0} };
uint32_t MISO_RCCs [numOfSPIs][2] =       {   {RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOB}, //SPI1
                                                    {RCC_APB2Periph_GPIOB, 0} }; //SPI2

GPIO_TypeDef* SCK_ports [numOfSPIs][2] =  {   {GPIOA, GPIOB}, //SPI1
                                                    {GPIOB, NULL} }; //SPI2
const uint16_t SCK_pins [numOfSPIs][2] =  {   {GPIO_Pin_5, GPIO_Pin_3}, //SPI1
                                                    {GPIO_Pin_13, 0} }; //SPI2
uint32_t SCK_RCCs [numOfSPIs][2] = {             {RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOB}, //SPI1
                                                    {RCC_APB2Periph_GPIOB, 0} }; //SPI2

GPIO_TypeDef* hard_NSS_ports [numOfSPIs][2] = {   {GPIOA, GPIOA}, //SPI1
                                                        {GPIOB, NULL} };      //SPI2
const uint16_t hard_NSS_pins [numOfSPIs][2] = {   {GPIO_Pin_4, GPIO_Pin_15},  //SPI1
                                                        {GPIO_Pin_12, 0} };            //SPI2
uint32_t hard_NSS_RCCs [numOfSPIs][2] = {         {RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOA},   //SPI1
                                                        {RCC_APB2Periph_GPIOB, 0} };                        //SPI2
/**< end sys_conf */

/* Functions implementation for class SPI */
SPI::SPI (uint16_t usedSPI){
    this->usedSPI = usedSPI - 1;

    /**< init default value for data member */
    SS_pins_set = 0x00;
    SM_numOfNSSLines = 0;
    SM_numOfDevices = 0x01 << SM_numOfNSSLines;
    SM_deviceInUse = allFree;
}

/**
  * @brief init GPIOs for MOSI, MISO, SCK.
  * @return None
  * @attention set remap value for SPI properly.
  */
void SPI::M2F_GPIOs_Init (void) {
    bool remap_value = GPIO_isRemap [usedSPI];

    /**< Init RCC */
    RCC_APB2PeriphClockCmd (MOSI_RCCs[usedSPI][remap_value], ENABLE);
    RCC_APB2PeriphClockCmd (MISO_RCCs[usedSPI][remap_value], ENABLE);
    RCC_APB2PeriphClockCmd (SCK_RCCs[usedSPI][remap_value], ENABLE);

    /**< Init MOSI, MISO, SCK */
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Pin = MOSI_pins[usedSPI][remap_value];
    GPIO_Init (MOSI_ports[usedSPI][remap_value], &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = SCK_pins[usedSPI][remap_value];
    GPIO_Init (SCK_ports[usedSPI][remap_value], &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Pin = MISO_pins[usedSPI][remap_value];
    GPIO_Init (MISO_ports[usedSPI][remap_value], &GPIO_InitStruct);

    return;
}

/**
  * @brief Init SPI, base on parameters.
  * @return SPI_ns::status_t.
  * @attention I have implemented this function only for master mode, 2 lines, full duplex.
  */
status_t SPI::init (SPI_params_t *params_struct){

    /* Init SPI */
    (* RCCSPI_FPtrs[usedSPI])(RCCSPIs[usedSPI], ENABLE);

    SPI_InitTypeDef SPI_InitStruct;
    SPI_InitStruct.SPI_BaudRatePrescaler = params_struct->baudRatePrescaler;
    SPI_InitStruct.SPI_CPHA = params_struct->CPHA;
    SPI_InitStruct.SPI_CPOL = params_struct->CPOL;
    SPI_InitStruct.SPI_CRCPolynomial = params_struct->crcPoly;
    SPI_InitStruct.SPI_DataSize = params_struct->dataSize;
    SPI_InitStruct.SPI_Direction = params_struct->direction;
    SPI_InitStruct.SPI_FirstBit = params_struct->firstBit;
    SPI_InitStruct.SPI_Mode = params_struct->mode;
    SPI_InitStruct.SPI_NSS = params_struct->nss;

    SPI_Init (SPIs[usedSPI], &SPI_InitStruct);

    /* Enable SPI */
    SPI_Cmd (SPIs[usedSPI], ENABLE);

    /* Init NSS if NSS is hard_NSS */
    if (params_struct->nss == SPI_NSS_Hard){
        //TODO: implement for hard NSS if needed.
    }

    /**< Master mode init */
    if ( params_struct->mode == SPI_Mode_Master ){
        switch (params_struct->direction){

        case SPI_Direction_2Lines_FullDuplex :
            M2F_GPIOs_Init ();
            break;

        default :
            break;

        }/* end switch-case */
    }

    return successful;
}


/**< -------------- master mode --------------------------------*/

/**< slave_mgr interface */

/**
  * @brief SM_numOfSSLines_set.
  * @param uint8_t numOfSSLines, numOfSSLines < SPI_SSLines_max.
  * @return SPI_ns::status_t
  */
status_t SPI::SM_numOfSSLines_set (uint8_t numOfSSLines){
    /**< update SM_num_of_ss_lines */
    if (numOfSSLines > SSLines_max)
        return failed;

    SM_numOfNSSLines = numOfSSLines;
    SM_numOfDevices = 0x01 << SM_numOfNSSLines;

    return successful;
}

/**
  * @brief SM_GPIO_set, map a GPIO to a SSLine.
  * @param SPI_ns::SM_GPIOs_params *params_struct, params_struct->ss_lines is : 0 <= and < SPI_SSLines_max.
  * @return SPI_ns::status_t
  * @attention GPIO will be init-ed, SM_numOfSSLines must be set up before.
  */
status_t SPI::SM_GPIO_set (SM_GPIOParams_s *params_struct){
    /**< check conditions */
    if (params_struct->ssLine >= SSLines_max)
        return failed;

    /**< map GPIO to a ssLine */
    uint8_t line = params_struct->ssLine;

    softNSS_ports[line] = params_struct->GPIO_port;
    softNSS_pins[line] = params_struct->GPIO_pin;
    softNSS_RCCs[line] = params_struct->GPIO_clk;

    /**< Init GPIO */
    RCC_APB2PeriphClockCmd (softNSS_RCCs[line], ENABLE);

    GPIO_InitTypeDef GPIO_init_struct;
    GPIO_init_struct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_init_struct.GPIO_Pin = softNSS_pins[line];
    GPIO_init_struct.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_Init (softNSS_ports[line], &GPIO_init_struct);

    /**< update SS_pins_set */
    SS_pins_set |= (0x01 << (line + 1) );

    return successful;
}

/**
  * @brief SM_deviceToDecoder_set, map a device id to decoder value table.
  * @param SPI_ns::SM_device_t device : a device id.
  * @param uint8_t decode_value : when select a device id, decoder ic use decoder_value to set one of its pin low.
  * decode_value = 0 - SM_numOfDevices -1.
  * @return SPI_ns::status_t
  * This function will set SM_decode_value_all_free if device is SPI_ns::allFree (use in select, deselect a device)
  * and init decoder to all_free value ( call SM_device_deselect () ).
  */
status_t SPI::SM_deviceToDecoder_set (SM_device_t device, uint8_t decode_value){
    if (decode_value >= SM_numOfDevices)
        return failed;

    SM_deviceToDecoder_table [decode_value] = device;

    if (device == allFree){
        SM_decode_all_free = decode_value;
        SM_deviceInUse = allFree;
        SM_device_deselect(allFree);
    }

    return successful;
}

/**
  * @brief SM_device_attach, attach SPI to a device if SPI is free.
  * @param SPI_ns::SM_device_t device : a device id.
  * @return status_t
  * @attention SM_deviceToDecoder_table have been set up.
  */
status_t SPI::SM_device_attach (SM_device_t device){
    if (this->SM_deviceInUse != allFree)
        return busy;

    this->SM_deviceInUse = device;
    if ( SM_decodeValueInUse_update () != successful)
        return decodeValueNotFound;

    return successful;
}

/**
  * @brief SM_decodeValueInUse_update
  * @return status_t
  * @attention SM_deviceToDecoder_table have been set up.
  * This function will update SS_decode_value_in_use using SM_deviceInUse value;
  */
status_t SPI::SM_decodeValueInUse_update (void){
    uint8_t a_count;
    bool found;

    found = false;
    for (a_count = 0; a_count < SSDevices_max; a_count++){
        if (SM_deviceToDecoder_table[a_count] == SM_deviceInUse){
            found = true;
            break;
        }
    }

    if (found) {
        SM_decodeValueInUse = a_count;
        return successful;
    }

    return decodeValueNotFound;
}

/**
  * @brief SM_device_release, release SPI to a device if SPI is using by this device.
  * @param SPI_ns::SM_device_t device : a device id.
  * @return SPI_ns::status_t.
  * This function will deslect device and set SM_deviceInUse = allFree, also set allFree value for decoder.
  */
status_t SPI::SM_device_release (SM_device_t device){
    if (device != SM_deviceInUse)
        return failed;

    SM_device_deselect(SM_deviceInUse);
    SM_deviceInUse = allFree;
    return successful;
}

/**
  * @brief SM_device_select, set CS of the device on. (usually CS = low).
  * @param SPI_ns::SM_device_t device : a device id.
  * @return SPI_ns::status_t.
  * @attention : SPI_decode_value_in_use has been set up in attach function. All ss_lines have been set up.
  * - The device called this function has attached successfully before. Otherwise, there will be an infinite loop.
  */
status_t SPI::SM_device_select (SM_device_t device){
    /**< check conditions */
    if ((SS_pins_set & 0xFFFE) != ((0x01 << (SM_numOfNSSLines + 1)) - 2) ) // Not all SSLines have been set up.
        return failed;

    /**< check device */
    if (device != SM_deviceInUse){
        while (1);
    }

    /**< okay, all ss_lines have been set, set decode value to select device*/
    uint8_t temp_value, i;
    temp_value = SM_decodeValueInUse;

    for (i = 0; i < SM_numOfNSSLines; i++){
        if ((temp_value & 0x01) == 0)
            GPIO_ResetBits (softNSS_ports[i], softNSS_pins[i]);
        else
            GPIO_SetBits (softNSS_ports[i], softNSS_pins[i]);

        temp_value = temp_value >> 1;
    }

    return successful;
}

/**
  * @brief SM_device_deselect, deassert CS of the device on. (usually CS = high).
  * @param SPI_ns::SM_device_t device : a device id.
  * @return SPI_typesstatus_t.
  * @attention : SPI_decode_value_in_use has been set up in attach function. All ss_lines have been set up.
  * - The device called this function has attached successfully before. Otherwise, there will be an infinite loop.
  */
status_t SPI::SM_device_deselect (SM_device_t device){
    /**< check conditions */
    if ((SS_pins_set & 0xFFFE) != ((0x01 << (SM_numOfNSSLines + 1)) - 2) ) // Not all SSLines have been set up.
        return failed;

    /**< check device */
    if (device != SM_deviceInUse){
        while (1);
    }

    /**< okay, set decoder's value to all_free */
    uint8_t temp_value, i;
    temp_value = SM_decode_all_free;

    for (i = 0; i < SM_numOfNSSLines; i++){
        if ((temp_value & 0x01) == 0)
            GPIO_ResetBits (softNSS_ports[i], softNSS_pins[i]);
        else
            GPIO_SetBits (softNSS_ports[i], softNSS_pins[i]);

        temp_value = temp_value >> 1;
    }

    return successful;
}

/**< end slave_mgr */

/**< master 2 lines, full duplex interface */

/**
  * @brief M2F_sendAndGet_blocking, send data and read received data.
  * @param SPI_ns::SM_device_t device : a device id.
  * @param uint16_t data.
  * @return uint16_t : received data
  * @attention : blocking functions.
  * - The device called this function has attached successfully before. Otherwise, there will be an infinite loop.
  */
uint16_t SPI::M2F_sendAndGet_blocking (SM_device_t device, uint16_t data){
    /**< check device */
    if (device != SM_deviceInUse){
        while (1);
    }

    /* wait TXE = 1 */
    while (SPI_I2S_GetFlagStatus(SPIs[usedSPI], SPI_I2S_FLAG_TXE) == RESET);
    /* TXE is set */
    SPI_I2S_SendData (SPIs[usedSPI], data);

    /* wait RXNE = 1 */
    while (SPI_I2S_GetFlagStatus(SPIs[usedSPI], SPI_I2S_FLAG_RXNE) == RESET);
    /* RXNE is set */
    return SPI_I2S_ReceiveData (SPIs[usedSPI]);
}

/**< master 2 lines, full duplex interface */

/**< -------------- master mode --------------------------------*/
