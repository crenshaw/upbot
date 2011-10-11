# makefile for communication 
CC=arm-linux-gcc
CFLAGS= -static  

all: appSimple appHardy appStop

appSimple:	appSimple.c utility.c led.c move.c song.c sensors.c ../communication/commandQueue.c 
	$(CC) $(CFLAGS) -o appSimple.out appSimple.c utility.c led.c move.c song.c sensors.c ../communication/commandQueue.c

appHardy:	appHardy.c utility.c led.c move.c song.c sensors.c ../communication/commandQueue.c
	$(CC) $(CFLAGS) -o appHardy.out appHardy.c utility.c led.c move.c song.c sensors.c ../communication/commandQueue.c

appStop: 	appStop.c utility.c led.c move.c song.c sensors.c ../communication/commandQueue.c
	$(CC) $(CFLAGS) -o stop.out appStop.c utility.c led.c move.c song.c sensors.c ../communication/commandQueue.c 

clean:
	rm *.o *.out
