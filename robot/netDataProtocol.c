/**
 * netDataProtocol.c
 *
 * contains the functions needed to package and unpackage sensor data
 * sent over the network.
 *
 * @author Matt Holland
 * @since August 2013
 */

#include "netDataProtocol.h"

/**
 * packageData(char* package, char* snsData, int state, int nextState,
 * 		int transition, time_t lastStateChange)
 *
 * package data the supervisor may need into a string (aka byte array).
 *
 * @param package: the package to place the data in
 * @param snsData: the sensor data from the robot
 * @param state: the initial state of the robot
 * @param nextState: the state the robot will be in after the 
 * 		event responder
 * @param transition: the transition that occured
 * @param lastStateChange: the last time the state changed
 *
 * TODO: avoid using literals with snsData
 * TODO: come up witha better name for snsData and lastStateChange 
 */
void packageData(char* package, char* snsData, int state, int nextState, int transition, time_t lastStateChange) {

	package[snsBumpLeft] = *snsData & SENSOR_BUMP_LEFT;
	package[snsBumpRight] = *snsData & SENSOR_BUMP_RIGHT;
	
	package[snsCliffLeft] = *(snsData+2);
	package[snsCliffFrontLeft] = *(snsData+3);
	package[snsCliffFrontRight] = *(snsData+4);
	package[snsCliffRight] = *(snsData+5);
	package[snsVWall] = *(snsData+6);

	*((int*)(package+stateInitial)) = state;
	*((int*)(package+stateFinal)) = nextState;
	*((int*)(package+transitionID)) = transition;

	*((int*)(package+clockLastSet)) = lastStateChange;

	time_t rawtime;
	time(&rawtime); 
	*((int*)(package+clockCurTime)) = rawtime;

	package[DATA_PACKAGE_SIZE-1] = '\0';
	
}

/**
 * getCharFromPackage(int position, char* package)
 *
 * gets a char(1 byte) from the package at the given location
 *
 * @param position: the location of the data in the package.
 * 		use the predfined constants found in netDataProtocol.h
 * 		for locations
 * @param package: the package to read from
 *
 * @return the char at the postion given
 */
char getCharFromPackage(int position, char* package) {
	return package[position];
}

/**
 * getIntFromPackage(int position, char* package)
 *
 * gets a int(4 bytes) from the package at the given location
 *
 * @param position: the location of the data in the package.
 * 		use the predfined constants found in netDataProtocol.h
 * 		for locations
 * @param package: the package to read from
 *
 * @return the int at the postion given
 */
int getIntFromPackage(int position, char* package) {
	return *((int *)(package+position));
}

/**
 * getTimeFromPackage(int position, char* package)
 *
 * gets a time_t(4 bytes) from the package at the given location
 *
 * @param position: the location of the data in the package.
 * 		use the predfined constants found in netDataProtocol.h
 * 		for locations
 * @param package: the package to read from
 *
 * @return the time_t at the postion given
 */
time_t getTimeFromPackage(int position, char* package) {
	return *((time_t *)(package+position));
}

/**
 * printPackage(char* package)
 *
 * prints all data stored in the package to stdio
 *
 * @param package: the package to read from
 */
void printPackage(char* package) {
	printf("\n\n");
	
	printf("Bump Left: %i\n",package[snsBumpLeft]);
	printf("Bump Right: %i\n",package[snsBumpRight]);
	
	printf("Cliff Left: %i\n",package[snsCliffLeft]);
	printf("Cliff Front Left: %i\n",package[snsCliffFrontLeft]);
	printf("Cliff Front Right: %i\n",package[snsCliffFrontRight]);
	printf("Cliff Right: %i\n",package[snsCliffRight]);
	printf("VWall: %i\n",package[snsVWall]);

	printf("Inital State %i\n",getIntFromPackage(stateInitial,package));
	printf("Final State %i\n", getIntFromPackage(stateFinal,package));
	printf("Transtion %i\n", getIntFromPackage(transitionID,package));  

	printf("Last State Change: %i\n",getTimeFromPackage(clockLastSet,package));
	printf("Cur Time %i\n",getIntFromPackage(clockCurTime,package));
}

