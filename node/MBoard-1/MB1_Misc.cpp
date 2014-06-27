/**
 * @file MB1_Misc.h
 * @author  Pham Huu Dang Nhat  <phamhuudangnhat@gmail.com>, HLib MBoard team.
 * @version 1.1
 * @date 17-11-2013
 * @brief This is header file for some misc functions for MBoard-1.
 *
 */

/* Includes */
#include "MB1_Misc.h"

/* Exported global vars */
static int16_t LedBeat_count = -1; // for IRQ of Led Beat
static Led *LedBeat_LedPtr = NULL;
uint16_t ledBeat_period = 0;

static uint32_t Delayms_count = 0; // for IRQ of Delayms

uint16_t miscTIM_period = 0;



/* Functions implementation */
void bugs_fix (void){
    Led Red (Led_ns::RGB_Red), Green (Led_ns::RGB_Green), Blue (Led_ns::RGB_Blue);

    Red.off();
    Green.off();
    Blue.off();
}

/**
 * @brief miscTIM_run config miscTIM to run with msec interrupt periodically.
 * @param TIM_Typedef* miscTIM, can be only basic timer (TM6 and TM7)
 * @param uint16_t prescaler
 * @param uint16_t reloadVal
 * @return void
 */
 void miscTIM_run (TIM_TypeDef* miscTIM, uint16_t prescaler, uint16_t reloadVal){
    /**< declare vars */
    bool isTIM6;
    uint32_t PCLK1, prescaler_x_reloadVal;
    RCC_ClocksTypeDef clocksStruct;
    NVIC_InitTypeDef nvicStruct;


    /**< check condition */
    if ( miscTIM == TIM6)
        isTIM6 = true;
    else if (miscTIM == TIM7)
        isTIM6 = false;
    else
        return;

    /**< init miscTIM in update mode with prescaler and reloadVal */
    RCC_APB1PeriphClockCmd ( (isTIM6 ? RCC_APB1Periph_TIM6 : RCC_APB1Periph_TIM7), ENABLE );

    TIM_ARRPreloadConfig (miscTIM, ENABLE);
    TIM_ITConfig (miscTIM, TIM_IT_Update, ENABLE);
    TIM_PrescalerConfig (miscTIM, prescaler, TIM_PSCReloadMode_Update);
    TIM_SetAutoreload (miscTIM, reloadVal);
    TIM_UpdateDisableConfig (miscTIM, DISABLE);
    TIM_UpdateRequestConfig (miscTIM, TIM_UpdateSource_Global);
    TIM_SelectOnePulseMode (miscTIM, TIM_OPMode_Repetitive);

    TIM_Cmd (miscTIM, ENABLE);

    /**< init NVIC TIM6 channel */
    nvicStruct.NVIC_IRQChannel = TIM6_IRQn;
    nvicStruct.NVIC_IRQChannelCmd = ENABLE;
    nvicStruct.NVIC_IRQChannelPreemptionPriority = 0x0F;
    nvicStruct.NVIC_IRQChannelSubPriority = 0x0F;

    NVIC_Init (&nvicStruct);


    /**< calculate miscTIM_period */
    RCC_GetClocksFreq (&clocksStruct);
    PCLK1 = clocksStruct.PCLK1_Frequency;

    PCLK1 /= 1000;

    prescaler_x_reloadVal = prescaler*reloadVal;

    miscTIM_period = prescaler_x_reloadVal / PCLK1;


    return;
}

/**
 * @brief LedBeat : turn On, Off, and config time base for led beat.
 *
 * @param bool On : on or off.
 * @param uint16_t msec : period of time between 2 led toogle (msec).
 * @param Leds *aLed : Ptr to a led.
 * @return void
 * - msec > miscTIM_period, and it should be : msec = n.miscTIM_period
 * - ledBeat_period = [msec / miscTIM_period] * miscTIM_period.
 */
 void LedBeat (bool On, uint16_t msec, Led* aLed){
    if (On == false){
        LedBeat_count = -1;
        LedBeat_LedPtr->off();
        LedBeat_LedPtr = NULL;
        return;
    }

    // On == true
    LedBeat_count = msec / miscTIM_period;
    ledBeat_period = LedBeat_count * miscTIM_period;
    LedBeat_LedPtr = aLed;

    return;
 }

/**
 * @brief LedBeat_SysTickISR
 * @return void
 * Use global var : LedBeat_count, LedBeat_LedPtr, miscTIM_period.
 */
void LedBeat_miscTIMISR (void){
    if (LedBeat_count < 0) // LedBeat Off.
        return;

    LedBeat_count--;
    if (LedBeat_count <= 0){
        LedBeat_count = ledBeat_period / miscTIM_period;
        if (LedBeat_LedPtr != NULL)
            LedBeat_LedPtr->toggle();
    }

    return;
}

/**
 * @brief delay_ms (uint32_t msec)
 * @param uint32_t msec : time of delay in msec.
 * @return void
 * Use global var : miscTIM_period, Delayms_count (static).
 * It should be : msec = n.miscTIM_period
 */
 void delay_ms (uint32_t msec){
    Delayms_count = (msec / miscTIM_period) + 1;
    while (Delayms_count > 0);

    return;
 }

/**
 * @brief Delayms_SysTickISR
 * @return void
 * Use global var : SysTick_period, Delayms_count (static).
 */
void delay_ms_miscTIMISR (void){
    if (Delayms_count > 0)
        Delayms_count--;

    return;
}
