#
# makefile for example code directory
# 
# @author: Tanya L. Crenshaw
# @since:  July 19, 2012
#
CC=arm-linux-gcc
CFLAGS= -static  
CORE_OBJS=commandQueue.o led.o move.o sensors.o song.o utility.o
VPATH=../

# Notes:
# $^ is the make automatic variable that lists all the prerequisites of a rule, including their vpath directories.
# $@ is the make automatic variable for the target name.

all:	drive.out driveAndBump.out initLed.out stop.out

drive.out:	drive.c $(CORE_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ 

driveAndBump.out:	driveAndBump.c $(CORE_OBJS) 
	$(CC) $(CFLAGS) $^ -o $@ 

stop.out:	stop.c $(CORE_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ 

initLed.out:	initLed.c $(CORE_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ 

# Rules for the core set of source files that the examples depend upon.
#
commandQueue.o:	../../communication/commandQueue.c
	$(CC) $(CFLAGS) -c $^ -o $@

led.o:	led.c
	$(CC) $(CFLAGS) -c $^ -o $@

move.o:	move.c
	$(CC) $(CFLAGS) -c $^ -o $@

sensors.o:	sensors.c
	$(CC) $(CFLAGS) -c $^ -o $@

song.o:	song.c
	$(CC) $(CFLAGS) -c $^ -o $@

utility.o:	utility.c  
	$(CC) $(CFLAGS) -c $^ -o $@


clean:	
	-rm *~
	-rm *.*~
	-rm *.o
	-rm *.out