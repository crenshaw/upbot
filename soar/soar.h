#ifndef _SOAR_H_
#define _SOAR_H_

/**
* soar.h
*
* This is the header file for a reimplementation of Dr. Nuxoll's
* Soar Eaters agent.
*
* Author: Zachary Paul Faltersack
* Last edit: June 7, 2011
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#include "vector.h"
#include "../communication/communication.h"

// Boolean values
#define TRUE                1
#define FALSE               0
#define SUCCESS             0

// Matching defines
#define FINAL_SCORE         500

// Defines for Q-Learning algorithm
#define DISCOUNT            0.9

#define BESTMATCHMIN        (0.75)

// Macros
#define DECREASE_RANDOM(randChance) if((randChance) > 4) { (randChance) *= .6;}

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

// Global variable for memory
Vector* g_epMem;

// Global variables for monitoring and connecting
int g_connectToRoomba;
int g_statsMode;

// Functions for WMEs
extern int   tickWME(char* wmeString); // DUPL
int          addEpisodeWME(EpisodeWME* item); // DUPL
int          compareEpisodesWME(EpisodeWME* ep1, EpisodeWME* ep2, int compCmd); // DUPL
int          compareWME(WME* wme1, WME* wme2);
EpisodeWME*  createEpisodeWME(Vector* wmes); // DUPL
void         displayEpisodeWME(EpisodeWME* ep); // DUPL
void         displayEpisodeWMEShort(EpisodeWME* ep); // DUPL
void         displayWME(WME* wme);
void         displayWMEList(Vector *sensors);  // DUPL (sort of) of interpretSensorsShort
int 		 episodeContainsReward(EpisodeWME* ep);
int          isCloseMatch(EpisodeWME* ep1, EpisodeWME* ep2);
int          getReward(EpisodeWME* ep);
int          getScore(EpisodeWME* ep);
void         freeEpisodeWME(EpisodeWME* ep);
void         freeWME(WME* wme);
Vector*		 stringToWMES(char* senseString);

// Old functions
// Function declarations
//-----------------------------------------
int          chooseCommand(EpisodeWME* ep);
int          setCommand(EpisodeWME* ep);
double       findDiscountedCommandScore(int command);
int          findLastReward();
void         initSoar();
void         endSoar();
char*        interpretCommand(int cmd);
char*        interpretCommandShort(int cmd);

#endif // _SOAR_H_
