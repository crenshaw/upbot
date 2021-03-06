#include "events.h"

/**
 * events.c contains the events used by eventresponder.c
 *
 * An event is a function that is used to control check the status of a robot
 * It returns a boolean based upon a roombas sensor data passed into it
 * 
 */



/**
 * eventTrue
 * 
 * Default eventPredicate function.  Always returns true.
 */
int eventTrue(char * data)
{
	return 1;
}

/**
 * eventFalse
 * 
 * Default eventPredicate function.  Always returns false.
 */
int eventFalse(char * data)
{
	return 0;
}

/**
 * eventBump
 * 
 * Example eventPredicate function for checking for bump events.
 */
int eventBump(char * data) {

	// Check bump sensors.

	// TODO: Need to correct the use of literal 0 when checking
	// the state of the bump sensors.
	if(((data[0] & SENSOR_BUMP_RIGHT) == SENSOR_BUMP_RIGHT) || 
			((data[0] & SENSOR_BUMP_LEFT ) == SENSOR_BUMP_LEFT))
	{
		return 1;
	}
	else
		return 0;
}

/**
 * eventBumpRight
 * 
 * Example eventPredicate function for checking if the right bumper 
 * has been hit.
 */
int eventBumpRight(char * data) {
	if((data[0] & SENSOR_BUMP_RIGHT ) == SENSOR_BUMP_RIGHT)
		return 1;
	else
		return 0;
}

/**
 * eventBumpLeft
 * 
 * Example eventPredicate function for checking if the left bumper 
 * has been hit.
 */
int eventBumpLeft(char * data) {
	if((data[0] & SENSOR_BUMP_LEFT ) == SENSOR_BUMP_LEFT)
		return 1;
	else
		return 0;
}


/**
 * eventNotBump
 * 
 * Example eventPredicate function for checking for bump events.
 */
/*
int eventNotBump(char * data) {

	// Check bump sensors.

	// TODO: Need to correct the use of literal 0 when checking
	// the state of the bump sensors.
	if(((data[0] & SENSOR_BUMP_RIGHT) == SENSOR_BUMP_RIGHT) || 
			((data[0] & SENSOR_BUMP_LEFT ) == SENSOR_BUMP_LEFT))
	{
		setLED(1,0,0);
		return 0;
	}
	else
		setLED(2,0,0);
	return 1;
}
*/

/**
 * eventAlarm
 */
int eventAlarm(char * data) {
	if (data[15] != '0') {
		return 1;
	}
	return 0;
}

/**
 * eventVWall(char* data)
 *
 * inidicates weather or not the roomba is touching a virtual wall
 *
 * @param data is a char array of all sensor data for robot
 *
 * @return int of either 0 or 1
 */
int eventVWall(char * data) {
	return *(data+6);
}

/**
 * eventCliffLeft(char* data)
 *
 * inidicates if the sensor detects the ground(1) or nothing (0)
 *
 * @param data is a char array of all sensor data for robot
 *
 * @return int of either 0 or 1
 */
int eventCliffLeft(char * data) {
	return *(data+2);
}

/**
 * eventCliffFrontLeft(char* data)
 *
 * inidicates if the sensor detects the ground(1) or nothing (0)
 *
 * @param data is a char array of all sensor data for robot
 *
 * @return int of either 0 or 1
 */
int eventCliffFrontLeft(char * data) {
	return *(data+3);
}

/**
 * eventCliffFrontRight(char* data)
 *
 * inidicates if the sensor detects the ground(1) or nothing (0)
 *
 * @param data is a char array of all sensor data for robot
 *
 * @return int of either 0 or 1
 */
int eventCliffFrontRight(char * data) {
	return *(data+4);
}

/**
 * eventCliffRight(char* data)
 *
 * inidicates if the sensor detects the ground(1) or nothing (0)
 *
 * @param data is a char array of all sensor data for robot
 *
 * @return int of either 0 or 1
 */
int eventCliffRight(char * data) {
	return *(data+5);
}


