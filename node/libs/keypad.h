/**
 * @defgroup
 * @brief
 * @ingroup     libraries
 * @{
 *
 * @file        keypad.h
 * @brief       Matrix keypad driver for MBoard-1
 *
 * @author      DangNhat Pham-Huu <51002279@stu.hcmut.edu.vn>
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_

#include "stm32f10x.h"

/* Definitions */
#define KEYPAD_DEBUG (1)

/* keypad namespace */
namespace keypad_ns{
	enum status_e: int16_t {
		successful = 0,
		failed = -1,
	};

	enum key_e: int8_t {
		nokey = -1,
	};
}

class keypad {
public:
	/**
	 * @brief   keypad constructor. Init gpios, private vars.
	 */
	keypad(void);

	/**
	 * @brief   get pressed key of keypad.
	 *
	 * @param[out]	longkey, true if it has been pressed and held for a long time.
	 *
	 * @return		pressed key, keypad_ns::nokey if there is no key has been pressed.
	 */
	int8_t get_key(bool &longkey);

	/**
	 * @brief   keypad constructor. Init gpios, private vars.
	 *
	 * @param[in]	key, key need to be checked.
	 *
	 * @return		true of this key is still held by user.
	 */
	bool is_longkey_still_hold(int8_t key);

private:
	/**
	 * @brief   init gpios for keypad.
	 */
	void init_gpios(void);
};

/* keypad processing function. Should be placed in a periodically interrupted-timer ISR */
void keypad_timerISR(void);

/** @} */
#endif //
