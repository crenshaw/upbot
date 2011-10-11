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
	$(CC) $(CFLAGS) -o sserver.out simpleServer.c

sclient:	simpleClient.c communication.h
	gcc -o sclient.out simpleClient.c

testCQ: testCommandQueue.c commandQueue.c tell.c serverUtility.c
	$(CC) $(CFLAGS) -o testCQ.out testCommandQueue.c commandQueue.c tell.c serverUtility.c

# make sure that the path 'usr/lib/jvm/defaul-java' exists and points to
# sunjava's 64 bit jdk. Also make sure that '/usr/lib64/jvm/default-java/jre/lib/amd64/server/'
# is in the LD_LIBRARY_PATH. Add to bashrc or add on a one-time basis.
supclient:	supervisorClient.c communication.h serverUtility.c ../supervisor/supervisor.h ../supervisor/vector.h ../supervisor/knearest.h ../wme/wme.h commandQueue.c
	gcc $(DEBUG_OPT) -I '/usr/lib/jvm/default-java/include' -I '/usr/lib/jvm/default-java/include/linux' -o supervisorClient.out supervisorClient.c serverUtility.c ../supervisor/supervisor.c ../supervisor/vector.c ../supervisor/knearest.c ../wme/wme.c commandQueue.c ../supervisor/filter_KNN.c ../supervisor/hallucinogen.c ../supervisor/saccFilt.c -lm -L'/usr/lib/jvm/default-java/jre/lib/amd64/server' -ljvm
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

brainstem: brainstem.c communication.h serverUtility.c commandQueue.c tell.c compass.o nerves.o move.o sensors.o led.o song.o utility.o ../roomba/roomba.h
	$(CC) $(CFLAGS) -o brainstem.out brainstem.c serverUtility.c commandQueue.c tell.c compass.o nerves.o move.o sensors.o led.o song.o utility.o

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
