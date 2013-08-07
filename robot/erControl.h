#include "eventresponder.h"
#include "myEventResponders.h"

#ifndef _ER_CONTROL_C_
#define _ER_CONTROL_C_


//file includes the following functions
void setEventResponder(char * erName, eventResponder* myER);
void cleanupER(eventResponder* myER);
void initalizeStopER(eventResponder* myER);


//selectNextER is not defined in this file but it is expected to be placer here
//void selectNextER(char * erName, eventresponder* myER);

#endif
