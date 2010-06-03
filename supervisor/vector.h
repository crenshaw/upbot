#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <stdlib.h>
#include <assert.h>

/*
	This is a simple implementation of the c++ vector class
	It will store resizable arrays of void* and will be the 
	'guts' for vectors for specific structs (types)
*/

typedef struct VectorStruct
{
	int capacity;
	size_t size;
	void ** array;
} Vector;

// Function declarations ( see vector.c)
Vector* newVector();
void freeVector(Vector* victim);
int addEntry(Vector* vector, void* item);
void* getEntry(Vector* vector, int index);


#endif // _VECTOR_H_
