/*
** supervisorClient.c -- a stream socket client demo
*/
#include "communication.h"

int main(int argc, char *argv[])
{
        int sockfd, numbytes;  
        char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];
	char cmd[1];
	char input = '\0';
	FILE* fp = fopen("sensorFile.txt", "w");
	if (argc != 2) {
	    fprintf(stderr,"usage: client hostname\n");
	    exit(1);
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("client: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "client: failed to connect\n");
		return 2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("client: connecting to %s\n", s);

	freeaddrinfo(servinfo); // all done with this structure

	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	    perror("recv");
	    exit(1);
	}

	buf[numbytes] = '\0';

	printf("client: received '%s'\n",buf);

	while(1)
    {	       
		printf("Receiving sensor data.\n");
	    numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0);
	    printf("client: sensor data: '%s'\n", buf);	   
	    printf("numbytes: %d\n", numbytes);
	    fprintf(fp, "%s", buf);
	    fflush(fp);

		Episode * ep = (Episode*) malloc(sizeof(Episode));

		int retVal;

		retVal = parseEpisode(ep, buf);
		if(retVal != 0)
		{
			char errBuf[1024];
			sprintf(errBuf, "Error in parsing: %s\n", buf);
			perror(errBuf);
			exit(retVal);
		}

	    retVal = tick(ep);
		if(retVal != 0)
		{
			perror("Tick returned error\n");
			exit(retVal);
		}

	    if(ep->cmd < CMD_NO_OP || ep->cmd >= NUM_COMMANDS)
        {
			perror("Illegal command");
			exit(ep->cmd);
        }
    	if(send(sockfd, cmd, 1, 0) == -1)
		{
        	perror("send");
		}
	    
	  	printf("The command value sent was: %d\n", cmd[0]);

		free(ep);
	}
	send(sockfd, &input, 1, 0);
	close(sockfd);

	return 0;
}//main

