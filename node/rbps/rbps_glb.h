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

/* Command id */
const uint16_t update_node_id = 0x0003; // out
const uint16_t update_node_rep_id = 0x0016; // in
const uint16_t detail_info_id = 0x0014; // out
const uint16_t detail_info_rep_id = 0x0015; // in
const uint16_t measure_node_id = 0x0004; // in
const uint16_t hold_measurement_id = 0x0018; // out
const uint16_t node_report_id = 0x0017; // out
const uint16_t prediction_rep_id = 0x0010; // in

/* shared queue between main task and znp task */
extern OS_EventID main2znp_queue_id;
extern OS_EventID znp2main_queue_id;
const uint8_t rbps_ack = 'a';
const uint8_t rbps_command = 'c';

/* frame to share data between tasks */
/* firts byte: rbps_ack or rbps_command. If first byte is rbps_command then it's followed
 * by general frame format.
 */

/**
 * @brief 	Init RBPS system,
 * 			- Assign ISR for rbps_keypad.
 * 			- Create queue.
 */
void rbps_init(void);

/** @} */
#endif // RBPS_GLB_H_
