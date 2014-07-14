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

int parent2child_mq_id = -1;
int child2parent_mq_id = -1;

short numOfConn = 0;
int sock;   //a new sock be created when has a connection.

int backgroundProc[1024];
int tempProc[1024];

/* Define functions */
void work(int sock, int pid);
void sigfunc(int sock);
void init_msg_queue(void);
int search_pid(int pid);

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
	mesg_t msg_type;
    int i, n, length, cmd;

    char recvBuff[mtext_max_size]; //receiving buffer.
    char sendBuff[mtext_max_size+1];
    char endService[10];
    strcpy(endService, "end\n");

    bzero(recvBuff, sizeof(recvBuff));
    bzero(sendBuff, sizeof(sendBuff));

    while(1) {
        printf("child process...\n");
        n = read(sock, recvBuff, sizeof(recvBuff));

        printf("h: %s\n", recvBuff);

        if(recvBuff[258] == 'b') {
        	printf("background process...\n");
        }else {
        	cmd = recvBuff[1]|(recvBuff[2]<<8);
			switch(cmd) {
			case '1':
				/* Save PID in array */
				backgroundProc[search_pid(0)] = pid;
				msg_type.mtext = recvBuff;
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

				if(send(sock, sendBuff, strlen(sendBuff), 0) < 0) {
					printf("ERROR sending to socket");
				}
				printf("cmd: detail\n");
				break;
			case '4':

				if(send(sock, sendBuff, strlen(sendBuff), 0) < 0) {
					printf("ERROR sending to socket");
				}
				printf("cmd: predict\n");
				break;
			case '5':

				if(send(sock, sendBuff, strlen(sendBuff), 0) < 0) {
					printf("ERROR sending to socket");
				}
				printf("cmd: schedule\n");
				break;
			default:
				printf("no cmd\n");
				break;
			}
        }
        printf("end\n");
    }
}

void init_msg_queue(void) {
	parent2child_mq_id = msgget(IPC_PRIVATE, S_IRUSR | S_IWUSR);
	if (parent2child_mq_id == -1) {
		printf("can't create parent2child_mq_id\n");
	}

	child2parent_mq_id = msgget(IPC_PRIVATE, S_IRUSR | S_IWUSR);
	if (child2parent_mq_id == -1) {
		printf("can't create child2parent_mq_id\n");
	}

	printf("msg queue between parent and children has been created\n");
}

void sigfunc(int sock) {
    numOfConn--;
    printf("Terminate a connection!\n");
    close(sock);
}

int search_pid(int pid) {
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

