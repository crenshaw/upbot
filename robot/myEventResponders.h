/**
 * myEventResponders.h
 *
 * File contains the event responders that will be loaded on the robot
 * at the begining of run time. (there is the potential for the 
 * supervisor to add mroe during run time over the network). It also
 * contains the function selectNextER which allows the event responder
 * to be changed by use of comparison of Strings. The Stop ER is loaded
 * at the begining of runtime until the program is directed otherwise.
 *
 * @author Matt Holland
 * @since July 2013 
 */

#include "eventresponder.h"
#include "events.h"
#include "responders.h"

#ifndef MY_EVENT_RESPONDER_H
#define MY_EVENT_RESPODNER_H 

/**
 * Function headers. See myEventResponder.c for more details
 */

void initalizeWanderER(eventResponder* myER);
void initalizeStopER(eventResponder* myER);

void selectNextER(char * erName, eventResponder* myER);

#endif
