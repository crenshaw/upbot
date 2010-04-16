//appHardy
//  Simple program for iRobot Create Roomba
//  Created By:	 Tanya L. Crenshaw
//  Date:	 14 April 2010
//  Description: This program features the robust control
//               commands for the roomba.  It drives and bumps
//               into stuff, but the control commands get
//               feedback from the roomba.  "I told you to drive
//               at 500 mm per second"  Then ask, "What did I just
//               tell you to do?"
//
//  Depends on:	 led.c utility.c

#include <stdio.h>
#include "roomba.h"
#include "../communication/communication.h"

int main()
{
  int c = -1;
  char x[10];
  char * currTime; //[100];
  if (openPort() == 0)
    {
      printf("Port failed to open \n");
      return -1;
    }
  
  currTime = (char *)malloc(100 * 8);

  initialize();
  
  sleep(1);

  setLED(RED, PLAY_ON, ADVANCE_ON);

  FILE* cmdFile = fopen("cmdFile.txt", "r");
  FILE* sensorFile = fopen("sensorFile.txt", "w");

  printf("appHardy running...\n");

  if (cmdFile == NULL || sensorFile == NULL)
    {
      printf("appHardy fopen\n");
      return -1;
    }

  if (driveStraightWithFeedback(MED) == -1)
    printf("Problem with driving");

  
  while(1)
    {
      setLED(RED, PLAY_ON, ADVANCE_ON);
      
      if((c = readAndExecute(cmdFile) == -1))
	{
	  printf("readAndExecute failed\n");
	}
      
      receiveSensorData(SP_BUMPS_WHEELDROPS, x, 1, 1);

      int sensorVal = x[0] & BUMP_SENSORS;
      
      if((sensorVal) == SENSOR_BUMP_RIGHT)
	{
	  currTime = getTime();
	  //turn left
	  turnCounterClockwise(90);

	  if (driveStraightWithFeedback(MED) == -1)
	    printf("Problem with driving");
	}
      
      if((sensorVal) == SENSOR_BUMP_BOTH)
	{
	  currTime = getTime();
	  driveBackwardsUntil(1, MED);
	  //turn right
	  turnClockwise(90);

	  if (driveStraightWithFeedback(MED) == -1)
	    printf("Problem with driving");
	}
      
      if((sensorVal) == SENSOR_BUMP_LEFT)
	{
	  currTime = getTime();
	  //turn right
	  turnClockwise(90);

	  if (driveStraightWithFeedback(MED) == -1)
	    printf("Problem with driving");
	}

      //write the sensor data to a file, if fail print message
      if(writeSensorDataToFile(sensorVal, sensorFile, currTime) == -1)
	{
	  // printf("Problem with writing sensor data to file\n");
	}
	  

      x[0] = 0x00;
    }
  
  fclose(cmdFile);
  fclose(sensorFile);
  free(currTime);

  if (closePort() == -1)
    {
      printf("Port failed to close \n");
      return -1;
    }
  return 0;
}
