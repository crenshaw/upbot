#ifndef _KNEAREST_H_
#define _KNEAREST_H_

/**
* knearest.h
*
* This file contains the functions for creating, maintaining and destroying 
* neighborhoods. It can store any type of data, whether it be episodes or some 
* other type of item from which a neighborhood metric can be calculated.
*
* Preconditions For Use, The following functions (or functionality) are required
*
*     calculateNValue() -  This function will determine the neighborhood
*                           metric for a given item. Since this is an attempt
*                           to generalize, we're making no assumptions about
*                           what the metric calculates or how it's calculated.
*
*     locateKNearestNeighbors(id) - This function will iterate over the episodic
*                           memory (from oldest to most recent) and calculate the 
*                           neighborhood metric for each episode using the above 
*                           function. Once the NM has been determined for an episode, 
*                           it can be pushed into a neighborhood. The functions below 
*                           handle the necessary book-keeping.
*                           (id) is optional. Alternative methods for identifying the
*                           neighborhood can be accomplished. In our case, the id
*                           will be the integer representation of a command (action).
*
* Once locateKNearestNeighbors has determined the metric for a neighbor it will call
* addNeighbor(neighborhood*, episode*, metric). The rest is taken care of.
* Once the entire collection of episodic memories has been processed, they final K
* selected neighbors can be accessed with getNeighbors(neighborhood*, i).
*
* In addition, a custom displayNeighborhood function will need to be written if that
* functionality is desired due to the fact that a Neighbor is arbitrary and can contain
* any amount and type of data.
*
* Author: Zachary Paul Faltersack
* Last edit: March 17, 2011
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

//#include "../communication/communication.h"

// Neighborhood struct for finding k-nearest neighbors
typedef struct NeighborhoodStruct
{
    int id;             // int to represent id associated with this neighborhood
                        //    --> This will be 'action' for our purposes
    int kValue;         // k is the number of neighbors
    int numNeighbors;   // represents how many neighbors we have so far in the array
    void** neighbors;   // init to array of len k to hold ptrs to k Nearest Neighbors
    int* nValues;       // init to array of len k to hold ptrs to k NN scores
} KN_Neighborhood;

//-----------------------------------------
// Functions for creating, maintaining and viewing neighborhoods
KN_Neighborhood* KN_initNeighborhood(int id, int k);
void KN_cleanNeighborhood(KN_Neighborhood* nbHd);
void KN_destroyNeighborhood(KN_Neighborhood* nbHd);
int KN_addNeighbor(KN_Neighborhood* nbHd, void* nbr, int n);
void KN_sortNeighborhood(KN_Neighborhood* nbHd);
void* KN_getNeighbor(KN_Neighborhood* nbHd, int i);
//-----------------------------------------

#endif // _KNEAREST_H_
