/**
 * netERProtocol.c
 *
 * This contains the packing and unpacking functions for event
 * responders into a portable data structure. The idea is that this
 * will allow event responders to be sent across the network.
 *
 * @author Matt Holland
 * @since August 2013
 */


#include <stdlib.h>
#include <string.h>

#include "eventresponder.h"
#include "myEventResponders.h"

#ifndef _NET_ER_PROTOCOL_H_
#define _NET_ER_PROTOCOL_H_

/**
 * Pre-Procesor Constants
 */

#define INT_SIZE (4)
#define ERPRO_PACKAGE_IDENTIFIER 33

 /**
  * Function Headers
	*/
int packageEventResponder(eventResponder* er, char** package);
void unpackageEventResponder(int size,eventResponder* er, char* package);

#endif
