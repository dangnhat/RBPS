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
#include "controller_thread.h"

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

	/* start controller thread */
	controller_thread_func(NULL);

	return 0;
}
