/*
 * This file contains low level commands to the roomba
 */

#include <strings.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>
#include <stdlib.h>
#include "led.c"

// This global file descriptor, once initialized,
// keeps track of the serial port opened to the iRobot
// Create.  It is used only by this file for easier port
// management.
int fd = -1;

/**
 * openPort()
 *
 * Opens ttyS2 port to write to iRobot.
 * 
 * @return -1 if port failed to open and 0 if port successfuly opens
 */
int openPort()
{
  fd = open("/dev/ttyS2", O_RDWR | O_NOCTTY);

  if (!fd)
    {
#ifdef DEBUG
      printf("File failed to open \n");
#endif
      return 0;
    }
  
#ifdef DEBUG  
  printf("File successfully opened with descriptor, fd = %d.\n", fd);
#endif

  return fd;
}

/**
 * closePort()
 * 
 * Closes serial port to the iRobot.
 *
 * @return 1 if successful, -1 if failed.
 */
int closePort()
{
  if(!close(fd))
    {
#ifdef DEBUG
      printf("file successfully closed \n");
#endif
      return 1;
    }

  else
    return -1;
}

/**
 * byteTx()
 * 
 * Transmits a byte to the iRobot over the serial interface.
 *
 * @arg value the 8-bit value to be transmitted.
 * 

 * @return void
 */
void byteTx(char value)
{
	char* buffer;
	buffer = &value;
	write(fd, buffer, 1);
    //usleep(50);
}

/**
 * byteRx()
 *
 * Reads a byte from the iRobot over the serial interface.
 *
 * @arg buffer a character pointer to the buffer which will
 *      store the received data.
 * @arg nbytes an int expressing the number of bytes to read.
 * @arg iter an int expressing the number of times to receive
 *      nbytes.
 *
 * @return void
 */
void byteRx(char* buffer, int nbytes, int iter)
{
  int data = 0;


  if(iter == 1)
    {
      data = read(fd, buffer, nbytes);
      usleep(10000);
      return;
    }
  else
    {
      printf("Error, can only read one byte.\n");
    }
 
  return;
}

/**
 * initialize()
 *
 * Sends the necessary byste sequence to the iRobot to start it
 * up, give the user control of it, and put it into safe mode.
 *
 * @return void
 */
void initialize()
{
  //Start the SCI
  byteTx(CmdStart);
  
  //Give user control and put in safe mode

  // Note: Using the CmdSafe command rather than 
  // the deprecated CmdControl command.
  byteTx(CmdFull);

  sleep(1);
}
