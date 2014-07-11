/**
 * @ingroup     MainProgramSource
 * @{
 *
 * @file        zigbee_thread.cpp
 * @brief       Implementation of zigbee thread.
 *
 * @author      DangNhat Pham-Huu <51002279@stu.hcmut.edu.vn>
 *
 * @}
 */

#include <iostream>
#include <sys/types.h>
#include <sys/msg.h>
#include "zigbee_thread.h"
#include "rbps_glb.h"
#include "HA_System.h"


/* Debug definition */
#if ZIGBEE_DEBUG
#define ZIGBEE_PRINTF(...) printf(__VA_ARGS__)
#else
#define ZIGBEE_PRINTF(...)
#endif

/*-------------------------- Configuration interface -------------------------*/
using namespace rbps_ns;

static const uint32_t logic_type = ZbStack_ns::ZCD_coordinator;
static const uint32_t pan_id = 0xFFFF;
static const uint32_t channel_list = ZbStack_ns::ZCD_chanList_15 | ZbStack_ns::ZCD_chanList_25 ;

static const uint8_t app_endpoint = 2;
static const uint16_t app_profile_id = 0x000A;
static const uint16_t app_device_id = 0x0002;
static const uint16_t app_device_version = 0x0001;
static const uint8_t input_cmds_num = 4;
static const uint16_t input_cmds[input_cmds_num] = {
		update_node_id,
		detail_info_id,
		hold_measurement_id,
		node_report_id,
};
static const uint8_t output_cmds_num = 4;
static const uint16_t output_cmds[output_cmds_num] = {
		update_node_rep_id,
		detail_info_rep_id,
		measure_node_id,
		prediction_rep_id,
};

/*----------------------------------------------------------------------------*/
/**
 * @brief   init zigbee stack with configuration in config interface of znp_task.cpp
 */
static void init_zigbee_stack(void) {
	ZbStack_ns::Zb_callbackType_t zigbee_callback;
	ZbStack_ns::Zb_startConfirmStatus_t start_confirm_status;

	 /* start ZbStack */
	ZbStack_ns::Zb_appInfo_s appInfo_s;
	appInfo_s.appEndPoint = app_endpoint;
	appInfo_s.appProfileID = app_profile_id;
	appInfo_s.deviceID = app_device_id;
	appInfo_s.deviceVersion = app_device_version;
	appInfo_s.inputCmdsNum = input_cmds_num;
	memcpy(appInfo_s.inputCmdsList, input_cmds, input_cmds_num*sizeof(uint16_t));
	appInfo_s.outputCmdsNum = output_cmds_num;
	memcpy(appInfo_s.outputCmdsList, output_cmds, output_cmds_num*sizeof(uint16_t));

	HA_ZbStack.CONF_write(ZbStack_ns::ZCD_NV_startupOption, ZbStack_ns::ZCD_startOpt_noClear);
	// no clear state
	HA_ZNP.sys_reset_hard();

	HA_ZbStack.CONF_write(ZbStack_ns::ZCD_NV_logicalType, logic_type);
	HA_ZbStack.CONF_write(ZbStack_ns::ZCD_NV_panID, pan_id);
	HA_ZbStack.CONF_write(ZbStack_ns::ZCD_NV_chanList, channel_list);

	HA_ZbStack.Zb_appRegister_request(appInfo_s);
	HA_ZbStack.Zb_start_request();

	/* wait for start confirm */
	ZIGBEE_PRINTF("zigbee_thread:Requested to start Zigbee stack, wait for start confirm\n");
	while (1){
		HA_ZbStack.Zb_callback_get(zigbee_callback);

		if (zigbee_callback == ZbStack_ns::Zb_startConfirm){
			HA_ZbStack.Zb_startConfirm_get(start_confirm_status);
			if (start_confirm_status == ZbStack_ns::Zb_success)
				break;
		}
	}
	ZIGBEE_PRINTF("zigbee_thread:Zigbee stack started\n");
}

/*----------------------------------------------------------------------------*/
void* zigbee_thread_func(void *pdata) {
	ZbStack_ns::Zb_callbackType_t zigbee_callback;
	ZbStack_ns::Zb_dataStruct_s zigbee_data_s;

	mesg_t a_mesg;
	int ret_val;
	uint8_t ret_handle;
	ZbStack_ns::status_t ret_status;

	/* Init HA_system */
	HA_system_init();

	/* start Zigbee stack */
	init_zigbee_stack();

	while (1) {
		/* check received data from Zigbee */
		HA_ZbStack.Zb_callback_get(zigbee_callback);
		if (zigbee_callback == ZbStack_ns::Zb_receiveDataIndication) {
			HA_ZbStack.Zb_receiveDataIndication_get(zigbee_data_s);
			ZIGBEE_PRINTF("zigbee_thread:Received data from zigbee with cmd %u\n",
							zigbee_data_s.cmdID);

			/* pack zigbee data to message */
			a_mesg.mtype = zigbee_data_s.srcAddr; // don't care this value, but it's should be > 0.
			a_mesg.mtext[0] = (uint8_t) zigbee_data_s.len;
			a_mesg.mtext[1] = (uint8_t) zigbee_data_s.cmdID;
			a_mesg.mtext[2] = (uint8_t) (zigbee_data_s.cmdID >> 8);
			memcpy(&a_mesg.mtext[3], zigbee_data_s.data_p, (uint8_t) zigbee_data_s.len);

			/* send message to cc thread */
			if (msgsnd(zb2cc_mq_id, (const void *)&a_mesg, zigbee_data_s.len +3, 0) == 0) {
				ZIGBEE_PRINTF("zigbee_thread:Sent message to zb2cc_mq(%d)\n", zb2cc_mq_id);
			}
			else {
				ZIGBEE_PRINTF("zigbee_thread:Can't send message to zb2cc_mq(%d)\n", zb2cc_mq_id);
			}

			/* continue */
			continue;
		}/* end if */

		/* check received data from cc */
		ret_val = msgrcv(cc2zb_mq_id, &a_mesg, mtext_max_size, 0, IPC_NOWAIT);
		if (ret_val > 0) {
			ZIGBEE_PRINTF("zigbee_thread:Received mesg from cc\n");

			/* send mesg to Zigbee */
			zigbee_data_s.destAddr = a_mesg.mtype;
			zigbee_data_s.len = a_mesg.mtext[0];
			zigbee_data_s.cmdID = a_mesg.mtext[1] | (a_mesg.mtext[2] << 8);
			zigbee_data_s.data_p = (uint8_t *)&a_mesg.mtext[3];
			HA_ZbStack.Zb_sendData_request(zigbee_data_s, 1, true, 10);
			ZIGBEE_PRINTF("zigbee_thread:Sent data to ZNP, destAddr %x, len %x, cmdID %x\n",
					zigbee_data_s.destAddr, zigbee_data_s.len, zigbee_data_s.cmdID);

			/* wait for Send data confirm */
			while (1) {
				HA_ZbStack.Zb_callback_get(zigbee_callback);
				if (zigbee_callback == ZbStack_ns::Zb_sendDataConfirm) {
					HA_ZbStack.Zb_sendDataConfirm_get(ret_handle, ret_status);
					ZIGBEE_PRINTF("zigbee_thread:Received send data confirm, handle %d, status %x",
							ret_handle, ret_status);
					break;
				}// end if
			}/* end while (1) */

			continue;
		}

	}/* end while (1) */

	return 0;
}
