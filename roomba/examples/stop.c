/**
 * stop.c
 *
 * Simple example program for iRobot Create Roomba.  
 *
 * @author Tanya L. Crenshaw
 * @since  14 April 2010
 * 
 *
 */

#include <stdio.h>
#include "../roomba.h"
#include "../../communication/communication.h"

/** 
 * main()
 *
 * This program just stops the iRobot. Sometimes a program has crashed
 * and the iRobot is off and running, mashing itself into the wall.
 * Stop it remotely instead of running across the room to pick it up.
 */

int main(int argc, char * argv[])
{
  int c = -1;
  char x[10];

  printf("%s running ... \n", argv[0]);

  if (openPort() == 0)
    {
      printf("Port failed to open \n");
      return -1;
    }
  
  initialize();

  stop();

  return 0;
}
