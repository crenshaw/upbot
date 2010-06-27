/**
* supervisorClient.c
*
* This is the client that connects to the Roomba server.
* It creates a connection and receives char arrays containing the sensor
* data and sends the commands that the Supervisor decides
*
* Author: Dr. Crenshaw, Dr. Nuxoll, Zachary Faltersack, Steve Beyer
* Last edit: 21/6/10
*
* Usage: supervisorClient.out <ip_addr> -c <roomba/test> -m <stats/visual>
*/

#include "communication.h"
#include "../supervisor/supervisor.h"

#define TIMEOUT_SECS	5
#define MAX_TRIES		10

int g_goalsFound = 0;				// Number of times we found the goal
int g_goalsTimeStamp[NUM_GOALS_TO_FIND];
int g_tries;

/**
 * exitError
 *
 * Close the Supervisor upon error and print error code
 */
void exitError(int errCode)
{
	printf("Error. Ending...");
	endSupervisor();
	exit(errCode);
}

/**
 * parseArguments
 *
 * Pass the arguments from the commandline and set the appropriate
 * global variables to accomplish the desired results
 *
 * @arg argc The number of arguments
 * @arg *argv pointer to array of arguments
 */
void parseArguments(int argc, char *argv[])
{
	int i;
	for(i = 0; i < argc; i++)
	{
		if(strcmp(argv[i], "-c") == 0)
		{
			if(strcmp(argv[i+1], "roomba") == 0)
			{
				g_connectToRoomba = 1;
			}
			else
			{
				g_connectToRoomba = 0;
			}
		}
		else if(strcmp(argv[i], "-m") == 0)
		{
			if(strcmp(argv[i+1], "stats") == 0)
			{
				g_statsMode = 1;
			}
			else
			{
				g_statsMode = 0;
			}
		}
	}
}

/**
 * sendCommand
 *
 */
int sendCommand(int sockfd, int cmd)
{
	int retVal = send(sockfd, &cmd, 1, 0);
	// Else send command to Roomba server and exit if unsuccessful
	if(retVal != -1 && g_statsMode == 0)
	{
		// Print command sent to Roomba on stdout
		printf("The command value sent was: %s (%i)\n", interpretCommand(cmd), cmd);
	}
	return retVal;
}

/**
 * recvCommand
 *
 */
int recvCommand(int sockfd, char* buf)
{
	g_tries = 0;
	if(g_statsMode == 0)
	{
		// Receive sensor data from socket and store in 'buf'
		printf("Receiving sensor data.\n");
	}
	int numbytes;
	// Insert null terminating character at end of sensor string
	sprintf(&buf[numbytes], "\0");

	if(g_statsMode == 0)
	{
		printf("client: sensor data: '%s'\n", buf);	   
		printf("numbytes: %d\n", numbytes);
	}

	alarm(TIMEOUT_SECS);        /* Set the timeout */
	while ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) < 0)
	{
		if (errno == EINTR)     /* Alarm went off  */
		{
			if (g_tries < MAX_TRIES)      /* incremented by signal handler */
			{
				printf("timed out, %d more tries...\n", MAX_TRIES-g_tries);
				int cmd = CMD_NO_OP;
				if (sendCommand(sockfd, cmd))
				{
					perror("Error sending CMD_NO_OP on retry\n");
				}
				alarm(TIMEOUT_SECS);
			} 
			else
			{
				perror("Timed out on send command\n");
			}
		} 
		else
		{
			perror("Error on receive\n");
		}
	}
	// Cancel alarm
	alarm(0);
}

/**
* catchAlarm
*/
void catchAlarm(int ignored)
{
	g_tries +=1;
}

/**
 * handshake
 *
 */
int handshake(char* ipAddr)
{
	int sockfd, numbytes;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
	char* buf = (char*) malloc(sizeof(char) * MAXDATASIZE);
	struct sigaction myAction;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	myAction.sa_handler = catchAlarm;
	if(sigfillset(&myAction.sa_mask) < 0)
	{
		perror("sigfillset failed\n");
	}
	myAction.sa_flags = 0;

	if(sigaction(SIGALRM, &myAction, 0) < 0)
	{
		perror("sigaction failed for SIGALRM");
	}

	if ((rv = getaddrinfo(ipAddr, PORT, &hints, &servinfo)) != 0) {
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
		free(buf);
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure
	// We seem to need to do some 'handshaking' to get into a send/recv feedback loop
	// Still working on getting this to work correctly
	if(g_connectToRoomba == 1)
	{
		// Receive initial poem from Roomba upon connection
		numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0);
		sprintf(&buf[numbytes], "\0");
		// Print poem and length of poem
		printf("Poem: %s", buf);	   
		printf("numbytes: %d\n", numbytes);

		int cmd = CMD_NO_OP;
		// Send a first command to finish initializing the send/receive sequence
		if(send(sockfd, &cmd, 1, 0) == -1)
		{
		}
	}
	free(buf);

	return sockfd;
}

/**
 * printStats
 *
 */
int printStats(FILE* log)
{
	if(g_statsMode == 0)
	{
		// Print the number of goals found and episodes recieved
		printf("Roomba has found the Goal %i times.\nSupervisor has received %i episodes.\n", NUM_GOALS_TO_FIND, g_episodeList->size);
		int i;
		// Print the timestamp that each goal was found at
		for(i = 0; i < NUM_GOALS_TO_FIND; i++)
		{
			printf("Goal %i was found at time %i", i, g_goalsTimeStamp[i]);

			//If not the first goal, print number of episodes between previous and current goal
			if(i > 0)
			{
				printf(" and it took %i episodes to find it after goal %i\n", g_goalsTimeStamp[i]-g_goalsTimeStamp[i-1], i-1);
			}
			else
			{
				printf("\n");
			}
		}
	}
	else
	{
		int i;
		for(i = 0; i < NUM_GOALS_TO_FIND; i++)
		{
			fprintf(log, "%i:", (i < 1 ? g_goalsTimeStamp[i] : g_goalsTimeStamp[i]-g_goalsTimeStamp[i-1]));
		}
		fprintf(log, "\n");
		fflush(log);
	}
}

/**
 * reportGoalFound
 *
 */
void reportGoalFound(int sockfd, FILE* log)
{
	g_goalsTimeStamp[g_goalsFound] = ((Episode*)getEntry(g_episodeList, g_episodeList->size - 1))->now;
	g_goalsFound++;

	if(g_goalsFound > 1)
	{
		printf("Goal %i found after %i episodes\n", g_goalsFound, g_goalsTimeStamp[g_goalsFound - 1]-g_goalsTimeStamp[g_goalsFound - 2]);
	}
	else
	{
		printf("Goal %i found after %i episodes\n", g_goalsFound, g_goalsTimeStamp[g_goalsFound - 1]);
	}

	int cmd = CMD_SONG;
	sendCommand(sockfd, cmd);

	if(g_connectToRoomba == 1)
	{
		printf("Press enter to continue\n");
		getchar();
	}
}

/**
 * processCommand
 *
 */
void processCommand(int* cmd, char* buf, FILE* log)
{
	// Call Supervisor tick to process recently added episode
	*cmd = tick(buf);

	if(g_statsMode == 0)
	{
		// Print sensor data to log file and force write
		fprintf(log, "Sensor data: [%s] Command received: %i\n", buf, *cmd);
		fflush(log);
	}

	// Error in processing, exit with appropriate error code
	if(*cmd < CMD_NO_OP)
	{
		perror("Tick returned error\n");
		exit(*cmd);
	}

	// If tick gave us an invalid command, exit with appropriate error code
	if(*cmd >= NUM_COMMANDS)
	{
		perror("Illegal command");
		exit(*cmd);
	}
}


int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr,"usage (minimum): client hostname\n");
		exit(1);
	}

	char* buf = (char*) malloc(sizeof(char) * MAXDATASIZE);
	FILE* log = fopen("supClient.log", "a");

	if(!log)
	{
		fprintf(stderr, "File not opened correctly");
		exit(1);
	}
	// Variables used for Supervisor loop processing
	initSupervisor();				// Initialize the Supervisor
	parseArguments(argc, argv);		// Parse the arguments and set up global monitoring vars

	// Socket stuff
	int sockfd = handshake(argv[1]);
	int cmd = CMD_LEFT;				// command to send to Roomba


	// Print raw sensor data to stdout along with size in bytes
	while(1)
	{	       
		recvCommand(sockfd, buf);
		processCommand(&cmd, buf, log);

		// If goal is found increase goal count and store the index it was found at
		if(((Episode*)getEntry(g_episodeList, g_episodeList->size - 1))->sensors[SNSR_IR] == 1)
		{
			reportGoalFound(sockfd, log);
		}
		else
		{
			if(sendCommand(sockfd, cmd) < 0)
			{
				perror("Error sending to socket");
			}
		}

		// Once we've found all the goals, print out some data about the search
		if(g_goalsFound >= NUM_GOALS_TO_FIND)
		{
			printStats(log);
			// exit the while loop
			printf("All goals found. Exiting.\n");
			break;
		}

	}// while

	// End Supervisor, call frees memory associated with Supervisor vectors
	endSupervisor();

	// close the connection to Roomba server
	close(sockfd);
	fclose(log);

	return 0;
}//main

