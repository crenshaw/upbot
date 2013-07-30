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

int main(void)
{

  serviceHandler sh;

#ifdef GUMSTIX
  printf("\n\nIt is my belief that this producer demo is running on a gumstix.\n\n");
#endif

#ifdef MAC
  printf("\n\nIt is my belief that this producer demo is running on a mac.\n\n");
#endif

  // Start up a data service, collector endpoint.
  servStart(SERV_DATA_SERVICE_COLLECTOR, &sh);

  

  return 0;

}
