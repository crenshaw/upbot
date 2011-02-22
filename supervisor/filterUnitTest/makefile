# Filename: makefile (for filter1_1)
# Authors: Ben Walker, Dustin Dalen
# Created: 1/29/11
#
CC = gcc
SRCS = main.c filter1_1.c vector.c
OBJS = main.o filter1_1.o

# Compile the filter1_1 executable from a list 
# of object files.  If none of the object files 
# has changed, make will report Ôfilter1_1 is up to date.'
filter1_1:	main.o filter1_1.o vector.o
		$(CC) -o filter1_1 main.o filter1_1.o vector.o

# State the dependencies of the main object file.
main.o:	main.c filter1_1.h vector.h
	$(CC) -g -c main.c

# State the dependencies of the filter1_1 object file.
filter1_1.o: filter1_1.c filter1_1.h
	$(CC) -g -c filter1_1.c	

# State the dependencies of the vector object file.
vector.o: vector.c vector.h
	$(CC) -g -c vector.c

clean: 
	-rm $(OBJS) filter1_1
