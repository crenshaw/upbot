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
#include "commandQueue.h"

#ifndef _COMMUNICATION_H
#define _COMMUNICATION_H
void *get_in_addr(struct sockaddr *sa);
int createListener(const char * name);
void sigchld_handler(int s);
void writeCommandToFile(char* cmd, FILE* fp);
int checkValue(char v);
int readSensorDataFromFile(char* data, FILE* fp);
int receiveDataAndStore(int newSock, char* cmdBuf, char* sensData, FILE* cmdFile, FILE* sensorFile, int* fd, caddr_t sensArea, caddr_t cmdArea);
int createSharedMem(char * deviceName, caddr_t* area);
int createServer(void);
int establishConnection(int s);
int readSensorDataFromSharedMemory(char* data, caddr_t shm);
int checkArgName(int argc, char* argv[], char addresses[3][13]);



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
#define ssDriveDistance 'n'
#define ssAdjustLeft 'h'
#define ssAdjustRight 'j'
#define ssNoOp 'o'
#define ssBlinkLED 'l'
#define ssSong 'y'
#define NUM_TOTAL_CMDS 23  // The total number of commands issued to the iRobot (i.e. excluding ssQuit)

#define BACKLOG 10
#define MSG "And indeed there will be time\nTo wonder, 'Do I dare?' and, 'Do I dare?'\n"
#define PORT "8080"
#define MAXDATASIZE 100 // max number of bytes we can get at once 
#define SEMAPHORE_OFFSET 0x400


// Command definitions
#define CMD_ILLEGAL         0x0
#define CMD_NO_OP 			0x1
#define CMD_FORWARD			0x2
#define CMD_LEFT			0x3
#define CMD_RIGHT			0x4
#define CMD_ADJUST_LEFT		0x5
#define CMD_ADJUST_RIGHT	0x6
#define CMD_SONG            0x7
#define CMD_BLINK			0x8

#define NUM_COMMANDS		0x9	// Always make sure this is at the end
                                // The NUM_COMMANDS is the total number
                                // of commands that can be issued by
                                // the artificially intelligent
                                // supervisor.

#define LAST_MOBILE_CMD		0x6  //last command that is not generally used for
                                 //human debugging



// Sensor Data Indices
#define SNSR_IR			0x0
#define SNSR_CLIFF_RIGHT	0x1
#define SNSR_CLIFF_F_RIGHT	0x2
#define SNSR_CLIFF_F_LEFT	0x3
#define SNSR_CLIFF_LEFT		0x4
#define SNSR_CASTER		0x5
#define SNSR_DROP_LEFT		0x6
#define SNSR_DROP_RIGHT		0x7
#define SNSR_BUMP_LEFT		0x8
#define SNSR_BUMP_RIGHT		0x9
#define NUM_SENSORS		0xA	// Always make sure this is at the end

#endif
