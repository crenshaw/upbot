/* Filename: filter_KNN.h
 * Authors:	Ben Walker, Dustin Dalen
 * Created: 1/27/11
 * Last Modified: 4/19/11 by Ben Walker
 */

#ifndef filter_KNN
#define filter_KNN

#include <stdio.h>
#include <stdlib.h>
#include "vector.h"

//defines
#define NUM_SENSES 10           // number of binary senses that the agent has
#define TRUE 1
#define FALSE 0
#define GOAL_BIT 0              // index of the goal bit in the senses array
#define MIN_CONF 0.70           //what is the minimum threshold for being confident 
                                //about a bit's relevancy
#define MAX_CONF 0.95           //what is the maximum threshold for being confident 
                                //about a bit's relevancy

//global variables
Vector * database;                      //the whole sha-bang
int maxDepth;                           //used to track how far the deepest is
                                        //that we've gone on recursive calls
double confidence[NUM_SENSES];			//the array of confidence for each sense    
double weight;                          //the total number of runs recursed on
                                        //so far.  Each time analyze is called,
                                        //this value is increased by the number
                                        //of runs analyzed


//a pair of an action and a state.
//A vector of these structs is used to store a single run to the goal.
//
//NOTE: Unlike the code in supervisor, the state is the state that *resulted*
//      from the action rather than the state that the agent was in when it took
//      the action.  As a result, the first action in any run is CMD_NO_OP.
//
typedef struct episode_struct			
{							
	int action;                         //what the agent did before the state occured
	char * state;                       //10 bits of information about the world
} episode;


// Function Prototypes
void printRun(Vector * run);
extern char * receiveState(char * input);
extern int receiveAction(int command);
char * thin(char * state, double * conf);
episode * createFilterEpisode(char * ourSenses, int ourAction);
void freeEpisode(episode * theEpisode);
void freeRun(Vector * theRun);
void analyze(Vector * runsToAnalyze, int epDepth);
Vector * storeRun(Vector * runsToAnalyze, Vector * runWithActionToStore, int depth);
double flip(double probability);



#endif //filter_KNN

