#ifndef _EATERS_H_
#define _EATERS_H_

#include <stdlib.h>
#include "../communication/communication.h"

// Objects in the virtual world
#define V_WALL		(1)
#define V_AGENT		(2)
#define V_EMPTY		(0)
#define V_FOOD1		(5)
#define V_FOOD2		(10)

// Initial coords
#define X_INIT		(1)
#define Y_INIT		(1)

// Map dimensions
#define MAP_WIDTH			(18)
#define MAP_HEIGHT			(18)
#define MAP_PERCENT_WALLS 	(15)

// Commands
#define MOVE_N	(0)
#define MOVE_S	(1)
#define MOVE_E	(2)
#define MOVE_W	(3)

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

#endif
