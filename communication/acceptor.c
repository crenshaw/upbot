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
 * accCreateConnection
 *
 * Create a passive-mode socket, bind it to a particular port number
 * on the calling host's IP address.
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
 * If the IP of this device cannot be determined, the function returns
 * SERV_NO_DEVICE_IP.
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

  // Sanity-check the incoming parameters.
  if(sh == NULL) return ACC_NULL_SH;
  if(port == NULL) return ACC_BAD_PORT;

  // Set the default values for the serviceHandler.
  servHandlerSetDefaults(sh);

  // Create an endpoint of communication.
  if((result = servCreateEndpoint(SERV_TCP_ACCEPTOR_ENDPOINT, port, sh)) != SERV_SUCCESS) return result;

  // Endpoint of communication has been successfully created, so we
  // can set the service type and port fields in the serviceHandler.
  servHandlerSetService(sh, type);
  servHandlerSetPort(sh, port);

  // Populate sh with the IP of this device.  Note that the IP bound
  // to the socket was likely 0.0.0.0 or 127.0.0.0 since NULL was the
  // first parameter in the getaddrinfo() call.  Instead, we want the
  // IP of the ethernet or wireless interface on this machine.  
  if(servQueryIP(sh) == SERV_NO_DEVICE_IP)
    {
      return SERV_NO_DEVICE_IP;  
    }

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
 * the endpoint handler (i.e., socket) prescribed by sh and set up
 * listening passively for the arrival of connection requests.  Since
 * listening passively can be a blocking call (i.e. accept()), it may
 * be worthwhile to use this function in a separate thread.
 *
 * To reiterate: This function blocks until a connection is
 * established!
 *
 * @returns ACC_SOCK_ACCEPT_FAILURE if the accept() call fails.  Otherwise,
 * ACC_SUCCESS to indicate a succesfully established connection.
 * 
 */
int accAcceptConnection(serviceHandler * sh)
{

  // TODO: This should run as long as I can accept more collectors.  Right now
  // it will only accept one collector.

  char p[INET6_ADDRSTRLEN];
  struct sockaddr_storage theirAddr; // connector's address information
  int newSock = -1;
  socklen_t size;

  printf("Waiting for connections...\n");

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

  inet_ntop(theirAddr.ss_family, servGetInAddr((struct sockaddr *)&theirAddr), p, sizeof(p));

  // Create a thread to activate the functionality that will be
  // servicing this connection from this point forward. The function
  // servActivate() will look at the type of service in sh and create
  // a thread that will call the correct activate() function to
  // service the endpoint from this point forward.
  int status = servActivate(sh);

  // TODO: Need to return status, not just blind success.
  
  // TODO:  This function represents a thread of execution that should
  // eventually be cleaned up. 
  
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

  // Extract the target port from the incoming service handler.
  char * port = sh->port;

  int howManyBroadcasts = 100;
  int result = -1;  
  int s;       

  // The broadcast address below has been observed to work
  // as a broadcast address on host IP 10.81.3.130.
  //static char * bc_addr = "10.81.3.255:10005";

  // The broadcast address below has been observed to work
  // as a broadcast address on host IP 10.12.19.1.
  //
  // TODO: Concatenate the broadcast address to the 
  // port extracted from the incoming service handler.
  static char * bc_addr = "255.255.255.255:10005";

  struct sockaddr_in adr_bc;  /* AF_INET */  
  int len_bc;

  static int so_broadcast = 1;  
  char * bcbuf = "Broadcasting excellent services since 2013!";

  len_bc = sizeof adr_bc;  

  // Create a broadcast address.  
  if( mkaddr(&adr_bc, &len_bc, bc_addr, "udp") == -1)
    {      
      return -1;
    }

  // Create a UDP endpoint of communication for broadcasting the
  // service.  Use the port number extracted from the incoming
  // service handler.
  servCreateEndpoint(SERV_UDP_BROADCAST_ENDPOINT, port, sh);
  
  while(howManyBroadcasts)
    {
      
      /* 
       * Broadcast the info
       */  
      result = sendto(sh->bh,  
		      bcbuf,  
		      strlen(bcbuf),  
		      0,  
		      (struct sockaddr *)&adr_bc,  
		      len_bc);   
      
      if ( result == -1 )  
	{
	  perror("Cannot send broadcast: ");
	}
      
      sleep(2);

      howManyBroadcasts--;
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

