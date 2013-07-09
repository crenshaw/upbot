/**
 * connector.h
 *
 * Provides the interfaces necessary to connect to remote services in
 * the UPBOT robotics system.  Entities that use the connector
 * actively seek out a passive-mode endpoint and establish a
 * connection.
 *
 * @author Tanya L. Crenshaw
 * @since July 2013
 */

#include "services.h"


/**
 * Function prototypes.  See connector.c for details on
 * this/these functions.
 */
int conInitiateConnection(char * wellKnownIP, int port, serviceType type, serviceHandler * sh);
