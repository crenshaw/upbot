//#include "nerves.h"
#include <mqueue.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef _COMMAND_QUEUE_H_
#define _COMMAND_QUEUE_H_

#define CMD_BUFFER_SIZE 9000
#define QUIT_MESSAGE "quit"


//this file contains
int cmdQ_hasMsg(mqd_t mqd_cmd);
void cmdQ_getMsg(mqd_t mqd_cmd, char* buffer);
mqd_t setupCommandQueue();
void thread_cmdNet_start(mqd_t mqd_cmd);

#endif
