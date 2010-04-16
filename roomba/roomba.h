#include <stdio.h>
#ifndef _GUMSTIX_H
#define _GUMSTIX_H

int defineSongs();
int openPort();
int closePort();
char readAndexecute(FILE *fp);
void byteTx(char value);
void byteRx(char* buffer, int nbytes, int iter);
void initialize();
void *ReaderThread( void *param);
//void calcFileLoc(int c);
void calcFileLoc(char c);
char* getTime();

int driveStraightWithFeedback(int velocity);
void driveStraightUntil(int sec, int speed);
void driveStraight(int velocity);
void turnCounterClockwise(int degrees);
void turnClockwise(int degrees);
void driveBackwardsUntil(int sec, int speed);
void driveBackwards(int speed);
void stop();

void setLED(int powerSetting, int playSetting, int advanceSetting);

#define HIGH_BYTE 0
#define LOW_BYTE 1

//Command Values
#define CmdStart    128
#define CmdBaud	    129
#define CmdControl  130
#define CmdSafe     131
#define CmdFull     132
#define CmdPower    133
#define CmdSpot     134
#define CmdClean    135
#define CmdMax      136
#define CmdDrive    137
#define CmdMotors   138
#define CmdLeds     139
#define CmdSong     140
#define CmdPlay     141
#define CmdSensors  142


// iRobot Create Sensor Data Values
#define SENSOR_BUMP_LEFT   0x02
#define SENSOR_BUMP_RIGHT  0x01
#define SENSOR_BUMP_BOTH   0x03
#define BUMP_SENSORS 0x03

//Baud Rate definitions according to the
//iRobot Serial Command Interface
#define BaudRate_9600   5
#define BaudRate_57600  10
#define BaudRate_115200 11

// iRobot Create Speed Values
#define HIGH_SPEED 0x01F3
#define MED_SPEED 0x012c
#define LOW_SPEED 0x0064

//Led values
#define PWR_RED 255
#define PWR_GREEN 0
#define PWR_BLUE 100
#define SET_ADVANCE 0x08
#define SET_PLAY 0x02
#define SET_ALL 0xFF
#define FULL_INTENSITY 255
#define OFF 0
#define RED 1
#define GREEN 2
#define PLAY_OFF 0
#define PLAY_ON 1
#define ADVANCE_OFF 0
#define ADVANCE_ON 1


//Drive values
#define HIGH 3	//+-500mm/s
#define MED 2	//+-300mm/s
#define LOW 1	//+-100mm/s
#define FORWARD 1
#define BACKWARD -1
#define CLOCKWISE 90
#define CCLOCKWISE -90

//Sensor packet numbers
#define SP_BUMPS_WHEELDROPS 7
#define SP_REQUESTED_VELOCITY 39

#endif
