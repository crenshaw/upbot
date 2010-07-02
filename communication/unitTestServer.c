#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include "../supervisor/unitTest.h"
#include "communication.h"

/**
*
* unitTestServer
*
* This is an implementation of a server that communicates with the Supervisor
* client. It receives commands from the Supervisor and passes them to the
* unit test function which keeps a 'picture' of the Roomba environment. This 
* allows it to emulate a Roomba in a perfect environment and creates appropriate
* sensor data based on the commands it receives.
*
* Author: Zachary Paul Faltersack (adapted from simpleServer written by Tanya Crensaw)
* Last Edit: June 2, 2010
*
*/

int main(void)
{

	struct sockaddr_storage theirAddr; // connector's address information
	socklen_t size;
	struct sigaction sa;
	char p[INET6_ADDRSTRLEN];

	int s;
	int newSock;

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

		// Make a child process to handle the request.  The server will continue
		// to listen on s.
		if(!fork()) 
		{
			close(s);   // child process doesn't need the listener.
			char* str;
			int* cmd = (int*) malloc(sizeof(int));
			*cmd = 1;

			while(1)
			{
				// send command to unitTester and receive resulting sensor data
				str = unitTest2(*cmd, 0);

				if(g_statsMode == 0)
				{
					printf("Sending: [%s] to Supervisor\n", str);
				}

				// Send the sensor data to Supervisor client
				if(send(newSock, str, strlen(str), 0) == -1)
				{
					perror("send");
					break;
				}

				if(g_statsMode == 0)
				{
					printf("---------------------------------------\n");

					// Receive command from Supervisor client
					printf("Waiting for command\n->");
				}

				if(recv(newSock, cmd, 1, 0) == -1)
				{
					perror("receive");
					// send cleanup command to unit test
					unitTest2(*cmd, 1);
					break;
				}
			}

			// close the socket
			close(newSock);
			exit(0);

		}

		close(newSock);  // parent process doesn't need the new socket created to handle request.

	}

	return 0;  

}
