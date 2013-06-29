# makefile for communication 
CC=arm-linux-gcc
CFLAGS= -static

ifneq ($(DEBUG),none)
DEBUG_OPT=-g 
endif

all: client sclient supclient mccClient unittest server sserver eaters brainstem

#all non-ARM targets
virt: client sclient supclient mccClient unittest eaters soarClient

server:	server.c communication.h serverUtility.c commandQueue.c 
	$(CC) $(CFLAGS) -o server.out server.c serverUtility.c commandQueue.c

client:	client.c communication.h serverUtility.c commandQueue.c
	gcc -o client.out client.c serverUtility.c commandQueue.c

sserver:	simpleServer.c
	$(CC) -o sserver.out simpleServer.c

sclient:	simpleClient.c communication.h
	gcc -o sclient.out simpleClient.c

testCQ: testCommandQueue.c commandQueue.c tell.c serverUtility.c
	$(CC) $(CFLAGS) -o testCQ.out testCommandQueue.c commandQueue.c tell.c serverUtility.c

#------------------------------------------------------------------------
# TWO IMPORTANT NOTES for CORRECT COMPILATION
#------------------------------------------------------------------------
#
# 1. To support the java-based implementation of the testbed, make
# sure that the path 'usr/lib/jvm/default-java' exists and points to
# sun's java jdk. On Debian, this may be as simple as installing the
# default java package, like so:
#
#   $ sudo apt-get install default-jdk
#
# Or, you may need to add it manually with a command like this:
#   $ sudo ln -s /usr/lib/jvm/java-6-sun-1.6.0.26 /usr/lib/jvm/default-java
#
# Making sure to modify the above to include the proper Java version number
#
#------------------------------------------------------------------------
#
# 2. Make sure that the libjvm.so file is in the LD_LIBRARY_PATH.
#
# For 64 bit computers: 
#   $ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib/jvm/default-java/jre/lib/amd64/server/
#
# For 32 bit computers: 
#   $ export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/lib/jvm/default-java/jre/lib/i386/server/
#
# One may add the above line to .bashrc or run the command on a
# one-time basis.  If the .bashrc is altered, be sure to update the
# environment by opening a new terminal, or using:
#
#   $ source .bashrc
#------------------------------------------------------------------------

supclient:	supervisorClient.c communication.h serverUtility.c ../supervisor/supervisor.h ../supervisor/vector.h ../supervisor/knearest.h ../wme/wme.h commandQueue.c
	gcc $(DEBUG_OPT) -I '/usr/lib/jvm/default-java/include' -I '/usr/lib/jvm/default-java/include/linux' -o supervisorClient.out supervisorClient.c serverUtility.c ../supervisor/supervisor.c ../supervisor/vector.c ../supervisor/knearest.c ../wme/wme.c commandQueue.c ../supervisor/filter_KNN.c ../supervisor/hallucinogen.c ../supervisor/saccFilt.c -lm -L'/usr/lib/jvm/default-java/jre/lib/amd64/server' -L'/usr/lib/jvm/default-java/jre/lib/i386/server' -ljvm
	javac ../supervisor/SaccFilter.java

mccClient: mccallumClient.c communication.h serverUtility.c ../mccallum/nsm.h ../mccallum/forgetfulmem.h ../mccallum/vector.h commandQueue.c
	gcc $(DEBUG_OPT) -o mccallumClient.out mccallumClient.c serverUtility.c ../mccallum/nsm.c ../mccallum/forgetfulmem.c ../mccallum/vector.c commandQueue.c ../supervisor/filter_KNN.c ../supervisor/hallucinogen.c

soarClient: soarClient.c communication.h serverUtility.c ../soar/soar.h ../soar/vector.h ../wme/wme.h commandQueue.c
	gcc $(DEBUG_OPT) -o soarClient.out soarClient.c serverUtility.c ../soar/soar.c ../soar/vector.c ../wme/wme.c commandQueue.c -lm

unittest: unitTestServer.c serverUtility.c ../supervisor/unitTest.h commandQueue.c
	gcc $(DEBUG_OPT)-o unitTestServer.out unitTestServer.c serverUtility.c ../supervisor/unitTest.c commandQueue.c
	gcc $(DEBUG_OPT)-o simpleTest.out ../supervisor/unitTestMain.c ../supervisor/supervisor.c ../supervisor/vector.c ../supervisor/knearest.c ../wme/wme.c -lm

eaters: eatersServer.c communication.h serverUtility.c ../supervisor/eaters.h commandQueue.c
	gcc $(DEBUG_OPT)-o eaters.out eatersServer.c serverUtility.c ../supervisor/eaters.c commandQueue.c

brainstem: brainstem.c communication.h serverUtility.c commandQueue.c tell.c nerves.o move.o sensors.o led.o song.o utility.o ../roomba/roomba.h
	$(CC) $(CFLAGS) -o brainstem.out brainstem.c serverUtility.c commandQueue.c tell.c nerves.o move.o sensors.o led.o song.o utility.o -lrt

# Make sure that object files are built in the appropriate order; if
# anything in roomba.h changes, the accompanying nerves, move, sensor,
# led, and utility functions need to be updated.  Without this
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

song.o:	../roomba/song.c ../roomba/roomba.h
	$(CC) $(CFLAGS) -c ../roomba/song.c

utility.o:	../roomba/utility.c ../roomba/roomba.h
	$(CC) $(CFLAGS) -c ../roomba/utility.c

compass.o:	../devices/compass.c ../devices/compass.h
	$(CC) $(CFLAGS) -c ../devices/compass.c

clean:
	rm -rf *.dSYM
	rm *.out *.o 
