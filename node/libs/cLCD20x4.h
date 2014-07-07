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
#define CLCD20x4_DEBUG (0)

/* cLCD namespace */
namespace clcd20x4_ns{
	enum status_e: int16_t{
		successful = 0,
		failed = -1,
	};
}

class clcd20x4 {
public:
	/**
	 * @brief   clcd20x4 contructor, init lcd in 4 bit mode, increase counter.
	 */
    clcd20x4(void);

    /**
	 * @brief   set position of cursor.
	 *
	 * @param[in]	line, should be between 1 and 4.
	 * @param[in]	pos, should be between 1 and 20.
	 *
	 * @return		success or failed.
	 */
	int16_t set_cursor(uint8_t line, uint8_t pos);

	/**
	 * @brief   print a character to LCD.
	 *
	 * @param[in]	a_char, a character.
	 */
	void putc(int8_t a_char);

	/**
	 * @brief   print a string to LCD.
	 *
	 * @param[in]	a_string, pointer to a string of character.
	 *
	 * @return		number of character has been print to LCD.
	 */
	int16_t puts(int8_t *a_string);

	/**
	 * @brief   lcd's printf (only for 20x4-1 characters)
	 *
	 * @param[in]	format, in printf's style.
	 *
	 * @return		number of characters has been printed, negative value if error.
	 */
	int16_t printf(const char *format, ...);

	/**
	 * @brief   clear screen and set cursor to 1-1.
	 */
	void clear(void);

	/**
	 * @brief 	Set cursor to 1-1. (no clearing)
	 */
	void home(void);

	/**
	 * @brief 	Set cursor to 1-1 without shift. (no clearing)
	 */
	void home_woshift(void);

	/**
	 * @brief   store a custom code generated pattern to CGRAM.
	 *
	 * @param[in]	pattern, an 8-elements array.
	 * @param[in]	addr, DDRAM address for generated pattern; should be between 0 and 7.
	 * 				(CGRAM address = addr << 3).
	 */
	void gen_pattern(const uint8_t *pattern, uint8_t addr);

	/**
	 * @brief   store a custom code generated pattern to CGRAM.
	 *
	 * @param[in]	pattern, an 8-elements array.
	 * @param[in]	addr, DDRAM address for generated pattern; should be between 0 and 7.
	 * 				(CGRAM address = addr << 3).
	 */
	void set_backlight_level(uint8_t percent); //TODO: implement this function.
private:

	/* Private vars */
	uint16_t cur_line; //begin from 1
	uint16_t cur_pos; //begin from 1, too.

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
	void send_byte_wowait(uint8_t RS, uint8_t RW, uint8_t D7D0);

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
#endif // CLCD20x4_H_
