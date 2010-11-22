/**
* This is the implementation file for a set of functions used to
* create and handle a struct that act like a queue.
*
* Author: Zachary Paul Faltersack
* Last Edit: November 4, 2010
*/

#include "queue.h"

/**
* newQueue
*
* This function allocates space for a Queue struct and initializes
* its variables.
*
* @param cap An int indicating the desired capacity of the queue
* @return Queue* A pointer to the Queue
*/
Queue* newQueue(int cap)
{
	Queue* q 	= (Queue*)malloc(sizeof(Queue));
	q->capacity = cap;
	q->size 	= 0;
	q->currPhysicalIdx = 0;
	q->array	= (void**)malloc(cap * sizeof(void*));

	return q;
}//newQueue

/**
* freeQueue
*
* This function frees a Queue's associated memory. The caller is
* responsible for freeing the memory used by the items in the Queue.
*
* @param q A pointer to the Queue being deleted
*/
void freeQueue(Queue* q)
{
	if(q == NULL) return;

	free(q->array);
	free(q);
}//freeQueue

void* addEntry(Queue* q, void* item)
{
	assert(q != NULL);	// Don't want a null queue

	// pointer to store an item that may need to be freed from memory
	void* temp = NULL;

	// If we still haven't filled the array then tack on to the end
	if(q->size < q->capacity)
	{
		q->array[q->size] = item;
		q->size++;
	}
	else // otherwise store the item to be pushed off the end and add the new item
	{
		temp = q->array[q->currPhysicalIdx];
		q->array[q->currPhysicalIdx] = item;
	}

	// update the current physical index marking the 'top' of the array and
	// wrap if necessary
	q->currPhysicalIdx++;
	if(q->currPhysicalIdx >= q->capacity)
	{
		q->currPhysicalIdx = 0;
	}

	// return pointer to memory that needs to be freed if necessary
	return temp;
}//addEntry

void* getEntry(Queue* q, int index)
{
	// make sure index is within appropriate bounds
	if(index >= q->size || index >= q->capacity || index < 0) return NULL;

	int physIdx = (q->size < q->capacity ? index : (q->currPhysicalIdx + index) % q->capacity);

	return q->array[physIdx];
}//getEntry


















