#include <string.h>
#include "eaters.h"
#include "vector.h"
#include "supervisor.h"

int main()
{
	printf("Initializing world\n");
	initWorld(TRUE);
	printf("Printing world\n");
	displayWorld();

	printf("Running through commands\n");
	printf("Moving east\n");
	printf("%s\n", unitTest(MOVE_E, FALSE));
	printf("Moving south\n");
	printf("%s\n", unitTest(MOVE_S, FALSE));
	printf("Moving west\n");
	printf("%s\n", unitTest(MOVE_W, FALSE));
	printf("Moving north\n");
	printf("%s\n", unitTest(MOVE_N, FALSE));

	printf("Freeing memory\n");
	freeWorld();

	return 0;
}//main


