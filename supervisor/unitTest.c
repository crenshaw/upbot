#include "unitTest.h"
#include "supervisor.h"

/**
* initWorld
* 
* This function is called to allocate and initialize the 2d world
* array that contains a map + location for the Roomba.
* World is defined by columns in switch statement, so world can be
* easily changed by defining different columns and ordering them.
*
* 0 represents a hallway
* 1 represents a wall
* 2 represents the Roomba
*
* @return int** to 2d world array
*/
int** initWorld()
{
	// allocate space for ptrs to columns
	int** world = (int**) malloc(9 * sizeof(int*));
	int i, j;
	// allocate columns
	for(i = 0; i < 9; i++)
	{
		world[i] = (int*) malloc(7 * sizeof(int));
	}// for

	// initialize the values to represent walls and hallways
	for(i = 0; i < 9; i++)
		for(j = 0; j < 7; j++)
		{
			switch(i)
			{
				case 0:
				case 8:
					world[i][j] = V_WALL;
					break;
				case 1:
				case 4:
				case 7:
					if(j == 0 || j == 6)
						world[i][j] = V_WALL;
					else
						world[i][j] = V_HALLWAY;
					break;
				case 2:
				case 3:
				case 5:
				case 6:
					if(j == 0 || j== 2 || j == 3 || j == 4 || j == 6)
						world[i][j] = V_WALL;
					else
						world[i][j] = V_HALLWAY;
					break;
			}
		}//for

	// Set up Roomba starting location
	world[1][1] = V_ROOMBA;
	// Set up goal
	world[7][5] = V_GOAL;

	return world;
}

/**
 * freeWorld
 *
 * This function frees the map of the environment used by the unit test
 */
void freeWorld(int** world)
{
	int i;
	for(i = 0; i < 9; i++)
	{
		free(world[i]);
	}
	free(world);
}

/**
 * unitTest2
 *
 * This subroutine emulates a Roomba in McKallum's gridworld.
 * It receives an action from the supervisor and updates a world map with its
 * location. This allows us to determine the next set of sensor data to return
 * to the supervisor.
 *
 * @arg command This is a command from the supervisor
 * @arg cleanup 1: unit test is over, 2: continue unit test
 * @return char* a string containing fake sensor data
 */
char* unitTest2(int command, int cleanup)
{
	static int timeStamp = 0; // counter acting as time stamp
	static int** world;	// 2d array to ints that represents the world
	static int x, y;	// world coords
	static int heading;	// the direction we're pointing
	static int hitGoal;	// did we hit the goal last time?
	int	lBump, rBump, lCliff, rCliff, lCliffFront, rCliffFront,
		IR, caster, lDrop, rDrop;	// Sensor vars
	int sensorReturn;
	int abort;
	char* str = (char*) malloc(sizeof(char) * 24); // memory for data

	if(cleanup == 1)
	{
		freeWorld(world);
		return str;
	}

	// init sensor values
	lBump = rBump = lCliff = rCliff = lCliffFront = rCliffFront = IR = caster = lDrop = rDrop = SNSR_OFF;
	sensorReturn = NONE_HIT;
	abort = SNSR_OFF;

	if(timeStamp == 0)
	{
		world = initWorld();	// initialize the world
		hitGoal = 1;			// init to 1 so it becomes 0 in next check;
		x = y = 1;
		if(g_statsMode == 0)
		{
			printf("Initial World View\n");
			displayWorld(world, heading);
		}
	}

	// If we hit a goal last time reset Roomba location and heading
	// and reset goal location
	if(hitGoal == SNSR_ON)
	{
		hitGoal = SNSR_OFF;
		heading = HDG_E;
		world[x][y] = V_HALLWAY;
		x = y = 1;
		if(g_statsMode == 1)
		{
			if(timeStamp != 0)
			{
				printf("Hit Goal\n");
			}
		}
	}

	// Switch on the command that was received to update world appropriately
	switch(command)
	{
		// Left and Right just turn the Roomba
		case CMD_LEFT:
			if(g_statsMode == 0) {
				printf("Turn left 45 degrees\n");
			}
			heading = (heading > HDG_N ? heading - 1 : HDG_NW);
			break;
		case CMD_RIGHT:
			if(g_statsMode == 0) {
				printf("Turn right 45 degrees\n");
			}
			heading = (heading < HDG_NW ? heading + 1 : HDG_N);
			break;
			// Adjusts currently not implemented in 'perfect' world
		case CMD_ADJUST_LEFT:
			if(g_statsMode == 0) {
				printf("Adjust left\n");
			}
			break;
		case CMD_ADJUST_RIGHT:
			if(g_statsMode == 0) {
				printf("Adjust right\n");
			}
			break;
			// Forward must check for walls and IR in direction of heading
		case CMD_FORWARD:
			if(g_statsMode == 0) {
				printf("Move forward one world unit\n");
			}

			// First check the diagonal headings
			if(heading == HDG_NE)
			{
				sensorReturn = bumpSensor(world[x][y-1], world[x+1][y]);
			}
			else if(heading == HDG_SE)
			{
				sensorReturn = bumpSensor(world[x+1][y], world[x][y+1]);
			}
			else if(heading == HDG_SW)
			{
				sensorReturn = bumpSensor(world[x][y+1], world[x-1][y]);
			}
			else if(heading == HDG_NW)
			{
				sensorReturn = bumpSensor(world[x-1][y], world[x][y-1]);
			}
			// Next check the horizontal and vertical headings
			// Also begin check for IR in spot we move to
			else if(heading == HDG_N)
			{
				if(world[x][y-1] == V_WALL)
				{
					sensorReturn = BOTH_HIT;
				}
				else if(world[x][y-1] == V_GOAL)
				{
					IR = SNSR_ON;
				}
			}
			else if(heading == HDG_E)
			{
				if(world[x+1][y] == V_WALL)
				{
					sensorReturn = BOTH_HIT;
				}
				else if(world[x+1][y] == V_GOAL)
				{
					IR = SNSR_ON;
				}
			}
			else if(heading == HDG_S)
			{
				if(world[x][y+1] == V_WALL)
				{
					sensorReturn = BOTH_HIT;
				}
				else if(world[x][y+1] == V_GOAL)
				{
					IR = SNSR_ON;
				}
			}
			else if(heading == HDG_W)
			{
				if(world[x-1][y] == V_WALL)
				{
					sensorReturn = BOTH_HIT;
				}
				else if(world[x-1][y] == V_GOAL)
				{
					IR = SNSR_ON;
				}
			}

			if(IR == SNSR_ON)
			{
				hitGoal = SNSR_ON;
			}
			else if(sensorReturn == BOTH_HIT)
			{
				lBump = rBump = SNSR_ON;
				abort = SNSR_ON;
			}
			else if(sensorReturn == RIGHT_HIT)
			{
				rBump = SNSR_ON;
				abort = SNSR_ON;
			}
			else if(sensorReturn == LEFT_HIT)
			{
				lBump = SNSR_ON;
				abort = SNSR_ON;
			}
			else if(sensorReturn == NONE_HIT)
			{
				switch(heading)
				{
					case HDG_N:
						world[x][y] = V_HALLWAY;
						y--;
						world[x][y] = V_ROOMBA;
						break;
					case HDG_E:
						world[x][y] = V_HALLWAY;
						x++;
						world[x][y] = V_ROOMBA;
						break;
					case HDG_S:
						world[x][y] = V_HALLWAY;
						y++;
						world[x][y] = V_ROOMBA;
						break;
					case HDG_W:
						world[x][y] = V_HALLWAY;
						x--;
						world[x][y] = V_ROOMBA;
						break;
				}
			}

			break;
		case CMD_BLINK:
			if(g_statsMode == 0) {
				printf("Blink\n");
			}
			break;
		case CMD_NO_OP:
			if(g_statsMode == 0) {
				printf("No operation\n");
			}
			break;
		case CMD_SONG:
			if(g_statsMode == 0)
			{
				printf("Song\n");
			}
			break;
		default:
			if(g_statsMode == 0) {
				printf("Invalid command: %i\n", command);
			}
			break;
	}

	// Fill out sensor string
	sprintf(&str[SNSR_IR],				"%i", IR);
	sprintf(&str[SNSR_CLIFF_RIGHT], 	"%i", rCliff);
	sprintf(&str[SNSR_CLIFF_F_RIGHT], 	"%i", rCliffFront);
	sprintf(&str[SNSR_CLIFF_F_LEFT], 	"%i", lCliffFront);
	sprintf(&str[SNSR_CLIFF_LEFT], 		"%i", lCliff);
	sprintf(&str[SNSR_CASTER], 			"%i", caster);
	sprintf(&str[SNSR_DROP_LEFT],		"%i", lDrop);
	sprintf(&str[SNSR_DROP_RIGHT], 		"%i", rDrop);
	sprintf(&str[SNSR_BUMP_LEFT], 		"%i", lBump);
	sprintf(&str[SNSR_BUMP_RIGHT], 		"%i", rBump);

	// Fill out rest of episode string with timestamp and abort signal
	int i, temp;
	for(i = 10; i < 24; i++)
	{
		// insert blank space
		if(i < 15)
		{
			str[i] = ' ';
			// insert timestamp
		}else if(i < 16)
		{
			sprintf(&str[i], "%i", timeStamp);	// print timestamp into str
			// determine num digits in timeStamp and adjust i accordingly
			temp = timeStamp;
			while(temp > 9)
			{
				temp = temp / 10;
				i++;
			}
			// more padding
		}else if(i < 22)
		{
			str[i] = ' ';
			// insert null terminating char
		}else if(i < 23)
		{
			sprintf(&str[i], "%i", abort);
		}else
		{
			str[i] = '\0';
		}
	}

	if(g_statsMode == 0) {
		displayWorld(world, heading);
	}


	timeStamp++;

	return str;
}// unitTest2

/**
 * bumpSensor
 *
 * This function returns a value that tells the calling function
 * which bump sensors may or may not have been hit
 *
 * @arg north int that represents the wall N of the Roomba when normalized to point NE
 * @arg east int that represents the wall E of the Roomba when normalized to point NE
 * @return int that represents bump sensor values
 */
int bumpSensor(int north, int east)
{
	// return the appropriate value representing which, if any, bumpers are hit
	// account for value representing goal spot (-1) which is also a hallway
	if((north == V_HALLWAY || north == V_GOAL) && (east == V_HALLWAY || east == V_GOAL))
		return BOTH_HIT;
	if((north == V_HALLWAY || north == V_GOAL) && east == V_WALL)
		return RIGHT_HIT;
	if(north == V_WALL && (east == V_HALLWAY || east == V_GOAL))
		return LEFT_HIT;
	if(north == V_WALL && east == V_WALL)
		return BOTH_HIT;

	return -1;
}

/**
 * displayWorld
 *
 * Print the current view of the world
 *
 * @arg world 2d array of ints representing the world (-1:goal, 0:hallway, 1:wall, 2:Roomba)
 */
void displayWorld(int** world, int heading)
{
	int i,j;
	for(i = 0; i < 7; i++)
	{
		for(j = 0; j < 9; j++)
		{
			if(world[j][i] == V_WALL)
			{
				printf("W");
			}else if(world[j][i] == V_HALLWAY)
			{
				printf(" ");
			}else if(world[j][i] == V_GOAL)
			{
				printf("G");
			}else if(world[j][i] == V_ROOMBA)
			{
				switch(heading)
				{
					case HDG_N:
						printf("^");
						break;
					case HDG_NE:
						printf("/");
						break;
					case HDG_E:
						printf(">");
						break;
					case HDG_SE:
						printf("\\");
						break;
					case HDG_S:
						printf("v");
						break;
					case HDG_SW:
						printf("L");
						break;
					case HDG_W:
						printf("<");
						break;
					case HDG_NW:
						printf("*");
						break;
				}
			}
		}
		printf("\n");
	}
}


/**
 * unitTest
 *
 * This subroutine spits back a false set of sensor data
 * for testing purposes. As of now the fake data only contains
 * 8 bits representing the actual data, and a single int as a timestamp
 *
 * @return char* a string containing the fake sensor data
 */
char* unitTest()
{
	static int timeStamp = 0; // counter for num times routine is called
	char* str = (char*) malloc(sizeof(char) * 24); // memory for data
	int temp;	// temp int for various purposes
	int i;		// index for looping
	char random;// random char (0/1) to represent the fake data

	for(i = 0; i < sizeof(*str)*24/sizeof(char); i++)
	{
		// first 8 bits are the 0/1 data values
		if(i < 8)
		{
			random = rand() % 2 + '0'; // random number % 2 returns 0 or 1
			str[i] = random;
			// insert blank space
		}else if(i < 15)
		{
			str[i] = ' ';
			// insert timestamp
		}else if(i < 16)
		{
			sprintf(&str[i], "%i", timeStamp);	// print timestamp into str
			// determine num digits in timeStamp and adjust i accordingly
			temp = timeStamp;
			while(temp > 9)
			{
				temp = temp / 10;
				i++;
			}
			// more padding
		}else if(i < 23)
		{
			str[i] = ' ';
			// insert null terminating char
		}else
		{
			str[i] = '\0';
		}
	}
	// increase time
	timeStamp++;
	// return the fake data
	return str;
}// unitTest
