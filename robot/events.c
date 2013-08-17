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
 * TODO: the TODOs in this file should probally wait until I get input
 * about the cleanest way to add constants since the data is somewhat
 * dynamicly generated (placed by a for loop not by constants)
 *
 * @author Matt Holland
 * @since July 2013
 */

#include "events.h"

/**
 * StringToEvent(char* eventIn)
 *
 * Will used to get the event of the same name as the
 * given string. If no event is found then prints an error message
 * and returns eventFalse.
 *
 * @param eventIn: name of the event as a string
 *
 * @return the event of the same name as the string
 */
eventPredicate* StringToEvent(char* eventIn) {
	STRING_TO_EVENT(eventTrue);
	STRING_TO_EVENT(eventFalse);
	STRING_TO_EVENT(eventBump);
	STRING_TO_EVENT(eventBumpRight);
	STRING_TO_EVENT(eventBumpLeft);
	STRING_TO_EVENT(eventClock);
	STRING_TO_EVENT(eventVWall);
	STRING_TO_EVENT(eventCliffLeft);
	STRING_TO_EVENT(eventCliffFrontLeft);
	STRING_TO_EVENT(eventCliffFrontRight);
	STRING_TO_EVENT(eventCliffRight);

	printf("ERROR: failed to convert string '%s' to an event\n",eventIn);	
	return eventFalse;
}

/**
 * EventToString(eventPredicate* eventIn)
 *
 * Will used to get the name of the given event. If no event
 * is found then prints an error message and returns "eventFalse".
 *
 * @param eventIn: the event to get the anme of
 *
 * @return the name of the event
 */
char* EventToString(eventPredicate* eventIn) {
	EVENT_TO_STRING(eventTrue);
	EVENT_TO_STRING(eventFalse);
	EVENT_TO_STRING(eventBump);
	EVENT_TO_STRING(eventBumpRight);
	EVENT_TO_STRING(eventBumpLeft);
	EVENT_TO_STRING(eventClock);
	EVENT_TO_STRING(eventVWall);
	EVENT_TO_STRING(eventCliffLeft);
	EVENT_TO_STRING(eventCliffFrontLeft);
	EVENT_TO_STRING(eventCliffFrontRight);
	EVENT_TO_STRING(eventCliffRight);

	printf("ERROR:failed to convert eventPredicate to a string\n");
	return "eventFalse";
}


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
 * eventClock(char* data)
 * 
 * checks if the clock has been triggered
 *
 * @param data: the data from the robot to check against
 *
 * @return if the clock has been triggered
 * '1' is triggered, '0' is not triggered
 *
 * TODO: use constant instead of the literal 15
 */
int eventClock(char * data) {
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


