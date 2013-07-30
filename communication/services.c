/**
 * services.c
 * 
 * There are multiple types of services executing on the system that
 * allow flexible connections between robots and supervisors.  This
 * file contains the function implementations for the services in the
 * UPBOT system.
 *
 * For now, there are only two services planned for UPBOT, so
 * everything is piled into this file for now.  If there are ever more
 * services, or the services get more complicated, it may be
 * worthwhile to break up this file into pieces.
 *
 * 
 * @author Tanya L. Crenshaw
 * @since July 2013
 *
 */

#include "services.h"
#include "acceptor.h"
#include "connector.h"

// ************************************************************************
// FUNCTIONS GENERIC TO ALL SERVICE HANDLERS
// ************************************************************************

/**
 * servSigchldHandler
 *
 * This function is an interrupt-driven function which 
 * reaps dead children that are forked by the server.
 *
 * This function is local to services.c and should not be made
 * available to other source.
 */
void servSigchldHandler(int s)
{
  // The waitpid() call waits for a child process to reach a certain
  // state.  The WNOHANG option means that the waitpid() call will
  // return immediately if no child has exited.
  while(waitpid(-1, NULL, WNOHANG) > 0);
}


/**
 * servStop
 *
 * Given a service handler, clean up all of the resources (e.g.,
 * threads and sockets) currently in use by the service and shut
 * it down.
 *
 */
int servStop(serviceHandler * sh)
{

}

/**
 * servStart
 *
 * Given a serviceHandler and a serviceType, start up the endpoint of
 * a service.  
 *
 * This function has side-effects.  
 * 
 * For an acceptor service, this function creates multiple threads.
 * They are:
 * 
 * 1. A broadcast thread, accessible by sh->broadcast, that is
 * responsible for the discovery portion of the service.
 * 
 * 2. A connection thread, accessible by sh->connection, that is
 * responsible for the end-to-end communication with its endpoint
 * counterpart, e.g., a data collector paired with a data aggregator.
 *
 * TODO: Who is responsible for cleaning up these threads?
 *
 * For a connector service, the call blocks until a full connection
 * has been established.  
 *
 * @param[in] type the type of service endpoint to start up, e.g.,
 * SERV_DATA_SERVICE_AGGREGATOR or SERV_EVENT_RESPONDER_ROBOT.
 *
 * @param[out] sh a pointer to the serviceHandler that will be
 * populated as the result of this call.  Subsequent servRead() and
 * servWrite() calls should be invoked on the service via this
 * handler.
 *
 * @returns an indication of success or failure.
 */
int servStart(serviceType type, serviceHandler * sh)
{
  int status = 0;

#ifdef DONOTCOMPILE
  // TODO: I think this needs to be here, but I need to confirm this.
  // --TLC

  // TODO: Need to set up a signal handler such that when Ctrl+C gets
  // issued, all the threads and sockets get closed.

  // When the SIGCHLD signal occurs, the status of a child process has
  // changed and we need to call one of the wait functions to
  // determine what has happened. The default action for SIGCHLD is to
  // ignore the signal.  The following code alters that default so
  // that any SIGCHLD signal is handled by the signal handler,
  // accSigchldHandler() (see above).
  struct sigaction sa;
  sa.sa_handler = servSigchldHandler; 
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  
  // Set the action.
  if (sigaction(SIGCHLD, &sa, NULL) == -1) return ACC_SIGACTION_FAILURE;  // The action could not be set.
#endif

  // This is a generic function to start any of the possible service
  // endpoints.  There are multiple kinds of service endpoints, but
  // all endpoints are placed into two categories:
  //
  // Acceptor endpoints.  Create an endpoint of communication,
  // broadcast a service, and wait passively for other entities to
  // establish a connection.
  //
  // Connector endpoints.  Create an endpoint of communciation, listen
  // for broadcasts, and actively approach other acceptor endpoints to
  // establish a fully-connected line of communication.

  // So, the first step is to figure out: is this an acceptor or a
  // connector?

  // Is it an acceptor?  Right now, there are only two kinds of
  // acceptors:
  if((type == SERV_DATA_SERVICE_AGGREGATOR) || (type == SERV_EVENT_RESPONDER_PROGRAMMER))
    {
      // Create an acceptor-side passive-mode endpoint of communication
      // for the type of service specified.
      if((status = accCreateConnection(servToPort(type), type, sh)) != ACC_SUCCESS)
	{
	  perror("Cannot create an endpoint of communication.\naccCreateConnection() failed: ");
	  printf("Status = %d\n", status);
	}

      // Create a thread to broadcast the service.
      if(pthread_create(&(sh->broadcast), NULL, accBroadcastService, sh) != 0)
	{
	  perror("Cannot create a thread for broadcasting.\npthread_create() failed: ");
	  return SERV_CANNOT_CREATE_THREAD;
	}

      // Just to mark progress, print the resulting service handler
      servHandlerPrint(sh);  

      // Create a thread to wait for a connection.
      if(pthread_create(&(sh->connection), NULL, accAcceptConnection, sh) != 0)
	{
	  perror("Cannot create a thread to wait for connections.\npthread_create() failed: ");
	  return SERV_CANNOT_CREATE_THREAD;
	}
    }

  if(type == SERV_DATA_SERVICE_COLLECTOR)
    {
      // Listen for a service to connect with.
      printf("Listening for a service...\n");

      // Note: Since aggregators always broadcast on a particular port and
      // collectors always listen on a particular port, it's enough for
      // now to simply listen for any broadcasts on the data service port.
      // This call blocks until something is heard.
      conListenForService(type, sh);

      // The broadcast has been heard.  Now, establish a connection
      // with the other endpoint of the service.
      conInitiateConnection(sh);
    }


}

/**
 * servActivate
 *
 * Given a serviceHandler, create a thread of execution that will
 * actually perform the functionality of the service for the
 * fully-established endpoint.
 *
 * This function may only be called if this endpoint already has a
 * fully-established connection with another endpoint; that is, sh->eh
 * cannot be set to SERV_HANDLER_NOT_SET.
 *
 * @param[in/out] sh a partially populated service handler whose
 * service field will be populated as a result of a successful call to
 * this function.
 *
 * @returns If the endpoint handler field of sh is not set, the
 * function returns SERV_HANDLER_NOT_SET.  If the type of service is
 * not one of the four endpoint service types for the data service or
 * the event:responder service, the function returns SERV_BAD_TYPE.
 * If the thread of execution to service the endpoint cannot be
 * created, the function returns SERV_CANNOT_CREATE_THREAD.
 * Otherwise, the function returns SERV_SUCCESS;
 */
int servActivate(serviceHandler * sh)
{

  // First, this function can only activate a service for an endpoint that
  // is already fully connected.  
  if(sh->eh == SERV_HANDLER_NOT_SET)
    return SERV_HANDLER_NOT_SET;

  // TODO: Later work will implement asynchronous communication.  For
  // now, I am just trying to get an end-to-end service connection
  // working. -- TLC
  
  // Now, determine what type of service this is and create a thread
  // for the correct activate() function.
  void * activatorFunction = NULL;

  if((activatorFunction = servToActivate(sh->typeOfService)) == NULL)
    return SERV_BAD_TYPE;
  
  // Create a thread to service the connection using the
  // previously-determined activate() function.
  if(pthread_create(&(sh->service), NULL, activatorFunction, sh) != 0)
    {
      perror("Cannot create a thread for servicing the connection.\npthread_create() failed: ");
      return SERV_CANNOT_CREATE_THREAD;
    }  
  
  return SERV_SUCCESS;

}

/**
 * servIsReady
 *
 * Given a serviceHandler, determine if the service endpoint has
 * completed its start-up phase and is fully-connected with another
 * service endpoint.  For example, if one called...
 * 
 * serviceHandler sh;
 * servStart(SERV_EVENT_RESPONDER_PROGRAMMER, &sh); 
 *
 * ...then this function would return TRUE (i.e., 1) if sh was a fully
 * established connection to a SERV_EVENT_RESPONDER_ROBOT endpoint,
 * making for a fully-operational and ready to access event:responder
 * service.
 */
int servIsReady(serviceHandler * sh)
{

  // TODO: This isn't exactly accurate.  A service handler is ready
  // when the service field representing the service thread is
  // running.  Need to update this.  -- TLC
  
  // A service handler is ready if the endpoint handler has been set
  // as that indicates a fully-connected service has been established.
  if(sh == NULL || sh->eh == SERV_HANDLER_NOT_SET)
    return 0;
  else
    return 1;

}


/**
 * servRead
 *
 * Given a serviceHandler, read any available data from the service.
 * 
 * Likely this will dispatch a service-dependent read() function, but
 * I think it would be desirable to just have one read() function for
 * all services.
 *
 * TODO: Perhaps there should be a blocking and non-blocking version?  Not
 * sure yet. -- TLC.
 * 
 * @param[in] sh a pointer to the serviceHandler from which to read.
 *
 * @returns an indication of success or failure.  For example, if
 * the serviceHandler has lost connection, indicate that the read
 * cannot be completed as dialed.
 */
int servRead(serviceHandler * sh)
{

}

/**
 * servWrite
 * 
 * Given a serviceHandler, write data to the service.
 * 
 * @param[in] sh a pointer to the serviceHandler to which to write.
 *
 * @returns an indication of success or failure.
 */
int servWrite(serviceHandler * sh)
{

}

/**
 * servGetInAddr
 *
 * Get sockaddr, IPv4 or IPv6.
 *
 * @param[in] sa a pointer to a socket address whose IP we are
 * interested in obtaining.
 *
 * @returns a pointer to the IPv4 or IPv6 address.  The sa_family
 * field of the input parameter controls which type of IP is returned.
 * If sa_family is AF_INET, the IPv4 address is returned.  Otherwise,
 * IPv6 is returned.
 */
void * servGetInAddr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


/**
 * servToPort
 *
 * Given a service type, return the communication port # to use.
 * 
 * @param[in] s the serviceType, e.g., SERV_DATA_SERVICE_AGGREGATOR or
 * SERV_DATA_SERVICE_COLLECTOR (the two halves of the data service
 * that communicate on port "10006").
 * 
 * @returns a pointer to the string representing the port number,
 * e.g., "10006".  If the serviceType is SERV_SERVICE_NOT_SET or
 * SERV_NUMBER_OF_SERVICES, the empty string is returned, "".
 *
 */
char * servToPort(serviceType s)
{
  if((s == SERV_DATA_SERVICE_AGGREGATOR) || (s == SERV_DATA_SERVICE_COLLECTOR))
    {
      return SERV_DATA_SERVICE_PORT;
    }
     
  else if((s == SERV_EVENT_RESPONDER_PROGRAMMER) || (s == SERV_EVENT_RESPONDER_ROBOT) )
    {
      return SERV_EVENT_RESPONDER_SERVICE_PORT;
    }

  else
    {
      return SERV_PORT_NOT_SET;	  
    }
}

/**
 * servToActivate
 *
 * Given a service type, return the appropriate activate() function.
 * 
 * @param[in] s the serviceType, e.g., SERV_DATA_SERVICE_AGGREAGATOR
 * or SERV_DATA_SERVICE_COLLECTOR.
 *
 * @returns a function pointer of type (void *) to the activate()
 * function for the serviceType set in sh.  If sh is NULL or if there
 * is no activate() function for the serviceType, this function
 * returns NULL
 */
void * servToActivate(serviceType s)
{

  // This is a sophomoric little if-statment sequence that maps
  // types of service to the activate() function.  
  if(s == SERV_DATA_SERVICE_COLLECTOR)
    return dsCollectorActivate;
  
  else if(s == SERV_DATA_SERVICE_AGGREGATOR)
    return dsCollectorActivate; 

  else if(s  == SERV_EVENT_RESPONDER_ROBOT)
    return erRobotActivate; 

  else if(s  == SERV_EVENT_RESPONDER_PROGRAMMER)
    return erProgrammerActivate; 

  else
    return NULL;
}

/**
 * servHandlerSetDefaults
 *
 * Given a serviceHandler, sh, set its default values.
 * 
 * @param[in] sh the serviceHandler to populate with default values.
 * 
 * @returns If sh is NULL, return SERV_NULL_SH to indicate an error.
 * Otherwise, return SERV_SUCCESS.
 *
 */
int servHandlerSetDefaults(serviceHandler * sh)
{
  if(sh == NULL) return SERV_NULL_SH;

  sh->typeOfService = SERV_SERVICE_NOT_SET;

  sh->handler = SERV_HANDLER_NOT_SET;
  sh->eh = SERV_HANDLER_NOT_SET;
  sh->bh = SERV_HANDLER_NOT_SET;

  // Make some empty strings 
  sh->port[0] = '\0';
  sh->ip[0] = '\0';
  sh->rip[0] = '\0';
  sh->interface[0] = '\0';

  return SERV_SUCCESS;
}

/**
 * servHandlerSetPort
 *
 * Given a serviceHandler, sh, set its port field.
 *
 * @param[in] sh the serviceHandler to whose port field is to be set.
 * @param[in] port the port value.
 * 
 * @returns If sh is NULL, return SERV_NULL_SH to indicate an error.
 * Otherwise, return SERV_SUCCESS.  
 *
 */
int servHandlerSetPort(serviceHandler * sh, char * port)
{
  if(sh == NULL) return SERV_NULL_SH;
  if(port == NULL) return SERV_BAD_PORT;

  // Copy the stringified port number into the port field.
  strncpy(sh->port, port, SERV_MAX_PORT_LENGTH);

  // Assure that the stringified port number is null-terminated
  sh->port[SERV_MAX_PORT_LENGTH - 1] = '\0';

  return SERV_SUCCESS;
}

/**
 * servHandlerSetEndpointHandle
 *
 * Given a serviceHandler, sh, set its endpointHandler field.
 *
 * @param[in] sh the serviceHandler to whose port field is to be set.
 * @param[in] eh the endpointHandler (i.e., socket) value.
 * 
 * @returns If sh is NULL, return SERV_NULL_SH to indicate an error.
 * Otherwise, return SERV_SUCCESS.  
 *
 */
int servHandlerSetEndpointHandle(serviceHandler * sh, int eh)
{
  if(sh == NULL) return SERV_NULL_SH;

  sh->eh = eh;

  return SERV_SUCCESS;
}

/**
 * servHandlerSetBroadcastHandle
 *
 * Given a serviceHandler, sh, set its broadcast hHandler field.
 *
 * @param[in] sh the serviceHandler to whose port field is to be set.
 * @param[in] bh the broadcast handle (i.e., socket) value.
 * 
 * @returns If sh is NULL, return SERV_NULL_SH to indicate an error.
 * Otherwise, return SERV_SUCCESS.  
 *
 */
int servHandlerSetBroadcastHandle(serviceHandler * sh, int bh)
{
  if(sh == NULL) return SERV_NULL_SH;

  sh->bh = bh;

  return SERV_SUCCESS;
}


/**
 * servHandlerSetType
 *
 * Given a serviceHandler, sh, set its endpoint type field.
 *
 * @param[in] sh the serviceHandler to whose port field is to be set.
 * @param[in] et the endpoint type value (SERV_TCP_ENDPOINT or SERV_UDP_ENDPOINT).
 * 
 * @returns If sh is NULL, return SERV_NULL_SH to indicate an error.
 * Otherwise, return SERV_SUCCESS.  
 *
 */
int servHandlerSetType(serviceHandler * sh, int et)
{

#ifdef DONOTCOMPILE
  if(sh == NULL) return SERV_NULL_SH;

  sh->et = et;

  return SERV_SUCCESS;
#endif

}

/**
 * servHandlerSetService
 *
 * Given a serviceHandler, sh, set its service type field.
 *
 * @param[in] sh the serviceHandler to whose port field is to be set.
 * @param[in] type the service type.
 * 
 * @returns If sh is NULL, return SERV_NULL_SH to indicate an error.
 * Otherwise, return SERV_SUCCESS.  
 *
 */
int servHandlerSetService(serviceHandler * sh, serviceType type)
{
  if(sh == NULL) return SERV_NULL_SH;
  
  sh->typeOfService = type;

  return SERV_SUCCESS;
}

/**
 * servHandlerPrint
 *
 * Given a serviceHandler, sh, pretty-print its contents to 
 * the screen.
 *
 * @param[in] sh the serviceHandler to be printed. 
 * 
 * @returns If sh is NULL, return SERV_NULL_SH to indicate an error.
 * Otherwise, return SERV_SUCCESS.  
 *
 */
int servHandlerPrint(serviceHandler * sh)
{
  if(sh == NULL) return SERV_NULL_SH;
  
  printf("\n***** SERVICE HANDLER ************************\n");
  printf("   Service type:       %s.\n", serviceNames[sh->typeOfService]);
  printf("   Endpoint handle:    %d.\n", sh->eh); 
  printf("   Broadcast handle:   %d.\n", sh->bh);

  if(sh->handler == -1)
    {
      printf("   Connection handle:  Unset.\n");
    }
  else
    {
      printf("   Connection handle:  %d.\n", sh->handler);
    }

  printf("   My IP:              %s.\n", sh->ip);
  printf("   Remote IP:          %s.\n", sh->rip);
  printf("   Port number:        %s.\n", sh->port);    
  printf("   Interface:          %s.\n", sh->interface);
  printf("**********************************************");
  printf("\n\n");

  return SERV_SUCCESS;   
}

/**
 * servQueryIP
 *
 * Get the IP address attached to a particular network device.  The
 * value is returned via the caller-allocated serviceHandler passed
 * into the function.
 *
 * @param[out] sh the serviceHandler whose IP field will be populated
 * by this call.
 *
 * @returns If sh is NULL, return SERV_NULL_SH to indicate an error.
 * If the particular interface name for the target platform cannot be
 * located, or it has no IP address, return SERV_NO_DEVICE_IP to
 * indicate an error.  Otherwise, return SERV_SUCCESS.
 * 
 */
int servQueryIP(serviceHandler * sh)
{

  // The name of the interface of interest is dependent on the target platform 
  // for which this software has been compiled.  Use a compiler flag
  // -DGUMSTIX or -DMAC (as seen in makefile) to indicate the target platform
  // and thereby get the correct interface name.
#ifdef GUMSTIX
  char * interfaceName = "wlan0";
#elif MAC
  char * interfaceName = "en0";        // TODO: I need to make this better than
  // char * interfaceName = "eth0";    // a hard-coded variable I have to change all the time!
#else
  char * interfaceName = "";
#endif

  if(sh == NULL) return SERV_NULL_SH;

  struct ifaddrs * interfaces = NULL;
  struct ifaddrs * current = NULL;

  // Set the interface name in the serviceHandler.  Note that an
  // interface name in the service handler only indicates the
  // interface we desire, not a working or existing interface.
  strncpy(sh->interface, interfaceName, SERV_MAX_INTERFACE_LENGTH);
  sh->interface[SERV_MAX_INTERFACE_LENGTH - 1] = '\0';  // I don't trust strncpy.

  // Get a linked-list of interfaces on this machine.
  if (!getifaddrs(&interfaces)) {

    // Use a pointer to loop through linked list of interfaces
    current = interfaces;

    while(current != NULL) {

      // If this interface is an Internet interface....
      if(current->ifa_addr->sa_family == AF_INET) {

	if(!(strcmp(current->ifa_name, interfaceName))){
	  strncpy(sh->ip, inet_ntoa(((struct sockaddr_in*)current->ifa_addr)->sin_addr), SERV_MAX_IP_LENGTH);
	  sh->ip[SERV_MAX_IP_LENGTH - 1] = '\0';  // I don't trust strncpy.

	  // Check if the IP is actually an empty string.  If so,
	  // return error.  Otherwise free memory and return success.
	  if(sh->ip[0] == '\0') {
	    printf("1: No device IP\n");
	    return SERV_NO_DEVICE_IP;
	  }

	  else
	    {
	      printf("Setting acceptor IP to: %s\n", sh->ip);

	      // Free memory
	      freeifaddrs(interfaces);
  
	      return SERV_SUCCESS;
	      
	    }
	}
      }
      
      /// Advance to the next in the list.
      current = current->ifa_next;
    }
  }

  // Free memory
  freeifaddrs(interfaces);

  // If we reached this point, it means we looped through all the interfaces
  // without finding the specific name.

  printf("2: No device IP\n");

  return SERV_NO_DEVICE;

}

/**
 * servCreateEndpoint
 *
 * Create an endpoint of communication (i.e., a socket).  
 *
 * The purpose of this function is to abstract away all the socket
 * programming details that individual functions shouldn't have to
 * worry about.  For the UPBOT system, there are two kinds of sockets
 * of interest:
 *
 * 1. A TCP socket for fully-connected network communication between
 *    two entities.
 *
 * 
 * 2. A UDP socket for broadcasting a service or listening for
 *    broadcasts.
 *
 * These two kinds of sockets require four different approaches to
 * creating and setting up the socket.  This type is defined by
 * 'endpointType' in service.h.
 *
 * - SERV_TCP_ACCEPTOR_ENDPOINT is a socket endpoint of communication
 *   for the acceptor half of the fully-connected TCP connection.
 *
 * - SERV_TCP_CONNECTOR_ENDPOINT is a socket endpoint of communication
 *   for the connector half of the fully-connected TCP connection.
 *
 * - SERV_UDP_BROADCAST_ENDPOINT is a socket endpoint of communication
 *   for any service that needs to broadcast its availability to
 *   other entities.
 * 
 * - SERV_UDP_LISTENER_ENDPOINT is a socket endpoint of communication
 *   for any entity that wants to discover a service.
 *
 * @param[in] type the type of communication:
 * SERV_TCP_ACCEPTOR_ENDPOINT, SERV_TCP_CONNECTOR_ENDPOINT,
 * SERV_UDP_BROADCAST_ENDPOINT or SERV_UDP_LISTENER_ENDPOINT.
 *
 * @param[in] port the port to use, e.g. "10006" or "22".
 *
 * @param[out] sh the serviceHandler whose field will be populated by
 * this call.  If operation is successful, and the type of
 * communication set is SERV_TCP*ENDPOINT, the endpoint handle field
 * will be set.  If the operation is successful, and the type of
 * communication is SERV_UDP*ENDPOINT, the broadcast handle field
 * will be set.
 *
 * @returns If sh is NULL, return SERV_NULL_SH to indicate an error.
 * If port is NULL, return SERV_BAD_PORT to indicate an error.  If an
 * attempt to get an Internet address to bind to the socket fails,
 * return SERV_CANNOT_GET_ADDRESS.  If an attempt to set the socket
 * options fails, return SERV_SOCK_OPT_FAILURE.  If an attempt to bind
 * the socket fails, return SERV_SOCK_BIND_FAILURE.  Callers may use
 * perror() to get more information on why the particular failures
 * occurred.
 * 
 *  Otherwise, return SERV_SUCCESS.
 * 
 */

int servCreateEndpoint(endpointType type, char * port, serviceHandler * sh) 
{

  int s;           // socket handler
  int optval = 1;  // boolean option value for the SO_REUSEADDR option.

  // Defining the fields for socket structs is challenging.  The fields depend on the
  // address, type of socket, and communication protocol.  This function uses getaddrinfo()
  // to aid in defining the struct socket fields.  This function fills a struct of
  // type addrinfo.
  struct addrinfo hints;
  struct addrinfo * servinfo, *p; 

  if(port == NULL) return SERV_BAD_PORT;
  if(sh == NULL) return SERV_NULL_SH;
  
  // Set the hints.  These are the criteria that getAddrInfo() will
  // use to construct a list of possible addresses.  Most of the hints
  // vary based on whether or not we want a TCP or UDP endpoint.
  memset(&hints, 0, sizeof(hints));
  

  // Initialize the hints structure based on what little we care about
  // in terms of the socket.  The goal is to listen in on host's IP
  // address on the port provided by the 'port' parameter.

  // For either TCP or UDP endpoints, we are willing to accept on any
  // address or port.
  hints.ai_flags = AI_PASSIVE;        

  // Now for the hints that are TCP- or UDP-specific.
  if ((type == SERV_UDP_BROADCAST_ENDPOINT) || (type == SERV_UDP_LISTENER_ENDPOINT)) {
    hints.ai_family = AF_UNSPEC;       // any address family
    hints.ai_socktype = SOCK_DGRAM;    // datagram-style sockets.
    //hints.ai_protocol = IPPROTO_UDP;   //  only UDP protocol 
  }
  
  else if (type == SERV_TCP_ACCEPTOR_ENDPOINT) {
    hints.ai_family = AF_INET;          // don't care if its IPv4 or IPv6.
    hints.ai_socktype = SOCK_STREAM;    // stream-style sockets.  
  }


  // If we are to create a SERV_TCP_CONNECTOR_ENDPOINT, then we must use
  // the already discovered acceptor endpoint IP to bind to the socket.
  if (type == SERV_TCP_CONNECTOR_ENDPOINT)
    {
    if((getaddrinfo(sh->rip, port, &hints, &servinfo)) != 0)
      {
	return SERV_CANNOT_GET_ADDRESS;
      }
    }
  
  // Otherwise, just get an address that can be bound to a socket.
  else { 
    if((getaddrinfo(NULL, port, &hints, &servinfo)) != 0)
      {
	return SERV_CANNOT_GET_ADDRESS;
      }
  }
  
    
  // As a result of the previous call to getaddrinfo(), servinfo now
  // points to a linked list of 1 or more struct addrinfos.  Note that
  // they may not all be valid.  Scan through the servinfo until
  // something makes sense.
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((s = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      continue;   // On error, try next address in servinfo
    }
    
    // If control reaches here, there was a succesfully created
    // socket.
    
    // Set the socket options so that local addresses may be reused.
    // For the AF_INET family, this means that the subsequent bind
    // call should succeed except in cases when there is already an
    // active listening socket bound to the address.
    
    // The socket options vary based on whether or not we want a
    // TCP or UDP endpoint.
    if (type == SERV_TCP_ACCEPTOR_ENDPOINT) {
      if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1) {
	return SERV_SOCK_OPT_FAILURE;  // Attempt to set socket options on this socket failed.
      }
    }
    
    if (type == SERV_UDP_BROADCAST_ENDPOINT) {
      if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(int)) == -1) {
	return SERV_SOCK_OPT_FAILURE;  // Attempt to set socket options on this socket failed.
      }
    }
    
    // Except for the TCP connector endpoint, all other endpoints need
    // to bind the address to the newly-created socket.
    if (type != SERV_TCP_CONNECTOR_ENDPOINT)
      {
	if (bind(s, p->ai_addr, p->ai_addrlen) == -1) {
	  close(s);  // bind() failed.  Close this socket and try next address in servinfo.
	  continue;
	}
      }

    // For the TCP connector endpoint, we need to call connect to establish
    // a fully-connected line of communication.
    if (type == SERV_TCP_CONNECTOR_ENDPOINT)
      {
	if (connect(s, p->ai_addr, p->ai_addrlen) == -1) {
	  close(s);
	  continue;
	}
      }
    
    break; // Success.  A socket has been successfully created, optioned, and bound.
  }

  if (p == NULL)  {
    // TODO: Need to update this failure as it is more generic now.
    // Either a call to bind or a call to connect() failed.
    //
    return SERV_SOCK_BIND_FAILURE;  // Socket failed to bind.
  }

  // Now that we have a socket, set the appropriate handler for the
  // serviceHandler.  If we just created a TCP socket, set the
  // endpoint handler.  Otherwise, set the broadcast handler.
  if((type == SERV_TCP_ACCEPTOR_ENDPOINT) || (type == SERV_TCP_CONNECTOR_ENDPOINT)) { 
    servHandlerSetEndpointHandle(sh, s);
  }
  
  else  {
    servHandlerSetBroadcastHandle(sh, s);
  }

  // Don't need servinfo anymore
  freeaddrinfo(servinfo);

  return SERV_SUCCESS;
  
}


// Stubs for all the activate() functions for the four service
// endpoints.
int dsAggregatorActivate(serviceHandler * sh)
{

  printf("Aggregator activated\n");

}

int dsCollectorActivate(serviceHandler * sh)
{

  printf("Collector activated\n");

}

int erProgrammerActivate(serviceHandler * sh)
{

  printf("Programmer activated\n");

}

int erRobotActivate(serviceHandler * sh)
{
  printf("Robot activated\n");
}


// ************************************************************************
//
// DATA SERVICE.  The data service accepts sensor data collected by
// the robots in the physical world and delivers the data to other
// software entities, e.g. the system supervisor.
// 
// The data service is implemented in two halves.  
// 
// a. The Collector.  The data service collector collects sensor data
// locally at a given sensor data collection point, i.e., a robot.
// The Collector is responsible for forwarding data to a collator.
//
// b. The Collator.  The data service collator is a repository for one
// or many sensor data collectors.  It also allows entities to
// subscribe to sensor data.
// 
// ************************************************************************

/**
 * dsCreateCollector
 *
 * The Collector half of the data service collects sensor data locally
 * at a given robot.  If specified by the connectRemote parameter, it
 * also connects to a remote data collator service to whom the locally
 * collected data will be reported.  Use constant values
 * SERV_CONNECT_REMOTE or SERV_NO_CONNECT to indicate whether or not
 * to make this remote connection.
 * 
 * Upon calling dsCreateCollector, it is possible that the operating
 * system may not have the resources necessary for the caller to
 * communicate locally with this service.  If so, the function returns
 * SERV_LOCAL_FAILURE (-1) to indicate an error.
 *
 * Upon calling dsCreateCollector, it is possible that the operating
 * system may not have the resources necessary to communicate remotely
 * with its data collator service.  If so, the function returns
 * SERV_REMOTE_FAILURE to indicate an error.
 *
 * Upon calling dsCreateCollector, it is possible that no initial
 * connection may be established with the remote service.  Based on
 * the value of the parameter continueLocally, dsCreateCollector will
 * either:
 *
 *  i) SERV_NO_REMOTE_CONTINUE: create a working local service that
 *  will make subsequent attempts to connect to the remote service,
 *  though no guarantee is made on the success of these attempts.
 * 
 *  ii) SERV_NO_REMOTE_FAIL: no local service is created in the
 *  absence of the remote connection.
 *
 * In either case, the function will return SERV_NO_CONNECTION if no
 * remote connection was established.
 * 
 * The parameter, sh, must exist.  If sh is NULL, then we do not have a
 * well-formed service handler and the function returns SERV_NULL_SH
 * (-3) to indicate an error.
 *
 * @param[in] connectRemote a boolean value indicating whether or not
 * this local data collector should establish a connection with a
 * remote data collator service.
 * 
 * @param[out] sh the serviceHandler to be populated by this
 * dsCreateCollector call.  This serviceHandler is how the caller make
 * subsequent accesses of the service.
 * 
 * @returns an integer value indicating success or failure of the
 * dsCreateCollector call.  
 */
int dsCreateCollector(int connectRemote, int continueLocally, serviceHandler * sh)
{

}


/**
 * dsCreateCollator
 * 
 * The Collator half of the data service gathers sensor data from
 * collectors and forwards it to interested entities
 */
void dsCreateCollator(void)
{

}


/**
 * dsConnectToRobot()
 * 
 * Allow an entity to subscribe to the data of a particular
 * robot via a Data Service Collator.
 */
serviceHandler * dsConnectToRobot(const char * name)
{

}

/**
 * dsGetData()
 * 
 * Allow an entity to get the data to which it has subscribed
 * from the data service.
 * 
 * @param[in] sh the serviceHandler for the service from which data will
 * be read.
 *
 * @param[in] control an indicator for how much data will be read, 1 sensor
 * report or all available sensor reports.
 *
 * @param[out] dest a pointer to where the data shall be placed.
 *
 * @returns an integer value describing the success or failure of the
 * operation.
 */
int dsGetData(serviceHandler * sh, int control, void * dest)
{

}


/**
 * dsWrite()
 *
 * Allow an entity to write sensor data or control commands to a
 * data service.
 *
 * @param[in] sh the serviceHandler for the service to which data will
 * be written.
 *
 * @param[in] src a pointer to the data to be written
 *
 * @returns an integer value describing the success or failure of the
 * operation.
 */
int dsWrite(serviceHandler * sh, void * src)
{

}