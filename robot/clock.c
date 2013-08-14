/**
 * clock.c
 *
 * File contains functions needed for our robot to implement
 * timed automata. Contains the function setupClock() which needs
 * to be called once at the begining of program exectuion. 
 * setClock() can be called whenever you want to set a clock event.
 * checkClock() can be used as frequently as you want and is used to
 * check the status of the clock. resetClock() is used to set the
 * alarm to its untriggered state. signalrmHandler() is called by
 * the system and should never need to be called by the user.
 *
 * @author: Matthew Holland
 * @since July 2013
 */

#include "clock.h"

//variable to keep track of if the clock has gone off
static volatile sig_atomic_t gotAlarm = 0;   

/**
 * signalrmHandler(int sig)
 * simple singal handler to set set the gotAlarm variable
 * so to notify the user about alarms
 */
static void signalrmHandler(int sig)
{
	gotAlarm = 1;
}

/**
 * setupClock()
 *
 * function makes the necessary calls to setup our clock
 * this only needs to be called once at the beging of
 * the program execution
 *
 * @return int indicating if it was successful
 * -1 indicates failure, 0 indicates success
 */
int setupClock(void) {
	struct sigaction sa;        // Signal sets
	struct sigaction toggledsa;   

	// Initialize a random number generator to help with the
	// generation of random data.
	srand(time(NULL));  

	// Initialize and empty a signal set
	sigemptyset(&sa.sa_mask);

	// Set the signal set.
	sa.sa_flags = 0;
	sa.sa_handler = signalrmHandler;

	// Update the signal set with the new flags and handler.
	if (sigaction(SIGALRM, &sa, NULL) == -1)
	{
		return -1;
	}

	return 0;
}

/**
 * setClock(int sec, int usec)
 *
 * Function will set the clock so that it will go off in
 * the amount of time indicated by its paramaters. It's
 * paramaters are cumalitive.
 *
 * @param sec: number of seconds until the clock will go off
 * @param usec: number of useconds until the clock will go off
 *
 * @return int to indicate if setting the alarm was successful
 * -1 indicates failure, 0 inidcates success
 *
 * TODO: figure out how to cancle old alarms
 */
int setClock(int sec, int usec) {

	struct itimerval itv;       // Specify when a timer should expire 

	// Initialize timer start time and period:
	// First, the period between now and the first timer interrupt 
	itv.it_value.tv_sec = sec; //seconds
	itv.it_value.tv_usec = usec; // microseconds

	// Second, the intervals between successive timer interrupts 
	itv.it_interval.tv_sec = 0; // seconds
	itv.it_interval.tv_usec = 0; // microseconds

	if (setitimer(ITIMER_REAL, &itv, NULL) == -1) {
		return -1;
	}

	return 0;
}

/**
 * checkClock()
 *
 * function for checking if the clock has gone off
 *
 * @return int to indicate if the clock has gone off
 * 1 indciates that it has, 0 indicates that is has not
 */
int checkClock(void) {
	return gotAlarm;
}

/**
 * resetClock()
 *
 * resets the clock so that it indicates that it has not gone off.
 * This function does not affect when the next clock has gone off
 */
void resetClock(void) {
	gotAlarm = 0;
}
