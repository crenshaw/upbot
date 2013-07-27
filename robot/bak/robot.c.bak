/**
 * robot.c
 *
 * From the software perspective, A "robot" in the UPBOT robotics
 * system is a single-threaded state machine that is controlled over a
 * given "handle" or serial port.
 *
 * This file contains the functions to initialize and update the
 * software representation of a robot.
 * 
 * @author: Tanya L. Crenshaw
 * @since: July 2013
 *
 */




/**
 * robotInitialize()
 *
 * Perform the necessary software initialization to proxy a robot.  
 * - Lookup the robot's IP based on the given name.
 * - Open a direct serial line to the robot.  
 * - Send the necessary initialization commands to the robot.
 * - Blink an LED to indicate initialization is complete.
 * - Populate the robot structure and return it.
 *
 * This function may only be called by the hardware directly
 * controlling the robot.  It may not be called by a remote entity.
 * Remote entities interested in connecting to a robot should use
 * alternative services.
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
robot * robotInitialize(const char * name)
{

}


/**
 * robotDriveStraight()
 *
 * Issue a "drive" command to the given robot with velocity of
 * LOW, MED, or HIGH.
 *
 * @param[in] robot a handle representing the robot to command.
 * @param[in] velocity a constant value.
 *
 * @return none
 */
void robotDriveStraight(robot * robot, int velocity)
{

}

/**
 * robotStop()
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
void robotStop(robot * robot)
{

}


/**
 * setResponder()
 *
 * Set the responder for the robot.  Software considers the robot a
 * single-threaded state machine.  If this function is called when a
 * robot already has a responder, the function will block until the
 * current responder is cleared.  This function will not block if it
 * is called from within the current responder.
 *
 * @param[in] robot a handle representing the robot to command.
 * @param[in] responder 
 * 
 * @return none.
 */


void setResponder(robot * robot, eventresponder * er)
{

}



