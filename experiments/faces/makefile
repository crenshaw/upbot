# Filename: Makefile (for interfaces experiments)
# Author: Tanya L. Crenshaw
# Date created: 7/9/2013

VPATH=../../communication : ../../robot

#all: supervisor.out prod.out 

# Target: supervisor.out
# A demonstration supervisor running on a native machine.
supervisor.out:	CC=gcc
supervisor.out:	CFLAGS+=-pthread -I ../../communication -I ../../robot
supervisor.out: OBJS=genericServiceDemo.o netDataProtocol.o acceptor.o connector.o services.o mkaddr.o 
supervisor.out: DEPS=services.h acceptor.h connector.h netDataProtocol.h

# Target: producer.out
# A demonstration data producer running on a native machine.
#producer.out:	CC=gcc
#producer.out:	CFLAGS+=-pthread -I ../../communication -I ../../robot
#producer.out:	OBJS= genericProducerDemo.o connector.o acceptor.o services.o mkaddr.o

# Target: gumstix.out
# A demonstration data producer running on a gumstix machine.
#gumstix.out:	CC=arm-gnu-linux
#gumstix.out:	CFLAGS+= -lrt -I ../../communication -I ../../robot
#gumstix.out:	OBJS= genericProducerDemo.o connector.o acceptor.o services.o mkaddr.o

# Compile the acceptor side of the demo for a native machine.
supervisor.out:	$(OBJS)
	$(CC) $(CFLAGS) -o supervisor.out $(OBJS)

%.o:	%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm *.out
	rm *.o
