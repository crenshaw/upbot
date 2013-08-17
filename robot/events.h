/**
 * events.c 
 *
 * contains the events used by eventresponder.c
 *
 * An event is a function that is used to control check
 * the status of a robot
 *
 * It returns a boolean based upon a roombas sensor data passed into it
 * 
 */

#include "roomba/roomba.h"
#include "eventresponder.h"

#ifndef _EVENTS_C_
#define _EVENTS_C_

/**
 * STRING_TO_EVENT(EVENT)
 *
 * Macro to write a simple conidtional statment to determine
 * if an argument responderIn has the same name as the event
 * given to the macro. returns the event if that is the case
 *
 */
#define STRING_TO_EVENT(EVENT) \
	if (strcmp(eventIn,#EVENT) == 0) { \
		return EVENT; \
	}


/**
 * EVENT_TO_STRING(EVENT)
 *
 * Macro to write a simple conidtional statment to determine
 * if an argument eventIn is the same event as the one
 * given to the macro. Returns the event name if that is the case
 *
 */
#define EVENT_TO_STRING(EVENT) \
	if (EVENT == eventIn) { \
		return #EVENT; \
	}


/**
 * Function prototypes. For more detailed information see events.h
 */
eventPredicate* StringToEvent(char* eventIn);
char* EventToString(eventPredicate* eventIn);

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
