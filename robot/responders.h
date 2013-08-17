/**
 * responders.h
 *
 * A responder is the action preformed after an event has been 
 * triggered. All responders take no arguments and return nothing.
 *
 * @author Matt Holland
 * @since July 2013
 */



#include "roomba/roomba.h"
#include "eventresponder.h"

#ifndef _RESPONDERS_C_
#define _RESPONDERS_C_

/**
 * STRING_TO_RESPONDER(RESPONDER)
 *
 * Macro to write a simple conidtional statment to determine
 * if an argument responderIn has the same name as the responder
 * given to the macro. returns the responder if that is the case
 *
 */
#define STRING_TO_RESPONDER(RESPONDER) \
	if (strcmp(responderIn,#RESPONDER) == 0) { \
		return RESPONDER; \
	}


/**
 * RESPONDER_TO_STRING(RESPONDER)
 *
 * Macro to write a simple conidtional statment to determine
 * if an argument responderIn is the same responder as the one
 * given to the macro. Returns the responder name if that is the case
 *
 */
#define RESPONDER_TO_STRING(RESPONDER) \
	if (RESPONDER == responderIn) { \
		return #RESPONDER; \
	}


/**
 * Function prototypes. For more detailed information see responders.h
 */
responder* StringToResponder(char* responderIn);
char* ResponderToString(responder* responderIn);

void respondStop(void);

void respondDriveLow(void);
void respondDriveMed(void); 
void respondDriveHigh(void);

void respondTurnRandom(void); 

void respondLedBlink(void);
void respondLedRed(void);
void respondLedGreen(void);


#endif
