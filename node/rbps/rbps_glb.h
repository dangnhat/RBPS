/**
 * @defgroup
 * @brief
 * @ingroup     MainProgramSource
 * @{
 *
 * @file        rbps_glb.h
 * @brief       Global header for remote blood pressure and heart rate monitoring system.
 *
 * @author      DangNhat Pham-Huu <51002279@stu.hcmut.edu.vn>
 */

#ifndef RBPS_GLB_H_
#define RBPS_GLB_H_

/* Includes */
#include "MB1_System.h"
#include "coocox.h"
#include "rbpm_gui.h"
#include "keypad.h"

/* extern global variables */
extern keypad rbps_keypad;
extern rbpm_gui rbps_node_gui;

/* definitions */
#define RBPS_DEBUG (0)
#if RBPS_DEBUG
#define RBPS_PRINTF(...) printf(__VA_ARGS__)
#else
#define RBPS_PRINTF(...)

/* shared queue between main task and znp task */
extern OS_EventID main2znp_queue_id;
extern OS_EventID znp2main_queue_id;
const uint8_t rbps_ack = 'A';

/**
 * @brief 	Init RBPS system,
 * 			- Assign ISR for rbps_keypad.
 * 			- Create queue.
 */
void rbps_init(void);

/** @} */
#endif // CLCD20x4_H_
