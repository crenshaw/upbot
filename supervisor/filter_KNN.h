/* Filename: filter1_1.h
 * Authors:	Ben Walker, Dustin Dalen
 * Created: 1/27/11
 * Last Modified: 2/8/11 by Dustin Dalen
 */

#ifndef filter1_1
#define filter1_1

#include <stdio.h>
#include <stdlib.h>
#include "vector.h"

//defines
#define NUM_SENSES 10
#define LENGTH_COMMAND_NAME 25          //no longer needed
#define END_OF_RUN "song"               //no longer needed
#define TRUE 1
#define FALSE 0
#define GOAL_BIT 0
#define CONFIDENT 0.6                   //what is the threshold for being confident 
                                        //about a bit's relevancy

//global variables
Vector * database;                      //the whole sha-bang
int maxDepth;                                   //how deep we go

typedef struct episode					//A pair of an action and a state		
{							
	int action;                         //what the agent did before the state occured
	char * state;                       //10 bits of information about the world
} episode;


double confidence[NUM_SENSES];				//the array of confidence for a given sense
double weight;

//prototypes
episode * createFilterEpisode(char * ourSenses, int ourAction);
void freeEpisode(episode * theEpisode);
void freeRun(Vector * theRun);
void analyze(Vector * runsToAnalyze, int epDepth);
Vector * storeRun(Vector * runsToAnalyze, Vector * runWithActionToStore, int depth);
double flip(double probability);
char * thin(char * state, double * conf);
extern char * receiveState(char * input);
extern int receiveAction(int command);

#endif
