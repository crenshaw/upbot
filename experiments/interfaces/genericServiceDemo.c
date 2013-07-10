/**
 * genericServiceDemo.c
 *
 * A simple demo for testing the basic functionality of acceptors and connectors
 *
 * @author Tanya L. Crenshaw
 * @since July 2013
 */

#include <stdio.h>
#include "acceptor.h"

int main(void)
{

  serviceHandler sh;
  int status = 0;

  if((status = accCreateConnection("20", &sh)) != ACC_SUCCESS)
    {
      perror("accCreateConnection() failed.");
      printf("Status = %d\n", status);
    }

  // Print the resulting service handler
  servHandlerPrint(&sh);

  // Listen.


  // Establish a connection.

  

  return 0;

}



