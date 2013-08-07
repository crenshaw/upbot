#include "roomba.h"
/*
 * This file contains functions pertaining to roomba sensors
 */

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
  
  byteTx(RCMD_SENSORS);
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
 * checkSensorData()
 *
 * This boolean function checks if any of the following 
 * sensors in packet group 1 have been activated: bump,
 * cliff, and virtual wall sensors. 
 *
 * @arg x pointer to sensor data group 1
 * 
 * @return 1 if any sensor has been activated, 0 otherwise.
 */ 
int checkSensorData(char *x)
{
  int i;
  // Check cliff and virtual wall sensors; return true 
  // if any sensor has been activated  
  for(i = SP_G1_CLIFF_LEFT; i <= SP_G1_VIRTUAL_WALL; i++)
    {
      if(x[i] == ACTIVE_SENSOR)
	{
	  STOP_MACRO;
	  return TRUE;
	}
    }

  // Check bump sensors; return true if either bump sensor
  // has been activated 
  if(((x[0] & SENSOR_BUMP_RIGHT) == SENSOR_BUMP_RIGHT) || 
     ((x[0] & SENSOR_BUMP_LEFT ) == SENSOR_BUMP_LEFT) ||
     ((x[0] & SENSOR_WHEELDROP_RIGHT) == SENSOR_WHEELDROP_RIGHT) ||
     ((x[0] & SENSOR_WHEELDROP_LEFT) == SENSOR_WHEELDROP_LEFT) ||
     ((x[0] & SENSOR_WHEELDROP_BOTH) == SENSOR_WHEELDROP_BOTH) ||
     ((x[0] & SENSOR_WHEELDROP_CASTER) == SENSOR_WHEELDROP_CASTER))
    {
      STOP_MACRO;
      return TRUE;
    }

  // No sensor has been activated. 
  return FALSE;
}
