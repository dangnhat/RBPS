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

#include "cLCD20x4.h"

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
//	CoCreateTask(bp_monitoring_task, 0, 0, &bp_monitoring_task_stack[1024-1], 1024);
//	CoCreateTask(keypad_task, 0, 0, &keypad_task_stack[1024-1], 1024);
//	CoCreateTask(controller_task, 0, 0, &controller_task_stack[1024-1], 1024);
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
	clcd20x4 a_lcd;
	const uint8_t pattern[8] = {0x0F, 0x00, 0x0F, 0x0F, 0x0, 0x0, 0xF, 0x0};
	const uint8_t bat_100[8] = {0x0E, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F};
	const uint8_t bat_lt66[8] = {0x0E, 0x1F, 0x11, 0x11, 0x1F, 0x1F, 0x1F, 0x1F};
	const uint8_t bat_lt33[8] = {0x0E, 0x1F, 0x11, 0x11, 0x11, 0x1F, 0x1F, 0x1F};
	const uint8_t bat_lt5[8] = {0x0E, 0x1F, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1F};

	a_lcd.putc(0x7F);
	a_lcd.gen_pattern(pattern, 0);
	a_lcd.gen_pattern(bat_100, 1);
	a_lcd.gen_pattern(bat_lt66, 2);
	a_lcd.gen_pattern(bat_lt33, 3);
	a_lcd.gen_pattern(bat_lt5, 4);
	a_lcd.putc(0);
	a_lcd.putc(1);
	a_lcd.putc(2);
	a_lcd.putc(3);
	a_lcd.putc(4);

	/* test keypad */
	GPIO_InitTypeDef gpio_init_s;
	gpio_init_s.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_init_s.GPIO_Speed = GPIO_Speed_10MHz;
	gpio_init_s.GPIO_Pin = GPIO_Pin_0;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_Init(GPIOC, &gpio_init_s);

	gpio_init_s.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOC, &gpio_init_s);

	GPIO_ResetBits(GPIOC, GPIO_Pin_0 | GPIO_Pin_1);

	GPIO_SetBits(GPIOC, GPIO_Pin_0);

	GPIO_SetBits(GPIOC, GPIO_Pin_1);

	GPIO_ResetBits(GPIOC, GPIO_Pin_0);


	while(1) {
		printf("This is znp task\n");
		CoTimeDelay(0, 0, 1, 0);
	}
}
