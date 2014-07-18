/**
 * @ingroup     libraries
 * @{
 *
 * @file        bpm_inf.cpp
 * @brief       implementation of blood pressure measure module interface	
 * 				
 * 				
 *
 * @author      viettien
 *
 * @}
 */

#include "bpm_inf.h"
#include "coocox.h"
#include "stm32f10x_usart.h"
#include "stm32f10x.h"

/* Definitions */
#define NewDevice

#if BPM_INF_DEBUG
#define BPM_INF_PRINTF(...) printf(__VA_ARGS__)
#else
#define BPM_INF_PRINTF(...)
#endif


#define numOfRetry_max  11
/*--------------------------- Config interface -------------------------------*/

/*--------------------------- Static vars ------------------------------------*/

#ifndef NewDevice

	#define TxBufferSize1   7
	#define RxBufferSize1   TxBufferSize1
	static volatile uint8_t TxBuffer1[TxBufferSize1] ={0x51, 0x22, 0x00, 0x00, 0x00, 0x00, 0xA3};
	static volatile uint8_t RxBuffer1[RxBufferSize1] ={0x51, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00};

	const uint8_t NbrOfDataToTransfer1 = TxBufferSize1;
	const uint8_t NbrOfDataToRead1 = RxBufferSize1;

#else
	// New Device
	#define TxBufferSize1   8
	#define RxBufferSize1   TxBufferSize1
	static volatile uint8_t TxBuffer1[TxBufferSize1] ={0x51, 0x22, 0x00, 0x00, 0x00, 0x00, 0xA3,0x00};
	static volatile uint8_t RxBuffer1[RxBufferSize1] ={0x51, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00,0x00};

	const uint8_t NbrOfDataToTransfer1 = TxBufferSize1;
	const uint8_t NbrOfDataToRead1 = RxBufferSize1;
#endif
__IO uint8_t TxCounter1 = 0x00;
__IO uint8_t RxCounter1 = 0x00;



/*--------------------------- Processing functions ---------------------------*/


/*--------------------------- Public methods ---------------------------------*/
bpm_inf::bpm_inf(void) {
	/* init gpio */
	init_bpm_inf();
}

/*----------------------------------------------------------------------------*/
uint16_t bpm_inf::get_memory_data(uint8_t low, uint8_t high){
	uint8_t try_c = 0;
	uint8_t start = 0;
	uint16_t time_out,temp = 0;
	TxBuffer1[2] = low;
	TxBuffer1[3] = high;
	time_out = 0;


	while ( try_c < numOfRetry_max )
	{	
		if(start == 0)
		{
			try_c++;
			USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
			USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
			RxCounter1 = 0x00;
			TxCounter1 = 0x00;
			time_out = 0;
			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
			USART_ITConfig(USART2, USART_IT_TXE, ENABLE);	
			start = 1;
		}
		else		
		{
			while(TxCounter1 < TxBufferSize1 )
			{
				CoTickDelay(1);
			}
			if (RxCounter1 == NbrOfDataToRead1)			
			{
				if ((RxBuffer1[0] == 0x51)&&(RxBuffer1[1] == 0x22)&&(RxBuffer1[2] == low)
				&&(RxBuffer1[3] == high)&&(RxBuffer1[6] == 0xA5))
//				if ((RxBuffer1[0] == 0x51)&&(RxBuffer1[1] == 0x22)&&(RxBuffer1[6] == 0xA5))
				{
					// ACK ok. 
					temp = RxBuffer1[5];
					temp = temp << 8;
					temp = temp + RxBuffer1[4];
					return (temp);
				}
				else 
				{
					// ACK wrong, retry.
					start = 0;
				}
			
			}
			else
			{
				time_out++;
				CoTickDelay(1);
				if(time_out >= 0xf)
					start = 0;

			}
		}				
		
	}
	return (0xffff);
}


void bpm_inf::init_bpm_inf(void)
{
 	NVIC_InitTypeDef NVIC_InitStructure;
 	GPIO_InitTypeDef GPIO_InitStructure;
 	USART_InitTypeDef USART_InitStruct;

    /* Enable the USARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

	/* Disable USARTy Receive and Transmit interrupts */
    USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
    USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
#ifdef NewDevice
    /* reconfig USART 2 */
    USART_Cmd(USART2, DISABLE);

    USART_InitStruct.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStruct.USART_Parity     = USART_Parity_No;
    USART_InitStruct.USART_BaudRate   = 19200;
    USART_InitStruct.USART_StopBits   = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART2, &USART_InitStruct);

    USART_Cmd(USART2, ENABLE);

#endif
    /* Init GPIO to control BPM */

    /* GPIOA Periph clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* Configure PA0 and PA1 in output pushpull mode */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* GPIOC Periph clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    /* Configure PA0 and PA1 in output pushpull mode */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* init state of GPIO */
    GPIO_SetBits(GPIOA,GPIO_Pin_0);
    GPIO_SetBits(GPIOA,GPIO_Pin_1);
}

uint8_t bpm_inf::measure_complete(void)
{	uint8_t temp = 1;
	temp = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2);
	if (temp == 0)
		return(0);
	else
		return(1);
}

void bpm_inf::start_measure(void)
{
	this->press_menu();
}
void bpm_inf::press_menu(void)
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);
	CoTickDelay(5);
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
}
void bpm_inf::enter_pcl_mode(void)
{	uint16_t temp = 0;
	GPIO_ResetBits(GPIOA,GPIO_Pin_0); // pull down PCL wire.



#ifndef NewDevice

	this->press_menu(); // start PCL( old version need it).

	CoTickDelay (0,0,0,50); // wait for PCL startup.
#else

	CoTimeDelay (0,0,2,0); // wait for PCL startup(New Device need more time).

#endif

	// Now PCL should started.

}
void bpm_inf::exit_pcl_mode(void)
{
	GPIO_SetBits(GPIOA,GPIO_Pin_0); // pull up PCL wire.
}

void bpm_inf::get_measure_value(void)
{
	uint16_t pos = 0;
	uint16_t temp = 0;
	uint8_t high,low;
	uint8_t try_c = 0;
	uint8_t start = 0;
	uint8_t check_sum = 0;
	uint16_t time_out = 0;

#ifndef NewDevice
	{
	this->newest_id = this->get_memory_data(60,0);

	if (this->newest_id == 65535)
	{
		this->newest_id = 0;
	}

	pos = 1024 + (this->newest_id) * 8; // start position of newest data.
	pos = pos + 4;
	high = pos >> 8;
	low = pos;

	temp = this->get_memory_data(low,high); // get sys value.
	this->sys = temp;

	pos = pos + 2;
	high = pos >> 8;
	low = pos;

	temp = this->get_memory_data(low,high); // get dia and pulse value.
	this->dia = temp;
	this->pulse = temp >> 8;
	}
#else
	// new device
	{
		TxBuffer1[0] = 0x51;
		TxBuffer1[1] = 0x26;
		TxBuffer1[2] = 0x00;
		TxBuffer1[3] = 0x00;
		TxBuffer1[4] = 0x00;
		TxBuffer1[5] = 0x00;
		TxBuffer1[6] = 0xA3;
		TxBuffer1[7] = 0x1A;

		while ( try_c < numOfRetry_max )
			{
				if(start == 0)
				{
					try_c++;
					USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
					USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
					RxCounter1 = 0x00;
					TxCounter1 = 0x00;
					time_out = 0;
					USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
					USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
					start = 1;
				}
				else
				{
					while(TxCounter1 < TxBufferSize1 )
					{
						CoTickDelay(2);
					}
					if (RxCounter1 == NbrOfDataToRead1)
					{
//						check_sum = RxBuffer1[0]+RxBuffer1[1]+RxBuffer1[2]+RxBuffer1[3]+
//								RxBuffer1[4]+RxBuffer1[5]+RxBuffer1[6];
//						if ((RxBuffer1[0] == 0x51)&&(RxBuffer1[1] == 0x26)&&(RxBuffer1[6] == 0xA5)
//								&&(RxBuffer1[7] == check_sum))
						if ((RxBuffer1[0] == 0x51)&&(RxBuffer1[1] == 0x26)&&(RxBuffer1[6] == 0xA5))
						{
							// ACK ok.
							this->sys = RxBuffer1[2];
							this->dia = RxBuffer1[4];
							this->pulse =  RxBuffer1[5];
							return;
						}
						else
						{
							// ACK wrong, retry.
							start = 0;
						}

					}
					else
					{
						time_out++;
						CoTickDelay(1);
						if(time_out >= 0xf)
							start = 0;

					}
				}

			}
	}
#endif

}
uint8_t bpm_inf::get_pulse_value(void)
{
	return(this->pulse);
}
uint8_t bpm_inf::get_sys_value(void)
{
	return(this->sys);
}
uint8_t bpm_inf::get_dia_value(void)
{
	return(this->dia);
}
void USART2_IRQHandler(void)
{

	CoEnterISR();

  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {
    /* Read one byte from the receive data register */
    RxBuffer1[RxCounter1++] = USART_ReceiveData(USART2);

    if(RxCounter1 == NbrOfDataToRead1)
    {
      /* Disable the USART2 Receive interrupt */
      USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
    }
  }

  if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
  {
    /* Write one byte to the transmit data register */
    USART_SendData(USART2, TxBuffer1[TxCounter1++]);

    if(TxCounter1 == NbrOfDataToTransfer1)
    {
      /* Disable the USART2 Transmit interrupt */
      USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
    }
  }
  CoExitISR();
}

/* test method */
void bpm_inf::test_receiv(void)
{
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
	USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
	RxCounter1 = 0x00;
	TxCounter1 = 0x00;
	TxBuffer1[0] = 0x51;
	TxBuffer1[1] = 0x26;
	TxBuffer1[2] = 0x00;
	TxBuffer1[3] = 0x00;
	TxBuffer1[4] = 0x00;
	TxBuffer1[5] = 0x00;
	TxBuffer1[6] = 0xA3;
	TxBuffer1[7] = 0x1A;
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
}
void bpm_inf::test(uint8_t test[8])
{
	test[0] = RxBuffer1[0];
	test[1] = RxBuffer1[1];
	test[2] = RxBuffer1[2];
	test[3] = RxBuffer1[3];
	test[4] = RxBuffer1[4];
	test[5] = RxBuffer1[5];
	test[6] = RxBuffer1[6];
	test[7] = RxBuffer1[7];
}
