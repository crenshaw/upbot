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

  int result = -1;

  // Set the default values for the serviceHandler.
  servHandlerSetDefaults(sh);

  // Create an endpoint of communication.
  if((result = servCreateEndpoint(SERV_TCP_ENDPOINT, port, sh)) != SERV_SUCCESS) return result;

  // Endpoint of communication has been successfully created, so we can set the port
  // field in the serviceHandler.
  servHandlerSetPort(sh, port);

  // Populate sh with the IP of this device.  Note that the IP bound
  // to the socket was likely 0.0.0.0 or 127.0.0.0 since NULL was the
  // first parameter in the getaddrinfo() call.  Instead, we want the
  // IP of the ethernet or wireless interface on this machine.  
  servQueryIP(sh);

#ifdef DONOTCOMPILE
  // TODO: The sig-handler setup only needs to happen once, not once
  // per call to accCreateConnection!  In fact, as this code becomes
  // multi-threaded instead of multi-processed, I'm not even certain
  // if it is necessary.  For now, I will conditionally compile it out
  // of the source until I determine its proper need and place. -TLC

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
#endif

  // The passive-mode endpoint has successfully been created.  Now it
  // is time to listen and wait for another entity to approach and
  // accept their connection.
  if (listen(sh->eh, ACC_BACKLOG) == -1) return ACC_SOCK_LISTEN_FAILURE;  // listen() call failed.

  return ACC_SUCCESS;
}

/**
 * accAcceptConnection
 *
 * Given a fully-populated serviceHandler, sh, wait for an approach on
 * the endpoint handler (i.e., socket) prescribed by sh.
 *
 * @returns ACC_SOCK_ACCEPT_FAILURE if the accept() call fails.  Otherwise,
 * ACC_SUCCESS to indicate a succesfully established connection.
 * 
 */
int accAcceptConnection(serviceHandler * sh)
{
  char pee[INET6_ADDRSTRLEN];
  struct sockaddr_storage theirAddr; // connector's address information
  int newSock = -1;
  socklen_t size;
  
  // Wait for an approach.  Note that the accept() call blocks until a
  // connection is established.
  while (newSock == -1) 
    {      
      size = sizeof(theirAddr);
      newSock = accept(sh->eh, (struct sockaddr *)&theirAddr, &size);
      if (newSock == -1)
	{
	  return ACC_SOCK_ACCEPT_FAILURE;
	}
    }

  inet_ntop(theirAddr.ss_family, accGetInAddr((struct sockaddr *)&theirAddr), pee, sizeof(pee));

  return ACC_SUCCESS;
}


/**
 * accBroadcastService
 *
 * Broadcast the service on the network; the service's type, IP, and
 * port are described by sh.
 */
int accBroadcastService(serviceHandler * sh)
{
  
  int result = -1;  
 
  int s;      /* Socket */  

  struct sockaddr_in adr_bc;  /* AF_INET */  
  int len_bc;

  static char * bc_addr = "255.255.255.255:20";
  static int so_broadcast = 1;  
  char * bcbuf = "Broadcasting excellent services since 2013!";

  len_bc = sizeof adr_bc;  

  // Manually create a broadcast address?
  result = mkaddr(  
		  &adr_bc, /* Returned address */  
		  &len_bc, /* Returned length */  
		  bc_addr, /* Input string addr */  
		  "udp"); /* UDP protocol */  
  
  if ( result == -1 )  
    {
      printf("Bad broadcast address");  
      return -1;
    }

  servCreateEndpoint(SERV_UDP_ENDPOINT, "20", sh);

#ifdef DONOTCOMPILE
  // Create a UDP socket for broadcasting.
  s = socket(AF_INET,SOCK_DGRAM,0);  
  
  if ( s == -1 )  
    return -1;
  
  // Set options to broadcast
  result = setsockopt(s,  
		      SOL_SOCKET,  
		      SO_BROADCAST,  
		      &so_broadcast,  
		      sizeof so_broadcast);  
  
  if ( result == -1 )  return -1;
#endif
  
#ifdef DONOTCOMPILE
  // Bind
  result = bind(s, (struct sockaddr *)&adr_srvr, len_srvr);  
  
  if ( result == -1 )  
    displayError("bind()");  
#endif
  
  while(1)
    {
      
      /* 
       * Broadcast the info
       */  
      result = sendto(sh->eh,  
		      bcbuf,  
		      strlen(bcbuf),  
		      0,  
		      (struct sockaddr *)&adr_bc,  
		      len_bc);   
      
      if ( result == -1 )  
	{
	  printf("sendto\n");
	  return -1;
	}
      
      sleep(2);
    }
  
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

