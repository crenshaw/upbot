#include <stdlib.h>
#include <stdio.h>
#include "forgetfulmem.h"

void printQueue(ForgetfulMem* fm)
{
	printf("Physical Layout:\n");
	int i;
	for(i = 0; i < fm->size; i++) printf("%i,", *(int*)fm->array[i]);
	printf("\n");

	printf("Virtual Layout:\n");
	for(i = 0; i < fm->size; i++) printf("%i,", *(int*)getEntry(fm, i));
	printf("\n");
}//printQueue

int main()
{
	ForgetfulMem* fm = newFMem(10);
	int* int1 = (int*)malloc(sizeof(int)); *int1 = 1;
	int* int2 = (int*)malloc(sizeof(int)); *int2 = 2;
	int* int3 = (int*)malloc(sizeof(int)); *int3 = 3;
	int* int4 = (int*)malloc(sizeof(int)); *int4 = 4;
	int* int5 = (int*)malloc(sizeof(int)); *int5 = 5;
	int* int6 = (int*)malloc(sizeof(int)); *int6 = 6;
	int* int7 = (int*)malloc(sizeof(int)); *int7 = 7;
	int* int8 = (int*)malloc(sizeof(int)); *int8 = 8;
	int* int9 = (int*)malloc(sizeof(int)); *int9 = 9;
	int* int10 = (int*)malloc(sizeof(int)); *int10 = 10;
	int* int11 = (int*)malloc(sizeof(int)); *int11 = 11;
	int* int12 = (int*)malloc(sizeof(int)); *int12 = 12;
	int* int13 = (int*)malloc(sizeof(int)); *int13 = 13;
	int* int14 = (int*)malloc(sizeof(int)); *int14 = 14;
	int* int15 = (int*)malloc(sizeof(int)); *int15 = 15;
	int* int16 = (int*)malloc(sizeof(int)); *int16 = 16;

	printQueue(fm);

	printf("Adding items 1,2,3,4\n");
	addEntry(fm, int1);
	addEntry(fm, int2);
	addEntry(fm, int3);
	addEntry(fm, int4);
	printQueue(fm);

	printf("------------\n");
	printf("Adding items 5,6,7,8\n");
	addEntry(fm, int5);
	addEntry(fm, int6);
	addEntry(fm, int7);
	addEntry(fm, int8);
	printQueue(fm);

	printf("------------\n");
	printf("Adding items 9,10,11,12\n");
	addEntry(fm, int9);
	addEntry(fm, int10);
	addEntry(fm, int11);
	addEntry(fm, int12);
	printQueue(fm);

	printf("------------\n");
	printf("Adding items 13,14,15,16\n");
	addEntry(fm, int13);
	addEntry(fm, int14);
	addEntry(fm, int15);
	addEntry(fm, int16);
	printQueue(fm);

	free(int1);
	free(int2);
	free(int3);
	free(int4);
	free(int5);
	free(int6);
	free(int7);
	free(int8);
	free(int9);
	free(int10);
	free(int11);
	free(int12);
	free(int13);
	free(int14);
	free(int15);
	free(int16);

	freeFMem(fm);

	return 0;
}
