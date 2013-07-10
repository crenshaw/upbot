# Filename: Makefile (for interfaces experiments)
# Author: Tanya L. Crenshaw
# Date created: 7/9/2013
# 
# Note: One may want to change the compiler CC based on if they are
# compiling for a desktop or an embedded system.
CC = gcc

# Compile the demo
service.out:	genericServiceDemo.c acceptor.c services.c services.h
	$(CC)  -o service.out genericServiceDemo.c acceptor.c services.c

clean:
	rm *.out
