/**
* supervisorClient.c
*
* This is the client that connects to the Roomba server.
* It creates a connection and receives char arrays containing the sensor
* data and sends the commands that the Supervisor decides
*
* Author: Dr. Crenshaw, Dr. Nuxoll, Zachary Faltersack, Steve Beyer
* Last edit: July 5, 2010
*
* Usage: supervisorClient.out <ip_addr> -c <roomba/test> -m <stats/visual>
*/

#include "communication.h"
#include "../supervisor/supervisor.h"

#define TIMEOUT_SECS	5	// Num seconds in timeout on recv
#define MAX_TRIES		10	// Num tries to reconnect on lost connection

int g_goalsFound = 0;				// Number of times we found the goal
int g_goalsTimeStamp[NUM_GOALS_TO_FIND];	// Timestamps of found goals
int g_tries;	// Number of tries to reconnect

/**
 * exitError
 *
 * Close the Supervisor upon error and print error code
 *
 * @arg errCode A error code indicating the type of failure
 */
void exitError(int errCode)
{
	printf("Error. Ending...");
	endSupervisor();
	exit(errCode);
}// exitError

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
	// Iterate through arguments and set vars based on flags found
	for(i = 0; i < argc; i++)
	{
		// -c : connect (roomba/test)
		if(strcmp(argv[i], "-c") == 0)
		{
			if(strcmp(argv[i+1], "roomba") == 0)
			{
				g_connectToRoomba = 1;
			}
			else if(strcmp(argv[i+1], "test") == 0)
			{
				g_connectToRoomba = 0;
			}
		}
		// -m : mode (stats/visual)
		else if(strcmp(argv[i], "-m") == 0)
		{
			if(strcmp(argv[i+1], "stats") == 0)
			{
				g_statsMode = 1;
			}
			else if(strcmp(argv[i+1], "visual") == 0)
			{
				g_statsMode = 0;
			}
		}// if
	}// for
}// parseArguments

/**
 * sendCommand
 *
 * Send a command (int) through socket (sockfd)
 *
 * @arg sockfd A socket identifier
 * @arg cmd An integer representing the command to be sent
 * @return int A success code
 */
int sendCommand(int sockfd, int cmd)
{
	// Attempt to send command and catch error code
	int retVal = send(sockfd, &cmd, 1, 0);
	// Else send command to Roomba server and exit if unsuccessful
	if(retVal != -1 && g_statsMode == 0)
	{
		// Print command sent to Roomba on stdout
		printf("The command value sent was: %s (%i)\n", interpretCommand(cmd), cmd);
	}
	return retVal;
}// sendCommand

/**
 * recvCommand
 *
 * Derived from exmaple code in "TCP/IP Sockets in C: Practical Guide for Programmers"
 * written by Michael J. Donahoo and Kennet L. Calvert
 *
 * This function takes a socket id and a char buffer and waits to receive sensor data
 * from the Roomba. If a command is not received directly, it drops into a loop that
 * queries for the Roomba until contact is made succesfully.
 *
 * @arg sockfd A socket identifier
 * @arg buf A char buffer to read data into
 * @return int A success code
 */
int recvCommand(int sockfd, char* buf)
{
	// reset the number of tries
	g_tries = 0;
	if(g_statsMode == 0)
	{
		// Receive sensor data from socket and store in 'buf'
		printf("Receiving sensor data.\n");
	}

	// Number of bytes written to char buffer
	int numbytes;
	// Insert null terminating character at end of sensor string
    buf[0] = '\0';

	alarm(TIMEOUT_SECS);        /* Set the timeout */
	while ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) < 0)
	{
		if (errno == EINTR)     /* Alarm went off  */
		{
			if (g_tries < MAX_TRIES)      /* incremented by signal handler */
			{
				// Timed out and have mroe tries
				printf("timed out, %d more tries...\n", MAX_TRIES-g_tries);
				// Attempt to send a CMD_NO_OP
				int cmd = CMD_NO_OP;
				if (sendCommand(sockfd, cmd))
				{
					perror("Error sending CMD_NO_OP on retry\n");
				}
				// Reset alarm
				alarm(TIMEOUT_SECS);
			} 
			else		/* have used up all of out tries  */
			{
				perror("Timed out on send command\n");
			}
		} 
		else			/* error was not caused by alarm  */
		{
			perror("Error on receive\n");
		}
	}// while

	// Insert null terminating character at end of sensor string
	sprintf(&buf[numbytes], "\0");

	// Print out the contents of buf
	if(g_statsMode == 0)
	{
		printf("client: sensor data: '%s'\n", buf);	   
		printf("numbytes: %d\n", numbytes);
	}
	// Cancel alarm
	alarm(0);

	return 0;
}// recvCommand

/**
* catchAlarm
*
* Registered function for catching the recv timeout alarm
*
* @arg ignored Unsure
*/
void catchAlarm(int ignored)
{
	g_tries +=1;
}// catchAlarm

/**
 * handshake
 *
 * This is becoming a catchall function used for initializing the connection
 * between the Supervisor and the Roomba.
 *
 * @arg ipAddr This is the IP address of the Roomba we are connecting to
 * @return int Socket ID
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

	// Set up the alarm for recv timeouts
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
	//---------------------------------------

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

	/*---------------------------------------------*/
	// ABOVE: Creating connection
	// BELOW: Conducting handshake
	/*---------------------------------------------*/

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
	else if(g_statsMode == 1)
	{
		// For the unit test this will toggle its copy of g_statsMode
		int cmd = CMD_BLINK;
		if(send(sockfd, &cmd, 1, 0) == -1)
		{
		}
	}// if

	// Free the space allocated for recv buffer
	free(buf);

	return sockfd;
}// handshake

/**
 * printStats
 *
 * Print information about the search for goals. Called once the Roomba
 * has found all the goals.
 *
 * @arg log A file handle for file IO
 */
void printStats(FILE* log)
{
	// == 0 means print to console
	Vector* episodeList = g_epMem->array[0];
	if(g_statsMode == 0)
	{
		// Print the number of goals found and episodes recieved
		printf("Roomba has found the Goal %i times.\nSupervisor has received %i episodes.\n", NUM_GOALS_TO_FIND, episodeList->size);
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
		}// for
	}
	else	// otherwise print to file for import into spreadsheet
	{
		int i;
		for(i = 0; i < NUM_GOALS_TO_FIND; i++)
		{
			fprintf(log, "%i:", (i < 1 ? g_goalsTimeStamp[i] : g_goalsTimeStamp[i]-g_goalsTimeStamp[i-1]));
		}// for
		fprintf(log, "\n");
		fflush(log);
	}// if
}// printStats

/**
 * reportGoalFound
 *
 * Print that a goal as found to console
 *
 * @arg sockfd A socket identifier
 * @arg log A file handle for IO
 */
void reportGoalFound(int sockfd, FILE* log)
{
	// Store the new goal timestamp and increment count
	Vector* episodeList = g_epMem->array[0];
	g_goalsTimeStamp[g_goalsFound] = ((Episode*)episodeList->array[episodeList->size - 1])->now;
	g_goalsFound++;

	// Only print if not in stats mode
	if(g_goalsFound > 1)
	{
		printf("Goal %i found after %i episodes at timestamp %i\n"
							, g_goalsFound
							, g_goalsTimeStamp[g_goalsFound - 1] - g_goalsTimeStamp[g_goalsFound - 2]
							, g_goalsTimeStamp[g_goalsFound - 1]);
	}
	else
	{
		printf("Goal %i found at timestamp %i\n"
							, g_goalsFound
							, g_goalsTimeStamp[g_goalsFound - 1]);
	}

	// Send a success command
	int cmd = CMD_SONG;
	sendCommand(sockfd, cmd);

	// If connected to Roomba pause to allow time to return Roomba to Init
	if(g_connectToRoomba == 1)
	{
		printf("Press enter to continue\n");
		getchar();
	}
}// reportGoalFound

/**
 * processCommand
 *
 * Call the Supervisor tick method and pass it the sensor data from
 * the Roomba and catch the command chosen to send to the Roomba
 *
 * @arg cmd Int pointer to location of command
 * @arg buf Char buffer containing the sensor data
 * @arg log A file handle for IO
 */
void processCommand(int* cmd, char* buf, FILE* log)
{
	// Call Supervisor tick to process recently added episode
	*cmd = tick(buf);

	if(g_statsMode == 0)
	{
		// Print sensor data to log file and force write
		fprintf(log, "Sensor data: [%s] Command received: %s\n", buf, interpretCommand(*cmd));
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
}// processCommand

/**
 * main
 *
 * The main function for dealing with send/recv loop
 */
int main(int argc, char *argv[])
{
	// User did not pass in minimum number of arguments
	if (argc < 2) {
		fprintf(stderr,"\nUSAGE: %s <hostname> [-m stats|visual] [-c roomba|test]\n\n",
                argv[0]);
		exit(1);
	}

	// Open log file for output
	char* buf = (char*) malloc(sizeof(char) * MAXDATASIZE);
	FILE* log = fopen("supClient.log", "a");

	// File was not opened correctly
	if(!log)
	{
		fprintf(stderr, "File not opened correctly");
		exit(1);
	}

	initSupervisor();				// Initialize the Supervisor
	parseArguments(argc, argv);		// Parse the arguments and set up global monitoring vars

	// Socket stuff
	int sockfd = handshake(argv[1]);
	int cmd = CMD_LEFT;				// command to send to Roomba

	Vector* episodeList = g_epMem->array[0];
	// Main send/recv processing loop
	while(1)
	{	       
		// receive the sensor data
		recvCommand(sockfd, buf);
		// determine the next command to send
		processCommand(&cmd, buf, log);

		// If goal is found increase goal count and store the index it was found at
//		if(((Episode*)getEntry(episodeList, episodeList->size - 1))->sensors[SNSR_IR] == 1)
		if(((Episode*)episodeList->array[episodeList->size - 1])->sensors[SNSR_IR] == 1)
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

