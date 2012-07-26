# makefile for roomba directory
CC=arm-linux-gcc
CFLAGS= -static  

all:  controller.out

controller.out:	main.c utility.c led.c move.c song.c sensors.c ../communication/commandQueue.c
	$(CC) $(CFLAGS) -o controller.out main.c utility.c led.c move.c song.c sensors.c ../communication/commandQueue.c

clean:
	rm *.out
