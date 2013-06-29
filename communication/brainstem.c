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
 * NOTE: When compiling with the arm-linux-gnueabi-gcc compiler, the
 * compiler issues this warning when compiling with the -static flag:
 * 
 * "brainstem.c:(.text+0x2dc): warning: Using 'getaddrinfo' in
 *  statically linked applications requires at runtime the shared
 *  libraries from the glibc version used for linking."
 *
 * These libraries must be available at runtime on the gumstix verdex
 * pro.
 */

#include "tell.h"
#include "communication.h"
#include "../roomba/roomba.h"


#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include <fcntl.h>
#include <mqueue.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>


#define SIZE_OF_EMPTY_DATA 11
//#define DEBUG 1


typedef struct msgbuf {
  long mtype;
  char mtext[3];
} msgbuf;



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
int main(int argc, char* argv[])
{

  /* Create a message queue using O_CREAT so that if the queue doesn't
   * already exist, it will be created.  When using mq_open with
   * O_CREAT, one must supply four arguments.  The first "name"
   * argument must begin with a slash.  The third "mode" argument is
   * derived from the symbolic constants is <sys/stat.h>.
   */
   mqd_t mqd_cmd = mq_open("/q_cmd", 
		    O_RDWR | O_CREAT , 
		    S_IRWXU | S_IRWXG | S_IRWXO, 
		    NULL);

   mqd_t mqd_sns = mq_open("/q_sns", 
		    O_RDWR | O_CREAT, 
		    S_IRWXU | S_IRWXG | S_IRWXO, 
		    NULL);
 
  printf("The message queue id is: %d\n", mqd_cmd);

  /* Determine the size of messages for this message queue
   */
  struct mq_attr a;
  mq_getattr(mqd_cmd,&a);  

  printf("The default message size is: %d\n", a.mq_msgsize);
   
   if( mqd_cmd == -1)
    {
      perror("mq_open():");
      return -1;
    }


   if( mqd_sns == -1)
    {
      perror("mq_open():");
      return -1;
    }

  

  int check = 0;
  char addresses[3][13];
  if ((check = checkArgName(argc, argv, addresses)) == -1)
    {
      printf("Not correct name entered. Exiting.\n");
      exit(0);
    }
  int i, counter, serverID, clientSock, numBytes;
  pid_t pid, pid2;

  // Arrays to hold most recent command sent by the
  // supervisor-client and the most recent command sent to the iRobot.
  char commandFromSupervisor[MAXDATASIZE] = {'\0'};
  char commandToRobot[MAXDATASIZE] = {'\0'};

  // An array to hold sensor data sent to the supervisor-client and
  // obtained from the iRobot.
  char sensDataToSupervisor[MAXDATASIZE] = {'\0'};
  char sensDataFromRobot[MAXDATASIZE] = {'\0'};
  char emptyDataToSupervisor[MAXDATASIZE] = "0000000000 ";
  char rawTimeString[12] = {'\0'};

  // An array to hold the timestamp.
  char currTime[100];


  // Create pipe to communicate between parent and child
  int fd[2];
  if(pipe(fd) < 0)
    perror("pipe error");

  //------------------------------------------------------------------------
  // Added Code to implement client
  //------------------------------------------------------------------------
  int sockfd, numbytes;  
  //buffer to store sensor information
  char sensorBuf[MAXDATASIZE];
  char msgBuf[MAXDATASIZE];
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];
  //array to hold the command sent
  char cmd[1];
  //initialize the input to NULL char
  char input = '\0';
  //------------------------------------------------------------------------
  // End of added section
  //------------------------------------------------------------------------ 

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


#ifdef TARGET_WEBBY
      // Initialize the compass device.
      initializeCompass();

      // This is temporary until turn() works.
      // Turn Left 10 degrees
      turn(TURN_LEFT, 10);
#endif 


      // Turn on the LED to indicate the robot is ready and listening.
      // It's a nice sanity check.
      setLED(RED, PLAY_ON, ADVANCE_ON);

      //------------------------------------------------------------------------
      // Added Code to implement client
      //------------------------------------------------------------------------
      if(check == 1)
	{
#ifdef DEBUG
	  printf("%s %d \n", __FILE__, __LINE__);
#endif
      
	  memset(&hints, 0, sizeof hints);
	  hints.ai_family = AF_UNSPEC;
	  hints.ai_socktype = SOCK_STREAM;
    
	  if ((rv = getaddrinfo(addresses[1], PORT, &hints, &servinfo)) != 0) 
	    {
	      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
	      return 1;
	    }
    
	  // loop through all the results and connect to the first we can
	  for(p = servinfo; p != NULL; p = p->ai_next) 
	    {
	      if ((sockfd = socket(p->ai_family, p->ai_socktype,
				   p->ai_protocol)) == -1) {
		perror("client: socket");
		continue;
	      }
      
	      if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
		close(sockfd);
		perror("client: connect");
		continue;
	      }
      
	      break;
	    }
    
	  if (p == NULL) 
	    {
	      fprintf(stderr, "client: failed to connect\n");
	      return 2;
	    }
    
	  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
		    s, sizeof s);
	  printf("client: connecting to %s\n", s);
    
	  freeaddrinfo(servinfo); // all done with this structure
    
	  //if the client does not recieve anything from server then exit
	  if ((numbytes = recv(sockfd, msgBuf, MAXDATASIZE-1, 0)) == -1) 
	    {
	      perror("recv");
	      exit(1);
	    }
	}

      //------------------------------------------------------------------------
      // End of added section
      //------------------------------------------------------------------------

      while(commandToRobot[0] != ssQuit)
	{
	  commandToRobot[0] = 'z';
	  // Wait until a valid command is received.
	  while(commandToRobot[0] == 'z')
	    {
          commandToRobot[0] = readFromMessageQueueAndExecute(mqd_cmd);
	      
	    }

	  printf("commandToRobot: %d\n", commandToRobot[0]);
	  //------------------------------------------------------------------------
	  // Added Code to implement client
	  //------------------------------------------------------------------------
	  if(check == 1)
	    {
	      if(send(sockfd, &commandToRobot[0], 1, 0) == -1)
		perror("send");
	      printf("      the command code sent was: %d\n", commandToRobot[0]);
	    }
	  //------------------------------------------------------------------------
	  // End of added section
	  //------------------------------------------------------------------------

	  receiveGroupOneSensorData(sensDataFromRobot);

	  // check if any of the sensor data indicates a 
	  // sensor has been activated.  If so, react be
	  // driving backwards briefly and then stopping.
	  
	  int sensData = 0;

	  // Check sensor data first to stop ramming into wall.
	  sensData = checkSensorData(sensDataFromRobot);

#ifdef DEBUG
	  printf("%s %d \n", __FILE__, __LINE__);
#endif
	  // Wait until child has sent previous sensor data.
	  WAIT_CHILD();

#ifdef DEBUG

	  printf("%s %d \n", __FILE__, __LINE__);
	  printf("%d \n", sensData);
#endif

	  if(sensData)
	    {
#ifdef DEBUG
	      printf("%s %d \n", __FILE__, __LINE__);	     	      
#endif
	      // Drive backwards and then stop.
	      driveBackwardsUntil(EIGHTH_SECOND, MED);
	      STOP_MACRO;	      

	      // Convey sensorData back to the child.
	      writeSensorDataToMessageQueue(sensDataFromRobot, mqd_sns, getTime(), getRawTime());

	    }  
	  // Done writing sensor data, tell child to proceed reading sensor data.
	  TELL_CHILD(pid);
	  
	  // Reset the array; fill it again in the next loop.
	  for(i = 0; i <= 6; i++)
	    {
	      sensDataFromRobot[i]= FALSE;
	    }
	}


      if (closePort() == -1)
	{
	  perror("Port failed to close \n");
	  exit(-1);
	}
      send(sockfd, &input, 1, 0);
      close(sockfd);
      kill(0, SIGTERM);
      mq_close(mqd_cmd);

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
	  // Write the read command into shared memory so that the
	  // parent (nerves) may read and execute it.
	  //writeCommandToSharedMemory(commandFromSupervisor, cmdArea,mqd_cmd);
      writeCommandToMessageQueue(commandFromSupervisor,mqd_cmd);

	  // Wait until parent has written sensor data.
	  WAIT_PARENT();

#ifdef DEBUG
	  printf("%s %d \n", __FILE__, __LINE__);
#endif

	  // If there is sensor data available, send it to the
	  // supervisor-client.
	  if(readSensorDataFromMessageQueue(sensDataToSupervisor, mqd_sns))
	    {
	      printf("\nsensDataToSupervisor: %s \n", sensDataToSupervisor);
	      if(send(clientSock, sensDataToSupervisor, strlen(sensDataToSupervisor)-1 , 0) == -1)
		perror("send");
	    }
	  
	  // Otherwise, assume no sensor was activated.  Send an empty
	  // sensor message to the supervisor-client.
	  else
	    {
	      itoa(getRawTime(), rawTimeString);
	      
	      // Construct an empty sensor data message and send it to
	      // the supervisor-client.
	      strncat(emptyDataToSupervisor, rawTimeString, MAXDATASIZE-SIZE_OF_EMPTY_DATA);
	      strncat(emptyDataToSupervisor, " ", 1);
	      strncat(emptyDataToSupervisor, getTime(), MAXDATASIZE-SIZE_OF_EMPTY_DATA);
	      printf("\nemptySensDataToSupervisor: %s \n", emptyDataToSupervisor);
	      if(send(clientSock, emptyDataToSupervisor, MAXDATASIZE-1, 0) == -1)
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
      mq_close(mqd_cmd);

      exit(0);
    }
  // Added following code to implement communication between roomba's
 
  
	  
}
