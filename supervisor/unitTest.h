#ifndef _UNITTEST_H_
#define _UNITTEST_H_

#include <stdlib.h>
#include "../communication/communication.h"

// Headings the Roomba can be pointed in
#define HDG_N	0
#define HDG_NE	1
#define HDG_E	2
#define HDG_SE	3
#define HDG_S	4
#define HDG_SW	5
#define HDG_W	6
#define HDG_NW	7

// Objects in the virtual world
#define V_HALLWAY	0
#define V_WALL		1
#define V_ROOMBA	2
#define V_GOAL		3
#define V_DROP		4

// Sensor values
#define SNSR_OFF	0
#define SNSR_ON		1

// Number of bumps hit
#define NONE_HIT	0
#define BOTH_HIT	1
#define LEFT_HIT	2
#define RIGHT_HIT	3

// Initial coords
#define X_INIT		1
#define Y_INIT		1
// Goal coords
#define X_GOAL		3
#define Y_GOAL		1

// Map dimensions
#define MAP_WIDTH	9
#define MAP_HEIGHT	7

// Booleans
#define TRUE		1
#define FALSE		0

int g_statsMode;

// Functions headers
void loadWorld();
void initWorld();
void freeWorld();
void resetWorld();
void displayWorld();
char* unitTest(int comand, int needCleanup);
char* doMove(int command);
char* setSensorString(int IR, int rCliff, int rCliffFront, 
					 int lCliffFront, int lCliff, int caster, 
					 int lDrop, int rDrop, int lBump, int rBump,
					 int abort);
int bumpSensor(int north, int east);

#endif
