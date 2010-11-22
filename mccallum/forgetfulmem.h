#ifndef _FORGETFULMEM_H_
#define _FORGETFULMEM_H_

#include <stdlib.h>
#include <assert.h>

/**
* This file contains the function definitions for creating a
* queue of a particular length. It will be implemented as a
* circular array, and the user can retrieve items using the
* getEntry function and use the traditional 0<i<size indices
* which will get mapped to the correct location.
*
* Author: Zachary Paul Faltersack
* Last Edit: November 4, 2010
*/

// A struct to contain our queue and its metadata
typedef struct ForgetFulMemStruct 
{
	int 	capacity;			// Total storage size
	size_t 	size;				// Current number of items
	void** 	array;				// The actual queue
	int 	currPhysicalIdx;	// The current index that final item is at
} ForgetfulMem;

//---------Function declarations-----------
ForgetfulMem* newFMem(int cap);
void freeFMem(ForgetfulMem* fm);
void* addEntryFM(ForgetfulMem* fm, void* item);
void* getEntryFM(ForgetfulMem* fm, int index);

#endif	// _FORGETFULMEM_H_
