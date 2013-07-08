/**
 * connector.c
 *
 *
 * Provides location transparency to application-level components. As
 * noted by Schmidt in "Applying Design Patterns to Flexibly Configure
 * Network Services, the connector initiates a connection with a
 * passive-mode endpoint and activates the appropriate serviceHandler.
 *
 * Once a connection is established, neither the application nor the
 * service utilize the acceptor until another connection must be
 * established.
 *
 * @author Tanya L. Crenshaw
 * @since July 2013
 */

#include "services.h"

/**
 * conInitiateConnection
 *
 * Based on D. Schmidt's "Acceptor-Connector" design pattern.
 * 
 * 1. Establish a connection with a passive-mode endpoint.
 * 
 * 2. Activate a service handler for this connection and return it.
 * 
 */
serviceHandler * conInitiateConnection(char * wellKnownIP, int port, serviceType type)
{
}
