/**
 * supervisor.c
 *
 * A simple demo for testing the basic functionality of services in
 * the UPBOT robotics testbed.  This demo tests the acceptor side of
 * the functionality and will eventually evolve into the supervisor of
 * the UPBOT robotics system.
 *
 * Others should use this demo for * guidance on how to utilize the
 * serv*() functionality available in * services.[c,h].
 *
 * This half of the demo:
 * 
 * 1. Creates a data aggregator service endpoint for the data service
 * and awaits connection from a data collector.
 * 
 * 2. Creates a event:responder programmer service endpoint so that it
 * may issue commands to a robot.
 *
 * @author Tanya L. Crenshaw
 * @since July 2013
 */

#include <stdio.h>
#include "services.h"
#include "netDataProtocol.h"


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

  // Declare a serviceHandler for the data service (aggregator endpoint)
  // and the event:responder service (programmer endpoint).
  serviceHandler dsh;
  serviceHandler ersh;

  // The first step is to set the default values for the
  // serviceHandler.
  servHandlerSetDefaults(&dsh);
  servHandlerSetDefaults(&ersh);

  
  // Start up the service based on whether or not broadcast mode is on.
  int bcast = SERV_BROADCAST_OFF;
  if(argc == 3) bcast = SERV_BROADCAST_ON;    

  int status = -1;

  // Start up a data service, aggregator endpoint.  Affirm that
  // start was successful.
  if((status = servStart(SERV_DATA_SERVICE_AGGREGATOR, argv[1], bcast, &dsh)) != SERV_SUCCESS)
    {
      printf("Could not start service.\nStatus=%d\n", status);
      return EXIT_FAILURE;
    }
  
  // Start up an event:responder service endpoint.
  if((status = servStart(SERV_EVENT_RESPONDER_PROGRAMMER, argv[1], bcast, &ersh)) != SERV_SUCCESS)
    {
      printf("Could not start service.\nStatus=%d\n", status);
      return EXIT_FAILURE;
    }
  
  // I want to send a message to an event:responder robot endpoint, but
  // I cannot do so until the ersh says its ready.  Spin until the
  // call to servIsReady() returns TRUE.
  while( !servIsReady(&ersh) );


  // The event:responder service is ready.  Get the party started by
  // making the roomba start driving around autonomously.
  erWrite(&ersh, "go");
  printf("Sent 'go'\n");

  // Obtain data from the collector.  If the data shows that
  // we have seen a virtual wall, tell the robot to stop.
  char data[DPRO_PACKAGE_SIZE] = {'\0'};
  int vwall = 0;

  while(!vwall)
    {  
      if( dsRead(&dsh, data) == SERV_SUCCESS) 
	{
	  // Does the sensor data show that we've seen a virtual wall?
	  if(getCharFromPackage(DPRO_SNS_VWALL, data) == 1) {
	    erWrite(&ersh, "stop");
	    printf("Sent 'stop'\n");
	    vwall = 1;
	  }
	}
    }

  while( dsRead(&dsh, data) == SERV_SUCCESS);

  servStop(&dsh);
  
  printf("Stopped data service\n");

  pthread_exit(NULL);

  return 0;
}




