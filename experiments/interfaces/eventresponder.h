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
#define ER_ARRAY_MISMATCH (-1)
#define ER_NULL_ARRAY (-2)
#define ER_NULL_ER (-3)
#define ER_SUCCESS (0)

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

/**
 * A type to represent the state a required for an event to be 
 * triggered
 */
typedef int initialState;

typedef int alarmTime;

/**
 * A type to represent an event:responder pair.  An event:responder is
 * an array of eventPredicate and responder function pairs.  Each pair
 * {e, r} can be interpreted as:
 *
 * if the eventPredicate function, e, returns true then
 *    execute the responder, r.
 * 
 */
typedef struct eventresponderTag {
  initialState * i;     /**< An array of startState pointers >**/
  eventPredicate ** e;  /**< An array of eventPredicate functions */
  responder ** r;       /**< An array of responder functions */
  statePointer * p;     /**< An array of statePointers >**/
  alarmTime * a;
  int length;           /**< The total number of pairs */
  int state;
} eventresponder;



/**
 * Function prototypes.  See eventresponder.c for details on
 * this/these functions.
 */

int createResponder(eventPredicate * e[], responder * r[], eventresponder * er);
//int createResponder(eventPredicate * e, responder * r, eventresponder * er);

int eventTrue(char * data);
void respondStop(void);



// A global, default, event:responder, in case something bad happens or an
// application developer just wants a boring default.  This default
// event:responder is:
//
//  if  true  then
//    stop robot
//
static eventPredicate * eDefault[ER_DEFAULT_SIZE] = {eventTrue, NULL};
static responder * rDefault[ER_DEFAULT_SIZE] = {respondStop, NULL};

//TODO: fix defualt responder 
static eventresponder erDefault = {NULL,eDefault, rDefault,NULL,NULL,1,0};

#endif
