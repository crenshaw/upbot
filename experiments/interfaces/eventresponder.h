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
 * An event:responder is a set of states, each with a set of 
 * transitions to other states
 *
 * This file contains the types and function prototypes for an event:responder.
 * 
 * @author Tanya L. Crenshaw, Matthew Holland
 * @since July 20, 2013
 */

#ifndef _EVENT_RESPONDER_H_
#define _EVENT_RESPONDER_H_


/**
 *  CONSTANT DEFINITIONS.  All constants in this file should begin
 *  with 'ER' to indicate their membership in eventresponder.h  
 */
#define ER_DEFAULT_SIZE 2
#define ER_SENS_BUFFER_SIZE (150)

/**
 * A type to represent events, i.e., sensor data.  Empty for now.
 *
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
typedef int nextState;

/**
 * A type to contain an eventPredicate, a responder if the event
 * passes, and a nextState identifier to direct it to the next state
 */
typedef struct transitionTag {
   eventPredicate * e;  /**< An event predicate function  */
   responder * r;       /**< A responder function */
   nextState n;         /**< The state to go to after this function*/
} transition;

/**
 * A type that contains the duration until an alarm should go off
 * a list of transitions, and a count of how many transitionss
 * are in the list
 */
typedef struct stateTag {
  int clockTime;            /**< Time for the clock to go off */ 
  transition * transitions; /**< List of transitions from the state*/
  int count;                /**< The number of transitions from state */
} state;


/**
 * A type that containts a list of states the robot can be in,
 * an indicator as to which state it is currently in, and
 * a count of how many states are within the event responder
 */
typedef struct eventResponderTag {
  state * states;       /**< A list of states within the event responder */
  int curState;         /**< The current state of the event responder */
  int stateCount;       /**< The number of states within the event responder */
} eventResponder;

#endif
