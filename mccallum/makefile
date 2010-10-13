#makefile for supervisor
CC=gcc

all: supervisor

supervisor: nsm.c nsm.h vector.h vector.c neighborhood.h neighborhood.c 
	$(CC) -o vector.o -c vector.c
	$(CC) -o neighborhood.o -c neighborhood.c
	$(CC) -o nsm.o -c nsm.c

clean:
	rm *.out *.o
