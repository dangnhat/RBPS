/**
 * @file        main.cpp
 * @brief       Main program for nodes of Remote blood pressure and heart rate
 * 				monitoring system.
 *
 * @author      DangNhat Pham-Huu <51002279@hcmut.edu.vn>
 */

#include <cstdio>
#include <cstring>
#include "stm32f10x.h"
#include "coocox.h"
#include "MB1_System.h"
#include "rbps_glb.h"

/* stacks */
OS_STK bp_monitoring_task_stack [1024];
OS_STK keypad_task_stack [1024];
OS_STK controller_task_stack [1024];
OS_STK znp_task_stack [1024];

/* task's function prototypes */
void bp_monitoring_task(void *pdata);
void keypad_task(void *pdata);
void controller_task(void *pdata);
void znp_task(void *pdata);

int main (void) {
	/* System initialization */
	MB1_system_init();
	rbps_init();

	/* Start OS */
	CoInitOS();
	CoCreateTask(bp_monitoring_task, 0, 0, &bp_monitoring_task_stack[1024-1], 1024);
	CoCreateTask(keypad_task, 0, 0, &keypad_task_stack[1024-1], 1024);
	CoCreateTask(controller_task, 0, 0, &controller_task_stack[1024-1], 1024);
	CoCreateTask(znp_task, 0, 0, &znp_task_stack[1024-1], 1024);
	CoStartOS();

	while (1) {
		;
	}

	return 0;
}

/**
 * @brief   Blood pressure and heart rate monitoring task
 *
 * @param[in]   param
 *
 * @return      NERVER RETURN
 */
void bp_monitoring_task(void *param) {
	while(1) {
		printf("This is bp_monitoring_task\n");
		CoTimeDelay(0, 0, 1, 0);
	}
}

/**
 * @brief   Keypad task
 *
 * @param[in]   param
 *
 * @return      NERVER RETURN
 */
void keypad_task(void *param) {
	while(1) {
		printf("This is keypad_task\n");
		CoTimeDelay(0, 0, 1, 0);
	}
}

/**
 * @brief   Controller task
 *
 * @param[in]   param
 *
 * @return      NERVER RETURN
 */
void controller_task(void *param) {
	while(1) {
		printf("This is controller task\n");
		CoTimeDelay(0, 0, 1, 0);
	}
}

/**
 * @brief   znp task
 *
 * @param[in]   param
 *
 * @return      NERVER RETURN
 */
void znp_task(void *param) {
	keypad a_keypad;
	bool longkey;
	int8_t key;
	uint32_t patient_id = 0;

	MB1_ISRs.subISR_assign(ISRMgr_ns::ISRMgr_TIM6, keypad_timerISR);

	rbpm_gui a_gui;
	a_gui.start_print_default_screen();
	while (1) {
		key = a_keypad.get_key(longkey);

		if ((key != keypad_ns::nokey) && (key < 10)) {
			patient_id = patient_id * 10 + key;
			a_gui.start_print_patient_id(patient_id);
		}
		else if (key == 10) {
			break;
		}
		else if (key == 11) {
			a_gui.start_clear_patient_id();
			patient_id = 0;
		}
	}

	/* check patient id */
	a_gui.start_print_checking();
	CoTimeDelay(0, 0, 1, 0);

	if (patient_id != 1992 && patient_id != 13456) {
		a_gui.start_print_incorrect();
	}
	else {
		a_gui.welcome_print_default_screen((int8_t *)"Pham Huu Dang Nhat");

		while (1) {
			key = a_keypad.get_key(longkey);

			if (key == 1) {
				break;
			}
		}

		a_gui.measuring_print_mesg();
		CoTimeDelay(0, 0, 1, 0);

		a_gui.direct_print_default_screen(120, 78, 60, false, 123, 79, 67, 23, true, false, true, false);

		while (1) {
			key = a_keypad.get_key(longkey);

			if (key == 10) {
				if (longkey) {
					while(a_keypad.is_longkey_still_hold(key)){
						a_gui.direct_scroll_down();
						CoTimeDelay(0, 0, 0, 300);
					}
				}
				else {
					a_gui.direct_scroll_down();
				}
			}
			else if (key == 11) {
				if (longkey) {
					while(a_keypad.is_longkey_still_hold(key)){
						a_gui.direct_scroll_up();
						CoTimeDelay(0, 0, 0, 300);
					}
				}
				else {
					a_gui.direct_scroll_up();
				}
			}
		}
	}

	while(1) {
		printf("This is znp task\n");
		CoTimeDelay(0, 0, 1, 0);
	}
}
