/**
 * services.h
 *
 * There are multiple types of services executing on the system that
 * allow flexible connections between robots and supervisors.  This
 * file contains types and function prototypes for the services
 * in the UPBOT system.
 * 
 * @author Tanya L. Crenshaw
 * @since July 01, 2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <signal.h>
#include <ifaddrs.h>

#include <fcntl.h>

#include <mqueue.h>

#include <arpa/inet.h>

#include <net/if.h>
#include <netinet/in.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/mman.h>

#ifndef _SERVICES_H_
#define _SERVICES_H_

/**
 *  CONSTANT DEFINITIONS.  All constants in this file should begin
 *  with 'ER' to indicate their membership in eventresponder.h  
 */
#define SERV_MAX_PORT_LENGTH 6 // e.g., "10006"
#define SERV_MAX_IP_LENGTH 30  // e.g., "::ffff:10.81.3.131"
#define SERV_MAX_INTERFACE_LENGTH 10 // e.g., "en1" or "wlan0"

#define SERV_LOCAL_FAILURE (-1)
#define SERV_REMOTE_FAILURE (-2)
#define SERV_NULL_SH (-3)
#define SERV_NO_CONNECTION (-4)
#define SERV_NO_DEVICE_IP (-5)
#define SERV_NO_DEVICE (-6)
#define SERV_BAD_PORT (-7)

#define SERV_CANNOT_GET_ADDRESS (-8)
#define SERV_SOCK_OPT_FAILURE (-9)
#define SERV_SOCK_BIND_FAILURE (-10)
#define SERV_BAD_TYPE (-11)
#define SERV_CANNOT_CREATE_THREAD (-12)
#define SERV_NULL_NAME (-13)
#define SERV_NO_REMOTE_IP (-14)
#define SERV_NULL_IP (-15)
#define SERV_NULL_DATA (-16)
#define SERV_NO_HANDLER (-17)
#define SERV_BAD_BROADCAST_ADDR (-18)
#define SERV_CANNOT_CREATE_QUEUE (-19)
#define SERV_NO_DATA (-20)

#define SERV_SUCCESS (0)

#define SERV_CONNECT_REMOTE (1)
#define SERV_NO_CONNECT (0)

#define SERV_NO_REMOTE_CONTINUE (1)
#define SERV_NO_REMOTE_FAIL (0)


// Enumerate the different possible service types.  Note that the
// compiler shall assign integer values to the terms
// 'SERV_SERVICE_NOT_SET', 'SERV_DATA_SERVICE_AGGREGATOR' and so
// on.
typedef enum serviceTypeTag serviceType;
enum serviceTypeTag {
  SERV_SERVICE_NOT_SET,
  SERV_DATA_SERVICE_AGGREGATOR, 
  SERV_DATA_SERVICE_COLLECTOR, 
  SERV_EVENT_RESPONDER_PROGRAMMER,
  SERV_EVENT_RESPONDER_ROBOT,
  SERV_NUMBER_OF_SERVICES        // Keep this value at the end of the
                                 // list, and we can automatically
                                 // account for how many services
                                 // there are available.
};

// Make a list of strings so that pretty-printing the service types is
// easy.  Note that serviceNames and serviceTypeTag need to be carefully
// coordinated in terms of order.  
//
// TODO: Find a more maintable way of doing this....not sure if there
// is one.  -- TLC
static char * serviceNames[5] = {"No service set", "Data Aggregator", "Data Collector", "E:R Programmer", "E:R Robot"};

// Enumerate the different possible broadcast types for the services.
// When a service is started by servStart(), it may or may not choose
// to broadcast its availability on the network.
typedef enum broadcastTypeTag broadcastType;
enum broadcastTypeTag {
  SERV_BROADCAST_ON,
  SERV_BROADCAST_OFF,
};


// Enumerate the different possible endpoints of communication.  
typedef enum endpointTypeTag endpointType;
enum endpointTypeTag {
  SERV_TCP_ACCEPTOR_ENDPOINT,
  SERV_TCP_CONNECTOR_ENDPOINT,
  SERV_UDP_BROADCAST_ENDPOINT,
  SERV_UDP_LISTENER_ENDPOINT
}; 


#define SERV_HANDLER_NOT_SET (-1)  // A failed socket call returns -1.  Thus, use -1 to
                                   // indicate that the socket field has not yet been set
                                   // for a service handler.

// Define the transport protocol ports that the services will be using.
#define SERV_DATA_SERVICE_PORT "10006"
#define SERV_EVENT_RESPONDER_SERVICE_PORT "10007"
#define SERV_PORT_NOT_SET ""

// Entities in the system access services via a service handler.
// Define the service handler type.
typedef struct serviceHandler {  
  serviceType typeOfService; /**< The type of service (see serviceType
				enum) */

  int eh;                    /**< The endpoint handler of the created
				  connection.  This is unused once the
				  connection is established.  It may
				  be worthwhile to keep in case the
				  connection is dropped and
				  restablishment is necessary. */

  int bh;                    /**< The endpoint handler of the
				   broadcaster or broadcast listener
				   associated with this
				   serviceHandler */

  int handler;               /**< The handle of the established
				connection. */

  char port[SERV_MAX_PORT_LENGTH];  /**< The port number for the
				       connection */

  char ip[SERV_MAX_IP_LENGTH];      /**< The IP address of the local
				       end of the connection. */

  char bcaddr[SERV_MAX_IP_LENGTH];  /**< The broadcast IP address of the
				       local end of the connection. */

  char rip[SERV_MAX_IP_LENGTH];     /**< The IP address of the remote
				       end of the connection.  */

  char interface[SERV_MAX_INTERFACE_LENGTH]; /**< The interface name
						of the local end of
						the connection,
						(e.g. en1 or
						wlan0); */

  pthread_t broadcast;              /**< The thread that will handle
				         either broadcasting a service
				         or listening for a
				         broadcast */

  pthread_t connection;             /**< The thread that will handle
				       either waiting for a connection
				       or establishing a connection */

  pthread_t service;                /**< The thread that will handle
				       the actual service
				       functionality.  */

  mqd_t mqd;                        /**< The message queue used by
				       this service endpoint so that
				       its own threads may communicate
				       with each other */

  int ready;                        /**< An indication, TRUE or FALSE,
				       for whether or not the service
				       has been activated */

} serviceHandler;


/**
 * Function prototypes.  See services.c for details on these
 * functions.
 */
void servHandlerPrintSocketAddr(struct sockaddr * sa);
void * servGetInAddr(struct sockaddr * sa);

int servStart(serviceType type, char * name, broadcastType b, serviceHandler * sh);

char * servToPort(serviceType s);
void * servToActivate(serviceType s);

int servHandlerSetDefaults(serviceHandler * sh);
int servHandlerSetInterface(serviceHandler * sh, char * name);
int servHandlerSetPort(serviceHandler * sh, char * port);
int servHandlerSetService(serviceHandler * sh, serviceType type);
int servHandlerSetEndpointHandle(serviceHandler * sh, int eh);
int servHandlerSetBroadcastHandle(serviceHandler * sh, int bh);
int servHandlerSetRemoteIP(serviceHandler * sh, char * rip);

int servHandlerPrint(serviceHandler * sh);
int servQueryIP(serviceHandler * sh);

int servCreateEndpoint(endpointType type, char * port, serviceHandler * sh);

/**
 * The generic activate() and all the endpoint-specific activation
 *  routines.  Again, see services.c for details on these functions.
 */
int servActivate(serviceHandler * sh);
int dsAggregatorActivate(serviceHandler * sh);
int dsCollectorActivate(serviceHandler * sh);
int erProgrammerActivate(serviceHandler * sh);
int erRobotActivate(serviceHandler * sh);


/**
 * Data Service API.  Again, see services.c for details on these
 * functions.
 */
int dsWrite(serviceHandler * sh, char * src);
void dsAggregatorGetDataFromCollector(serviceHandler * sh);


/**
 * Event:Responder Service API.  Again, see services.c for details on
 * these functions.
 */
int erRobotService(serviceHandler * sh);
int erRead(serviceHandler * sh, char * rb);
int erWrite(serviceHandler * sh, char * src);

#endif
