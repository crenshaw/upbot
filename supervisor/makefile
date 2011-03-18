#makefile for supervisor
CC=gcc

all: supervisor filter_KNN

supervisor: supervisor.c supervisor.h vector.h vector.c 
	$(CC) -o vector.o -c vector.c
	$(CC) -o supervisor.o -c supervisor.c

filter_KNN: filter_KNN_unitTestMain.c filter_KNN.c vector.o supervisor.o
	$(CC) -g -c filter_KNN.c
	$(CC) -g -c filter_KNN_unitTestMain.c
	$(CC) -o filter_KNN.out filter_KNN_unitTestMain.o filter_KNN.o vector.o supervisor.o

KNN_unitTest: KNN_unitTest.c knearest.c
	$(CC) -o KNN_unitTest.out KNN_unitTest.c knearest.c

clean:
	rm *.out *.o
