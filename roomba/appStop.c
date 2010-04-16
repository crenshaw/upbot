//appStop
//  Simple program for iRobot Create Roomba
//  Created By:	 Tanya L. Crenshaw
//  Date:	 14 April 2010
//  Description: This program just stops the iRobot.
//               Sometimes a program has crashed and the
//               iRobot is off and running, mashing itself
//               into the wall.  Stop it remotely instead
//               of running across the room to pick it up.
//

#include <stdio.h>
#include "roomba.h"
#include "../communication/communication.h"

int main()
{
  int c = -1;
  char x[10];

  if (openPort() == 0)
    {
      printf("Port failed to open \n");
      return -1;
    }
  
  initialize();

  setLED(RED, PLAY_ON, ADVANCE_ON);

  stop();

  return 0;
}
