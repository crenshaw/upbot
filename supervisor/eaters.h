#ifndef _EATERS_H_
#define _EATERS_H_

#include <stdlib.h>
#include "../communication/communication.h"

// Map dimensions
#define MAP_WIDTH			(18)
#define MAP_HEIGHT			(18)
#define MAP_PERCENT_WALLS 	(15)

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
char* setSenseString();

#endif // _EATERS_H_
