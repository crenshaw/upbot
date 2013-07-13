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

#include <stdlib.h>
#include <stddef.h>
#include "acceptor.h"
#include "services.h"

/**
 * accSigchldHandler
 *
 * This function is an interrupt-driven function which 
 * reaps dead children that are forked by the server.
 *
 * This function is local to acceptor.c and should not be made
 * available to other source.
 */
void accSigchldHandler(int s)
{
  // The waitpid() call waits for a child process to reach a certain
  // state.  The WNOHANG option means that the waitpid() call will
  // return immediately if no child has exited.
  while(waitpid(-1, NULL, WNOHANG) > 0);
}

/**
 * accGetInAddr
 *
 * Get sockaddr, IPv4 or IPv6.
 * 
 * This function is local to acceptor.c and should not be made
 * available to other source.
 *
 */
void * accGetInAddr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


/**
 * accCreateConnection
 *
 * Create a passive-mode socket, bind it to a particular port number
 * on the calling host's IP address and listen passively for the
 * arrival of connection requests.  Since listening passively can be a
 * blocking call (i.e. accept()), it may be worthwhile to use this
 * function in a separate thread.
 *
 * To reiterate: This function blocks until a connection is
 * established!
 * 
 * Failure Modes: 
 *
 * If either of the parameters are NULL, the functions
 * returns ACC_BAD_PORT or ACC_NULL_SH.
 *
 * If the socket options cannot be set, the function returns
 * ACC_SOCK_OPT_FAILURE to indicate an error.
 * 
 * If the socket cannot be bound to the address, the function returns
 * ACC_SOCK_BIND_FAILURE to indicate an error.
 *
 * If the socket cannot listen(), the function returns
 * ACC_SOCK_LISTEN_FAILURE.
 *
 * If the child reaper cannot be set using setaction(), the function returns
 * ACC_SIGACTION_FAILURE.
 *
 * Adapted from: "Advance Programming in the UNIX Environment."  page
 * 501 as well as "Beej's Guide to Network Programming."
 *
 * For more details on any of the errors that may occur as the result
 * of this call, users may take advantage of perror() as the socket library
 * sets errno subsequent to its calls.
 *
 * @param[in] port the port number to listen to.
 * 
 * @param[in] type of service (see serviceType enum for possible
 * values).
 *
 * @param[out] sh the serviceHandler that will be partially populated
 * by this call; if successful, its ep field is the handler for the
 * endpoint.
 * 
 * @returns an indication of success or failure.
 */
int accCreateConnection(char * port, serviceType type, serviceHandler * sh)
{
  int s;           // socket handler
  int optval = 1;  // boolean option value for the SO_REUSEADDR option.

  // Was this function handed a well-formed string and service
  // handler?  If not, leave and indicate an error.
  if(port == NULL) return ACC_BAD_PORT;
  if(sh == NULL) return ACC_NULL_SH;

  // Set the default values for the serviceHandler.
  servHandlerSetDefaults(sh);
  servHandlerSetPort(sh, port);

  // Defining the fields for socket structs is challenging.  The fields depend on the
  // address, type of socket, and communication protocol.  This function uses getaddrinfo()
  // to aid in defining the struct socket fields.  This function fills a struct of
  // type addrinfo.
  struct addrinfo hints;
  struct addrinfo * servinfo, *p; 
  
  // Initialize the hints structure based on what little we care about
  // in terms of the socket.  The goal is to listen in on host's IP
  // address on the port provided by the 'port' parameter.
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;          // don't care if its IPv4 or IPv6.
  hints.ai_socktype = SOCK_STREAM;    // stream-style sockets.
  hints.ai_flags = AI_PASSIVE;        // fill in my IP automatically.

  // Get an Internet address that can be bound to a socket.
  if((getaddrinfo(NULL, port, &hints, &servinfo)) != 0)
    {
      return -1;
    }

  // As a result of the previous call to getaddrinfo(), servinfo now
  // points to a linked list of 1 or more struct addrinfos.  Note that
  // they may not all be valid.  Scan through the servinfo until
  // something makes sense.
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((s = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      continue;   // On error, try next address in servinfo
    }
    
    // Set the socket options so that local addresses may be reused.
    // For the AF_INET family, this means that the subsequent bind
    // call should succeed except in cases when there is already an
    // active listening socket bound to the address.
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1) {
      return ACC_SOCK_OPT_FAILURE;  // Attempt to set socket options on this socket failed.
    }
    
    if (bind(s, p->ai_addr, p->ai_addrlen) == -1) {
      close(s);  // bind() failed.  Close this socket and try next address in servinfo.
      continue;
      }
    
    break; // Success.  A socket has been successfully created, optioned, and bound.
  }

  if (p == NULL)  {
    return ACC_SOCK_BIND_FAILURE;  // Socket failed to bind.
  }

  // Populate sh with the IP of this device.  Note that the IP bound
  // to the socket was likely 0.0.0.0 or 127.0.0.0 since NULL was the
  // first parameter in the getaddrinfo() call.  Instead, we want the
  // IP of the ethernet or wireless interface on this machine.  
  servQueryIP(sh);

  // Don't need servinfo anymore
  freeaddrinfo(servinfo);

  return 0;

  servHandlerSetEndpoint(sh, s);

  // TODO: The sig-handler setup only needs to happen once, not once
  // per call to accCreateConnection!

  // When the SIGCHLD signal occurs, the status of a child process has
  // changed and we need to call one of the wait functions to
  // determine what has happened. The default action for SIGCHLD is to
  // ignore the signal.  The following code alters that default so
  // that any SIGCHLD signal is handled by the signal handler,
  // accSigchldHandler() (see above).
  struct sigaction sa;
  sa.sa_handler = accSigchldHandler; 
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  
  // Set the action.
  if (sigaction(SIGCHLD, &sa, NULL) == -1) return ACC_SIGACTION_FAILURE;  // The action could not be set.

  // The passive-mode endpoint has successfully been created.  Now it
  // is time to listen and wait for another entity to approach and
  // accept their connection.
  if (listen(s, ACC_BACKLOG) == -1) return ACC_SOCK_LISTEN_FAILURE;  // listen() call failed.


  // TODO: At this point, we can broadcast our existence to the world,
  // so that others may know about our fantastic service.  Start the
  // thread that will perform broadcasting.



  char pee[INET6_ADDRSTRLEN];
  struct sockaddr_storage theirAddr; // connector's address information
  int newSock = -1;
  socklen_t size;
  
  // Wait for an approach.  Note that the accept() call blocks until a
  // connection is established.
  while (newSock == -1) 
    {      
      size = sizeof(theirAddr);
      newSock = accept(s, (struct sockaddr *)&theirAddr, &size);
      if (newSock == -1)
	{
	  return ACC_SOCK_ACCEPT_FAILURE;
	}
    }

  inet_ntop(theirAddr.ss_family, accGetInAddr((struct sockaddr *)&theirAddr), pee, sizeof(pee));

  return ACC_SUCCESS;
}


/**
 * accBroadcastConnection
 *
 * Broadcast the service on the network; the service's type and IP are
 * described by sh.
 */
int accBroadcastConnection(serviceHandler * sh)
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
 * @param[in/out] sh the serviceHandler partially populated by a call
 * to accCreateConnection() that will be fully populated by this call.
 * Subsequent read and write operations on this connection are
 * parameterized by this handler.
 *
 * @returns an indication of success or failure.
 *
 */
int accCompleteConnection(serviceHandler * sh)
{

  

}

