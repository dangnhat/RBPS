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
#include "MB1_System.h"
#include "coocox.h"

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
	while(1) {
		printf("This is znp task\n");
		CoTimeDelay(0, 0, 1, 0);
	}
}
