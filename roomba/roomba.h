#include <stdio.h>
#include <sys/types.h>
#ifndef _GUMSTIX_H
#define _GUMSTIX_H

/* In utility.c */
int defineSongs();
int openPort();
int closePort();
char readAndExecute(FILE *fp);
void byteTx(char value);
void byteRx(char* buffer, int nbytes, int iter);
void initialize();
void *ReaderThread( void *param);
void calcFileLoc(char c);
char* getTime();
void fprintBinaryAsString(FILE* fp, int n);
int checkSensorData(char *x);
void writeSensorDataToFile(char* sensorArray, FILE* fp, char* currTime);
void writeSensorDataToSharedMemory(char* sensorArray, caddr_t shm, char* currTime);

/* In move.c */
int driveStraightWithFeedback(int velocity);
void driveStraightUntil(int sec, int speed);
void driveStraight(int velocity);
void turnCounterClockwise(int degrees);
void turnClockwise(int degrees);
void turn(int direction, int degrees);
void driveBackwardsUntil(int sec, int speed);
void driveBackwards(int speed);
void stop();
void adjustLEFT();
void adjustRIGHT();

/* In led.c */
void setLED(int powerSetting, int playSetting, int advanceSetting);
void blinkLED();

/* In nerves.c */
int nerves(caddr_t cmdArea, caddr_t sensArea, pid_t pid);

/* In song.c */
void song();

#define HIGH_BYTE 0
#define LOW_BYTE 1
#define FALSE 0
#define TRUE 1

#define TURN_LEFT 0
#define TURN_RIGHT 1

#define ACTIVE_SENSOR 1

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
#define CmdStream   148
#define CmdToggleStream    150


// iRobot Create Sensor Data Values
#define SENSOR_BUMP_LEFT   0x02
#define SENSOR_BUMP_RIGHT  0x01
#define SENSOR_BUMP_BOTH   0x03
#define SENSOR_WHEELDROP_RIGHT 0x04
#define SENSOR_WHEELDROP_LEFT  0x08
#define SENSOR_WHEELDROP_BOTH  0x0C
#define SENSOR_WHEELDROP_CASTER 0x10
#define SENSOR_BUMPS_WHEELDROPS 0x001F

// TLC: The following define is deprecated.  Remove it
// once it becomes clear that no other file is using it.
// 'SENSOR_BUMP_BOTH should be used in its place.
//#define BUMP_SENSORS 0x03

// Baud Rate definitions according to the
// iRobot Serial Command Interface
#define BaudRate_9600   5
#define BaudRate_57600  10
#define BaudRate_115200 11

// iRobot Create Speed Values
#define HIGH_SPEED 0x01F3
//#define MED_SPEED 0x012c
#define MED_SPEED 0x00D0
#define LOW_SPEED 0x0064
#define HIGH_SPEED_BACK 0xFE0C
//#define MED_SPEED_BACK 0xFED4
#define MED_SPEED_BACK 0xFF50
#define LOW_SPEED_BACK 0xFF9C

// LED values
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

// Drive values
#define HIGH 3	//+-500mm/s
#define MED 2	//+-300mm/s
#define LOW 1	//+-100mm/s
#define FORWARD 1
#define BACKWARD -1
#define CLOCKWISE 90
#define CCLOCKWISE -90
#define STRAIGHT 0x8000
#define TILE_DISTANCE 24 //length of tile in mm

// Sensor packet indices for group 1
#define SP_GROUP_ONE 1
#define SP_G1_BUMPS_WHEELDROPS 0
#define SP_G1_CLIFF_LEFT 2
#define SP_G1_CLIFF_FRONT_LEFT 3
#define SP_G1_CLIFF_FRONT_RIGHT 4
#define SP_G1_CLIFF_RIGHT 5
#define SP_G1_VIRTUAL_WALL 6

// Sensor packet numbers according to the 
// iRobot Open Interface
#define SP_BUMPS_WHEELDROPS 7
#define SP_CLIFF_LEFT 9
#define SP_CLIFF_FRONT_LEFT 10
#define SP_CLIFF_FRONT_RIGHT 11
#define SP_CLIFF_RIGHT 12
#define SP_VIRTUAL_WALL 13
#define SP_REQUESTED_VELOCITY 39
#define SP_CLIFF_LEFT_SIGNAL 28
#define SP_CLIFF_F_LEFT_SIGNAL 29
#define SP_CLIFF_F_RIGHT_SIGNAL 30
#define SP_CLIFF_RIGHT_SIGNAL 31

// Timing values
#define HALF_SECOND 500000
#define QUARTER_SECOND 250000
#define EIGHTH_SECOND 125000
#define DEGREES_45 540000
#define DEGREES_90 1080000
#define ADJUST_AMOUNT 166666

#define STOP_MACRO {byteTx(CmdDrive); byteTx(0x00); byteTx(0x00); byteTx(STRAIGHT >> 8); byteTx(STRAIGHT & 0x00FF);}
#define PAUSE_STREAM_MACRO {byteTx(CmdToggleStream); byteTx(0);}
#define RESUME_STREAM_MACRO {byteTx(CmdToggleStream); byteTx(1);}

#endif
