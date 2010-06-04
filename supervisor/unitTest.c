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
					world[i][j] = 1;
					break;
				case 1:
				case 4:
				case 7:
					if(j == 0 || j == 6)
						world[i][j] = 1;
					else
						world[i][j] = 0;
					break;
				case 2:
				case 3:
				case 5:
				case 6:
					if(j == 0 || j== 2 || j == 3 || j == 4 || j == 6)
						world[i][j] = 1;
					else
						world[i][j] = 0;
					break;
			}
		}//for

		// Set up Roomba starting location
		world[1][1] = 2;
		// Set up goal
		world[7][5] = -1;

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
* @arg int This is a command from the supervisor
* @return char* a string containing fake sensor data
*/
char* unitTest2(int command)
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

	// init sensor values
	lBump = rBump = lCliff = rCliff = lCliffFront = rCliffFront = IR = caster = lDrop = rDrop = 0;
	sensorReturn = 0;
	abort = 0;

	if(timeStamp == 0)
	{
		world = initWorld();	// initialize the world
		x = y = 1;				// set the initial location for Roomba
		heading = HDG_E;		// init the heading
		hitGoal = 1;			// init to 1 so it becomes 0 in next check;
		printf("Initial World View\n");
		displayWorld(world, heading);
	}

	// If we hit a goal last time reset Roomba location and heading
	// and reset goal location
	if(hitGoal == 1)
	{
		hitGoal = 0;
		heading = HDG_E;
		x = y = 1;
	}

	// Switch on the command that was received to update world appropriately
	switch(command)
	{
		// Left and Right just turn the Roomba
		case CMD_LEFT:
			printf("Turn left 45 degrees\n");
			heading = (heading > HDG_N ? heading - 1 : HDG_NW);
			break;
		case CMD_RIGHT:
			printf("Turn right 45 degrees\n");
			heading = (heading < HDG_NW ? heading + 1 : HDG_N);
			break;
		// Adjusts currently not implemented in 'perfect' world
		case CMD_ADJUST_LEFT:
			printf("Adjust left\n");
			break;
		case CMD_ADJUST_RIGHT:
			printf("Adjust right\n");
			break;
		// Forward must check for walls and IR in direction of heading
		case CMD_FORWARD:
			printf("Move forward one world unit\n");

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
				if(world[x][y-1] == 1)
				{
					sensorReturn = 3;
				}
				else if(world[x][y-1] == -1)
				{
					IR = 1;
				}
			}
			else if(heading == HDG_E)
			{
				if(world[x+1][y] == 1)
				{
					sensorReturn = 3;
				}
				else if(world[x+1][y] == -1)
				{
					IR = 1;
				}
			}
			else if(heading == HDG_S)
			{
				if(world[x][y+1] == 1)
				{
					sensorReturn = 3;
				}
				else if(world[x][y+1] == -1)
				{
					IR = 1;
				}
			}
			else if(heading == HDG_W)
			{
				if(world[x-1][y] == 1)
				{
					sensorReturn = 3;
				}
				else if(world[x-1][y] == -1)
				{
					IR = 1;
				}
			}

			if(IR == 1)
			{
				heading = 1;
			}
			else if(sensorReturn == 3)
			{
				lBump = rBump = 1;
				abort = 1;
			}
			else if(sensorReturn == 2)
			{
				rBump = 1;
				abort = 1;
			}
			else if(sensorReturn == 1)
			{
				lBump = 1;
				abort = 1;
			}
			else if(sensorReturn == 0)
			{
				switch(heading)
				{
					case HDG_N:
						world[x][y] = 0;
						y--;
						world[x][y] = 2;
						break;
					case HDG_E:
						world[x][y] = 0;
						x++;
						world[x][y] = 2;
						break;
					case HDG_S:
						world[x][y] = 0;
						y++;
						world[x][y] = 2;
						break;
					case HDG_W:
						world[x][y] = 0;
						x--;
						world[x][y] = 2;
						break;
				}
			}
			
			break;
		// cmd backward and blink not imlpemented in 'perfect' world
		case CMD_BACKWARD:
			printf("Move backwards one world unit\n");
			break;
		case CMD_BLINK:
			printf("Blink\n");
			break;
		case CMD_NO_OP:
			printf("No operation\n");
			break;
		default:
			printf("Invalid command: %i\n", command);
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

	displayWorld(world, heading);

	timeStamp++;

	return str;
}// unitTest2

/**
* bumpSensor
*
* This function returns a value that tells the calling function
* which bump sensors may or may not have been hit
*
* -1 - error
* 1 - left
* 2 - right
* 3 - both
*
* @arg north int that represents the wall N of the Roomba when normalized to point NE
* @arg east int that represents the wall E of the Roomba when normalized to point NE
* @return int that represents bump sensor values, see above
*/
int bumpSensor(int north, int east)
{
	// return the appropriate value representing which, if any, bumpers are hit
	// account for value representing goal spot (-1) which is also a hallway
	if((north == 0 || north == -1) && (east == 0 || east == -1))
		return 3;
	if((north == 0 || north == -1) && east == 1)
		return 2;
	if(north == 1 && (east == 0 || east == -1))
		return 1;
	if(north == 1 && east == 1)
		return 3;

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
			if(world[j][i] == 1)
			{
				printf("W");
			}else if(world[j][i] == 0)
			{
				printf(" ");
			}else if(world[j][i] == -1)
			{
				printf("G");
			}else if(world[j][i] == 2)
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
