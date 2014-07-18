/**
 * @defgroup
 * @brief
 * @ingroup     libraries
 * @{
 *
 * @file        bpm_inf.h
 * @brief       Interface for BPM module
 *
 * @author      viettien
 */

#ifndef BPM_INF_H_
#define BPM_INF_H_

#include "stm32f10x.h"
#include "stm32f10x_usart.h"

/* Definitions */
#define BPM_INF_DEBUG (0)

/* BPM_inf namespace */
namespace bpm_inf_ns{
	enum status_e: int16_t {
		successful = 0,
		failed = -1,
	};

	enum data_e: int8_t {
		ack_fail = -1,
	};
}

class bpm_inf {
public:
	/**
	 * @brief   bpm_inf constructor. Init interrupt, private vars.
	 * 			pc2 input for detect measuring completed
	 *			pa0 PCL control / pa1 Menu control
	 */
	bpm_inf(void);

	/**
	 * @brief   start measure blood pressure and heart rate.
	 */
	void start_measure(void);


	/**
	 * @brief   press menu button on device.
	 */
	void press_menu(void);

	/**
	 * @brief   enter pcl mode on device.
	 */
	void enter_pcl_mode(void);

	/**
	 * @brief   exit pcl mode on device.
	 */
	void exit_pcl_mode(void);

	/**
	 * @brief   test for measure is completed or not.
	 *
	 * @return	1 if complete, 0 fail.
	 */
	uint8_t measure_complete(void);

	/**
	 * @brief   get 2 bytes in memory data.
	 *
	 * @param[in]	low address and high address.
	 *
	 * @return	16( 8 high + 8 low) memory data.
	 */
	uint16_t get_memory_data(uint8_t low, uint8_t high);

	/**
	 * @brief   update blood pressure and heart rate data to private var.
	 *
	 * @param[in]	none
	 *
	 * @return	 none
	 */
	void get_measure_value(void);

	/**
	 * @brief   get blood pressure and heart rate data.
	 *
	 * @param[in]	none
	 *
	 * @return	 8bits data.
	 */
	uint8_t get_pulse_value(void);
	uint8_t get_sys_value(void);
	uint8_t get_dia_value(void);
	void test_receiv(void);
	void test(uint8_t*);
private:
	/**
	 * @brief   init bpm interface.
	 */
	void init_bpm_inf(void);


	uint8_t sys =99,dia =19 ,pulse =29;
	uint16_t oldest_id,newest_id;
};


#ifdef __cplusplus
extern "C" {
#endif

/* USART2 interrupt handler */
void USART2_IRQHandler (void);

#ifdef __cplusplus
}
#endif
/** @} */
#endif //
