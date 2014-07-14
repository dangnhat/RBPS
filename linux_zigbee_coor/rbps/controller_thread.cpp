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
static void node_report_func(cc_info_t &cc_info, rbps_ns::mesg_t &mesg);

static uint16_t buffer_to_uint16(uint8_t *buffer); // LSB first
static uint32_t buffer_to_uint32(uint8_t *buffer); // LSB first

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
 *
 * @return		0 if successful, otherwise, error.
 */
static int8_t get_basic_info(uint32_t patient_id, char* name, uint16_t* date_of_birth);

/**
 * @brief   Get basic info (medical data) from file.
 *
 * @param[in]	patient_id,
 * @param[out]	height,
 * @param[out]	diabetes,
 * @param[out]	dyslipidemia,
 * @param[out]	atherosclerosis,
 * @param[out]	gout,
 *
 * @return		0 if successful, otherwise, error.
 */
static int8_t get_basic_info(uint32_t patient_id, uint8_t &height,
		bool &diabetes, bool &dyslipidemia, bool &atherosclerosis, bool &gout);

/*----------------------------------------------------------------------------*/
void* controller_thread_func(void *pdata) {
	int ret_val;
	rbps_ns::mesg_t mesg;
	uint16_t cmd_id;

	while (1) {
		/* check data from zigbee thread */
		ret_val = msgrcv(rbps_ns::zb2cc_mq_id, &mesg, rbps_ns::mtext_max_size, 0, IPC_NOWAIT);
		if (ret_val > 0) {
			CTRL_PRINTF("ctrl: New message from zigbee, src addr: %x\n", (uint16_t)mesg.mtype);

			cmd_id = buffer_to_uint16((uint8_t *)&mesg.mtext[1]);
			CTRL_PRINTF("ctrl: len %d, cmd %x\n", mesg.mtext[0], cmd_id);

			switch (cmd_id) {
			case rbps_ns::update_node_id:
				update_node_func(cc_info, mesg);
				break;
			case rbps_ns::detail_info_id:
				detai_info_node_func(cc_info, mesg);
				break;
			case rbps_ns::node_report_id:
				node_report_func(cc_info, mesg);
				break;
			}// end switch
		}// end if, data from zigbee thread;
	}

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
	ret_data_buffer[0] = rbps_ns::status_false;
	forward_gframe(rbps_ns::cc2zb_mq_id, mesg.mtype, rbps_ns::update_node_rep_length,
			rbps_ns::update_node_rep_id, ret_data_buffer);
	CTRL_PRINTF("ctrl: unf: sent incorrect update_node_rep to zigbee (dest addr %x)",
			(uint16_t)mesg.mtype);

	/* update node data */
	node_p = find_node(cc_info, node_id);

	if (node_p != NULL) {
		CTRL_PRINTF("ctrl: unf: node_id %d exists, update new patient_id %d\n",
				node_id, patient_id);
		node_p->patient_id = patient_id;
	}
	else {
		CTRL_PRINTF("ctrl: unf: node_id %d doesn't exists, insert new node with pa_id %d\n",
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

	/* extract patient_id */
	patient_id = buffer_to_uint32(&mesg.mtext[3]);

	/* do nothing if patient_id = 0 */
	if (patient_id == 0) {
		CTRL_PRINTF("ctrl: dinf: patient_id %d, return\n", patient_id);
		return;
	}

	/* get name and date of birth */
	get_basic_info(patient_id, name, date_of_birth);

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
static void node_report_func(cc_info_t &cc_info, rbps_ns::mesg_t &mesg) {
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

	rbps_ns::mesg_t wifi_mesg;

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
	month = (uint8_t)br_time.tm_mon; // 0 - 11
	year = (uint16_t)br_time.tm_year + 1990;

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
	sprintf(filename, "%s/%u/%hu-%hu-%u.txt", patient_data_path, patient_id, day, month, year);
	fp = fopen(filename, "a");
	if (fp == NULL) {
		CTRL_PRINTF("ctrl: nr: Can't open %s, ret!\n", filename);
		return;
	}

	fprintf(fp, "Time: %hu:%hu\n", hour, min);
	fprintf(fp, "Blood pressure: %u/%u (mmHg)\n", sys, dias);
	fprintf(fp, "Heart rate: %u (pulses/min)\n", heart_rate);
	fprintf(fp, "Weight: (kg)\n");

	/* close file */
	fclose(fp);

	CTRL_PRINTF("ctrl: nr: saved data to file %s\n", filename);

	/* forward frame to wifi process */
	*hour_pos = hour;
	*min_pos = min;
	*day_pos = day;
	*month_pos = month;
	*year_firstpos = (uint8_t)year;
	*(year_firstpos + 1) = (uint8_t)(year >> 8);

	memcpy(wifi_mesg.mtext, mesg.mtext, rbps_ns::node_report_length);
	forward_gframe(rbps_ns::cc2wi_mq_id, rbps_ns::wifi_boardcast_mtype,
			rbps_ns::node_report_length, rbps_ns::node_report_id, &wifi_mesg.mtext[3]);
	CTRL_PRINTF("ctrl: nr: forwarded to wifi process\n");

	/* check prediction */
	if (need_predict == rbps_ns::status_true) {
		CTRL_PRINTF("ctrl: nr: node need predict\n");

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
static int8_t get_basic_info(uint32_t patient_id, char* name, uint16_t* date_of_birth) {
	int8_t ret_val;
	char basic_info_path[128];
	FILE* fp;

	/* retrieve data */
	sprintf(basic_info_path, "%s/%d/%s", patient_data_path, patient_id, basic_info_name);
	fp = fopen(basic_info_path, "r");
	if (fp == NULL) {
		CTRL_PRINTF("ctrl: gbi: Can't open %s\n", basic_info_path);
		return -1;
	}

	/* get name */
	fscanf(fp, "Name: %s\n", name);

	/* get date of birth */
	fscanf(fp, "Date of birth: %hu/%hu/%hu\n", &date_of_birth[0], &date_of_birth[1], &date_of_birth[2]);

	/* close file */
	fclose(fp);

	return 0;
}

/*----------------------------------------------------------------------------*/
static int8_t get_basic_info(uint32_t patient_id, uint8_t &height,
		bool &diabetes, bool &dyslipidemia, bool &atherosclerosis, bool &gout) {
	int8_t ret_val;
	char basic_info_path[128];
	FILE* fp;
	char buffer[128];

	/* retrieve data */
	sprintf(basic_info_path, "%s/%d/%s", patient_data_path, patient_id, basic_info_name);
	fp = fopen(basic_info_path, "r");
	if (fp == NULL) {
		CTRL_PRINTF("ctrl: gbi: Can't open %s\n", basic_info_path);
		return -1;
	}

	/* get name, date of birth, --- Medical data --- */
	fgets(buffer, 128, fp);
	fgets(buffer, 128, fp);
	fgets(buffer, 128, fp);

	/* get height */
	fscanf(fp, "Height: %hu cm\n", (uint16_t *)&height);

	fscanf(fp, "Diabetes: %s\n", buffer);
	(strcmp(buffer, "no") == 0) ? diabetes = false : diabetes = true;

	fscanf(fp, "Dyslipidemia: %s\n", buffer);
	(strcmp(buffer, "no") == 0) ? dyslipidemia = false : dyslipidemia = true;

	fscanf(fp, "Atherosclerosis: %s\n", buffer);
	(strcmp(buffer, "no") == 0) ? atherosclerosis = false : atherosclerosis = true;

	fscanf(fp, "Gout: %s\n", buffer);
	(strcmp(buffer, "no") == 0) ? gout = false : gout = true;

	/* close file */
	fclose(fp);

	return 0;
}

