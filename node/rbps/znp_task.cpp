/**
 * @ingroup     MainProgramSource
 * @{
 *
 * @file        znp_task.h
 * @brief       Implementation of ZNP task.
 *
 * @author      DangNhat Pham-Huu <51002279@stu.hcmut.edu.vn>
 *
 * @}
 */

#include "znp_task.h"
#include "HA_System.h"
#include <cstring>

/* Debug definition */
#if ZNP_TASK_DEBUG
#define ZNP_TASK_PRINTF(...) RBPS_PRINTF(__VA_ARGS__)
#else
#define ZNP_TASK_PRINTF(...)
#endif

/*-------------------------- Configuration interface -------------------------*/
using namespace rbps_ns;

static const uint16_t coor_short_addr = 0x0000;

static const uint32_t logic_type = ZbStack_ns::ZCD_endDevice;
static const uint32_t pan_id = 0xFFFF;
static const uint32_t channel_list = ZbStack_ns::ZCD_chanList_15 | ZbStack_ns::ZCD_chanList_25 ;

static const uint8_t app_endpoint = 2;
static const uint16_t app_profile_id = 0x000A;
static const uint16_t app_device_id = 0x0001;
static const uint16_t app_device_version = 0x0001;
static const uint8_t input_cmds_num = 4;
static const uint16_t input_cmds[input_cmds_num] = {
		update_node_rep_id,
		detail_info_rep_id,
		measure_node_id,
		prediction_rep_id,
};
static const uint8_t output_cmds_num = 4;
static const uint16_t output_cmds[output_cmds_num] = {
		update_node_id,
		detail_info_id,
		hold_measurement_id,
		node_report_id,
};

/*--------------------------- znp_task Stack ---------------------------------*/
OS_STK znp_task_stack[znp_task_stack_size];

/*----------------------------------------------------------------------------*/
void znp_task_func(void *pdata) {
	ZbStack_ns::Zb_callbackType_t zigbee_callback;
	ZbStack_ns::Zb_dataStruct_s zigbee_data_s;

	StatusType mesg_status;
	void *mesg_p;
	uint8_t send_data_buffer[100];

	/* Init HA_system */
	HA_system_init();

	/* init Zigbee stack */
	init_zigbee_stack();

	while (1) {
		/* check received data from Zigbee */
		HA_ZbStack.Zb_callback_get(zigbee_callback);
		if (zigbee_callback == ZbStack_ns::Zb_receiveDataIndication) {
			HA_ZbStack.Zb_receiveDataIndication_get(zigbee_data_s);
			ZNP_TASK_PRINTF("znp_task:Received data from zigbee with cmd %u\n",
							zigbee_data_s.cmdID);

			/* move data to znp2main_cir_queue */
			znp2main_cir_queue.add_data((uint8_t) zigbee_data_s.len);
			znp2main_cir_queue.add_data((uint8_t) zigbee_data_s.cmdID);
			znp2main_cir_queue.add_data((uint8_t) (zigbee_data_s.cmdID >> 8));
			znp2main_cir_queue.add_data(zigbee_data_s.data_p, (uint8_t) zigbee_data_s.len);

			/* send message to znp2main_queue */
			CoPostQueueMail(znp2main_queue_id, (void *)&message_pending);

			ZNP_TASK_PRINTF("znp_task:Sent message to znp2main_queue(%d)\n", znp2main_queue_id);

			/* continue */
			continue;
		}

		/* check received message from main task */
		mesg_p = CoAcceptQueueMail(main2znp_queue_id, &mesg_status);
		if ((mesg_status == E_OK) && (*(uint8_t*)mesg_p == message_pending)) {
			ZNP_TASK_PRINTF("znp_task:Received new message from main task.\n");

			/* Send data to Coordinator */
			zigbee_data_s.len = main2znp_cir_queue.get_data();
			zigbee_data_s.cmdID = main2znp_cir_queue.get_data();
			zigbee_data_s.cmdID = (main2znp_cir_queue.get_data() << 8) | zigbee_data_s.cmdID;
			main2znp_cir_queue.get_data(send_data_buffer, zigbee_data_s.len);
			zigbee_data_s.data_p = send_data_buffer;
			zigbee_data_s.destAddr = coor_short_addr;

			HA_ZbStack.Zb_sendData_request(zigbee_data_s, 0x01, false, 10);
			ZNP_TASK_PRINTF("znp_task:Sent data (cmd: %u, len: %u) to Coor (%u)",
					zigbee_data_s.cmdID, zigbee_data_s.len, zigbee_data_s.destAddr);

			/* continue */
			continue;
		}
	}/* end while (1) */
}

/*----------------------------------------------------------------------------*/
static void init_zigbee_stack(void) {
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

	HA_ZNP.cmd_SPI_attach();
	HA_ZNP.SPI_reInit();
	HA_ZbStack.CONF_write(ZbStack_ns::ZCD_NV_startupOption, ZbStack_ns::ZCD_startOpt_noClear); // no clear state
	HA_ZNP.sys_reset_hard();

	HA_ZbStack.CONF_write(ZbStack_ns::ZCD_NV_logicalType, logic_type);
	HA_ZbStack.CONF_write(ZbStack_ns::ZCD_NV_panID, pan_id);
	HA_ZbStack.CONF_write(ZbStack_ns::ZCD_NV_chanList, channel_list);

	HA_ZbStack.Zb_appRegister_request(appInfo_s);
	HA_ZbStack.Zb_start_request();
}

/*----------------------------------------------------------------------------*/
