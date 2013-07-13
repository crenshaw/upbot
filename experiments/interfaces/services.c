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
 * servHandlerSetEndpoint
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
int servHandlerSetEndpoint(serviceHandler * sh, int eh)
{
  if(sh == NULL) return SERV_NULL_SH;

  sh->eh = eh;

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
  
  printf("Service Handler\n");
  printf("   Service type:       %s.\n", serviceNames[sh->typeOfService]);
  printf("   Endpoint handler:   %d.\n", sh->eh); 

  if(sh->handler == -1)
    {
      printf("   Connection handler: Unset.\n");
    }
  else
    {
      printf("   Connection handler: %d.\n", sh->handler);
    }

  printf("   IP:                 %s.\n", sh->ip);
  printf("   Port number:        %s.\n", sh->port);    
  printf("   Interface:          %s.\n", sh->interface);

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
#endif

#ifdef MAC
  char * interfaceName = "en1";
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
 * @param[in] type the type of communication: SERV_TCP_ENDPOINT or
 * SERV_UDP_ENDPOINT.
 *
 * @param[in] port the port to use, e.g. "10005" or "22".
 *
 * @param[out] sh the serviceHandler whose xx field will be populated
 * by this call.
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
  if(port == NULL) return SERV_BAD_PORT;
  if(sh == NULL) return SERV_NULL_SH;
  
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
    
    // Set the socket options so that local addresses may be reused.
    // For the AF_INET family, this means that the subsequent bind
    // call should succeed except in cases when there is already an
    // active listening socket bound to the address.
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1) {
      return SERV_SOCK_OPT_FAILURE;  // Attempt to set socket options on this socket failed.
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

  // Now that we have a successfully bound socket, set the endpoint handler
  // field of the serviceHandler that was passed to this function.
  servHandlerSetEndpoint(sh, s);
  
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

