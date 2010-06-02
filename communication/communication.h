#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "../roomba/roomba.h"

#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H
void *get_in_addr(struct sockaddr *sa);
int createListener(const char * name);
void sigchld_handler(int s);
void writeCommandToFile(char* cmd, FILE* fp);
int checkValue(char v);
int readSensorDataFromFile(char* data, FILE* fp);
int receiveDataAndStore(int newSock, char* cmdBuf, char* sensData, FILE* cmdFile, FILE* sensorFile, int* fd, caddr_t shm);
int createSharedMem(char * deviceName, caddr_t* area);
int createServer(void);
int establishConnection(int s);
int readSensorDataFromSharedMemory(char* data, caddr_t shm);

//Human issued commands from the keyboard
#define ssDriveLow 'i'
#define ssDriveMed 'w'
#define ssDriveHigh 'W'
#define ssDriveBackwardLow 'k'
#define ssDriveBackwardMed 's'
#define ssDriveBackwardHigh 'S'
#define ssTurnCwise 'd'
#define ssTurnCCwise 'a'
#define ssStop 'x'
#define ssQuit 'q'
#define BACKLOG 10
#define MSG "And indeed there will be time\nTo wonder, 'Do I dare?' and, 'Do I dare?'\n"
#define PORT "8080"
#define MAXDATASIZE 100 // max number of bytes we can get at once 

// Artificial intelligence issued commands from an external process
#define CMD_FORWARD 0x01
#define CMD_BACKWARD 0x02
#define CMD_LEFT 0x03
#define CMD_RIGHT 0x04
#define CMD_BLINK 0x05
#define CMD_ADJUST_LEFT 0x06
#define CMD_ADJUST_RIGHT 0x07

#endif
