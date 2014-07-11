/**
 * @file HA_System.h
 * @author  Pham Huu Dang Nhat  <phamhuudangnhat@gmail.com>, HLib MBoard team.
 * @version 1.0
 * @date 19-11-2013
 * @brief This is entry-point for HA system.
 */

#ifndef HA_SYSTEM_H_
#define HA_SYSTEM_H_

/**< includes */
#include "CC2530ZNP.h"
#include "ZigbeeStack.h"
#include "HA_Glb.h"

/**<-------------- Global vars and objects in the HA_system ------------*/

/**< CC2530ZNP */
extern CC2530ZNP HA_ZNP;

/**< ZbStack */
extern ZbStack HA_ZbStack;

/**<-------------- Global vars and objects in the HA_system ------------*/

/**<-------------- HA_system_init ------------*/
void HA_system_init (void);
/**<-------------- HA_system_init ------------*/



#endif // HA_SYSTEM_H_
