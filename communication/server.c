#include "communication.h"

/** 
 * main()
 *
 * This server program executes on the iRobot Create.  It communicates 
 * with a client running on a remote machine.  This server conveys
 * iRobot Create sensor information to the client.  It receives control
 * commands from the client and writes them to a shared file.  Another
 * program obtains these commands from the shared file and conveys
 * them to the iRobot.
 *
 * This is a multi-process program.  The parent process is in charge
 *  
 */


int main(void)
{ 
  struct sockaddr_storage theirAddr; // connector's address information
  socklen_t size;
  struct sigaction sa;
  char p[INET6_ADDRSTRLEN];
  char buf[MAXDATASIZE] = {'\0'};
  char sensData[40] = {'\0'};
  char choice = '\0';

  int s, i;
  int newSock, numbytes;

  system("rm cmdFile.txt");
  system("touch cmdFile.txt");

  // Create a socket to listen on port 22.  
  s = createListener(PORT);

  // Handle any problems raised by createListener().
  if(s == -2)
    {
      perror("bind");
      exit(1);
    }

  else if(s == -1)
    {
      perror("socket");
    }

  // Listen to the socket, wait for incoming requests.  Allow 
  // a BACKLOG of requests to come in.
  if (listen(s, BACKLOG) == -1)
    {
      perror("listen");
      exit(1);
    }


  // Set up the child reaper, a sig-action handler.
  sa.sa_handler = sigchld_handler; 
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  
  if (sigaction(SIGCHLD, &sa, NULL) == -1) 
    {
      perror("sigaction");
      exit(1);
    }

  printf("Server: waiting for connections...\n");

  FILE* cmdFile = fopen("cmdFile.txt", "w");
  FILE* sensorFile = fopen("sensorFile.txt", "r");
  if (cmdFile == NULL || sensorFile == NULL)
    {
      perror("Server fopen");
      close(s);
      return -1;
    }

  printf("Enter q to quit. \n");

      size = sizeof(theirAddr);
      newSock = accept(s, (struct sockaddr *)&theirAddr, &size);
      if (newSock == -1)
	{
	  perror("accept");
	  //	  continue;
	}

      inet_ntop(theirAddr.ss_family, get_in_addr((struct sockaddr *)&theirAddr), p, sizeof(p));
      
      printf("server: got connection from %s.\n", p);
      fflush(cmdFile);

      int fd[2];
      if(pipe(fd) < 0)
	perror("pipe error");

      // Make a child process to handle the request.  The server will continue
      // to listen on s.
      if(!fork()) 
	{
	  close(fd[0]);
	  printf("Forked buf[0] = %c\n", buf[0]);
	  if(send(newSock, MSG, sizeof(MSG), 0) == -1)
	    perror("send");
	  close(s);   // child process doesn't need the listener.
	  while(buf[0] != ssQuit)
	    {
	      printf("Entered while loop \n");
	      
	      if ((numbytes = recv(newSock, buf, MAXDATASIZE-1, 0)) == -1) 
		{
		  perror("recv");
		  exit(1);
		}
	      printf("%c\n", buf[0]);
	      writeCommandToFile(buf, cmdFile);
	      write(fd[1], buf, 1);
	      if(readSensorDataFromFile(sensData, sensorFile))
		{
		  printf("sensorData: %s\n", sensData);
		  if(send(newSock, sensData, sizeof(sensData), 0) == -1)
		    perror("send");
	    }
	    }


	  close(newSock);
	  close(fd[1]);
	  exit(0);
	     
	}
      // parent process doesn't need the new socket created to handle request.
      fprintf(stdout, "sanity check\n");
      close(newSock);  
      close(fd[1]);
      char line;

      //      fprintf(stdout, "outer line: %c\n", line);
      while(line != ssQuit)
	{
	  read(fd[0], &line, 1);
	  fprintf(stdout, "inner line: %c\n", line);
	  fflush(stdout);
	  /*	  if(readSensorDataFromFile(sensData, sensorFile))
	    {
	      printf("sensorData: %s\n", sensData);
	      if(send(newSock, sensData, sizeof(sensData), 0) == -1)
		perror("send");
		}*/
	}
      printf("Exited final while loop!\n");

      fclose(cmdFile);
      fclose(sensorFile);
      close(fd[0]);
      close(s);
      exit(0);  

}//main

