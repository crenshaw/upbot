/**
 * netERProtocol.c
 *
 * This contains the packing and unpacking functions for event
 * responders into a portable data structure. The idea is that this
 * will allow event responders to be sent across the network.
 *
 * @author Matt Holland
 * @since August 2013
 */

#include "netERProtocol.h"


/**
 * packageEventResponder()
 *
 * This function packages the given event responder and packages it.
 * The given package pointer will be changed to point toward the new
 * package. Once it has been formed, the package is no longer
 * dependant on any information on the computer (such as function
 * pointers)
 *
 * @param er: pointer to the event responder to package
 *
 * @param pacakge: a character pointer pointer that will be modified
 * to point towards the new package that will be created on the heap.
 *
 * @return the size of the package in bytes
 */
int packageEventResponder(eventResponder* er, char** package) {

	int packageSize = 0; //packageType

	packageSize += 2 * INT_SIZE; //size of curState and state Count
	packageSize += (2 * er->stateCount * INT_SIZE); //add every state

	int s,t;
	for (s=0;s< er->stateCount; ++s) {

		//the next state value
		packageSize += (er->states[s].count * INT_SIZE);

		for (t=0; t<  er->states[s].count; ++t) {
			char* e = EventToString(er->states[s].transitions[t].e);
			char* r = ResponderToString(er->states[s].transitions[t].r);

			packageSize += (strlen(e) + strlen(r) + 2);
		}

	}

	printf("package size: %i\n",packageSize);

	*package = malloc(packageSize);

	char* spot = *package;

	//package the current state
	*((int*)spot) = er->curState;
	spot += INT_SIZE;

	//package the state count
	*((int*)spot) = er->stateCount;
	spot += INT_SIZE;

	for (s=0; s< er->stateCount; ++s) {
		//package the states transition count
		*((int*)spot) = er->states[s].count;
		spot += INT_SIZE;

		//package the states clock time
		*((int*)spot) = er->states[s].clockTime;
		spot += INT_SIZE;

		for (t=0; t< er->states[s].count; ++t) {
			//package the transitions event by id
			char* e = EventToString(er->states[s].transitions[t].e);
			strcpy(spot,e);
			spot += strlen(e)+1;

			//package the transitions responder by id
			char* r = ResponderToString(er->states[s].transitions[t].r);
			strcpy(spot,r);
			spot += strlen(r)+1;

			//package the transitions next state
			*((int*)spot) = er->states[s].transitions[t].n;
			spot += INT_SIZE;

		}
	}

	return packageSize;

}

/**
 * unpackageEventResponder()
 * 
 * unpacks a given package to a given event responder. This will not
 * clean the event responder. So the user must check if it needs to be
 * cleaned and do that themself if it is necicary. 
 *
 * @param size: the size of the package being unpacked in bytes
 *
 * @param er: pointer to the were the new event responder will be
 * unpacked to
 *
 * @param package: pointer to the package
 */
void unpackageEventResponder(int size, eventResponder* er, char* package) {

	char* spot = package;

	//unpack the current state
	er->curState = *((int*)spot);
	spot += INT_SIZE;

	//unpack the number of states
	er->stateCount = *((int*)spot);
	spot += INT_SIZE;

	int sSize = er->stateCount;
	er->states = malloc(sizeof(state)*(er->stateCount));

	int s;
	for (s=0; s< er->stateCount; ++s) {
		//transitions in the state
		er->states[s].count = *((int*)spot);
		spot += INT_SIZE;

		//Clock time for the state
		er->states[s].clockTime = *((int*)spot);
		spot += INT_SIZE;

		er->states[s].transitions = malloc(sizeof(transition) * er->states[s].count);

		int t;
		for (t=0; t< er->states[s].count; ++t) {

			//event
			er->states[s].transitions[t].e = StringToEvent(spot);
			//printf("%s\n",spot);
			spot += strlen(spot)+1;

			//responder
			er->states[s].transitions[t].r = StringToResponder(spot);
			//printf("%s\n",spot);
			spot += strlen(spot)+1;

			//next state
			er->states[s].transitions[t].n = *((int*)spot);
			spot += INT_SIZE;

		}

	}	

}
