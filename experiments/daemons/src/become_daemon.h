#define BD_MAX_CLOSE  8192          /* Maximum file descriptors to close if
                                       sysconf(_SC_OPEN_MAX) is indeterminate */

#define BUFF_SIZE 1024              // Size of the message buffer

#define QUEUE_SIZE 10               // Queue size for messages

#include <arpa/inet.h>              // For internet related things
#include <errno.h>                  // Because errors and stuff
#include <fcntl.h>                  // File control options
#include <netinet/in.h>             // TCP/IP related things
#include <pthread.h>                // POSIX threads
#include <stdbool.h>                // Booleans!
#include <stdio.h>                  // Print statements
#include <stdlib.h>                 // Some nice constants like EXIT_SUCCESS
#include <string.h>                 // For strncpy and other things
#include <syslog.h>                 // Access to the syslog library function
#include <sys/types.h>              // Various necessary data types
#include <sys/socket.h>             // Socket functions!
#include <unistd.h>                 // Symbolic constant types

int becomeDaemon();
void* connection(void* socket);
