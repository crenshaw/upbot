#include "supervisor.h"

/**
* This file contains the code for the Supervisor. All the functions
* that are needed for processing raw sensor data are contained in 
* this file as well as those for determining new commands
*
* Author: Dr. Andrew Nuxoll and Zachary Paul Faltersack
* Last Edit: June 4, 2010
*
*/


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
	chooseCommand(ep);

	// Return ep's new command
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
		printf("Sensor data successfully parsed into new episode\n");
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
	static int randChance = 100;	// The chance of choosing a random move
	static int goalCount = 0;		// Number of goals found so far
	static int* goalIdx;			// Array to store indices of goal
	int random;						// int for storing random number
	int indexMatch;					// store the index for a match search
	int* score;						// The score for a match search
	
	// Allocate space for the score pointer
	score = (int*) malloc(sizeof(int));

	// seed rand and allocate goal arr if this is first time a command is chosen
	if(randChance == 100)
	{
		goalIdx = (int*)malloc(sizeof(int) * 10);
		srand(time(NULL));
	}
	// If new episode is goal state store idx and increase count
	if(ep->sensors[SNSR_IR] == 1)
	{
		goalIdx[goalCount] = ep->now;
		goalCount++;
	}

	// Process the episode list and determine which move may be the best
	int i, j;				// indices for loops
	int matchScoreForward;	// the match score for CMD_FORWARD
	int matchScoreRight;	// the match score for CMD_RIGHT
	int matchScoreLeft;		// the match score for CMD_LEFT
	int tempIdx, tempDist;	// temp vars
	int bestMatch;			// index for current best match
	
	// init scores to 0
	matchScoreForward = matchScoreRight = matchScoreLeft = 0; 
	// init distance to size of history
	tempDist = g_episodeList->size;

	// Test out three commands and find best match for each command
	// Then if a goal has been found, determine the distance to the goal
	// and find command with the least distance
	for(i = 0; i < 3; i++)
	{
		// can only successfully search if at minimum history contains
		// NUM_TO_MATCH episodes
		if(g_episodeList->size > NUM_TO_MATCH)
		{
			// for each run test out next command
			// keep track of index of the best match as well as its score
			if(i == 0)
			{
				ep->cmd = CMD_FORWARD;
				tempIdx = match(g_episodeList, score);
				matchScoreForward = *score;
			}else if(i == 1)
			{
				ep->cmd = CMD_RIGHT;
				tempIdx = match(g_episodeList, score);
				matchScoreRight = *score;
			}else
			{
				ep->cmd = CMD_LEFT;
				tempIdx = match(g_episodeList, score);
				matchScoreLeft = *score;
			}
			// If the goal has been found then determine which of the three episodes
			// with the greatest scores is closest to the goal
			if(goalCount > 0)
			{
				for(j = 0; j < goalCount; j++)
				{
					// Make sure the goal is after the current episode
					if(abs(((Episode*)getEntry(g_episodeList, goalIdx[j]))->now - 
						   ((Episode*)getEntry(g_episodeList, tempIdx))->now) < 0)
					{

					}
					// If the distance between the episode and goal is less than previous
					// then save it
					else if(abs(((Episode*)getEntry(g_episodeList, goalIdx[j]))->now - 
						   ((Episode*)getEntry(g_episodeList, tempIdx))->now) < tempDist)
					{
						// keep track of the current best distance
						tempDist = abs(((Episode*)getEntry(g_episodeList, goalIdx[j]))->now - 
						   ((Episode*)getEntry(g_episodeList, tempIdx))->now);
						// keep track of which command gave the best distance so far
						bestMatch = i;
					}// if
				}
			}// if
		}// if
	}// for

	// Determine the next command, random possiblity
	if((random = rand() % 100) < randChance || g_episodeList->size < NUM_TO_MATCH)
	{
		ep->cmd = rand() % NUM_COMMANDS;
	}else
	{
		// If a goal has been found then we have a distance we can use above to find best match
		if(goalCount > 0)
		{
			switch(bestMatch)
			{
				case 0:
					ep->cmd = CMD_FORWARD;
					break;
				case 1:
					ep->cmd = CMD_RIGHT;
					break;
				case 2:
					ep->cmd = CMD_LEFT;
					break;
			}
		}
		// Otherwise just choose the move with the greatest score
		else
		{
			// If this is > instead of >= then the Supervisor prefers CMD_LEFT
			// If this is >= instead of > then the Supervisor prefers CMD_FORWARD
			// My assumption for the reason is that the majority of the time, the scores
			// are the same. I think this problem will go away when we start incorporating
			// the Milestones into the decision process
			if(matchScoreForward >= matchScoreRight && matchScoreForward >= matchScoreLeft)
			{
				ep->cmd = CMD_FORWARD;
			}else if(matchScoreRight >= matchScoreLeft)
			{
				ep->cmd = CMD_RIGHT;
			}else
			{
				ep->cmd = CMD_LEFT;
			}
		}
	}

	// free memory allocated for score
	free(score);

	// decrease random move chance down to a limit
	if(randChance > 10)
	{
		randChance--;
	}
	return ep->cmd;
}// chooseCommand

/**
 * parseEpisode
 *
 *        dataArr contains string of the following format
 *        0000000011  <will be timestamp>  <abort signal>
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

	// Pull out the timestamp
	parsedData->now = atoi(&dataArr[i]);

	// set these to default values for now
	parsedData->aborted = atoi(&dataArr[strlen(dataArr) - 2]);
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
	printf("Sensors:    ");

	// iterate through sensor values and print to stdout
	for(i = 0; i < NUM_SENSORS; i++)
	{
		printf("%i", ep->sensors[i]);
	}

	// print rest of episode data to stdout
	printf("\nTime stamp: %i\nAborted:    %i\nCommand:    %i\n\n", (int)ep->now, ep->aborted, ep->cmd);
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
	*score = 0;

	// Iterate through vector and search from each index
	for(i = vector->size - NUM_TO_MATCH - 1; i >= 0; i--)
	{
		// reset tempscore
		tempScore = 0;
		for(j = 0; j < NUM_TO_MATCH; j++)
		{
			// compare two episodes and add result to score
			tempScore += compare(vector->array[i + j], vector->array[vector->size - NUM_TO_MATCH + j]);

			// greatest possible score is 2 * NUM_SENSORS * NUM_TO_MATCH
			// give a goal episode half this value for 1/3 total value
			// also quit searching if at goal state
			if(((Episode*)(vector->array[i + j]))->sensors[SNSR_IR] == 1)
			{
				tempScore += (NUM_TO_MATCH * NUM_SENSORS);
				j = NUM_TO_MATCH;
			}
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

	// If we have a full match return 1 to add to score, else 0
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
	g_episodeList = newVector();
	g_milestoneList = newVector();
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
