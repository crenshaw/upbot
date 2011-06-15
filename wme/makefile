#makefile for supervisor
CC=gcc

all: WME_unitTest 

WME_unitTest: WME_unitTest.c wme.c ../supervisor/vector.c
	$(CC) -o WME_unitTest.out WME_unitTest.c wme.c ../supervisor/vector.c 

clean:
	rm -r *.dSYM
	rm *.out *.o
