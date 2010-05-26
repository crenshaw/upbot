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
* Last edit: 22/5/10
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "vector.h"

// Command definitions
#define CMD_NO_OP 			0x0
#define CMD_FORWARD			0x1
#define CMD_BACKWARD		0x2
#define CMD_LEFT			0x3
#define CMD_RIGHT			0x4
#define CMD_BLINK			0x5
#define CMD_ADJUST_LEFT		0x6
#define CMD_ADJUST_RIGHT	0x7
#define NUM_COMMANDS		0x8	// Always make sure this is at the end

// Sensor Data Indices
#define SNSR_IR				0x0
#define SNSR_CLIFF_RIGHT	0x1
#define SNSR_CLIFF_F_RIGHT	0x2
#define SNSR_CLIFF_F_LEFT	0x3
#define SNSR_CLIFF_LEFT		0x4
#define SNSR_CASTER			0x5
#define SNSR_DROP_LEFT		0x6
#define SNSR_DROP_RIGHT		0x7
#define SNSR_BUMP_LEFT		0x8
#define SNSR_BUMP_RIGHT		0x9
#define NUM_SENSORS			0xA	// Always make sure this is at the end

// Boolean values
#define TRUE			1
#define FALSE			0

// Matching defines
#define NUM_TO_MATCH	10

// Sensor data struct
typedef struct EpisodeStruct
{
	int 	sensors[NUM_SENSORS];
	time_t	now;
	int 	aborted;
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


// Function declarations
int tick(Episode *episode);
int parseEpisode(Episode* parsedData, char* dataArr);
int addEpisode(Vector* episodes, Episode* item);
void displayEpisode(Episode* ep);
int match(Vector* vector, int* score);
int compare(Episode* ep1, Episode* ep2);

#endif // _SUPERVISOR_H_
