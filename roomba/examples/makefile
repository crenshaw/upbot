# makefile for example directory
CC=arm-linux-gcc
CFLAGS= -static  

drive:	driveAndBump.c ../utility.c ../led.c ../move.c ../song.c ../sensors.c ../../communication/commandQueue.c 
	$(CC) $(CFLAGS) -o drive.out driveAndBump.c ../utility.c ../led.c ../move.c ../song.c ../sensors.c ../../communication/commandQueue.c