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

/*-------------------------- Configuration interface -------------------------*/
using namespace rbps_ns;

const uint32_t logic_type = ZbStack_ns::ZCD_endDevice;
const uint32_t pan_id = 0xFFFF;
const uint32_t channel_list = ZbStack_ns::ZCD_chanList_15 | ZbStack_ns::ZCD_chanList_25 ;

const uint8_t app_endpoint = 2;
const uint16_t app_profile_id = 0x000A;
const uint16_t app_device_id = 0x0001;
const uint8_t input_cmds_num = 4;
const uint16_t input_cmds[input_cmds_num] = {
		update_node_rep_id, // update_node_rep
		detail_info_rep_id, // detail_info_rep
		measure_node_id, // measure node
		0x
};


		ZbStack_ns::Zb_appInfo_s appInfo_s;
		    appInfo_s.appEndPoint = 2;
		    appInfo_s.appProfileID = 0x000A;
		    appInfo_s.deviceID = 0x0001;
		    appInfo_s.deviceVersion = 0x01;
		    appInfo_s.inputCmdsNum = 3;
		    appInfo_s.inputCmdsList[0] = 0x0001; // disco
		    appInfo_s.inputCmdsList[1] = 0x0002; // led1 on
		    appInfo_s.inputCmdsList[2] = 0x0003; // led2 on
		    appInfo_s.outputCmdsNum = 1;
		    appInfo_s.outputCmdsList[0] = 0x1001; // disco rep



/*----------------------------------------------------------------------------*/
void znp_task_func(void *pdata) {


	while (1) {
		;
	}
}

/*----------------------------------------------------------------------------*/
void init_zigbee_stack(void) {

}
