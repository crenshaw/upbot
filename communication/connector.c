/**
 * connector.c
 *
 *
 * Provides location transparency to application-level components. As
 * noted by Schmidt in "Applying Design Patterns to Flexibly Configure
 * Network Services, the connector initiates a connection with a
 * passive-mode endpoint and activates the appropriate serviceHandler.
 *
 * Once a connection is established, neither the application nor the
 * service utilize the acceptor until another connection must be
 * established.
 *
 * @author Tanya L. Crenshaw
 * @since July 2013
 */

#define DEBUG 1

#include "connector.h"
#include "services.h"

/**
 * conListenForService
 *
 * Listen for broadcasts of a particular service.
 *
 * @param[in] type the type of service to listen for.  
 *
 * @param[in] sh the serviceHandler that will be populated as a result
 * of this call.
 *
 * @returns an indication of success or failure.
 */
int conListenForService(serviceType type, serviceHandler * sh)
{

#define MAXBUFLEN 100

  int sockfd;
  struct addrinfo hints, *servinfo, *p;
  int rv;
  int numbytes;
  struct sockaddr_storage their_addr;
  char buf[MAXBUFLEN];
  socklen_t addr_len;
  char s[INET6_ADDRSTRLEN];

  if(sh == NULL)
    return -1;  //TODO: Improve error codes for this call.

  // Create a UDP endpoint of communication for listening for
  // broadcasted services.
  int status = servCreateEndpoint(SERV_UDP_LISTENER_ENDPOINT, sh->port, sh);

#ifdef DEBUG
  servHandlerPrint(sh);  
#endif

  if(status != SERV_SUCCESS)
    {
      printf("Failed to make endpoint.\nStatus of endpoint creation: %d\n", status);
      return -1;
    }

  // The subsequent code based on tutorial from Beej, listener.c.
  // For more details, see: http://beej.us/guide/bgnet/examples/listener.c
  printf("listener: waiting to recvfrom...\n");

  addr_len = sizeof their_addr;
  if ((numbytes = recvfrom(sh->bh, buf, MAXBUFLEN-1 , 0,
                           (struct sockaddr *)&their_addr, &addr_len)) == -1) {
    perror("recvfrom");
    exit(1);
  }

  // Copy their address to the remote IP field of this service handler.
  strncpy(sh->rip, inet_ntop(their_addr.ss_family, servGetInAddr((struct sockaddr *)&their_addr), s, sizeof s), SERV_MAX_IP_LENGTH);
  
  // Assure the stringified IP is null-terminated
  sh->rip[SERV_MAX_IP_LENGTH - 1] = '\0';


  // Print some information for sanity checking.
  printf("listener: got packet from %s\n", sh->rip);
  printf("listener: packet is %d bytes long\n", numbytes);
  buf[numbytes] = '\0';
  printf("listener: packet contains \"%s\"\n", buf);


  // I do not need the broadcast endpoint anymore.  Close it and
  // update the service handler field to indicate the broadcast
  // endpoint handler is unset.
  close(sh->bh);
  servHandlerSetBroadcastHandle(sh, SERV_HANDLER_NOT_SET);
  
  return 0; 
}


/**
 * conInitiateConnection
 *
 * Based on D. Schmidt's "Acceptor-Connector" design pattern.
 * 
 * 1. Establish a connection with a passive-mode endpoint.
 * 
 * 2. Activate a service handler for this connection and populate the
 * parameter sh.
 *
 * @param[in/out] sh a partially populated serviceHandler that will be
 * fully populated by this call.  The serviceHandler should already
 * indicate the type of service and the remote ip and port to connect
 * to.  Subsequent read and write operations on this connection are
 * parameterized by this handler.
 *
 * @returns an indication of success or failure, based on whatever
 * servCreateEndpoint() returns.
 * 
 */
int conInitiateConnection(serviceHandler * sh)
{

  int status = -1;

  // Create an connector endpoint of communication to make a full
  // connection with an acceptor endpoint.

  printf("connector.c: Attempting to create TCP endpoint\n");

  // And...

  // Did it work?
  if((status = servCreateEndpoint(SERV_TCP_CONNECTOR_ENDPOINT, sh->port, sh)) != SERV_SUCCESS) {
    printf("connector.c: %d. Failed to create endpoint \n", __LINE__);  
    return status;
  }
  
  // Create a thread to activate the functionality that will be
  // servicing this connection from this point forward. The function
  // servActivate() will look at the type of service in sh and create
  // a thread that will call the correct activate() function to
  // service the endpoint from this point forward.
  status = servActivate(sh);

  return status;

}
