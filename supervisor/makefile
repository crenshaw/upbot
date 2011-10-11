#makefile for supervisor
CC=gcc

# make sure that the path 'usr/lib/jvm/defaul-java' exists and points to
# sunjava's 64 bit jdk. Also make sure that '/usr/lib64/jvm/default-java/jre/lib/amd64/server/'
# is in the LD_LIBRARY_PATH. Add to bashrc or add on a one-time basis.

all: supervisor filter_KNN KNN_unitTest saccFilt

supervisor: supervisor.c supervisor.h vector.h vector.c knearest.h knearest.c
	$(CC) -o vector.o -c vector.c
	$(CC) -o knearest.o -c knearest.c
	$(CC) -o ../wme/wme.o -c ../wme/wme.c
	$(CC) -o supervisor.o -c supervisor.c

filter_KNN: filter_KNN_unitTestMain.c filter_KNN.c supervisor
	$(CC) -g -c filter_KNN.c
	$(CC) -g -c filter_KNN_unitTestMain.c
	$(CC) -o filter_KNN.out filter_KNN_unitTestMain.o filter_KNN.o vector.o supervisor.o knearest.o ../wme/wme.o -lm

saccFilt: saccFilt.c supervisor SaccFilter.java
	$(CC) -g -I '/usr/lib/jvm/default-java/include' -I '/usr/lib/jvm/default-java/include/linux' -c saccFilt.c -L'/usr/lib/jvm/default-java/jre/lib/amd64/server' -ljvm
	javac SaccFilter.java

KNN_unitTest: KNN_unitTest.c knearest.c
	$(CC) -o KNN_unitTest.out KNN_unitTest.c knearest.c
	
SF_UnitTest: SF_UnitTest.c saccFilt.c SaccFilter.java
	$(CC) -o SF_UnitTest.out -I '/usr/lib/jvm/default-java/include' -I '/usr/lib/jvm/default-java/include/linux' SF_UnitTest.c saccFilt.c -L'/usr/lib/jvm/default-java/jre/lib/amd64/server' -ljvm
	javac SaccFilter.java

WME_unitTest: WME_unitTest.c supervisor
	$(CC) -g -c WME_unitTest.c
	$(CC) -o WME_unitTest.out WME_unitTest.o vector.o supervisor.o knearest.o

EATERS_unitTest: EATERS_unitTest.c eaters.c vector.c supervisor
	$(CC) -g -c EATERS_unitTest.c 
	$(CC) -g -c eaters.c 
	$(CC) -o EATERS_unitTest.out EATERS_unitTest.o eaters.o vector.o supervisor.o knearest.o
	
jni_demo: FilterInterface.c
	$(CC) -g -I '/usr/lib/jvm/default-java/include' -I '/usr/lib/jvm/default-java/include/linux' -o FilterInterface.out FilterInterface.c -L'/usr/lib/jvm/default-java/jre/lib/amd64/server' -ljvm

clean:
	rm -rf *.dSYM
	rm *.out *.o *.class

#	$(CC) -o ../soar/soar.o -c ../soar/soar.c
