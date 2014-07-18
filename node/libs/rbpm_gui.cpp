/**
 * @ingroup     libraries
 * @{
 *
 * @file        rbpm_gui.cpp
 * @brief       implementation of declared functions of rbpm_gui class.
 *
 * @author      DangNhat Pham-Huu <51002279@hcmut.edu.vn>
 *
 * @}
 */

#include "rbpm_gui.h"
#include "string.h"

/* Definitions */
#if RBPM_GUI_DEBUG
#define RBPM_GUI_PRINTF(...) printf(__VA_ARGS__)
#else
#define RBPM_GUI_PRINTF(...)
#endif

/*--------------------------- Public methods ---------------------------------*/
rbpm_gui::rbpm_gui(void) {
	/* Init private vars */
	cur_battery_status = 100;
	cur_direct_pos = 0;
	start_old_patient_id_strlen = 0;
	start_old_check_mesg_strlen = 0;
	direct_max_rows = 0;

	/* generate patterns */
	gen_pattern(rbpm_gui_ns::bat_100, rbpm_gui_ns::bat_100_code);
	gen_pattern(rbpm_gui_ns::bat_lt66, rbpm_gui_ns::bat_66_code);
	gen_pattern(rbpm_gui_ns::bat_lt33, rbpm_gui_ns::bat_33_code);
	gen_pattern(rbpm_gui_ns::bat_lt5, rbpm_gui_ns::bat_5_code);
	gen_pattern(rbpm_gui_ns::box_check, rbpm_gui_ns::box_check_code);
	gen_pattern(rbpm_gui_ns::box_wocheck, rbpm_gui_ns::box_wocheck_code);
	gen_pattern(rbpm_gui_ns::down_arrow, rbpm_gui_ns::down_arrow_code);
}

/*----------------------------------------------------------------------------*/
void rbpm_gui::start_print_default_screen(void) {
	/* clear lcd */
	clear();

	/* print battery */
	print_battery_status(cur_battery_status);

	/* set default cursor position */
	home_woshift();

	/* print screen */
	this->printf("%s", rbpm_gui_ns::start_screen);
}

/*----------------------------------------------------------------------------*/
void rbpm_gui::start_print_patient_id(uint32_t patient_id) {
	uint8_t count;
	int8_t patient_id_str[16];

	/* set cursor */
	set_cursor(rbpm_gui_ns::patient_id_pos[0], rbpm_gui_ns::patient_id_pos[1]);

	/* clear old patient_id */
	for (count = 0; count < start_old_patient_id_strlen; count++) {
		this->printf(" ");
	}

	/* set cursor */
	set_cursor(rbpm_gui_ns::patient_id_pos[0], rbpm_gui_ns::patient_id_pos[1]);

	/* print  patient_id */
	this->printf("%d", patient_id);

	/* save patient_id strlen*/
	snprintf((char *)patient_id_str, 16, "%lu", patient_id);
	start_old_patient_id_strlen = strlen((const char *)patient_id_str);

}

/*----------------------------------------------------------------------------*/
void rbpm_gui::start_clear_patient_id(void) {
	uint8_t count;

	/* set cursor */
	set_cursor(rbpm_gui_ns::patient_id_pos[0], rbpm_gui_ns::patient_id_pos[1]);

	/* clear old patient_id */
	for (count = 0; count < start_old_patient_id_strlen; count++) {
		this->printf(" ");
	}

	/* save patient_id strlen*/
	start_old_patient_id_strlen = 0;
}
/*----------------------------------------------------------------------------*/
void rbpm_gui::start_print_checking(void) {
	uint8_t count;
	int8_t check_msg_str[16];

	/* set cursor */
	set_cursor(rbpm_gui_ns::check_mesg_pos[0], rbpm_gui_ns::check_mesg_pos[1]);

	/* clear old message */
	for (count = 0; count < start_old_check_mesg_strlen; count++) {
		this->printf(" ");
	}

	/* set cursor */
	set_cursor(rbpm_gui_ns::check_mesg_pos[0], rbpm_gui_ns::check_mesg_pos[1]);

	/* print checking message */
	this->printf("%s", rbpm_gui_ns::check_mesg);

	/* save checking message strlen */
	snprintf((char *)check_msg_str, 16, "%s", rbpm_gui_ns::check_mesg);
	start_old_check_mesg_strlen = strlen((const char *)check_msg_str);
}

/*----------------------------------------------------------------------------*/
void rbpm_gui::start_print_incorrect(void) {
	uint8_t count;

	/* set cursor */
	set_cursor(rbpm_gui_ns::check_mesg_pos[0], rbpm_gui_ns::check_mesg_pos[1]);

	/* clear old message */
	for (count = 0; count < start_old_check_mesg_strlen; count++) {
		this->printf(" ");
	}

	/* set cursor */
	set_cursor(rbpm_gui_ns::check_mesg_pos[0], rbpm_gui_ns::check_mesg_pos[1]);

	/* print checking message */
	this->printf("%s", rbpm_gui_ns::check_incorrect);

	/* save checking message strlen */
	start_old_check_mesg_strlen = strlen((const char *)rbpm_gui_ns::check_incorrect);
}

/*----------------------------------------------------------------------------*/
void rbpm_gui::welcome_print_default_screen(int8_t *name) {
	/* clear */
	clear();

	/* print battery status */
	print_battery_status(cur_battery_status);

	/* set default cursor position */
	home_woshift();

	/* print welcome message */
	this->printf((const char *)rbpm_gui_ns::welcome_screen, name);
}

/*----------------------------------------------------------------------------*/
void rbpm_gui::measuring_print_mesg(void) {
	/* clear */
	clear();

	/* print battery status */
	print_battery_status(cur_battery_status);

	/* set default cursor position */
	home_woshift();

	/* print welcome message */
	this->printf((const char *)rbpm_gui_ns::measuring_mesg);
}

/*----------------------------------------------------------------------------*/
void rbpm_gui::direct_print_default_screen(int16_t sys, int16_t dias, int16_t heart_rate,
			bool hypertension_risk, int16_t mavg_sys, int16_t mavg_dias, int16_t mavg_heart_rate,
			int16_t mavg_bmi, bool is_diabte, bool is_dyslipidemia, bool is_atherosclerosis,
			bool is_gout) {
	int8_t temp_str[(rbpm_gui_ns::direct_screen_ncols+1)*rbpm_gui_ns::direct_screen_nrows];
	uint8_t count, cur_row;
	int8_t *strp;

	/* clear */
	clear();

	/* print battery status */
	print_battery_status(cur_battery_status);

	/* set default cursor position */
	home_woshift();

	/* print to buffer */
	snprintf((char *)temp_str,
			((rbpm_gui_ns::direct_screen_ncols+1)*rbpm_gui_ns::direct_screen_nrows - 1),
			(const char *)rbpm_gui_ns::direct_screen,
			sys, dias, heart_rate,
			rbpm_gui_ns::down_arrow_code,
			hypertension_risk ? rbpm_gui_ns::box_check_code : rbpm_gui_ns::box_wocheck_code,
			mavg_sys, mavg_dias, mavg_heart_rate, mavg_bmi,
			is_diabte ? rbpm_gui_ns::box_check_code : rbpm_gui_ns::box_wocheck_code,
			is_dyslipidemia ? rbpm_gui_ns::box_check_code : rbpm_gui_ns::box_wocheck_code,
			is_atherosclerosis ? rbpm_gui_ns::box_check_code : rbpm_gui_ns::box_wocheck_code,
			is_gout ? rbpm_gui_ns::box_check_code : rbpm_gui_ns::box_wocheck_code);

	strp = temp_str;
	count = 0;
	cur_row = 0;
	while (*strp != '\0') {
		if (*strp == '\n') {
			memcpy(direct_screen_buffer[cur_row], strp-count, count+1); // include '\n'

			strp++;
			count = 0;
			cur_row++;
			continue;
		}

		strp++;
		count++;
	}

	/* save cur_row */
	direct_max_rows = cur_row;

	/* set cur_direct_pos and print buffer to screen */
	cur_direct_pos = 0;
	direct_print_buffer(cur_direct_pos);
}

/*----------------------------------------------------------------------------*/
void rbpm_gui::direct_scroll_down(void) {
	if (cur_direct_pos + 1 + rbpm_gui_ns::direct_screen_height <= direct_max_rows) {
		cur_direct_pos++;
		direct_print_buffer(cur_direct_pos);
	}
}

/*----------------------------------------------------------------------------*/
void rbpm_gui::direct_scroll_up(void) {
	if (cur_direct_pos > 0) {
		cur_direct_pos--;
		direct_print_buffer(cur_direct_pos);
	}
}

/*----------------------------------------------------------------------------*/
void rbpm_gui::remote_print_screen(uint16_t measure_time) {
	/* clear */
	clear();

	/* print battery status */
	print_battery_status(cur_battery_status);

	/* set default cursor position */
	home_woshift();

	/* print remote monitoring screen */
	this->printf((const char *)rbpm_gui_ns::remote_screen, measure_time);
}

/*----------------------------------------------------------------------------*/
void rbpm_gui::remote_print_measure_time(uint16_t measure_time) {
	uint8_t count;
	int8_t measure_time_str[16];

	/* set cursor */
	set_cursor(rbpm_gui_ns::measure_time_pos[0], rbpm_gui_ns::measure_time_pos[1]);

	/* clear old patient_id */
	for (count = 0; count < remote_old_measure_time_strlen; count++) {
		this->printf(" ");
	}

	/* set cursor */
	set_cursor(rbpm_gui_ns::measure_time_pos[0], rbpm_gui_ns::measure_time_pos[1]);

	/* print  patient_id */
	this->printf("%d", measure_time);

	/* save patient_id strlen*/
	snprintf((char *)measure_time_str, 16, "%u", measure_time);
	remote_old_measure_time_strlen = strlen((const char *)measure_time_str);

}

/*----------------------------------------------------------------------------*/
void rbpm_gui::print_battery_status(uint8_t status) {
	/* set cursor */
	set_cursor(rbpm_gui_ns::battery_pos[0], rbpm_gui_ns::battery_pos[1]);

	if (status > 66) {
		this->printf("%c", rbpm_gui_ns::bat_100_code);
	}
	else if (status > 33) {
		this->printf("%c", rbpm_gui_ns::bat_66_code);
	}
	else if (status > 5) {
		this->printf("%c", rbpm_gui_ns::bat_33_code);
	}
	else {
		this->printf("%c", rbpm_gui_ns::bat_5_code);
	}

	/* save battery status */
	cur_battery_status = status;
}

/*---------------------------- Private method --------------------------------*/
void rbpm_gui::direct_print_buffer(uint8_t first_row) {
	uint8_t count;

	/* clear */
	clear();

	/* print battery status */
	print_battery_status(cur_battery_status);

	/* set default cursor position */
	home_woshift();

	for (count = first_row; count < first_row+rbpm_gui_ns::direct_screen_height; count++) {
		this->printf("%s", direct_screen_buffer[count]);
	}
}
