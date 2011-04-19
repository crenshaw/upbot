/* Filename: filter_KNN.c
 * Authors:	Ben Walker, Dustin Dalen
 * Created: 1/27/11
 * Last Modified: 4/19/11 by Ben Walker
 *
 * TODO *
 * Eventually to get more out of each run we should recurse on any action 
 * which is repeated.
 *
 **/
 

#include "filter_KNN.h"
#include "../communication/communication.h"
#include "supervisor.h"



/* printRun()
 * this function takes a vector of episodes and prints them to the screen
 * for debugging purposes
 * @param vector of episodes
 */
void printRun(Vector * run)
{
    printf("Printing out a run.\n");
    int idx;
    for(idx=0; idx<(run->size); ++idx)
    {
        episode * temp = getEntry(run,idx);
        printf("%d\t%s \n",temp->action, temp->state);
    }
    
}

/* 
 *receiveState() takes in a 10 bit sense and creates the appropiate episode
 *creating a new run if necessary. Then calls analyzeRuns() to select the important
 *bits and calls tick() with the thinned sense.
 * @param the 10 bit sense
 */
char * receiveState(char * input)
{
    //instance variables
    Vector * mostRecentRun = NULL;
    episode * lastEpisode = NULL;
    
    //is it a valid length sense
    if(strlen(input)<NUM_SENSES)
        return;
    
    //create the database if not already done
    if(database == NULL)
    {
        database = newVector();
        Vector * newRun = newVector();                                    //create the new run
        episode * newEpisode = createFilterEpisode(input, CMD_NO_OP);     //create first episode
        addEntry(newRun, newEpisode);
        addEntry(database,newRun);                                        //add the run to the database
    }
    //otherwise adding on
    else 
    {
        mostRecentRun = getEntry(database, database->size-1);
        lastEpisode = getEntry(mostRecentRun, mostRecentRun->size-1);
        
        //if last action was a song, make a new run
        if(lastEpisode->action == CMD_SONG)
        {
            Vector * newRun = newVector();                                    //create the new run
            episode * newEpisode = createFilterEpisode(input, CMD_NO_OP);     //create first episode
            addEntry(newRun, newEpisode);
            addEntry(database,newRun);                                        //add the run to the database
        }
        //continue adding to the run
        else
        {
            strncpy(lastEpisode->state,input,NUM_SENSES);       //add the senses to the most recent episode
            lastEpisode->state[NUM_SENSES] = '\0';              //making sure null terminated 
        }
    }
    
    //use probablility of importance to thin
    return thin(input,confidence);                        //note this function has sideeffects
    
}


/**
 *receiveAction() takes an integer command and stores that in the database
 *then passes the command to the environment
 *@param int command code.
 */
int receiveAction(int command)
{
    //get most recent run to work with
    Vector * mostRecentRun = getEntry(database, database->size-1);
    
    //add new episode to last position in last run
    //add in null which will be over written the next state input or kept if the action was CMD_SONG
    episode * newEpisode = createFilterEpisode(NULL, command);
    addEntry(mostRecentRun, newEpisode);
    
    if (command == CMD_SONG)
    {
        weight = 0;
        analyze(database, 0);
        int n;

        printf("\n New confidence levels \n");
        for(n = 0; n < NUM_SENSES; ++n)
        {
            printf("%d: %g\n", n, confidence[n]);
        }
        printf("\n");
    }

    //send command to the environment
    return command;
}

/*
 *thin() takes in a fresh state and the current confidence levels of all the bits. The goal bit's data is 
 * preserved. Also any data which coincides with a confidence level above the constant CONFIDENT are 
 * preserved. Anything below that threshold is set to a 0.
 * @param state, char[] of length NUM_SENSES which the robot took in
 * @param conf, double[] of length NUM_SENSEs which contains the confidence level for each bit
 */
char * thin(char * state, double * conf)
{
    int i;
    for(i=0;i<NUM_SENSES;i++)                   //go through each sense
    {
        if(i == GOAL_BIT);                          //case one it's the goal bit, dont' change

        else if(conf[i]<CONFIDENT)                  //case two it's not confident about this bit
            state[i] = '0';                        //reassign to zero
    }
    return state;
}

/**
 *createFilterEpisode() creates an episode for analysis.
 *@param char * bits sensed, int action taken resulting in senses received
 */
episode * createFilterEpisode(char * ourSenses, int ourAction)
{
	//allocate memory for a new episode.
	episode * newEpisode;
	newEpisode = malloc(sizeof(episode));
    newEpisode->state = malloc(NUM_SENSES+1);           //allocate space for 10 bit senses and '\0'
	
	//initialize the new episode.
    newEpisode->action = ourAction;
    if(ourSenses != NULL)                               //if we are passed an actual sense
    {
        strncpy(newEpisode->state,ourSenses,NUM_SENSES);
        newEpisode->state[NUM_SENSES] = '\0';           //null terminate
    }
	return newEpisode;
}


/* freeEpisode takes a ptr to an episode and frees the memory */
void freeEpisode(episode * theEpisode)
{
    free(theEpisode->state);
    free(theEpisode);
}


/* free a run of any length, given a ptr to that run */
void freeRun(Vector * theRun)
{
    int i;
    for(i=0; i<theRun->size; i++)
    {
        freeEpisode(getEntry(theRun, i));                //free each episode
    }
    freeVector(theRun);                                 //free the run itself
}


/** 
 * analyze() takes a vector of run and analyzes it.
 * analysis consists of:
 *      - sorting runs into a vector based on the action at a given depth
 *      - recursively comparing states of runs with similar actions at a given depth
 **/
void analyze(Vector * runsToAnalyze, int epDepth)
{
    //first thing to be done is reset the previous confidences
    int n;
    if (epDepth == 0)
    {
        for(n = 0; n < NUM_SENSES; ++n)
        {
            confidence[n] = 0;
        }
    }
    
	//temp variables
    Vector * currentRun;
    episode * currentEp;
	char * currentState;
	int currentAction;
    double currentConf;
    
	//array of confidence for a given sense
    double * conf = (double *)malloc(sizeof(double)*NUM_SENSES);                            
	
	//BASE CASE: less than 2 runs with common substrings of episodes 
    //       OR: we've reached the end of 2+ identical runs.
	if((int)runsToAnalyze->size < 2 || epDepth >= (int)runsToAnalyze->size)
	{
		//update confidence with default values.
		int i;
		for(i = 0; i < NUM_SENSES; ++i)
		{
            conf[i] = 0;
		}
        
        //set max depth
        maxDepth = epDepth;
        return;
	}
    
    //passed the base case: initialize useful variables.
    Vector * statesToAnalyze = newVector();             //vector of senses, correlating to runsOWTR
	Vector * runsOnWhichToRecurse = newVector();        //vector of run, sorted by action
    
    
	//for each run get episode: state and action
	int run_idx;
	for (run_idx = 0; run_idx < runsToAnalyze->size; ++run_idx)
    {
		//get run
        currentRun = getEntry(runsToAnalyze, run_idx);
        
        //send currentRun off for storing by action
        runsOnWhichToRecurse = storeRun(runsOnWhichToRecurse, currentRun, epDepth);
    }   
    
	//find most common action on which to recurse
	int mostActions = 0;                    //size of vector with the most actions in it
	int actionIdx;                          //index of the vector with ^^
	int m;
	for(m = 0; m < runsOnWhichToRecurse->size; ++m)
	{
		Vector * tempVector = getEntry(runsOnWhichToRecurse, m);
            
		if(tempVector->size > mostActions)
		{
			mostActions = tempVector->size;
			actionIdx = m;
		}
	}

	//prepare to recurse!!
    Vector * tempRunOfAction;
    if (runsOnWhichToRecurse->size != 0)
    {
        //get vector of run to recurse on
        tempRunOfAction = getEntry(runsOnWhichToRecurse, actionIdx);
        
        //update weight
        if(tempRunOfAction->size > 1)
        {
            weight += tempRunOfAction->size;
        }
    }

    
    //******************RECURSE HERE******************
        analyze(tempRunOfAction, ++epDepth);
    //************************************************
    
    
    //for each run get state so we can analyze them
	for (run_idx = 0; run_idx < tempRunOfAction->size; ++run_idx)
    {
        Vector * currentRun = getEntry(tempRunOfAction, run_idx);
        if(currentRun->size != 0)
        {           
            currentEp = getEntry(currentRun, epDepth);
        }
        
        //extract and store state
		currentState = currentEp->state;
		addEntry(statesToAnalyze, currentState);
    }
    
	//reset counter
	run_idx = 0;                //iterate through runs
    int p = 0;                  //iterate through senses
	int sum = 0;                //our sum
	
    //if episode is not at base case depth (one too far)
    if(epDepth < maxDepth)
    {
        //for each state in vector of states, sum 
        for (p = 0; p < NUM_SENSES; ++p)
        {
            //printf("Analyzing sense %d: \n", p);
            sum = 0;
            for(run_idx = 0; run_idx < statesToAnalyze->size; ++run_idx)
            {
                currentState = getEntry(statesToAnalyze, run_idx);
                
                //convert from char to int, sum it
                sum += currentState[p] - '0';
            }
            
            //average values for conf
            if(statesToAnalyze->size != 0)
            {   double dSum = (double) sum;
                double dStateSize = (double)statesToAnalyze->size;
                conf[p] = flip(dSum/dStateSize) * (dStateSize/weight);
            }
        }
    }
    
    //update confidence
    int q = 0;
    for (q = 0; q < NUM_SENSES; ++q)
	{
		confidence[q] += conf[q];
	}
}
/**
 * storeRun() stores a run into a vector based on the action at the given depth.
 **/
Vector * storeRun(Vector * runsToAnalyzeNext, Vector * runWithActionToStore, int depth)
{

    //function variables
	Vector * actionVector;
    episode * currentEp = (episode *)getEntry(runWithActionToStore, depth);
    int currentAction = currentEp->action;                      //current action to store
    int currentActionStored = FALSE;                            //stored the current action?

    
    //store first run regardless of the action taken
    if(runsToAnalyzeNext->size == 0)
    {
		//make new vector
		actionVector = newVector();
        //add it to new vector
        addEntry(actionVector, runWithActionToStore);
        //add new vector to analyzeNext
        addEntry(runsToAnalyzeNext, actionVector);
        //its been stored
        currentActionStored = TRUE;
        //get the heck outta here
        return runsToAnalyzeNext;
    }
    else //check to see if we've seen it yet.
    {
        //compare it to action at depth of every run seen thus far
        int i;
        for(i = 0; i < runsToAnalyzeNext->size; ++i)
        {
            //get action to compare to
            Vector * tempVectorOfRun = (Vector *)getEntry(runsToAnalyzeNext,i);
            Vector * tempRun = (Vector *)getEntry(tempVectorOfRun, 0);
            episode * tempEp = (episode *)getEntry(tempRun, depth);
            int actionSeen = tempEp->action;
            
            //store it only if we've seen it
            if (currentAction == actionSeen)
            {
                actionVector = getEntry(runsToAnalyzeNext, i);
                addEntry(actionVector, runWithActionToStore);
                currentActionStored = TRUE;
                return runsToAnalyzeNext;
            }
        }
    }
    
    //if the action hasn't been stored, we haven't seen it yet
    if (!currentActionStored)
    {
		actionVector = newVector();
        addEntry(actionVector, runWithActionToStore);
        addEntry(runsToAnalyzeNext, actionVector);
        currentActionStored = TRUE;
        return runsToAnalyzeNext;
    }
}


/**
 * flip() takes a double representing a probability (of getting a 1) and returns
 * it's probability of relevancy. (probability of consistency)
 * The range of the output is from 0.5 to 1.
 **/
double flip(double probability)
{
    if (probability < 0.5)
        return 1 - probability;
    else
        return probability;
    
}

