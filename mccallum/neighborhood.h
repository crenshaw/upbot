#ifndef _NEIGHBORHOOD_H_
#define _NEIGHBORHOOD_H_

/**
 * neighborhood.h
 *
 * This is the header file for the code that will produce
 * and maintain the neighborhoods that are needed for the
 * NSM Q-Learning agent.
 *
 * Author: Zachary Paul Faltersack
 * Last Edit: October 11, 2010
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct NeighborhoodStruct
{
	int action;			// int to represent action associated with this neighborhood
	int kValue;			// k is the number of neighbors
	int numNeighbors;	// represents how many neighbors we have so far in the array
	Episode** episodes;	// init to array of len k to hold ptrs to k Nearest Neighbors
	int* nValues;		// init to array of len k to hold ptrs to l NN scores
} Neighborhood;

Neighborhood* initNeighborhood(int cmd, int k);
void cleanNeighborhood(Neighborhood* nbHd);
int addPotentialNeighbor(Neighborhood* nbHd, Episode* ep, int n);
int addNeighbor(Neighborhood* nbHd, Episode* ep, int n);
Episode* getNeighbor(Neighborhood* nbHd, int i);

#endif // _NEIGHBORHOOD_H_
