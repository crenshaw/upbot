#include "unitTest.h"
#include "superisor.h"

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

	return world;
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
	static int heading;

	if(timeStamp == 0)
	{
		world = initWorld();
		heading = HDG_E;
	}
	
	switch(command)
	{
		case CMD_LEFT:
			heading = (heading > HDG_N ? heading - 1 : HDG_NW);
			break;
		case CMD_RIGHT:
			heading = (heading < HDG_NW ? heading + 1 : HDG_N);
			break;
		case CMD_FORWARD:
			break;
		case CMD_BACKWARD:
			break;
		case CMD_BLINK:
			break;
		case CMD_NO_OP:
		default:
			break;
	}



	timeStamp++;

	return "hello";
}// unitTest2

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
