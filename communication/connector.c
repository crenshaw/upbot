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

#include "connector.h"
#include "services.h"

/**
 * conListenForService
 *
 * Listen for broadcasts of a service.
 * 
 */
int conListenForService(serviceHandler * sh)
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

  // Set the default values for the serviceHandler.
  servHandlerSetDefaults(sh);

  // Create a UDP endpoint of communication for listening for
  // broadcasted services.
  int status = servCreateEndpoint(SERV_UDP_LISTENER_ENDPOINT, "10005", sh);

  printf("Status of endpoint: %d\n", status);

  // The subsequent code based on tutorial from Beej, listener.c.
  // For more details, see: http://beej.us/guide/bgnet/examples/listener.c
  printf("listener: waiting to recvfrom...\n");

  addr_len = sizeof their_addr;
  if ((numbytes = recvfrom(sh->bh, buf, MAXBUFLEN-1 , 0,
                           (struct sockaddr *)&their_addr, &addr_len)) == -1) {
    perror("recvfrom");
    exit(1);
  }

  printf("listener: got packet from %s\n",
         inet_ntop(their_addr.ss_family,
                   servGetInAddr((struct sockaddr *)&their_addr),
                   s, sizeof s));
  printf("listener: packet is %d bytes long\n", numbytes);
  buf[numbytes] = '\0';
  printf("listener: packet contains \"%s\"\n", buf);

  close(sh->bh);

  return 0; 
}


/**
 * conInitiateConnection
 *
 * Based on D. Schmidt's "Acceptor-Connector" design pattern.
 * 
 * 1. Establish a connection with a passive-mode endpoint.
 * 
 * 2. Activate a service handler for this connection and populate the parameter sh.
 *
 * @param[in] wellKnownIP a well-known IP for the passive-mode
 * endpoint (i.e., the IP address of the service must have already
 * been discovered).
 * 
 *
 * @param[in] port the port number to connect to.
 * 
 * @param[in] type the type of service at the passive-mode endpoint.
 * 
 * @param[out] sh the serviceHandler that will be fully populated by
 * this call.  Subsequent read and write operations on this connection
 * are parameterized by this handler.
 *
 * @returns an indication of success or failure.
 * 
 */
int conInitiateConnection(char * wellKnownIP, int port, serviceType type, serviceHandler * sh)
{
}
