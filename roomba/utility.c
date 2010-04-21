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
// Create.f  Its use is internal to this file for
// easier port management.
int fd = -1;


/*openPort
*
*	Opens ttyS2 port to write to Roomba
*	Returns -1 if port failed to open 
*	and 0 if port successfuly opens
*/
int openPort()
{

  // Close the port that was previously open and
  // likely not closed.
  //close(3);

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

/*
 * closePort()
 *  Description: Closes serial port using handler fd as
 *               passed to the function.
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

/*byteTx
*	Parameters: 
*		char value: value to write to Roomba
*	Descirption:
*		writes value out to Roomba
*/
void byteTx(char value)
{
	char* buffer;
	buffer = &value;
	write(fd, buffer, 1);
}

/*byteRx
*	Parameters:
*		char* buffer: pointer to buffer to receive
*		int nbytes: number of bytes to receive
*		int iter: number of times to receive nbytes
*	Description:
*		reads values from Roomba
*/
void byteRx(char* buffer, int nbytes, int iter)
{
  int i;

  for (i = 0; i < iter; i++)
  {
    read(fd, buffer, nbytes);
  }

}

/*initialize
*
*	Sends bits to Roomba to start the Roomba,
*	give the user control of the Roomba,
*	and put the SCI into safe mode
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
    turnClockwise(90);
    break;
  case ssTurnCCwise:
    turnCounterClockwise(90);
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

int writeSensorDataToFile(int bumper, FILE* fp, char* currTime)
{
  int SensorData = 0x00;

  if(bumper == SENSOR_BUMP_LEFT)
    {
      sendBinaryValue(fp, SensorData | SENSOR_BUMP_LEFT);
    }
  else if(bumper == SENSOR_BUMP_RIGHT)
    {
      sendBinaryValue(fp, SensorData | SENSOR_BUMP_RIGHT);
    }
  else if(bumper == SENSOR_BUMP_BOTH)
    {
      sendBinaryValue(fp, SensorData | SENSOR_BUMP_RIGHT | SENSOR_BUMP_LEFT);
    }
  else
    return -1;

  printf("%x \n", bumper);
  fprintf(fp, " %s \n", currTime);
  fflush(fp);
  return 0;
}


char* getTime()
{
  time_t rawtime;
  struct tm * timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  return asctime(timeinfo);
}

/* Print n as a binary number */
void sendBinaryValue(FILE* fp, int n) 
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
