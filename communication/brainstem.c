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

  int i, counter, serverID, clientSock, numBytes;
  pid_t pid, pid2;

  // A pointer to shared memory for conveying commands across
  // processes.
  caddr_t cmdArea;

  // A pointer to shared memory for conveying sensor data across
  // processes.
  caddr_t sensArea;

  int smv = '\0';

  // An array to hold most recent command sent by the
  // supervisor-client
  char commandFromSupervisor[MAXDATASIZE] = {'\0'};

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

  // Initialize shared memory for commands as a command queue
  // data structure, maximum number of commands, 10.
  createCommandQueue(cmdArea, 10);


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
  // activities: communicating sensor data to the client
  // and receiving control commands from the client.
  if(( pid = fork()) < 0)
    {
      perror("fork error");
    }

  //------------------------------------------------------------------------
  // Code for parent (nerves)
  //------------------------------------------------------------------------
  else if (pid > 0)
    {
      close(clientSock);
      TELL_CHILD(pid);

      
      

      nerves(cmdArea, sensArea, pid);
    }

  //------------------------------------------------------------------------
  // Code for child (brain)
  //------------------------------------------------------------------------
  else
    {
      // Child process doesn't need the listener.
      close(serverID);

      // Send the client the initial connection message.
      if(send(clientSock, MSG, sizeof(MSG), 0) == -1)
	perror("send");

      printf("brainstem: within child process \n");

      // At the request of the supervisor implementation team, The
      // brain follows a strict alternation of 'receive control
      // command' then 'send sensor data'.  If the control command
      // sent by the supervisor-client is 'turn left' and the iRobot
      // happens to bump into something, then the subsequent message
      // to the supervisor-client will indicate which bumper was activated.  
      // Similarly, if the control command sent by the supervisor is
      // 'turn left' and the iRobot experiences no sensory input, the
      // subsequent message to the supervisor will indicate that no sensors
      // were activated.

      // As long as the quit command 'q' has not been sent by the supervisor-client,
      // receive a control command and send the subsequent sensor
      // data.
      while(commandFromSupervisor[0] != ssQuit)
	{
	
	  // Wait to receive command from supervisor-client; read the command
	  // into cmdBuf.
	  if ((numBytes = recv(clientSock, commandFromSupervisor, MAXDATASIZE-1, 0)) == -1)
	    {
	      perror("recv");
	      return -1;
	    }

	  // Write the read command into shared memory so that the
	  // parent (nerves) may read and execute it.
	  writeCommandToSharedMemory(commandFromSupervisor, cmdArea);	      

	  // If there is sensor data available, send it to the
	  // supervisor-client.
	  if(readSensorDataFromSharedMemory(sensData, sensArea))
	    {
	      printf("sensData: %s \n", sensData);
	      if(send(clientSock, sensData, strlen(sensData), 0) == -1)
		perror("send");
	    }
	}
	
      close(clientSock);
      exit(0);
    }

}
