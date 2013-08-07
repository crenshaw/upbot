#include "commandQueue.h"
/*
#define CMD_BUFFER_SIZE 9000
#define QUIT_MESSAGE "quit"

//this file contains
int cmdQ_hasMsg(mqd_t mqd_cmd);
void cmdQ_getMsg(mqd_t mqd_cmd, char* buffer);
mqd_t setupCommandQueue();
void thread_cmdNet_start(mqd_t mqd_cmd);
 */

int cmdQ_hasMsg(mqd_t mqd_cmd) {
	struct mq_attr a;
	mq_getattr(mqd_cmd,&a);
	return a.mq_curmsgs;
}

void cmdQ_getMsg(mqd_t mqd_cmd, char* buffer) {
	if (mq_receive(mqd_cmd, buffer, CMD_BUFFER_SIZE, NULL) == -1) {
		perror("mq_receive(): ");
		pthread_exit(NULL);
	}     
}

mqd_t setupCommandQueue() {

	/* Create a message queue using O_CREAT so that if the queue doesn't
	 * already exist, it will be created.  When using mq_open with
	 * O_CREAT, one must supply four arguments.  The first "name"
	 * argument must begin with a slash.  The third "mode" argument is
	 * derived from the symbolic constants is <sys/stat.h>.
	 */
	mqd_t mqd_cmd = mq_open("/q_cmd", 
			O_RDWR | O_CREAT , 
			S_IRWXU | S_IRWXG | S_IRWXO, 
			NULL);

	if( mqd_cmd == -1)
	{
		perror("mq_open() cmd:");
		return -1;
	} 

	pthread_t tCmdNet;
	/* Create a posix thread for network input.
	 */
	if(pthread_create(&tCmdNet, NULL, thread_cmdNet_start, mqd_cmd) != 0)
	{
		perror("pthread_create(), cmdNet:");
		return -1;
	}

	return mqd_cmd;
}

/* thread_cmdNet_start()
 * Purpose: Read network messages and forward them to event responder
 * via message queue
 * 
 * Justifying the use of a message queue and thread because if
 * something goes wrong with the network we wan't to ensure robot
 * isn't stuck in some half broken state.
 */
void thread_cmdNet_start(mqd_t mqd_cmd)
{

	printf("cmdNet started\n");

	/*
		 while (1) {
		 char * message = servRead();

		 if(mq_send(mqd_cmd, message, 3, 0) != 0)
		 {
		 perror("msgsend() nerves: ");
		 pthread_exit(NULL);
		 }

		 if (strcmp(message, QUIT_MESSAGE) == 0) {
		 break;
		 }
		 }
	 */
	char * message = "go";  //servRead();

	if(mq_send(mqd_cmd, message, 3, 0) != 0)
	{
		perror("msgsend() nerves: ");
		pthread_exit(NULL);
	}

	/*cleanup*/
	//TODO: close socket here
	pthread_exit(NULL);

}

