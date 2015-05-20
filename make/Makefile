CC = gcc
CFLAGS = -I.

server: server.c common.h
	$(CC) -o server server.c $(CFLAGS)

.PHONY: clean

clean:
	rm -f server.o server
