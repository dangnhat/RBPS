/**
 @file hl_crc.h
 @brief Implement some functions to calculate CRC and to check error from received CRC

 @author  Do Mai Anh Tu <aberrant.accolades@gmail.com>
 @version 1.0
 @date 1/11/2013
 @copyright
 This project and all its relevant hardware designs, documents, source codes, compiled libraries
 belong to <b> Smart Sensing and Intelligent Controlling Group (SSAIC Group)</b>.
 You have to comply with <b> Non-Commercial Share-Alike Creative Common License </b>
 in order to share (copy, distribute, transmit) or remix (modify, reproduce, adapt) these works.\n
 SSAIC Group shall not be held liable for any direct, indirect or consequential damages
 with respect to any claims arising from the content of hardware, firmware and/or the use
 made by customers of the coding information contained herein in connection with their products.\n
 You are prohibited from commercializing in any kind that using or basing on these works
 without written permission from SSAIC Group. Please contact ssaic@googlegroups.com for commercializing
*/


/***********************************************************************/
#include "hl_crc.h"

/**
 @brief Enable CRC clock, reset all registers to default values
 @return None
 @attention This method have to be called to enable CRC block before other methods
*/
void CRC_c::Start() {
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC,ENABLE);
	CRC->CR = CRC_CR_RESET;
}



/**
 @brief Disable CRC clock
 @return None
*/
void CRC_c::Shutdown() {
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, DISABLE);
}



/**
 @brief Clear current calculating CRC value to zero
 @return None
 @attention This function have to be called internally before recalculate CRC
*/
void CRC_c::Clear() {
  CRC->CR = CRC_CR_RESET;
}



/**
 @brief Clear current calculating CRC and calculate new CRC for a 32-bit data
 @param data Data to calculate CRC
 @return CRC value that is calculated by CRC peripheral
 @attention return value is a 32 bit unsigned integer
*/
uint32_t CRC_c::Calculate(uint32_t data){
	Clear();
	return CalculateCont(data);
}



/**
 @overload
 @brief Clear current calculating CRC and calculate new CRC for a data block
 @param dataBuffer Array of data in buffer
 @param bufferSize Size of buffer or the number of data in buffer
 @return CRC value that is calculated by CRC peripheral
 @attention return value is a 32 bit unsigned integer
*/
uint32_t CRC_c::Calculate(uint32_t dataBuffer[], uint16_t bufferSize) {
  Clear();
	return CalculateCont(dataBuffer, bufferSize);
}



/**
 @brief Use current calculating CRC and calculate CRC continuously for a 32-bit data
 @param data Data to calculate CRC
 @return CRC value that is calculated by CRC peripheral
 @attention return value is a 32 bit unsigned integer
*/
uint32_t CRC_c::CalculateCont(uint32_t data){
  CRC->DR = data;
  return (CRC->DR);
}



/**
 @overload
 @brief Use current calculating CRC and calculate CRC continuously for a data block
 @param dataBuffer Array of data in buffer
 @param bufferSize Size of buffer or the number of data in buffer
 @return CRC value that is calculated by CRC peripheral
 @attention return value is a 32 bit unsigned integer
*/
uint32_t CRC_c::CalculateCont(uint32_t dataBuffer[], uint16_t bufferSize) {
  uint16_t index;
  for (index=0; index < bufferSize; index++){
    CRC->DR = dataBuffer[index];
  }
  return (CRC->DR);
}


/**
 @brief Check CRC error for a data block
 @param dataBuffer array of data in buffer
 @param bufferSize size of buffer or the number of data in buffer
 @param receivedCRC CRC value received to check
 @retval TRUE check result is OK
 @retval FALSE chec result is FAIL
*/
bool CRC_c::Check(uint32_t dataBuffer[], uint16_t bufferSize, uint32_t receivedCRC) {
  return (receivedCRC == Calculate(dataBuffer, bufferSize));
}

