# makefile for communication 
CC=arm-linux-gcc
CFLAGS= -static  

all: server client sserver sclient supclient unittest

server:	server.c communication.h serverUtility.c
	$(CC) $(CFLAGS) -o server.out server.c serverUtility.c

client:	client.c communication.h serverUtility.c
	gcc -o client.out client.c serverUtility.c 

sserver:	simpleServer.c
	$(CC) $(LFLAGS) -o sserver.out simpleServer.c

sclient:	simpleClient.c communication.h
	gcc -o sclient.out simpleClient.c

supclient:	supervisorClient.c communication.h serverUtility.c ../supervisor/supervisor.h ../supervisor/vector.h
	gcc -o supervisorClient.out supervisorClient.c serverUtility.c ../supervisor/supervisor.c ../supervisor/vector.c

unittest: unitTestServer.c serverUtility.c ../supervisor/unitTest.h
	gcc -o unitTestServer.out unitTestServer.c serverUtility.c ../supervisor/unitTest.c


clean:
	rm *.out *.o
