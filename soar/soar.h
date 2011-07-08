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
#include "../wme/wme.h"

#define STATS_MODE          0

// Boolean values
#define TRUE                1
#define FALSE               0
#define SUCCESS             0

// Matching defines
#define MAX_STEPS           1000
#define NUM_GOALS_TO_FIND   50

#define DISCOUNT            (0.75)

#define LOOK_AHEAD_N        0

// Global variable for memory
Vector* g_epMem;

// Global variables for monitoring and connecting
int g_connectToRoomba;
int g_statsMode;

// Tick and extra WME functions
extern int   tickWME(char* wmeString); // DUPL
int          addEpisodeWME(EpisodeWME* item); // DUPL

// Function for determining next command
//-----------------------------------------
int          chooseCommand(EpisodeWME* ep);
int          setCommand(EpisodeWME* ep);
double       findDiscountedCommandScore(int command);
int          findLastReward();
void         initSoar(int numCommands);
void         endSoar();
char*        interpretCommand(int cmd);
char*        interpretCommandShort(int cmd);

#endif // _SOAR_H_
