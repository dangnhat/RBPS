/**
 * @file HA_Glb.h
 * @author  Pham Huu Dang Nhat  <phamhuudangnhat@gmail.com>.
 * @version 1.1
 * @date 16-6-2014
 * @brief This is header file which include common things for all module in HA system (RBPS).
 */

#ifndef HA_GLB_H_
#define HA_GLB_H_

#ifdef EN_CC_DEBUG
#define CC_DEBUG(...) printf(__VA_ARGS__)
#else
#define CC_DEBUG(...)
#endif

/**< includes */
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "rs232.h"
#include "timer.h"

#endif // HA_GLB_H_
