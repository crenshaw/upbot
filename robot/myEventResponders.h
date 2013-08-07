#ifndef MY_EVENT_RESPONDER_H
#define MY_EVENT_RESPODNER_H 

#include "eventresponder.h"
#include "events.h"
#include "responders.h"

void initalizeWanderER(eventResponder* myER);
void selectNextER(char * erName, eventResponder* myER);

#endif
