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

  if(iter == 1)
    {
      read(fd, buffer, nbytes);
      return;
    }
  for (i = 0; i < iter; i++)
  {
    read(fd, buffer, nbytes);
    buffer++;
    usleep(1);
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
  byteTx(CmdFull);

  sleep(1);
}

/**
 * readAndExecute()
 * 
 * 
 */
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

  // SEEK_SET indicates that the function should seek from the
  // beginning of file
  if(fseek(fp, count, SEEK_SET) != 0)
    {
      perror("readAndExecute (fseek)");
      return -1;
    }

  // Read a command from the file.
  if(fscanf(fp,"%c", &c) != 1)
    {
      return -2;
    }

  // Read a command from shared memory.
  

#ifdef DEBUG

  // Debugging shared memory
  printf("Shared memory is located at: 0x%x", shm);
  printf("The contents of shared memory are %c \n", *(char *)shm);
  fflush(stdout);

  // Debugging shared files
  printf("Location: %d\n", count);
#endif

  calcFileLoc(c);

  if (c != '\0')
#ifdef DEBUG
     printf("%c \n", c);
#endif
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
  case CMD_RIGHT:
    turnClockwise(DEGREES_90);
    break;
  case ssTurnCCwise:
  case CMD_LEFT:
    turnCounterClockwise(DEGREES_90);
    break;
  case ssDriveDistance:
    driveDistance();
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

int readFromSharedMemoryAndExecute(caddr_t shm)
{
  char cmd = '\0';

  cmd = getCommandCodeFromQueue(shm);

  switch (cmd){
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
  case CMD_RIGHT:
    turnClockwise(DEGREES_90);
    break;
  case ssTurnCCwise:
  case CMD_LEFT:
    turnCounterClockwise(DEGREES_90);
    break;
  case ssDriveDistance:
  case CMD_FORWARD:
    driveDistance();
    break;
  case ssAdjustRight:
  case CMD_ADJUST_RIGHT:
    adjustRIGHT();
    break;
  case ssAdjustLeft:
  case CMD_ADJUST_LEFT:
    adjustLEFT();
    break;
  case ssBlinkLED:
  case CMD_BLINK:
    blinkLED();
    break;
  case ssStop:
  case CMD_NO_OP:
  case ssNoOp:
    stop();
    break;
  case ssQuit:
    break;
  default:
    return -1;
  }

  return cmd;
}

    
/**
 * calcFileLoc()
 *
 */
void calcFileLoc(char c)
{
  if(c != '\0')
    {
      count+=2;
    }
}

/**
 * writeSensorDataToSharedMemory()
 * 
 */
void writeSensorDataToSharedMemory(char* sensorArray, caddr_t shm, char* currTime)
{

  int sensorData = 0x00;
  int i, j;
  char sensorDataString[40] = {'\0'};
  int bitMask = 0x1;

  sensorData |= sensorArray[0] & SENSOR_BUMPS_WHEELDROPS;

  for(i=2; i<=6; i++)
    {
      sensorData |= sensorArray[i]<<i+3;
    }

  //examine each sensor bit one at a time
  //and if it is a 1 write a '1' into shared mem
  //otherwise write a '0'
  for (j = 9; j >= 0; j--)
  {
    if ((sensorData & bitMask) == bitMask )
      {
	*(char *)(shm + j) = '1';
      }
    else
      {
	*(char *)(shm + j) = '0';
      }
    bitMask = bitMask << 1;
  }
  
  //add space to end of sensor data
  *(char *)(shm + 10) = ' ';

  //copy timestamp to end of sensor data
  strncpy((char*)(shm + 11), currTime, strlen(currTime));

  

  return;
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

  sensorData |= sensorArray[0] & SENSOR_BUMPS_WHEELDROPS;

  for(i=2; i<=6; i++)
    {
      sensorData |= sensorArray[i]<<i+3;
    }

  fprintBinaryAsString(fp, sensorData);
  
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

  i = 1<<(9);
  
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
  // Check cliff and virtual wall sensors; return true 
  // if any sensor has been activated  
  for(i = SP_G1_CLIFF_LEFT; i <= SP_G1_VIRTUAL_WALL; i++)
    {
      if(x[i] == ACTIVE_SENSOR)
	{
	  STOP_MACRO;
	  return TRUE;
	}
    }

  // Check bump sensors; return true if either bump sensor
  // has been activated 
  if(((x[0] & SENSOR_BUMP_RIGHT) == SENSOR_BUMP_RIGHT) || 
     ((x[0] & SENSOR_BUMP_LEFT ) == SENSOR_BUMP_LEFT) ||
     ((x[0] & SENSOR_WHEELDROP_RIGHT) == SENSOR_WHEELDROP_RIGHT) ||
     ((x[0] & SENSOR_WHEELDROP_LEFT) == SENSOR_WHEELDROP_LEFT) ||
     ((x[0] & SENSOR_WHEELDROP_BOTH) == SENSOR_WHEELDROP_BOTH) ||
     ((x[0] & SENSOR_WHEELDROP_CASTER) == SENSOR_WHEELDROP_CASTER))
    {
      STOP_MACRO;
      return TRUE;
    }

  // No sensor has been activated. 
  return FALSE;
}
