# makefile for communication 
CC=arm-linux-gcc
CFLAGS= -static  

all: server client sserver sclient

server:	server.c communication.h serverUtility.c
	$(CC) $(CFLAGS) -o server.out server.c serverUtility.c

client:	client.c communication.h serverUtility.c
	gcc -o client.out client.c serverUtility.c 

sserver:	simpleServer.c
	$(CC) $(CFLAGS) -o sserver.out simpleServer.c

sclient:	simpleClient.c communication.h
	gcc -o sclient.out simpleClient.c

clean:
	rm *.out *.o