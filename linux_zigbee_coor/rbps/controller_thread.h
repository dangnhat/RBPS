/**
 * @defgroup
 * @brief
 * @ingroup     MainProgramSource
 * @{
 *
 * @file        controller_thread.h
 * @brief       Header file for controller thread.
 *
 * @author      DangNhat Pham-Huu <51002279@stu.hcmut.edu.vn>
 */

#ifndef CONTROLLER_THREAD_H_
#define CONTROLLER_THREAD_H_

#include <cstdint>

/* Debug definition */
#define CTRL_DEBUG (1)

namespace ctrl_ns {
	typedef struct schedule_s {
		bool abs_on = false;
		uint8_t abs_hour;
		uint8_t abs_min;

		bool in_day_on = false;
		uint8_t in_day_hour;
		uint8_t in_day_min;
	} schedule_t;

	typedef struct node_s {
		uint32_t node_id = 0;
		uint32_t patient_id = 0;
		schedule_t sched;
	} node_t;

	const uint16_t max_num_nodes = 128;

	typedef struct cc_info_s {
		uint16_t num_nodes = 0;
		node_t nodes_list[max_num_nodes];
	} cc_info_t;

	const char patient_data_path[] = "../patient_data";
	const char patient_id_list_path[] = "../patient_data/patients_id_list.txt";
	const char basic_info_name[] = "basic_info.txt";
};

/**
 * @brief   controller thread function.
 *
 * @param[in]   pdata
 *
 * @return      NERVER RETURN
 */
void* controller_thread_func(void *pdata);

/** @} */
#endif // CONTROLLER_THREAD_H_
