/**
 * @file MB1_Misc.h
 * @author  Pham Huu Dang Nhat  <phamhuudangnhat@gmail.com>, HLib MBoard team.
 * @version 1.1
 * @date 17-11-2013
 * @brief This is header file for some misc functions for MBoard-1.
 *
 */

#ifndef __MB1_MISC_H
#define __MB1_MISC_H

/* Includes */
#include "MB1_Glb.h"
#include "MB1_Leds.h" // for bug fix.

/* End includes */

/* Global vars */
//namespace Misc_ns{

extern uint16_t ledBeat_period; // real value in msec.
extern uint16_t miscTIM_period; // in msec.

//}

/* End global var */

/* Prototypes */
void bugs_fix (void);

//TODO Complete these funcs.
void delay_ms (uint32_t msec);
void delay_ms_miscTIMISR (void); // It should be placed in miscTIMISR.

void miscTIM_run (TIM_TypeDef *miscTIM, uint16_t prescaler, uint16_t reloadVal);
//uint32_t run_SysTick (uint16_t msec);

void LedBeat (bool On, uint16_t msec, Led *aLed);
void LedBeat_miscTIMISR (void); // It should be placed in miscTIMISR.

#endif // __MB1_MISC_H
