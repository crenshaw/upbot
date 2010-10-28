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
 * Last updated: October 22, 2010
 */

#define DEBUGGING 1


// The chance of choosing a random move
double g_randChance = 100;
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
    printf("Updating history\n");
    Episode* ep = updateHistory(sensorInput);
    printf("History updated\n\n");

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
        // we then must return because we have no sensor data with
        // which to fill this initial episode
        ep = (Episode*) malloc(sizeof(Episode));
        addEpisode(g_epMem, ep);
        return ep;
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

//TODO: hacky, and not quite correct. Need to avoid updating Qs after the very first goal
//		but immediately after all subsequent goals is fine.
	if(g_goalCount > 0/* && g_goalIdx[g_goalCount - 1] != ep->now*/) updateAllLittleQ(ep);

    // Print out the parsed episode if not in statsMode
    if(g_statsMode == 0)
    {
        printf("Most recent completed episode:\n");
        displayEpisode(ep);
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
        // Assign reward for success
        parsedData->reward = REWARD_SUCCESS;
    }
    else
    {
        // Assign reward for failure
        parsedData->reward = REWARD_FAIL;
    }

	// Default the expected future discounted reward for now
	parsedData->qValue = 0;

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
    // Print the action of the episode
    printf("\nAction: %s", interpretCommand(ep->action));
    
    // iterate through sensor values and print to stdout
    int i;
    printf("\nSensors:    ");
    for(i = 0; i < NUM_SENSORS; i++)
    {
        printf("%i", ep->sensors[i]);
    }

    // Print the reward this episode received
    printf("\nReward: %f\n", ep->reward);

	//Print the expected future discounted reward
	printf("Q Value: %f\n", ep->qValue);

    // Print time stamp
    printf("Time stamp: %i\n\n", (int)ep->now);
}// displayEpisode

/**
 * displayEpisodeShort
 *
 * This function prints an episode as a condensed triple {Action,Percept,Reward}
 *
 * @arg ep A pointer to the episode to be displayed
 */
void displayEpisodeShort(Episode* ep)
{
    printf("{%s,%i,%f}", interpretCommandShort(ep->action), interpretSensorsShort(ep->sensors), ep->reward);
}//displayEpisodeShort

//---------------------------------------------------------------------------------
// Functions for McCallum's NSM Q-Learning agent

/**
 * updateAllLittleQ
 *
 * This function will update the expected future discounted rewards for the action that
 * was most recently executed. We cannot guarantee that the chosen action was executed
 * because of the exploration rate. To account for this we will index into the vector
 * of neighborhoods and update the neighborhood relevant to the executed action.
 *
 * @arg ep A pointerto the episode containing the most recently exectued action
 */
void updateAllLittleQ(Episode* ep)
{
	printf("\n================================================================\n");
    printf("Updating expected future discounted rewards for voting states in the following neighborhood\n\n");
    // set a pointerto the neighborhood related to the most recently executed action
    // remember to offset for the relative action base
    Neighborhood* nbHd = g_neighborhoods->array[ep->action - CMD_NO_OP];
	
	// Display the neighborhood with the voting states
	displayNeighborhood(nbHd);
	printf("================================================================\n\n");

	printf("Calculating utility\n");
	// Recalculate the Q value to be used as the utility for updating expected rewards
	double utility = calculateQValue(nbHd);
	printf("Utility calculated: %f\n", utility);
    int i;
    for(i = 0; i < nbHd->numNeighbors; i++)
    {
        setNewLittleQ(nbHd->episodes[i], utility);
    }

	// Finally, update the most recent episode's Q value 
	setNewLittleQ(ep, utility);

    printf("Done updating expected discounted rewards\n");
	printf("Updated neighborhood\n");
	displayNeighborhood(nbHd);
	printf("================================================================\n\n");
}//updateAllLittleQ

/**
 * setNewLittleQ
 *
 * This functions takes an episode and the current utility and updates the episode's
 * expected future discounted reward. 
 *
 * @arg ep A pointer to an episode to update
 * @arg utility A double that contains the current state's utility used to update
 *              the episodes that voted for the most recent action
 *
 * @return int A success code
 */
int setNewLittleQ(Episode* ep, double utility)
{
printf("Calculating and setting new expected future discounted reward\n");
    ep->qValue = (1.0 - LEARNING_RATE) * (ep->qValue) + LEARNING_RATE * (ep->reward + DISCOUNT * utility);
    return SUCCESS;
}//setNewLittleQ

/**
 * populateNeighborhoods
 *
 * This function will repopulate the neighborhoods with the new relevant data
 *
 * @return int A success status
 */
int populateNeighborhoods()
{
// We want to make sure the neighborhood vector was created correctly.
// These two values should be equal if that is the case
assert(g_neighborhoods->size == LAST_MOBILE_CMD);
    int i;
    for(i = 0; i < g_neighborhoods->size; i++)
    {
        // free the current neighborhood related to the current action
        destroyNeighborhood(g_neighborhoods->array[i]);
        // must offset i with CMD_NO_OP because that is the relative base to our actions
		printf("==========>> Populating neighborhood for action: %s\n",interpretCommand(i + CMD_NO_OP));
        g_neighborhoods->array[i] = locateKNearestNeighbors(i + CMD_NO_OP);
    }
	printf("\n=====================================================\n\n");
    return SUCCESS;
}//populateNeighborhoods

/**
* locateKNearestNeighbors
*
* This function initializes a neighborhood and populates it with the K nearest
* neighbors of the action that it is passed
*
* @arg action An integer that represents the action whose neighborhood we are populating
*
* @return Neighborhood* A pointer to the neighborhood that was populated for the action
*/
Neighborhood* locateKNearestNeighbors(int action)
{
    // initialize a neighborhood with the action and K
printf("initializing neighborhood\n");
    Neighborhood* nbHd = initNeighborhood(action, K_NEAREST);
    // set current episode action temporarily to the current testing action
printf("Setting neighborhood action\n");
    ((Episode*)g_epMem->array[g_epMem->size - 1])->action = action;

    int i;
    int n;
    // Iterate from oldest to newest episode and process results for neighborhood metric
    // then send to be tested for addition to the neighborhood
printf("Populating neighborhood\n");
    for(i = 0; i < g_epMem->size - 1; i++)
    {
        // send Neighborhood*, Episode*, and Episode Neighborhood Metric to be processed
        // for a potential addition to the neighborhood
        if((n = calculateNValue(i)) >= 0)
        {
            addNeighbor(nbHd, g_epMem->array[i], n);
        }
    }//for

printf("Returning full neighborhood\n");

    return nbHd;
}//locateNearestNeighbors

/**
* calculateNValue
*
* This function takes an index into the vector of states (g_epMem)
* and calculates the neighborhood metric for the associated state
* 
* Pre-condition: The current episode (the final in g_epMem) has been temporarily set
*               to the action whose neighborhood we are populating.
*
* @arg currState An int that contains the index of the current state being processed
*
* @return int This is the n value that was calculated for the state
*               A value of -1 means the actions were not a match and should not be evaluated
*               as a potential new neighbor
*/
int calculateNValue(int currState)
{
    int i = 0; // i stores the neighborhood metric

    // Determine if the action matches the one that we are testing for. If not, then
    // the n value is zero and we can return
    if(currState < g_epMem->size - 1 && 
        ((Episode*)g_epMem->array[currState])->action == ((Episode*)g_epMem->array[g_epMem->size - 1])->action)
    {
        // Make sure the array indices are within bounds and
        // compare the respective episodes to see if we need to increment the neighborhood metric
        while(currState - 1 - i >= 0 && g_epMem->size - 2 - i >= 0 &&
        equalEpisodes(g_epMem->array[currState - 1 - i], g_epMem->array[g_epMem->size - 2 - i]))
        {
            i++;
        }//while
    }
    else
    {
        i = -1;
    }
//printf("Done calculating Nhood metric\n");
    
    return i;
}//calculateNValue

/**
 * calculateQValue
 *
 * This function will take a neighborhood and calculate its total Q value.
 * This is the average of the expected future discounted rewards of all the neighbors
 * in the neighborhood
 *
 * @arg nbHd A pointer to a neighborhood
 *
 * @return double The calculated Q value for the neighborhood and its action
 */
double calculateQValue(Neighborhood* nbHd)
{
    int i;
    double total = 0;
    // Iterate through neighborhood, stopping at numNeighbors in case it wasn't fully populated
    for(i = 0; i < nbHd->numNeighbors; i++)
    {
        total += ((Episode*)nbHd->episodes[i])->qValue;
    }
    
    // Divide by numNeighbors to get the average
    return (total / (double)nbHd->numNeighbors);
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
    nbHd->episodes  = (Episode**)malloc(k * sizeof(Episode*));
    nbHd->nValues   = (int*)malloc(k * sizeof(int));

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
    free(nbHd->episodes);   // free the array containing episode pointers
    free(nbHd->nValues);    // free the array containing ep n values
    free(nbHd);             // free the memory containing the neighborhood itself
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
//printf("Checking if potential neighbor is usable and numNeighbors is low\n");
    if(nbHd->numNeighbors > 0 &&
        n < nbHd->nValues[nbHd->numNeighbors - 1] &&
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
    // No need to sort a single item
    if(nbHd->numNeighbors <= 1) return;

    int i;
    Episode* tempEp;
    int tempN;
    //iterate from back to front
    for(i = nbHd->numNeighbors - 1; i > 0; i--)
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
        }
        else
        {
            // Must break because the new addition is not always the highest
            // rated neighborhood metric. When that is the case, if we continue
            // to traverse through the neighborhood then we can accidentally swap
            // the desired order or previously determined neighbors
            break;
        }
    }//for
}//sortNeighborhood

/**
 * getNeighbor
 *
 * Return the i-th neighbor in the neighborhood if available
 *
 * @arg i An int representing the desired index of neighbor
 * @return Episode* A pointer to the episode that is that neighbor: NULL if error occurs
 */
Episode* getNeighbor(Neighborhood* nbHd, int i)
{
    // Catch any out-of-bounds errors and neighbor unavailable
    if(i < 0 || i >= nbHd->kValue) return NULL;
    if(i >= nbHd->numNeighbors) return NULL;

    return (nbHd->episodes[i]);
}//getNeighbor

/**
 * displayNeighborhood
 *
 * Display the neighborhood 
 *
 * @arg nbHd A pointer to the neighborhood being printed
 */
void displayNeighborhood(Neighborhood* nbHd)
{
    int i;
    // introduce the neighborhood
    printf("========== The Neighborhood for Action: %s ==========\n\n", interpretCommand(nbHd->action));

    // Check if there were any neighbors found
    if(nbHd->numNeighbors == 0)
    {
        printf("There were no neighbors found for this neighborhood\n");
        return;
    }

    // Print the current state sequence. The length will match the highest neighborhood metric + 1
    printf("The current sequence being matched: ");
    displayNeighborSequence(g_epMem->array[g_epMem->size - 2], nbHd->nValues[0], TRUE);
    printf(" =>>> {%s,NA,NA}\n\n", interpretCommandShort(nbHd->action));

    
    // Display all the neighbors that were found
    for(i = 0; i < nbHd->numNeighbors; i++)
    {
        // introduce the current episode
        printf("=====>> The following episode has a Neighborhood Metric of: %i\n", nbHd->nValues[i]);
        displayEpisode(nbHd->episodes[i]);
        printf("Sequence leading to episode: ");
        displayNeighborSequence(nbHd->episodes[i], nbHd->nValues[i], FALSE);
        printf("\n\n");
    }//for
}//displayNeighborhood

/**
  * displayNeighborSequence
  *
  * This function takes a neighbor and its neighborhood metric and prints it to stdout
  * as a sequence for easy viewing
  *
  * @arg ep A pointer to the episode that is the neighbor
  * @arg n An integer that is the neighborhod metric for this neighbor
  * @arg isCurr A boolean indicating if this is the current state sequence
  */
void displayNeighborSequence(Episode* ep, int n, int isCurr)
{
    int i;
    for(i = n; i >= 0; i--)
    {
        if(i != n && i >= 1) printf(" ==> ");
        if(isCurr && i == 0 && n > 0) printf(" ==> ");
        if(!isCurr && i == 0) printf(" =>>> ");
        displayEpisodeShort(g_epMem->array[ep->now - i]);
    }
}//displayNeighborSequence

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

    // Once we have found the first goal we want to start creating neighborhoods
    if(g_goalCount > 0) populateNeighborhoods();

    // Determine the next command, possibility of random command
    if((rand() % 100) < g_randChance || // Probability of choosing random
        g_epMem->size < NUM_TO_MATCH || // or if list is too short
        g_goalCount == 0)   // or we have not yet found a goal
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
    int i, holder;
    double tempQ, topQ = 0;
    for(i = 0; i < LAST_MOBILE_CMD; i++)
    {
        if((tempQ = calculateQValue(g_neighborhoods->array[i])) > topQ)
        {
            topQ = tempQ;
            holder = i;
        }
    }

    // do action offset
    ep->action = holder + CMD_NO_OP;
    // return success
    return 0;
}// setCommand

/*
 * equalEpisodes
 *
 * This method takes two episodes and returns a boolean indicating if they are a match
 *
 * Equal if: percepts are equal
 *           actions are equal
 *           rewards are equal
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

    // Ensure episodes received same reward
    if(ep1->reward != ep2->reward) return FALSE;

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
    g_neighborhoods = newVector();

    int i;
    for(i = CMD_NO_OP; i <= LAST_MOBILE_CMD; i++)
    {
        addEntry(g_neighborhoods, initNeighborhood(i, K_NEAREST));
    }

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

    int i;
    // Free each neighborhood in the vector
    for(i = 0; i < LAST_MOBILE_CMD; i++)
    {
        destroyNeighborhood(g_neighborhoods->array[i]);
    }
    // Free the vector
    freeVector(g_neighborhoods);
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

