#include "unitTest.h"

/**
* initWorld
* 
* This function is called to allocate and initialize the 2D world
* array that contains a map + location for the Roomba.
* World is defined by columns in switch statement, so world can be
* easily changed by defining different columns and ordering them.
*
*/
void initWorld()
{
	// Do not want stats mode most of the time
	g_statsMode = 0;

	// Set up global vars relating to location and orientation of Roomba
	g_heading	= HDG_E;
	g_X			= X_INIT;
	g_Y			= Y_INIT;
	g_hitGoal	= FALSE;

	// allocate space for ptrs to columns
	g_world = (int**) malloc(MAP_WIDTH * sizeof(int*));

	int i, j;
	// allocate columns
	for(i = 0; i < MAP_WIDTH; i++)
	{
		g_world[i] = (int*) malloc(MAP_HEIGHT * sizeof(int));
	}// for

	// initialize the values to represent walls and hallways
	for(i = 0; i < MAP_WIDTH; i++)
		for(j = 0; j < MAP_HEIGHT; j++)
		{
			switch(i)
			{
				case 0:
				case 8:
					g_world[i][j] = V_WALL;
					break;
				case 1:
				case 4:
				case 7:
					if(j == 0 || j == 6)
						g_world[i][j] = V_WALL;
					else
						g_world[i][j] = V_HALLWAY;
					break;
				case 2:
				case 3:
				case 5:
				case 6:
					if(j == 0 || j== 2 || j == 3 || j == 4 || j == 6)
						g_world[i][j] = V_WALL;
					else
						g_world[i][j] = V_HALLWAY;
					break;
			}
		}//for

    //%%%%DEBUGGING
    g_world[X_INIT][Y_INIT+2] = V_GOAL;
    

	// Set up Roomba starting location
	g_world[X_INIT][Y_INIT] = V_ROOMBA;
	// Set up goal
	g_world[X_GOAL][Y_GOAL] = V_GOAL;
}// initWorld

/**
 * freeWorld
 *
 * This function frees the map of the environment used by the unit test
 *
 */
void freeWorld()
{
	int i;
	for(i = 0; i < MAP_WIDTH; i++)
	{
		free(g_world[i]);
	}
	free(g_world);
}// freeWorld

/**
* resetWorld
*
* After a goal is found this is called to reset the world
*
*/
void resetWorld()
{
	g_hitGoal = FALSE;
	g_heading = HDG_E;
	g_world[X_GOAL][Y_GOAL] = V_GOAL;

    //%%%DEBUGGING
    g_world[X_INIT][Y_INIT+2] = V_GOAL;

    
	g_X = X_INIT;
	g_Y = Y_INIT;

	g_world[g_X][g_Y] = V_ROOMBA;

	if(g_statsMode)	printf("Hit Goal\n");
}// resetWorld

/**
 * unitTest2
 *
 * This subroutine emulates a Roomba in the grid world defined by g_world
 * It receives an action from the supervisor and updates the world map with its
 * location. This allows us to determine the next set of sensor data to return
 * to the supervisor.
 *
 * @arg command This is a command from the supervisor
 * @arg needCleanup Use as Boolean, if TRUE then test is over and need to free memory
 *
 * @return char* a string containing fake sensor data
 *
 */
char* unitTest2(int command, int needCleanup)
{
	// Check if we've completed the unit test and need to clean up
	if(needCleanup)
	{
		freeWorld();
		return NULL;
	}

	// If we hit a goal last time reset Roomba and Goal locations
	if(g_hitGoal) resetWorld();

 	return doMove(command);
}// unitTest2

/**
 * doMove
 *
 * This function applies the command received from the Supervisor to the
 * world and returns the resulting sensor string
 */
char* doMove(int command)
{
	int	lBump, rBump, 	// Sensor vars
		lCliff, rCliff, 
		lCliffFront, rCliffFront,
		lDrop, rDrop,	
		IR, caster;
	int sensorReturn;	// Total sensor return
	int abort;			// Abort bit (deprecated)

	// init sensor values
	lBump 		= rBump 		= lCliff 	= rCliff 	= IR 		= SNSR_OFF;
	lCliffFront = rCliffFront 	= lDrop 	= rDrop 	= caster 	= SNSR_OFF;
	sensorReturn = NONE_HIT;
	abort = FALSE;

	// Switch on the command that was received to update g_world appropriatelg_Y
	switch(command)
	{
		// Left and Right just turn the Roomba
		case CMD_LEFT:
			if(!g_statsMode) printf("Turn left 45 degrees\n");
			g_heading = (g_heading > HDG_N ? g_heading - 1 : HDG_NW);
			break;
		case CMD_RIGHT:
			if(!g_statsMode) printf("Turn right 45 degrees\n");
			g_heading = (g_heading < HDG_NW ? g_heading + 1 : HDG_N);
			break;
			// Adjusts currentlg_Y not implemented in 'perfect' g_world
		case CMD_ADJUST_LEFT:
			if(!g_statsMode) printf("Adjust left\n");
			break;
		case CMD_ADJUST_RIGHT:
			if(!g_statsMode) printf("Adjust right\n");
			break;
			// Forward must check for walls and IR in direction of g_heading
		case CMD_FORWARD:
			if(!g_statsMode) printf("Move forward one world unit\n");

			// First check the diagonal g_headings
			if(g_heading == HDG_NE)
			{
				sensorReturn = bumpSensor(g_world[g_X][g_Y-1], g_world[g_X+1][g_Y]);
			}
			else if(g_heading == HDG_SE)
			{
				sensorReturn = bumpSensor(g_world[g_X+1][g_Y], g_world[g_X][g_Y+1]);
			}
			else if(g_heading == HDG_SW)
			{
				sensorReturn = bumpSensor(g_world[g_X][g_Y+1], g_world[g_X-1][g_Y]);
			}
			else if(g_heading == HDG_NW)
			{
				sensorReturn = bumpSensor(g_world[g_X-1][g_Y], g_world[g_X][g_Y-1]);
			}
			// Next check the horizontal and vertical headings
			// Also begin check for IR in spot we move to
			else if(g_heading == HDG_N)
			{
				if(g_world[g_X][g_Y-1] == V_WALL)
				{
					sensorReturn = BOTH_HIT;
				}
				else if(g_world[g_X][g_Y-1] == V_GOAL)
				{
					IR = SNSR_ON;
				}
			}
			else if(g_heading == HDG_E)
			{
				if(g_world[g_X+1][g_Y] == V_WALL)
				{
					sensorReturn = BOTH_HIT;
				}
				else if(g_world[g_X+1][g_Y] == V_GOAL)
				{
					IR = SNSR_ON;
				}
			}
			else if(g_heading == HDG_S)
			{
				if(g_world[g_X][g_Y+1] == V_WALL)
				{
					sensorReturn = BOTH_HIT;
				}
				else if(g_world[g_X][g_Y+1] == V_GOAL)
				{
					IR = SNSR_ON;
				}
			}
			else if(g_heading == HDG_W)
			{
				if(g_world[g_X-1][g_Y] == V_WALL)
				{
					sensorReturn = BOTH_HIT;
				}
				else if(g_world[g_X-1][g_Y] == V_GOAL)
				{
					IR = SNSR_ON;
				}
			}

			if(sensorReturn == BOTH_HIT)
			{
				lBump = rBump = SNSR_ON;
				abort = TRUE;
			}
			else if(sensorReturn == RIGHT_HIT)
			{
				rBump = SNSR_ON;
				abort = TRUE;
			}
			else if(sensorReturn == LEFT_HIT)
			{
				lBump = SNSR_ON;
				abort = TRUE;
			}
			else if(sensorReturn == NONE_HIT)
			{
				if(IR == SNSR_ON)
				{
					g_hitGoal = TRUE;
				}

				switch(g_heading)
				{
					case HDG_N:
						g_world[g_X][g_Y] = V_HALLWAY;
						g_Y--;
						g_world[g_X][g_Y] = V_ROOMBA;
						break;
					case HDG_E:
						g_world[g_X][g_Y] = V_HALLWAY;
						g_X++;
						g_world[g_X][g_Y] = V_ROOMBA;
						break;
					case HDG_S:
						g_world[g_X][g_Y] = V_HALLWAY;
						g_Y++;
						g_world[g_X][g_Y] = V_ROOMBA;
						break;
					case HDG_W:
						g_world[g_X][g_Y] = V_HALLWAY;
						g_X--;
						g_world[g_X][g_Y] = V_ROOMBA;
						break;
				}// switch
			}// if
			break;
		case CMD_BLINK:
			if(!g_statsMode) printf("Blink\n");
			break;
		case CMD_NO_OP:
			if(!g_statsMode) printf("No operation\n");
			break;
		case CMD_SONG:
			if(!g_statsMode) printf("Song\n");
			break;
		default:
			if(!g_statsMode) printf("Invalid command: %i\n", command);
			break;
	}// switch

	if(!g_statsMode) displayWorld();

	return setSensorString(IR, rCliff, rCliffFront, lCliffFront, lCliff, 
			caster, lDrop, rDrop, lBump, rBump, abort);
}// doMove

/**
 * setSensorString
 *
 * This function takes the sensor values and fills out the sensor string 
 * to be sent to the Supervisor
 *
 * @arg IR 			Sensor values
 * @arg rCliff 		^
 * @arg rCliffFront	^
 * @arg lCliffFront	^
 * @arg lCliff		^
 * @arg caster		^
 * @arg lDrop		^
 * @arg rDrop		^
 * @arg lBump		^
 * @arg rBump		^
 * @return char*	The sensor data to be sent to the Supervisor
 */
char* setSensorString(int IR, int rCliff, int rCliffFront, int lCliffFront, int lCliff,
		int caster, int lDrop, int rDrop, int lBump, int rBump, int abort)
{
	static int timeStamp = 0;
	// Memory for data string to return to Supervisor
	char* str = (char*) malloc(sizeof(char) * 24); 

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
	}// for

	timeStamp++;

	return str;
}// setSensorString

/**
 * bumpSensor
 *
 * This function returns a value that tells the calling function
 * which bump sensors have been hit
 *
 * @arg north int that represents the wall N of the Roomba when normalized to point NE
 * @arg east int that represents the wall E of the Roomba when normalized to point NE
 * @return int that represents bump sensor values
 */
int bumpSensor(int north, int east)
{
	// return the appropriate value representing which, if any, bumpers are hit
	// account for value representing goal spot which is also a hallway
	if((north == V_HALLWAY || north == V_GOAL) && (east == V_HALLWAY || east == V_GOAL))
		return BOTH_HIT;
	if((north == V_HALLWAY || north == V_GOAL) && east == V_WALL)
		return RIGHT_HIT;
	if(north == V_WALL && (east == V_HALLWAY || east == V_GOAL))
		return LEFT_HIT;
	if(north == V_WALL && east == V_WALL)
		return BOTH_HIT;

	return NONE_HIT;
}// bumpSensor

/**
 * displayWorld
 *
 * Print the current view of the world
 *
 */
void displayWorld()
{
	int i,j;
	for(i = 0; i < MAP_HEIGHT; i++)
	{
		for(j = 0; j < MAP_WIDTH; j++)
		{
			if(g_world[j][i] == V_WALL)
			{
				printf("W");
			}else if(g_world[j][i] == V_HALLWAY)
			{
				printf(" ");
			}else if(g_world[j][i] == V_GOAL)
			{
				printf("G");
			}else if(g_world[j][i] == V_ROOMBA)
			{
				// Have a different symbol depending on the heading of the Roomba
				// This is simply for visual feedback
				switch(g_heading)
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
				}// switch
			}// if
		}// for
		printf("\n");
	}// for
}// displayWorld


/**
 * CURRENTLY UNUSABLE
 *
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
	char* str = (char*) malloc(sizeof(char) * 24); // memorg_Y for data
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
