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
int     addEntry   (Vector* vector, void* item);
Vector* cloneVector(Vector* vector);
int     findEntry  (Vector* vector, void* entry);
void    freeVector (Vector* victim);
void*   getEntry   (Vector* vector, int index);
Vector* newVector  ();
int     removeEntry(Vector* vector, void* item);
int     removeEntryByIndex(Vector* vector, int index);
 
#endif // _VECTOR_H_
