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

	// Check if interface is en1.
	//
	// TODO: The interface name is going to be predicated on the
	// target device that is being used.  I might add a compiler
	// flag to indicate the target and conditionally compile this
	// section of code based of target.  It may also be necessary
	// to pass the interface name to this function.  Not sure
	// yet....
	if(!(strcmp(current->ifa_name, "en1"))){
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

