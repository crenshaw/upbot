/**
 * serverUtility.c
 *
 * A collection of supporting functions for the iRobot server programs.
 *
 * @author Tanya L. Crenshaw & Steven Beyer.
 * 
 */

#include "communication.h"

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include <fcntl.h>
#include <mqueue.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>

#define SIZE 40

static int true = 1;
static int false = 0;
char lastDataSent[MAXDATASIZE] = {'\0'};


/** 
 * get_in_addr()
 *
 * Get sockaddr, IPv4 or IPv6:
 */
void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


/**
 * createListener()
 *  
 * Create a server endpoint of communication.
 * Adapted from: "Advance Programming in the UNIX Environment."  page 501
 * as well as "Beej's Guide to Network Programming."
 */
int createListener(const char * name)
{
  int status;
  int s;
  int yes = 1;
 
  // Defining the fields for socket structs is challenging.  The fields depend of the
  // address, type of socket, and communication protocol.  This function uses getaddrinfo()
  // to aid in defining the struct socket fields.  This function fills a struct of
  // type addrinfo.
  struct addrinfo hints;
  struct addrinfo * servinfo, *p; 
  
  // Initialize the hints structure based on what little we care about
  // in terms of the socket.  The goal is to listen in on host's IP
  // address on port 22.
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;        // don't care if its IPv4 or IPv6.
  hints.ai_socktype = SOCK_STREAM;    // stream-style sockets.
  hints.ai_flags = AI_PASSIVE;        // fill in my IP automatically.

  if((status = getaddrinfo(NULL, name, &hints, &servinfo)) != 0)
    {
      return -1;
    }

  // Servinfo now points to a linked list of 1 or more struct
  // addrinfos.  Note that they may not all be valid.  Scan through
  // the servinfo until something makes sense.
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((s = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("server: socket");
      continue;
    }
    
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("setsockopt");
      exit(1);
    }
    
    if (bind(s, p->ai_addr, p->ai_addrlen) == -1) {
      close(s);
      perror("server: bind");
      continue;
      }
    
    break;

  }

  if (p == NULL)  {
    fprintf(stderr, "server: failed to bind\n");
    return -1;
  }

  // Don't need servinfo anymore
  freeaddrinfo(servinfo);

  return s;
}


/** 
 * sigchld_handler()
 *
 * This function is an interrupt-driven function which 
 * reaps dead children that are forked by the server.
 */
void sigchld_handler(int s)
{
  while(waitpid(-1, NULL, WNOHANG) > 0);
}


/**
 * writeCommandToFile()
 * 
 * Takes a command and writes it to a file.
 */
void writeCommandToFile(char* cmd, FILE* fp)
{
  if(fp != NULL && cmd[0] != '\0' && cmd[0] != ssQuit)
    {
      fprintf(fp, "%c", cmd[0]);
      fprintf(fp, "%s", " ");
      fflush(fp);
    }

  return;
}

/**
 * writeCommandToSharedMemory()
 * 
 * Takes a command and writes it to shared memory.
 * If the command is null or '(' it is not written.
 * This function only works for a single character command.
 *
 * @arg cmd pointer to command
 * @arg shm pointer to shared memory
 *
 * @return int 1 if wrote something and 0 otherwise
 */
int writeCommandToSharedMemory(char* cmd, mqd_t qd)
//int writeCommandToSharedMemory(char* cmd, caddr_t shm, mqd_t qd)
{

    //I WAS HERE
  //char * timestamp = gettime();

  char * msg;


  //TODO: find how to support format of shared memory commands (includes timestamp)
  
  if((cmd[0] != '\0' || cmd[0] == CQ_COMMAND_CANARY_VALUE))
    {
      
      if(mq_send(qd, cmd, sizeof(cmd), 0) != 0)
      {
         perror("failed to write cmd to message queue");
         exit(-1);
       }

       printf("Wrote %c to message queue.\n",*cmd);

      return 1;
    }

  return 0;
}

/**
 * readSensorDataFromFile()
 *
 * Reads a line from the file containing sensor data.
 * This file is shared between the server and the roomba app
 *
 * @arg data pointer to read the file to
 * @arg fp file pointer
 *
 * @return int 1 if we read something and 0 otherwise
 */
int readSensorDataFromFile(char* data, FILE* fp)
{
  data[0] = '\0';
  int count = 0;
  /* if(freopen("sensorFile.txt", "r+a", fp) != NULL)
    {
      perror("readSensorDataFromFile (freopen)");
      return 0;
      }*/
  if(fp != NULL)
    {
      //confirm that you are not at the EOF
      if(fgets(data, 40, fp) != NULL)
	{
	  count += strlen(data);

	  if(fseek(fp, count, SEEK_SET) != 0)
	    {
	      perror("readSensorDataFromFile (fseek)");
	    }
	  return 1;
	}
      perror("readSensorDataFromFile (fgets)");
      return 0;
    }
  perror("readSensorDataFromFile (bad fp)");
  return 0;
}

/**
 * readSensorDataFromSharedMemory()
 *
 * copy sensor data from shared memory into memory pointed
 * to by data
 *
 * @arg data pointer to destination
 * @arg shm pointer to source
 *
 * @return int 
 */
int readSensorDataFromSharedMemory(char* data, caddr_t shm)
{
  int shmLength = strlen((char*)shm);

  if (strncmp(lastDataSent, (char *)(shm), shmLength) != 0)
    {
      strncpy(data, (char *)(shm), shmLength);
      strncpy(lastDataSent, (char *)shm, shmLength);

      return 1;
    }

  return 0;
  
}
  

/** 
 * checkValue() 
 * 
 *      checks the value of the character being passed
 *      to ensure that it is a valid command and returns
 *      1, else if not valid command return 0
 */
int checkValue(char v)
{

  if(v == ssAdjustRight || v == CMD_ADJUST_RIGHT)
    return true;

  if(v == ssAdjustLeft || v == CMD_ADJUST_LEFT)
    return true;

  if(v == ssDriveLow)
    return true;

  if(v == ssDriveMed)
    return true;

  if(v == ssDriveHigh)
    return true;

  if(v == ssDriveBackwardLow)
    return true;

  if(v == ssDriveBackwardMed)
    return true;

  if(v == ssDriveBackwardHigh)
    return true;

  if(v == ssTurnCwise || v == CMD_RIGHT)
    return true;

  if(v == ssTurnCCwise || v == CMD_LEFT)
    return true;

  if(v == ssStop || v == ssNoOp || v == CMD_NO_OP)
    return true;

  if(v == ssQuit)
    return true;

  if(v == ssDriveDistance || v == CMD_FORWARD)
    return true;

  if(v == ssBlinkLED || v == CMD_BLINK)
    return true;

  if(v == ssSong || v == CMD_SONG)
    return true;

  return false;
}

/**
 * receiveDataAndStore()
 *
 * Receive a control command and convey sensor data.
 */
int receiveDataAndStore(int newSock, char* cmdBuf, char* sensData, FILE* cmdFile, 
			FILE* sensorFile, int* fd, caddr_t sensArea, 
			caddr_t cmdArea)
{
  int numbytes;

  // While the quit command is not sent receive commands from client
  while(cmdBuf[0] != ssQuit)
    {
      if ((numbytes = recv(newSock, cmdBuf, MAXDATASIZE-1, 0)) == -1)
	{
	  perror("recv");
	  return -1;
	}
      printf("receiveDataAndStore: %c\n", cmdBuf[0]);

      // Write command to the cmdFile.txt
      writeCommandToFile(cmdBuf, cmdFile);
      
      /*
       Currently unused code so modified.
       if future self or other needs to retrofit the new function is
        writeCommandToMessageQueue

      writeCommandToSharedMemory(cmdBuf, cmdArea);
      */

      // Send command to parent process
      write(fd[1], cmdBuf, 1);
      
      if(readSensorDataFromSharedMemory(sensData, sensArea))
      {
	printf("sensData: %s \n", sensData);
	if(send(newSock, sensData, strlen(sensData), 0) == -1)
	  perror("send");
      }
    }
  return 0;
}

/**
 * createSharedMem()
 * 
 * Create a small piece of shared memory labeled with the 
 * label 'area'.
 *
 * @arg deviceName the deviceName to use for the memory location.
 * @arg area the label of the shared memory location.
 */
int createSharedMem(char * deviceName, caddr_t* area)
{
  int fd;

  // Map the I/O device to a piece of shared memory 
  if( (fd = open(deviceName, O_RDWR)) < 0)
    {
      perror("open error");
      return -1;
    }

  if ( (*area = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == (caddr_t) -1)
    {
      perror("mmap error");
      return -1;
    }

  // Now that device has been mapped, it may be closed. 
  close(fd);
  return 0;
}

/**
 *
 * createServer()
 *
 * Create a server to listen on a socket
 *
 * @return s, the socket, if success and -1 if fail.
 *
 */
int createServer(void)
{
  struct sigaction sa;
  int s;

  // Create a socket to listen on port 22.  
  s = createListener(PORT);

  // Handle any problems raised by createListener().
  if(s == -2)
    {
      perror("bind");
      return -1;
    }

  else if(s == -1)
    {
      perror("socket");
      return -1;
    }

  // Listen to the socket, wait for incoming requests.  Allow 
  // a BACKLOG of requests to come in.
  if (listen(s, BACKLOG) == -1)
    {
      perror("listen");
      return -1;
    }


  // Set up the child reaper, a sig-action handler.
  sa.sa_handler = sigchld_handler; 
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  
  if (sigaction(SIGCHLD, &sa, NULL) == -1) 
    {
      perror("sigaction");
      return -1;
    }
  
  return s;
}

/**
 * establishConnection()
 * 
 * 'accept' a connection on socket s.  This function blocks until
 * a connection is established.
 *
 * @arg s the socket on which the connection should be established.
 *
 * @return the socket id.
 */
int establishConnection(int s)
{
  char p[INET6_ADDRSTRLEN];
  struct sockaddr_storage theirAddr; // connector's address information
  int newSock = -1;
  socklen_t size;

  while(newSock == -1)
    {
      size = sizeof(theirAddr);
      newSock = accept(s, (struct sockaddr *)&theirAddr, &size);
      if (newSock == -1)
	{
	  perror("accept");
	}
    }
  inet_ntop(theirAddr.ss_family, get_in_addr((struct sockaddr *)&theirAddr), p, sizeof(p));

  return newSock;
}

int checkArgName(int argc, char* argv[], char addresses[3][13])
{
  int i = 0;
  if(argc > 1)
    {
      for(i = 1; i < argc; i++)
	{
	  if(strncmp(argv[i], "frank", 5) == 0)
	    {
	      strncpy(addresses[i], "10.11.17.124", 13);
	    }
	  else if(strncmp(argv[i], "webby", 5) == 0)
	    {
	      strncpy(addresses[i], "10.11.17.15", 11);
	    }
	  else if(strncmp(argv[i], "mo", 2) == 0)
	    {
	      strncpy(addresses[i], "10.11.17.16", 11);
	    }
	  else
	    {
	      return -1;
	    }
	}
      return 1;
    }
  return 0;
}
