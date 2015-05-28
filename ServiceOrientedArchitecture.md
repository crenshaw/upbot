## The Services ##

There are two services available on UPBOT.  These are the Data Service and the Event:Responder Service.  The Data Service provides a means for sensor data obtained by the robots to be collected and aggregated for remote entities.  The Event:Responder Service provides a means for changing the event:responder currently used by a robot to control its autonomous behavior.

Each service has two different service endpoints depending on the type of entity using the service.  These endpoint types are enumerated by the serviceType defined in communication/services.h.

### Data Service ###
  * **Data Collector.**  The Data Service endpoint for those entities that produce data, i.e., the robots.  A robot may start a `SERV_DATA_SERVICE_COLLECTOR` service endpoint and write sensor data to the service endpoint.  The service endpoint shall handle transmitting the data to an Data Aggregator service endpoint.

  * **Data Aggregator.**  The Data Service endpoint for those entities that are interesting in data produced by one or more robots.  An entity may start a `SERV_DATA_SERVICE_AGGREGATOR` service endpoint and read sensor data from the service endpoint.  The service endpoint shall handle receiving sensor from any Data Collectors on the network.

### Event:Responder Service ###

  * **Robot.**  The Event:Responder Service endpoint for those entities executing event:responders, i.e., the robots.  A robot may start a `SERV_EVENT_RESPONDER_ROBOT` service so that it may receive notice from remote entities to change the event:responder currently executing on the robot.  At this time, event:responders are determined at compile time, so robots may only change their event:responder to one that is already available in the executing binary.

  * **Programmer.**  The Event:Responder Service endpoint for those entities interesting in directing robots to change their event:responder.  An entity may start a `SERV_EVENT_RESPONDER_PROGRAMMER` service so that it may send notice to robot entities to change their event:responder.

## Acceptors and Connectors ##

The four service endpoints are divided into two categories of endpoint type.  Each service endpoint is either an Acceptor or a Connector.  An acceptor is a service endpoint that creates an endpoint of communication, broadcasts its availability, and waits passively for other entities to initiate a fully established line of communication.  A connector is a service endpoint that creates an endpoint of communication, listens for available acceptors, and actively initiates a fully-established line of communication.

The use of acceptors and connectors was based on Douglas Schmidt's pattern for Flexible Networks, as seen in Linda Rising's book "Design Patterns for Communication Software."

### Execution Phases of Acceptors and Connectors ###

Acceptors have these phases of execution.

  * **Start Phase.**  Create an endpoint for point-to-point communication with another service endpoint.  Create two threads that will execute the broadcast and accept phases, respectively.

  * **Broadcast Phase.**  Create an endpoint of communication to broadcast service availability.  Broadcast service available on the port defined for this service endpoint.

  * **Accept Phase.**  Wait passively for a connector-style service endpoint to establish a fully established line of communication.

  * **Activation Phase**.  Establish a full line of communication and activate the thread that will be handling the service's functionality, i.e., aggregate data or receive event:responder notifications.

Connectors have these four phases of execution.

  * **Start Phase.**  Create an endpoint for point-to-point communication with another service endpoint.

  * **Listening Phase.**  Create an endpoint of communication to listen for service broadcasts.  This phase blocks until a broadcast is heard.

  * **Initiation Phase.**  Based on the broadcast received, initiate a fully established line of communication with an acceptor-style service endpoint.  This phase blocks until communication is established.

  * **Activation Phase.**  Activate the thread that will be handling the service's functionality, i.e., collect data or send event:responder notifications.

## The Service Handler ##

Applications start and access the UPBOT services via a serviceHandler whose type is defined in communication/services.h.   As a service is started and activated.

To start a service endpoint, an entity must allocate a serviceHandler and call servStart(), indicating the type of service endpoint.  For example,

```
serviceHandler sh;
int status = servStart(SERV_DATA_SERVICE_AGGREGATOR, &sh);
```