/* Strawman interface definitions in the UPBOT source */



// ************************************************************************
// PART 1: ROBOT
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


// ************************************************************************
// PART 2: EVENT:RESPONDERS
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


evreCreateEventResponder()


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
// PART 3: ACCEPTOR.  Provides location transparency to service-level
// components. As noted by Schmidt in "Applying Design Patterns to
// Flexibly Configure Network Services, acceptors "initialize
// endpoints of communication at a particular address and wait
// passively for the other endpoints to connect with it."  That said,
// the acceptor also allows for the flexibility for application-level
// to passively wait for services to initiate the connection.
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
 * connected endpoint with a remote peer.
 *
 * 2. Create a service handler to process data requests arriving from
 * the remote peer.
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
// PART 4: CONNECTOR.  
// ************************************************************************



// ************************************************************************
// PART 5: SUPERVISOR.  A supervisor is a process executing on a
// remote machine that may do any of the following:
//
//  a. Connect to a data service and subscribe to any robot's sensor data.
//  b. Connect to a event:responder service and push new event:responders
//     to any connected robots.
// ************************************************************************


void supervisorInitialize()
{

}



// ************************************************************************
// PART X: SERVICES.  There are multiple types of services executing
// on the system that allow flexible connections between robots and
// supervisors.  There are some concepts that are generic to all
// services.  
// ************************************************************************

#define SERV_DATA_SERVICE_COLLATOR 0
#define SERV_DATA_SERVICE_COLLECTOR 1
#define SERV_EVENT_RESPONDER_SERVICE 2

// What are the types of services available?
typedef enum serviceType {
  SERV_DATA_SERVICE_COLLATOR, 
  SERV_DATA_SERVICE_COLLECTOR, 
  SERV_EVENT_RESPONDER_SERVICE
} serviceType;

// Entities in the system access services via a service handler.  
typedef struct serviceHandler {  
  serviceType typeOfService;
  int handler;   
} serviceHandler;


// ************************************************************************
// PART X: DATA SERVICE.  The data service accepts sensor data
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
// or many sensor data collectors.  It also allows remote entities to
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
 * Allow a remote entity to subscribe to the data of a particular
 * robot via a Data Service Collator.
 */
serviceHandler * dsConnectToRobot(const char * name)
{

}

/**
 * dsGetData()
 * 
 * Allow a remote entity to get the data to which it has subscribed
 * from the data service.
 * 
 * @param[in] sh the serviceHandler for the service from which data will
 * be read.
 *
 * @param[in] control a tuner for how much data will be read.  
 *
 * @returns an integer value describe success or failure of the
 * operation.
 */
int dsGetData(  dsTuner control)
{

}


/**
 * dsWrite()
 *
 * Allow an entity to 
 */
int 

// ************************************************************************
// PART 4: EXAMPLE USAGE
// ************************************************************************

void doStuff() {

  robot * robot = initializeRobot("Webby");
  driveStraight(robot, LOW);
  setResponder(robot, onBumpEvent);

}
