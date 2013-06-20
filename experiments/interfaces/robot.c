/* Strawman interface definition for a robot in the UPBOT source */


// ************************************************************************
// PART 1: ROBOT-RELATED
// ************************************************************************

/** 
 * A structure to represent a robot.  From the software perspective, a
 * "robot" is a single-threaded state machine that is controlled over
 * a given "handle" or serial port.  A robot has a character string
 * representing its unique "name" as well as a unique IP "address".  A
 * robot's "responder" describes how it will transition from its
 * current state to its next state.  Every robot is associated with a
 * "commandService" which provides it with remotely issued responders.
 * Every robot is associated with a "dataService" into which it
 * reports its sensor data.
 */
typedef struct robot 
{
  FILE * handle;   /**< The handle of the serial port */
  char * name;     /**< The name of the robot, i.e. "Webby" or "Frank" */
  char * address;  /**< The IP address of the robot */
  void * responder; /**< The robot's currently executing event-responder */
  char * commandService;  /**< The command service to which this robot is currently connected */
  char * dataService;     /**< The sensor data service to which this robot is currently connected */
} robot;


/**
 * initializeRobot()
 *
 * Perform the necessary software initialization to proxy a robot.  
 * - Lookup the robot's IP based on the given name.
 * - Open a direct serial line to the robot.  
 * - Send the necessary initialization commands to the robot.
 * - Blink an LED to indicate initialization is complete.
 * - Populate the robot structure and return it.
 *
 * This function may only be called by the hardware directly
 * controlling the robot.  It may not be called by a remote service.
 * Remote services interested in connecting to a robot should use
 * connectToRobot().
 *  
 * @param[in] name a string representing the robot's name,
 * i.e. "Webby" or "Frank".
 *
 * @return a pointer to a robot structure containing the necessary
 * information to control the robot and connect it to other services.
 * If an error results in initializing the robot, this is a NULL
 * pointer.
 * 
 */
robot * initializeRobot(const char * name)
{

}


/**
 * driveStraight()
 *
 * Issue a "drive" command to the given robot with velocity of
 * LOW, MED, or HIGH.
 *
 * @param[in] robot a handle representing the robot to command.
 * @param[in] velocity a constant value.
 *
 * @return none
 */
void driveStraight(robot * robot, int velocity)
{

}

/**
 * stop()
 * 
 * Issue a "stop" command to the given robot.
 * 
 * @param[in] robot a handle representing the robot to command.
 * 
 * @return none
 *
 * NOTE: In the current code, stop is a macro.  We may want to 
 * keep it that way for performance reasons.  The faster the 
 * roomba can stop, the better.
 *
 */
void stop(robot * robot)
{

}


// ************************************************************************
// PART 2: RESPONDERS
// ************************************************************************

/**
 * A type to represent state.  Empty for now.
 */
typedef struct state state;

/**
 * A type to represent a responder. A responder is a function that
 * examines the robot's current state, i.e. the sensor data and issues
 * an appropriate command to react to the state.  Responder functions
 * return no value.  Defining this function as a type allows us to 
 * pass responder functions to other functions.
 *
 * @param[in] currentState the sensor data representing the current
 * state of the robot.
 *
 * @return none.
 */
typedef void responder(state * currentState);


/**
 * setResponder()
 *
 * Set the responder for the robot.  Software considers the robot a
 * single-threaded state maching.  If this function is called when a
 * robot already has a responder, the function will block until the
 * current responder is cleared.  This function will not block if it
 * is called from within the current responder.
 *
 * @param[in] robot a handle representing the robot to command.
 * @param[in] responder 
 * 
 * @return none.
 */
void setResponder(robot * robot, responder responder)
{

}


/**
 * onBumpEvent
 * 
 * Example responder for responding to bump events.  That is, if any
 * of the bump sensor data is set, this responder stops the robot
 * and clears the current itself as the robot's responder.
 * 
 */
void onBumpEvent(state * state) {

  // Check bump sensors.
  if(((state[BUMP_STATE] & SENSOR_BUMP_RIGHT) == SENSOR_BUMP_RIGHT) || 
     ((state[BUMP_STATE] & SENSOR_BUMP_LEFT ) == SENSOR_BUMP_LEFT))
    {
      stop(robot);
      printf("Robot bumped!\n");
      setResponder(robot, NULL);  
    }
}


// ************************************************************************
// PART 3: SERVICE-RELATED
// ************************************************************************

/**
 * connectToRobot()
 *
 * Connect a remote entity to a robot with a given name.  Details on
 * this are not yet clear to me.
 */
robot * connectToRobot(const char * name)
{

}

// ************************************************************************
// PART 4: EXAMPLE USAGE
// ************************************************************************

void doStuff() {

  robot * robot = initializeRobot("Webby");
  driveStraight(robot, LOW);
  setResponder(robot, onBumpEvent);

}