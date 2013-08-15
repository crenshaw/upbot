/**
 * responders.c
 *
 * A responder is the action preformed after an event has been 
 * triggered. All responders take no arguments and return nothing.
 *
 * Note: Due to the roombas limited serial capabilites it is easy to
 * clobber its input buffer. Therefore responders may need to contain 
 * delays to prevent such issues.
 *
 * @author Matt Holland
 * @since July 2013
 */

#include "responders.h"

/**
 * respondStop()
 * 
 * Stops the robot.
 */ 
void respondStop(void)
{
	//printf("stop\n");
	stop();
	usleep(30);
}

/**
 * respondDriveLow()
 * 
 * Causes the robot to drive at its low speed
 */ 
void respondDriveLow(void) {
	//printf("Drive low\n");
	driveStraightUntil(1,LOW);
}

/**
 * respondDriveMed()
 * 
 * Causes the robot to drive at its medium speed
 */
void respondDriveMed(void) {
	//printf("Drive Med\n");
	driveStraightUntil(1,MED);
}

/**
 * respondDriveHigh()
 * 
 * Causes the robot to drive at its high speed
 */
void respondDriveHigh(void) { 
	driveStraightUntil(1,HIGH);
}

/**
 * respondTurn()
 *
 * Turns the robot a random amount
 *
 * TODO: rename to something less ambigous
 */
void respondTurn(void) {
	turnRandom(300000,1800000);
}

/**
 * respondLedBlink()
 * 
 * Causes the led on the robot to blink
 */
void respondLedBlink(void) {
	blinkLED();
}

/**
 * respondLedRed()
 * 
 * Causes the leftmost led on the robot to turn red
 * turns off all other leds
 */
void respondLedRed(void) {
	setLED(1,0,0);
}

/**
 * respondLedGreen()
 * 
 * Causes the leftmost led on the robot to turn green
 * turns off all other leds
 */
void respondLedGreen(void) {
	setLED(2,0,0);
}
