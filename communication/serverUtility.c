#include "communication.h"

static int true = 1;
static int false = 0;

// get_in_addr()
//   Get sockaddr, IPv4 or IPv6:
//
void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


// serv_listen()
//   Create a server endpoint of communication.
//   Adapted from: "Advance Programming in the UNIX Environment."  page 501
//   as well as "Beej's Guide to Network Programming."

int createListener(const char * name)
{
  int status;
  int s;
  int yes = 1;
 
  // Defining the fields for socket structs is challenging.  The fields depend of the
  // address, type of socket, and communication protocol.  This function uses getaddrinfo()
  // to aid in defining the struct socket fields.  This function fills a struct of
  // type addrinfo.
  struct addrinfo hints;
  struct addrinfo * servinfo, *p; 
  
  // Initialize the hints structure based on what little we care about
  // in terms of the socket.  The goal is to listen in on host's IP
  // address on port 22.
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;        // don't care if its IPv4 or IPv6.
  hints.ai_socktype = SOCK_STREAM;    // stream-style sockets.
  hints.ai_flags = AI_PASSIVE;        // fill in my IP automatically.

  if((status = getaddrinfo(NULL, name, &hints, &servinfo)) != 0)
    {
      return -1;
    }

  // Servinfo now points to a linked list of 1 or more struct
  // addrinfos.  Note that they may not all be valid.  Scan through
  // the servinfo until something makes sense.
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((s = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("server: socket");
      continue;
    }
    
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("setsockopt");
      exit(1);
    }
    
    if (bind(s, p->ai_addr, p->ai_addrlen) == -1) {
      close(s);
      perror("server: bind");
      continue;
      }
    
    break;

  }

  if (p == NULL)  {
    fprintf(stderr, "server: failed to bind\n");
    return -1;
  }

  // Don't need servinfo anymore
  freeaddrinfo(servinfo);

  return s;
}


// sigchld_handler()
//  This function is an interrupt-driven function which 
//  reaps dead children that are forked by the server.
void sigchld_handler(int s)
{
  while(waitpid(-1, NULL, WNOHANG) > 0);
}


//writeCommandToFile()
//Takes a command and writes it to a file.
void writeCommandToFile(char* cmd, FILE* fp)
{
  if(fp != NULL && cmd[0] != '\0' && cmd[0] != ssQuit)
    {
      fprintf(fp, "%c", cmd[0]);
      fprintf(fp, "%s", " ");
      fflush(fp);
    }
}

/*checkValue
 *
 *      checks the value of the character being passed
 *      to ensure that it is a valid command and returns
 *      1, else if not valid command return 0
 */
int checkValue(char v)
{
  if(v == ssDriveLow)
    return true;

  if(v == ssDriveMed)
    return true;

  if(v == ssDriveHigh)
    return true;

  if(v == ssDriveBackwardLow)
    return true;

  if(v == ssDriveBackwardMed)
    return true;

  if(v == ssDriveBackwardHigh)
    return true;

  if(v == ssTurnCwise)
    return true;

  if(v == ssTurnCCwise)
    return true;

  if(v == ssStop)
    return true;

  if(v == ssQuit)
    return true;

  return false;
}
