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

/**
 * findEntry
 *
 * Get the index of a given entry in a vector 
 *
 * @arg vector pointer to vector
 * @arg entry  entry to find
 * @return the index of that entry (or -1 if not found)
 */
int findEntry(Vector* vector, void *entry)
{
    int i;
    for(i = 0; i < vector->size; i++)
    {
        if (vector->array[i] == entry) return i;
    }

    return -1;                  // not found
}// findentry

/**
 * cloneVector
 *
 * creates a new vector that is a duplicate of a given vector.
 * NOTE:  This could probably be made more efficient via a memcpy call
 *
 * CAVEAT:  the new vector is a shallow copy!
 * CAVEAT:  this method allocates memory that the caller is responsible for
 *
 * @arg vector is the vector to clone
 * @return the copy
 */
Vector *cloneVector(Vector *vector)
{
    Vector *result = newVector();
    int i;

    for(i = 0; i < vector->size; i++)
    {
        addEntry(result, vector->array[i]);
    }
}//cloneVector

/**
 * addVector
 *
 * Copy the contents of one vector to another.  Note:  the source vector is
 * unaffected by this operation.
 *
 * @arg target pointer to vector to add entries to
 * @arg source pointer to vector to add entries from
 * @return int status code (0 == success)
 */
int addVector(Vector* target, Vector* source)
{
	assert(target != NULL);
    assert(source != NULL);

    //perform the copy here
    int i;
    for(i = 0; i < source->size; i++)
    {
        addEntry(target, source->array[i]);
    }

	return 0;
}// addVector

