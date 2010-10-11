#include "neighborhood.h"

/**
 * neighborhood.c
 *
 * This is an implementation of a sorted array that will do the 
 * book-keeping side of finding the k nearest neighbors for McCallum's
 * NSM Q-Learning agent
 *
 * Author: Zachary Paul Faltersack
 * Last Edit: October 11, 2010
 */

/**
* initNeighborhood
*
* This function takes two arguments and creates a new neighborhood
* using this data. A pointer to the neighborhood is returned to the
* caller.
*
* @arg cmd An integer representing the command associated with the neighborhood
* @arg k An integer indicating how many neighbors to find.
* @return Neighborhood* A pointer to the new neighborhood
*/
Neighborhood* initNeighborhood(int cmd, int k)
{
	// Allocate space for the new neighborhood
	Neighborhood* nbHd = (Neighborhood*)malloc(sizeof(Neighborhood));

	// Commit neighborhood meta-data
	nbHd->action = cmd;
	nbHd->kValue = k;
	nbHd->numNeighbors = 0;

	// Allocate memory for the buffers containing our neighbors
	// and their n-values
	nbHd->episodes 	= (Episode**)malloc(k * sizeof(Episode*));
	nbHd->nValues	= (int*)malloc(k * sizeof(int));

	// return a pointer to the new neighborhood
	return nbHd;
}//initNeighborhood

/**
* cleanNeighborhood
*
* Takes a pointer to a neighborhood and frees all associated memory
*
* @arg nbHd A pointer to a neighborhood
*/
void cleanNeighborhood(Neighborhood* nbHd)
{
	free(nbHd->episodes);	// free the array containing episode pointers
	free(nbHd->nValues);	// free the array containing ep n values
	free(nbHd);				// free the memory containing the neighborhood itself
}//cleanNeighborhood

/**
* addPotentialNeighbor
*
* This function takes a potential new neighbor and determines if
* it fits the criteria for determining the k nearest neighbors
*
* @arg nbHd A pointer to a Neighborhood
* @arg ep A pointer to the potential new neighbor
* @arg n The n value associated with the new neighbor
* @return int A boolean to represent success/fail
*/
int addPotentialNeighbor(Neighborhood* nbHd, Episode* ep, int n)
{
	if(nbHd->numNeighbors < nbHd->kValue)
	{
		
	}


}//addPotentialNeighbor

/**
* addNeighbor
*
* This function adds the new neighbor to the neighborhood
* and then calls a sort method to maintain the order
*
* @arg nbHd A pointer to the current neighborhood
* @arg ep A pointer to the new neighbor
* @arg n The n value of the new neighbor
* @return int Success/fail
*/
int addNeighbor(Neighborhood* nbHd, Episode* ep, int n)
{
	if(nbHd->numNeighbors < nbHd->kValue)
	{
		nbHd->episodes[nbHd->numNeighbors] = ep;
		nbHd->nValues[nbHd->numNeighbors] = n;
		nbHd->numNeighbors++;
	}
	else
	{
		
	}
}//addNeighbor

/**
* getNeighbor
*
* Return the i-th neighbor in the neighborhood if available
*
* @arg i An int representing the desired index of neighbor
* @return Episode* A pointer to the episode that is that neighbor
					NULL if error occurs
*/
Episode* getNeighbor(Neighborhood* nbHd, int i)
{
	// Catch any out-of-bounds errors and neighbor unavailable
	if(i < 0 || i >= nbHd->kValue) return NULL;
	if(i >= nbHd->numNeighbors) return NULL;
	
	return (nbHd->episodes + i);
}//getNeighbor














