/**
 * @file        main.cpp
 * @brief       Main program for linux coordinator of Remote blood pressure and heart rate
 * 				monitoring system.
 *
 * @author      DangNhat Pham-Huu <51002279@hcmut.edu.vn>
 */

#include <iostream>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/msg.h>
#include "pthread.h"
#include "rbps_glb.h"
#include "zigbee_thread.h"
#include "wifi_process.h"

using namespace std;

int main() {
	wifi_process_func();
	return 0;
}
