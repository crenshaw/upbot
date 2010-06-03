#include "supervisor.h"

// Begin main function
int main(int argc, const char* argv[])
{
	int i;		// index for loop
	char* tmp;	// c string containing fake sensor data
	g_episodeList = newVector();

	// seed the random number generator to ensure higher degree of random
	srand(time(NULL));

	// allocate and fill space for episodes
	for(i = 0; i < atoi(argv[1]); i++)
	{
		tmp = unitTest();
		addEpisode(g_episodeList, parseEpisode(tmp));
		free(tmp);
	//	displayEpisode(g_episodeList->array[g_episodeList->size - 1]);
	}

	int* score = (int*) malloc(sizeof(int));

	int indexMatch = match(g_episodeList, score);

	printf("The closest matching series is at index %i with a score of %i\n", indexMatch, *score);


	// Free memory taken by episode list
	freeVector(g_episodeList);
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
int tick(Episode *episode)
{
	/*
		Insert processing code here
	*/
	return 0;
}// tick

/**
* parseEpisode
*
*        dataArr contains string of the following format
*        0000000011  <will be timestamp>  <abort signal>
*
* Take a raw sensor packet from Roomba and parse information
* out to an instance of Episode.
*
* @arg parsedData A pointer to an Episode to be populated
* @arg dataArr the char array that contains the raw sensor data
* @return int an error code
*
*/
int parseEpisode(Episode * parsedData, char* dataArr)
{
	// Allocate space for an episode
	int i;
	char* tmp;
	int foundDigitCount = 0;

	// set the episodes sensor values to the sensor data
	for(i = 0; i < NUM_SENSORS; i++)
	{
		int bit = (dataArr[i] - '0');
		if ((bit < 0) || (bit > 1))
		{
			return -1;	
		}

		parsedData->sensors[i] = bit;
	}

	// Pull out the timestamp
	parsedData->now = atoi(&dataArr[i]);

	// set these to default values for now
	parsedData->aborted = 0;
	parsedData->cmd = CMD_NO_OP;

	return 0;
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
