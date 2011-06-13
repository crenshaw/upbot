#include "soar.h"

/*
 * soar.c
 *
 * This file contains the reimplementation of Dr. Nuxoll's
 * Soar Eaters agent.
 *
 * Author: Zachary Paul Faltersack
 * Last updated: June 7, 2011
 */

#define DEBUGGING 1

// Global strings for printing to console
// Full commands
char* g_north    = "north";
char* g_east     = "east";
char* g_south    = "south";
char* g_west     = "west";
char* g_unknown  = "unknown";

// Condensed commands
char* g_northS   = "N";
char* g_eastS    = "E";
char* g_southS   = "S";
char* g_westS    = "W";
char* g_unknownS = "U";

// Keep track of goals
int g_goalCount = 0;                // Number of goals found so far

/**
 * tickWME
 *
 * This function is called at regular intervals and processes
 * the recent sensor data to determine the next action to take.
 *
 * @param wmeString A char array that defines an agent's state
 * @return int a command for the Roomba (negative is error)
 */
int tickWME(char* wmeString)
{
    g_statsMode = FALSE;

    EpisodeWME* ep;
    if (wmeString[0] == ':')
    {
        ep = createEpisodeWME(stringToWMES(wmeString));
    }
    else
    {
        ep = createEpisodeWME(roombaSensorsToWME(wmeString));
    }
    
    addEpisodeWME(ep);
    
    if(!g_statsMode) printf("++++++++++++++++++++++++++++++++++++++++++\n");
	if(!g_statsMode) printf("Number of goals found: %i\n", g_goalCount);
	if(!g_statsMode) printf("Current Score: %i\n", getScore(ep));
    if(!g_statsMode) printf("++++++++++++++++++++++++++++++++++++++++++\n");
    fflush(stdout);
    
    // Select the next command to be sent to the roomba
    if(!g_statsMode) printf("Choosing next command\n");
    chooseCommand(ep);
    if(!g_statsMode) printf("Command selected\n");
    if(!g_statsMode) fflush(stdout);
    
    return ep->cmd;
}//tickWME

/**
 * addEpisodeWME
 *
 * Add new episode to episodic memory.
 *
 * @arg episodes pointer to vector containing episodes
 * @arg item pointer to episode to be added
 * @return int status code (0 == success)
 */
int addEpisodeWME(EpisodeWME* item)
{
    return addEntry(g_epMem, item);
}//addEpisodeWME

/**
 * compareEpisodesWME
 *
 * Compare the sensor arrays of two episodes and return if they match or not
 *
 * @arg ep1 a pointer to an EpisodeWME
 * @arg ep2 a pointer to another EpisodeWME
 * @arg compCmd  TRUE indicates the comparison should include the cmd. FALSE
 *               indicates that only the sensor array should be compared
 * @return TRUE if the episodes match and FALSE otherwise
 */
int compareEpisodesWME(EpisodeWME* ep1, EpisodeWME* ep2, int compCmd)
{
    if(ep1->sensors->size != ep2->sensors->size) return FALSE;

    int i;
    for(i = 0; i < ep1->sensors->size; i++)
    {
        if(!compareWME(getEntry(ep1->sensors, i), getEntry(ep2->sensors, i))) return FALSE;
    }//for

    //Compare the commands if that's required
    if(compCmd && ep1->cmd != ep2->cmd) return FALSE;

    return TRUE;
}//compareEpisodesWME

/**
 * compareWME
 *
 * This function takes two WMEs and confirms that they contain
 * the same information.
 *
 * @param wme1 A pointer to a WME
 * @param wme2 A pointer to a WME
 * @return int Boolean value
 */
int compareWME(WME* wme1, WME* wme2)
{
    // This may be more complicated than need be.
    // I feel like it's likely we don't need to switch on the type
    // except for that one is a string which requires a function
    // for copmarison
    if(strcmp(wme1->attr, wme2->attr) == 0 &&
       wme1->type == wme2->type)
    {
        if(wme1->type == WME_INT && wme1->value.iVal == wme2->value.iVal) return TRUE;
        if(wme1->type == WME_CHAR && wme1->value.cVal == wme2->value.cVal) return TRUE;
        if(wme1->type == WME_DOUBLE && wme1->value.dVal == wme2->value.dVal) return TRUE;
        if(wme1->type == WME_STRING && strcmp(wme1->value.sVal, wme2->value.sVal) == 0) return TRUE;
    }
    return FALSE;
}//compareWME

/**
 * episodeContainsReward
 *
 * This routine determines whether a given episode contains a
 * goal.
 *
 * @param ep A pointer to an episode
 * @return TRUE if the entry contains a reward and FALSE otherwise
 */
int episodeContainsReward(EpisodeWME* ep)
{
    int reward = getReward(ep);
    if(reward == 0) return FALSE;
    else  return TRUE;
}//episodeContainsReward

/**
 * getReward
 *
 * Return the reward given to an episode converted to
 * a double.
 *
 * @param ep An EpisodeWME* indicating the episode
 * @return int The reward
 */
int getReward(EpisodeWME* ep)
{
    Vector* wmes = ep->sensors;
    int i;
    for(i = 0; i < wmes->size; i++)
    {
        WME* wme = (WME*)getEntry(wmes, i);
        if(strcmp("reward", wme->attr) == 0) return wme->value.iVal;
    }//for
    return 0;
}//getReward

/**
 * getScore
 *
 * Return the score at the Episode's time.
 *
 * @param ep An EpisodeWME* indicating the episode
 * @return int The score at that time
 */
int getScore(EpisodeWME* ep)
{
    Vector* wmes = ep->sensors;
    int i;
    for(i = 0; i < wmes->size; i++)
    {
        WME* wme = (WME*)getEntry(wmes, i);
        if(strcmp("score", wme->attr) == 0) return wme->value.iVal;
    }//for
    return 0;
}//getScore

/**
 * getNumSteps
 *
 * Return the score at the Episode's time.
 *
 * @param ep An EpisodeWME* indicating the episode
 * @return int The score at that time
 */
int getNumSteps(EpisodeWME* ep)
{
    Vector* wmes = ep->sensors;
    int i;
    for(i = 0; i < wmes->size; i++)
    {
        WME* wme = (WME*)getEntry(wmes, i);
        if(strcmp("steps", wme->attr) == 0) return wme->value.iVal;
    }//for
    return 0;
}//getNumSteps

/**
 * displayEpisodeWME
 *
 * Display the contents of an Episode struct in a verbose human readable format
 *
 * @arg ep a pointer to an episode
 */
void displayEpisodeWME(EpisodeWME* ep)
{
    Vector* sensors = ep->sensors;

    printf("\nSenses:");
    int i;
    for(i = 0; i < sensors->size; i++) displayWME(getEntry(sensors, i));

    printf("\nCommand: %s", interpretCommandShort(ep->cmd));
    printf("\nTime: %d\n\n", ep->now);
}//displayEpisodeWME

/**
 * displayWME
 *
 * Print a WME to console.
 *
 * @param wme A pointer to a WME to print
 */
void displayWME(WME* wme)
{
    printf("[%s:", wme->attr);
    if(wme->type == WME_INT)    printf("%d]",wme->value.iVal);
    if(wme->type == WME_CHAR)   printf("%c]",wme->value.cVal);
    if(wme->type == WME_DOUBLE) printf("%lf]",wme->value.dVal);
    if(wme->type == WME_STRING) printf("%s]",wme->value.sVal);
}//displayWME

/**
 * createEpisodeWME
 *
 * Takes a sensor data string and allocates space for episode
 * then parses the data and populates the episode and adds it
 * to the global episode list
 *
 * @arg sensorData char* filled with sensor information
 * @return Episode* a pointer to the newly added episode
 */
EpisodeWME* createEpisodeWME(Vector* wmes)
{
    // Timestamp to mark episodes
    static int timestamp = 0;

    if(wmes == NULL)
    {
        printf("WME vector in parse is null");
        return NULL;;
    }//if

    // Allocate space for episode and score
    EpisodeWME* ep = (EpisodeWME*) malloc(sizeof(EpisodeWME));

    // Set EpisodeWME sensors vector to our WME vector
    ep->sensors = wmes;
    ep->now = timestamp++;  // Just set it to our timestamp for now
    ep->cmd = MOVE_N;       // Default command for now

    if(episodeContainsReward(ep)) g_goalCount++;

    return ep;
}//createEpisodeWME

/**
 * freeEpisodeWME
 *
 * This function frees the memory associated with an EpisodeWME.
 *
 * @param ep A pointer to an EpisodeWME
 */
void freeEpisodeWME(EpisodeWME* ep)
{
    Vector* sensors = ep->sensors;
    int i;
    for(i = 0; i < sensors->size; i++)
    {
        freeWME(getEntry(sensors, i));
    }//for
    freeVector(sensors);
    free(ep);
}//freeEpisodeWME

/**
 * freeWME
 * 
 * This function frees the memory associated with a WME.
 *
 * @param wme A pointer to a WME.
 */
void freeWME(WME* wme)
{
    free(wme->attr);
    free(wme);
}//freeWME

/**
 * stringToWMES
 *
 * This function takes a string that contains a series of WMEs
 * and converts them into actual WMEs contained in a vector.
 *
 * @param senses A char* indicating a string defining several WMEs
 *
 * A WME is defined by 3 values: Attribute Name, Attribute Type, Attribute Value
 * These are delineated with a comma. Multiple WMEs are delineated with a semi-colon.
 * Types are a single char: i (integer), c (char), d (double), s (string {char*}).
 * {:name,type,value:}
 *
 * ex:		:size,i,25:name,s,john:open,i,1:pi,d,3.14159:
 *
 * Note: The first WME is preceded by a ':' and the final WME is succeded by a ':'
 *
 * @return Vector* A vector of WMEs derived from the sense string.
 *					NULL if error
 */
Vector* stringToWMES(char* senses)
{
    Vector* wmes = newVector();
    char delims[] = ":,";
    char* result = strtok(senses, delims);
    int i;
    WME* wme;
    while(result != NULL)
    {
        wme = (WME*)malloc(sizeof(WME));
        for(i = 0; i < 3; i++)
        {
            //switch on field index
            switch(i)
            {
                case 0:
                    wme->attr = (char*)malloc(sizeof(char) * (strlen(result) + 1));
                    sprintf(wme->attr, "%s", result);
                    break;
                case 1:
                    if(strcmp(result, "i") == 0) wme->type = WME_INT;
                    else if(strcmp(result, "c") == 0) wme->type = WME_CHAR;
                    else if(strcmp(result, "d") == 0) wme->type = WME_DOUBLE;
                    else if(strcmp(result, "s") == 0) wme->type = WME_STRING;
                    else return NULL;
                    break;
                case 2:
                    if(wme->type == WME_INT) wme->value.iVal = atoi(result);
                    else if(wme->type == WME_CHAR) wme->value.cVal = (char)result[0];
                    else if(wme->type == WME_DOUBLE) wme->value.dVal = atof(result);
                    else if(wme->type == WME_STRING)
                    {
                        wme->value.sVal = (char*)malloc(sizeof(char) * (strlen(result) + 1));
                        sprintf(wme->value.sVal, "%s", result);
                    }
                    else return NULL;
                    break;
            }//switch	
            result = strtok(NULL, delims);
        }//for
        addEntry(wmes, wme);
    }//while

    return wmes;
}//stringToWMES

/**
 * getNumMatches
 *
 * This function compares two episodes and returns
 * the number of WMEs in common.
 *
 * @param ep1 A pointer to an episode
 * @param ep2 A pointer to an episode
 * @return int The number of WMEs in common
 */
int getNumMatches(EpisodeWME* ep1, EpisodeWME* ep2)
{
    if(ep1->cmd != ep2->cmd) return -1;

    int i,j, count = 0;
    if(ep1->sensors->size > ep2->sensors->size)
    {
        for(i = 0; i < ep1->sensors->size; i++)
        {
            for(j = 0; j < ep2->sensors->size; j++)
            {
                if(compareWME(getEntry(ep1->sensors, i), getEntry(ep2->sensors, j))) count++;
            }//for
        }//for
    }//if
    else
    {
        for(i = 0; i < ep2->sensors->size; i++)
        {
            for(j = 0; j < ep1->sensors->size; j++)
            {
                if(compareWME(getEntry(ep2->sensors, i), getEntry(ep1->sensors, j))) count++;
            }//for
        }//for
    }//else

    return count;
}//getNumMatches


//--------------------------------------------------------------------------------
// MAIN FUNCTIONS FOR DETERMINING NEXT ACTION

/**
 * chooseCommand
 *
 * This function takes a pointer to a new episode and chooses a command
 * that should accompany this episode
 *
 * @arg ep a pointer to the most recent episode
 * @return int the command that was chosen
 */
int chooseCommand(EpisodeWME* ep)
{
    int i, j;       // indices for loops

    // seed rand if first time called
    static int needSeed = TRUE;
    if(needSeed == TRUE)
    {
        needSeed = FALSE;
        srand(time(NULL));
    }//if

    // Determine the next command, possibility of random command
    if(g_epMem->size < 2)
    {
        if(!g_statsMode) printf(" selecting random command \n");
        fflush(stdout);
        ep->cmd = (rand() % 4);
    }//if
    else
    {
        // loop on setCommand until a route is chosen 
        // that will lead to a successful action
        if(!g_statsMode) printf(" selecting command from Nux Soar \n");
        fflush(stdout);
        if(setCommand(ep) < 0)
        {
            if(!g_statsMode) printf("Failed to set a command, choosing random\n");
            ep->cmd = (rand() % 4);
        }//if
        fflush(stdout);
    }//else

    return ep->cmd;
}//chooseCommand

/**
 * setCommand
 *
 * Find the match scores for each of the available commands 
 * First
 *
 * This is done by determining the score for each available
 *  Default command is North 
 *
 * @arg ep pointer to new episode
 * @return int status code
 */
int setCommand(EpisodeWME* ep)
{       
    int i;
    int holder = 0;             // current command
    int status = -1;            // did we find a matching episode for the
                                // current command?
    double topScore=0.0, tempScore=0.0;

    int tie = 0;
    //For each possible command
    for(i = 0; i < 4; i++)
    {
        // Here we calculate the score for the current command
        tempScore = findDiscountedCommandScore(i);

        if(tempScore < 0)
        {
            if (!g_statsMode) printf("\t%s: no valid reward found\n", interpretCommandShort(i));
            tie++;
        }//if
        else
        {
            if (!g_statsMode) printf("\t%s score= %lf\n", interpretCommandShort(i), tempScore);

            if(tempScore > topScore)
            {
                topScore = tempScore;
                holder = i;
                status = 1;
            }//if
            if(tempScore == topScore) tie++;
        }//else
    }//for

    // do action offset
    ep->cmd = holder;
    if(tie == 4) status = -1;
    return status;
}//setCommand

/**
 * findDiscountedCommandScore
 *
 * This function takes a command and finds the reward
 * associated with it, applies the appropriate discount,
 * and returns the final score for the command.
 *
 * The initial score is determined by finding an episode where:
 *  1. The candidate command was chosen
 *  2. Episode occurred *before* most recent reward
 *  3. Episode sensing matches current sensing
 *
 * Then: Count the number of episodes to the subsequent reward
 *
 * The discount is applied by multiplying the reward times
 *  a discount factor raised to the power of the above number
 *
 * @param command An integer indicating the command to score
 * @return double The discounted score for the command
 */
double findDiscountedCommandScore(int command)
{
    int i,j, lastRewardIdx = findLastReward();

    if(!g_statsMode) printf("Searching for command: %s\n", interpretCommand(command));
    if(!g_statsMode) printf("\tLast reward at index: %d\n", lastRewardIdx);
    
    EpisodeWME* curr = (EpisodeWME*)getEntry(g_epMem, g_epMem->size - 1);
    curr->cmd = command;
    
    int topMatch = 0, tempMatch = 0, holder = -1;
    for(i = 0; i < lastRewardIdx; i++)
    {
        tempMatch = getNumMatches(getEntry(g_epMem, i), curr);
        if(tempMatch >= topMatch)
        {
            if(!g_statsMode) printf("\tNew top match score: %d\n", tempMatch);
            topMatch = tempMatch;
            holder = i;
        }//if
    }//for

    if(holder < 0) return -1.0;

    if(!g_statsMode) printf("\tState best matched at index: %d\n", holder);
    for(i = 1; i + holder <= lastRewardIdx; i++)
    {
        EpisodeWME* ep = (EpisodeWME*)getEntry(g_epMem, i + holder);
        if(episodeContainsReward(ep))
        {
            if(!g_statsMode) printf("\tNondiscounted reward: %i at %i steps from match\n", getReward(ep), i);
            if(!g_statsMode) printf("\tDiscount: %lf\n", pow(DISCOUNT, i));
            return (((double)getReward(ep)) * (double)pow(DISCOUNT, i));
        }//if
    }//for
    return -1.0;
}//findDiscountedCommandScore

/**
 * findLastReward
 *
 * This function returns the index of the last episode
 * containing a reward.
 *
 * @return int The index of the last episode with a reward
 *              Negative if none have been received
 */
int findLastReward()
{
    int i;
    for(i = g_epMem->size - 1; i > 0; i--)
        if(episodeContainsReward(getEntry(g_epMem, i))) return i;

    return -1;
}//findLastReward

/**
 * initSoar
 *
 * Initialize the episodic memory vector
 */
void initSoar()
{
    g_epMem         = newVector();

    g_connectToRoomba       = 0;
    g_statsMode             = 0;
}//initSoar

/**
 * endSoar
 *
 * Free the memory allocated for the NSM agent
 */
void endSoar() 
{
    int i;
    for(i = 0; i < g_epMem->size; i++)
    {
        freeEpisodeWME(getEntry(g_epMem, i));
    }//for
    freeVector(g_epMem);
}//endSoar

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
        case 0:
            return g_north;
            break;
        case 1:
            return g_south;
            break;
        case 2:
            return g_east;
            break;
        case 3:
            return g_west;
            break;
        default:
            return g_unknown;
            break;
    }//switch
}//interpretCommand

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
        case 0:
            return g_northS;
            break;
        case 1:
            return g_southS;
            break;
        case 2:
            return g_eastS;
            break;
        case 3:
            return g_westS;
            break;
        default:
            return g_unknownS;
            break;
    }//switch
}//interpretCommandShort

/**
 * roombaSensorsToWME
 *
 * This function takes the sensor string received from a Roomba
 * and converts it into the WME vector used by Ziggurat (in the
 * near future).
 *
 * @param sensorInput a char string with Roomba sensor data
 * @return Vector* A vector of WMEs created from the Roomba data
 *                 NULL if error
 */
Vector* roombaSensorsToWME(char* dataArr)
{
    int i;
    Vector* wmeVec = newVector();
    // set the episodes sensor values to the sensor data
    for(i = 0; i < NUM_SENSORS; i++)
    {
        // convert char to int and return error if not 0/1
        int bit = (dataArr[i] - '0');
        if ((bit < 0) || (bit > 1))
        {
            printf("%s", dataArr);
            return NULL;     
        }

        // Create the WME for the current sensor
        WME* wme = (WME*)malloc(sizeof(WME));
        wme->type = WME_INT;
        wme->value.iVal = bit; 
        // Here we will set the IR bit attr name to 'reward' to be consistent
        // with how we expect to mark S/F from other state definitions.
        // All other sensor attr names will be named by their index
        if(i == 0)
        {
            wme->attr = (char*)malloc(sizeof(char) * 7); // "reward\0"
            sprintf(wme->attr, "%s", "reward");
        }//if
        else
        {
            wme->attr = (char*)malloc(sizeof(char) * 2);// Account for null term.
            sprintf(wme->attr, "%i", i); // Just use the index of the sensor.
        }//else
        // Add the new WME to the vector
        addEntry(wmeVec, wme);
    }//for
    return wmeVec;
}//roombaSensorsToWME

