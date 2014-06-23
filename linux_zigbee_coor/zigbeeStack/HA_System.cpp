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
 * - GPIOs for CS (CS1: PB5, CS0: PB4)  - reset : PA4
 * - decode table.                      - srdy : PB1
 * ----------------------------------   - mrdy : PB0
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
CC2530ZNP HA_ZNP;
ZbStack HA_ZbStack;

/**<------------------ global vars and objects in HA system ------------------*/

/**<------------------ conf (compile-time) interface ------------------*/

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
    HA_ZNP_init ();
    HA_ZbStack_init ();
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
	CC_ns::conf_USARTParams_s USparams;

	USparams.comport_num = 16;
	USparams.buad_rate = 115200;
	USparams.poll_period = 1000;


    HA_ZNP.init(&USparams);
    HA_ZNP.sys_reset_req ();

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
