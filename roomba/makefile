# makefile for roomba directory
CC=arm-linux-gcc
CFLAGS= -static  

all: appHardy 

appHardy:	appHardy.c utility.c led.c move.c song.c sensors.c ../communication/commandQueue.c
	$(CC) $(CFLAGS) -o appHardy.out appHardy.c utility.c led.c move.c song.c sensors.c ../communication/commandQueue.c

clean:
	rm *.o *.out
