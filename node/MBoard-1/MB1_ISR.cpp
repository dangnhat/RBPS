/**
 * @file MB1_ISR.cpp
 * @author  Pham Huu Dang Nhat  <phamhuudangnhat@gmail.com>, HLib MBoard team.
 * @version 1.0
 * @date 21-10-2013
 * @brief This is source file for interrupt handlers for MBoard-1.
 *
 */

/* Includes */
#include "MB1_ISR.h"
using namespace ISRMgr_ns;

/**<------------------- Sub ISR tables ---------------------*/

/**< SysTick sub ISR table */
void (* SysTick_subISR_table [numOfSubISR_max]) (void);
/**< SysTick sub ISR table */

/**< TIM6 sub ISR table */
void (* TIM6_subISR_table [numOfSubISR_max]) (void);
/**< TIM6 sub ISR table */

/**< USART1 sub ISR table */
void (* USART1_subISR_table [numOfSubISR_max]) (void);
/**< USART1 sub ISR table */

/**<------------------- Sub ISR tables ---------------------*/

/**< class ISRMgr */
ISRMgr :: ISRMgr (void){
    /**< Init sub ISR table of SysTick */
    SysTick_subISR_table_init ();
    TIM6_subISR_table_init ();
    USART1_subISR_table_init ();

    return;
}

/**
  * @brief subISR_assign. Assign a sub ISR function ptr to the sub ISR table of ISR_type
  * @param ISR_t ISR_type
  * @param void (* subISR_p)(void) (not a NULL ptr)
  * @return None.
  */
status_t ISRMgr::subISR_assign (ISR_t ISR_type, void (* subISR_p)(void) ){
    status_t retval = failed;

    if (subISR_p == NULL)
        return failed;

    switch (ISR_type){
    case ISRMgr_SysTick :
        retval = subISR_SysTick_assign (subISR_p);
        break;
    case ISRMgr_TIM6:
        retval = subISR_TIM6_assign (subISR_p);
        break;
    case ISRMgr_USART1:
        retval = subISR_USART1_assign (subISR_p);
        break;
    default:
        break;
    }

    return retval;
}

/**
  * @brief subISR_remove. remove a sub ISR function ptr to the sub ISR table of ISR_type
  * @param ISR_t ISR_type
  * @param void (* subISR_p)(void) (not a NULL ptr)
  * @return None.
  */
status_t ISRMgr::subISR_remove (ISR_t ISR_type, void (* subISR_p)(void) ){
    status_t retval = failed;

    if (subISR_p == NULL)
        return failed;

switch (ISR_type){
    case ISRMgr_SysTick:
        retval = subISR_SysTick_remove (subISR_p);
        break;
    case ISRMgr_TIM6:
        retval = subISR_TIM6_remove (subISR_p);
        break;
    case ISRMgr_USART1:
        retval = subISR_USART1_assign (subISR_p);
        break;
    default:
        break;
    }

    return retval;
}


/**< SysTick private */

/**
  * @brief subISR_SysTick_assign. assign a sub ISR func ptr to SysTick_subISR_table.
  * @param void (* subISR_p)(void)
  * @return None.
  */
status_t ISRMgr::subISR_SysTick_assign (void (* subISR_p)(void) ){
    uint8_t i;
    bool found = false;

    for (i = 0; i < numOfSubISR_max; i++){
        if (SysTick_subISR_table [i] == NULL){
            found = true;
            SysTick_subISR_table [i] = subISR_p;

            break;
        }
    }

    if (found)
        return successful;

    return failed;
}

/**
  * @brief subISR_SysTick_remove. Remove a sub ISR func ptr to SysTick_subISR_table.
  * @param void (* subISR_p)(void)
  * @return None.
  */
status_t ISRMgr::subISR_SysTick_remove (void (* subISR_p)(void) ){
    uint8_t i;
    bool found = false;

    for (i = 0; i < numOfSubISR_max; i++){
        if (SysTick_subISR_table [i] == subISR_p){
            found = true;
            SysTick_subISR_table [i] = NULL;

            break;
        }
    }

    if (found)
        return successful;

    return failed;
}


/**
  * @brief SysTick_subISR_table_init. Init all value in SysTick_subISR_table.
  * @return None.
  */
void ISRMgr::SysTick_subISR_table_init (void){
    uint8_t a_count;
    for (a_count = 0; a_count < numOfSubISR_max; a_count++ ){
        SysTick_subISR_table [a_count] = NULL;
    }
}
/**< SysTick private */

/**< TIM6 private */

/**
  * @brief subISR_TIM6_assign. assign a sub ISR func ptr to TIM6_subISR_table.
  * @param void (* subISR_p)(void)
  * @return None.
  */
status_t ISRMgr::subISR_TIM6_assign (void (* subISR_p)(void) ){
    uint8_t i;
    bool found = false;

    for (i = 0; i < numOfSubISR_max; i++){
        if (TIM6_subISR_table [i] == NULL){
            found = true;
            TIM6_subISR_table [i] = subISR_p;

            break;
        }
    }

    if (found)
        return successful;

    return failed;
}

/**
  * @brief subISR_TIM6_remove. Remove a sub ISR func ptr to TIM6_subISR_table.
  * @param void (* subISR_p)(void)
  * @return None.
  */
status_t ISRMgr::subISR_TIM6_remove (void (* subISR_p)(void) ){
    uint8_t i;
    bool found = false;

    for (i = 0; i < numOfSubISR_max; i++){
        if (TIM6_subISR_table [i] == subISR_p){
            found = true;
            TIM6_subISR_table [i] = NULL;

            break;
        }
    }

    if (found)
        return successful;

    return failed;
}


/**
  * @brief TIM6_subISR_table_init. Init all value in TIM6_subISR_table.
  * @return None.
  */
void ISRMgr::TIM6_subISR_table_init (void){
    uint8_t a_count;
    for (a_count = 0; a_count < numOfSubISR_max; a_count++ ){
        TIM6_subISR_table [a_count] = NULL;
    }
}
/**< TIM6 private */

/**< USART1 private */
/**
  * @brief subISR_USART1_assign. assign a sub ISR func ptr to USART1_subISR_table.
  * @param void (* subISR_p)(void)
  * @return None.
  */
status_t ISRMgr::subISR_USART1_assign (void (* subISR_p)(void) ){
    uint8_t i;
    bool found = false;

    for (i = 0; i < numOfSubISR_max; i++){
        if (USART1_subISR_table [i] == NULL){
            found = true;
            USART1_subISR_table [i] = subISR_p;

            break;
        }
    }

    if (found)
        return successful;

    return failed;
}

/**
  * @brief subISR_USART1_remove. Remove a sub ISR func ptr to USART1_subISR_table.
  * @param void (* subISR_p)(void)
  * @return None.
  */
status_t ISRMgr::subISR_USART1_remove (void (* subISR_p)(void) ){
    uint8_t i;
    bool found = false;

    for (i = 0; i < numOfSubISR_max; i++){
        if (USART1_subISR_table [i] == subISR_p){
            found = true;
            USART1_subISR_table [i] = NULL;

            break;
        }
    }

    if (found)
        return successful;

    return failed;
}


/**
  * @brief USART1_subISR_table_init. Init all value in USART1_subISR_table.
  * @return None.
  */
void ISRMgr::USART1_subISR_table_init (void){
    uint8_t a_count;
    for (a_count = 0; a_count < numOfSubISR_max; a_count++ ){
        USART1_subISR_table [a_count] = NULL;
    }
}
/**< USART1 private */



/* ISRs */
void SysTick_Handler (void){
    uint8_t a_count;

    for (a_count = 0; a_count < numOfSubISR_max; a_count++){
        if (SysTick_subISR_table[a_count] != NULL){
            SysTick_subISR_table[a_count] ();
        }
    }

    return;
}

void TIM6_IRQHandler (void){
    uint8_t a_count;

    /**< clear IT flag */
    TIM_ClearFlag (TIM6, TIM_FLAG_Update);

    for (a_count = 0; a_count < numOfSubISR_max; a_count++){
        if (TIM6_subISR_table[a_count] != NULL){
            TIM6_subISR_table[a_count] ();
        }
    }

    return;
}

void USART1_IRQHandler (void){
    uint8_t a_count;

    for (a_count = 0; a_count < numOfSubISR_max; a_count++){
        if (USART1_subISR_table[a_count] != NULL){
            USART1_subISR_table[a_count] ();
        }
    }

    /**< clear IT flag */
    USART_ClearITPendingBit  (USART1, USART_IT_RXNE);
    USART_ClearITPendingBit  (USART1, USART_IT_CTS);
    USART_ClearITPendingBit  (USART1, USART_IT_LBD);
    USART_ClearITPendingBit  (USART1, USART_IT_TC);

    return;
}


