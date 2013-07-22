# Filename: Makefile (for interfaces experiments)
# Author: Tanya L. Crenshaw
# Date created: 7/9/2013
# 
# Note: One may want to change the compiler CC based on if they are
# compiling for a desktop or an embedded system.
CC = gcc

mac:	service.out prod.out
gumstix:	gs.out


# Compile the acceptor side of the demo for the mac
service.out:	genericServiceDemo.c acceptor.c services.c services.h mkaddr.c
	$(CC) -pthread -o service.out -DMAC genericServiceDemo.c acceptor.c services.c mkaddr.c

# Compile the connector side of the demo for the mac
prod.out:	genericProducerDemo.c connector.c services.c services.h mkaddr.c
	$(CC)  -pthread -o prod.out -DMAC genericProducerDemo.c connector.c services.c mkaddr.c


# Compile the acceptor side of the demo for the gumstix, "gumstix service".
gs.out:	genericServiceDemo.c acceptor.c services.c services.h mkaddr.c
	arm-linux-gcc  -pthread -o gs.out -DGUMSTIX genericServiceDemo.c acceptor.c services.c mkaddr.c

clean:
	rm *.out
