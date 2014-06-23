//============================================================================
// Name        : main.cpp
// Author      : Pham Huu Dang Nhat
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <unistd.h>
#include <stdint.h>
#include "rs232.h"
#include "misc.h"
#include "HA_System.h"

using namespace std;

int main() {
	char usr_cmd[1024];
	int num_node_rep;
	uint16_t node_add[2];
	uint32_t count;
	uint32_t count2;

    ZbStack_ns::Zb_callbackType_t callbackType;
    ZbStack_ns::Zb_startConfirmStatus_t startConfirmStatus;
    ZbStack_ns::Zb_dataStruct_s aDataStruct_s;

    printf("Starting CC2530ZNP...\n");

	HA_system_init();

	/**< some const for ZigbeeStack */
	const uint32_t logicType = ZbStack_ns::ZCD_coordinator;
	const uint32_t panID = 0xFFFF;
	const uint32_t channelList = ZbStack_ns::ZCD_chanList_15 | ZbStack_ns::ZCD_chanList_25 ;
	ZbStack_ns::Zb_appInfo_s appInfo_s;
	appInfo_s.appEndPoint = 2;
	appInfo_s.appProfileID = 0x000A;
	appInfo_s.deviceID = 0x0000;
	appInfo_s.deviceVersion = 0x01;
	appInfo_s.outputCmdsNum = 3;
	appInfo_s.outputCmdsList[0] = 0x0001; // discovery
	appInfo_s.outputCmdsList[1] = 0x0002; // turn led on
	appInfo_s.outputCmdsList[2] = 0x0003; // turn led off
	appInfo_s.inputCmdsNum = 1;
	appInfo_s.inputCmdsList[1] = 0x1001; // discovery

	/**< start ZigbeeStack */
	HA_ZbStack.CONF_write(ZbStack_ns::ZCD_NV_startupOption, ZbStack_ns::ZCD_startOpt_noClear);

	HA_ZbStack.CONF_write(ZbStack_ns::ZCD_NV_logicalType, logicType);
	HA_ZbStack.CONF_write(ZbStack_ns::ZCD_NV_panID, panID);
	HA_ZbStack.CONF_write(ZbStack_ns::ZCD_NV_chanList, channelList);

	HA_ZbStack.Zb_appRegister_request (appInfo_s);
	HA_ZbStack.Zb_start_request ();

	/**< wait for start_confirm */
	printf("Starting Zigbee stack...\n");
	while (1){
		HA_ZbStack.Zb_callback_get(callbackType);

		if (callbackType == ZbStack_ns::Zb_startConfirm){
			HA_ZbStack.Zb_startConfirm_get (startConfirmStatus);
			if (startConfirmStatus == ZbStack_ns::Zb_success)
				break;
		}
	}
	printf("Zigbee stack started as Coordinator\n");

	printf("Input command (disco, led1_on, led1_off, led2_on, led2_off): ");
	printf("Input disco\n");
	memcpy(usr_cmd, "disco", 5);
	usr_cmd[5] = '\0';

	/* Send cmd disco */
	aDataStruct_s.cmdID = 0x0001;
	aDataStruct_s.data_p = NULL;
	aDataStruct_s.destAddr = 0xFFFF;
	aDataStruct_s.len = 0;

	HA_ZbStack.Zb_sendData_request(aDataStruct_s, 0x01, true, 10);

	/* wait for data */
	num_node_rep = 0;
	while (1){
		HA_ZbStack.Zb_callback_get(callbackType);

		if (callbackType == ZbStack_ns::Zb_receiveDataIndication){
			HA_ZbStack.Zb_receiveDataIndication_get(aDataStruct_s);

			if (aDataStruct_s.cmdID == 0x1001) {
				printf("Node replied %x\n", aDataStruct_s.srcAddr);

				node_add[num_node_rep] = aDataStruct_s.srcAddr;

				num_node_rep++;
				if (num_node_rep == 2) {
					break;
				}
			}// end if
		}
	}// end while 1

	for (count = 0; count < 1000000000; count++);

	/* turn led 1 on */
	aDataStruct_s.cmdID = 0x0002;
	aDataStruct_s.data_p = NULL;
	aDataStruct_s.destAddr = node_add[0];
	aDataStruct_s.len = 0;

	HA_ZbStack.Zb_sendData_request(aDataStruct_s, 0x01, true, 10);
	printf("turn led 1 on\n");

	for (count = 0; count < 1000000000; count++);

	/* turn led 2 on */
	aDataStruct_s.cmdID = 0x0002;
	aDataStruct_s.data_p = NULL;
	aDataStruct_s.destAddr = node_add[1];
	aDataStruct_s.len = 0;

	HA_ZbStack.Zb_sendData_request(aDataStruct_s, 0x01, true, 10);
	printf("turn led 2 on\n");

	for (count = 0; count < 1000000000; count++);

	/* turn led 1 off */
	aDataStruct_s.cmdID = 0x0003;
	aDataStruct_s.data_p = NULL;
	aDataStruct_s.destAddr = node_add[0];
	aDataStruct_s.len = 0;

	HA_ZbStack.Zb_sendData_request(aDataStruct_s, 0x01, true, 10);
	printf("turn led 1 off\n");

	for (count = 0; count < 1000000000; count++);

	/* turn led 2 off */
	aDataStruct_s.cmdID = 0x0003;
	aDataStruct_s.data_p = NULL;
	aDataStruct_s.destAddr = node_add[1];
	aDataStruct_s.len = 0;

	HA_ZbStack.Zb_sendData_request(aDataStruct_s, 0x01, true, 10);
	printf("turn led 2 off\n");

	return 0;
}
