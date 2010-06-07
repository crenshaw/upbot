/*
** client.c -- a stream socket client demo
*/
#include "communication.h"

int main(int argc, char *argv[])
{
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
    //open the sensorFile.txt to write sensor information to
    FILE* sensorFile = fopen("sensorFile.txt", "w");
    
    if (argc != 2) {
      fprintf(stderr,"usage: client hostname\n");
      exit(1);
    }
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
      return 1;
    }
    
    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
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
    
    if (p == NULL) {
      fprintf(stderr, "client: failed to connect\n");
      return 2;
    }
    
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
	      s, sizeof s);
    printf("client: connecting to %s\n", s);
    
    freeaddrinfo(servinfo); // all done with this structure
    
    //if the client does not recieve anything from server then exit
    if ((numbytes = recv(sockfd, msgBuf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }
    

    sensorBuf[MAXDATASIZE - 1] = '\0';
    
    printf("client: received '%s'\n",sensorBuf);
    
    // fork a child process to receive sensor data
    if(!fork())
      {
	// infinitely receive sensor data from server
	// and store the information into sensor file
	while(1)
	  {
	    //clear the sensor buffer
	    sensorBuf[0] = '\0';
	    printf("Receiving sensor data.\n");
	    numbytes = recv(sockfd, sensorBuf, MAXDATASIZE-1, 0);
	    printf("client: sensor data: '%s'\n", sensorBuf);	   
	    printf("numbytes: %d\n", numbytes);
	    fprintf(sensorFile, "%s", sensorBuf);
	  }
      }
    
    // while the quit command has not been issued
    // send the command value to the server
    while (input != ssQuit)
      {
	printf("Please enter a command code for the robot: \n\n");
	
	//block until input from user
	scanf("%c", &input);
	
	//store the value from the user into cmd array
	cmd[0] = input;
	
	//ensure that it user input is valid
	if(checkValue(cmd[0])== 1)
	  {
	    //send the value to the server
	    if(send(sockfd, cmd, 1, 0) == -1)
	      perror("send");
	    printf("   the command code sent was: %d\n", cmd[0]);
	  }//if
      }//while
    
    //send the quit command
    send(sockfd, &input, 1, 0);
    //close the child
    close(sockfd);
    //kill the child process
    kill(0, SIGTERM);
    return 0;

}//main

