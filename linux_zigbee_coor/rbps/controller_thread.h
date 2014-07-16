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
		uint8_t abs_day;
		uint8_t abs_month;
		uint16_t abs_year;

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

	extern char patient_data_path[1024];
	extern char patient_id_list_path[1024];
	extern char daily_patient_data[1024];
	const char basic_info_name[] = "basic_info.txt";

	const char patient_data_path_c[] = "/patient_data";
	const char patient_id_list_path_c[] = "/patient_data/patients_id_list.txt";
	const char daily_patient_data_c[] = "/patient_data/%u/%hu-%hu-%u.txt";

	const char patient_data_block[] =
			"Time: %c:%c\n"
			"Blood pressure: %hu/%hu (mmHg)\n"
			"Heart rate: %hu (pulses/min)\n"
			"Weight: %c (kg)\n"
			"Height: %c (cm)\n";

	const char basic_info_block[] =
			"name: %s\n"
			"date of birth: %hu/%hu/%hu\n"
			"diabetes: %s\n"
			"dyslipidemia: %s\n"
			"atherosclerosis: %s\n"
			"gout: %s\n";

	const uint8_t no_data = 0;
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
