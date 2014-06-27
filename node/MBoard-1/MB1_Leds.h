/**
 * @file MB1_Leds.h
 * @author  Pham Huu Dang Nhat  <phamhuudangnhat@gmail.com>, HLib MBoard team.
 * @version 1.0
 * @date 18-10-2013
 * @brief This is header file for Leds on MBoard-1.
 *
 */

#ifndef MB1_LEDS_H
#define MB1_LEDS_H

/* Includes */
#include "MB1_Glb.h"

/* Defintions for Leds on MBoard-1 */
namespace Led_ns{
    typedef enum {D5 = 0, RGB_Red = 1, RGB_Green = 2, RGB_Blue = 3} Led_t;
}

class Led {
private:
    Led_ns::Led_t used_Led;
public:
    Led(Led_ns::Led_t used_Led); // Init used Led
    void on (void);
    void off (void);
    void toggle (void);
};


#endif // MB1_LEDS_H
