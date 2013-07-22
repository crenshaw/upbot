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

  int z;  
  int x;  
  struct sockaddr_in adr;  /* AF_INET */  
  int len_inet;            /* length */  
  int s;                   /* Socket */  
  char dgram[1000];         /* Recv buffer */  
  static int so_reuseaddr = 1;  
  static char  
    *bc_addr = "255.255.255.255:10005";  

  // Set the default values for the serviceHandler.
  servHandlerSetDefaults(sh);

#ifdef DONOTCOMPILE  
  /* 
   * Create a UDP socket to use: 
   */  
  s = socket(AF_INET,SOCK_DGRAM,0);  
  if ( s == -1 )  
    perror("socket()");  
  
#endif

  // Create a UDP endpoint of communication for listening for
  // broadcasted services.
  int status = servCreateEndpoint(SERV_UDP_LISTENER_ENDPOINT, "10005", sh);


  printf("Status of endpoint: %d\n", status);

  /* 
   * Form the broadcast address: 
   */  
  len_inet = sizeof adr;  
  
  z = mkaddr(&adr,  
	     &len_inet,  
	     bc_addr,  
	     "udp");  
  
  if ( z == -1 )  
    perror("Bad broadcast address");  



  
  /* 
   * Allow multiple listeners on the 
   * broadcast address: 
   */  
  z = setsockopt(sh->bh,  
		 SOL_SOCKET,  
		 SO_REUSEADDR,  
		 &so_reuseaddr,  
		 sizeof so_reuseaddr);  
  
  if ( z == -1 )  
    perror("setsockopt(SO_REUSEADDR)");  

#ifdef DONOTCOMPILE        

  /* 
   * Bind our socket to the broadcast address: 
   */  
  z = bind(s,  
	   (struct sockaddr *)&adr,  
	   len_inet);  
  
  if ( z == -1 )  
    perror("bind(2)");  
#endif  
  
  while(1) {  
    /* 
     * Wait for a broadcast message: 
     */  
    z = recvfrom(sh->bh, /* Socket */  
		 dgram,  /* Receiving buffer */  
		 sizeof dgram,/* Max rcv buf size */  
		 0,      /* Flags: no options */  
		 (struct sockaddr *)&adr, /* Addr */  
		 &x);    /* Addr len, in & out */  
    
    if ( z < 0 )  
      {
	perror("recvfrom(2)"); /* else err */  
	exit(1);
      }
	
    dgram[1000] = '\0';
	
    fwrite(dgram,z,1,stdout);  
    putchar('\n');  
    
    fflush(stdout);  

  }  
  
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
