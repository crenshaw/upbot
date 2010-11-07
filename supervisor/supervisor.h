#ifndef _SUPERVISOR_H_
#define _SUPERVISOR_H_

/**
 * supervisor.h
 *
 * This is an implementation of the Supervisor for a Roomba.
 * It is a client that connects through a socket to the Roomba
 * and receives data packets which it then processes. It then 
 * returns a command to the Roomba based on the results of the 
 * data.
 *
 * Authors:      Zachary Paul Faltersack, Dr. Andrew Nuxoll, Brian Burns
 * Last updated: November 3, 2010
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "vector.h"
#include "../communication/communication.h"

// Boolean values
#define TRUE				1
#define FALSE				0

// Function return codes
#define SUCCESS             0
#define NO_GOAL_IN_LEVEL    1    // used by initRoute
#define LEVEL_NOT_POPULATED 2    // used by initRoute

// Matching defines
#define NUM_TO_MATCH		15
#define NUM_GOALS_TO_FIND	50
#define DISCOUNT            1.0
#define MAX_LEN_LHS	        1
#define MAX_LEVEL_DEPTH		4
#define MAX_ROUTE_LEN		15

// Collecting data for stats
#define STATS_MODE		0

#define DECREASE_RANDOM(randChance) if((randChance) > 10) { (randChance) -= 5;}

// Sensor data struct
typedef struct EpisodeStruct
{
	int 	sensors[NUM_SENSORS];
	int		now;
	int 	cmd;
} Episode;

// Action struct
typedef struct ActionStruct
{
    Vector *epmem;            // the episodic memory for this action.  This will
                              // contain either Episodes (for level 0)
                              // or sequences (for level 1+)
    int  level;               // what level is this action?
    int  index;               // index into epmem where the action's LHS ends
    int  length;              // number of entries in the LHS
    int  freq;                // number of times this action has "matched" epmem
    int* overallFreq;         // number of times just the most recent sensor
                              // data has matched epmem. This is a pointer
                              // since the value is shared by a group of cousins
    int  outcome;             // index to the outcome state *or* a flag
                              // indicating it doesn't exist yet
    int  isIndeterminate;     // is this action indeterminate?
    Vector* cousins;          // a pointer to a jointly held list of all
                              // "cousin" actions including itself.
                              // Non-indeterminate actions have a NULL list.
    int containsGoal;         // Does this action contain a goal on the RHS?
    int containsStart;        // Does this action contain a starting state on the LHS?
} Action;

typedef struct RouteStruct
{
    int level;                // The level of this route
    Vector* sequences;        // An ordered list of sequences that make up
                              // this route  
    int currSeqIndex;         // The current sequence in this plan that is being executed
    int currActIndex;         // An index into the current sequence in this plan
                              // that indicates what action is currently being
                              // executed
    int needsRecalc;          // Indicates that this route is no longer valid
                              // (probably because environmental input is no
                              // longer matching)
} Route;

typedef struct ReplacementStruct
{
    int     level;            // action level of this replacement "rule"
    Vector* original;         // vector of Actions to replace
    Action* replacement;      // single Action to replace original
    double  confidence;       // level of certainty in the reliablility of this
                              // replacment (0.0 ... 1.0)
} Replacement;

// Global variables for monitoring and connecting
int g_connectToRoomba;
int g_statsMode;

// This vector will contain all episodes received from Roomba
Vector* g_epMem;
Vector* g_actions;
Vector* g_sequences;
Vector* g_plan;         // a plan is a vector of N routes, 1 per level
Vector* g_replacements; // list of all of our replacement "rules"

// Function Prototypes
extern char* interpretCommand(int cmd);
extern void  simpleTest();
extern int   tick(char* sensorInput);

Action*  actionMatch(int action);
int      addAction(Vector* actions, Action* item, int checkRedundant);
void     addActionToRoute(int actionIdx);
int      addActionToSequence(Vector* sequence,  Action* action);
int      addEpisode(Vector* episodes, Episode* item);
int      chooseCommand();
int      compareEpisodes(Episode* ep1, Episode* ep2, int compCmd);
Vector*  containsSequence(Vector* sequenceList, Vector* seq, int ignoreSelf);
Episode* createEpisode(char* sensorData);
void     displayAction(Action* action);
void     displayActions(Vector* actionList, Vector* episodeList);
void     displayEpisode(Episode* ep);
void     displayPlan();
void     displayRoute(Route *, int recurse);
void     displaySequence(Vector* sequence);
void     displaySequences(Vector* sequences);
Vector*  doReplacement(Vector* seq, Replacement* r);
void     endSupervisor();
Vector*  findReplacements();
int      findTopMatch(double* scoreTable, double* indvScore, int command);
void     freePlan(Vector *plan);
void     freeRoute(Route *r);
int      generateScoreTable(Vector* vector, double* score);
Vector*  initPlan();
void     initRouteFromSequence(Route *route, Vector *seq);
void     initSupervisor();
char*    interpretCommandShort(int cmd);
int      interpretSensorsShort(int *sensors);
Vector*  newPlan();
int      nextStepIsValid();
int      parseEpisode(Episode* parsedData, char* dataArr);
int      planRoute(Episode* currEp);
int      setCommand(Episode* ep);
int      setCommand2(Episode* ep);
int      takeNextStep(Episode* currEp);
int      updateAll();

#endif //_SUPERVISOR_H_
