#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include <fcntl.h>
#include <mqueue.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>

#include "tell.h"
#include "communication.h"
#include "../roomba/roomba.h"

typedef struct cmdbuf {
    long mtype;
    char mtext[3];
} cmdbuf;

typedef struct snsbuf {
    long mtype;
    char mtext[3];
} snsbuf;

typedef struct tArg {
    mqd_t mqd_cmd;
    mqd_t mqd_sns;    
} tArg;

typedef struct net {
    int sockfd;
    int numbytes;

    //sensor info buffer
    char sensorBuf[MAXDATASIZE];
    char msgBuf[MAXDATASIZE];
    struct addrinfo hints, *serverubfi, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    //cmd array
    char cmd[1];

    char input;

} net;

net * initalizeNet() {
    net nInfo;
    nInfo.input = '\0';

    int serverID,clientSock,numBytes;

    if ((serverID = createServer()) == -1) {
        perror("createServer()");
        return NULL;
    }  

    printf("Waiting for a connection\n");


    return NULL;
};

void thread_brain_start(tArg share);
void thread_nerves_start(tArg share);

int main(void) {
    pthread_t tBrain, tNerves;
    
    //Create Message Queues
    mqd_t mqd_cmd = mq_open("/q1",
        O_RDWR | O_CREAT,
        S_IRWXU | S_IRWXG | S_IRWXO,
        NULL);

    mqd_t mqd_sns = mq_open("/q1",
        O_RDWR | O_CREAT,
        S_IRWXU | S_IRWXG | S_IRWXO,
        NULL);

    printf("Created Message Queues");

    struct mq_attr a_cmd;
    mq_getattr(mqd_cmd,&a_cmd);
    
    struct mq_attr a_sns;
    mq_getattr(mqd_sns,&a_sns);

    //do some error handeling
    if (mqd_cmd == -1 || mqd_sns == -1) {
        perror("mq_open():");
    }

    //package shared resouces into
    tArg share;
    share.mqd_cmd = mqd_cmd;
    share.mqd_sns = mqd_sns;

    //start the threads
    if (pthread_create(&tBrain, NULL, thread_brain_start, mqd_cmd) != 0) {
        perror("pthread_create(), Brain:");
        return -1;
    }
   
    if (pthread_create(&tNerves, NULL, thread_nerves_start, mqd_sns) != 0) {
        perror("pthread_create(), Nerves:");
        return -1;
    } 

    return 0;
}



void thread_brain_start(tArg share) {

    //Initalize Network?

    //for ever  {

        //check network in

        //if in == estop
            //write to front of cmd queue (priority queue?)

        //if in
            //write to cmd queue

        //check responce queue
        //if responce
            //send responce over network

        

    //}

    printf("Brain Thread exited.\n");
}

void thread_nerves_start(tArg share) {

    //Initalize Serial Connection?

    // for ever {

        //check cmd queue in

        //while (cmd_complete == false) {
        
            //set next state conditions

            // while (state_complete == false) {
                //read sensors
                
                //boss around the roomba
            //}

        //}

    //}

    printf("Nerves Thread exited.\n");
}


