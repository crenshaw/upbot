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


// ************************************************************************
// FUNCTIONS GENERIC TO ALL SERVICE HANDLERS
// ************************************************************************

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

  printf("   IP:                 %s.\n", sh->ip);
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
  char * interfaceName = "en1";
#else
  char * interfaceName = "";
#endif

  if(sh == NULL) return SERV_NULL_SH;

  struct ifaddrs * interfaces = NULL;
  struct ifaddrs * current = NULL;

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
	  if(sh->ip[0] == '\0') return SERV_NO_DEVICE_IP;

	  else
	    {
	      printf("Setting acceptor IP to: %s\n", sh->ip);
	      strncpy(sh->interface, current->ifa_name, SERV_MAX_INTERFACE_LENGTH);
	      sh->interface[SERV_MAX_INTERFACE_LENGTH - 1] = '\0';  // I don't trust strncpy.

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
 * 2. A UDP socket for broadcasting a service or listening for
 *    broadcasts.
 *
 * @param[in] type the type of communication: SERV_TCP_ENDPOINT,
 * SERV_UDP_BROADCAST_ENDPOINT or SERV_UDP_LISTENER_ENDPOINT.
 *
 * @param[in] port the port to use, e.g. "10005" or "22".
 *
 * @param[out] sh the serviceHandler whose field will be populated by
 * this call.  If operation is successful, and the type of
 * communication set is SERV_TCP_ENDPOINT, the endpoint handle field
 * will be set.  If the operation is successful, and the type of
 * communication is SERV_UDP_*_ENDPOINT, the broadcast handle field
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

int servCreateEndpoint(int type, char * port, serviceHandler * sh) 
{

  int s;           // socket handler
  int optval = 1;  // boolean option value for the SO_REUSEADDR option.

  // Defining the fields for socket structs is challenging.  The fields depend on the
  // address, type of socket, and communication protocol.  This function uses getaddrinfo()
  // to aid in defining the struct socket fields.  This function fills a struct of
  // type addrinfo.
  struct addrinfo hints;
  struct addrinfo * servinfo, *p; 

  // Perform error-checking on the input.
  if(!(type == SERV_TCP_ENDPOINT || 
       type == SERV_UDP_BROADCAST_ENDPOINT ||
       type == SERV_UDP_LISTENER_ENDPOINT)) {
    return SERV_BAD_TYPE;
  }

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
  if (type == SERV_UDP_BROADCAST_ENDPOINT || type == SERV_UDP_LISTENER_ENDPOINT ) {
    hints.ai_family = AF_UNSPEC;       // any address family
    hints.ai_socktype = SOCK_DGRAM;    // datagram-style sockets.
    hints.ai_protocol = IPPROTO_UDP;   //  only UDP protocol 
  }
  
  else if (type == SERV_TCP_ENDPOINT) {
    hints.ai_family = AF_INET;          // don't care if its IPv4 or IPv6.
    hints.ai_socktype = SOCK_STREAM;    // stream-style sockets.  
  }


  // Get an Internet address that can be bound to a socket.
  if((getaddrinfo(NULL, port, &hints, &servinfo)) != 0)
    {
      return SERV_CANNOT_GET_ADDRESS;
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
    
    // If we just want to create a socket to listen to UDP
    // broadcasts, we are all done.  Get out of this loop.
    if (type == SERV_UDP_LISTENER_ENDPOINT) {
      printf("Successfully created listener\n");
      break;
    }
  
    // Set the socket options so that local addresses may be reused.
    // For the AF_INET family, this means that the subsequent bind
    // call should succeed except in cases when there is already an
    // active listening socket bound to the address.
    
    // The socket options vary based on whether or not we want a
    // TCP or UDP endpoint.
    if (type == SERV_TCP_ENDPOINT) {
      if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1) {
	return SERV_SOCK_OPT_FAILURE;  // Attempt to set socket options on this socket failed.
      }
    }

    if (type == SERV_UDP_BROADCAST_ENDPOINT) {
      if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(int)) == -1) {
	return SERV_SOCK_OPT_FAILURE;  // Attempt to set socket options on this socket failed.
      }
    }
    
    if (bind(s, p->ai_addr, p->ai_addrlen) == -1) {
      close(s);  // bind() failed.  Close this socket and try next address in servinfo.
      continue;
    }
    
    break; // Success.  A socket has been successfully created, optioned, and bound.
  }

  if (p == NULL)  {
    return SERV_SOCK_BIND_FAILURE;  // Socket failed to bind.
  }

  // Now that we have a successfully bound socket, set the appropriate
  // handler for the serviceHandler.  If we just created a TCP socket,
  // set the endpoint handler.  Otherwise, set the broadcast handler.
  if(type == SERV_TCP_ENDPOINT) { 
    servHandlerSetEndpointHandle(sh, s);
  }
  
  else  {
    servHandlerSetBroadcastHandle(sh, s);
    servHandlerPrint(sh);
  }
  // Don't need servinfo anymore
  freeaddrinfo(servinfo);

  return SERV_SUCCESS;
  
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

