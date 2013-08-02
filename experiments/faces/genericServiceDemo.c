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
  if(! (argc == 2 || argc == 3))
    {
      printf("This is a command line program that requires the interface name you'd like to communicate on, e.g., en1 or wlan0.  Optionally, if you give it three parameters, it will turn broadcast mode on.  Otherwise, with only two parameters it will run with broadcast mode off.\n");

      printf("usage: %s <interface name> <optional broadcast on>\n", argv[0]);
      return 0;
    }

#ifdef GUMSTIX
  printf("\n\nIt is my belief that this demo is running on a gumstix.\n\n");
#endif

  serviceHandler sh;

  // The first step is to set the default values for the
  // serviceHandler.
  servHandlerSetDefaults(&sh);

  // Start up the service based on whether or not broadcast mode is on.
  if(argc == 3) {

    // Start up a data service, aggregator endpoint.
    servStart(SERV_DATA_SERVICE_AGGREGATOR, argv[1], SERV_BROADCAST_ON, &sh);
  }

  else {

    // Start up a data service, aggregator endpoint.
    servStart(SERV_DATA_SERVICE_AGGREGATOR, argv[1], SERV_BROADCAST_OFF, &sh);

  }

  printf("Serv start returns control flow to main\n");

  // TODO: Write a function called servClose to close 
  // up connections and such.
  sleep(25);

  pthread_exit(NULL);


  return 0;
}




