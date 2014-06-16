/**
 * @file CC2530ZNP.h
 * @author  Pham Huu Dang Nhat  <phamhuudangnhat@gmail.com>, HLib MBoard team.
 * @version 1.3
 * @date 15-6-2014
 * @brief This is header file for CC2530ZNP APIs.
 * How to use this lib: //TODO: rewrite this
 * - Declare CC_ns::conf_SPIParams_s, CC_ns::conf_GPIOParams_s and add SPI and GPIOs params.
 * - Declare an CC2530ZNP instance.
 * - Call init with params from these two structs.
 * - sys_reset_hard should be called.
 * - When we want to use this instance.
 *  + attach SPI.
 *  + do s.t
 *  + release SPI so other device can use.
 */

#ifndef _CC2530ZNP_H_
#define _CC2530ZNP_H_

#include "HA_Glb.h"


/**< ---------------------------- Definitions ------------------------------------- */
namespace CC_ns{

/**< attributes */
typedef enum {
    successful,
    failed,
} status_t;
/**< end attributes */

/**< configurations */
typedef struct {
    int32_t buad_rate;
    int32_t comport_num;
    int32_t poll_period;
} conf_USARTParams_s;

/**< end configuration */

/**< ---------------------------- Definitions ------------------------------------- */

}

class CC2530ZNP {
public:
    CC2530ZNP (void);

    /**< conf (run-time) */
    CC_ns::status_t init (CC_ns::conf_UARTParams_s *USARTParams_s);
    CC_ns::status_t USART_reInit (void);

    /**< command interface */
    CC_ns::status_t cmd_POLL (void);
    CC_ns::status_t cmd_SREQ (uint16_t cmd, uint8_t len, uint8_t* data_p);
    CC_ns::status_t cmd_AREQ (uint16_t cmd, uint8_t len, uint8_t* data_p);

    uint8_t *cmd_dataBuffer_get (void){ return dataBuffer; }
    uint8_t cmd_dataLen_get (void) { return dataLen; }
    uint8_t *cmd_cmdBuffer_get (void) { return cmdBuffer; }
    uint8_t cmd_cmdLen_get (void) { return 2; }

    bool cmd_isNewMessage (void);

    /**< end command interface */

    /**< SYS interface */
    CC_ns::status_t sys_reset_hard (void);
    CC_ns::status_t sys_reset_req (void);

    /**< end SYS interface */


private:

    /**< conf (run-time) interface */
    int32_t buad_rate;
    int32_t comport_num;
    int32_t poll_period;
    
    void (* timer_call_back)(void);

    /**< end conf (run-time) interface */

    /**< command interface */
    uint8_t dataBuffer [250];
    uint8_t dataLen;
    uint8_t cmdBuffer [2];
    /**< end command interface */

    /**< attributes */
    uint8_t transportRev;
    uint8_t productID;
    uint8_t majorRel;
    uint8_t minorRel;
    uint8_t maintRel;

    uint8_t errno;
    /**< end attributes */
    
    /* get data from comport */
    int32_t rx_buf[1024];
    
    
    void pollComport(void);
    /* end get data from comport */
};

#endif // _CC2530ZNP_H_
