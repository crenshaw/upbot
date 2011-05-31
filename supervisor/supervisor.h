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
 * Last updated: November 29, 2010
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "vector.h"
#include "../communication/communication.h"
#include "knearest.h"

// Boolean values
#define TRUE				1
#define FALSE				0

//Flags
#define USE_WMES 1              // use to activate EpisodeWME


// Function return codes
#define SUCCESS             0
#define NO_GOAL_IN_LEVEL    1    // used by initRoute
#define LEVEL_NOT_POPULATED 2    // used by initRoute, updatePlan
#define PLAN_NOT_FOUND      3    // used by initRoute
#define PLAN_ON_OUTCOME     4    // used by updatePlan

// Matching defines
#define NUM_TO_MATCH         (15)
#define NUM_GOALS_TO_FIND    (25)
#define DISCOUNT             (1.0)
#define MAX_LEN_LHS          (1)
#define MAX_LEVEL_DEPTH      (4)
#define MIN_LEVEL0_MATCH_LEN (2) // do not set this to anything less than 2!
#define K_NEAREST            (8)
#define MIN_NEIGHBORS        (1) //minimum number of neighbors required for match

//Planning defines
#define MAX_ROUTE_LEN        (50)
#define MAX_ROUTE_CANDS      (20) // maximum number of candidate routes to
                                   // examine before giving up

//Replacement defines
#define MAX_CONFIDENCE       (1.0)
#define MIN_CONFIDENCE       (0.0)
#define INIT_SELF_CONFIDENCE (0.5)
#define INIT_REPL_CONFIDENCE (0.05)
#define MAX_REPLS            (1)    // maximum number of replacements per plan
#define MAX_REPL_RISK        (0.0)  // a more flexible limitation to repls per
                                    // plan (min=0.0)


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
    int containsStart;        // Does this action contain a starting state on
                              // the LHS?
} Action;

typedef struct RouteStruct
{
    int level;                // The level of this route
    Vector* sequences;        // An ordered list of sequences that make up
                              // this route
    Vector* replSeq;          // If a replacement has been applied to the
                              // current sequence this contains a pointer to the
                              // modified sequence
    int currSeqIndex;         // The current sequence in this plan that is being executed
    int currActIndex;         // An index into the current sequence in this plan
                              // that indicates what action is currently being
                              // executed
    int needsRecalc;          // Indicates that this route is no longer valid
} Route;

typedef struct ReplacementStruct
{
    int     level;            // action level of this replacement "rule"
    Vector* original;         // vector of Actions to replace
    Action* replacement;      // single Action to replace original
    double  confidence;       // level of certainty in the reliablility of this
                              // replacment (0.0 ... 1.0)
} Replacement;

//Used to identify the agent's position as part of finding routes
typedef struct StartStruct
{
    Vector *seq;                // this sequence is a "position" though
    int level;                  // the level of the sequence
    int index;                  // the route generated from this start should
                                // start at the action specified by this index
} Start;

//Used for passing arbitrary information as agent's state
typedef struct WMEStruct {
    char* attr;                 // name of attribute
    int type;                   // var type of attribute
    union {
        int iVal;
        char cVal;
        double dVal;
        char* sVal;
    } value;                    // value of attribute
} WME;

// Episode struct for WMEs. Only difference is Vector WMEs instead of int[] sensors
typedef struct EpisodeWMEStruct
{
	Vector*	sensors;            // A Vector of WMEs
	int		now;
	int 	cmd;
} EpisodeWME;

// Global variables for monitoring and connecting
int g_connectToRoomba;
int g_statsMode;

// These vectors contain the entire episodic memory
Vector* g_epMem;
Vector* g_actions;
Vector* g_sequences;


//These variables have to do with creating and following plans
Vector* g_plan;           // a plan is a vector of N routes, 1 per level
Vector* g_replacements;   // list of all of our replacement "rules"
double  g_selfConfidence; // how confident the agent is in its current plan
Vector* g_activeRepls;    // these are replacements that have recently been
                          // applied and are awaiting reward/punishment
int     g_lastUpdateLevel;// the highest level that was updated in the last
                          // updateAll().  Used to aid findInterimStart().



// Function Prototypes
extern char* interpretCommand(int cmd);
extern void  simpleTest();
extern int   tick(char* sensorInput);

// Functions added to accomodate WMEs ----------
// Duplicate functions are commented DUPL. This means they are
// the same functionality and function name as a previously existing
// one, except modified to handle EpisodeWME instead of Episode
extern int   tickWME(Vector* wmes); // DUPL
int          addEpisodeWME(EpisodeWME* item); // DUPL
int          compareEpisodesWME(EpisodeWME* ep1, EpisodeWME* ep2, int compCmd); // DUPL
int          compareWME(WME* wme1, WME* wme2);
void         displayEpisodeWME(EpisodeWME* ep); // DUPL
void         displayWME(WME* wme);
EpisodeWME*  createEpisodeWME(Vector* wmes); // DUPL
void         freeEpisodeWME(EpisodeWME* ep);
void         freeWME(WME* wme);
Vector*      roombaSensorsToWME(char* sensorInput);
// Plan: Once everything functions, roomba calls tick(), which converts
//       to WME vector then calls tickWME()
//----------------------------------------------

Action*      actionMatch(int action);
int          addAction(Vector* actions, Action* item, int checkRedundant);
void         addActionToRoute(int actionIdx);
int          addActionToSequence(Vector* sequence,  Action* action);
int          addEpisode(Episode* item);
int          addSequenceAsEpisode(Vector* sequence);
void         applyReplacementToPlan(Vector *plan, Replacement *repl);
Vector*      applyReplacementToSequence(Vector* seq, Replacement* repl);
int          chooseCommand();
int          compareEpisodes(Episode* ep1, Episode* ep2, int compCmd);
Vector*      containsSequence(Vector* sequenceList, Vector* seq, int ignoreSelf);
Episode*     createEpisode(char* sensorData);
void         displayAction(Action* action);
void         displayActions(Vector* actionList);
void         displayEpisode(Episode* ep);
void         displayEpisodeShort(Episode* ep);
void         displayEpisodes(Vector* epList, int level);
void         displayPlan();
void         displayRoute(Route *, int recurse);
void         displaySequence(Vector* sequence);
void         displaySequenceShort(Vector* sequence);
void         displaySequences(Vector* sequences);
void         endSupervisor();
int 		 episodeContainsGoal(void* entry, int level);
Vector*      findInterimStart_KNN();
Vector*      findInterimStart_NO_KNN();
Vector*      findInterimStartPartialMatch_KNN(int *offset);
Vector*      findInterimStartPartialMatch_NO_KNN(int *offset);
Replacement* findBestReplacement();
//int          findTopMatch(double* scoreTable, double* indvScore, int command);
void         freePlan(Vector *plan);
void         freeRoute(Route *r);
//int          generateScoreTable(Vector* vector, double* score);
Route*       getTopRoute(Vector *plan);
Vector*      initPlan();
void         initRouteFromSequence(Route *route, Vector *seq);
void         initSupervisor();
char*        interpretCommandShort(int cmd);
int          interpretSensorsShort(int *sensors);
Vector*      newPlan();
int          nextStepIsValid();
int          parseEpisode(Episode* parsedData, char* dataArr);
void         penalizeAgent();
void         penalizeReplacements();
int          planNeedsRecalc(Vector *plan);
//int          planRoute(Episode* currEp);
void         rewardAgent();
void         rewardReplacements();
//int          setCommand(Episode* ep);
//int          setCommand2(Episode* ep);
//int          takeNextStep(Episode* currEp);
int          updateAll();

#endif //_SUPERVISOR_H_
