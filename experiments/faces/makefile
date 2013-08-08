# Filename: Makefile (for interfaces experiments)
# Author: Tanya L. Crenshaw
# Date created: 7/9/2013

VPATH=../../communication : ../../robot
OBJS=netDataProtocol.o acceptor.o connector.o services.o mkaddr.o 
DEPS=services.h acceptor.h connector.h netDataProtocol.h mkaddr.h

# Target: supervisor.out
# A demonstration supervisor running on a native machine.
supervisor.out:	CC=gcc
supervisor.out:	CFLAGS+=-pthread -I ../../communication -I ../../robot

# Target: producer.out
# A demonstration data producer running on a native machine.
producer.out:	CC=gcc
producer.out:	CFLAGS+=-pthread -I ../../communication -I ../../robot

# Target: gumstix.out
# A demonstration data producer running on a gumstix machine.
gumstix.out:	CC=arm-gnu-linux
gumstix.out:	CFLAGS+= -lrt -I ../../communication -I ../../robot


supervisor.out:	$(OBJS) $(DEPS) supervisor.c
	$(CC) $(CFLAGS) -o $@ supervisor.c $(OBJS)

producer.out:	$(OBJS) $(DEPS) genericProducerDemo.c
	$(CC) $(CFLAGS) -o $@ genericProducerDemo.c $(OBJS)

gumstix.out:	$(OBJS) $(DEPS) genericProducerDemo.c
	$(CC) $(CFLAGS) -o $@ genericProducerDemo.c $(OBJS)

%.o:	%.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm *.out
	rm *.o
