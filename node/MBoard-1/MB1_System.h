/**
 * @file MB1_System.h
 * @author  Pham Huu Dang Nhat  <phamhuudangnhat@gmail.com>, HLib MBoard team.
 * @version 1.1
 * @date 17-11-2013
 * @brief This is the entry point of MBoard-1 system.
 */

#ifndef __MB1_SYSTEM_H
#define __MB1_SYSTEM_H

/* Inlcudes */
#include "MB1_Glb.h"
#include "MB1_Leds.h"
#include "MB1_Serial_t.h"
#include "MB1_Misc.h"
#include "MB1_ISR.h"
#include "MB1_SPI.h"
#include "MB1_Buttons.h"
#include "hl_crc.h"

/**<-------------- Global vars and objects in the system of MB1 ------------*/

/**< SPIs */
extern SPI MB1_SPI1;
extern SPI MB1_SPI2;

/**< LEDs */
extern Led MB1_Led_D5;
extern Led MB1_RGB_red;
extern Led MB1_RGB_blue;
extern Led MB1_RGB_green;

/**< Buttons */
extern Button MB1_usrBtn0;
extern Button MB1_usrBtn1;

/**< USARTs */
extern serial_t MB1_USART1;
extern serial_t MB1_USART2;

/**< CRC */
extern CRC_c MB1_crc;

/**< Misc, global functions ! */

/**< ISRs */
extern ISRMgr MB1_ISRs;

/**<-------------- Global vars and objects in the system of MB1 ------------*/

/**<-------------- MB1 system functions ------------*/
void MB1_system_init (void);
/**<-------------- MB1 system functions ------------*/


#endif // __MB1_SYSTEM_H
