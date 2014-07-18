/*
 * bat_inf.h
 *
 * @ingroup     libraries
 *  Created on: Jul 15, 2014
 *      Author: viettien
 */

#ifndef BAT_INF_H_
#define BAT_INF_H_
#include "stm32f10x.h"
#include "stm32f10x_adc.h"

/* Definitions */
#define BAT_INF_DEBUG (0)

/* bat_inf namespace */
namespace bat_inf_ns{
	enum status_e: int16_t {
		successful = 0,
		failed = -1,
	};
}

class bat_inf {
public:
	/**
	 * @brief   bat_inf constructor. Init port, private vars...
	 */
	bat_inf(void);

	/**
	 * @brief   bat_inf constructor. Init port, private vars...
	 *
	 * @param[in]	none
	 *
	 * @return	8bit data. 0 to 100
	 */
	uint8_t get_bat_percent(void);

	/**
	 * @brief   get battery adc value
	 *
	 * @param[in]	none
	 *
	 * @return	16bit data.
	 */
	uint16_t get_bat_adc(void);


private:
	/**
	 * @brief   init bat interface.
	 */
	void init_bat_inf(void);


};




#endif /* BAT_INF_H_ */
