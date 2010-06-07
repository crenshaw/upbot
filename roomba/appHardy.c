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

  unsigned char x[10] = {0};
  char y[10] = {0};

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

  // if (driveStraightWithFeedback(LOW) == -1)
  //  printf("Problem with driving");
  stop();
  driveStraight(MED);

  setLED(RED, PLAY_ON, ADVANCE_ON);

  while(1)
    {
      
      
      // check if there is an external command to execute;
      // if so, execute it.
      if((c = readAndExecute(cmdFile)) == -1)
	{
	  printf("readAndExecute failed\n");
	}
      
      // query sensor data from the iRobot Create.

      
      // receiveSensorData(SP_BUMPS_WHEELDROPS, x, 1, 1);
      
      receiveGroupOneSensorData(x);
      //receiveSensorData(SP_CLIFF_F_LEFT_SIGNAL, y, 2, 1);

      //receiveSensorData(SP_GROUP_ONE, x, 10, 1);      
      //int i;

      /*      receiveGroupFourSensorData(y);
      printf("sensorSignal: ");
      for(i = 0; i <= 3; i++)
	{
	  printf("0x%x ", y[i]);
	}
      fflush(stdout);
      printf("\n");
      */
      // check if any of the sensor data indicates a 
      // sensor has been activated.  If so, react be
      // driving backwards briefly, stopping, and then
      // conveying the sensor data to a file.
      if(checkSensorData(x))
	{
	  // For debugging purposes, print all the sensor
	  // data just read.
	  /* for(i = 0; i <= 6; i++)
	    {
	      printf("0x%x ", x[i]);
	      fflush(stdout);
	      }*/

	  //	  printf("\n");

	  //drive backwards and then stop
	  driveBackwardsUntil(EIGHTH_SECOND, MED);
	  //printf("sensorSignal: %x\n", y[0]);
	  //printf("sensorSignal: %x\n", y[1]);
	  currTime = getTime();
	  writeSensorDataToFile(x, sensorFile, currTime);
	}
      int i;
      for(i = 0; i <= 6; i++)
	{
	  x[i]= FALSE;
	}

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
