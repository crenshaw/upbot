#include "netDataProtocol.h"

/*
 * package any data the supervisor may need into a 
 * string (aka byte array)
 */
void packageData(char* package, char* snsData, int state, int nextState, int transition, time_t lastStateChange) {
	//char* package = malloc(dataPackageSize);

	package[snsBumpLeft] = *snsData & SENSOR_BUMP_LEFT;
	package[snsBumpRight] = *snsData & SENSOR_BUMP_RIGHT;
	package[snsCliff] = 1;
	package[snsVWall] = *(snsData+6);

	*((int*)(package+stateInitial)) = state;
	*((int*)(package+stateFinal)) = nextState;
	*((int*)(package+transitionID)) = transition;

	*((int*)(package+clockLastSet)) = lastStateChange;

	time_t rawtime;
	time(&rawtime); 
	*((int*)(package+clockCurTime)) = rawtime;

	package[DATA_PACKAGE_SIZE-1] = '\0';
	
}

char getCharFromPackage(int position, char* package) {
	return package[position];
}

int getIntFromPackage(int position, char* package) {
	return *((int *)(package+position));
}

time_t getTimeFromPackage(int position, char* package) {
	return *((time_t *)(package+position));
}

void printPackage(char* package) {
	printf("Bump Left: %i\n",package[snsBumpLeft]);
	printf("Bump Right: %i\n",package[snsBumpRight]);
	printf("Cliff: %i\n",package[snsCliff]);
	printf("VWall: %i\n",package[snsVWall]);

	printf("Inital State %i\n",getIntFromPackage(stateInitial,package));
	printf("Final State %i\n", getIntFromPackage(stateFinal,package));
	printf("Transtion %i\n", getIntFromPackage(transitionID,package));  

	printf("Last State Change: %i\n",getTimeFromPackage(clockLastSet,package));
	printf("Cur Time %i\n",getIntFromPackage(clockCurTime,package));
}

/*
	 int main(void) {

	 char package[dataPackageSize];  

	 int state = 10;
	 int nextState = 33;
	 int clockDuration = 0;

	 char test[10] = "asdf";
	 test[0] = SENSOR_BUMP_LEFT;

	 packageData(package,test, state, clockDuration);
	 printPackage(package);
	 packageEventData(package, nextState);
	 printPackage(package); 

//printf("%s\n",test2);
return 0;
}
 */
