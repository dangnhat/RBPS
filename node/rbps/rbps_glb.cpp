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

/* Global variables */
keypad rbps_keypad;
rbpm_gui rbps_node_gui;

ISRMgr_ns::ISR_t keypad_timer = ISRMgr_ns::ISRMgr_TIM6;

/* shared queue id */
namespace rbps_ns {
	OS_EventID main2znp_queue_id;
	OS_EventID znp2main_queue_id;

	static const uint8_t main2znp_queue_size = 128;
	static const uint8_t znp2main_queue_size = 128;

	static void *main2znp_queue[main2znp_queue_size];
	static void *znp2main_queue[znp2main_queue_size];

	cir_queue main2znp_cir_queue;
	cir_queue znp2main_cir_queue;
}

using namespace rbps_ns;
/*----------------------------------------------------------------------------*/
void rbps_init(void) {
	/* assign ISR for keypad */
	MB1_ISRs.subISR_assign(keypad_timer, keypad_timerISR);

	/* init queue */
	main2znp_queue_id = CoCreateQueue(main2znp_queue,
			main2znp_queue_size, EVENT_SORT_TYPE_FIFO);
	znp2main_queue_id = CoCreateQueue(znp2main_queue,
			znp2main_queue_size, EVENT_SORT_TYPE_FIFO);
}
