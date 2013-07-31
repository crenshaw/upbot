/**
 * genericServiceDemo.c
 *
 * A simple demo for testing the basic functionality of services in
 * the UPBOT robotics testbed.  Others should use this demo for
 * guidance on how to utilize the serv*() functionality available in
 * services.[c,h].
 *
 * This half of the demo creates a data aggregator endpoint for the
 * data service and awaits connection from a data collector.
 *
 * @author Tanya L. Crenshaw
 * @since July 2013
 */

#include <stdio.h>
#include "acceptor.h"

void threadAccBroadcastService(serviceHandler * sh);

int main(int argc, char * argv[])
{
  // Check command line parameters.
  if(argc != 2)
    {
      printf("This is a command line program that requires the interface name you'd like to communicate on, e.g., en1 or wlan0\n");
      printf("usage: %s <interface name>\n", argv[0]);
    }

#ifdef GUMSTIX
  printf("\n\nIt is my belief that this demo is running on a gumstix.\n\n");
#endif

#ifdef MAC
  printf("\n\nIt is my belief that this demo is running on a mac.\n\n");
#endif

  serviceHandler sh;

  // Start up a data service, aggregator endpoint.
  servStart(SERV_DATA_SERVICE_AGGREGATOR, argv[1], &sh);

  printf("Serv start returns control flow to main\n");

  sleep(25);

  // TODO: Write a function called servCleanup to close 
  // up connections and 
  pthread_exit(NULL);


  return 0;
}




