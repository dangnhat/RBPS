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
#include "controller_thread.h"
#include "rbps_glb.h"

/* Debug definition */
#if CTRL_DEBUG
#define CTRL_PRINTF(...) printf(__VA_ARGS__)
#else
#define CTRL_PRINTF(...)
#endif

using namespace ctrl_ns;

/*---------------------- static function prototypes --------------------------*/
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
 * @brief   Update/or insert data of a node, forward message (broadcast) to
 * 			wifi process if patient id is correct.
 *
 * @param[out]	cc_info,
 * @param[in]	mesg, message from queue.
 */
static void update_node_func(cc_info_t &cc_info, rbps_ns::mesg_t &mesg);

/*----------------------------------------------------------------------------*/
void* controller_thread_func(void *pdata) {
	int ret_val;
	rbps_ns::mesg_t mesg;
	uint16_t cmd_id;

	while (1) {
		/* check data from zigbee thread */
		ret_val = msgrcv(rbps_ns::zb2cc_mq_id, &mesg, rbps_ns::mtext_max_size, 0, IPC_NOWAIT);
		if (ret_val > 0) {
			CTRL_PRINTF("ctrl: New message from zigbee, src addr: %x\n", mesg.mtype);

			cmd_id = buffer_to_uint16((uint8_t *)&mesg.mtext[1]);
			CTRL_PRINTF("ctrl: len %d, cmd %x\n", mesg.mtext[0], cmd_id);

			switch (cmd_id) {
			case rbps_ns::update_node_id:
				update_node_func(cc_info, mesg);
				break;
			}// end switch
		}// end if, data from zigbee thread;
	}

	return 0;
}

/*----------------------------- Static functions -----------------------------*/
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
static void update_node_func(cc_info_t &cc_info, rbps_ns::mesg_t &mesg) {
	uint16_t count;
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


