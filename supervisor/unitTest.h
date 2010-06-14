#ifndef _UNITTEST_H_
#define _UNITTEST_H_

#include <stdlib.h>
#include "supervisor.h"

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

// Sensor values
#define SNSR_OFF	0
#define SNSR_ON		1

// Number of bumps hit
#define NONE_HIT	0
#define BOTH_HIT	1
#define LEFT_HIT	2
#define RIGHT_HIT	3

int** initWorld();
void freeWorld(int** world);
char* unitTest();
char* unitTest2(int comand, int cleanup);
void displayWorld(int** world, int heading);

#endif
