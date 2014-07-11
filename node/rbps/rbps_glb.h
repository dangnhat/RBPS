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
#include "cir_queue.h"

/* definitions */
#define RBPS_DEBUG (1)
#if RBPS_DEBUG
#define RBPS_PRINTF(...) printf(__VA_ARGS__)
#else
#define RBPS_PRINTF(...)
#endif

namespace rbps_ns {
	/* extern global variables */
	extern keypad node_keypad;
	extern rbpm_gui node_gui;
	extern uint16_t node_id; // node_id will be initialized after Zigbee stack has been
							// started. 0xFFFF mean it's not ready to use.
	static const uint16_t coor_short_addr = 0x0000;

	/* Command id */
	const uint16_t update_node_id = 0x0003; // out
	const uint16_t update_node_rep_id = 0x0016; // in
	const uint16_t detail_info_id = 0x0014; // out
	const uint16_t detail_info_rep_id = 0x0015; // in
	const uint16_t measure_node_id = 0x0004; // in
	const uint16_t hold_measurement_id = 0x0018; // out
	const uint16_t node_report_id = 0x0017; // out
	const uint16_t prediction_rep_id = 0x0010; // in

	const uint8_t update_node_length = 8;
	const uint8_t update_node_rep_length = 1;
	const uint8_t detail_info_length = 4;
	const uint8_t detail_info_rep_length = 24;
	const uint8_t measure_node_length = 4;
	const uint8_t hold_measurement_length = 5;
	const uint8_t prediction_rep_length = 21;

	/* shared queue between main task and znp task */
	extern OS_EventID main2znp_queue_id;
	extern OS_EventID znp2main_queue_id;
	const uint8_t message_pending = 'm';

	extern cir_queue main2znp_cir_queue;
	extern cir_queue znp2main_cir_queue;
}

/**
 * @brief 	Init RBPS system,
 * 			- Assign ISR for rbps_keypad.
 * 			- Create queue.
 */
void rbps_init(void);

/** @} */
#endif // RBPS_GLB_H_
