#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include <fcntl.h>
#include <mqueue.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>

#include <signal.h>
#include <sys/time.h>
#include <time.h>

#include "../../roomba/roomba.h"
#include "eventresponder.h"
#include "robot.h"
#include "services.h"

#include "roomba/sensors.c"
#include "roomba/utility.c"
#include "roomba/commands.c"

#include "events.c"
#include "responders.c"

#include "clock.c"

#define CMD_BUFFER_SIZE 9000
#define STOP_MESSAGE "STOP_MESSAGE"

static eventResponder myER;
#include "myEventResponders.c"


typedef struct msgbuf {
  long mtype;
  char mtext[3];
} msgbuf;

void thread_cmdNet_start(mqd_t mqd_cmd);
void doEventResponder();
void setEventResponder();

mqd_t setupNetworkQueue();
void setupRobot();

int mq_hasMsg(mqd_t mqd_cmd) {
  struct mq_attr a;
  mq_getattr(mqd_cmd,&a);
  return a.mq_curmsgs;
}

void mq_getMsg(mqd_t mqd_cmd, char* buffer) {
  if (mq_receive(mqd_cmd, buffer, CMD_BUFFER_SIZE, NULL) == -1) {
    perror("mq_receive(): ");
    pthread_exit(NULL);
  }     
}

void setEventResponder(char * erName) {
  cleanupER();

  selectNextER(erName);
  
  //now set the clock for the next event responder if needed
  if (myER.states[myER.curState].clockTime > 0) {
    setClock(myER.states[myER.curState].clockTime,0);
  }
}

int main(void)
{
  //start the program with an event responder to tell it to stop
  initalizeStopER();

  setupRobot();
  setupClock();
  mqd_t mqd_cmd = setupNetworkQueue();

  char cmd_buffer[CMD_BUFFER_SIZE]; 

  while (1) {
    if (mq_hasMsg(mqd_cmd) > 0) {

      mq_getMsg(mqd_cmd, cmd_buffer);
      printf("Got Message: .%s.\n",cmd_buffer);

      //check if this command is telling us to stop
      if (strcmp(cmd_buffer,STOP_MESSAGE) == 0) {
        //TODO: should probally stop the robot and
        //shut down the program. 
        cleanupER();
      }

      setEventResponder(cmd_buffer); 
    }

    doEventResponder();

  }

  printf("Main exiting\n");

  /* Cleanup */
  pthread_exit(NULL);
  mq_close(mqd_cmd);

  return 0;
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

  //initalize things
  printf("cmdNet started\n");


  //while (1) {

  char * message = "go";  //servRead();

  if(mq_send(mqd_cmd, message, 3, 0) != 0)
  {
    perror("msgsend() nerves: ");
    pthread_exit(NULL);
  }

  //if message == STOP_MESSAGE then
  //exit thread after forwarding message

  //}

}

void setupRobot() {
  //Start up robotly things
  if (openPort() == 0) {
    printf("Port failed to open");
    exit(-1);
  }
  initialize();

  //give the roomba some time to process this
  sleep(1);


}

void doEventResponder() { 

  // some pointers to save myself a lot of typing
  eventPredicate * e;
  responder * r;
  nextState n;
  transition * transitions= myER.states[myER.curState].transitions;
  int transitionsCount = myER.states[myER.curState].count;


  //read sensor data
  char sensDataFromRobot[ER_SENS_BUFFER_SIZE] = {'\0'};
  receiveGroupOneSensorData(sensDataFromRobot);
  //gotAlarm contained within clock.c
  sensDataFromRobot[15] = '0'+gotAlarm; 
  gotAlarm = 0;
  //TODO: format data to be sent over the network
  //sendData()


  //Debug printing
  printf("bump right: %d\n",(sensDataFromRobot[0] & SENSOR_BUMP_RIGHT));
  printf("bump left: %d\n",(sensDataFromRobot[0] & SENSOR_BUMP_LEFT));
  printf("wheeldrops: %d\n",(sensDataFromRobot[0] & SENSOR_WHEELDROP_BOTH));

  printf("vwall: %d\n",sensDataFromRobot[0]);

  int eventOccured = 0;  

  // Loop over all of the eventPredicate and responder pairs
  // in the current state.
  int i = 0;
  for(i = 0; i < transitionsCount; i++)
  {

    if (eventOccured == 0) 
    {
      e = transitions[i].e; //event to check against

      if((e)(sensDataFromRobot))
      {
        r = transitions[i].r; //the responder to execute
        r();


        n = transitions[i].n; //the next state that we need to be in
        if (myER.curState != n) {
          printf("State changing from %d to %d\n",myER.curState,n);

          myER.curState = n;
          //transitions = myER.states[n].transitions;
          //transitionsCount = myER.states[n].count;

          int nextAlarm =  myER.states[myER.curState].clockTime;
          if (nextAlarm > 0)  {
            //TODO: reset the alarm somewhere
            setClock(nextAlarm,0);
          }
        }

        //event occured, we don't want to check anymore
        eventOccured = 1;
      }
    }
  }

}


mqd_t setupNetworkQueue() {
  pthread_t tCmdNet;

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

  /* Create a posix thread for net.
   */
  if(pthread_create(&tCmdNet, NULL, thread_cmdNet_start, mqd_cmd) != 0)
  {
    perror("pthread_create(), cmdNet:");
    return -1;
  }

  return mqd_cmd;
}
