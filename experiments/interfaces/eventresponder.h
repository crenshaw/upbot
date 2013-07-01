/**
 * eventresponder.h
 *
 * Types and function prototypes for an event:responder.
 * 
 * @author Tanya L. Crenshaw
 * @since June 29, 2013
 */

#ifndef _EVENT_RESPONDER_H_
#define _EVENT_RESPONDER_H_

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
typedef int eventPredicate(int * data);


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
 * A type to represent an event:responder pair.  An event:responder is
 * an array of eventPredicate and responder function pairs.  Each pair
 * {e, r} can be interpreted as:
 *
 * if the eventPredicate function, e, returns true then
 *    execute the responder, r.
 * 
 */
typedef struct eventresponderTag {
  eventPredicate * e;
  responder * r;
} eventresponder;

#endif
