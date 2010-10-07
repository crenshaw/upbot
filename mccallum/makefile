#makefile for supervisor
CC=gcc

all: supervisor

supervisor: supervisor.c supervisor.h vector.h vector.c 
	$(CC) -o vector.o -c vector.c
	$(CC) -o supervisor.o -c supervisor.c

clean:
	rm *.out *.o
