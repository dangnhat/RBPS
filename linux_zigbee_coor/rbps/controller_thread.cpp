/**
 * @ingroup     MainProgramSource
 * @{
 *
 * @file        controller_thread.cpp
 * @brief       Implementation of controller thread.
 *
 * @author      DangNhat Pham-Huu <51002279@stu.hcmut.edu.vn>
 *
 * @}
 */

#include <sys/types.h>
#include <sys/msg.h>
#include <cstring>
#include <ctime>
#include "controller_thread.h"
#include "rbps_glb.h"

/* Debug definition */
#if CTRL_DEBUG
#define CTRL_PRINTF(...) printf(__VA_ARGS__)
#else
#define CTRL_PRINTF(...)
#endif

namespace ctrl_ns {
	char patient_data_path[1024];
	char patient_id_list_path[1024];
	char daily_patient_data[1024];
}

using namespace ctrl_ns;

/* CC INFO struct */
cc_info_t cc_info;

/*---------------------- static function prototypes --------------------------*/
/**
 * @brief   Update/or insert data of a node, forward message (broadcast) to
 * 			wifi process if patient id is correct.
 *
 * @param[out]	cc_info,
 * @param[in]	mesg, message from queue.
 */
static void update_node_func(cc_info_t &cc_info, rbps_ns::mesg_t &mesg);

/**
 * @brief   Retrieve detail patient's information and send to node.
 *
 * @param[out]	cc_info,
 * @param[in]	mesg, message from queue.
 */
static void detai_info_node_func(cc_info_t &cc_info, rbps_ns::mesg_t &mesg);

/**
 * @brief   Save data from node, send the prediction to node if node needs prediction,
 * 			Forward data to all mobile phones.
 *
 * @param[out]	cc_info,
 * @param[in]	mesg, message from queue.
 */
static void node_report_node_func(cc_info_t &cc_info, rbps_ns::mesg_t &mesg);

/**
 * @brief   Get the list of nodes and send to queue.
 *
 * @param[out]	cc_info,
 * @param[in]	mesg, message from queue.
 */
static void scan_node_wifi_func(cc_info_t &cc_info, rbps_ns::mesg_t &mesg);

/**
 * @brief   Send patient info to wifi process.
 *
 * @param[out]	cc_info,
 * @param[in]	mesg, message from queue.
 */
static void detail_info_wifi_func(cc_info_t &cc_info, rbps_ns::mesg_t &mesg);

/**
 * @brief   Send measure command to node.
 *
 * @param[out]	cc_info,
 * @param[in]	mesg, message from queue.
 */
static void measure_node_wifi_func(cc_info_t &cc_info, rbps_ns::mesg_t &mesg);

/**
 * @brief   Send blood pressure data to wifi process.
 *
 * @param[out]	cc_info,
 * @param[in]	mesg, message from queue.
 */
static void detail_blood_pressure_wifi_func(cc_info_t &cc_info, rbps_ns::mesg_t &mesg);

/**
 * @brief   Send heart rate data to wifi process.
 *
 * @param[out]	cc_info,
 * @param[in]	mesg, message from queue.
 */
static void detail_heart_rate_wifi_func(cc_info_t &cc_info, rbps_ns::mesg_t &mesg);

/**
 * @brief   Send height data to wifi process.
 *
 * @param[out]	cc_info,
 * @param[in]	mesg, message from queue.
 */
static void detail_height_wifi_func(cc_info_t &cc_info, rbps_ns::mesg_t &mesg);

/**
 * @brief   Send weight data to wifi process.
 *
 * @param[out]	cc_info,
 * @param[in]	mesg, message from queue.
 */
static void detail_weight_wifi_func(cc_info_t &cc_info, rbps_ns::mesg_t &mesg);

/**
 * @brief   Send medical history data to wifi process.
 *
 * @param[out]	cc_info,
 * @param[in]	mesg, message from queue.
 */
static void detail_medical_history_wifi_func(cc_info_t &cc_info, rbps_ns::mesg_t mesg);

/**
 * @brief   Send prediction data to wifi process.
 *
 * @param[out]	cc_info,
 * @param[in]	mesg, message from queue.
 */
static void prediction_wifi_func(cc_info_t &cc_info, rbps_ns::mesg_t &mesg);

/**
 * @brief   Send schedule data to wifi process.
 *
 * @param[out]	cc_info,
 * @param[in]	mesg, message from queue.
 */
static void get_schedule_wifi_func(cc_info_t &cc_info, rbps_ns::mesg_t mesg);

/**
 * @brief   Update node's schedule and broad cast to all wifi processes.
 *
 * @param[out]	cc_info,
 * @param[in]	mesg, message from queue.
 */
static void update_schedule_wifi_func(cc_info_t &cc_info, rbps_ns::mesg_t mesg);

static uint16_t buffer_to_uint16(uint8_t *buffer); // LSByte first
static uint32_t buffer_to_uint32(uint8_t *buffer); // LSByte first
static void uint16_to_buffer(uint16_t data, uint8_t* buffer); // LSByte fisrt
static void uint32_to_buffer(uint32_t data, uint8_t* buffer); // LSByte fisrt
static float buffer_to_float(uint8_t *buffer); // dec-2byte, frac-2byte (2 digits)
static void float_to_buffer(float data, uint8_t *buffer); // dec-2byte, frac-2byte (2 digits)
static void timestamp_to_buffer(rbps_ns::timestamp_t timestamp, uint8_t *buffer);
static inline bool is_leap_year(uint16_t year) {
	return ((year%4 == 0) ? ((year%100 == 0) ? ((year%400 == 0) ? true : false) : true): false);
}

/**
 * @brief   Find address of a node.
 *
 * @param[in]	cc_info,
 * @param[in]	node_id,
 *
 * @return		address of node which has node_id. NULL if this node doesn't exist.
 */
static node_t* find_node(cc_info_t &cc_info, uint32_t node_id);

/**
 * @brief   Forward a general frame to another queue.
 *
 * @param[in]	queue_id,
 * @param[in]	mtype,
 * @param[in]	len,
 * @param[in]	cmd_id,
 * @param[in]	data_p
 */
static void forward_gframe(int queue_id, long mtype,
		uint8_t len, uint16_t cmd_id, uint8_t* data_p);

/**
 * @brief   Get basic info (name and date of birth) from file.
 *
 * @param[in]	patient_id,
 * @param[out]	name,
 * @param[out]	date_of_birth, day, month, year
 * @param[out]	diabetes,
 * @param[out]	dyslipidemia,
 * @param[out]	atherosclerosis,
 * @param[out]	gout,
 *
 * @return		0 if successful, otherwise, error.
 */
static int8_t get_basic_info(uint32_t patient_id, char* name, uint16_t* date_of_birth,
		bool &diabetes, bool &dyslipidemia, bool &atherosclerosis, bool &gout);

/**
 * @brief   Prehypertension prediction.
 *
 * @param[in]	patient_id,
 * @param[out]	predict_data,
 */
static void prehypertension_predict(uint32_t patient_id, rbps_ns::predict_data_t &predict_dt);

/**
 * @brief   get detail blood pressure information.
 *
 * @param[in]	patient_id,
 * @param[out]	detail_bp,
 */
static void get_detail_blood_pressure(uint32_t patient_id, rbps_ns::detail_bp_t &detail_bp);

/**
 * @brief   get detail heart rate information.
 *
 * @param[in]	patient_id,
 * @param[out]	detail_hr,
 */
static void get_detail_heart_rate(uint32_t patient_id, rbps_ns::detail_hr_t &detail_hr);

/**
 * @brief   get most recent height information. (in one month)
 *
 * @param[in]	patient_id,
 * @param[out]	detail_height,
 */
static void get_detail_height(uint32_t patient_id, rbps_ns::detail_height_t &detail_height);

/**
 * @brief   get 3 most recent weight information. (in one month)
 *
 * @param[in]	patient_id,
 * @param[out]	detail_weight,
 */
static void get_detail_weight(uint32_t patient_id, rbps_ns::detail_weight_t &detail_weight);

/**
 * @brief   Decrease one date.
 *
 * @param[in, out]	day, range from 1 - 31
 * @param[in, out]	month, range from 1 - 12
 * @param[in, out]	year,
 */
static void decrease_date(uint8_t &day, uint8_t &month, uint16_t &year);

/**
 * @brief   Decrease one month.
 *
 * @param[in, out]	month, range from 1 - 12
 * @param[in, out]	year,
 */
static void decrease_month(uint8_t &month, uint16_t &year);

/*----------------------------------------------------------------------------*/
void* controller_thread_func(void *pdata) {
	int ret_val;
	rbps_ns::mesg_t mesg;
	uint16_t cmd_id;

	time_t cur_time;
	time_t old_time;
	tm cur_time_s;

	time(&old_time);
	time(&cur_time);

	node_t *node_p;
	uint8_t data_buffer[rbps_ns::mtext_max_size - 3];

	/* set data paths */
	strcpy(patient_data_path, rbps_ns::cur_path);
	strcat(patient_data_path, patient_data_path_c);
	printf("ctrl: patient_data_path: %s\n", patient_data_path);

	strcpy(patient_id_list_path, rbps_ns::cur_path);
	strcat(patient_id_list_path, patient_id_list_path_c);
	printf("ctrl: patient_id_list_path: %s\n", patient_id_list_path);

	strcpy(daily_patient_data, rbps_ns::cur_path);
	strcat(daily_patient_data, daily_patient_data_c);
	printf("ctrl: daily_patient_data: %s\n", daily_patient_data);

	/* test */
//	cc_info.num_nodes = 1;
//	cc_info.nodes_list[0].node_id = 1;
//	cc_info.nodes_list[0].patient_id = 1;
//	uint32_to_buffer(1, &mesg.mtext[3]);
//	node_report_node_func(cc_info, mesg);

	while (1) {
		/* check data from zigbee thread */
		ret_val = msgrcv(rbps_ns::zb2cc_mq_id, &mesg, rbps_ns::mtext_max_size, 0, IPC_NOWAIT);
		if (ret_val > 0) {
			CTRL_PRINTF("ctrl: New message from zigbee, src addr: %x\n", (uint16_t)mesg.mtype);

			cmd_id = buffer_to_uint16(&mesg.mtext[1]);
			CTRL_PRINTF("ctrl: len %d, cmd %x\n", mesg.mtext[0], cmd_id);

			switch (cmd_id) {
			case rbps_ns::update_node_id:
				CTRL_PRINTF("ctrl: update_node\n");
				update_node_func(cc_info, mesg);
				break;
			case rbps_ns::detail_info_id:
				CTRL_PRINTF("ctrl: detail_info\n");
				detai_info_node_func(cc_info, mesg);
				break;
			case rbps_ns::node_report_id:
				CTRL_PRINTF("ctrl: node_report\n");
				node_report_node_func(cc_info, mesg);
				break;
			case rbps_ns::hold_measurement_id:
				CTRL_PRINTF("ctrl: hold messure, do nothing\n");
				break;
			default:
				CTRL_PRINTF("ctrl: unknown cmd from node %hu\n", cmd_id);
				break;
			}// end switch
		}// end if, data from zigbee thread;

		/* check data from wifi process */
		ret_val = msgrcv(rbps_ns::wi2cc_mq_id, &mesg, rbps_ns::mtext_max_size, 0, IPC_NOWAIT);
		if (ret_val > 0) {
			CTRL_PRINTF("ctrl: new message from wifi process, src mtype: %lu\n", mesg.mtype);

			cmd_id = buffer_to_uint16(&mesg.mtext[1]);
			CTRL_PRINTF("ctrl: len %d, cmd %x\n", mesg.mtext[0], cmd_id);

			switch (cmd_id) {
			case rbps_ns::scan_node:
				CTRL_PRINTF("ctrl: scan_node\n");
				scan_node_wifi_func(cc_info, mesg);
				break;
			case rbps_ns::detail_info_id:
				CTRL_PRINTF("ctrl: detail_info\n");
				detail_info_wifi_func(cc_info, mesg);
				break;
			case rbps_ns::measure_node_id:
				CTRL_PRINTF("ctrl: measure_node\n");
				measure_node_wifi_func(cc_info, mesg);
				break;
			case rbps_ns::detail_blood_pressure:
				CTRL_PRINTF("ctrl: detail_blood_pressure\n");
				detail_blood_pressure_wifi_func(cc_info, mesg);
				break;
			case rbps_ns::detail_heart_rate:
				CTRL_PRINTF("ctrl: detail_heart_rate\n");
				detail_heart_rate_wifi_func(cc_info, mesg);
				break;
			case rbps_ns::detail_height:
				CTRL_PRINTF("ctrl: detail_height\n");
				detail_height_wifi_func(cc_info, mesg);
				break;
			case rbps_ns::detail_weight:
				CTRL_PRINTF("ctrl: detail_weight\n");
				detail_weight_wifi_func(cc_info, mesg);
				break;
			case rbps_ns::detail_medical_history:
				CTRL_PRINTF("ctrl: detail med his\n");
				detail_medical_history_wifi_func(cc_info, mesg);
				break;
			case rbps_ns::prediction:
				CTRL_PRINTF("ctrl: prediction\n");
				prediction_wifi_func(cc_info, mesg);
				break;
			case rbps_ns::get_schedule:
				CTRL_PRINTF("ctrl: get schedule\n");
				get_schedule_wifi_func(cc_info, mesg);
				break;
			case rbps_ns::update_schedule:
				CTRL_PRINTF("ctrl: update schedule\n");
				update_schedule_wifi_func(cc_info, mesg);
				break;
			default:
				CTRL_PRINTF("ctrl: uk cmd id %d\n", cmd_id);
				break;
			}
		}// end if, data from wifi process.

		/* check schedule */
		time(&cur_time);
		if (difftime(cur_time, old_time) > 60) {
			old_time = cur_time;

			CTRL_PRINTF("ctrl: trigger schedule\n");

			localtime_r(&old_time, &cur_time_s);

			CTRL_PRINTF("ctrl: curtime: %dh%d, %d/%d/%d\n",
					cur_time_s.tm_hour, cur_time_s.tm_min,
					cur_time_s.tm_mday, cur_time_s.tm_mon+1, cur_time_s.tm_year + 1900);
			for (uint16_t count = 0; count < cc_info.num_nodes; count++) {
				node_p = &cc_info.nodes_list[count];
				if (node_p->sched.abs_on) {
					if ((node_p->sched.abs_hour == cur_time_s.tm_hour) &&
							(node_p->sched.abs_min == cur_time_s.tm_min) &&
							(node_p->sched.abs_day == cur_time_s.tm_mday) &&
							(node_p->sched.abs_month == cur_time_s.tm_mon+1) &&
							(node_p->sched.abs_year == cur_time_s.tm_year+1900)) {
						/* forward measure node command to node id */
						uint32_to_buffer(node_p->node_id, data_buffer);
						forward_gframe(rbps_ns::cc2zb_mq_id, (uint16_t)node_p->node_id,
								rbps_ns::measure_node_length, rbps_ns::measure_node_id,
								data_buffer);
						CTRL_PRINTF("ctrl: sent measure node (abs) to node %hu\n",
								(uint16_t)node_p->node_id);
					}
				}// end abs on

				if (node_p->sched.in_day_on) {
					if ((node_p->sched.in_day_hour == cur_time_s.tm_hour) &&
							(node_p->sched.in_day_min == cur_time_s.tm_min)) {
						/* forward measure node command to node id */
						uint32_to_buffer(node_p->node_id, data_buffer);
						forward_gframe(rbps_ns::cc2zb_mq_id, (uint16_t)node_p->node_id,
								rbps_ns::measure_node_length, rbps_ns::measure_node_id,
								data_buffer);
						CTRL_PRINTF("ctrl: sent measure node (in_day) to node %hu\n",
								(uint16_t)node_p->node_id);
					}
				}// end in_day on

			}// end for all nodes.
		}// end if diff time > 60

	}// end while (1)

	return 0;
}

/*----------------------------- Static functions -----------------------------*/
static void update_node_func(cc_info_t &cc_info, rbps_ns::mesg_t &mesg) {
	uint32_t node_id;
	uint32_t patient_id;
	uint8_t* data_buffer = &mesg.mtext[3];

	FILE *fp;
	uint32_t patient_id_file;
	bool found_patient_id = false;

	node_t* node_p;
	uint8_t ret_data_buffer[rbps_ns::gframe_data_max_size];

	/* extract node id and patient id */
	node_id = buffer_to_uint32(data_buffer);
	patient_id = buffer_to_uint32(data_buffer + 4);

	if (patient_id != 0) {
		CTRL_PRINTF("ctrl: unf: patient_id != 0, check patient_id\n");

		/* find patient id in patient id list */
		fp = fopen(patient_id_list_path, "r");
		if (fp == NULL) {
			CTRL_PRINTF("ctrl: unf: can't open %s\n", patient_id_list_path);
			return;
		}

		found_patient_id = false;
		while (!feof(fp)) {
			fscanf(fp, "%u", &patient_id_file);

			if (patient_id_file == patient_id) {
				found_patient_id = true;
				break;
			}
		}

		fclose(fp);

		if (found_patient_id == false) {
			CTRL_PRINTF("ctrl: unf: can't find patient_id %d, ret\n", patient_id);

			/* send incorrect update_node_rep to zigbee thread */
			ret_data_buffer[0] = rbps_ns::status_false;
			forward_gframe(rbps_ns::cc2zb_mq_id, mesg.mtype, rbps_ns::update_node_rep_length,
					rbps_ns::update_node_rep_id, ret_data_buffer);
			CTRL_PRINTF("ctrl: unf: sent incorrect update_node_rep to zigbee (dest addr %x)",
					(uint16_t)mesg.mtype);

			return;
		}
		CTRL_PRINTF("ctrl: unf: found patient_id %d\n", patient_id);

		/* send correct update_node_rep to zigbee thread */
		ret_data_buffer[0] = rbps_ns::status_true;
		forward_gframe(rbps_ns::cc2zb_mq_id, mesg.mtype, rbps_ns::update_node_rep_length,
				rbps_ns::update_node_rep_id, ret_data_buffer);
		CTRL_PRINTF("ctrl: unf: sent correct update_node_rep to zigbee (dest addr %x)\n",
				(uint16_t)mesg.mtype);
	}

	/* update node data */
	node_p = find_node(cc_info, node_id);

	if (node_p != NULL) {
		CTRL_PRINTF("ctrl: unf: node_id %x exists, update new patient_id %d\n",
				node_id, patient_id);
		node_p->patient_id = patient_id;
	}
	else {
		CTRL_PRINTF("ctrl: unf: node_id %x doesn't exists, insert new node with pa_id %d\n",
				node_id, patient_id);
		if (cc_info.num_nodes >= max_num_nodes) {
			CTRL_PRINTF("ctrl: unf: max_num_nodes %d is reached\n", max_num_nodes);
			return;
		}

		cc_info.nodes_list[cc_info.num_nodes].node_id = node_id;
		cc_info.nodes_list[cc_info.num_nodes].patient_id = patient_id;
		cc_info.num_nodes++;
		CTRL_PRINTF("ctrl: unf: updated new node\n");
	}

	/* forward frame to wifi process */
	forward_gframe(rbps_ns::cc2wi_mq_id, rbps_ns::wifi_boardcast_mtype,
			rbps_ns::update_node_length, rbps_ns::update_node_id, &mesg.mtext[3]);
	CTRL_PRINTF("ctrl: unf: forward update node to wifi process with mtype %x\n",
			rbps_ns::wifi_boardcast_mtype);

	return;
}

/*----------------------------------------------------------------------------*/
static void detai_info_node_func(cc_info_t &cc_info, rbps_ns::mesg_t &mesg) {
	uint32_t patient_id;
	char name[128] = "No data";
	uint16_t date_of_birth[3] = {0, 0, 0};
	uint8_t data_buffer[rbps_ns::gframe_data_max_size];

	bool dummy1, dummy2, dummy3, dummy4;

	/* extract patient_id */
	patient_id = buffer_to_uint32(&mesg.mtext[3]);

	/* do nothing if patient_id = 0 */
	if (patient_id == 0) {
		CTRL_PRINTF("ctrl: dinf: patient_id %d, return\n", patient_id);
		return;
	}

	/* get name and date of birth */
	get_basic_info(patient_id, name, date_of_birth, dummy1, dummy2, dummy3, dummy4);

	/* send data to node */
	data_buffer[0] = (uint8_t)date_of_birth[0];
	data_buffer[1] = (uint8_t)date_of_birth[1];
	data_buffer[2] = (uint8_t)date_of_birth[2];
	data_buffer[3] = (uint8_t)(date_of_birth[2] >> 8);
	memcpy(&data_buffer[4], name, 20);
	forward_gframe(rbps_ns::cc2zb_mq_id, mesg.mtype, rbps_ns::detail_info_rep_length,
			rbps_ns::detail_info_rep_id, data_buffer);
	CTRL_PRINTF("ctrl: dinf: sent detail_info_rep to zigbee thread\n");
}

/*----------------------------------------------------------------------------*/
static void node_report_node_func(cc_info_t &cc_info, rbps_ns::mesg_t &mesg) {
	uint32_t node_id;
	uint16_t sys, sys_dummy;
	uint16_t dias, dias_dummy;
	uint16_t heart_rate, heart_rate_dummy;
	uint8_t hour, min;
	uint8_t day, month;
	uint16_t year;
	uint8_t need_predict;

	uint8_t *hour_pos, *min_pos, *day_pos, *month_pos;
	uint8_t *year_firstpos;

	time_t cur_time;
	tm br_time;

	uint8_t* buffer_p = &mesg.mtext[3];

	node_t* node_p;
	uint32_t patient_id;

	FILE* fp;
	char filename[256];

	rbps_ns::mesg_t zigbee_mesg;

	rbps_ns::predict_data_t predict_s;
	rbps_ns::mesg_t wifi_mesg;
	uint8_t *zigbee_data_p;

	/* extract data from frame */
	node_id = buffer_to_uint32(buffer_p);
	buffer_p += 4;

	sys = buffer_to_uint16(buffer_p);
	buffer_p += 2;
	sys_dummy = buffer_to_uint16(buffer_p);
	buffer_p += 2;

	dias = buffer_to_uint16(buffer_p);
	buffer_p += 2;
	dias_dummy = buffer_to_uint16(buffer_p);
	buffer_p += 2;

	heart_rate = buffer_to_uint16(buffer_p);
	buffer_p += 2;
	heart_rate_dummy = buffer_to_uint16(buffer_p);
	buffer_p += 2;

	hour = *buffer_p;
	hour_pos = buffer_p;
	buffer_p++;

	min = *buffer_p;
	min_pos = buffer_p;
	buffer_p++;

	day = *buffer_p;
	day_pos = buffer_p;
	buffer_p++;

	month = *buffer_p;
	month_pos = buffer_p;
	buffer_p++;

	year = buffer_to_uint16(buffer_p);
	year_firstpos = buffer_p;
	buffer_p += 2;

	need_predict = *buffer_p;

	/* get timestamp */
	time(&cur_time);
	localtime_r(&cur_time, &br_time);

	hour = (uint8_t)br_time.tm_hour;
	min = (uint8_t)br_time.tm_min;
	day = (uint8_t)br_time.tm_mday;
	month = (uint8_t)br_time.tm_mon + 1; // 1 -> 12
	year = (uint16_t)br_time.tm_year + 1900;

	CTRL_PRINTF("ctrl: nr: Data extracted, node %u, sys %u, dias %u, hr %hu, hour %hu, min %hu,"
			"day %hu, month %hu, year %u\n",
			node_id, sys, dias, heart_rate, hour, min, day, month, year);

	/* find patient_id */
	node_p = find_node(cc_info, node_id);
	if (node_p == NULL) {
		CTRL_PRINTF("ctrl: nr: Can't find node %u, ret!\n", node_id);
		return;
	}
	else {
		patient_id = node_p->patient_id;
		CTRL_PRINTF("ctrl: nr: found patient_id %u\n", patient_id);
		if (patient_id == 0) {
			CTRL_PRINTF("ctrl: nr: patient_id = 0, ret!\n");
			return;
		}
	}

	/* save data to file */
	sprintf(filename, daily_patient_data, patient_id, day, month, year);
	fp = fopen(filename, "a");
	if (fp == NULL) {
		CTRL_PRINTF("ctrl: nr: Can't open %s, ret!\n", filename);
		return;
	}

	fprintf(fp, patient_data_block,
			hour, min,
			sys, dias, heart_rate,
			no_data, no_data);

	/* close file */
	fclose(fp);

	CTRL_PRINTF("ctrl: nr: saved data to file %s\n", filename);

	/* forward frame to wifi process */
	*hour_pos = hour;
	*min_pos = min;
	*day_pos = day;
	*month_pos = month;
	uint16_to_buffer(year, year_firstpos);

	memcpy(wifi_mesg.mtext, mesg.mtext, rbps_ns::node_report_length + 3); // fix
	forward_gframe(rbps_ns::cc2wi_mq_id, rbps_ns::wifi_boardcast_mtype,
			rbps_ns::node_report_length, rbps_ns::node_report_id, &wifi_mesg.mtext[3]);
	CTRL_PRINTF("ctrl: nr: forwarded to wifi process\n");

	/* check prediction */
	if (need_predict == rbps_ns::status_true) {
		CTRL_PRINTF("ctrl: nr: node need predict\n");
		prehypertension_predict(patient_id, predict_s);
		CTRL_PRINTF("ctrl: nr: predicted\n");

		/* send predict data to node */
		zigbee_mesg.mtype = mesg.mtype;
		zigbee_mesg.mtext[0] = rbps_ns::prediction_rep_length;
		zigbee_mesg.mtext[1] = (uint8_t)rbps_ns::prediction_rep_id;
		zigbee_mesg.mtext[2] = (uint8_t)(rbps_ns::prediction_rep_id >> 8);
		zigbee_mesg.mtext[3] = predict_s.prehypertension_risk ? 1 : 0;

		zigbee_data_p = &zigbee_mesg.mtext[4];
		float_to_buffer(predict_s.avg_sys, zigbee_data_p);
		zigbee_data_p += 4;

		float_to_buffer(predict_s.avg_dias, zigbee_data_p);
		zigbee_data_p += 4;

		float_to_buffer(predict_s.avg_heart_rate, zigbee_data_p);
		zigbee_data_p += 4;

		float_to_buffer(predict_s.avg_bmi, zigbee_data_p);
		zigbee_data_p += 4;

		*zigbee_data_p = predict_s.diabetes ? 1 : 0;
		zigbee_data_p++;
		*zigbee_data_p = predict_s.dyslipidemia ? 1 : 0;
		zigbee_data_p++;
		*zigbee_data_p = predict_s.atherosclerosis ? 1 : 0;
		zigbee_data_p++;
		*zigbee_data_p = predict_s.gout ? 1 : 0;
		zigbee_data_p++;

		forward_gframe(rbps_ns::cc2zb_mq_id, mesg.mtype, rbps_ns::prediction_rep_length,
				rbps_ns::prediction_rep_id, &zigbee_mesg.mtext[3]);
		CTRL_PRINTF("ctrl: nr: sent predict_rep to %x\n", (uint16_t)mesg.mtype);
	}
	else {
		CTRL_PRINTF("ctrl: nr: node don't need predict\n");
	}
}

/*----------------------------------------------------------------------------*/
static uint16_t buffer_to_uint16(uint8_t *buffer) {
	uint16_t ret_val;

	ret_val = (*buffer) | (*(buffer + 1) << 8);

	return ret_val;
}

/*----------------------------------------------------------------------------*/
static uint32_t buffer_to_uint32(uint8_t *buffer) {
	uint32_t ret_val;

	ret_val = (*buffer) | (*(buffer + 1) << 8) | (*(buffer + 2) << 16) | (*(buffer + 3) << 24);

	return ret_val;
}

/*----------------------------------------------------------------------------*/
static void uint16_to_buffer(uint16_t data, uint8_t* buffer) {
	*buffer = (uint8_t)data;
	*(buffer+1) = (uint8_t)(data >> 8);
}

/*----------------------------------------------------------------------------*/
static void uint32_to_buffer(uint32_t data, uint8_t* buffer) {
	*buffer = (uint8_t)data;
	*(buffer+1) = (uint8_t)(data >> 8);
	*(buffer+2) = (uint8_t)(data >> 16);
	*(buffer+3) = (uint8_t)(data >> 24);
}

/*----------------------------------------------------------------------------*/
static float buffer_to_float(uint8_t *buffer) {
	float ret_val;
	uint16_t dec;
	uint16_t frac;

	dec = buffer_to_uint16(buffer);
	frac = buffer_to_uint16(buffer + 2);

	ret_val = (float)dec;
	ret_val += ((float)frac)/100;

	return ret_val;
}

/*----------------------------------------------------------------------------*/
static void float_to_buffer(float data, uint8_t *buffer) {
	uint16_t dec, frac;

	dec = (uint16_t) data;
	frac = ((uint16_t) (data * 100)) % 100;

	uint16_to_buffer(dec, buffer);
	uint16_to_buffer(frac, buffer+2);
}

/*----------------------------------------------------------------------------*/
static void timestamp_to_buffer(rbps_ns::timestamp_t timestamp, uint8_t *buffer) {
	*buffer = timestamp.hour;
	*(buffer + 1) = timestamp.min;
	*(buffer + 2) = timestamp.day;
	*(buffer + 3) = timestamp.month;
	uint16_to_buffer(timestamp.year, buffer+4);
}

/*----------------------------------------------------------------------------*/
static node_t* find_node(cc_info_t &cc_info, uint32_t node_id) {
	uint16_t count;
	node_t* retptr = NULL;

	for (count = 0; count < cc_info.num_nodes; count++) {
		if (cc_info.nodes_list[count].node_id == node_id) {
			retptr = &cc_info.nodes_list[count];
			break;
		}
	}

	return retptr;
}

/*----------------------------------------------------------------------------*/
static void forward_gframe(int queue_id, long mtype,
		uint8_t len, uint16_t cmd_id, uint8_t* data_p) {
	rbps_ns::mesg_t a_mesg;

	/* pack data */
	a_mesg.mtype = mtype;
	a_mesg.mtext[0] = len;
	a_mesg.mtext[1] = (uint8_t)cmd_id;
	a_mesg.mtext[2] = (uint8_t)(cmd_id >> 8);
	memcpy(&a_mesg.mtext[3], data_p, len);

	/* send data */
	if (msgsnd(queue_id, (const void*) &a_mesg, len+3, 0) == 0) {
		CTRL_PRINTF("ctrl: fgf: sent data to queue %d\n", queue_id);
	}
	else {
		CTRL_PRINTF("ctrl: fgf: can't send data to queue %d\n", queue_id);
	}
}

/*----------------------------------------------------------------------------*/
static int8_t get_basic_info(uint32_t patient_id, char* name, uint16_t* date_of_birth,
		bool &diabetes, bool &dyslipidemia, bool &atherosclerosis, bool &gout) {
	int8_t ret_val;
	char basic_info_path[128];
	FILE* fp;
	char diabetes_str[10];
	char dyslipidemia_str[10];
	char atherosclerosis_str[10];
	char gout_str[10];

	/* retrieve data */
	sprintf(basic_info_path, "%s/%d/%s", patient_data_path, patient_id, basic_info_name);
	fp = fopen(basic_info_path, "r");
	if (fp == NULL) {
		CTRL_PRINTF("ctrl: gbi: Can't open %s\n", basic_info_path);
		return -1;
	}

	/* get basic info data */
	fscanf(fp, basic_info_block,
			name, &date_of_birth[0], &date_of_birth[1], &date_of_birth[2],
			diabetes_str, dyslipidemia_str, atherosclerosis_str, gout_str);

	(strcmp(diabetes_str, "no") == 0) ? diabetes = false : diabetes = true;
	(strcmp(dyslipidemia_str, "no") == 0) ? dyslipidemia = false : dyslipidemia = true;
	(strcmp(atherosclerosis_str, "no") == 0) ? atherosclerosis = false : atherosclerosis = true;
	(strcmp(gout_str, "no") == 0) ? gout = false : gout = true;

	/* close file */
	fclose(fp);

	return 0;
}

/*----------------------------------------------------------------------------*/
static void decrease_date(uint8_t &day, uint8_t &month, uint16_t &year) {

	if (day != 1) {
		day--;
		return;
	}
	else {
		month = (month -1 +12 -1) % 12 + 1;

		switch (month) {
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
			day = 31;
			break;

		case 12:
			day = 31;
			year--;
			break;

		case 4:
		case 6:
		case 9:
		case 11:
			day = 30;
			break;

		case 2:
			if (is_leap_year(year)) {
				day = 29;
			}
			else {
				day = 28;
			}
			break;

		default:
			CTRL_PRINTF("ctrl: -d: unexpected month %hu\n", month);
			break;
		}
	}

}

static void decrease_month(uint8_t &month, uint16_t &year) {
	if (month != 1) {
		month--;
	}
	else {
		month = 12;
		year--;
	}
}

/*----------------------------------------------------------------------------*/
static void get_detail_blood_pressure(uint32_t patient_id, rbps_ns::detail_bp_t &detail_bp) {
	time_t cur_time;
	tm br_time;

	uint8_t day_count, day_dest, wday;
	uint8_t month_count, month_dest;
	uint16_t year_count, year_dest;

	FILE *fp;
	char filename[256];

	/* patient data block */
	uint16_t hour, min;
	uint16_t sys, dias;
	uint16_t heartrate;
	uint16_t weight;
	uint16_t height;

	/* continue finding flags */
	bool cont_recent_flag = true;
	bool found_recent_flag = false;

	bool cont_day_flag = true;
	uint8_t avg_day_count = 0;

	bool cont_week_flag = true;
	uint8_t avg_week_count = 0;

	uint8_t avg_month_count = 0;

	/* get current time */
	time(&cur_time);
	localtime_r(&cur_time, &br_time);

	/* find blood pressure data */
	day_count = br_time.tm_mday;
	month_count = br_time.tm_mon + 1;
	year_count = br_time.tm_year + 1900;

	wday = br_time.tm_wday;

	day_dest = day_count;
	month_dest = month_count;
	year_dest = year_count;
	decrease_month(month_dest, year_dest);

	/* init value of detail_bp */
	memset(&detail_bp, 0, sizeof(rbps_ns::detail_bp_t));

	while ((day_count != day_dest) || (month_count != month_dest) || (year_count != year_dest)) {
		/* make filename */
		sprintf(filename, daily_patient_data, patient_id, day_count, month_count, year_count);
		/* open file */
		fp = fopen(filename, "r");
		if (fp != NULL) {

			while (!feof(fp)) {
				/* a block of data */
				fscanf(fp, patient_data_block,
						&hour, &min,
						&sys, &dias, &heartrate,
						&weight, &height);

				if (sys > 0 && dias > 0) {
					/* for recent */
					if (cont_recent_flag) {
						detail_bp.recent_sys = (float) sys;
						detail_bp.recent_dias = (float) dias;
						detail_bp.recent_time.hour = hour;
						detail_bp.recent_time.min = min;
						detail_bp.recent_time.day = day_count;
						detail_bp.recent_time.month = month_count;
						detail_bp.recent_time.year = year_count;

						found_recent_flag = true;
					} // end if

					/* for avg and peak day */
					if (cont_day_flag) {
						detail_bp.avg_sys_day += (float)sys; // sum
						detail_bp.avg_dias_day += (float)dias; // sum
						avg_day_count++;

						if (((float)sys) > detail_bp.peak_sys_day) {
							detail_bp.peak_sys_day = (float)sys;
							detail_bp.peak_sys_day_time.hour = hour;
							detail_bp.peak_sys_day_time.min = min;
						}
						if (((float)dias) > detail_bp.peak_dias_day) {
							detail_bp.peak_dias_day = (float)dias;
							detail_bp.peak_sys_day_time.hour = hour;
							detail_bp.peak_sys_day_time.min = min;
						}
					} // end if

					/* for avg and peak week */
					if (cont_week_flag){
						detail_bp.avg_sys_week += (float)sys; // sum
						detail_bp.avg_dias_week += (float)dias; // sum
						avg_week_count++;

						if (((float)sys) > detail_bp.peak_sys_week) {
							detail_bp.peak_sys_week = (float)sys;
							detail_bp.peak_sys_week_time.hour = hour;
							detail_bp.peak_sys_week_time.min = min;
							detail_bp.peak_sys_week_time.day = day_count;
							detail_bp.peak_sys_week_time.month = month_count;
							detail_bp.peak_sys_week_time.year = year_count;
						}
						if (((float)dias) > detail_bp.peak_dias_week) {
							detail_bp.peak_dias_week = (float)dias;
							detail_bp.peak_dias_week_time.hour = hour;
							detail_bp.peak_dias_week_time.min = min;
							detail_bp.peak_dias_week_time.day = day_count;
							detail_bp.peak_dias_week_time.month = month_count;
							detail_bp.peak_dias_week_time.year = year_count;
						}
					}// end if week

					/* for avg and peak month */
					detail_bp.avg_sys_month += (float)sys; // sum
					detail_bp.avg_dias_month += (float)dias; // sum
					avg_month_count++;

					if (((float)sys) > detail_bp.peak_sys_month) {
						detail_bp.peak_sys_month = (float)sys;
						detail_bp.peak_sys_month_time.hour = hour;
						detail_bp.peak_sys_month_time.min = min;
						detail_bp.peak_sys_month_time.day = day_count;
						detail_bp.peak_sys_month_time.month = month_count;
						detail_bp.peak_sys_month_time.year = year_count;
					}
					if (((float)dias) > detail_bp.peak_dias_month) {
						detail_bp.peak_dias_month = (float)dias;
						detail_bp.peak_dias_month_time.hour = hour;
						detail_bp.peak_dias_month_time.min = min;
						detail_bp.peak_dias_month_time.day = day_count;
						detail_bp.peak_dias_month_time.month = month_count;
						detail_bp.peak_dias_month_time.year = year_count;
					}

				}// end if correct data

			}// end file

			/* close file */
			fclose(fp);
		}

		/* set flags */
		if (found_recent_flag == true) {
			cont_recent_flag = false;
		}

		cont_day_flag = false;

		if (wday == 0) {
			cont_week_flag = false;
		}
		else {
			wday--;
		}

		/* next date */
		decrease_date(day_count, month_count, year_count);
	}// end finding

	/* cal avgs */
	if (avg_day_count > 0) {
		detail_bp.avg_sys_day = detail_bp.avg_sys_day / avg_day_count;
		detail_bp.avg_dias_day = detail_bp.avg_dias_day / avg_day_count;
	}

	if (avg_week_count > 0) {
		detail_bp.avg_sys_week = detail_bp.avg_sys_week / avg_week_count;
		detail_bp.avg_dias_week = detail_bp.avg_dias_week / avg_week_count;
	}

	if (avg_month_count > 0) {
		detail_bp.avg_sys_month = detail_bp.avg_sys_month / avg_month_count;
		detail_bp.avg_dias_month = detail_bp.avg_dias_month / avg_month_count;
	}
}

/*----------------------------------------------------------------------------*/
static void get_detail_heart_rate(uint32_t patient_id, rbps_ns::detail_hr_t &detail_hr) {
	time_t cur_time;
	tm br_time;

	uint8_t day_count, day_dest, wday;
	uint8_t month_count, month_dest;
	uint16_t year_count, year_dest;

	FILE *fp;
	char filename[256];

	/* patient data block */
	uint16_t hour, min;
	uint16_t sys_dummy, dias_dummy;
	uint16_t heartrate;
	uint16_t weight;
	uint16_t height;

	/* continue finding flags */
	bool cont_recent_flag = true;
	bool found_recent_flag = false;

	bool cont_day_flag = true;
	uint8_t avg_day_count = 0;

	bool cont_week_flag = true;
	uint8_t avg_week_count = 0;

	uint8_t avg_month_count = 0;

	/* get current time */
	time(&cur_time);
	localtime_r(&cur_time, &br_time);

	/* find blood pressure data */
	day_count = br_time.tm_mday;
	month_count = br_time.tm_mon + 1;
	year_count = br_time.tm_year + 1900;

	wday = br_time.tm_wday;

	day_dest = day_count;
	month_dest = month_count;
	year_dest = year_count;
	decrease_month(month_dest, year_dest);

	/* init value of detail_bp */
	memset(&detail_hr, 0, sizeof(rbps_ns::detail_hr_t));

	while ((day_count != day_dest) || (month_count != month_dest) || (year_count != year_dest)) {
		/* make filename */
		sprintf(filename, daily_patient_data, patient_id, day_count, month_count, year_count);
		/* open file */
		fp = fopen(filename, "r");
		if (fp != NULL) {

			while (!feof(fp)) {
				/* a block of data */
				fscanf(fp, patient_data_block,
						&hour, &min,
						&sys_dummy, &dias_dummy, &heartrate,
						&weight, &height);

				if (heartrate > 0) {
					/* for recent */
					if (cont_recent_flag) {
						detail_hr.recent_hr = (float) heartrate;
						detail_hr.recent_time.hour = hour;
						detail_hr.recent_time.min = min;
						detail_hr.recent_time.day = day_count;
						detail_hr.recent_time.month = month_count;
						detail_hr.recent_time.year = year_count;

						found_recent_flag = true;
					} // end if

					/* for avg and peak day */
					if (cont_day_flag) {
						detail_hr.avg_hr_day += (float)heartrate; // sum
						avg_day_count++;

						if (((float)heartrate) > detail_hr.peak_hr_day) {
							detail_hr.peak_hr_day = (float)heartrate;
							detail_hr.peak_hr_day_time.hour = hour;
							detail_hr.peak_hr_day_time.min = min;
						}
					} // end if

					/* for avg and peak week */
					if (cont_week_flag){
						detail_hr.avg_hr_week += (float)heartrate; // sum
						avg_week_count++;

						if (((float)heartrate) > detail_hr.peak_hr_week) {
							detail_hr.peak_hr_week = (float)heartrate;
							detail_hr.peak_hr_week_time.hour = hour;
							detail_hr.peak_hr_week_time.min = min;
							detail_hr.peak_hr_week_time.day = day_count;
							detail_hr.peak_hr_week_time.month = month_count;
							detail_hr.peak_hr_week_time.year = year_count;
						}
					}// end if week

					/* for avg and peak month */
					detail_hr.avg_hr_month += (float)heartrate; // sum
					avg_month_count++;

					if (((float)heartrate) > detail_hr.peak_hr_month) {
						detail_hr.peak_hr_month = (float)heartrate;
						detail_hr.peak_hr_month_time.hour = hour;
						detail_hr.peak_hr_month_time.min = min;
						detail_hr.peak_hr_month_time.day = day_count;
						detail_hr.peak_hr_month_time.month = month_count;
						detail_hr.peak_hr_month_time.year = year_count;
					}

				}// end if correct data

			}// end file

			/* close file */
			fclose(fp);

		}

		/* set flags */
		if (found_recent_flag == true) {
			cont_recent_flag = false;
		}

		cont_day_flag = false;

		if (wday == 0) {
			cont_week_flag = false;
		}
		else {
			wday--;
		}

		/* next date */
		decrease_date(day_count, month_count, year_count);
	}// end finding

	/* cal avgs */
	if (avg_day_count > 0) {
		detail_hr.avg_hr_day = detail_hr.avg_hr_day / avg_day_count;
	}

	if (avg_week_count > 0) {
		detail_hr.avg_hr_week = detail_hr.avg_hr_week / avg_week_count;
	}

	if (avg_month_count > 0) {
		detail_hr.avg_hr_month = detail_hr.avg_hr_month / avg_month_count;
	}
}

/*----------------------------------------------------------------------------*/
static void get_detail_height(uint32_t patient_id, rbps_ns::detail_height_t &detail_height) {
	time_t cur_time;
	tm br_time;

	uint8_t day_count, day_dest, wday;
	uint8_t month_count, month_dest;
	uint16_t year_count, year_dest;

	FILE *fp;
	char filename[256];

	bool cont_flag = true;


	/* patient data block */
	uint16_t hour, min;
	uint16_t sys_dummy, dias_dummy;
	uint16_t heartrate;
	uint16_t weight;
	uint16_t height;

	/* get current time */
	time(&cur_time);
	localtime_r(&cur_time, &br_time);

	/* find blood pressure data */
	day_count = br_time.tm_mday;
	month_count = br_time.tm_mon + 1;
	year_count = br_time.tm_year + 1900;

	wday = br_time.tm_wday;

	day_dest = day_count;
	month_dest = month_count;
	year_dest = year_count;
	decrease_month(month_dest, year_dest);

	/* init value of detail_bp */
	memset(&detail_height, 0, sizeof(rbps_ns::detail_height_t));

	cont_flag = true;
	while ((day_count != day_dest) || (month_count != month_dest) || (year_count != year_dest)) {
		/* make filename */
		sprintf(filename, daily_patient_data, patient_id, day_count, month_count, year_count);
		/* open file */
		fp = fopen(filename, "r");
		if (fp != NULL) {

			while (!feof(fp)) {
				/* a block of data */
				fscanf(fp, patient_data_block,
						&hour, &min,
						&sys_dummy, &dias_dummy, &heartrate,
						&weight, &height);

				if (height > 0) {
					/* for recent */
					detail_height.recent_height = height;
					detail_height.recent_time.hour = hour;
					detail_height.recent_time.min = min;
					detail_height.recent_time.day = day_count;
					detail_height.recent_time.month = month_count;
					detail_height.recent_time.year = year_count;
					cont_flag = false;

				}// end if correct data

			}// end file

			/* close file */
			fclose(fp);
		}

		/* check flags */
		if (cont_flag == false) {
			break;
		}

		/* next date */
		decrease_date(day_count, month_count, year_count);
	}// end finding
}

/*----------------------------------------------------------------------------*/
static void get_detail_weight(uint32_t patient_id, rbps_ns::detail_weight_t &detail_weight) {
	time_t cur_time;
	tm br_time;

	uint8_t day_count, day_dest, wday;
	uint8_t month_count, month_dest;
	uint16_t year_count, year_dest;

	FILE *fp;
	char filename[256];

	uint8_t wcount;
	bool full_flag = false;

	/* patient data block */
	uint16_t hour, min;
	uint16_t sys_dummy, dias_dummy;
	uint16_t heartrate;
	uint16_t weight;
	uint16_t height;

	/* get current time */
	time(&cur_time);
	localtime_r(&cur_time, &br_time);

	/* find blood pressure data */
	day_count = br_time.tm_mday;
	month_count = br_time.tm_mon + 1;
	year_count = br_time.tm_year + 1900;

	wday = br_time.tm_wday;

	day_dest = day_count;
	month_dest = month_count;
	year_dest = year_count;
	decrease_month(month_dest, year_dest);

	/* init value of detail_bp */
	memset(&detail_weight, 0, sizeof(rbps_ns::detail_weight_t));

	wcount = 0;
	while ((day_count != day_dest) || (month_count != month_dest) || (year_count != year_dest)) {
		/* make filename */
		sprintf(filename, daily_patient_data, patient_id, day_count, month_count, year_count);
		/* open file */
		fp = fopen(filename, "r");
		if (fp != NULL) {

			while (!feof(fp)) {
				/* a block of data */
				fscanf(fp, patient_data_block,
						&hour, &min,
						&sys_dummy, &dias_dummy, &heartrate,
						&weight, &height);

				if (weight > 0) {
					/* for recent */
					detail_weight.recent_weight[wcount] = weight;
					detail_weight.recent_time[wcount].hour = hour;
					detail_weight.recent_time[wcount].min = min;
					detail_weight.recent_time[wcount].day = day_count;
					detail_weight.recent_time[wcount].month = month_count;
					detail_weight.recent_time[wcount].year = year_count;
					wcount++;
					if (wcount == 3) {
						full_flag = true;
						wcount = 0;
					}

				}// end if correct data

			}// end file

			/* close file */
			fclose(fp);
		}


		/* check flags */
		if (full_flag) {
			break;
		}

		/* next date */
		decrease_date(day_count, month_count, year_count);
	}// end finding

	/* cal avg */
	uint16_t sum_w = 0;
	uint8_t w_count2 = 0;
	for (uint16_t count = 0; count < 3; count++) {
		if (detail_weight.recent_weight[count] > 0) {
			sum_w += detail_weight.recent_weight[count];
			w_count2++;
		}
	}

	detail_weight.avg_weight = ((float)sum_w)/((float)w_count2);
}

/*----------------------------------------------------------------------------*/
static void prehypertension_predict(uint32_t patient_id, rbps_ns::predict_data_t &predict_dt) {

	/* get detail blood pressure */
	rbps_ns::detail_bp_t detail_bp;
	get_detail_blood_pressure(patient_id, detail_bp);

	/* get detail heart rate */
	rbps_ns::detail_hr_t detail_hr;
	get_detail_heart_rate(patient_id, detail_hr);

	/* get detail height */
	rbps_ns::detail_height_t detail_height;
	get_detail_height(patient_id, detail_height);

	/* get detail weight */
	rbps_ns::detail_weight_t detail_weight;
	get_detail_weight(patient_id, detail_weight);

	/* set risk of prehypertension */
	if (detail_bp.avg_sys_month > (float)rbps_ns::prehypertension_lsys ||
			detail_bp.avg_dias_month > (float)rbps_ns::prehypertension_ldias) {
		predict_dt.prehypertension_risk = true;
	}

	/* set detail data */
	predict_dt.avg_sys = detail_bp.avg_sys_month;
	predict_dt.avg_dias = detail_bp.avg_dias_month;
	predict_dt.avg_heart_rate = detail_hr.avg_hr_month;

	if (detail_height.recent_height > 0) {
		predict_dt.avg_bmi = detail_weight.avg_weight * detail_weight.avg_weight
				/ detail_height.recent_height;
	}
	else {
		predict_dt.avg_bmi = 0;
	}

	char name_dummy[128];
	uint16_t day_of_birth[3];
	get_basic_info(patient_id, name_dummy, day_of_birth,
			predict_dt.diabetes, predict_dt.dyslipidemia,
			predict_dt.atherosclerosis, predict_dt.gout);
}

/*-------------------------- Wifi process message func -----------------------*/
static void scan_node_wifi_func(cc_info_t &cc_info, rbps_ns::mesg_t &mesg) {
	uint16_t count;
	uint8_t data_buffer[rbps_ns::mtext_max_size-3];
	uint8_t* data_p = &data_buffer[1];
	uint8_t datalen = 1;

	for (count = 0; count < cc_info.num_nodes; count++) {
		/* check next data */
		if ((datalen + 8) > (rbps_ns::mtext_max_size - 3)) {
			/* forward data frame to wifi process */
			data_buffer[0] = rbps_ns::status_true;
			forward_gframe(rbps_ns::cc2wi_mq_id, mesg.mtype, datalen, rbps_ns::scan_node_rep,
					data_buffer);

			CTRL_PRINTF("ctrl: scn: forwarded scan_node_rep to wifi process with cont %d\n",
					data_buffer[0]);

			/* reset data buffer */
			datalen = 1;
			data_p = &data_buffer[1];
		}

		/* save node data to buffer */
		uint32_to_buffer(cc_info.nodes_list[count].node_id, data_p);
		data_p += 4;
		uint32_to_buffer(cc_info.nodes_list[count].patient_id, data_p);
		data_p += 4;

		datalen += 8;
	}

	/* forwar the last data frame to wifi process */
	data_buffer[0] = rbps_ns::status_false;
	forward_gframe(rbps_ns::cc2wi_mq_id, mesg.mtype, datalen, rbps_ns::scan_node_rep,
						data_buffer);
	CTRL_PRINTF("ctrl: scn: forwarded scan_node_rep to wifi process with cont %d\n",
						data_buffer[0]);
}

/*----------------------------------------------------------------------------*/
static void detail_info_wifi_func(cc_info_t &cc_info, rbps_ns::mesg_t &mesg) {
	uint32_t patient_id;
	char name[128] = "No data";
	uint16_t date_of_birth[3] = {0, 0, 0};
	uint8_t data_buffer[rbps_ns::gframe_data_max_size];

	bool dummy1, dummy2, dummy3, dummy4;

	/* extract patient_id */
	patient_id = buffer_to_uint32(&mesg.mtext[3]);

	/* do nothing if patient_id = 0 */
	if (patient_id == 0) {
		CTRL_PRINTF("ctrl: dinwifi: patient_id %d, return\n", patient_id);
		return;
	}

	/* get name and date of birth */
	get_basic_info(patient_id, name, date_of_birth, dummy1, dummy2, dummy3, dummy4);

	/* send data to node */
	data_buffer[0] = (uint8_t)date_of_birth[0];
	data_buffer[1] = (uint8_t)date_of_birth[1];
	data_buffer[2] = (uint8_t)date_of_birth[2];
	data_buffer[3] = (uint8_t)(date_of_birth[2] >> 8);
	memcpy(&data_buffer[4], name, 20);
	forward_gframe(rbps_ns::cc2wi_mq_id, mesg.mtype, rbps_ns::detail_info_rep_length,
			rbps_ns::detail_info_rep_id, data_buffer);
	CTRL_PRINTF("ctrl: dinwifi: sent detail_info_rep to wifi process\n");
}

/*----------------------------------------------------------------------------*/
static void measure_node_wifi_func(cc_info_t &cc_info, rbps_ns::mesg_t &mesg) {
	uint32_t node_id;

	node_id = buffer_to_uint32(&mesg.mtext[3]);

	/* forward to node */
	forward_gframe(rbps_ns::cc2zb_mq_id, node_id, rbps_ns::measure_node_length,
			rbps_ns::measure_node_id, &mesg.mtext[3]);
	CTRL_PRINTF("ctrl: msn: forward measure node cmd to %x\n", (uint16_t)node_id);
}

/*----------------------------------------------------------------------------*/
static void detail_blood_pressure_wifi_func(cc_info_t &cc_info, rbps_ns::mesg_t &mesg) {
	rbps_ns::detail_bp_t detail_bp;
	uint32_t patient_id;
	uint8_t data_buffer[rbps_ns::mtext_max_size - 3];
	uint8_t datalen;
	uint8_t *data_p;

	/* get patient_id */
	patient_id = buffer_to_uint32(&mesg.mtext[3]);

	/* get detail bp */
	get_detail_blood_pressure(patient_id, detail_bp);
	CTRL_PRINTF("ctrl: dbpwifi: got detail bp of patient %d\n", patient_id);

	/* forward detail_blood_pressure_rep to wifi process */
	datalen = 91; // limit our scope (cont always = 0)
	data_buffer[0] = rbps_ns::status_false; // limit our scope

	data_p = &data_buffer[1];
	float_to_buffer(detail_bp.recent_sys, data_p);
	data_p += 4;
	float_to_buffer(detail_bp.recent_dias, data_p);
	data_p += 4;
	timestamp_to_buffer(detail_bp.recent_time, data_p);
	data_p += 6;

	float_to_buffer(detail_bp.avg_sys_day, data_p);
	data_p += 4;
	float_to_buffer(detail_bp.avg_dias_day, data_p);
	data_p += 4;

	float_to_buffer(detail_bp.peak_sys_day, data_p);
	data_p += 4;
	timestamp_to_buffer(detail_bp.peak_sys_day_time, data_p);
	data_p += 2; // only need hour, min

	float_to_buffer(detail_bp.peak_dias_day, data_p);
	data_p += 4;
	timestamp_to_buffer(detail_bp.peak_dias_day_time, data_p);
	data_p += 2; // only need hour, min

	float_to_buffer(detail_bp.avg_sys_week, data_p);
	data_p += 4;
	float_to_buffer(detail_bp.avg_dias_week, data_p);
	data_p += 4;

	float_to_buffer(detail_bp.peak_sys_week, data_p);
	data_p += 4;
	timestamp_to_buffer(detail_bp.peak_sys_week_time, data_p);
	data_p += 6;

	float_to_buffer(detail_bp.peak_dias_week, data_p);
	data_p += 4;
	timestamp_to_buffer(detail_bp.peak_dias_week_time, data_p);
	data_p += 6;

	float_to_buffer(detail_bp.avg_sys_month, data_p);
	data_p += 4;
	float_to_buffer(detail_bp.avg_dias_month, data_p);
	data_p += 4;

	float_to_buffer(detail_bp.peak_sys_month, data_p);
	data_p += 4;
	timestamp_to_buffer(detail_bp.peak_sys_month_time, data_p);
	data_p += 6;

	float_to_buffer(detail_bp.peak_dias_month, data_p);
	data_p += 4;
	timestamp_to_buffer(detail_bp.peak_dias_month_time, data_p);
	data_p += 6;

	forward_gframe(rbps_ns::cc2wi_mq_id, mesg.mtype, datalen, rbps_ns::detail_blood_pressure_rep,
			data_buffer);
	CTRL_PRINTF("ctrl: dbpwifi: Forwarded detail bp rep to wifi %ld\n", mesg.mtype);
}

/*----------------------------------------------------------------------------*/
static void detail_heart_rate_wifi_func(cc_info_t &cc_info, rbps_ns::mesg_t &mesg) {
	rbps_ns::detail_hr_t detail_hr;
	uint32_t patient_id;
	uint8_t data_buffer[rbps_ns::mtext_max_size - 3];
	uint8_t datalen;
	uint8_t *data_p;

	/* get patient_id */
	patient_id = buffer_to_uint32(&mesg.mtext[3]);

	/* get detail hr */
	get_detail_heart_rate(patient_id, detail_hr);
	CTRL_PRINTF("ctrl: dhrwifi: got detail hr of patient %d\n", patient_id);

	/* forward detail_blood_pressure_rep to wifi process */
	datalen = 49; // limit our scope (cont always = 0)
	data_buffer[0] = rbps_ns::status_false; // limit our scope

	data_p = &data_buffer[1];
	float_to_buffer(detail_hr.recent_hr, data_p);
	data_p += 4;
	timestamp_to_buffer(detail_hr.recent_time, data_p);
	data_p += 6;

	float_to_buffer(detail_hr.avg_hr_day, data_p);
	data_p += 4;

	float_to_buffer(detail_hr.peak_hr_day, data_p);
	data_p += 4;
	timestamp_to_buffer(detail_hr.peak_hr_day_time, data_p);
	data_p += 2; // only need hour, min

	float_to_buffer(detail_hr.avg_hr_week, data_p);
	data_p += 4;

	float_to_buffer(detail_hr.peak_hr_week, data_p);
	data_p += 4;
	timestamp_to_buffer(detail_hr.peak_hr_week_time, data_p);
	data_p += 6;

	float_to_buffer(detail_hr.avg_hr_month, data_p);
	data_p += 4;

	float_to_buffer(detail_hr.peak_hr_month, data_p);
	data_p += 4;
	timestamp_to_buffer(detail_hr.peak_hr_month_time, data_p);
	data_p += 6;

	forward_gframe(rbps_ns::cc2wi_mq_id, mesg.mtype, datalen, rbps_ns::detail_heart_rate_rep,
			data_buffer);
	CTRL_PRINTF("ctrl: dhrwifi: Forwarded detail hr rep to wifi %ld\n", mesg.mtype);
}

/*----------------------------------------------------------------------------*/
static void detail_height_wifi_func(cc_info_t &cc_info, rbps_ns::mesg_t &mesg) {
	rbps_ns::detail_height_t detail_height;
	uint32_t patient_id;
	uint8_t data_buffer[rbps_ns::mtext_max_size - 3];
	uint8_t datalen = rbps_ns::detail_height_rep_length;

	/* get patient_id */
	patient_id = buffer_to_uint32(&mesg.mtext[3]);

	/* get detail height */
	get_detail_height(patient_id, detail_height);
	CTRL_PRINTF("ctrl: dhewifi: got detail height of patient %d\n", patient_id);

	/* forward data to wifi process */
	data_buffer[0] = detail_height.recent_height;
	data_buffer[1] = detail_height.recent_time.day;
	data_buffer[2] = detail_height.recent_time.month;
	uint16_to_buffer(detail_height.recent_time.year, &data_buffer[3]);

	forward_gframe(rbps_ns::cc2wi_mq_id, mesg.mtype, datalen, rbps_ns::detail_height_rep,
			data_buffer);
	CTRL_PRINTF("ctrl: dhewifi: Forwarded detail height rep to wifi %ld\n", mesg.mtype);
}

/*----------------------------------------------------------------------------*/
static void detail_weight_wifi_func(cc_info_t &cc_info, rbps_ns::mesg_t &mesg) {
	rbps_ns::detail_weight_t detail_weight;
	uint32_t patient_id;
	uint8_t data_buffer[rbps_ns::mtext_max_size - 3];
	uint8_t datalen = rbps_ns::detail_weight_rep_length;

	uint16_t count;
	uint8_t *data_p;

	/* get patient_id */
	patient_id = buffer_to_uint32(&mesg.mtext[3]);

	/* get detail height */
	get_detail_weight(patient_id, detail_weight);
	CTRL_PRINTF("ctrl: dwewifi: got detail weight of patient %d\n", patient_id);

	/* forward data to wifi process */
	data_p = data_buffer;
	for (count = 0; count < 3; count++) {
		*data_p = detail_weight.recent_weight[count];
		data_p++;
		*data_p = detail_weight.recent_time[count].day;
		data_p++;
		*data_p = detail_weight.recent_time[count].month;
		data_p++;
		uint16_to_buffer(detail_weight.recent_time[count].year, data_p);
		data_p += 2;
	}

	forward_gframe(rbps_ns::cc2wi_mq_id, mesg.mtype, datalen, rbps_ns::detail_weight_rep,
			data_buffer);
	CTRL_PRINTF("ctrl: dhewifi: Forwarded detail weight rep to wifi %ld\n", mesg.mtype);
}

/*----------------------------------------------------------------------------*/
static void detail_medical_history_wifi_func(cc_info_t &cc_info, rbps_ns::mesg_t mesg) {
	uint32_t patient_id;
	uint8_t data_buffer[rbps_ns::mtext_max_size - 3];
	uint8_t datalen = rbps_ns::detail_medical_history_rep_length;

	char name[128];
	uint16_t date_of_birth[3];
	bool diabetes, dyslipidemia, atherosclerosis, gout;

	/* get patient_id */
	patient_id = buffer_to_uint32(&mesg.mtext[3]);

	/* get medical data */
	get_basic_info(patient_id, name, date_of_birth,
			diabetes, dyslipidemia, atherosclerosis, gout);
	data_buffer[0] = diabetes ? rbps_ns::status_true : rbps_ns::status_false;
	data_buffer[1] = dyslipidemia ? rbps_ns::status_true : rbps_ns::status_false;
	data_buffer[2] = atherosclerosis ? rbps_ns::status_true : rbps_ns::status_false;
	data_buffer[3] = gout ? rbps_ns::status_true : rbps_ns::status_false;

	forward_gframe(rbps_ns::cc2wi_mq_id, mesg.mtype, datalen,
			rbps_ns::detail_medical_history_rep, data_buffer);
	CTRL_PRINTF("ctrl: dmdwifi: Forwarded detail med data rep to wifi %ld\n", mesg.mtype);
}

/*----------------------------------------------------------------------------*/
static void prediction_wifi_func(cc_info_t &cc_info, rbps_ns::mesg_t &mesg) {
	uint32_t patient_id;
	uint8_t data_buffer[rbps_ns::mtext_max_size - 3];
	uint8_t datalen = rbps_ns::prediction_rep_length;
	rbps_ns::predict_data_t predict_dt;
	uint8_t *data_p = data_buffer;

	/* get patient_id */
	patient_id = buffer_to_uint32(&mesg.mtext[3]);

	/* get prediction data */
	prehypertension_predict(patient_id, predict_dt);

	/* forward data to wifi process */
	*data_p = predict_dt.prehypertension_risk ? rbps_ns::status_true : rbps_ns::status_false;
	data_p++;

	float_to_buffer(predict_dt.avg_sys, data_p);
	data_p += 4;
	float_to_buffer(predict_dt.avg_dias, data_p);
	data_p += 4;

	float_to_buffer(predict_dt.avg_heart_rate, data_p);
	data_p += 4;

	float_to_buffer(predict_dt.avg_bmi, data_p);
	data_p += 4;

	*data_p = predict_dt.diabetes ? rbps_ns::status_true : rbps_ns::status_false;
	data_p++;
	*data_p = predict_dt.dyslipidemia ? rbps_ns::status_true : rbps_ns::status_false;
	data_p++;
	*data_p = predict_dt.atherosclerosis ? rbps_ns::status_true : rbps_ns::status_false;
	data_p++;
	*data_p = predict_dt.gout ? rbps_ns::status_true : rbps_ns::status_false;
	data_p++;

	forward_gframe(rbps_ns::cc2wi_mq_id, mesg.mtype, datalen, rbps_ns::prediction_rep_id,
			data_buffer);
	CTRL_PRINTF("ctrl: dprwifi: Forwarded predict data rep to wifi %ld\n", mesg.mtype);
}

/*----------------------------------------------------------------------------*/
static void get_schedule_wifi_func(cc_info_t &cc_info, rbps_ns::mesg_t mesg) {
	uint32_t node_id;
	node_t *node_p;
	uint8_t data_buffer[rbps_ns::mtext_max_size - 3];
	uint8_t datalen = rbps_ns::update_schedule_length;
	uint8_t *data_p = data_buffer;

	/* get node_id */
	node_id = buffer_to_uint32(&mesg.mtext[3]);

	/* get schedule data */
	node_p = find_node(cc_info, node_id);
	if (node_p == NULL) {
		CTRL_PRINTF("ctrl: gswifi: can't find node %x\n", (uint16_t)node_id);
		return;
	}

	uint32_to_buffer(node_id, data_p);
	data_p += 4;

	*data_p = node_p->sched.abs_on ? rbps_ns::status_true : rbps_ns::status_false;
	data_p++;
	*data_p = node_p->sched.abs_hour;
	data_p++;
	*data_p = node_p->sched.abs_min;
	data_p++;
	*data_p = node_p->sched.abs_day;
	data_p++;
	*data_p = node_p->sched.abs_month;
	data_p++;
	uint16_to_buffer(node_p->sched.abs_year, data_p);
	data_p += 2;

	*data_p = node_p->sched.in_day_on ? rbps_ns::status_true : rbps_ns::status_false;
	data_p++;
	*data_p = node_p->sched.in_day_hour;
	data_p++;
	*data_p = node_p->sched.in_day_min;
	data_p++;

	forward_gframe(rbps_ns::cc2wi_mq_id, mesg.mtype, datalen, rbps_ns::update_schedule,
			data_buffer);
	CTRL_PRINTF("ctrl: gswifi: Forwarded update sched to wifi %ld\n", mesg.mtype);
}

/*----------------------------------------------------------------------------*/
static void update_schedule_wifi_func(cc_info_t &cc_info, rbps_ns::mesg_t mesg) {
	uint32_t node_id;
	node_t *node_p;
	uint8_t *rec_data_p = &mesg.mtext[3];

	uint8_t data_buffer[rbps_ns::mtext_max_size - 3];
	uint8_t datalen = rbps_ns::update_schedule_length;
	uint8_t *data_p = data_buffer;

	/* get node_id */
	node_id = buffer_to_uint32(&mesg.mtext[3]);
	rec_data_p += 4;

	/* set schedule data */
	node_p = find_node(cc_info, node_id);
	if (node_p == NULL) {
		CTRL_PRINTF("ctrl: upswifi: can't find node %x\n", (uint16_t)node_id);
		return;
	}

	node_p->sched.abs_on = (*rec_data_p == rbps_ns::status_true) ? true : false;
	rec_data_p++;
	node_p->sched.abs_hour = *rec_data_p;
	rec_data_p++;
	node_p->sched.abs_min = *rec_data_p;
	rec_data_p++;
	node_p->sched.abs_day = *rec_data_p;
	rec_data_p++;
	node_p->sched.abs_month = *rec_data_p;
	rec_data_p++;
	node_p->sched.abs_year = buffer_to_uint16(rec_data_p);
	rec_data_p += 2;

	node_p->sched.in_day_on = (*rec_data_p == rbps_ns::status_true) ? true : false;
	rec_data_p++;
	node_p->sched.in_day_hour = *rec_data_p;
	rec_data_p++;
	node_p->sched.in_day_min = *rec_data_p;
	rec_data_p++;

	CTRL_PRINTF("ctrl: upswifi: updated node %x sched\n"
			"abs(%d), %hu:%hu %hu-%hu-%hu\n"
			"inday(%d), %hu:%hu\n",
			(uint16_t)node_id,
			node_p->sched.abs_on, node_p->sched.abs_hour, node_p->sched.abs_min,
			node_p->sched.abs_day, node_p->sched.abs_month, node_p->sched.abs_year,
			node_p->sched.in_day_on, node_p->sched.in_day_hour, node_p->sched.in_day_min);

	/* reforward to all wifi processes */
	uint32_to_buffer(node_id, data_p);
	data_p += 4;

	*data_p = node_p->sched.abs_on ? rbps_ns::status_true : rbps_ns::status_false;
	data_p++;
	*data_p = node_p->sched.abs_hour;
	data_p++;
	*data_p = node_p->sched.abs_min;
	data_p++;
	*data_p = node_p->sched.abs_day;
	data_p++;
	*data_p = node_p->sched.abs_month;
	data_p++;
	uint16_to_buffer(node_p->sched.abs_year, data_p);
	data_p += 2;

	*data_p = node_p->sched.in_day_on ? rbps_ns::status_true : rbps_ns::status_false;
	data_p++;
	*data_p = node_p->sched.in_day_hour;
	data_p++;
	*data_p = node_p->sched.in_day_min;
	data_p++;

	forward_gframe(rbps_ns::cc2wi_mq_id, rbps_ns::wifi_boardcast_mtype,
			datalen, rbps_ns::update_schedule, data_buffer);
	CTRL_PRINTF("ctrl: upwifi: Forwarded update sched to all wifi %d\n",
			rbps_ns::wifi_boardcast_mtype);
}
