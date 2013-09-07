# Filename: Makefile (for interfaces experiments)
# Author: Tanya L. Crenshaw
# Date created: 7/9/2013

# TODO: Currently, this makefile only builds one set of .o files for
# all of the targets.  As a result, the gumstix.out target won't
# compile correctly if the .o files have already been built for the
# supervisor.out target.  I need to figure out how to write this
# Makefile so that it creates two sets of .o files.  Until then, type
# 'make clean' before making any of the three targets.  -- TLC.
#

VPATH=../../communication : ../../robot
OBJS=netDataProtocol.o acceptor.o connector.o services.o mkaddr.o 
DEPS=services.h acceptor.h connector.h netDataProtocol.h mkaddr.h

.PHONY:	all
all: robot.out supervisor.out

# Target: supervisor.out
# A demonstration supervisor running on a native machine.
supervisor.out:	CC=gcc
supervisor.out:	CFLAGS+=-lrt -I ../../communication -I ../../robot

# Target: producer.out
# A demonstration data producer running on a native machine.
robot.out:	CC=gcc
robot.out:	CFLAGS+=-lrt -I ../../communication -I ../../robot

# Target: gumstix.out
# A demonstration data producer running on a gumstix machine.
gumstix.out:	CC=arm-linux-gcc
gumstix.out:	CFLAGS+= -lrt -I ../../communication -I ../../robot


supervisor.out:	$(OBJS) $(DEPS) supervisor.c
	$(CC) $(CFLAGS) -o $@ supervisor.c $(OBJS)

robot.out:	$(OBJS) $(DEPS) robot.c
	$(CC) $(CFLAGS) -o $@ robot.c $(OBJS)

gumstix.out:	$(OBJS) $(DEPS) robot.c
	$(CC) $(CFLAGS) -o $@ robot.c $(OBJS)

%.o:	%.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY:	clean
clean:
	rm *.o
	rm *.out