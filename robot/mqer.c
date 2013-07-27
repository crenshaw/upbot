#include "mqer.h"

#include "eventresponder.h"
static eventResponder myER;

#include "../roomba/roomba.h"
#include "roomba/roomba.h"

#include "events.c"
#include "responders.c"

#include "clock.c"
#include "commandQueue.c"
#include "erControl.c"
#include "myEventResponders.c"

int main(void)
{
  //start the program with an event responder to tell it to stop
  initalizeStopER();

  setupRoomba();
  setupClock();
  mqd_t mqd_cmd = setupCommandQueue();

  char cmd_buffer[CMD_BUFFER_SIZE]; 

  transition * transitions= myER.states[myER.curState].transitions;
  int transitionsCount = myER.states[myER.curState].count;

  while (1) {
    if (cmdQ_hasMsg(mqd_cmd) > 0) {

      cmdQ_getMsg(mqd_cmd, cmd_buffer);
      printf("Got Message: .%s.\n",cmd_buffer);

      //check if this command is telling us to stop
      if (strcmp(cmd_buffer, QUIT_MESSAGE) == 0) {
        //okay, break out of the loop so we can clean things up
        break;
      }

      setEventResponder(cmd_buffer);
      
      int state = myER.curState;
      transitions = myER.states[state].transitions;
      transitionsCount = myER.states[state].count;
    }

    //read sensor data
    char sensDataFromRobot[ER_SENS_BUFFER_SIZE] = {'\0'};
    getSensorData(sensDataFromRobot);
    
    
    int eventOccured = 0;  

    // Loop over all of the eventPredicate/responders in current state
    int i = 0;
    for(i = 0; i < transitionsCount; i++) {

      if (eventOccured == 0) {
        eventPredicate* e = transitions[i].e; //event to check against

        if((e)(sensDataFromRobot)) {
          responder* r = transitions[i].r; //the responder to execute
          r();


          nextState n = transitions[i].n; //the next state to go to
          if (myER.curState != n) {
            printf("State changing from %d to %d\n",myER.curState,n);

            myER.curState = n;
            transitions = myER.states[n].transitions;
            transitionsCount = myER.states[n].count;

            int nextAlarm =  myER.states[myER.curState].clockTime;
            if (nextAlarm > 0)  {
              //TODO: reset the alarm somewhere
              setClock(nextAlarm,0);
            }
          }

          //event occured, we don't want to check anymore
          eventOccured = 1;
        
        } //if event passes
      } //if event hasn'toccured
    } //transitions loop
  } //forever loop

  printf("Main exiting\n");

  /* Cleanup */
  respondStop();  //stop the robot
  cleanupER();  //cleanup er data on heap
  closePort();  //close connection to roomba
  mq_close(mqd_cmd); //close our command queue

  return 0;
}


void getSensorData(char* sensDataFromRobot) {
  receiveGroupOneSensorData(sensDataFromRobot);
  //gotAlarm contained within clock.c
  sensDataFromRobot[15] = '0'+gotAlarm; 
  gotAlarm = 0;
  //TODO: format data to be sent over the network
  //sendData() 

  //Debug printing
  printf("bump right:%d\n",(sensDataFromRobot[0] & SENSOR_BUMP_RIGHT));
  printf("bump left:%d\n",(sensDataFromRobot[0] & SENSOR_BUMP_LEFT));
  printf("wheeldrops:%d\n",(sensDataFromRobot[0] & SENSOR_WHEELDROP_BOTH));
  printf("vwall:%d\n",sensDataFromRobot[0]);
}

