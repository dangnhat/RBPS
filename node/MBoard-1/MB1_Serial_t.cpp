/**
 * @file serial_com.cpp
 * @author  Bui Van Hieu <bvhieu@cse.hcmut.edu.vn>
 * @version 1.0
 * @date 07-07-2012
 *
 * @copyright
 * This poject and all of its relevant documents, source codes, compiled libraries are belong
 * to <b> Smart Sensing and Intelligent Controlling Group (SSAIC)</b> \n
 * You are prohibited to broadcast, distribute, copy, modify, print, or reproduce this in anyway
 * without written permission from SSAIC \n
 * <b> Copyright (C) 2012 by SSAIC, All Right Reversed </b>
 *
 * @brief
 * Implement USART's functions for STM32 \n
 * The supported UARTs are form USART1 to UART5. \n
 * The communication are fixed : 8 bit data, 1 stop bit, no parity, no flow control  \n
 * @attention
 * The USARTs must be initialized first or an infinitive wait will be executed
 */

#include "MB1_Serial_t.h"

#define NUM_UARTs  5
const uint16_t       _USART_TXD_PIN[NUM_UARTs]  = {GPIO_Pin_9,  GPIO_Pin_2, GPIO_Pin_10, GPIO_Pin_10, GPIO_Pin_12};
const uint16_t       _USART_RXD_PIN[NUM_UARTs]  = {GPIO_Pin_10, GPIO_Pin_3, GPIO_Pin_11, GPIO_Pin_11, GPIO_Pin_2};
      GPIO_TypeDef*  _USART_TXD_PORT[NUM_UARTs] = {GPIOA, GPIOA, GPIOB, GPIOC, GPIOC};
      GPIO_TypeDef*  _USART_RXD_PORT[NUM_UARTs] = {GPIOA, GPIOA, GPIOB, GPIOC, GPIOD};
const uint32_t       _USART_TXD_CLK[NUM_UARTs]  = {RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOB, RCC_APB2Periph_GPIOC, RCC_APB2Periph_GPIOC};
const uint32_t       _USART_RXD_CLK[NUM_UARTs]  = {RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOB, RCC_APB2Periph_GPIOC, RCC_APB2Periph_GPIOD};
const uint32_t       _USART_CLK[NUM_UARTs]      = {RCC_APB2Periph_USART1, RCC_APB1Periph_USART2, RCC_APB1Periph_USART3, RCC_APB1Periph_UART4, RCC_APB1Periph_UART5};
      USART_TypeDef* _USARTs[NUM_UARTs]         = {USART1, USART2, USART3, UART4, UART5};

/* for retarget */
static serial_t* USART_stdoutPtr = NULL;
static serial_t* USART_stderrPtr = NULL;
static serial_t* USART_stdinPtr = NULL;

////////////////////////////////////////////////////////////
/**
  * @brief Construction function
  * @param usedUart UART will be used for this object
  * @return None
  */
serial_t::serial_t(uint8_t usedUart){
  this->usedUart = usedUart-1;
}



/**
  * @brief Init one USART
  * @param baudRate USART's baud rate
  * @return None
  * @attention This function have to be called only one time and before other functions
  */
void  serial_t::Restart(uint32_t baudRate){
  GPIO_InitTypeDef  GPIO_InitStruct;
  USART_InitTypeDef USART_InitStruct;

  if (usedUart > 4) { return;}

  /* enable clock */
  RCC_APB2PeriphClockCmd(_USART_TXD_CLK[usedUart], ENABLE);
  RCC_APB2PeriphClockCmd(_USART_RXD_CLK[usedUart], ENABLE);
  (usedUart==0) ? RCC_APB2PeriphClockCmd(_USART_CLK[usedUart], ENABLE) : RCC_APB1PeriphClockCmd(_USART_CLK[usedUart], ENABLE);

  /* config IO pins */
  GPIO_InitStruct.GPIO_Pin   = _USART_TXD_PIN[usedUart];
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
  GPIO_Init(_USART_TXD_PORT[usedUart], &GPIO_InitStruct);

  GPIO_InitStruct.GPIO_Pin   = _USART_RXD_PIN[usedUart];
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
  GPIO_Init(_USART_RXD_PORT[usedUart], &GPIO_InitStruct);

  /*config USART */
  USART_InitStruct.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStruct.USART_Parity     = USART_Parity_No;
  USART_InitStruct.USART_BaudRate   = baudRate;
  USART_InitStruct.USART_StopBits   = USART_StopBits_1;
  USART_InitStruct.USART_WordLength = USART_WordLength_8b;
  USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_Init(_USARTs[usedUart], &USART_InitStruct);

  /*enable USART*/
  USART_Cmd(_USARTs[usedUart], ENABLE);
}



/**
  * @brief Send one character to serial port
  * @param outChar character to send
  * @return None
  * @attention The USARTs must be initialized first or an infinitive wait will be executed
  */
void  serial_t::Print(uint8_t outChar){
  /* Wait until output buffer is empty */
  while (USART_GetFlagStatus(_USARTs[usedUart], USART_FLAG_TXE) == RESET)  {  };
  USART_SendData(_USARTs[usedUart], outChar);
}


/**
  * @brief Send one character to serial port
  * @param outChar character to send
  * @return None
  * @attention The USARTs must be initialized first or an infinitive wait will be executed
  */
void  serial_t::Print(char outChar){
  /* Wait until output buffer is empty */
  while (USART_GetFlagStatus(_USARTs[usedUart], USART_FLAG_TXE) == RESET)  {  };
  USART_SendData(_USARTs[usedUart], (uint8_t) outChar);
}


/**
  * @brief Send one string to serial number
  * @param outStr string to send
  * @return None
  * @attention The USARTs must be initialized first or an infinitive wait will be executed
  */
void  serial_t::Print(uint8_t* outStr){
  while (*outStr != '\0'){
    while (USART_GetFlagStatus(_USARTs[usedUart], USART_FLAG_TXE) == RESET);
    USART_SendData(_USARTs[usedUart], *outStr);
    outStr++;
  }
}


/**
  * @brief Send one string to serial number
  * @param outStr string to send
  * @return None
  * @attention The USARTs must be initialized first or an infinitive wait will be executed
  */
void  serial_t::Print(char* outStr){
  while (*outStr != '\0'){
    while (USART_GetFlagStatus(_USARTs[usedUart], USART_FLAG_TXE) == RESET);
    USART_SendData(_USARTs[usedUart], (uint8_t) (*outStr));
    outStr++;
  }
}


/**
  * @brief Send one data buffer to serial number
  * @param outBuf Data buffer
  * @param bufLen Number in byte of data buffer
  * @return None
  * @attention The USARTs must be initialized first or an infinitive wait will be executed
  */
void  serial_t::Out(uint8_t outBuf[], uint32_t bufLen){
  uint32_t count = 0;

  while (count < bufLen){
    while (USART_GetFlagStatus(_USARTs[usedUart], USART_FLAG_TXE) == RESET);
    USART_SendData(_USARTs[usedUart], outBuf[count]);
    count++;
  }
}


/**
  * @brief Send one signed integer number to serial port
  * @param usedUart choosen USART. Valid values are 1 to 5. Users must ensure used chip has correspond USART.
  * @param outNum signed interger number to send
  * @return None
  * @attention The USARTs must be initialized first or an infinitive wait will be executed
  */
void  serial_t::Print(int32_t outNum){
  if (outNum<0){
    Print('-');
    Print((uint32_t) (-outNum));
  }
  else{
    Print((uint32_t) outNum);
  }
}


/**
  * @brief Send one unsigned integer number to serial port
  * @param outNum unsigned interger number to send
  * @return None
  * @attention The USARTs must be initialized first or an infinitive wait will be executed
  */
void  serial_t::Print(uint32_t outNum){
  uint32_t quotient, remainder;
  int      count; //must int type, uint is incorrect
  uint8_t  outStr[12];

  remainder = outNum;
  count     = 0;
  do{
    quotient    = remainder % 10;
    remainder   = remainder / 10;
    outStr[count++]  = 0x30 | ((uint8_t) quotient);
  }while (remainder !=0);

  while (count > 0){
    while (USART_GetFlagStatus(_USARTs[usedUart], USART_FLAG_TXE) == RESET);
    USART_SendData(_USARTs[usedUart], outStr[--count]);
  }
}



void  serial_t::Out(uint8_t outNum){
  while (USART_GetFlagStatus(_USARTs[usedUart], USART_FLAG_TXE) == RESET)  {  };
  USART_SendData(_USARTs[usedUart], (uint8_t) outNum);
}


void  serial_t::Out(uint16_t outNum){
  while (USART_GetFlagStatus(_USARTs[usedUart], USART_FLAG_TXE) == RESET)  {  };
  USART_SendData(_USARTs[usedUart], (uint8_t) (outNum));
  while (USART_GetFlagStatus(_USARTs[usedUart], USART_FLAG_TXE) == RESET)  {  };
  USART_SendData(_USARTs[usedUart], (uint8_t) (outNum >> 8));
}


void  serial_t::Out(uint32_t outNum){
  while (USART_GetFlagStatus(_USARTs[usedUart], USART_FLAG_TXE) == RESET)  {  };
  USART_SendData(_USARTs[usedUart], (uint8_t) (outNum));
  while (USART_GetFlagStatus(_USARTs[usedUart], USART_FLAG_TXE) == RESET)  {  };
  USART_SendData(_USARTs[usedUart], (uint8_t) (outNum >> 8));
  while (USART_GetFlagStatus(_USARTs[usedUart], USART_FLAG_TXE) == RESET)  {  };
  USART_SendData(_USARTs[usedUart], (uint8_t) (outNum >> 16));
  while (USART_GetFlagStatus(_USARTs[usedUart], USART_FLAG_TXE) == RESET)  {  };
  USART_SendData(_USARTs[usedUart], (uint8_t) (outNum >> 24));
}

/**
  * @brief get data from serial port
  * @return uint16_t
  * @attention The USARTs must be initialized first or an infinitive wait will be executed
  */
uint16_t serial_t::Get (void){
    while (USART_GetFlagStatus(_USARTs[usedUart], USART_FLAG_RXNE) == RESET);
    return USART_ReceiveData (_USARTs[usedUart]);
}

/**
  * @brief get data from serial port (use in ISR)
  * @return uint16_t
  * @attention The USARTs must be initialized first or an infinitive wait will be executed
  */
uint16_t serial_t::Get_ISR (void){
    return USART_ReceiveData (_USARTs[usedUart]);
}

/**
  * @brief Set USART for retarget functions
  * @param uint8_t stdStream, stdStream can be USART_stdStream_stdout,
  * USART_stdStream_stdin, USART_stdStream_stderr,or USART_stdStream_stdout | USART_stdStream_stdin, etc.
  * @return None
  * stdStream (8bit) : x x x x _ x stderr stdin stdout. (bit = 0: off, otherwise : on).
  * when stdxxx is on, USART_stdxxxPtr will be replace by this pointer.
  * when stdxxx if off, if USART_stdxxxPtr == this then USART_stdxxxPtr = NULL, otherwise, do nothing.
  */
void serial_t::Retarget (uint8_t stdStream){
    if ((stdStream & 0x01) == 0x01) //stdout bit = 1
        USART_stdoutPtr = this;
    else{ //stdout bit = 0
        if (USART_stdoutPtr == this)
            USART_stdoutPtr = NULL;
    }

    if (( (stdStream >> 1) & 0x01) == 0x01) //stdin bit = 1
        USART_stdinPtr = this;
    else{ //stdin bit = 0
        if (USART_stdinPtr == this)
            USART_stdinPtr = NULL;
    }

    if (( (stdStream >> 2) & 0x01) == 0x01) //stderr bit = 1
        USART_stderrPtr = this;
    else{ //stderr bit = 0
        if (USART_stderrPtr == this)
            USART_stderrPtr = NULL;
    }
}

/********* Retarget ************/
/**
  * @brief Write one char to USART pointed by USARTptr.
  * @param int fd file id = STDOUT_FILENO (fixed).
  * @param char* ptr : pointer to a array of char.
  * @param int len : number of char to print out.
  * @return None
  */
int _write (int fd, char *ptr, int len) {
    int32_t i;

    switch (fd){
    case STDOUT_FILENO:
        if (USART_stdoutPtr == NULL)
            return -1;
        //USART_stdoutPtr != NULL
        for (i = 0; i < len; i++){
            USART_stdoutPtr->Print (ptr[i]);
        }
        break;

    case STDERR_FILENO:
        if (USART_stderrPtr == NULL)
            return -1;
        for (i = 0; i < len; i++){
            USART_stderrPtr->Print (ptr[i]);
        }
        break;

    default:
        return -1;
    }

    return len;
}
