#include <strings.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>
#include "roomba.h"
#include "../communication/communication.h"

int count = 0;

//#define DEBUG 1

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
  fd = open("/dev/ttyS2", O_RDWR );

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
    printf("file successfully closed \n");
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
  int i;

  for (i = 0; i < iter; i++)
  {
    read(fd, buffer, nbytes);
  }

}

/**
 * initialize()
 *
 * Sends the necessary byte sequence to the iRobot to start it
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
  byteTx(CmdSafe);

  sleep(1);
}

char readAndExecute(FILE *fp)
{
  char c = '\0';

  // Reopen the commands file; it contains the list of
  // commands to be executed by the roomba.
  if(freopen("cmdFile.txt", "r+a", fp) == NULL)
    {
      perror("readAndExecute (freopen)");
      return -1;
    }


  // Seek to the appropriate location in the file, the
  // position immediately after the last command executed.
  if(fseek(fp, count, SEEK_SET) != 0)
    {
      perror("readAndExecute (fseek)");
      return -1;
    }

  // Read a command.
  if(fscanf(fp,"%c", &c) != 1)
    {
      return -2;
    }

  printf("Location: %d\n", count);
  calcFileLoc(c);

  if (c != '\0')
     printf("%c \n", c);

  switch (c){
  case ssDriveLow:
    driveStraight(LOW);
    break;
  case ssDriveMed:
    driveStraight(MED);
    break;
  case ssDriveHigh:
    driveStraight(HIGH);
    break;
  case ssDriveBackwardLow:
    driveBackwards(LOW);
    break;
  case ssDriveBackwardMed:
    driveBackwards(MED);
    break;
  case ssDriveBackwardHigh:
    driveBackwards(HIGH);
    break;
  case ssTurnCwise:
    turnClockwise(DEGREES_90);
    break;
  case ssTurnCCwise:
    turnCounterClockwise(DEGREES_90);
    break;
  case ssStop:
    stop();
    break;
  case ssQuit:
    break;
  default:
    return -1;
  }
  return c;
}
    
void calcFileLoc(char c)
{
  if(c != '\0')
    {
      count+=2;
    }
}

/**
 * writeSensorDataToFile()
 *
 * converts sensor data to a 8-bit number and then prints the 8-bit
 * number and the timestamp to a file.  The bits, listed from high
 * -order bit 7 to low-order bit 0 are:
 *
 * 7: Unused.
 * 6: Virtual Wall
 * 5: Cliff Right
 * 4: Cliff Front Right
 * 3: Cliff Front Left
 * 2: Cliff Left
 * 1: Bump Left
 * 0: Bump Right
 *
 * For each bit, '1' indicates the sensor has been activated.
 *
 * @arg sensorArray character pointer to group of sensor data
 * @arg fp file pointer
 * @arg currTime character pointer to the timestamp
 *
 * @return void
 */

void writeSensorDataToFile(char* sensorArray, FILE* fp, char* currTime)
{
  int sensorData = 0x00;
  int i;

  sensorData |= sensorArray[0] & SENSOR_BUMP_BOTH;

  for(i=2; i<=6; i++)
    {
      sensorData |= sensorArray[i]<<i;
    }

  fprintBinaryAsString(fp, sensorData);
  
  // printf("%x \n",sensorData);
  fprintf(fp, " %s", currTime);
  fflush(fp);
  return;
}

/**
 * getTime()
 * 
 * Get the current time from the operating system and
 * convert it into human-readable format.  For example,
 * 
 *  Fri May 21 12:44:12 2010
 * 
 * @return char pointer to a string containing the 
 *         human readable timestamp.
 */
char* getTime()
{
  time_t rawtime;
  struct tm * timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  return asctime(timeinfo);
}

/**
 * fprintBinaryAsString()
 *
 * Given a file pointer fp and an integer value n,
 * convert n to a string representing the value of
 * n in binary.  For example 7 would be printed to
 * fp as the string "00000111".
 *
 * @arg fp file pointer
 * @arg n value to be printed
 *
 * @return void
 */
void fprintBinaryAsString(FILE* fp, int n) 
{
  int i;

  i = 1<<(7);
  
  while (i > 0)
  {
    if (n & i)
      fprintf(fp, "1");
    else
      fprintf(fp, "0");
    i >>= 1;
  }
  fflush(fp);
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

  // Check bump sensors; return true if either bump sensor
  // has been activated 
  if(((x[0] & SENSOR_BUMP_RIGHT) == SENSOR_BUMP_RIGHT) || 
     ((x[0] & SENSOR_BUMP_LEFT ) == SENSOR_BUMP_LEFT))
    {
      return TRUE;
    }

  // Check cliff and virtual wall sensors; return true 
  // if any sensor has been activated  
  for(i = SP_G1_CLIFF_LEFT; i <= SP_G1_VIRTUAL_WALL; i++)
    {
      if(x[i] == ACTIVE_SENSOR)
	{
	  return TRUE;
	}
    }

  // No sensor has been activated. 
  return FALSE;
}
