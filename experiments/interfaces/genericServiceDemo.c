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

serviceHandler sh;

void threadAccCreateConnection(void);

int main(void)
{

#ifdef GUMSTIX
  printf("\n\nIt is my belief that this demo is running on a gumstix.\n\n");
#endif

#ifdef MAC
  printf("\n\nIt is my belief that this demo is running on a mac.\n\n");
#endif

  threadAccCreateConnection();

  // Print the resulting service handler
  servHandlerPrint(&sh);  


  return 0;

}

/**
 * threadAccCreateConnection
 *
 * This thread is in charge of creating a passive-mode endpoint and
 * waiting for other entities to establish a connection.
 */
void threadAccCreateConnection(void)
{

  // Create an acceptor-side passive-mode endpoint of communication
  // for a Data Aggregator Service.
  int status = 0;

  if((status = accCreateConnection("20", SERV_DATA_SERVICE_AGGREGATOR, &sh)) != ACC_SUCCESS)
    {
      perror("accCreateConnection() failed.");
      printf("Status = %d\n", status);
    }

  // At this point, we can broadcast our existence to the world, so
  // that others may know about our fantastic service.  Start the
  // thread that will perform broadcasting.
  accBroadcastService(&sh);

  /*
    From POSIX Thread tutorial:

    "All threads within a process share the same address
    space. Inter-thread communication is more efficient and in many
    cases, easier to use than inter-process communication."
  */

}
