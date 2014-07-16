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

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>

#include "rbps_glb.h"

namespace rbps_ns {
	/* shared queue between controller thread and zigbee thread */
	int cc2zb_mq_id = -1;
	int zb2cc_mq_id = -1;

	/* shared queue between controller (and zigbee) process and wifi process */
	int cc2wi_mq_id = -1;
	int wi2cc_mq_id = -1;

	char cur_path[1024];
}

using namespace rbps_ns;
/*----------------------------------------------------------------------------*/
void rbps_init(void) {
	/* create IPC_private queues */
	cc2zb_mq_id = msgget(IPC_PRIVATE, S_IRUSR | S_IWUSR);
	if (cc2zb_mq_id == -1) {
		RBPS_PRINTF("rbps_init:can't create cc2zb_mq\n");
	}

	zb2cc_mq_id = msgget(IPC_PRIVATE, S_IRUSR | S_IWUSR);
	if (zb2cc_mq_id == -1) {
		RBPS_PRINTF("rbps_init:can't create zb2cc_mq_id\n");
	}

	/* create shared queue between */
	cc2wi_mq_id = msgget(IPC_PRIVATE, S_IRUSR | S_IWUSR);
	if (zb2cc_mq_id == -1) {
		RBPS_PRINTF("rbps_init:can't create cc2wi_mq_id\n");
	}

	wi2cc_mq_id = msgget(IPC_PRIVATE, S_IRUSR | S_IWUSR);
	if (wi2cc_mq_id == -1) {
		RBPS_PRINTF("rbps_init:can't create wi2cc_mq_id\n");
	}

	RBPS_PRINTF("rbps_init:mqs created\n");

	/* find cur path */
	char buffer[1024];
	int num_bytes;
	int last_spos;
	int seclast_spos;
	int count;

	num_bytes = readlink("/proc/self/exe", buffer, 1024);
	last_spos = 0;
	seclast_spos = 0;
	for (count = 0; count < num_bytes; count++) {
		if (buffer[count] == '/') {
			seclast_spos = last_spos;
			last_spos = count;
		}
	}

	snprintf(cur_path, seclast_spos+1, "%s", buffer);
	RBPS_PRINTF("rbps_init: cur path %s\n", cur_path);
}
