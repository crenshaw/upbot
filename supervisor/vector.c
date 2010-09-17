#include "vector.h"

/**
* newVector
*
* Allocates and initializes a vector
* Caller is responsible for calling 'freeVector'
*
* @return Vector* Pointer to allocated vector
*/
Vector* newVector()
{
	size_t initCap = 64;
	Vector* result = (Vector*) malloc(sizeof(Vector));
	result->array = (void**) malloc(initCap * sizeof(void*));
	result->capacity = initCap;
	result->size = 0;
	return result;
}// newVector

/**
* freeVector
*
* Deallocates memory from vector
*
* @arg victim pointer to memory to be freed
*/
void freeVector(Vector* victim)
{
	if(victim == NULL)
	{
		return;
	}
	int i;
	// Free all memory pointed to by vector before freeing vector
	for(i = 0; i < victim->size; i++)
	{
		free(victim->array[i]);
		victim->array[i] = NULL;
	}
	// Free the vector memory
	free(victim->array);
	free(victim);
}// freeVector

/**
* addEntry
*
* Add new entry to vector and increase capacity if necessary
*
* @arg vector pointer to vector
* @arg item pointer to item to add to array
* @return int status code (0 == success)
*/
int addEntry(Vector* vector, void* item)
{
	// Make sure we aren't adding to a null vector
	assert(vector != NULL);
	// Check if the vector is currently full
	if(vector->size == vector->capacity)
	{
		// If so, double the size of the array and copy elements
		void** tempArr = (void**) malloc(vector->capacity * 2 * sizeof(void*));
		int i;
		for(i = 0; i < vector->size; i++)
		{
			tempArr[i] = vector->array[i];
		}
		// Free memory for original array and point vector to new doubled array
		free(vector->array);
		vector->array = tempArr;
		vector->capacity = vector->capacity * 2;
	}
	
	// Add new element
	vector->array[vector->size] = item;
	vector->size++;

	return 0;
}// addEntry

/**
* getEntry
*
* Get the entry of a vector at a given index
*
* @arg vector pointer to vector
* @arg index index of item to retrieve
* @return void* pointer to the entry at given index
*/
void* getEntry(Vector* vector, int index)
{
	assert(index >= 0 && index < vector->size);
	return vector->array[index];
}// getEntry
