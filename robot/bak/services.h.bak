/**
 * services.h
 *
 * There are multiple types of services executing on the system that
 * allow flexible connections between robots and supervisors.  This
 * file contains types and function prototypes generic to all services
 * in the UPBOT system.
 * 
 * @author Tanya L. Crenshaw
 * @since July 01, 2013
 */

#ifndef _SERVICES_H_
#define _SERVICES_H_

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

#endif
