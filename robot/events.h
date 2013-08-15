/**
 * events.c 
 *
 * contains the events used by eventresponder.c
 *
 * An event is a function that is used to control check
 * the status of a robot
 * It returns a boolean based upon a roombas sensor data passed into it
 * 
 */

#include "roomba/roomba.h"

#ifndef _EVENTS_C_
#define _EVENTS_C_

/**
 * Function prototypes. For more detailed information see events.h
 */
int eventTrue(char * data);
int eventFalse(char * data);
int eventBump(char * data) ;
int eventBumpRight(char * data);
int eventBumpLeft(char * data);

int eventClock(char * data);

int eventVWall(char * data);

int eventCliffLeft(char * data);
int eventCliffFrontLeft(char * data);
int eventCliffFrontRight(char * data);
int eventCliffRight(char * data);

#endif
