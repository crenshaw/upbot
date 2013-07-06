#include "become_daemon.h"

int main(int argc, char *argv[]) {

    // Check args
    if (argc != 2) {
        printf( "Usage %s <Port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Become a daemon
    becomeDaemon();

    // Start server
    int serverSocket;
    int clientSocket;
    struct sockaddr_in serverData = {'\0'}; //inet address for server
    struct sockaddr_in clientData = {'\0'}; //inet address for client

    serverData = (struct sockaddr_in){
        .sin_family = AF_INET, //IPv4
        .sin_addr.s_addr = htonl(INADDR_ANY), //accept clients from any address
        .sin_port = htons(atoi(argv[1])), //port for incoming packets
    };

    // create a socket of type tcp (stream)
    serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    // bind the socket
    if(bind(serverSocket, (struct sockaddr*)&serverData, sizeof(serverData)) < 0) {
        syslog(LOG_DAEMON | LOG_DEBUG, "Error: Unable to bind socket.");
        close(serverSocket);
        return EXIT_FAILURE;
    } else if (listen(serverSocket, QUEUE_SIZE) < 0 ) {
        syslog(LOG_DAEMON | LOG_DEBUG, "Error: Unable to listen on port %s", argv[1]);
        close(serverSocket);
        return EXIT_FAILURE;
    } else {
        int clientDataLength = sizeof(clientData);
        while (true) {
            clientSocket = accept(serverSocket, (struct sockaddr*)&clientData, &clientDataLength);
            syslog(LOG_DAEMON | LOG_DEBUG, "Accepted connection from %s:%d",
                    inet_ntoa(clientData.sin_addr),
                    ntohs(clientData.sin_port));
            pthread_t pth; //thread id
            pthread_create(&pth, NULL, (void* (*)(void*))&connection, &clientSocket);
        }
    }

    return EXIT_SUCCESS;
}

void* connection(void* socket) {
    int clientSocket = *(int*)socket;

    //buffer for i/o data
    char buffer[BUFF_SIZE];
    int bytesReceived;

    while ( 0 < (bytesReceived = recv(clientSocket, buffer, BUFF_SIZE, 0))) {
        buffer[bytesReceived] = '\0';
        syslog(LOG_DAEMON | LOG_DEBUG, "Received %s", buffer);

        // send a response
        char replyText[] = "Packet received!";
        strncpy(buffer,replyText,strlen(replyText)+1);
        syslog(LOG_DAEMON | LOG_DEBUG, "Replying with %s", buffer);

        send(clientSocket,buffer,strlen(replyText),0);
    }
    syslog(LOG_DAEMON | LOG_DEBUG, "Connection to client closed", buffer);
    close(clientSocket);
    return NULL;
}
