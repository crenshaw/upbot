//sensors.c
//Description:
//		Provides functions which carry out Roomba sensor commands
//Author:
//		Steven M. Beyer
//Date Created:
//		18 February 2010

#include "roomba.h"

//receive specified packet from the roomba sensor
void receiveSensorData(int packet, char* x, int numBytes, int numIter)
{
  byteTx(CmdSensors);
  byteTx(packet);
  byteRx(x, numBytes, numIter);
}
