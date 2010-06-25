//sensors.c
//Description:
//		Provides functions which carry out Roomba sensor commands
//Author:
//		Steven M. Beyer
//Date Created:
//		18 February 2010

#include "roomba.h"

/**
 * receiveSensorData()
 * 
 * Query for and receive the specified sensor data packet from the
 * iRobot.
 *
 * @return void
 */
void receiveSensorData(int packet, char* x, int numBytes, int numIter)
{
  
  byteTx(CmdSensors);
  byteTx(packet);
  byteRx(x, numBytes, numIter);
}

/**
 * receiveGroupOneSensorData()
 *
 * Query for and receive the follow sensor packets from the group
 * packet 1: bump, cliff, virtual wall.
 *
 * @return void
 */
void receiveGroupOneSensorData(char * x)
{
  int i = 0;

  receiveSensorData(SP_BUMPS_WHEELDROPS, x, 1, 1);

  //skip wall sensor data
  x++;

  for(i = SP_CLIFF_LEFT; i <= SP_VIRTUAL_WALL; i++)
    {
      x++;
      receiveSensorData(i, x, 1, 1);
    }
}

/**
 */
void receiveGroupFourSensorData(char * x)
{
  int i = 0;
 
  for(i = SP_CLIFF_LEFT_SIGNAL; i<= SP_CLIFF_RIGHT_SIGNAL; i++)
    {
      receiveSensorData(i, x, 1, 1);
      x++;
    }
}
/** streamSensorData
 *
 * request specific packets to be sent every 15 ms
 * and parse the data the first time
 * (incomplete function needs to implement constant read of data from
 * roomba)
 *
 * @arg x pointer to character array
 *
 * @return void
 */
void streamSensorData(char * x)
{
  char  disregardedData = '\0';
  int i;
  byteTx(CmdStream);
  byteTx(6); // request for 6 packets
  byteTx(7); // bumps and wheel drops
  byteTx(9); // cliff left
  byteTx(10); // cliff front left
  byteTx(11); // cliff front right
  byteTx(12); // cliff right
  byteTx(13); // virtual wall
  
  // for the amount of returned data
  for(i = 0; i < 15; i++)
    {
      // if the data received is packet data then store
      // into saved value
      if(i > 2 && i%2 != 0)
	{
	  byteRx(x, 1, 1);
	}
      // else store and disregard to allow to increment
      // roomba's buffer
      else
	{
	  byteRx(&disregardedData, 1, 1);
	}
    }
}
