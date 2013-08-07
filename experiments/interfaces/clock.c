


// Global value to keep track of the alarm occurrence 
// Set nonzero on receipt of SIGALRM 
// TODO: Global?  Really?  Can this be better?
static volatile sig_atomic_t gotAlarm = 0; 

//NOTE: removed the function to create eventResponders
//  I don't think it is needed with new data format
//  If we decide to be put back it would have needed
//  to be completly rewriten anyway

/*
   simple singal handler to set set the gotAlarm variable
   so to notify the user about alarms
 */
static void signalrmHandler(int sig)
{
  gotAlarm = 1;
}


void setupClock() {
  struct sigaction sa;        // Signal sets
  struct sigaction toggledsa;   

  // Initialize a random number generator to help with the
  // generation of random data.
  srand(time(NULL));  


  //TODO: move timer setup to a seperate function
  //with the intent of reducing clutter

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

