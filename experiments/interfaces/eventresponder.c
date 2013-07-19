/**
 * eventresponder.c
 *
 * The UPBOT System treats a robot as a single-threaded state machine
 * that is programmed using an event:responder.
 *
 * Events are the sensor data provided by the robot.  The UPBOT system
 * considers the following subset of sensor data: Wheeldrop caster,
 * wheeldrop left, wheeldrop right, bump left, bump right, cliff left,
 * cliff front left, cliff front right, cliff right, and the virtual
 * wall.  
 *
 * Checking for a particular event is done by an eventPredicate
 * function.
 *
 * Responses are the commands that may be issued to the iRobot Create
 * that alter its behaviour in the physical world, such as drive or 
 * blink LEDs.
 *
 * Issuing a response is done by a response function.
 *
 * An event:responder is a set of eventPredicate() and response()
 * function pairs.  
 * 
 * 
 * @author: Tanya L. Crenshaw
 * @since: June 2013
 * 
 */

#include <stdio.h>
#include <stdlib.h>
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

// Global value to keep track of the alarm occurrence 
// Set nonzero on receipt of SIGALRM 
// TODO: Global?  Really?  Can this be better?
static volatile sig_atomic_t gotAlarm = 0; 

/**
 * createResponder
 *
 * Given e, an array of eventPredicate functions and r, an array of
 * responder functions, populate an eventresponder struct with 
 * e, r, and the length of the arrays.  
 *
 * Each array must be terminated by NULL.  Here is an example
 * definition of an eventPredicate array:
 *
 *   eventPredicate * eArray[3] = {eventOne, eventTwo, NULL};
 * 
 * The array lengths must match.  If they do not, then we do not have
 * a well-formed event:responder and the function returns
 * ER_ARRAY_MISMATCH (-1) to indicate an error.
 *
 * Each array must exist.  If either array is NULL, then we do not
 * have a well-formed event:responder and the function returns
 * ER_NULL_ARRAY (-2) to indicate an error.
 *
 * The eventresponder, er, must already be allocated before calling
 * this function.  If er is NULL, then we cannot populate the
 * event:responder and the function returns ER_NULL_ER (-3) to
 * indicate an error.
 *
 * If both arrays and er are not NULL, and the two arrays are the same
 * length, the function returns ER_SUCCESS (0) to indicate success.
 *
 * @param[in] e an array of eventPredicate functions.
 * @param[in] r an array of responder functions.
 * @param[out] the newly populated eventresponder.
 * 
 */

int createResponder(eventPredicate * e[], responder * r[], eventresponder * er)
{
  int eSize = 0;
  int rSize = 0;
  int i = 0;

  // If either array is NULL, bail with an error.
  if (e == NULL || r == NULL )
    {
      return ER_NULL_ARRAY;
    }

  // If the event:responder to populate is NULL, bail with an error.
  if (er == NULL)
    {
      return ER_NULL_ER;
    }
  
  // How long is each array?
  while (e[i] != NULL)
    {
      eSize++;
      i++;
    }

  i = 0;

  while (r[i] != NULL)
    {
      rSize++;
      i++;
    }

  // If the array sizes differ, bail with an error.
  if(eSize != rSize)
    {
      return ER_ARRAY_MISMATCH;
    }

  // OTHERWISE, populate the event:responder and return success.
  er->e = e;
  er->r = r;
  er->length = rSize;

  return ER_SUCCESS;
  

}


// ************************************************************************
// EXAMPLE CLOCK HANDLER
//
// ************************************************************************

static void
signalrmHandler(int sig)
{
  gotAlarm = 1;
}


// ************************************************************************
// EXAMPLE MAIN.
// Create timer-based interrupts and an event:responder to handle a set
// of clocks and sensor data events in a faked up system.
//
// This example main should eventually evolve into the new "nerves"
// portion of the UPBOT robotics system.
// ************************************************************************

int main(void)
{


 //*
 erPair erS0[3] = {
    {eventAlarm, respondDriveMed, 1},
    {eventNotBump, respondDriveLow, 0},
    {eventBump, respondTurn, 0}    
 }; 

 state S0 = {
    5, //alarm time
    erS0,
    3  //number of events
 };

 erPair erS1[2] = {
    {eventNotBump, respondDriveMed, 1},
    {eventBump, respondTurn, 0}
 };
 
 state S1 = {
    0, //alarm time
    erS1,
    2  //number of events
 };


 state states[2] = {S0,S1};

 er myER[2] = {
    states,//list of states
    0,   //default state
    2    //number of states
 };

  //*/

 //int state = 0;
 /*
 :( this format doesn't work, I miss you Lua.
 It's so pretty I think i'll keep it for a while
  
  er myER[2] = {
      { //states
        { //state 0
            5,  //alarm time
            {
                {eventAlarm, respondDriveMed, 1},
                {eventNotBump, respondDriveLow, 0},
                {eventBump, respondTurn, 0}
            },
            3   //number of events
        },
        { //state 1
            0,  //alarm time
            {
                {eventNotBump, respondDriveMed, 1},
                {eventBump, respondTurn, 0}
            },
            2   //number of events
        }
      },
      0,    //default state
      2     //number of states
  }
  //*/

  // Create arrays of eventPredicate and responder functions
  //IMPORTANT REMINDER: Events must be ordered in priority you
  //want them to be chosen in, this means alarms must be the
  //first event per state
  initialState iArray[6] = {0, 0, 0, 1, 1, -1};
  eventPredicate * eArray[6] = {eventAlarm, eventNotBump, eventBump, eventNotBump, eventBump, NULL};
  responder * rArray[6] = {respondDriveMed, respondDriveLow, respondTurn, respondDriveMed, respondTurn,  NULL};
  statePointer pArray[6] = {1, 0, 0, 1, 0, -1};
  
  alarmTime aArray[3] = {5,0,-1};
   
  // Manually create and initialize an event:responder
  eventresponder myEventResponder = {iArray, eArray, rArray, pArray,aArray, 5, 0};



  // Declare the variables necessary to support timer-based interrupts.
  struct itimerval itv;       // Specify when a timer should expire 

  struct sigaction sa;        // Signal sets
  struct sigaction toggledsa;   

  // Initialize a random number generator to help with the
  // generation of random data.
  srand(time(NULL));  



  // Initialize and empty a signal set
  sigemptyset(&sa.sa_mask);

  // Set the signal set.
  sa.sa_flags = 0;
  sa.sa_handler = signalrmHandler;

  // Update the signal set with the new flags and handler.
  if (sigaction(SIGALRM, &sa, NULL) == -1)
    {
      exit(EXIT_FAILURE);
    }

  
  // Initialize timer start time and period:
  // First, the period between now and the first timer interrupt 
  itv.it_value.tv_sec = myER->states[myER->curState].alarmTime;
  //itv.it_value.tv_sec = aArray[myER->curState];  // seconds
  itv.it_value.tv_usec = 0; // microseconds

  // Second, the intervals between successive timer interrupts 
  itv.it_interval.tv_sec = 0; // seconds
  itv.it_interval.tv_usec = 0; // microseconds
  
  if (myER->states[myER->curState].alarmTime > 0)
  {
    if (setitimer(ITIMER_REAL, &itv, NULL) == -1)
    {
      exit(EXIT_FAILURE);
    }
  }

  // Create and initialize a robot.
  robot myRobot = {NULL, 
		   "Webby", 
		   "10.81.3.181", 
		   &myEventResponder, 
		   NULL, 
		   NULL};

  //Start up robotly things
  if (openPort() == 0) {
      printf("Port failed to open");
      exit(-1);
  }
  initialize();
  sleep(1);

  // Issues that the event:responder loop must be able to handle.
  // 1. The event:responder is accessed via a robot variable.
  // 2. The event:responder may update itself at any response.
  // x done x 3. The event:responder is a set of pairs of eventPredicate
  //    and response functions.
  // 4. Clocks must be masked whenever they aren't being checked.
  //    (I am not sure if this is true).
  // 5. A small library should be created to hide the complexity of clocks
  //    from the application programmer.


  // Access the event:responder via the robot variable.  (I am doing
  // this to make sure I understand the types.  I know that I can
  // access the event:responder via myEventResponder, declared above).
  eventresponder * er = myRobot.er;

  // Need a pointer for the e's and a pointer for the r's.
  eventPredicate * e;
  responder * r;
  statePointer p;


  erPair * pairs= myER->states[myER->curState].erPairs;
  int pairsCount = myER->states[myER->curState].count;
   
  // Create an event loop
  while(1)
    {
      
      //usleep(200000);
      char sensDataFromRobot[150] = {'\0'};
      receiveGroupOneSensorData(sensDataFromRobot);
      
      printf("bump right: %d\n",(sensDataFromRobot[0] & SENSOR_BUMP_RIGHT));
      printf("bump left: %d\n",(sensDataFromRobot[0] & SENSOR_BUMP_LEFT));
      printf("wheeldrops: %d\n",(sensDataFromRobot[0] & SENSOR_WHEELDROP_BOTH));
     
      printf("vwall: %d\n",sensDataFromRobot[0]);
      
      sensDataFromRobot[15] = '0'+gotAlarm; 
      gotAlarm = 0;
      
      // ***********************************************************
      // I leave this here because I may need to know about masking
      // signals among threads later. -- TLC
      // 
      // Mask signals ?
      //   Kernel maintains a _signal mask_ for each process
      //   The signal mask is actually a per-thread attribute!

      //  Note that, "The pthread_sigmask() function is just like
      //  sigprocmask(2), with the difference that its use in
      //  multithreaded programs is explicitly specified by
      //  POSIX.1-2001."
      // ***********************************************************
   

#ifdef DONOTCOMPILE
      // Masking the signal interrupt.
      printf("    masking....\n");
      sigprocmask(SIG_BLOCK, &sa.sa_mask, NULL);
#endif


      // Loop over all of the eventPredicate and responder pairs
      // in the robot's event:responder.
      int i = 0;
      
      //int length = er->length;
                 
      int eventOccured = 0;  
      for(i = 0; i < pairsCount; i++)
	{
	  //is = (er->i)[i];  //get the i'th state requirment
      if (eventOccured == 0) 
      {
        //e = (er->e)[i];  // Get the i'th eventPredicate function.
        e = pairs[i].e;

	    if((e)(sensDataFromRobot))
	      {
	        r = pairs[i].r;
            //r = (er->r)[i];  // Get the i'th responder function.
            //p = (er->p)[i];
            p = pairs[i].p;
            r();
            if (myER->curState != p) {
                printf("State changing from %d to %d\n",myER->curState,p);
                myER->curState = p;
                pairs = myER->states[p].erPairs;
                pairsCount = myER->states[p].count;

            }


            //BUG: when going from event occurs but state doesn't change
            //      alarm isn't reset. Probable that this occurs
            //      across states. Investigate further.
            int nextAlarm =  myER->states[myER->curState].alarmTime;
            if (nextAlarm > 0) 
            { 
                itv.it_value.tv_sec = nextAlarm;
                if (setitimer(ITIMER_REAL, &itv, NULL) == -1)
                {
                    exit(EXIT_FAILURE);
                }
            }

            eventOccured = 1;
	      }
      }
	}


#ifdef DONOTCOMPILE
      // Unmasking the signal interrupt.
      sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL);     
      printf("    unmasking....\n");
#endif
  }
  return 0;

}
