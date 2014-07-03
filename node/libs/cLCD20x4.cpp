/**
 * @ingroup     libraries
 * @{
 *
 * @file        cLCD20x4.cpp
 * @brief       implementation of declared functions of object clcd20x4 class
 *
 * @author      DangNhat Pham-Huu <51002279@hcmut.edu.vn>
 *
 * @}
 */

#include "cLCD20x4.h"
#include "stdarg.h"

/* Definitions */
#if CLCD20x4_DEBUG
#define CLCD20x4_PRINTF(...) printf(__VA_ARGS__)
#else
#define CLCD20x4_PRINTF(...)
#endif

/*--------------------------- Config interface -------------------------------*/
GPIO_TypeDef *RS_port = GPIOB;
const uint16_t RS_pin = GPIO_Pin_9;
GPIOMode_TypeDef RS_mode = GPIO_Mode_Out_PP;
GPIOSpeed_TypeDef RS_speed = GPIO_Speed_2MHz;
const uint32_t RS_RCC = RCC_APB2Periph_GPIOB;

GPIO_TypeDef *RW_port = GPIOB;
const uint16_t RW_pin = GPIO_Pin_8;
GPIOMode_TypeDef RW_mode = GPIO_Mode_Out_PP;
GPIOSpeed_TypeDef RW_speed = GPIO_Speed_2MHz;
const uint32_t RW_RCC = RCC_APB2Periph_GPIOB;

GPIO_TypeDef *EN_port = GPIOB;
const uint16_t EN_pin = GPIO_Pin_7;
GPIOMode_TypeDef EN_mode = GPIO_Mode_Out_PP;
GPIOSpeed_TypeDef EN_speed = GPIO_Speed_2MHz;
const uint32_t EN_RCC = RCC_APB2Periph_GPIOB;

GPIO_TypeDef *DB4_port = GPIOA;
const uint16_t DB4_pin = GPIO_Pin_15;
GPIOMode_TypeDef DB4_mode = GPIO_Mode_Out_PP;
GPIOSpeed_TypeDef DB4_speed = GPIO_Speed_2MHz;
const uint32_t DB4_RCC = RCC_APB2Periph_GPIOA;

GPIO_TypeDef *DB5_port = GPIOB;
const uint16_t DB5_pin = GPIO_Pin_3;
GPIOMode_TypeDef DB5_mode = GPIO_Mode_Out_PP;
GPIOSpeed_TypeDef DB5_speed = GPIO_Speed_2MHz;
const uint32_t DB5_RCC = RCC_APB2Periph_GPIOB;

GPIO_TypeDef *DB6_port = GPIOB;
const uint16_t DB6_pin = GPIO_Pin_10;
GPIOMode_TypeDef DB6_mode = GPIO_Mode_Out_PP;
GPIOSpeed_TypeDef DB6_speed = GPIO_Speed_2MHz;
const uint32_t DB6_RCC = RCC_APB2Periph_GPIOB;

GPIO_TypeDef *DB7_port = GPIOB;
const uint16_t DB7_pin = GPIO_Pin_11;
GPIOMode_TypeDef DB7_mode[] = {GPIO_Mode_Out_PP, GPIO_Mode_IN_FLOATING}; //0 for output, 1 for input
GPIOSpeed_TypeDef DB7_speed = GPIO_Speed_2MHz;
const uint32_t DB7_RCC = RCC_APB2Periph_GPIOB;

void (*GPIOs_RCC_fp)(uint32_t, FunctionalState) = RCC_APB2PeriphClockCmd;

/*--------------------- LCD-specific data, definitions -----------------------*/
/* define cLCD's pin state */
enum RS_state_e: uint8_t {cmd = 0, data = 1};
enum RW_state_e: uint8_t {write = 0, read = 1};

enum DB7_mode_e: uint8_t {out = 0, in = 1};
/* end define pin state */

/** define cLCD's instructions **/
enum instruction_e : uint8_t {

/* clear Display :
 * Write 20H to DDRAM and set DDRAM address to 00h from AC.
 * RS = 0, RW = 0.
 * 1.5 ms.
 */
cLCD_cmd_ClearDisplay = 0x01,

/* return Home :
 * set DDRAM address to 00h from AC, return cursor to it's original location if shifted (contents of DDRAM not changed).
 * RS = 0, RW = 0.
 * 1.5 ms.
 */
cLCD_cmd_ReturnHome = 0x02,

/* Entry mode set :
 * Set cursor move direction and specifies display shift.
 * RS = 0, RW = 0.
 * 37 us.
 */
cLCD_cmd_EntryMode_inc = 0x06,
cLCD_cmd_EntryMode_dec = 0x04,
cLCD_cmd_EntryMode_SL = 0x07,
cLCD_cmd_EntryMode_SR = 0x05,

/* Display ON/OFF :
 * RS = 0, RW = 0, 0 0 0 0 1 D C B
 * D : display on (1) / off (0)
 * C : cursor on (1) / off (0)
 * B : cursor blink on (1) / off (0)
 * 37 us
 */
cLCD_cmd_DCB_off = 0x08,
cLCD_cmd_D_on = 0x0C,
cLCD_cmd_C_on = 0x0A,
cLCD_cmd_B_on = 0x09,

/* Cursor or display shift :
 * shift cursor/display left/right
 * RS = 0, RW = 0, 0 0 0 1 S/C R/L x x.
 * 37 us.
 */
cLCD_cmd_cursorSL = 0x10, // AC-1
cLCD_cmd_cursorSR = 0x14, // AC+1
cLCD_cmd_displaySL = 0x18, // AC = AC
cLCD_cmd_displaySR = 0x1C, // AC = AC

/* Function set :
 * RS = 0, RW = 0, 0 0 1 DL N F x x
 * DL : 1 - 8bit, 0 - 4bit.
 * N : 1 - 2 line, 0 - 1 line.
 * F : 1 - 5x11 dots, 0 - 5x8 dots.
 * 37 us.
 */
cLCD_cmd_FS_4bit_2line = 0x28,

/* Set CGRAM Address :
 * RS = 0, RW = 0, 0 1 AC5 AC4 AC3 AC2 AC1 AC0.
 * Set CGRAM address to Ac.
 * 37us.
 */

/* Set DDRAM Address :
 * RS = 0, RW = 0, 1 AC6 AC5 AC4 AC3 AC2 AC1 AC0
 * Set DDRAM address to Ac.
 * 1 line mode: ?
 * 2 line mode:
 * 00 - 13 (line 1, pos 1-20)
 * 40 - 53 (line 2)
 * 14 - 27 (line 3)
 * 54 - 67 (line 4)
 * 37 us.
 */
cLCD_DDRAM_line1 = 0x00,
cLCD_DDRAM_line2 = 0x40,
cLCD_DDRAM_line3 = 0x14,
cLCD_DDRAM_line4 = 0x54,

/* Read busy flag and AC
 * RS = 0, RW = 1, 0us, output BF AC6 - AC0
 * BF = 0 : not busy.
 */

/* Write data to RAM :
 * RS = 1, RW = 1, D7-D0 : data.
 * After write AC +/- 1.
 */

/* Read data from RAM */
};

/*----------------------------Public functions--------------------------------*/
clcd20x4::clcd20x4(void) {
	/* Init GPIOs */
	init_gpios();

	/* Init lcd */
	init_lcd();

	/* Init private vars */
	cur_line = 1;
	cur_pos = 1;
}

/*----------------------------------------------------------------------------*/
int16_t clcd20x4::set_cursor(uint8_t line, uint8_t pos) {
	uint8_t ddram_addr = 0;

	/* check line and pos */
	if ((line < 1) || (line > 4)) {
		return clcd20x4_ns::failed;
	}

	if ((pos < 1) || (pos > 20)) {
		return clcd20x4_ns::failed;
	}

	/* calculate DDRAM address */
	switch (line) {
	case 1:
		ddram_addr = cLCD_DDRAM_line1 + pos - 1;
		break;
	case 2:
		ddram_addr = cLCD_DDRAM_line2 + pos - 1;
		break;
	case 3:
		ddram_addr = cLCD_DDRAM_line3 + pos - 1;
		break;
	case 4:
		ddram_addr = cLCD_DDRAM_line4 + pos - 1;
		break;
	default:
		return clcd20x4_ns::failed;
	}

	/* send set DDRAM address command */
	send_byte(cmd, write, ddram_addr | 0x80);

	/* save state */
	cur_line = line;
	cur_pos = pos;

	return clcd20x4_ns::successful;
}

/*----------------------------------------------------------------------------*/
void clcd20x4::clear(void) {
	/* send clear command */
	send_byte(cmd, write, cLCD_cmd_ClearDisplay);

	/* save state */
	cur_line = 1;
	cur_pos = 1;
}

/*----------------------------------------------------------------------------*/
void clcd20x4::home(void) {
	/* send clear command */
	send_byte(cmd, write, cLCD_cmd_ReturnHome);

	/* save state */
	cur_line = 1;
	cur_pos = 1;
}

/*----------------------------------------------------------------------------*/
void clcd20x4::putc(int8_t a_char) {
	/* send data */
	send_byte(data, write, a_char);

	/* save state (only correct when in 2 line mode) */
	cur_pos++;
	if (cur_pos > 20) {
		cur_pos = 1;

		cur_line = cur_line + 1;
		if (cur_line == 5) {
			cur_line = 1;
		}

		set_cursor(cur_line, cur_pos);
	}
}

/*----------------------------------------------------------------------------*/
int16_t clcd20x4::puts(int8_t* a_string) {
	int16_t num_char = 0;

	while (*a_string != '\0') {
		putc(*a_string++);
		num_char++;
	}

	return num_char;
}

/*----------------------------------------------------------------------------*/
int16_t clcd20x4::printf(const char *format, ...) {
	int8_t buffer[20*4];
	int16_t retval;
	va_list args;

	va_start(args, format);
	vsnprintf((char*)buffer, 80, format, args);
	va_end(args);

	/* print buffer to lcd */
	retval = puts(buffer);

	return retval;
}

/*----------------------------Private functions-------------------------------*/
void clcd20x4::wait_for_lcd(void) {
	GPIO_InitTypeDef gpio_init_s;
	gpio_init_s.GPIO_Pin = DB7_pin;
	gpio_init_s.GPIO_Speed = DB7_speed;

	/* wait at least 80us before checking BF */
	delay_us(80);

	/* init DB7 to input */
	gpio_init_s.GPIO_Mode = DB7_mode[in];
	GPIO_Init(DB7_port, &gpio_init_s);

	/* check BF */
	cmd ? GPIO_SetBits(RS_port, RS_pin) : GPIO_ResetBits(RS_port, RS_pin);
	read ? GPIO_SetBits(RW_port, RW_pin) : GPIO_ResetBits(RW_port, RW_pin);

	/* addition delay */
	delay_us(1);

	while (1){
		/* EN = 1 */
		GPIO_SetBits(EN_port, EN_pin);

		/* Tpw = 480 ns */
		delay_us(1);

		/* check BF */
		if(GPIO_ReadInputDataBit(DB7_port, DB7_pin) == 0){
			break;
		}

		/* EN = 0 */
		GPIO_ResetBits(EN_port, EN_pin);
	}/* end while : cLCD is ready */

	/* EN = 0 */
	GPIO_ResetBits(EN_port, EN_pin);

	/* init DB7 to output */
	gpio_init_s.GPIO_Mode = DB7_mode[out];
	GPIO_Init(DB7_port, &gpio_init_s);
}

/*----------------------------------------------------------------------------*/
void clcd20x4::send_byte(uint8_t RS, uint8_t RW, uint8_t D7D0) {
	/* wait for lcd is not busy */
	wait_for_lcd();

	/* send data */
	send_byte_wowait(RS, RW, D7D0);
}

/*----------------------------------------------------------------------------*/
void clcd20x4::init_lcd(void) {
	/* wait for >40ms */
	delay_us(100000);

	/* send function set 1 */
	send_4bit(cmd, write, 0x30);
	/* wait for >37us */
	delay_us(50);

	/* send  function set 2 */
	send_byte_wowait(cmd, write, cLCD_cmd_FS_4bit_2line);
	/* wait for >37us */
	delay_us(50);

	/* send  function set 3 */
	send_byte_wowait(cmd, write, cLCD_cmd_FS_4bit_2line);
	/* wait for > 37us */
	delay_us(50);

	/* send display control, Display on */
	send_byte(cmd, write, cLCD_cmd_D_on);

	/* send display clear, clear all DDRAM to 20H, set DDRAM address to 0 from AC */
	send_byte(cmd, write, cLCD_cmd_ClearDisplay);

	/* send Entry mode set, AC++ */
	send_byte(cmd, write, cLCD_cmd_EntryMode_inc);

	/* additional delay */
	delay_us(1000);
}

/*----------------------------------------------------------------------------*/
void clcd20x4::send_4bit(uint8_t RS, uint8_t RW, uint8_t D7D4) {

	RS ? GPIO_SetBits(RS_port, RS_pin) : GPIO_ResetBits(RS_port, RS_pin);
	RW ? GPIO_SetBits(RW_port, RW_pin) : GPIO_ResetBits(RW_port, RW_pin);

	/* addition delay */
	delay_us(1);

	/* EN = 1 */
	GPIO_SetBits(EN_port, EN_pin);

	/* Send data */
	((D7D4 & 0x80) >> 7) ? GPIO_SetBits(DB7_port, DB7_pin) : GPIO_ResetBits(DB7_port, DB7_pin);
	((D7D4 & 0x40) >> 6) ? GPIO_SetBits(DB6_port, DB6_pin) : GPIO_ResetBits(DB6_port, DB6_pin);
	((D7D4 & 0x20) >> 5) ? GPIO_SetBits(DB5_port, DB5_pin) : GPIO_ResetBits(DB5_port, DB5_pin);
	((D7D4 & 0x10) >> 4) ? GPIO_SetBits(DB4_port, DB4_pin) : GPIO_ResetBits(DB4_port, DB4_pin);

	/* addition delay */
	delay_us(1);

	/* EN = 0 */
	GPIO_ResetBits(EN_port, EN_pin);
}

/*----------------------------------------------------------------------------*/
void clcd20x4::send_byte_wowait(uint8_t RS, uint8_t RW, uint8_t D7D0) {
	RS ? GPIO_SetBits(RS_port, RS_pin) : GPIO_ResetBits(RS_port, RS_pin);
	RW ? GPIO_SetBits(RW_port, RW_pin) : GPIO_ResetBits(RW_port, RW_pin);

	/* addition delay */
	delay_us(1);

	/* Send 4 MSB bits D7-D4 */
	GPIO_SetBits(EN_port, EN_pin);

	((D7D0 & 0x80) >> 7) ? GPIO_SetBits(DB7_port, DB7_pin) : GPIO_ResetBits(DB7_port, DB7_pin);
	((D7D0 & 0x40) >> 6) ? GPIO_SetBits(DB6_port, DB6_pin) : GPIO_ResetBits(DB6_port, DB6_pin);
	((D7D0 & 0x20) >> 5) ? GPIO_SetBits(DB5_port, DB5_pin) : GPIO_ResetBits(DB5_port, DB5_pin);
	((D7D0 & 0x10) >> 4) ? GPIO_SetBits(DB4_port, DB4_pin) : GPIO_ResetBits(DB4_port, DB4_pin);

	/* addition delay */
	delay_us(1);

	GPIO_ResetBits(EN_port, EN_pin);

	/* tEC > 1.2 us */
	delay_us(2);

	/* Send next 4 bits D3-D0 */
	GPIO_SetBits(EN_port, EN_pin);

	((D7D0 & 0x08) >> 3) ? GPIO_SetBits(DB7_port, DB7_pin) : GPIO_ResetBits(DB7_port, DB7_pin);
	((D7D0 & 0x04) >> 2) ? GPIO_SetBits(DB6_port, DB6_pin) : GPIO_ResetBits(DB6_port, DB6_pin);
	((D7D0 & 0x02) >> 1) ? GPIO_SetBits(DB5_port, DB5_pin) : GPIO_ResetBits(DB5_port, DB5_pin);
	(D7D0 & 0x01) ? GPIO_SetBits(DB4_port, DB4_pin) : GPIO_ResetBits(DB4_port, DB4_pin);

	/* addition delay */
	delay_us(1);

	GPIO_ResetBits(EN_port, EN_pin);
}

/*----------------------------------------------------------------------------*/
void clcd20x4::init_gpios(void) {
	GPIO_InitTypeDef gpio_init_s;

	/* Init RS, RW, EN */
	gpio_init_s.GPIO_Mode = RS_mode;
	gpio_init_s.GPIO_Speed = RS_speed;
	gpio_init_s.GPIO_Pin = RS_pin;

	GPIOs_RCC_fp(RS_RCC, ENABLE);
	GPIO_Init(RS_port, &gpio_init_s);

	gpio_init_s.GPIO_Mode = RW_mode;
	gpio_init_s.GPIO_Speed = RW_speed;
	gpio_init_s.GPIO_Pin = RW_pin;

	GPIOs_RCC_fp(RW_RCC, ENABLE);
	GPIO_Init(RW_port, &gpio_init_s);

	gpio_init_s.GPIO_Mode = EN_mode;
	gpio_init_s.GPIO_Speed = EN_speed;
	gpio_init_s.GPIO_Pin = EN_pin;

	GPIOs_RCC_fp(EN_RCC, ENABLE);
	GPIO_Init(EN_port, &gpio_init_s);

	/* Init data lines */
	gpio_init_s.GPIO_Mode = DB7_mode[out];
	gpio_init_s.GPIO_Speed = DB7_speed;
	gpio_init_s.GPIO_Pin = DB7_pin;

	GPIOs_RCC_fp(DB7_RCC, ENABLE);
	GPIO_Init(DB7_port, &gpio_init_s);

	gpio_init_s.GPIO_Mode = DB6_mode;
	gpio_init_s.GPIO_Speed = DB6_speed;
	gpio_init_s.GPIO_Pin = DB6_pin;

	GPIOs_RCC_fp(DB6_RCC, ENABLE);
	GPIO_Init(DB6_port, &gpio_init_s);

	gpio_init_s.GPIO_Mode = DB5_mode;
	gpio_init_s.GPIO_Speed = DB5_speed;
	gpio_init_s.GPIO_Pin = DB5_pin;

	GPIOs_RCC_fp(DB5_RCC, ENABLE);
	GPIO_Init(DB5_port, &gpio_init_s);

	gpio_init_s.GPIO_Mode = DB4_mode;
	gpio_init_s.GPIO_Speed = DB4_speed;
	gpio_init_s.GPIO_Pin = DB4_pin;

	GPIOs_RCC_fp(DB4_RCC, ENABLE);
	GPIO_Init(DB4_port, &gpio_init_s);
}

/*----------------------------------------------------------------------------*/
void clcd20x4::delay_us(uint32_t n) {
	uint32_t num_of_clockcycles = SystemCoreClock/1000000 * n;
	uint32_t count;

	for (count = 0; count < num_of_clockcycles; count++) {
		__asm__ __volatile__("nop");
	}
}
