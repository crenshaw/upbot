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
* Last updated: October 13, 2010
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
#define DISCOUNT			1.0
#define MAX_LEN_LHS			1
#define MAX_LEVEL_DEPTH		4
#define MAX_ROUTE_LEN		15

// Collecting data for stats
#define STATS_MODE			0

#define DECREASE_RANDOM(randChance) if((randChance) > 10) { (randChance) -= 5;}

// Sensor data struct
typedef struct EpisodeStruct
{
	int 	sensors[NUM_SENSORS];
	int		now;
	int 	cmd;
} Episode;

// Rule struct
typedef struct RuleStruct
{
    Vector *epmem;              // the episodic memory for this rule.  This will
                                // contain either Episodes (for level 0)
                                // or sequences (for level 1+)
    int level;                  // what level is this rule?
	int index;                  // index into epmem where the rule's LHS ends
	int length;                 // number of entries in the LHS
	int freq;                   // number of times this rule has "matched" epmem
	int* overallFreq;           // number of times just the most recent sensor
                                // data has matched epmem. This is a pointer
                                // since the value is shared by a group of cousins
	int outcome;                // index to the outcome state *or* a flag
                                // indicating it doesn't exist yet
	int isIndeterminate;        // is this rule indeterminate?
	Vector* cousins;            // a pointer to a jointly held list of all
                                // "cousin" rules including itself.
                                // Non-percentage rules have a NULL list.
    int containsGoal;           // Does this rule contain a goal on the RHS?
    int containsStart;          // Does this rule contain a starting state on the LHS?
} Rule;

typedef struct RouteStruct
{
	Vector* route;				// The ordered list of actions stored as int indices
								// into ruleList
	int currSequence;			// Our current location in the execution of the
								// rules
	int currEpInRule;			// Index into LHS of the current rule
	int needsRecalc;			// If the data received while following the rules
								// differs from the expected value then it needs
								// to be recalculated and this informs when it 
								// happens
	int numRules;				// Number of rules in the route to goal
    int level;                  // The level of this route
} Route;

// Global variables for monitoring and connecting
int g_connectToRoomba;
int g_statsMode;

// This vector will contain all episodes received from Roomba
Vector* g_epMem;
Vector* g_actionRules;
Vector* g_sequenceRules;
Route*  g_route;

// Function declarations
extern void  simpleTest();
extern int   tick(char* sensorInput);
extern char* interpretCommand(int cmd);
char*    interpretCommandShort(int cmd);
int      interpretSensorsShort(int *sensors);
Episode* createEpisode(char* sensorData);
int      chooseCommand(Episode* ep);
int      setCommand(Episode* ep);
int      parseEpisode(Episode* parsedData, char* dataArr);
int      updateRules();
int      addEpisode(Vector* episodes, Episode* item);
int      addActionToSequence(Vector* sequence,  Rule* action);
int      addRule(Vector* rules, Rule* item, int checkRedundant);
void     addRuleToRoute(int ruleIdx);
int      planRoute(Episode* currEp);
int      takeNextStep(Episode* currEp);
Vector*  newPlan();
void     freePlan(Vector *plan);
int      setCommand2(Episode* ep);
int      nextStepIsValid();
void     displayRoute();
void     displayEpisode(Episode* ep);
void     displayRules(Vector* ruleList, Vector* episodeList);
void     displayRule(Rule* rule);
void     displaySequence(Vector* sequence);
void     displaySequences(Vector* sequences);
Vector*  containsSequence(Vector* sequenceList, Vector* seq, int ignoreSelf);
Rule*    ruleMatch(int action);
int      equalEpisodes(Episode* ep1, Episode* ep2, int isCurrMatch);
int      findTopMatch(double* scoreTable, double* indvScore, int command);
int      generateScoreTable(Vector* vector, double* score);
double   compareEpisodes(Episode* ep1, Episode* ep2, int isCurrMatch);
void     initSupervisor();
void     endSupervisor();

#endif // _SUPERVISOR_H_
