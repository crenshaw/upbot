# Filename: Makefile (for daemon demo)
# Author: Tanya L. Crenshaw
# Date created: 7/6/2013
# 
# Note: One may want to change the compiler CC based on if they are
# compiling for a desktop or an embedded system.
CC = gcc

# Compile the server
server:	become_daemon.c become_daemon.h test_become_daemon.c
	$(CC) -o server.out -lrt become_daemon.c test_become_daemon.c

# Compile the client
client:	test_client.c
	$(CC) -o client.out test_client.c