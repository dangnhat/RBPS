/**
 * @file HA_System.cpp
 * @author  Pham Huu Dang Nhat  <phamhuudangnhat@gmail.com>, HLib MBoard team.
 * @version 1.0
 * @date 21-11-2013
 * @brief This file implement HA_System.
 * HA_System:
 *
 * (HA_SPI_p) ------------------------> (HA_ZNP) ---------------------------------> (HA_ZbStack)
 * (from other system)                  conf_SPIParams_s:
 * MB1_SPI1, SPI : master mode          - baudRate = 2.25 MHz
 * with SM configuration:               - SPI_ns::device_t = SPI_ns::cc2530_1
 * - numOfSSLines (2)                   conf_GPIOsParams_s:
 * - GPIOs for CS (CS1: PB13, CS0: PB12)  - reset : PA4
 * - decode table.                      - srdy : PB15
 * ----------------------------------   - mrdy : PB14
 * CS1 | CS0 | device_t             |   (remember to attach, and re-init SPI
 * ----------------------------------    before use)
 *   0 |   0 | SPI_ns::allFree      |
 *   0 |   1 | SPI_ns::cc2530_1     |
 *   1 |   0 | SPI_ns::at25Flash_1  |
 *   1 |   1 | SPI_ns::at25Flash_2  |
 * ----------------------------------
 *
 *                                      (HA_flash_1)                                (HA_flash_2)
 *
 */

/**< includes */
#include "HA_System.h"

/**<------------------ global vars and objects in HA system ------------------*/
SPI *HA_SPI_p = &MB1_SPI1;

CC2530ZNP HA_ZNP;
ZbStack HA_ZbStack;
HA_flash HA_aFlash;

/**<------------------ global vars and objects in HA system ------------------*/

/**<------------------ conf (compile-time) interface ------------------*/

/**< HA_SPI */
const uint8_t HA_SPI_numOfSSLines = 2;

GPIO_TypeDef *HA_SPI_Cs0GPIO_port = GPIOB;
const uint16_t HA_SPI_Cs0GPIO_pin = GPIO_Pin_12;
const uint32_t HA_SPI_Cs0GPIO_clk = RCC_APB2Periph_GPIOB;

GPIO_TypeDef *HA_SPI_Cs1GPIO_port = GPIOB;
const uint16_t HA_SPI_Cs1GPIO_pin = GPIO_Pin_13;
const uint32_t HA_SPI_Cs1GPIO_clk = RCC_APB2Periph_GPIOB;

SPI_ns::SM_device_t HA_SPI_decodeTable [0x01 << HA_SPI_numOfSSLines] = {
    SPI_ns::allFree,    //CS = 0
    SPI_ns::cc2530_1,   //CS = 1
    SPI_ns::at25Flash_1,//CS = 2
    SPI_ns::at25Flash_2 //CS = 3
};

void HA_SPI_init (void);
/**< HA_SPI */

/**< HA_ZNP */
const uint16_t HA_ZNP_SPI_baudRatePrescaler = SPI_BaudRatePrescaler_32;
SPI *HA_ZNP_SPI_p = HA_SPI_p;
const SPI_ns::SM_device_t HA_ZNP_SPI_deviceType = SPI_ns::cc2530_1;

GPIO_TypeDef *HA_ZNP_resetGPIO_port = GPIOA;
const uint16_t HA_ZNP_resetGPIO_pin = GPIO_Pin_4;
const uint32_t HA_ZNP_resetGPIO_clk = RCC_APB2Periph_GPIOA;

GPIO_TypeDef *HA_ZNP_srdyGPIO_port = GPIOB;
const uint16_t HA_ZNP_srdyGPIO_pin = GPIO_Pin_15;
const uint32_t HA_ZNP_srdyGPIO_clk = RCC_APB2Periph_GPIOB;

GPIO_TypeDef *HA_ZNP_mrdyGPIO_port = GPIOB;
const uint16_t HA_ZNP_mrdyGPIO_pin = GPIO_Pin_14;
const uint32_t HA_ZNP_mrdyGPIO_clk = RCC_APB2Periph_GPIOB;
const bool HA_ZNP_mrdyGPIO_isUsed = true;

void HA_ZNP_init (void);
/**< HA_ZNP */

/**< HA_ZbStack */
CC2530ZNP *HA_ZbStack_aZNP_p = &HA_ZNP;

void HA_ZbStack_init (void);
/**< HA_ZbStack */

/**<------------------ conf (compile-time) interface ------------------*/

/**<------------------ function implement ------------------*/
/**
  * @brief HA_system_init, init HA system.
  * @return None
  * It simply call :
  * + HA_SPI_init (); to init SPI with params in conf interface.
  * + HA_ZNP_init (); to init ZNP.
  * + HA_ZbStack_init (); to init ZbStack.
  *
  */
void HA_system_init (void){
    HA_SPI_init ();
    HA_ZNP_init ();
    HA_ZbStack_init ();
}

/**
  * @brief HA_SPI_init, init HA_SPI.
  * @return None
  * It uses params in conf interface to config Slave managerment for SPI:
  * number of SS lines, GPIOs for SS Lines, and decode table.
  * @attention This function only config SM for SPI, and SPI object must exist before.
  */
void HA_SPI_init (void){
    if (HA_SPI_p == NULL)
        return;

    /**< HA_SPI exist, conf SM */
    HA_SPI_p->SM_numOfSSLines_set (HA_SPI_numOfSSLines);

    SPI_ns::SM_GPIOParams_s HA_SPI_GPIOCs_s;

    HA_SPI_GPIOCs_s.GPIO_port = HA_SPI_Cs0GPIO_port;
    HA_SPI_GPIOCs_s.GPIO_pin = HA_SPI_Cs0GPIO_pin;
    HA_SPI_GPIOCs_s.GPIO_clk = HA_SPI_Cs0GPIO_clk;
    HA_SPI_GPIOCs_s.ssLine = 0;

    HA_SPI_p->SM_GPIO_set (&HA_SPI_GPIOCs_s);

    HA_SPI_GPIOCs_s.GPIO_port = HA_SPI_Cs1GPIO_port;
    HA_SPI_GPIOCs_s.GPIO_pin = HA_SPI_Cs1GPIO_pin;
    HA_SPI_GPIOCs_s.GPIO_clk = HA_SPI_Cs1GPIO_clk;
    HA_SPI_GPIOCs_s.ssLine = 1;

    HA_SPI_p->SM_GPIO_set (&HA_SPI_GPIOCs_s);

    uint8_t i;
    for ( i = 0; i < (0x01 << HA_SPI_numOfSSLines); i++ ){
        HA_SPI_p->SM_deviceToDecoder_set (HA_SPI_decodeTable[i], i);
    }

    return;
}

/**
  * @brief HA_ZbStack_init, init HA_ZbStack.
  * @return None
  * It uses params in conf interface to config SPI and GPIOs for HA_ZNP:
  * SPI params, GPIOs params
  * @attention
  * - After this function, SPI, and GPIOs is init-ed for HA_ZNP, but
  * it's recommended to attach, and re init SPI b/c SPI may be changed by other device.
  * - Release SPI after finish so other device can use.
  * - HA_SPI_init () has been called before this function.
  */
void HA_ZNP_init (void){

    /**< init SPI, GPIOs for HA_ZNP */
    CC_ns::conf_SPIParams_s CC_SPIParams_s;
    CC_SPIParams_s.baudRatePrescaler = HA_ZNP_SPI_baudRatePrescaler;
    CC_SPIParams_s.SPI_p = HA_ZNP_SPI_p;
    CC_SPIParams_s.deviceType = HA_ZNP_SPI_deviceType;

    CC_ns::conf_GPIOParams_s CC_GPIOParams_s;
    CC_GPIOParams_s.resetGPIO_port = HA_ZNP_resetGPIO_port;
    CC_GPIOParams_s.resetGPIO_pin = HA_ZNP_resetGPIO_pin;
    CC_GPIOParams_s.resetGPIO_clk = HA_ZNP_resetGPIO_clk;

    CC_GPIOParams_s.srdyGPIO_port = HA_ZNP_srdyGPIO_port;
    CC_GPIOParams_s.srdyGPIO_pin = HA_ZNP_srdyGPIO_pin;
    CC_GPIOParams_s.srdyGPIO_clk = HA_ZNP_srdyGPIO_clk;

    CC_GPIOParams_s.mrdyGPIO_port = HA_ZNP_mrdyGPIO_port;
    CC_GPIOParams_s.mrdyGPIO_pin = HA_ZNP_mrdyGPIO_pin;
    CC_GPIOParams_s.mrdyGPIO_clk = HA_ZNP_mrdyGPIO_clk;
    CC_GPIOParams_s.mrdyGPIO_isUsed = HA_ZNP_mrdyGPIO_isUsed;

    HA_ZNP.init (&CC_SPIParams_s, &CC_GPIOParams_s);
    HA_ZNP.cmd_SPI_attach ();
    HA_ZNP.sys_reset_hard ();
    HA_ZNP.cmd_SPI_release ();

    return;
}

/**
  * @brief HA_ZbStack_init, init HA_ZbStack.
  * @return None
  * It uses params in conf interface to config HA_ZbStack.
  * HA_ZbStack_aZNP_p
  * @attention
  * - This function map assign pointer to HA_ZNP to aZNP_p in HA_ZbStack.
  */
void HA_ZbStack_init (void){
    HA_ZbStack.init (HA_ZbStack_aZNP_p);

    return;
}

/**<------------------ function implement ------------------*/
