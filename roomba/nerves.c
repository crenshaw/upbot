/** 
 * nerves.c
 * 
 * Simple program for iRobot Create Roomba
 * Created By:	 Tanya L. Crenshaw
 * Date:	 14 April 2010
 * 
 */
#include <stdio.h>
#include "roomba.h"
#include "../communication/communication.h"

/**
 * nerves()
 *
 * @arg cmdArea a pointer to a small piece of shared memory by
 * which this function receives control commands from another
 * process.
 *
 * @return 0 if successful, -1 otherwise
 */
int nerves(caddr_t cmdArea, caddr_t sensArea)
{
  int cmd = -1;
  
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

  if (cmdFile == NULL || sensorFile == NULL)
    {
      perror("nerves fopen\n");
      return -1;
    }

  stop();
  driveStraight(MED);

  setLED(RED, PLAY_ON, ADVANCE_ON);

  while(cmd != ssQuit)
    {   
      // Look in the shared memory cmdArea to determine if 
      // there is an external command to execute;
      // if so, execute it.
      if((cmd = readAndExecute(cmdFile, cmdArea)) == -1)
	{
	  perror("readAndExecute failed\n");

	}
      
      receiveGroupOneSensorData(x);

      
      // check if any of the sensor data indicates a 
      // sensor has been activated.  If so, react be
      // driving backwards briefly, stopping, and then
      // conveying the sensor data to a file.
      if(checkSensorData(x))
	{

	  //drive backwards and then stop
	  driveBackwardsUntil(EIGHTH_SECOND, MED);

	  currTime = getTime();
	  writeSensorDataToFile(x, sensorFile, currTime);
	  
	  writeSensorDataToSharedMemory(x, sensArea, currTime);

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
      perror("Port failed to close \n");
      return -1;
    }
  return 0;
  
}
