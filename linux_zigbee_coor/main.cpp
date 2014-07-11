/**
 * @file        main.cpp
 * @brief       Main program for linux coordinator of Remote blood pressure and heart rate
 * 				monitoring system.
 *
 * @author      DangNhat Pham-Huu <51002279@hcmut.edu.vn>
 */

#include <iostream>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/msg.h>
#include "pthread.h"
#include "rbps_glb.h"
#include "zigbee_thread.h"

using namespace std;

int main() {
	pthread_t zigbee_thread;
	int ret_val;
	rbps_ns::mesg_t a_mesg;
	uint16_t count;

	/* init system */
	rbps_init();

	/* create zigbee thread */
	ret_val = pthread_create(&zigbee_thread, NULL, zigbee_thread_func, NULL);
	if (ret_val != 0) {
		RBPS_PRINTF("main_thread:Can't create thread\n");
	}

	/* wait for message */
	while (1) {
		ret_val = msgrcv(rbps_ns::zb2cc_mq_id, &a_mesg, rbps_ns::mtext_max_size, 0, IPC_NOWAIT);
		if (ret_val > 0) {
			RBPS_PRINTF("main_thread:New message from zigbee thread\n");
			RBPS_PRINTF("main_thread:SrcAddr %x\n", (uint16_t)a_mesg.mtype);
			RBPS_PRINTF("main_thread:len %d, cmd %x\n",
					a_mesg.mtext[0], a_mesg.mtext[1] | (a_mesg.mtext[2] << 8));
			for (count = 0; count < a_mesg.mtext[0]; count++) {
				RBPS_PRINTF("%x ", (uint8_t)a_mesg.mtext[count+3]);
			}
			RBPS_PRINTF("\n");
		}
	}

	return 0;
}
