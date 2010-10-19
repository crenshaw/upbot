#include "nsm.h"

/*
 * nsm.c
 *
 * This file contains our implementation of McCallum's NSM Q-Learning
 * agent. Much of the code is drawn from the Supervisor being developed
 * by Dr. Andrew Nuxoll, Zachary Paul Faltersack and Brian Burns.
 *
 * Author: Zachary Paul Faltersack
 *
 * Last updated: October 11, 2010
 */

#define DEBUGGING 1


// The chance of choosing a random move
int g_randChance = 100;
// global strings for printing to console
char* g_forward = "forward";
char* g_right   = "right";
char* g_left    = "left";
char* g_adjustR = "adjust right";
char* g_adjustL = "adjust left";
char* g_blink   = "blink";
char* g_no_op   = "no operation";
char* g_song    = "song";
char* g_unknown = "unknown";

char* g_forwardS = "FW";
char* g_rightS   = "RT";
char* g_leftS    = "LT";
char* g_adjustRS = "AR";
char* g_adjustLS = "AL";
char* g_blinkS   = "BL";
char* g_no_opS   = "NO";
char* g_songS    = "SO";
char* g_unknownS = "$$";

// Keep track of goals
int g_goalCount = 0;                                            // Number of goals found so far
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
    printf("Creating and adding episode...");
    Episode* ep = updateHistory(sensorInput);
    printf("Episode created\n");

/*
    // If we found a goal, send a song to inform the world of success
    // and if not then send ep to determine a valid command
    if(episodeContainsGoal(ep, FALSE))
    {
        ep->action = CMD_SONG;
    }
    else
    {
*/
        chooseCommand(ep);
/*    } 
*/
    
    // Print out the parsed episode if not in statsMode
    if(g_statsMode == 0)
    {
        displayEpisode(ep);
    }

    return ep->action;
}//tick

/**
 * updateHistory
 *
 * Takes a sensor data string and parses it into the corresponding
 * episode, which is the final one that contains the most recent
 * command. Then it creates a new episode and adds it to the end
 * of our history.
 *
 * @arg sensorData char* filled with sensor information
 * @return Episode* a pointer to the newly added episode
 */
Episode* updateHistory(char* sensorData)
{
	Episode* ep;
	// If we have not experienced a single episode, we must
	// create the first one and add it to the beginning of 
	// our history
	if(g_epMem->size == 0)
	{
		ep = (Episode*) malloc(sizeof(Episode));
		addEpisode(g_epMem, ep);
	}
	else
	{
    	// Assign a pointer to the final episode in our history
    	ep = g_epMem->array[g_epMem->size - 1];
	}
    int retVal;     

    // If error in parsing print appropriate error message and exit
    if((retVal = parseSensors(ep, sensorData)) != 0)
    {
        char errBuf[1024];
        sprintf(errBuf, "Error in parsing: %s\n", sensorData);
        perror(errBuf);
        exit(retVal);
    }

	// Prep the next episode in our history. This will be assigned
	// a command, and once the sensor string is received, that data
	// will be paired with the command that caused it to occur
    ep = (Episode*) malloc(sizeof(Episode));

	addEpisode(g_epMem, ep);
    return ep;
}//updateHistory

/**
 * parseSensors
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
int parseSensors(Episode * parsedData, char* dataArr)
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

	// Set the episode's timestamp equal to it's location in the
	// history array
    parsedData->now = timeStamp++;

    // Found a goal so decrease chance of random move
    if(parsedData->sensors[SNSR_IR] == 1)
    {
        DECREASE_RANDOM(g_randChance);
        g_goalIdx[g_goalCount] = parsedData->now;
        g_goalCount++;
    }

    return 0;
}// parseSensors

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
    printf("\nTime stamp: %i\nCommand:    %i\n\n", (int)ep->now, ep->action);
}// displayEpisode

//---------------------------------------------------------------------------------
// Functions for McCallum's NSM Q-Learning agent

void updateLittleQ()
{
}//updateLittleQ

Neighborhood* locateKNearestNeighbors(int action)
{
	Neighborhod* nbHd = initNeighborhood(action, K_NEAREST);

	int i;


}//locateNearestNeighbors

/**
* calculateNValue
*
* This function takes an index into the vector of states (g_epMem)
* and calculates the neighborhood metric for the associated state
*
* @arg currState An int that contains the index of the current state being processed
*
* @return int This is the n value that was calculated for the state
*/
int calculateNValue(int currState)
{
	int i = 0;

	// Determine if the action matches the one that we are testing for. If not, then
	// the n value is zero and we can return
	if(((Episode*)g_epMem->array[currState])->action == ((Episode*)g_epMem->array[g_epMem->size - 1])->action)
	{
		// Make sure the array indices are within bounds and
		// compare the respective episodes to see if we need to increment the neighborhood metric
		while(currState - i >= 0 && g_epMem->size - 1 - i >= 0 &&
		equalEpisodes(g_epMem->array[currState - 1 - i], g_epMem->array[g_epMem->size - 1 - i]))
		{
			i++;
		}
	}
	
	return i;
}//calculateNValue

void calculateQValue()
{
}//calculateQValue

//--------------------------------------------------------------------------------
// Functions for creating and maintaining neighborhods

/**
* initNeighborhood
*
* This function takes two arguments and creates a new neighborhood
* using this data. A pointer to the neighborhood is returned to the
* caller.
*
* @arg action An integer representing the command associated with the neighborhood
* @arg k An integer indicating how many neighbors to find.
* @return Neighborhood* A pointer to the new neighborhood
*/
Neighborhood* initNeighborhood(int action, int k)
{
	// Allocate space for the new neighborhood
	Neighborhood* nbHd = (Neighborhood*)malloc(sizeof(Neighborhood));

	// Commit neighborhood meta-data
	nbHd->action = action;
	nbHd->kValue = k;
	nbHd->numNeighbors = 0;

	// Allocate memory for the buffers containing our neighbors
	// and their n-values
	nbHd->episodes 	= (Episode**)malloc(k * sizeof(Episode*));
	nbHd->nValues	= (int*)malloc(k * sizeof(int));

	// return a pointer to the new neighborhood
	return nbHd;
}//initNeighborhood

/**
* destroyNeighborhood
*
* Takes a pointer to a neighborhood and frees all associated memory
*
* @arg nbHd A pointer to a neighborhood
*/
void destroyNeighborhood(Neighborhood* nbHd)
{
	free(nbHd->episodes);	// free the array containing episode pointers
	free(nbHd->nValues);	// free the array containing ep n values
	free(nbHd);				// free the memory containing the neighborhood itself
}//destroyNeighborhood

/**
* cleanNeighborhood
*
* Takes a pointer to a neighborhood and resets necessary values in 
* order to reuse it in the future.
* By setting the numNeighbor count back to 0, we can in effect clear
* the entire struct, because all additions are entered with respect 
* to that counter.
*
* @arg nbHd A pointer to the neighborhood
*/
void cleanNeighborhood(Neighborhood* nbHd)
{
	// Reset the counter to 0
	nbHd->numNeighbors = 0;
}//cleanNeighborhood

/**
* addNeighbor
*
* This function adds the new neighbor if it is one of the nearest neighbors
* seen so far. It then calls a sort method to maintain the order
*
* @arg nbHd A pointer to the current neighborhood
* @arg ep A pointer to the new neighbor
* @arg n The n value of the new neighbor
* @return int Success/fail
*/
int addNeighbor(Neighborhood* nbHd, Episode* ep, int n)
{
	// If the new neighbor has a smaller n value than the last
	// neighbor in our current neighborhood and our neighborhood 
	// is full, then we don't need it.
	if(n < nbHd->nValues[nbHd->numNeighbors - 1] &&
		nbHd->numNeighbors < nbHd->kValue) return 0;

	// Check if neighborhood is full, this affects where we enter
	// the new neighbor.
	if(nbHd->numNeighbors < nbHd->kValue)
	{
		nbHd->episodes[nbHd->numNeighbors] = ep;
		nbHd->nValues[nbHd->numNeighbors] = n;
		nbHd->numNeighbors++;
	}
	else
	{
		nbHd->episodes[nbHd->numNeighbors - 1] = ep;
		nbHd->nValues[nbHd->numNeighbors - 1] = n;
	}

	sortNeighborhood(nbHd);

	return 1;
}//addNeighbor

/**
* sortNeighborhood
*
* This function will sort a neighborhood from greatest n value
* to least n value. A more recent addition is considered a greater
* value when compared to an old neighbor with an equivalent n value.
*
* @arg nbHd A pointer to a neighborhood that needs to be sorted
*/
void sortNeighborhood(Neighborhood* nbHd)
{
	int i;
	Episode* tempEp;
	int tempN;
	//iterate from back to front
	for(i = nbHd->kValue - 1; i > 0; i++)
	{
		// Want to move up if equal to or greater
		// this helps to account for always choosing the closest neighbor
		// to the current time when dealing with equivalent matches
		if(nbHd->nValues[i] >= nbHd->nValues[i - 1])
		{
			// Store the moving values in temp value holders
			tempEp = nbHd->episodes[i];
			tempN = nbHd->nValues[i];

			// push back the smaller match
			nbHd->episodes[i] = nbHd->episodes[i - 1];
			nbHd->nValues[i] = nbHd->nValues[i - 1];

			// resave the new addition
			nbHd->episodes[i - 1] = tempEp;
			nbHd->nValues[i - 1] = tempN;
		}//if
	}//for
}//sortNeighborhood

/**
 * getNeighbor
 *
 * Return the i-th neighbor in the neighborhood if available
 *
 * @arg i An int representing the desired index of neighbor
 * @return Episode* A pointer to the episode that is that neighbor
 NULL if error occurs
 */
Episode* getNeighbor(Neighborhood* nbHd, int i)
{
	// Catch any out-of-bounds errors and neighbor unavailable
	if(i < 0 || i >= nbHd->kValue) return NULL;
	if(i >= nbHd->numNeighbors) return NULL;

	return (nbHd->episodes[i]);
}//getNeighbor

//--------------------------------------------------------------------------------
// Main logic functions for determining next action

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
    int i, j;       // indices for loops

    // seed rand if first time called
    static int needSeed = TRUE;
    if(needSeed == TRUE)
    {
        needSeed = FALSE;
        srand(time(NULL));
    }

    // Determine the next command, possibility of random command
    if((rand() % 100) < g_randChance || // Probability of choosing random
		g_epMem->size < NUM_TO_MATCH || // or if list is too short
		g_goalCount == 0)	// or we have not yet found a goal
    {
        ep->action = (rand() % (LAST_MOBILE_CMD)) + CMD_NO_OP;
    }else
    {
        // loop on setCommand until a route is chosen 
		// that will lead to a successful action
        while(setCommand(ep))
        {
            if(g_statsMode == 0) printf("Failed to set a command\n");
        }
    }

    return ep->action;
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
    int tempIdx, tempDist; // temp vars
    int i,j,k;             // looping indices
    int bestMatch = CMD_NO_OP;
    double commandScores[NUM_COMMANDS]; // Array to store scores for commands
    int commandIdxs[NUM_COMMANDS];
    int toggle = 0;
    Vector* episodeList = g_epMem->array[0];
    double partialScoreTable[episodeList->size];

    // initialize scores to 0
    for(i = 0; i < NUM_COMMANDS; i++)
    {
        commandScores[i] = 0;
        commandIdxs[i] = 0;
    }

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
        tempDist = episodeList->size;
        // Test out the available commands
        for(i = CMD_NO_OP; i <= LAST_MOBILE_CMD; i++)
        {
            // find index closest to a subsequent goal
            tempIdx = commandIdxs[i];

            // If the goal has been found then determine which of the three episodes
            // with the greatest scores is closest to the goal

            for(j = 0; j < g_goalCount; j++)
            {
                /*                                              printf("dist: %i\n", g_goalIdx[j] - tempIdx);
                                                                printf("idx1: %i idx2: %i\n", g_goalIdx[j], tempIdx);
                */

                // Make sure distance is greater than 0 and 
                // If the distance between the episode and goal is less than previous
                // then save it
                if(g_goalIdx[i] - tempIdx > 0 && g_goalIdx[j] - tempIdx < tempDist)
                {
                    // keep track of the current best distance
                    tempDist = g_goalIdx[j] - tempIdx;
                    // keep track of which command gave the best distance so far
                    bestMatch = i;
                    toggle = 1;
                    //                                                      printf("Setting toggle, action: %s, tempDist: %i\n", interpretCommand(i), tempDist);
                }// if
            }// for
        }// for 
    }// if


    // If a goal has been found then we have an index of closest goal match
    if(g_goalCount > 0 && toggle == 1)
    {
        ep->action = bestMatch;
    }
    else
    { 
        // else we find action with greatest score
        int max = CMD_NO_OP;
        for(i = CMD_NO_OP; i <= LAST_MOBILE_CMD; i++)
        {
            //Debug lines
            /*                      double maxScore = NUM_TO_MATCH * NUM_SENSORS * 2;
                                    printf("Max score: %g out of: %g for command: %s\n", commandScores[i], maxScore, interpretCommand(i)); */
            if(commandScores[max] < commandScores[i])
            {
                max = i;
            }
        }
        ep->action = max;
    }

    // If not stats mode print scores for actions
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
      printf("Still have not found valid bestMatch, action: %s\n", interpretCommand(ep->action));
      }
      else
      {
      printf("Found valid bestMatch, action: %s\n", interpretCommand(ep->action));
      }
    */


    // Report malformed episode and location of error report
    if(ep->action <= CMD_ILLEGAL || ep->action >= NUM_COMMANDS)
    {
        printf("Episode is bad: setCommand %s (%i)\n", interpretCommand(ep->action), ep->action);
    }

    // return success
    return 0;
}// setCommand

/*
 * equalEpisodes
 *
 * This method takes two episodes and returns a boolean indicating
 * if they are a match. A match is when both the sensor data and 
 * the action are the same in each episode.
 *
 * @arg ep1 A pointer to the first episode
 * @arg ep2 A pointer to the second episode
 *
 * @return int A boolean indicating if they are a full match
 */
int equalEpisodes(Episode* ep1, Episode* ep2)
{
    int i;
    // Ensure the sensor data match between episodes and return
    // false if not
    for(i = 0; i < NUM_SENSORS; i++)
    {
        if(ep1->sensors[i] != ep2->sensors[i]) return FALSE;
    }

    // Ensure episodes have same command, return false if not
    if(ep1->action != ep2->action) return FALSE;

    return TRUE;
}// equalEpisodes

/**
 * findTopMatch
 */
int findTopMatch(double* scoreTable, double* indvScore, int command)
{
    int i, max;
    double maxVal = 0.0, tempVal = 0.0;
    Vector* episodeList = g_epMem->array[0];
    for(i = episodeList->size - 1; i >= 0; i--)
    {
        tempVal = scoreTable[i] + (((Episode*)(episodeList->array[i]))->action == command ? NUM_TO_MATCH : 0);

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
 * initNSM
 *
 * Initialize the NSM vectors
 * 
 */
void initNSM()
{
    g_epMem         = newVector();

    g_connectToRoomba       = 0;
    g_statsMode             = 0;
}//initNSM

/**
 * endNSM
 *
 * Free the memory allocated for the NSM agent
 */
void endNSM() 
{
    freeVector(g_epMem);
}//endNSM

/**
 * interpretCommand
 *
 * Return a char* with the string equivalent of a command
 * Use for printing to console
 *
 * @arg action Integer representing the command
 * @return char* Char array with command as string
 */
char* interpretCommand(int action)
{
    switch(action)
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

/**
 * interpretCommandShort
 *
 * Return a char* with the string equivalent of a command
 * Use for printing to console
 *
 * @arg action Integer representing the command
 * @return char* Char array with command as string
 */
char* interpretCommandShort(int action)
{
    switch(action)
    {
        case CMD_NO_OP:
            return g_no_opS;
            break;
        case CMD_FORWARD:
            return g_forwardS;
            break;
        case CMD_LEFT:
            return g_leftS;
            break;
        case CMD_RIGHT:
            return g_rightS;
            break;
        case CMD_BLINK:
            return g_blinkS;
            break;
        case CMD_ADJUST_LEFT:
            return g_adjustLS;
            break;
        case CMD_ADJUST_RIGHT:
            return g_adjustRS;
            break;
        case CMD_SONG:
            return g_songS;
            break;
        default:
            return g_unknownS;
            break;
    }
}// interpretCommandShort

/**
 * interpretSensorsShort
 *
 * Return an integer that summaries the sensor values in an episdode.
 * Since all sensors are binary we can combine them all into one
 * binary integer.
 *
 * @arg int* Sensors array of ints representing the sensors (must be
 *           of length NUM_SENSORS)
 * @return int that summarizes sensors
 */
int interpretSensorsShort(int *sensors)
{
    int i, result = 0;
    int sumval = 1;  //This is always = to 2^i
    for(i = NUM_SENSORS-1; i >= 0; i--)
    {
        if (sensors[i])
        {
            result += sumval;
        }

        sumval *= 2;
    }

    return result;
}// interpretSensorsShort

