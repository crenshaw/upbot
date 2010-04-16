//aptLed
//Simple program for iRobot Create Roomba
//Created By:	Steven M. Beyer
//Date:		24 February 2010
//Description:	Turns on all LEDs with the Play at Red at full intensity.
//Depends on:	led.c utility.c

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

	setLED(RED, PLAY_ON, ADVANCE_ON);

	if (closePort() == -1)
	{
		printf("Port failed to close \n");
		return -1;
	}
	return 0;
}
