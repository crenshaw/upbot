/**
 * acceptor.h
 *
 * Provides the interfaces necessary to connect to remote services in
 * the UPBOT robotics system.  Entities that use the acceptor
 * establish a passive-mode endpoint and await connection from other
 * entities.
 *
 * @author Tanya L. Crenshaw
 * @since July 2013
 */

#include "services.h"

/**
 * Function prototypes.  See acceptor.c for details on
 * this/these functions.
 */
int accCreateConnection(int port, serviceHandler * sh);
int accCompleteConnection(int endpointHandler, serviceType type, serviceHandler * sh);
