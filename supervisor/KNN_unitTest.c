/*
* unitTest.c
*
* This is a unit test for the knearest mini-module.
* There are up to 4 possible command line arguments. Any number can
* be passed, but ALL arguments before the final argument must be presented.
* In order:
*
* knum (int): K for KNN (The number of neighbors to gather)
* dataVariety (int): The number of possible varieties of episode (eg: command)
* memLen (int): The number of episodes to create in memory
* finToPrint (int): The number of final episodes to print
*
* Author: Zachary Paul Faltersack
* Last Edit: March 17, 2011
*/

#include <stdio.h>
#include <stdlib.h>

#include "knearest.h"

#define TRUE 1          // Booleans
#define FALSE 0

#define MEMLEN 1000     // The length of our episodic memory
#define DATAVARIETY 10  // The variety of data in our test episodes
#define KNUM 8          // Neighborhood size

#define FINTOPRINT 10   // Final number of episodes to print from memory

// Mini testing episode
typedef struct EpisodeStruct
{
    int time;
    int data;
} Episode;

// Necessary for KNN
int calculateNValue(int index);
void locateKNearestNeighbors(KN_Neighborhood* nbrHood);
// Necessary - Should already exist
int match(Episode* ep1, Episode* ep2);
void displayEpisode(Episode* ep);
// Not Necessary - Convenient for debugging
void displayNeighborhood(KN_Neighborhood* nbrHood, int includeSequence);
void displayNeighborSequence(int index, int nm);
void displayFinalZEpisodes(int z);

// Array for our episodic memory
Episode** g_epMem;

// Globals allowing cmdline args to modify test parameters
int g_knum = KNUM;
int g_dataVariety = DATAVARIETY;
int g_memLen = MEMLEN;
int g_finToPrint = FINTOPRINT;

/*
* main
*
* You know.
*/
int main(int argc, char* argv[])
{
    // Set up optional custom values for testing
    // Could be done nicer, but good enough
    if(argc > 1)
    {
        g_knum = atoi(argv[1]);
        if(argc > 2)
        {
            g_dataVariety = atoi(argv[2]);
            if(argc > 3)
            {
                g_memLen = atoi(argv[3]);
                if(argc > 4)
                {
                    g_finToPrint = atoi(argv[4]);
                }//if
            }//if
        }//if
    }//if

    // Ready the troops
    g_epMem = (Episode**)malloc(g_memLen * sizeof(Episode*));
    srand(time(NULL));

    // Set up some episodic memory to iterate over
    int i;
    for(i = 0; i < g_memLen; i++)
    {
        g_epMem[i] = (Episode*)malloc(sizeof(Episode));
        g_epMem[i]->time = i;
        g_epMem[i]->data = rand()%g_dataVariety;
    }//for

    // Ready the neighborhood we'll be using.
    // Using a generic id(1). In the McCallum code, this was an action
    // that identified an episode. A separate neighborhood is created for
    // each potential action that could be the next action. 
    KN_Neighborhood* neighborhood = KN_initNeighborhood(1, g_knum);

    // In the McCallum code this neighborhood id is passed here
    // to be used to set up the potential next episode for the purposes
    // of the string comparisons.
    locateKNearestNeighbors(neighborhood);

    // Print test parameters
    printf("\n==> Test Parameters <==\n\n");
    printf("KNum: %i\nDataVariety: %i\nMemLen: %i\n\n", g_knum, g_dataVariety, g_memLen);

    // Final series of episodes to compare our results with
    printf("==> Final Series Of Episodes <==\n\n");
    displayFinalZEpisodes(g_finToPrint);

    // Let's look at the results
    printf("==> Neighborhood Residents And Their Neighborhood Metric <==\n\n");
    displayNeighborhood(neighborhood, TRUE);

    // Give back memory
    printf("==> Freeing Memory : ");
    for(i = 0; i < g_memLen; i++) free(g_epMem[i]);
    free(g_epMem);
    printf("Memory Freed <==\n\n");

    printf("Unit Test Complete\n\n");

    return 0;
}//main

//--------------------------------------------------------------------------------
// Functions Necessary For KNN

/*
* calculateNValue
*
* Calculate the neighborhood metric for the episode at the given index.
*
* @arg index An integer indicating the episode whose metric is being calculated
* @return int Indicating the neighborhood metric calculated for the given episode
*/
int calculateNValue(int index)
{
    int nVal = 0;
    while((index - nVal) >= 0 && 
          match(g_epMem[index - nVal], g_epMem[g_memLen - 1 - nVal])) 
          nVal++;

    return nVal;
}//calculateNValue

/*
* locateKNearestNeighbors
*
* Iterate over the episodic memory and calculate a neighborhood metric
* for each episode. This must be from oldest to most recent. Then the
* episode and its neighborhood metric are passed to the KNN functions
* to populate the given neighborhood.
*
* @arg nbrHood A KN_Neighborhood* presenting the neighborhood to be populated
*/
void locateKNearestNeighbors(KN_Neighborhood* nbrHood)
{
    int i,n;
    for(i = 0; i < g_memLen - 1; i++) KN_addNeighbor(nbrHood, g_epMem[i], calculateNValue(i));
}//locateKNearestNeighbors

//--------------------------------------------------------------------------------
// Functions Necessary For KNN - Should already exist

/*
* match
*
* Compare two episodes and return a boolean value idicating their equality
* by the chosen metric.
*
* @arg ep1 An Episode* to the first episode
* @arg ep2 An Episode* to the second episode
* @return int Boolean indicating equality (TRUE/FALSE)
*/
int match(Episode* ep1, Episode* ep2)
{
    if(ep1->data == ep2->data) return TRUE;

    return FALSE;
}//match

/*
* displayEpisode
*
* Print the episode's contents.
*
* @arg ep An Episode* to the episode being printed
*/
void displayEpisode(Episode* ep)
{
    printf("Episode at time: %i has data: %i\n", ep->time, ep->data);
}//displayEpisode

//--------------------------------------------------------------------------------
// Non-Necessary Functions - Nice for debugging

/*
* displayNeighborhood
*
* Print each member within the given KN_Neighborhood.
*
* @arg nbrHood A KN_Neighborhood* to the neighborhood being displayed
* @arg includeSequence A boolean value indicating if the preceding sequence should
*                       be printed
*/
void displayNeighborhood(KN_Neighborhood* nbrHood, int includeSequence)
{
    int i;
    for(i = 0; i < g_knum; i++)
    {
        printf("[Neighbor %i has NM %i] ", i, nbrHood->nValues[i]);
        displayEpisode(nbrHood->neighbors[i]); // This should always be the same data
        if(includeSequence) 
        {
            displayNeighborSequence(((Episode*)nbrHood->neighbors[i])->time, nbrHood->nValues[i]);
        }
    }//for
}//displayNeighborhood

/*
* displayNeighborSequence
*
* Display the sequence leading up to and including the episode at the given index.
*
* @arg index An integer idicating the root episode of the sequence
* @arg nm An integer indicating the length of the sequence
*/
void displayNeighborSequence(int index, int nm)
{
    int i;
    for(i = 0; i < nm; i++)
    {
        printf("\t>");
        displayEpisode(g_epMem[index - i]);
    }
    printf("\n");
}//displayNeighborhood

/*
* displayFinalZEpisodes
*
* Print the final episodes in memory.
*
* @arg z The number of final episodes to print
*/
void displayFinalZEpisodes(int z)
{
    printf("Final %i Episodes In Memory:\n\n", z);
    displayNeighborSequence(g_memLen - 1, z);
}//displayFinalZEpisodes
