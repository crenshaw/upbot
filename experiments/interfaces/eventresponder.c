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


// Global value to keep track of the alarm occurrence 
// Set nonzero on receipt of SIGALRM 
// TODO: Global?  Really?  Can this be better?
static volatile sig_atomic_t gotAlarm = 0; 


// ************************************************************************
// Usage
// ************************************************************************


/**
 * eventOne
 *
 * Example eventPredicate function, as written by an application developer
 */
int eventOne(int * data)
{
  printf("   Calling eventOne\n");

  if(data[0] == 1)
    return 1;
  else
    return 0;
}


/**
 * respondOne
 *
 * Example responder function 
 */
void respondOne(void)
{
  printf("      Got a 1!\n");

  return;
}


/**
 * eventBump
 * 
 * Example eventPredicate function for checking for bump events.
 */
int eventBump(int * data) {

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
// ************************************************************************

int main(void)
{

  // Declare the variables necessary to support timer-based interrupts.
  struct itimerval itv;       // Specify when a timer should expire 

  struct sigaction sa;        // Signal sets
  struct sigaction toggledsa;   

  // Create some fake data to treat as sensor data, i.e., events.
  int fakeData[11] = {0};

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

  // Create and initialize an event:responder
  eventresponder myEventResponder = {eventOne, respondOne};

  // Create an event loop
  while(1)
    {
      // Make the fake data change at index 0.  Choose a random number
      // between 0 and 2.  This would be equivalent to asking the
      // robot for some sensor data.
      fakeData[0] = rand()% (3);
      printf("The fake data value is %d \n", fakeData[0]);

      // Sleep for one second just to make output of prototype manageable.
      sleep(1);

      // Mask signals ?
      //   Kernel maintains a _signal mask_ for each process
      //   The signal mask is actually a per-thread attribute!

      //  Note that, "The pthread_sigmask() function is just like
      //  sigprocmask(2), with the difference that its use in
      //  multithreaded programs is explicitly specified by
      //  POSIX.1-2001."
      
      printf("    masking....\n");

      sigprocmask(SIG_BLOCK, &sa.sa_mask, NULL);

      if((myEventResponder.e)(fakeData))
	{
	  (myEventResponder.r)();
	}

      sigprocmask(SIG_UNBLOCK, &sa.sa_mask, NULL);     

      printf("    unmasking....\n");
    
      if(gotAlarm)
	{
	  gotAlarm = 0;
	  printf("**The british are coming!**\n");
	}

    }

  return 0;

}
