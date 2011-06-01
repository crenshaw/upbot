#makefile for supervisor
CC=gcc

all: supervisor filter_KNN KNN_unitTest

supervisor: supervisor.c supervisor.h vector.h vector.c knearest.h knearest.c
	$(CC) -o vector.o -c vector.c
	$(CC) -o knearest.o -c knearest.c
	$(CC) -o supervisor.o -c supervisor.c

filter_KNN: filter_KNN_unitTestMain.c filter_KNN.c supervisor
	$(CC) -g -c filter_KNN.c
	$(CC) -g -c filter_KNN_unitTestMain.c
	$(CC) -o filter_KNN.out filter_KNN_unitTestMain.o filter_KNN.o vector.o supervisor.o knearest.o

KNN_unitTest: KNN_unitTest.c knearest.c
	$(CC) -o KNN_unitTest.out KNN_unitTest.c knearest.c

WME_unitTest: WME_unitTest.c supervisor
	$(CC) -g -c WME_unitTest.c
	$(CC) -o WME_unitTest.out WME_unitTest.o vector.o supervisor.o knearest.o

EATERS_unitTest: EATERS_unitTest.c eaters.c
	$(CC) -o EATERS_unitTest.out EATERS_unitTest.c eaters.c

clean:
	rm -r *.dSYM
	rm *.out *.o
