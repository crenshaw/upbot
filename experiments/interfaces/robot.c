/* Strawman interface definitions in the UPBOT source */



// ************************************************************************
// PART 1: ROBOT
// ************************************************************************

/** 
 * A structure to represent a robot.  From the software perspective, a
 * "robot" is a single-threaded state machine that is controlled over
 * a given "handle" or serial port.  A robot has a character string
 * representing its unique "name" as well as a unique IP "address".  A
 * robot's "evre", or event:responder, describes how it will
 * transition from its current state to its next state.  Every robot
 * is associated with a "dsCollector" which collects the sensor data
 * to be provided to other entities.  Every robot is associated with a
 * "evreX" through which other entities may update this robot's
 * event:responder function.
 */
typedef struct robot 
{
  FILE * handle;   /**< The handle of the serial port */
  char * name;     /**< The name of the robot, i.e. "Webby" or "Frank" */
  char * address;  /**< The IP address of the robot */
  eventresponder * er;         /**< The robot's currently executing event-responder */
  serviceHandler * dsCollector;     /**< The sensor data collector service to which this robot is currently connected */
  serviceHandler * erService;  /**< The event:responder service to which this robot is currently connected */
} robot;


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


// ************************************************************************
// PART 3: CLOCKS.
//
// ************************************************************************

// ************************************************************************
// PART 4: ACCEPTOR.  Provides location transparency to service-level
// components. As noted by Schmidt in "Applying Design Patterns to
// Flexibly Configure Network Services, acceptors "initialize
// endpoints of communication at a particular address and wait
// passively for the other endpoints to connect with it."  That said,
// the acceptor also allows for the flexibility for application-level
// to passively wait for services to initiate the connection.
//
// Once a connection is established, neither the application nor the
// service utilize the acceptor until another connection must be
// established.
// ************************************************************************


/**
 * accCreateConnection
 *
 * Create a passive-mode socket, bind it to a particular port number
 * on the calling host's IP address and listen passively for the
 * arrival of connection requests.  Since listening passively can be a
 * blocking call (i.e. accept()), it may be worthwhile to use this
 * function in a separate thread.
 *
 * NOTE: Similar to "createServer()" in serverUtility.c
 *
 * @param[in] port the port number to listen to.
 * 
 * @returns a handler for the endpoint.
 */
int accCreateConnection(int port)
{

}


/**
 * accCompleteConnection
 *
 * Based on D. Schmidt's "Acceptor-Connector" design pattern.
 *
 * 1. Use the passive-mode endpoint, endpointHandler, to create a
 * connected endpoint with a peer.
 *
 * 2. Create a service handler to process data requests arriving from
 * the peer.
 * 
 * 3. "Invoke the service handler's activation hook method which
 * allows the service handler to finish initializing itself."
 *
 * NOTE: Similar to "establishConnection()" in serverUtility.c 
 *
 * @param[in] endpointHandler a handler for the connection endpoint.
 * @param[in] type of service (see serviceType enum for possible
 * values).
 *
 * @returns a handler for the service connection.  Subsequent read and write
 * operations on this connection are parameterized by this handler 
*/
serviceHandler * accCompleteConnection(int endpointHandler, serviceType type)
{

}


// ************************************************************************
// PART 5: CONNECTOR.  Provides location transparency to
// application-level components. As noted by Schmidt in "Applying
// Design Patterns to Flexibly Configure Network
// Services, the connector initiates a connection with a passive-mode
// endpoint and activates the appropriate serviceHandler.
//
// Once a connection is established, neither the application nor the
// service utilize the acceptor until another connection must be
// established.
// ************************************************************************

/**
 * conInitiateConnection
 *
 * Based on D. Schmidt's "Acceptor-Connector" design pattern.
 * 
 * 1. Establish a connection with a passive-mode endpoint.
 * 
 * 2. Activate a service handler for this connection and return it.
 * 
 */
serviceHandler * conInitiateConnection(char * wellKnownIP, int port, serviceType type)
{
}


// ************************************************************************
// PART 6: SUPERVISOR.  A supervisor is a process executing on a that
// may do any of the following:
//
//  a. Connect to a data service collator and subscribe to any robot's
//  sensor data.  
//
//  b. Connect to a event:responder service and push new
//  event:responders to any connected robots.
//
//************************************************************************


void supervisorInitialize()
{

}





// ************************************************************************
// PART 7: DATA SERVICE.  The data service accepts sensor data
// collected by the robots in the physical world and delivers the data
// to other software entities, e.g. the system supervisor.
// 
// The data service is implemented in two halves.  
// 
// a. The Collector.  The data service collector collects sensor data
// locally at a given sensor data collection point, i.e., a robot.
// The Collector is responsible for forwarding data to a collator.
//
// b. The Collator.  The data service collator is a repository for one
// or many sensor data collectors.  It also allows entities to
// subscribe to sensor data.
// 
// ************************************************************************

/**
 * dsCreateCollector
 *
 * The Collector half of the data service collects sensor data locally
 * at a given robot.  
 */
void dsCreateCollector(void)
{
}


/**
 * dsCreateCollator
 * 
 * The Collator half of the data service gathers sensor data from
 * collectors and forwards it to interested entities
 */
void dsCreateCollator(void)
{

}


/**
 * dsConnectToRobot()
 * 
 * Allow an entity to subscribe to the data of a particular
 * robot via a Data Service Collator.
 */
serviceHandler * dsConnectToRobot(const char * name)
{

}

/**
 * dsGetData()
 * 
 * Allow an entity to get the data to which it has subscribed
 * from the data service.
 * 
 * @param[in] sh the serviceHandler for the service from which data will
 * be read.
 *
 * @param[in] control a tuner for how much data will be read, 1 sensor
 * report or all available sensor reports.
 *
 * @param[out] dest a pointer to where the data shall be placed.
 *
 * @returns an integer value describing the success or failure of the
 * operation.
 */
int dsGetData(serviceHandler * sh, dsTuner control, void * dest)
{

}


/**
 * dsWrite()
 *
 * Allow an entity to write sensor data or control commands to a
 * data service.
 *
 * @param[in] sh the serviceHandler for the service to which data will
 * be written.
 *
 * @param[in] src a pointer to the data to be written
 *
 * @returns an integer value describing the success or failure of the
 * operation.
 */
int dsWrite(serviceHandler * sh, void * src)
{

}


// ************************************************************************
// EXAMPLE USAGE
// ************************************************************************

void doStuff() {

  robot * robot = initializeRobot("Webby");
  driveStraight(robot, LOW);
  setResponder(robot, onBumpEvent);

}
