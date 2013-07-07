BIN = tbd tc

all: daemon client

clean:
	-@rm -rf *.o $(BIN) 2> /dev/null || true

daemon: daemon_objects
	@gcc -pthread -o tbd become_daemon.o test_become_daemon.c

client:
	@gcc -o tc test_client.c

daemon_objects:
	@gcc -c become_daemon.c
