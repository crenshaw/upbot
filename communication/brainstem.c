/**
 * brainstem.c
 *
 * The main server program for the upbot system.  This two-process
 * program runs on the iRobot + gumstix platform; it accepts commands
 * from a client, executes them and conveys any resulting sensor data
 * back to the client.
 *
 * @author Steven Beyer & Tanya L. Crenshaw
 * @since May 2010
 *
 */

#include "tell.h"
#include "communication.h"
#include "../roomba/roomba.h"

#define SIZE  40
#define SIZE_OF_EMPTY_DATA 11
//#define DEBUG 1


/** 
 * main()
 * 
 * This program comprises two processes.  First, the "brain" which
 * communicates with a supervisor-client.  The brain receives
 * high-level control commands from the supervisor-client and then
 * conveys any resulting sensor data back.  The second process is the
 * "nerves"; this process translates high-level commands into
 * low-level iRobot SCI commands, executes them on the iRobot, and
 * then obtains any resulting sensor data.
 * 
 */
int main(void)
{

  int i, counter, serverID, clientSock, numBytes;
  pid_t pid, pid2;

  // A pointer to shared memory for conveying commands across
  // processes and conveying sensor data across processes.
  caddr_t cmdArea;
  caddr_t sensArea;

  // Arrays to hold most recent command sent by the
  // supervisor-client and the most recent command sent to the iRobot.
  char commandFromSupervisor[MAXDATASIZE] = {'\0'};
  char commandToRobot[MAXDATASIZE] = {'\0'};

  // An array to hold sensor data sent to the supervisor-client and
  // obtained from the iRobot.
  char sensDataToSupervisor[SIZE] = {'\0'};
  char sensDataFromRobot[SIZE] = {'\0'};
  char emptyDataToSupervisor[SIZE] = "0000000000 ";

  // An array to hold the timestamp.
  char currTime[100];

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

      // Close the client socket since this parent won't be using it.
      close(clientSock);

      //TELL_CHILD(pid);

      // Open the serial port to the robot.  If this is unsuccessful,
      // there is no point in continuing.
      if(openPort() == 0)
	{
	  printf("Port failed to open \n");
	  exit(-1);
	}

      // Initialize the robot, prepare it to receive commands.  Wait
      // for a second to give this some time to take effect.
      initialize();
      sleep(1);

      // Turn on the LED to indicate the robot is ready and listening.
      // It's a nice sanity check.
      setLED(RED, PLAY_ON, ADVANCE_ON);

      while(commandToRobot[0] != ssQuit)
	{
	  fprintf(stdout, "%s %d\n", __FILE__, __LINE__);
	  // Wait until a valid command is received.
	  while((commandToRobot[0] = readFromSharedMemoryAndExecute(cmdArea) == -1));
	  fprintf(stdout, "finished readFromShared");
	  receiveGroupOneSensorData(sensDataFromRobot);

	  // check if any of the sensor data indicates a 
	  // sensor has been activated.  If so, react be
	  // driving backwards briefly and then stopping.
	  
	  int sensData = 0;
	  fprintf(stdout, "about to sensData");
	  // Check sensor data first to stop ramming into wall.
	  sensData = checkSensorData(sensDataFromRobot);
	  fprintf(stdout, "about to wait on child\n");
	  // Wait until child has sent previous sensor data.
	  WAIT_CHILD();
	  fprintf(stdout, "Done waiting on child\n");
	  if(sensData)
	    {
	     	      
	      // Drive backwards and then stop.
	      driveBackwardsUntil(EIGHTH_SECOND, MED);
	      stop();	      

	      // Convey sensorData back to the child.
	      writeSensorDataToSharedMemory(sensDataFromRobot, sensArea, getTime());

	    }
	  fprintf(stdout, "About to tell child\n");
	  // Done writing sensor data, tell child to proceed reading sensor data.
	  TELL_CHILD(pid);
	  
	  // Reset the array; fill it again in the next loop.
	  for(i = 0; i <= 6; i++)
	    {
	      sensDataFromRobot[i]= FALSE;
	    }
	}

      // Close the serial port.
      printf("Closing the serial port \n");

      if (closePort() == -1)
	{
	  perror("Port failed to close \n");
	  exit(-1);
	}
      
      exit(0);
    }

  //------------------------------------------------------------------------
  // Code for child (brain)
  //------------------------------------------------------------------------
  else
    {
      // Child process doesn't need the listener.
      close(serverID);

      // Initially tell the parent to proceed and write sensor data.
      TELL_PARENT(getppid());

      // Send the client the initial connection message.
      if(send(clientSock, MSG, sizeof(MSG), 0) == -1)
	perror("send");

      fprintf(stdout, "brainstem: within child process \n");

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

      // As long as the quit command 'q' has not been sent by the
      // supervisor-client, receive a control command and send the
      // subsequent sensor data.
      while(commandFromSupervisor[0] != ssQuit)
	{
	  // Wait to receive command from supervisor-client; read the command
	  // into cmdBuf.
	  if ((numBytes = recv(clientSock, commandFromSupervisor, MAXDATASIZE-1, 0)) == -1)
	    {
	      perror("recv");
	      return -1;
	    }
	  fprintf(stdout, "%s  commandFromSupervisor: %d\n", __FILE__, commandFromSupervisor[0]); 

	  // Write the read command into shared memory so that the
	  // parent (nerves) may read and execute it.
	  writeCommandToSharedMemory(commandFromSupervisor, cmdArea);	      
	  printCommandQueueHeader(cmdArea);
	  fprintf(stdout, "%s %d\n", __FILE__, __LINE__);
	  // Wait until parent has written sensor data.
	  WAIT_PARENT();
	  fprintf(stdout, "Done Waiting on Parent\n");

	  // If there is sensor data available, send it to the
	  // supervisor-client.
	  if(readSensorDataFromSharedMemory(sensDataToSupervisor, sensArea))
	    {
	      printf("sensDataToSupervisor: %s \n", sensDataToSupervisor);
	      if(send(clientSock, sensDataToSupervisor, strlen(sensDataToSupervisor), 0) == -1)
		perror("send");
	    }
	  
	  // Otherwise, assume no sensor was activated.  Send an empty
	  // sensor message to the supervisor-client.
	  else
	    {
	      printf("%s %d\n", __FILE__, __LINE__);
	      printf("sensDataToSupervisor: nothing happened. \n"); 
	    
	      // Construct an empty sensor data message and send it to
	      // the supervisor-client.
	      strncat(emptyDataToSupervisor, getTime(), MAXDATASIZE-11);

	      if(send(clientSock, emptyDataToSupervisor, 40, 0) == -1)
		perror("send");

	      emptyDataToSupervisor[SIZE_OF_EMPTY_DATA] = '\0';
	    }

	  // Done reading sensor data, allow parent to write more sensor data.
	  TELL_PARENT(getppid());
	}
	

      // All done.  Clean up the toys and go home.
      printf("Closing socket and terminating processes.\nHave a nice day!\n");
      kill(pid, SIGTERM);
      close(clientSock);

      exit(0);
    }

}
