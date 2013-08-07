/*
 * File contains functions needed for our robot
 * to implement timed automata
 */

#include <signal.h>
#include <sys/time.h>
#include <time.h>


#ifdef CLOCK_H
#define CLOCK_H
//file includes the following functions
static void signalrmHandler(int sig);
void setupClock(void);
void setClock(int sec, int usec);

int checkClock(void);
void resetClock(void);

//lookup extern
//or define global variable 

// Global value to keep track of the alarm occurrence 
// Set nonzero on receipt of SIGALRM 
// TODO: Global?  Really?  Can this be better?
//static volatile sig_atomic_t gotAlarm; 

#endif
