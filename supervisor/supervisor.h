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
* Author: Zachary Paul Faltersack
* Last edit: 27/5/10
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

// Matching defines
#define NUM_TO_MATCH		15
#define NUM_GOALS_TO_FIND	75
#define DISCOUNT			1.0
#define MAX_LEN_LHS			4
#define MAX_META_DEPTH		4
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
                                // contain either Episodes (for base rules)
                                // or Rules (for meta-rules)
    int isBaseRule;             // Is this a base-rule or a meta-rule?
	int index;                  // index into epmem where the rule's LHS ends
	int length;                 // number of entries in the LHS
	int freq;                   // number of times this rule has "matched" epmem
	int* overallFreq;           // number of times just the most recent sensor
                                // data has matched epmem. This is a pointer
                                // since the value is shared by a group of cousins
	int outcome;                // index to the outcome state *or* a flag
                                // indicating it doesn't exist yet
	int isPercentageRule;       // is this rule a percentage rule?
	Vector* cousins;            // a pointer to a jointly held list of all
                                // "cousin" rules including itself.
                                // Non-percentage rules have a NULL list.
    int containsGoal;           // Does this rule contain a goal on the RHS?
} Rule;

typedef struct RouteStruct
{
	Vector* route;				// The ordered list of rules(stored as int indices
								// into ruleList
	int currRule;				// Where we currently are in the execution of the
								// rules
	int currEpInRule;			// Index into LHS of the current rule
	int needsRecalc;			// If the data received while following the rules
								// differs from the expected value then it needs
								// to be recalculated and this informs when it 
								// happens
	int numRules;				// Number of rules in the route to goal
} Route;

// Global variables for monitoring and connecting
int g_connectToRoomba;
int g_statsMode;

// This vector will contain all episodes received from Roomba
Vector* g_epMem;
Vector* g_semMem;
Route* g_route;

// Function declarations
extern int tick(char* sensorInput);
extern char* interpretCommand(int cmd);
char* interpretCommandShort(int cmd);
int interpretSensorsShort(int *sensors);
Episode* createEpisode(char* sensorData);
int chooseCommand(Episode* ep);
int setCommand(Episode* ep);
int parseEpisode(Episode* parsedData, char* dataArr);
int updateRules();
int addEpisode(Vector* episodes, Episode* item);
int addRule(Vector* rules, Rule* item, int checkRedundant);
void addRuleToRoute(int ruleIdx);
int planRoute(Episode* currEp);
int takeNextStep(Episode* currEp);
int setCommand2(Episode* ep);
int nextStepIsValid();
void displayRoute();
void displayEpisode(Episode* ep);
void displayRules();
void displayRule(Rule* rule);
Rule* ruleMatch(int action);
int equalEpisodes(Episode* ep1, Episode* ep2, int isCurrMatch);
int findTopMatch(double* scoreTable, double* indvScore, int command);
int generateScoreTable(Vector* vector, double* score);
double compareEpisodes(Episode* ep1, Episode* ep2, int isCurrMatch);
void initSupervisor();
void endSupervisor();

#endif // _SUPERVISOR_H_
