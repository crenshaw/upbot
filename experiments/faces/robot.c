/**
 * robot.c
 *
 * A simple demo for testing the basic functionality of acceptors and
 * connectors.  This demo tests the connector side of the
 * functionality and is equivalent to the robot in the UPBOT robotics
 * system.  That is, in order for the nerves to start a Data Collector
 * service endpoint, it would need to make a similar call as seen
 * below.
 *
 * Others should use this demo for * guidance on how to utilize the
 * serv*() functionality available in * services.[c,h].
 *
 * @author Tanya L. Crenshaw
 * @since July 2013
 */

#include <stdio.h>
#include "services.h"

int main(int argc, char * argv[])
{

  // Check command line parameters.
  if( ! (argc == 2 || argc ==3 ))
    {
      printf("This is a command line program that requires the interface name you'd like to communicate on, e.g., en1 or wlan0.  It also has an optional third parameter to manually set a remote ip address of the entity to whom you want to connect.  If no ip is given, it will run in broadcast mode. \n");
      
      printf("usage: %s <interface name> <optional remote ip>\n", argv[0]);
      return 0;
    }

  serviceHandler dsh;
  serviceHandler ersh;

  // The first step is to set the default values for the
  // serviceHandlers.
  servHandlerSetDefaults(&dsh);
  servHandlerSetDefaults(&ersh);

  int bcast = SERV_BROADCAST_ON;
  int status = -1;

  // Manually set the remote ip if we have a third argument.
  if(argc == 3)
    {
      printf("...Executing program in manual mode using %s\n", argv[2]);
      servHandlerSetRemoteIP(&dsh, argv[2]);
      servHandlerSetRemoteIP(&ersh, argv[2]);
      bcast = SERV_BROADCAST_OFF;
    }

#ifdef DONOTCOMPILE
  if((status = servStart(SERV_DATA_SERVICE_COLLECTOR, argv[1], bcast, &dsh)) != SERV_SUCCESS)
    {
      printf("Could not start collector: %d\n", status);
      perror("Personal problems");
      return EXIT_FAILURE;
    }
#endif

  if((status = servStart(SERV_EVENT_RESPONDER_ROBOT, argv[1], bcast, &ersh)) != SERV_SUCCESS)
    {
      printf("Could not start programmer: %d\n", status);
      return EXIT_FAILURE;
    }
 
  while(1){
    char data[100];

    if(erRead(&ersh, data) == SERV_SUCCESS)
      {
	printf("%s\n", data);
      }
    
    else
      {
	printf("Nothing\n");
      }

    sleep(1);    
  }

  return 0;

}
