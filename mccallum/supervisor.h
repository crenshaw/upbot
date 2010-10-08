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
* Author: Zachary Paul Faltersack, Andrew Nuxoll, Brian Burns
* Last edit: October 4, 2010
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
#define SUCCESS             0

// Matching defines
#define NUM_TO_MATCH		15
#define NUM_GOALS_TO_FIND	50

// Defines for Q-Learning algorithm
#define DISCOUNT			1.0
#define LEARNING_RATE		1.0

// Collecting data for stats
#define STATS_MODE			0

#define DECREASE_RANDOM(randChance) if((randChance) > 10) { (randChance) -= 5;}

// Sensor data struct
typedef struct EpisodeStruct
{
	int		now;
	int 	sensors[NUM_SENSORS]; 	// Percept
	int 	action;					// Action
	double	reward;					// Reward
	double	qValue;					// Expected future discount reward
} Episode;


// Global variables for monitoring and connecting
int g_connectToRoomba;
int g_statsMode;

// This vector will contain all episodes received from Roomba
Vector* g_epMem;

// Function declarations
extern int   tick(char* sensorInput);
Episode* updateHistory(char* sensorData);
int      parseSensors(Episode* parsedData, char* dataArr);
int      addEpisode(Vector* episodes, Episode* item);
void     displayEpisode(Episode* ep);
//-----------------------------------------
// Functions to add for McCallum's algorithm
void	 updateLittleQ();
void	 locateKNearestNeighbors(int action);
void	 calculateQValue();
//-----------------------------------------
int      chooseCommand(Episode* ep);
int      setCommand(Episode* ep);
int      equalEpisodes(Episode* ep1, Episode* ep2);
void     initSupervisor();
void     endSupervisor();
extern char* interpretCommand(int cmd);
char*    interpretCommandShort(int cmd);
int      interpretSensorsShort(int *sensors);

#endif // _SUPERVISOR_H_
