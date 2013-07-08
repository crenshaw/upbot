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

/**
 *  CONSTANT DEFINITIONS.  All constants in this file should begin
 *  with 'ER' to indicate their membership in eventresponder.h  
 */
#define SERV_LOCAL_FAILURE (-1)
#define SERV_REMOTE_FAILURE (-2)
#define SERV_NULL_SH (-3)
#define SERV_NO_CONNECTION (-4)
#define SERV_SUCCESS (0)

#define SERV_CONNECT_REMOTE (1)
#define SERV_NO_CONNECT (0)

#define SERV_NO_REMOTE_CONTINUE (1)
#define SERV_NO_REMOTE_FAIL (0)

// Enumerate the different possible service types.  Note that the compiler
// shall assign integer values to the terms 'SERV_DATA_SERVICE_COLLATOR'
// and so on.
enum serviceTypeTag {
  SERV_DATA_SERVICE_COLLATOR, 
  SERV_DATA_SERVICE_COLLECTOR, 
  SERV_EVENT_RESPONDER_SERVICE
};

typedef enum serviceTypeTag serviceType;

// Entities in the system access services via a service handler.  Define the
// service handler type.
typedef struct serviceHandler {  
  serviceType typeOfService; /**< The type of service (see serviceType enum) */
  int handler;               /**< The handle of the established connection. */
} serviceHandler;


/**
 * Function prototypes.  See services.c for details on
 * this/these functions.
 */
int dsCreateCollector(int connectRemote, int continueLocally, serviceHandler * sh);

#endif
