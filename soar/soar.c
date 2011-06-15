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
char* g_no_op    = "no operation";
char* g_north    = "north";
char* g_east     = "east";
char* g_south    = "south";
char* g_west     = "west";
char* g_unknown  = "unknown";

// Condensed commands
char* g_no_opS   = "O";
char* g_northS   = "N";
char* g_eastS    = "E";
char* g_southS   = "S";
char* g_westS    = "W";
char* g_unknownS = "U";

// Keep track of goals
int g_goalCount = 0;                // Number of goals found so far
int g_CMD_COUNT = 0;

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
    int found;
    if(!g_statsMode) printf("++++++++++++++++++++++++++++++++++++++++++\n");
	if(!g_statsMode) printf("Number of goals found: %i\n", g_goalCount);
	if(!g_statsMode) printf("Current Score: %i\n", getINTValWME(ep, "score", &found));
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
        ep->cmd = ((rand() % g_CMD_COUNT) + CMD_NO_OP);
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
            ep->cmd = ((rand() % g_CMD_COUNT) + CMD_NO_OP);
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
    int holder = CMD_NO_OP;     // current command
    int status = -1;            // did we find a matching episode for the
                                // current command?
    double topScore=0.0, tempScore=0.0;

    int found;
    //For each possible command
    for(i = CMD_NO_OP; i <= g_CMD_COUNT; i++)
    {
        if((i == CMD_MOVE_N && getINTValWME(ep, "UM", &found) == V_WALL) ||
           (i == CMD_MOVE_S && getINTValWME(ep, "LM", &found) == V_WALL) ||
           (i == CMD_MOVE_E && getINTValWME(ep, "RT", &found) == V_WALL) ||
           (i == CMD_MOVE_W && getINTValWME(ep, "LT", &found) == V_WALL))
            continue;
        // Here we calculate the score for the current command
        tempScore = findDiscountedCommandScore(i);

        if(tempScore < 0)
        {
            if (!g_statsMode) printf("\t%s: no valid reward found\n", interpretCommandShort(i));
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
        }//else
    }//for

    // do action offset
    ep->cmd = holder;
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
    int i,j, lastRewardIdx = g_epMem->size - 1; //findLastReward();

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
            topMatch = tempMatch;
            holder = i;
        }//if
    }//for

    if(holder < 0) return -1.0;

    if(!g_statsMode) printf("\tState best matched at index: %d\n", holder);
#if LOOK_AHEAD_N
    for(i = 1; i <= LOOK_AHEAD_N && i + holder <= lastRewardIdx; i++)
#else
    for(i = 1; i + holder <= lastRewardIdx; i++)
#endif
    {
        EpisodeWME* ep = (EpisodeWME*)getEntry(g_epMem, i + holder);

        if(ep == curr) 
        {
            if(!g_statsMode) printf("\tNo subsequent reward found\n");
            return 0;
        }

        if(episodeContainsReward(ep))
        {
            int found;
            if(!g_statsMode) printf("\tNondiscounted reward: %i at %i steps from match\n", getINTValWME(ep, "reward", &found), i);
            if(!g_statsMode) printf("\tDiscount: %lf\n", pow(DISCOUNT, i));
            return (((double)getINTValWME(ep, "reward", &found)) * (double)pow(DISCOUNT, i));
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
 *
 * @param numCommands An integer indicating the number
 *      of available commands in the current environment
 */
void initSoar(int numCommands)
{
    g_CMD_COUNT             = numCommands;
    g_epMem                 = newVector();
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
        case CMD_NO_OP:
            return g_no_op;
            break;
        case CMD_MOVE_N:
            return g_north;
            break;
        case CMD_MOVE_S:
            return g_south;
            break;
        case CMD_MOVE_E:
            return g_east;
            break;
        case CMD_MOVE_W:
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
        case CMD_NO_OP:
            return g_no_opS;
            break;
        case CMD_MOVE_N:
            return g_northS;
            break;
        case CMD_MOVE_S:
            return g_southS;
            break;
        case CMD_MOVE_E:
            return g_eastS;
            break;
        case CMD_MOVE_W:
            return g_westS;
            break;
        default:
            return g_unknownS;
            break;
    }//switch
}//interpretCommandShort


