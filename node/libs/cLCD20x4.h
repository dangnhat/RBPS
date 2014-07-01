/**
 * @defgroup
 * @brief
 * @ingroup     libraries
 * @{
 *
 * @file        cLCD20x4.h
 * @brief       Character LCD 20x4 driver for MBoard-1
 *
 * @author      DangNhat Pham-Huu <51002279@stu.hcmut.edu.vn>
 */

#ifndef CLCD20x4_H_
#define CLCD20x4_H_

/* Includes */
#include "stm32f10x.h"
#include <cstdio>

/* Definitions */
#define CLCD20x4_DEBUG (1)

/* cLCD namespace */
namespace clcd20x4_ns{

}

class clcd20x4 {
public:
	/**
	 * @brief   clcd20x4 contructor, init lcd in 4 bit mode, increase counter.
	 */
    clcd20x4(void);

private:

    /**
	 * @brief   init all GPIOs for LCD. RS, RW, EN, and all data line will be
	 * 			configured as output push pull.
	 */
	void init_gpios(void);

	/**
	 * @brief   wait until lcd is ready
	 */
	void wait_for_lcd(void);

	/**
	 * @brief   Send 4 MSB bit to LCD (only used in init).
	 *
	 * @param[in]	RS, can be cmd or data.
	 * @param[in]	RW, can be write or read.
	 * @param[in]	D7D4, only 4 MSB bit will be sent to LCD.
	 */
	void send_4bit(uint8_t RS, uint8_t RW, uint8_t D7D4);

	/**
	 * @brief   Send a byte to LCD without wait_for_ready (only used in init).
	 *
	 * @param[in]	RS, can be cmd or data.
	 * @param[in]	RW, can be write or read.
	 * @param[in]	D7D0, data will be sent to LCD.
	 */
	void send_byte_init(uint8_t RS, uint8_t RW, uint8_t D7D0);

	/**
	 * @brief   init lcd, 4 bit mode, increase counter.
	 */
	void init_lcd(void);

	/**
	 * @brief   Send a byte to LCD (only used in init).
	 *
	 * @param[in]	RS, can be cmd or data.
	 * @param[in]	RW, can be write or read.
	 * @param[in]	D7D0, data will be sent to LCD.
	 */
	void send_byte(uint8_t RS, uint8_t RW, uint8_t D7D0);

    /**
     * @brief   delay a number of ms
     *
     * @param[in]   n, number of ms to delay
     *
     * @return      none.
     * @attention	This function is hardware/OS-specific and should be thread-safe.
     */
    void delay_ms(uint16_t n);

    /**
	 * @brief   delay a number of us (at least)
	 *
	 * @param[in]   n, number of us to delay
	 *
	 * @return      none.
	 * @attention	This function is hardware/OS-specific and should be thread-safe.
	 */
    void delay_us(uint32_t n);
};


/** @} */
#endif //
