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

	if(timeStamp == 0)
		world == initWorld();

	


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

// Begin main function
int main(int argc, const char* argv[])
{
	int i;		// index for loop
	char* tmp;	// c string containing fake sensor data
	Vector* episodeList = newVector();

	// seed the random number generator to ensure higher degree of random
	srand(time(NULL));

	// allocate and fill space for episodes
	for(i = 0; i < atoi(argv[1]); i++)
	{
		tmp = unitTest();
		addEpisode(episodeList, parseEpisode(tmp));
		free(tmp);
	//	displayEpisode(episodeList->array[episodeList->size - 1]);
	}

	int* score = (int*) malloc(sizeof(int));

	int indexMatch = match(episodeList, score);

	printf("The closest matching series is at index %i with a score of %i\n", indexMatch, *score);


	// Free memory taken by episode list
	freeVector(episodeList);
	return 0;
}// main

/**
* tick
*
* This function is called at regular intervals and processes
* the recent sensor data to determine the next action to take.
*
* @param Episode * sd : a pointer to a Episode struct
* @return int : a status code
*/
int tick(Vector* vector, Episode *episode)
{
	/*
		Insert processing code here
	*/
	return 0;
}// tick

/**
* parseEpisode
*
* Take a raw sensor packet from Roomba and parse information
* out to an instance of Episode.
*
* @arg dataArr : the raw char[] that contains the sensor data
* @return Episode* : an instance of Episode that has been 
*	initialized to contain the data from the data packet
*/
Episode * parseEpisode(char* dataArr)
{
	// Allocate space for an episode
	Episode* parsedData = (Episode*) malloc(sizeof(Episode));
	int i;
	char* tmp;
	int foundDigitCount = 0;

	// yank off sensor bit array
	int sensor = atoi(dataArr);
	
	// set the episodes sensor values to the sensor data
	for(i = 0; i < 8; i++)
	{
		parsedData->sensors[7 - i] = sensor % 10;
		sensor = sensor / 10;
	}

	// Pull out the timestamp
	parsedData->now = atoi(&dataArr[i]);

	// set these to default values for now
	parsedData->aborted = 0;
	parsedData->cmd = CMD_NO_OP;

	return parsedData;
}// parseEpisode

/**
* addEpisode
*
* Add new episode to episode history vector
*
* @arg episodes pointer to vector containing episodes
* @arg item pointer to episode to be added
* @return int status code (0 == success)
*/
int addEpisode(Vector* episodes, Episode* item)
{
	return addEntry(episodes, item);
}// addEpisode

/**
* displayEpisode
*
* Display the contents of an episode
*
* @arg ep a pointer to an episode
*/
void displayEpisode(Episode * ep)
{
	int i;
	printf("Sensors:    ");
	// iterate through sensor values and print to stdout
	for(i = 0; i < 8; i++)
	{
		printf("%i", ep->sensors[i]);
	}
	// print rest of episode data to stdout
	printf("\nTime stamp: %i\nAborted:    %i\nCommand:    %i\n\n", ep->now, ep->aborted, ep->cmd);
}// displayEpisode

/**
* match
*
* search for a series of episodes that best matches the last NUM_TO_MATCH
* episodes
*
* @arg vector the vector containing full history of episodes
* @arg score a pointer to int we can use to store/return score
* @return int index into the vector for the best matching series
*/
int match(Vector* vector, int* score)
{
	int i,j, tempScore = 0, returnIdx = 0;
	*score = 0;

	// Iterate through vector and search from each index
	for(i = vector->size - NUM_TO_MATCH - 1; i >=0; i--)
	{
		// reset tempscore
		tempScore = 0;
		for(j = 0; j < NUM_TO_MATCH; j++)
		{
			// compare two episodes and add result to score
			tempScore += compare(vector->array[i + j], vector->array[vector->size - NUM_TO_MATCH + j]);
		}
		// If we ended up with a greater score than previous, store index and score
		if(tempScore > *score)
		{
			*score = tempScore;
			returnIdx = i;
		}
	}
	return returnIdx;
}// match

/**
* compare
*
* Compare the sensor arrays of two episodes and return if they match or not
*
* @arg ep1 a pointer to an episode
* @arg ep2 a pointer to another episode
* @return int (0/1) Did they match?
*/
int compare(Episode* ep1, Episode* ep2)
{
	int i, match = 0;
	for(i = 0; i < 8; i++)
	{
		if(ep1->sensors[i] == ep2->sensors[i])
			match++;
	}
	return (match == 8 ? 1 : 0);
}// compare
