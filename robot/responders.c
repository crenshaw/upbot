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
 * StringToResponder()
 *
 * Will used to get the responder of the same name as the
 * given string. If no responder is found then prints an error message
 * and returns respondStop.
 *
 * @param responderIn: name of the responder as a string
 *
 * @return the responder of the same name as the string
 */
responder* StringToResponder(char* responderIn) {
	STRING_TO_RESPONDER(respondStop);
	STRING_TO_RESPONDER(respondDriveLow);
	STRING_TO_RESPONDER(respondDriveMed);
	STRING_TO_RESPONDER(respondDriveHigh);
	STRING_TO_RESPONDER(respondTurnRandom);
	STRING_TO_RESPONDER(respondLedBlink);
	STRING_TO_RESPONDER(respondLedRed);
	STRING_TO_RESPONDER(respondLedGreen);

	//something went wrong
	printf("ERROR: Unable to find responder from string '%s'\n",responderIn);
	return respondStop;
}

/**
 * ResponderToString()
 *
 * Will used to get the name of the given responder. If no responder
 * is found then prints an error message and returns "respondStop".
 *
 * @param responderIn: the responder to get the anme of
 *
 * @return the name of the responder
 */
char* ResponderToString(responder* responderIn) {
	RESPONDER_TO_STRING(respondStop);
	RESPONDER_TO_STRING(respondDriveLow);
	RESPONDER_TO_STRING(respondDriveMed);
	RESPONDER_TO_STRING(respondDriveHigh);
	RESPONDER_TO_STRING(respondTurnRandom);
	RESPONDER_TO_STRING(respondLedBlink);
	RESPONDER_TO_STRING(respondLedRed);
	RESPONDER_TO_STRING(respondLedGreen);
	
	printf("ERROR: Unable to find name for given responder\n");
	return "respondStop";
}


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
 */
void respondTurnRandom(void) {
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
