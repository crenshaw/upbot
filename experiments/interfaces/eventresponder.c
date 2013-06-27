#include <stdio.h>
#include "../../roomba/roomba.h"

// ************************************************************************
// PART 2: EVENT:RESPONDERS
//
// TODO: Need to separate the notions of events, responses, and clocks.
// Right now, this is all wadded up in a single "responder" function.
// ************************************************************************

/** 
 * A type to represent state ?
 */

/**
 * A type to represent events, i.e., sensor data.  Empty for now.
 */
typedef struct event event;

/**
 * A type to represent an event predicate.  An event predicate is a
 * function that returns true (1) or false (0), indicating whether or
 * not an event or set of events have occurred based on a set of
 * sensor data.  Defining this function as a type allows us to pass
 * eventPredicate functions to other functions.
 *
 * @param[in] data an integer array of sensor data.
 *
 * @returns true (1) or false (0) indicating whether or not an event
 * or set of events have occured.
 */
typedef int eventPredicate(int * data);


/**
 * A type to represent a responder. A responder is a function that
 * issues an appropriate command or set of commands to the robot.
 * Responder functions return no value.  Defining this function as a
 * type allows us to pass responder functions to other functions.
 *
 * @returns none.
 */
typedef void responder(void);


/**
 * A type to represent an event:responder pair.  An event:responder is
 * an array of eventPredicate and responder function pairs.  Each pair
 * {e, r} can be interpreted as:
 *
 * if the eventPredicate function, e, returns true then
 *    execute the responder, r.
 * 
 */
typedef struct erTag {
  eventPredicate * e;
  responder * r;
} er;





/**
 * setResponder()
 *
 * Set the responder for the robot.  Software considers the robot a
 * single-threaded state machine.  If this function is called when a
 * robot already has a responder, the function will block until the
 * current responder is cleared.  This function will not block if it
 * is called from within the current responder.
 *
 * @param[in] robot a handle representing the robot to command.
 * @param[in] responder 
 * 
 * @return none.
 */


/*
void setResponder(robot * robot, responder responder)
{

}
*/







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
// EXAMPLE MAIN
// ************************************************************************

int main(void)
{

  // Create some fake data to treat as sensor data, i.e., events.
  int fakeData[11] = {0};

  // Initialize a random number generator to help with the
  // generation of random data.
  srand(time(NULL));  

  er myEventResponder = {eventOne, respondOne};

  // Create an event loop
  while(1)
    {
      // Make the fake data change at index 0.  Choose a random number
      // between 0 and 2.  This would be equivalent to asking the
      // robot for some sensor data.
      fakeData[0] = rand()% (3);
      printf("The fake data value is %d \n", fakeData[0]);
      sleep(1);


      if((myEventResponder.e)(fakeData))
	{
	  (myEventResponder.r)();
	}
     
    
    }

  return 0;

}
