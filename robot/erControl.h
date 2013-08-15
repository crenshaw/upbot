/**
 * erControl.h
 *
 * erControl contains the functions used to set and change the
 * event responder.
 *
 * @author Matt Holland
 * @since July 2013
 */

#include "eventresponder.h"
#include "myEventResponders.h"

#ifndef _ER_CONTROL_C_
#define _ER_CONTROL_C_


/**
 * Function Prototpes, to see more details look in erControl.c
 */
void setEventResponder(char * erName, eventResponder* myER);
void cleanupER(eventResponder* myER);
void initalizeStopER(eventResponder* myER);

#endif
