/**
 * acceptor.h
 *
 * Provides the interfaces necessary to connect to remote services in
 * the UPBOT robotics system.  Entities that use the acceptor
 * establish a passive-mode endpoint and await connection from other
 * entities.
 *
 * @author Tanya L. Crenshaw
 * @since July 2013
 */

#ifndef _ACCEPTOR_H_
#define _ACCEPTOR_H_

#include "services.h"

/**
 *  CONSTANT DEFINITIONS.  All constants in this file should begin
 *  with 'ACC' to indicate their membership in eventresponder.h  
 */

#define ACC_SUCCESS 0

// ** The multitude of ways that accCreateConnection can fail. **
// TODO: I don't particularly like this goofy laundry list of errors
// and I wonder if I should just return -1 and leave it up to the user
// to use perror().  At the very least, I might consider using an enum
// so that these values are assigned by the compiler. -- TLC
#define ACC_SOCK_OPT_FAILURE (-1)
#define ACC_SOCK_BIND_FAILURE (-2)
#define ACC_BAD_PORT (-3)
#define ACC_NULL_SH (-4)
#define ACC_SOCK_LISTEN_FAILURE (-5)
#define ACC_SIGACTION_FAILURE (-6)
#define ACC_SOCK_ACCEPT_FAILURE (-7)

// The number of backlog requests accepted by any acceptor.
#define ACC_BACKLOG 10  

/**
 * Function prototypes.  See acceptor.c for details on
 * this/these functions.
 */
int accCreateConnection(char * port, serviceType type, serviceHandler * sh);
int accCompleteConnection(serviceHandler * sh);

#endif
