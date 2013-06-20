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

typedef struct msgbuf {
  long mtype;
  char mtext[3];
} msgbuf;

void thread_alice_start(mqd_t qd);
void thread_bob_start(mqd_t qd);

int main(void)
{

  pthread_t tAlice;
  pthread_t tBob;

  /* Create a message queue using O_CREAT so that if the queue doesn't
   * already exist, it will be created.  When using mq_open with
   * O_CREAT, one must supply four arguments.  The first "name"
   * argument must begin with a slash.  The third "mode" argument is
   * derived from the symbolic constants is <sys/stat.h>.
   */
  mqd_t mqd = mq_open("/q1", 
		    O_RDWR | O_CREAT , 
		    S_IRWXU | S_IRWXG | S_IRWXO, 
		    NULL);


  printf("The message queue id is: %d\n", mqd);

  /* Determine the size of messages for this message queue
   */
  struct mq_attr a;
  mq_getattr(mqd,&a);  

  printf("The default message size is: %d\n", a.mq_msgsize);

  if( mqd == -1)
    {
      perror("mq_open():");
      return -1;
    }

  /* Create a posix thread for each Alice and Bob threads.
   */
  if(pthread_create(&tAlice, NULL, thread_alice_start, mqd) != 0)
    {
      perror("pthread_create(), Alice:");
      return -1;
    }

  if(pthread_create(&tBob, NULL, thread_bob_start, mqd) != 0)
    {
      perror("pthread_create(), Bob:");
      return -1;
    }


  /* Cleanup */
  pthread_exit(NULL);
  mq_close(mqd);

  return 0;
}

/* thread_bob_start()
 * Purpose: Send a message to the message queue pointed to by
 * the queue descriptor, qd.
 */
void thread_bob_start(mqd_t qd)
{
  char * message = "Po";

  printf("Bob thread started.\n");

  if(mq_send(qd, message, 3, 0) != 0)
    {
      perror("msgsend() Bob: ");
      pthread_exit(NULL);
    }

  printf("Bob thread exited.\n");
}

/* thread_alice_start()
 * Purpose: Get a message from the message queue pointed to by
 * the queue descriptor, qd.
 */
void thread_alice_start(mqd_t qd)
{
  char recv_buffer[9000];

  printf("Alice thread started.\n");

  if (mq_receive(qd, recv_buffer, 9000, NULL) == -1)
    {
      perror("mq_receive(): Alice:");
      pthread_exit(NULL);
    }

  printf("Alice obtained message: %s\n", recv_buffer);

  printf("Alice thread exited.\n");
  
  fflush(stdout);

  return;
}
