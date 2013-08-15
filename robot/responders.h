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

#ifndef _RESPONDERS_C_
#define _RESPONDERS_C_

/**
 * Function prototypes. For more details look in responders.c
 */

void respondStop(void);

void respondDriveLow(void);
void respondDriveMed(void); 
void respondDriveHigh(void);

void respondTurn(void); 

void respondLedBlink(void);
void respondLedRed(void);
void respondLedGreen(void);


#endif
