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
// EVENTPREDICATES, WRITTEN BY USER
// ************************************************************************

/**
 * eventTrue
 * 
 * Default eventPredicate function.  Always returns true.
 */
int eventTrue(char * data)
{
  return 1;
}

/**
 * eventOne
 *
 * Example eventPredicate function, as written by an application developer.
 */
int eventOne(char * data)
{
  printf("   Calling eventOne\n");

  if(data[0] == 1)
    return 1;
  else
    return 0;
}

/**
 * eventTwo
 *
 * Example eventPredicate function, as written by an application developer.
 */
int eventTwo(char * data)
{
  printf("   Calling eventTwo\n");

  if(data[0] == 2)
    return 1;
  else
    return 0;
}

/**
 * eventBump
 * 
 * Example eventPredicate function for checking for bump events.
 */
int eventBump(char * data) {

  // Check bump sensors.

  // TODO: Need to correct the use of literal 0 when checking
  // the state of the bump sensors.
  if(((data[0] & SENSOR_BUMP_RIGHT) == SENSOR_BUMP_RIGHT) || 
     ((data[0] & SENSOR_BUMP_LEFT ) == SENSOR_BUMP_LEFT))
    {
      return 1;
    }
  else
    return 0;
}

/**
 * eventNotBump
 * 
 * Example eventPredicate function for checking for bump events.
 */
int eventNotBump(char * data) {

  // Check bump sensors.

  // TODO: Need to correct the use of literal 0 when checking
  // the state of the bump sensors.
  if(((data[0] & SENSOR_BUMP_RIGHT) == SENSOR_BUMP_RIGHT) || 
     ((data[0] & SENSOR_BUMP_LEFT ) == SENSOR_BUMP_LEFT))
    {
      return 0;
    }
  else
    return 1;
}


// ************************************************************************
// RESPONDERS, WRITTEN BY USER
// ************************************************************************

/**
 * respondStop
 * 
 * Default responder function.  Stop the robot.
 */ 
void respondStop(void)
{
  // TODO: Add code to stop the robot.
  printf("Reminder: add code to actually stop the robot\n");

  return;
}

void respondDrive(void) {
      printf("Drive!"); 
      driveStraightUntil(1,LOW);
}


/**
 * respondOne
 *
 * Example responder function, as written by an application developer.
 */
void respondOne(void)
{
  printf("      Got a 1!\n");

  return;
}

/**
 * respondTwo
 *
 * Example responder function, as written by an application developer. 
 */
void respondTwo(void)
{
  printf("      Got a 2!\n");

  return;
}

/**
 * respondTurn
 *
 * Turns the robot a random amount
 */
void respondTurn(void) {
    //turnClockwise(90);
    turnRandom(300000,1800000);
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
  itv.it_value.tv_sec = 1;  // seconds
  itv.it_value.tv_usec = 0; // microseconds

  // Second, the intervals between successive timer interrupts 
  itv.it_interval.tv_sec = 10; // seconds
  itv.it_interval.tv_usec = 0; // microseconds

  if (setitimer(ITIMER_REAL, &itv, NULL) == -1)
    {
      exit(EXIT_FAILURE);
    }


  // Create arrays of eventPredicate and responder functions
  eventPredicate * eArray[3] = {eventNotBump, eventBump, NULL};
  responder * rArray[3] = {respondDrive, respondTurn, NULL};

  // Manually create and initialize an event:responder
  eventresponder myEventResponder = {eArray, rArray, 2};

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
  eventPredicate * e = (er->e)[0]; 
  responder * r = (er->r)[0];

  //turnClockwise(90000000);

  // Create an event loop
  while(1)
    {
      
      char sensDataFromRobot[150] = {'\0'};
      receiveGroupOneSensorData(sensDataFromRobot);
      int a = checkSensorData(sensDataFromRobot);

      printf("data in: %d\n",a);

      // Make the fake data change at index 0.  Choose a random number
      // between 0 and 2.  This would be equivalent to asking the
      // robot for some sensor data.
      //fakeData[0] = rand()% (3);
      //printf("The fake data value is %d \n", fakeData[0]);

      // Sleep for one second just to make output of prototype manageable.
      sleep(1);

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
      int length = er->length;

      for(i = 0; i < length; i++)
	{
	  e = (er->e)[i];  // Get the i'th eventPredicate function.
	  r = (er->r)[i];  // Get the i'th responder function.

	  if((e)(sensDataFromRobot))
	  //if((e)(fakeData))
	    {
	      r();
	    }
	}


#ifdef DONOTCOMPILE
      // Unmasking the signal interrupt.
      sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL);     
      printf("    unmasking....\n");
#endif

      // I am checking for the signal interrupt, just once, here.
      // That's not my favorite, but I'm not using signals in a
      // meaningful way yet. 
      if(gotAlarm)
	{
	  gotAlarm = 0;
	  printf("**The british are coming!**\n");
	}

    }

  return 0;

}
