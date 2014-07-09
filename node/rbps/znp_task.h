/**
 * @defgroup
 * @brief
 * @ingroup     MainProgramSource
 * @{
 *
 * @file        znp_task.h
 * @brief       Header file for znp task.
 *
 * @author      DangNhat Pham-Huu <51002279@stu.hcmut.edu.vn>
 */

#ifndef ZNP_TASK_H_
#define ZNP_TASK_H_

#include "rbps_glb.h"

/* Debug definition */
#define ZNP_TASK_DEBUG (1)

/* stack */
const uint16_t znp_task_stack_size = 1024;
extern OS_STK znp_task_stack[znp_task_stack_size];

/**
 * @brief   znp task function.
 *
 * @param[in]   pdata
 *
 * @return      NERVER RETURN
 */
void znp_task_func(void *pdata);

/**
 * @brief   init zigbee stack with configuration in config interface of znp_task.cpp
 */
static void init_zigbee_stack(void);

/** @} */
#endif //ZNP_TASK_H_
