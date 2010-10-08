#include "supervisor.h"

/*
 * This file contains the code for the Supervisor. All the functions
 * that are needed for processing raw sensor data are contained in 
 * this file as well as those for determining new commands
 *
 * Author: Dr. Andrew Nuxoll, Zachary Paul Faltersack, Brian Burns
 *
 * Last updated: October 4, 2010
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
        ep->cmd = CMD_SONG;
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

    return ep->cmd;
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
	// If we have not experienced a single episode, we must
	// create the first one and add it to the beginning of 
	// our history
	if(g_epMem->size == 0)
	{
		Episode* ep = (Episode*) malloc(sizeof(Episode));
		addEpisode(g_epMem, ep);
	}
	else
	{
    	// Assign a pointer to the final episode in our history
    	Episode* ep = g_epMem->array[g_epMem->size - 1];
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
    Episode* ep = (Episode*) malloc(sizeof(Episode));

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
		episodeList->size < NUM_TO_MATCH || // or if list is too short
		g_goalCount == 0)	// or we have not yet found a goal
    {
        ep->cmd = (rand() % (LAST_MOBILE_CMD)) + CMD_NO_OP;
    }else
    {
        // loop on setCommand until a route is chosen 
		// that will lead to a successful cmd
        while(setCommand(ep))
        {
            if(g_statsMode == 0) printf("Failed to set a command\n");
        }
    }

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

    // generate partial score table
    generateScoreTable(episodeList, partialScoreTable);

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
                    //                                                      printf("Setting toggle, cmd: %s, tempDist: %i\n", interpretCommand(i), tempDist);
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
            /*                      double maxScore = NUM_TO_MATCH * NUM_SENSORS * 2;
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
    if(ep1->cmd != ep2->cmd) return FALSE;

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
        tempVal = scoreTable[i] + (((Episode*)(episodeList->array[i]))->cmd == command ? NUM_TO_MATCH : 0);

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
 * initSupervisor
 *
 * Initialize the Supervisor vectors
 * 
 */
void initSupervisor()
{
    g_epMem         = newVector();

    g_connectToRoomba       = 0;
    g_statsMode             = 0;
}//initSupervisor

/**
 * endSupervisor
 *
 * Free the memory allocated for the Supervisor
 */
void endSupervisor() 
{
    freeVector(g_epMem);
}//endSupervisor

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

/**
 * interpretCommandShort
 *
 * Return a char* with the string equivalent of a command
 * Use for printing to console
 *
 * @arg cmd Integer representing the command
 * @return char* Char array with command as string
 */
char* interpretCommandShort(int cmd)
{
    switch(cmd)
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

