/**
 * events.c 
 *
 * An event contains a conditional check so the robot can determine
 * what it should do next based on the sensor data it collects.
 *
 * Events are should never be called by the user directly but instead
 * put within a transition so that it may be used by 
 * the event responder.
 *
 * @author Matt Holland
 * @since July 2013
 */

#include "events.h"

/**
 * eventTrue(char* data)
 * 
 * Always returns true.
 *
 * @param data: the data from the robot to check against
 *
 * @return 1 always
 */
int eventTrue(char * data)
{
	return 1;
}

/**
 * eventFalse(char* data)
 * 
 * Always returns false.
 *
 * @param data: the data from the robot to check against
 *
 * @return 0 always
 */
int eventFalse(char * data)
{
	return 0;
}

/**
 * eventBump(char* data)
 * 
 * checks if either the left or right bump sensor have been triggered
 *
 * @param data: the data from the robot to check against
 *
 * @return if the bump sesors have been triggered
 * 1 is triggered, 0 is not triggered
 *
 * TODO: Need to correct the use of literal 0 when checking
 * the state of the bump sensors.
 */
int eventBump(char * data) {

	if(((data[0] & SENSOR_BUMP_RIGHT) == SENSOR_BUMP_RIGHT) || 
			((data[0] & SENSOR_BUMP_LEFT ) == SENSOR_BUMP_LEFT))
	{
		return 1;
	}
	else
		return 0;
}

/**
 * eventBumpRight(char* data)
 * 
 * checks if the right bump sensor has been triggered
 *
 * @param data: the data from the robot to check against
 *
 * @return if the right bump sesor has been triggered
 * 1 is triggered, 0 is not triggered
 *
 * TODO: Need to correct the use of literal 0 when checking
 * the state of the bump sensors.
 */
int eventBumpRight(char * data) {
	if((data[0] & SENSOR_BUMP_RIGHT ) == SENSOR_BUMP_RIGHT)
		return 1;
	else
		return 0;
}

/**
 * eventBumpLeft(char* data)
 * 
 * checks if the left bump sensor has been triggered
 *
 * @param data: the data from the robot to check against
 *
 * @return if the left bump sesor has been triggered
 * 1 is triggered, 0 is not triggered
 *
 * TODO: Need to correct the use of literal 0 when checking
 * the state of the bump sensors.
 */
int eventBumpLeft(char * data) {
	if((data[0] & SENSOR_BUMP_LEFT ) == SENSOR_BUMP_LEFT)
		return 1;
	else
		return 0;
}

/**
 * eventAlarm(char* data)
 * 
 * checks if the clock has been triggered
 *
 * @param data: the data from the robot to check against
 *
 * @return if the clock has been triggered
 * '1' is triggered, '0' is not triggered
 *
 * TODO: rename to eventClock
 * TODO: use constant instead of the literal 15
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
 *
 * TODO: use constant instead of the literal 6
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
 *
 * TODO: use constant instead of the literal 2
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
 *
 * TODO: use constant instead of the literal 3
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
 *
 * TODO: use constant instead of the literal 4
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
 *
 * TODO: use constant instead of the literal 5
 */
int eventCliffRight(char * data) {
	return *(data+5);
}


