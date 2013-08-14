#include "responders.h"

/**
 * respondStop
 * 
 * Default responder function.  Stop the robot.
 */ 
void respondStop(void)
{
	//printf("stop\n");
	stop();
	usleep(30);
}

void respondDriveLow(void) {
	//printf("Drive low\n");
	driveStraightUntil(1,LOW);
}

void respondDriveMed(void) {
	//printf("Drive Med\n");
	driveStraightUntil(1,MED);
}

void respondDriveHigh(void) { 
	driveStraightUntil(1,HIGH);
}

/**
 * respondTurn
 *
 * Turns the robot a random amount
 */
void respondTurn(void) {
	turnRandom(300000,1800000);
}

/**
 * Led responders control the three leds on top of the roomba
 *
 */

void respondLedBlink(void) {
	blinkLED();
}

void respondLedRed(void) {
	setLED(1,0,0);
}

void respondLedGreen(void) {
	setLED(2,0,0);
}
