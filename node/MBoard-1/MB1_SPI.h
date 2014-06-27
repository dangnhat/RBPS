/**
 * @file MB1_SPI.h
 * @author  Pham Huu Dang Nhat  <phamhuudangnhat@gmail.com>, HLib MBoard team.
 * @version 1.2
 * @date 16-11-2013
 * @brief This is header file for SPIs on MBoard-1. MBoard-1 only supports SP1 and
 * SP2 as AF. SPI1 on APB2 clock brigde with Fmax = SystemClock, SP2 on APB1 with
 * Fmax = SystemClock/2.
 * How to use this lib:
 * - Declare an SPI instance.
 * ------ master mode ---------
 * - Set up numOfSSLines.
 * - Set up GPIO for NSS lines by calling SM_NSS_GPIOs_set.
 * - Set up device-to-decoder table by calling SM_deviceToDecoder_set (remember to set decode value for allFree).
 * - When using SPI :
 *  + init SPI.
 *  + attach SPI to a device.
 *  + do somethings.
 *  + after finished, release SPI, so other device can use.
 */

#ifndef _MB1_SPI_H_
#define _MB1_SPI_H_

/* Includes */
#include "MB1_Glb.h"
#include "MB1_Misc.h"

namespace SPI_ns{

/**< config (compile-time), we should config at compile time. */
const uint8_t numOfSPIs = 2;
const uint8_t SSLines_max = 3;
const uint8_t SSDevices_max = 0x01 << SSLines_max;

/**< config (compile-time), we should config at compile time. */

/**< SPI_global */
typedef enum {
    successful,
    failed,
    busy,
    decodeValueNotFound

} status_t;

/**< end SPI_global */


/**< conf (run-time) typedefs and global vars */
typedef struct {
    uint16_t baudRatePrescaler;   //SPI_BaudRatePrescaler_X, where X can be : 2,4,8,16,32,64,128,256
    uint16_t CPHA;                  //SPI_CPHA_1Edge or SPI_CPHA_2Edge
    uint16_t CPOL;                  //SPI_CPOL_High or SPI_CPOL_Low
    uint16_t crcPoly;
    uint16_t dataSize;             //SPI_DataSize_16b or SPI_DataSize_8b
    uint16_t direction;             //SPI_Direction_1Line_Rx, SPI_Direction_1Line_Tx, SPI_Direction_2Lines_FullDuplex, SPI_Direction_2Lines_RxOnly
    uint16_t firstBit;             //SPI_FirstBit_LSB or SPI_FirstBit_MSB
    uint16_t mode;                  //SPI_Mode_Master, SPI_Mode_Slave
    uint16_t nss;                   //SPI_NSS_Hard, SPI_NSS_Soft.
} SPI_params_t;

/**< end conf (run-time) */

/**< -------------- master mode --------------------------------*/

/**< slave_mgr interface */
typedef struct {
    GPIO_TypeDef * GPIO_port;
    uint16_t GPIO_pin;
    uint32_t GPIO_clk;

    uint8_t ssLine;
} SM_GPIOParams_s;

typedef enum {
    allFree,
    cc2530_1,
    at25Flash_1,
    at25Flash_2,

} SM_device_t;

/**< end slave_mgr */

/**< -------------- master mode --------------------------------*/
}

class SPI {

public:
    SPI (uint16_t usedSPI);

    /**< conf (run-time) interface */
    SPI_ns::status_t init (SPI_ns::SPI_params_t *params_struct);

    /**< end conf (run-time) */

    /**< -------------- master mode --------------------------------*/

    /**< slave_mgr interface (soft NSS) */

    /**< conf (run-time) */
    SPI_ns::status_t SM_numOfSSLines_set (uint8_t numOfSSLines);
    SPI_ns::status_t SM_GPIO_set (SPI_ns::SM_GPIOParams_s *params_struct);
    SPI_ns::status_t SM_deviceToDecoder_set (SPI_ns::SM_device_t device, uint8_t decode_value);
    /**< conf (run-time) */

    SPI_ns::status_t SM_device_attach (SPI_ns::SM_device_t device);
    SPI_ns::status_t SM_device_release (SPI_ns::SM_device_t device);

    SPI_ns::status_t SM_device_select (SPI_ns::SM_device_t device);
    SPI_ns::status_t SM_device_deselect (SPI_ns::SM_device_t device);

    /**< end slave_mgr */

    /**< master 2 lines, full duplex interface */
    uint16_t M2F_sendAndGet_blocking (SPI_ns::SM_device_t device, uint16_t data);

    /**< master 2 lines, full duplex interface */

    /**< -------------- master mode --------------------------------*/

private:
    uint16_t usedSPI;

    /**< app_conf config at run-time by methods */
    GPIO_TypeDef * softNSS_ports [SPI_ns::SSLines_max];
    uint16_t softNSS_pins [SPI_ns::SSLines_max];
    uint32_t softNSS_RCCs [SPI_ns::SSLines_max];

    void M2F_GPIOs_Init (void);
    /**< end app_conf */

    /**< -------------- master mode --------------------------------*/

    /**< slave_mgr interface */
    uint16_t SS_pins_set;             //It's used to know which SSPin has been set or not, bit 0 is for HardNSS pin.
    uint8_t SM_numOfNSSLines;
    uint8_t SM_numOfDevices; // always = 2^SM_numOfNSSLines

    SPI_ns::SM_device_t SM_deviceToDecoder_table [SPI_ns::SSDevices_max];
    SPI_ns::SM_device_t SM_deviceInUse;
    uint8_t SM_decodeValueInUse; // use to select a device, only change when device_in_use change in attach fucntion.
    uint8_t SM_decode_all_free; // use when deselect a device, it set only after set decode value for SPI_allFree.

    SPI_ns::status_t SM_decodeValueInUse_update (void);

    /**< end slave_mgr */

    /**< -------------- master mode --------------------------------*/
};


#endif // _MB1_SPI_H_
