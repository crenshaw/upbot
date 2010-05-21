#include "communication.h"

// main()
//  

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

  printf("server: waiting for connections...\n");

  FILE* fp = fopen("cmdFile.txt", "w");
  FILE* sensorFile = fopen("sensorFile.txt", "r");
  if (fp == NULL)
    {
      perror("Server fopen");
      close(s);
      return -1;
    }

  printf("Enter q to quit. \n");
  while(1)
    {
      size = sizeof(theirAddr);
      newSock = accept(s, (struct sockaddr *)&theirAddr, &size);
      if (newSock == -1)
	{
	  perror("accept");
	  continue;
	}

      inet_ntop(theirAddr.ss_family, get_in_addr((struct sockaddr *)&theirAddr), p, sizeof(p));
      
      printf("server: got connection from %s.\n", p);
      fflush(fp);
      
      // Make a child process to handle the request.  The server will continue
      // to listen on s.
      if(!fork()) 
	{
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
	      writeCommandToFile(buf, fp);

	      readSensorDataFromFile(sensData, sensorFile);
	      printf("sensorData: %s\n", sensData);
	      if(send(newSock, sensData, sizeof(sensData), 0) == -1)
		perror("send");
	    }
	  close(newSock);
	  exit(0);
	     
	}

      close(newSock);  // parent process doesn't need the new socket created to handle request.
      //scanf("%c", &choice);
    }
  fclose(fp);
  return 0;  

}//main

