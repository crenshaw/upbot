/* Filename: filter1_1.h
 * Authors:	Ben Walker, Dustin Dalen
 * Created: 1/27/11
 * Last Modified: 4/19/11 by Ben Walker
 */

#ifndef filter1_1
#define filter1_1

#include <stdio.h>
#include <stdlib.h>
#include "vector.h"

//defines
#define NUM_SENSES 10
#define TRUE 1
#define FALSE 0
#define GOAL_BIT 0
#define CONFIDENT 0.80                  //what is the threshold for being confident 
                                        //about a bit's relevancy

//global variables
Vector * database;                      //the whole sha-bang
int maxDepth;                           //how deep we go recursively

typedef struct episode_struct			//a pair of an action and a state		
{							
	int action;                         //what the agent did before the state occured
	char * state;                       //10 bits of information about the world
} episode;


double confidence[NUM_SENSES];			//the array of confidence for each sense
double weight;                          //the total number of states recursed on

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



#endif
