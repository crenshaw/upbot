/**
 * acceptor.c
 *
 *
 * Provides location transparency to service-level components. As
 * noted by Schmidt in "Applying Design Patterns to Flexibly Configure
 * Network Services, acceptors "initialize endpoints of communication
 * at a particular address and wait passively for the other endpoints
 * to connect with it."  That said, the acceptor also allows for the
 * flexibility for application-level to passively wait for services to
 * initiate the connection.
 * 
 * Once a connection is established, neither the application nor the
 * service utilize the acceptor until another connection must be
 * established.
 *
 * @author Tanya L. Crenshaw
 * @since July 2013
 *
 */


/**
 * accCreateConnection
 *
 * Create a passive-mode socket, bind it to a particular port number
 * on the calling host's IP address and listen passively for the
 * arrival of connection requests.  Since listening passively can be a
 * blocking call (i.e. accept()), it may be worthwhile to use this
 * function in a separate thread.
 *
 * NOTE: Similar to "createServer()" in serverUtility.c
 *
 * @param[in] port the port number to listen to.
 * 
 * @returns a handler for the endpoint.
 */
int accCreateConnection(int port)
{

}


/**
 * accCompleteConnection
 *
 * Based on D. Schmidt's "Acceptor-Connector" design pattern.
 *
 * 1. Use the passive-mode endpoint, endpointHandler, to create a
 * connected endpoint with a peer.
 *
 * 2. Create a service handler to process data requests arriving from
 * the peer.
 * 
 * 3. "Invoke the service handler's activation hook method which
 * allows the service handler to finish initializing itself."
 *
 * NOTE: Similar to "establishConnection()" in serverUtility.c 
 *
 * @param[in] endpointHandler a handler for the connection endpoint.
 * @param[in] type of service (see serviceType enum for possible
 * values).
 *
 * @returns a handler for the service connection.  Subsequent read and write
 * operations on this connection are parameterized by this handler 
*/
serviceHandler * accCompleteConnection(int endpointHandler, serviceType type)
{

}

