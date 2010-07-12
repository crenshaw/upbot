#include "supervisor.h"

/**
* This file contains the code for the Supervisor. All the functions
* that are needed for processing raw sensor data are contained in 
* this file as well as those for determining new commands
*
* Author: Dr. Andrew Nuxoll and Zachary Paul Faltersack
* Last Edit: July 5, 2010
*
*/

// The chance of choosing a random move
int g_randChance = 80;
// global strings for printing to console
char* g_forward = "forward";
char* g_right	= "right";
char* g_left	= "left";
char* g_adjustR	= "adjust right";
char* g_adjustL	= "adjust left";
char* g_blink	= "blink";
char* g_no_op	= "no operation";
char* g_song	= "song";
char* g_unknown	= "unknown";
// Keep track of goals
int g_goalCount = 0;						// Number of goals found so far
int g_goalIdx[NUM_GOALS_TO_FIND];


/**
* tick
*
* This function is called at regular intervals and processes
* the recent sensor data to determine the next action to take.
*
* @param sensorInput a char string wth sensor data
* @return int a command for the Roomba (negative is error)
*/
int tick(char* sensorInput)
{
	// Create new Episode
	Episode* ep = createEpisode(sensorInput);
	// Add new episode to the history
	addEpisode(g_episodeList, ep);
	// Send ep to receive a command
	// Will return -1 if no command could be set
	chooseCommand(ep);
	// Print out the parsed episode if not in statsMode
	if(g_statsMode == 0)
	{
		displayEpisode(ep);
	}
	return ep->cmd;
}// tick

/**
 * createEpisode
 *
 * Takes a sensor data string and allocates space for episode
 * then parses the data and populates the episode and adds it
 * to the global episode list
 *
 * @arg sensorData char* filled with sensor information
 * @return Episode* a pointer to the newly added episode
 */
Episode* createEpisode(char* sensorData)
{
	// Allocate space for episode and score
	Episode* ep = (Episode*) malloc(sizeof(Episode));
	int retVal;	

	// If error in parsing print appropriate error message and exit
	if((retVal = parseEpisode(ep, sensorData)) != 0)
	{
		char errBuf[1024];
		sprintf(errBuf, "Error in parsing: %s\n", sensorData);
		perror(errBuf);
		exit(retVal);
	}else
	{
		if(g_statsMode == 0)
		{
			printf("Sensor data successfully parsed into new episode\n");
		}
	}
	return ep;
}// createEpisode

/**
 * parseEpisode
 *
 *        dataArr contains string of the following format
 *        0000000011  <will be timestamp> 
 *
 * Take a raw sensor packet from Roomba and parse information
 * out to an instance of Episode.
 *
 * @arg parsedData A pointer to an Episode to be populated
 * @arg dataArr the char array that contains the raw sensor data
 * @return int an error code
 *
 */
int parseEpisode(Episode * parsedData, char* dataArr)
{
	// temporary timestamp
	static int timeStamp = 0;
	int i; // index

	if(dataArr == NULL)
	{
		printf("data arr in parse is null");
		return -1;
	}

	// set the episodes sensor values to the sensor data
	for(i = 0; i < NUM_SENSORS; i++)
	{
		// convert char to int and return error if not 0/1
		int bit = (dataArr[i] - '0');
		if ((bit < 0) || (bit > 1))
		{
			printf("%s", dataArr);
			return -1;	
		}

		// else save sensor bit
		parsedData->sensors[i] = bit;
	}

	if(g_connectToRoomba == 1)
	{
		// Pull out the timestamp
		parsedData->now = timeStamp++;
	}else
	{
		// Alg for determining timestamp from string of chars
		int time = 0;
		for(i = NUM_SENSORS; dataArr[i] != '\0'; i++)
		{
			if(dataArr[i] != ' ')
			{
				time = time * 10 + (dataArr[i] - '0');
			}
			if(dataArr[i] == ' ' && time != 0)
			{
				break;
			}
		}
		// Store the time
		parsedData->now = time;
	}

	// Found a goal so decrease chance of random move
	if(parsedData->sensors[SNSR_IR] == 1)
	{
		DECREASE_RANDOM(g_randChance);
		g_goalIdx[g_goalCount] = parsedData->now;
		g_goalCount++;
	}

	// Command gets a default value for now
	parsedData->cmd = CMD_NO_OP;

	return 0;
}// parseEpisode

/**
 * addEpisode
 *
 * Add new episode to episode history vector
 *
 * @arg episodes pointer to vector containing episodes
 * @arg item pointer to episode to be added
 * @return int status code (0 == success)
 */
int addEpisode(Vector* episodes, Episode* item)
{
	return addEntry(episodes, item);
}// addEpisode

/**
 * displayEpisode
 *
 * Display the contents of an episode
 *
 * @arg ep a pointer to an episode
 */
void displayEpisode(Episode * ep)
{
	int i;
	printf("\nSensors:    ");

	// iterate through sensor values and print to stdout
	for(i = 0; i < NUM_SENSORS; i++)
	{
		printf("%i", ep->sensors[i]);
	}

	// print rest of episode data to stdout
	printf("\nTime stamp: %i\nCommand:    %i\n\n", (int)ep->now, ep->cmd);
}// displayEpisode

/**
 * chooseCommand
 *
 * This function takes a pointer to a new episode and chooses a command
 * that should accompany this episode
 *
 * @arg ep a pointer to the most recent episode
 * @return int the command that was chosen
 */
int chooseCommand(Episode* ep)
{
	int i, j;	// indices for loops

	// seed rand if first tiem called
	static int needSeed = TRUE;
	if(needSeed == TRUE)
	{
		needSeed = FALSE;
		srand(time(NULL));
	}

	// Determine the next command, possibility of random command
	if((rand() % 100) < g_randChance || g_episodeList->size < NUM_TO_MATCH)
	{
		// Command 0 is now illegal command so adjust NUM_COMMANDS to account for this
		// Then increment to push back into valid command range
		ep->cmd = (rand() % LAST_MOBILE_CMD) + CMD_NO_OP;
	}else
	{
		// find the best match scores for the three commands
		// if no goal has been found (returns 0) then we take the command with the greatest score
		if(setCommand(ep) != 0)
		{
			if(g_statsMode == 0)
			{
				printf("Failed to set a Command");
			}
			return -1;
		}
	}
	//	printf("COMMAND TO BE SENT %s (%i)\n", interpretCommand(ep->cmd), ep->cmd);


	return ep->cmd;
}// chooseCommand

/**
 * setCommand
 *
 * Find the match scores for each of the available commands (currently condensed list)
 * If a goal has been found, then find the index of the best match (closest to subsequent goal)
 *
 * @arg ep pointer to new episode
 * @return int status code
 */
int setCommand(Episode* ep)
{	
	int tempIdx, tempDist;							// temp vars
	int i,j,k;										// looping indices
	int bestMatch = CMD_NO_OP;
	double commandScores[NUM_COMMANDS];				// Array to store scores for commands
	int commandIdxs[NUM_COMMANDS];
	int toggle = 0;
	double partialScoreTable[g_episodeList->size];

	// initialize scores to 0
	for(i = 0; i < NUM_COMMANDS; i++)
	{
		commandScores[i] = 0;
		commandIdxs[i] = 0;
	}

	// generate partial score table
	generateScoreTable(g_episodeList, partialScoreTable);

	// determine index with top match per command
	for(i = CMD_NO_OP; i <= LAST_MOBILE_CMD; i++)
	{
		commandIdxs[i] = findTopMatch(partialScoreTable, commandScores, i);
	}

	// can only successfully search if at minimum history contains
	// NUM_TO_MATCH episodes
	if(g_goalCount > 0)
	{
		// Set distance to goal equal to largest possible distance
		tempDist = g_episodeList->size;
		// Test out the available commands
		for(i = CMD_NO_OP; i <= LAST_MOBILE_CMD; i++)
		{
			// find index closest to a subsequent goal
			tempIdx = commandIdxs[i];

			// If the goal has been found then determine which of the three episodes
			// with the greatest scores is closest to the goal

			for(j = 0; j < g_goalCount; j++)
			{
				/*						printf("dist: %i\n", g_goalIdx[j] - tempIdx);
										printf("idx1: %i idx2: %i\n", g_goalIdx[j], tempIdx);
				 */

				// Make sure the goal is after the current episode
				if(g_goalIdx[j] - tempIdx < 0)
				{

				}
				// If the distance between the episode and goal is less than previous
				// then save it
				else if(g_goalIdx[j] - tempIdx < tempDist)
				{
					// keep track of the current best distance
					tempDist = g_goalIdx[j] - tempIdx;
					// keep track of which command gave the best distance so far
					bestMatch = i;
					toggle = 1;
					//							printf("Setting toggle, cmd: %s, tempDist: %i\n", interpretCommand(i), tempDist);
				}// if
			}// for
		}// for 
	}// if


	// If a goal has been found then we have an index of closest goal match
	if(g_goalCount > 0 && toggle == 1)
	{
		ep->cmd = bestMatch;
	}
	else
	{ 
		// else we find cmd with greatest score
		int max = CMD_NO_OP;
		for(i = CMD_NO_OP; i <= LAST_MOBILE_CMD; i++)
		{
			//Debug lines
			/*			double maxScore = NUM_TO_MATCH * NUM_SENSORS * 2;
						printf("Max score: %g out of: %g for command: %s\n", commandScores[i], maxScore, interpretCommand(i)); */
			if(commandScores[max] < commandScores[i])
			{
				max = i;
			}
		}
		ep->cmd = max;
	}

	// If not stats mode print scores for cmds
	if(g_statsMode == 0)
	{
		for(i = CMD_NO_OP; i < NUM_COMMANDS; i++)
		{
			printf("%s score: %f\n", interpretCommand(i), commandScores[i]);
		}
	}
	/*
	   if(toggle != 1)
	   {
	   printf("Still have not found valid bestMatch, cmd: %s\n", interpretCommand(ep->cmd));
	   }
	   else
	   {
	   printf("Found valid bestMatch, cmd: %s\n", interpretCommand(ep->cmd));
	   }
	 */


	// Report malformed episode and location of error report
	if(ep->cmd <= CMD_ILLEGAL || ep->cmd >= NUM_COMMANDS)
	{
		printf("Episode is bad: setCommand %s (%i)\n", interpretCommand(ep->cmd), ep->cmd);
	}

	// return success
	return 0;
}// setCommand


/**
* findTopMatch
*/
int findTopMatch(double* scoreTable, double* indvScore, int command)
{
	int i, max;
	double maxVal = 0.0, tempVal = 0.0;
	for(i = g_episodeList->size - 1; i >= 0; i--)
	{
		tempVal = scoreTable[i] + (((Episode*)(g_episodeList->array[i]))->cmd == command ? NUM_TO_MATCH : 0);

		if(tempVal > maxVal)
		{
			max = i;
			maxVal = tempVal;
		}
	}

	indvScore[command] = maxVal;

	return max;
}// findTopMatch


/**
 * match
 *
 * search for a series of episodes that best matches the last NUM_TO_MATCH
 * episodes
 *
 * @arg vector the vector containing full history of episodes
 * @arg score a pointer to double we can use to store/return score
 * @arg topIdxArr pointer to array of 3 ints to store top 3 matching indices
 * @return int Error code
 */
int generateScoreTable(Vector* vector, double* score)
{
	int i,j, returnIdx = 0;
	double tempScore = 0.0, discount = 1.0;
	int start = g_episodeList->size - NUM_TO_MATCH - 1; // subtract 1 to begin 1 episode before curr state

	if(g_goalCount > 0)
	{
		start = g_goalIdx[g_goalCount - 1] - NUM_TO_MATCH;
	}

	// Iterate through vector and search from each index
	for(i = start; i >= 0; i--)
	{
		// reset tempscore
		tempScore = 0;

		for(j = NUM_TO_MATCH - 1; j >= 0; j--)
		{
			// If any state that occurs before the final episode in the match is a goal
			// state then break because that means our current final episode is being 
			// matched to one after a goal, which doesn't help to find the goal again
			if(((Episode*)(vector->array[i + j]))->sensors[SNSR_IR] == 1)
			{
				break;
			}

			// compare two episodes and add result to score with appropriate discount
			//					V-index iterate to beginning		V-index to current episode frame
			tempScore += (discount * compare(vector->array[i + j], vector->array[vector->size - NUM_TO_MATCH + j], (j == NUM_TO_MATCH - 1 ? TRUE : FALSE)));
			discount *= DISCOUNT;
		}//for

		score[i + NUM_TO_MATCH - 1] = tempScore;
	}// for
	// return success
	return 0;
}// match

/**
 * compare
 *
 * Compare the sensor arrays of two episodes and return if they match or not
 *
 * @arg ep1 a pointer to an episode
 * @arg ep2 a pointer to another episode
 * @return double The score telling us how close these episodes match
 */
double compare(Episode* ep1, Episode* ep2, int isCurrMatch)
{
	int i;
	double match = 0;

	// Iterate through the episodes' sensor data and determine if they are matching episodes
	for(i = 0; i < NUM_SENSORS; i++)
	{
		if(ep1->sensors[i] == ep2->sensors[i])
		{
			match++;
		}
	}

	if(isCurrMatch == FALSE)
	{
		// add num_sensors to give cmd 1/2 value
		if(ep1->cmd == ep2->cmd && ep1->cmd != CMD_NO_OP)
		{
			match += NUM_SENSORS;
		}
	}

	// return the total value of the match between episodes
	return match;
}// compare

/**
 * initSupervisor
 *
 * Initialize the Supervisor vectors
 * 
 */
void initSupervisor()
{
	g_episodeList 		= newVector();
	g_milestoneList 	= newVector();
	g_connectToRoomba 	= 0;
	g_statsMode 		= 0;
}// initSupervisor

/**
 * endSupervisor
 *
 * Free the memory allocated for the Supervisor
 */
void endSupervisor()
{
	freeVector(g_episodeList);
	freeVector(g_milestoneList);
}// endSupervisor

/**
 * interpretCommand
 *
 * Return a char* with the string equivalent of a command
 * Use for printing to console
 *
 * @arg cmd Integer representing the command
 * @return char* Char array with command as string
 */
char* interpretCommand(int cmd)
{
	switch(cmd)
	{
		case CMD_NO_OP:
			return g_no_op;
			break;
		case CMD_FORWARD:
			return g_forward;
			break;
		case CMD_LEFT:
			return g_left;
			break;
		case CMD_RIGHT:
			return g_right;
			break;
		case CMD_BLINK:
			return g_blink;
			break;
		case CMD_ADJUST_LEFT:
			return g_adjustL;
			break;
		case CMD_ADJUST_RIGHT:
			return g_adjustR;
			break;
		case CMD_SONG:
			return g_song;
			break;
		default:
			return g_unknown;
			break;
	}
}// interpretCommand

