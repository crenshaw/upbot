# Filename: Makefile (for daemon demo)
# Author: Fernando Freire
# Date created: 7/7/2013
BIN = tbd.out tc.out

all: daemon client

clean:
	-@rm -rf *.o $(BIN) 2> /dev/null || true

daemon: daemon_objects
	@gcc -pthread -o tbd.out become_daemon.o test_become_daemon.c

client:
	@gcc -o tc.out test_client.c

daemon_objects:
	@gcc -c become_daemon.c
