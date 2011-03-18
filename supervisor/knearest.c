#include "knearest.h"

/*
 * knearest.c
 *
 * This file contains the implementation of the functions necessary
 * to create generic k nearest neighborhood(s).
 *
 * Author: Zachary Paul Faltersack
 *
 * Last updated: March 17, 2011
 */

//--------------------------------------------------------------------------------
// Functions for creating and maintaining neighborhods

/**
* KN_initNeighborhood
*
* This function takes two arguments and creates a new neighborhood
* using this data. A pointer to the neighborhood is returned to the
* caller.
*
* @arg id An integer used to identify this neighborhood. In our case this is the
*           integer representation of a command (action).
* @arg k An integer indicating how many neighbors to find.
* @return KN_Neighborhood* A pointer to the new neighborhood
*/
KN_Neighborhood* KN_initNeighborhood(int id, int k)
{
    // Allocate space for the new neighborhood
    KN_Neighborhood* nbHd = (KN_Neighborhood*)malloc(sizeof(KN_Neighborhood));

    // Commit neighborhood meta-data
    nbHd->id = id;
    nbHd->kValue = k;
    nbHd->numNeighbors = 0;

    // Allocate memory for the buffers containing our neighbors
    // and their n-values
    nbHd->neighbors  = (void**)malloc(k * sizeof(void*));
    nbHd->nValues   = (int*)malloc(k * sizeof(int));

    // return a pointer to the new neighborhood
    return nbHd;
}//KN_initNeighborhood

/**
* KN_destroyNeighborhood
*
* Takes a pointer to a neighborhood and frees all associated memory
*
* @arg nbHd A pointer to a neighborhood
*/
void KN_destroyNeighborhood(KN_Neighborhood* nbHd)
{
    free(nbHd->neighbors);   // free the array containing neighbor pointers
    free(nbHd->nValues);    // free the array containing neighborhood metrics
    free(nbHd);             // free the memory containing the neighborhood itself
}//KN_destroyNeighborhood

/**
* KN_cleanNeighborhood
*
* Takes a pointer to a neighborhood and resets necessary values in 
* order to reuse it in the future.
* By setting the numNeighbor count back to 0, we can in effect clear
* the entire struct, because all additions are entered with respect 
* to that counter.
*
* NOTE: This may be broken, explanation is in the paragraph comment 
*       in addNeighbor below.
*
* @arg nbHd A pointer to the neighborhood
*/
void KN_cleanNeighborhood(KN_Neighborhood* nbHd)
{
    int i;
    
    //Reset all the array values. (for safety)
	for(i = 0; i < nbHd->kValue; i++)
	{
        nbHd->nValues[i] = -1;
    }

    // Reset the counter to 0
    nbHd->numNeighbors = 0;

}//KN_cleanNeighborhood

/**
* KN_addNeighbor
*
* This function adds the new neighbor if it is one of the nearest neighbors
* seen so far. It then calls a sort method to maintain the order
*
* @arg nbHd A pointer to the current neighborhood
* @arg nbr A pointer to a potential new neighbor
* @arg n The n value of the new neighbor
* @return int Success/fail
*/
int KN_addNeighbor(KN_Neighborhood* nbHd, void* nbr, int n)
{
    // Default offset = 1 (Assume the neighborhood is full)
    int offset = 1;

	// If not full, correct offset
	if(nbHd->numNeighbors < nbHd->kValue) offset = 0;

	// If the neighborhood is not empty, then determine if the new
	// neighbor needs to be added
	if(nbHd->numNeighbors > 0)
	{
        //If the neighborhood is not full, go ahead and add this one
        //Otherwise only add it if its metric is great than an existing neighbor
        if ( (offset == 0) || (n > nbHd->nValues[nbHd->numNeighbors - 1]) )
		{
			// Save the episode and the neighborhood metric
			nbHd->neighbors[nbHd->numNeighbors - offset] = nbr;
			nbHd->nValues[nbHd->numNeighbors - offset] = n;

			if(nbHd->numNeighbors < nbHd->kValue) nbHd->numNeighbors++;

			// Maintain the order of the episodes for sanity's sake
			KN_sortNeighborhood(nbHd);
		}
	}
	else	// otherwise add the first neighbor
	{
		nbHd->neighbors[0] = nbr;
		nbHd->nValues[0] = n;

		nbHd->numNeighbors++;
	}

	return 1;
}//KN_addNeighbor

/**
 * KN_sortNeighborhood
 *
 * This function will sort a neighborhood from greatest n value
 * to least n value. A more recent addition is considered a greater
 * value when compared to an old neighbor with an equivalent n value.
 * This is due to the fact that we are expecting neighbors to be added
 * from oldest to most recent, and ties are determined by recency.
 *
 * @arg nbHd A pointer to a neighborhood that needs to be sorted
 */
void KN_sortNeighborhood(KN_Neighborhood* nbHd)
{
	// No need to sort a single item
	if(nbHd->numNeighbors <= 1) return;

	int i, tempN;
	void* tempNbr;
	//iterate from back to front
	for(i = nbHd->numNeighbors - 1; i > 0; i--)
	{
		// Want to move up if equal to or greater
		// this helps to account for always choosing the closest neighbor
		// to the current time when dealing with equivalent matches
		if(nbHd->nValues[i] >= nbHd->nValues[i - 1])
		{
			// Store the moving values in temp value holders
			tempNbr = nbHd->neighbors[i];
			tempN = nbHd->nValues[i];

			// push back the smaller match
			nbHd->neighbors[i] = nbHd->neighbors[i - 1];
			nbHd->nValues[i] = nbHd->nValues[i - 1];

			// resave the new addition
			nbHd->neighbors[i - 1] = tempNbr;
			nbHd->nValues[i - 1] = tempN;
		}
		else
		{
			// Must break because the new addition is not always the highest
			// rated neighborhood metric. When that is the case, if we continue
			// to traverse through the neighborhood then we can accidentally swap
			// the desired order or previously determined neighbors
			break;
		}//else
	}//for
}//KN_sortNeighborhood

/**
 * KN_getNeighbor
 *
 * Return the i-th neighbor in the neighborhood if available
 *
 * @arg nbHd A pointer to a neighborhood whose neighbors we wish to access
 * @arg i An int representing the desired index of neighbor
 * @return void* A pointer to the episode that is that neighbor: NULL if error occurs
 */
void* KN_getNeighbor(KN_Neighborhood* nbHd, int i)
{
	// Catch any out-of-bounds errors and neighbor unavailable
	if(i < 0 || i >= nbHd->kValue) return NULL;
	if(i >= nbHd->numNeighbors) return NULL;

	return (nbHd->neighbors[i]);
}//KN_getNeighbor
