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

/*--------------------------- znp_task Stack ---------------------------------*/
OS_STK znp_task_stack[znp_task_stack_size];

/*----------------------------------------------------------------------------*/
void znp_task_func(void *pdata) {
	ZbStack_ns::Zb_callbackType_t zigbee_callback;
	ZbStack_ns::Zb_dataStruct_s zigbee_data_s;

	StatusType mesg_status;
	void *mesg_p;
	uint8_t send_data_buffer[100];
	uint8_t ret_handle;
	ZbStack_ns::status_t ret_status;

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

			HA_ZbStack.Zb_sendData_request(zigbee_data_s, 0x01, true, 10);
			ZNP_TASK_PRINTF("znp_task:Sent data (cmd: %u, len: %u) to Coor (%u)\n",
					zigbee_data_s.cmdID, zigbee_data_s.len, zigbee_data_s.destAddr);

			/* wait for Send data confirm */
			while (1) {
				HA_ZbStack.Zb_callback_get(zigbee_callback);
				if (zigbee_callback == ZbStack_ns::Zb_sendDataConfirm) {
					HA_ZbStack.Zb_sendDataConfirm_get(ret_handle, ret_status);
					ZNP_TASK_PRINTF("znp_task:Received send data confirm, handle %d, status %x\n",
							ret_handle, ret_status);
					break;
				}// end if
			}/* end while (1) */

			/* continue */
			continue;
		}
	}/* end while (1) */
}

/*----------------------------------------------------------------------------*/
