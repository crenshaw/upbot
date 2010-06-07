/**
 * appSimple
 *
 * Simple program for iRobot Create Roomba
 *
 * @author Steven M. Beyer
 * @since  24 February 2010
 *
 */

#include <stdio.h>
#include "roomba.h"
#include "../communication/communication.h"

/** 
 * main()
 *
 * Turns on all LEDs with the Play at Red at full
 * intensity. Subsequently drives until a bump sensor is
 * activated. Upon bump sensor activation, the iRobot Create turns and
 * continues driving. All the while, the program looks for external
 * commands to execute in cmdFile.txt.
 * 
 * Useful for initial debugging of new gumstix hardware interfaced to
 * an iRobot Create.
 *
 */
int main()
{
  int c = -1;
  char x[10];

  printf("appSimple running ... \n");

  if (openPort() == 0)
    {
      printf("Port failed to open \n");
      return -1;
    }
  
  initialize();

  printf("   initialization of roomba complete. \n");

  sleep(1);

  setLED(RED, PLAY_ON, ADVANCE_ON);

  printf("   led initialization complete. \n");

  FILE* fp = fopen("cmdFile.txt", "r+a");

  if (fp == NULL)
    {
      printf("appSimple fopen\n");
      return -1;
    }
  driveStraight(MED);

  printf("   driving...\n");
  
  while(1)
    {

      setLED(RED, PLAY_ON, ADVANCE_ON);
      
      if((c = readAndExecute(fp)) == -1)
	{
	  printf("readAndExecute failed\n");
	}
      
      receiveSensorData(SP_BUMPS_WHEELDROPS, x, 1, 1);
      
      
      if((x[0] & SENSOR_BUMP_BOTH) == SENSOR_BUMP_RIGHT)
	{
	  //turn left
	  turnCounterClockwise(90);
	  driveStraight(MED);
	}
      
      if((x[0] & SENSOR_BUMP_BOTH) == SENSOR_BUMP_BOTH)
	{
	  driveBackwardsUntil(1, MED);
	  //turn right
	  turnClockwise(90);
	  driveStraight(MED);
	}
      
      if((x[0] & SENSOR_BUMP_BOTH) == SENSOR_BUMP_LEFT)
	{
	  //turn right
	  turnClockwise(90);
	  driveStraight(MED);
	}

      x[0] = 0x00;
    }
  
  fclose(fp);
  
  if (closePort() == -1)
    {
      printf("Port failed to close \n");
      return -1;
    }
  return 0;
}
