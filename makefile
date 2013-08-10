# Filename: Makefile (For compiling portions that are run on the  
# 	gumstix/roomba)
# Author: Tanya L. Crenshaw
# Date created: 7/9/2013
# 
# Note: One may want to change the compiler CC based on if they are
# compiling for a desktop or an embedded system.
CC=arm-linux-gcc
VPATH=./communication: ./robot: ./robot/roomba
CFLAGS+=-lrt -I ./communication -I ./robot -I ./robot/roomba

OBJS=nerves.o erQueue.o netDataProtocol.o led.o commands.o utility.o sensors.o responders.o events.o clock.o erControl.o myEventResponders.o services.o acceptor.o connector.o mkaddr.o

### GUMSTIX TARGET ####

# Compile the acceptor side of the demo for the gumstix, "gumstix service".
nerves.out:	$(OBJS)
	$(CC) $(CFLAGS) -o nerves.out -DGUMSTIX $(OBJS)

%.o:	%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o
	rm -f *.out
