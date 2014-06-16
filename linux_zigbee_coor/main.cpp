//============================================================================
// Name        : main.cpp
// Author      : Pham Huu Dang Nhat
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <unistd.h>
#include "rs232.h"
using namespace std;

const int CTS_ACTIVE_STATE = 1;
const int RX_BUF_SIZE = 1024;

bool checkFCS(unsigned char *rx_buf, int size);

int main() {
    int comport_num = 17; //ttyUSB1
    unsigned char rxBuf[RX_BUF_SIZE];
    int rx_count;

    int count;
    bool isNeedToGetData;
    int old_cts_state, new_cts_state;

    /* Test comport */
    if (RS232_OpenComport(comport_num, 115200) == -1){
        cout << "Cannot open comport ttyUSB1\n";
        return -1;
    }

    /* receive chars when CTS is set*/
    isNeedToGetData = false;

    RS232_enableRTS(comport_num);
    printf("Enabled RTS pin\n");
    
    old_cts_state = RS232_IsCTSEnabled(comport_num);
    new_cts_state = RS232_IsCTSEnabled(comport_num);
    while (1) {
        rx_count = RS232_PollComport(comport_num, rxBuf, RX_BUF_SIZE);

        /* print data */
        if (rx_count > 0){
            printf("New data from comport\n");
            
            for (count = 0; count < rx_count; count++){
                printf("%x ", rxBuf[count]);
            }
            printf("\n");
        }
        
        new_cts_state = RS232_IsCTSEnabled(comport_num);
        if (new_cts_state != old_cts_state){
            printf("CTS's state changed %d\n", new_cts_state);
            
            old_cts_state = new_cts_state;
        }
    }

    RS232_CloseComport(comport_num);

    return 0;
}

bool checkFCS(unsigned char *rx_gframe, int size, unsigned char FCS){
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
