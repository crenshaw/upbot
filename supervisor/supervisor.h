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
#define CMD_NO_OP 		0x0
#define CMD_FORWARD		0x1
#define CMD_BACKWARD	0x2
#define CMD_LEFT		0x3
#define CMD_RIGHT		0x4
#define CMD_BLINK		0x5

// Boolean values
#define TRUE			1
#define FALSE			0

// Sensor definitions
#define SNSR_LEFT_BUMP	0
#define SNSR_RIGHT_BUMP	1
#define SNSR_MID_BUMP	2

// Matching defines
#define NUM_TO_MATCH	10

// Sensor data struct
typedef struct EpisodeStruct
{
	int 	sensors[8];
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

// Function declarations
int tick(Vector* vector, Episode *episode);
Episode * parseEpisode(char* dataArr);
int addEpisode(Vector* episodes, Episode* item);
void displayEpisode(Episode* ep);
int match(Vector* vector, int* score);
int compare(Episode* ep1, Episode* ep2);

#endif // _SUPERVISOR_H_
