# makefile for communication 
CC=arm-linux-gcc
CFLAGS= -static  

all: server client sserver sclient brain brainstem

server:	server.c communication.h serverUtility.c
	$(CC) $(CFLAGS) -o server.out server.c serverUtility.c

client:	client.c communication.h serverUtility.c
	gcc -o client.out client.c serverUtility.c 

sserver:	simpleServer.c
	$(CC) $(CFLAGS) -o sserver.out simpleServer.c

sclient:	simpleClient.c communication.h
	gcc -o sclient.out simpleClient.c

testCQ: testCommandQueue.c commandQueue.c tell.c serverUtility.c
	$(CC) $(CFLAGS) -o testCQ.out testCommandQueue.c commandQueue.c tell.c serverUtility.c

brainstem: brainstem.c communication.h serverUtility.c tell.c nerves.o move.o sensors.o led.o utility.o ../roomba/roomba.h
	$(CC) $(CFLAGS) -o brainstem.out brainstem.c serverUtility.c tell.c nerves.o move.o sensors.o led.o utility.o 



# Make sure that object files are built in the appropriate order; if
# anything in roomba.h changes, the accompanying nervers, move,
# sensor, led, and utility functions need to be updated.  Without this
# addition to the makefile, if roomba.h changes, the dependency
# between it and these files doesn't become apparent in the targets
# above.
nerves.o:	../roomba/nerves.c ../roomba/roomba.h move.o sensors.o led.o utility.o 
	$(CC) $(CFLAGS) -c ../roomba/nerves.c

move.o:	../roomba/move.c ../roomba/roomba.h 
	$(CC) $(CFLAGS) -c ../roomba/move.c

sensors.o:	../roomba/sensors.c ../roomba/roomba.h
	$(CC) $(CFLAGS) -c ../roomba/sensors.c

led.o:	../roomba/led.c ../roomba/roomba.h
	$(CC) $(CFLAGS) -c ../roomba/led.c

utility.o:	../roomba/utility.c ../roomba/roomba.h
	$(CC) $(CFLAGS) -c ../roomba/utility.c

clean:
	rm *.out *.o