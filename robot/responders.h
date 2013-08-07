#ifndef RESPONDERS_C
#define RESPONDERS_C

#include "roomba/roomba.h"

void respondStop(void);

void respondDriveLow(void);
void respondDriveMed(void); 
void respondDriveHigh(void);

void respondTurn(void); 

void respondLedBlink(void);
void respondLedRed(void);
void respondLedGreen(void);


#endif
