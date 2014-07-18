/*
 * bat_inf.cpp
 *
 *  Created on: Jul 15, 2014
 *      Author: viettien
 */
#include "bat_inf.h"
#include "coocox.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x.h"

/* Definitions */
#if BAT_INF_DEBUG
#define BAT_INF_PRINTF(...) printf(__VA_ARGS__)
#else
#define BAT_INF_PRINTF(...)
#endif


/*--------------------------- Config interface -------------------------------*/

/*--------------------------- Static vars ------------------------------------*/

__IO uint16_t ADCConvertedValue;
/*--------------------------- Processing functions ---------------------------*/


/*--------------------------- Public methods ---------------------------------*/
bat_inf::bat_inf(void) {
	/* init gpio */
	init_bat_inf();
}

/*----------------------------------------------------------------------------*/
uint8_t bat_inf::get_bat_percent(void)
{
	uint16_t temp = 0;
	if ( ADCConvertedValue >= 2430)
		return (100);
	if ( ADCConvertedValue >= 2370)
	{
		temp = ADCConvertedValue - 2370;
		temp = temp >> 1;
		return (70 + temp);
	}

	if ( ADCConvertedValue >= 2300)
	{
		temp = ADCConvertedValue - 2300;
		temp = temp >> 2;
		return (50 + temp);
	}

	if ( ADCConvertedValue >= 2250)
	{
		temp = ADCConvertedValue - 2250;
		temp = temp >> 1;
		return (30 + temp);
	}

	if ( ADCConvertedValue >= 2180)
	{
		temp = ADCConvertedValue - 2180;
		temp = temp >> 2;
		return (10 + temp);
	}

	if ( ADCConvertedValue >= 2140)
	{
		temp = ADCConvertedValue - 2140;
		temp = temp >> 2;
		return (temp);
	}

	return (0);
}
uint16_t bat_inf::get_bat_adc(void){

	return (ADCConvertedValue);
}


void bat_inf::init_bat_inf(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
 	GPIO_InitTypeDef GPIO_InitStructure;


 	/* ADCCLK = PCLK2/2 */
 	RCC_ADCCLKConfig(RCC_PCLK2_Div2);
 	  /* Enable peripheral clocks ------------------------------------------------*/
 	  /* Enable DMA1 clock */
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

 	  /* Enable ADC1 and GPIOC clock */
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC, ENABLE);

 	  /* Configure PC.03 (ADC Channel13) as analog input -------------------------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* DMA1 channel1 configuration ----------------------------------------------*/
	  DMA_DeInit(DMA1_Channel1);
	  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);;
	  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue;
	  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	  DMA_InitStructure.DMA_BufferSize = 1;
	  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	  DMA_Init(DMA1_Channel1, &DMA_InitStructure);

	  /* Enable DMA1 channel1 */
	  DMA_Cmd(DMA1_Channel1, ENABLE);

	  /* ADC1 configuration ------------------------------------------------------*/
	  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
	  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	  ADC_InitStructure.ADC_NbrOfChannel = 1;
	  ADC_Init(ADC1, &ADC_InitStructure);

	  /* ADC1 regular channel13 configuration */
	  ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 1, ADC_SampleTime_55Cycles5);

	  /* Enable ADC1 DMA */
	  ADC_DMACmd(ADC1, ENABLE);

	  /* Enable ADC1 */
	  ADC_Cmd(ADC1, ENABLE);

	  /* Enable ADC1 reset calibration register */
	  ADC_ResetCalibration(ADC1);
	  /* Check the end of ADC1 reset calibration register */
	  while(ADC_GetResetCalibrationStatus(ADC1))
	  {
		  __asm__ __volatile__("nop");
	  }

	  /* Start ADC1 calibration */
	  ADC_StartCalibration(ADC1);
	  /* Check the end of ADC1 calibration */
	  while(ADC_GetCalibrationStatus(ADC1))
	  {
		  __asm__ __volatile__("nop");
	  }

	  /* Start ADC1 Software Conversion */
	  ADC_SoftwareStartConvCmd(ADC1, ENABLE);

}

