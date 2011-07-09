#ifndef _EATERS_H_
#define _EATERS_H_

#include <stdlib.h>
#include <stdio.h>
#include "../communication/communication.h"

// Map dimensions
#define MAP_WIDTH			(18)
#define MAP_HEIGHT			(18)
#define MAP_PERCENT_WALLS 	(20)
#define MAX_WALL_LEN        (7)
#define WALLTYPE_NONE       (0) // no interior walls
#define WALLTYPE_RANDOM     (1) // randomly generated as spec'd by above defines
#define WALLTYPE_STATIC     (2) // a fixed pattern

// Map columns containing special food
#define SFOODC1     2
#define SFOODC2     5
#define SFOODC3     8
#define SFOODC4     11
#define SFOODC5     14

// Booleans
#define TRUE		1
#define FALSE		0

int g_statsMode;

// Functions headers
void initWorld(int firstInit);
void freeWorld();
void displayWorld();
char* unitTest(int comand, int needCleanup);
char* doMove(int command);
char* setSenseString(int command);

#endif // _EATERS_H_
