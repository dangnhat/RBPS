/**
 * @file MB1_Buttons.h
 * @author  Pham Huu Dang Nhat  <phamhuudangnhat@gmail.com>, HLib MBoard team.
 * @version 1.0
 * @date 17-11-2013
 * @brief
 * ***************************************************************************************
 *          reset           pressed                             release
 * button ---- i i i i i i i i a a a a a a a a a a a a a a a a a i i i i i i i i i -----
 *                          time_out > 0 ------------- = 0
 *          nothing_new     (set time out).            long_key nothing_new
 *---------------------------------------------------------------------------------------
 *          reset           pressed                 release     read_key()
 * button ---- i i i i i i i i a a a a a a a a a a a i i i i i i i i i i i i i i i -----
 *                          time_out > 0 ---------- >0
 *          nothing_new     (set time out).         new_key     new_key nothing_new
 *
 * ***************************************************************************************
 * How to use this lib :
 * - Declare an instance of class Button.
 * - assign btnProcessing_SysTickISR to MiscTIM ISR.
 */

#ifndef MB1_BUTTONS_H_
#define MB1_BUTTONS_H_

#include "MB1_Glb.h"
#include "MB1_Misc.h"

namespace Btn_ns{
typedef enum {usrBtn_0 = 0, usrBtn_1 = 1} Btn_t;
typedef enum {noNewKey, newKey, newLongKey} retval_t;
}

class Button {
public:
    Button (Btn_ns::Btn_t usedBtn);
    Btn_ns::retval_t pressedKey_get (void);
    bool isStillLongPressed (void);

private:
    Btn_ns::Btn_t usedBtn;

    void GPIO_init (void);
};

/**< SysTick ISRs for buttons */
void btnProcessing_miscTIMISR (void);


#endif // MB1_BUTTONS_H_
