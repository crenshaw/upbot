/**
 * genericServiceDemo.c
 *
 * A simple demo for testing the basic functionality of acceptors and connectors.
 *
 * This is a multi-threaded demo of the acceptor-side of the
 * connection.  There are three threads.
 * 
 * Thread 1 (threadAccCreateConnection): Creates the passive-mode
 * endpoint and waits for other entities to establish a connection.
 * 
 * Thread 2: Started by thread 1.  Broadcasts availability of service.
 * 
 * Thread 3: Handles clients.
 *
 * @author Tanya L. Crenshaw
 * @since July 2013
 */

#include <stdio.h>
#include "acceptor.h"

void threadAccBroadcastService(serviceHandler * sh);

int main(void)
{

  serviceHandler sh;

#ifdef GUMSTIX
  printf("\n\nIt is my belief that this demo is running on a gumstix.\n\n");
#endif

#ifdef MAC
  printf("\n\nIt is my belief that this demo is running on a mac.\n\n");
#endif

  int status = 0;

  pthread_t tAccBroadcast;
  pthread_t tAccWaitOnAccepts;

#ifdef DONOTCOMPILE
  // TODO: The sig-handler setup only needs to happen once, not once
  // per call to accCreateConnection!  In fact, as this code becomes
  // multi-threaded instead of multi-processed, I'm not even certain
  // if it is necessary.  

  // When the SIGCHLD signal occurs, the status of a child process has
  // changed and we need to call one of the wait functions to
  // determine what has happened. The default action for SIGCHLD is to
  // ignore the signal.  The following code alters that default so
  // that any SIGCHLD signal is handled by the signal handler,
  // accSigchldHandler() (see above).
  struct sigaction sa;
  sa.sa_handler = accSigchldHandler; 
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  
  // Set the action.
  if (sigaction(SIGCHLD, &sa, NULL) == -1) return ACC_SIGACTION_FAILURE;  // The action could not be set.
#endif

  // Create an acceptor-side passive-mode endpoint of communication
  // for a Data Aggregator Service.
  if((status = accCreateConnection("10005", SERV_DATA_SERVICE_AGGREGATOR, &sh)) != ACC_SUCCESS)
    {
      perror("accCreateConnection() failed.");
      printf("Status = %d\n", status);
    }

  // Just to mark progress, print the resulting service handler
  servHandlerPrint(&sh);  

  // Create a thread to broadcast the service.
  if(pthread_create(&tAccBroadcast, NULL, threadAccBroadcastService, &sh) != 0)
    {
      perror("pthread_create(), Accepter Broadcast:");
      return -1;
    }

  printf("Thread Created.  I shall make 10 broadcasts\n");


  // Wait for entities to connect to the service
  // For now, I've established an early proof of correctness
  // by testing it out with telnet.
  accAcceptConnection(&sh);  

  sleep(25);

  // Just to mark progress, print the resulting service handler
  // The broadcast handler should be set now.
  servHandlerPrint(&sh);  

  pthread_exit(NULL);

  // Also gotta close my connections.

  return 0;
}

/**
 * threadAccBroadcastService
 *
 */
void threadAccBroadcastService(serviceHandler * sh)
{

  // At this point, we can broadcast our existence to the world, so
  // that others may know about our fantastic service.  Start the
  // thread that will perform broadcasting.
  accBroadcastService(sh);

  /*
    From POSIX Thread tutorial:

    "All threads within a process share the same address
    space. Inter-thread communication is more efficient and in many
    cases, easier to use than inter-process communication."
  */

}
