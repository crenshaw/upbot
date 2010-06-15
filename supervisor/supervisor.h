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

// Collecting data for stats
#define STATS_MODE			1

#define DECREASE_RANDOM(randChance) if((randChance) > 5) { (randChance) -= 5;}

// Sensor data struct
typedef struct EpisodeStruct
{
	int 	sensors[NUM_SENSORS];
	time_t	now;
	int 	cmd;
} Episode;

typedef struct MilestoneStruct
{
	Vector* episodes;
	Vector* milestones;
	int name;
} Milestone;

// This vector will contain all episodes received from Roomba
Vector* g_episodeList;
Vector* g_milestoneList;

// Function declarations
extern int tick(char* sensorInput);
Episode* createEpisode(char* sensorData);
int chooseCommand(Episode* ep);
int setCommand(Episode* ep);
int parseEpisode(Episode* parsedData, char* dataArr);
int addEpisode(Vector* episodes, Episode* item);
void displayEpisode(Episode* ep);
int match(Vector* vector, int* score);
int compare(Episode* ep1, Episode* ep2);
void initSupervisor();
void endSupervisor();

#endif // _SUPERVISOR_H_
