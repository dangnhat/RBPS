/**
 * @file MB1_Buttons.cpp
 * @author  Pham Huu Dang Nhat  <phamhuudangnhat@gmail.com>, HLib MBoard team.
 * @version 1.0
 * @date 17-11-2013
 * @brief This file source file for buttons on MBoard-1.
 *
 */

#include "MB1_Buttons.h"
using namespace Btn_ns;

/**< conf interface (compile-time) */
const uint8_t numOfBtns = 2;

const uint8_t btn_activeStates [numOfBtns] = {0, 0};
const uint8_t btn_samplingTimeCycle = 5; // * miscTIM_period
const uint16_t btn_longPressedTime = 200; // * btn_samplingTimeCycle

GPIO_TypeDef *btn_GPIO_ports [numOfBtns] = {GPIOB, GPIOA};
const uint16_t btn_GPIO_pins [numOfBtns] = {GPIO_Pin_2, GPIO_Pin_8};
GPIOMode_TypeDef btn_GPIO_modes [numOfBtns] = {GPIO_Mode_IN_FLOATING, GPIO_Mode_IN_FLOATING};
GPIOSpeed_TypeDef btn_GPIO_speeds [numOfBtns] = {GPIO_Speed_10MHz, GPIO_Speed_10MHz};

const uint32_t btn_GPIO_clks [numOfBtns] = {RCC_APB2Periph_GPIOB, RCC_APB2Periph_GPIOA};
/**< end conf interface */

/**< private vars */
uint8_t keyRead_1 [numOfBtns] = {!btn_activeStates[usrBtn_0], !btn_activeStates[usrBtn_1]};
uint8_t keyRead_2 [numOfBtns] = {!btn_activeStates[usrBtn_0], !btn_activeStates[usrBtn_1]};
uint8_t keyRead_3 [numOfBtns] = {!btn_activeStates[usrBtn_0], !btn_activeStates[usrBtn_1]};

uint8_t keyOldState [numOfBtns] = {!btn_activeStates[usrBtn_0], !btn_activeStates[usrBtn_1]};

bool isNewKeyPressed [numOfBtns] = {false, false};
bool isLongTimePressed [numOfBtns] = {false, false};
uint16_t longTimePressed_count [numOfBtns] = {0, 0};

uint8_t timeCycle_count = 0;

/**< prototypes */
void usrBtn_0_processing (void);
void usrBtn_1_processing (void);

void (*btn_processingFunctions_p [numOfBtns]) (void) = {usrBtn_0_processing, usrBtn_1_processing};
/**< private vars */

/**<----------------------------- class Button ----------------------------------*/

Button::Button (Btn_t usedBtn){
    this->usedBtn = usedBtn;

    this->GPIO_init ();
}

void Button::GPIO_init (void){

    RCC_APB2PeriphClockCmd (btn_GPIO_clks[usedBtn], ENABLE);

    GPIO_InitTypeDef GPIO_init_s;
    GPIO_init_s.GPIO_Mode = btn_GPIO_modes [usedBtn];
    GPIO_init_s.GPIO_Speed = btn_GPIO_speeds [usedBtn];
    GPIO_init_s.GPIO_Pin = btn_GPIO_pins [usedBtn];
    GPIO_Init (btn_GPIO_ports [usedBtn], &GPIO_init_s);
}

/**
 * @brief pressedKey_get, get key pressed value.
 * @return Btn_ns::retval_t
 * - noNewKey if there is now new key pressed.
 * - newKey : there is a short-time key pressed.
 * newKey just return once per key pressed.
 * - newLongKey : there is a long-time key pressed.
 * newLongKey will remain until the long-time key pressed end.
 */
retval_t Button::pressedKey_get (void){
    if (isNewKeyPressed [usedBtn] == true){
        isNewKeyPressed [usedBtn] = false;

        return newKey;
    }
    if (isLongTimePressed [usedBtn] == true)
        return newLongKey;

    return noNewKey;
}

/**
 * @brief isStillLongPressed.
 * @return bool, true if long key pressed is still hold.
 */
bool Button::isStillLongPressed (void){
    return isLongTimePressed [usedBtn];
}

/**<------------------------- end class Button ----------------------------------*/

/**< ISR for buttons */
void btnProcessing_miscTIMISR (void){
    timeCycle_count = (timeCycle_count + 1) % btn_samplingTimeCycle;

    if (timeCycle_count == (btn_samplingTimeCycle - 1)){
        /**< sampling and processing buttons */
        uint8_t a_count;
        for (a_count = 0; a_count < numOfBtns; a_count++){
            if (btn_processingFunctions_p[a_count] != NULL)
                btn_processingFunctions_p[a_count] ();
        }
    }
}

void usrBtn_0_processing (void){
    /**< sampling */
    keyRead_3 [usrBtn_0] = keyRead_2 [usrBtn_0];
    keyRead_2 [usrBtn_0] = keyRead_1 [usrBtn_0];
    keyRead_1 [usrBtn_0] = GPIO_ReadInputDataBit (btn_GPIO_ports[usrBtn_0], btn_GPIO_pins[usrBtn_0]);

    /**< update longTimePressed_count value */
    if (longTimePressed_count [usrBtn_0] != 0){

        longTimePressed_count [usrBtn_0] --;

        if (longTimePressed_count [usrBtn_0] == 0) // time out, long_key_pressed.
            isLongTimePressed [usrBtn_0] = true;
    }

    /**< processing */
    if ( (keyRead_1[usrBtn_0] == keyRead_2[usrBtn_0]) && (keyRead_2[usrBtn_0] == keyRead_3[usrBtn_0]) ){ /**< new stable state */

        if (keyRead_1[usrBtn_0] != keyOldState[usrBtn_0]){ /**< change state */

            if (keyRead_1[usrBtn_0] == btn_activeStates[usrBtn_0]){ /**< change from inactive->active*/
                keyOldState[usrBtn_0] = keyRead_1[usrBtn_0];

                longTimePressed_count [usrBtn_0] = btn_longPressedTime; //set time out value
            }

            if (keyRead_1[usrBtn_0] == !btn_activeStates[usrBtn_0]){ /**< change from active->inactive*/
                keyOldState[usrBtn_0] = keyRead_1[usrBtn_0];

                if (longTimePressed_count [usrBtn_0] > 0) // not time out yet, new key.
                    isNewKeyPressed[usrBtn_0] = true;

                else // long time pressed key end.
                    isLongTimePressed[usrBtn_0] = false;

                longTimePressed_count [usrBtn_0] = 0;
            }

        }/* end if */

    }/* end if */
}

void usrBtn_1_processing (void){
    /**< sampling */
    keyRead_3 [usrBtn_1] = keyRead_2 [usrBtn_1];
    keyRead_2 [usrBtn_1] = keyRead_1 [usrBtn_1];
    keyRead_1 [usrBtn_1] = GPIO_ReadInputDataBit (btn_GPIO_ports[usrBtn_1], btn_GPIO_pins[usrBtn_1]);

    /**< update longTimePressed_count value */
    if (longTimePressed_count [usrBtn_1] != 0){

        longTimePressed_count [usrBtn_1] --;

        if (longTimePressed_count [usrBtn_1] == 0) // time out, long_key_pressed.
            isLongTimePressed [usrBtn_1] = true;
    }

    /**< processing */
    if ( (keyRead_1[usrBtn_1] == keyRead_2[usrBtn_1]) && (keyRead_2[usrBtn_1] == keyRead_3[usrBtn_1]) ){ /**< new stable state */

        if (keyRead_1[usrBtn_1] != keyOldState[usrBtn_1]){ /**< change state */

            if (keyRead_1[usrBtn_1] == btn_activeStates[usrBtn_1]){ /**< change from inactive->active*/
                keyOldState[usrBtn_1] = keyRead_1[usrBtn_1];

                longTimePressed_count [usrBtn_1] = btn_longPressedTime; //set time out value
            }

            if (keyRead_1[usrBtn_1] == !btn_activeStates[usrBtn_1]){ /**< change from active->inactive*/
                keyOldState[usrBtn_1] = keyRead_1[usrBtn_1];

                if (longTimePressed_count [usrBtn_1] > 0) // not time out yet, new key.
                    isNewKeyPressed[usrBtn_1] = true;

                else // long time pressed key end.
                    isLongTimePressed[usrBtn_1] = false;

                longTimePressed_count [usrBtn_1] = 0;
            }

        }/* end if */

    }/* end if */


}
/**< end ISR for buttons */


