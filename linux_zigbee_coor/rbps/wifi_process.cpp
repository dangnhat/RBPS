/**
 * @ingroup     MainProgramSource
 * @{
 *
 * @file        wifi_process.cpp
 * @brief       Implementation of wifi-communication process.
 *
 * @author      Nguyen Van Hien <51001042@hcmut.edu.vn>
 *
 * @}
 */

#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include "rbps_glb.h"
#include "wifi_process.h"

#define PORT 9999

using namespace rbps_ns;

short numOfConn = 0;
int sock;   //a new sock be created when has a connection.
mesg_t wi2zb_msq;
int backgroundProc[1024];
int tempProc[1024];
/* Define functions */
void work(int sock, int pid);
void sigfunc(int sock);
int searchPID(int pid);

void wifi_process_func() {
    int sockln; //server create sock to listening to client.

    int n, leng, pid;
    struct sockaddr_in server_addr, client_addr; //structs contains info about address of server or client.

    /* Initialize value for memories */
    memset(&backgroundProc, 0, sizeof(backgroundProc));
    memset(&tempProc, 0, sizeof(tempProc));
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    /* Set address for server */
    server_addr.sin_family = AF_INET;           //default.
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);   //ip server.
    server_addr.sin_port = htons(PORT);         //port number.
//    char a[258];
//    char u[258];
//    strcpy(u, "nvhien");
//    strcpy(a, u);
//    printf("name: %s\n", a);
    /* Create a listening sock */
    sockln = socket(AF_INET, SOCK_STREAM, 0);
    if(sockln < 0) {
    	printf("ERROR opening sock");
    }
    fcntl(sockln, F_SETFL, O_NONBLOCK);

    /* Binding sock */
    int b = bind(sockln, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_in));
    if(b < 0) {
    	printf("ERROR on binding");
    }

    /* Listening */
    listen(sockln, 5); //maximum 5 backlog in queue.
    printf("listening...\n");

    leng = sizeof(client_addr);

    /* always listen client */
    while (1) {
        sock = accept(sockln, (struct sockaddr*) &client_addr, (socklen_t*) &leng);
        if (sock >= 0) {
        	numOfConn++;
			if(numOfConn == 1)
				printf("Have 1 connection!\n");
			else
				printf("Have %d connections!\n", numOfConn);

            pid = fork();
            printf("pid: %d\n", pid);

            if (pid < 0)
            	printf("ERROR on fork");

            if (pid == 0) {
                close(sockln);
                work(sock, pid);
                signal(SIGCHLD, sigfunc);
                exit(0);
            }else {
                close(sock);
            }
        }else {
        	char sendBuff[128];
        	fgets(sendBuff, sizeof(sendBuff), stdin);
        	printf("key: %s", sendBuff);
        }
    } /* end of while */

    return;
}

/******************* WORK() ************************
 There is a separate instance of this function
 for each connection. It handles all communication
 once a connnection has been established.
 **************************************************/
void work(int sock, int pid) {
    int i, n, length;

    char recvBuff[258]; //receiving buffer.
    char sendBuff[258];
    char endService[10];
    strcpy(endService, "end\n");

    bzero(recvBuff, sizeof(recvBuff));
    bzero(sendBuff, sizeof(sendBuff));

    while(1) {
        printf("child process...\n");
        n = read(sock, recvBuff, sizeof(recvBuff));

        printf("h: %s\n", recvBuff);
        switch(recvBuff[1]) {
        case '1':
        	/* Save PID in array */
        	backgroundProc[searchPID(0)] = pid;
        	wi2zb_msq.mtext[0] = (uint8_t)4;
        	wi2zb_msq.mtext[1] = 0x00;
        	wi2zb_msq.mtext[2] = 0x01;
            if(send(sock, sendBuff, strlen(sendBuff), 0) < 0) {
            	printf("ERROR sending to socket");
            }
            printf("cmd: scan\n");
            break;
        case '2':
            if(send(sock, sendBuff, strlen(sendBuff), 0) < 0) {
            	printf("ERROR sending to socket");
            }
            printf("cmd: measure\n");
            break;
        case '3':
			strcpy(sendBuff, "1n1pNvhienrecently110sys66dias78hr164010072014avgdate112sys69dias69hrpeakdate113sys70dias70hr124007072014130306072014173408072014avgweek112sys69dias69hrpeakweek113sys70dias70hr124007072014130306072014173408072014avgmonth112sys69dias69hrpeakmonth113sys70dias70hr124007072014130306072014173408072014height170cm144504062013weight60kg130407072014history0000\n");
            if(send(sock, sendBuff, strlen(sendBuff), 0) < 0) {
            	printf("ERROR sending to socket");
            }
            printf("cmd: detail\n");
            break;
        case '4':
			strcpy(sendBuff, "1n1pNvhienprehyper0avgmonth137sys69dias69hr21.5history0011\n");
            if(send(sock, sendBuff, strlen(sendBuff), 0) < 0) {
            	printf("ERROR sending to socket");
            }
            printf("cmd: predict\n");
            break;
        case '5':
			strcpy(sendBuff, "0schedule\n");
            if(send(sock, sendBuff, strlen(sendBuff), 0) < 0) {
            	printf("ERROR sending to socket");
            }
            printf("cmd: schedule\n");
            break;
        default:
            printf("no cmd\n");
            break;
        }

//        fgets(sendBuff, sizeof(sendBuff), stdin);
        /* send end signal to client */
        if(send(sock, endService, strlen(endService), 0) < 0) {
        	printf("ERROR sending to socket");
        }
        printf("end\n");
    }
}

void sigfunc(int sock) {
    numOfConn--;
    printf("Terminate a connection!\n");
    close(sock);
}

int searchPID(int pid) {
	int i;
	for(i = 0; i < sizeof(backgroundProc); i++) {
		if(backgroundProc[i] == pid)
			return i;
	}
	return -1;
}

int sendToMsgQueue(int msqid, rbps_ns::mesg_t* msgType) {
	return msgsnd(msqid, msgType, rbps_ns::mtext_max_size, IPC_NOWAIT);
}

