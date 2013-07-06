// For the sake of simplicty we'll just have a monolithic client file

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUF_SIZE 1024

int main (int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <IP Address> <Port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int serverSocket;
    struct sockaddr_in serverData = {'\0'};
    struct hostent *host;

    serverData.sin_family = AF_INET;
    host = gethostbyname(argv[1]);
    memmove(&(serverData.sin_addr.s_addr), host->h_addr, host->h_length);
    serverData.sin_port = htons(atoi(argv[2]));
    serverSocket = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);

    if(connect(serverSocket,(struct sockaddr *)&serverData,sizeof(serverData))<0) {
        int errsv = errno;
        printf("Error connecting due to %d\n",errsv);
        close(serverSocket);
        return EXIT_FAILURE;
    } else {
        char buffer[BUF_SIZE];
        int bytesReceived;
        while(true) {
            printf("Send> ");
            fgets(buffer,BUF_SIZE,stdin);
            send(serverSocket,buffer,strlen(buffer),0);
            bytesReceived = recv(serverSocket,buffer,BUF_SIZE,0);
            buffer[bytesReceived] = '\0';
            printf("Received: %s\n",buffer);
        }
        close(serverSocket);
    }
    return EXIT_SUCCESS;
}
