/**
 * netDataProtocol.h
 *
 * contains the functions needed to package and unpackage sensor data
 * sent over the network.
 *
 *
 * @author Matt Holland
 * @since August 2013
 */


#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "roomba/roomba.h"

#ifndef _NET_DATA_PROTOCOL_H_
#define _NET_DATA_PROTOCOL_H_

/**
 * CONSTANT DEFINITIONS.
 */

//robot sensor data (1 byte each)
#define DPRO_SNS_BUMP_LEFT (0)
#define DPRO_SNS_BUMP_RIGHT (DPRO_SNS_BUMP_LEFT+1)

#define DPRO_SNS_CLIFF_LEFT (DPRO_SNS_BUMP_RIGHT+1)
#define DPRO_SNS_CLIFF_FRONT_LEFT (DPRO_SNS_CLIFF_LEFT+1)
#define DPRO_SNS_CLIFF_FRONT_RIGHT (DPRO_SNS_CLIFF_FRONT_LEFT+1)
#define DPRO_SNS_CLIFF_RIGHT (DPRO_SNS_CLIFF_LEFT+1)

#define DPRO_SNS_VWALL (DPRO_SNS_CLIFF_RIGHT+1)
#define DPRO_SNS_LAST DPRO_SNS_VWALL

//state before transition (int, 4 bytes)
#define DPRO_STATE_INITIAL (DPRO_SNS_LAST+1)
//state after transition (int, 4 bytes)
#define DPRO_STATE_FINAL (DPRO_STATE_INITIAL+4)
//the id of the transition occuring (int, 4 bytes)
#define DPRO_TRANSITION_ID (DPRO_STATE_FINAL+4)

//when the clock was last set in seconds (time_t, 4 bytes)
#define DPRO_CLOCK_LAST_SET (DPRO_TRANSITION_ID+4)
//the current time  (time_t, 4 bytes)
#define DPRO_CLOCK_CURTIME (DPRO_CLOCK_LAST_SET+4)

//size of the data package
#define DPRO_PACKAGE_SIZE (DPRO_CLOCK_CURTIME+6)

// The data service protocol ACK message and message
// size.
static char * dproAck = "ACK";
#define DPRO_ACK_SIZE 4

/**
 * Function prototypes. See netDataProtocol.c for more details
 */
void packageData(char* package, char* snsData, int state, int nextState, int transition, time_t lastStateChange);
char getCharFromPackage(int position, char* package);
int getIntFromPackage(int position, char* package);
time_t getTimeFromPackage(int position, char* package);
void printPackage(char* package);

#endif
