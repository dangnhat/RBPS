/**
 * @ingroup     libraries
 * @{
 *
 * @file        keypad.cpp
 * @brief       implementation of matrix keypad class (always using a decoder such as
 * 				74HC138).
 * 				row_in0
 * 				row_in1 ---> decoder ---> 2^n rows.
 * 				...
 * 				row_inn
 *
 * 				col0
 * 				...		<--- n cols
 * 				coln
 *
 * @author      DangNhat Pham-Huu <51002279@hcmut.edu.vn>
 *
 * @}
 */

#include "keypad.h"

/* Definitions */
#if KEYPAD_DEBUG
#define KEYPAD_PRINTF(...) printf(__VA_ARGS__)
#else
#define KEYPAD_PRINTF(...)
#endif

/*--------------------------- Config interface -------------------------------*/
const bool keypad_active_state = 0;
const uint8_t keypad_sampling_timecycle = 10; /* *time cycle of timer */
const uint8_t keypad_long_pressed_time = 100; /* *keypad_sampling_timecycle */

const uint8_t num_of_row_in = 2;
GPIO_TypeDef *row_in_port[num_of_row_in] = {GPIOC, GPIOC};
const uint16_t row_in_pin[num_of_row_in] = {GPIO_Pin_0, GPIO_Pin_1};
const GPIOMode_TypeDef row_in_mode[num_of_row_in] = {GPIO_Mode_Out_PP, GPIO_Mode_Out_PP};
const GPIOSpeed_TypeDef row_in_speed[num_of_row_in] = {GPIO_Speed_2MHz, GPIO_Speed_2MHz};
const uint32_t row_in_RCC[num_of_row_in] = {RCC_APB2Periph_GPIOC, RCC_APB2Periph_GPIOC};

const uint8_t num_of_col = 4;
GPIO_TypeDef *col_port[num_of_col] = {GPIOB, GPIOB, GPIOB, GPIOB};
const uint16_t col_pin[num_of_col] = {GPIO_Pin_6, GPIO_Pin_7, GPIO_Pin_8, GPIO_Pin_9};
const GPIOMode_TypeDef col_mode[num_of_col] = {GPIO_Mode_IN_FLOATING, GPIO_Mode_IN_FLOATING,\
												GPIO_Mode_IN_FLOATING, GPIO_Mode_IN_FLOATING};
const GPIOSpeed_TypeDef col_speed[num_of_col] = {GPIO_Speed_2MHz, GPIO_Speed_2MHz,\
												GPIO_Speed_2MHz, GPIO_Speed_2MHz};
const uint32_t col_RCC[num_of_col] = {RCC_APB2Periph_GPIOB, RCC_APB2Periph_GPIOB,\
										RCC_APB2Periph_GPIOB, RCC_APB2Periph_GPIOB};

static void (*GPIOs_RCC_fp)(uint32_t, FunctionalState) = RCC_APB2PeriphClockCmd;

/*--------------------------- Static vars ------------------------------------*/
static int8_t key_read1 = keypad_ns::nokey;
static int8_t key_read2 = keypad_ns::nokey;
static int8_t key_read3 = keypad_ns::nokey;
static int8_t key_old = keypad_ns::nokey;

static bool is_newkey = false;
static int8_t newkey = keypad_ns::nokey;
static bool is_longkey = false;

static uint8_t timecycle_count = 0;
static uint8_t longkey_count = 0;

/*--------------------------- Processing functions ---------------------------*/
static void keypad_processing(void);
static void keypad_set_rows_in(uint8_t decoder_input);
static int8_t keypad_get_cols(void);

/*--------------------------- Public methods ---------------------------------*/
keypad::keypad(void) {
	/* init gpio */
	init_gpios();
}

/*----------------------------------------------------------------------------*/
int8_t keypad::get_key(bool &longkey) {
	if (is_newkey) {
		is_newkey = false;
		longkey = false;
		return newkey;
	}
	else if (is_longkey) {
		longkey = true;
		return newkey;
	}
	else {
		return keypad_ns::nokey;
	}
}

/*----------------------------------------------------------------------------*/
bool keypad::is_longkey_still_hold(int8_t key) {
	if (newkey == key && is_longkey == true) {
		return true;
	}
	else {
		return false;
	}
}

/*--------------------------- Private methods --------------------------------*/
void keypad::init_gpios(void) {
	GPIO_InitTypeDef gpio_init_s;
	uint8_t count;

	for (count = 0; count < num_of_row_in; count++) {
		gpio_init_s.GPIO_Mode = row_in_mode[count];
		gpio_init_s.GPIO_Pin = row_in_pin[count];
		gpio_init_s.GPIO_Speed = row_in_speed[count];

		GPIOs_RCC_fp(row_in_RCC[count], ENABLE);
		GPIO_Init(row_in_port[count], &gpio_init_s);
	}

	for (count = 0; count < num_of_col; count++) {
		gpio_init_s.GPIO_Mode = col_mode[count];
		gpio_init_s.GPIO_Pin = col_pin[count];
		gpio_init_s.GPIO_Speed = col_speed[count];

		GPIOs_RCC_fp(col_RCC[count], ENABLE);
		GPIO_Init(col_port[count], &gpio_init_s);
	}
}

/*--------------------------- Processing functions impl ----------------------*/
void keypad_timerISR(void) {
	timecycle_count = (timecycle_count + 1) % keypad_sampling_timecycle;

	if (timecycle_count == (keypad_sampling_timecycle - 1)){
		/* sampling and processing buttons */
		keypad_processing();
	}
}

void keypad_set_rows_in(uint8_t decoder_input) {
	uint8_t count;

	for (count = 0; count < num_of_row_in; count++) {
		if ((decoder_input & (0x01 << count)) != 0) {
			GPIO_SetBits(row_in_port[count], row_in_pin[count]);
		}
		else {
			GPIO_ResetBits(row_in_port[count], row_in_pin[count]);
		}
	}
}

int8_t keypad_get_cols(void) {
	uint8_t count;
	int8_t col_value = keypad_ns::nokey;

	for (count = 0; count < num_of_col; count++) {
		if (GPIO_ReadInputDataBit(col_port[count], col_pin[count]) == keypad_active_state) {
			col_value = count;
		}
	}

	return col_value;
}

void keypad_processing(void) {
	uint8_t row;
	int8_t key_read;
	int8_t col_value;

	/* sampling */
	key_read = keypad_ns::nokey;

	for (row = 0; row < (0x01 << num_of_row_in); row++) {
		keypad_set_rows_in(row);

		col_value = keypad_get_cols();
		if(col_value != keypad_ns::nokey) {
			key_read = row * num_of_col + col_value;
			break;
		}
	}

	key_read3 = key_read2;
	key_read2 = key_read1;
	key_read1 = key_read;

	/* update longTimePressed_count value */
	if (longkey_count != 0){

		longkey_count--;

		if (longkey_count == 0) { // time out, long_key_pressed.
			is_longkey = true;
			newkey = key_old;
		}
	}

	/* processing */
	if ( (key_read1 == key_read2) && (key_read2 == key_read3) ) { /* new stable state */

		if (key_read1 != key_old){ /* change state */

			if (key_read1 != keypad_ns::nokey) { /* change from inactive->active */
				key_old = key_read1;

				longkey_count = keypad_long_pressed_time; //set time out value
			}

			if (key_read1 == keypad_ns::nokey) { /* change from active->inactive */

				if (longkey_count > 0) { // not time out yet, new key.
					is_newkey = true;
					newkey = key_old;
				}

				else { // long time pressed key end.
					is_longkey = false;
					newkey = keypad_ns::nokey;
				}

				key_old = key_read1;
				longkey_count = 0;
			}

		}/* end if */

	}/* end if */
}
