#makefile for supervisor
CC=gcc

all: soar

soar: soar.c soar.h vector.h vector.c
	$(CC) -lm -c soar.c vector.c

clean:
	rm -rf *.dSYM
	rm *.out *.o
