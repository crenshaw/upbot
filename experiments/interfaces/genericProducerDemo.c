/**
 * genericProducerDemo.c
 *
 * A simple demo for testing the basic functionality of acceptors and connectors.
 *
 * @author Tanya L. Crenshaw
 * @since July 2013
 */

#include <stdio.h>
#include "connector.h"

int main(int argc, char * argv[])
{

  // Check command line parameters.
  if(argc != 2)
    {
      printf("This is a command line program that requires the interface name you'd like to communicate on, e.g., en1 or wlan0\n");
      printf("usage: %s <interface name>\n", argv[0]);
    }

  serviceHandler sh;

#ifdef GUMSTIX
  printf("\n\nIt is my belief that this producer demo is running on a gumstix.\n\n");
#endif

#ifdef MAC
  printf("\n\nIt is my belief that this producer demo is running on a mac.\n\n");
#endif

  // Start up a data service, collector endpoint.
  servStart(SERV_DATA_SERVICE_COLLECTOR, argv[1], &sh);

  return 0;

}
