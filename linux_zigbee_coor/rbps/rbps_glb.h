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
#include "stdint.h"

/* definitions */
#define RBPS_DEBUG (1)
#if RBPS_DEBUG
#define RBPS_PRINTF(...) printf(__VA_ARGS__)
#else
#define RBPS_PRINTF(...)
#endif

namespace rbps_ns {
	/* extern global variables */

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

	/* shared queue between main thread (controller) and zigbee thread */
	extern int cc2zb_mq_id;
	extern int zb2cc_mq_id;

	/* shared queue between controller (and zigbee) process and wifi process */
	extern int cc2wi_mq_id;
	extern int wi2cc_mq_id;

	/* message mtext max size */
	const uint8_t mtext_max_size = 3+255;
	/* message type */
	typedef struct mesg_s {
		long mtype;
		char mtext[mtext_max_size];
	} mesg_t;
}

/**
 * @brief 	Init RBPS system,
 * 			- Create queue.
 */
void rbps_init(void);

/** @} */
#endif // RBPS_GLB_H_
