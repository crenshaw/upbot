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

#include "connector.h"
#include "services.h"


/**
 * conInitiateConnection
 *
 * Based on D. Schmidt's "Acceptor-Connector" design pattern.
 * 
 * 1. Establish a connection with a passive-mode endpoint.
 * 
 * 2. Activate a service handler for this connection and populate the parameter sh.
 *
 * @param[in] wellKnownIP a well-known IP for the passive-mode
 * endpoint (i.e., we are not implementing a discovery mechanism at
 * this time).
 *
 * @param[in] port the port number to connect to.
 * 
 * @param[in] type the type of service at the passive-mode endpoint.
 * 
 * @param[out] sh the serviceHandler that will be fully populated by
 * this call.  Subsequent read and write operations on this connection
 * are parameterized by this handler.
 *
 * @returns an indication of success or failure.
 * 
 */
int conInitiateConnection(char * wellKnownIP, int port, serviceType type, serviceHandler * sh)
{
}
