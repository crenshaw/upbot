/**
 * initLed.c
 *
 * Simple example program for iRobot Create Roomba.  
 *
 * @author Steven M. Beyer
 * @since  24 February 2010
 * 
 *
 */

#include <stdio.h>
#include "../roomba.h"
#include "../../communication/communication.h"

/** 
 * main()
 *
 * Initialize the iRobot and turn on all LEDs with the Play at Red at
 * full intensity.  So, from left-to-right on the iRobot, the LEDs
 * should be RED, GREEN, GREEN.
 */
int main(int argc, char * argv[])
{
  printf("%s running ... \n", argv[0]);

  if (openPort() == 0)
    {
      perror("Port failed to open \n");
      return -1;
    }
  
  initialize();
  
  setLED(RED, PLAY_ON, ADVANCE_ON);
  
  if (closePort() == -1)
    {
      perror("Port failed to close \n");
      return -1;
    }
  return 0;
}
