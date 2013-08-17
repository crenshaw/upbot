#include "netERProtocol.h"

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
			//"respondTurnRandom";//STUFF

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
			//"respondTurnRandom";
			strcpy(spot,r);
			spot += strlen(r)+1;

			//package the transitions next state
			*((int*)spot) = er->states[s].transitions[t].n;
			spot += INT_SIZE;

		}
	}

	return packageSize;

}

void unpackageEventResponder(int size, eventResponder* er, char* package) {

	char* spot = package;

	er->curState = *((int*)spot);
	spot += INT_SIZE;

	er->stateCount = *((int*)spot);
	spot += INT_SIZE;

	int sSize = er->stateCount;
	er->states = malloc(sizeof(state)*(er->stateCount));
	//malloc here

	int s;
	for (s=0; s< er->stateCount; ++s) {

		er->states[s].count = *((int*)spot);
		spot += INT_SIZE;

		er->states[s].clockTime = *((int*)spot);
		spot += INT_SIZE;

		//malloc here
		er->states[s].transitions = malloc(sizeof(transition) * er->states[s].count);

		int t;
		for (t=0; t< er->states[s].count; ++t) {

			er->states[s].transitions[t].e = StringToEvent(spot);
			printf("%s\n",spot);
			spot += strlen(spot)+1;

			er->states[s].transitions[t].r = StringToResponder(spot);
			printf("%s\n",spot);
			spot += strlen(spot)+1;

			er->states[s].transitions[t].n = *((int*)spot);
			spot += INT_SIZE;

		}

	}	

}
