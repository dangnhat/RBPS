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
#define MAX_DEV 128

using namespace rbps_ns;

int parent2child_mq_id = -1;
int child2parent_mq_id = -1;

short numOfConn = 0;
int sock;   //a new sock be created when has a connection.
int backgroundProc[MAX_DEV];
bool isBackgroundProc = false;

/* Define functions */
void work(int sock);

/**
 * @brief   called handler function when the connection is terminated.
 *
 * @param[out]	none.
 * @param[in]	none.
 */
void endBackgroundProc(int sock);

/**
 * @brief   searching position of pid of a background process
 * 				that is saved in backgroundProc array.
 *
 * @param[out]	index.
 * @param[in]	pid.
 */
int search_pid(int pid);

/**
 * @brief   searching position of pid of a background process
 * 				that is saved in backgroundProc array.
 *
 * @param[out]	none.
 * @param[in]	none.
 */
void create_local_msg_queue(void);

void wifi_process_func() {
	/* Create msg_queues */
	create_local_msg_queue();

	int n, leng, pid;
    int sockln; //server create sock to listening to client.
    struct sockaddr_in server_addr, client_addr; //structs contains info about address of server or client.

    /* Initialize value for variables */
    memset(&backgroundProc, 0, MAX_DEV);
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    /* Set address for server */
    server_addr.sin_family = AF_INET;           //default.
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);   //ip server.
    server_addr.sin_port = htons(PORT);         //port number.

    /* Create a listening sock */
    sockln = socket(AF_INET, SOCK_STREAM, 0);
    if(sockln < 0) {
    	printf("Wifi: ERROR on opening socket.\n");
    }
    fcntl(sockln, F_SETFL, O_NONBLOCK);

    /* Binding sockln */
    int b = bind(sockln, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_in));
    if(b < 0) {
    	printf("Wifi: ERROR on binding socket.\n");
    }

    /* Listening on sockln */
    listen(sockln, 5); //maximum 5 backlog in queue.
    printf("Wifi: Listening on sockln...\n");

    leng = sizeof(client_addr);

    mesg_t msg_s;
	int ret;
    /* Always listen to connection from clients */
    while (1) {
        sock = accept(sockln, (struct sockaddr*) &client_addr, (socklen_t*) &leng);
        if (sock >= 0) {
            pid = fork();

            if (pid < 0)
            	printf("Wifi: ERROR on forking.\n");

            if (pid == 0) {
                close(sockln);
                work(sock);
            }else {
                close(sock);
            }
        }else {
        	/* Update number of connection */
        	ret = msgrcv(child2parent_mq_id, &msg_s, 1, 0, IPC_NOWAIT);
			if(ret > 0) {

				printf("Wifi: Update number of background connection.\n");
				int pos = 0;
				if(msg_s.mtext[0] == 1) {
					printf("Wifi: Add pid %ld into array\n", msg_s.mtype);
					pos = search_pid(0);
					backgroundProc[pos] = msg_s.mtype;
					printf("Wifi: %d\n", backgroundProc[pos]);
				}else if(msg_s.mtext[0] == 0){
					pos = search_pid((int)msg_s.mtype);
					if(pos >= 0) {
						printf("Wifi: Removed pid %ld out of array.\n", msg_s.mtype);
						backgroundProc[pos] = 0;
					}else printf("Wifi: No pid %ld in array\n", msg_s.mtype);
				}
				printf("Wifi: pos of pid %ld in array is %d\n", msg_s.mtype, pos);
				int j;
				for(j = 0; j < MAX_DEV; j++)
					printf("Wifi: %d\n", backgroundProc[j]);
			}

			/* Get update data from node */
        	ret = msgrcv(cc2wi_mq_id, &msg_s, mtext_max_size, wifi_boardcast_mtype, IPC_NOWAIT);
        	if(ret > 0) {
        		int i;
        		for(i = 0; i < MAX_DEV; i++) {
        			if(backgroundProc[i] > 0) {
						printf("Wifi: Broadcast msg to all background process.\n");
        				msg_s.mtype = backgroundProc[i];
        				msgsnd(parent2child_mq_id, &msg_s, mtext_max_size, IPC_NOWAIT);
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
    int n, cmd, ret, pid;
    uint8_t recvBuff[mtext_max_size+1]; //receiving buffer.
    uint8_t sendBuff[mtext_max_size];

    bzero(recvBuff, sizeof(recvBuff));
    bzero(sendBuff, sizeof(sendBuff));

    pid = getpid();

    /* Receiving data from client */
    n = read(sock, recvBuff, sizeof(recvBuff));
    if(n > 0) {
		int i;
		for(i = 0; i<258; i++)
			printf("%d ", recvBuff[i]);
		printf("\n");

		if(recvBuff[mtext_max_size] == 1) {	//background process(keeping up connection with Android dev).
			printf("==================================================\n");
			printf("Wifi: Background process %d...\n", pid);
			isBackgroundProc = true;
			numOfConn++;
			if(numOfConn == 1)
				printf("Wifi: Have 1 background connection!\n");
			else
				printf("Wifi: Have %d background connections!\n", numOfConn);

			int msg_leng = 1;
			msg_s.mtype = pid;
			msg_s.mtext[0] = 1;
			/* Send pid to parent via msg_queue */
			ret = msgsnd(child2parent_mq_id, &msg_s, msg_leng, 0);
			if(ret != -1) {
				printf("Wifi: Sent bg proc info to parent via queue.\n");
			}else
				printf("Wifi: ERROR on sending bg proc info\n");

			/* Get data from msg_queue */
            while(1) {
			    ret = msgrcv(parent2child_mq_id, &msg_s, mtext_max_size, pid, IPC_NOWAIT);
			    if(ret != -1) {
				    printf("Wifi: Received a broadcast msg from queue\n");
				    memcpy(&sendBuff, &msg_s.mtext, mtext_max_size);
				    if(send(sock, sendBuff, mtext_max_size, 0) < 0) {
					    printf("Wifi: ERROR on sending udt msg to socket");
					    return;
				    }
			    }
			    n = read(sock, recvBuff, sizeof(recvBuff));
			    if(n > 0) {
			    	if(recvBuff[mtext_max_size] == 2) { //terminate the background process.
						printf("Wifi: Terminate the bg proc.\n");
						endBackgroundProc(sock);
						return;
					}
			    }
            }
		}else {	//temporary process when an Anroid device sends request.
			printf("==================================================\n");
			printf("Wifi: Temporary process %d...\n", pid);

			cmd = recvBuff[1]|(recvBuff[2]<<8);
			/* Forwarding msg to msg_queue */
			msg_s.mtype = pid;
			memcpy(&msg_s.mtext, &recvBuff, mtext_max_size);
			ret = msgsnd(wi2cc_mq_id, &msg_s, mtext_max_size, 0);
			if(ret != -1) {
				printf("Wifi: Forward msg from socket to queue\n");
			}else printf("Wifi: ERROR on sending the forwarding msg\n");

			switch(cmd) {
			case measure_node_id:
			case update_schedule:
				printf("Wifi: No waiting\n");
				return;
				break;
			default:
				printf("Wifi: Waiting\n");
				ret = msgrcv(cc2wi_mq_id, &msg_s, mtext_max_size, pid, 0);
				if(ret != -1) {
					printf("Wifi: Received reply data from queue\n");

					memcpy(&sendBuff, &msg_s.mtext, mtext_max_size);
					int i;
					for(i = 0; i<mtext_max_size; i++) {
						printf("%d ", sendBuff[i]);
					}
					printf("\n");
					if(send(sock, sendBuff, mtext_max_size, 0) < 0) {
						printf("Wifi: ERROR on sending reply data to socket");
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
		printf("Wifi: Can't create parent2child_mq\n");
	}

	child2parent_mq_id = msgget(IPC_PRIVATE, S_IRUSR | S_IWUSR);
	if (child2parent_mq_id == -1) {
		printf("Wifi: Can't create child2parent_mq\n");
	}

	printf("Wifi: msg queue between parent and children has been created\n");
}

void endBackgroundProc(int sock) {
	int pid = getpid();
	if(isBackgroundProc) {
		mesg_t msg_s;
		int msg_leng = 1;
		msg_s.mtype = pid;
		msg_s.mtext[0] = 0;
		msgsnd(child2parent_mq_id, &msg_s, msg_leng, 0);
	}

	numOfConn--;
	if(numOfConn == 1)
		printf("Wifi: Remaining 1 background connection!\n");
	else if(numOfConn > 0)
		printf("Wifi: Remaining %d background connections!\n", numOfConn);

//	close(sock);
	printf("Wifi: Terminated the background process %d!\n", pid);
	printf("==================================================\n");
}

int search_pid(int pid) {
	int i;
	for(i = 0; i < MAX_DEV; i++) {
		if(backgroundProc[i] == pid) {
			return i;
		}
	}
	return -1;
}
