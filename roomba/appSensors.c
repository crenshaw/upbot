//aptSensors
//Simple program for iRobot Create Roomba
//Created By:	Steven M. Beyer
//Date:		25 February 2010
//Description:	Drives around getting sensor data
//Depends on:	sensors.c move.c utility.c

#include <stdio.h>
#include "roomba.h"

int main()
{
  char x[10];

 if(!openPort())
    {
      printf("Unable to open serial port to Roomba. \n");
      return -1;
      }
  initialize();
  
  while(1)
    {
      x[0] = 0;
      receiveSensorData(SP_BUMPS_WHEELDROPS, x, 1, 1);
	
      driveStraight(MED);

      if((x[0] & BUMP_SENSORS) == SENSOR_BUMP_RIGHT)
	{
	  //turn left
	  turnCounterClockwise(90);
	  driveStraight(MED);
	}

      if((x[0] & BUMP_SENSORS) == SENSOR_BUMP_BOTH)
	{
	  driveBackwardsUntil(1, MED);
	  //turn right
	  turnClockwise(90);
	  driveStraight(MED);
	}
      
      if((x[0] & BUMP_SENSORS) == SENSOR_BUMP_LEFT)
	{
	  //turn right
	  turnClockwise(90);
	  driveStraight(MED);
	}

    }

  if(closePort())
    printf("file closed \n");

  return 0;
}

