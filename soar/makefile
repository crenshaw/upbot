#makefile for supervisor
CC=gcc

all: soar

soar: soar.c soar.h vector.h vector.c
	$(CC) -o soar.out soar.c vector.c

clean:
	rm -r *.dSYM
	rm *.out *.o
