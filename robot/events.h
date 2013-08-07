#ifndef EVENTS_C
#define EVENTS_C

#include "roomba/roomba.h"

int eventTrue(char * data);
int eventFalse(char * data);
int eventBump(char * data) ;
int eventBumpRight(char * data);
int eventBumpLeft(char * data);
int eventNotBump(char * data);
int eventAlarm(char * data);


#endif
