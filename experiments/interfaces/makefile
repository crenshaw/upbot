# Filename: Makefile (for interfaces experiments)
# Author: Tanya L. Crenshaw
# Date created: 7/9/2013
# 
# Note: One may want to change the compiler CC based on if they are
# compiling for a desktop or an embedded system.
CC=gcc
VPATH=../../communication
CFLAGS+=-pthread -I ../../communication

native:	service.out prod.out
gumstix:	gs.out

### NATIVE TARGET ####

# Compile the acceptor side of the demo for a native machine.
service.out:	genericServiceDemo.c acceptor.o connector.o services.o services.h mkaddr.c
	$(CC) $(CFLAGS) -o service.out -DMAC genericServiceDemo.c acceptor.o connector.o services.o mkaddr.c

# Compile the connector side of the demo for a native machine.
prod.out:	genericProducerDemo.c connector.o acceptor.o services.o services.h mkaddr.c
	$(CC) $(CFLAGS)  -o prod.out -DMAC genericProducerDemo.c connector.o acceptor.o services.o mkaddr.c

genericServiceDemo.o:	genericServiceDemo.c services.h
	$(CC) $(CFLAGS) -o $@ -c $<


### Some sources are not located in this directory.  Use VPATH and automatic
### variables to build them.
acceptor.o:	acceptor.c acceptor.h services.h
	$(CC) $(CFLAGS) -DMAC -o $@ -c $<

connector.o:	connector.c connector.h services.h
	$(CC) $(CFLAGS) -DMAC -o $@ -c $<

services.o:	services.c acceptor.h connector.h services.h
	$(CC) $(CFLAGS) -DMAC -o $@ -c $<



### GUMSTIX TARGET ####

# Compile the acceptor side of the demo for the gumstix, "gumstix service".
gs.out:	genericServiceDemoG.o acceptorG.o servicesG.o services.h mkaddr.c
	arm-linux-gcc  $(CFLAGS) -o gs.out -DGUMSTIX genericServiceDemo.c acceptor.c services.c mkaddr.c

genericServiceDemoG.o:	genericServiceDemo.c services.h
	arm-linux-gcc $(CFLAGS) -DGUMSTIX -o $@ -c $<

### Some sources are not located in this directory.  Use VPATH and automatic
### variables to build them.
acceptorG.o:	acceptor.c acceptor.h services.h
	arm-linux-gcc $(CFLAGS) -DGUMSTIX -o $@ -c $<

connectorG.o:	connector.c connector.h services.h
	arm-linux-gcc $(CFLAGS) -DGUMSTIX -o $@ -c $<

servicesG.o:	services.c services.h
	arm-linux-gcc $(CFLAGS) -DGUMSTIX -o $@ -c $<

clean:
	rm *.out
	rm *.o
