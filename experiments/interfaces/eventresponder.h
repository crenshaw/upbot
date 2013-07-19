/**
 * eventresponder.h
 * 
 * The UPBOT System treats a robot as a single-threaded state machine
 * that is programmed using an event:responder.
 *
 * Events are the sensor data provided by the robot.  The UPBOT system
 * considers the following subset of sensor data: Wheeldrop caster,
 * wheeldrop left, wheeldrop right, bump left, bump right, cliff left,
 * cliff front left, cliff front right, cliff right, and the virtual
 * wall.  
 *
 * Checking for a particular event is done by an eventPredicate
 * function.
 *
 * Responses are the commands that may be issued to the iRobot Create
 * that alter its behaviour in the physical world, such as drive or 
 * blink LEDs.
 *
 * Issuing a response is done by a response function.
 *
 * An event:responder is a set of eventPredicate() and response()
 * function pairs.
 *
 * This file contains the types and function prototypes for an event:responder.
 * 
 * @author Tanya L. Crenshaw
 * @since June 29, 2013
 */

#ifndef _EVENT_RESPONDER_H_
#define _EVENT_RESPONDER_H_


/**
 *  CONSTANT DEFINITIONS.  All constants in this file should begin
 *  with 'ER' to indicate their membership in eventresponder.h  
 */
#define ER_DEFAULT_SIZE 2

/**
 * A type to represent events, i.e., sensor data.  Empty for now.
 *
 * TODO: I'm not sure if this is even necessary, since "events" are
 * sensor data which can be most easily represented as an array
 * of integers. -tlc-
 */
typedef struct event event;


/**
 * A type to represent an event predicate.  An event predicate is a
 * function that returns true (1) or false (0), indicating whether or
 * not an event or set of events have occurred based on a set of
 * sensor data.  Defining this function as a type allows us to pass
 * eventPredicate functions to other functions.
 *
 * @param[in] data an integer array of sensor data.
 *
 * @returns true (1) or false (0) indicating whether or not an event
 * or set of events have occured.
 */
typedef int eventPredicate(char * data);


/**
 * A type to represent a responder. A responder is a function that
 * issues an appropriate command or set of commands to the robot.
 * Responder functions return no value.  Defining this function as a
 * type allows us to pass responder functions to other functions.
 *
 * @returns none.
 */
typedef void responder(void);


/**
 * A type to point to the next state after a event responder has
 * been executed.
 */
typedef int statePointer;

typedef int alarmTime;


typedef struct erPairTag {
   eventPredicate * e;
   responder * r;
   statePointer p;
} erPair;

typedef struct stateTag {
  int alarmTime;
  erPair * erPairs;
  int count;
} state;

typedef struct eventResponderTag {
  state * states;
  int curState;
  int stateCount;
} eventResponder;


eventResponder erDefault = {};
//TODO:create a new default responder
#endif
