#include "supervisor.h"

/**
* This file contains the code for the Supervisor. All the functions
* that are needed for processing raw sensor data are contained in 
* this file as well as those for determining new commands
*
* Author: Dr. Andrew Nuxoll and Zachary Paul Faltersack
* Last Edit: June 21, 2010
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
* @return int : a command for the Roomba (negative is error)
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
	// Print out the parsed episode
	displayEpisode(ep);
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
* chooseCommand
*
* This function takes a pointer to a new episode and chooses a command
* that should accompany this episode
*
* @arg ep a pointer to the most recent episode
* @return int the command that was chosen
*
*/
int chooseCommand(Episode* ep)
{
	int random;						// int for storing random number
	int i, j;				// indices for loops
	
	// seed rand and allocate goal arr if this is first time a command is chosen
	static int needSeed = TRUE;
	if(needSeed == TRUE)
	{
		needSeed = FALSE;
		srand(time(NULL));
	}

	// Determine the next command, possibility of random command
	if((random = rand() % 100) < g_randChance || g_episodeList->size < NUM_TO_MATCH)
	{
		// Command 0 is now illegal command so adjust NUM_COMMANDS to account for this
		// Then increment to push back into valid command range
		ep->cmd = (rand() % (LAST_MOBILE_CMD)) + CMD_NO_OP;
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
 * If a goal has been found, then find the index of the best match
 *
 * @arg ep pointer to new episode
 * @return int status code
 *
 * @return int status code
 */
int setCommand(Episode* ep)
{	
	int tempIdx, tempDist;							// temp vars
	int i,j;										// looping indices
	int bestMatch = -42;
	int commandScores[NUM_COMMANDS];				// Array to store scores for commands

	// initialize scores to 0
	for(i = 0; i < NUM_COMMANDS; i++)
	{
		commandScores[i] = 0;
	}

	// Set distance to goal equal to largest possible distance
	tempDist = g_episodeList->size;

	// can only successfully search if at minimum history contains
	// NUM_TO_MATCH episodes
	if(g_episodeList->size > NUM_TO_MATCH)
	{
		// Test out three commands and find best match for each command
		// Then if a goal has been found, determine the distance to the goal
		// and find command with the least distance
		for(i = CMD_NO_OP; i <= LAST_MOBILE_CMD; i++)
		{
			// for each run test out next command
			// keep track of index of the best match as well as its score

			ep->cmd = i;
			tempIdx = match(g_episodeList, commandScores + i);

			// If the goal has been found then determine which of the three episodes
			// with the greatest scores is closest to the goal
			if(g_goalCount > 0)
			{
				// init distance to size of history
				tempDist = g_episodeList->size;
				for(j = 0; j < g_goalCount; j++)
				{
					// Make sure the goal is after the current episode
					if(abs(((Episode*)getEntry(g_episodeList, g_goalIdx[j]))->now - 
								((Episode*)getEntry(g_episodeList, tempIdx))->now) < 0)
					{

					}
					// If the distance between the episode and goal is less than previous
					// then save it
					else if(abs(((Episode*)getEntry(g_episodeList, g_goalIdx[j]))->now - 
								((Episode*)getEntry(g_episodeList, tempIdx))->now) < tempDist)
					{
						// keep track of the current best distance
						tempDist = abs(((Episode*)getEntry(g_episodeList, g_goalIdx[j]))->now - 
								((Episode*)getEntry(g_episodeList, tempIdx))->now);
						// keep track of which command gave the best distance so far
						bestMatch = i;
					}// if
				}// for
			}// if
		}// for
	}// if

	// If a goal has been found then we have a distance we can use above to find best match
	if(g_goalCount > 0)
	{
		ep->cmd = bestMatch;
	}
	else
	{
		int max = CMD_NO_OP;
		for(i = CMD_NO_OP; i < NUM_COMMANDS; i++)
		{
			if(commandScores[max] < commandScores[i])
			{
				max = i;
			}
		}
		ep->cmd = max;
	}

	if(g_statsMode == 0)
	{
		for(i = CMD_NO_OP; i < NUM_COMMANDS; i++)
		{
			printf("%s score: %i\n", interpretCommand(i), commandScores[i]);
		}
	}
	if(ep->cmd <= CMD_ILLEGAL || ep->cmd >= NUM_COMMANDS)
	{
		printf("Episode is bad SET\n");
	}

	// return success
	return 0;
}// setCommand


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
	// Allocate space for an episode
	int i;
	char* tmp;
	int foundDigitCount = 0;

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

	if(parsedData->sensors[SNSR_IR] == 1)
	{
		DECREASE_RANDOM(g_randChance);
	}

	if(g_connectToRoomba == 1)
	{
		// Pull out the timestamp
		parsedData->now = timeStamp++;
	}else
	{
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
		parsedData->now = time;
	}

	// set these to default values for now
	parsedData->cmd = CMD_NO_OP;

	if(parsedData->sensors[SNSR_IR] == 1)
	{
		g_goalIdx[g_goalCount] == parsedData->now;
		g_goalCount++;
	}

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
 * match
 *
 * search for a series of episodes that best matches the last NUM_TO_MATCH
 * episodes
 *
 * @arg vector the vector containing full history of episodes
 * @arg score a pointer to int we can use to store/return score
 * @return int index into the vector for the best matching series
 */
int match(Vector* vector, int* score)
{
	int i,j, tempScore = 0, returnIdx = 0;
	double discount = 1;

	// Iterate through vector and search from each index
	for(i = vector->size - NUM_TO_MATCH; i >= 0; i--)
	{
		// reset tempscore
		tempScore = 0;

		for(j = NUM_TO_MATCH - 1; j >= 0; j--)
		{
			// If any state that occurs before the final episode in the match is a goal
			// state then break because that means our current final episode is being 
			// matched to one after a goal, which doesn't help to find the goal again
			// Have to subtract 1 to prevent from breaking if the final episode is a goal
			if(((Episode*)(vector->array[i + j]))->sensors[SNSR_IR] == 1)
			{
				break;
			}

			// compare two episodes and add result to score with appropriate discount
			//					V-index iterate to beginning		V-index to current episode frame
			tempScore += (discount * compare(vector->array[i + j], vector->array[vector->size - NUM_TO_MATCH + j]));
			discount *= DISCOUNT;
		}

		// If we ended up with a greater score than previous, store index and score
		if(tempScore > *score)
		{
			*score = tempScore;
			returnIdx = i;
		}
	}
	// The index of the -closest- match, was not necessarily a full milestone match
	return returnIdx;
}// match

/**
 * compare
 *
 * Compare the sensor arrays of two episodes and return if they match or not
 *
 * @arg ep1 a pointer to an episode
 * @arg ep2 a pointer to another episode
 * @return int The score telling us how close these episodes match
 */
int compare(Episode* ep1, Episode* ep2)
{
	int i, match = 0;

	// Iterate through the episodes' sensor data and determine if they are matching episodes
	for(i = 0; i < NUM_SENSORS; i++)
	{
		if(ep1->sensors[i] == ep2->sensors[i])
			match++;
	}

	// add num_sensors to give cmd 1/2 value
	if(ep1->cmd == ep2->cmd && ep1->cmd != CMD_NO_OP)
	{
		match += NUM_SENSORS;
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

