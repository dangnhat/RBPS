/**
 @file hl_crc.h
 @brief Implement some prototype functions to calculate CRC and check error from CRC received

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
#ifndef __HL_CRC_H
#define __HL_CRC_H

#include "MB1_Glb.h"

/**
 @class CRC_c
 @brief Providing controlling method for CRC peripheral of STM32
 @attention
 CRC polynomial in STM32F1 cannot reconfigure. The CRC polinomial is:
 - 04D11CDB7h or 100110000010001110110110111b \n
 32-bit CRC is always used
*/
class CRC_c{
private:
    void 		Clear();
public:
    void 		Start();
	void      	Shutdown();
	uint32_t 	Calculate(uint32_t data);
    uint32_t 	Calculate(uint32_t dataBuffer[], uint16_t bufferSize);
	uint32_t 	CalculateCont(uint32_t data);
	uint32_t 	CalculateCont(uint32_t dataBuffer[], uint16_t bufferSize);
    bool 		Check(uint32_t dataBuffer[], uint16_t bufferSize, uint32_t receivedCRC);
}; //end class

#endif //__HL_CRC_H
