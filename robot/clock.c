/*
 * File contains functions needed for our robot
 * to implement timed automata
 */

#include <signal.h>
#include <sys/time.h>
#include <time.h>

//file includes the following functions
static void signalrmHandler(int sig);
void setupClock();
void setClock(int sec, int usec);



// Global value to keep track of the alarm occurrence 
// Set nonzero on receipt of SIGALRM 
// TODO: Global?  Really?  Can this be better?
static volatile sig_atomic_t gotAlarm = 0; 

/*
   simple singal handler to set set the gotAlarm variable
   so to notify the user about alarms
 */
static void signalrmHandler(int sig)
{
  gotAlarm = 1;
}

/*
 * function makes the necicary calls to setup our clock
 * this only needs to be called once at the beging of
 * the program execution
 */
void setupClock() {
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
    exit(EXIT_FAILURE);
  }
}

/*
 * Function will set the clock so that it will go off in
 * the amount of time indicated by its paramaters
 */
void setClock(int sec, int usec) {

  //TODO: figure out how to cancel old alarms

  struct itimerval itv;       // Specify when a timer should expire 

  // Initialize timer start time and period:
  // First, the period between now and the first timer interrupt 
  itv.it_value.tv_sec = sec; //seconds
  itv.it_value.tv_usec = usec; // microseconds

  // Second, the intervals between successive timer interrupts 
  itv.it_interval.tv_sec = 0; // seconds
  itv.it_interval.tv_usec = 0; // microseconds

  if (setitimer(ITIMER_REAL, &itv, NULL) == -1) {
    exit(EXIT_FAILURE);
  }

}

