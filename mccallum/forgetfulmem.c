/**
* This is the implementation file for a set of functions used to
* create and handle a struct that act like a queue.
*
* Author: Zachary Paul Faltersack
* Last Edit: November 4, 2010
*/

#include "forgetfulmem.h"

/**
* newFMem
*
* This function allocates space for a ForgetfulMem struct and initializes
* its variables.
*
* @param cap An int indicating the desired capacity of the FMem
* @return ForgetfulMem* A pointer to the FMem
*/
ForgetfulMem* newFMem(int cap)
{
	ForgetfulMem* fm 	= (ForgetfulMem*)malloc(sizeof(ForgetfulMem));
	fm->capacity = cap;
	fm->size 	= 0;
	fm->currPhysicalIdx = 0;
	fm->array	= (void**)malloc(cap * sizeof(void*));

	return fm;
}//newFMem

/**
* freeFMem
*
* This function frees a ForgetfulMem's associated memory. The caller is
* responsible for freeing the memory used by the items in the FMem.
*
* @param fm A pointer to the ForgetfulMem being deleted
*/
void freeFMem(ForgetfulMem* fm)
{
	if(fm == NULL) return;

	free(fm->array);
	free(fm);
}//freeFMem

void* addEntryFM(ForgetfulMem* fm, void* item)
{
	assert(fm != NULL);	// Don't want a null queue

	// pointer to store an item that may need to be freed from memory
	void* temp = NULL;

	// If we still haven't filled the array then tack on to the end
	if(fm->size < fm->capacity)
	{
		fm->array[fm->size] = item;
		fm->size++;
	}
	else // otherwise store the item to be pushed off the end and add the new item
	{
		temp = fm->array[fm->currPhysicalIdx];
		fm->array[fm->currPhysicalIdx] = item;
	}

	// update the current physical index marking the 'top' of the array and
	// wrap if necessary
	fm->currPhysicalIdx++;
	if(fm->currPhysicalIdx >= fm->capacity)
	{
		fm->currPhysicalIdx = 0;
	}

	// return pointer to memory that needs to be freed if necessary
	return temp;
}//addEntry

void* getEntryFM(ForgetfulMem* fm, int index)
{
	// make sure index is within appropriate bounds
	if(index >= fm->size || index >= fm->capacity || index < 0) return NULL;

	int physIdx = (fm->size < fm->capacity ? index : (fm->currPhysicalIdx + index) % fm->capacity);

	return fm->array[physIdx];
}//getEntry


















