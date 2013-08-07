#include "mqer.h"

#define CMD_BUFFER_SIZE 9000
#define QUIT_MESSAGE "quit"


//this file contains
int cmdQ_hasMsg(mqd_t mqd_cmd);
void cmdQ_getMsg(mqd_t mqd_cmd, char* buffer);
mqd_t setupCommandQueue();
void thread_cmdNet_start(mqd_t mqd_cmd);

