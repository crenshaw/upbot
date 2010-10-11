#makefile for supervisor
CC=gcc

all: supervisor

supervisor: nsm.c nsm.h vector.h vector.c 
	$(CC) -o vector.o -c vector.c
	$(CC) -o nsm.o -c nsm.c

clean:
	rm *.out *.o
