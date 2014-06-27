/**
 * @file MB1_Leds.cpp
 * @author  Pham Huu Dang Nhat  <phamhuudangnhat@gmail.com>, HLib MBoard team.
 * @version 1.0
 * @date 18-10-2013
 * @brief This is source file for Leds on MBoard-1.
 *
 */

/* Includes */
#include "MB1_Leds.h"
using namespace Led_ns;

/* Private vars and defs */
GPIO_TypeDef * LedGPIOPorts [] = {GPIOC, GPIOC, GPIOC, GPIOC};
GPIOMode_TypeDef LedGPIOMode [] = {GPIO_Mode_Out_PP, GPIO_Mode_Out_PP, GPIO_Mode_Out_PP, GPIO_Mode_Out_PP};
const uint16_t LedGPIOPin [] = {GPIO_Pin_4, GPIO_Pin_7, GPIO_Pin_5, GPIO_Pin_9};
GPIOSpeed_TypeDef LedGPIOSpeed [] = {GPIO_Speed_2MHz, GPIO_Speed_2MHz, GPIO_Speed_2MHz, GPIO_Speed_2MHz};

const uint32_t LedGPIOClk [] = {RCC_APB2Periph_GPIOC,RCC_APB2Periph_GPIOC,RCC_APB2Periph_GPIOC,RCC_APB2Periph_GPIOC};

/* Functions implementation */
/* for class Leds */

Led::Led(Led_t used_Led){
    GPIO_InitTypeDef GPIO_InitStruct;

    this->used_Led = used_Led;

    /* Init used Led */
    RCC_APB2PeriphClockCmd(LedGPIOClk[used_Led], ENABLE);

    GPIO_InitStruct.GPIO_Mode = LedGPIOMode[used_Led];
    GPIO_InitStruct.GPIO_Pin = LedGPIOPin[used_Led];
    GPIO_InitStruct.GPIO_Speed = LedGPIOSpeed[used_Led];
    GPIO_Init (LedGPIOPorts[used_Led], &GPIO_InitStruct);
}

void Led::on(void){
    GPIO_SetBits(LedGPIOPorts[used_Led], LedGPIOPin[used_Led]);
}

void Led::off(void){
    GPIO_ResetBits(LedGPIOPorts[used_Led], LedGPIOPin[used_Led]);
}

void Led::toggle(void){
    GPIO_ReadOutputDataBit(LedGPIOPorts[used_Led], LedGPIOPin[used_Led]) ? GPIO_ResetBits(LedGPIOPorts[used_Led], LedGPIOPin[used_Led]) : GPIO_SetBits(LedGPIOPorts[used_Led], LedGPIOPin[used_Led]) ;
}
