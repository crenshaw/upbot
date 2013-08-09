#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "../roomba/roomba.h"

#ifndef _NET_DATA_PROTOCOL_H_
#define _NET_DATA_PROTOCOL_H_

//robot sensor data (1 byte each)
//avoiding masks because I want the future users to easily be able to
//modify the data protocol. I don't think the data savings is worth it
#define snsBumpLeft (0)
#define snsBumpRight (snsBumpLeft+1)

#define snsCliffLeft (snsBumpRight+1)
#define snsCliffFrontLeft (snsCliffLeft+1)
#define snsCliffFrontRight (snsCliffFrontLeft+1)
#define snsCliffRight (snsCliffFrontRight+1)

#define snsVWall (snsCliffRight+1)
#define snsLast snsVWall

//state before transition (int, 4 bytes)
#define stateInitial (snsLast+1)
//state after transition (int, 4 bytes)
#define stateFinal (stateInitial+4)
//the id of the transition occuring (int, 4 bytes)
#define transitionID (stateFinal+4)

//when the clock was last set in seconds (time_t, 4 bytes)
#define clockLastSet (transitionID+4)
//the current time  (time_t, 4 bytes)
#define clockCurTime (clockLastSet+4)

//size of the data package
#define DATA_PACKAGE_SIZE (clockCurTime+6)


void packageData(char* package, char* snsData, int state, int nextState, int transition, time_t lastStateChange);
char getCharFromPackage(int position, char* package);
int getIntFromPackage(int position, char* package);
time_t getTimeFromPackage(int position, char* package);
void printPackage(char* package);

#endif
