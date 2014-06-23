/**
 * @file CC2530ZNP.h
 * @author  Pham Huu Dang Nhat  <phamhuudangnhat@gmail.com>, HLib MBoard team.
 * @version 1.3
 * @date 18-11-2013
 * @brief This is source file for CC2530ZNP APIs.
 */

/* Includes */
#include "CC2530ZNP.h"

using namespace CC_ns;

/* Definitions of Len (1 byte, len = FF means vary) and Cmd (2 bytes) for CC2530ZNP API */
/* System Interface */
#define SYS_RESET_REQ               0x014100
#define SYS_RESET_IND               0x064180
#define SYS_VERSION                 0x002102
#define SYS_VERSION_SRSP            0x056102
#define SYS_OSAL_NV_READ            0x032108
#define SYS_OSAL_NV_SRSP            0xFF6108
#define SYS_OSAL_NV_WRITE           0xFF2109
#define SYS_OSAL_NV_WRITE_SRSP      0x016109
#define SYS_OSAL_NV_ITEM_INIT       0xFF2107
#define SYS_OSAL_NV_ITEM_INIT_SRSP  0x016107
#define SYS_OSAL_NV_DELETE          0x042112
#define SYS_OSAL_NV_DELETE_SRSP     0x016112
#define SYS_OSAL_NV_LENGTH          0x022113
#define SYS_OSAL_NV_LENGTH_SRSP     0x026113
#define SYS_ADC_READ                0x02210D
#define SYS_ADC_READ_SRSP           0x02610D
#define SYS_GPIO                    0x02210E
#define SYS_GPIO_SRSP               0x01610E
#define SYS_RANDOM                  0x00210C
#define SYS_RANDOM_SRSP             0x02610C
#define SYS_SET_TIME                0x0B2110
#define SYS_SET_TIME_SRSP           0x016110
#define SYS_GET_TIME                0x002111
#define SYS_GET_TIME_SRSP           0x0B6111
#define SYS_SET_TX_POWER            0x012114
#define SYS_SET_TX_POWER_SRSP       0x016114

/* Configuration Interface */
#define ZB_READ_CONFIGURATION       0x012604
#define ZB_READ_CONFIGURATION_SRSP  0xFF6604
#define ZB_WRITE_CONFIGURATION      0xFF2605
#define ZB_WRITE_CONFIGURATION_SRSP 0x016605

/* Simple API interface */
#define ZB_APP_REGISTER_REQUEST     0xFF260A
#define ZB_APP_REGISTER_REQUEST_SRSP 0x01660A
#define ZB_START_REQUEST            0x002600
#define ZB_START_REQUEST_SRSP       0x006600
#define ZB_START_CONFIRM            0x014680
#define ZB_PERMIT_JOINING_REQUEST   0x032608
#define ZB_PERMIT_JOINING_REQUEST_SRSP 0x016608
#define ZB_BIND_DEVICE              0x0B2601
#define ZB_BIND_DEVICE_SRSP         0x006601
#define ZB_BIND_CONFIRM             0x034681
#define ZB_ALLOW_BIND               0x012602
#define ZB_ALLOW_BIND_SRSP          0x006602
#define ZB_ALLOW_BIND_CONFIRM       0x024682
#define ZB_SEND_DATA_REQUEST        0xFF2603
#define ZB_SEND_DATA_REQUEST_SRSP   0x006603
#define ZB_SEND_DATA_CONFIRM        0x024683
#define ZB_RECEIVE_DATA_INDICATION  0xFF4687
#define ZB_GET_DEVICE_INFO          0x012606
#define ZB_GET_DEVICE_INFO_SRSP     0x096606
#define ZB_FIND_DEVICE_REQUEST      0x082607
#define ZB_FIND_DEVICE_REQUEST_SRSP 0x006607
#define ZB_FIND_DEVICE_CONFIRM      0x0B4685

/* static members */
int32_t CC2530ZNP::buad_rate;
int32_t CC2530ZNP::comport_num;
int32_t CC2530ZNP::poll_period;
cir_queue CC2530ZNP::rx_cir_queue;

/**
  * @brief Constructor of CC2530ZNP class
  * @return None
  * - Allocate mem for a new CC2530 object.
  * - Init value for data members. Numerical vars set to 0, pointer vars set to NULL.
  * - Stop timer.
  * - timer_call_back set to CC2530ZNP::pollComport as default
  */
CC2530ZNP::CC2530ZNP (void){

    /**< conf (run-time) interface */
    buad_rate = 0;
    comport_num = 0;
    poll_period = 0;
    
    timer_call_back = this->pollComport;
    
    stop_timer();

    /**< end conf (run-time) interface */

    /**< attributes */
    transportRev = 0;
    productID = 0;
    majorRel = 0;
    minorRel = 0;
    maintRel = 0;

    errno = successful;
    /**< end attributes */

    return;
}

CC2530ZNP::~CC2530ZNP(void){

	/* stop timer and close comport */
    stop_timer();
    RS232_CloseComport(comport_num);

    return;
}

void CC2530ZNP::pollComport(void) {
	uint8_t buffer[512];
	int32_t rev_count;
	uint16_t count;

	CC_DEBUG("In poll comport\n");

	/* get data */
	rev_count = RS232_PollComport(comport_num, buffer, 255);

	CC_DEBUG("rev_count %d\n", rev_count);
	for (count = 0; count < rev_count; count++) {
		CC_DEBUG("%x ", buffer[count]);
	}
	CC_DEBUG("\n");
	/* store to queue */
	rx_cir_queue.add_data(buffer, rev_count);
}

/**
  * @brief init, Init cc2530 for the first time. //TODO: rewrite
  * @param CC_types::conf_SPIParams_s *SPIParams_s.
  * @param CC_types::conf_GPIOParams_s *GPIOParams_s.
  * @return CC_types::status_t.
  */
status_t CC2530ZNP::init(CC_ns::conf_USARTParams_s *USARTParams_s){

	/* Set data for private vars */
	buad_rate = USARTParams_s->buad_rate;
	comport_num = USARTParams_s->comport_num;
	poll_period = USARTParams_s->poll_period;

	/* open comport */
    if (RS232_OpenComport(comport_num, buad_rate) == -1){
        printf("Cannot open comport %d\n", comport_num);
        return failed;
    }

    /* Start timer */
    start_timer(poll_period, pollComport);

    /* Init GPIOs and SPI */

    return successful;
}

/**
  * @brief send POLL cmd, place respond message in dataBuffer, dataLen and cmdBuffer.
  * @return CC_types::status_t.
  * @attention
  * + GPIO and SPI have been setup before call this functions.
  * + NSS must be high before call this function.
  * + attach a SPI before call this function, otherwise, there will be an infinite loop.
  */
status_t CC2530ZNP::cmd_POLL (void){
    /* some vars */
    uint16_t count;
    uint8_t data;
    bool newdata;
    uint8_t FCS;
    uint8_t gframe[253];

    /* check size */
	if (rx_cir_queue.get_size() == 0) {
		return failed;
	}

	/* check data and remove garbages */
	data = rx_cir_queue.preview_data(false);
	if (data != 0xFE) {
		/* remove data from queue */
		rx_cir_queue.get_data();

		for (count = 1; count < rx_cir_queue.get_size(); count++) {
			data = rx_cir_queue.preview_data(true);
			if (data != 0xFE) {
				rx_cir_queue.get_data();
			}
			else {
				newdata = true;
				break;
			}
		}
	}
	else {
		newdata = true;
	}

	if (newdata) {
		/* remove 0xFE */
		rx_cir_queue.get_data();

		dataLen = rx_cir_queue.get_data();

		cmdBuffer[0] = rx_cir_queue.get_data();
		cmdBuffer[1] = rx_cir_queue.get_data();

		rx_cir_queue.get_data(dataBuffer, dataLen);

		FCS = rx_cir_queue.get_data();

		/* check FCS */
		gframe[0] = dataLen;
		memcpy(gframe + 1, cmdBuffer, 2);
		memcpy(gframe + 3, dataBuffer, dataLen);

		if (this->checkFCS(gframe, dataLen + 3, FCS) == true) {
			return successful;
		}
	}

    return failed;
}

/*----------------------------------------------------------------------------*/
bool CC2530ZNP::checkFCS(unsigned char *rx_gframe, int size, unsigned char FCS){
    int count;
    unsigned char calFCS;

    calFCS = 0;
    for (count = 0; count < size; count++){
        calFCS ^= rx_gframe[count];
    }

    if (calFCS == FCS){
        return true;
    }
    else{
        return false;
    }
}

/*----------------------------------------------------------------------------*/
uint8_t CC2530ZNP::genFCS(uint8_t *rx_gframe, int size) {
	int count;
	uint8_t calFCS;

	calFCS = 0;
	for (count = 0; count < size; count++){
		calFCS ^= rx_gframe[count];
	}

	return calFCS;
}

/**
  * @brief send SREQ cmd, place respond message in dataBuffer, dataLen and cmdBuffer.
  * @param uint16_t cmd : cmd to send, format (MSB->LSB) : cmd1 cmd0.
  * @param uint8_t len : length of data to send in array pointed by data_p. len should be less than or equal to 250.
  * @param uint8_t* data_p : pointer to array of data to send.
  * @return CC_types::status_t.
  * @attention
  * + GPIO and SPI have been setup before call this functions.
  * + NSS must be high before call this function.
  * + SPI must be attached successfully, otherwise, there will be an infinite loop.
  */
status_t CC2530ZNP::cmd_SREQ (uint16_t cmd, uint8_t len, uint8_t* data_p){
    uint8_t count;
    uint8_t gframe[253];

    /* wait until CC2530ZNP is not busy */
    while (RS232_IsCTSEnabled(comport_num) == 0) {
    	;
    }

    /* pack data */
    gframe[0] = len;
    gframe[1] = (uint8_t)cmd;
    gframe[2] = (uint8_t)(cmd >> 8);
    memcpy(gframe + 3, data_p, len);

    /* send data */
    RS232_SendByte(comport_num, 0xFE);
    for (count = 0; count < (len + 3); count++) {
    	RS232_SendByte(comport_num, gframe[count]);
    }
    RS232_SendByte(comport_num, this->genFCS(gframe, len + 3));

    /* wait for respond message */
    while (cmd_isNewMessage() == false){
    	;
    }

    return successful;
}

/*----------------------------------------------------------------------------*/
status_t CC2530ZNP::cmd_AREQ (uint16_t cmd, uint8_t len, uint8_t* data_p){
    uint8_t count;
    uint8_t gframe[253];

    /* wait until CC2530ZNP is not busy */
    while (RS232_IsCTSEnabled(comport_num) == 0) {
    	;
    }

    /* pack data */
    gframe[0] = len;
    gframe[1] = (uint8_t)cmd;
    gframe[2] = (uint8_t)(cmd >> 8);
    memcpy(gframe + 3, data_p, len);

    /* send data */
    RS232_SendByte(comport_num, 0xFE);
    for (count = 0; count < (len + 3); count++) {
    	RS232_SendByte(comport_num, gframe[count]);
    }
    RS232_SendByte(comport_num, this->genFCS(gframe, len + 3));

    return successful;
}


/**
  * @brief cmd_isNewMessage, check if there is a new message from CC2530,
  * and POLL to get message to cmdBuffer, dataLen, dataBuffer.
  * @return bool, true if there is new message. Otherwise, false.
  * @attention
  * + GPIO and SPI have been setup before call this functions.
  * + NSS must be high before call this function.
  * + SPI must be attached successfully, otherwise, there will be an infinite loop.
  * + application should get data from cmdBuffer, dataLen, dataBuffer before call other CC2350 function.
  */
bool CC2530ZNP::cmd_isNewMessage (void){
	uint16_t count;
	uint8_t data, length;
	bool newdata = false;

	/* check size */
	if (rx_cir_queue.get_size() == 0) {
		return false;
	}

    /* check data and remove garbages */
	data = rx_cir_queue.preview_data(false);
	if (data != 0xFE) {
		/* remove data from queue */
		CC_DEBUG("data is not 0xFE, remove\n");
		rx_cir_queue.get_data();

		for (count = 1; count < rx_cir_queue.get_size(); count++) {
			data = rx_cir_queue.preview_data(true);
			if (data != 0xFE) {
				CC_DEBUG("data is not 0xFE, remove\n");
				rx_cir_queue.get_data();
			}
			else {
				CC_DEBUG("data is 0xFE, get data\n");
				newdata = true;
				break;
			}
		}
	}
	else {
		newdata = true;
	}

	if (newdata == true) {
		/* get and check size */
		length = rx_cir_queue.preview_data(true);
		if (rx_cir_queue.get_size() < (length + 5)) {
			CC_DEBUG("Data is not ready\n");
			return false;
		}

		/**< new message */
		if (cmd_POLL() == successful) {
			CC_DEBUG("Data ok\n");
			return true;
		}
	}

    return false;
}

/**
  * @brief CC2530 hard reset, reset and send POLL command to get SYS_RESET_IND message
  * and update versions info in data members.
  * @return CC_types::status_t.
  * @attention
  * + GPIOs have been setup before call this functions.
  */
status_t CC2530ZNP::sys_reset_req (void){
	uint8_t data_buf[1] = {0};
	uint32_t count;

	/* send data */
	cmd_AREQ(0x0041, 1, data_buf);

	/* delay */
	for (count = 0; count < 1000000000; count++);
	/* send 0x07 (force jump ZNP code) */
	RS232_SendByte(comport_num, 0x07);

    while (cmd_isNewMessage() == false) {
    	;
    }

    if (cmdBuffer[0] != 0x41 || cmdBuffer[1] != 0x80)
        return failed;

    transportRev = dataBuffer[1];
    productID = dataBuffer[2];
    majorRel = dataBuffer[3];
    minorRel = dataBuffer[4];
    maintRel = dataBuffer[5];

    return successful;
}
