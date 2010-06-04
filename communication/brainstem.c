#include "tell.h"
#include "communication.h"
#include "../roomba/roomba.h"

#define NLOOPS 1000
#define SIZE  40
//#define DEBUG 1

static int update(char *ptr)
{
  return ((*ptr)++ ); /* return value before increment */
}


int main(void)
{

  int i, counter, serverID, clientSock;
  pid_t pid;

  // A pointer to shared memory for conveying commands across
  // processes.
  caddr_t cmdArea;

  // A pointer to shared memory for conveying sensor data across
  // processes.
  caddr_t sensArea;

  int smv = '\0';

  // An array to hold most recent command
  char cmdBuf[MAXDATASIZE] = {'\0'};

  // An array to hold sensor data
  char sensData[40] = {'\0'};

  // Perform file cleanup before progressing.
  system("rm cmdFile.txt");
  system("touch cmdFile.txt");

  // Open text files to store commands and sensor information
  FILE* cmdFile = fopen("cmdFile.txt", "w");
  FILE* sensorFile = fopen("sensorFile.txt", "r");


  // Check if file open is successful
  if (cmdFile == NULL || sensorFile == NULL)
    {
      perror("Server fopen");
      return -1;
    }

  // Create pipe to communicate between parent and child
  int fd[2];
  if(pipe(fd) < 0)
    perror("pipe error");


  // Create a small piece of shared memory for the child
  // (brain) to communicate commands received from the client
  // to the parent (nervous system).
  if(createSharedMem("/dev/zero", &cmdArea) == -1)
    {
      perror("createSharedMem()");
      return -1;
    }

  // Create a small piece of shared memory for the parent (nervous
  // system) to communicate sensor information to the child (brain).
  if(createSharedMem("/dev/zero", &sensArea) == -1)
    {
      perror("createSharedMem()");
      return -1;
    }
#ifdef DEBUG  
  printf("Successfully created shared memory at location 0x%x and 0x%x. \n", 
	 cmdArea, 
	 sensArea);
 
  smv = update((char *)cmdArea);
  smv = update((char *)cmdArea);  
  printf("Shared memory has been initialized with %d.\n", smv);
#endif

  // Create a socket-based server
  if((serverID = createServer()) == -1)
    {
      perror("createServer()");
      return -1;
    }

  printf("brainstem: waiting for connections...\n");

  // Establish a client-connection for the socket-based server
  if((clientSock = establishConnection(serverID)) == -1)
    {
      perror("establishConnection(serverID)");
      return -1;
    }
  

  // Set up signal-based communication between the child 
  // (brain) and parent (nervous system) to avoid possible
  // race conditions.
  TELL_WAIT();

  // Fork a child process.  This process will handle the "brain"
  // activities, such as communicating sensor data to the client
  // and receiving control commands from the client.
  if(( pid = fork()) < 0)
    {
      perror("fork error");
    }

  else if (pid > 0) // Code for parent (nerves)
    {
#ifdef DEBUG
      printf("Parent \n");
#endif
      close(clientSock);
      nerves(cmdArea, sensArea);
    }

  else // Code for child (brain)
    {
#ifdef DEBUG
      printf("Child \n");
#endif
      close(serverID);   // child process doesn't need the listener.
      if(send(clientSock, MSG, sizeof(MSG), 0) == -1)
	perror("send");
      //loops until user enter 'q' (quit command)
      if(receiveDataAndStore(clientSock, cmdBuf, sensData, cmdFile, sensorFile, fd, sensArea) == -1)
	{
	  perror("receiveDataAndStore");
	  exit(-1);
	}
      close(clientSock);
      exit(0);
    }
  
  exit(0);
}
