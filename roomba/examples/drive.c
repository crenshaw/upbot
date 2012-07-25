/**
 * drive.c
 *
 * Simple example program for iRobot Create Roomba.  
 *
 * @author Steven M. Beyer
 * @since  25 February 2010
 * 
 *
 */
#include <stdio.h>
#include "../roomba.h"

/** 
 * main()
 *
 * Drives the iRobot in a fixed N-shaped pattern.
 */
int main(int argc, char * argv[])
{

  printf("%s running ... \n", argv[0]);

  if (openPort() == 0)
    {
      printf("Port failed to open \n");
      return -1;
    }
  
  initialize();

  sleep(1);

  driveStraightUntil(2, MED);

  turnCounterClockwise(DEGREES_45);
  driveBackwardsUntil(2, MED);

  turnClockwise(DEGREES_90);
  driveStraightUntil(2, MED);

  stop();
  
  
  if (closePort() == -1)
    {
      printf("Port failed to close \n");
      return -1;
    }
  
  return 0;

}
