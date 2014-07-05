/**
 * @file MB1_ISR.h
 * @author  Pham Huu Dang Nhat  <phamhuudangnhat@gmail.com>, HLib MBoard team.
 * @version 1.0
 * @date 21-10-2013
 * @brief This is header file for interrupt handlers for MBoard-1.
 *
 */

#ifndef __MB1_ISR_H_
#define __MB1_ISR_H_

/* Includes */
#include "MB1_Glb.h"
#include "MB1_Misc.h"

namespace ISRMgr_ns {

const uint8_t numOfSubISR_max = 8;

typedef enum {
    successful,
    failed
} status_t;

typedef enum {
    ISRMgr_SysTick,
    ISRMgr_TIM6,
    ISRMgr_USART1,
} ISR_t;

}

class ISRMgr {
public:
    ISRMgr (void);
    ISRMgr_ns::status_t subISR_assign (ISRMgr_ns::ISR_t ISR_type, void (* subISR_p)(void) );
    ISRMgr_ns::status_t subISR_remove (ISRMgr_ns::ISR_t ISR_type, void (* subISR_p)(void) );

private:
    /**< SysTick */
    void SysTick_subISR_table_init (void);
    ISRMgr_ns::status_t subISR_SysTick_assign (void (* subISR_p)(void) );
    ISRMgr_ns::status_t subISR_SysTick_remove (void (* subISR_p)(void) );
    /**< SysTick */

    /**< TIM6 */
    void TIM6_subISR_table_init (void);
    ISRMgr_ns::status_t subISR_TIM6_assign ( void (* subISR_p) (void) );
    ISRMgr_ns::status_t subISR_TIM6_remove ( void (* subISR_p) (void) );
    /**< TIM6 */

    /**< USART1 */
    void USART1_subISR_table_init (void);
    ISRMgr_ns::status_t subISR_USART1_assign ( void (* subISR_p) (void) );
    ISRMgr_ns::status_t subISR_USART1_remove ( void (* subISR_p) (void) );
    /**< USART1 */
};

#ifdef __cplusplus
extern "C" {
#endif

/* ISRs */
//void SysTick_Handler (void);
void TIM6_IRQHandler (void);
void USART1_IRQHandler (void);

#ifdef __cplusplus
}
#endif

#endif // __MB1_IRQ_H_
