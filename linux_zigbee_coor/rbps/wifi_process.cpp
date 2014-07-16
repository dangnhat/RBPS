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
long backgroundProc[128];
bool isBackgroundProc = false;

/* Define functions */
void work(int sock);
void sigfunc(int sock);
void create_local_msg_queue(void);
int search_pid(long pid);

void wifi_process_func() {
	/* Create msg_queues */
	create_local_msg_queue();

	int n, leng;
	long pid;
    int sockln; //server create sock to listening to client.
    struct sockaddr_in server_addr, client_addr; //structs contains info about address of server or client.

    /* Initialize value for variables */
    bzero(backgroundProc, sizeof(backgroundProc));
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

    /* Binding sockln */
    int b = bind(sockln, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_in));
    if(b < 0) {
    	printf("ERROR on binding");
    }

    /* Listening on sockln */
    listen(sockln, 5); //maximum 5 backlog in queue.
    printf("listening...\n");

    leng = sizeof(client_addr);

    mesg_t msg_s;
	int ret;
    /* Always listen to connection from clients */
    while (1) {
        sock = accept(sockln, (struct sockaddr*) &client_addr, (socklen_t*) &leng);
        if (sock >= 0) {
        	numOfConn++;
			if(numOfConn == 1)
				printf("Have 1 connection!\n");
			else
				printf("Have %d connections!\n", numOfConn);

            pid = fork();
            printf("pid: %ld\n", pid);

            if (pid < 0)
            	printf("ERROR on fork");

            if (pid == 0) {
                close(sockln);
                work(sock);
                signal(SIGCHLD, sigfunc);
                exit(0);
            }else {
                close(sock);
            }
        }else {
        	/* Update number of connection */
        	ret = msgrcv(child2parent_mq_id, &msg_s, 1, 0, IPC_NOWAIT);
			if(ret > 0) {
				printf("update connection\n");
				if(msg_s.mtext[0] > 0) {
					backgroundProc[search_pid(0)] = msg_s.mtext[0];
				}else {
					int pos = search_pid(msg_s.mtext[0]);
					if(pos > 0)
						backgroundProc[pos] = 0;
					else printf("No pid in array\n");
				}
			}

			/* Get update data from node */
        	ret = msgrcv(cc2wi_mq_id, &msg_s, mtext_max_size, wifi_boardcast_mtype, IPC_NOWAIT);
        	if(ret > 0) {
        		int i;
        		for(i = 0; i < sizeof(backgroundProc); i++) {
        			if(backgroundProc[i] > 0) {
        				msg_s.mtype = backgroundProc[i];
        				msgsnd(parent2child_mq_id, &msg_s, mtext_max_size, 0);
        			}
        		}
        	}
        }
    } /* end of while */

    return;
}

/******************* WORK() ************************
 There is a separate instance of this function
 for each connection. It handles all communication
 once a connnection has been established.
 **************************************************/
void work(int sock) {
	mesg_t msg_s;
    int n, cmd, ret;
    long pid;
    char recvBuff[mtext_max_size]; //receiving buffer.
    char sendBuff[mtext_max_size+1];

    bzero(recvBuff, sizeof(recvBuff));
    bzero(sendBuff, sizeof(sendBuff));

    pid = getpid();

    while(1) {
        /* Receiving data from client */
        n = read(sock, recvBuff, sizeof(recvBuff));
        if(n > 0) {
			printf("Receiving buffer data: %s\n", recvBuff);

			if(recvBuff[0] == 'b') {	//background process (keeping up connection with Android device).
				printf("==================================================\n");
				printf("background process...\n");
				isBackgroundProc = true;

				int msg_leng = 1;
				msg_s.mtype = pid;
				msg_s.mtext[0] = 1;
				/* Send pid to parent via msg_queue */
				while(1) {
					ret = msgsnd(child2parent_mq_id, &msg_s, msg_leng, 0);
					if(ret != -1) {
						printf("Sent msg to queue\n");
						break;
					}
					printf("ERROR on sending msg\n");
				}
				/* Get data from msg_queue */
				while(1) {
					ret = msgrcv(parent2child_mq_id, &msg_s, mtext_max_size, pid, 0);
					if(ret != -1) {
						printf("Received msg from queue\n");
						memcpy(&sendBuff, &msg_s.mtext, mtext_max_size);
						sendBuff[mtext_max_size] = 'e';
						if(send(sock, sendBuff, strlen(sendBuff), 0) < 0) {
							printf("ERROR sending to socket");
							return;
						}
					}
					printf("ERROR on receiving msg\n");
				}
			}else {	//temporary process when an Anroid device sends request.
				printf("==================================================\n");
				printf("temporary process...\n");

				cmd = recvBuff[1]|(recvBuff[2]<<8);
				/* Forwarding msg to msg_queue */
				while(1) {
					msg_s.mtype = pid;
					memcpy(&msg_s.mtext, &recvBuff, mtext_max_size);
					ret = msgsnd(wi2cc_mq_id, &msg_s, mtext_max_size, 0);
					if(ret != -1) {
						printf("Sent the forwarding msg to queue\n");
						break;
					}else printf("ERROR on sending the forwarding msg\n");
				}

				switch(cmd) {
				case measure_node_id:
				case new_schedule_id:
					printf("no waiting\n");
					return;
					break;
				default:
					printf("waiting\n");
					ret = msgrcv(cc2wi_mq_id, &msg_s, mtext_max_size, pid, 0);
					if(ret != -1) {
						printf("Received msg from queue\n");
					}else printf("ERROR on receiving the forwarding msg\n");
					memcpy(&sendBuff, &msg_s.mtext, mtext_max_size);
					sendBuff[mtext_max_size] = 'e';
					if(send(sock, sendBuff, strlen(sendBuff), 0) < 0) {
						printf("ERROR sending to socket");
						return;
					}
					break;
				}
			}
        }
    }
}

void create_local_msg_queue(void) {
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
	long pid = getpid();
	if(isBackgroundProc) {
		mesg_t msg_s;
		int msg_leng = 1;
		msg_s.mtype = pid;
		msg_s.mtext[0] = 0;
		msgsnd(child2parent_mq_id, &msg_s, msg_leng, 0);
	}
    numOfConn--;
    printf("Terminate the process %ld!\n", pid);
    printf("==================================================\n");
    close(sock);
}

int search_pid(long pid) {
	int i;
	for(i = 0; i < sizeof(backgroundProc); i++) {
		if(backgroundProc[i] == pid)
			return i;
	}
	return -1;
}
