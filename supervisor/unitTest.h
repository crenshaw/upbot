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

int** initWorld();
void freeWorld(int** world);
char* unitTest();
char* unitTest2(int comand);
void displayWorld(int** world, int heading);

#endif
