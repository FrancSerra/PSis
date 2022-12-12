all: server client

server: server.c library.h
	gcc -Wall -pedantic server.c -g -o server -lncurses

client: client.c library.h
	gcc -Wall -pedantic client.c -g -o client -lncurses

clean:
	rm server client
