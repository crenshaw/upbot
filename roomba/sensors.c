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

  //TLC: Determine if "numIter" is really necessary anymore.
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

  for(i = SP_CLIFF_LEFT; i <= SP_VIRTUAL_WALL; i++)
    {
      x++;
      receiveSensorData(i, x, 1, 1);
    }
}
