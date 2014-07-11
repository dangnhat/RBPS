/**
 * @ingroup     MainProgramSource
 * @{
 *
 * @file        rbps_glb.h
 * @brief       Global header for remote blood pressure and heart rate monitoring system.
 *
 * @author      DangNhat Pham-Huu <51002279@stu.hcmut.edu.vn>
 *
 * @}
 */

#include "rbps_glb.h"
#include "HA_System.h"

/* Global variables */
keypad rbps_keypad;
rbpm_gui rbps_node_gui;

ISRMgr_ns::ISR_t keypad_timer = ISRMgr_ns::ISRMgr_TIM6;

/* shared queue id */
namespace rbps_ns {
	OS_EventID main2znp_queue_id;
	OS_EventID znp2main_queue_id;
	uint16_t node_id = 0xFFFF;

	static const uint8_t main2znp_queue_size = 32;
	static const uint8_t znp2main_queue_size = 32;

	static void *main2znp_queue[main2znp_queue_size];
	static void *znp2main_queue[znp2main_queue_size];

	cir_queue main2znp_cir_queue;
	cir_queue znp2main_cir_queue;
}

using namespace rbps_ns;

/*-------------------------- Configuration interface -------------------------*/

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

/*----------------------------------------------------------------------------*/
/**
 * @brief   init zigbee stack with configuration in config interface of znp_task.cpp
 */
static void init_zigbee_stack(void) {
	ZbStack_ns::Zb_callbackType_t zigbee_callback;
	ZbStack_ns::Zb_startConfirmStatus_t start_confirm_status;
	ZbStack_ns::Zb_deviceInfoParams_s device_info_s;

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
	HA_ZbStack.CONF_write(ZbStack_ns::ZCD_NV_startupOption, ZbStack_ns::ZCD_startOpt_noClear);
	// no clear state
	HA_ZNP.sys_reset_hard();

	HA_ZbStack.CONF_write(ZbStack_ns::ZCD_NV_logicalType, logic_type);
	HA_ZbStack.CONF_write(ZbStack_ns::ZCD_NV_panID, pan_id);
	HA_ZbStack.CONF_write(ZbStack_ns::ZCD_NV_chanList, channel_list);

	HA_ZbStack.Zb_appRegister_request(appInfo_s);
	HA_ZbStack.Zb_start_request();

	/* wait for start confirm */
	RBPS_PRINTF("rbps_init:Requested to start Zigbee stack, wait for start confirm\n");
	while (1){
		HA_ZbStack.Zb_callback_get(zigbee_callback);

		if (zigbee_callback == ZbStack_ns::Zb_startConfirm){
			HA_ZbStack.Zb_startConfirm_get(start_confirm_status);
			if (start_confirm_status == ZbStack_ns::Zb_success)
				break;
		}
	}
	RBPS_PRINTF("rbps_init:Zigbee stack started\n");

	/* get short address, assign to node id */
	device_info_s.infoParam = ZbStack_ns::Zb_deviceShortAddr;
	HA_ZbStack.Zb_deviceInfo_get(device_info_s);
	node_id = device_info_s.values[0];
	node_id = (device_info_s.values[1] << 8) | node_id;
	RBPS_PRINTF("rbps_init:Zigbee stack started, node_id(short addr): %x\n", node_id);
}

/*----------------------------------------------------------------------------*/
void rbps_init(void) {
	/* assign ISR for keypad */
	MB1_ISRs.subISR_assign(keypad_timer, keypad_timerISR);

	/* init queue */
	main2znp_queue_id = CoCreateQueue(main2znp_queue,
			main2znp_queue_size, EVENT_SORT_TYPE_FIFO);
	znp2main_queue_id = CoCreateQueue(znp2main_queue,
			znp2main_queue_size, EVENT_SORT_TYPE_FIFO);

	/* Init HA Zigbee stack */
	HA_system_init();

	/* init Zigbee stack */
	init_zigbee_stack();
}
