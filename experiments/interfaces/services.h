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

#ifndef _SERVICES_H_
#define _SERVICES_H_

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

#include <arpa/inet.h>

#include <net/if.h>
#include <netinet/in.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/mman.h>

/**
 *  CONSTANT DEFINITIONS.  All constants in this file should begin
 *  with 'ER' to indicate their membership in eventresponder.h  
 */
#define SERV_MAX_PORT_LENGTH 6 // e.g., "10005"
#define SERV_MAX_IP_LENGTH 17  // e.g., "10.81.3.131"
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

#define SERV_SUCCESS (0)

#define SERV_CONNECT_REMOTE (1)
#define SERV_NO_CONNECT (0)

#define SERV_NO_REMOTE_CONTINUE (1)
#define SERV_NO_REMOTE_FAIL (0)





// Types of endpoints of communication.
#define SERV_TCP_ENDPOINT 0
#define SERV_UDP_ENDPOINT 1

#define SERV_HANDLER_NOT_SET (-1)  // A failed socket call returns -1.  Thus, use -1 to
                                   // indicate that the socket field has not yet been set
                                   // for a service handler.

// Enumerate the different possible service types.  Note that the
// compiler shall assign integer values to the terms
// 'SERV_SERVICE_NOT_SET', 'SERV_DATA_SERVICE_AGGREGATOR' and so
// on.
enum serviceTypeTag {
  SERV_SERVICE_NOT_SET,
  SERV_DATA_SERVICE_AGGREGATOR, 
  SERV_DATA_SERVICE_COLLECTOR, 
  SERV_EVENT_RESPONDER_SERVICE,
  SERV_NUMBER_OF_SERVICES        // Keep this value at the end of the
                                 // list, and we can automatically
                                 // account for how many services
                                 // there are available.
};

// Make a list of strings so that pretty-printing the service types is
// easy.  Note that serviceNames and serviceTypeTag need to be carefully
// coordinated in terms of order.  
//
// TODO: Find a more maintable way of doing this.
static char * serviceNames[4] = {"No service set", "Data Aggregator", "Data Collector", "Event:Responder"};


typedef enum serviceTypeTag serviceType;

// Entities in the system access services via a service handler.  Define the
// service handler type.
typedef struct serviceHandler {  
  serviceType typeOfService; /**< The type of service (see serviceType enum) */
  int eh;                    /**< The endpoint handler of the created connection.
				  This is unused once the connection is established.
				  It may be worthwhile to keep in case the connection
				  is dropped and restablishment is necessary. */
  int handler;               /**< The handle of the established connection. */

  char port[SERV_MAX_PORT_LENGTH];  /**< The original port number for the connection */

  char ip[SERV_MAX_IP_LENGTH];      /**< The original IP for the connection. */

  char interface[SERV_MAX_INTERFACE_LENGTH]; /**< The interface name of the connection, (e.g. en1 or wlan0); */

} serviceHandler;


/**
 * Function prototypes.  See services.c for details on
 * this/these functions.
 */
int servHandlerSetDefaults(serviceHandler * sh);
int servHandlerSetPort(serviceHandler * sh, char * port);
int servHandlerSetEndpoint(serviceHandler * sh, int eh);
int servHandlerPrint(serviceHandler * sh);
int servQueryIP(serviceHandler * sh);
int servCreateEndpoint(int type, char * port, serviceHandler * sh);

int dsCreateCollector(int connectRemote, int continueLocally, serviceHandler * sh);

#endif
