/**
 * clock.h
 * 
 * File contains functions needed for our robot
 * to implement timed automata
 *
 * @author Matt Holland
 * @since July 2013
 */

#include <signal.h>
#include <sys/time.h>
#include <time.h>

#ifdef _CLOCK_H_
#define _CLOCK_H_

/**
 * Function Prototypes, see clock.c for additional details on 
 * these functions
 */
static void signalrmHandler(int sig);
void setupClock(void);
void setClock(int sec, int usec);

int checkClock(void);
void resetClock(void);

#endif
