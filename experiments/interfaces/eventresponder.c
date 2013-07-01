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
#include "../../roomba/roomba.h"
#include "eventresponder.h"
#include "robot.h"
#include "services.h"


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

  eventresponder myEventResponder = {eventOne, respondOne};

  // Create an event loop
  while(1)
    {
      // Make the fake data change at index 0.  Choose a random number
      // between 0 and 2.  This would be equivalent to asking the
      // robot for some sensor data.
      fakeData[0] = rand()% (3);
      printf("The fake data value is %d \n", fakeData[0]);

//
      sleep(1);


      if((myEventResponder.e)(fakeData))
	{
	  (myEventResponder.r)();
	}
     
    
    }

  return 0;

}
