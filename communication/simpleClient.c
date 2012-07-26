/*
 * simpleClient.c -- a stream socket client demo
 *
 * Description: This client program accepts an IP address as a
 * command-line argument and then attempts to connect to a server at
 * that IP address at port PORT.  It receives up to MAXDATASIZE bytes
 * from the server, prints the information to the console, and then
 * exits.
 *
 * This simple program is largely based on Beej's Socket Programming
 * Tutorial.  Thanks Brian Hall!
 *
 * NOTE: Though not part of the iRobot testbed functionality, it is
 * included in the repository as a nice working example of a simple
 * socket client.
 * 
 */


// Header files
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>



// Constant definitions
#define PORT "8080"           // Destination port
#define MAXDATASIZE 100       // Message size, in bytes
#define REQUIRED_ARGUMENTS 2  // This program expects the program name and an IP address.


// Function prototypes
void *get_in_addr(struct sockaddr *sa);


// main() entrypoint of the program.
int main(int argc, char *argv[])
{

	int sockfd, numbytes;  
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];


	// Affirm that the correct number of command line arguments were
	// used.  If not, print the usage and exit the program.
	if (argc != REQUIRED_ARGUMENTS) {
	    fprintf(stderr,"usage: client hostname\n");
	    exit(EXIT_FAILURE);
	}


	// Prepare the struct addrinfo hints by zeroing out all the fields.
	memset(&hints, 0, sizeof hints);


	// Initialize the known fields of the struct addrinfo hints.
	// Specify the family as AF_UNSPEC and specify the socket type
	// as a SOCK_STREAM stream socket.
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;


	// Use getaddrinfo() to initialize the remaining fields of struct
	// addrinfo hints.  Note that getaddrinfo() will also place 
	// some useful information in the struct servinfo.
       	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return EXIT_FAILURE;
	}


	// Loop through all the results in struct servinfo 
	// and connect to the first viable entry.  Based on documentation for
	// getaddrinfo(), this is usually the first entry.
	for(p = servinfo; p != NULL; p = p->ai_next) {

	  // Attempt to make a socket.
	  if ((sockfd = socket(p->ai_family, p->ai_socktype,
			       p->ai_protocol)) == -1) 
	    {
	      perror("client: socket");
	      continue;
	    }

	  // Attempt to connect on that socket.
	  if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
	    {
	      close(sockfd);
	      perror("client: connect");
	      continue;
	  }
	  
	  break;

	}

	// If the program reaches this point and p is NULL, then no
	// connection was made.
	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return EXIT_FAILURE;
	}

       
	// Convert the IP address to a textual IP address that
	// can be printed to the console.  Sure, one could use the
	// argument that was passed to the program, but this 
	// ensures the exact IP we are connected to is the one
	// that gets converted and printed.  The result is placed
	// in s.
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	

	// Print the IP address to the console.
	printf("client: connecting to %s\n", s);

	// Free the struct servinfo; we've successfully connected, to the
	// results from getaddrinfo() are not needed anymore.
	freeaddrinfo(servinfo); 


	// Receive information from the server.
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	    perror("recv");
	    exit(1);
	}

	// NULL terminate the information from the server.
	buf[numbytes] = '\0';


	// Print the information to the console.
	printf("client: received '%s'\n",buf);


	close(sockfd);

	return 0;
}


//
// Function: get_in_addr()
// Description: Get an IPv4 or IPv6 socket address
// 
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
