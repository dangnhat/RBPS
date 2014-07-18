/**
 * @defgroup
 * @brief
 * @ingroup     libraries
 * @{
 *
 * @file        rbpm_gui.h
 * @brief       Node's graphical user interface of remote blood pressure monitoring system.
 *
 * @author      DangNhat Pham-Huu <51002279@stu.hcmut.edu.vn>
 */

#include "stm32f10x.h"
#include "cLCD20x4.h"

#ifndef RBPM_GUI_H_
#define RBPM_GUI_H_

/* Definitions */
#define RBPM_GUI_DEBUG (0)

/* Name space */
namespace rbpm_gui_ns {
	/* defined patterns */
	const uint8_t bat_100[8] = {0x0E, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F};
	const uint8_t bat_lt66[8] = {0x0E, 0x1F, 0x11, 0x11, 0x1F, 0x1F, 0x1F, 0x1F};
	const uint8_t bat_lt33[8] = {0x0E, 0x1F, 0x11, 0x11, 0x11, 0x1F, 0x1F, 0x1F};
	const uint8_t bat_lt5[8] = {0x0E, 0x1F, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1F};
	const uint8_t box_check[8] = {0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00}; //TODO: redefine
	const uint8_t box_wocheck[8] = {0x00, 0x00, 0x1F, 0x11, 0x11, 0x11, 0x1F, 0x00};
	const uint8_t down_arrow[8] = {0x04, 0x04, 0x04, 0x04, 0x15, 0x0E, 0x04, 0x00};

	const int8_t bat_100_code = 1;
	const int8_t bat_66_code = 2;
	const int8_t bat_33_code = 3;
	const int8_t bat_5_code = 4;
	const int8_t box_check_code = 5;
	const int8_t box_wocheck_code = 6;
	const int8_t down_arrow_code = 7;

	const uint8_t battery_pos[] = {1, 20}; /* line 1, pos 19 */

	const int8_t start_screen[] =
			"---- HCMUT ----\n"
			"Patient's id:";
	const uint8_t patient_id_pos[] = {2, 14};
	const uint8_t check_mesg_pos[] = {3, 1};
	const int8_t check_mesg[] = "Checking...";
	const int8_t check_incorrect[] = "Incorrect";

	const int8_t welcome_screen[] =
			"Welcome,\n"
			"%s\n"
			"\t1.Measure directly\n"
			"\t2.Remote monitoring";

	const int8_t measuring_mesg[] =
			"Please wait,\n"
			"Measuring...";

	const int8_t direct_screen[] =
			"-Measure directly-\n"
			"BP: %d/%d\n"
			"HR: %d\n"
			"-Prediction-%c\n"
			"Hypertension risk\t%c\n"
			"Monthly avg BP:\n"
			"\t%d/%d\n"
			"Monthly avg HR:\n"
			"\t%d\n"
			"Monthly avg BMI:\n"
			"\t%d\n"
			"-Medical history-\n"
			"Diabetes\t%c\n"
			"Dyslipidemia\t%c\n"
			"Atherosclerosis\t%c\n"
			"Gout\t%c\n";
	const uint8_t direct_screen_nrows = 16;
	const uint8_t direct_screen_ncols = 20;
	const uint8_t direct_screen_height = 4;

	const int8_t remote_screen[] =
			"-Remote monitoring-\n"
			"\tMeasure in: %d'";
	const uint8_t measure_time_pos[] = {2, 14};
};

/* class */
class rbpm_gui: public clcd20x4 {
public:
	/**
	 * @brief   rbpm_gui constructor, init private vars.
	 */
	rbpm_gui(void);

	/**
	 * @brief   print the start screen.
	 */
	void start_print_default_screen(void);

	/**
	 * @brief   print patient id.
	 *
	 * @param[in]	patient_id.
	 */
	void start_print_patient_id(uint32_t patient_id);

	/**
	 * @brief   clear patient id.
	 */
	void start_clear_patient_id(void);

	/**
	 * @brief   print checking message.
	 */
	void start_print_checking(void);

	/**
	 * @brief   print incorrect message.
	 */
	void start_print_incorrect(void);

	/**
	 * @brief   print welcome screen.
	 *
	 * @param[in]	name, patient's name.
	 */
	void welcome_print_default_screen(int8_t *name);

	/**
	 * @brief   print measuring message.
	 */
	void measuring_print_mesg(void);

	/**
	 * @brief   print result screen of direct measurement mode.
	 *
	 * @param[in]	sys, systolic pressure (mmHg).
	 * @param[in]	dias, diastolic pressure (mmHg).
	 * @param[in]	heart_rate, heart rate value/
	 * @param[in]	hypertension_risk, risk of having hypertension.
	 * @param[in]	mavg_sys, monthly average systolic pressure.
	 * @param[in]	mavg_dias, monthly average diastolic pressure.
	 * @param[in]	mavg_heart_rate, monthly average heart rate.
	 * @param[in]	mavg_bmi, monthly average BMI.
	 * @param[in]	is_diabetes.
	 * @param[in]	is_dyslipidemia.
	 * @param[in]	is_atherosclerosis.
	 * @param[in]	is_gout.
	 */
	void direct_print_default_screen(int16_t sys, int16_t dias, int16_t heart_rate,
			bool hypertension_risk, int16_t mavg_sys, int16_t mavg_dias, int16_t mavg_heart_rate,
			int16_t mavg_bmi, bool is_diabte, bool is_dyslipidemia, bool is_atherosclerosis,
			bool is_gout);

	/**
	 * @brief   scroll direct measurement result screen down one line.
	 */
	void direct_scroll_down(void);

	/**
	 * @brief   scroll direct measurement result screen up one line.
	 */
	void direct_scroll_up(void);

	/**
	 * @brief   print remote monitoring screen.
	 *
	 * @param[in]	measure_time, time in second to measure.
	 */
	void remote_print_screen(uint16_t measure_time);

	/**
	 * @brief   print only measure time in remote monitoring screen.
	 *
	 * @param[in]	measure_time, time in second to measure.
	 */
	void remote_print_measure_time(uint16_t measure_time);

	/**
	 * @brief   print battery status.
	 *
	 * @param[in]	status, battery status (%).
	 */
	void print_battery_status(uint8_t status);

private:
	/**
	 * @brief   print "4" line of buffer to screen.
	 *
	 * @param[in]	first_row, first row of buffer.
	 */
	void direct_print_buffer(uint8_t first_row);

	uint8_t cur_battery_status;

	uint8_t start_old_patient_id_strlen;
	uint8_t start_old_check_mesg_strlen;

	uint8_t remote_old_measure_time_strlen;

	int8_t direct_screen_buffer[rbpm_gui_ns::direct_screen_nrows]
	                           [rbpm_gui_ns::direct_screen_ncols + 1];
	uint8_t cur_direct_pos;
	uint8_t direct_max_rows;
};


/** @} */
#endif /* RBPM_GUI_H_ */
