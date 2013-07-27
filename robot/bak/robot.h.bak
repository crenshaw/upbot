/**
 * robot.h
 *
 * Types and function prototypes for a robot.
 * 
 * @author Tanya L. Crenshaw
 * @since June 29, 2013
 */

#ifndef _ROBOT_H_
#define _ROBOT_H_

#include "eventresponder.h"
#include "services.h"

/** 
 * A structure to represent a robot.  From the software perspective, a
 * "robot" is a single-threaded state machine that is controlled over
 * a given "handle" or serial port.  A robot has a character string
 * representing its unique "name" as well as a unique IP "address".  A
 * robot's "evre", or event:responder, describes how it will
 * transition from its current state to its next state.  Every robot
 * is associated with a "dsCollector" which collects the sensor data
 * to be provided to other entities.  Every robot is associated with a
 * "er" through which other entities may update this robot's
 * event:responder function.
 */
typedef struct robot 
{
  FILE * handle;   /**< The handle of the serial port */
  char * name;     /**< The name of the robot, i.e. "Webby" or "Frank" */
  char * address;  /**< The IP address of the robot */
  eventResponder * er;         /**< The robot's currently executing event-responder */
  serviceHandler * dsCollector;     /**< The sensor data collector service to which this robot is currently connected */
  serviceHandler * erService;  /**< The event:responder service to which this robot is currently connected */
} robot;

/**
 * Function prototypes.  See robot.c for details on this/these
 * functions.
 */
robot * robotInitialize(const char * name);

#endif
