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
#include <cstdint>
#include <cstdio>

/* definitions */
#define RBPS_DEBUG (1)
#if RBPS_DEBUG
#define RBPS_PRINTF(...) printf(__VA_ARGS__)
#else
#define RBPS_PRINTF(...)
#endif

namespace rbps_ns {
	/* lower prehypertension threshold for sys and dias */
	const uint16_t prehypertension_lsys = 135; /* mmHg */
	const uint16_t prehypertension_ldias = 85; /* mmHg */

	/* broad cast id for wifi process */
	const uint16_t wifi_boardcast_mtype = 0xFFFF;

	/* Command id */
	const uint16_t update_node_id = 0x0003; // in
	const uint16_t update_node_rep_id = 0x0016; // out
	const uint16_t detail_info_id = 0x0014; // in
	const uint16_t detail_info_rep_id = 0x0015; // out
	const uint16_t measure_node_id = 0x0004; // in
	const uint16_t hold_measurement_id = 0x0018; // in
	const uint16_t node_report_id = 0x0017; // in
	const uint16_t prediction_rep_id = 0x0010; // out

	const uint16_t scan_node = 0x0001; // in
	const uint16_t scan_node_rep = 0x0002; // out
	const uint16_t detail_blood_pressure = 0x0005; // in
	const uint16_t detail_blood_pressure_rep = 0x0006; // out
	const uint16_t detail_heart_rate = 0x0007; // in
	const uint16_t detail_heart_rate_rep = 0x0008; // out
	const uint16_t detail_height = 0x0009; // in
	const uint16_t detail_height_rep = 0x000A; // out
	const uint16_t detail_weight = 0x000B; // in
	const uint16_t detail_weight_rep = 0x000C; //out
	const uint16_t detail_medical_history = 0x000D; // in
	const uint16_t detail_medical_history_rep = 0x000E; // out
	const uint16_t update_schedule = 0x0013; // out
	const uint16_t get_schedule = 0x0019; // in
	const uint16_t prediction = 0x000F; //in

	const uint8_t update_node_length = 8;
	const uint8_t update_node_rep_length = 1;
	const uint8_t detail_info_length = 4;
	const uint8_t detail_info_rep_length = 24;
	const uint8_t measure_node_length = 4;
	const uint8_t node_report_length = 17;
	const uint8_t hold_measurement_length = 5;
	const uint8_t prediction_rep_length = 21;

	const uint8_t detail_height_rep_length = 5;
	const uint8_t detail_weight_rep_length = 15;
	const uint8_t detail_medical_history_rep_length = 4;
	const uint8_t update_schedule_length = 14;

	const uint8_t status_false = 0;
	const uint8_t status_true = 1;

	/* shared queue between main thread (controller) and zigbee thread */
	extern int cc2zb_mq_id;
	extern int zb2cc_mq_id;

	/* shared queue between controller (and zigbee) process and wifi process */
	extern int cc2wi_mq_id;
	extern int wi2cc_mq_id;

	/* message mtext max size */
	const uint16_t mtext_max_size = 3+255;
	const uint16_t gframe_data_max_size = 255;
	/* message type */
	typedef struct mesg_s {
		long mtype;
		uint8_t mtext[mtext_max_size];
	} mesg_t;

	/* predict type */
	typedef struct predict_data_s {
		bool prehypertension_risk;
		float avg_sys; /* in one month */
		float avg_dias; /* in one month */
		float avg_heart_rate; /* in one month */
		float avg_bmi;

		bool diabetes;
		bool dyslipidemia;
		bool atherosclerosis;
		bool gout;
	} predict_data_t;

	/* timestamp */
	typedef struct timestamp_s {
		uint8_t hour;
		uint8_t min;
		uint8_t day;
		uint8_t month;
		uint16_t year;
	} timestamp_t;

	/* detail blood pressure type */
	typedef struct detail_bp_s {
		float recent_sys;
		float recent_dias;
		timestamp_t recent_time;

		float avg_sys_day;
		float avg_dias_day;

		float peak_sys_day;
		timestamp_t peak_sys_day_time;
		float peak_dias_day;
		timestamp_t peak_dias_day_time;

		float avg_sys_week;
		float avg_dias_week;

		float peak_sys_week;
		timestamp_t peak_sys_week_time;
		float peak_dias_week;
		timestamp_t peak_dias_week_time;

		float avg_sys_month;
		float avg_dias_month;

		float peak_sys_month;
		timestamp_t peak_sys_month_time;
		float peak_dias_month;
		timestamp_t peak_dias_month_time;
	} detail_bp_t;

	/* detail blood pressure type */
	typedef struct detail_hr_s {
		float recent_hr;
		timestamp_t recent_time;

		float avg_hr_day;

		float peak_hr_day;
		timestamp_t peak_hr_day_time;

		float avg_hr_week;

		float peak_hr_week;
		timestamp_t peak_hr_week_time;

		float avg_hr_month;

		float peak_hr_month;
		timestamp_t peak_hr_month_time;
	} detail_hr_t;

	/* detail height type */
	typedef struct detail_height_s {
		uint8_t recent_height;
		timestamp_t recent_time;
	} detail_height_t;

	/* detail height type */
	typedef struct detail_weight_s {
		uint8_t recent_weight[3];
		timestamp_t recent_time[3];

		float avg_weight;
	} detail_weight_t;

	/* current path */
	extern char cur_path[1024];
}

/**
 * @brief 	Init RBPS system,
 * 			- Create queue.
 */
void rbps_init(void);

/** @} */
#endif // RBPS_GLB_H_
