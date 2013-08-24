/**
 * myEventResponders.c
 *
 * File contains the event responders that will be loaded on the robot
 * at the begining of run time. (there is the potential for the 
 * supervisor to add mroe during run time over the network). It also
 * contains the function selectNextER which allows the event responder
 * to be changed by use of comparison of Strings. The Stop ER is loaded
 * at the begining of runtime until the program is directed otherwise.
 *
 * @author Matt Holland
 * @since July 2013 
 */

#include "myEventResponders.h"


/**
 * selectNextER()
 *
 * A function that sets up the event responder based on a conditonal
 * string comparison.
 *
 * @param erName: String to indicate which event responder to load
 *
 * @param myER: eventResponder data structure to setup the event
 * responder
 */
void selectNextER(char * erName, eventResponder* myER) {
	if (strcmp(erName,"go")==0) {
		initalizeWanderER(myER);
	} else if (strcmp(erName,"stop")==0) {
		initalizeStopER(myER);
	}																														
}

/**
 * initalizeWanderER()
 *
 * An example event responder which tells the robot to drive forward
 * until the robot hits either a wall or a virtual wall. Once it hits
 * a wall it will turn a random amount in a random direction and
 * continue driving forward. After driving forward at its LOW speed
 * for five seconds it will accelerate to a faster speed until the 
 * next time it hits the wall.
 *
 * @param myER: eventResponder data structure to initalize.
 */
void initalizeWanderER(eventResponder* myER) {

	myER->curState = 0;
	myER->stateCount = 2;
	myER->states = malloc(sizeof(state)*2);

		myER->states[0].count = 4;  
		myER->states[0].clockTime = 5; 
		myER->states[0].transitions = malloc(sizeof(transition)*4); 

			myER->states[0].transitions[0].e = eventClock; 
			myER->states[0].transitions[0].r = respondDriveMed;
			myER->states[0].transitions[0].n = 1;

			myER->states[0].transitions[1].e = eventBump; 
			myER->states[0].transitions[1].r = respondTurnRandom;
			myER->states[0].transitions[1].n = 0;

			myER->states[0].transitions[2].e = eventVWall; 
			myER->states[0].transitions[2].r = respondTurnRandom;
			myER->states[0].transitions[2].n = 0;

			myER->states[0].transitions[3].e = eventTrue; 
			myER->states[0].transitions[3].r = respondDriveLow;
			myER->states[0].transitions[3].n = 0;

		myER->states[1].count = 3;
		myER->states[1].clockTime = 0;
		myER->states[1].transitions = malloc(sizeof(transition)*3); 

			myER->states[1].transitions[0].e = eventBump; 
			myER->states[1].transitions[0].r = respondTurnRandom;
			myER->states[1].transitions[0].n = 0;

			myER->states[1].transitions[1].e = eventVWall; 
			myER->states[1].transitions[1].r = respondTurnRandom;
			myER->states[1].transitions[1].n = 0;

			myER->states[1].transitions[2].e = eventTrue; 
			myER->states[1].transitions[2].r = respondDriveMed;
			myER->states[1].transitions[2].n = 1;

}


/**
 * initalizeStopER()
 *
 * stop responder is always present. Thatway we can
 * initalize the robot in a responder which won't have it running
 * away
 *
 * @param myER: the empty event responder data structure to initalize
 * 
 */
void initalizeStopER(eventResponder* myER) {
	myER->curState = 0;
	myER->stateCount = 1;
	myER->states = malloc(sizeof(state));

	myER->states[0].count = 1;  
	myER->states[0].clockTime = 0; 
	myER->states[0].transitions = malloc(sizeof(transition)); 

	myER->states[0].transitions[0].e = eventTrue; 
	myER->states[0].transitions[0].r = respondStop;
	myER->states[0].transitions[0].n = 0;

}

