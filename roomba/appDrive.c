//aptDrive
//Simple program for iRobot Create Roomba
//Created By:	Steven M. Beyer
//Date:		25 February 2010
//Description:	Drives around
//Depends on:	move.c utility.c

#include <stdio.h>
#include "roomba.h"


int main()
{
	if (openPort() == 0)
	{
		printf("Port failed to open \n");
		return -1;
	}

	initialize();
	sleep(1);
	driveStraightUntil(2, MED);
	turnCounterClockwise(180);
	driveBackwardsUntil(2, MED);
	turnClockwise(90);
	driveStraightUntil(2, MED);
	stop();


	if (closePort() == -1)
	{
		printf("Port failed to close \n");
		return -1;
	}
	return 0;
}
