// Posix Message Queue Example
//
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include <fcntl.h>
#include <mqueue.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>

typedef struct mq_holder {
  mqd_t cmd;
  mqd_t sns;
} mq_holder;

typedef struct msgbuf {
  long mtype;
  char mtext[3];
} msgbuf;

void thread_brain_start(mq_holder* holder);
void thread_nerves_start(mq_holder* holder);

int main(void)
{

  pthread_t tBrain;
  pthread_t tNerves;

  /* Create a message queue using O_CREAT so that if the queue doesn't
   * already exist, it will be created.  When using mq_open with
   * O_CREAT, one must supply four arguments.  The first "name"
   * argument must begin with a slash.  The third "mode" argument is
   * derived from the symbolic constants is <sys/stat.h>.
   */
  mqd_t mqd_c = mq_open("/q_cmd", 
		    O_RDWR | O_CREAT , 
		    S_IRWXU | S_IRWXG | S_IRWXO, 
		    NULL);

  mqd_t mqd_sns = mq_open("/q_sns", 
		    O_RDWR | O_CREAT , 
		    S_IRWXU | S_IRWXG | S_IRWXO, 
		    NULL);
  
  static const mqd_t mqd_cmd = mqd_c;

  if( mqd_cmd == -1)
    {
      perror("mq_open() cmd:");
      return -1;
    }

  if( mqd_sns == -1)
    {
      perror("mq_open() sns:");
      return -1;
    }

  printf("cmd: %d sns: %d\n",mqd_cmd,mqd_sns);
  printf("cmd: %d sns: %d\n",*(&mqd_cmd),*(&mqd_sns));
  
  mq_holder holder = {mqd_cmd,mqd_sns};

  printf("cmd: %d sns: %d\n",holder.cmd,holder.sns);
  printf("cmd: %d sns: %d\n",(&holder)->cmd,(&holder)->sns);
 
  
  printf("cmda: %d snsa: %d\n",&mqd_cmd,&mqd_sns);

  /* Create a posix thread for each Alice and Bob threads.
   */
  if(pthread_create(&tBrain, NULL, thread_brain_start, &holder) != 0)
    {
      perror("pthread_create(), brain:");
      return -1;
    }

  if(pthread_create(&tNerves, NULL, thread_nerves_start, &holder) != 0)
    {
      perror("pthread_create(), nerves:");
      return -1;
    }

  //sleep(1);
  
  printf("cmd: %d sns: %d\n",(&holder)->cmd,(&holder)->sns);
  /* Cleanup */
  pthread_exit(NULL);
  mq_close(mqd_cmd);
  mq_close(mqd_sns);

  return 0;
}

/* thread_nerves_start()
 * Purpose: Send a message to the message queue pointed to by
 * the queue descriptor, qd.
 */
void thread_nerves_start(mq_holder * holder)
{
  //initalize things
  printf("nerves started");
  printf("cmd: %d sns: %d\n",(holder)->cmd,(holder)->sns);
  mqd_t mqd_cmd = holder->cmd;
  mqd_t mqd_sns = holder->sns;

  //recieve message telling nerves which er to start
  char recv_buffer[9000];

  //loop
  if (mq_receive(mqd_cmd, recv_buffer, 9000, NULL) == -1)
    {
      perror("mq_receive(): nerves:");
      pthread_exit(NULL);
    }

  printf("nerves obtained message: %s\n", recv_buffer);
  //set event responder

  //run event responder

  //format data

  //send data

  //end loop

  char * message = "go";

  if(mq_send(mqd_sns, message, 3, 0) != 0)
    {
      perror("msgsend() nerves: ");
      pthread_exit(NULL);
    }

  printf("nerves thread exited.\n");

 }

/* thread_brain_start()
 * 
 * Functinality and better comments forth coming
 */
void thread_brain_start(mq_holder* holder)
{
  //initalize
  printf("brain started\n");
  printf("cmd: %d sns: %d\n",(holder)->cmd,(holder)->sns);
  mqd_t mqd_cmd = holder->cmd;
  mqd_t mqd_sns = holder->sns;

  //send a message to the nerves to tell them to start
  //an event responder
  char * message = "go";

  if(mq_send(mqd_cmd, message, 3, 0) != 0)
    {
      perror("msgsend() brain: ");
      pthread_exit(NULL);
    }

  //loop

  //listen to sensor data

  //if sensor data == something then send message to tell nerves
  //to change responder to stop


 
  char recv_buffer[9000];

  if (mq_receive(mqd_sns, recv_buffer, 9000, NULL) == -1)
    {
      perror("mq_receive(): brain:");
      pthread_exit(NULL);
    }

  printf("brain obtained message: %s\n", recv_buffer);
  
  fflush(stdout);

  printf("brain thread exited\n");
  return;
}
